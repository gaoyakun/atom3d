/**	\file cpuinfo.h
 *	ATOM_CPUInfo类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_CPUINFO_H_
#define __ATOM_KERNEL_CPUINFO_H_

#define STORE_CLASSICAL_NAME(x)		sprintf (ChipID.ProcessorName, x)
#define STORE_TLBCACHE_INFO(x,y)	x = (x < y) ? y : x
#define VENDOR_STRING_LENGTH		(12 + 1)
#define CHIPNAME_STRING_LENGTH		(48 + 1)
#define SERIALNUMBER_STRING_LENGTH	(29 + 1)
#define TLBCACHE_INFO_UNITS			(15)
#define CLASSICAL_CPU_FREQ_LOOP		10000000
#define RDTSC_INSTRUCTION			_asm _emit 0x0f _asm _emit 0x31
#define	CPUSPEED_I32TO64(x, y)		(((__int64) x << 32) + y)

#define CPUID_AWARE_COMPILER
#ifdef CPUID_AWARE_COMPILER
	#define CPUID_INSTRUCTION		cpuid
#else
	#define CPUID_INSTRUCTION		_asm _emit 0x0f _asm _emit 0xa2
#endif

#define MMX_FEATURE					0x00000001
#define MMX_PLUS_FEATURE			0x00000002
#define SSE_FEATURE					0x00000004
#define SSE2_FEATURE				0x00000008
#define AMD_3DNOW_FEATURE			0x00000010
#define AMD_3DNOW_PLUS_FEATURE		0x00000020
#define IA64_FEATURE				0x00000040
#define MP_CAPABLE					0x00000080
#define HYPERTHREAD_FEATURE			0x00000100
#define SERIALNUMBER_FEATURE		0x00000200
#define APIC_FEATURE				0x00000400
#define SSE_FP_FEATURE				0x00000800
#define SSE_MMX_FEATURE				0x00001000
#define CMOV_FEATURE				0x00002000
#define MTRR_FEATURE				0x00004000
#define L1CACHE_FEATURE				0x00008000
#define L2CACHE_FEATURE				0x00010000
#define L3CACHE_FEATURE				0x00020000
#define ACPI_FEATURE				0x00040000
#define THERMALMONITOR_FEATURE		0x00080000
#define TEMPSENSEDIODE_FEATURE		0x00100000
#define FREQUENCYID_FEATURE			0x00200000
#define VOLTAGEID_FREQUENCY			0x00400000

#ifdef _WIN32
	// Include Windows header files.
	#include <windows.h>
#endif // _WIN32

// Include generic C / C++ header files.
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include "basedefs.h"

typedef	void (*DELAY_FUNC)(unsigned int uiMS);

class CPUSpeed 
{
public:
    CPUSpeed (void);
    ~CPUSpeed (void);
    ATOM_LongLong getCyclesDifference (DELAY_FUNC, unsigned int);
    int CPUSpeedInMHz;
		
private:
    static void delay (unsigned int);
    static void delayOverhead (unsigned int);
};

//! \class ATOM_CPUInfo
//! 获取当前计算机系统中CPU信息的类
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_CPUInfo 
{
public:
	//! 构造函数
    ATOM_CPUInfo (void);

	//! 析构函数
    virtual ~ATOM_CPUInfo (void);

	//! 得到供应商信息
	//! \return 供应商信息字符串
    const char *getVendorString () const;

	//! 得到供应商索引
	//! \return  供应商索引信息
    const char *getVendorID () const;

	//! 得到类型标示
	//! \return 标示字符串
    const char *getTypeID () const;

	//! 得到所属家族标示
	//! \return 家族标示字符串
    const char *getFamilyID () const;

	//! 得到模块标示
	//! \return 模块标示字符串
    const char *getModelID () const;

	//! 得到步进码
	//! \return 步进码字符串
    const char *getSteppingCode () const;

	//! 得到扩展的CPU名称
	//! \return  扩展的CPU名称字符串
    const char *getExtendedProcessorName () const;

	//! 得到CPU的序列号
	//! \return  CPU序列号字符串
    const char *getProcessorSerialNumber () const;

	//! 查询每个物理CPU包含几个逻辑CPU
	//! \return 包括的数量
    int getLogicalProcessorsPerPhysical () const;

	//! 查询CPU的主频
	//! \return CPU的主频，单位HZ
    int getProcessorClockFrequency () const;

	//! 查询CPU的APICID
	//! \return APICID
    int getProcessorAPICID () const;

	//! 查询CPU的缓存大小
	//! \param type 缓存类型，可以是以下值
	/** @verbatim
		L1CACHE_FEATURE
		L2CACHE_FEATURE
		L3CACHE_FEATURE
		@endverbatim
	*/
	//! \return 缓存大小
    int getProcessorCacheXSize (DWORD type) const;

	//! 查询CPU是否支持某特性
	//! \param feature 要查询的特性，可以是以下值的组合
	/** @verbatim
		MMX_FEATURE
		MMX_PLUS_FEATURE
		SSE_FEATURE
		SSE_FP_FEATURE
		SSE_MMX_FEATURE
		SSE2_FEATURE
		AMD_3DNOW_FEATURE
		AMD_3DNOW_PLUS_FEATURE
		IA64_FEATURE
		MP_CAPABLE
		SERIALNUMBER_FEATURE
		APIC_FEATURE
		CMOV_FEATURE
		MTRR_FEATURE
		L1CACHE_FEATURE
		L2CACHE_FEATURE
		L3CACHE_FEATURE
		ACPI_FEATURE
		THERMALMONITOR_FEATURE
		TEMPSENSEDIODE_FEATURE
		FREQUENCYID_FEATURE
		VOLTAGEID_FREQUENCY
		@endverbatim
	*/
	//! \return 支持的组合
    DWORD doesCPUSupportFeature (DWORD feature) const;

	//! 查询CPU是否支持CPUID指令.
	//! \return true 支持 false 不支持
    bool doesCPUSupportCPUID () const;

private:
    typedef struct tagID 
	{
	    int Type;
	    int Family;
	    int ATOM_Model;
	    int Revision;
	    int ExtendedFamily;
	    int ExtendedModel;
	    char ProcessorName[CHIPNAME_STRING_LENGTH];
	    char Vendor[VENDOR_STRING_LENGTH];
	    char SerialNumber[SERIALNUMBER_STRING_LENGTH];
    } ID;

    typedef struct tagCPUPowerManagement 
	{
	    bool HasVoltageID;
	    bool HasFrequencyID;
	    bool HasTempSenseDiode;
    } CPUPowerManagement;

    typedef struct tagCPUExtendedFeatures 
  {
	    bool Has3DNow;
	    bool Has3DNowPlus;
	    bool SupportsMP;
	    bool HasMMXPlus;
	    bool HasSSEMMX;
	    bool SupportsHyperthreading;
	    int LogicalProcessorsPerPhysical;
	    int APIC_ID;
	    CPUPowerManagement PowerManagement;
    } CPUExtendedFeatures;	
	
    typedef struct CPUtagFeatures 
  {
	    bool HasFPU;
	    bool HasTSC;
	    bool HasMMX;
	    bool HasSSE;
	    bool HasSSEFP;
	    bool HasSSE2;
	    bool HasIA64;
	    bool HasAPIC;
	    bool HasCMOV;
	    bool HasMTRR;
	    bool HasACPI;
	    bool HasSerial;
	    bool HasThermal;
	    int CPUSpeed;
	    int L1CacheSize;
	    int L2CacheSize;
	    int L3CacheSize;
	    CPUExtendedFeatures ExtendedFeatures;
    } CPUFeatures;
    
    enum Manufacturer {
	    AMD, Intel, NSC, UMC, Cyrix, NexGen, IDT, Rise, Transmeta, UnknownManufacturer
    };

    bool retrieveCPUFeatures (void);
    bool retrieveCPUIdentity (void);
    bool retrieveCPUCacheDetails (void);
    bool retrieveClassicalCPUCacheDetails (void);
    bool retrieveCPUClockSpeed (void);
    bool retrieveClassicalCPUClockSpeed (void);
    bool retrieveCPUExtendedLevelSupport (unsigned);
    bool retrieveExtendedCPUFeatures (void);
    bool retrieveProcessorSerialNumber (void);
    bool retrieveCPUPowerManagement (void);
    bool retrieveClassicalCPUIdentity (void);
    bool retrieveExtendedCPUIdentity (void);
	
    // Variables.
    Manufacturer ChipManufacturer;
    CPUFeatures Features;
    CPUSpeed * Speed;
    ID ChipID;

  char szTypeID[32];
  char szFamilyID[32];
    char szModelID[32];
    char szSteppingCode[32];
};

class ATOM_KERNEL_API ATOM_CPUInfo2
{
public:
	ATOM_CPUInfo2 (void);

public:
	unsigned getNumProcessors (void) const;
	const char *getProcessorName (unsigned) const;
};

#endif // __ATOM_KERNEL_CPUINFO_H_
/*! @} */
