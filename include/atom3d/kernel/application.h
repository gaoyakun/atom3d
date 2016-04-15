/**	\file application.h
 *	Application类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_APPLICATION_H
#define __ATOM_KERNEL_APPLICATION_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include "../ATOM_dbghlp.h"
#include "basedefs.h"
#include "eventdispatcher.h"
#include "framestamp.h"

#define ATOM_APP ATOM_GetApplication()

//! 应用程序初始化事件，在刚进入主循环时产生.
//! 该事件响应函数false会导致应用程序退出,退出码由errorcode决定
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_AppInitEvent: public ATOM_Event
{
public:
  ATOM_AppInitEvent (void): success(true), errorcode(0) {}
  ATOM_AppInitEvent (int argc_, char **argv_): argc(argc_), argv(argv_) {}
  int argc;
  char **argv;
  bool success;
  int errorcode;

  ATOM_DECLARE_EVENT(ATOM_AppInitEvent)
};

//! 应用程序退出事件，在退出主循环前产生.
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_AppExitEvent: public ATOM_Event
{
  ATOM_DECLARE_EVENT(ATOM_AppExitEvent)
};

//! 向ATOM_Application投递此事件来要求应用程序退出主循环，返回值由returnValue决定.
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_AppQuitEvent: public ATOM_Event
{
public:
  ATOM_AppQuitEvent (void): returnValue(0) {}
  ATOM_AppQuitEvent (int returnValue_): returnValue(returnValue_) {}
  int returnValue;

  ATOM_DECLARE_EVENT(ATOM_AppQuitEvent)
};

//! 应用程序激活或转入后台事件.
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_AppActiveEvent: public ATOM_Event
{
public:
  ATOM_AppActiveEvent (void): active(true) {}
  ATOM_AppActiveEvent (bool active_): active(active_) {}
  bool active;

  ATOM_DECLARE_EVENT(ATOM_AppActiveEvent)
};

//! Fatal事件，当程序运行时产生致命错误时可以发送此事件到APP对象，让APP结束程序.
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_AppFatalEvent: public ATOM_Event
{
public:
  ATOM_AppFatalEvent (void): notify(true), showmsg(true) {}
  ATOM_AppFatalEvent (bool notify_, bool showmsg_, const char *msg_): notify(notify_), showmsg(showmsg_), msg(msg_) {}

  bool notify;
  bool showmsg;
  ATOM_STRING msg;

  ATOM_DECLARE_EVENT(ATOM_AppFatalEvent)
};

//! Idle事件，会在应用程序空闲时持续产生.
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_AppIdleEvent: public ATOM_Event
{
  ATOM_DECLARE_EVENT(ATOM_AppIdleEvent)
};

//! 电池电量不足事件（目前尚未使用）.
class ATOM_KERNEL_API ATOM_AppBatteryLowEvent: public ATOM_Event
{
  ATOM_DECLARE_EVENT(ATOM_AppBatteryLowEvent)
};

//! Windows消息事件.
//! 该事件响应函数返回true表示已经完全处理此事件不做默认处理
class ATOM_KERNEL_API ATOM_AppWMEvent: public ATOM_Event
{
public:
  ATOM_AppWMEvent (void): hWnd(NULL), msg(0), wParam(0), lParam(0), returnValue(0), eat(false) {}

  ATOM_AppWMEvent (HWND hWnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_)
	: hWnd(hWnd_)
	, msg(msg_)
	, wParam(wParam_)
	, lParam(lParam_)
	, returnValue(0)
	, eat(false)
  {
  }

  HWND hWnd;
  UINT msg;
  WPARAM wParam;
  LPARAM lParam;
  unsigned returnValue;
  bool eat;

  ATOM_DECLARE_EVENT(ATOM_AppWMEvent)
};

class ATOM_KERNEL_API ATOM_Application;

//! 全局函数，用于获取当前的应用程序实例指针.
//! \return 应用程序实例指针
ATOM_KERNEL_API ATOM_Application * ATOM_CALL ATOM_GetApplication (void);

//! \class ATOM_Application
//! ATOM应用程序类.
//! 推荐使用此类作为ATOM引擎应用程序框架.
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_Application: public ATOM_EventDispatcher
{
public:
	class ErrorHandler: public ATOM_AppErrorHandler
	{
	public:
		virtual void _reportError (PEXCEPTION_POINTERS p, ATOM_AppErrorHandler::ErrorType errorType);
	};

public:
	//! 构造函数.
	ATOM_Application (void);

	//! 析构函数.
	virtual ~ATOM_Application (void);

public:
	//! 设置游戏循环间隔时间.
	//! \param ms 间隔时间，单位毫秒。如果为零则表示进行全速循环。
	void setFrameInterval (int ms);

	//! 获取游戏循环间隔时间.
	//! \return 间隔时间，单位毫秒。
	int getFrameInterval (void);

public:
	void onAppInit (ATOM_AppInitEvent *event);
	void onAppExit (ATOM_AppExitEvent *event);
	void onAppQuit (ATOM_AppQuitEvent *event);
	void onAppFatal (ATOM_AppFatalEvent *event);

public:
	//! 获取当前帧的时间戳.
	//! \return 时间戳
	//! \sa ATOM_FrameStamp
	const ATOM_FrameStamp &getFrameStamp (void);

	//! 设置时间流逝速度的比例因子.
	//! 这个因子越大，getFrameStamp函数返回的时间流逝的越快
	//! \param timescale 比例因子
	void setTimeScale (float timescale);

	//! 获取时间流逝速度的比例因子.
	//! \return 比例因子
	float getTimeScale (void) const;

	//! 处理当前事件队列里所有未处理的事件.
	//! \param retVal 如果返回true, 这个指针指向退出码
	//! \return true表示得到了应用程序退出的请求， 反之为false
	bool pumpEvents (int *retVal);

	//! 处理系统窗口事件
	//! 此方法不会处理应用程序事件
	//! \return true表示得到了应用程序退出的请求， 反之为false
	bool pumpWMEvents (void);

	//! 投递一个退出请求.
	//! 如果调用了此函数，主循环将会在下一次循环后结束
	//! \param ret 退出码
	void postQuitEvent (int ret);

	//! 设置主窗口句柄
	//! \param hWnd 主窗口句柄
	void setMainWindow (HWND hWnd);

	//! 获取主窗口句柄
	//! \return 主窗口句柄
	HWND getMainWindow (void) const;

	//! 更新帧时间戳
	void updateFrameStamp (void);

	//! 消息循环
	void eventLoop (void);

	//! 设置默认转储文件名
	void setDefaultDumpFileName (const char *dmpFileName);

	//! 获取默认转储文件名
	const char *getDefaultDumpFileName (void) const;

public:
	//! 设置是否允许当发生程序异常时弹出错误提示消息框
	//! \param enable true允许当程序发生异常时弹出错误提示消息框, false则不允许弹出消息框
	static void enableErrorPopup (bool enable);

	//! 查询当前是否允许当发生程序异常时弹出错误提示消息框
	//! \return true允许 false不允许
	static bool isErrorPopupEnabled (void);

	//! 
	static bool isAppThread (void);

public:
	//! 开始主循环
	//! 这个函数会在主循环结束以后退出
	//! \return 退出码
	virtual int run (void);

protected:
	virtual void onPushEvent (void);
	virtual void onReportError (ATOM_AppErrorHandler::ErrorType errorType);

private:
	void _eventloop (void);

protected:
	int _returnCode;
	int _frameInterval;
	bool _timerChanged;
	HANDLE _timerHandle;
	HANDLE _eventHandle;
	HWND _mainWindow;
	ATOM_FrameStamp _frameStamp;
	ATOM_STRING _dmpFileName;
	static bool _enableErrorPopup;

	ATOM_DECLARE_EVENT_MAP(ATOM_Application, ATOM_EventDispatcher)
};

#endif // __ATOM_KERNEL_APPLICATION_H

/*! @} */
