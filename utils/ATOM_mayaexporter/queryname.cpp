#include "queryname.h"
#include "resource.h"

extern HINSTANCE hModelHandle;

struct QueryNameInfo
{
	std::string str;
	std::string title;
	VerifyFunction verifyFunction;
	void *verifyData;
};

INT_PTR CALLBACK QueryNameDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static QueryNameInfo *info = 0;

	switch(message) 
	{
	case WM_INITDIALOG:
		{
			info = (QueryNameInfo*)lParam;
			::SetWindowText (hWnd, info->title.c_str());
			return TRUE;
		}
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDOK:
				char buffer[1024];
				::GetWindowText (::GetDlgItem (hWnd, IDC_NAME), buffer, 1024);
				if (strlen(buffer) > 63)
				{
					::MessageBox (hWnd, "Ãû×ÖÌ«³¤!", "atom exporter", MB_OK|MB_ICONHAND);
					break;
				}
				if (!info->verifyFunction || info->verifyFunction(buffer, info->verifyData))
				{
					info->str = buffer;
					::EndDialog (hWnd, IDOK);
					break;
				}
				break;
			case IDCANCEL:
				{
					::EndDialog (hWnd, IDCANCEL);
					break;
				}
			default:
				break;
			}
			break;
		}
	case WM_CLOSE:
		{
			::EndDialog (hWnd, IDCANCEL);
			break;
		}
	};
	return 0;
}

bool QueryName (HWND hWndParent, const char *title, std::string &result, VerifyFunction verifyFunction, void *verifyData)
{
	QueryNameInfo info;
	info.title = title ? title : "";
	info.verifyFunction = verifyFunction;
	info.verifyData = verifyData;

	if (IDOK == ::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_QUERYNAME), hWndParent, QueryNameDlgProc, (LPARAM)&info))
	{
		result = info.str;
		return true;
	}

	return false;
}

