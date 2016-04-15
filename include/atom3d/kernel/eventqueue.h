/**	\file eventqueue.h
 *	事件队列类
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_EVENTQUEUE_H
#define __ATOM_KERNEL_EVENTQUEUE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "kernel.h"
#include "event.h"

struct ATOM_BaseEventHandlerMap;
class ATOM_EventTrigger;

class ATOM_EventQueue
{
public:
  struct EventDispatchInfo
  {
    ATOM_Event *event;
    void *handler;
    void (__cdecl *handlerProc)(void*, ATOM_Event*);
    const ATOM_BaseEventHandlerMap *handlerMap;
	ATOM_EventTrigger **trigger;
  };

public:
  ATOM_EventQueue (void);
  ~ATOM_EventQueue (void);

public:
  void inject (ATOM_Event *event, void *handler = 0, void (__cdecl *handlerProc)(void*, ATOM_Event*) = 0, const ATOM_BaseEventHandlerMap *handlerMap = 0, ATOM_EventTrigger **trigger = 0);
  bool peek (ATOM_EventQueue::EventDispatchInfo *info, bool remove);
  void clear (void);

private:
  ATOM_FastMutex _eventQueueLock;
  ATOM_DEQUE<ATOM_EventQueue::EventDispatchInfo> *_eventQueueReading;
  ATOM_DEQUE<ATOM_EventQueue::EventDispatchInfo> *_eventQueueWriting;
};

#endif // __ATOM_KERNEL_EVENTQUEUE_H
/*! @} */
