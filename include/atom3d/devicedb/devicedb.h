#ifndef __ATOM3D_DEVICEDB_H
#define __ATOM3D_DEVICEDB_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class ATOM_DeviceDB
{
public:
	struct Device
	{
		unsigned short vendorId;
		unsigned short deviceId;
		const char *deviceName;
		// 0: fixed function
		// 1: forward
		// 2: deferred
		int desiredLevel;
	};

public:
	static bool findDevice (unsigned short vendorId, unsigned short deviceId, Device *device);
	static unsigned getNumDevices (void);
	static unsigned short getVendorId (unsigned device);
	static unsigned short getDeviceId (unsigned device);
};

#endif // __ATOM3D_DEVICEDB_H
