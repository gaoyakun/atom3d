#include <assert.h>

#include "Header.h"
#include "vertexanimation.h"
#include "resource.h"

static HWND btnFixedKeyFrames = NULL;
static HWND btnCustomKeyFrames = NULL;
static HWND editKeyFrames = NULL;
static HWND spin = NULL;
static HWND spinedit = NULL;
static HWND nameedit = NULL;
static HWND lbActionList = NULL;
static HWND btnAddAction = NULL;
static HWND btnRemoveAction = NULL;
static HWND btnModifyAction = NULL;
static VertexAnimationInfo *animationInfo = 0;
static std::string currentAction;

static char *removeSpace (char *s)
{
	char *h = s;
	while (*h == ' ' || *h == '\t' || *h == '\n')
	{
		++h;
	}

	unsigned l = strlen (h);
	if (l)
	{
		char *e = h + l - 1;
		while (*e == ' ' || *e == '\t' || *e == '\n')
		{
			*e = '\0';
			--e;
		}
	}
	return h;
}

static int findStringFromList (HWND listBox, const char *str)
{
	int searchRegion = -1;

	for (;;)
	{
		int index = ::SendMessage (listBox, LB_FINDSTRINGEXACT, (WPARAM)searchRegion, (LPARAM)str);
		if (index == LB_ERR)
		{
			return LB_ERR;
		}
		char buffer[256];
		::SendMessage (listBox, LB_GETTEXT, index, (LPARAM)buffer);
		if (!strcmp (buffer, str))
		{
			return index;
		}
		searchRegion = index;
	}
}

void applyAction (VertexAnimationAction *action)
{
	bool customKeyFrame = (BST_CHECKED == ::SendMessage (btnCustomKeyFrames, BM_GETCHECK, 0L, 0L));
	action->useCustomKeyFrames = customKeyFrame;

	char buffer[1024];
	::GetWindowText (spinedit, buffer, 1024);
	action->frameInterval = atoi (buffer);

	action->keyFrames.resize (0);
	::GetWindowText (editKeyFrames, buffer, 1024);
	char *p = buffer;
	for (;;)
	{
		char *n = strchr (p, ',');
		if (!n)
		{
			p = removeSpace (p);
			if (*p)
			{
				int frame = atoi (p);
				action->keyFrames.push_back (frame);
			}
			break;
		}
		else
		{
			*n = '\0';
			p = removeSpace (p);
			int frame = atoi (p);
			action->keyFrames.push_back (frame);
			p = n+1;
		}
	}

	::GetWindowText (nameedit, buffer, 1024);
	action->actionName = buffer;
}

void syncAction (const VertexAnimationAction *action)
{
	if (action)
	{
		char buffer[32];

		// keyframe list
		std::string sKeyFrames;
		for (unsigned i = 0; i < action->keyFrames.size(); ++i)
		{
			sprintf (buffer, "%d", action->keyFrames[i]);
			sKeyFrames += buffer;
			if (i != action->keyFrames.size()-1)
			{
				sKeyFrames += ",";
			}
		}
		::SetWindowText (editKeyFrames, sKeyFrames.c_str());

		// interval
		sprintf (buffer, "%d", action->frameInterval);
		::SetWindowText (spinedit, buffer);

		// name
		::SetWindowText (nameedit, action->actionName.c_str());

		// radio button 
		if (action->useCustomKeyFrames)
		{
			::SendMessage (btnCustomKeyFrames, BM_SETCHECK, BST_CHECKED, 0);
			::SendMessage (btnFixedKeyFrames, BM_SETCHECK, BST_UNCHECKED, 0);
			::EnableWindow (spin, FALSE);
			::EnableWindow (spinedit, FALSE);
		}
		else
		{
			::SendMessage (btnCustomKeyFrames, BM_SETCHECK, BST_UNCHECKED, 0);
			::SendMessage (btnFixedKeyFrames, BM_SETCHECK, BST_CHECKED, 0);
			::EnableWindow (spin, TRUE);
			::EnableWindow (spinedit, TRUE);
		}
	}
	else
	{
		VertexAnimationAction action;
		action.useCustomKeyFrames = false;
		action.frameInterval = 1;

		char namebuf[32];
		unsigned nameIndex = 0;
		for(;;)
		{
			sprintf (namebuf, "Action%u", ++nameIndex);
			if (animationInfo->actionSet.find (namebuf) == animationInfo->actionSet.end())
			{
				break;
			}
		}
		action.actionName = namebuf;

		syncAction (&action);
	}
}

bool selectAction (const char *action)
{
	if (action)
	{
		std::map<std::string, VertexAnimationAction>::const_iterator it = animationInfo->actionSet.find (action);
		if (it == animationInfo->actionSet.end ())
		{
			return false;
		}
		currentAction = action ? action : "";
		const VertexAnimationAction &action = it->second;
		syncAction (&action);
		::EnableWindow (btnRemoveAction, TRUE);
		::EnableWindow (btnModifyAction, TRUE);
	}
	else
	{
		syncAction (0);
		currentAction = "";
		::EnableWindow (btnRemoveAction, FALSE);
		::EnableWindow (btnModifyAction, FALSE);
	}

	return true;
}

static INT_PTR CALLBACK KeyFrameOptionsDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			animationInfo = (VertexAnimationInfo*)lParam;

			btnFixedKeyFrames = ::GetDlgItem (hWnd, IDC_FIXED_KEYFRAME);
			spin = ::GetDlgItem (hWnd, IDC_SPIN1);
			spinedit = ::GetDlgItem (hWnd, IDC_FRAMEINTERVAL);
			btnCustomKeyFrames = ::GetDlgItem (hWnd, IDC_CUSTOM_KEYFRAME);
			editKeyFrames = ::GetDlgItem (hWnd, IDC_KEYFRAMES);
			nameedit = ::GetDlgItem (hWnd, IDC_ACTIONNAME);
			lbActionList = ::GetDlgItem (hWnd, IDC_ACTIONLIST);
			btnAddAction = ::GetDlgItem (hWnd, IDC_ADDACTION);
			btnRemoveAction = ::GetDlgItem (hWnd, IDC_REMOVEACTION);
			btnModifyAction = ::GetDlgItem (hWnd, IDC_MODIFYACTION);

			::SendMessage (lbActionList, LB_RESETCONTENT, 0, 0);
			if (animationInfo->actionSet.size() > 0)
			{
				for (std::map<std::string, VertexAnimationAction>::const_iterator it = animationInfo->actionSet.begin();
					 it != animationInfo->actionSet.end ();
					 ++it)
				{
					::SendMessage (lbActionList, LB_ADDSTRING, 0, (LPARAM)it->second.actionName.c_str());
				}
				::SendMessage (lbActionList, LB_SETCURSEL, 0, 0);
				char buffer[256];
				::SendMessage (lbActionList, LB_GETTEXT, 0, (LPARAM)buffer);
				selectAction (buffer);
			}
			else
			{
				selectAction (0);
			}

			return TRUE;
		}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			{
				for (std::map<std::string, VertexAnimationAction>::iterator it = animationInfo->actionSet.begin();
					 it != animationInfo->actionSet.end();
					 ++it)
				{
					VertexAnimationAction &action = it->second;
					if (!action.useCustomKeyFrames)
					{
						int startFrame = action.keyFrames.front();
						int endFrame = action.keyFrames.back();
						while (startFrame < endFrame)
						{
							action.frameList.push_back (startFrame);
							startFrame += action.frameInterval;
						}
						action.frameList.push_back (endFrame);
					}
					else
					{
						action.frameList = action.keyFrames;
					}
				}

				EndDialog (hWnd, IDOK);
				return TRUE;
			}

		case IDCANCEL:
			{
				EndDialog (hWnd, IDCANCEL);
				return TRUE;
			}

		case IDC_FIXED_KEYFRAME:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				::EnableWindow (spin, TRUE);
				::EnableWindow (spinedit, TRUE);
			}
			break;

		case IDC_CUSTOM_KEYFRAME:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				::EnableWindow (spin, FALSE);
				::EnableWindow (spinedit, FALSE);
			}
			break;

		case IDC_ADDACTION:
			{
				VertexAnimationAction newAction;
				applyAction (&newAction);
				if (animationInfo->actionSet.find (newAction.actionName) != animationInfo->actionSet.end ())
				{
					::MessageBox (hWnd, "名字重复拉!", "atom导出器", MB_OK|MB_ICONHAND);
				}
				else if (validateAction (hWnd, newAction))
				{
					animationInfo->actionSet[newAction.actionName] = newAction;
					int index = ::SendMessage (lbActionList, LB_ADDSTRING, 0, (LPARAM)newAction.actionName.c_str());
					::SendMessage (lbActionList, LB_SETCURSEL, index, 0);
					selectAction (newAction.actionName.c_str());
				}
				break;
			}

		case IDC_REMOVEACTION:
			{
				if (currentAction != "")
				{
					std::map<std::string, VertexAnimationAction>::iterator it = animationInfo->actionSet.find (currentAction);
					assert(it != animationInfo->actionSet.end ());
					animationInfo->actionSet.erase (it);
					int index = findStringFromList (lbActionList, currentAction.c_str());
					//int index = ::SendMessage (lbActionList, LB_FINDSTRING, 0, (LPARAM)currentAction.c_str());
					assert(index != LB_ERR);
					::SendMessage (lbActionList, LB_DELETESTRING, index, 0);
					unsigned count = ::SendMessage (lbActionList, LB_GETCOUNT, 0, 0);
					if (count == 0)
					{
						selectAction (0);
					}
					else
					{
						if (index >= count)
						{
							index = count - 1;
						}
						::SendMessage (lbActionList, LB_SETCURSEL, index, 0);
						char buffer[256];
						::SendMessage (lbActionList, LB_GETTEXT, index, (LPARAM)buffer);
						selectAction (buffer);
					}
				}
				break;
			}
		case IDC_MODIFYACTION:
			{
				if (currentAction != "")
				{
					VertexAnimationAction newAction;
					applyAction (&newAction);
					if (newAction.actionName != currentAction)
					{
						if (animationInfo->actionSet.find (newAction.actionName) != animationInfo->actionSet.end())
						{
							::MessageBox (hWnd, "名字重复拉!", "atom导出器", MB_OK|MB_ICONHAND);
							break;
						}

						if (validateAction (hWnd, newAction))
						{
							animationInfo->actionSet[newAction.actionName] = newAction;
							int index = ::SendMessage (lbActionList, LB_ADDSTRING, 0, (LPARAM)newAction.actionName.c_str());
							::SendMessage (lbActionList, LB_SETCURSEL, index, 0);

							index = findStringFromList (lbActionList, currentAction.c_str());
							//index = ::SendMessage (lbActionList, LB_FINDSTRING, 0, (LPARAM)currentAction.c_str());
							assert(index != LB_ERR);
							::SendMessage (lbActionList, LB_DELETESTRING, index, 0);

							std::map<std::string, VertexAnimationAction>::iterator it = animationInfo->actionSet.find (currentAction);
							assert(it != animationInfo->actionSet.end ());
							animationInfo->actionSet.erase (it);

							selectAction (newAction.actionName.c_str());
						}
					}
					else if (validateAction (hWnd, newAction))
					{
						animationInfo->actionSet[currentAction] = newAction;
					}
				}
				break;
			}
		case IDC_ACTIONPROPS:
			{
				if (currentAction != "")
				{
					std::map<std::string, VertexAnimationAction>::iterator it = animationInfo->actionSet.find (currentAction);
					assert(it != animationInfo->actionSet.end ());
					it->second.properties.edit (hWnd);
				}
				break;
			}
		case IDC_ACTIONLIST:
			{
				if (HIWORD(wParam) == LBN_SELCHANGE)
				{
					int index = ::SendMessage (lbActionList, LB_GETCURSEL, 0, 0);
					if (index != LB_ERR)
					{
						char buffer[256];
						::SendMessage (lbActionList, LB_GETTEXT, index, (LPARAM)buffer);
						selectAction (buffer);
					}
					else
					{
						selectAction (0);
					}
				}
				break;
			}

		default:
			break;
		}
	}
	return FALSE;
}

void getAnimationRange (AnimationRangeInfo *info)
{
	Interval ivAnimRange = GetCOREInterface()->GetAnimRange();
	info->msInc = GetTicksPerFrame();
	info->msStart = ivAnimRange.Start();
	info->msEnd = ivAnimRange.End();

	info->frameStart = info->msStart/info->msInc;
	info->frameEnd = info->msEnd/info->msInc;
}

bool editVertexAnimation (HMODULE module, HWND parentWnd, VertexAnimationInfo *info)
{
	VertexAnimationInfo tmpInfo = *info;

	if (IDOK == ::DialogBoxParam(module, MAKEINTRESOURCE(IDD_KEYFRAME_OPTIONS), parentWnd, KeyFrameOptionsDlgProc, (LPARAM)&tmpInfo))
	{
		*info = tmpInfo;
		return true;
	}

	return false;
}

bool validateAction (HWND dlgWnd, const VertexAnimationAction &action)
{
	char name[64];

	if (action.actionName.length() > 63)
	{
		::MessageBox (dlgWnd, "名字太长!", "atom导出器", MB_OK|MB_ICONHAND);
		return false;
	}

	if (!*removeSpace (name))
	{
		::MessageBox (dlgWnd, "不是有效的名字!", "atom导出器", MB_OK|MB_ICONHAND);
		return false;
	}

	AnimationRangeInfo info;
	getAnimationRange (&info);

	if (action.keyFrames.size() == 1)
	{
		::MessageBox (dlgWnd, "关键帧输入不合法!", "atom导出器", MB_OK|MB_ICONHAND);
		return false;
	}
	else if (action.keyFrames.size() == 0)
	{
		::MessageBox (dlgWnd, "请输入关键帧!", "atom导出器", MB_OK|MB_ICONHAND);
		return false;
	}
	else
	{
		if (action.keyFrames[0] < info.frameStart)
		{
			::MessageBox (dlgWnd, "关键帧超出范围!", "atom导出器", MB_OK|MB_ICONHAND);
			return false;
		}

		for (unsigned i = 1; i < action.keyFrames.size(); ++i)
		{
			if (action.keyFrames[i] <= action.keyFrames[i-1])
			{
				::MessageBox (dlgWnd, "关键帧必须由小到大排列且不能重复!", "atom导出器", MB_OK|MB_ICONHAND);
				return false;
			}

			if (action.keyFrames[i] < info.frameStart || action.keyFrames[i] > info.frameEnd)
			{
				::MessageBox (dlgWnd, "关键帧超出范围!", "atom导出器", MB_OK|MB_ICONHAND);
				return false;
			}
		}

		if (!action.useCustomKeyFrames)
		{
			if (action.frameInterval == 0)
			{
				::MessageBox (dlgWnd, "关键帧步长不能为0!", "atom导出器", MB_OK|MB_ICONHAND);
				return false;
			}
			else
			{
				int start = action.keyFrames.front();
				int end = action.keyFrames.back ();
				if (start + action.frameInterval > end)
				{
					::MessageBox (dlgWnd, "关键帧步长过大!", "atom导出器", MB_OK|MB_ICONHAND);
					return false;
				}
			}
		}
	}

	return true;
}

