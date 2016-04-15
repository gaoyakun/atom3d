#ifndef __ATOM_DEVICEDB_DISPLAYINFO_H_
#define __ATOM_DEVICEDB_DISPLAYINFO_H_

#include "basedefs.h"
#include <vector>
#include <string>

class ATOM_DDB_DisplayInfo
{
public:        // object creation/destruction
  struct DxVersionInfo
  {
    int nDXVersionMajor;
    int nDXVersionMinor;
    char sDXVersionLetter[128];
  };

  ATOM_DDB_DisplayInfo (void);
  ~ATOM_DDB_DisplayInfo (void);

  unsigned GetNumDevices (void) const;
  const char *GetDeviceDescription (unsigned device) const;
  const char *GetDeviceManufacturer (unsigned device) const;
  const char *GetDeviceChipType (unsigned device) const;
  const char *GetDeviceMemory (unsigned device) const;
  const char *GetDeviceDisplayMode (unsigned device) const;
  const char *GetDeviceDriverName (unsigned device) const;
  const char *GetDeviceDriverVersion (unsigned device) const;
  const char *GetDeviceDriverDate (unsigned device) const;
  unsigned GetDeviceVendorId (unsigned device) const;
  unsigned GetDeviceId (unsigned device) const;
  unsigned GetDeviceSubSystemId (unsigned device) const;
  unsigned GetDeviceRevisionId (unsigned device) const;
  bool IsDDrawAccelerationEnabled (unsigned device) const;
  bool Is3DAccelerationExists (unsigned device) const;
  bool Is3DAccelerationEnabled (unsigned device) const;
  bool IsAGPExists (unsigned device) const;
  bool IsAGPEnabled (unsigned device) const;
  const DxVersionInfo *GetDxVersionInfo (void) const;
  const char *GetDXVersionString (void) const;

private:       // attributes
	std::vector<struct DisplayDeviceInfo*> devices;
	DxVersionInfo dxverion;
	mutable std::string dxversionStr;
};

#endif // __ATOM_DEVICEDB_DISPLAYINFO_H_
