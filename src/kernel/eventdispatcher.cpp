#include "stdafx.h"
#include "eventdispatcher.h"



ATOM_BEGIN_EVENT_MAP_NOPARENT(ATOM_EventDispatcher)
	ATOM_EVENT_HANDLER(ATOM_EventDispatcher, ATOM_AsyncTaskEvent, onAsyncTask)
ATOM_END_EVENT_MAP

ATOM_EventDispatcher::ATOM_EventDispatcher (void)
{
	_quitRequest = false;
}

ATOM_EventDispatcher::~ATOM_EventDispatcher (void)
{
  _eventQueue.clear ();
}

void ATOM_EventDispatcher::registerEventHandler (void *handler)
{
  ATOM_Mutex::ScopeMutex lock(_handlerSetLock);

  _handlerSet.insert (handler);
}

void ATOM_EventDispatcher::unregisterEventHandler (void *handler)
{
  ATOM_Mutex::ScopeMutex lock(_handlerSetLock);

  std::pair<HandlerSetIter, HandlerSetIter> range = _handlerSet.equal_range (handler);
  if (range.first != range.second)
  {
    _handlerSet.erase (range.first, range.second);
  }
}

bool ATOM_EventDispatcher::dispatchPendingEvents (void)
{
  for (;;)
  {
    ATOM_EventQueue::EventDispatchInfo info;
    if (!peekEvent (&info, true))
    {
      return false;
    }

    if (info.handler && info.handlerMap)
    {
	  // accquire the lock to prevent the handler been destroyed while the handler is involving.
      ATOM_Mutex::ScopeMutex lock(_handlerSetLock);
      
      HandlerSetIter it = _handlerSet.find (info.handler);
      if (it != _handlerSet.end ())
      {
        _handlerSet.erase (it);

        ATOM_EventTrigger *trigger = *info.trigger;

        if (trigger)
        {
          trigger->setHost (info.handler);
          trigger->setHandlerProc (info.handlerProc);
          trigger->handleEvent (info.event);
        }
        else
        {
          info.handlerMap->handleEvent(info.handler, info.event);
        }
      }
    }
	else
	{
        handleEvent (info.event);
	}

    ATOM_DELETE(info.event);
  }
}

bool ATOM_EventDispatcher::peekEvent (ATOM_EventQueue::EventDispatchInfo *info, bool remove)
{
  return _eventQueue.peek (info, remove);
}

void ATOM_EventDispatcher::pushEvent (ATOM_Event *event, void *handler, void (__cdecl *handlerProc)(void*, ATOM_Event*), const ATOM_BaseEventHandlerMap *handlerMap, ATOM_EventTrigger **trigger)
{
  if (event)
  {
    _eventQueue.inject (event, handler, handlerProc, handlerMap, trigger);

	onPushEvent ();
  }
}

void ATOM_EventDispatcher::onPushEvent (void)
{
}

void ATOM_EventDispatcher::onAsyncTask (ATOM_AsyncTaskEvent *event)
{
	if (event->task)
	{
		event->task->lock ();

		if (!event->task->isCanceled())
		{
			event->task->execute (event->param);
		}
		event->task->unlock ();
	}
}



