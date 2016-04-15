#include "stdafx.h"
#include "keymapping.h"

#ifndef PBT_APMQUERYSUSPEND
#define PBT_APMQUERYSUSPEND             0x0000
#endif

#ifndef PBT_APMBATTERYLOW
#define PBT_APMBATTERYLOW               0x0009
#endif

#define REPEATED_KEYMASK	(1<<30)
#define EXTENDED_KEYMASK	(1<<24)

static ATOM_Key translateVK (WPARAM vk, UINT scancode)
{
	if (vk == VK_RETURN && (scancode & 0x100))
	{
		return KEY_KP_ENTER;
	}
	else
	{
		return TranslateKey (vk);
	}
}

ATOM_RenderWindow::ATOM_RenderWindow() {
  memset(&_M_window_hints, 0, sizeof(_M_window_hints));
  _M_device = 0;
  _M_window_info = 0;
  _M_saved_position_x = 0;
  _M_saved_position_y = 0;
  _M_active = false;
  _M_saved_char[0] = 0;
  _M_saved_char[1] = 0;
  _M_saved_char[2] = 0;
  memset (_M_key_state, 0, sizeof(_M_key_state));
  _M_view = 0;
  _M_insizemove = false;
  _M_key_mod = 0;
  _M_saved_winpos[0] = _M_saved_winpos[1] = 0;
}

ATOM_RenderWindow::~ATOM_RenderWindow (void)
{
	HWND hWnd = (HWND)getWindowId();
	if (::IsWindow(hWnd))
	{
		::DestroyWindow(hWnd);
	}
	ATOM_DELETE(_M_window_info);
}

void ATOM_RenderWindow::setCapture()
{
  ::SetCapture((HWND)getWindowId());
}

void ATOM_RenderWindow::releaseCapture()
{
  ::ReleaseCapture();
}

LRESULT ATOM_RenderWindow::processWindowMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static UINT CP_USER_DEFINE_VALUE	= 936;
  //ATOM_LOGGER::information ("[ATOM_RenderWindow::processWindowMessage] %x, %x, %x, %x \n", hWnd, uMsg, wParam, lParam);
  if (ATOM_APP)
  {
	  if (uMsg == WM_CHAR)
	  {
		  int volatile a = uMsg;
	  }

	ATOM_AppWMEvent e(hWnd, uMsg, wParam, lParam);
	ATOM_APP->handleEvent (&e);
	if (e.eat)
	{
		return e.returnValue;
	}
  }

  if (!_M_window_info)
  {
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
  }

  switch ( uMsg)
  {
  case WM_CHAR:
    {
      ATOM_CharEvent e;
      e.win = this;
	  e.codec = ATOM_CC_CP936;
	  e.unicc = wParam;
	  WideCharToMultiByte (CP_USER_DEFINE_VALUE, 0, (LPCWSTR)&e.unicc, 1, (LPSTR)&e.mbcc, 2, 0, 0);
	  ATOM_APP->handleEvent (&e);
	}
/*
	  WideCharToMultiByte (CP_ACP, 0, &e.unicc, 1
      if (_M_saved_char[0])
      {
        _M_saved_char[1] = (wParam & 0xFF);
        e.mbcc = (_M_saved_char[0] & 0xFF) + ((_M_saved_char[1] & 0xFF) << 8);
        MultiByteToWideChar (CP_ACP, 0, _M_saved_char, -1, (LPWSTR)&e.unicc, 1);

		if (ATOM_CC_IsLeadByte (ATOM_CC_CP936, _M_saved_char[0]))
		{
			e.codec = ATOM_CC_CP936;
		}
		else if (ATOM_CC_IsLeadByte (ATOM_CC_CP950, _M_saved_char[0]))
		{
			e.codec = ATOM_CC_CP950;
		}
		else
		{
			e.codec = ATOM_CC_UNKNOWN;
		}

        _M_saved_char[0] = 0;
        _M_saved_char[1] = 0;
        _M_saved_char[2] = 0;

		ATOM_APP->handleEvent (&e);
      }
      else if (IsDBCSLeadByteEx (CP_ACP, wParam & 0xFF))
      {
        _M_saved_char[0] = (wParam & 0xFF);
      }
      else
      {
        e.mbcc = (wParam & 0xFF);
        e.unicc = (wParam & 0xFF);

		ATOM_APP->handleEvent (&e);
      }
    }
*/
    break;
  
 case WM_SYSCHAR:	  
	    return 0;


  case WM_IME_COMPOSITION:
    {
#if 0
      HIMC hIMC;
      if (lParam & GCS_RESULTSTR)
      {
        hIMC = ::ImmGetContext (hWnd);
        DWORD dwSize = ::ImmGetCompositionString (hIMC, GCS_RESULTSTR, 0, 0);
        if (dwSize == 1)
        {
          char ch;
          ::ImmGetCompositionString (hIMC, GCS_RESULTSTR, &ch, dwSize);
          short vk = ::VkKeyScan (ch);
          signed char c = vk & 0xFF;
          if (c != -1)
          {
            ATOM_Key key = translateKey2TKey (c);
            if (key != KEY_UNKNOWN)
            {
              {
                _M_key_state[key] = 1;
                ATOM_KeyDownEvent e(this, key, 0);
                ATOM_APP->handleEvent (&e);
              }
              {
                _M_key_state[key] = 0;
                ATOM_KeyUpEvent e(this, key, 0);
                ATOM_APP->handleEvent (&e);
              }
            }
          }
        }
      }
#endif
    }
    break;

  case WM_SYSKEYDOWN :
  case WM_KEYDOWN :
    {
		if (lParam & REPEATED_KEYMASK)
		{
			// Ignore repeated keys
			return 0;
		}

		switch (wParam)
		{
		case VK_CONTROL:
			if (lParam & EXTENDED_KEYMASK)
				wParam = VK_RCONTROL;
			else
				wParam = VK_LCONTROL;
			break;
		case VK_SHIFT:
			if (::GetKeyState(VK_LSHIFT) & 0x8000)
				wParam = VK_LSHIFT;
			else if (::GetKeyState(VK_RSHIFT) & 0x8000)
				wParam = VK_RSHIFT;
			else
			{
				int sc = HIWORD(lParam) & 0xFF;
				if (sc == 0x2A)
					wParam = VK_LSHIFT;
				else if (sc == 0x36)
					wParam = VK_RSHIFT;
				else
					wParam = VK_LSHIFT;
			}
			break;
		case VK_MENU:
			if (lParam & EXTENDED_KEYMASK)
				wParam = VK_RMENU;
			else
				wParam = VK_LMENU;
			break;
		}

		ATOM_Key key = translateVK (wParam, HIWORD(lParam));

		unsigned modState = _M_key_mod;
		switch (key)
		{
		case KEY_LCTRL:
			modState |= KEYMOD_LCTRL;
			break;

		case KEY_RCTRL:
			modState |= KEYMOD_RCTRL;
			break;

		case KEY_LSHIFT:
			modState |= KEYMOD_LSHIFT;
			break;

		case KEY_RSHIFT:
			modState |= KEYMOD_RSHIFT;
			break;

		case KEY_LALT:
			modState |= KEYMOD_LALT;
			break;

		case KEY_RALT:
			modState |= KEYMOD_RALT;
			break;
		}

		_M_key_mod = modState;
		if (key != KEY_UNKNOWN && _M_key_state[key] == 0)
		{
			_M_key_state[key] = 1;
			ATOM_KeyDownEvent e(this, key, modState);
			ATOM_APP->handleEvent (&e);
		}
	}
    break;

  case WM_KEYUP:
  case WM_SYSKEYUP:
	  {
		  switch (wParam)
		  {
		  case VK_CONTROL:
			  if (lParam & EXTENDED_KEYMASK)
				  wParam = VK_RCONTROL;
			  else
				  wParam = VK_LCONTROL;
			  break;
		  case VK_SHIFT:
			  if (::GetKeyState(VK_LSHIFT) & 0x8000)
				  wParam = VK_LSHIFT;
			  else if (::GetKeyState(VK_RSHIFT) & 0x8000)
				  wParam = VK_RSHIFT;
			  else
			  {
				  int sc = HIWORD(lParam) & 0xFF;
				  if (sc == 0x2A)
					  wParam = VK_LSHIFT;
				  else if (sc == 0x36)
					  wParam = VK_RSHIFT;
				  else
					  wParam = VK_LSHIFT;
			  }
			  break;
		  case VK_MENU:
			  if (lParam & EXTENDED_KEYMASK)
				  wParam = VK_RMENU;
			  else
				  wParam = VK_LMENU;
			  break;
		  }

		  ATOM_Key key = translateVK (wParam, HIWORD(lParam));

		  unsigned modState = _M_key_mod;
			switch (key)
			{
			case KEY_LCTRL:
				modState &= ~KEYMOD_LCTRL;
				break;

			case KEY_RCTRL:
				modState &= ~KEYMOD_RCTRL;
				break;

			case KEY_LSHIFT:
				modState &= ~KEYMOD_LSHIFT;
				break;

			case KEY_RSHIFT:
				modState &= ~KEYMOD_RSHIFT;
				break;

			case KEY_LALT:
				modState &= ~KEYMOD_LALT;
				break;

			case KEY_RALT:
				modState &= ~KEYMOD_RALT;
				break;
			}
			_M_key_mod = modState;

			if (key != KEY_UNKNOWN && _M_key_state[key] != 0)
			{
				_M_key_state[key] = 0;
				ATOM_KeyUpEvent e(this, key, modState);
				ATOM_APP->handleEvent (&e);
			}
	  }
	  break;

  case WM_MOVE:
    {
	  if (lParam != 0 && lParam != 0x83008300)
      {
        short x = LOWORD(lParam);
        short y = HIWORD(lParam);

		if (x != _M_saved_winpos[0] || y != _M_saved_winpos[1])
        {
          ATOM_WindowMoveEvent e(this, x, y, x - _M_saved_winpos[0], y - _M_saved_winpos[1]);
          _M_saved_winpos[0] = x;
		  _M_saved_winpos[1] = y;
          ATOM_APP->handleEvent (&e);
        }
	  }
    }
	break;

  case WM_MOUSEMOVE:
    {
      short x = LOWORD(lParam);
      short y = HIWORD(lParam);
      if (x != _M_saved_position_x || y != _M_saved_position_y)
      {
        ATOM_MouseMoveEvent e(this, x, y, x - _M_saved_position_x, y - _M_saved_position_y, 
          ((wParam & MK_LBUTTON) != 0) ? KEYSTATE_PRESSED : KEYSTATE_RELEASED,
          ((wParam & MK_MBUTTON) != 0) ? KEYSTATE_PRESSED : KEYSTATE_RELEASED,
          ((wParam & MK_RBUTTON) != 0) ? KEYSTATE_PRESSED : KEYSTATE_RELEASED);

        _M_saved_position_x = x;
        _M_saved_position_y = y;

		ATOM_APP->handleEvent (&e);
      }
    }
    break;

  case WM_MOUSEWHEEL:
    {
		unsigned keymod = 0;

		if (wParam & MK_CONTROL)
		{
			if (::GetKeyState(VK_LCONTROL) & 0x8000)
				keymod |= KEYMOD_LCTRL;
			else if (::GetKeyState(VK_RCONTROL) & 0x8000)
				keymod |= KEYMOD_RCTRL;
			else
				keymod |= KEYMOD_LCTRL;
		}

		if (wParam & MK_SHIFT)
		{
			if (::GetKeyState(VK_LSHIFT) & 0x8000)
				keymod |= KEYMOD_LSHIFT;
			else if (::GetKeyState(VK_RSHIFT) & 0x8000)
				keymod |= KEYMOD_RSHIFT;
			else
				keymod |= KEYMOD_LSHIFT;
		}

		if (::GetKeyState(VK_LMENU) & 0x8000)
			keymod |= KEYMOD_LALT;
		else if (::GetKeyState(VK_RMENU) & 0x8000)
			keymod |= KEYMOD_RALT;

		short x = LOWORD(lParam);
		short y = HIWORD(lParam);

		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		::ScreenToClient (hWnd, &pt);

		DWORD ks = LOWORD(wParam);

		ATOM_MouseWheelEvent e(this, pt.x, pt.y, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA, keymod, 
			((ks & MK_LBUTTON) != 0) ? KEYSTATE_PRESSED : KEYSTATE_RELEASED,
			((ks & MK_MBUTTON) != 0) ? KEYSTATE_PRESSED : KEYSTATE_RELEASED,
			((ks & MK_RBUTTON) != 0) ? KEYSTATE_PRESSED : KEYSTATE_RELEASED);

		ATOM_APP->handleEvent (&e);
    }
    break;

  case WM_LBUTTONDBLCLK:
  case WM_MBUTTONDBLCLK:
  case WM_RBUTTONDBLCLK:
  case WM_LBUTTONUP:
  case WM_LBUTTONDOWN:
  case WM_MBUTTONUP:
  case WM_MBUTTONDOWN:
  case WM_RBUTTONUP:
  case WM_RBUTTONDOWN:
    {
		unsigned keymod = 0;

		if (wParam & MK_CONTROL)
		{
			if (::GetKeyState(VK_LCONTROL) & 0x8000)
				keymod |= KEYMOD_LCTRL;
			else if (::GetKeyState(VK_RCONTROL) & 0x8000)
				keymod |= KEYMOD_RCTRL;
			else
				keymod |= KEYMOD_LCTRL;
		}

		if (wParam & MK_SHIFT)
		{
			if (::GetKeyState(VK_LSHIFT) & 0x8000)
				keymod |= KEYMOD_LSHIFT;
			else if (::GetKeyState(VK_RSHIFT) & 0x8000)
				keymod |= KEYMOD_RSHIFT;
			else
				keymod |= KEYMOD_LSHIFT;
		}

		if (::GetKeyState(VK_LMENU) & 0x8000)
			keymod |= KEYMOD_LALT;
		else if (::GetKeyState(VK_RMENU) & 0x8000)
			keymod |= KEYMOD_RALT;

		ATOM_MouseButton button;
		short x = LOWORD(lParam);
		short y = HIWORD(lParam);

		switch ( uMsg)
		{
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			button = BUTTON_LEFT; 
			break;
		case WM_MBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
			button = BUTTON_MIDDLE; 
			break;
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
			button = BUTTON_RIGHT; 
			break;
		default:
			button = BUTTON_UNKNOWN; 
			break;
		}

		switch ( uMsg)
		{
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
			{
				ATOM_MouseDblClickEvent e(this, x, y, button, keymod);
				ATOM_APP->handleEvent (&e);
				break;
			}
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			{
				ATOM_MouseButtonUpEvent e(this, x, y, button, keymod);
				ATOM_APP->handleEvent (&e);
				::ReleaseCapture ();
				break;
			}
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			{
				ATOM_MouseButtonDownEvent e(this, x, y, button, keymod);
				ATOM_APP->handleEvent (&e);
				::SetCapture (hWnd);
				break;
			}
		}
    }
    break;

  case WM_ENTERSIZEMOVE:
	  {
		  _M_insizemove = true;
	  }
	  break;

  case WM_EXITSIZEMOVE:
	  {
		  checkResizeDevice (getDevice ());
		  _M_insizemove = false;
	  }
	  break;

  case WM_SETFOCUS:
  case WM_KILLFOCUS:
	  {
		  resetKeyStates ();
		  break;
	  }

  case WM_SIZE:
     {
		 if (wParam != SIZE_MINIMIZED)
		 {
            RECT rcCurrentClient;
			::GetClientRect( hWnd, &rcCurrentClient );
            if( rcCurrentClient.top == 0 && rcCurrentClient.bottom == 0 )
			{
				break;
			}

			if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
			{
				ATOM_RenderWindow::Hints& hints = getWindowHints();

				if (hints.bounds.first != LOWORD(lParam) || hints.bounds.second != HIWORD(lParam))
				{
					int nw = LOWORD(lParam);
					int ny = HIWORD(lParam);
					ATOM_LOGGER::information ("WM_SIZE :%d,%d,%d,%d \n", nw, ny, hints.bounds.first, hints.bounds.second);
					if (nw == 0 || ny == 0)
						break;

					hints.bounds.first = LOWORD(lParam);
					hints.bounds.second = HIWORD(lParam);

					if (!_M_insizemove)
					{
						checkResizeDevice (getDevice());
					}
				}
			}
		}
    }
    break;

  case WM_SETCURSOR:
    {
		ATOM_RenderDevice *device = getDevice();
		if (device)
		{
			if (LOWORD(lParam) == HTCLIENT)
			{
				SetCursor (NULL);
				if (1 || !device->isCustomCursorShown(this))
				{
					device->showCustomCursor (this, true);
				}
				return TRUE;
			}
			else
			{
				device->showCustomCursor (this, false);
				::ShowCursor (TRUE);
			}
		}
    }
    break;

  case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC dc = BeginPaint(hWnd, &ps);
      ATOM_WindowExposeEvent e(this);
      ATOM_APP->handleEvent (&e);
      EndPaint(hWnd, &ps);
    }
    return 0;

  case WM_ACTIVATE:
    {
      ATOM_WindowActiveEvent e(this, (wParam != WA_INACTIVE) && !HIWORD(wParam));
      if (e.active ^ _M_active)
      {
		  ATOM_RenderDevice *device = getDevice();
		  if (device)
		  {
			  device->activate (e.active);
			  if (e.active)
			  {
				  device->setViewCursor (nullptr);
			  }
		  }
		  _M_active = e.active;
		  ATOM_APP->handleEvent (&e);
      }
    }
    break;

  case WM_ACTIVATEAPP:
    {
      ATOM_AppActiveEvent e(wParam == TRUE);
      ATOM_APP->handleEvent (&e);
    }
    break;

  case WM_CLOSE:
    {
      ATOM_WindowCloseEvent e(this);
      ATOM_APP->handleEvent (&e);
    }
    return 0;

  case WM_SYSCOMMAND:
    {
      switch (wParam)
      {
      case SC_CLOSE:
        {
          ATOM_WindowCloseEvent e(this);
          ATOM_APP->handleEvent (&e);
          return 0;
        }
      case SC_SCREENSAVE:
        {
          return 0;
        }
      default:
        {
          break;
        }
      }
	  break;
    }

  case WM_DROPFILES:
	{
		if (ATOM_APP)
		{
			ATOM_WindowFilesDroppedEvent e(this);
			HDROP hDrop = (HDROP)wParam;
			unsigned numFiles = ::DragQueryFileA (hDrop, 0xFFFFFFFF, NULL, 0);
			char filenameBuffer[ATOM_VFS::max_filename_length];
			char vfilenameBuffer[ATOM_VFS::max_filename_length];
			for (unsigned i = 0; i < e.filesDropped.size(); ++i)
			{
				::DragQueryFileA (hDrop, i, filenameBuffer, sizeof(filenameBuffer));
				if (ATOM_GetVFSPathName (filenameBuffer, vfilenameBuffer))
				{
					e.filesDropped.push_back (vfilenameBuffer);
				}
			}
			::DragFinish (hDrop);

			if (e.filesDropped.size() > 0)
			{
				ATOM_APP->handleEvent (&e);
			}
		}
		break;
	}

  case WM_POWERBROADCAST:
    {
      if (wParam == PBT_APMQUERYSUSPEND)
      {
        return BROADCAST_QUERY_DENY;
      }
      else if (wParam == PBT_APMBATTERYLOW)
      {
        ATOM_AppBatteryLowEvent e;
        ATOM_APP->handleEvent (&e);
      }
    }
    return 1;

  default:
    break;
  }

  if ( getWindowInfo() && getWindowInfo()->oldproc)
    return getWindowInfo()->oldproc(hWnd, uMsg, wParam, lParam);
  else
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

ATOM_WindowInfo* ATOM_RenderWindow::getWindowInfo() const
{
  return _M_window_info;
}

void ATOM_RenderWindow::getMousePosition(int* px, int* py) const {
  if (_M_window_info)
  {
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(_M_window_info->handle, &pt);
    if ( px)
      *px = pt.x;
    if ( py)
      *py = pt.y;
  }
  else
  {
    if (px)
      *px = 0;
    if (py)
      *py = 0;
  }
}

void ATOM_RenderWindow::setMousePosition(int x, int y) {
  if (_M_window_info)
  {
    POINT pt;
    pt.x = x;
    pt.y = y;
    ClientToScreen(_M_window_info->handle, &pt);
    SetCursorPos(pt.x, pt.y);
    _M_saved_position_x = x;
    _M_saved_position_y = y;
  }
}

bool ATOM_RenderWindow::keyPressed(ATOM_Key key) const {
	return (key < KEY_COUNT) && _M_key_state[key] != 0;
}

void ATOM_RenderWindow::showMouseCursor(bool show) {
  ::ShowCursor(show);
}

void *ATOM_RenderWindow::getWindowId() const
{
  return _M_window_info ? (void*) _M_window_info->handle : 0;
}

bool ATOM_RenderWindow::isActive () const
{
  return _M_active;
}

ATOM_Key ATOM_RenderWindow::translateKey2TKey (int from) {
  return TranslateKey (from);
}

void ATOM_RenderWindow::resizeDevice (ATOM_RenderDevice *device) {
}

bool ATOM_RenderWindow::isMultisampleSupported (void)
{
  for (unsigned i = 2; i < 16; ++i)
  {
    if (isSampleSupported (i))
    {
      return true;
    }
  }

  return false;
}

int ATOM_RenderWindow::getMultiSampleType (void) const
{
  return _M_window_hints.samples;
}

bool ATOM_RenderWindow::isSampleSupported(unsigned sample) {
  if (_M_window_info && sample > 0 && sample < 16)
  {
    return _M_window_info->multisample[sample];
  }
  return false;
}

void ATOM_RenderWindow::present (void)
{
	presentImpl ();
}

void ATOM_RenderWindow::getWindowBounds (unsigned *w, unsigned *h, unsigned *client_w, unsigned *client_h) {
  HWND hWnd = (HWND)getWindowId();
  if (hWnd)
  {
    if (IsIconic (hWnd))
    {
      WINDOWPLACEMENT wp;
      memset (&wp, 0, sizeof(wp));
      wp.length = sizeof(WINDOWPLACEMENT);
      GetWindowPlacement (hWnd, &wp);

      if ((wp.flags & WPF_RESTORETOMAXIMIZED) != 0 && wp.showCmd == SW_SHOWMINIMIZED)
      {
        ShowWindow (hWnd, SW_RESTORE);
        RECT rc;

        GetClientRect (hWnd, &rc);
        if (client_w)
        {
          *client_w = rc.right - rc.left;
        }
        if (client_h)
        {
          *client_h = rc.bottom - rc.top;
        }

        GetWindowRect (hWnd, &rc);
        if (w)
        {
          *w = rc.right - rc.left;
        }
        if (h)
        {
          *h = rc.bottom - rc.top;
        }

        ShowWindow (hWnd, SW_MINIMIZE);
      }
      else
      {
        RECT rcFrame = { 0 };
        LONG style = GetWindowLong (hWnd, GWL_STYLE);
        AdjustWindowRect (&rcFrame, style, GetMenu(hWnd) != NULL);
        LONG frameWidth = rcFrame.right - rcFrame.left;
        LONG frameHeight = rcFrame.bottom - rcFrame.top;

        if (w)
        {
          *w = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
        }
        if (h)
        {
          *h = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
        }

        if (client_w)
        {
          *client_w = wp.rcNormalPosition.right - wp.rcNormalPosition.left - frameWidth;
        }
        if (client_h)
        {
          *client_h = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top - frameHeight;
        }
      }
    }
    else
    {
      RECT rc;
      GetClientRect (hWnd, &rc);

      if (client_w)
      {
        *client_w = rc.right - rc.left;
      }
      if (client_h)
      {
        *client_h = rc.bottom - rc.top;
      }

      GetWindowRect (hWnd, &rc);
      if (w)
      {
        *w = rc.right - rc.left;
      }
      if (h)
      {
        *h = rc.bottom - rc.top;
      }
    }
  }
  else
  {
    if (w)
    {
      *w = 0;
    }

    if (h)
    {
      *h = 0;
    }

    if (client_w)
    {
      *client_w = 0;
    }

    if (client_h)
    {
      *client_h = 0;
    }
  }
}

LONG WINAPI ATOM_RenderWindow::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  static ATOM_VECTOR<ATOM_RenderWindow*> windows;
  static ATOM_RenderWindow *lastMouseFocusWindow = 0;

  if (uMsg == WM_MOUSEMOVE || uMsg == WM_NCMOUSEMOVE)
  {
    for (unsigned i = 0; i < windows.size(); ++i)
    {
      if (windows[i]->getWindowInfo() 
        && windows[i]->getWindowInfo()->handle == hWnd
        && lastMouseFocusWindow != windows[i])
      {
        lastMouseFocusWindow = windows[i];
        ATOM_RenderDevice *device = lastMouseFocusWindow->getDevice();
        if (device)
        {
          device->setViewCursor (lastMouseFocusWindow);
        }
        break;
      }
    }
  }

  switch ( uMsg)
  {
  case WMU_RENDERWINDOW_ATTACH:
    windows.push_back ((ATOM_RenderWindow*)wParam);
    return 0;

  case WM_DESTROY:
    for (unsigned i = 0; i < windows.size(); ++i)
    {
      if (windows[i]->getWindowInfo() && windows[i]->getWindowInfo()->handle == hWnd)
      {
        windows.erase (windows.begin() + i);
        break;
      }
    }

  default:
    {
      for (unsigned i = 0; i < windows.size(); ++i)
      {
        if (windows[i]->getWindowInfo() && windows[i]->getWindowInfo()->handle == hWnd)
          return windows[i]->processWindowMessage(hWnd, uMsg, wParam, lParam);
      }

	  if (ATOM_APP)
	  {
		  ATOM_AppWMEvent e(hWnd, uMsg, wParam, lParam);
		  ATOM_APP->handleEvent(&e);
		  return e.eat ? e.returnValue : DefWindowProcW(hWnd, uMsg, wParam, lParam);
	  }
	  else
	  {
		  return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	  }
    }
  }
}

unsigned ATOM_RenderWindow::getWindowWidth (void) const
{
	return _M_window_hints.bounds.first;
}

unsigned ATOM_RenderWindow::getWindowHeight (void) const
{
	return _M_window_hints.bounds.second;
}

const ATOM_RenderWindow::Hints& ATOM_RenderWindow::getWindowHints(void) const 
{
	return _M_window_hints;
}

ATOM_RenderWindow::Hints& ATOM_RenderWindow::getWindowHints(void) 
{
	return _M_window_hints;
}

ATOM_RenderDevice* ATOM_RenderWindow::getDevice(void) const 
{
	return _M_device;
}

ATOM_RenderView *ATOM_RenderWindow::getRenderView (void) const 
{
	return _M_view;
}

void ATOM_RenderWindow::checkResizeDevice (ATOM_RenderDevice *device)
{
	if (device)
	{
		RECT rc;
		::GetClientRect (this->getWindowInfo()->handle, &rc);

		int w, h;
		device->getBackbufferSize (&w, &h);

		int nw = rc.right - rc.left;
		int nh = rc.bottom - rc.top;
		if (w != nw || h != nh)
		{
			if (device)
			{
				resizeDevice (device);
			}

			ATOM_WindowResizeEvent e(this, nw, nh);
			ATOM_APP->handleEvent (&e);
		}
	}
}

void ATOM_RenderWindow::resetKeyStates (void)
{
	_M_key_mod = 0;
	memset (_M_key_state, 0, sizeof(_M_key_state));
}

const char *ATOM_RenderWindow::getKeyDesc (ATOM_Key key) const
{
	return GetKeyDesc (key);
}

