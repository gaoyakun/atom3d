#include <windows.h>
#include <windowsx.h>
#include <algorithm>
#include "skeletonviewer.h"
#include "skeleton.h"
#include "resource.h"
#include "queryname.h"
#include "nm_translator.h"

#define TREEVIEW_STATE_CHANGE (WM_APP + 100)

extern HINSTANCE hModelHandle;

SkeletonViewer::SkeletonViewer (void)
{
	_skeletonInfo.skeleton = 0;
}

SkeletonViewer::~SkeletonViewer (void)
{
}

void SkeletonViewer::clear (void)
{
	_skeletonInfo.skeleton = 0;
	_skeletonInfo.treeItems.resize (0);
}

void setUsed_R (ATOM_Skeleton *skeleton, std::vector<int> &used, int joint)
{
	used[joint] = 1;

	if (skeleton->getJoints()[joint].parentIndex >= 0)
	{
		setUsed_R (skeleton, used, skeleton->getJoints()[joint].parentIndex);
	}
}

void setUsed (ATOM_Skeleton *skeleton, std::vector<int> &used, int joint)
{
	setUsed_R (skeleton, used, joint);

	for (unsigned i = 0; i < skeleton->getJoints().size(); ++i)
	{
		if (skeleton->getJoints()[i].parentIndex < 0 && used[i] == 1)
		{
			for (unsigned j = 0; j < skeleton->getJoints().size(); ++j)
			{
				if (skeleton->getJoints()[j].parentIndex == i)
				{
					used[j] = 1;
				}
			}
		}
	}
}

bool SkeletonViewer::setSkeleton (ATOM_Skeleton *skeleton, const std::list<CMeshData*> &meshList, bool selectBoneByName)
{
	if (skeleton->getJoints().size() == 0)
	{
		return false;
	}

	_skeletonInfo.skeleton = skeleton;
	_skeletonInfo.treeItems.resize (skeleton->getJoints().size());

	std::vector<int> used(skeleton->getJoints().size());
	memset (&used[0], 0, sizeof(int) * used.size());

	if (!selectBoneByName)
	{
		for (std::list<CMeshData*>::const_iterator it = meshList.begin(); it != meshList.end(); ++it)
		{
			CMeshData *meshData = *it;

			for (unsigned n = 0; n < meshData->getWeightJoints().size(); ++n)
			{
				const MIntArray &weightJoints = meshData->getWeightJoints()[n];

				for (unsigned k = 0; k < weightJoints.length(); ++k)
				{
					int joint = weightJoints[k];
					if (joint >= 0 && joint < skeleton->getJoints().size())
					{
						setUsed (skeleton, used, joint);
					}
				}
			}
		}
	}
	else
	{
		for (unsigned i = 0; i < used.size(); ++i)
		{
			const char *jointName = skeleton->getJoints()[i].name.asChar ();
			if (!strncmp (jointName, "pu_", 3) || !strncmp (jointName, "pd_", 3))
			{
				used[i] = 1;
			}
		}
	}

	_skeletonInfo.skeleton->getAttachPoints().resize (0);
	for (unsigned i = 0; i < used.size(); ++i)
	{
		if (used[i])
		{
			_skeletonInfo.skeleton->getAttachPoints().push_back (i);
		}
	}

	skeleton->remapJoint ();

	return true;
}

const SkeletonInfo &SkeletonViewer::getSkeleton (void) const
{
	return _skeletonInfo;
}

SkeletonInfo &SkeletonViewer::getSkeleton (void)
{
	return _skeletonInfo;
}

void SkeletonViewer::initTree (HWND treeCtrl)
{
	TreeView_DeleteAllItems (treeCtrl);

	ATOM_Skeleton *skeleton = _skeletonInfo.skeleton;
	std::vector<HTREEITEM> &_jointItems = _skeletonInfo.treeItems;

	for (unsigned i = 0; i < skeleton->getJoints().size(); ++i)
	{
		const joint &theJoint = skeleton->getJoints()[i];

		TVINSERTSTRUCT tvis;
		memset(&tvis, 0, sizeof(tvis));
		tvis.item.mask = TVIF_TEXT;
		tvis.hParent = theJoint.parentIndex < 0 ? TVI_ROOT : _jointItems[theJoint.parentIndex];
		tvis.hInsertAfter = TVI_LAST;
		tvis.item.pszText = (LPSTR)theJoint.name.asChar();
		_jointItems[i] = TreeView_InsertItem (treeCtrl, (LPARAM)&tvis);
	}

	for (unsigned i = 0; i < skeleton->getJoints().size(); ++i)
	{
		if (std::find (_skeletonInfo.skeleton->getAttachPoints().begin(), _skeletonInfo.skeleton->getAttachPoints().end(), i) != _skeletonInfo.skeleton->getAttachPoints().end())
		{
			TreeView_SetCheckState (treeCtrl, _jointItems[i], TRUE);
		}
		else
		{
			TreeView_SetCheckState (treeCtrl, _jointItems[i], FALSE);
		}
	}

	TreeView_Expand (treeCtrl, _jointItems[0], TVE_EXPAND);
}

void SkeletonViewer::syncUI (HWND hWnd)
{
	initTree (::GetDlgItem (hWnd, IDC_SKELETON_TREE));
}

INT_PTR CALLBACK SkeletonViewerDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static SkeletonViewer *viewer = NULL;

	switch(message) 
	{
		case WM_INITDIALOG:
			{
				HWND treeCtrl = ::GetDlgItem (hWnd, IDC_SKELETON_TREE);
				DWORD style = ::GetWindowLongA (treeCtrl, GWL_STYLE);
				style ^= TVS_CHECKBOXES;
				SetWindowLongA (treeCtrl, GWL_STYLE, style);
				style |= TVS_CHECKBOXES;
				SetWindowLongA (treeCtrl,GWL_STYLE, style);

				viewer = (SkeletonViewer*)lParam;
				viewer->syncUI (hWnd);
				return TRUE;
			}

		case WM_NOTIFY:
			{
				LPNMHDR pnmh = (LPNMHDR)lParam;
				if (pnmh->hwndFrom == ::GetDlgItem(hWnd, IDC_SKELETON_TREE))
				{
					switch (pnmh->code)
					{
					case NM_CLICK:
						{
							DWORD pos = ::GetMessagePos ();
							POINT point;
							point.x = LOWORD(pos);
							point.y = HIWORD(pos);
							::ScreenToClient (::GetDlgItem(hWnd, IDC_SKELETON_TREE), &point);
							TVHITTESTINFO hti;
							hti.pt = point;
							HTREEITEM h = (HTREEITEM)::SendMessage (::GetDlgItem(hWnd, IDC_SKELETON_TREE), TVM_HITTEST, 0, (LPARAM)&hti);
							if (h && (hti.flags & TVHT_ONITEMSTATEICON) != 0)
							{
								::PostMessage (hWnd, TREEVIEW_STATE_CHANGE, (WPARAM)pnmh->hwndFrom, (LPARAM)h);
							}
							break;
						}
					case NM_RCLICK:
						{
							DWORD pos = ::GetMessagePos ();
							viewer->trackMenu (hWnd, LOWORD(pos), HIWORD(pos));
							break;
						}
					case TVN_KEYDOWN:
						{
							LPNMTVKEYDOWN tvKeyDown = (LPNMTVKEYDOWN)(pnmh);
							if (tvKeyDown->wVKey == VK_SPACE)
							{
								HTREEITEM item = TreeView_GetSelection (pnmh->hwndFrom);
								if (item != NULL)
								{
									::PostMessage (hWnd, TREEVIEW_STATE_CHANGE, (WPARAM)pnmh->hwndFrom, (LPARAM)item);
								}
							}
							break;
						}
					default:
						break;
					}
				}
				break;
			}
		case TREEVIEW_STATE_CHANGE:
			{
				HWND hwndTree = (HWND)wParam;
				HTREEITEM item = (HTREEITEM)lParam;
				BOOL checked = TreeView_GetCheckState (hwndTree, item);
				viewer->setCheckState (hwndTree, item, checked);
				break;
			}
		case WM_COMMAND:
			{
				switch (LOWORD(wParam))
				{
				case ID_MENU_SELECTALL:
					{
						HWND treeCtrl = ::GetDlgItem (hWnd, IDC_SKELETON_TREE);
						const SkeletonInfo &si = viewer->getSkeleton();
						for (unsigned s = 0; s < si.treeItems.size(); ++s)
						{
							TreeView_SetCheckState (treeCtrl, si.treeItems[s], TRUE);
							viewer->setCheckState (treeCtrl, si.treeItems[s], TRUE);
						}
						break;
					}
				case ID_MENU_DESELECTALL:
					{
						HWND treeCtrl = ::GetDlgItem (hWnd, IDC_SKELETON_TREE);
						const SkeletonInfo &si = viewer->getSkeleton();
						for (unsigned s = 0; s < si.treeItems.size(); ++s)
						{
							TreeView_SetCheckState (treeCtrl, si.treeItems[s], FALSE);
							viewer->setCheckState (treeCtrl, si.treeItems[s], FALSE);
						}
						break;
					}
				case IDOK:
					{
						SkeletonInfo &si = viewer->getSkeleton();
						si.skeleton->getAttachPoints().resize (0);
						for (unsigned i = 0; i < si.treeItems.size(); ++i)
						{
							BOOL checked = TreeView_GetCheckState (::GetDlgItem(hWnd, IDC_SKELETON_TREE), si.treeItems[i]);
							if (checked)
							{
								si.skeleton->getAttachPoints().push_back (i);
							}
						}
						si.skeleton->remapJoint ();

						viewer = 0;
						::EndDialog (hWnd, IDOK);
						break;
					}
				case IDCANCEL:
					{
						viewer = 0;
						::EndDialog (hWnd, IDCANCEL);
						break;
					}
				}
				break;
			}
		case WM_CLOSE:
			viewer = 0;
			::EndDialog (hWnd, IDCANCEL);
			return TRUE;
	}
	return 0;
}

void SkeletonViewer::setCheckState (HWND treeCtrl, HTREEITEM item, BOOL check)
{
	setCheckStateR_down (treeCtrl, item, check);

	if (check)
	{
		setCheckStateUp (treeCtrl, item, check);
	}
}

void SkeletonViewer::setCheckStateUp (HWND treeCtrl, HTREEITEM item, BOOL check)
{
	if (check)
	{
		HTREEITEM itemParent = TreeView_GetParent (treeCtrl, item);
		while (itemParent)
		{
			TreeView_SetCheckState (treeCtrl, itemParent, check);
			itemParent = TreeView_GetParent (treeCtrl, itemParent);
		}
	}
}

void SkeletonViewer::setCheckStateR_down (HWND treeCtrl, HTREEITEM item, BOOL check)
{
	HTREEITEM itemChild = TreeView_GetChild (treeCtrl, item);
	while (itemChild)
	{
		TreeView_SetCheckState (treeCtrl, itemChild, check);
		setCheckStateR_down (treeCtrl, itemChild, check);
		itemChild = TreeView_GetNextSibling (treeCtrl, itemChild); 
	}
}

bool SkeletonViewer::showDialog (HWND hWndParent)
{
	return IDOK == ::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_SKELETONVIEWER), GetActiveWindow(), SkeletonViewerDlgProc, (LPARAM)this);
}

void SkeletonViewer::trackMenu (HWND hWnd, int x, int y)
{
	HMENU treeMenu = ::LoadMenu (hModelHandle, MAKEINTRESOURCE(IDR_MENU_BONELIST));

	if (treeMenu)
	{
		::HMENU subMenu = ::GetSubMenu (treeMenu, 0);

		if (subMenu)
		{
			::TrackPopupMenu (subMenu, TPM_LEFTBUTTON, x, y, 0, hWnd, 0);
		}

		::DestroyMenu (treeMenu);
	}
}

