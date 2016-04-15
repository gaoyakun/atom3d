/**	\file eventdispatcher.h
 *	事件分发基类类
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_EVENTDISPATCHER_H
#define __ATOM_KERNEL_EVENTDISPATCHER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include "basedefs.h"
#include "event.h"
#include "eventqueue.h"

//! 异步任务基类
class ATOM_KERNEL_API ATOM_AsyncTask
{
public:
	ATOM_AsyncTask (void);

public:
	virtual void execute (void *param) = 0;
	virtual void cancel (void);
	virtual bool isCanceled (void) const;
	virtual void lock (void);
	virtual void unlock (void);

private:
	ATOM_Mutex _lock;
	bool _canceled;
};

//! 异步事件.
//! 另一个线程可能发送此事件要求主线程完成一件任务
class ATOM_KERNEL_API ATOM_AsyncTaskEvent: public ATOM_Event
{
public:
	ATOM_AsyncTaskEvent (void);
	ATOM_AsyncTaskEvent (ATOM_AsyncTask *_task, void *_param):task(_task), param(_param) {}

	ATOM_AsyncTask *task;
	void *param;

	ATOM_DECLARE_EVENT(ATOM_AsyncTaskEvent)
};

//! \class ATOM_EventDispatcher
//! 事件分发器基类
//! 支持事件的分发和传递
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_EventDispatcher
{
public:
	//! 构造函数
	ATOM_EventDispatcher (void);

	//! 析构函数
	virtual ~ATOM_EventDispatcher (void);

public:
	//! 注册事件处理过程
	void registerEventHandler (void *handler);

	//! 注销事件处理过程
	void unregisterEventHandler (void *handler);

	//! 分发队列中的事件
	bool dispatchPendingEvents (void);

	//! 查看队列中的下一个事件.
	//! \param info 事件信息结构
	//! \param remove true表示从队列中摘除此事件，否则表示不摘除
	//! \return true表示成功获得了队列中下一个事件的信息，否则表示事件队列已空
	bool peekEvent (ATOM_EventQueue::EventDispatchInfo *info, bool remove);

	//! 投递一个事件到事件队列.
	void pushEvent (ATOM_Event *event, void *handler, void (__cdecl *handlerProc)(void*, ATOM_Event*), const ATOM_BaseEventHandlerMap *handlerMap, ATOM_EventTrigger **trigger);

protected:
	virtual void onPushEvent (void);

protected:
	typedef ATOM_HASHMULTISET<void*> HandlerSet;
	typedef HandlerSet::iterator HandlerSetIter;
	typedef HandlerSet::const_iterator HandlerSetConstIter;
	HandlerSet _handlerSet;
	ATOM_Mutex _handlerSetLock;
	ATOM_EventQueue _eventQueue;
	bool _quitRequest;

public:
	void onAsyncTask (ATOM_AsyncTaskEvent *event);

	ATOM_DECLARE_EVENT_MAP_NOPARENT(ATOM_EventDispatcher)
};

#endif // __ATOM_KERNEL_EVENTDISPATCHER_H
/*! @} */
