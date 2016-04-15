#include "stdafx.h"
#include "eventqueue.h"
#include "event.h"

ATOM_EventQueue::ATOM_EventQueue (void)
{
  _eventQueueReading = ATOM_NEW(ATOM_DEQUE<ATOM_EventQueue::EventDispatchInfo>);
  _eventQueueWriting = ATOM_NEW(ATOM_DEQUE<ATOM_EventQueue::EventDispatchInfo>);
}

ATOM_EventQueue::~ATOM_EventQueue (void)
{
  clear ();

  ATOM_DELETE(_eventQueueReading);
  ATOM_DELETE(_eventQueueWriting);
}

void ATOM_EventQueue::inject (ATOM_Event *event, void *handler, void (__cdecl *handlerProc)(void*, ATOM_Event*), const ATOM_BaseEventHandlerMap *handlerMap, ATOM_EventTrigger **trigger)
{
  ATOM_EventQueue::EventDispatchInfo info;
  info.event = event;
  info.handler = handler;
  info.handlerProc = handlerProc;
  info.handlerMap = handlerMap;
  info.trigger = trigger;

  {
    ATOM_FastMutex::ScopeMutex lock(_eventQueueLock);

    _eventQueueWriting->push_front (info);
  }
}

bool ATOM_EventQueue::peek (ATOM_EventQueue::EventDispatchInfo *info, bool remove)
{
  if (_eventQueueReading->empty ())
  {
    ATOM_FastMutex::ScopeMutex lock(_eventQueueLock);
    std::swap (_eventQueueReading, _eventQueueWriting);
  }

  if (!_eventQueueReading->empty ())
  {
    if (info)
    {
      *info = _eventQueueReading->back ();
    }

    if (remove)
    {
      _eventQueueReading->pop_back ();
    }

    return true;
  }

  return false;
}

void ATOM_EventQueue::clear (void)
{
  // temporal prevent event injection
  ATOM_FastMutex::ScopeMutex lock(_eventQueueLock);

  ATOM_EventQueue::EventDispatchInfo info;

  for (;;)
  {
    if (peek (&info, true))
    {
      ATOM_DELETE(info.event);
    }
    else
    {
      return;
    }
  }
}

