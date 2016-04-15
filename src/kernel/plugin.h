#ifndef __ATOM_KERNEL_PLUGIN_H
#define __ATOM_KERNEL_PLUGIN_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <algorithm>
#include <ATOM_utils.h>
#include "basedefs.h"
#include "dynlib.h"

class ATOM_KernelServer;
class ATOM_Plugin : public ATOM_DynamicLib
{
public:
	struct ClassInfo
	{
		void* creation_func;
		void* destroy_func;
		void* purge_func;
		void* script_interface;
		char* name;
		int cache;
    };

	typedef unsigned    (ATOM_CALL *FcnGetVersion) (void);
	typedef const char*(ATOM_CALL *FcnGetDescription) (void);
	typedef unsigned    (ATOM_CALL *FcnGetCount) (void);
	typedef ClassInfo*(ATOM_CALL *FcnGetClassInfo) (unsigned);
	typedef int (ATOM_CALL *FcnInit) (void);
	typedef void (ATOM_CALL *FcnFini) (void);

public:
    ATOM_Plugin (ATOM_KernelServer* ks);
    virtual ~ATOM_Plugin (void);

    void getVersionNumber(int* major, int* minor, int* micro);
    bool hasClass(const char* name) const;
    unsigned getClassCount() const;
    const ATOM_STRING& getClassNameString(int index) const;
    const ATOM_STRING& getDescription() const;

public:
    virtual bool open (const char* filename);
    virtual void close (void);

    void registerObjectTypes (void);
    void unregisterObjectTypes (void);
	void finalize ();

private:
	ATOM_KernelServer* _M_kernel_server;
	ATOM_HASHSET<ATOM_STRING> _M_plugin_classes;
	unsigned _M_version;
	ATOM_STRING _M_description;
	FcnGetClassInfo _M_fcn_getclassinfo;
	FcnGetCount _M_fcn_getclasscount;
	FcnInit _M_fcn_init;
	FcnFini _M_fcn_fini;
};

#endif // __ATOM_KERNEL_PLUGIN_H
