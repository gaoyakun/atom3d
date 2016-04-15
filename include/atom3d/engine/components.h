#ifndef __ATOM3D_ENGINE_COMPONENTS_H
#define __ATOM3D_ENGINE_COMPONENTS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class ATOM_Mesh;
class ATOM_SharedModel;
class ATOM_JointAnimationTrack;
class ATOM_Skeleton;

class ATOM_ENGINE_API ATOM_Components
{
public:
	ATOM_Components (void);

	//--- wangjian modified ---//
#if 0
	ATOM_Components (const char *filename, bool mtload = false, unsigned flags = 0, ATOM_LoadingRequestCallback callback = 0, void *userData = 0, long group = 0, ATOM_LoadInterface *owner = 0);
#else
	ATOM_Components (const char *filename, int loadPriority = ATOM_LoadPriority_IMMEDIATE/*ATOM_LoadPriority_ASYNCBASE*/, unsigned flags = 0, ATOM_LoadingRequestCallback callback = 0, void *userData = 0, long group = 0, ATOM_LoadInterface *owner = 0);
#endif
	//----------------------//

	ATOM_Components (ATOM_SharedModel *model);
	ATOM_Components (const ATOM_Components &other);
	~ATOM_Components (void);
	ATOM_Components & operator = (const ATOM_Components &other);
	friend bool operator == (const ATOM_Components &c1, const ATOM_Components &c2);
	friend bool operator != (const ATOM_Components &c1, const ATOM_Components &c2);

public:
	unsigned getNumMeshes (void) const;
	unsigned getNumTracks (void) const;
	ATOM_SharedMesh *getMesh (unsigned index) const;
	ATOM_JointAnimationTrack *getTrack (unsigned index) const;
	const char *getTrackName (int id) const;
	ATOM_Skeleton *getSkeleton (void) const;
	void setModel (ATOM_SharedModel *model);
	ATOM_SharedModel *getModel (void) const;

	//--- wangjian modified ---//
	// 异步加载： 设置加载优先级

#if 0 

	bool loadComponents (const char *filename, bool mtload = false, unsigned flags = 0, ATOM_LoadingRequestCallback callback = 0, void *userData = 0, long group = 0, ATOM_LoadInterface *owner = 0);

#else
	
	bool loadComponents (const char *filename, int loadPriority = ATOM_LoadPriority_ASYNCBASE, unsigned flags = 0, ATOM_LoadingRequestCallback callback = 0, void *userData = 0, long group = 0, ATOM_LoadInterface *owner = 0);
	
#endif
	//----------------------//

	//bool loadComponentsFromMemory (const void *mem, unsigned size);
	bool isValid (void) const;
	void reset (void);

public:
	static ATOM_AUTOREF(ATOM_SharedModel) findModel (const char *filename);
	static ATOM_AUTOREF(ATOM_SharedModel) newModel (const char *filename, bool &createNew);

private:
	struct ComponentsData *_internalData;
};

#endif // __ATOM3D_ENGINE_COMPONENTS_H
