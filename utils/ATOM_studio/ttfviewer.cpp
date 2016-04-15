#include "StdAfx.h"
#include "ttfviewer.h"
#include "resource.h"

extern char gRootDir[MAX_PATH];
extern int CALLBACK BrowseCallbackProc(HWND hwnd, UINT msg, LPARAM lp, LPARAM pData);
extern BOOL chooseDir (HWND hWndParent, const char *title, char *initialDir);
extern bool isDirectory (const char *filename);
extern void CenterWindow (HWND child, HWND parent);

TTFViewer::TTFViewer (void)
{
}

TTFViewer::~TTFViewer (void)
{
}

void TTFViewer::showDialog (HWND hWnd, const char *fontFileName)
{
	::DialogBoxParam(::GetModuleHandleA(0), MAKEINTRESOURCE(IDD_TTFVIEWER), hWnd, &DlgProc, (LONG)this);
}

static void setEditString (HWND ed, const char *str)
{
	SetWindowTextA (ed, str);
}

static void setCheckboxState (HWND cb, bool checked)
{
	Button_SetCheck (cb, checked?1:0);
}

static void clearComboBox (HWND cb)
{
	ComboBox_ResetContent (cb);
}

static void comboBoxAppend (HWND cb, const char *str)
{
	wchar_t wMsg[1024];
	MultiByteToWideChar(CP_ACP, 0, str, -1, wMsg, 1024);
	ComboBox_AddString (cb, wMsg);
}

bool TTFViewer::updateViewerDialog (HWND hDlg, const char *fontFileName)
{
	_FaceInfos.resize (0);

	HWND hEditFileName = ::GetDlgItem (hDlg, IDC_TTFFILENAME);
	HWND hComboFaceName = ::GetDlgItem (hDlg, IDC_FACENAME);

	SetWindowTextA (hEditFileName, "");
	ComboBox_ResetContent (hComboFaceName);
	updateFaceInfo (hDlg, -1);

	if (!fontFileName)
	{
		return true;
	}

	ATOM_FaceInfo faceInfo;
	void *handle = ATOM_BeginQueryFaceInfo (fontFileName, &faceInfo);
	if (!handle)
	{
		_FaceInfos.clear ();
		return false;
	}

	do
	{
		_FaceInfos.push_back (faceInfo);
	}
	while (ATOM_QueryNextFaceInfo (handle, &faceInfo));

	ATOM_EndQueryFaceInfo (handle);

	if (_FaceInfos.size() == 0)
	{
		return false;
	}

	SetWindowTextA (hEditFileName, fontFileName);
	for (unsigned i = 0; i < _FaceInfos.size(); ++i)
	{
		char tmp[1024];
		sprintf (tmp, "%s - %s", _FaceInfos[i].familyName, _FaceInfos[i].styleName);
		comboBoxAppend (hComboFaceName, tmp);
		//ComboBox_AddString (hComboFaceName, tmp);
	}

	ComboBox_SetCurSel (hComboFaceName, 0);
	updateFaceInfo (hDlg, 0);

	return true;
}

void TTFViewer::updateFaceInfo (HWND hDlg, int faceIndex)
{
	wchar_t wMsg[1024];

	HWND hCheckboxScalable = ::GetDlgItem (hDlg, IDC_FLAGS_SCALABLE);
	HWND hCheckboxFixedSizes = ::GetDlgItem (hDlg, IDC_FLAGS_FIXED_SIZE);
	HWND hCheckboxFixedWidth = ::GetDlgItem (hDlg, IDC_FLAGS_FIXED_WIDTH);
	HWND hCheckboxVertical = ::GetDlgItem (hDlg, IDC_FLAGS_HAS_V_METRICS);
	HWND hCheckboxHorizonal = ::GetDlgItem (hDlg, IDC_FLAGS_HAS_H_METRICS);
	HWND hCheckboxKerning = ::GetDlgItem (hDlg, IDC_FLAGS_HAS_KERNING);
	HWND hCheckboxHinter = ::GetDlgItem (hDlg, IDC_FLAGS_HAS_HINTER);
	HWND hListCharmaps = ::GetDlgItem (hDlg, IDC_CHARMAP_LIST);
	HWND hListFixedSizes = ::GetDlgItem (hDlg, IDC_FIXEDSIZES);
	HWND hEditMisc = ::GetDlgItem (hDlg, IDC_TTFMISC);

	if (faceIndex < 0)
	{
		Button_SetCheck (hCheckboxScalable, false);
		Button_SetCheck (hCheckboxFixedSizes, false);
		Button_SetCheck (hCheckboxFixedWidth, false);
		Button_SetCheck (hCheckboxVertical, false);
		Button_SetCheck (hCheckboxHorizonal, false);
		Button_SetCheck (hCheckboxKerning, false);
		Button_SetCheck (hCheckboxHinter, false);
		ListBox_ResetContent (hListCharmaps);
		ListBox_ResetContent (hListFixedSizes);
		SetWindowTextA (hEditMisc, "");
	}
	else
	{
		const ATOM_FaceInfo &info = _FaceInfos[faceIndex];

		Button_SetCheck (hCheckboxScalable, (info.faceFlags & ATOM_FontFaceFlag_Scalable) != 0);
		Button_SetCheck (hCheckboxFixedSizes, (info.faceFlags & ATOM_FontFaceFlag_FixedSizes) != 0);
		Button_SetCheck (hCheckboxFixedWidth, (info.faceFlags & ATOM_FontFaceFlag_FixedWidth) != 0);
		Button_SetCheck (hCheckboxVertical, (info.faceFlags & ATOM_FontFaceFlag_Vertical) != 0);
		Button_SetCheck (hCheckboxHorizonal, (info.faceFlags & ATOM_FontFaceFlag_Horizontal) != 0);
		Button_SetCheck (hCheckboxKerning, (info.faceFlags & ATOM_FontFaceFlag_Kerning) != 0);
		Button_SetCheck (hCheckboxHinter, (info.faceFlags & ATOM_FontFaceFlag_Hinter) != 0);

		ListBox_ResetContent (hListCharmaps);
		for (unsigned i = 0; i < info.numCharmaps; ++i)
		{
			const char *charmapName;
			switch (info.charmaps[i])
			{
			case ATOM_FontEncodingNone:
				charmapName = "None";
				break;
			case ATOM_FontEncodingMsSymbol:
				charmapName = "Ms Symbol";
				break;
			case ATOM_FontEncodingUnicode:
				charmapName = "Unicode";
				break;
			case ATOM_FontEncodingSJIS:
				charmapName = "SJIS";
				break;
			case ATOM_FontEncodingGB2312:
				charmapName = "GB2312";
				break;
			case ATOM_FontEncodingBIG5:
				charmapName = "BIG5";
				break;
			case ATOM_FontEncodingWANSUNG:
				charmapName = "WANSUNG";
				break;
			case ATOM_FontEncodingJOHAB:
				charmapName = "JOHAB";
				break;
			case ATOM_FontEncodingAdobeStandard:
				charmapName = "Adobe Standard";
				break;
			case ATOM_FontEncodingAdobeExpert:
				charmapName = "Adobe Expert";
				break;
			case ATOM_FontEncodingAdobeCustom:
				charmapName = "Adobe Custom";
				break;
			case ATOM_FontEncodingAdobeLatin1:
				charmapName = "Adobe Latin1";
				break;
			case ATOM_FontEncodingOldLatin2:
				charmapName = "Old Latin2";
				break;
			case ATOM_FontEncodingAppleRoman:
				charmapName = "Apple Roman";
				break;
			case ATOM_FontEncodingUnknown:
			default:
				charmapName = "(Unknown)";
				break;
			}

			MultiByteToWideChar(CP_ACP, 0, charmapName, -1, wMsg, 1024);
			ListBox_AddString (hListCharmaps, wMsg);
		}

		ListBox_ResetContent (hListFixedSizes);
		for (unsigned i = 0; i < info.numFixedSizes; ++i)
		{
			char buffer[256];
			sprintf (buffer, "(%d, %d)", info.fixedSizes[i].width, info.fixedSizes[i].height);
			MultiByteToWideChar(CP_ACP, 0, buffer, -1, wMsg, 1024);
			ListBox_AddString (hListFixedSizes, wMsg);
		}

		SetWindowTextA (hEditMisc, "");
	}
}

INT_PTR TTFViewer::DlgProc (HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static TTFViewer *viewer = NULL;
	switch(message) {
		case WM_INITDIALOG:
			{
				viewer = (TTFViewer*)lParam;
				viewer->updateViewerDialog (hWnd, 0);
				::CenterWindow(hWnd,::GetParent(hWnd));
				return TRUE;
			}

		case WM_COMMAND:
			{
				switch(LOWORD(wParam))
				{
				case IDC_FACENAME:
					{
						if (HIWORD(wParam) == CBN_SELCHANGE)
						{
							int sel = ComboBox_GetCurSel (::GetDlgItem (hWnd, IDC_FACENAME));
							viewer->updateFaceInfo (hWnd, sel);
						}
						break;
					}
				case IDC_BTN_SELECT_FONT:
					{
						OPENFILENAMEA ofn;
						char fileNameBuffer[MAX_PATH];

						memset (&ofn, 0, sizeof(ofn));
						ofn.lStructSize = sizeof(ofn);
						ofn.hwndOwner = hWnd;
						ofn.lpstrFile = fileNameBuffer;
						ofn.lpstrFile[0] = '\0';
						ofn.nMaxFile = sizeof(fileNameBuffer);

						ofn.lpstrFilter = "ttf文件(*.ttf)\0*.ttf\0ttc文件(*.ttc)\0*.ttc\0fon文件(*.fon)\0*.fon\0所有文件(*.*)\0*.*\0\0";
						ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;

						if (::GetOpenFileNameA (&ofn))
						{
							viewer->updateViewerDialog (hWnd, fileNameBuffer);

							return true;
						}
						break;
					}
				case IDOK:
					{
						::EndDialog (hWnd, 0);
						break;
					}
				case IDSAVE:
					{
						break;
					}
				}
				break;
			}

		case WM_CLOSE:
			{
				EndDialog(hWnd, 0);
				break;
			}
			return 1;
	}
	return 0;
}

