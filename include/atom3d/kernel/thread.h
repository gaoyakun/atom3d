/**	\file kernel/thread.h
 *	线程基类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_THREAD_H
#define __ATOM_KERNEL_THREAD_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#include "eventdispatcher.h"
#include "framestamp.h"

//! 线程初始化事件.
//! 当一个线程开始运行后会先收到这个事件
class ATOM_KERNEL_API ATOM_ThreadInitEvent: public ATOM_Event
{
public:
  ATOM_ThreadInitEvent (void): success(true), errorcode(0) {}
  bool success;
  int errorcode;

  ATOM_DECLARE_EVENT(ATOM_ThreadInitEvent)
};

//! 线程退出事件.
//! 当一个线程准备退出前会收到这个事件
class ATOM_KERNEL_API ATOM_ThreadExitEvent: public ATOM_Event
{
  ATOM_DECLARE_EVENT(ATOM_ThreadExitEvent)
};

//! 投递此事件给线程请求线程退出.
class ATOM_KERNEL_API ATOM_ThreadQuitEvent: public ATOM_Event
{
public:
  ATOM_ThreadQuitEvent (void): returnValue(0) {}
  ATOM_ThreadQuitEvent (int returnValue_): returnValue(returnValue_) {}
  int returnValue;

  ATOM_DECLARE_EVENT(ATOM_ThreadQuitEvent)
};

//! 线程主循环事件.
//! 线程在主循环中会不断收到这个事件
class ATOM_KERNEL_API ATOM_ThreadIdleEvent: public ATOM_Event
{
  ATOM_DECLARE_EVENT(ATOM_ThreadIdleEvent)
};

//! \class ATOM_SoundInfo
//! 线程基类.
//! 封装了一个线程，可使用本类创建并运行一个线程
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_Thread: public ATOM_EventDispatcher
{
public:
	//! 构造函数
	ATOM_Thread (void);

	//! 析构函数
	virtual ~ATOM_Thread (void);

public:
	//! 设置主循环帧间隔.
	//! \param ms 间隔时间，单位为毫秒
	void setFrameInterval (int ms);

	//! 获取主循环帧间隔.
	//! \return 间隔时间，单位为毫秒
	int getFrameInterval (void);

public:
	void onThreadInit (ATOM_ThreadInitEvent *event);
	void onThreadExit (ATOM_ThreadExitEvent *event);
	void onThreadQuit (ATOM_ThreadQuitEvent *event);

public:
	//! 获取当前帧的时间戳
	//! \return 时间戳
	const ATOM_FrameStamp &getFrameStamp (void);

	//! 处理投递到线程的事件
	//! \param 如果返回true的时候保存退出码
	//! \return true 线程要求退出 false 线程未要求退出
	bool pumpEvents (int *retVal);

	//! 投递ATOM_ThreadQuitEvent到线程请求线程退出
	//! \param ret 退出码
	void postQuitEvent (int ret);

	//! 查询线程是否处于运行状态
	//! \return true 运行 false 未运行
	bool isRunning (void) const;

	//! 等待线程结束
	//! \param timeout 等待时间
	//! \param retval 如果返回true保存退出码
	//! \return true 线程已退出 false 等待超时
	bool wait (unsigned timeout, int *retval);

	//! 强制结束线程.
	//! 这个方法等待timeout时间后如果线程尚未结束则会强制结束线程，这种情况下线程不会收到ATOM_ThreadExitEvent
	//! \param timeout 等待时间
	//! \param retval 线程退出码
	void terminate (unsigned timeout, int retval);

public:
	//! 开始运行该线程.
	//! 线程对象构造出来以后并未处于运行状态，需要调用此函数让线程开始运行
	//! \return 总是返回0
	virtual int run (void);

private:
	static unsigned __stdcall threadMain (void *arg);
	int eventLoop (void);

protected:
	virtual void onPushEvent (void);

protected:
	int _returnCode;
	int _frameInterval;
	bool _timerChanged;
	HANDLE _threadHandle;
	HANDLE _timerHandle;
	HANDLE _eventHandle;
	ATOM_FrameStamp _frameStamp;

	ATOM_DECLARE_EVENT_MAP(ATOM_Thread, ATOM_EventDispatcher)
};

#endif // __ATOM_KERNEL_THREAD_H
/*! @} */
