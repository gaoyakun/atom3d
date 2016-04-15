#include "selectprop.h"
#include "resource.h"

extern HINSTANCE hModelHandle;

struct PropInfo
{
	const char *desc;
	const char *key;
};

static const PropInfo propInfos[] = 
{
	{ "公告板",			"ATOM_Billboard"		},
	{ "非平滑UV动画",	"UVNoIpol"		},
	{ "融合模式",		"BlendMode"		},
	{ "材质风格",		"MaterialStyle"	},
	{ "光照模式",		"LightingMode"	},
	{ "刀光颜色1",		"LightnColor1"	},
	{ "刀光颜色2",		"LightnColor2"	},
	{ "刀光颜色3",		"LightnColor3"	},
	{ "刀光颜色4",		"LightnColor4"	},
	{ "刀光颜色5",		"LightnColor5"	},
	{ "刀光颜色6",		"LightnColor6"	},
	{ "刀光强度1",		"LightnDens1"	},
	{ "刀光强度2",		"LightnDens2"	},
	{ "刀光强度3",		"LightnDens3"	},
	{ "刀光强度4",		"LightnDens4"	},
	{ "刀光强度5",		"LightnDens5"	},
	{ "刀光强度6",		"LightnDens6"	},
};

INT_PTR CALLBACK SelectPropDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static SelectProp *sp = NULL;

	switch(message) 
	{
	case WM_INITDIALOG:
		{
			sp = (SelectProp*)lParam;

			HWND hWndList = ::GetDlgItem (hWnd, IDC_DEFPROPLIST);
			for (unsigned i = 0; i < sizeof(propInfos)/sizeof(propInfos[0]); ++i)
			{
				ListBox_AddString (hWndList, propInfos[i].desc);
				ListBox_SetCurSel (hWndList, 0);
			}
			return TRUE;
		}
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDOK:
				{
					int sel = ListBox_GetCurSel (::GetDlgItem (hWnd, IDC_DEFPROPLIST));
					if (sel == LB_ERR)
					{
						::MessageBox (hWnd, "未选择属性!", "选择属性", MB_OK|MB_ICONHAND);
						break;
					}
					sp->setSelection (propInfos[sel].key);
					::EndDialog (hWnd, IDOK);
					break;
				}
			case IDCANCEL:
				{
					::EndDialog (hWnd, IDCANCEL);
					break;
				}
			default:
				{
					break;
				}
			}
		}
	default:
		{
			break;
		}
	}

	return FALSE;

}

const char *SelectProp::select (HWND parentWnd)
{
	if (IDOK == ::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_SELECTPROP), parentWnd, SelectPropDlgProc, (LPARAM)this))
	{
		return _selectedProp;
	}

	return 0;
}

