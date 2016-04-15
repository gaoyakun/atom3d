#ifndef __ATOM3D_KERNEL_CONTENTSTREAM_H
#define __ATOM3D_KERNEL_CONTENTSTREAM_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "basedefs.h"

class ATOM_KERNEL_API ATOM_LoadInterface;

enum ATOM_LoadingStage
{
	LOADINGSTAGE_UNKNOWN,
	LOADINGSTAGE_TOBELOAD,
	LOADINGSTAGE_TOBELOCK,
	LOADINGSTAGE_TOBECREATE,
	LOADINGSTAGE_TOBEUNLOCK,
	LOADINGSTAGE_TRYAGAIN,
	LOADINGSTAGE_DISCARD,
	LOADINGSTAGE_NOTLOADED,
	LOADINGSTAGE_FINISH
};

struct ATOM_KERNEL_API ATOM_LoadingRequest;
typedef void (__cdecl *ATOM_LoadingRequestCallback)(ATOM_LoadingRequest *);

struct ATOM_KERNEL_API ATOM_LoadingRequest
{
	ATOM_LoadInterface *loadInterface;
	ATOM_LoadingStage stage;
	unsigned flags;
	void *userData;
	ATOM_LoadingRequestCallback callback;
	int groupId;
	long discardStamp;
	long inqueue;

	ATOM_LoadingRequest (void);
	~ATOM_LoadingRequest (void);
};


class ATOM_RequestQueue
{
public:
  ATOM_RequestQueue (void);
  void append (ATOM_LoadingRequest *val);
  void clear (void);
  ATOM_LoadingRequest *getAndRemove (void);
  ATOM_LoadingRequest *getAndRemove (ATOM_LoadInterface *loadInterface);
  ATOM_LoadingRequest *discardRequest (ATOM_LoadInterface *loadInterface);

private:
  ATOM_DEQUE<ATOM_LoadingRequest*> _queuePing;
  ATOM_DEQUE<ATOM_LoadingRequest*> _queuePong;
  ATOM_Mutex _lock;
  ATOM_DEQUE<ATOM_LoadingRequest*> *_queueR;
  ATOM_DEQUE<ATOM_LoadingRequest*> *_queueW;
};

class ATOM_ContentCreationThread;

class ATOM_IOThread: public ATOM_Thread
{
public:
	ATOM_IOThread (void);
	virtual ~ATOM_IOThread (void);

public:
	void appendRequest (ATOM_LoadingRequest *request);
	void onIdle (ATOM_ThreadIdleEvent *event);
	void processRequests (void);
	void clear (HANDLE finishEvent);
	void discardRequest (ATOM_LoadInterface *loadInterface);

public:
	virtual int run (void);

private:
	ATOM_RequestQueue _requests;
	ATOM_ContentCreationThread *_creationThread;

	ATOM_DECLARE_EVENT_MAP(ATOM_IOThread, ATOM_Thread)
};

class ATOM_ContentCreationThread: public ATOM_Thread
{
public:
	ATOM_ContentCreationThread (void);
	virtual ~ATOM_ContentCreationThread (void);

public:
	void appendRequest (ATOM_LoadingRequest *request);
	void processRequests (void);
	void onIdle (ATOM_ThreadIdleEvent *event);
	void discardRequest (ATOM_LoadInterface *loadInterface);

private:
	ATOM_RequestQueue _creationQueue;
	ATOM_RequestQueue _gfxThreadQueue;

	ATOM_DECLARE_EVENT_MAP(ATOM_ContentCreationThread, ATOM_Thread)
};

class ATOM_KERNEL_API ATOM_ContentStream
{
	friend class ATOM_IOThread;
	friend class ATOM_ContentCreationThread;

	struct RequestGroup
	{
		ATOM_VECTOR<ATOM_LoadingRequest*> unorderedQueue;
		ATOM_DEQUE<ATOM_LoadingRequest*> orderedQueue;
		long id;
	};

public:
	static long allocGroup (void);
	static bool appendRequest (ATOM_LoadingRequest *request);
	static void processRequests (void);
	static void run (void);
	static bool haveQueuedRequests (void); // must be called from the graphics thread!!
	static void discardAll (void);
	static void discardRequest (ATOM_LoadInterface *loadInterface);
	static void waitForAllDone (void); // must be called from the graphics thread!!
	static void waitForInterfaceDone (ATOM_LoadInterface *loadInterface);
	static void waitForGroupDone (long group);
	static void setHighPriorityInterface (ATOM_LoadInterface *loadInterface);
	static long getNumRequestsPending (void);
	static void setMaxRequestsPending (long maxCount);
	static long getMaxRequestsPending (void);
	static void setProcessBatchBusy (long batch);
	static long getProcessBatchBusy (void);
	static void setProcessBatch (long batch);
	static long getProcessBatch (void);

private:
	static void processGroupRequests (long groupId, RequestGroup *group);

private:
	static ATOM_IOThread _ioThread;
	static ATOM_MAP<long, RequestGroup> _groups;
	static ATOM_LoadInterface * volatile _waitInterface;
	static ATOM_LoadingRequest * volatile _waitRequest;
	static long volatile _discardStamp;
	static long volatile _maxRequestCount;
	static long volatile _processBatchBusy;
	static long volatile _processBatch;
	static long _nextGroup;
};

#endif // __ATOM3D_KERNEL_CONTENTSTREAM_H
