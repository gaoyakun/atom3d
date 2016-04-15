#include "materialviewer.h"
#include "resource.h"

extern "C" HINSTANCE hModelHandle;

INT_PTR CALLBACK MaterialEditDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static MaterialViewer *viewer = NULL;
	static MaterialViewer *copy = NULL;
	static int itemIndex = -1;
	static bool modified = false;

	switch(message) 
	{
	case WM_INITDIALOG:
		{
			viewer = (MaterialViewer*)lParam;
			copy = new MaterialViewer(*viewer);
			modified = false;

			HWND hWndList = ::GetDlgItem (hWnd, IDC_MATERIALLIST);
			for (unsigned i = 0; i < copy->getNumMaterials (); ++i)
			{
				ListBox_AddString (hWndList, copy->getMaterialName (i));
			}
			return TRUE;
		}

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_MATERIALPROP:
				{
					int sel = ListBox_GetCurSel (::GetDlgItem (hWnd, IDC_MATERIALLIST));
					if (sel != LB_ERR)
					{
						copy->getProperty (sel)->edit (hWnd);
						modified = true;
					}
					break;
				}
			case IDOK:
				{
					*viewer = *copy;
					delete copy;
					copy = 0;
					::EndDialog (hWnd, IDOK);
					break;
				}
			case IDCANCEL:
				{
					delete copy;
					copy = 0;
					::EndDialog (hWnd, IDCANCEL);
					break;
				}
			default:
				{
					break;
				}
			}
			break;
		}
	case WM_CLOSE:
		if (modified)
		{
			int r = ::MessageBox (hWnd, "要保存修改吗?", "编辑材质", MB_YESNOCANCEL);
			if (r == IDCANCEL)
			{
				return TRUE;
			}
			if (r == IDYES)
			{
				*viewer = *copy;
				delete copy;
				copy = 0;
				::EndDialog (hWnd, IDOK);
			}
			else
			{
				delete copy;
				copy = 0;
				::EndDialog (hWnd, IDCANCEL);
			}
		}
		else
		{
			delete copy;
			copy = 0;
			::EndDialog (hWnd, IDCANCEL);
		}
		break;
	default:
		break;
	}
	return FALSE;
}

void MaterialViewer::addMaterials (const char *name)
{
	bool found = false;
	for (unsigned j = 0; j < _materialNames.size(); ++j)
	{
		if (_materialNames[j] == name)
		{
			found = true;
			break;;
		}
	}
	if (!found)
	{
		_materialNames.push_back (name);
		_shaderProps.resize (_shaderProps.size() + 1);
	}
}

unsigned MaterialViewer::getNumMaterials (void) const
{
	return _shaderProps.size();
}

const char *MaterialViewer::getMaterialName (unsigned index) const
{
	return _materialNames[index].c_str();
}

PropertyEditor *MaterialViewer::getProperty (unsigned index)
{
	return &_shaderProps[index];
}

const PropertyEditor *MaterialViewer::getProperty (unsigned index) const
{
	return &_shaderProps[index];
}

PropertyEditor *MaterialViewer::getPropertyByName (const char *name)
{
	for (unsigned i = 0; i < _shaderProps.size(); ++i)
	{
		if (_materialNames[i] == name)
		{
			return &_shaderProps[i];
		}
	}
	return 0;
}

const PropertyEditor *MaterialViewer::getPropertyByName (const char *name) const
{
	for (unsigned i = 0; i < _shaderProps.size(); ++i)
	{
		if (_materialNames[i] == name)
		{
			return &_shaderProps[i];
		}
	}
	return 0;
}

bool MaterialViewer::edit (HWND parentWnd)
{
	return IDOK == ::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_MATERIALEDIT), parentWnd, MaterialEditDlgProc, (LPARAM)this);
}

void MaterialViewer::clear (void)
{
	_shaderProps.resize (0);
	_materialNames.resize (0);
}
