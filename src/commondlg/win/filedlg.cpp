#include <ATOM_dbghlp.h>
#include <ATOM_kernel.h>
#include <ATOM_vfs.h>
#include "win/filedlg.h"
#include "filedlg_template.h"
#include "../resource.h"

static char szBuffer[ATOM_VFS::max_filename_length];
static int nBufferLen = ATOM_VFS::max_filename_length;
static char szDesc[ATOM_VFS::max_filename_length];
static int nDescBufferLen = ATOM_VFS::max_filename_length;

//Return szBuffer.
//szBuffer format:
//	Multi select: "path|filename1|filename2|....|filenameN"
//	Single select: "path/filename"
ATOM_FileDlg::ATOM_FileDlg(unsigned dwMode,  const char* szInitPath, const char* szDefaultExt, const char*szDefaultFileName, const char* szFilter, HWND hParent)

{
	m_dwMode			= dwMode;
	if(szInitPath)			m_szInitPath		= szInitPath;
	if(szDefaultExt)		m_szDefaultExt		= szDefaultExt;
	if(szDefaultFileName)	m_szDefaultFileName = szDefaultFileName;
	createFilter(szFilter);
	m_nCurrFilter		= 0;
	m_hParent			= hParent?hParent:ATOM_APP->getMainWindow();

	ATOM_ASSERT(szBuffer);
	m_szBuffer			= szBuffer;
	m_nBufferLen		= nBufferLen;
	m_szDesc = szDesc;
	m_szDesc[0] = '\0';
	m_nDescBufferLen = nDescBufferLen;
}

ATOM_FileDlg::~ATOM_FileDlg(void)
{

}

void ATOM_FileDlg::createFilter(const char* szFilter)
{
	if(szFilter == NULL)
	{
		ATOM_VECTOR<ATOM_STRING> vtExt;
		vtExt.push_back(ATOM_STRING(".*"));
		m_Filter.push_back(std::pair<ATOM_STRING, ATOM_VECTOR<ATOM_STRING> >("All Files(*.*)", vtExt));
		return;
	}

	char szBuffer[256];
	const char *lpFrom = szFilter;
	while(lpFrom)
	{
		int nLen = strlen(lpFrom);
		//decripsion ATOM_STRING.
		const char *lpMode = strstr(lpFrom, "|");
		if(lpMode == NULL)
			//OutputDebugString("ATOM_FileDlg: Error of Filter ATOM_STRING \n");
			break;

		int nRemainLen = strlen(lpMode);
		memcpy(szBuffer, lpFrom, nLen - nRemainLen);
		szBuffer[nLen - nRemainLen] = 0;

		ATOM_STRING szDesc = szBuffer;

		//ext ATOM_STRING...
		lpFrom = lpMode + 1;
		nLen   = strlen(lpFrom);
		lpMode = strstr(lpFrom, "|");
		if(lpMode == NULL)
		{
			OutputDebugString("ATOM_FileDlg: Error of Filter ATOM_STRING \n");
			break;
		}
		nRemainLen = strlen(lpMode);
		memcpy(szBuffer, lpFrom, nLen - nRemainLen);
		szBuffer[nLen - nRemainLen] = 0;

		ATOM_STRING szExt = szBuffer;

		ATOM_VECTOR<ATOM_STRING> vtExt;
		//Extract .ext from szExt.
		while(1)
		{
			int nDot = szExt.find_first_of('.');
			if(nDot < 0 || nDot >= szExt.size() - 1)
			{
				break;
			}
			int nSemicolon = szExt.find_first_of(';', nDot);
			if(nSemicolon < 0)
			{
				szExt.copy(szBuffer, szExt.size() - nDot, nDot);
				szBuffer[szExt.size() - nDot] = 0;
				vtExt.push_back(ATOM_STRING(szBuffer));
				szExt = "";
			}
			else
			{
				szExt.copy(szBuffer, nSemicolon - nDot, nDot);
				szBuffer[nSemicolon - nDot] = 0;
				_strlwr(szBuffer);
				vtExt.push_back(ATOM_STRING(szBuffer));
				szExt.copy(szBuffer, szExt.size() - nSemicolon - 1, nSemicolon + 1);
				szBuffer[szExt.size() - nSemicolon - 1] = 0;
				szExt = szBuffer;
			}
		}

    m_Filter.push_back(std::pair<ATOM_STRING, ATOM_VECTOR<ATOM_STRING> >(szDesc, vtExt));
		lpFrom = lpMode + 1;
	}
}

int ATOM_FileDlg::doModal()
{
	return DialogBoxIndirectParam(NULL, (LPCDLGTEMPLATE)dlg_102, m_hParent,
							l3FileDlgProc, (LPARAM)this);
}

BOOL ATOM_FileDlg::OnInitDialog(HWND hWnd, ATOM_FileDlg* lpDlg)
{
#pragma warning(disable : 4311)
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (long)lpDlg);
#pragma warning( default: 4311)
	//TODO: Init...
	InitCommonControls();

	::SetWindowText (hWnd, lpDlg->m_szTitle.c_str());
	lpDlg->m_hWnd = hWnd;
	lpDlg->m_hLCFile = GetDlgItem(hWnd, IDC_LIST_FILE);
	ATOM_ASSERT(lpDlg->m_hLCFile);
	lpDlg->m_hCBRange = GetDlgItem(hWnd, IDC_COMBO_RANGE);
	ATOM_ASSERT(lpDlg->m_hCBRange);
	lpDlg->m_hCBFileType = GetDlgItem(hWnd, IDC_COMBO_FILETYPE);
	ATOM_ASSERT(lpDlg->m_hCBFileType);
	lpDlg->m_hEdtFileName = GetDlgItem(hWnd, IDC_EDIT_FILENAME);
	ATOM_ASSERT(lpDlg->m_hEdtFileName);
	lpDlg->m_hBtnListStyle = GetDlgItem(hWnd, IDC_BUTTON_LISTSTYLE);
	ATOM_ASSERT(lpDlg->m_hBtnListStyle);
	lpDlg->m_hBtnParentDir = GetDlgItem(hWnd, IDC_BUTTON_PARENTDIR);
	ATOM_ASSERT(lpDlg->m_hBtnParentDir);
	lpDlg->m_hBtnOK = GetDlgItem(hWnd, IDOK);
	ATOM_ASSERT(lpDlg->m_hBtnOK);


	//ATOM_ListView ImageList.
	lpDlg->m_hImageListLarge = ImageList_Create(32, 32, ILC_COLORDDB | ILC_MASK, 10, 10);
	ImageList_SetBkColor(lpDlg->m_hImageListLarge, CLR_NONE );
	lpDlg->m_hImageListSmall = ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK, 10, 10);
	ImageList_SetBkColor(lpDlg->m_hImageListSmall, CLR_NONE );
	HICON hSmall, hLarge;
	if(lpDlg->getShellIcon(3, hSmall, hLarge))	//ImageFolder Close
	{
		lpDlg->m_nImageFolderClose = ImageList_AddIcon(lpDlg->m_hImageListLarge, hLarge);
		lpDlg->m_nImageFolderClose = ImageList_AddIcon(lpDlg->m_hImageListSmall, hSmall);
		//lpDlg->insertImage(3, lpDlg->m_nImageFolderClose);
	}
	if(lpDlg->getShellIcon(4, hSmall, hLarge))	//ImageFolder Open
	{
		lpDlg->m_nImageFolderOpen = ImageList_AddIcon(lpDlg->m_hImageListLarge, hLarge);
		lpDlg->m_nImageFolderOpen = ImageList_AddIcon(lpDlg->m_hImageListSmall, hSmall);
		//lpDlg->insertImage(4, lpDlg->m_nImageFolderOpen);
	}
	if(lpDlg->getShellIcon(0, hSmall, hLarge))	//ImageFolder Open
	{
		lpDlg->m_nImageFileDefault = ImageList_AddIcon(lpDlg->m_hImageListLarge, hLarge);
		lpDlg->m_nImageFileDefault = ImageList_AddIcon(lpDlg->m_hImageListSmall, hSmall);
		//lpDlg->insertImage(0, lpDlg->m_nImageFileDefault);
	}

	//ATOM_Dialog Title
	if(lpDlg->m_dwMode & MODE_SAVEFILE)
	{
		SetWindowText(hWnd, "Save file...");
	}
	else if(lpDlg->m_dwMode & MODE_OPENDIR)
	{
		SetWindowText(hWnd, "Open directory...");
	}
	else
	{
		SetWindowText(hWnd, "Open file...");
	}

	//ATOM_ListView of the file.
	ListView_SetImageList(lpDlg->m_hLCFile, lpDlg->m_hImageListLarge, LVSIL_NORMAL);
	ListView_SetImageList(lpDlg->m_hLCFile, lpDlg->m_hImageListSmall, LVSIL_SMALL);
	DWORD dwStyle = GetWindowLong(lpDlg->m_hLCFile, GWL_STYLE);
	DWORD dwView  = dwStyle & LVS_TYPEMASK;
	dwStyle &= ~dwView;
	dwView = LVS_LIST;
	dwStyle |= dwView;
	if(lpDlg->m_dwMode & MODE_SAVEFILE || (lpDlg->m_dwMode & MODE_MULTISELECT) != MODE_MULTISELECT || lpDlg->m_dwMode & MODE_OPENDIR)
		dwStyle |= LVS_SINGLESEL;
	SetWindowLongPtr(lpDlg->m_hLCFile, GWL_STYLE, dwStyle);

	//Default file name
	if(lpDlg->m_szDefaultFileName.size() > 0)
		SetWindowText(lpDlg->m_hEdtFileName, lpDlg->m_szDefaultFileName.c_str());
	//Filter
	if(lpDlg->m_Filter.size() > 0)
	{
		for(FileFilter::iterator i = lpDlg->m_Filter.begin(); i != lpDlg->m_Filter.end(); i++)
		{
			SendMessage(lpDlg->m_hCBFileType, CB_ADDSTRING, 0, (LPARAM) i->first.c_str());
		}
		SendMessage(lpDlg->m_hCBFileType, CB_SETCURSEL, 0, 0);
	}
	//Init Path
	if(lpDlg->m_szInitPath.size() > 0)
	{
		lpDlg->m_szSelectedPath = lpDlg->m_szInitPath;
		//char szBuffer[256];
		//int nPath = lpDlg->m_szInitPath.find_last_of('/');
		//if(nPath < 0)
		//{
		//	nPath = lpDlg->m_szInitPath.find_last_of('\\');
		//}
		//if(nPath >= 0)
		//{
		//	lpDlg->m_szInitPath.copy(szBuffer, nPath + 1, 0);
		//	//memcpy(szBuffer, lpDlg->m_szInitPath.c_str(), nPath + 1);
		//	szBuffer[nPath + 1] = 0;
		//	lpDlg->m_szSelectedPath = szBuffer;

		//	int nFilenameLen = lpDlg->m_szInitPath.size() - nPath - 1;
		//	if(nFilenameLen > 0)
		//	{
		//		lpDlg->m_szInitPath.copy(szBuffer, nFilenameLen, nPath + 1);
		//		szBuffer[nFilenameLen] = 0;
		//		lpDlg->m_ltSelectedFileName.push_back(ATOM_STRING(szBuffer));
		//	}
		//}
	}
	else
	{
		char szBuffer[MAX_PATH];
		ATOM_GetCWD(szBuffer);
		lpDlg->m_szSelectedPath = szBuffer;
	}
	lpDlg->listFile((lpDlg->m_dwMode & ATOM_FileDlg::MODE_OPENDIR) == 0);

	return TRUE;
}

void ATOM_FileDlg::listFile(bool showFile)
{
	ComboBox_ResetContent(m_hCBRange);

	const char* szFile = 0;
	if(m_szSelectedPath.empty()
		|| !ATOM_IsDirectory(m_szSelectedPath.c_str()))
	{
		m_szSelectedPath = "/";
	}

	//m_hCBRange中列出m_szSelectPath各级目录。
	int nSlash = m_szSelectedPath.find_first_of('/', 0);
	if (nSlash < 0)
		nSlash = m_szSelectedPath.find_first_of('\\', 0);
	int nFrom = 0;
	for(;;)
	{
		ATOM_STRING szDir;
		if(nSlash >= 0)
		{
			szDir.assign(m_szSelectedPath, nFrom, nSlash - nFrom + 1);
			ComboBox_AddString(m_hCBRange, szDir.c_str());
		}
		else
		{
			szDir.assign(m_szSelectedPath, nFrom, m_szSelectedPath.size() - nFrom);
			ComboBox_AddString(m_hCBRange, szDir.c_str());
			break;
		}

		nFrom = nSlash + 1;
		if(nFrom >= m_szSelectedPath.size())
			break;

		nSlash = m_szSelectedPath.find_first_of('/', nFrom);
		if (nSlash < 0)
			nSlash = m_szSelectedPath.find_first_of('\\', nFrom);
	}
	int nLineCount = ComboBox_GetCount(m_hCBRange);
	ATOM_ASSERT(nLineCount > 0);
	ComboBox_SetCurSel(m_hCBRange, nLineCount-1);


	//5. 列出szRealPath下面的文件。
	ListView_DeleteAllItems(m_hLCFile);

	LVITEM item;
	item.mask = LVIF_TEXT | LVIF_IMAGE;
	item.iSubItem = 0;

	SHFILEINFO fileInfo;

	int nIndex = 0;
	char szBuffer[256] = "";
	ATOM_SetCWD(m_szSelectedPath.c_str());
	ATOM_VFS::finddata_t *findData = ATOM_FindFirst(m_szSelectedPath.c_str());
	while (findData)
	{
		szFile = findData->filenameNoPath;
		if(ATOM_IsDirectory(szFile))
		{//目录
			if(strcmp(szFile, ".") != 0)		//"."当前目录不列出。
			{
				item.iImage  = m_nImageFolderClose;
				strcpy(szBuffer, szFile);
				item.pszText = szBuffer;
				item.iItem   = nIndex;
				ListView_InsertItem(m_hLCFile, &item);
			}
		}//End of  目录
		else if (showFile)
		{//文件
			//过滤Filter
			ATOM_ASSERT(m_nCurrFilter >= 0);
			bool bShowFile = false;
			ATOM_STRING szExt = "";
			ATOM_STRING szFilename = szFile;
			int nDot = szFilename.find_last_of('.');
			if(nDot >= 0)
			{//有扩展名的文件
				szExt = &szFilename[nDot];
			}

			ATOM_VECTOR<ATOM_STRING>& vtFilter = m_Filter[m_nCurrFilter].second;
			for(int nFilterIndex = 0; nFilterIndex < vtFilter.size(); nFilterIndex++)
			{
				if(vtFilter[nFilterIndex] == ATOM_STRING(".*"))
				{
					bShowFile = true;
					break;
				}
				else
				{
					if (!szExt.empty ())
					{
						_strlwr(&szExt[0]);
					}

					if(stricmp(vtFilter[nFilterIndex].c_str(), szExt.c_str()) == 0)
					{
						bShowFile = true;
						break;
					}
				}
			}//for each filter .ext

			if(bShowFile)
			{
				int nImage = -1;
				if(SHGetFileInfo(szFile, 0, &fileInfo, sizeof(fileInfo), SHGFI_LARGEICON | SHGFI_ICON))
				{
					nImage = lookupImage(fileInfo.iIcon);
					if(nImage < 0)
					{
						nImage = ImageList_AddIcon(m_hImageListLarge, fileInfo.hIcon);
						insertImage(fileInfo.iIcon, nImage);
						SHGetFileInfo(szFile, 0, &fileInfo, sizeof(fileInfo), SHGFI_SMALLICON| SHGFI_ICON);
						ImageList_AddIcon(m_hImageListSmall, fileInfo.hIcon);
					}
				}
				if(nImage == -1)
					nImage = m_nImageFileDefault;

				item.iImage  = nImage;
				strcpy(szBuffer, szFile);
				item.pszText = szBuffer;
				item.iItem   = nIndex;
				ListView_InsertItem(m_hLCFile, &item);
			}//End show file...
		}//end of 文件

		++nIndex;
    if (!ATOM_FindNext (findData))
    {
      ATOM_CloseFind (findData);
      findData = 0;
    }
	}//End while(1)

	//Find Seleted file in ListFile...
	if( !(m_ltSelectedFileName.empty()) )
	{
		LVFINDINFO findInfo;
		findInfo.flags = LVFI_STRING;
		strcpy(szBuffer, m_ltSelectedFileName.begin()->c_str());
		findInfo.psz   = szBuffer;

		nIndex = ListView_FindItem(m_hLCFile, -1, &findInfo);
		if(LB_ERR != nIndex)
		{
			ListView_SetItemState(m_hLCFile, nIndex, LVIS_SELECTED, LVIS_SELECTED);
		}
	}
}

BOOL ATOM_FileDlg::OnButtonListViewFileStyle()
{
	DWORD dwStyle = GetWindowLong(m_hLCFile, GWL_STYLE);
	DWORD dwView = dwStyle & LVS_TYPEMASK;
	dwStyle &= ~dwView;
	if(dwView == LVS_ICON)
	{
		dwView = LVS_LIST;
	}
	//else if(dwView == LVS_SMALLICON)
	//{
	//	dwView = LVS_LIST;
	//}
	//else if(dwView == LVS_LIST)
	//{
	//	dwView = LVS_REPORT;
	//}
	else if(dwView == LVS_LIST)
	{
		dwView = LVS_ICON;
	}
	dwStyle |= dwView;
	SetWindowLongPtr(m_hLCFile, GWL_STYLE, dwStyle);
	return TRUE;
}

BOOL ATOM_FileDlg::OnCBRangeSelChange()
{
	int nSel = ComboBox_GetCurSel(m_hCBRange);
	if(CB_ERR == nSel)
	{
		m_szSelectedPath = "";
	}
	else
	{
		char szBuffer[256];
		int  nIndex = nSel;
		m_szSelectedPath = "";
		for(;nIndex >= 0;)
		{
			ComboBox_GetLBText(m_hCBRange, nIndex, szBuffer);
			m_szSelectedPath = szBuffer + m_szSelectedPath;

			nIndex--;
		}

	}
	listFile((m_dwMode & ATOM_FileDlg::MODE_OPENDIR) == 0);
	return TRUE;
}

BOOL ATOM_FileDlg::OnLBFileSelChange(LPNMLISTVIEW lpNMListView)
{
	char szBuffer[256];
	int nItemCount = ListView_GetItemCount(m_hLCFile);
	m_ltSelectedFileName.clear();
	for(int i = 0; i < nItemCount; i++)
	{
		if(LVIS_SELECTED == ListView_GetItemState(m_hLCFile, i, LVIS_SELECTED))
		{
			ListView_GetItemText(m_hLCFile, i, 0, szBuffer, 256);
			if(!ATOM_IsDirectory(szBuffer))
			{
				m_ltSelectedFileName.push_back(szBuffer);
			}
		}
	}

	ATOM_STRING szTemp;
  for(ATOM_VECTOR<ATOM_STRING>::iterator itSel = m_ltSelectedFileName.begin(); itSel != m_ltSelectedFileName.end(); itSel++)
	{
		if( !(szTemp.empty()) )
		{
			szTemp += "; ";
		}
		szTemp += *itSel;
	}

  if (!szTemp.empty())
  {
	  Edit_SetText(m_hEdtFileName, szTemp.c_str());
  }

	return TRUE;
}


BOOL ATOM_FileDlg::OnLBFileDBClick(void *lpNMItemActive)
{
	//GetCurrSel
	char szBuffer[256];
	int nItemCount = ListView_GetItemCount(m_hLCFile);
	for(int i = 0; i < nItemCount; i++)
	{
		if(LVIS_SELECTED == ListView_GetItemState(m_hLCFile, i, LVIS_SELECTED))
		{
			ListView_GetItemText(m_hLCFile, i, 0, szBuffer, 256);
			if(ATOM_IsDirectory(szBuffer))
			{//选中目录 + double click
				if(strcmp(szBuffer, "..") == 0)
				{//到上一层目录
					OnButtonParentDir();
					return TRUE;
				}
				else
				{
					if(m_szSelectedPath.size() > 0 && m_szSelectedPath[m_szSelectedPath.size() - 1] != '/' && m_szSelectedPath[m_szSelectedPath.size() - 1] != '\\')
					{
						m_szSelectedPath += "/";
					}
					m_szSelectedPath += szBuffer;
					listFile((m_dwMode & ATOM_FileDlg::MODE_OPENDIR) == 0);
					return TRUE;
				}
			}//end of IsDirectory
			else
			{//选中文件，返回
				return OnOK();
			}
		}
	}

	return TRUE;
}

BOOL ATOM_FileDlg::OnOK()
{
	char szBuffer[256];
	int nItemCount = ListView_GetItemCount(m_hLCFile);
	for(int i = 0; i < nItemCount; i++)
	{
		if(LVIS_SELECTED == ListView_GetItemState(m_hLCFile, i, LVIS_SELECTED))
		{
			ListView_GetItemText(m_hLCFile, i, 0, szBuffer, 256);
			if(ATOM_IsDirectory(szBuffer))
			{//选中目录
				if(strcmp(szBuffer, "..") == 0)
				{//到上一层目录
					OnButtonParentDir();
				}
				else
				{
					if(m_szSelectedPath.size() > 0 && m_szSelectedPath[m_szSelectedPath.size() - 1] != '/' && m_szSelectedPath[m_szSelectedPath.size() - 1] != '\\')
					{
						m_szSelectedPath += "/";
					}
					m_szSelectedPath += szBuffer;
					listFile((m_dwMode & ATOM_FileDlg::MODE_OPENDIR) == 0);
					SetFocus(m_hLCFile);
					if(ListView_GetItemCount(m_hLCFile) > 0)
						ListView_SetItemState(m_hLCFile, 0, LVIS_FOCUSED, LVIS_FOCUSED);
					return TRUE;
				}
			}//end of IsDirectory
		}
	}
	//copy m_szSelectedPath + m_ltSelectFile to szBuffer...
	int nRemainSize = m_nBufferLen;
	bool bNotEnoughBuffer = false;
	if(m_szSelectedPath.size() > m_nBufferLen)
	{
		bNotEnoughBuffer = true;
	}
	else
	{
		if (m_szDesc && m_nDescBufferLen)
		{
			unsigned sz = m_Filter[m_nCurrFilter].first.length();
			if (sz >= m_nDescBufferLen)
			{
				sz = m_nDescBufferLen - 1;
			}
			strncpy (m_szDesc, m_Filter[m_nCurrFilter].first.c_str(), sz);
			m_szDesc[sz] = '\0';

			char *p = strrchr (m_szDesc, '(');
			if (p)
			{
				*p = '\0';
			}
		}

		strcpy(m_szBuffer, m_szSelectedPath.c_str());
		nRemainSize -= m_szSelectedPath.size();
		if((m_dwMode & MODE_SAVEFILE) != MODE_SAVEFILE && (m_dwMode & MODE_OPENDIR) != MODE_OPENDIR)
		{//Open file mode
			if((m_dwMode & MODE_MULTISELECT) != MODE_MULTISELECT)
			{//单选---------------------------------------------------------------------------------------------
				Edit_GetText(m_hEdtFileName, szBuffer, 256);

				if(strlen(szBuffer) == 0)
				{
					m_ltSelectedFileName.push_back (szBuffer);
					return TRUE;
				}

				//判断szFile是否存在？
				ATOM_STRING szFile = szBuffer;
				bool bFoundFile = false;
				for(int i = 0; i < nItemCount; i++)
				{
					ListView_GetItemText(m_hLCFile, i, 0, szBuffer, 256);
					if(!ATOM_IsDirectory(szBuffer))
					{
						if(stricmp(szFile.c_str(), szBuffer) == 0)
						{//存在
							bFoundFile = true;
							ListView_SetItemState(m_hLCFile, i, LVIS_SELECTED, LVIS_SELECTED);
							break;
						}
					}
				}
				if(!bFoundFile)
				{
					return TRUE;
				}
				else if(szFile.size() + 1> nRemainSize)
				{
					bNotEnoughBuffer = true;
				}
				else
				{
					if(m_szBuffer[strlen(m_szBuffer) - 1] != '/' && m_szBuffer[strlen(m_szBuffer) - 1] != '\\')
						strcat(m_szBuffer, "/");
					strcat(m_szBuffer, szFile.c_str());
				}

				m_ltSelectedFileName.resize (0);
				m_ltSelectedFileName.push_back (m_szBuffer);
				//End 判断szFile是否存在？

			}//End 单选 open 模式
			else
			{//多选---------------------------------------------------------------------------------------------
        for(ATOM_VECTOR<ATOM_STRING>::iterator itFile = m_ltSelectedFileName.begin();
					itFile != m_ltSelectedFileName.end() && nRemainSize >= 0; itFile++)
				{
					ATOM_STRING s = m_szBuffer;
					unsigned l = s.length ();
					if (l == 0 || s[l-1] != '/')
					{
						s += '/';
					}
					*itFile = s + *itFile;
				}
			}//End 多选 open 模式
		}//End open mode
		else if(m_dwMode & MODE_OPENDIR)
		{//打开目录mode---------------------------------------------------------------------------------------------
			m_ltSelectedFileName.resize (0);
			m_ltSelectedFileName.push_back (m_szSelectedPath);

			if(m_szSelectedPath.size() > m_nBufferLen)
			{
				bNotEnoughBuffer = true;
			}
			else if(m_szSelectedPath.empty() )
				return TRUE;
			else
			{
				strcpy(m_szBuffer, m_szSelectedPath.c_str());
				if(m_szBuffer[strlen(m_szBuffer) - 1] != '/' && m_szBuffer[strlen(m_szBuffer) - 1] != '\\')
					strcat(m_szBuffer, "/");
			}
		}//End open dir mode.
		else
		{//Write mode---------------------------------------------------------------------------------------------
			Edit_GetText(m_hEdtFileName, szBuffer, 256);
			m_ltSelectedFileName.resize (0);

			if(strlen(szBuffer) == 0)
			{
				m_ltSelectedFileName.push_back (szBuffer);
				return TRUE;
			}
			else
			{//
				if( !(m_szDefaultExt.empty()) )
				{
					_strlwr(szBuffer);
					_strlwr(&m_szDefaultExt[0]);

					int nLen = strlen(szBuffer);

					if( (nLen > m_szDefaultExt.size() && stricmp(&szBuffer[nLen - m_szDefaultExt.size()], m_szDefaultExt.c_str()) != 0)
						|| (nLen <= m_szDefaultExt.size()))
					{
						strcat(szBuffer, ".");
						strcat(szBuffer, m_szDefaultExt.c_str());
					}
				}
				ATOM_STRING szFile = szBuffer;
				if(m_dwMode & MODE_OVERWRITEPROMPT)
				{//判断szFile是否已经存在？
					for(int i = 0; i < nItemCount; i++)
					{
						ListView_GetItemText(m_hLCFile, i, 0, szBuffer, 256);
						if(!ATOM_IsDirectory(szBuffer))
						{
							if(stricmp(szFile.c_str(), szBuffer) == 0)
							{//如果存在，提示是否需要覆盖。
								char szTemp[512];
								sprintf(szTemp, "%s has exist.\nDo you want to overwirte it?", szBuffer);
								if(MessageBox(m_hWnd, szTemp, "Save warning", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
								{
									m_ltSelectedFileName.push_back (szBuffer);
									return TRUE;
								}
								else
								{
									break;
								}
							}
						}
					}//End 判断szFile是否已经存在？
				}

				if(szFile.size() + 1> nRemainSize)
				{
					bNotEnoughBuffer = true;
				}
				else
				{
					if (m_szBuffer[strlen(m_szBuffer)-1] != '/')
					{
						strcat(m_szBuffer, "/");
					}
					strcat(m_szBuffer, szFile.c_str());
				}
				m_ltSelectedFileName.push_back (m_szBuffer);
			}//End Write mode
		}
	}

	if(bNotEnoughBuffer)
	{
		m_szBuffer[0] = 0;
	}
	return EndDialog(m_hWnd, IDOK);
}

void ATOM_FileDlg::setTitle (const char *title)
{
	m_szTitle = title ? title : "";
}

int ATOM_FileDlg::getNumFilesSelected (void) const
{
	return m_ltSelectedFileName.size ();
}

const char *ATOM_FileDlg::getSelectedFileName (int index) const
{
	return m_ltSelectedFileName[index].c_str();
}

const char *ATOM_FileDlg::getSelectedFileDesc (void) const
{
	return szDesc;
}

BOOL ATOM_FileDlg::OnButtonParentDir()
{
	int nIndex = ComboBox_GetCurSel(m_hCBRange);
	if(nIndex != CB_ERR && nIndex > 0)
	{
		ComboBox_SetCurSel(m_hCBRange, nIndex - 1);
		OnCBRangeSelChange();
	}
	return TRUE;
}

BOOL ATOM_FileDlg::OnCBFileTypeSelChange()
{
	m_nCurrFilter = ComboBox_GetCurSel(m_hCBFileType);
	listFile((m_dwMode & ATOM_FileDlg::MODE_OPENDIR) == 0);
	return TRUE;
}

BOOL ATOM_FileDlg::OnLBFileFilenameChange(NMLVDISPINFO* lpInfo)
{
	return TRUE;
}

BOOL ATOM_FileDlg::OnButtonNewDir()
{
	char szNewName[256];
	int i = 0;
	for(;;i++)
	{
		sprintf(szNewName, "New Folder(%d)", i);
		if(ATOM_FileExists(szNewName))
			continue;

		if(!ATOM_CreateDirectory(szNewName))
			return FALSE;

		listFile((m_dwMode & ATOM_FileDlg::MODE_OPENDIR) == 0);

		LVFINDINFO findInfo;
		findInfo.flags = LVFI_STRING;
		findInfo.psz   = szNewName;

		int nIndex = ListView_FindItem(m_hLCFile, -1, &findInfo);
		if(LB_ERR != nIndex)
		{
			SetFocus(m_hLCFile);
			ListView_SetItemState(m_hLCFile, nIndex, LVIS_SELECTED, LVIS_SELECTED);
			ListView_EditLabel(m_hLCFile, nIndex);
		}

		break;
	}
	return TRUE;
}


INT_PTR CALLBACK ATOM_FileDlg::l3FileDlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch (message) // 接受消息循环的函数，采用switch处理各种消息
	{
	case WM_INITDIALOG:
		return OnInitDialog(hwnd, (ATOM_FileDlg*)lParam);
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if(HIWORD(wParam) == 0)
				return ((ATOM_FileDlg*)GetWindowLong(hwnd, GWLP_USERDATA))->OnOK();
			break;
		case IDCANCEL:
			((ATOM_FileDlg*)GetWindowLong(hwnd, GWLP_USERDATA))->m_ltSelectedFileName.clear ();
			return EndDialog(hwnd, IDCANCEL);
		case IDC_BUTTON_PARENTDIR:
			return ((ATOM_FileDlg*)GetWindowLong(hwnd, GWLP_USERDATA))->OnButtonParentDir();
		case IDC_BUTTON_LISTSTYLE:
			{
				((ATOM_FileDlg*)GetWindowLong(hwnd, GWLP_USERDATA))->OnButtonListViewFileStyle();
			}
			break;
		case IDC_BUTTON_NEWDIR:
			return ((ATOM_FileDlg*)GetWindowLong(hwnd, GWLP_USERDATA))->OnButtonNewDir();
			break;
		case IDC_COMBO_RANGE:
			{
				switch(HIWORD(wParam))
				{
				case CBN_SELCHANGE:
					return ((ATOM_FileDlg*)GetWindowLong(hwnd, GWLP_USERDATA))->OnCBRangeSelChange();
				}
			}
			break;
		case IDC_COMBO_FILETYPE:
			{
				switch(HIWORD(wParam))
				{
				case CBN_SELCHANGE:
					return ((ATOM_FileDlg*)GetWindowLong(hwnd, GWLP_USERDATA))->OnCBFileTypeSelChange();
				}
			}
			break;
		}
		break;
	case WM_NOTIFY:
		switch (wParam)
		{
		case IDC_LIST_FILE:
			{
				NMHDR* lpNMHeader = (NMHDR*)lParam;
				switch (lpNMHeader->code)
				{
				case LVN_ITEMCHANGED:
					return ((ATOM_FileDlg*)GetWindowLong(hwnd, GWLP_USERDATA))->OnLBFileSelChange((LPNMLISTVIEW)lParam);
				case NM_DBLCLK:
					return ((ATOM_FileDlg*)GetWindowLong(hwnd, GWLP_USERDATA))->OnLBFileDBClick((LPNMITEMACTIVATE)lParam);
				case LVN_ENDLABELEDIT:
					return ((ATOM_FileDlg*)GetWindowLong(hwnd, GWLP_USERDATA))->OnLBFileFilenameChange((NMLVDISPINFO*)lParam);
				}
			}
			break;
		}
		break;
	}

	return FALSE;//DefDlgProc(hwnd, message, wParam, lParam); // 默认消息处理
}

int ATOM_FileDlg::lookupImage(int nSysImageIndex)
{
  ATOM_MAP<int,int>::iterator i = m_mpImageFile.find(nSysImageIndex);
	if(i != m_mpImageFile.end())
	{
		return i->second;
	}
	return -1;
}
void ATOM_FileDlg::insertImage(int nSysImageIndex, int nMyImageIndex)
{
  m_mpImageFile.insert(std::pair<int,int>(nSysImageIndex, nMyImageIndex));
}

//nIndex here is system imagelist index.
BOOL ATOM_FileDlg::getShellIcon(int nIndex, HICON& hSmall, HICON& hLarge)
{
	HICON hIcon = NULL;

	//HKEY hkeyShellIcons;
	//if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons",
	//				0, KEY_READ, &hkeyShellIcons) == ERROR_SUCCESS)
	//{
	//	char szBuffer[ MAX_PATH];
	//	DWORD dwSize = MAX_PATH;
	//	char szIndex[4];
	//	sprintf( szIndex, "%d", nIndex );
	//
	//	if (RegQueryValueEx( hkeyShellIcons, szIndex, NULL, NULL, (LPBYTE)szBuffer, &dwSize) == ERROR_SUCCESS)
	//	{
	//		CString strFile, strIndex;
	//		AfxExtractSubString( strFile, szBuffer, 0, _T(',') );
	//		AfxExtractSubString( strIndex, szBuffer, 1, _T(',') );
	//		ExtractIconEx( strFile, atoi(strIndex), NULL, &hIcon, 1 );
	//	}
	//
	//	RegCloseKey( hkeyShellIcons );
	//}

	// Add folder images
	//if (!hIcon)
	ExtractIconEx("SHELL32.DLL", nIndex, &hLarge, &hSmall, 1);
	return TRUE;
}


//0	默认 图标
//1	默认的 .doc 图标
//2	可执行文件图标
//3	关闭的文件夹图标
//4	打开的文件夹图标
//5	5.25' 驱动器图标
//6	3.5' 驱动器图标
//7	可移动的驱动器图标
//8	硬盘驱动器图标
//9	网络驱动器图标
//10	断开的网络驱动器图标
//11	CD-ROM驱动器图标
//12	RAM驱动器图标
//13	整个网络图标
//14	网络连接图标 u
//15	网络工作站图标
//16	本地打印机图标
//17	网络图标 u
//18	网络工作组图标 u
//19	程序组图标 s
//20	文档图标 s
//21	设置图标 s
//22	查找图标 s
//23	帮助图标 s
//24	运行图标 s
//25	睡眠图标 s
//26	Docking Station 图标
//27	关机图标 s
//28	共享图标 t
//29	快捷方式的箭头图标
//30	大箭头图标 u
//31	空回收站图标 *
//32	满的回收站图标
//33	拨号网络图标 *
//34	桌面图标
//35	控制台图标
//36	程序组图标 s
//37	打印机文件夹图标
//38	字体文件夹图标
//39	Windows旗帜图标
//40	Audio CD 图标
//还有一些图标可以在Hkey_Classes_Root\CLSID下面的键中设置，包括上面的15、17、31、32、33、35、37和38 。
//这些图标有：
//	1、我的计算机 HKEY_CLASSES_ROOT\CLSID\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\ DefaultIcon\ 默认值 = explorer.exe,0
//  2、网络邻居 HKEY_CLASSES_ROOT\CLSID\{208D2C60-3AEA-1069-A2D7-08002B30309D}\ DefaultIcon\ 默认值= shell32.dll,17
//	3、回收站 HKEY_CLASSES_ROOT\CLSID\{645FF040-5081-101B-9F08-00AA002F954E}\ DefaultIcon\ 默认值= shell32.dll,32 空的默认值= shell32.dll,31 满的默认值= shell32.dll,32
//	4、收件箱 HKEY_CLASSES_ROOT\CLSID\{00020D75-0000-0000-C000-000000000046}\ DefaultIcon\ 默认值= c:\exchange\exchng32.exe
//	5、Internet Explore Internet Explore 4.0以上： HKEY_CLASSES_ROOT\CLSID\{FBF23B42-E3F0-101B-8488-00AA003E56F8}\ DefaultIcon\
//			默认值=C:\Program Files\Internet Explorer\iexplore.exe,-32528
//	   Internet Explorer Version 3.x： HKEY_CLASSES_ROOT\CLSID\{FBF23B42-E3F0-101B-8488-00AA003E56F8}\ DefaultIcon\
//			默认值= C:\Program Files\Internet Explorer\iexplore.exe,0"
