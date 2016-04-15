#include "StdAfx.h"
#include "contentstream.h"
#include "loader.h"
#include "loadinterface.h"

ATOM_BEGIN_EVENT_MAP(ATOM_IOThread, ATOM_Thread)
	ATOM_EVENT_HANDLER(ATOM_IOThread, ATOM_ThreadIdleEvent, onIdle)
ATOM_END_EVENT_MAP

//#define DEBUG_MTLOAD

#ifdef DEBUG_MTLOAD
struct CallstackEntry
{
	void *entries[16];
	unsigned num;

	void *entries_d[16];
	unsigned num_d;
};

typedef USHORT (WINAPI *CaptureStackBackTraceFunc)(ULONG,ULONG,PVOID*,PULONG);
static HMODULE hNtDll = ::GetModuleHandle ("NTDLL.DLL");
static CaptureStackBackTraceFunc pfnCaptureStackBackTrace = (CaptureStackBackTraceFunc)::GetProcAddress(hNtDll, "RtlCaptureStackBackTrace");
static std::map<void*, CallstackEntry> requestEntries;
static ATOM_Mutex requestEntriesLock;
#endif

ATOM_LoadingRequest::ATOM_LoadingRequest (void)
{
	loadInterface = 0;
	stage = LOADINGSTAGE_UNKNOWN;
	flags = 0;
	userData = 0;
	callback = 0;
	groupId = -1;
}

ATOM_LoadingRequest::~ATOM_LoadingRequest (void)
{
#ifdef DEBUG_MTLOAD
	std::map<void*, CallstackEntry>::iterator it = requestEntries.find (this);

	if (it == requestEntries.end ())
	{
		__asm int 3
	}
	else
	{
		CallstackEntry &e = it->second;
		e.num_d = pfnCaptureStackBackTrace(0, 16, e.entries_d, 0);
	}
#endif
}

ATOM_RequestQueue::ATOM_RequestQueue (void)
{
    _queueR = &_queuePing;
    _queueW = &_queuePong;
}

void ATOM_RequestQueue::append (ATOM_LoadingRequest *val)
{
    ATOM_Mutex::ScopeMutex lock(_lock);

    _queueW->push_back (val);
}

ATOM_LoadingRequest *ATOM_RequestQueue::getAndRemove (void)
{
    if (_queueR->empty ())
    {
		ATOM_Mutex::ScopeMutex lock(_lock);
		std::swap (_queueW, _queueR);
    }   

	if (_queueR->empty ())
	{
		return 0;
	}

	for (ATOM_DEQUE<ATOM_LoadingRequest*>::iterator it = _queueR->begin(); it != _queueR->end(); ++it)
	{
		ATOM_LoadingRequest *val = *it;
		if ((val->flags & ATOM_LoadInterface::LF_HIGH_PRIORITY) != 0)
		{
			_queueR->erase (it);
			return val;
		}
	}

	ATOM_LoadingRequest *val = _queueR->front ();
	_queueR->pop_front ();
	return val;
}

ATOM_LoadingRequest *ATOM_RequestQueue::getAndRemove (ATOM_LoadInterface *loadInterface)
{
	for (unsigned n = 0; n < 2; ++n)
	{
		if (n == 1)
		{
			ATOM_Mutex::ScopeMutex lock(_lock);
			std::swap (_queueW, _queueR);
		}

		ATOM_DEQUE<ATOM_LoadingRequest*>::iterator it = _queueR->begin ();
		while (it != _queueR->end ())
		{
			ATOM_LoadInterface *i = (*it)->loadInterface;
			if (i == loadInterface)
			{
				ATOM_LoadingRequest *request = *it;
				_queueR->erase (it);
				return request;
			}
			++it;
		}
	}

	return getAndRemove ();
}

ATOM_LoadingRequest *ATOM_RequestQueue::discardRequest (ATOM_LoadInterface *loadInterface)
{
	for (unsigned n = 0; n < 2; ++n)
	{
		if (n == 1)
		{
			ATOM_Mutex::ScopeMutex lock(_lock);
			std::swap (_queueW, _queueR);
		}

		ATOM_DEQUE<ATOM_LoadingRequest*>::iterator it = _queueR->begin ();
		while (it != _queueR->end ())
		{
			ATOM_LoadInterface *i = (*it)->loadInterface;
			if (i == loadInterface)
			{
				ATOM_LoadingRequest *request = *it;
				request->discardStamp = -1L;
				return request;
			}
			++it;
		}
	}

	return 0;
}

static inline void setRequestStage (ATOM_LoadingRequest *request, ATOM_LoadingStage stage)
{
	//if (stage == LOADINGSTAGE_TOBEUNLOCK && request->loadInterface->getLoadingState() == ATOM_LoadInterface::LS_LOADFAILED)
	//{
	//	__asm int 3
	//}

#if ATOM3D_COMPILER_MSVC
	::InterlockedExchange ((volatile LONG*)&request->stage, (LONG)stage);
#else
	::InterlockedExchange ((LONG*)&request->stage, (LONG)stage);
#endif
}

static inline void setRequestInQueue (ATOM_LoadingRequest *request, long inqueue)
{
#if ATOM3D_COMPILER_MSVC
	::InterlockedExchange ((volatile LONG*)&request->inqueue, inqueue);
#else
	::InterlockedExchange ((LONG*)&request->inqueue, inqueue);
#endif
}

ATOM_IOThread::ATOM_IOThread (void)
{
	_creationThread = ATOM_NEW(ATOM_ContentCreationThread);
}

ATOM_IOThread::~ATOM_IOThread (void)
{
	ATOM_DELETE(_creationThread);
}

void ATOM_IOThread::appendRequest (ATOM_LoadingRequest *request)
{
	_requests.append (request);
}

void ATOM_IOThread::processRequests (void)
{
	_creationThread->processRequests ();
}

void ATOM_IOThread::discardRequest (ATOM_LoadInterface *loadInterface)
{
	if (!_requests.discardRequest (loadInterface))
	{
		_creationThread->discardRequest (loadInterface);
	}
}

int ATOM_IOThread::run (void)
{
	//--- wangjian added ---//
	// 异步加载 ： 禁止运行
	return 0;
	//----------------------//

	_creationThread->run ();

	return ATOM_Thread::run ();
}

#ifdef DEBUG_MTLOAD
static void printEntry (void *p)
{
	requestEntriesLock.lock ();

	std::map<void*, CallstackEntry>::iterator it = requestEntries.find (p);

	if (it == requestEntries.end ())
	{
		ATOM_LOGGER::Log ("Entry 0x%08X not found\n", p);
	}
	else
	{
		CallStack callstack(0, CallStack::SYM_BUILDPATH);
		ATOM_LOGGER::Log ("Begin log entry 0x%08X\n", p);
		ATOM_LOGGER::Log ("allocation callstack:\n");
		for (unsigned i = 0; i < it->second.num; ++i)
		{
            CallStack::StackEntry e;
            e.offset = it->second.entries[i];
            callstack.resolveSymbols (e);
			ATOM_LOGGER::Log ("  %s(%s@%d)\n", e.und_name, e.line_filename, e.line_number);
		}
		ATOM_LOGGER::Log ("deallocation callstack:\n");
		for (unsigned i = 0; i < it->second.num_d; ++i)
		{
            CallStack::StackEntry e;
            e.offset = it->second.entries_d[i];
            callstack.resolveSymbols (e);
			ATOM_LOGGER::Log ("  %s(%s@%d)\n", e.und_name, e.line_filename, e.line_number);
		}
	}
}
#endif

void ATOM_IOThread::onIdle (ATOM_ThreadIdleEvent *event)
{
	for (;;)
	{
		ATOM_LoadingRequest *request = 0;

		if (ATOM_ContentStream::_waitInterface)
		{
			request = _requests.getAndRemove (ATOM_ContentStream::_waitInterface);

			if (request && request->loadInterface == ATOM_ContentStream::_waitInterface)
			{
				ATOM_ContentStream::_waitRequest = request;
			}
		}

		//if (!request)
		else {
			request = _requests.getAndRemove ();
		}

		if (!request)
		{
			::Sleep (50);
			break;
		}

#ifdef DEBUG_MTLOAD
		__try
		{
#endif
			if (request->discardStamp < ATOM_ContentStream::_discardStamp)
			{
				setRequestStage (request, LOADINGSTAGE_DISCARD);
				setRequestInQueue (request, 0);
			}
			else
			{
				ATOM_BaseResourceLoader::LOADRESULT result = request->loadInterface->getLoader() ? request->loadInterface->getLoader()->loadFromDisk () : ATOM_BaseResourceLoader::LOADERR_OK;
				if (result == ATOM_BaseResourceLoader::LOADERR_FAILED)
				{
					setRequestStage (request, LOADINGSTAGE_NOTLOADED);
					setRequestInQueue (request, 0);
				}
				else
				{
					setRequestStage (request, LOADINGSTAGE_TOBELOCK);
					_creationThread->appendRequest (request);
				}
			}
#ifdef DEBUG_MTLOAD
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			printEntry (request);
			__asm int 3
		}
#endif
	}
}

ATOM_BEGIN_EVENT_MAP(ATOM_ContentCreationThread, ATOM_Thread)
	ATOM_EVENT_HANDLER(ATOM_ContentCreationThread, ATOM_ThreadIdleEvent, onIdle)
ATOM_END_EVENT_MAP

ATOM_ContentCreationThread::ATOM_ContentCreationThread (void)
{
}

ATOM_ContentCreationThread::~ATOM_ContentCreationThread (void)
{
}

void ATOM_ContentCreationThread::appendRequest (ATOM_LoadingRequest *request)
{
	_gfxThreadQueue.append (request);
}

void ATOM_ContentCreationThread::discardRequest (ATOM_LoadInterface *loadInterface)
{
	if (!_creationQueue.discardRequest (loadInterface))
	{
		_gfxThreadQueue.discardRequest (loadInterface);
	}
}

void ATOM_ContentCreationThread::processRequests (void)
{
	// this function always run in the primary thread, static variable is safety.
	static ATOM_VECTOR<ATOM_LoadingRequest*> remains;
	static const int processCount = 24;

	int processed = 0;

	for (;;)
	{
		if (processed >= processCount)
		{
			break;
		}

		ATOM_LoadingRequest *request = 0;

		if (ATOM_ContentStream::_waitInterface)
		{
			request = _gfxThreadQueue.getAndRemove (ATOM_ContentStream::_waitInterface);

			if (request && request->loadInterface == ATOM_ContentStream::_waitInterface)
			{
				ATOM_ContentStream::_waitRequest = request;
			}
		}

		//if (!request)
		else {
			request = _gfxThreadQueue.getAndRemove ();
		}

		if (!request)
		{
			break;
		}

#ifdef DEBUG_MTLOAD
		try
		{
#endif
			if (request->discardStamp < ATOM_ContentStream::_discardStamp)
			{
				setRequestStage (request, LOADINGSTAGE_DISCARD);
				setRequestInQueue (request, 0);
				++processed;
			}
			else
			{
				switch (request->stage)
				{
				case LOADINGSTAGE_TOBELOCK:
					{
						if (!request->loadInterface->getLoader()->lock ())
						{
							setRequestStage(request, LOADINGSTAGE_NOTLOADED);
							setRequestInQueue (request, 0);
						}
						else
						{
							_creationQueue.append (request);
						}
						++processed;
						break;
					}
				case LOADINGSTAGE_TOBEUNLOCK:
					{
						setRequestStage (request, (ATOM_LoadingStage)request->loadInterface->getLoader()->unlock ());
						if (request->stage == LOADINGSTAGE_TRYAGAIN)
						{
							setRequestStage (request, LOADINGSTAGE_TOBEUNLOCK);
							remains.push_back (request);
						}
						else
						{
							setRequestInQueue (request, 0);
							++processed;
						}
						break;
					}
				case LOADINGSTAGE_TOBECREATE:
					{
						request->loadInterface->getLoader()->unlock ();
						setRequestStage (request, LOADINGSTAGE_NOTLOADED);
						setRequestInQueue (request, 0);
						++processed;
						break;
					}
				case LOADINGSTAGE_NOTLOADED:
					{
						setRequestInQueue (request, 0);
						++processed;
						break;
					}
				default:
					{
						::DebugBreak ();
						setRequestInQueue (request, 0);
						break;
					}
				}
			}
#ifdef DEBUG_MTLOAD
		}
		catch (...)
		{
			printEntry (request);
			__asm int 3
		}
#endif
	}

	if (remains.size() > 0)
	{
		for (unsigned i = 0; i < remains.size(); ++i)
		{
			if (remains[i]->discardStamp < ATOM_ContentStream::_discardStamp)
			{
				setRequestStage (remains[i], LOADINGSTAGE_DISCARD);
				setRequestInQueue (remains[i], 0);
			}
			else
			{
				_gfxThreadQueue.append (remains[i]);
			}
		}
		remains.resize (0);
	}
}

void ATOM_ContentCreationThread::onIdle (ATOM_ThreadIdleEvent*event)
{
	bool waitFound = false;

	for (;;)
	{
		ATOM_LoadingRequest *request = 0;

		if (ATOM_ContentStream::_waitInterface)
		{
			request = _creationQueue.getAndRemove (ATOM_ContentStream::_waitInterface);

			if (request && request->loadInterface == ATOM_ContentStream::_waitInterface)
			{
				ATOM_ContentStream::_waitRequest = request;
			}
		}

		//if (!request)
		else {
			request = _creationQueue.getAndRemove ();
		}

		if (!request)
		{
			::Sleep (50);
			break;
		}

#ifdef DEBUG_MTLOAD
		__try
		{
#endif
			if (request->discardStamp < ATOM_ContentStream::_discardStamp)
			{
				setRequestStage (request, LOADINGSTAGE_DISCARD);
				setRequestInQueue (request, 0);
			}
			else
			{
				if (!request->loadInterface->getLoader()->realize ())
				{
					setRequestStage (request, LOADINGSTAGE_TOBECREATE);
				}
				else
				{
					setRequestStage (request, LOADINGSTAGE_TOBEUNLOCK);
				}

				_gfxThreadQueue.append (request);
			}
#ifdef DEBUG_MTLOAD
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			printEntry (request);
			__asm int 3
		}
#endif
	}
}

ATOM_IOThread ATOM_ContentStream::_ioThread;
ATOM_MAP<long, ATOM_ContentStream::RequestGroup> ATOM_ContentStream::_groups;
ATOM_LoadInterface * volatile ATOM_ContentStream::_waitInterface = 0;
ATOM_LoadingRequest * volatile ATOM_ContentStream::_waitRequest = 0;
volatile long ATOM_ContentStream::_discardStamp = 0;
volatile long ATOM_ContentStream::_maxRequestCount = 50;
volatile long ATOM_ContentStream::_processBatchBusy = 8;
volatile long ATOM_ContentStream::_processBatch = 2;
long ATOM_ContentStream::_nextGroup = 1;

static long numRequests = 0;

long ATOM_ContentStream::allocGroup (void)
{
	long id = _nextGroup++;

	RequestGroup &group = _groups[id];
	group.id = id;

	return id;
}

bool ATOM_ContentStream::appendRequest (ATOM_LoadingRequest *request)
{
	if (!_ioThread.run ())
	{
		run ();
	}

	if (!request)
	{
		return false;
	}

	request->inqueue = 0;
	long group = request->groupId;

#ifdef DEBUG_MTLOAD
	{
		ATOM_Mutex::ScopeMutex l(requestEntriesLock);

		CallstackEntry &e = requestEntries[request];
		e.num = pfnCaptureStackBackTrace(0, 16, e.entries, 0);
		e.num_d = 0;
	}
#endif
	
	ATOM_MAP<long, RequestGroup>::iterator it = _groups.find (group);
	RequestGroup *requestGroup = 0;
	if (it == _groups.end ())
	{
		requestGroup = &_groups[group];
		requestGroup->id = group;
	}
	else
	{
		requestGroup = &it->second;
	}

	request->discardStamp = _discardStamp;

	ATOM_LoadInterface::LoadingState state = request->loadInterface->getLoadingState();
	if (state == ATOM_LoadInterface::LS_NOTLOADED)
	{
		setRequestStage (request, LOADINGSTAGE_TOBELOAD);

		request->loadInterface->getLoader()->setLoadingRequest (request);
		request->loadInterface->setLoadingState (ATOM_LoadInterface::LS_LOADING);
		request->inqueue = 1;
		_ioThread.appendRequest (request);
	}
	else
	{
		switch (state)
		{
		case ATOM_LoadInterface::LS_LOADED:
			setRequestStage (request, LOADINGSTAGE_FINISH);
			break;
		case ATOM_LoadInterface::LS_LOADFAILED:
			setRequestStage (request, LOADINGSTAGE_NOTLOADED);
			break;
		case ATOM_LoadInterface::LS_LOADING:
			setRequestStage (request, LOADINGSTAGE_UNKNOWN);
			break;
		default:
			return false;
		}
	}

	if ((request->flags & ATOM_LoadInterface::LF_ORDERED) != 0)
	{
		requestGroup->orderedQueue.push_back (request);
	}
	else
	{
		requestGroup->unorderedQueue.push_back (request);
	}

	numRequests++;

	return true;
}

void ATOM_ContentStream::processGroupRequests (long groupId, RequestGroup *group)
{
	while (!group->orderedQueue.empty())
	{
		ATOM_LoadingRequest *request = group->orderedQueue.front();
		ATOM_LoadInterface::LoadingState state = request->loadInterface->getLoadingState ();

		if (request->inqueue || (request->stage == LOADINGSTAGE_UNKNOWN && state == ATOM_LoadInterface::LS_LOADING))
		{
			break;
		}

		if (request->loadInterface == _waitInterface)
		{
#if ATOM3D_COMPILER_MSVC
			::InterlockedExchange ((volatile LONG*)&_waitRequest, 0);
			::InterlockedExchange ((volatile LONG*)&_waitInterface, 0);
#else
			::InterlockedExchange ((LONG*)&_waitRequest, 0);
			::InterlockedExchange ((LONG*)&_waitInterface, 0);
#endif
		}

		switch (request->stage)
		{
		case LOADINGSTAGE_FINISH:
			request->loadInterface->setLoadingState (ATOM_LoadInterface::LS_LOADED);
			break;
		case LOADINGSTAGE_NOTLOADED:
			request->loadInterface->setLoadingState (ATOM_LoadInterface::LS_LOADFAILED);
			break;
		case LOADINGSTAGE_DISCARD:
			request->loadInterface->unload ();
			request->loadInterface->setLoadingState (ATOM_LoadInterface::LS_NOTLOADED);
			break;
		case LOADINGSTAGE_UNKNOWN:
			break;
		default:
			::DebugBreak ();
			break;
		}

		ATOM_LoadInterface *loadInterface = request->loadInterface;

		if (request->callback)
		{
			request->callback (request);
		}
		loadInterface->finishLoad ();

		numRequests--;

		group->orderedQueue.pop_front();
	}

	unsigned index = 0;
	for (;;)
	{
		if (index == group->unorderedQueue.size())
		{
			break;
		}

		ATOM_LoadingRequest *request = group->unorderedQueue[index];

#ifdef DEBUG_MTLOAD
		__try
		{
#endif
			ATOM_LoadInterface::LoadingState state = request->loadInterface->getLoadingState ();

			if (request->inqueue || (request->stage == LOADINGSTAGE_UNKNOWN && state == ATOM_LoadInterface::LS_LOADING))
			{
				++index;
				continue;
			}

			if (request->loadInterface == _waitInterface)
			{
#if ATOM3D_COMPILER_MSVC
				::InterlockedExchange ((volatile LONG*)&_waitRequest, 0);
				::InterlockedExchange ((volatile LONG*)&_waitInterface, 0);
#else
				::InterlockedExchange ((LONG*)&_waitRequest, 0);
				::InterlockedExchange ((LONG*)&_waitInterface, 0);
#endif
			}

			ATOM_LoadingStage stage = request->stage;
			switch (stage)
			{
			case LOADINGSTAGE_FINISH:
				request->loadInterface->setLoadingState (ATOM_LoadInterface::LS_LOADED);
				break;
			case LOADINGSTAGE_NOTLOADED:
				request->loadInterface->setLoadingState (ATOM_LoadInterface::LS_LOADFAILED);
				break;
			case LOADINGSTAGE_DISCARD:
				request->loadInterface->unload ();
				request->loadInterface->setLoadingState (ATOM_LoadInterface::LS_NOTLOADED);
				break;
			case LOADINGSTAGE_UNKNOWN:
				break;
			default:
				::DebugBreak ();
				break;
			}

			ATOM_LoadInterface *loadInterface = request->loadInterface;
			if (request->callback)
			{
				request->callback (request);
			}
			loadInterface->finishLoad ();

			group->unorderedQueue[index] = group->unorderedQueue.back();
			group->unorderedQueue.pop_back ();

			numRequests--;

#ifdef DEBUG_MTLOAD
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			printEntry (request);
			__asm int 3
		}
#endif
	}
}

static long requestCount = 0;

void ATOM_ContentStream::processRequests (void)
{
	long batch = (numRequests > _maxRequestCount) ? _processBatchBusy : _processBatch;

	for (long i = 0; i < batch; ++i)
	{
		_ioThread.processRequests ();

		if (_waitInterface && _waitRequest)
		{
			long waitGroup = _waitRequest->groupId;

			ATOM_MAP<long, RequestGroup>::iterator it = _groups.find (waitGroup);
			if (it != _groups.end ())
			{
				processGroupRequests (it->first, &it->second);

				if (it->second.orderedQueue.empty () && it->second.unorderedQueue.empty ())
				{
					_groups.erase (it);

#if ATOM3D_COMPILER_MSVC
					::InterlockedExchange ((volatile LONG*)&_waitRequest, 0);
					::InterlockedExchange ((volatile LONG*)&_waitInterface, 0);
#else
					::InterlockedExchange ((LONG*)&_waitRequest, 0);
					::InterlockedExchange ((LONG*)&_waitInterface, 0);
#endif
				}
			}
			else
			{
				ATOM_LOGGER::error ("Wait group is empty while the request still alive\n");
#if ATOM3D_COMPILER_MSVC
				::InterlockedExchange ((volatile LONG*)&_waitRequest, 0);
				::InterlockedExchange ((volatile LONG*)&_waitInterface, 0);
#else
				::InterlockedExchange ((LONG*)&_waitRequest, 0);
				::InterlockedExchange ((LONG*)&_waitInterface, 0);
#endif
			}
		}
		else
		{
			ATOM_MAP<long, RequestGroup>::iterator it = _groups.begin();
			if (it != _groups.end ())
			{
				processGroupRequests (it->first, &it->second);

				if (it->second.orderedQueue.empty () && it->second.unorderedQueue.empty ())
				{
					_groups.erase (it);
				}
			}
		}
	}
}

void ATOM_ContentStream::run (void)
{
	_ioThread.run ();
}

bool ATOM_ContentStream::haveQueuedRequests (void)
{
	return !_groups.empty ();
}

void ATOM_ContentStream::discardAll (void)
{
#if ATOM3D_COMPILER_MSVC
	::InterlockedIncrement (&_discardStamp);
#else
	::InterlockedIncrement ((LONG*)&_discardStamp);
#endif

	waitForAllDone ();
}

void ATOM_ContentStream::waitForAllDone (void)
{
	while (haveQueuedRequests())
	{
		processRequests ();
	}
}

void ATOM_ContentStream::setHighPriorityInterface (ATOM_LoadInterface *loadInterface)
{
#if ATOM3D_COMPILER_MSVC
	::InterlockedExchange ((volatile LONG*)&_waitInterface, (LONG)loadInterface);
	::InterlockedExchange ((volatile LONG*)&_waitRequest, 0L);
#else
	::InterlockedExchange ((LONG*)&_waitInterface, (LONG)loadInterface);
	::InterlockedExchange ((LONG*)&_waitRequest, 0L);
#endif
}

void ATOM_ContentStream::waitForInterfaceDone (ATOM_LoadInterface *loadInterface)
{
	if (!loadInterface)
	{
		return;
	}

	if (loadInterface->getLoadingState () != ATOM_LoadInterface::LS_LOADING)
	{
		return;
	}

#if ATOM3D_COMPILER_MSVC
	::InterlockedExchange ((volatile LONG*)&_waitInterface, (LONG)loadInterface);
	::InterlockedExchange ((volatile LONG*)&_waitRequest, 0L);
#else
	::InterlockedExchange ((LONG*)&_waitInterface, (LONG)loadInterface);
	::InterlockedExchange ((LONG*)&_waitRequest, 0L);
#endif

	while (_waitInterface)
	{
		processRequests ();
		::Sleep (1);
	}
}

void ATOM_ContentStream::waitForGroupDone (long group)
{
	ATOM_MAP<long, RequestGroup>::iterator it;;

	for (;;)
	{
		it = _groups.find (group);

		if (it == _groups.end ())
		{
			break;
		}

		if (it->second.unorderedQueue.size() > 0)
		{
			it->second.unorderedQueue.front()->loadInterface->insureLoadDone ();
		}
		else
		{
			break;
		}

		processRequests ();
	}

	for (;;)
	{
		it = _groups.find (group);

		if (it == _groups.end ())
		{
			break;
		}

		if (it->second.orderedQueue.size() > 0)
		{
			it->second.orderedQueue.front()->loadInterface->insureLoadDone ();
		}
		else
		{
			break;
		}

		processRequests ();
	}
}

long ATOM_ContentStream::getNumRequestsPending (void)
{
	return numRequests;
}

void ATOM_ContentStream::discardRequest (ATOM_LoadInterface *loadInterface)
{
	if (loadInterface && loadInterface->getLoadingState() == ATOM_LoadInterface::LS_LOADING)
	{
		_ioThread.discardRequest (loadInterface);
	}
}

void ATOM_ContentStream::setMaxRequestsPending (long maxCount)
{
#if ATOM3D_COMPILER_MSVC
	::InterlockedExchange (&_maxRequestCount, maxCount);
#else
	::InterlockedExchange ((LONG*)&_maxRequestCount, maxCount);
#endif
}

long ATOM_ContentStream::getMaxRequestsPending (void)
{
	return _maxRequestCount;
}

void ATOM_ContentStream::setProcessBatchBusy (long batch)
{
	if (batch <= 0)
	{
		batch = 1;
	}

#if ATOM3D_COMPILER_MSVC
	::InterlockedExchange (&_processBatchBusy, batch);
#else
	::InterlockedExchange ((LONG*)&_processBatchBusy, batch);
#endif
}

long ATOM_ContentStream::getProcessBatchBusy (void)
{
	return _processBatchBusy;
}

void ATOM_ContentStream::setProcessBatch (long batch)
{
	if (batch <= 0)
	{
		batch = 1;
	}

#if ATOM3D_COMPILER_MSVC
	::InterlockedExchange (&_processBatch, batch);
#else
	::InterlockedExchange ((LONG*)&_processBatch, batch);
#endif
}

long ATOM_ContentStream::getProcessBatch (void)
{
	return _processBatch;
}

