#include <map>

#include "StdAfx.h"
#include "assertion.h"
#include "callstack.h"
#include "callstackdlg_template.h"
#include "address_template.h"
#include "mem.h"
#include "resource.h"
#include "send_error.h"

#pragma comment(lib, "comctl32.lib")

#if ATOM3D_MINIMAL_BUILD
extern"C" ATOM_DBGHLP_API int ATOM_ShowCallstackDialog (void *parentWnd, const char *caption, const char *filename, int lineno, const ATOM_CallStack *callstack, const char *desc, int skip, unsigned mask = ATOM_ASSERT_ALL_MASK, const char *files = 0);
extern"C" ATOM_DBGHLP_API void ATOM_SendErrorFiles (const char *errorFiles);
extern"C" ATOM_DBGHLP_API void *ATOM_SetDialogParentWindow (void *hWnd);
extern"C" ATOM_DBGHLP_API ATOM_AssertionReporter *ATOM_SetAssertionReporter (ATOM_AssertionReporter *reporter);
#else

HWND _ParentWindow = 0;

ATOM_AssertionReporter *_DefaultReporter = 0;

static char block_dump_buffer[16384];

static std::string UL2Str (ATOM_ULongLong num)
{
  char buffer[32];
  sprintf (buffer, "0x%I64X", num);
  return buffer;
}

static std::string L2Str (unsigned long num)
{
  char buffer[32];
  sprintf (buffer, "%d", num);
  return buffer;
}

struct DlgParam
{
  const ATOM_CallStack *callstack;
  const char *filename;
  int lineno;
  const char *errmsg;
  const char *caption;
  int skip;
  unsigned mask;
  const char *files;
};

static std::string ChooseSaveFileName (HWND parentHwnd, const char *defaultFileName, const char *defaultExt, const char *filter)
{
	OPENFILENAMEA ofn;
	char szFile[MAX_PATH];
	strcpy (szFile, defaultFileName);

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = parentHwnd;
	ofn.lpstrFile = szFile;

	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrDefExt = defaultExt;
	ofn.Flags = OFN_SHOWHELP|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST|OFN_EXPLORER;

	//　显示打开选择文件对话框。
	if (::GetSaveFileNameA(&ofn))
	{
		return szFile;
	}

	return "";
}

struct AddressInfo
{
	void *pointer;
	bool userblock;
};

static INT_PTR CALLBACK AddressDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			return TRUE;
		}

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDOK:
					{
						HWND hWndEdit = ::GetDlgItem (hDlg, IDC_ADDRESS);
						char buffer[256];
						::GetWindowTextA (hWndEdit, buffer, 256);
						void *pointer = 0;
						int n = ::sscanf (buffer, "%p", &pointer);
						if (n != 1 || pointer == 0)
						{
							::MessageBox (hDlg, "格式错误!", "输入地址", MB_OK|MB_ICONHAND);
						}
						else
						{
							pointer = pointer;
							bool userblock = (BST_CHECKED == ::SendMessage (::GetDlgItem(hDlg, IDC_USERBLOCK), BM_GETCHECK, 0, 0));
							if (!ATOM_DumpMemoryBlockToMemory (block_dump_buffer, pointer, userblock))
							{
								strcpy (block_dump_buffer, "获取内存分配信息失败!");
							}
							::SetWindowTextA (::GetDlgItem(hDlg, IDC_ALLOCDETAIL), block_dump_buffer);
						}
						break;
					}
				case IDCANCEL:
					{
						::EndDialog (hDlg, IDCANCEL);
						break;
					}
				default:
					{
						break;
					}
			}
			return TRUE;
		}

	case WM_CLOSE:
		{
			EndDialog (hDlg, IDCANCEL);
			return TRUE;
		}

	default:
		{
			return FALSE;
		}
	}
}

static void writeDumpFiles (void)
{
};

ATOM_DBGHLP_API void ATOM_SendErrorFiles (const char *errorFiles)
{
	char dirName[MAX_PATH];
	if (errorFiles)
	{
		SYSTEMTIME stLocalTime;
		::GetLocalTime( &stLocalTime );

		sprintf (dirName, "%04d%02d%02d-%02d%02d%02d"
		, stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay
		, stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);

		while (errorFiles[0])
		{
			bool text = errorFiles[0] == 't';
			ATOM_ErrorSender::sendFile (errorFiles+1, dirName, text);
			errorFiles = errorFiles + strlen(errorFiles) + 1;
		}
	}
}

static INT_PTR CALLBACK DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lparam)
{
  switch (message)
  {
  case WM_INITDIALOG:
  {
      InitCommonControls();

      DlgParam *param = (DlgParam*)lparam;

#if 0
	  if (param->files)
	  {
		  ATOM_SendErrorFiles (param->files);
	  }
#endif

      SetWindowText (hDlg, param->caption);

      std::string sourcefile = "源文件: ";
      sourcefile += param->filename ? param->filename : "(不可用)";
      SetDlgItemText (hDlg, IDC_FILENAME, sourcefile.c_str());

      std::string lineno = "行号: ";
      lineno += param->filename ? L2Str(param->lineno) : "(不可用)";
      SetDlgItemText (hDlg, IDC_LINENO, lineno.c_str());

      SetDlgItemText (hDlg, IDC_ERRMSG, (param->errmsg && param->errmsg[0]) ? param->errmsg : "(no message)");

      TV_INSERTSTRUCT tvinsert;
      tvinsert.hParent = NULL;
      tvinsert.hInsertAfter = TVI_ROOT;
      tvinsert.item.mask = TVIF_TEXT;
      tvinsert.item.pszText = "调用堆栈";
      tvinsert.item.iImage = 0;
      tvinsert.item.iSelectedImage = 0;
      HTREEITEM root = (HTREEITEM)SendDlgItemMessage(hDlg, IDC_STACKTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);

      tvinsert.hInsertAfter = TVI_LAST;

      char temp[32];

      for (size_t i = param->skip; i < param->callstack->getNumEntries(); ++i)
      {
        const ATOM_CallStack::StackEntry &entry = param->callstack->getEntry (i);
        std::string s = entry.module_name[0] ? entry.module_name : "(unknown module)";
        sprintf (temp, "(0x%I64X)", entry.base_of_image);
        s += temp;
        s += '!';
        s += entry.und_fullname[0] ? entry.und_fullname : "(unknown function)";
        s += "(";
        s += UL2Str((unsigned long)entry.offset);
        s += ")";

        tvinsert.hParent = root;
        tvinsert.item.pszText = const_cast<char*>(s.c_str());
        HTREEITEM p = (HTREEITEM)SendDlgItemMessage(hDlg, IDC_STACKTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);

        tvinsert.hParent = p;
        tvinsert.item.pszText = const_cast<char*>(entry.line_filename[0] ? entry.line_filename : "no filename");
        SendDlgItemMessage(hDlg, IDC_STACKTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);

        s = L2Str(entry.line_number);
        tvinsert.item.pszText = (char*)s.c_str();
        SendDlgItemMessage(hDlg, IDC_STACKTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
      }

      SendDlgItemMessage (hDlg, IDC_STACKTREE, TVM_EXPAND, (WPARAM)TVE_EXPAND, (LPARAM)root);

      std::string callstackDesc;
      callstackDesc += sourcefile;
      callstackDesc += "\r\n";
      callstackDesc += lineno;
      callstackDesc += "\r\n";
      if (param->errmsg)
      {
        callstackDesc += "\r\n";
        callstackDesc += param->errmsg;
        callstackDesc += "\r\n";
      }
      callstackDesc += "\r\n";
      callstackDesc += "ATOM_CallStack:\r\n";
      callstackDesc += param->callstack->getDescription (param->skip);

      SetDlgItemText (hDlg, IDC_HIDENEDIT, callstackDesc.c_str());

	  if ((param->mask & ATOM_ASSERT_CONTINUE_MASK) == 0)
	  {
		  ::ShowWindow (::GetDlgItem (hDlg, IDC_CONTINUE), SW_HIDE);
	  }
	  if ((param->mask & ATOM_ASSERT_BREAK_MASK) == 0)
	  {
		  ::ShowWindow (::GetDlgItem (hDlg, IDC_BREAK), SW_HIDE);
	  }
	  if ((param->mask & ATOM_ASSERT_TERMINATE_MASK) == 0)
	  {
		  ::ShowWindow (::GetDlgItem (hDlg, IDC_ABORT), SW_HIDE);
	  }

      return TRUE;
    }

  case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
      case IDC_CONTINUE:
        EndDialog (hDlg, ATOM_ASSERT_CONTINUE);
        return TRUE;

      case IDC_BREAK:
        EndDialog (hDlg, ATOM_ASSERT_BREAK);
        return TRUE;

      case IDC_ABORT:
        EndDialog (hDlg, ATOM_ASSERT_TERMINATE);
        return TRUE;

      case IDC_COPYTOCLIPBOARD:
        if (OpenClipboard (NULL))
        {
          EmptyClipboard ();
          HGLOBAL hTextBuffer = GlobalAlloc(GMEM_DDESHARE, 2048);
          char *p = (char*)GlobalLock(hTextBuffer);
          GetDlgItemText (hDlg, IDC_HIDENEDIT, p, 2048);
          GlobalUnlock (hTextBuffer);
          SetClipboardData (CF_TEXT, hTextBuffer);
          CloseClipboard ();
          GlobalFree (hTextBuffer);
        }
        break;

	  case IDC_MEMLOG:
		  {
			  std::string filename = ChooseSaveFileName (hDlg, "memlog.txt", "txt", "All(*.*)\0*.*\0Text(*.txt)\0*.txt\0\0");
			  if (!filename.empty ())
			  {
				  ATOM_DumpMemoryState (filename.c_str(), 1);
			  }
			  break;
		  }

	  case IDC_MEMBLOCK:
		  {
		      DialogBoxIndirectA (NULL, (LPCDLGTEMPLATE)dlg_102, hDlg, &AddressDlgProc);
			  break;
		  }

      default:
        break;
      }
      return TRUE;
    }

  case WM_CLOSE:
	//if ((param->mask & ATOM_ASSERT_TERMINATE_MASK) == 0)
	//{
	//	EndDialog (hDlg, ATOM_ASSERT_TERMINATE);
	//}
	//if ((param->mask & ATOM_ASSERT_CONTINUE_MASK) == 0)
	//{
	//	EndDialog (hDlg, ATOM_ASSERT_CONTINUE);
	//}
	//if ((param->mask & ATOM_ASSERT_BREAK_MASK) == 0)
	//{
	//	EndDialog (hDlg, ATOM_ASSERT_BREAK);
	//}
    return TRUE;

  default:
    return FALSE;
  }
}

ATOM_AssertionReporter::ATOM_AssertionReporter (void) 
{
}

ATOM_AssertionReporter::~ATOM_AssertionReporter (void) 
{
}

int ATOM_AssertionReporter::report (int /* counter */, const char *msg, const char *filename, int lineno, unsigned mask, const char *title, const char *files) 
{
  ATOM_CallStack callstack;
  return ATOM_ShowCallstackDialog (_ParentWindow, title ? title : "断言失败!!!", filename, lineno, &callstack, msg, 4, mask, files);
}

ATOM_DBGHLP_API ATOM_AssertionReporter *ATOM_SetAssertionReporter (ATOM_AssertionReporter *reporter)
{
  ATOM_AssertionReporter *ret = _DefaultReporter;
  _DefaultReporter = reporter;
  return ret;
}

ATOM_DBGHLP_API void * ATOM_SetDialogParentWindow (void *hWnd)
{
  HWND ret = _ParentWindow;
  _ParentWindow = (HWND)hWnd;
  return ret;
}

ATOM_DBGHLP_API int ATOM_ShowCallstackDialog (void *parentWnd, const char *caption, const char *filename, int lineno, const ATOM_CallStack *callstack, const char *desc, int skip, unsigned mask, const char *files)
{
  DlgParam param;
  param.callstack = callstack;
  param.errmsg = desc;
  param.filename = filename;
  param.lineno = lineno;
  param.caption = caption;
  param.skip = skip;
  param.mask = mask;
  param.files = files;
  return DialogBoxIndirectParam (NULL, (LPCDLGTEMPLATE)dlg_101, parentWnd ? (HWND)parentWnd : _ParentWindow, &DlgProc, (LPARAM)&param);
}

struct AssertionCounter
{
  const char *filename;
  int counter;
};

ATOM_Assertion::ATOM_Assertion (const char *filename, int lineno, bool expr, ATOM_AssertionReporter *reporter, unsigned mask, const char *title, const char *files)
{
	_filename = filename;
	_lineno = lineno;
	_expr = expr;
	_reporter = reporter;
	_mask = mask;
	_title = title;
	_files = files;
}

void ATOM_Assertion::operator () ()
{
	if (!_expr)  doAssertion ("");
}

void ATOM_Assertion::operator () (const char *format, ...)
{
	static const size_t MSG_BUFFER_SIZE = 8192;
	static char msgbuffer[MSG_BUFFER_SIZE];

	if (!_expr)
	{
		va_list args;
		va_start (args, format);
		_vsnprintf (msgbuffer, MSG_BUFFER_SIZE, format, args);
		va_end (args);
		msgbuffer[MSG_BUFFER_SIZE-1] = '\0';
		doAssertion (msgbuffer);
	}
}

void ATOM_Assertion::doAssertion (const char *msg)
{
	typedef std::multimap<int, AssertionCounter> AssertionMap;
	typedef AssertionMap::iterator AssertionMapIter;
	static AssertionMap Assertions;
	static ATOM_AssertionReporter DefaultReporter;

	AssertionMapIter it = Assertions.find(_lineno);
	int count = 0;

	std::pair<AssertionMapIter, AssertionMapIter> iters = Assertions.equal_range(_lineno);
	for (it = iters.first; it != iters.second; ++it)
	{
		if (!strcmp(it->second.filename, _filename))
		{
			count = ++it->second.counter;
			break;
		}
	}

	if (count == 0)
	{
		count = 1;
		AssertionCounter counter = { _filename, count };
		Assertions.insert (std::make_pair(_lineno, counter));
	}

	if (!_reporter)
	{
	  _reporter = _DefaultReporter ? _DefaultReporter : &DefaultReporter;
	}

	switch (_reporter->report (count, msg, _filename, _lineno, _mask, _title, _files))
	{
	case ATOM_ASSERT_CONTINUE: 
		{
			break;
		}
	case ATOM_ASSERT_BREAK: 
		{
			::DebugBreak ();
			break;
		}
	case ATOM_ASSERT_TERMINATE:
	default: 
		{
			::TerminateProcess (GetCurrentProcess(), 1);
		}
	}
}

#endif
