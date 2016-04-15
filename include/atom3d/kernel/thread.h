/**	\file kernel/thread.h
 *	�̻߳��������.
 *
 *	\author ������
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

//! �̳߳�ʼ���¼�.
//! ��һ���߳̿�ʼ���к�����յ�����¼�
class ATOM_KERNEL_API ATOM_ThreadInitEvent: public ATOM_Event
{
public:
  ATOM_ThreadInitEvent (void): success(true), errorcode(0) {}
  bool success;
  int errorcode;

  ATOM_DECLARE_EVENT(ATOM_ThreadInitEvent)
};

//! �߳��˳��¼�.
//! ��һ���߳�׼���˳�ǰ���յ�����¼�
class ATOM_KERNEL_API ATOM_ThreadExitEvent: public ATOM_Event
{
  ATOM_DECLARE_EVENT(ATOM_ThreadExitEvent)
};

//! Ͷ�ݴ��¼����߳������߳��˳�.
class ATOM_KERNEL_API ATOM_ThreadQuitEvent: public ATOM_Event
{
public:
  ATOM_ThreadQuitEvent (void): returnValue(0) {}
  ATOM_ThreadQuitEvent (int returnValue_): returnValue(returnValue_) {}
  int returnValue;

  ATOM_DECLARE_EVENT(ATOM_ThreadQuitEvent)
};

//! �߳���ѭ���¼�.
//! �߳�����ѭ���л᲻���յ�����¼�
class ATOM_KERNEL_API ATOM_ThreadIdleEvent: public ATOM_Event
{
  ATOM_DECLARE_EVENT(ATOM_ThreadIdleEvent)
};

//! \class ATOM_SoundInfo
//! �̻߳���.
//! ��װ��һ���̣߳���ʹ�ñ��ഴ��������һ���߳�
//! \author ������
class ATOM_KERNEL_API ATOM_Thread: public ATOM_EventDispatcher
{
public:
	//! ���캯��
	ATOM_Thread (void);

	//! ��������
	virtual ~ATOM_Thread (void);

public:
	//! ������ѭ��֡���.
	//! \param ms ���ʱ�䣬��λΪ����
	void setFrameInterval (int ms);

	//! ��ȡ��ѭ��֡���.
	//! \return ���ʱ�䣬��λΪ����
	int getFrameInterval (void);

public:
	void onThreadInit (ATOM_ThreadInitEvent *event);
	void onThreadExit (ATOM_ThreadExitEvent *event);
	void onThreadQuit (ATOM_ThreadQuitEvent *event);

public:
	//! ��ȡ��ǰ֡��ʱ���
	//! \return ʱ���
	const ATOM_FrameStamp &getFrameStamp (void);

	//! ����Ͷ�ݵ��̵߳��¼�
	//! \param �������true��ʱ�򱣴��˳���
	//! \return true �߳�Ҫ���˳� false �߳�δҪ���˳�
	bool pumpEvents (int *retVal);

	//! Ͷ��ATOM_ThreadQuitEvent���߳������߳��˳�
	//! \param ret �˳���
	void postQuitEvent (int ret);

	//! ��ѯ�߳��Ƿ�������״̬
	//! \return true ���� false δ����
	bool isRunning (void) const;

	//! �ȴ��߳̽���
	//! \param timeout �ȴ�ʱ��
	//! \param retval �������true�����˳���
	//! \return true �߳����˳� false �ȴ���ʱ
	bool wait (unsigned timeout, int *retval);

	//! ǿ�ƽ����߳�.
	//! ��������ȴ�timeoutʱ�������߳���δ�������ǿ�ƽ����̣߳�����������̲߳����յ�ATOM_ThreadExitEvent
	//! \param timeout �ȴ�ʱ��
	//! \param retval �߳��˳���
	void terminate (unsigned timeout, int retval);

public:
	//! ��ʼ���и��߳�.
	//! �̶߳���������Ժ�δ��������״̬����Ҫ���ô˺������߳̿�ʼ����
	//! \return ���Ƿ���0
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
