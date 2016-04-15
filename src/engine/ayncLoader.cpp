#include "StdAfx.h"
#include "ayncLoader.h"

#define DEBUG_ASYNCLOADING_STATISTICS 1
#define DEBUG_ASYNCLOADING_PRIORITY_DOWN 1
#define DEBUG_ASYNCLOADING_IOQUEUE 0

///////////////////////////////////////////////////////////////////////////////////
ATOM_VECTOR<ATOM_AsyncLoadIOThread*>							ATOM_AsyncLoader::m_ioThreads;
ATOM_VECTOR<ATOM_AsyncResFillThread*>							ATOM_AsyncLoader::m_fillThreads;

#if USE_TWO_IO_THREAD

ATOM_DEQUE<ATOM_AsyncableLoadObject*>							ATOM_AsyncLoader::m_createResQueue;
ATOM_DEQUE<ATOM_AsyncableLoadObject*>							ATOM_AsyncLoader::m_fillResQueue;

#else

CREATE_RES_QUEUE_SORT											ATOM_AsyncLoader::m_createResQueue;
CREATE_RES_QUEUE_SORT											ATOM_AsyncLoader::m_fillResQueue;

#endif

ATOM_Mutex														ATOM_AsyncLoader::m_lock;
ATOM_Mutex														ATOM_AsyncLoader::m_lock_fill;

bool															ATOM_AsyncLoader::m_run = false;

int																ATOM_AsyncLoader::m_frameCount = INT_MIN;

bool															ATOM_AsyncLoader::m_gcProcedureEnable = false;
unsigned int													ATOM_AsyncLoader::m_garbageCollectFrameID = 1;
unsigned int													ATOM_AsyncLoader::m_garbageCollectFrameInterval = 1;
unsigned int													ATOM_AsyncLoader::m_garbageCollectCountPerFrame = 5;

ATOM_LoadRequest_EventList										ATOM_AsyncLoader::m_requestEvents;
//ATOM_MAP<ATOM_STRING,ATOM_AUTOREF(ATOM_AsyncableLoadObject)>	ATOM_AsyncLoader::m_asyncLoadResources;
ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>	ATOM_AsyncLoader::m_asyncLoadResources;
ATOM_MAP<ATOM_STRING,ATOM_AUTOREF(ATOM_Object)>					ATOM_AsyncLoader::m_asyncLoadHosts;
ATOM_MAP<ATOM_STRING,int>										ATOM_AsyncLoader::m_asyncLoadPriorities;
bool															ATOM_AsyncLoader::m_bLoadAtStart = false;
ATOM_SDLScene*													ATOM_AsyncLoader::m_scene = 0;
ATOM_VECTOR<ATOM_AUTOREF(ATOM_CoreMaterial)>					ATOM_AsyncLoader::m_preloadMaterials;
ATOM_VECTOR<ATOM_AUTOREF(ATOM_Texture)>							ATOM_AsyncLoader::m_preloadTextures;
bool															ATOM_AsyncLoader::m_preloadGlobalResource = true;
int																ATOM_AsyncLoader::m_iocount_per_procedure = 8;
int																ATOM_AsyncLoader::m_fillcount_per_procedure = 8;
int																ATOM_AsyncLoader::m_lockcount_per_procedure = 8;

int																ATOM_AsyncLoader::m_enable_log = 0;
int																ATOM_AsyncLoader::m_enable_statistics = 0;

int																ATOM_AsyncLoader::m_enable_scene_mtload = 0;
int																ATOM_AsyncLoader::m_scene_mtload_priority_highest = 400;
int																ATOM_AsyncLoader::m_maxcount_requesterProcess = 8;
int																ATOM_AsyncLoader::m_reprocess_priority_threshold = 2001;

bool															ATOM_AsyncLoader::m_use_two_io_threads = true;

// 节点属性文件异步加载对象集合
//ATOM_AsyncLoader::ATTRIBFILECONTAINER							ATOM_AsyncLoader::m_attribFileContainer;

FuncCheckFileLocal												ATOM_AsyncLoader::m_funcCheckFile = 0;

//////////////////////////////////////////////////////////////////////////////////

static volatile long					g_index = 0;
ATOM_AsyncLoader						g_loader;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "wbemuuid.lib")


#if defined(WIN32) && defined(WIN64)
extern "C" void cpuid64(int*);
#endif

class CApicExtractor
{
public:
	CApicExtractor(unsigned int logProcsPerPkg = 1, unsigned int coresPerPkg = 1)
	{
		SetPackageTopology(logProcsPerPkg, coresPerPkg );
	}

	unsigned char SmtId(unsigned char apicId) const
	{
		return apicId & m_smtIdMask.mask;
	}

	unsigned char CoreId(unsigned char apicId) const
	{
		return (apicId & m_coreIdMask.mask) >> m_smtIdMask.width;
	}

	unsigned char PackageId(unsigned char apicId) const
	{
		return (apicId & m_pkgIdMask.mask) >> (m_smtIdMask.width + m_coreIdMask.width);
	}

	unsigned char PackageCoreId(unsigned char apicId) const
	{
		return (apicId & (m_pkgIdMask.mask | m_coreIdMask.mask)) >> m_smtIdMask.width;
	}

	unsigned int GetLogProcsPerPkg() const
	{
		return m_logProcsPerPkg;
	}

	unsigned int GetCoresPerPkg() const
	{
		return m_coresPerPkg;
	}

	void SetPackageTopology(unsigned int logProcsPerPkg, unsigned int coresPerPkg)
	{
		m_logProcsPerPkg   = (unsigned char) logProcsPerPkg;
		m_coresPerPkg      = (unsigned char) coresPerPkg;

		m_smtIdMask.width   = GetMaskWidth(m_logProcsPerPkg / m_coresPerPkg);
		m_coreIdMask.width  = GetMaskWidth(m_coresPerPkg);
		m_pkgIdMask.width   = 8 - (m_smtIdMask.width + m_coreIdMask.width);

		m_pkgIdMask.mask    = (unsigned char) (0xFF << (m_smtIdMask.width + m_coreIdMask.width));
		m_coreIdMask.mask   = (unsigned char) ((0xFF << m_smtIdMask.width) ^ m_pkgIdMask.mask);
		m_smtIdMask.mask    = (unsigned char) ~(0xFF << m_smtIdMask.width);
	}

private:
	unsigned char GetMaskWidth(unsigned char maxIds) const
	{
		--maxIds;
		unsigned char msbIdx(8);
		unsigned char msbMask(0x80);
		while (msbMask && !(msbMask & maxIds))
		{
			--msbIdx;
			msbMask >>= 1;
		}
		return msbIdx;
	}

	struct IdMask
	{
		unsigned char width;
		unsigned char mask;
	};

	unsigned char m_logProcsPerPkg;
	unsigned char m_coresPerPkg;
	IdMask m_smtIdMask;
	IdMask m_coreIdMask;
	IdMask m_pkgIdMask;
};

static bool IsAMD()
{
#if defined(WIN32) || defined(WIN64)
	int CPUInfo[4];
	char refID[] = "AuthenticAMD";
	__cpuid( CPUInfo, 0x00000000 );
	return ((int*) refID)[0] == CPUInfo[1] && ((int*) refID)[1] == CPUInfo[3] && ((int*) refID)[2] == CPUInfo[2];
#else
	return false;
#endif
}


static bool IsIntel()
{
#if defined(WIN32) || defined(WIN64)
	int CPUInfo[4];
	char refID[] = "GenuineIntel";
	__cpuid( CPUInfo, 0x00000000 );
	return ((int*) refID)[0] == CPUInfo[1] && ((int*) refID)[1] == CPUInfo[3] && ((int*) refID)[2] == CPUInfo[2];
#else
	return false;
#endif
}

static bool IsVistaOrAbove()
{
	typedef BOOL (WINAPI* FP_VerifyVersionInfo) (LPOSVERSIONINFOEX, DWORD, DWORDLONG);
	FP_VerifyVersionInfo pvvi((FP_VerifyVersionInfo) GetProcAddress(GetModuleHandle("kernel32"), "VerifyVersionInfoA"));

	if (pvvi)
	{
		typedef ULONGLONG (WINAPI* FP_VerSetConditionMask) (ULONGLONG, DWORD, BYTE);
		FP_VerSetConditionMask pvscm((FP_VerSetConditionMask) GetProcAddress(GetModuleHandle("kernel32"), "VerSetConditionMask"));
		assert(pvscm);

		OSVERSIONINFOEX osvi;
		memset(&osvi, 0, sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		osvi.dwMajorVersion = 6;
		osvi.dwMinorVersion = 0;
		osvi.wServicePackMajor = 0;
		osvi.wServicePackMinor = 0;

		ULONGLONG mask(0);
		mask = pvscm(mask, VER_MAJORVERSION, VER_GREATER_EQUAL);
		mask = pvscm(mask, VER_MINORVERSION, VER_GREATER_EQUAL);
		mask = pvscm(mask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
		mask = pvscm(mask, VER_SERVICEPACKMINOR, VER_GREATER_EQUAL);

		if (pvvi(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR, mask))
			return true;
	}

	return false;
}

// Preferred solution to determine the number of available CPU cores, works reliably only on WinVista/Win7 32/64 and above
// See http://msdn2.microsoft.com/en-us/library/ms686694.aspx for reasons
static void GetNumCPUCoresGlpi(unsigned int& totAvailToSystem, unsigned int& totAvailToProcess)
{
	typedef BOOL (WINAPI *FP_GetLogicalProcessorInformation)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
	FP_GetLogicalProcessorInformation pglpi((FP_GetLogicalProcessorInformation) GetProcAddress(GetModuleHandle("kernel32"), "GetLogicalProcessorInformation"));
	if (pglpi && IsVistaOrAbove())
	{
		unsigned long bufferSize(0);
		pglpi(0, &bufferSize);

		void* pBuffer(ATOM_MALLOC(bufferSize));

		SYSTEM_LOGICAL_PROCESSOR_INFORMATION* pLogProcInfo((SYSTEM_LOGICAL_PROCESSOR_INFORMATION*) pBuffer);
		if (pLogProcInfo && pglpi(pLogProcInfo, &bufferSize))
		{
			DWORD_PTR processAffinity, systemAffinity;
			GetProcessAffinityMask(GetCurrentProcess(), &processAffinity, &systemAffinity);

			unsigned long numEntries(bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
			for (unsigned long i(0); i < numEntries; ++i)
			{
				switch (pLogProcInfo[i].Relationship)
				{
				case RelationProcessorCore:
					{
						++totAvailToSystem;
						if (pLogProcInfo[i].ProcessorMask & processAffinity)
							++totAvailToProcess;
					}
					break;

				default:
					break;
				}
			}
		}

		ATOM_FREE(pBuffer);
	}
}
// Fallback solution for WinXP 32/64
static void GetNumCPUCoresApic(unsigned int& totAvailToSystem, unsigned int& totAvailToProcess)
{
#if defined(_M_IA64)||defined(_M_AMD64)
	totAvailToProcess = 2;
	totAvailToSystem = 2;
#else
	unsigned int numLogicalPerPhysical(1);
	unsigned int numCoresPerPhysical(1);

	int CPUInfo[4];
	__cpuid(CPUInfo, 0x00000001);
	if ((CPUInfo[3] & 0x10000000) != 0) // Hyperthreading / Multicore bit set
	{
		numLogicalPerPhysical = (CPUInfo[1] & 0x00FF0000) >> 16;

		if (IsIntel())
		{
			__cpuid(CPUInfo, 0x00000000);
			if (CPUInfo[0] >= 0x00000004)
			{
#if defined(WIN32) && !defined(WIN64)
				__asm
				{
					mov eax, 4
						xor ecx, ecx
						cpuid
						mov CPUInfo, eax
				}
#else
				CPUInfo[0] = 4;
				CPUInfo[2] = 0;
				cpuid64(CPUInfo);
#endif
				numCoresPerPhysical = ((CPUInfo[0] & 0xFC000000) >> 26) + 1;
			}
		}
		else if (IsAMD())
		{
			__cpuid(CPUInfo, 0x80000000);
			if (CPUInfo[0] >= 0x80000008)
			{
				__cpuid(CPUInfo, 0x80000008);
				if (CPUInfo[2] & 0x0000F000)
					numCoresPerPhysical = 1 << ((CPUInfo[2] & 0x0000F000) >> 12);
				else
					numCoresPerPhysical = (CPUInfo[2] & 0xFF) + 1;
			}
		}
	}

	HANDLE hCurProcess(GetCurrentProcess());
	HANDLE hCurThread(GetCurrentThread());

	const int c_maxLogicalProcessors(sizeof(DWORD_PTR) * 8);
	unsigned char apicIds[c_maxLogicalProcessors] = { 0 };
	unsigned char items(0);

	DWORD_PTR processAffinity, systemAffinity;
	GetProcessAffinityMask(hCurProcess, &processAffinity, &systemAffinity);

	if (systemAffinity == 1)
	{
		assert(numLogicalPerPhysical == 1);
		apicIds[items++] = 0;
	}
	else
	{
		if (processAffinity != systemAffinity)
			SetProcessAffinityMask(hCurProcess, systemAffinity);

		DWORD_PTR prevThreadAffinity(0);
		for (DWORD_PTR threadAffinity = 1; threadAffinity && threadAffinity <= systemAffinity; threadAffinity <<= 1)
		{
			if (systemAffinity & threadAffinity)
			{
				if (!prevThreadAffinity)
				{
					assert(!items);
					prevThreadAffinity = SetThreadAffinityMask(hCurThread, threadAffinity);
				}
				else
				{
					assert(items > 0);
					SetThreadAffinityMask(hCurThread, threadAffinity);
				}

				Sleep(0);

				int CPUInfo[4];
				__cpuid(CPUInfo, 0x00000001);
				apicIds[items++] = (unsigned char) ((CPUInfo[1] & 0xFF000000) >> 24);
			}
		}

		SetProcessAffinityMask(hCurProcess, processAffinity);
		SetThreadAffinityMask(hCurThread, prevThreadAffinity);
		Sleep(0);
	}

	CApicExtractor apicExtractor(numLogicalPerPhysical, numCoresPerPhysical);

	totAvailToSystem = 0;
	{
		unsigned char pkgCoreIds[c_maxLogicalProcessors] = { 0 };
		for (unsigned int i(0); i < items; ++i)
		{
			unsigned int j(0);
			for (; j < totAvailToSystem; ++j)
			{
				if (pkgCoreIds[j] == apicExtractor.PackageCoreId(apicIds[i]))
					break;
			}
			if (j == totAvailToSystem)
			{
				pkgCoreIds[j] = apicExtractor.PackageCoreId(apicIds[i]);
				++totAvailToSystem;
			}
		}
	}

	totAvailToProcess = 0;
	{
		unsigned char pkgCoreIds[c_maxLogicalProcessors] = { 0 };
		for (unsigned int i(0); i < items; ++i)
		{
			if (processAffinity & ((DWORD_PTR) 1 << i))
			{
				unsigned int j(0);
				for (; j < totAvailToProcess; ++j)
				{
					if (pkgCoreIds[j] == apicExtractor.PackageCoreId(apicIds[i]))
						break;
				}
				if (j == totAvailToProcess)
				{
					pkgCoreIds[j] = apicExtractor.PackageCoreId(apicIds[i]);
					++totAvailToProcess;
				}
			}
		}
	}
#endif
}
static void GetNumCPUCores(unsigned int& totAvailToSystem, unsigned int& totAvailToProcess)
{
	totAvailToSystem = 0;
	totAvailToProcess = 0;

	GetNumCPUCoresGlpi(totAvailToSystem, totAvailToProcess);

	if (!totAvailToSystem)
		GetNumCPUCoresApic(totAvailToSystem, totAvailToProcess);
}





/////////////////////////////////////////////////////////////////////////////////////////////////////





ATOM_AsyncableLoadObject::ATOM_AsyncableLoadObject() : 
	filename(""),
	data(0),
	data_size(0),
	start_time(0),
	done_time(0),
	priority(ATOM_LoadPriority_ASYNCBASE),
	flag(ATOM_AsyncLoader::ATOM_ASYNCLOAD_NOTLOADSTARTED),
	result(ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_ALL),
	errorno(ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_NONE),
	abandoned(false)
{
	host = 0;
	//inQueueFlag = 0;

	//////////////////////
	// DEBUG
	flag_debug = flag;
	//////////////////////
}
ATOM_AsyncableLoadObject::~ATOM_AsyncableLoadObject()
{
	ATOM_STACK_TRACE(ATOM_AsyncableLoadObject::~ATOM_AsyncableLoadObject);

	/*if (inQueueFlag)
	{
		ATOM_LOGGER::error ("Deleting loader object while it is loading!\n");
	}*/

	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::log ( "%s : AsyncableLoadObject %s [ Address : %x ] is Destroyed!\n", __FUNCTION__, filename.c_str(), (unsigned)this);

	// 如果被锁定
	if( IsLocked() )
	{
		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::log ( "AsyncableLoadObject %s is locked, unlock it!\n", filename.c_str());

		// 设置FILLED标记
		SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_FILLED);

		// 解锁
		Unlock();
	}

	//ATOM_AsyncLoader::AbandonTask(this);

	//ClearAllListeners();

	GarbageCollect();
}
void ATOM_AsyncableLoadObject::SetPriority(int p)
{
	// 只有优先级比当前的高 才更改
	if( p != ATOM_LoadPriority_IMMEDIATE && p < priority )
		return;

	priority = p;
}
int ATOM_AsyncableLoadObject::GetPriority( void ) const
{
	return priority;
}
void* ATOM_AsyncableLoadObject::GetBuffer(unsigned int & size)
{
	ATOM_STACK_TRACE(ATOM_AsyncableLoadObject::GetBuffer);

	if( !data && size > 0 )
	{
		//ATOM_LOGGER::log( "%s : %s is Allocated \n",__FUNCTION__,filename.c_str() );
		data = ATOM_MALLOC( size );
	}
	if( !data )
		return 0;

	if( size != 0 )
		data_size = size;
	else
		size = data_size;

	return data;
}
bool ATOM_AsyncableLoadObject::Done()
{
	if( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_INLOADING )
	{
		ATOM_LOGGER::error ("%s(%s) Async loading stage flag is not match.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH;		// 失败原因：加载阶段标记不匹配

		return false;
	}

	// 加载结果：加载成功
	result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_OK;

	// 整个加载过程完成(但是资源创建过程还没有)
	flag = ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADED;

#if DEBUG_ASYNCLOADING_STATISTICS
	flag_debug |= flag;
#endif

	return true;
}
bool ATOM_AsyncableLoadObject::Start( const char* file, int pri, unsigned time )
{
	// 阶段标记必须匹配
	if( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_NOTLOADSTARTED )
	{
		ATOM_LOGGER::error ("%s(%s) Async loading stage flag is not match.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH;		// 失败原因：加载阶段标记不匹配
		return false;
	}

	if( !file )
	{
		filename = "annoy";	// 匿名文件
	}
	else if( '\0' == file[0] )
	{
		return false;
	}
	else
	{
		filename = file;
	}

	// 转换成小写
	std::transform( filename.begin(), filename.end(), filename.begin(), ::tolower );

	// 设置优先级
	priority = pri;

	// 设置开始时间
	//start_time = time;

	// 开始加载 增加引用计数
	ATOM_AsyncLoader::AddAsyncTask( this );
	//addRef();
	
	// 整个加载过程正式开始了
	flag = ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADSTARTED;

#if DEBUG_ASYNCLOADING_STATISTICS
	flag_debug |= flag;
#endif

	//inQueueFlag = 1;

	abandoned = false;

	return true;
}
bool ATOM_AsyncableLoadObject::Load()
{
	// 阶段标记必须匹配
	if( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADSTARTED )
	{
		ATOM_LOGGER::error ("%s(%s) Async loading stage flag is not match.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH;		// 失败原因：加载阶段标记不匹配
		return false;
	}

	// 整个加载过程在进行之中。。。。。。
	flag = ATOM_AsyncLoader::ATOM_ASYNCLOAD_INLOADING;

#if DEBUG_ASYNCLOADING_STATISTICS
	flag_debug |= flag;
#endif

	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::log ("%s(%s) has load.\n", __FUNCTION__, filename.c_str());

	return true;
}
// 加载结束 或者 加载放弃
bool ATOM_AsyncableLoadObject::OnloadEnd()
{
	ATOM_STACK_TRACE(ATOM_AsyncableLoadObject::OnloadEnd);

	// 阶段标记必须匹配
	// 当加载完成 或者 加载被放弃时 
	if( ( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADED ) && 
		( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_ABANDONED ) )
	{
		ATOM_LOGGER::error ("%s(%s) Async loading stage flag is not match.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH;		// 失败原因：加载阶段标记不匹配
		return false;
	}

	//// 当加载完成时 调用监听者
	//if( flag == ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADED )
	//{
	//	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)>::iterator iter = listeners.begin();
	//	for( ; iter != listeners.end();  )
	//	{
	//		ATOM_Node * listener = (*iter).get();
	//		if( listener )
	//			listener->onLoadFinished();
	//	}
	//}
	//// 清空 自动递减引用计数
	//listeners.clear();

	// 清除数据
	GarbageCollect();

	// 整个加载过程包括资源创建的过程完全成功结束
	flag = ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED;

	return true;
}

bool ATOM_AsyncableLoadObject::Lock(bool & needRecurse)
{
	// 阶段标记必须匹配
	// 当加载完成
	if( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADED )
	{
		ATOM_LOGGER::error ("%s(%s) Async loading stage flag is not match.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH;		// 失败原因：加载阶段标记不匹配
		return false;
	}

	// 如果还需要进行，直接退出
	if( needRecurse )
		return true;

	// 当前被锁定
	flag = ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOCKED;

#if DEBUG_ASYNCLOADING_STATISTICS
	flag_debug |= flag;
#endif

	ATOM_AsyncLoader::AddFillTask(this);

	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::log ("%s(%s) has locked.\n", __FUNCTION__, filename.c_str());

	return true;
}
bool ATOM_AsyncableLoadObject::Fill()
{
	// 阶段标记必须匹配
	// 当加载完成
	if( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOCKED )
	{
		ATOM_LOGGER::error ("%s(%s) Async loading stage flag is not match.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH;		// 失败原因：加载阶段标记不匹配

		return false;
	}

	// 当前被填充
	flag = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FILLED;

#if DEBUG_ASYNCLOADING_STATISTICS
	flag_debug |= flag;
#endif

	return true;
}
bool ATOM_AsyncableLoadObject::Unlock()
{
	// 阶段标记必须匹配
	// 当加载完成
	if( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_FILLED )
	{
		ATOM_LOGGER::error ("%s(%s) Async loading stage flag is not match.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH;		// 失败原因：加载阶段标记不匹配
		return false;
	}

	//// 当加载完成时 调用监听者
	//if( flag == ATOM_AsyncLoader::ATOM_ASYNCLOAD_FILLED )
	//{
	//	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)>::iterator iter = listeners.begin();
	//	for( ; iter != listeners.end();  )
	//	{
	//		ATOM_Node * listener = (*iter).get();
	//		if( listener )
	//			listener->onLoadFinished();
	//	}
	//}
	//// 清空 自动递减引用计数
	//listeners.clear();

	// 回收内存
	//GarbageCollect();

	// 当前整个过程完成了
	flag = ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED;

#if DEBUG_ASYNCLOADING_STATISTICS
	flag_debug |= flag;
#endif
 
	//ATOM_AsyncLoader::OnAsyncLoadObjectFinished(this);

	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::log ("%s(%s) has unlocked and ALL FINISHED.\n", __FUNCTION__, filename.c_str());

	return true;
}

bool ATOM_AsyncableLoadObject::Abandon()
{
	// 阶段标记必须匹配
	// 如果当前处于加载中状态 
	if( flag == ATOM_AsyncLoader::ATOM_ASYNCLOAD_INLOADING )
	{
		ATOM_LOGGER::error ("%s(%s) Async loading stage flag is not match.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH;		// 失败原因：加载阶段标记不匹配
		return false;
	}

	flag = ATOM_AsyncLoader::ATOM_ASYNCLOAD_ABANDONED;

	while( false == OnloadEnd() );

	return true;
}

void ATOM_AsyncableLoadObject::GarbageCollect()
{
	_garbageCollect();
}
void ATOM_AsyncableLoadObject::_garbageCollect()
{
	if( data && data_size > 0 )
	{
		ATOM_FREE(data);
		data = 0;
		data_size = 0;
		//ATOM_LOGGER::error ("%s(%s) GARBAGE COLLCETION.\n", __FUNCTION__, filename.c_str());
	}
}

unsigned ATOM_AsyncableLoadObject::CheckStageFlagDebug()
{
	ATOM_LOGGER::log( "========== %s ========= \n",filename.c_str() );
	bool bValid = true;

	bool bLoadStart = flag_debug & ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADSTARTED;
	if( bLoadStart )
	{
		ATOM_LOGGER::log( "has in load started stage \n" );
	}
	else
	{
		ATOM_LOGGER::log( "========== Failed! ========= \n" );
		ATOM_LOGGER::log( "Even has not started \n" );
		return ATOM_AsyncLoader::ATOM_ASYNCLOAD_NOTLOADSTARTED;
	}

	bool bInLoading = flag_debug & ATOM_AsyncLoader::ATOM_ASYNCLOAD_INLOADING;
	if( bInLoading )
	{
		ATOM_LOGGER::log( "has in loading stage \n" );
	}
	else
	{
		ATOM_LOGGER::log( "========== Failed! Even has not in loading =========\n" );
		return ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADSTARTED;
	}

	bool bLoaded = flag_debug & ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADED;
	if( bLoaded )
	{
		ATOM_LOGGER::log( "has in loaded stage \n" );
	}
	else
	{
		ATOM_LOGGER::log( "========== Failed! Even has not be loaded =========\n" );
		return ATOM_AsyncLoader::ATOM_ASYNCLOAD_INLOADING;
	}

	bool bLocked = flag_debug & ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOCKED;
	if( bLocked )
	{
		ATOM_LOGGER::log( "has in locked stage \n" );
	}
	else
	{
		ATOM_LOGGER::log( "========== Failed! Even has not be locked =========\n" );
		return ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADED;
	}

	bool bFilled = flag_debug & ATOM_AsyncLoader::ATOM_ASYNCLOAD_FILLED;
	if( bFilled )
	{
		ATOM_LOGGER::log( "has in filled stage \n" );
	}
	else
	{
		ATOM_LOGGER::log( "========== Failed! Even has not be filled =========\n" );
		return ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOCKED;
	}

	bool bAllFinished = flag_debug & ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED;
	if( bAllFinished )
	{
		ATOM_LOGGER::log( "has in all finished stage \n" );
	}
	else
	{
		ATOM_LOGGER::log( "========== Failed! Even has not be unlocked and finished =========\n" );
		return ATOM_AsyncLoader::ATOM_ASYNCLOAD_FILLED;
	}

		
	ATOM_LOGGER::log( "========== Succeeded! ========= \n" );
	return ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ATOM_AsyncableLoadObject::AddListener( ATOM_Node * listener )
{
	/*ATOM_ASSERT(listener!=0);

	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)>::iterator iter = listeners.begin();
	for( ; iter != listeners.end(); ++iter )
	{
		if( (*iter).get() == listener )
			return;
	}
	
	listeners.push_back(listener);*/
}
void ATOM_AsyncableLoadObject::RemoveListener( ATOM_Node * listener )
{
	/*ATOM_ASSERT(listener!=0);

	bool bfound = false;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)>::iterator iter = listeners.begin();
	for( ; iter != listeners.end(); ++iter )
	{
		if( (*iter).get() == listener )
		{
			bfound = true;
			break;
		}
	}
	if( bfound )
		listeners.erase(iter);*/
}
void ATOM_AsyncableLoadObject::ClearAllListeners()
{
	//listeners.clear();
}
/////////////////////////////////////////////////////////////////////////////
bool ATOM_AsyncableLoadObject::IsNotStartLoad()
{
	return ( flag == ATOM_AsyncLoader::ATOM_ASYNCLOAD_NOTLOADSTARTED );
}
bool ATOM_AsyncableLoadObject::IsStartLoad()
{
	return ( flag == ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADSTARTED );
}
bool ATOM_AsyncableLoadObject::IsInLoading()
{
	return ( flag == ATOM_AsyncLoader::ATOM_ASYNCLOAD_INLOADING );
}
bool ATOM_AsyncableLoadObject::IsDone( unsigned time )
{
	done_time = time;
	return ( flag == ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOADED );
}
bool ATOM_AsyncableLoadObject::IsLocked()
{
	return ( flag == ATOM_AsyncLoader::ATOM_ASYNCLOAD_LOCKED );
}
bool ATOM_AsyncableLoadObject::IsFilled()
{
	return ( flag == ATOM_AsyncLoader::ATOM_ASYNCLOAD_FILLED );
}
bool ATOM_AsyncableLoadObject::IsLoadAllFinished()
{
	bool ret =	( ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED == flag ) || 
				( ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED == result ) || 
				( ATOM_AsyncLoader:: ATOM_ASYNCLOAD_RES_CREATE_FAILED == result ); // && ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_FILEOPENFAILED == errorno */);
	return ret;
}
void ATOM_AsyncableLoadObject::SetLoadAllFinished()
{
	flag		= ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED;
	flag_debug	= ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALL;
}
unsigned int ATOM_AsyncableLoadObject::GetLoadStage()
{
	return flag;
}
void ATOM_AsyncableLoadObject::SetLoadStage(unsigned int stageid)
{
	flag = stageid;
}
unsigned int ATOM_AsyncableLoadObject::GetResult()
{
	return result;
}
unsigned int ATOM_AsyncableLoadObject::GetFailedError()
{
	return errorno;
}
bool ATOM_AsyncableLoadObject::IsLoadFailed()
{
	return	( ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED == result ) || 
			( ATOM_AsyncLoader:: ATOM_ASYNCLOAD_RES_CREATE_FAILED == result );
}
void ATOM_AsyncableLoadObject::processAllDependents()
{
	return;
}

//============================================================================//
//========================= ATOM_AsyncableLoadAttributeFile ==================//
//============================================================================//
ATOM_AsyncableLoadAttributeFile::ATOM_AsyncableLoadAttributeFile()
{

}
ATOM_AsyncableLoadAttributeFile::~ATOM_AsyncableLoadAttributeFile()
{
	GarbageCollect();
}
bool ATOM_AsyncableLoadAttributeFile::Load()
{
	if( __super::Load() == false )
		return false;

	if( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_INLOADING )
	{
		ATOM_LOGGER::error ("%s(%s) Async loading stage flag is not match.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH;		// 失败原因：加载阶段标记不匹配
		return false;
	}

	ATOM_AutoFile f(filename.c_str(), ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		ATOM_LOGGER::error ("%s(%s) file open failed.\n", __FUNCTION__, filename.c_str());

		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_FILEOPENFAILED;		// 失败原因：文件大小出错

		return false;
	}

	unsigned size = f->size ();
	if (!size)
	{
		ATOM_LOGGER::error ("%s(%s) file size is too small.\n", __FUNCTION__, filename.c_str());

		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_FILESIZETOOSMALL;	// 失败原因：文件大小出错

		return false;
	}

	size += 1;
	void* buffer = GetBuffer(size);
	ATOM_ASSERT(buffer!=0);
	if( buffer )
	{
		// 读取文件内容
		unsigned long size_read = f->read (buffer, size);
		if( size_read >= size )
		{
			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;			// 加载失败
			errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;	// 失败原因：读取内部错误

			GarbageCollect();

			ATOM_LOGGER::error ("%s(%s) file size is not correct.\n", __FUNCTION__, filename.c_str());
			return false;
		}

		( (char*)buffer )[size_read] = '\0';
	}
	else
	{
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;			// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;	// 失败原因：读取内部错误

		ATOM_LOGGER::error ("%s(%s) memory allocation failed.\n", __FUNCTION__, filename.c_str());
		return false;
	}

	// 获得读取的数据
	size = 0;
	char* data = (char*)GetBuffer(size);

	// 如果数据存在
	if( size > 0 && data )
	{
		// 解析数据
		ATOM_TiXmlDocument & doc = ( (ATOM_NodeAttributeFile*)host )->getDocument();
		doc.Parse ( data );

		if (doc.Error ())
		{
			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;		// 解析失败
			errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;	// 失败原因：内部错误

			// 释放
			GarbageCollect();

			ATOM_LOGGER::error ("%s(%s) XML parse failed.\n", __FUNCTION__, filename.c_str());
			return false;
		}
	}
	else
	{
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// 解析失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;		// 失败原因：内部错误

		ATOM_LOGGER::error ("%s(%s) No data buffer.\n", __FUNCTION__, filename.c_str());
		return false;
	}

	// 释放
	GarbageCollect();

	if( host )
	{
		ATOM_NodeAttributeFile* af = (ATOM_NodeAttributeFile*)host;
		if( af )
			af->OnFileLoadFinished();
	}
	

	// 设置加载完成标记
	SetLoadAllFinished();

	return true;
}


/////////////////////////////////////////////////////////////////////////////////

ATOM_NodeAttributeFile::ATOM_NodeAttributeFile()
{

}
ATOM_NodeAttributeFile::~ATOM_NodeAttributeFile()
{
	if( ATOM_AsyncLoader::isEnableLog() )
	{
		ATOM_LOGGER::log("<%s> : %s [ Address : %x ] is Destroyed \n",	__FUNCTION__, 
																		getObjectName() ? getObjectName() : "",
																		_asyncLoader.get() ? (unsigned)_asyncLoader.get() : 0	);
	}
	// 将host设置为0
	if( _asyncLoader.get() )
		_asyncLoader->host = 0;
	ATOM_AsyncLoader::OnAsyncLoadObjectFinished(_asyncLoader.get());
}
bool ATOM_NodeAttributeFile::loadSync(const char* file)
{
	if( !file || file[0]=='\0' )
	{
		ATOM_LOGGER::error ("<%s> load attribute file failed : file name is null!\n", __FUNCTION__);
		getAsyncLoader()->result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;
		return false;
	}
	
	// 直接读取属性文件
	ATOM_AutoFile f(file, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		ATOM_LOGGER::error ("<%s> load attribute file failed (%s) : file is not exist!\n", __FUNCTION__, file);
		getAsyncLoader()->result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;
		return false;
	}
	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';

	_doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (_doc.Error ())
	{
		ATOM_LOGGER::error ("<%s> load attribute file failed (%s) : XML Parse error!\n", __FUNCTION__, file);
		getAsyncLoader()->result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;
		return false;
	}

	// 设置加载完成
	getAsyncLoader()->host		= this;
	getAsyncLoader()->filename	= file;
	getAsyncLoader()->priority	= ATOM_LoadPriority_IMMEDIATE;
	getAsyncLoader()->SetLoadAllFinished();

	return true;
}
bool ATOM_NodeAttributeFile::loadAsync(const char* filename, const int loadPriority)
{
	return getAsyncLoader()->Start(filename,loadPriority);
}
ATOM_TiXmlDocument & ATOM_NodeAttributeFile::getDocument()
{
	return _doc;
}
const ATOM_TiXmlDocument & ATOM_NodeAttributeFile::getDocument() const
{
	return _doc;
}
ATOM_AsyncableLoadAttributeFile * ATOM_NodeAttributeFile::getAsyncLoader()
{
	if (!_asyncLoader)
	{
		_asyncLoader = ATOM_NEW(ATOM_AsyncableLoadAttributeFile);
		_asyncLoader->host = this;
	}
	return _asyncLoader.get();
}


//=======================================================================//
//============================ IO 线程 ==================================//
//=======================================================================//

ATOM_BEGIN_EVENT_MAP(ATOM_AsyncLoadIOThread, ATOM_Thread)
	ATOM_EVENT_HANDLER(ATOM_AsyncLoadIOThread, ATOM_ThreadIdleEvent, onIdle)
ATOM_END_EVENT_MAP
ATOM_AsyncLoadIOThread::ATOM_AsyncLoadIOThread(int _io_count_per_pro, unsigned id)
{
	runflag = 1;
	swapFlag = 1;
	abandonflag = 0;

	totalTaskCount = 0;
	io_count_per_procedure = _io_count_per_pro;

	m_id = id;
}
ATOM_AsyncLoadIOThread::~ATOM_AsyncLoadIOThread()
{
	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::debug(" +++++++++++++ ATOM_AsyncLoadIOThread is destroyed +++++++++++++++\n");

	{
		ATOM_Mutex::ScopeMutex lock(m_lock);
		m_taskQueue.clear();
	}

	runflag = 0;
}

void ATOM_AsyncLoadIOThread::AddTask( ATOM_AsyncableLoadObject * obj )
{
	ATOM_Mutex::ScopeMutex lock(m_lock);

	m_taskQueue.insert(obj);

	#if DEBUG_ASYNCLOADING_IOQUEUE
		ATOM_LOGGER::log("+++++++++++++++++++++++++ IO QUEUE CONTENT START ++++++++++++++++++++++++ \n " );
		CREATE_RES_QUEUE_SORT::iterator iter = m_taskQueue.begin();
		for( ; iter != m_taskQueue.end(); ++iter )
		{
			ATOM_AsyncableLoadObject * o = *iter;
			if( o )
				ATOM_LOGGER::log("+++ %s is in io thread queue, the priority is %d ! +++ \n ", o->filename.c_str(), o->GetPriority() );
		}
		ATOM_LOGGER::log("+++++++++++++++++++++++++ IO QUEUE CONTENT END ++++++++++++++++++++++++ \n " );
	#endif
	
	//::InterlockedExchangeAdd (&totalTaskCount, 1);
}

void ATOM_AsyncLoadIOThread::onIdle (ATOM_ThreadIdleEvent *event)
{
	if( runflag )
	{
		if(m_taskQueue.empty())
		{
			::Sleep(50);

			if(!runflag && !abandonflag)
				abandonflag = 1;

			return;
		}
		
		ATOM_AsyncableLoadObject * object = 0;
		int count = ATOM_min2( io_count_per_procedure, m_taskQueue.size() );

		if( ATOM_AsyncLoader::isEnableLog() )
		{
			ATOM_Mutex::ScopeMutex lock(m_lock);

			ATOM_LOGGER::debug("++++++++++++++++++++++++ %d objects total is in IO queue +++++++++++++++++++\n", (int)m_taskQueue.size());

			CREATE_RES_QUEUE_SORT::const_iterator iter = m_taskQueue.begin();
			for( ; iter != m_taskQueue.end(); ++iter )
			{
				object = *iter;
				ATOM_LOGGER::debug( "****** object %s is in queue ******\n",object ? object->filename.c_str() : " object is not valid " );
			}

			ATOM_LOGGER::debug("-------------------------- %d objects needs to be process ------------------------------\n",count);
		}

		for( int i = 0; i < count; ++i )
		{
			{
				ATOM_Mutex::ScopeMutex lock(m_lock);

				object = (*m_taskQueue.begin());

				if( ATOM_AsyncLoader::isEnableLog() )
				{
					ATOM_LOGGER::debug("==== object %s  ====\n",object ? object->filename.c_str() : " object is not valid " );
				}

				m_taskQueue.erase(m_taskQueue.begin());
			}

			if( object && object->IsStartLoad() )
			{
				DWORD startTick = ::GetTickCount ();

				if( object->Load() )
				{
					ATOM_AsyncLoader::TaskDone( object );
				}
				else
				{
					ATOM_LOGGER::error ("%s(%s) Load Failed the error num is : %u.\n", __FUNCTION__, object->filename.c_str(), object->GetFailedError() );
				}

				

				//=========================================//
				if( ATOM_AsyncLoader::isEnableStatistics() )
				{
					_statistics.resize(_statistics.size()+1);
					_statistics.back()._timeForLoad = ::GetTickCount() - startTick;
					_statistics.back()._filename = object->filename;
					_statistics.back()._loadPriority = object->priority;
				}
				//=========================================//

			}	
		}

		if( ATOM_AsyncLoader::isEnableLog() )
		{
			ATOM_LOGGER::debug("++++++++++++++++++++++++ ++++++++++++++++++ +++++++++++++++++++\n",count);
		}

	}

	if( !runflag && !abandonflag )
		abandonflag = 1;
}

unsigned int ATOM_AsyncLoadIOThread::GetTaskCount()
{
	/*ATOM_Mutex::ScopeMutex lock(m_lock);
	return m_taskQueue_W.size();*/
	return 0;
}

void ATOM_AsyncLoadIOThread::AbandonAll()
{
	// 等待放弃标记为真
	while( !abandonflag );

	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::debug(" +++++++++++++ ATOM_AsyncLoadIOThread::AbandonAll is called +++++++++++++++\n");

	// 清除所有当前余下的还没有加载的任务
	{
		ATOM_Mutex::ScopeMutex lock(m_lock);

		if( ATOM_AsyncLoader::isEnableLog() )
		{
			ATOM_LOGGER::log("++++++++++++++++++++++++ ATOM_AsyncLoadIOThread::AbandonAll --- START Abandon All unload object +++++++++++++++++++++\n");

			CREATE_RES_QUEUE_SORT::iterator iter = m_taskQueue.begin();
			while(iter!=m_taskQueue.end())
			{
				CREATE_RES_QUEUE_SORT::iterator current = iter++;
				ATOM_AsyncableLoadObject * object = *current;

				// 如果已经锁定
				if( object )
				{
					ATOM_LOGGER::log("%s : <address : %x> \n", object->filename.c_str(), (unsigned)object);
				}
			}
			ATOM_LOGGER::log("++++++++++++++++++++++++ ATOM_AsyncLoadIOThread::AbandonAll --- END Abandon All unload object +++++++++++++++++++++\n");
		}

		// 清空队列
		m_taskQueue.clear();
	}

	// 恢复放弃标记
	//abandonflag = 0;

	/////////////////////////////////////////////////////
	if( ATOM_AsyncLoader::isEnableStatistics() )
	{
		ATOM_LOGGER::debug("============================== %s ==============================\n", ( m_id == DISK_IO_THREADID ) ? "DISK IO THREAD" : "DOWNLOAD IO THREAD" );
		for(int i = 0; i < _statistics.size(); ++i )
		{
			ATOM_LOGGER::debug("%s [ priority : %d ] loaded in %d(ms).\n",
				_statistics[i]._filename.c_str(),
				_statistics[i]._loadPriority,
				_statistics[i]._timeForLoad);
		}
		ATOM_LOGGER::debug("================================================================\n");
		_statistics.clear();
	}
	/////////////////////////////////////////////////////
}
void ATOM_AsyncLoadIOThread::AbandonTask(ATOM_AsyncableLoadObject * obj)
{
	
}

void ATOM_AsyncLoadIOThread::Pause()	
{	
	runflag=0;
}
void ATOM_AsyncLoadIOThread::Resume()
{	
	runflag=1;
	abandonflag = 0;
}
void ATOM_AsyncLoadIOThread::PauseSwap() 
{	
	swapFlag = 0;
}
void ATOM_AsyncLoadIOThread::ResumeSwap() 
{	
	swapFlag = 1;
}

//============================================================================//
//============================ 资源填充 线程 ==================================//
//============================================================================//

ATOM_BEGIN_EVENT_MAP(ATOM_AsyncResFillThread, ATOM_Thread)
	ATOM_EVENT_HANDLER(ATOM_AsyncResFillThread, ATOM_ThreadIdleEvent, onIdle)
ATOM_END_EVENT_MAP
ATOM_AsyncResFillThread::ATOM_AsyncResFillThread(int _fillcount_per_pro)
{
	runflag = 1;
	swapFlag = 1;
	abandonflag = 0;

	totalTaskCount = 0;
	fillcount_per_procedure = _fillcount_per_pro;
}
ATOM_AsyncResFillThread::~ATOM_AsyncResFillThread()
{	
	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::debug(" +++++++++++++ ATOM_AsyncResFillThread is destroyed +++++++++++++++\n");

	{
		ATOM_Mutex::ScopeMutex lock(m_lock);
		m_taskQueue.clear();
	}

	runflag = 0;
}

void ATOM_AsyncResFillThread::AddTask( ATOM_AsyncableLoadObject * obj )
{

	{
		ATOM_Mutex::ScopeMutex lock(m_lock);

#if USE_TWO_IO_THREAD
		m_taskQueue.push_back(obj);
#else
		m_taskQueue.insert(obj);
#endif
	}
	
	//::InterlockedExchangeAdd (&totalTaskCount, 1);
}

void ATOM_AsyncResFillThread::onIdle (ATOM_ThreadIdleEvent *event)
{
	if( runflag )
	{
		if(m_taskQueue.empty())
		{
			::Sleep(50);

			// 如果暂停，设置放弃标记
			if( !runflag && !abandonflag )
				abandonflag = 1;

			return;
		}

		ATOM_AsyncableLoadObject * object = 0;
		int count = ATOM_min2( fillcount_per_procedure, m_taskQueue.size() );
		for( int i = 0; i < count; ++i )
		{
			{
				ATOM_Mutex::ScopeMutex lock(m_lock);

#if USE_TWO_IO_THREAD
				object = m_taskQueue.front();
				m_taskQueue.pop_front();
#else
				object = (*m_taskQueue.begin());
				m_taskQueue.erase(m_taskQueue.begin());
#endif
			}

			// 是否已经锁定
			if( object && object->IsLocked() )
			{
				// 填充
				if( object->Fill() )
				{
					// 交给主线程UNLOCK
					ATOM_AsyncLoader::ResFillDone( object );
				}
				else
				{
					ATOM_LOGGER::error ("%s(%s) Fill Failed the error num is : %u.\n", __FUNCTION__, object->filename.c_str(), object->GetFailedError() );
				}
			}
		}
	}

	// 如果暂停，设置放弃标记
	if( !runflag && !abandonflag )
		abandonflag = 1;
}
unsigned int ATOM_AsyncResFillThread::GetTaskCount()
{
	/*ATOM_Mutex::ScopeMutex lock(m_lock); 
	return m_taskQueue_W.size();*/
	return 0;
}

void ATOM_AsyncResFillThread::AbandonAll()
{
	// 等待放弃标记为真
	while(!abandonflag);

	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::debug(" +++++++++++++ ATOM_AsyncResFillThread::AbandonAll is called +++++++++++++++\n");

	if( ATOM_AsyncLoader::isEnableLog() )
	{
		ATOM_LOGGER::log("++++++++++++++++++++++++ ATOM_AsyncResFillThread::AbandonAll --- START Abandon All locked object +++++++++++++++++++++\n");

#if USE_TWO_IO_THREAD
		ATOM_DEQUE<ATOM_AsyncableLoadObject*>::iterator iter = m_taskQueue.begin();
		while(iter!=m_taskQueue.end())
		{
			ATOM_DEQUE<ATOM_AsyncableLoadObject*>::iterator current = iter++;
			ATOM_AsyncableLoadObject * object = *current;

			// 如果已经锁定
			if( object )
			{
				ATOM_LOGGER::log(	"%s : <address : %x> \n", object->filename.c_str(), (unsigned)object	);

				// 不对其进行处理 因为之后还需要将其放入FILL队列重新FILL
				//// 设置FILLED标记
				//object->SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_FILLED);
				//// 解锁
				//object->Unlock();
			}
		}
#else
		CREATE_RES_QUEUE_SORT::iterator iter = m_taskQueue.begin();
		while(iter!=m_taskQueue.end())
		{
			CREATE_RES_QUEUE_SORT::iterator current = iter++;
			ATOM_AsyncableLoadObject * object = *current;

			// 如果已经锁定
			if( object )
			{
				ATOM_LOGGER::log(	"%s : <address : %x> \n", object->filename.c_str(), (unsigned)object	);

				// 不对其进行处理 因为之后还需要将其放入FILL队列重新FILL
				//// 设置FILLED标记
				//object->SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_FILLED);
				//// 解锁
				//object->Unlock();
			}
		}
#endif
		
		ATOM_LOGGER::log("++++++++++++++++++++++++ ATOM_AsyncResFillThread::AbandonAll --- END Abandon All locked object +++++++++++++++++++++\n");
	}

	// 清空队列
	m_taskQueue.clear();

	// 恢复放弃标记
	//abandonflag = 0;
}
void ATOM_AsyncResFillThread::AbandonTask(ATOM_AsyncableLoadObject * obj)
{
	
}

// 暂停
void ATOM_AsyncResFillThread::Pause()	
{	
	runflag=0;			// 设置运行标记为0	
}
// 继续
void ATOM_AsyncResFillThread::Resume()	
{	
	runflag=1;			// 设置运行标记为1
	abandonflag = 0;	// 设置放弃标记为0
}
void ATOM_AsyncResFillThread::PauseSwap()
{	
	swapFlag = 0; 
}
void ATOM_AsyncResFillThread::ResumeSwap() 
{	
	swapFlag = 1; 
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

ATOM_AsyncLoader::ATOM_AsyncLoader()
{
	
}
ATOM_AsyncLoader::~ATOM_AsyncLoader()
{
	
}

void ATOM_AsyncLoader::Run(AsyncLoaderRunParams & param)
{
	if( m_run )
		return;

	/*DWORD_PTR process_affinity_mask;
	unsigned int  thread_processor_mask = 1;
	process_affinity_mask = 1;
	unsigned int LogicalProcessors(1), NumAvailProcessors(1);
	SYSTEM_INFO   sys_info;
	DWORD_PTR system_affinity_mask;
	GetSystemInfo( &sys_info );
	LogicalProcessors = sys_info.dwNumberOfProcessors;
	NumAvailProcessors = LogicalProcessors;
	GetProcessAffinityMask( GetCurrentProcess(), &process_affinity_mask, &system_affinity_mask );
	SetProcessAffinityMask( GetCurrentProcess(), process_affinity_mask );
	unsigned int numSysCores(1), numProcessCores(1);
	GetNumCPUCores(numSysCores, numProcessCores);
	ATOM_LOGGER::log("Total number of logical processors: %u", LogicalProcessors);
	ATOM_LOGGER::log("Number of available logical processors: %u", NumAvailProcessors);
	ATOM_LOGGER::log("Total number of system cores: %u \n", numSysCores);
	ATOM_LOGGER::log("Number of cores available to process: %u \n", numProcessCores);*/

	{
		const unsigned int iocount = param._io_thread_count;//NumAvailProcessors;
		if( iocount == 0 )
			return;

		m_iocount_per_procedure = param._io_count_per_iterate;

		//////////////////////////////////////////////////////////////////////////////////////////////
#if !USE_TWO_IO_THREAD
		for( int i = 0; i < iocount; ++i )
			m_ioThreads.push_back( ATOM_NEW( ATOM_AsyncLoadIOThread,m_iocount_per_procedure, i ) );
#else
		for( int i = 0; i < 2; ++i )
			m_ioThreads.push_back( ATOM_NEW( ATOM_AsyncLoadIOThread,m_iocount_per_procedure, i ) );
#endif
		//////////////////////////////////////////////////////////////////////////////////////////////

		ATOM_VECTOR<ATOM_AsyncLoadIOThread*>::iterator iter = m_ioThreads.begin();
		for( ; iter != m_ioThreads.end(); ++iter )
		{
			ATOM_AsyncLoadIOThread * t = *iter;
			if( t )
			{
				//t->setFrameInterval(10);
				t->run();
			}
		}
	}
	
	{
		const unsigned int fillcount = param._fill_thread_count;//NumAvailProcessors;
		if( fillcount == 0 )
			return;

		m_fillcount_per_procedure = param._fill_count_per_iterate;
		for( int i = 0; i < fillcount; ++i )
			m_fillThreads.push_back( ATOM_NEW( ATOM_AsyncResFillThread, m_fillcount_per_procedure ) );

		ATOM_VECTOR<ATOM_AsyncResFillThread*>::iterator iter = m_fillThreads.begin();
		for( ; iter != m_fillThreads.end(); ++iter )
		{
			ATOM_AsyncResFillThread * t = *iter;
			if( t )
			{
				//t->setFrameInterval(10);
				t->run();
			}
		}
	}

	m_lockcount_per_procedure		= param._lock_count_per_iterate;
	m_maxcount_requesterProcess		= param._maxcount_requester_process;
	m_reprocess_priority_threshold	= param._reprocess_priority_threshold;

	setEnableLog(0);
	setEnableSceneMTLoad(0);
	ATOM_VECTOR<ATOM_STRING> args;
	ATOM_GetCommandLine (args);
	for (unsigned i = 0; i < args.size(); ++i)
	{
		if (args[i] == "--mtloadlog")
		{
			setEnableLog(1);
		}
		if (args[i] == "--scenemtload")
		{
			setEnableSceneMTLoad(1);
		}
	}

	m_funcCheckFile = param._funcCheckFile;

	m_run = true;
}

bool ATOM_AsyncLoader::IsRun()
{
	return m_run;
}

void ATOM_AsyncLoader::Stop()
{
	if( !m_run )
		return;

	m_run = false;

	{
		ATOM_VECTOR<ATOM_AsyncLoadIOThread*>::iterator iter = m_ioThreads.begin();
		for( ; iter != m_ioThreads.end(); ++iter )
		{
			ATOM_AsyncLoadIOThread * t = *iter;
			if( t )
			{
				t->terminate(2000,0);		// 等待IO线程结束,2秒之后还未结束,强制终结
				ATOM_DELETE(t);
			}
			
		}
		m_ioThreads.clear();
	}

	{
		ATOM_VECTOR<ATOM_AsyncResFillThread*>::iterator iter = m_fillThreads.begin();
		for( ; iter != m_fillThreads.end(); ++iter )
		{
			ATOM_AsyncResFillThread * t = *iter;
			if( t )
			{
				t->terminate(2000,0);		// 等待Fill线程结束,2秒之后还未结束,强制终结
				ATOM_DELETE(t);
			}
		}
		m_fillThreads.clear();
	}

	ClearAllGlboalPreLoads();

	m_asyncLoadResources.clear();
	m_asyncLoadHosts.clear();
	m_asyncLoadPriorities.clear();

	m_createResQueue.clear();
	m_fillResQueue.clear();

}
void ATOM_AsyncLoader::setEnableLog(int bEnable)
{
	m_enable_log = bEnable;
}
int ATOM_AsyncLoader::isEnableLog()
{
	return m_enable_log;
}
void ATOM_AsyncLoader::enableDoubleIOThread(bool bEnable)
{
	m_use_two_io_threads = bEnable;
}
bool ATOM_AsyncLoader::isEnableDoubleIOThread()
{
	return m_use_two_io_threads;
}

void ATOM_AsyncLoader::setEnableStatistics(int bEnable)
{
	m_enable_statistics = bEnable;
}
int ATOM_AsyncLoader::isEnableStatistics()
{
	return m_enable_statistics;
}

void ATOM_AsyncLoader::setEnableSceneMTLoad(int bEnable)
{
	m_enable_scene_mtload = bEnable;
}
int ATOM_AsyncLoader::isEnableSceneMTLoad()
{
	return m_run && m_enable_scene_mtload;
}

void ATOM_AsyncLoader::setSceneMTLoadPriorityHigh(int pri)
{
	m_scene_mtload_priority_highest = pri;
}
int ATOM_AsyncLoader::getSceneMTLoadPriorityHigh()
{
	return m_scene_mtload_priority_highest;
}

void ATOM_AsyncLoader::AddAsyncTask( ATOM_AsyncableLoadObject * obj )
{
	//return;

	if( !m_run )
		return;
	if( !obj )
		return;

	// 资源
	m_asyncLoadResources.insert( std::make_pair( obj->filename, obj ) );

	// 递增资源的引用
	m_asyncLoadHosts.insert( std::make_pair( obj->filename, obj->host ) );

	/////////////////////////////////////////////////////////////////
	// 分配任务
	m_requestEvents.AssignTask(obj);

	/////////////////////////////////////////////////////////////////
	if( ATOM_AsyncLoader::isEnableDoubleIOThread() )
	{
		// 添加加载任务到IO线程
#if 0
		if( CheckFileLocal(obj->filename.c_str()) )
#else
		if( ATOM_CheckFileExistence( obj->filename.c_str() ) )
#endif
		{
			m_ioThreads[DISK_IO_THREADID]->AddTask(obj);				// 如果文件在本地 添加到本地IO线程
		}
		else
		{
			m_ioThreads[DOWNLOAD_IO_THREADID]->AddTask(obj);			// 如果文件在本地 添加到本地IO线程
		}
	}
	else
	{
		m_ioThreads[DISK_IO_THREADID]->AddTask(obj);					// 添加到本地IO线程
	}
	/////////////////////////////////////////////////////////////////

	/*::InterlockedExchangeAdd (&g_index, 1);
	volatile long size = m_ioThreads.size();
	::InterlockedCompareExchange(&g_index,0,size);*/
}

void ATOM_AsyncLoader::TaskDone( ATOM_AsyncableLoadObject * obj )
{
	if( !m_run )
		return;
	if( !obj )
		return;


	ATOM_Mutex::ScopeMutex lock(m_lock);

#if USE_TWO_IO_THREAD
	m_createResQueue.push_back(obj);
#else
	m_createResQueue.insert(obj);
#endif

}

void ATOM_AsyncLoader::AddFillTask( ATOM_AsyncableLoadObject * obj )
{
	if( !m_run )
		return;
	if( !obj )
		return;

	/////////////////////////////////////////////////////////////////
	// 添加加载任务到填充线程
	m_fillThreads[g_index]->AddTask(obj);

	::InterlockedExchangeAdd (&g_index, 1);
	volatile long size = m_fillThreads.size();
	::InterlockedCompareExchange(&g_index,0,size);
}

void ATOM_AsyncLoader::ResFillDone( ATOM_AsyncableLoadObject * obj )
{
	if( !m_run )
		return;
	if( !obj )
		return;

	ATOM_Mutex::ScopeMutex lock(m_lock_fill);

#if USE_TWO_IO_THREAD
	m_fillResQueue.push_back(obj);
#else
	m_fillResQueue.insert(obj);
#endif
}

void ATOM_AsyncLoader::_abandonAllTasks(bool bDestroyAll)
{

#if 1
	bDestroyAll = true;
#endif

	if( !m_run )
		return;

	// 如果不放弃所有未完成的加载 降低当前所有加载对象的优先级
#if DEBUG_ASYNCLOADING_PRIORITY_DOWN
	if( !bDestroyAll )
	{
		PriorityDown();
	}
	// 放弃当前所有未完成的加载 清除所有加载对象和资源
	else
#endif
	{
		//====================== 暂停IO线程的处理并且清空IO队列 ========================================//
		{
		
			ATOM_VECTOR<ATOM_AsyncLoadIOThread*>::iterator iter = m_ioThreads.begin();
			for( iter; iter != m_ioThreads.end(); ++iter )
			{
				ATOM_AsyncLoadIOThread* io_thread = *iter;
				if( io_thread )
				{
					// 暂停执行IO线程
					io_thread->Pause();

					// 放弃所有的加载请求(如果已经有正在进行的请求，让请求加载完）
					io_thread->AbandonAll();
				}
			}
		}

		//====================== 暂停Fill线程的处理并且清空FILL队列 ========================================//
		{
			ATOM_VECTOR<ATOM_AsyncResFillThread*>::iterator iter = m_fillThreads.begin();
			for( iter; iter != m_fillThreads.end(); ++iter )
			{
				ATOM_AsyncResFillThread* fill_thread = *iter;
				if( fill_thread )
				{
					// 暂停执行IO线程
					fill_thread->Pause();

					// 放弃所有的加载请求(如果已经有正在进行的请求，让请求加载完）
					fill_thread->AbandonAll();
				}
			}
		}

		//================================================================================//
		// 清除掉主线程中的剩余的加载对象
		{
			if( ATOM_AsyncLoader::isEnableLog() )
				ATOM_LOGGER::log("++++++++++++++++++++++++ %s : START Unlock All filled object ++++++++++++++++++\n",__FUNCTION__);

			m_createResQueue.clear();

			// 注意： 这里调用了对象的unlock方法，如果是模型对象可能导致加载其依赖纹理，而IO线程的处理队列可能会添加新的IO对象

#if USE_TWO_IO_THREAD
			ATOM_DEQUE<ATOM_AsyncableLoadObject*>::iterator iter_fill = m_fillResQueue.begin();
#else
			CREATE_RES_QUEUE_SORT::iterator iter_fill = m_fillResQueue.begin();
#endif

			for (; iter_fill != m_fillResQueue.end(); ++iter_fill )
			{
				ATOM_AUTOPTR(ATOM_AsyncableLoadObject) object = *iter_fill;
				if( object )
					object->Unlock();
			}
			m_fillResQueue.clear();

			if( ATOM_AsyncLoader::isEnableLog() )
				ATOM_LOGGER::log("++++++++++++++++++++++++ %s : END Unlock All filled object ++++++++++++++++++\n",__FUNCTION__);
		}

		//====================== 再次清空IO线程的处理队列 ========================================//
		// 因为在清除主线程的对象时调用了unlock,而MODEL的unlock方法可能会导致加载模型的依赖纹理对象
		// 这样IO线程的处理队列可能会添加新的IO对象，所以这里需要再次清除一次
		{

			ATOM_VECTOR<ATOM_AsyncLoadIOThread*>::iterator iter = m_ioThreads.begin();
			for( iter; iter != m_ioThreads.end(); ++iter )
			{
				ATOM_AsyncLoadIOThread* io_thread = *iter;
				if( io_thread )
				{
					// 清空队列
					io_thread->AbandonAll();
				}
			}
		}

		//====================== 再次清空FILL线程的处理队列 ========================================//
		{
			ATOM_VECTOR<ATOM_AsyncResFillThread*>::iterator iter = m_fillThreads.begin();
			for( iter; iter != m_fillThreads.end(); ++iter )
			{
				ATOM_AsyncResFillThread* fill_thread = *iter;
				if( fill_thread )
				{
					// 清空队列
					fill_thread->AbandonAll();
				}
			}
		}
		
		//============================= 递减所有资源的引用计数（解锁）====================//

		m_asyncLoadHosts.clear();

		//============================= 递减所有资源的引用计数（解锁）====================//

		/*if( m_scene && m_scene->getRootNode() )
		{
			m_scene->getRootNode()->removeChild(m_scene->getRootNode()->getChild(0));
		}*/

		//===============================清空所有的请求事件================================//
		{
			m_requestEvents.AbandonAll(true);
		}

		//================================================================================//

#if DEBUG_ASYNCLOADING_STATISTICS

		//CheckInvalidObjects();

#endif

#if 1

		// 临时集合
		ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)> temp_objects;
		std::swap(temp_objects,m_asyncLoadResources);

		bool bHas = false;
		
		ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>::iterator iter = temp_objects.begin();
		for( ; iter != temp_objects.end(); ++iter )
		{
			ATOM_STRING name = iter->first;

			//ATOM_AUTOREF(ATOM_AsyncableLoadObject) obj = iter->second;
			ATOM_AUTOPTR(ATOM_AsyncableLoadObject) obj = iter->second;

			// 如果该对象尚未完全加载完成（包括该对象本身未加载完成，或者其所依赖对象未加载完成）
			if( obj && !obj->IsLoadAllFinished() )
			{
				if( !bHas )
				{
					ATOM_LOGGER::log("+++++++++++++++++++++++ interrupted Asyncable Load object +++++++++++++++++++++++++++++\n");
					bHas = true;
				}
				
				// 如果该对象的优先级小于“需要重新处理优先级”
				if( obj->priority < m_reprocess_priority_threshold )
				{
					ATOM_LOGGER::debug( "[ Address : %x ] %s priority %d is so low that should not be reprocessed!\n", 
										(unsigned)(obj.get()), 
										obj->filename.c_str(),
										obj->priority	);

					obj->abandoned = true;

					continue;
				}
				//---------------------------------------------------//

#if 0
				if( obj->host )
				{
					if( isEnableLog() )
					{
						ATOM_LOGGER::debug( "processing object [ Address : %x, priority %d ]\n", (unsigned)(obj.get()), obj->priority );
						const char* classname	= obj->host->getClassName() ? obj->host->getClassName() : "";
						const char* objectname	= obj->host->getObjectName() ? obj->host->getObjectName() : "";
						ATOM_LOGGER::log("%s [%s] should be re processed!\n", objectname, classname );
					}

					m_asyncLoadHosts.insert( std::make_pair( obj->filename, obj->host ) );
				}
				else
				{
					if( isEnableLog() )
					{
						ATOM_LOGGER::error( "object [ name : %s , Address : %x, priority %d ] has no host \n", 
											obj->filename.c_str(),
											(unsigned)(obj.get()), 
											obj->priority );
					}
				}

				// 重新添加进资源集合中
				m_asyncLoadResources.insert( std::make_pair( obj->filename, obj ) );

#if 0
				unsigned flag = obj->CheckStageFlagDebug();
#else
				unsigned flag = obj->flag;					// 得到标记
#endif

				if( flag != ATOM_ASYNCLOAD_ALLFINISHED )	// 如果标记为未加载完成
				{
					switch( flag )
					{
					case ATOM_ASYNCLOAD_NOTLOADSTARTED:	// 还未开始（如果已经start,一般不可能出现此情况）
					case ATOM_ASYNCLOAD_LOADSTARTED:	// 已经开始
					case ATOM_ASYNCLOAD_INLOADING:		// 在加载中
						{
							if( ATOM_AsyncLoader::isEnableLog() )
								ATOM_LOGGER::log("%s is started and need to load!\n",obj->filename.c_str());

							AddAsyncTask( obj.get() );			// 再次添加到IO队列
						}
						break;
					case ATOM_ASYNCLOAD_LOADED:			// 已经加载 
						{
							if( ATOM_AsyncLoader::isEnableLog() )
								ATOM_LOGGER::log("%s is loaded and need to lock!\n",obj->filename.c_str());

							bool bRec = false;
							obj->Lock(bRec);			// 锁
							
						}
						break;
					case ATOM_ASYNCLOAD_LOCKED:			// 已经锁
						{
							if( ATOM_AsyncLoader::isEnableLog() )
								ATOM_LOGGER::log("%s is locked and need to fill!\n",obj->filename.c_str());

							ATOM_AsyncLoader::AddFillTask( obj.get() );	// 添加到填充队列
						}
						break;
					case ATOM_ASYNCLOAD_FILLED :		// 已经填充
						{
							if( ATOM_AsyncLoader::isEnableLog() )
								ATOM_LOGGER::log("%s is filled and need to unlock!\n",obj->filename.c_str());

							ATOM_AsyncLoader::ResFillDone( obj.get() );	// 解锁队列
						}
						break;
					}
				}

				//-------------------------------------------------------------------------//
				// 如果该对象自身资源已经加载完成了，但是并未完全加载完成（因为依赖项没有加载完成）
				// 那么检查依赖项。
				// 如果依赖性被卸载了，重新加载依赖项。
				obj->processAllDependents();
				//-------------------------------------------------------------------------//

#else

				ReProcessLoadObject(obj.get());

#endif
			}
		}

		if( ATOM_AsyncLoader::isEnableLog() && bHas )
			ATOM_LOGGER::log("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

		//============================ 恢复执行FILL线程 =======================================//
		{
			ATOM_VECTOR<ATOM_AsyncResFillThread*>::iterator iter = m_fillThreads.begin();
			for( iter; iter != m_fillThreads.end(); ++iter )
			{
				if( (*iter) )
					(*iter)->Resume();
			}
		}

		//========================== 恢复执行IO线程 ======================================//
		{
			ATOM_VECTOR<ATOM_AsyncLoadIOThread*>::iterator iter = m_ioThreads.begin();
			for( iter; iter != m_ioThreads.end(); ++iter )
			{
				if( (*iter) )
					(*iter)->Resume();
			}
		}

#else	

		m_asyncLoadResources.clear();

#endif

	}
}
void ATOM_AsyncLoader::AbandonAllTask(bool bDestroyAll)
{
	if( !m_run )
		return;

	//=================== 如果当前场景，放弃所有加载任务(没有完成的请求继续完成） =//
	_abandonAllTasks(bDestroyAll);

	m_scene = 0;

	// 垃圾收集 FRAME ID
	m_garbageCollectFrameID = 0;

	// destroy all when quit
	if( bDestroyAll )
	{
		Stop();
	}
}

void ATOM_AsyncLoader::AbandonTask(ATOM_AsyncableLoadObject* obj)
{
	
}

void ATOM_AsyncLoader::Update(bool bCountFrame)
{
	if( !m_run )
		return;

	GarbageCollect();

	//===================================================//
	// 检查所有的请求者请求是否完成
	CheckAllRequester();
	//===================================================//


	{
		ATOM_AUTOPTR(ATOM_AsyncableLoadObject) object = 0;
		int count = ATOM_min2( m_lockcount_per_procedure, m_createResQueue.size() );

		if( ATOM_AsyncLoader::isEnableLog() && count > 0 )
			ATOM_LOGGER::log("++++++++++++++++++++ the lock count is : %u +++++++++++++++++++++++\n", count );

		for( int i = 0; i < count; ++i )
		{
			// 锁定
			bool needRecurse = false;

			{
				ATOM_Mutex::ScopeMutex lock(m_lock);

#if USE_TWO_IO_THREAD
				object = m_createResQueue.front();
				m_createResQueue.pop_front();
#else
				object = (*m_createResQueue.begin());
				m_createResQueue.erase(m_createResQueue.begin());
#endif
			}
			
			if( object )
			{
				// 如果加载完成
				if( object->IsDone(0) )
				{
					if( !object->Lock(needRecurse) )
					{
						// 设置Filled标志
						object->SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_FILLED);

						// 失败，解锁
						object->Unlock();

						ATOM_LOGGER::error ("%s(%s) Something is failed when Lock DEVICE BUFFER!.\n", __FUNCTION__, object->filename.c_str() );
					}
					//if( needRecurse )
					//	break;
				}
				else
				{
					//ATOM_LOGGER::error ("%s(%s) There is object not loaded feeding to the creation stage! : %u.\n", __FUNCTION__, object->filename.c_str() );
				}	

				//object->inQueueFlag = 0;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// unlock
	{
		if( m_fillResQueue.empty() )
		{
			if( bCountFrame )
			{
				++m_frameCount;

				// 如果在一段时间内 没有加载的任务 则表示所有场景对象都加载完成 
				// 此时 计算整个场景的包围盒
				if( m_frameCount >= 60 )
				{
					// 计算场景包围盒
					if( m_scene )
					{
						// 暂时屏蔽
						//m_scene->calculateBoundingBox();
					}

					m_frameCount = INT_MIN;

					// 如果使用GC
					if( m_gcProcedureEnable )
						m_garbageCollectFrameID = 1;
				}
			}

			// 垃圾回收
			//GarbageCollect();

			return;
		}

		m_frameCount = 0;

		ATOM_AUTOPTR(ATOM_AsyncableLoadObject) object = 0;
		int count = ATOM_min2( m_lockcount_per_procedure, m_fillResQueue.size() );

		if( ATOM_AsyncLoader::isEnableLog() && count > 0 )
			ATOM_LOGGER::log("++++++++++++++++++++ the unlock count is : %u +++++++++++++++++++++++\n", count );

		for( int i = 0; i < count; ++i )
		{
			{
				ATOM_Mutex::ScopeMutex lock(m_lock_fill);

#if USE_TWO_IO_THREAD
				object = m_fillResQueue.front();
				m_fillResQueue.pop_front();
#else
				object = (*m_fillResQueue.begin());
				m_fillResQueue.erase(m_fillResQueue.begin());
#endif
			}

			if( object )
			{
				// 如果已经填充了
				if( object->IsFilled() )
				{
					// 解锁
					object->Unlock();
				}
				else
				{
					// 设置Filled标志
					object->SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_FILLED);

					// 解锁
					object->Unlock();

					ATOM_LOGGER::error ("%s(%s) Fill Failed!.\n", __FUNCTION__, object->filename.c_str() );
				}
			}
		}
	}

	//===================================================//
	// 检查所有的请求者请求是否完成
	CheckAllRequester();
	//===================================================//
}

////////////////////////////////////////////////////////////////////////////////////
ATOM_LOADRequest_Event::ATOM_LOADRequest_Event( const char* filename, 
												ATOM_Node* requester, 
												int pri, 
												ATOM_AsyncableLoadObject* obj, 
												int showflag ) :
_file(filename),
_requester(requester),
_priority(pri),
_obj(obj),
_task_assigned(false),
_showflag(showflag)
{
	std::transform( _file.begin(), _file.end(), _file.begin(), ::tolower );
	ATOM_AsyncLoader::AddLoadRequest( requester, this );
}
////////////////////////////////////////////////////////////////////////////////////
bool requester_less::operator() (const ATOM_AUTOREF(ATOM_Node) n1, const ATOM_AUTOREF(ATOM_Node) n2) const
{	
	if( n1->_load_priority > n2->_load_priority )
	{
		return true;
	}
	else if( n1->_load_priority < n2->_load_priority )
	{
		return false;
	}
	else
	{
		return n1.get() < n2.get();
	}
}


ATOM_LoadRequest_EventList::~ATOM_LoadRequest_EventList()
{
	eventdict_t::iterator iter =  _list.begin();
	for( ; iter != _list.end(); ++iter )
	{
		REQUEST_SET & requests = iter->second;
		REQUEST_SET::iterator iter_req = requests.begin();
		for( ; iter_req != requests.end(); ++iter_req )
		{
			ATOM_LOADRequest_Event * req = *iter_req;
			if( req )
				ATOM_DELETE(req);
		}
		requests.clear();
	}
	_list.clear();
	_list_filekey.clear();
}
void ATOM_LoadRequest_EventList::AddLoadRequest( ATOM_Node* node, ATOM_LOADRequest_Event* request)
{
	if( !node || !request )
		return;

	// 查找该请求者节点是否已经在列表中
	eventdict_t::iterator iter = _list.find(node);
	if( iter != _list.end() )	
	{
		REQUEST_SET & requests = iter->second;

		// 检查该请求是否存在
		bool bExist = false;
		for( int i = 0 ; i < requests.size(); ++i )
		{
			ATOM_LOADRequest_Event* r = requests[i];
			if( r && r->_file == request->_file )
			{
				bExist = true;
				break;
			}
		}
		// 不存在 添加 
		if( !bExist )
		{
			requests.push_back(request);
		}
		// 否则 
		else
		{
			// 删除该请求
			ATOM_DELETE(request);

			//ATOM_LOGGER::log("node requester %s is dublicate\n",node->getDescribe().c_str());

			// 退出
			return;
		}
	}
	// 如果没有，添加一个请求者
	else
	{
		REQUEST_SET requests;
		requests.push_back(request);
		_list.insert( std::make_pair( node, requests ) );

		//ATOM_LOGGER::log("node requester %s is insert\n",node->getDescribe().c_str());
	}
	
	_list_filekey.insert( std::make_pair( request->_file, request ) );
}
void ATOM_LoadRequest_EventList::RemoveLoadRequest(ATOM_LOADRequest_Event* request)
{
		
}
bool ATOM_LoadRequest_EventList::IsRequestFinished(void * requester)
{
	return true;
}

void ATOM_LoadRequest_EventList::CheckAllRequester(void)
{
	ATOM_STACK_TRACE(ATOM_LoadRequest_EventList::CheckAllRequester);

	// 如果请求者集合为空
	if( _list.empty() )
		return;

	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::log("++++++++++++++++++++ the requester count is : %u +++++++++++++++++++++++\n", (unsigned int)_list.size() );

	////////////////////////////////////////////////////////////////////////////////////////////////

	// 需要调用onLoadFinished的节点对象集合
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> _node_tobeprocess;

	////////////////////////////////////////////////////////////////////////////////////////////////

	// 如果找不到该请求者 则退出(注意：是返回TRUE,因为这样请求者可以马上知道请求实际上已经完成了
	int count = 0;
	int maxcount = ATOM_AsyncLoader::GetMaxRequesterProcessCount();

	// 循环当前LIST中所有的请求者
	eventdict_t::iterator iter = _list.begin();
	while( ( iter != _list.end() )/* && ( count < maxcount )*/  )
	{	
		// 与该请求者相关的所有请求都完成了吗
		bool all_finish = true;
		bool show = false;
	
		eventdict_t::iterator current = iter++;
		
		ATOM_AUTOREF(ATOM_Node) node = current->first;
		if( !node.get() )
		{
			_list.erase(current);
			continue;
		}

		REQUEST_SET & requests = current->second;
		
		REQUEST_SET::iterator iter_request = requests.begin();
		while( iter_request != requests.end() )
		{
			ATOM_LOADRequest_Event * request = *iter_request;
			if( request )
			{
				if( ATOM_AsyncLoader::isEnableLog() )
					ATOM_LOGGER::log( " process request %s [ priority : %d ]now!\n", request->_file.c_str(), request->_priority );

				// 如果该请求的资源加载对象存在 且 该对象已经完成加载完成了
				if( ( request->_obj ) && ( request->_obj->IsLoadAllFinished() ) )
				{
					if( request->_showflag )
					{
						show = true;
					}

					if( ATOM_AsyncLoader::isEnableLog() )
						ATOM_LOGGER::log(" request %s is finished!\n", request->_file.c_str());

					// 删除该请求
					iter_request = requests.erase(iter_request);

					// 将该请求从filekey list中删除
					std::pair<eventdict_filekey::iterator, eventdict_filekey::iterator> range = _list_filekey.equal_range (request->_file);
					if( range.first != range.second )
					{
						eventdict_filekey::iterator iter = range.first;
						while(iter!=range.second)
						{
							if( iter->second == request )
							{
								_list_filekey.erase(iter);
								break;
							}
							iter++;
						}
					}

					ATOM_DELETE(request);

					// 递增计数
					++count;
					if( count == maxcount )
					{
						all_finish = false;
						break;
					}

					// 继续下一个该请求者的请求
					continue;
				}

				all_finish = false;
				break;
			}
		}

		// 所有请求完成 或 模型（动作）请求完成
		if( all_finish || show )
		{

			// 请求完成 处理节点
#if 0
			node->onLoadFinished();
#else
			_node_tobeprocess.push_back(node);
#endif

			// 如果所有请求都完成了  删除该请求者
			if( all_finish )
			{
				if( ATOM_AsyncLoader::isEnableLog() )
				{
					ATOM_Geode * geo = dynamic_cast<ATOM_Geode *>(node.get());
					if( geo )
					{
						/*ATOM_STRING fileName = geo->getModelFileName();
						ATOM_LOGGER::log(	" loading requester %s is all finished!\n", 
											fileName.empty() ? "none" : fileName.c_str() );

						for( int i = 0; i < geo->getModelFileNames().size(); ++i )
						{
							fileName = geo->getModelFileNames()[i];
							ATOM_LOGGER::log(	" loading requester %s is all finished!\n", 
												fileName.empty() ? "none" : fileName.c_str());
						}*/
					}
				}

				_list.erase(current);
			}
		}

		if( count == maxcount )
		{
			break;
		}
	}

	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::log("++++++++++++++++++++ the process count is : %u +++++++++++++++++++++++\n", count );

	////////////////////////////////////////////////////////////////////////////////////////////////

	// 如果需要处理的节点集合不为空
	if( !_node_tobeprocess.empty() )
	{
		ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)>::iterator iter =  _node_tobeprocess.begin();
		for(;iter!=_node_tobeprocess.end();++iter)
		{
			ATOM_AUTOREF(ATOM_Node) n = *iter;
			if( n )
				n->onLoadFinished();
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::log("================================================================\n" );
}
bool ATOM_LoadRequest_EventList::AssignTask( ATOM_AsyncableLoadObject * task)
{
	if( task )
	{
		//------- version 2 -----------//
		std::pair<eventdict_filekey::iterator, eventdict_filekey::iterator> range = _list_filekey.equal_range (task->filename);
		if( range.first == range.second )
			return false;

		eventdict_filekey::iterator iter = range.first;
		while(iter!=range.second)
		{
			ATOM_LOADRequest_Event* request = iter->second;

			// 如果该请求还未赋予资源加载任务
			if( request && !request->_task_assigned )
			{
				{
					// 那么该请求的资源对象就是该任务对象
					request->_obj = task;
					// 该请求被赋予了资源加载任务
					request->_task_assigned = true;

					// 设定该资源加载任务的优先级别
					//task->SetPriority(request->_priority);

					//if( ATOM_AsyncLoader::isEnableLog() )
					//	ATOM_LOGGER::log("the request %s assigned! \n", task->filename.c_str() );
				}
			}

			++iter;
		}

		return true;
	}

	return false;
}
void ATOM_LoadRequest_EventList::AbandonAll(bool Destroyall/*=true*/)
{
#if 1
	if( Destroyall )
	{
		// 清空filekey集合
		_list_filekey.clear();

		//eventdict_t temp_list;
		eventdict_t::iterator iter =  _list.begin();
		while( iter != _list.end() )
		{
			eventdict_t::iterator current = iter++;

			ATOM_AUTOREF(ATOM_Node) node = current->first;
			REQUEST_SET & requests = current->second;

			// 如果该节点的加载优先级达到一定级别 检查该节点的所有请求是否可丢弃
			if( ( node.get() ) &&
				( node->getLoadPriority() >= ATOM_AsyncLoader::GetReprocessPriorityThreshold() ) &&
				( node.get()->getRef() > requests.size() + 2 ) )
			{
				if( ATOM_AsyncLoader::isEnableLog() && !requests.empty() )
				{
					ATOM_LOGGER::log(	"Requester %s is still in reference : ref count is %u,  the request resource is %s. \n", 
										node->getObjectName(), 
										node.get()->getRef() - 1,
										requests[0]->_file.c_str() );
				}

				//---------------------------------------------------------------------//
				// 检查该节点的所有的请求
				// 如果该请求加载对象的host已经被delete了，那么该对象也应该已经从m_asyncLoadResources删除，
				// 那么就应该从集合中删除掉该请求
				REQUEST_SET::iterator iter_req = requests.begin();
				while( iter_req != requests.end() )
				{
					ATOM_LOADRequest_Event * req = *iter_req;
					if( req )
					{
						bool abandonRequest =	( ( req->_obj ) && ( 0 == req->_obj->host || req->_obj->priority < ATOM_AsyncLoader::GetReprocessPriorityThreshold() ) ) ||
												( req->_priority < ATOM_AsyncLoader::GetReprocessPriorityThreshold() );

						// 如果满足丢弃的条件 销毁并且从集合中删除					
						if( abandonRequest )
						{
							ATOM_DELETE(req);
							iter_req = requests.erase(iter_req);
							continue;
						}
						// 重新加入到filekey集合中去
						else
						{
							_list_filekey.insert( std::make_pair( req->_file, req ) );
						}
					}

					iter_req++;
				}

				if( requests.empty() )
				{
					_list.erase(current);
				}
				//---------------------------------------------------------------------//

				// 继续下一个请求者节点
				continue;
			}

			// 如果可以直接丢弃该节点所有的请求
			REQUEST_SET::iterator iter_req = requests.begin();
			for( ; iter_req != requests.end(); ++iter_req )
			{
				ATOM_LOADRequest_Event * req = *iter_req;
				if( req )
				{
					// 删除请求
					ATOM_DELETE(req);
				}
			}
			// 清空
			requests.clear();

			// 从请求者队列中删除该请求节点
			_list.erase(current);
		}

		//_list.clear();
		//std::swap(temp_list,_list);
	}
	else
	{

	}

	////////////////////////////////////////////////////////////////////////////////////

	// 清空filekey集合
	//_list_filekey.clear();

#else

	//if( Destroyall )
	//{
	//	// 交换到临时集合
	//	eventdict_t list_tmp;
	//	std::swap(list_tmp,_list);

	//	eventdict_t::iterator iter =  list_tmp.begin();
	//	while( iter != list_tmp.end() )
	//	{
	//		eventdict_t::iterator current = iter++;

	//		ATOM_AUTOREF(ATOM_Node) node = current->first;
	//		REQUEST_SET &			requests = current->second;

	//		// 如果该节点的加载优先级达到一定级别
	//		if( ( node.get() ) &&
	//			( node->getLoadPriority() >= ATOM_AsyncLoader::GetReprocessPriorityThreshold() ) &&
	//			( node.get()->getRef() > requests.size() + 2 ) )
	//		{
	//			if( ATOM_AsyncLoader::isEnableLog() && !requests.empty() )
	//			{
	//				ATOM_LOGGER::log(	"Requester %s is still in reference : ref count is %u,  the request resource is %s. \n", 
	//									node->getObjectName(), 
	//									node.get()->getRef() - 1,
	//									requests[0]->_file.c_str() );
	//			}

	//			//---------------------------------------------------------------------//
	//			// 检查该节点的所有的请求
	//			// 如果该请求加载对象的host已经被delete了，那么该对象也应该已经从m_asyncLoadResources删除，
	//			// 那么就应该从集合中删除掉该请求
	//			REQUEST_SET::iterator iter_req = requests.begin();
	//			while( iter_req != requests.end() )
	//			{
	//				ATOM_LOADRequest_Event * req = *iter_req;
	//				if( req )
	//				{
	//					bool abandonRequest =	( ( req->_obj ) && ( 0 == req->_obj->host || req->_obj->priority < ATOM_AsyncLoader::GetReprocessPriorityThreshold() ) ) ||
	//											( req->_priority < ATOM_AsyncLoader::GetReprocessPriorityThreshold() );
	//					if( !abandonRequest )
	//					{
	//						ATOM_DELETE(req);
	//						iter_req = requests.erase(iter_req);
	//						continue;
	//					}
	//				}
	//				iter_req++;
	//			}
	//			if( requests.empty() )
	//			{
	//				list_tmp.erase(current);
	//			}
	//			//---------------------------------------------------------------------//

	//			continue;
	//		}

	//		REQUEST_SET::iterator iter_req = requests.begin();
	//		for( ; iter_req != requests.end(); ++iter_req )
	//		{
	//			ATOM_LOADRequest_Event * req = *iter_req;
	//			if( req )
	//			{
	//				ATOM_DELETE(req);
	//			}
	//		}
	//		requests.clear();

	//		_list.erase(current);
	//	}

	//	//_list.clear();
	//	//std::swap(temp_list,_list);
	//}
	//else
	//{

	//}

	//_list_filekey.clear();

#endif
}
///////////////////////////////////////////////////////////////////////////////
void ATOM_AsyncLoader::AddLoadRequest( ATOM_Node* node, ATOM_LOADRequest_Event* request)
{
	if( !request )
		return;

	if( !m_run )
	{
		ATOM_DELETE(request);
		return;
	}

	// 如果添加进来的请求 其对应的加载对象不存在（一般都是如此）
	if( !request->_obj )
	{
		// 首先在已经添加进来的加载对象中进行查找，如果找到
		ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>::iterator iter = m_asyncLoadResources.find( request->_file );
		if( iter != m_asyncLoadResources.end() )
		{
			ATOM_AUTOPTR(ATOM_AsyncableLoadObject) obj = (iter->second);
			if( obj )
			{
				// 如果该加载对象已经加载完成了
				if( obj->IsLoadAllFinished() )
				{
					// 该请求无需执行了
					ATOM_DELETE(request);
					return;
				}
				else
				{
					// 否则直接赋予加载对象
					request->_obj = obj;
					request->_task_assigned = true;
				}
			}
		}
	}

	//--------------------------------------------------------------------//
	// 如果请求的优先级大于加载对象的优先级 更新加载对象优先级
	if( request->_obj && request->_priority > request->_obj->priority )
		request->_obj->priority = request->_priority;
	//--------------------------------------------------------------------//

	// 添加该请求
	m_requestEvents.AddLoadRequest( node, request);
}
void ATOM_AsyncLoader::RemoveLoadRequest(ATOM_LOADRequest_Event* request)
{
	if( !request )
		return;

	if( !m_run )
		return;

	m_requestEvents.RemoveLoadRequest(request);
}
bool ATOM_AsyncLoader::IsRequestFinished(void * requester)
{
	if( !requester )
		return false;

	if( !m_run )
		return false;

	return m_requestEvents.IsRequestFinished(requester);
}
void ATOM_AsyncLoader::CheckAllRequester(void)
{
	if( !m_run )
		return;

	return m_requestEvents.CheckAllRequester();
}
void ATOM_AsyncLoader::SetPreLoadGlobalResource(bool enable)
{
	m_preloadGlobalResource = enable;
}
bool ATOM_AsyncLoader::ReadGlboalPreLoadTable(const char* filename)
{
	// 如果异步加载 
	// 或者
	// 如果配置预先加载全局资源
	/*if( !m_run || !m_preloadGlobalResource )
	return true;*/
	if( !filename || filename[0] == '\0')
		return false;

	{
		ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
		if (!f)
		{
			ATOM_LOGGER::log ("%s(%s) File not found\n", __FUNCTION__, filename);
			return false;
		}
		unsigned size = f->size ();
		char *p = ATOM_NEW_ARRAY(char, size);
		size = f->read (p, size);
		p[size] = '\0';
		ATOM_TiXmlDocument doc;
		doc.Parse (p);
		ATOM_DELETE_ARRAY(p);
		if (doc.Error())
		{
			ATOM_LOGGER::error ("%s(%s)\n", __FUNCTION__, filename);
			return false;
		}

		ATOM_TiXmlElement *root = doc.RootElement ();
		if (!root)
		{
			ATOM_LOGGER::log ("%s(%s) Root XML element not found\n", __FUNCTION__, filename);
			return false;
		}

#if 0
		ATOM_LOGGER::log ("ATOM_AsyncLoader START to load the preload resources. \n", __FUNCTION__, filename);
#endif

		//===================== 在游戏开始就加载的资源 =====================//
		ATOM_TiXmlElement *ePreload_res = root->FirstChildElement ("preloads");
		if( ePreload_res )
		{
			ATOM_LOGGER::log ("ATOM_AsyncLoader START to load the preload materials. \n", __FUNCTION__, filename);
			// 材质
			ATOM_TiXmlElement *ePrecache_materials = ePreload_res->FirstChildElement ("materials");
			if (ePrecache_materials)
			{
				ATOM_TiXmlElement * eMaterial = 0;
				for( eMaterial = ePrecache_materials->FirstChildElement(); eMaterial ; eMaterial = eMaterial->NextSiblingElement() )
				{
					const char* mat_name = eMaterial->Attribute("file");
					char buffer[ATOM_VFS::max_filename_length];
					if (ATOM_CompletePath (mat_name, buffer))
					{
						_strlwr (buffer);
						m_preloadMaterials.push_back( ATOM_MaterialManager::loadCoreMaterial(ATOM_GetRenderDevice(), mat_name) );
						ATOM_LOGGER::log("preload material : %s \n", mat_name );
					}
				}
			}
			// 纹理
			ATOM_TiXmlElement *ePreload_texs = ePreload_res->FirstChildElement ("textures");
			if (ePreload_texs)
			{
				ATOM_LOGGER::log ("ATOM_AsyncLoader START to load the preload textures. \n", __FUNCTION__, filename);
				ATOM_TiXmlElement * eTex = 0;
				for( eTex = ePreload_texs->FirstChildElement(); eTex ; eTex = eTex->NextSiblingElement() )
				{
					const char* tex_name = eTex->Attribute("file");
					char buffer[ATOM_VFS::max_filename_length];
					if (ATOM_CompletePath (tex_name, buffer))
					{
						_strlwr (buffer);
						m_preloadTextures.push_back( ATOM_CreateTextureResource (buffer, ATOM_PIXEL_FORMAT_UNKNOWN,ATOM_LoadPriority_IMMEDIATE) );
						ATOM_LOGGER::log("preload image : %s \n", tex_name );
					}
				}
			}
		}

		//===================== 在游戏进行中立即加载的资源 =====================//

		ATOM_TiXmlElement *eImmediateload_res = root->FirstChildElement ("immediateloads");
		if( eImmediateload_res )
		{
			// 纹理
			ATOM_TiXmlElement *ePrecache_texs = eImmediateload_res->FirstChildElement ("textures");
			if (ePrecache_texs)
			{
				ATOM_LOGGER::log ("ATOM_AsyncLoader START to load the immediate load textures. \n", __FUNCTION__, filename);

				ATOM_TiXmlElement * eTex = 0;
				for( eTex = ePrecache_texs->FirstChildElement(); eTex ; eTex = eTex->NextSiblingElement() )
				{
					const char* tex_name = eTex->Attribute("file");
					char buffer[ATOM_VFS::max_filename_length];
					if (ATOM_CompletePath (tex_name, buffer))
					{
						_strlwr (buffer);			
						AddToImmediateTable(buffer);
					}
				}
			}
		}
	}

	return true;
}
bool ATOM_AsyncLoader::ReadLevelPreLoadTable(const char* filename)
{
	if( !m_run || !m_preloadGlobalResource )
		return true;

	ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		ATOM_LOGGER::log ("%s(%s) File not found\n", __FUNCTION__, filename);
		return false;
	}
	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error())
	{
		ATOM_LOGGER::error ("%s(%s)\n", __FUNCTION__, filename);
		return false;
	}

	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
	{
		ATOM_LOGGER::log ("%s(%s) Root XML element not found\n", __FUNCTION__, filename);
		return false;
	}

#if 0
	ATOM_LOGGER::log ("ATOM_AsyncLoader START to load the level specific preload resources. \n", __FUNCTION__, filename);
#endif 

	//===================== 在关卡开始就立即加载的资源 =====================//
	ATOM_TiXmlElement *ePreload_res = root->FirstChildElement ("preloads");
	if( ePreload_res )
	{
		// 纹理
		ATOM_TiXmlElement *ePreload_texs = ePreload_res->FirstChildElement ("textures");
		if (ePreload_texs)
		{
			ATOM_LOGGER::log ("ATOM_AsyncLoader START to load the level specific preload textures. \n", __FUNCTION__, filename);

			ATOM_TiXmlElement * eTex = 0;
			for( eTex = ePreload_texs->FirstChildElement(); eTex ; eTex = eTex->NextSiblingElement() )
			{
				const char* tex_name = eTex->Attribute("file");
				char buffer[ATOM_VFS::max_filename_length];
				if (ATOM_CompletePath (tex_name, buffer))
				{
					_strlwr (buffer);
					m_preloadTextures.push_back( ATOM_CreateTextureResource (buffer, ATOM_PIXEL_FORMAT_UNKNOWN, ATOM_LoadPriority_IMMEDIATE) );
					ATOM_LOGGER::log("preload image : %s \n", tex_name );
				}
			}
		}
	}

	//===================== 在关卡开始就异步加载的资源 =====================//
	ATOM_TiXmlElement *eLevelStart_stream_res = root->FirstChildElement ("levelStart_streams");
	if( eLevelStart_stream_res )
	{
		// 纹理
		ATOM_TiXmlElement *ePreload_texs = ePreload_res->FirstChildElement ("textures");
		if (ePreload_texs)
		{
#if 0
			ATOM_LOGGER::log ("ATOM_AsyncLoader START to load the level specific preload textures. \n", __FUNCTION__, filename);
#endif
			ATOM_TiXmlElement * eTex = 0;
			for( eTex = ePreload_texs->FirstChildElement(); eTex ; eTex = eTex->NextSiblingElement() )
			{
				const char* tex_name = eTex->Attribute("file");
				char buffer[ATOM_VFS::max_filename_length];
				if (ATOM_CompletePath (tex_name, buffer))
				{
					_strlwr (buffer);
					m_preloadTextures.push_back( ATOM_CreateTextureResource (buffer, ATOM_PIXEL_FORMAT_UNKNOWN, ATOM_LoadPriority_ASYNCBASE+1000) );
					ATOM_LOGGER::log("preload stream image : %s \n", tex_name );
				}
			}
		}
	}

	//===================== 在关卡进行中立即加载的资源 =====================//
	ATOM_TiXmlElement *eImmediateload_res = root->FirstChildElement ("runTime_immediateloads");
	if( eImmediateload_res )
	{
		// 纹理
		ATOM_TiXmlElement *ePrecache_texs = eImmediateload_res->FirstChildElement ("textures");
		if (ePrecache_texs)
		{
			ATOM_LOGGER::log ("ATOM_AsyncLoader START to load the level specific immediate load textures. \n", __FUNCTION__, filename);

			ATOM_TiXmlElement * eTex = 0;
			for( eTex = ePrecache_texs->FirstChildElement(); eTex ; eTex = eTex->NextSiblingElement() )
			{
				const char* tex_name = eTex->Attribute("file");
				char buffer[ATOM_VFS::max_filename_length];
				if (ATOM_CompletePath (tex_name, buffer))
				{
					_strlwr (buffer);			
					AddToImmediateTable(buffer);
				}
			}
		}
	}

	return true;
}
bool ATOM_AsyncLoader::AddToImmediateTable(const char* filename)
{
	// 只是路径
	ATOM_STRING texfile = filename;
	//texfile.erase(texfile.find_last_of('/'),ATOM_STRING::npos);
	// 转换成小写
	std::transform( texfile.begin(), texfile.end(), texfile.begin(), ::tolower );

	ATOM_MAP<ATOM_STRING,int>::iterator iter = m_asyncLoadPriorities.find(texfile);
	if( iter != m_asyncLoadPriorities.end() )
		return false;

	// 添加到查找表中
	m_asyncLoadPriorities.insert( std::make_pair( texfile, ATOM_LoadPriority_IMMEDIATE ) );
	
	return true;
}
bool ATOM_AsyncLoader::CheckImmediateResource(const char* resname )
{
	// 只是路径
	ATOM_STRING texfile = resname;
	// 转换成小写
	std::transform( texfile.begin(), texfile.end(), texfile.begin(), ::tolower );

	ATOM_MAP<ATOM_STRING,int>::iterator iter = m_asyncLoadPriorities.find(texfile);
	if( iter != m_asyncLoadPriorities.end() )
		return true;

	return false;
}
void ATOM_AsyncLoader::ClearAllGlboalPreLoads()
{
	m_preloadMaterials.clear();
	m_preloadTextures.clear();
	m_asyncLoadPriorities.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


void ATOM_AsyncLoader::SetCurrentScene( ATOM_SDLScene* scene )
{
	m_scene = scene;
}

// 这个函数主要用于依赖对象的加载完成判断（比如model的纹理依赖项）
bool ATOM_AsyncLoader::IsResourceFinshed(ATOM_STRING resName)
{
	if( !m_run )
		return true;

	ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>::iterator iter = m_asyncLoadResources.find(resName);
	if( iter != m_asyncLoadResources.end() )
	{
		ATOM_AUTOPTR(ATOM_AsyncableLoadObject) object = iter->second;
		if( object && object->IsLoadAllFinished() )
			return true;
	}
	// m_asyncLoadResources 会在跳关卡的时候清空 所以比如角色的装备纹理什么的就会找不到，找不到就会返回FALSE
	// 所以在ATOM_AsyncableLoadModel中有一个标记用来记录该模型资源是否已经全部加载完成，如果加载完成，后面的
	// 检查将不会到这里来了（不会再对依赖项进行检查）
	return false;
}

void ATOM_AsyncLoader::enableProcedureGC(bool bEnable)
{
	m_gcProcedureEnable = bEnable;
}
bool ATOM_AsyncLoader::isProcedureGCEnabled()
{
	return m_gcProcedureEnable;
}
void ATOM_AsyncLoader::GarbageCollect()
{
	if( !m_run )
		return;

	if( !m_gcProcedureEnable )
		return;

	if( !m_garbageCollectFrameID )
		return;

	static unsigned index = 0;
	static unsigned countPerUpdate = 20;

#if 0
	// 如果帧ID到了
	if( !m_asyncLoadResources.empty() && 
		( m_garbageCollectFrameID == m_garbageCollectFrameInterval+1 ) )
#else
	// 如果帧ID到了
	if( !m_asyncLoadHosts.empty() &&
		( m_garbageCollectFrameID == m_garbageCollectFrameInterval+1 ) )
#endif
	{
		int count = 0;
		
#if 0
		
		ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>::iterator iter = m_asyncLoadResources.begin() + index;
		while( iter != m_asyncLoadResources.end() )
		{
			ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>::iterator current = iter++;
			ATOM_AUTOPTR(ATOM_AsyncableLoadObject) obj = current->second;

			// 如果对象加载已经完成了
			if( obj && obj->IsLoadAllFinished() )
			{
				// 回收内存
				obj->GarbageCollect();

				// 删除之
				m_asyncLoadResources.erase(current);

				++count;
				if( count == m_garbageCollectCountPerFrame )
					break;
			}
		}

#else

		if( index >= m_asyncLoadHosts.size() )
			index = 0;

		unsigned index_end = index + countPerUpdate;

		// 将HOST对象从缓存集合中移除
		ATOM_MAP<ATOM_STRING,ATOM_AUTOREF(ATOM_Object)>::iterator iter_host = m_asyncLoadHosts.begin();
		unsigned index_tmp = 0;
		while( index_tmp != index )
		{
			index_tmp++;
			iter_host++;
		}

		unsigned NOW = ATOM_GetTick();
		while( iter_host != m_asyncLoadHosts.end() && ( index++ != index_end ) )
		{
			ATOM_MAP<ATOM_STRING,ATOM_AUTOREF(ATOM_Object)>::iterator current = iter_host++;

			ATOM_Object * host = current->second.get();

			if( !host )
				continue;

			ATOM_AsyncableLoadObject * obj = 0;
			ATOM_SharedModel * model = dynamic_cast<ATOM_SharedModel*>(host);
			if(model)
			{
				obj = model->getAsyncLoader();
			}
			ATOM_Texture * texture = dynamic_cast<ATOM_Texture*>(host);
			if( texture )
			{
				obj = texture->getAsyncLoader();
			}
			ATOM_NodeAttributeFile * nattribfile = dynamic_cast<ATOM_NodeAttributeFile*>(host);
			if( nattribfile )
			{
				obj = nattribfile->getAsyncLoader();
			}

			if( !obj )
				continue;

			// 如果对象加载未完成
			if( !obj->IsLoadAllFinished() )
				continue;

			// 对象的优先级是否为不可回收优先级
			if( obj->GetPriority() >= 10000 )
				continue;

			// HOST对象的引用计数为1
			// obj对象的引用计数为2
			if( host->getRef() > 1 || obj->getRef() > 2 )
			{
				// 重置计时
				obj->start_time = 0;
				continue;
			}

			// 如果当前对象没有被引用的情况持续超过5秒 删除该对象
			if( obj->start_time > 0 && ( NOW - obj->start_time >= 5000 ) )
			{
				if( 1 )
				{
					ATOM_LOGGER::debug("<%s> : %s is garbage collected!\n",__FUNCTION__,obj->filename.c_str());
				}

				// 删除之
				m_asyncLoadHosts.erase(current);
				++count;

				/*if( count == m_garbageCollectCountPerFrame )
					break;*/
			}
			// 开始计时
			else if( obj->start_time == 0 )
			{
				if( 1 )
				{
					ATOM_LOGGER::debug("<%s> : %s is in gc timing!\n",__FUNCTION__,obj->filename.c_str());
				}

				obj->start_time = NOW ;
			}
		}

		if( 1 )
		{
			ATOM_LOGGER::debug("<%s> : current host count is %u !\n",__FUNCTION__,(unsigned)m_asyncLoadHosts.size());
		}
#endif

		// 重置
		m_garbageCollectFrameID = 1;

		return;
	}

	// 递增GC的帧ID
	++m_garbageCollectFrameID;
}

void ATOM_AsyncLoader::CheckInvalidObjects()
{
	ATOM_LOGGER::log("/////////////////////// ASYNCLOADING DEBUG STATISTICS START //////////////////////\n");
	ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)> failedSet;
	ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>::iterator iter = m_asyncLoadResources.begin();
	for( ; iter != m_asyncLoadResources.end(); ++iter )
	{
		ATOM_STRING name = iter->first;
		ATOM_AUTOPTR(ATOM_AsyncableLoadObject) obj = iter->second;
		if( obj )
			if( obj->CheckStageFlagDebug() != ATOM_ASYNCLOAD_ALLFINISHED )
				failedSet.insert(std::make_pair(name,obj));
	}
	if( !failedSet.empty() )
	{
		ATOM_LOGGER::log("/////////////////////// ASYNCLOADING FAILED OBJECTS START //////////////////////\n");
		ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>::iterator iter = failedSet.begin();
		for( ; iter != failedSet.end(); ++iter )
		{
			ATOM_STRING name = iter->first;
			ATOM_AUTOPTR(ATOM_AsyncableLoadObject) obj = iter->second;
			if( obj )
			{
				ATOM_LOGGER::log("--- %s ---\n", name.c_str());
			}
		}
		failedSet.clear();
		ATOM_LOGGER::log("/////////////////////// ASYNCLOADING FAILED OBJECTS END //////////////////////\n");
	}
	ATOM_LOGGER::log("/////////////////////// ASYNCLOADING DEBUG STATISTICS END //////////////////////\n");
}

void ATOM_AsyncLoader::PriorityDown()
{
	ATOM_LOGGER::log("/////////////////////// Decrease the priority of Objects In loading START //////////////////////\n");

	ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>::iterator iter = m_asyncLoadResources.begin();
	for( ; iter != m_asyncLoadResources.end(); ++iter )
	{
		ATOM_STRING name = iter->first;
		ATOM_AUTOPTR(ATOM_AsyncableLoadObject) obj = iter->second;
		if( obj && !obj->IsLoadAllFinished() )
		{
			// 递减优先级别
			int pri = obj->GetPriority();
			pri -= 1;
			pri = ATOM_max2(pri,0);
			obj->SetPriority(pri);

			ATOM_LOGGER::log( "--- %s priority is down to %u ---\n", name.c_str(), pri );
		}
	}

	ATOM_LOGGER::log("/////////////////////// Decrease the priority of Objects In loading END //////////////////////\n");
}
////////////////////////////////////////////////////////////////////////////////////////////
void ATOM_AsyncLoader::SetMaxIOCountPerIteration(int _io_count_max/*=8*/)
{
	m_iocount_per_procedure = _io_count_max;
}
void ATOM_AsyncLoader::SetMaxFillCountPerIteration(int _fill_count_max/*=4*/)
{
	m_fillcount_per_procedure = _fill_count_max;
}
void ATOM_AsyncLoader::SetMaxLockCountPerIteration(int _lock_count_max/*=8*/)
{
	m_lockcount_per_procedure = _lock_count_max;
}
void ATOM_AsyncLoader::SetMaxRequesterProcessCount(int count/* = 8*/)
{
	m_maxcount_requesterProcess = count;
}
int ATOM_AsyncLoader::GetMaxRequesterProcessCount(void)
{
	return m_maxcount_requesterProcess;
}
void ATOM_AsyncLoader::SetReprocessPriorityThreshold(int threshold/* = 2001*/)
{
	m_reprocess_priority_threshold = threshold;
}
int  ATOM_AsyncLoader::GetReprocessPriorityThreshold(void)
{
	return m_reprocess_priority_threshold;
}
////////////////////////////////////////////////////////////////////////////////////////////
void ATOM_AsyncLoader::OnAsyncLoadObjectFinished( ATOM_AsyncableLoadObject * obj )
{
	if( !m_run )
		return;

	if( obj && !m_asyncLoadResources.empty())
	{
		bool bDeleted = false;
		ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>::iterator iter = m_asyncLoadResources.find(obj->filename);
		if( iter != m_asyncLoadResources.end() )
		{
			m_asyncLoadResources.erase(iter);
			bDeleted = true;
		}

		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::debug( "%s : %s [ Address : %x ] is %s ---\n",	__FUNCTION__, 
																		obj->filename.c_str(),
																		(unsigned)obj,
																		bDeleted ? "found & deleted" : "not found" );
	}
}

bool ATOM_AsyncLoader::ProcessLevelOpenedFileList( const char* levelsToBeProcess )
{
	ATOM_VECTOR<ATOM_STRING> scene_list;

	ATOM_AutoFile f(levelsToBeProcess, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		ATOM_LOGGER::log ("%s(%s) File not found\n", __FUNCTION__, levelsToBeProcess);
		return false;
	}
	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error())
	{
		ATOM_LOGGER::error ("%s(%s)\n", __FUNCTION__, levelsToBeProcess);
		return false;
	}

	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
	{
		ATOM_LOGGER::log ("%s(%s) Root XML element not found\n", __FUNCTION__, levelsToBeProcess);
		return false;
	}

	ATOM_STRING output_folder ="";
	ATOM_TiXmlElement *eOutFolder = root->FirstChildElement ("output");
	if( eOutFolder )
	{
		
#if 1
		output_folder = eOutFolder->Attribute("folder");
#else
		const char* folder = eOutFolder->Attribute("folder");
		char buffer[ATOM_VFS::max_filename_length];
		if (ATOM_CompletePath (folder, buffer))
		{
			_strlwr (buffer);
			output_folder = buffer;
		}
#endif
	}

	ATOM_TiXmlElement *eSceneList = root->FirstChildElement ("scene_list");
	if( eSceneList )
	{
		ATOM_TiXmlElement *eScene = eSceneList->FirstChildElement ("scene");
		while (eScene)
		{
			const char* scene_file = eScene->Attribute("file");
			char buffer[ATOM_VFS::max_filename_length];
			if (ATOM_CompletePath (scene_file, buffer))
			{
				_strlwr (buffer);
				scene_list.push_back( buffer );
			}
			eScene = eScene->NextSiblingElement();
		}
	}

	bool bRecordFilenameInEditorMode = false;
	ATOM_VECTOR<ATOM_STRING> args;
	ATOM_GetCommandLine (args);
	for (unsigned i = 0; i < args.size(); ++i)
	{
		if (args[i] == "--record-filename-editor")
		{
			bRecordFilenameInEditorMode = true;
		}
	}

	if( bRecordFilenameInEditorMode )
	{
		for( int i = 0; i < scene_list.size();++i )
		{
			ATOM_DeferredScene * _scene = ATOM_NEW(ATOM_DeferredScene);
			if (_scene)
			{
				ATOM_KernelConfig::setRecordFileName(true);
				ATOM_ClearOpenedFileList(0);
				
				ATOM_STRING::size_type mapid_last = scene_list[i].find_last_of('/');
				ATOM_STRING subString = scene_list[i].substr(0,mapid_last);
				ATOM_STRING::size_type mapid_first = subString.find_last_of('/');
				ATOM_STRING subString_ = subString.substr(mapid_first,subString.size()-mapid_first);

				_scene->load (scene_list[i].c_str());

				ATOM_STRING output = output_folder + subString_;
				ATOM_CopyOpenedFileList(output.c_str(),0,true);

				ATOM_KernelConfig::setRecordFileName(false);
			}

			ATOM_DELETE(_scene);
		}
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////
bool ATOM_AsyncLoader::IsObjectExistIn( ATOM_AsyncableLoadObject * obj )
{
	if( !m_run )
		return false;

	if( obj && !m_asyncLoadResources.empty())
	{
		ATOM_MAP<ATOM_STRING,ATOM_AUTOPTR(ATOM_AsyncableLoadObject)>::iterator iter = m_asyncLoadResources.find(obj->filename);
		if( iter != m_asyncLoadResources.end() )
		{
			return true;
		}
	}

	return false;
}
// 重新处理加载对象
void ATOM_AsyncLoader::ReProcessLoadObject( ATOM_AsyncableLoadObject * obj )
{
	if( !obj )
		return;

	obj->abandoned = false;

	if( obj->host )
	{
		//if( isEnableLog() )
		{
			ATOM_LOGGER::debug( "processing object [ Address : %x, priority %d ]\n", (unsigned)(obj), obj->priority );
			const char* classname	= obj->host->getClassName() ? obj->host->getClassName() : "";
			const char* objectname	= obj->host->getObjectName() ? obj->host->getObjectName() : "";
			ATOM_LOGGER::log("%s [%s] should be re processed!\n", objectname, classname );
		}

		m_asyncLoadHosts.insert( std::make_pair( obj->filename, obj->host ) );
	}
	else
	{
		//if( isEnableLog() )
		{
			ATOM_LOGGER::error( "object [ name : %s , Address : %x, priority %d ] has no host \n", 
								obj->filename.c_str(),
								(unsigned)(obj), 
								obj->priority );
		}
	}

	// 重新添加进资源集合中
	m_asyncLoadResources.insert( std::make_pair( obj->filename, obj ) );

#if 0
	unsigned flag = obj->CheckStageFlagDebug();
#else
	unsigned flag = obj->flag;					// 得到标记
#endif

	if( flag != ATOM_ASYNCLOAD_ALLFINISHED )	// 如果标记为未加载完成
	{
		switch( flag )
		{
		case ATOM_ASYNCLOAD_NOTLOADSTARTED:	// 还未开始（如果已经start,一般不可能出现此情况）
		case ATOM_ASYNCLOAD_LOADSTARTED:	// 已经开始
		case ATOM_ASYNCLOAD_INLOADING:		// 在加载中
			{
				ATOM_LOGGER::log("%s is started and need to load!\n",obj->filename.c_str());

				AddAsyncTask( obj );			// 再次添加到IO队列
			}
			break;
		case ATOM_ASYNCLOAD_LOADED:			// 已经加载 
			{
				ATOM_LOGGER::log("%s is loaded and need to lock!\n",obj->filename.c_str());

				bool bRec = false;
				obj->Lock(bRec);			// 锁

			}
			break;
		case ATOM_ASYNCLOAD_LOCKED:			// 已经锁
			{
				ATOM_LOGGER::log("%s is locked and need to fill!\n",obj->filename.c_str());

				ATOM_AsyncLoader::AddFillTask( obj );	// 添加到填充队列
			}
			break;
		case ATOM_ASYNCLOAD_FILLED :		// 已经填充
			{
				ATOM_LOGGER::log("%s is filled and need to unlock!\n",obj->filename.c_str());

				ATOM_AsyncLoader::ResFillDone( obj );	// 解锁队列
			}
			break;
		}
	}

	//-------------------------------------------------------------------------//
	// 如果该对象自身资源已经加载完成了，但是并未完全加载完成（因为依赖项没有加载完成）
	// 那么检查依赖项。
	// 如果依赖性被卸载了，重新加载依赖项。
	obj->processAllDependents();
	//-------------------------------------------------------------------------//
}

// 检查加载文件是否在本地
bool ATOM_AsyncLoader::CheckFileLocal(const char* file)
{
	// 如果有检查函数
	if( m_funcCheckFile )
	{
		char id[ATOM_VFS::max_filename_length];
		if (!ATOM_CompletePath (file, id))
		{
			return false;
		}
		strlwr (id);
		for (char *ch = id; *ch; ++ch)
		{
			if (*ch == '/')
			{
				*ch = '\\';
			}
		}

		//ATOM_LOGGER::debug("use check file local function to check\n");

		// 使用函数进行检查
		if( !m_funcCheckFile (id) )
		{
			char native[ATOM_VFS::max_filename_length];
			ATOM_GetNativePathName(file, native);
			return ATOM_PhysicVFS().doesFileExists (native);
		}

		return true;
	}
	// 否则 使用VFS检查
	else
	{
		//ATOM_LOGGER::debug("use VFS to check\n");

		char native[ATOM_VFS::max_filename_length];
		ATOM_GetNativePathName(file, native);
		return ATOM_PhysicVFS().doesFileExists (native);
	}
}
//========================================================================//
//bool ATOM_AsyncLoader::FindAttribFileLoader(const char* filename)
//{
//	ATTRIBFILEITER iter = m_attribFileContainer.find(filename);
//	if( iter == m_attribFileContainer.end() )
//		return false;
//	return true;
//}
//ATOM_AsyncableLoadFile * ATOM_AsyncLoader::GetAttribFileLoader(const char* filename)
//{
//	// 首先在集合中查找
//	ATTRIBFILEITER iter = m_attribFileContainer.find(filename);
//	if( iter == m_attribFileContainer.end() )
//	{
//		// 没有创建一个
//		ATOM_AsyncableLoadAttributeFile * attribFile = ATOM_NEW(ATOM_AsyncableLoadAttributeFile);
//		if( attribFile )
//		{
//			sAttributeFile file;
//			file._attribFileLoader = attribFile;
//			file._refCount = 1;
//			m_attribFileContainer.insert(std::make_pair(filename,file));
//			return attribFile;
//		}
//		else
//		{
//			ATOM_LOGGER::error("%s : the attribute file loader of %s create FAILED!\n", __FUNCTION__, filename );
//			return 0;
//		}
//	}
//
//	// 有 直接返回
//	iter->second._refCount++;
//	return iter->second._attribFileLoader;
//}
//void ATOM_AsyncLoader::DestroyAttribFileLoader(ATOM_AsyncableLoadFile * attribFileLoader)
//{
//	if( !attribFileLoader )
//		return;
//
//	ATTRIBFILEITER iter = m_attribFileContainer.find(attribFileLoader->filename);
//	if( iter != m_attribFileContainer.end() )
//	{
//		iter->second._refCount--;
//		if( iter->second._refCount == 0 )
//		{
//			ATOM_AUTOPTR(ATOM_AsyncableLoadFile) tmp = attribFileLoader;
//
//			m_attribFileContainer.erase(iter);
//
//			OnAsyncLoadObjectFinished(attribFileLoader);
//		}
//	}
//}
//void ATOM_AsyncLoader::ClearAttribFileContainer()
//{
//	m_attribFileContainer.clear();
//}

///////////////////////////////////////////////////////////////////////////////////保存


//ATOM_BEGIN_EVENT_MAP_NOPARENT(ATOM_NodeAsyncLoadMgrBase)
//ATOM_EVENT_HANDLER(ATOM_NodeAsyncLoadMgrBase, ATOM_NodeAsyncLoadEvent, OnNodeAsyncLoad)
//ATOM_END_EVENT_MAP

ATOM_NodeAsyncLoadMgrGroup::MGRVEC ATOM_NodeAsyncLoadMgrGroup::_mgrVec;

ATOM_NodeAsyncLoadMgrGroup::ATOM_NodeAsyncLoadMgrGroup()
{

}
ATOM_NodeAsyncLoadMgrGroup::~ATOM_NodeAsyncLoadMgrGroup()
{
	_mgrVec.clear();
}

void ATOM_NodeAsyncLoadMgrGroup::RegisterNodeAsyncLoadMgr(ATOM_NodeAsyncLoadMgrBase * pNodeALM )
{
	_mgrVec.push_back(pNodeALM);
}

void ATOM_NodeAsyncLoadMgrGroup::OnNodeAsyncLoad(ATOM_NodeAsyncLoadEvent *event)
{
	MGRITER iter = _mgrVec.begin();
	for( ; iter != _mgrVec.end(); ++iter )
	{
		ATOM_NodeAsyncLoadMgrBase* nslm = (*iter);
		if( nslm )
			nslm->OnNodeAsyncLoad(event);
	}
}

