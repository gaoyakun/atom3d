#include "stdafx.h"

typedef BOOL (WINAPI* PFNGETDISKFREESPACEEX)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

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

ATOM_DriveInfo::ATOM_DriveInfo (void)
{
  HINSTANCE hModule = NULL;                     

  if ((hModule = LoadLibrary("KERNEL32.DLL")) != NULL)
  {
    DWORD dwDriveList = ::GetLogicalDrives();
    DWORD dwMinMask = 0x00000001;
    m_vdriveStats.clear();

    // check to see if they want to test for floppy drives
    for (int i = 0; i < 26; i++)
    {
      ATOM_DriveStats driveStats;

      if (dwDriveList & (dwMinMask << i))
      {
          // set the drive name
        std::ostringstream os;
        os << (char)('A' + i) << ":\\" << std::ends;
        driveStats.m_strName = os.str().c_str();

        // make sure the drive is a valid type
        // we don't want floppy or cdrom, because they will
        // require the user to insert a disk if there is no disk....
        unsigned int unType = ::GetDriveType(driveStats.m_strName.c_str());
        if (unType != DRIVE_NO_ROOT_DIR)
        {
          if ((unType == DRIVE_FIXED) || (unType == DRIVE_REMOTE))
          {
            PFNGETDISKFREESPACEEX pDiskFreeSpaceEx = NULL;

            // set the string version of the type based on unType
            switch (unType)
            {
            case DRIVE_UNKNOWN:
              driveStats.m_strType = "Unknown drive";
              break;
            case DRIVE_REMOVABLE:
              driveStats.m_strType = "Removable drive";
              break;
            case DRIVE_FIXED:
              driveStats.m_strType = "Fixed drive";
              break;
            case DRIVE_REMOTE:
              driveStats.m_strType = "Network drive";
              break;
            case DRIVE_CDROM:
              driveStats.m_strType = "CD-ROM drive";
              break;
            case DRIVE_RAMDISK:
              driveStats.m_strType = "RAM disk";
              break;
            }

            // use GetDiskFreeSpaceEx, if available. otherwise, use the crappy version [GetDiskFreeSpace]
            if ((pDiskFreeSpaceEx = (PFNGETDISKFREESPACEEX)GetProcAddress(hModule, "GetDiskFreeSpaceExA")) == NULL)
            {	
              DWORD dwSectorsPerCluster = 0; 
              DWORD dwBytesPerSector = 0;    
              DWORD dwFreeClusters = 0;      
              DWORD dwClusters = 0;          		
              BOOL fResult = GetDiskFreeSpace(driveStats.m_strName.c_str(), &dwSectorsPerCluster, &dwBytesPerSector, 
                                              &dwFreeClusters, &dwClusters);

              DWORD dwTotal = fResult ? (dwClusters * dwBytesPerSector * dwSectorsPerCluster) : 0;
              DWORD dwFree = fResult ? (dwFreeClusters * dwBytesPerSector * dwSectorsPerCluster) : 0;

              driveStats.m_strTotalSpace = ByteToStr(dwTotal);
              driveStats.m_strFreeSpace = ByteToStr(dwFree);
            }
            else
            {
              ULARGE_INTEGER uliTotalNumBytes;         
              ULARGE_INTEGER uliTotalFreeBytes;        
              ULARGE_INTEGER uliTotalAvailableToCaller;

              BOOL fResult = pDiskFreeSpaceEx(driveStats.m_strName.c_str(), &uliTotalAvailableToCaller, &uliTotalNumBytes, &uliTotalFreeBytes);
              driveStats.m_strTotalSpace = (fResult ? ByteToStr(uliTotalNumBytes.QuadPart) : "0");
              driveStats.m_strFreeSpace = (fResult ? ByteToStr(uliTotalFreeBytes.QuadPart) : "0");
            }

            m_vdriveStats.push_back(driveStats);
          }
        }
      }
    }

	    FreeLibrary(hModule);
    }
}

unsigned ATOM_DriveInfo::getNumDrives (void) const
{
  return m_vdriveStats.size();
}

const ATOM_DriveStats &ATOM_DriveInfo::getDriveState (unsigned i) const
{
  return m_vdriveStats[i];
}

