#ifndef __ATOM3D_ENGINE_SCENE_H
#define __ATOM3D_ENGINE_SCENE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "basedefs.h"
#include "node.h"
#include "posteffect.h"
#include "rendertask.h"
#include "timevalue.h"
#include "nodeoctree.h"
#include "path.h"

class ATOM_Camera;
class ATOM_Scene;
class ATOM_RenderScheme;
class ATOM_PostEffectChain;
class ATOM_Sky;

class ATOM_ENGINE_API ATOM_Scene
{
public:
	typedef void (*LoadCallback) (unsigned current, unsigned total, void *userdata);
	typedef ATOM_AUTOREF(ATOM_Node) (*LoadNodeCallback) (ATOM_RenderDevice *device, const ATOM_TiXmlElement *xml, bool bLoad, void *userData);

	ATOM_Scene (void);
	virtual ~ATOM_Scene (void);

public:
	ATOM_Node *getRootNode (void) const;
	void setSkyNode (ATOM_Sky *skyNode);
	ATOM_Sky *getSkyNode (void) const;
	ATOM_Camera *getCamera (void) const;
	void setRenderScheme (ATOM_RenderScheme *renderScheme);
	ATOM_RenderScheme *getRenderScheme (void) const;
	void setAmbientLight (const ATOM_Vector4f &color);
	const ATOM_Vector4f &getAmbientLight (void) const;
	ATOM_PostEffectChain *getPostEffectChain (void) const;
	void setLoadNodeCallback (LoadNodeCallback callback, void *userData);
	const ATOM_Path3D *getPath (const char *name) const;
	void clearPathes (void);
	void addPath (const char *name, const ATOM_Path3D &path);
	const ATOM_HASHMAP<ATOM_STRING, ATOM_Path3D> &getPathes (void) const;

public:
	virtual void render (ATOM_RenderDevice *device, bool clear);
	virtual bool save (const char *filename, ATOM_Node *parent = 0);
	virtual bool load (const char *filename, ATOM_Node *parent = 0, ATOM_Scene::LoadCallback callback = 0, void *userdata = 0);
	virtual bool resizeView (int w, int h);

protected:
	ATOM_BBox calculateSceneBoundingBox (void) const;

public:
	//static ATOM_Scene *getCurrentScene (void);
	//static void setCurrentScene (ATOM_Scene *scene);

protected:
	mutable ATOM_AUTOREF(ATOM_NodeOctree) _octreeNode;
	mutable ATOM_AUTOREF(ATOM_Node) _rootNode;
	mutable ATOM_AUTOREF(ATOM_Node) _skyNode;
	ATOM_Camera *_camera;
	ATOM_STRING _renderSchemeName;
	ATOM_RenderScheme *_renderScheme;
	ATOM_Vector4f _ambientLight;
	unsigned _width;
	unsigned _height;
	ATOM_HASHMAP<ATOM_STRING, ATOM_Path3D> _pathes;
	ATOM_PostEffectChain *_postEffectChain;
	LoadNodeCallback _loadNodeCallback;
	void *_loadNodeUserData;
};

#endif // __ATOM3D_ENGINE_SCENE_H
