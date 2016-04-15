/**	\file eventdispatcher.h
 *	�¼��ַ�������
 *
 *	\author ������
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

//! �첽�������
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

//! �첽�¼�.
//! ��һ���߳̿��ܷ��ʹ��¼�Ҫ�����߳����һ������
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
//! �¼��ַ�������
//! ֧���¼��ķַ��ʹ���
//! \author ������
class ATOM_KERNEL_API ATOM_EventDispatcher
{
public:
	//! ���캯��
	ATOM_EventDispatcher (void);

	//! ��������
	virtual ~ATOM_EventDispatcher (void);

public:
	//! ע���¼��������
	void registerEventHandler (void *handler);

	//! ע���¼��������
	void unregisterEventHandler (void *handler);

	//! �ַ������е��¼�
	bool dispatchPendingEvents (void);

	//! �鿴�����е���һ���¼�.
	//! \param info �¼���Ϣ�ṹ
	//! \param remove true��ʾ�Ӷ�����ժ�����¼��������ʾ��ժ��
	//! \return true��ʾ�ɹ�����˶�������һ���¼�����Ϣ�������ʾ�¼������ѿ�
	bool peekEvent (ATOM_EventQueue::EventDispatchInfo *info, bool remove);

	//! Ͷ��һ���¼����¼�����.
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
