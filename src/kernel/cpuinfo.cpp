#include "stdafx.h"

ATOM_CPUInfo::ATOM_CPUInfo ()
{
	strcpy (szTypeID, "n/a");
	strcpy (szFamilyID, "n/a");
	strcpy (szModelID, "n/a");
	strcpy (szSteppingCode, "n/a");

	ChipManufacturer = UnknownManufacturer;
	memset (&Features, 0, sizeof(Features));
	memset (&ChipID, 0, sizeof(ChipID));
	Speed = 0;

	if (doesCPUSupportCPUID ()) 
	{
		retrieveCPUIdentity ();
		retrieveCPUFeatures ();

		if (!retrieveCPUClockSpeed ()) 
		{
			retrieveClassicalCPUClockSpeed ();
		}

		if (!retrieveCPUCacheDetails ()) 
		{
			retrieveClassicalCPUCacheDetails ();
		}

		if (!retrieveExtendedCPUIdentity ()) 
		{
			retrieveClassicalCPUIdentity ();
		}

		retrieveExtendedCPUFeatures ();

		retrieveProcessorSerialNumber ();
	}
}

ATOM_CPUInfo::~ATOM_CPUInfo ()
{
	ATOM_DELETE(Speed);
}

const char * ATOM_CPUInfo::getVendorString () const
{
	return ChipID.Vendor;
}

const char * ATOM_CPUInfo::getVendorID () const
{
	// Return the vendor ID.
	switch (ChipManufacturer) {
	case Intel:
		return "Intel Corporation";
	case AMD:
		return "Advanced Micro Devices";
	case NSC:
		return "National Semiconductor";
	case Cyrix:
		return "Cyrix Corp., VIA Inc.";
	case NexGen:
		return "NexGen Inc., Advanced Micro Devices";
	case IDT:
		return "IDT\\Centaur, Via Inc.";
	case UMC:
		return "United Microelectronics Corp.";
	case Rise:
		return "Rise";
	case Transmeta:
		return "Transmeta";
	case UnknownManufacturer:
	default:
		return "Unknown Manufacturer";
	}
}

const char * ATOM_CPUInfo::getTypeID () const
{
	return szTypeID;
}

const char * ATOM_CPUInfo::getFamilyID () const
{
	return szFamilyID;
}

const char * ATOM_CPUInfo::getModelID () const
{
	return szModelID;
}

const char * ATOM_CPUInfo::getSteppingCode () const
{
	return szSteppingCode;
}

const char * ATOM_CPUInfo::getExtendedProcessorName () const
{
	return ChipID.ProcessorName;
}

const char * ATOM_CPUInfo::getProcessorSerialNumber () const
{
	return ChipID.SerialNumber;
}

int ATOM_CPUInfo::getLogicalProcessorsPerPhysical () const
{
	return Features.ExtendedFeatures.LogicalProcessorsPerPhysical;
}

int ATOM_CPUInfo::getProcessorClockFrequency () const
{
	return Speed ? Speed->CPUSpeedInMHz : 0;
}

int ATOM_CPUInfo::getProcessorAPICID () const
{
	return Features.ExtendedFeatures.APIC_ID;
}

int ATOM_CPUInfo::getProcessorCacheXSize (DWORD dwCacheID) const
{
	switch (dwCacheID) 
	{
	case L1CACHE_FEATURE:
		return Features.L1CacheSize;
	case L2CACHE_FEATURE:
		return Features.L2CacheSize;
	case L3CACHE_FEATURE:
		return Features.L3CacheSize;
	default:
		break;
	}

	return 0;
}

DWORD ATOM_CPUInfo::doesCPUSupportFeature (DWORD dwFeature) const
{
	DWORD bHasFeature = 0;

	if (((dwFeature & MMX_FEATURE) != 0) && Features.HasMMX) 
		bHasFeature |= MMX_FEATURE;

	if (((dwFeature & MMX_PLUS_FEATURE) != 0) && Features.ExtendedFeatures.HasMMXPlus) 
		bHasFeature |= MMX_PLUS_FEATURE;

	if (((dwFeature & SSE_FEATURE) != 0) && Features.HasSSE) 
		bHasFeature |= SSE_FEATURE;

	if (((dwFeature & SSE_FP_FEATURE) != 0) && Features.HasSSEFP) 
		bHasFeature |= SSE_FP_FEATURE;

	if (((dwFeature & SSE_MMX_FEATURE) != 0) && Features.ExtendedFeatures.HasSSEMMX) 
		bHasFeature |= SSE_MMX_FEATURE;

	if (((dwFeature & SSE2_FEATURE) != 0) && Features.HasSSE2) 
		bHasFeature |= SSE2_FEATURE;

	if (((dwFeature & AMD_3DNOW_FEATURE) != 0) && Features.ExtendedFeatures.Has3DNow) 
		bHasFeature |= AMD_3DNOW_FEATURE;

	if (((dwFeature & AMD_3DNOW_PLUS_FEATURE) != 0) && Features.ExtendedFeatures.Has3DNowPlus) 
		bHasFeature |= AMD_3DNOW_PLUS_FEATURE;

	if (((dwFeature & IA64_FEATURE) != 0) && Features.HasIA64) 
		bHasFeature |= IA64_FEATURE;

	if (((dwFeature & MP_CAPABLE) != 0) && Features.ExtendedFeatures.SupportsMP) 
		bHasFeature |= MP_CAPABLE;

	if (((dwFeature & SERIALNUMBER_FEATURE) != 0) && Features.HasSerial) 
		bHasFeature |= SERIALNUMBER_FEATURE;

	if (((dwFeature & APIC_FEATURE) != 0) && Features.HasAPIC) 
		bHasFeature |= APIC_FEATURE;

	if (((dwFeature & CMOV_FEATURE) != 0) && Features.HasCMOV) 
		bHasFeature |= CMOV_FEATURE;

	if (((dwFeature & MTRR_FEATURE) != 0) && Features.HasMTRR) 
		bHasFeature |= MTRR_FEATURE;

	if (((dwFeature & L1CACHE_FEATURE) != 0) && (Features.L1CacheSize != 0)) 
		bHasFeature |= L1CACHE_FEATURE;

	if (((dwFeature & L2CACHE_FEATURE) != 0) && (Features.L2CacheSize != 0)) 
		bHasFeature |= L2CACHE_FEATURE;

	if (((dwFeature & L3CACHE_FEATURE) != 0) && (Features.L3CacheSize != 0)) 
		bHasFeature |= L3CACHE_FEATURE;

	if (((dwFeature & ACPI_FEATURE) != 0) && Features.HasACPI) 
		bHasFeature |= ACPI_FEATURE;

	if (((dwFeature & THERMALMONITOR_FEATURE) != 0) && Features.HasThermal) 
		bHasFeature |= THERMALMONITOR_FEATURE;

	if (((dwFeature & TEMPSENSEDIODE_FEATURE) != 0) && Features.ExtendedFeatures.PowerManagement.HasTempSenseDiode) 
		bHasFeature |= TEMPSENSEDIODE_FEATURE;

	if (((dwFeature & FREQUENCYID_FEATURE) != 0) && Features.ExtendedFeatures.PowerManagement.HasFrequencyID) 
		bHasFeature |= FREQUENCYID_FEATURE;

	if (((dwFeature & VOLTAGEID_FREQUENCY) != 0) && Features.ExtendedFeatures.PowerManagement.HasVoltageID) 
		bHasFeature |= VOLTAGEID_FREQUENCY;

	return bHasFeature;
}

bool ATOM_CPUInfo::doesCPUSupportCPUID (void) const
{
#if defined(_M_IA64) || defined(_M_AMD64)
	return true;
#else
	// TODO:
#if ATOM3D_COMPILER_MSVC
	int CPUIDPresent = 0;

#ifdef _WIN32 
	// Use SEH to determine CPUID presence
	__try 
	{
		__asm 
		{
#ifdef CPUID_AWARE_COMPILER
			push eax
				push ebx
				push ecx
				push edx
#endif
				mov eax, 0
				CPUID_INSTRUCTION

#ifdef CPUID_AWARE_COMPILER
				pop edx
				pop ecx
				pop ebx
				pop eax
#endif
		}
	}
	__except (1) 
	{
		CPUIDPresent = false;
		return false;
	}
#else
	__try 
	{
		__asm 
		{
			pushfd                      ; save EFLAGS to stack.
				pop     eax                 ; store EFLAGS in eax.
				mov     edx, eax            ; save in ebx for testing later.
				xor     eax, 0200000h       ; switch bit 21.
				push    eax                 ; copy "changed" value to stack.
				popfd                       ; save "changed" eax to EFLAGS.
				pushfd
				pop     eax
				xor     eax, edx            ; See if bit changeable.
				jnz     short cpuid_present ; if so, mark 
				mov     eax, -1             ; CPUID not present - disable its usage
				jmp     no_features

cpuid_present:
			mov   eax, 0        ; CPUID capable CPU - enable its usage.

no_features:
			mov     CPUIDPresent, eax ; Save the value in eax to a variable.
		}
	}
	__except (1) 
	{
		CPUIDPresent = false;
		return false;
	}
#endif

	return (CPUIDPresent == 0) ? true : false;
#else
	return false;
#endif

#endif
}

bool ATOM_CPUInfo::retrieveCPUFeatures (void)
{
#if defined(_M_IA64)||defined(_M_AMD64)
	return false;
#else
	// TODO:
#if ATOM3D_COMPILER_MSVC
	int cpuFeatures = 0;
	int cpuAdvanced = 0;
	__try 
	{
		__asm 
		{
#ifdef CPUID_AWARE_COMPILER
			push eax
				push ebx
				push ecx
				push edx
#endif
				mov eax,1
				CPUID_INSTRUCTION
				mov cpuFeatures, edx
				mov cpuAdvanced, ebx

#ifdef CPUID_AWARE_COMPILER
				pop edx
				pop ecx
				pop ebx
				pop eax
#endif
		}
	}
	__except (1) 
	{
		return false;
	}

	// Retrieve the features of CPU present.
	Features.HasFPU =   ((cpuFeatures & 0x00000001) != 0);    // FPU Present --> Bit 0
	Features.HasTSC =   ((cpuFeatures & 0x00000010) != 0);    // TSC Present --> Bit 4
	Features.HasAPIC =    ((cpuFeatures & 0x00000200) != 0);    // APIC Present --> Bit 9
	Features.HasMTRR =    ((cpuFeatures & 0x00001000) != 0);    // MTRR Present --> Bit 12
	Features.HasCMOV =    ((cpuFeatures & 0x00008000) != 0);    // CMOV Present --> Bit 15
	Features.HasSerial =  ((cpuFeatures & 0x00040000) != 0);    // Serial Present --> Bit 18
	Features.HasACPI =    ((cpuFeatures & 0x00400000) != 0);    // ACPI Capable --> Bit 22
	Features.HasMMX =   ((cpuFeatures & 0x00800000) != 0);    // MMX Present --> Bit 23
	Features.HasSSE =   ((cpuFeatures & 0x02000000) != 0);    // SSE Present --> Bit 25
	Features.HasSSE2 =    ((cpuFeatures & 0x04000000) != 0);    // SSE2 Present --> Bit 26
	Features.HasThermal = ((cpuFeatures & 0x20000000) != 0);    // Thermal Monitor Present --> Bit 29
	Features.HasIA64 =    ((cpuFeatures & 0x40000000) != 0);    // IA64 Present --> Bit 30

	if (Features.HasSSE) 
	{
		__try 
		{
			__asm 
			{
				_emit 0x0f
					_emit 0x56
					_emit 0xc0  
			}
			Features.HasSSEFP = true;
		}
		__except (1) 
		{
			Features.HasSSEFP = false;
		}
	} 
	else 
	{
		Features.HasSSEFP = false;
	}

	if (ChipManufacturer == Intel) 
	{
		Features.ExtendedFeatures.SupportsHyperthreading =  ((cpuFeatures & 0x10000000) != 0);
		Features.ExtendedFeatures.LogicalProcessorsPerPhysical = (Features.ExtendedFeatures.SupportsHyperthreading) ? ((cpuAdvanced & 0x00FF0000) >> 16) : 1;

		if ((Features.ExtendedFeatures.SupportsHyperthreading) && (Features.HasAPIC))
		{
			Features.ExtendedFeatures.APIC_ID = ((cpuAdvanced & 0xFF000000) >> 24);
		}
	}

	return true;
#else
	return false;
#endif

#endif
}

bool ATOM_CPUInfo::retrieveCPUIdentity (void)
{
#if defined(_M_IA64) || defined(_M_AMD64)
	return false;
#else
#if ATOM3D_COMPILER_MSVC
	int CPUVendor[3] = {0, 0, 0};
	int CPUSignature = 0;

	__try 
	{
		__asm 
		{
#ifdef CPUID_AWARE_COMPILER
			push eax
				push ebx
				push ecx
				push edx
#endif
				mov eax, 0
				CPUID_INSTRUCTION
				mov CPUVendor[0 * TYPE int], ebx
				mov CPUVendor[1 * TYPE int], edx
				mov CPUVendor[2 * TYPE int], ecx

				mov eax,1
				CPUID_INSTRUCTION
				mov CPUSignature, eax

#ifdef CPUID_AWARE_COMPILER
				pop edx
				pop ecx
				pop ebx
				pop eax
#endif
		}
	}
	__except (1) 
	{
		return false;
	}

	memcpy (ChipID.Vendor, &(CPUVendor[0]), sizeof (int));
	memcpy (&(ChipID.Vendor[4]), &(CPUVendor[1]), sizeof (int));
	memcpy (&(ChipID.Vendor[8]), &(CPUVendor[2]), sizeof (int));
	ChipID.Vendor[12] = '\0';

	if (strcmp (ChipID.Vendor, "GenuineIntel") == 0)    
		ChipManufacturer = Intel;       // Intel Corp.
	else if (strcmp (ChipID.Vendor, "UMC UMC UMC ") == 0) 
		ChipManufacturer = UMC;         // United Microelectronics Corp.
	else if (strcmp (ChipID.Vendor, "AuthenticAMD") == 0) 
		ChipManufacturer = AMD;         // Advanced Micro Devices
	else if (strcmp (ChipID.Vendor, "AMD ISBETTER") == 0) 
		ChipManufacturer = AMD;         // Advanced Micro Devices (1994)
	else if (strcmp (ChipID.Vendor, "CyrixInstead") == 0) 
		ChipManufacturer = Cyrix;       // Cyrix Corp., VIA Inc.
	else if (strcmp (ChipID.Vendor, "NexGenDriven") == 0) 
		ChipManufacturer = NexGen;        // NexGen Inc. (now AMD)
	else if (strcmp (ChipID.Vendor, "CentaurHauls") == 0) 
		ChipManufacturer = IDT;         // IDT/Centaur (now VIA)
	else if (strcmp (ChipID.Vendor, "RiseRiseRise") == 0) 
		ChipManufacturer = Rise;        // Rise
	else if (strcmp (ChipID.Vendor, "GenuineTMx86") == 0) 
		ChipManufacturer = Transmeta;     // Transmeta
	else if (strcmp (ChipID.Vendor, "TransmetaCPU") == 0) 
		ChipManufacturer = Transmeta;     // Transmeta
	else if (strcmp (ChipID.Vendor, "Geode By NSC") == 0) 
		ChipManufacturer = NSC;         // National Semiconductor
	else                          
		ChipManufacturer = UnknownManufacturer; // Unknown manufacturer

	ChipID.ExtendedFamily =   ((CPUSignature & 0x0FF00000) >> 20);  // Bits 27..20 Used
	ChipID.ExtendedModel =    ((CPUSignature & 0x000F0000) >> 16);  // Bits 19..16 Used
	ChipID.Type =       ((CPUSignature & 0x0000F000) >> 12);  // Bits 15..12 Used
	ChipID.Family =       ((CPUSignature & 0x00000F00) >> 8);   // Bits 11..8 Used
	ChipID.ATOM_Model =        ((CPUSignature & 0x000000F0) >> 4);   // Bits 7..4 Used
	ChipID.Revision =     ((CPUSignature & 0x0000000F) >> 0);   // Bits 3..0 Used

	_itoa (ChipID.Type, szTypeID, 10);
	_itoa (ChipID.Family, szFamilyID, 10);
	_itoa (ChipID.ATOM_Model, szModelID, 10);
	_itoa (ChipID.Revision, szSteppingCode, 10);

	return true;
#else
	return false;
#endif

#endif
}

bool ATOM_CPUInfo::retrieveCPUCacheDetails (void)
{
#if defined(_M_IA64) || defined(_M_AMD64)
	return false;
#else
#if ATOM3D_COMPILER_MSVC
	int L1Cache[4] = { 0, 0, 0, 0 };
	int L2Cache[4] = { 0, 0, 0, 0 };

	if (retrieveCPUExtendedLevelSupport (0x80000005)) 
	{
		__try 
		{
			__asm 
			{
#ifdef CPUID_AWARE_COMPILER
				push eax
					push ebx
					push ecx
					push edx
#endif
					mov eax, 0x80000005
					CPUID_INSTRUCTION
					mov L1Cache[0 * TYPE int], eax
					mov L1Cache[1 * TYPE int], ebx
					mov L1Cache[2 * TYPE int], ecx
					mov L1Cache[3 * TYPE int], edx

#ifdef CPUID_AWARE_COMPILER
					pop edx
					pop ecx
					pop ebx
					pop eax
#endif
			}
		}
		__except (1) 
		{
			return false;
		}

		Features.L1CacheSize = ((L1Cache[2] & 0xFF000000) >> 24);
		Features.L1CacheSize += ((L1Cache[3] & 0xFF000000) >> 24);
	} 
	else 
	{
		Features.L1CacheSize = 0;
	}

	if (retrieveCPUExtendedLevelSupport (0x80000006)) 
	{
		__try 
		{
			__asm 
			{
#ifdef CPUID_AWARE_COMPILER
				push eax
					push ebx
					push ecx
					push edx
#endif
					mov eax, 0x80000006
					CPUID_INSTRUCTION
					mov L2Cache[0 * TYPE int], eax
					mov L2Cache[1 * TYPE int], ebx
					mov L2Cache[2 * TYPE int], ecx
					mov L2Cache[3 * TYPE int], edx

#ifdef CPUID_AWARE_COMPILER
					pop edx
					pop ecx
					pop ebx
					pop eax
#endif      
			}
		}
		__except (1) 
		{
			return false;
		}

		Features.L2CacheSize = ((L2Cache[2] & 0xFFFF0000) >> 16);
	} 
	else 
	{
		Features.L2CacheSize = 0;
	}

	Features.L3CacheSize = 0;

	return ((Features.L1CacheSize == 0) && (Features.L2CacheSize == 0)) ? false : true;
#else
	return false;
#endif

#endif
}

bool ATOM_CPUInfo::retrieveClassicalCPUCacheDetails (void)
{
#if defined(_M_IA64) || defined(_M_AMD64)
	return false;
#else
#if ATOM3D_COMPILER_MSVC
	int TLBCode = 0, TLBData = -0, L1Code = 0, L1Data = 0, L1Trace = 0, L2Unified = 0, L3Unified = 0;
	int TLBCacheData[4] = { 0, 0, 0, 0 };
	int TLBPassCounter = 0;
	int TLBCacheUnit = 0;

	do 
	{
		__try 
		{
			__asm 
			{
#ifdef CPUID_AWARE_COMPILER
				push eax
					push ebx
					push ecx
					push edx
#endif
					mov eax, 2
					CPUID_INSTRUCTION
					mov TLBCacheData[0 * TYPE int], eax
					mov TLBCacheData[1 * TYPE int], ebx
					mov TLBCacheData[2 * TYPE int], ecx
					mov TLBCacheData[3 * TYPE int], edx

#ifdef CPUID_AWARE_COMPILER
					pop edx
					pop ecx
					pop ebx
					pop eax
#endif
			}
		}
		__except (1) 
		{
			return false;
		}

		int bob = ((TLBCacheData[0] & 0x00FF0000) >> 16);

		for (int nCounter = 0; nCounter < TLBCACHE_INFO_UNITS; nCounter ++) 
		{
			switch (nCounter) 
			{
			case 0: TLBCacheUnit = ((TLBCacheData[0] & 0x0000FF00) >> 8); break;
			case 1: TLBCacheUnit = ((TLBCacheData[0] & 0x00FF0000) >> 16); break;
			case 2: TLBCacheUnit = ((TLBCacheData[0] & 0xFF000000) >> 24); break;

			case 3: TLBCacheUnit = ((TLBCacheData[1] & 0x000000FF) >> 0); break;
			case 4: TLBCacheUnit = ((TLBCacheData[1] & 0x0000FF00) >> 8); break;
			case 5: TLBCacheUnit = ((TLBCacheData[1] & 0x00FF0000) >> 16); break;
			case 6: TLBCacheUnit = ((TLBCacheData[1] & 0xFF000000) >> 24); break;

			case 7: TLBCacheUnit = ((TLBCacheData[2] & 0x000000FF) >> 0); break;
			case 8: TLBCacheUnit = ((TLBCacheData[2] & 0x0000FF00) >> 8); break;
			case 9: TLBCacheUnit = ((TLBCacheData[2] & 0x00FF0000) >> 16); break;
			case 10: TLBCacheUnit = ((TLBCacheData[2] & 0xFF000000) >> 24); break;

			case 11: TLBCacheUnit = ((TLBCacheData[3] & 0x000000FF) >> 0); break;
			case 12: TLBCacheUnit = ((TLBCacheData[3] & 0x0000FF00) >> 8); break;
			case 13: TLBCacheUnit = ((TLBCacheData[3] & 0x00FF0000) >> 16); break;
			case 14: TLBCacheUnit = ((TLBCacheData[3] & 0xFF000000) >> 24); break;

			default: return false;
			}

			switch (TLBCacheUnit) 
			{
			case 0x00: break;
			case 0x01: STORE_TLBCACHE_INFO (TLBCode, 4); break;
			case 0x02: STORE_TLBCACHE_INFO (TLBCode, 4096); break;
			case 0x03: STORE_TLBCACHE_INFO (TLBData, 4); break;
			case 0x04: STORE_TLBCACHE_INFO (TLBData, 4096); break;
			case 0x06: STORE_TLBCACHE_INFO (L1Code, 8); break;
			case 0x08: STORE_TLBCACHE_INFO (L1Code, 16); break;
			case 0x0a: STORE_TLBCACHE_INFO (L1Data, 8); break;
			case 0x0c: STORE_TLBCACHE_INFO (L1Data, 16); break;
			case 0x10: STORE_TLBCACHE_INFO (L1Data, 16); break;     // <-- FIXME: IA-64 Only
			case 0x15: STORE_TLBCACHE_INFO (L1Code, 16); break;     // <-- FIXME: IA-64 Only
			case 0x1a: STORE_TLBCACHE_INFO (L2Unified, 96); break;    // <-- FIXME: IA-64 Only
			case 0x22: STORE_TLBCACHE_INFO (L3Unified, 512); break;
			case 0x23: STORE_TLBCACHE_INFO (L3Unified, 1024); break;
			case 0x25: STORE_TLBCACHE_INFO (L3Unified, 2048); break;
			case 0x29: STORE_TLBCACHE_INFO (L3Unified, 4096); break;
			case 0x39: STORE_TLBCACHE_INFO (L2Unified, 128); break;
			case 0x3c: STORE_TLBCACHE_INFO (L2Unified, 256); break;
			case 0x40: STORE_TLBCACHE_INFO (L2Unified, 0); break;   // <-- FIXME: No integrated L2 cache (P6 core) or L3 cache (P4 core).
			case 0x41: STORE_TLBCACHE_INFO (L2Unified, 128); break;
			case 0x42: STORE_TLBCACHE_INFO (L2Unified, 256); break;
			case 0x43: STORE_TLBCACHE_INFO (L2Unified, 512); break;
			case 0x44: STORE_TLBCACHE_INFO (L2Unified, 1024); break;
			case 0x45: STORE_TLBCACHE_INFO (L2Unified, 2048); break;
			case 0x50: STORE_TLBCACHE_INFO (TLBCode, 4096); break;
			case 0x51: STORE_TLBCACHE_INFO (TLBCode, 4096); break;
			case 0x52: STORE_TLBCACHE_INFO (TLBCode, 4096); break;
			case 0x5b: STORE_TLBCACHE_INFO (TLBData, 4096); break;
			case 0x5c: STORE_TLBCACHE_INFO (TLBData, 4096); break;
			case 0x5d: STORE_TLBCACHE_INFO (TLBData, 4096); break;
			case 0x66: STORE_TLBCACHE_INFO (L1Data, 8); break;
			case 0x67: STORE_TLBCACHE_INFO (L1Data, 16); break;
			case 0x68: STORE_TLBCACHE_INFO (L1Data, 32); break;
			case 0x70: STORE_TLBCACHE_INFO (L1Trace, 12); break;
			case 0x71: STORE_TLBCACHE_INFO (L1Trace, 16); break;
			case 0x72: STORE_TLBCACHE_INFO (L1Trace, 32); break;
			case 0x77: STORE_TLBCACHE_INFO (L1Code, 16); break;     // <-- FIXME: IA-64 Only
			case 0x79: STORE_TLBCACHE_INFO (L2Unified, 128); break;
			case 0x7a: STORE_TLBCACHE_INFO (L2Unified, 256); break;
			case 0x7b: STORE_TLBCACHE_INFO (L2Unified, 512); break;
			case 0x7c: STORE_TLBCACHE_INFO (L2Unified, 1024); break;
			case 0x7e: STORE_TLBCACHE_INFO (L2Unified, 256); break;
			case 0x81: STORE_TLBCACHE_INFO (L2Unified, 128); break;
			case 0x82: STORE_TLBCACHE_INFO (L2Unified, 256); break;
			case 0x83: STORE_TLBCACHE_INFO (L2Unified, 512); break;
			case 0x84: STORE_TLBCACHE_INFO (L2Unified, 1024); break;
			case 0x85: STORE_TLBCACHE_INFO (L2Unified, 2048); break;
			case 0x88: STORE_TLBCACHE_INFO (L3Unified, 2048); break;  // <-- FIXME: IA-64 Only
			case 0x89: STORE_TLBCACHE_INFO (L3Unified, 4096); break;  // <-- FIXME: IA-64 Only
			case 0x8a: STORE_TLBCACHE_INFO (L3Unified, 8192); break;  // <-- FIXME: IA-64 Only
			case 0x8d: STORE_TLBCACHE_INFO (L3Unified, 3096); break;  // <-- FIXME: IA-64 Only
			case 0x90: STORE_TLBCACHE_INFO (TLBCode, 262144); break;  // <-- FIXME: IA-64 Only
			case 0x96: STORE_TLBCACHE_INFO (TLBCode, 262144); break;  // <-- FIXME: IA-64 Only
			case 0x9b: STORE_TLBCACHE_INFO (TLBCode, 262144); break;  // <-- FIXME: IA-64 Only

				// Default case - an error has occured.
			default: return false;
			}
		}

		TLBPassCounter ++;

	} while ((TLBCacheData[0] & 0x000000FF) > TLBPassCounter);

	if ((L1Code == 0) && (L1Data == 0) && (L1Trace == 0)) 
		Features.L1CacheSize = 0;
	else if ((L1Code == 0) && (L1Data == 0) && (L1Trace != 0)) 
		Features.L1CacheSize = L1Trace;
	else if ((L1Code != 0) && (L1Data == 0)) 
		Features.L1CacheSize = L1Code;
	else if ((L1Code == 0) && (L1Data != 0)) 
		Features.L1CacheSize = L1Data;
	else if ((L1Code != 0) && (L1Data != 0)) 
		Features.L1CacheSize = L1Code + L1Data;
	else 
		Features.L1CacheSize = 0;

	if (L2Unified == 0) 
		Features.L2CacheSize = 0;
	else 
		Features.L2CacheSize = L2Unified;

	if (L3Unified == 0) 
		Features.L3CacheSize = 0;
	else 
		Features.L3CacheSize = L3Unified;

	return true;
#else
	return false;
#endif

#endif
}

bool ATOM_CPUInfo::retrieveCPUClockSpeed (void)
{
	if (!Features.HasTSC) 
		return false;

	Speed = ATOM_NEW(CPUSpeed);
	if (Speed == NULL) 
		return false;

	return true;
}

bool ATOM_CPUInfo::retrieveClassicalCPUClockSpeed (void)
{
#if defined(_M_IA64) || defined(_M_AMD64)
	return false;
#else
#if ATOM3D_COMPILER_MSVC
	LARGE_INTEGER liStart, liEnd, liCountsPerSecond;
	double dFrequency, dDifference;

	QueryPerformanceCounter (&liStart);

	__try 
	{
		__asm 
		{
			mov eax, 0x80000000
				mov ebx, CLASSICAL_CPU_FREQ_LOOP
Timer_Loop: 
			bsf ecx,eax
				dec ebx
				jnz Timer_Loop
		} 
	}
	__except (1) 
	{
		return false;
	}

	QueryPerformanceCounter (&liEnd);
	QueryPerformanceFrequency (&liCountsPerSecond);
	dDifference = (((double) liEnd.QuadPart - (double) liStart.QuadPart) / (double) liCountsPerSecond.QuadPart);

	if (ChipID.Family == 3) 
	{
		dFrequency = (((CLASSICAL_CPU_FREQ_LOOP * 115) / dDifference) / 1048576);
	} 
	else if (ChipID.Family == 4) 
	{
		dFrequency = (((CLASSICAL_CPU_FREQ_LOOP * 47) / dDifference) / 1048576);
	} 
	else if (ChipID.Family == 5) 
	{
		dFrequency = (((CLASSICAL_CPU_FREQ_LOOP * 43) / dDifference) / 1048576);
	}
	else
	{
		dFrequency = 0;
	}

	Features.CPUSpeed = (int) dFrequency;

	return true;
#else
	return false;
#endif

#endif
}

bool  ATOM_CPUInfo::retrieveCPUExtendedLevelSupport (unsigned CPULevelToCheck)
{
#if defined(_M_IA64) || defined(_M_AMD64)
	return false;
#else
#if ATOM3D_COMPILER_MSVC
	int MaxCPUExtendedLevel = 0;

	if (ChipManufacturer == AMD) 
	{
		if (ChipID.Family < 5) return false;
		if ((ChipID.Family == 5) && (ChipID.ATOM_Model < 6)) return false;
	} 
	else if (ChipManufacturer == Cyrix) 
	{
		if (ChipID.Family < 5) return false;
		if ((ChipID.Family == 5) && (ChipID.ATOM_Model < 4)) return false;
		if ((ChipID.Family == 6) && (ChipID.ATOM_Model < 5)) return false;
	} 
	else if (ChipManufacturer == IDT) 
	{
		if (ChipID.Family < 5) return false;
		if ((ChipID.Family == 5) && (ChipID.ATOM_Model < 8)) return false;
	} 
	else if (ChipManufacturer == Transmeta) 
	{
		if (ChipID.Family < 5) return false;
	} 
	else if (ChipManufacturer == Intel) 
	{
		if (ChipID.Family < 0xf) return false;
	}

	__try 
	{
		__asm 
		{
#ifdef CPUID_AWARE_COMPILER
			push eax
				push ebx
				push ecx
				push edx
#endif
				mov eax,0x80000000
				CPUID_INSTRUCTION
				mov MaxCPUExtendedLevel, eax

#ifdef CPUID_AWARE_COMPILER
				pop edx
				pop ecx
				pop ebx
				pop eax
#endif
		}
	}
	__except (1) 
	{
		return false;
	}

	unsigned nLevelWanted = (CPULevelToCheck & 0x7FFFFFFF);
	unsigned nLevelReturn = (MaxCPUExtendedLevel & 0x7FFFFFFF);

	if (nLevelWanted > nLevelReturn) return false;

	return true;
#else
	return false;
#endif

#endif
}

bool  ATOM_CPUInfo::retrieveExtendedCPUFeatures (void)
{
#if defined(_M_IA64) || defined(_M_AMD64)
	return false;
#else
#if ATOM3D_COMPILER_MSVC
	int cpuExtendedFeatures = 0;

	if (ChipManufacturer == Intel) return false;

	if (!retrieveCPUExtendedLevelSupport (0x80000001)) return false;

	__try 
	{
		__asm 
		{
#ifdef CPUID_AWARE_COMPILER
			push eax
				push ebx
				push ecx
				push edx
#endif
				mov eax,0x80000001
				CPUID_INSTRUCTION
				mov cpuExtendedFeatures, edx

#ifdef CPUID_AWARE_COMPILER
				pop edx
				pop ecx
				pop ebx
				pop eax
#endif
		}
	}
	__except (1) 
	{
		return false;
	}

	Features.ExtendedFeatures.Has3DNow =    ((cpuExtendedFeatures & 0x80000000) != 0);  // 3DNow Present --> Bit 31.
	Features.ExtendedFeatures.Has3DNowPlus =  ((cpuExtendedFeatures & 0x40000000) != 0);  // 3DNow+ Present -- > Bit 30.
	Features.ExtendedFeatures.HasSSEMMX =   ((cpuExtendedFeatures & 0x00400000) != 0);  // SSE MMX Present --> Bit 22.
	Features.ExtendedFeatures.SupportsMP =    ((cpuExtendedFeatures & 0x00080000) != 0);  // MP Capable -- > Bit 19.

	if (ChipManufacturer == AMD) 
	{
		Features.ExtendedFeatures.HasMMXPlus =  ((cpuExtendedFeatures & 0x00400000) != 0);  // AMD specific: MMX-SSE --> Bit 22
	}

	if (ChipManufacturer == Cyrix) 
	{
		Features.ExtendedFeatures.HasMMXPlus =  ((cpuExtendedFeatures & 0x01000000) != 0);  // Cyrix specific: Extended MMX --> Bit 24
	}

	return true;
#else
	return false;
#endif

#endif
}

bool ATOM_CPUInfo::retrieveProcessorSerialNumber (void)
{
#if defined(_M_IA64) || defined(_M_AMD64)
	return false;
#else
#if ATOM3D_COMPILER_MSVC
	int SerialNumber[3] = { 0, 0, 0 };

	if (!Features.HasSerial) 
	{
		ChipID.SerialNumber[0] = '\0';
		return false;
	}

	__try 
	{
		__asm 
		{
#ifdef CPUID_AWARE_COMPILER
			push eax
				push ebx
				push ecx
				push edx
#endif
				mov eax, 3
				CPUID_INSTRUCTION
				mov SerialNumber[0 * TYPE int], ebx
				mov SerialNumber[1 * TYPE int], ecx
				mov SerialNumber[2 * TYPE int], edx
#ifdef CPUID_AWARE_COMPILER
				pop edx
				pop ecx
				pop ebx
				pop eax
#endif
		}
	}
	__except (1) 
	{
		return false;
	}

	sprintf (ChipID.SerialNumber, "%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x",
		((SerialNumber[0] & 0xff000000) >> 24),
		((SerialNumber[0] & 0x00ff0000) >> 16),
		((SerialNumber[0] & 0x0000ff00) >> 8),
		((SerialNumber[0] & 0x000000ff) >> 0),
		((SerialNumber[1] & 0xff000000) >> 24),
		((SerialNumber[1] & 0x00ff0000) >> 16),
		((SerialNumber[1] & 0x0000ff00) >> 8),
		((SerialNumber[1] & 0x000000ff) >> 0),
		((SerialNumber[2] & 0xff000000) >> 24),
		((SerialNumber[2] & 0x00ff0000) >> 16),
		((SerialNumber[2] & 0x0000ff00) >> 8),
		((SerialNumber[2] & 0x000000ff) >> 0));

	return true;
#else
	return false;
#endif

#endif
}

bool ATOM_CPUInfo::retrieveCPUPowerManagement (void)
{ 
#if defined(_M_IA64) || defined(_M_AMD64)
	return false;
#else
#if ATOM3D_COMPILER_MSVC
	int cpuPowerManagement = 0;

	if (!retrieveCPUExtendedLevelSupport (0x80000007)) 
	{
		Features.ExtendedFeatures.PowerManagement.HasFrequencyID = false;
		Features.ExtendedFeatures.PowerManagement.HasVoltageID = false;
		Features.ExtendedFeatures.PowerManagement.HasTempSenseDiode = false;
		return false;
	}

	__try 
	{
		__asm 
		{
#ifdef CPUID_AWARE_COMPILER
			push eax
				push ebx
				push ecx
				push edx
#endif
				mov eax,0x80000007
				CPUID_INSTRUCTION
				mov cpuPowerManagement, edx

#ifdef CPUID_AWARE_COMPILER
				pop edx
				pop ecx
				pop ebx
				pop eax
#endif
		}
	}
	__except (1) 
	{
		return false;
	}

	Features.ExtendedFeatures.PowerManagement.HasTempSenseDiode = ((cpuPowerManagement & 0x00000001) != 0);
	Features.ExtendedFeatures.PowerManagement.HasFrequencyID =    ((cpuPowerManagement & 0x00000002) != 0);
	Features.ExtendedFeatures.PowerManagement.HasVoltageID =    ((cpuPowerManagement & 0x00000004) != 0);

	return true;
#else
	return false;
#endif

#endif
}

bool ATOM_CPUInfo::retrieveExtendedCPUIdentity (void)
{
#if defined(_M_IA64) || defined(_M_AMD64)
	return false;
#else
#if ATOM3D_COMPILER_MSVC
	int ProcessorNameStartPos = 0;
	int CPUExtendedIdentity[12] = { 0 };

	if (!retrieveCPUExtendedLevelSupport (0x80000002)) return false;
	if (!retrieveCPUExtendedLevelSupport (0x80000003)) return false;
	if (!retrieveCPUExtendedLevelSupport (0x80000004)) return false;

	__try 
	{
		__asm 
		{
#ifdef CPUID_AWARE_COMPILER
			push eax
				push ebx
				push ecx
				push edx
#endif
				mov eax,0x80000002
				CPUID_INSTRUCTION
				mov CPUExtendedIdentity[0 * TYPE int], eax
				mov CPUExtendedIdentity[1 * TYPE int], ebx
				mov CPUExtendedIdentity[2 * TYPE int], ecx
				mov CPUExtendedIdentity[3 * TYPE int], edx

				mov eax,0x80000003
				CPUID_INSTRUCTION
				mov CPUExtendedIdentity[4 * TYPE int], eax
				mov CPUExtendedIdentity[5 * TYPE int], ebx
				mov CPUExtendedIdentity[6 * TYPE int], ecx
				mov CPUExtendedIdentity[7 * TYPE int], edx

				mov eax,0x80000004
				CPUID_INSTRUCTION
				mov CPUExtendedIdentity[8 * TYPE int], eax
				mov CPUExtendedIdentity[9 * TYPE int], ebx
				mov CPUExtendedIdentity[10 * TYPE int], ecx
				mov CPUExtendedIdentity[11 * TYPE int], edx

#ifdef CPUID_AWARE_COMPILER
				pop edx
				pop ecx
				pop ebx
				pop eax
#endif
		}
	}
	__except (1) 
	{
		return false;
	}

	memcpy (ChipID.ProcessorName, &(CPUExtendedIdentity[0]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[4]), &(CPUExtendedIdentity[1]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[8]), &(CPUExtendedIdentity[2]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[12]), &(CPUExtendedIdentity[3]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[16]), &(CPUExtendedIdentity[4]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[20]), &(CPUExtendedIdentity[5]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[24]), &(CPUExtendedIdentity[6]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[28]), &(CPUExtendedIdentity[7]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[32]), &(CPUExtendedIdentity[8]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[36]), &(CPUExtendedIdentity[9]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[40]), &(CPUExtendedIdentity[10]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[44]), &(CPUExtendedIdentity[11]), sizeof (int));
	ChipID.ProcessorName[48] = '\0';

	if (ChipManufacturer == Intel) 
	{
		for (int nCounter = 0; nCounter < CHIPNAME_STRING_LENGTH; nCounter ++) 
		{
			if ((ChipID.ProcessorName[nCounter] != '\0') && (ChipID.ProcessorName[nCounter] != ' ')) 
			{
				ProcessorNameStartPos = nCounter;
				break;
			}
		}

		if (ProcessorNameStartPos == 0) return true;

		memmove (ChipID.ProcessorName, &(ChipID.ProcessorName[ProcessorNameStartPos]), (CHIPNAME_STRING_LENGTH - ProcessorNameStartPos));
	}

	return true;
#else
	return false;
#endif

#endif
}

bool ATOM_CPUInfo::retrieveClassicalCPUIdentity (void)
{
	switch (ChipManufacturer) 
	{
	case Intel:
		switch (ChipID.Family) 
		{
		case 3:
			sprintf (ChipID.ProcessorName, "Newer i80386 family"); 
			break;
		case 4:
			switch (ChipID.ATOM_Model) 
			{
			case 0: STORE_CLASSICAL_NAME ("i80486DX-25/33"); break;
			case 1: STORE_CLASSICAL_NAME ("i80486DX-50"); break;
			case 2: STORE_CLASSICAL_NAME ("i80486SX"); break;
			case 3: STORE_CLASSICAL_NAME ("i80486DX2"); break;
			case 4: STORE_CLASSICAL_NAME ("i80486SL"); break;
			case 5: STORE_CLASSICAL_NAME ("i80486SX2"); break;
			case 7: STORE_CLASSICAL_NAME ("i80486DX2 WriteBack"); break;
			case 8: STORE_CLASSICAL_NAME ("i80486DX4"); break;
			case 9: STORE_CLASSICAL_NAME ("i80486DX4 WriteBack"); break;
			default: STORE_CLASSICAL_NAME ("Unknown 80486 family"); return false;
			}
			break;
		case 5:
			switch (ChipID.ATOM_Model) 
			{
			case 0: STORE_CLASSICAL_NAME ("P5 A-Step"); break;
			case 1: STORE_CLASSICAL_NAME ("P5"); break;
			case 2: STORE_CLASSICAL_NAME ("P54C"); break;
			case 3: STORE_CLASSICAL_NAME ("P24T OverDrive"); break;
			case 4: STORE_CLASSICAL_NAME ("P55C"); break;
			case 7: STORE_CLASSICAL_NAME ("P54C"); break;
			case 8: STORE_CLASSICAL_NAME ("P55C (0.25µm)"); break;
			default: STORE_CLASSICAL_NAME ("Unknown Pentium?family"); return false;
			}
			break;
		case 6:
			switch (ChipID.ATOM_Model) 
			{
			case 0: STORE_CLASSICAL_NAME ("P6 A-Step"); break;
			case 1: STORE_CLASSICAL_NAME ("P6"); break;
			case 3: STORE_CLASSICAL_NAME ("Pentium?II (0.28 µm)"); break;
			case 5: STORE_CLASSICAL_NAME ("Pentium?II (0.25 µm)"); break;
			case 6: STORE_CLASSICAL_NAME ("Pentium?II With On-Die L2 Cache"); break;
			case 7: STORE_CLASSICAL_NAME ("Pentium?III (0.25 µm)"); break;
			case 8: STORE_CLASSICAL_NAME ("Pentium?III (0.18 µm) With 256 KB On-Die L2 Cache "); break;
			case 0xa: STORE_CLASSICAL_NAME ("Pentium?III (0.18 µm) With 1 Or 2 MB On-Die L2 Cache "); break;
			case 0xb: STORE_CLASSICAL_NAME ("Pentium?III (0.13 µm) With 256 Or 512 KB On-Die L2 Cache "); break;
			default: STORE_CLASSICAL_NAME ("Unknown P6 family"); return false;
			}
			break;
		case 7:
			STORE_CLASSICAL_NAME ("Intel Merced (IA-64)");
			break;
		case 0xf:
			switch (ChipID.ExtendedFamily) 
			{
			case 0:
				switch (ChipID.ATOM_Model) 
				{
				case 0: STORE_CLASSICAL_NAME ("Pentium?IV (0.18 µm)"); break;
				case 1: STORE_CLASSICAL_NAME ("Pentium?IV (0.18 µm)"); break;
				case 2: STORE_CLASSICAL_NAME ("Pentium?IV (0.13 µm)"); break;
				default: STORE_CLASSICAL_NAME ("Unknown Pentium 4 family"); return false;
				}
				break;
			case 1:
				STORE_CLASSICAL_NAME ("Intel McKinley (IA-64)");
				break;
			}
			break;
		default:
			STORE_CLASSICAL_NAME ("Unknown Intel family");
			return false;
		}
		break;

	case AMD:
		switch (ChipID.Family) 
		{
		case 4:
			switch (ChipID.ATOM_Model) 
			{
			case 3: STORE_CLASSICAL_NAME ("80486DX2"); break;
			case 7: STORE_CLASSICAL_NAME ("80486DX2 WriteBack"); break;
			case 8: STORE_CLASSICAL_NAME ("80486DX4"); break;
			case 9: STORE_CLASSICAL_NAME ("80486DX4 WriteBack"); break;
			case 0xe: STORE_CLASSICAL_NAME ("5x86"); break;
			case 0xf: STORE_CLASSICAL_NAME ("5x86WB"); break;
			default: STORE_CLASSICAL_NAME ("Unknown 80486 family"); return false;
			}
			break;
		case 5:
			switch (ChipID.ATOM_Model) 
			{
			case 0: STORE_CLASSICAL_NAME ("SSA5 (PR75, PR90, PR100)"); break;
			case 1: STORE_CLASSICAL_NAME ("5k86 (PR120, PR133)"); break;
			case 2: STORE_CLASSICAL_NAME ("5k86 (PR166)"); break;
			case 3: STORE_CLASSICAL_NAME ("5k86 (PR200)"); break;
			case 6: STORE_CLASSICAL_NAME ("K6 (0.30 µm)"); break;
			case 7: STORE_CLASSICAL_NAME ("K6 (0.25 µm)"); break;
			case 8: STORE_CLASSICAL_NAME ("K6-2"); break;
			case 9: STORE_CLASSICAL_NAME ("K6-III"); break;
			case 0xd: STORE_CLASSICAL_NAME ("K6-2+ or K6-III+ (0.18 µm)"); break;
			default: STORE_CLASSICAL_NAME ("Unknown 80586 family"); return false;
			}
			break;
		case 6:
			switch (ChipID.ATOM_Model) 
			{
			case 1: STORE_CLASSICAL_NAME ("Athlon?(0.25 µm)"); break;
			case 2: STORE_CLASSICAL_NAME ("Athlon?(0.18 µm)"); break;
			case 3: STORE_CLASSICAL_NAME ("Duron?(SF core)"); break;
			case 4: STORE_CLASSICAL_NAME ("Athlon?(Thunderbird core)"); break;
			case 6: STORE_CLASSICAL_NAME ("Athlon?(Palomino core)"); break;
			case 7: STORE_CLASSICAL_NAME ("Duron?(Morgan core)"); break;
			case 8: 
				if (Features.ExtendedFeatures.SupportsMP)
					STORE_CLASSICAL_NAME ("Athlon?MP (Thoroughbred core)"); 
				else STORE_CLASSICAL_NAME ("Athlon?XP (Thoroughbred core)");
				break;
			default: STORE_CLASSICAL_NAME ("Unknown K7 family"); return false;
			}
			break;
		default:
			STORE_CLASSICAL_NAME ("Unknown AMD family");
			return false;
		}
		break;

	case Transmeta:
		switch (ChipID.Family) 
		{ 
		case 5:
			switch (ChipID.ATOM_Model) 
			{
			case 4: STORE_CLASSICAL_NAME ("Crusoe TM3x00 and TM5x00"); break;
			default: STORE_CLASSICAL_NAME ("Unknown Crusoe family"); return false;
			}
			break;
		default:
			STORE_CLASSICAL_NAME ("Unknown Transmeta family");
			return false;
		}
		break;

	case Rise:
		switch (ChipID.Family) 
		{ 
		case 5:
			switch (ChipID.ATOM_Model) 
			{
			case 0: STORE_CLASSICAL_NAME ("mP6 (0.25 µm)"); break;
			case 2: STORE_CLASSICAL_NAME ("mP6 (0.18 µm)"); break;
			default: STORE_CLASSICAL_NAME ("Unknown Rise family"); return false;
			}
			break;
		default:
			STORE_CLASSICAL_NAME ("Unknown Rise family");
			return false;
		}
		break;

	case UMC:
		switch (ChipID.Family) 
		{ 
		case 4:
			switch (ChipID.ATOM_Model) 
			{
			case 1: STORE_CLASSICAL_NAME ("U5D"); break;
			case 2: STORE_CLASSICAL_NAME ("U5S"); break;
			default: STORE_CLASSICAL_NAME ("Unknown UMC family"); return false;
			}
			break;
		default:
			STORE_CLASSICAL_NAME ("Unknown UMC family");
			return false;
		}
		break;

	case IDT:
		switch (ChipID.Family) 
		{ 
		case 5:
			switch (ChipID.ATOM_Model) 
			{
			case 4: STORE_CLASSICAL_NAME ("C6"); break;
			case 8: STORE_CLASSICAL_NAME ("C2"); break;
			case 9: STORE_CLASSICAL_NAME ("C3"); break;
			default: STORE_CLASSICAL_NAME ("Unknown IDT\\Centaur family"); return false;
			}
			break;
		case 6:
			switch (ChipID.ATOM_Model) 
			{
			case 6: STORE_CLASSICAL_NAME ("VIA Cyrix III - Samuel"); break;
			default: STORE_CLASSICAL_NAME ("Unknown IDT\\Centaur family"); return false;
			}
			break;
		default:
			STORE_CLASSICAL_NAME ("Unknown IDT\\Centaur family");
			return false;
		}
		break;

	case Cyrix:
		switch (ChipID.Family) 
		{ 
		case 4:
			switch (ChipID.ATOM_Model) 
			{
			case 4: STORE_CLASSICAL_NAME ("MediaGX GX, GXm"); break;
			case 9: STORE_CLASSICAL_NAME ("5x86"); break;
			default: STORE_CLASSICAL_NAME ("Unknown Cx5x86 family"); return false;
			}
			break;
		case 5:
			switch (ChipID.ATOM_Model) 
			{
			case 2: STORE_CLASSICAL_NAME ("Cx6x86"); break;
			case 4: STORE_CLASSICAL_NAME ("MediaGX GXm"); break;
			default: STORE_CLASSICAL_NAME ("Unknown Cx6x86 family"); return false;
			}
			break;
		case 6:
			switch (ChipID.ATOM_Model) 
			{
			case 0: STORE_CLASSICAL_NAME ("6x86MX"); break;
			case 5: STORE_CLASSICAL_NAME ("Cyrix M2 Core"); break;
			case 6: STORE_CLASSICAL_NAME ("WinChip C5A Core"); break;
			case 7: STORE_CLASSICAL_NAME ("WinChip C5B\\C5C Core"); break;
			case 8: STORE_CLASSICAL_NAME ("WinChip C5C-T Core"); break;
			default: STORE_CLASSICAL_NAME ("Unknown 6x86MX\\Cyrix III family"); return false;
			}
			break;
		default:
			STORE_CLASSICAL_NAME ("Unknown Cyrix family");
			return false;
		}
		break;

	case NexGen:
		switch (ChipID.Family) 
		{ 
		case 5:
			switch (ChipID.ATOM_Model) 
			{
			case 0: STORE_CLASSICAL_NAME ("Nx586 or Nx586FPU"); break;
			default: STORE_CLASSICAL_NAME ("Unknown NexGen family"); return false;
			}
			break;
		default:
			STORE_CLASSICAL_NAME ("Unknown NexGen family");
			return false;
		}
		break;

	case NSC:
		STORE_CLASSICAL_NAME ("Cx486SLC \\ DLC \\ Cx486S A-Step");
		break;

	default:
		STORE_CLASSICAL_NAME ("Unknown family");
		return false;
	}

	return true;
}

CPUSpeed::CPUSpeed (void)
{
	unsigned int uiRepetitions = 1;
	unsigned int uiMSecPerRepetition = 50;
	__int64 i64Total = 0, i64Overhead = 0;

	for (unsigned int nCounter = 0; nCounter < uiRepetitions; nCounter ++) 
	{
		i64Total += getCyclesDifference (CPUSpeed::delay, uiMSecPerRepetition);
		i64Overhead += getCyclesDifference (CPUSpeed::delayOverhead, uiMSecPerRepetition);
	}

	i64Total -= i64Overhead;
	i64Total /= uiRepetitions;
	i64Total /= uiMSecPerRepetition;
	i64Total /= 1000;

	CPUSpeedInMHz = (int) i64Total;
}

CPUSpeed::~CPUSpeed ()
{
}

ATOM_LongLong CPUSpeed::getCyclesDifference (DELAY_FUNC DelayFunction, unsigned int uiParameter)
{
#if defined(_M_IA64) || defined(_M_AMD64)
	return 0;
#else
#if ATOM3D_COMPILER_MSVC
	unsigned int edx1 = 0, eax1 = 0;
	unsigned int edx2 = 0, eax2 = 0;

	__try 
	{
		__asm 
		{
			push uiParameter    ; push parameter param
				mov ebx, DelayFunction  ; store func in ebx

				RDTSC_INSTRUCTION

				mov esi, eax      ; esi = eax
				mov edi, edx      ; edi = edx

				call ebx        ; call the delay functions

				RDTSC_INSTRUCTION

				pop ebx

				mov edx2, edx     ; edx2 = edx
				mov eax2, eax     ; eax2 = eax

				mov edx1, edi     ; edx2 = edi
				mov eax1, esi     ; eax2 = esi
		}
	}
	__except (1) 
	{
		return 0;
	}

	return (CPUSPEED_I32TO64 (edx2, eax2) - CPUSPEED_I32TO64 (edx1, eax1));
#else
	return 0;
#endif

#endif
}

void CPUSpeed::delay (unsigned int uiMS)
{
	LARGE_INTEGER Frequency, StartCounter, EndCounter;
	__int64 x;

	if (!QueryPerformanceFrequency (&Frequency)) return;
	x = Frequency.QuadPart / 1000 * uiMS;

	QueryPerformanceCounter (&StartCounter);

	do 
	{
		QueryPerformanceCounter (&EndCounter);
	} while (EndCounter.QuadPart - StartCounter.QuadPart < x);
}

void CPUSpeed::delayOverhead (unsigned int uiMS)
{
	LARGE_INTEGER Frequency, StartCounter, EndCounter;
	__int64 x;

	if (!QueryPerformanceFrequency (&Frequency)) return;
	x = Frequency.QuadPart / 1000 * uiMS;

	QueryPerformanceCounter (&StartCounter);

	do 
	{
		QueryPerformanceCounter (&EndCounter);
	} 
	while (EndCounter.QuadPart - StartCounter.QuadPart == x);
}
