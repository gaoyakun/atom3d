#include "stdafx.h"

struct ProfileInfo
{
  unsigned long long totalTime;
  unsigned numCalls;
  unsigned long long _last;
  ProfileInfo (void): totalTime(0), numCalls(0), _last(0) {}
};

ATOM_MAP<ATOM_STRING, ProfileInfo> *sProfilers = 0;

void ATOM_CleanupProfiler (void)
{
  ATOM_DELETE(sProfilers);
  sProfilers = 0;
}

static inline unsigned long long get_current_cpu_counter (void) {
	unsigned nLow = 0, nHigh = 0;
#if ATOM3D_COMPILER_GCC
	__asm__ __volatile__("rdtsc" : "=a" (nLow), "=d" (nHigh));
#elif !defined(_M_IA64) && !defined(_M_AMD64)
	__asm rdtsc
	__asm mov nLow, eax
	__asm mov nHigh, edx
#else
	return __rdtsc();
#endif
	unsigned long long h =nHigh;
	unsigned long long l = nLow;
	return (h<<32)|l;
}

ATOM_KERNEL_API void ATOM_CALL ATOM_BeginProfile (const char *id)
{
  typedef ATOM_MAP<ATOM_STRING,ProfileInfo> ProfileMap;

  if (!sProfilers)
    sProfilers = ATOM_NEW(ProfileMap);

  ProfileInfo & info = (*sProfilers)[id];
  info._last = get_current_cpu_counter ();
}

ATOM_KERNEL_API void ATOM_CALL ATOM_EndProfile (const char *id)
{
  if (!sProfilers)
    return;

  unsigned long long counter = get_current_cpu_counter();
  ProfileInfo &info = (*sProfilers)[id];
  info.numCalls++;
  info.totalTime += counter - info._last;
}

static inline bool info_greater (const std::pair<ATOM_STRING, ProfileInfo> &i1, const std::pair<ATOM_STRING, ProfileInfo> &i2)
{
  return i1.second.totalTime > i2.second.totalTime;
}

ATOM_KERNEL_API void ATOM_CALL ATOM_ProfileReport (void)
{
  if (sProfilers && !sProfilers->empty ())
  {
    ATOM_LOGGER::log ("==========================Profiler results==========================\n");

    typedef std::pair<ATOM_STRING, ProfileInfo> _SPPair;
    ATOM_VECTOR<_SPPair> infos;
    for (ATOM_MAP<ATOM_STRING, ProfileInfo>::const_iterator it = sProfilers->begin(); it != sProfilers->end(); ++it)
    {
      infos.push_back (*it);
    }
    std::sort (infos.begin(), infos.end(), info_greater);

    float maxCost = infos.begin()->second.totalTime;
    for (ATOM_VECTOR<_SPPair>::const_iterator it = infos.begin(); it != infos.end(); ++it)
    {
      ATOM_LOGGER::log ("ID: %s\n", it->first.c_str());
      ATOM_LOGGER::log ("Number calls: %u\n", it->second.numCalls);
      ATOM_LOGGER::log ("Cycle: %I64u\n", it->second.totalTime);
      ATOM_LOGGER::log ("Ratio: %f\n", it->second.totalTime / maxCost);
      ATOM_LOGGER::log ("Avg: %I64u\n", (unsigned long long)(it->second.totalTime / it->second.numCalls));
      ATOM_LOGGER::log ("\n");
    }
  }
}

