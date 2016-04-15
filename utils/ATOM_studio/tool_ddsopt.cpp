#include "StdAfx.h"
#include "tool_ddsopt.h"
#include "resource.h"

static void messagePump (void)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if ( msg.message != WM_QUIT)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

static int optimizeDDS (const char *filename, bool force, HWND fileList, std::string &msg, int &sizeInc)
{
	int size_before = 0;
	int size_after = 0;
	msg = filename;
	sizeInc = 0;

	FILE *fp = fopen (filename, "rb");
	if (fp)
	{
		fseek (fp, 0, SEEK_END);
		size_before = ftell (fp);
		fseek (fp, 0, SEEK_SET);

		ATOM_VECTOR<char> buffer(size_before);
		int bytesRead = fread (&buffer[0], 1, size_before, fp);
		fclose (fp);
		if (bytesRead != size_before)
		{
			msg += "(读取文件失败)";
			return 0;
		}
		else
		{
			ATOM_RenderDevice *device = ATOM_GetRenderDevice();
			ATOM_ImageFileInfo fileInfo;
			if (device->getImageFileInfoInMemory (&buffer[0], size_before, &fileInfo) && fileInfo.format == ATOM_PIXEL_FORMAT_DXT3 || fileInfo.format == ATOM_PIXEL_FORMAT_DXT5)
			{
				if (!force)
				{
					ATOM_BaseImage image;
					ATOM_RWops *rwops = ATOM_RWops::createFileRWops (filename, "rb");
					bool succ = image.load (rwops, ATOM_ImageCodec::DDS, ATOM_PIXEL_FORMAT_BGRA8888);
					ATOM_RWops::destroyRWops (rwops);
					if (!succ)
					{
						msg += "(读取DDS失败)";
						return 0;
					}
					else
					{
						ATOM_ColorARGB *colors = (ATOM_ColorARGB*)image.getData();
						for (unsigned i = 0; i < image.getWidth() * image.getHeight(); ++i)
						{
							unsigned char a = colors[i].getByteA ();
							if (a > 0 && a < 255)
							{
								return 0;
							}
						}
					}
				}
				ATOM_AUTOREF(ATOM_Texture) texture = device->createTexture (filename, 0, 0, ATOM_PIXEL_FORMAT_DXT1);
				void *fileInMem = texture->saveToMemEx (ATOM_PIXEL_FORMAT_DXT1, ATOM_ImageCodec::DDS, (unsigned*)&size_after);

				std::string fn_new = filename;
				fn_new += ".new";
				FILE *fp = fopen (fn_new.c_str(), "wb");
				if (fp)
				{
					int sizeWritten = fwrite (fileInMem, 1, size_after, fp);
					fclose (fp);
					if (sizeWritten != size_after)
					{
						msg += "(写入文件失败)";
						return 0;
					}
					else
					{
						BOOL b = ::CopyFileA (fn_new.c_str(), filename, FALSE);
						::DeleteFileA (fn_new.c_str());
						if (!b)
						{
							msg += "(重命名文件失败)";
							return 0;
						}
					}
				}
				else
				{
					msg += "(写入文件失败)";
					return 0;
				}

				char buf[256];
				sprintf (buf, "(%dk bytes -> %dk bytes)", size_before/1024, size_after/1024);
				msg += buf;
				sizeInc = size_after - size_before;
				return 1;
			}
		}
	}
	else
	{
		msg += "(打开文件失败)";
	}
	return 0;
}

static int optimizeDDS_r (const char *folderName, bool force, HWND fileList, HWND currentFile, int &sizeIncTotal)
{
	WIN32_FIND_DATAA data;
	ATOM_STRING dirname(folderName);
	unsigned num = 0;

	if ( dirname[dirname.length() - 1] != '\\')
	{
		dirname += "\\";
	}
	ATOM_STRING dirpath = dirname + "*";
	sizeIncTotal = 0;

	ATOM_FIND_HANDLE h = FindFirstFileA(dirpath.c_str(), &data);
	if ( h != ATOM_INVALID_FIND_HANDLE_VALUE)
	{
		do
		{
			if (strcmp(data.cFileName, ".") && strcmp(data.cFileName, ".."))
			{
				if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
				{
					ATOM_STRING s = dirname + data.cFileName;
					int inc = 0;
					num += optimizeDDS_r (s.c_str(), force, fileList, currentFile, inc);
					sizeIncTotal += inc;
				}
				else
				{
					ATOM_STRING s = dirname + data.cFileName;
					::SetWindowTextA (currentFile, s.c_str());
					unsigned len = s.length();
					if ((len > 4) && !stricmp (s.c_str() + len - 4, ".dds"))
					{
						int sizeInc = 0;
						std::string msg;
						int n = optimizeDDS (s.c_str(), force, fileList, msg, sizeInc);;
						if (n && !msg.empty())
						{
							num += n;
							wchar_t wMsg[1024];
							MultiByteToWideChar(CP_ACP, 0, msg.c_str(), -1, wMsg, 1024);
							int count = ListBox_GetCount(fileList);
							ListBox_AddString (fileList, wMsg);
							if (count)
							{
								ListBox_SetTopIndex(fileList, count-1);
							}
							sizeIncTotal += sizeInc;
						}
					}
				}
			}
			messagePump ();
		}
		while (FindNextFileA (h, &data));

		FindClose (h);
	}

	return num;
}

char gRootDir[MAX_PATH];

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT msg, LPARAM lp, LPARAM pData)   
{   
	if (msg == BFFM_INITIALIZED && gRootDir[0])   
	{   
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)gRootDir);   
	}   
	return   0;   
}  

BOOL chooseDir (HWND hWndParent, const char *title, char *initialDir)
{
	BROWSEINFOA bi;
	char folderName[MAX_PATH];
	memset(&bi, 0, sizeof(bi));
	memset(folderName, 0, sizeof(folderName));
	bi.hwndOwner = hWndParent;
	bi.pszDisplayName = 0;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_STATUSTEXT|BIF_USENEWUI|BIF_RETURNONLYFSDIRS;
	bi.lpfn = &BrowseCallbackProc;

	strcpy (gRootDir, initialDir);
	LPITEMIDLIST itemId = SHBrowseForFolderA (&bi);
	if (itemId)
	{
		SHGetPathFromIDListA (itemId, folderName);
		GlobalFreePtr (itemId);
		strcpy (initialDir, folderName);
		return TRUE;
	}
	return FALSE;
}

bool isDirectory (const char *filename)
{
	DWORD attributes = GetFileAttributesA(filename);
	if ( attributes == 0xFFFFFFFF)
		return false;
	return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

void CenterWindow (HWND child, HWND parent)
{
	RECT rcChild, rcParent;
	::GetWindowRect (child, &rcChild);
	::GetWindowRect (parent, &rcParent);
	int childWidth = rcChild.right - rcChild.left + 1;
	int childHeight = rcChild.bottom - rcChild.top + 1;
	int parentWidth = rcParent.right - rcParent.left + 1;
	int parentHeight = rcParent.bottom - rcParent.top + 1;
	int x = rcParent.left + (parentWidth - childWidth) / 2;
	int y = rcParent.top + (parentHeight - childHeight) / 2;
	::MoveWindow (child, x, y, childWidth, childHeight, FALSE);
}

INT_PTR CALLBACK DlgProc (HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static char dir[ATOM_VFS::max_filename_length];
	static bool canClose = true;

	switch(message) {
	case WM_INITDIALOG:
		{
			dir[0] = '\0';
			ATOM_GetPhysicalPathName ("/", dir);
			HWND edDir = ::GetDlgItem (hWnd, IDC_DIRECTORY);
			::SetWindowTextA (edDir, dir);
			::SetFocus (edDir);
			::CenterWindow(hWnd,::GetParent(hWnd));
			::EnableWindow (::GetDlgItem (hWnd, IDSTART), isDirectory(dir) ? TRUE : FALSE);

			return TRUE;
		}

	case WM_COMMAND:
		{
			char buffer[MAX_PATH];
			buffer[0] = '\0';

			HWND edDir = ::GetDlgItem (hWnd, IDC_DIRECTORY);
			if ((HWND)lParam == edDir && HIWORD(wParam) == EN_CHANGE)
			{
				::GetWindowTextA (edDir, buffer, MAX_PATH);
				::EnableWindow (::GetDlgItem (hWnd, IDSTART), isDirectory(buffer) ? TRUE : FALSE);
			}
			else
			{
				switch(LOWORD(wParam))
				{
				case IDC_SELECTDIR:
					{
						if (chooseDir (hWnd, "选择目录", dir))
						{
							::SetWindowTextA (edDir, dir);
							::EnableWindow (::GetDlgItem (hWnd, IDSTART), isDirectory(dir) ? TRUE : FALSE);
						}
						break;
					}
				case IDSTART:
					{
						buffer[0] = '\0';
						::GetWindowTextA (::GetDlgItem (hWnd, IDC_DIRECTORY), buffer, MAX_PATH);
						bool forced = ::SendMessage (::GetDlgItem (hWnd, IDC_FORCED), BM_GETCHECK, 0, 0) == BST_CHECKED;

						canClose = false;
						::EnableWindow (::GetDlgItem (hWnd, IDSTART), FALSE);
						::EnableWindow (::GetDlgItem (hWnd, IDEXIT), FALSE);

						int sizeIncTotal = 0;
						unsigned num = optimizeDDS_r (buffer, forced, ::GetDlgItem (hWnd, IDC_FILELIST), ::GetDlgItem (hWnd, IDC_CURRENTFILE), sizeIncTotal);

						sprintf (buffer, "%d个文件已转换, 总共节省文件大小%dk字节", num, (-sizeIncTotal)/1024);
						::MessageBoxA (hWnd, buffer, "DDS优化", MB_OK|MB_ICONINFORMATION);

						canClose = true;
						::EnableWindow (::GetDlgItem (hWnd, IDSTART), TRUE);
						::EnableWindow (::GetDlgItem (hWnd, IDEXIT), TRUE);

						::SetWindowTextA (::GetDlgItem (hWnd, IDC_CURRENTFILE), "");
						break;
					}
				case IDEXIT:
					{
						::EndDialog (hWnd, 0);
						break;
					}
				}
			}
			break;
		}

	case WM_CLOSE:
		if (canClose)
		{
			EndDialog(hWnd, 0);
		}
		return 1;
	}
	return 0;
}

void doDDSOptimization (HWND hWndParent)
{
	::DialogBoxParamA(::GetModuleHandleA(0), MAKEINTRESOURCEA(IDD_DDSOPT), hWndParent, &DlgProc, 0L);
}

