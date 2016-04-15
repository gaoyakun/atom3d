/**	\file cpuinfo.h
 *	ATOM_CPUInfo�������.
 *
 *	\author ������
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
//! ��ȡ��ǰ�����ϵͳ��CPU��Ϣ����
//! \author ������
class ATOM_KERNEL_API ATOM_CPUInfo 
{
public:
	//! ���캯��
    ATOM_CPUInfo (void);

	//! ��������
    virtual ~ATOM_CPUInfo (void);

	//! �õ���Ӧ����Ϣ
	//! \return ��Ӧ����Ϣ�ַ���
    const char *getVendorString () const;

	//! �õ���Ӧ������
	//! \return  ��Ӧ��������Ϣ
    const char *getVendorID () const;

	//! �õ����ͱ�ʾ
	//! \return ��ʾ�ַ���
    const char *getTypeID () const;

	//! �õ����������ʾ
	//! \return �����ʾ�ַ���
    const char *getFamilyID () const;

	//! �õ�ģ���ʾ
	//! \return ģ���ʾ�ַ���
    const char *getModelID () const;

	//! �õ�������
	//! \return �������ַ���
    const char *getSteppingCode () const;

	//! �õ���չ��CPU����
	//! \return  ��չ��CPU�����ַ���
    const char *getExtendedProcessorName () const;

	//! �õ�CPU�����к�
	//! \return  CPU���к��ַ���
    const char *getProcessorSerialNumber () const;

	//! ��ѯÿ������CPU���������߼�CPU
	//! \return ����������
    int getLogicalProcessorsPerPhysical () const;

	//! ��ѯCPU����Ƶ
	//! \return CPU����Ƶ����λHZ
    int getProcessorClockFrequency () const;

	//! ��ѯCPU��APICID
	//! \return APICID
    int getProcessorAPICID () const;

	//! ��ѯCPU�Ļ����С
	//! \param type �������ͣ�����������ֵ
	/** @verbatim
		L1CACHE_FEATURE
		L2CACHE_FEATURE
		L3CACHE_FEATURE
		@endverbatim
	*/
	//! \return �����С
    int getProcessorCacheXSize (DWORD type) const;

	//! ��ѯCPU�Ƿ�֧��ĳ����
	//! \param feature Ҫ��ѯ�����ԣ�����������ֵ�����
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
	//! \return ֧�ֵ����
    DWORD doesCPUSupportFeature (DWORD feature) const;

	//! ��ѯCPU�Ƿ�֧��CPUIDָ��.
	//! \return true ֧�� false ��֧��
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
