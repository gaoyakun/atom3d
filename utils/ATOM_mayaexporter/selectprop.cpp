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
	{ "�����",			"ATOM_Billboard"		},
	{ "��ƽ��UV����",	"UVNoIpol"		},
	{ "�ں�ģʽ",		"BlendMode"		},
	{ "���ʷ��",		"MaterialStyle"	},
	{ "����ģʽ",		"LightingMode"	},
	{ "������ɫ1",		"LightnColor1"	},
	{ "������ɫ2",		"LightnColor2"	},
	{ "������ɫ3",		"LightnColor3"	},
	{ "������ɫ4",		"LightnColor4"	},
	{ "������ɫ5",		"LightnColor5"	},
	{ "������ɫ6",		"LightnColor6"	},
	{ "����ǿ��1",		"LightnDens1"	},
	{ "����ǿ��2",		"LightnDens2"	},
	{ "����ǿ��3",		"LightnDens3"	},
	{ "����ǿ��4",		"LightnDens4"	},
	{ "����ǿ��5",		"LightnDens5"	},
	{ "����ǿ��6",		"LightnDens6"	},
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
						::MessageBox (hWnd, "δѡ������!", "ѡ������", MB_OK|MB_ICONHAND);
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

