#ifndef __ATOM3D_CONFIG_H__
#define __ATOM3D_CONFIG_H__

#define ATOM3D_MAKE_VERSION(major, minor, patch) (((unsigned)(major)<<16)+((unsigned)(minor)<<8)+patch)
#define ATOM3D_MAJOR_VERSION 0
#define ATOM3D_MINOR_VERSION 7
#define ATOM3D_PATCH_VERSION 0
#define ATOM3D_VERSION	ATOM3D_MAKE_VERSION(ATOM3D_MAJOR_VERSION, ATOM3D_MINOR_VERSION, ATOM3D_PATCH_VERSION)
#define ATOM3D_VERSION_GET_MAJOR(ver)	((unsigned)(ver)>>16)
#define ATOM3D_VERSION_GET_MINOR(ver)	(((unsigned)(ver)>>8)&0xFF)
#define ATOM3D_VERSION_GET_PATCH(ver)	((unsigned)(ver)&0xFF)

#define ATOM3D_SHARED_LIBS 0
#define ATOM3D_MINIMAL_BUILD 0

#endif // __ATOM3D_CONFIG_H__

