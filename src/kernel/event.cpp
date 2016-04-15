#include "stdafx.h"
#include "event.h"
#include "eventdispatcher.h"

struct EventIdMap
{
  ATOM_HASHMAP<ATOM_STRING, int> _eventIdMap;
  ATOM_FastMutex _lock;

  ATOM_HASHMAP<ATOM_STRING, int> *lock (void)
  {
    _lock.lock ();
    return &_eventIdMap;
  }

  void unlock (void)
  {
    _lock.unlock ();
  }
};

static EventIdMap & getEventIdMap (void)
{
  static EventIdMap eventIdMap;
  return eventIdMap;
}

struct AutoEventIdMap
{
  ATOM_HASHMAP<ATOM_STRING, int> *idMap;

  AutoEventIdMap (void): 
    idMap (getEventIdMap().lock())
  {
  }

  ~AutoEventIdMap (void)
  {
    getEventIdMap().unlock ();
  }

  ATOM_HASHMAP<ATOM_STRING, int> * operator -> (void) const
  {
    return idMap;
  }

  ATOM_HASHMAP<ATOM_STRING, int> & operator * (void) const
  {
    return *idMap;
  }
};

static volatile long _eventId = 0;

ATOM_EventTypeIdGenerator::ATOM_EventTypeIdGenerator (const char *EventTypeName)
{
  id = ATOM_Event::genTypeId (EventTypeName);
}

ATOM_Event::ATOM_Event (void)
{
	_wasHandled = false;
	_callerAlive = true;
}

ATOM_Event::~ATOM_Event (void)
{
}

int ATOM_Event::genTypeId (const char *eventTypeName)
{
  AutoEventIdMap idMap;
  ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = idMap->find (eventTypeName);
  if (it == idMap->end ())
  {
    (*idMap)[eventTypeName] = _eventId;
    return _eventId++;
  }
  return it->second;
}

ATOM_AutoUnregisterHandler::~ATOM_AutoUnregisterHandler (void)
{
	for (unsigned i = 0; i < entries.size(); ++i)
	{
		entries[i].dispatcher->unregisterEventHandler (entries[i].handler);
	}
	entries.clear ();
}

void ATOM_AutoUnregisterHandler::registerHandler (ATOM_EventDispatcher *dispatcher, void *p)
{
	if (p && dispatcher)
	{
		dispatcher->registerEventHandler (p);

		Entry entry;
		entry.dispatcher = dispatcher;
		entry.handler = p;

		entries.push_back (entry);
	}
}

ATOM_BEGIN_EVENT_MAP_NOPARENT_NOFILTER(ATOM_EventTrigger)
ATOM_END_EVENT_MAP

ATOM_EventTrigger::ATOM_EventTrigger (void)
{
  _host = 0;
  _autoCallHost = true;
  _handlerProc = 0;
}

void ATOM_EventTrigger::setHost (void *host)
{
  _host = host;
}

void *ATOM_EventTrigger::getHost (void) const
{
  return _host;
}

void ATOM_EventTrigger::setAutoCallHost (bool b)
{
	_autoCallHost = b;
}

bool ATOM_EventTrigger::getAutoCallHost (void) const
{
	return _autoCallHost;
}

void ATOM_EventTrigger::setHandlerProc (HandlerProc proc)
{
  _handlerProc = proc;
}

ATOM_EventTrigger::HandlerProc ATOM_EventTrigger::getHandlerProc (void) const
{
	return _handlerProc;
}

void ATOM_EventTrigger::callHost (ATOM_Event *event)
{
	if(_handlerProc)
	{
		_handlerProc (_host, event);
	}
}

bool ATOM_EventTrigger::preHandleEvent (ATOM_Event *event)
{
	_autoCallHost = true;
	return true;
}

void ATOM_EventTrigger::postHandleEvent (ATOM_Event *event)
{
	if (_autoCallHost)
	{
		callHost (event);
	}
}

#if !defined(NDEBUG)
DWORD ATOM_KERNEL_API ATOM_EventExceptionFilter(const char *typeName, const char *eventName) 
{ 
	char buffer[512];
	sprintf (buffer, "<%s>: Exception occured while handling event %s!", typeName, eventName);
	ATOM_LOGGER::fatal (buffer);
	::MessageBox (ATOM_APP->getMainWindow(), buffer, "Error", MB_OK|MB_ICONHAND);
	ATOM_CallStack callstack;
	ATOM_ShowCallstackDialog (ATOM_APP->getMainWindow(), "CallStack", __FILE__, __LINE__, &callstack, buffer, 0);
	return EXCEPTION_CONTINUE_SEARCH;
} 
#endif
