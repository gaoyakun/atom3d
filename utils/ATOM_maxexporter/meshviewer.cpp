#include "meshviewer.h"
#include "resource.h"

extern "C" HINSTANCE hModelHandle;

INT_PTR CALLBACK MeshEditDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static MeshViewer *viewer = NULL;
	static MeshViewer *copy = NULL;
	static int itemIndex = -1;
	static bool modified = false;

	switch(message) 
	{
	case WM_INITDIALOG:
		{
			viewer = (MeshViewer*)lParam;
			copy = new MeshViewer(*viewer);
			modified = false;

			HWND hWndList = ::GetDlgItem (hWnd, IDC_MESHLIST);
			for (unsigned i = 0; i < copy->getNumMeshes (); ++i)
			{
				ListBox_AddString (hWndList, copy->getMeshName (i));
			}
			return TRUE;
		}

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_MESHPROPS:
				{
					int sel = ListBox_GetCurSel (::GetDlgItem (hWnd, IDC_MESHLIST));
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
			int r = ::MessageBox (hWnd, "要保存修改吗?", "编辑网格", MB_YESNOCANCEL);
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

void MeshViewer::addMesh (const char *name, PropertyEditor *prop)
{
	bool found = false;
	for (unsigned j = 0; j < _meshNames.size(); ++j)
	{
		if (_meshNames[j] == name)
		{
			found = true;
			break;;
		}
	}
	if (!found)
	{
		_meshNames.push_back (name);
		_meshProps.push_back (prop);
	}
}

unsigned MeshViewer::getNumMeshes (void) const
{
	return _meshProps.size();
}

const char *MeshViewer::getMeshName (unsigned index) const
{
	return _meshNames[index].c_str();
}

PropertyEditor *MeshViewer::getProperty (unsigned index)
{
	return _meshProps[index];
}

bool MeshViewer::edit (HWND parentWnd)
{
	return IDOK == ::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_MESHEDIT), parentWnd, MeshEditDlgProc, (LPARAM)this);
}

void MeshViewer::clear (void)
{
	_meshProps.resize (0);
	_meshNames.resize (0);
}
