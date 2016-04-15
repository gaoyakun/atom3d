#ifndef __ATOM3D_KERNEL_RESLOADASYNC_H
#define __ATOM3D_KERNEL_RESLOADASYNC_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "basedefs.h"

class ATOM_AsyncLoader;
class ATOM_Node;
class ATOM_SDLScene;
class ATOM_CoreMaterial;
class ATOM_Texture;

struct ATOM_AsyncLoadObjListener
{
	virtual void OnLoadStart(){}
	virtual void OnLoadFinished(){}
};

enum ATOM_LoadPriority
{
	ATOM_LoadPriority_IMMEDIATE = -1,	// 马上加载 不进行异步加载
	ATOM_LoadPriority_ASYNCBASE = 1,	// 异步加载优先级从1开始
};


//==================================================//
//!节点加载完成事件
class ATOM_NodeAsyncLoadEvent
{
public:
	ATOM_NodeAsyncLoadEvent(void): _id(0) {}
	ATOM_NodeAsyncLoadEvent(unsigned int id): _id(id) {}

	unsigned int _id;
};

class ATOM_NodeAsyncLoadMgrBase
{
public:
	ATOM_NodeAsyncLoadMgrBase(){}
	virtual ~ATOM_NodeAsyncLoadMgrBase(){}

	virtual void OnNodeAsyncLoad(ATOM_NodeAsyncLoadEvent *event){}

	//ATOM_DECLARE_EVENT_MAP_NOPARENT(ATOM_NodeAsyncLoadMgrBase)
};

class ATOM_NodeAsyncLoadMgrGroup
{
public:
	ATOM_NodeAsyncLoadMgrGroup();
	~ATOM_NodeAsyncLoadMgrGroup();

	static void RegisterNodeAsyncLoadMgr(ATOM_NodeAsyncLoadMgrBase * pNodeALM );
	static void OnNodeAsyncLoad(ATOM_NodeAsyncLoadEvent *event);

protected:

	typedef ATOM_VECTOR<ATOM_NodeAsyncLoadMgrBase*>				MGRVEC;
	typedef ATOM_VECTOR<ATOM_NodeAsyncLoadMgrBase*>::iterator	MGRITER;
	static MGRVEC _mgrVec;
};
//==================================================//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_AsyncableLoadObject : public ATOM_ReferenceObj
{
	friend class ATOM_AsyncLoader;
public:
	//////////////////////////////////////////////////
	ATOM_AsyncableLoadObject();
	virtual ~ATOM_AsyncableLoadObject();

	//////////////////////////////////////////////////
	void SetPriority(int priority);
	int GetPriority( void ) const;
	void* GetBuffer(unsigned int & size);
	//////////////////////////////////////////////////
	bool Start( const char* file, int priority = ATOM_LoadPriority_ASYNCBASE, unsigned time = 0);
	virtual bool Load();
	bool Done();
	virtual bool OnloadEnd();
	/////////////////////////////////////////////////
	virtual bool Lock( bool & needRecurse );
	virtual bool Fill();
	virtual bool Unlock();
	/////////////////////////////////////////////////
	bool Abandon();
	/////////////////////////////////////////////////
	virtual void GarbageCollect();
	//////////////////////////////////////////////////
	bool IsNotStartLoad();
	bool IsStartLoad();
	bool IsInLoading();
	bool IsDone( unsigned time );
	bool IsLocked();
	bool IsFilled();
	virtual bool IsLoadAllFinished();
	virtual void SetLoadAllFinished();
	unsigned int GetLoadStage();
	void SetLoadStage(unsigned int stageid);
	unsigned int GetResult();
	unsigned int GetFailedError();
	bool IsLoadFailed();
	//////////////////////////////////////////////////
	void AddListener( ATOM_Node * listener );
	void RemoveListener( ATOM_Node * listener );
	void ClearAllListeners();
	//////////////////////////////////////////////////
	virtual void processAllDependents();
	//////////////////////////////////////////////////
//protected:
	ATOM_STRING		filename;
	void *			data;
	unsigned int	data_size;
	int				priority;					// 优先级
	unsigned		start_time;
	unsigned		done_time;
	unsigned int	flag;
	unsigned int	result;
	unsigned int	errorno;
	bool			abandoned;					// 是否被放弃

	ATOM_Object *	host;
	//ATOM_AUTOREF(ATOM_Object) host;

	//unsigned long inQueueFlag;

	///////////////////////////////////
	// DEBUG
	unsigned int flag_debug;
	unsigned CheckStageFlagDebug();
	///////////////////////////////////

	// 将指针改为引用 防止在监听者对象被删除之后再被调用方法
	//ATOM_VECTOR<ATOM_AsyncLoadObjListener*> listeners;
	//ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> listeners;

private:
	void _garbageCollect();
};

/////////////////////////////////////////////////////////////////////////////////

//============================================================================//
//========================= ATOM_NodeAttributeFile ===========================//
//============================================================================//

//============================================================================//
//========================= ATOM_AsyncableLoadAttributeFile ==================//
//============================================================================//
class ATOM_AsyncableLoadAttributeFile : public ATOM_AsyncableLoadObject
{
public :
	ATOM_AsyncableLoadAttributeFile();
	virtual ~ATOM_AsyncableLoadAttributeFile();

	//virtual bool Start( const char* file, int priority = ATOM_LoadPriority_ASYNCBASE, float time = 0.f);
	virtual bool Load();

protected:

private:
	
};
class ATOM_ENGINE_API ATOM_NodeAttributeFile : public ATOM_Object
{
	ATOM_CLASS(engine, ATOM_NodeAttributeFile, ATOM_NodeAttributeFile)

public:
	ATOM_NodeAttributeFile();
	virtual ~ATOM_NodeAttributeFile();

	bool loadSync(const char* filename);
	bool loadAsync(const char* filename, const int loadPriority);

	ATOM_TiXmlDocument & getDocument();
	const ATOM_TiXmlDocument & getDocument() const;

	ATOM_AsyncableLoadAttributeFile * getAsyncLoader();

	virtual void OnFileLoadFinished(){}

protected:
	ATOM_TiXmlDocument _doc;

	ATOM_AUTOPTR(ATOM_AsyncableLoadAttributeFile)	_asyncLoader;
};

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

//struct ci_less
//{
//	bool operator() (const ATOM_AUTOPTR(ATOM_AsyncableLoadObject) s1, const ATOM_AUTOPTR(ATOM_AsyncableLoadObject) s2) const
//	{
//		return s1->priority > s2->priority;
//	}
//};
//typedef ATOM_MULTISET< ATOM_AUTOPTR(ATOM_AsyncableLoadObject),ci_less> CREATE_RES_QUEUE_SORT;

// 使用两个IO线程 一个作为下载中使用 一个作为本地使用
#define USE_TWO_IO_THREAD 1
#if USE_TWO_IO_THREAD
#define DISK_IO_THREADID 0
#define DOWNLOAD_IO_THREADID 1
#endif

struct ci_less
{
	bool operator() (const ATOM_AsyncableLoadObject * s1, const ATOM_AsyncableLoadObject * s2) const
	{
		return s1->priority > s2->priority;
	}
};
struct ci_less_autoptr
{
	bool operator() (const ATOM_AUTOPTR(ATOM_AsyncableLoadObject) s1, const ATOM_AUTOPTR(ATOM_AsyncableLoadObject) s2) const
	{
		return s1->priority > s2->priority;
	}
};

typedef ATOM_MULTISET< ATOM_AsyncableLoadObject *,ci_less >						CREATE_RES_QUEUE_SORT;
typedef ATOM_MULTISET< ATOM_AUTOPTR(ATOM_AsyncableLoadObject),ci_less_autoptr > CREATE_RES_QUEUE_SORT_AUTOPTR;

/////////////////////////////////////////////////////////////////////////////////////////////////////


class ATOM_AsyncLoadIOThread : public ATOM_Thread
{
public:
	ATOM_AsyncLoadIOThread(int ios_per_procedure,unsigned id);
	virtual ~ATOM_AsyncLoadIOThread();

	//virtual void handleEvent(ATOM_Event *event);
	void onIdle (ATOM_ThreadIdleEvent *event);

	void AddTask( ATOM_AsyncableLoadObject * obj );

	unsigned int GetTaskCount();

	void AbandonAll();
	void AbandonTask(ATOM_AsyncableLoadObject * obj);

	void Pause();//	{	runflag=0;	}
	void Resume();//	{	runflag=1;	}
	void PauseSwap();// {	swapFlag = 0; }
	void ResumeSwap();// {	swapFlag = 1; }

private:

	CREATE_RES_QUEUE_SORT	m_taskQueue;

	ATOM_Mutex				m_lock;

	volatile bool			bRun;
	volatile int			runflag;
	volatile long			totalTaskCount;
	volatile int			swapFlag;
	volatile int			abandonflag;
	int						io_count_per_procedure;

	unsigned				m_id;

	ATOM_DECLARE_EVENT_MAP(ATOM_AsyncLoadIOThread, ATOM_Thread)

	//---------------------- 统计信息 -----------------------//
	struct sStaticInfo
	{
		ATOM_STRING _filename;			
		int			_loadPriority;
		DWORD		_timeForLoad;
	};
	ATOM_VECTOR<sStaticInfo> _statistics;
};

class ATOM_AsyncResCreateObject
{
public:

	struct sResCreateParam
	{
		void *			_data;
		unsigned int	_size;
	};

	ATOM_AsyncResCreateObject();
	~ATOM_AsyncResCreateObject();

public:
	void Start();
	void FillResource();

private:
	void * data;


};

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//=========== 填充线程 =========================//

class ATOM_AsyncResFillThread : public ATOM_Thread
{
public:
	ATOM_AsyncResFillThread(int _fillcount_per_pro);
	virtual ~ATOM_AsyncResFillThread();

	void onIdle (ATOM_ThreadIdleEvent *event);

	void AddTask( ATOM_AsyncableLoadObject * obj );

	unsigned int GetTaskCount();

	void AbandonAll();
	void AbandonTask(ATOM_AsyncableLoadObject * obj);

	void Pause();//	{	runflag=0;	}
	void Resume();//	{	runflag=1;	}
	void PauseSwap();// {	swapFlag = 0; }
	void ResumeSwap();// {	swapFlag = 1; }

private:

#if USE_TWO_IO_THREAD
	ATOM_DEQUE<ATOM_AsyncableLoadObject*>	m_taskQueue;
#else
	CREATE_RES_QUEUE_SORT					m_taskQueue;
#endif

	ATOM_Mutex				m_lock;

	volatile bool			bRun;
	volatile int			runflag;
	volatile long			totalTaskCount;
	volatile int			swapFlag;
	volatile int			abandonflag;
	int						fillcount_per_procedure;

	ATOM_DECLARE_EVENT_MAP(ATOM_AsyncResFillThread, ATOM_Thread)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ATOM_LOADRequest_Event
{
	enum
	{
		FLAG_NOT_SHOW = 0,
		FLAG_SHOW = 1,
	};

	//ATOM_AsyncableLoadObject * _obj;
	ATOM_AUTOPTR(ATOM_AsyncableLoadObject) _obj;
	ATOM_STRING					_file;
	ATOM_AUTOREF(ATOM_Node)		_requester;
	int							_priority;
	bool						_task_assigned;
	int							_showflag;

	ATOM_LOADRequest_Event(	const char* filename, 
							ATOM_Node* requester, 
							int pri,
							ATOM_AsyncableLoadObject * obj,
							int showflag = FLAG_NOT_SHOW );
};

struct requester_less
{
	bool operator() (const ATOM_AUTOREF(ATOM_Node) n1, const ATOM_AUTOREF(ATOM_Node) n2) const;
	/*{
		return n1->_load_priority > n2->_load_priority;
	}*/
};

struct ATOM_LoadRequest_EventList
{
	~ATOM_LoadRequest_EventList();

	/*typedef ATOM_MULTIMAP<void*,ATOM_LOADRequest_Event*> eventdict_t;*/

	typedef ATOM_VECTOR<ATOM_LOADRequest_Event*> REQUEST_SET;

	//typedef ATOM_MAP<ATOM_Node*,REQUEST_SET> eventdict_t;
	typedef ATOM_MAP< ATOM_AUTOREF(ATOM_Node), REQUEST_SET, requester_less > eventdict_t;
	eventdict_t _list;

	typedef ATOM_MULTIMAP<ATOM_STRING,ATOM_LOADRequest_Event*> eventdict_filekey;
	eventdict_filekey _list_filekey;

	void AddLoadRequest(ATOM_Node* node,ATOM_LOADRequest_Event* request);
	void RemoveLoadRequest(ATOM_LOADRequest_Event* request);
	bool IsRequestFinished(void * requester);
	void CheckAllRequester(void);
	bool AssignTask(ATOM_AsyncableLoadObject* task);
	void AbandonAll(bool Destroyall=true);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef bool (*FuncCheckFileLocal)(const char *filename);

class ATOM_AsyncLoader
{
public:

	enum
	{
		ATOM_ASYNCLOAD_RES_LOAD_FAILED = 0,
		ATOM_ASYNCLOAD_RES_LOAD_OK,
		ATOM_ASYNCLOAD_RES_CREATE_FAILED,
		ATOM_ASYNCLOAD_RES_CREATE_OK,
		ATOM_ASYNCLOAD_RES_CREATE_ALL,
	};

	enum
	{
		ATOM_ASYNCLOAD_FAIL_ERR_NONE = 0,
		ATOM_ASYNCLOAD_FAIL_ERR_FILEOPENFAILED,
		ATOM_ASYNCLOAD_FAIL_ERR_FILESIZETOOSMALL,
		ATOM_ASYNCLOAD_FAIL_ERR_FILESIZEMISMATCH,
		ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR,
		ATOM_ASYNCLOAD_FAIL_ERR_UNKNOWN,

		ATOM_ASYNCCREATE_FAIL_ERR_NONE,
		ATOM_ASYNCCREATE_FAIL_ERR_INDEXBUFFER_LOCKFAILED,
		ATOM_ASYNCCREATE_FAIL_ERR_VERTEXBUFFER_LOCKFAILED,
		ATOM_ASYNCCREATE_FAIL_ERR_DEVICEBUFFER_LOCKFAILED,
		ATOM_ASYNCCREATE_FAIL_ERR_MATERIAL_INITFAILED,
		ATOM_ASYNCCREATE_FAIL_ERR_DEVICEBUFFER_FILLDATAFAILED,
		ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR,
		ATOM_ASYNCCREATE_FAIL_ERR_UNKNOWN,

		ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH,
	};

	enum
	{
		ATOM_ASYNCLOAD_NOTLOADSTARTED	= (0),
		ATOM_ASYNCLOAD_LOADSTARTED		= (1 << 0),
		ATOM_ASYNCLOAD_INLOADING		= (1 << 1),
		ATOM_ASYNCLOAD_LOADED			= ( 1 << 2 ),
		ATOM_ASYNCLOAD_LOCKED			= ( 1 << 3 ),
		ATOM_ASYNCLOAD_FILLED			= ( 1 << 4 ),
		ATOM_ASYNCLOAD_ALLFINISHED		= ( 1 << 5 ),
		ATOM_ASYNCLOAD_ABANDONED		= ( 1 << 6 ),
		ATOM_ASYNCLOAD_ALL				=	ATOM_ASYNCLOAD_NOTLOADSTARTED | 
											ATOM_ASYNCLOAD_LOADSTARTED | 
											ATOM_ASYNCLOAD_INLOADING | 
											ATOM_ASYNCLOAD_LOADED | 
											ATOM_ASYNCLOAD_LOCKED | 
											ATOM_ASYNCLOAD_FILLED |
											ATOM_ASYNCLOAD_ALLFINISHED,
	};

	struct AsyncLoaderRunParams
	{
		int												_io_thread_count;
		int												_fill_thread_count;
		int												_io_count_per_iterate;
		int												_fill_count_per_iterate;
		int												_lock_count_per_iterate;
		int												_maxcount_requester_process;
		int												_reprocess_priority_threshold;	// 重新处理的优先级阈值[切换关卡时确定哪些需要重新处理]
		FuncCheckFileLocal								_funcCheckFile;

		AsyncLoaderRunParams():
			_io_thread_count(1),
			_fill_thread_count(2),
			_io_count_per_iterate(6),
			_fill_count_per_iterate(3),
			_lock_count_per_iterate(6),
			_maxcount_requester_process(8),
			_reprocess_priority_threshold(2001),_funcCheckFile(0)
		{

		}
		AsyncLoaderRunParams(	int io_thread_count, 
								int fill_thread_count, 
								int io_count_per_iterate,
								int fill_count_per_iterate,
								int lock_count_per_iterate,
								int maxcount_requester_process, 
								int reprocess_priority_threshold, FuncCheckFileLocal funcCheckFile = 0	):
			_io_thread_count(io_thread_count),
			_fill_thread_count(fill_thread_count),
			_io_count_per_iterate(io_count_per_iterate),
			_fill_count_per_iterate(fill_count_per_iterate),
			_lock_count_per_iterate(lock_count_per_iterate),
			_maxcount_requester_process(maxcount_requester_process),
			_reprocess_priority_threshold(reprocess_priority_threshold),_funcCheckFile(funcCheckFile)
		{

		}
	};

	ATOM_AsyncLoader();
	~ATOM_AsyncLoader();

	static void Run(AsyncLoaderRunParams & param);
	static bool IsRun();

	static void setEnableLog(int bEnable);
	static int isEnableLog();

	static void setEnableStatistics(int bEnable);
	static int isEnableStatistics();

	static void setEnableSceneMTLoad(int bEnable);
	static int isEnableSceneMTLoad();

	static void setSceneMTLoadPriorityHigh(int pri);
	static int getSceneMTLoadPriorityHigh();

	static void Update(bool bCountFrame = true);

	static void Stop();

	// 加载资源
	static void AddAsyncTask( ATOM_AsyncableLoadObject * obj );
	static void TaskDone( ATOM_AsyncableLoadObject * obj );

	// 填充资源
	static void AddFillTask( ATOM_AsyncableLoadObject * obj );
	static void ResFillDone( ATOM_AsyncableLoadObject * obj );

	static void AbandonAllTask(bool bDestroyAll);
	static void AbandonTask(ATOM_AsyncableLoadObject * obj);

	static void AddLoadRequest(ATOM_Node* node,ATOM_LOADRequest_Event* request);
	static void RemoveLoadRequest(ATOM_LOADRequest_Event* request);
	static bool IsRequestFinished(void * requester);
	static void CheckAllRequester(void);
	static bool IsResourceFinshed(ATOM_STRING resName);

	static void SetPreLoadGlobalResource(bool enable);
	static bool ReadGlboalPreLoadTable(const char* filename);
	static bool ReadLevelPreLoadTable(const char* filename);
	static bool AddToImmediateTable(const char* filename);
	static bool CheckImmediateResource(const char* resname );
	static void ClearAllGlboalPreLoads();


	static void SetCurrentScene( ATOM_SDLScene* scene );


	static void enableProcedureGC(bool bEnable=false);
	static bool isProcedureGCEnabled();

	static void GarbageCollect();

	static void SetMaxIOCountPerIteration(int _io_count_max=8);
	static void SetMaxFillCountPerIteration(int _fill_count_max=4);
	static void SetMaxLockCountPerIteration(int _fill_count_max=8);
	static void SetMaxRequesterProcessCount(int count = 8);
	static int	GetMaxRequesterProcessCount(void);
	static void SetReprocessPriorityThreshold(int threshold = 2001);
	static int  GetReprocessPriorityThreshold(void);

	static void OnAsyncLoadObjectFinished( ATOM_AsyncableLoadObject * obj );

	static bool ProcessLevelOpenedFileList( const char* levelsToBeProcess );

	// 重新处理加载对象
	static bool IsObjectExistIn( ATOM_AsyncableLoadObject * obj );
	static void ReProcessLoadObject( ATOM_AsyncableLoadObject * obj );

	// 检查加载文件是否在本地
	static bool CheckFileLocal(const char* file);
	static void enableDoubleIOThread(bool enable);
	static bool isEnableDoubleIOThread(void);
	//========================================================================//

//	// 节点属性文件异步加载对象
//	static bool FindAttribFileLoader(const char* filename);
//	static ATOM_AsyncableLoadFile * GetAttribFileLoader(const char* filename);
//	static void DestroyAttribFileLoader(ATOM_AsyncableLoadFile * attribFileLoader);
//	static void ClearAttribFileContainer();

private:

	static void _abandonAllTasks(bool bDestroyAll=true);

	/////////////////////////////////////////////////////////////////////////////////////

	// IO 线程
	static ATOM_VECTOR< ATOM_AsyncLoadIOThread* >			m_ioThreads;

	// 填充资源线程
	//static ATOM_AsyncResFillThread*						m_fillThread;
	static ATOM_VECTOR< ATOM_AsyncResFillThread* >			m_fillThreads;

	/////////////////////////////////////////////////////////////////////////////////////

#if USE_TWO_IO_THREAD
	static ATOM_DEQUE<ATOM_AsyncableLoadObject*>			m_createResQueue;
	static ATOM_DEQUE<ATOM_AsyncableLoadObject*>			m_fillResQueue;
#else
	static CREATE_RES_QUEUE_SORT							m_createResQueue;
	static CREATE_RES_QUEUE_SORT							m_fillResQueue;
#endif
	
	// IO线程QUEUE的锁
	static ATOM_Mutex										m_lock;

	// 填充线程QUEUE的锁
	static ATOM_Mutex										m_lock_fill;

	/////////////////////////////////////////////////////////////////////////////////////

	static bool												m_run;

	static int												m_frameCount;
	
	static bool												m_gcProcedureEnable;
	static unsigned int										m_garbageCollectFrameID;
	static unsigned int										m_garbageCollectFrameInterval;
	static unsigned int										m_garbageCollectCountPerFrame;

	static ATOM_LoadRequest_EventList						m_requestEvents;

	//static ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>	m_asyncLoadResources;
	static ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>	m_asyncLoadResources;
	static ATOM_MAP<ATOM_STRING,ATOM_AUTOREF(ATOM_Object)>	m_asyncLoadHosts;

	/////////////////////////////////////////////////////////////////////////////////////
	static ATOM_MAP<ATOM_STRING,int>						m_asyncLoadPriorities;
	static bool												m_bLoadAtStart;
	
	//==========================================================================//
	static ATOM_VECTOR<ATOM_AUTOREF(ATOM_CoreMaterial)>		m_preloadMaterials;
	static ATOM_VECTOR<ATOM_AUTOREF(ATOM_Texture)>			m_preloadTextures;
	//==========================================================================//

	static ATOM_SDLScene*									m_scene;

	static bool												m_preloadGlobalResource;

	static int												m_iocount_per_procedure;
	static int												m_fillcount_per_procedure;
	static int												m_lockcount_per_procedure;
	static int												m_maxcount_requesterProcess;
	static int												m_reprocess_priority_threshold;

	static int												m_enable_log;
	static int												m_enable_statistics;
	static int												m_enable_scene_mtload;
	static int												m_scene_mtload_priority_highest;

	static bool												m_use_two_io_threads;

	// 节点属性文件异步加载对象集合
	/*
	struct sAttributeFile
	{
		ATOM_AsyncableLoadFile *	_attribFileLoader;
		int							_refCount;
	};
	typedef ATOM_MAP<ATOM_STRING,sAttributeFile>			ATTRIBFILECONTAINER;
	typedef ATTRIBFILECONTAINER::iterator					ATTRIBFILEITER;
	static ATTRIBFILECONTAINER m_attribFileContainer;
	*/

	// 检查加载过程中不合法的对象
	static void CheckInvalidObjects();
	
	// 将当前加载中的所有对象优先级降低
	static void PriorityDown();

	static FuncCheckFileLocal								m_funcCheckFile;
};

//============================================================================//

#endif