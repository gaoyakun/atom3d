#include "stdafx.h"

static ATOM_STRING ByteToStr(const __int64& n64ByteCount = 0, const int& nPrecision = 2)
{
  const int NUMFORMATTERS = 5;
  char szFormatters[NUMFORMATTERS][10] = {" bytes", " KB", " MB", " GB", " TB" };
  double dblBase = n64ByteCount;
  int nNumConversions = 0;

  while (dblBase > 1000)
    {
	    dblBase /= 1024;
      nNumConversions++;
    }

  ATOM_STRING strUnits;
  if ((0 <= nNumConversions) && (nNumConversions < NUMFORMATTERS))
  {
      strUnits = szFormatters[nNumConversions];
  }
   
  std::ostringstream os;
  os.setf(std::ostringstream::fixed);
  os.precision(nPrecision);
  os << dblBase << strUnits << std::ends;
  return (os.str().c_str());   
}    

ATOM_MemoryInfo::ATOM_MemoryInfo (void)
{
  MEMORYSTATUS memstat;
  ::GlobalMemoryStatus (&memstat);
  totalRam = memstat.dwTotalPhys;
  totalPageFile = memstat.dwTotalPageFile;
  totalVirtual = memstat.dwTotalVirtual;
  strcpy (totalRamString, ByteToStr (memstat.dwTotalPhys + 655360, 0).c_str());
  strcpy (totalPageFileString, ByteToStr (memstat.dwTotalPageFile + 655360).c_str());
  strcpy (totalVirtualString, ByteToStr (memstat.dwTotalVirtual + 655360).c_str());
}

unsigned ATOM_MemoryInfo::getTotalRam (void) const
{
  return totalRam;
}

unsigned ATOM_MemoryInfo::getTotalPageFile (void) const
{
  return totalPageFile;
}

unsigned ATOM_MemoryInfo::getTotalVirtual (void) const
{
  return totalVirtual;
}

const char *ATOM_MemoryInfo::getTotalRamStr (void) const
{
  return totalRamString;
}

const char *ATOM_MemoryInfo::getTotalPageFileStr (void) const
{
  return totalPageFileString;
}

const char *ATOM_MemoryInfo::getTotalVirtualStr (void) const
{
  return totalVirtualString;
}

