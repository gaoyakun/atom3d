/**	\file application.h
 *	Application�������.
 *
 *	\author ������
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

//! Ӧ�ó����ʼ���¼����ڸս�����ѭ��ʱ����.
//! ���¼���Ӧ����false�ᵼ��Ӧ�ó����˳�,�˳�����errorcode����
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

//! Ӧ�ó����˳��¼������˳���ѭ��ǰ����.
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_AppExitEvent: public ATOM_Event
{
  ATOM_DECLARE_EVENT(ATOM_AppExitEvent)
};

//! ��ATOM_ApplicationͶ�ݴ��¼���Ҫ��Ӧ�ó����˳���ѭ��������ֵ��returnValue����.
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_AppQuitEvent: public ATOM_Event
{
public:
  ATOM_AppQuitEvent (void): returnValue(0) {}
  ATOM_AppQuitEvent (int returnValue_): returnValue(returnValue_) {}
  int returnValue;

  ATOM_DECLARE_EVENT(ATOM_AppQuitEvent)
};

//! Ӧ�ó��򼤻��ת���̨�¼�.
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_AppActiveEvent: public ATOM_Event
{
public:
  ATOM_AppActiveEvent (void): active(true) {}
  ATOM_AppActiveEvent (bool active_): active(active_) {}
  bool active;

  ATOM_DECLARE_EVENT(ATOM_AppActiveEvent)
};

//! Fatal�¼�������������ʱ������������ʱ���Է��ʹ��¼���APP������APP��������.
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

//! Idle�¼�������Ӧ�ó������ʱ��������.
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_AppIdleEvent: public ATOM_Event
{
  ATOM_DECLARE_EVENT(ATOM_AppIdleEvent)
};

//! ��ص��������¼���Ŀǰ��δʹ�ã�.
class ATOM_KERNEL_API ATOM_AppBatteryLowEvent: public ATOM_Event
{
  ATOM_DECLARE_EVENT(ATOM_AppBatteryLowEvent)
};

//! Windows��Ϣ�¼�.
//! ���¼���Ӧ��������true��ʾ�Ѿ���ȫ������¼�����Ĭ�ϴ���
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

//! ȫ�ֺ��������ڻ�ȡ��ǰ��Ӧ�ó���ʵ��ָ��.
//! \return Ӧ�ó���ʵ��ָ��
ATOM_KERNEL_API ATOM_Application * ATOM_CALL ATOM_GetApplication (void);

//! \class ATOM_Application
//! ATOMӦ�ó�����.
//! �Ƽ�ʹ�ô�����ΪATOM����Ӧ�ó�����.
//! \author ������
class ATOM_KERNEL_API ATOM_Application: public ATOM_EventDispatcher
{
public:
	class ErrorHandler: public ATOM_AppErrorHandler
	{
	public:
		virtual void _reportError (PEXCEPTION_POINTERS p, ATOM_AppErrorHandler::ErrorType errorType);
	};

public:
	//! ���캯��.
	ATOM_Application (void);

	//! ��������.
	virtual ~ATOM_Application (void);

public:
	//! ������Ϸѭ�����ʱ��.
	//! \param ms ���ʱ�䣬��λ���롣���Ϊ�����ʾ����ȫ��ѭ����
	void setFrameInterval (int ms);

	//! ��ȡ��Ϸѭ�����ʱ��.
	//! \return ���ʱ�䣬��λ���롣
	int getFrameInterval (void);

public:
	void onAppInit (ATOM_AppInitEvent *event);
	void onAppExit (ATOM_AppExitEvent *event);
	void onAppQuit (ATOM_AppQuitEvent *event);
	void onAppFatal (ATOM_AppFatalEvent *event);

public:
	//! ��ȡ��ǰ֡��ʱ���.
	//! \return ʱ���
	//! \sa ATOM_FrameStamp
	const ATOM_FrameStamp &getFrameStamp (void);

	//! ����ʱ�������ٶȵı�������.
	//! �������Խ��getFrameStamp�������ص�ʱ�����ŵ�Խ��
	//! \param timescale ��������
	void setTimeScale (float timescale);

	//! ��ȡʱ�������ٶȵı�������.
	//! \return ��������
	float getTimeScale (void) const;

	//! ����ǰ�¼�����������δ������¼�.
	//! \param retVal �������true, ���ָ��ָ���˳���
	//! \return true��ʾ�õ���Ӧ�ó����˳������� ��֮Ϊfalse
	bool pumpEvents (int *retVal);

	//! ����ϵͳ�����¼�
	//! �˷������ᴦ��Ӧ�ó����¼�
	//! \return true��ʾ�õ���Ӧ�ó����˳������� ��֮Ϊfalse
	bool pumpWMEvents (void);

	//! Ͷ��һ���˳�����.
	//! ��������˴˺�������ѭ����������һ��ѭ�������
	//! \param ret �˳���
	void postQuitEvent (int ret);

	//! ���������ھ��
	//! \param hWnd �����ھ��
	void setMainWindow (HWND hWnd);

	//! ��ȡ�����ھ��
	//! \return �����ھ��
	HWND getMainWindow (void) const;

	//! ����֡ʱ���
	void updateFrameStamp (void);

	//! ��Ϣѭ��
	void eventLoop (void);

	//! ����Ĭ��ת���ļ���
	void setDefaultDumpFileName (const char *dmpFileName);

	//! ��ȡĬ��ת���ļ���
	const char *getDefaultDumpFileName (void) const;

public:
	//! �����Ƿ��������������쳣ʱ����������ʾ��Ϣ��
	//! \param enable true�����������쳣ʱ����������ʾ��Ϣ��, false����������Ϣ��
	static void enableErrorPopup (bool enable);

	//! ��ѯ��ǰ�Ƿ��������������쳣ʱ����������ʾ��Ϣ��
	//! \return true���� false������
	static bool isErrorPopupEnabled (void);

	//! 
	static bool isAppThread (void);

public:
	//! ��ʼ��ѭ��
	//! �������������ѭ�������Ժ��˳�
	//! \return �˳���
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
