#ifndef __ATOM3D_ENGINE_LIGHTNODE_H
#define __ATOM3D_ENGINE_LIGHTNODE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "light.h"
#include "deferredscene.h"
#include "timevalue.h"

class ATOM_Visitor;

class ATOM_ENGINE_API ATOM_LightNode: public ATOM_Node
{
	ATOM_CLASS(engine, ATOM_LightNode, LightNode)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_LightNode)
	ATOM_DECLARE_NODE_FILE_TYPE(ATOM_LightNode, LightNode, "lgt", "ATOM3D light")

public:
	ATOM_LightNode (void);
	virtual ~ATOM_LightNode (void);

public:
	virtual void accept (ATOM_Visitor &visitor);
	virtual bool supportMTLoading (void);
	virtual bool supportFixedFunction (void) const;

protected:
	virtual bool onLoad (ATOM_RenderDevice *device);
	virtual void buildBoundingbox (void) const;
	virtual void onTransformChanged (void);
	virtual void assign (ATOM_Node *other) const;

public:
	void setLightType (int type);
	int getLightType (void) const;
	void setLightColor (const ATOM_Vector4f &color);
	const ATOM_Vector4f &getLightColor (void) const;
	void setLightIntensity (float intensity);
	float getLightIntensity (void) const;
	void setLightAttenuation (const ATOM_Vector3f &val);
	const ATOM_Vector3f &getLightAttenuation (void) const;

	//----------------------------------------------------//
	// wangjian added
	void setHasAttenuation (int has);
	int getHasAttenuation (void) const;
	//----------------------------------------------------//

	ATOM_Light *getLight (void) const;
	void updateLightParams (const ATOM_Matrix4x4f &projMatrix, const ATOM_Matrix4x4f &viewMatrix);

protected:
	bool _transformChanged;
	ATOM_Vector4f _color;
	ATOM_AUTOPTR(ATOM_Light) _light;
};

#endif // __ATOM3D_ENGINE_LIGHTNODE_H
