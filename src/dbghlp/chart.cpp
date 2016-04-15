#include "StdAfx.h"
#include <ATOM_thread.h>
#include "stl.h"
#include "chart.h"

#if defined(_MSC_VER)
# pragma warning(disable:4786)
#endif

#define WMU_CS_CREATE (WM_APP + 1)
#define WMU_CS_DESTROY (WM_APP + 2)
#define WMU_CS_SHOW (WM_APP + 3)
#define WMU_CS_UPDATE (WM_APP + 4)

struct ChartThreadParams
{
  DWORD request;
  ATOM_Chart *chart;
  bool isshown;
  char name[256];
  unsigned w;
  unsigned h;
  unsigned vw;
  unsigned vh;
  unsigned ww;
  unsigned wh;
  HANDLE hRequestEvent;
  HANDLE hFinishEvent;
  ATOM_FastMutex lock;
};

static HANDLE chartserviceEndEvent = 0;
static ATOM_VECTOR<ChartThreadParams> chartserviceParamsIn;
static ATOM_VECTOR<ChartThreadParams> chartserviceParamsProc;
static ATOM_VECTOR<ChartThreadParams> *ParamIn = &chartserviceParamsIn;
static ATOM_VECTOR<ChartThreadParams> *ParamProc = &chartserviceParamsProc;
static ATOM_FastMutex paramMutex;

unsigned __stdcall ChartService (void*)
{
  for (;;)
  {
    for (unsigned i = 0; i < ParamProc->size(); ++i)
    {
      const ChartThreadParams &params = (*ParamProc)[i];
      switch (params.request)
      {
      case WMU_CS_CREATE:
        params.chart->_create (
          params.name, 
          params.w, 
          params.h, 
          params.vw, 
          params.vh, 
          params.ww, 
          params.wh
          );
        break;

      case WMU_CS_DESTROY:
        params.chart->_destroy ();
        break;

      case WMU_CS_SHOW:
        params.chart->_show (params.isshown);
        break;

      case WMU_CS_UPDATE:
        params.chart->_update ();
        break;

      default:
        break;
      }
    }

    ParamProc->resize(0);

    {
      ATOM_FastMutex::ScopeMutex l(paramMutex);
      ATOM_VECTOR<ChartThreadParams> *tmp = ParamIn;
      ParamIn = ParamProc;
      ParamProc = tmp;
    }

    {
      MSG msg;

      while (::PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
      {
        ::TranslateMessage (&msg);
        ::DispatchMessage (&msg);
      }
    }

    if (::WaitForSingleObject (chartserviceEndEvent, 0) == WAIT_OBJECT_0)
    {
      break;
    }

    ::Sleep (50);
  }

  return 0;
};

static HANDLE chartserviceThread = 0;
static unsigned chartserviceThreadId = 0;

void ATOM_Chart::startChartService (void)
{
  chartserviceEndEvent = ::CreateEvent (0, FALSE, FALSE, 0);
  chartserviceThread = (HANDLE)::_beginthreadex (0, 0, &ChartService, 0, 0, &chartserviceThreadId);
}

void ATOM_Chart::stopChartService (void)
{
  if (chartserviceThread)
  {
    ::SetEvent (chartserviceEndEvent);
    ::WaitForSingleObject (chartserviceThread, INFINITE);
    ::CloseHandle (chartserviceThread);
    chartserviceThread = 0;
  }
}

bool ATOM_Chart::ChartServiceRunning (void)
{
  return chartserviceThread != NULL;
}

ATOM_Chart::ATOM_Chart (void)
{
  if (!ChartServiceRunning())
  {
    startChartService ();
  }

  m_isshown = false;
  m_hWnd = 0;
  m_memDC = 0;
  m_memBitmap = 0;
  m_width = 0;
  m_height = 0;
  m_virtual_width = 0;
  m_virtual_height = 0;
  m_pos_x = 0;
  m_pos_y = 0;
  m_bkcallback = 0;
  m_bkcallback_param = 0;
  m_window_w = 0;
  m_window_h = 0;
}

ATOM_Chart::~ATOM_Chart (void)
{
  destroy ();
}

static const char *CHART_WNDCLASS = "ATOM_CHART_WCLASS";
LRESULT CALLBACK ATOM_ChartWindowProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void _ChartRegisterClass (void)
{
  WNDCLASS wc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hbrBackground = (HBRUSH)::GetStockObject(DKGRAY_BRUSH);
  wc.hCursor = (HCURSOR)::LoadCursor(NULL, IDC_ARROW);
  wc.hIcon = (HICON)::LoadIcon(NULL, IDI_WINLOGO);
  wc.hInstance = (HINSTANCE)::GetModuleHandle (NULL);
  wc.lpfnWndProc = &ATOM_ChartWindowProc;
  wc.lpszClassName = CHART_WNDCLASS;
  wc.lpszMenuName = 0;
  wc.style = CS_VREDRAW|CS_HREDRAW;
  ::RegisterClass (&wc);
}

static ATOM_MAP<HWND, ATOM_Chart*> chartWindows;
static ATOM_FastMutex chartWindowLock;

LRESULT CALLBACK ATOM_ChartWindowProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  ATOM_Chart *chart = 0;

  if (msg == WM_CREATE)
  {
    CREATESTRUCT *cs = (CREATESTRUCT*)lParam;
    chart = (ATOM_Chart*)cs->lpCreateParams;

    {
      ATOM_FastMutex::ScopeMutex l(chartWindowLock);
      chartWindows[hWnd] = chart;
    }
  }
  else if (msg == WM_DESTROY)
  {
    ATOM_FastMutex::ScopeMutex l(chartWindowLock);
    ATOM_MAP<HWND, ATOM_Chart*>::iterator it = chartWindows.find (hWnd);
    if (it != chartWindows.end())
    {
      chartWindows.erase (it);
    }
  }
  else
  {
    ATOM_FastMutex::ScopeMutex l(chartWindowLock);
    ATOM_MAP<HWND, ATOM_Chart*>::iterator it = chartWindows.find (hWnd);
    chart = (it != chartWindows.end()) ? it->second : 0;
  }

  LRESULT lresult = 0;

  if (!chart || !chart->processMessage (msg, wParam, lParam, &lresult))
  {
    lresult = ::DefWindowProc (hWnd, msg, wParam, lParam);
  }

  return lresult;
}

bool ATOM_Chart::create (const char *name, unsigned w, unsigned h, unsigned vw, unsigned vh, unsigned win_w, unsigned win_h)
{
  if (!ChartServiceRunning ())
  {
    return false;
  }

  ChartThreadParams params;
  params.request = WMU_CS_CREATE;
  params.chart = this;
  strcpy (params.name, name);
  params.w = w;
  params.h = h;
  params.vw = vw;
  params.vh = vh;
  params.ww = win_w;
  params.wh = win_h;

  {
    ATOM_FastMutex::ScopeMutex l(paramMutex);
    ParamIn->push_back (params);
  }

  return true;
}

bool ATOM_Chart::_create (const char *name, unsigned w, unsigned h, unsigned vw, unsigned vh, unsigned win_w, unsigned win_h)
{
  _destroy ();

  _ChartRegisterClass ();

  DWORD style = WS_CAPTION|WS_SYSMENU|WS_MINIMIZE;
  RECT rc;
  rc.left = 0;
  rc.top = 0;
  rc.right = win_w;
  rc.bottom = win_h;

  ::AdjustWindowRect (&rc, style, FALSE);
  win_w = rc.right - rc.left;
  win_h = rc.bottom - rc.top;

  m_hWnd = ::CreateWindowA (CHART_WNDCLASS, name, style, CW_USEDEFAULT, CW_USEDEFAULT, win_w, win_h, NULL, NULL, ::GetModuleHandle(NULL), this);
  if (!m_hWnd)
  {
    return false;
  }

  m_window_w = win_w;
  m_window_h = win_h;

  HDC hDC = ::GetDC (m_hWnd);
  m_memDC = ::CreateCompatibleDC (hDC);
  m_memBitmap = ::CreateCompatibleBitmap (hDC, win_w, win_h);
  ::SelectObject (m_memDC, m_memBitmap);
  ::ReleaseDC (m_hWnd, hDC);

  m_width = w;
  m_height = h;
  m_virtual_width = vw ? vw : w;
  m_virtual_height = vh ? vh : h;
  m_pos_x = 0;
  m_pos_y = 0;
  return true;
}

void ATOM_Chart::destroy (void)
{
  if (!ChartServiceRunning ())
  {
    return;
  }

  if (m_hWnd )
  {
    ChartThreadParams params;
    params.request = WMU_CS_DESTROY;
    params.chart = this;

    {
      ATOM_FastMutex::ScopeMutex l(paramMutex);
      ParamIn->push_back (params);
    }
  }
}

void ATOM_Chart::_destroy (void)
{
  if (m_hWnd)
  {
    ::DestroyWindow (m_hWnd);
    m_hWnd = 0;

    ::DeleteDC (m_memDC);
    m_memDC = 0;

    ::DeleteObject (m_memBitmap);
    m_memBitmap = 0;
  }
}

void ATOM_Chart::update (void)
{
  if (!ChartServiceRunning ())
  {
    return;
  }

  if (m_hWnd )
  {
    ChartThreadParams params;
    params.request = WMU_CS_UPDATE;
    params.chart = this;

    {
      ATOM_FastMutex::ScopeMutex l(paramMutex);
      ParamIn->push_back (params);
    }
  }
}

void ATOM_Chart::_update (void)
{
  if (m_hWnd)
  {
    InvalidateRect (m_hWnd, NULL, FALSE);
  }
}

void ATOM_Chart::show (bool show)
{
  if (!ChartServiceRunning ())
  {
    return;
  }

  if (m_hWnd )
  {
    ChartThreadParams params;
    params.request = WMU_CS_SHOW;
    params.chart = this;
    params.isshown = show;

    {
      ATOM_FastMutex::ScopeMutex l(paramMutex);
      ParamIn->push_back (params);
    }

    m_isshown = show;
  }
}

void ATOM_Chart::_show (bool show)
{
  if (m_hWnd)
  {
    ::ShowWindow (m_hWnd, show ? SW_SHOW : SW_HIDE);
    ::UpdateWindow (m_hWnd);
  }
}

bool ATOM_Chart::isShown (void) const
{
  return m_isshown;
}

bool ATOM_Chart::_isShown (void) const
{
  return m_hWnd && ::IsWindowVisible (m_hWnd);
}

void ATOM_Chart::setPosition (unsigned x, unsigned y)
{
  if (m_pos_x != x || m_pos_y != y)
  {
    m_pos_x = x;
    m_pos_y = y;
    update ();
  }
}

void ATOM_Chart::drawBackground (HDC hdc)
{
  if (m_bkcallback)
  {
    for (unsigned i = 0; i < m_window_w; ++i)
      for (unsigned j = 0; j < m_window_h; ++j)
      {
        m_bkcallback (hdc, i, j, m_bkcallback_param);
      }
  }
  else
  {
    drawDefaultBackground (hdc);
  }
}

void ATOM_Chart::drawDefaultBackground (HDC hdc)
{
  HBRUSH hbr = (HBRUSH)::GetStockObject(GRAY_BRUSH);
  RECT rc;
  rc.left = 0;
  rc.top = 0;
  rc.right = m_window_w;
  rc.bottom = m_window_h;
  ::FillRect (hdc, &rc, hbr);
}

bool ATOM_Chart::processMessage (UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
  switch (msg)
  {
  case WM_ERASEBKGND:
    *result = 1;
    return true;

  case WM_PAINT:
    {
      PAINTSTRUCT ps;
      paint (m_memDC);
      ::BeginPaint (m_hWnd, &ps);
      ::BitBlt (ps.hdc, 0, 0, m_window_w, m_window_h, m_memDC, 0, 0, SRCCOPY);
      ::EndPaint (m_hWnd, &ps);
      *result = 0;
      return true;
    }

  case WM_CLOSE:
    _show (false);
    *result = 0;
    return true;

  default:
    break;
  }

  return false;
}

/////////////////////////////////////////ATOM_ChartLines///////////////////////////////////////////

ATOM_ChartLines::ATOM_ChartLines (void)
{
  m_clearback = true;
}

bool ATOM_ChartLines::newPath (const char *path, int linestyle, int width, COLORREF color)
{
  ATOM_HASHMAP<ATOM_STRING, PathInfo>::iterator it = m_pathes.find (path);
  if (it != m_pathes.end ())
  {
    return false;
  }
  PathInfo &pi = m_pathes[path];
  pi.pen = ::CreatePen (linestyle, width, color);
  return true;
}

void ATOM_ChartLines::insertPoint (const char *path, unsigned x, unsigned y, bool linknext)
{
  if (!isShown())
  {
    return;
  }

  ATOM_FastMutex::ScopeMutex l(m_lock);

  ATOM_HASHMAP<ATOM_STRING, PathInfo>::iterator it = m_pathes.find (path);
  if (it == m_pathes.end ())
  {
    return;
  }

  Point pt;
  pt.pt.x = x;
  pt.pt.y = y;
  pt.linknext = linknext;

  it->second.points.push_back (pt);
}

void ATOM_ChartLines::clearPoints (void)
{
  ATOM_FastMutex::ScopeMutex l(m_lock);

  for (ATOM_HASHMAP<ATOM_STRING, PathInfo>::iterator it = m_pathes.begin(); it != m_pathes.end(); ++it)
  {
    it->second.points.resize(0);
  }

  m_clearback = true;
}

void ATOM_ChartLines::clear (void)
{
  ATOM_FastMutex::ScopeMutex l(m_lock);

  for (ATOM_HASHMAP<ATOM_STRING, PathInfo>::iterator it = m_pathes.begin(); it != m_pathes.end(); ++it)
  {
    ::DeleteObject (it->second.pen);
  }

  m_pathes.clear ();
  m_clearback = true;
}

void ATOM_ChartLines::paint (HDC hdc)
{
  // fill background
  if (m_clearback)
  {
    m_clearback = false;
    drawBackground (hdc);
  }

  // draw mark
  HBRUSH hbrMark[2] = {
    (HBRUSH)::GetStockObject(WHITE_BRUSH),
    (HBRUSH)::GetStockObject(DKGRAY_BRUSH)
  };
  static int mark = 0;
  RECT rc;
  rc.left = 0;
  rc.top = 0;
  rc.right = 10;
  rc.bottom = 10;
  ::FillRect (hdc, &rc, hbrMark[mark++%2]);

  // draw lines
  {
    ATOM_FastMutex::ScopeMutex l(m_lock);

    for (ATOM_HASHMAP<ATOM_STRING, PathInfo>::iterator it = m_pathes.begin(); it != m_pathes.end(); ++it)
    {
      PathInfo &pi = it->second;

      if (pi.points.size() > 1)
      {
        HGDIOBJ oldPen = ::SelectObject (hdc, (HGDIOBJ)pi.pen);

        for (unsigned i = 0; i < pi.points.size()-1; ++i)
        {
          if (pi.points[i].linknext)
          {
            ::MoveToEx (hdc, pi.points[i].pt.x, pi.points[i].pt.y, 0);
            ::LineTo (hdc, pi.points[i+1].pt.x, pi.points[i+1].pt.y);
          }
          else
          {
            continue;
          }
        }

        ::SelectObject (hdc, oldPen);

        Point pt = pi.points.back();
        pi.points.resize(0);
        pi.points.push_back (pt);
      }
    }
  }
}

