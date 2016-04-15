#ifndef __ATOM3D_KERNELCONFIG_H
#define __ATOM3D_KERNELCONFIG_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_vfs.h"
#include "basedefs.h"

class ATOM_KERNEL_API ATOM_KernelConfig
{
public:
	static bool initialize (const char *password);
	static ATOM_PhysicVFS *initializeWithPath (const char *vfsroot);
	static ATOM_ZipVFS *initializeWithZIP (const char *zipFileName, const char *password);
	static ATOM_HttpVFS *initializeWithHTTP (const char *url, const char *url2, const char *cache, bool compressed, const char*param, int (*funcCheckFile)(const char*, int));
	static bool initializeEx (const char *cfgFileName, const char *password);
	static bool isRecordFileName (void);
	static void setRecordFileName(bool record);
};

#endif //__ATOM3D_KERNELCONFIG_H
