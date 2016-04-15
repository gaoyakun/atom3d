#ifndef __DEBUGHELP_CHART_H
#define __DEBUGHELP_CHART_H

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "../ATOM_thread.h"
#include "basedefs.h"
#include "stl.h"

class ATOM_DBGHLP_API ATOM_Chart
{
public:
  ATOM_Chart (void);
  virtual ~ATOM_Chart (void);

public:
  virtual bool create (const char *name, unsigned w, unsigned h, unsigned vw, unsigned vh, unsigned win_w, unsigned win_h);
  virtual void destroy (void);
  virtual void update (void);
  virtual void show (bool show);
  virtual void setPosition (unsigned x, unsigned y);
  virtual bool processMessage (UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);
  virtual bool isShown (void) const;
  virtual void drawBackground (HDC hdc);
  virtual void paint (HDC hdc) = 0;

  // internal usage
  virtual bool _create (const char *name, unsigned w, unsigned h, unsigned vw, unsigned vh, unsigned win_w, unsigned win_h);
  virtual void _destroy (void);
  virtual void _update (void);
  virtual void _show (bool show);
  virtual bool _isShown (void) const;

protected:
  virtual void drawDefaultBackground (HDC hdc);

public:
  static void startChartService (void);
  static void stopChartService (void);
  static bool ChartServiceRunning (void);

public:
  HWND getHWND (void) const { return m_hWnd; }
  void setDrawBackgroundCallback (void (*callback)(HDC,int,int,void*)) { m_bkcallback = callback; }
  void setDrawBackgroundCallbackParam (void *param) { m_bkcallback_param = param; }

protected:
  HWND m_hWnd;
  HDC m_memDC;
  HBITMAP m_memBitmap;

  unsigned m_width;
  unsigned m_height;
  unsigned m_virtual_width;
  unsigned m_virtual_height;
  unsigned m_pos_x;
  unsigned m_pos_y;
  unsigned m_window_w;
  unsigned m_window_h;
  void (*m_bkcallback) (HDC, int, int, void*);
  void *m_bkcallback_param;
  bool m_isshown;
};

#pragma warning(push)
#pragma warning(disable:4251)

class ATOM_DBGHLP_API ATOM_ChartLines: public ATOM_Chart
{
  struct Point
  {
    POINT pt;
    bool linknext;
  };

  struct PathInfo
  {
    ATOM_VECTOR<Point> points;
    HPEN pen;
  };

public:
  ATOM_ChartLines (void);

public:
  virtual bool newPath (const char *path, int linestyle, int width, COLORREF color);
  virtual void insertPoint (const char *path, unsigned x, unsigned y, bool linknext);
  virtual void clearPoints (void);
  virtual void clear (void);
  virtual void paint (HDC hdc);

protected:
  ATOM_HASHMAP<ATOM_STRING, PathInfo> m_pathes;
  bool m_clearback;
  ATOM_FastMutex m_lock;
};

#pragma warning(pop)

#endif // __DEBUGHELP_CHART_H
