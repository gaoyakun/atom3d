/**	\file event.h
 *	�¼������ͽṹ���Լ��������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_EVENT_H
#define __ATOM_KERNEL_EVENT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <typeinfo>

#if !defined(NDEBUG)
#define ATOM_DECLARE_EVENT(EventTypeName) \
  public: \
    static int eventTypeId (void) { \
      static ATOM_EventTypeIdGenerator idgen(typeid(EventTypeName).name()); \
      return idgen.id; \
    } \
    virtual int getEventTypeId (void) const { \
      return EventTypeName::eventTypeId (); \
    } \
	virtual const char *getEventName (void) const { \
	  return #EventTypeName; \
	}
#else
//! ��������һ���¼�����
#define ATOM_DECLARE_EVENT(EventTypeName) \
  public: \
    static int eventTypeId (void) { \
      static ATOM_EventTypeIdGenerator idgen(typeid(EventTypeName).name()); \
      return idgen.id; \
    } \
    virtual int getEventTypeId (void) const { \
      return EventTypeName::eventTypeId (); \
    }
#endif

//! ��������һ����֧���¼�����(����̳���һ��֧���¼��ĸ���).
//! ������Ϊ����������½ӿڣ�
//! virtual void handleEvent (ATOM_Event *); // ��������һ����Ϣ
//! virtual void queueEvent (ATOM_Event*, ATOM_EventDispatcher*); // ��һ����Ϣ������еȵ���һ��Ӧ�ó��������Ϣ�ַ�ʱ����
//! virtual bool callParentHandler (ATOM_Event *); // ���ø�����¼�����
//! ����������½ӿ�(�Ӹ���̳ж���)
//! virtual void setEventTrigger (ATOM_EventTrigger*); // �����¼��ص�
//! virtual ATOM_EventTrigger * getEventTrigger (void) const; // �����¼��ص�
#if !defined(NDEBUG)
#define ATOM_DECLARE_EVENT_MAP(ClassName, SuperClassName) \
public: \
  virtual void handleEventImpl (ATOM_Event *event); \
  virtual void queueEvent (ATOM_Event *event, ATOM_EventDispatcher *dispatcher); \
  virtual bool preHandleEvent (ATOM_Event *event); \
  virtual void postHandleEvent (ATOM_Event *event); \
  virtual const char *getTypeName (void) const { return #ClassName; } \
public: \
  bool callParentHandler (ATOM_Event *event) { return ClassName::getEventHandlerMap()->callParentHandler (this, event); } \
private: \
  static const ATOM_EventHandlerEntry<ClassName> _handlerEntries[]; \
  static void __cdecl handlerProc (void *p, ATOM_Event *event); \
protected: \
  static const ATOM_EventHandlerMap<ClassName> _handlerMap; \
  virtual const ATOM_BaseEventHandlerMap *getEventHandlerMap (void) const;
#else
#define ATOM_DECLARE_EVENT_MAP(ClassName, SuperClassName) \
public: \
  virtual void handleEventImpl (ATOM_Event *event); \
  virtual void queueEvent (ATOM_Event *event, ATOM_EventDispatcher *dispatcher); \
  virtual bool preHandleEvent (ATOM_Event *event); \
  virtual void postHandleEvent (ATOM_Event *event); \
public: \
  bool callParentHandler (ATOM_Event *event) { return ClassName::getEventHandlerMap()->callParentHandler (this, event); } \
private: \
  static const ATOM_EventHandlerEntry<ClassName> _handlerEntries[]; \
  static void __cdecl handlerProc (void *p, ATOM_Event *event); \
protected: \
  static const ATOM_EventHandlerMap<ClassName> _handlerMap; \
  virtual const ATOM_BaseEventHandlerMap *getEventHandlerMap (void) const;
#endif

//! MACRO ��������һ����֧���¼�����(����Ϊ������಻֧���¼�����).
//! ������Ϊ����������½ӿڣ�
//! virtual void handleEvent (ATOM_Event *); // ��������һ����Ϣ
//! virtual void queueEvent (ATOM_Event *, ATOM_EventDispatcher *); // ��һ����Ϣ������еȵ���һ��Ӧ�ó��������Ϣ�ַ�ʱ����
//! virtual bool callParentHandler (ATOM_Event *); // ���ø�����¼�����
//! virtual void setEventTrigger (ATOM_EventTrigger*); // �����¼��ص�
//! virtual ATOM_EventTrigger *getEventTrigger (void) const; // �����¼��ص�
#if !defined(NDEBUG)
#define ATOM_DECLARE_EVENT_MAP_NOPARENT(ClassName) \
public: \
  virtual void handleEvent (ATOM_Event *event); \
  virtual void handleEventEx (ATOM_Event *event, bool useTrigger); \
  virtual void handleEventImpl (ATOM_Event *event); \
  virtual void queueEvent (ATOM_Event *event, ATOM_EventDispatcher *dispatcher); \
  virtual void setEventTrigger (ATOM_EventTrigger *trigger); \
  virtual ATOM_EventTrigger *getEventTrigger (void) const; \
  virtual bool supportEvent (int id) const; \
  virtual bool preHandleEvent (ATOM_Event *event); \
  virtual void postHandleEvent (ATOM_Event *event); \
  virtual const char *getTypeName (void) const { return #ClassName; } \
public: \
  bool callParentHandler (ATOM_Event *event) { return ClassName::getEventHandlerMap()->callParentHandler (this, event); } \
protected: \
  ATOM_EventTriggerContainer _eventTrigger; \
private: \
  static const ATOM_EventHandlerEntry<ClassName> _handlerEntries[]; \
  static void __cdecl handlerProc (void *p, ATOM_Event *event); \
protected: \
  ATOM_AutoUnregisterHandler _unregisterHandler; \
protected: \
  static const ATOM_EventHandlerMap<ClassName> _handlerMap; \
  virtual const ATOM_BaseEventHandlerMap *getEventHandlerMap (void) const;
#else
#define ATOM_DECLARE_EVENT_MAP_NOPARENT(ClassName) \
public: \
  virtual void handleEvent (ATOM_Event *event); \
  virtual void handleEventEx (ATOM_Event *event, bool useTrigger); \
  virtual void handleEventImpl (ATOM_Event *event); \
  virtual void queueEvent (ATOM_Event *event, ATOM_EventDispatcher *dispatcher); \
  virtual void setEventTrigger (ATOM_EventTrigger *trigger); \
  virtual ATOM_EventTrigger *getEventTrigger (void) const; \
  virtual bool supportEvent (int id) const; \
  virtual bool preHandleEvent (ATOM_Event *event); \
  virtual void postHandleEvent (ATOM_Event *event); \
public: \
  bool callParentHandler (ATOM_Event *event) { return ClassName::getEventHandlerMap()->callParentHandler (this, event); } \
protected: \
  ATOM_EventTriggerContainer _eventTrigger; \
private: \
  static const ATOM_EventHandlerEntry<ClassName> _handlerEntries[]; \
  static void __cdecl handlerProc (void *p, ATOM_Event *event); \
protected: \
  ATOM_AutoUnregisterHandler _unregisterHandler; \
protected: \
  static const ATOM_EventHandlerMap<ClassName> _handlerMap; \
  virtual const ATOM_BaseEventHandlerMap *getEventHandlerMap (void) const;
#endif

//! �����¼�����ӿ�.
//! ����겻����preHandleEvent��postHandleEvent�ӿ�
#define ATOM_BEGIN_EVENT_MAP_NOFILTER(ClassName, SuperClassName) \
  const ATOM_BaseEventHandlerMap *ClassName::getEventHandlerMap (void) const { return &ClassName::_handlerMap; } \
  const ATOM_EventHandlerMap<ClassName> ClassName::_handlerMap(&SuperClassName::_handlerMap, ClassName::_handlerEntries, (int)((SuperClassName*)0)); \
  void __cdecl ClassName::handlerProc (void *p, ATOM_Event *event) { \
	if (p) ((ClassName*)p)->handleEventEx (event, false); \
  } \
  void ClassName::handleEventImpl (ATOM_Event *event) { \
	getEventHandlerMap()->handleEvent (this, event); \
  } \
  void ClassName::queueEvent (ATOM_Event *event, ATOM_EventDispatcher *dispatcher) { \
    _unregisterHandler.registerHandler (dispatcher, this); \
    dispatcher->pushEvent (event, this, &ClassName::handlerProc, getEventHandlerMap(), &_eventTrigger.trigger); \
  } \
  const ATOM_EventHandlerEntry<ClassName> ClassName::_handlerEntries[] = {

//! �����¼�����ӿ�.
//! ����궨��preHandleEvent��postHandleEvent�ӿ�
#define ATOM_BEGIN_EVENT_MAP(ClassName, SuperClassName) \
  bool ClassName::preHandleEvent (ATOM_Event *event) { return SuperClassName::preHandleEvent (event); } \
  void ClassName::postHandleEvent (ATOM_Event *event) { SuperClassName::postHandleEvent (event); } \
  ATOM_BEGIN_EVENT_MAP_NOFILTER(ClassName, SuperClassName)

//! �����¼�����ӿ�.
//! ����겻����preHandleEvent��postHandleEvent�ӿڣ�����û�и������
#if !defined(NDEBUG)
#define ATOM_BEGIN_EVENT_MAP_NOPARENT_NOFILTER(ClassName) \
  const ATOM_BaseEventHandlerMap *ClassName::getEventHandlerMap (void) const { return &ClassName::_handlerMap; } \
  const ATOM_EventHandlerMap<ClassName> ClassName::_handlerMap(0, ClassName::_handlerEntries, 0); \
  const char *className__##ClassName = 0; \
  void __cdecl ClassName::handlerProc (void *p, ATOM_Event *event) { \
	if (p) ((ClassName*)p)->handleEventEx (event, false); \
  } \
  void ClassName::handleEvent (ATOM_Event *event) { \
	handleEventEx (event, true); \
  } \
  void ClassName::handleEventEx (ATOM_Event *event, bool useTrigger) { \
	className__##ClassName = getTypeName (); \
    if (preHandleEvent (event)) \
	{ \
		if (useTrigger && _eventTrigger.trigger) \
		{ \
		    void *oldHost = _eventTrigger.trigger->getHost ();\
			ATOM_EventTrigger::HandlerProc oldHandlerProc = _eventTrigger.trigger->getHandlerProc();\
			_eventTrigger.trigger->setHost (this); \
			_eventTrigger.trigger->setHandlerProc (&ClassName::handlerProc); \
			_eventTrigger.trigger->handleEvent(event); \
			if (event->isCallerAlive ()) \
			{\
			_eventTrigger.trigger->setHost (oldHost); \
			_eventTrigger.trigger->setHandlerProc (oldHandlerProc); \
			}\
		} \
		else \
		{ \
			handleEventImpl (event); \
			if (event->isCallerAlive ()) \
			{ \
				postHandleEvent (event); \
			} \
		} \
	} \
  } \
  void ClassName::handleEventImpl (ATOM_Event *event) { \
	getEventHandlerMap()->handleEvent (this, event); \
  } \
  void ClassName::queueEvent (ATOM_Event *event, ATOM_EventDispatcher *dispatcher) { \
    _unregisterHandler.registerHandler (dispatcher, this); \
    dispatcher->pushEvent (event, this, &ClassName::handlerProc, getEventHandlerMap(), &_eventTrigger.trigger); \
  } \
  void ClassName::setEventTrigger (ATOM_EventTrigger *trigger) { _eventTrigger.trigger = trigger; } \
  ATOM_EventTrigger *ClassName::getEventTrigger (void) const { return _eventTrigger.trigger; } \
  bool ClassName::supportEvent (int id) const { \
	bool b = getEventHandlerMap()->supportEvent (id); \
	if (!b) { \
	  ATOM_EventTrigger *trigger = getEventTrigger (); \
	  if (trigger) b = trigger->supportEvent (id); \
	} \
	return b; \
  } \
  const ATOM_EventHandlerEntry<ClassName> ClassName::_handlerEntries[] = {
#else
#define ATOM_BEGIN_EVENT_MAP_NOPARENT_NOFILTER(ClassName) \
  const ATOM_BaseEventHandlerMap *ClassName::getEventHandlerMap (void) const { return &ClassName::_handlerMap; } \
  const ATOM_EventHandlerMap<ClassName> ClassName::_handlerMap(0, ClassName::_handlerEntries, 0); \
  void __cdecl ClassName::handlerProc (void *p, ATOM_Event *event) { \
	if (p) ((ClassName*)p)->handleEventEx (event, false); \
  } \
  void ClassName::handleEvent (ATOM_Event *event) { \
	handleEventEx (event, true); \
  } \
  void ClassName::handleEventEx (ATOM_Event *event, bool useTrigger) { \
    if (preHandleEvent (event)) \
	{ \
		if (useTrigger && _eventTrigger.trigger) \
		{ \
			void *oldHost = _eventTrigger.trigger->getHost (); \
			ATOM_EventTrigger::HandlerProc oldHandlerProc = _eventTrigger.trigger->getHandlerProc();\
			_eventTrigger.trigger->setHost (this); \
			_eventTrigger.trigger->setHandlerProc (&ClassName::handlerProc); \
			_eventTrigger.trigger->handleEvent(event); \
			if (event->isCallerAlive ()) \
			{\
			_eventTrigger.trigger->setHost (oldHost); \
			_eventTrigger.trigger->setHandlerProc (oldHandlerProc); \
			} \
		} \
		else \
		{ \
			handleEventImpl (event); \
			if (event->isCallerAlive ()) \
			{ \
				postHandleEvent (event); \
			} \
		} \
	} \
  } \
  void ClassName::handleEventImpl (ATOM_Event *event) { \
	getEventHandlerMap()->handleEvent (this, event); \
  } \
  void ClassName::queueEvent (ATOM_Event *event, ATOM_EventDispatcher *dispatcher) { \
    _unregisterHandler.registerHandler (dispatcher, this); \
    dispatcher->pushEvent (event, this, &ClassName::handlerProc, getEventHandlerMap(), &_eventTrigger.trigger); \
  } \
  void ClassName::setEventTrigger (ATOM_EventTrigger *trigger) { _eventTrigger.trigger = trigger; } \
  ATOM_EventTrigger *ClassName::getEventTrigger (void) const { return _eventTrigger.trigger; } \
  bool ClassName::supportEvent (int id) const { \
	bool b = getEventHandlerMap()->supportEvent (id); \
	if (!b) { \
	  ATOM_EventTrigger *trigger = getEventTrigger (); \
	  if (trigger) b = trigger->supportEvent (id); \
	} \
	return b; \
  } \
  const ATOM_EventHandlerEntry<ClassName> ClassName::_handlerEntries[] = {
#endif

//! �����¼�����ӿ�.
//! ����궨��preHandleEvent��postHandleEvent�ӿڣ�����û�и������
#define ATOM_BEGIN_EVENT_MAP_NOPARENT(ClassName) \
  bool ClassName::preHandleEvent (ATOM_Event *event) { return true; } \
  void ClassName::postHandleEvent (ATOM_Event *event) {} \
  ATOM_BEGIN_EVENT_MAP_NOPARENT_NOFILTER(ClassName)

//! �����¼�����ӿں���.
#define ATOM_EVENT_HANDLER(ClassName, eventType, proc) \
  { eventType::eventTypeId(), (void (ClassName::*)(ATOM_Event*))&ClassName::proc },

//! ����ӿں������ڴ�����������δ�����¼�.
#define ATOM_UNHANDLED_EVENT_HANDLER(ClassName, proc) \
  { -1, (void (ClassName::*)(ATOM_Event*))&ClassName::proc },

//! ���������¼�����ӿ�
//! \sa ATOM_BEGIN_EVENT_MAP
//! \sa ATOM_BEGIN_EVENT_MAP_NOFILTER
//! \sa ATOM_BEGIN_EVENT_MAP_NOPARENT_NOFILTER
//! \sa ATOM_BEGIN_EVENT_MAP_NOPARENT
#define ATOM_END_EVENT_MAP \
  { -1, 0 } };

class ATOM_KERNEL_API ATOM_EventTypeIdGenerator
{
public:
  ATOM_EventTypeIdGenerator (const char *EventTypeName);
  int id;
};

//! �¼�����.
//! �����û��Զ����¼�����Ҫ�����������
class ATOM_KERNEL_API ATOM_Event
{
	friend class ATOM_EventTypeIdGenerator;

public:
	//! ���캯��
	ATOM_Event (void);

	//! ��������
	virtual ~ATOM_Event (void);

public:
	//! �õ��Լ�����������
	//! \return �Լ�����������
	virtual int getEventTypeId (void) const = 0;

#if !defined(NDEBUG)
	//! �õ��Լ����������ƣ�������
	//! \return ��������
	virtual const char *getEventName (void) const = 0;
#endif

protected:
	static int genTypeId (const char *typeName);

public:
	//! ��ѯ�Ƿ��Ѿ�������
	bool wasHandled (void) const { return _wasHandled; }

	//! ����Ϊ�Ѵ���
	void markHandled (void) { _wasHandled = true; }

	//! ��ѯ�¼������߶����Ƿ��Ѿ���ɾ��
	bool isCallerAlive (void) const { return _callerAlive; }

	//! �����¼������߶����Ƿ�ɾ��
	void setCallerAlive (bool alive) { _callerAlive = alive; }

private:
	bool _wasHandled;
	bool _callerAlive;
};

class ATOM_KERNEL_API ATOM_EventTrigger;
class ATOM_KERNEL_API ATOM_EventDispatcher;

struct ATOM_EventTriggerContainer
{
  ATOM_EventTriggerContainer (void): trigger(0) {}
  ATOM_EventTrigger *trigger;
};

template <class T>
struct ATOM_EventHandlerEntry
{
  int eventId;
  void (T::*handler)(ATOM_Event*);
};

struct ATOM_BaseEventHandlerMap
{
  virtual void handleEvent (void *c, ATOM_Event *event) const = 0;
  virtual bool supportEvent (int id) const = 0;
  virtual bool callParentHandler (void *c, ATOM_Event *event) const = 0;
};

struct ATOM_KERNEL_API ATOM_AutoUnregisterHandler
{
  struct Entry
  {
	  ATOM_EventDispatcher *dispatcher;
	  void *handler;
  };

  ATOM_VECTOR<Entry> entries;

  ~ATOM_AutoUnregisterHandler (void);
  void registerHandler (ATOM_EventDispatcher *dispatcher, void *p);
};

template <class T>
struct ATOM_EventHandlerMap: public ATOM_BaseEventHandlerMap
{
  ATOM_EventHandlerMap(const ATOM_BaseEventHandlerMap *bm, const ATOM_EventHandlerEntry<T> *m, int o):baseMap(bm), theMap(m), pOffset(o) {}
  const ATOM_BaseEventHandlerMap *baseMap;
  const ATOM_EventHandlerEntry<T> *theMap;
  int pOffset;
  virtual bool callParentHandler (void *c, ATOM_Event *e) const {
    if (baseMap)
    {
      char *p = (char*)c;
      baseMap->handleEvent (p + pOffset, e);
	  return e->wasHandled();
    }
    return false;
  }
  virtual bool supportEvent (int id) const {
    const ATOM_EventHandlerEntry<T> *entry = theMap;
    for (; entry->handler; ++entry)
    {
      if (entry->eventId == -1 || entry->eventId == id)
      {
	    return true;
      }
    }
	return baseMap ? baseMap->supportEvent(id) : false;
  }
  virtual void handleEvent (void *c, ATOM_Event *e) const {
    T *t = (T*)c;
    const ATOM_EventHandlerEntry<T> *entry = theMap;
    const ATOM_EventHandlerEntry<T> *unhandled = 0;
    for (; entry->handler; ++entry)
    {
      if (entry->eventId == -1)
      {
        unhandled = entry;
      }
      else if (entry->eventId == e->getEventTypeId())
      {
        (t->*(entry->handler))(e);
		e->markHandled ();
		return;
      }
    }

    callParentHandler (c, e);

	if (!e->wasHandled () && unhandled)
    {
      (t->*(unhandled->handler))(e);
	  e->markHandled ();
    }
  }
};

//! �¼������߶���
class ATOM_KERNEL_API ATOM_EventTrigger
{
public:
	typedef void (__cdecl *HandlerProc)(void*, ATOM_Event*);
	//! ���캯��
	ATOM_EventTrigger (void);

public:
	void setHost (void *host);
	void *getHost (void) const;
	void setAutoCallHost (bool b);
	bool getAutoCallHost (void) const;
	void setHandlerProc (HandlerProc proc);
	HandlerProc getHandlerProc (void) const;
	void callHost (ATOM_Event *event);

protected:
	void *_host;
	bool _autoCallHost;
	HandlerProc _handlerProc;

	ATOM_DECLARE_EVENT_MAP_NOPARENT(ATOM_EventTrigger)
};

#if !defined(NDEBUG)
DWORD ATOM_KERNEL_API ATOM_EventExceptionFilter(const char *typeName, const char *eventName);
#endif

#endif // __ATOM_KERNEL_EVENT_H
/*! @} */
