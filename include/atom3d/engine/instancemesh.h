#ifndef __ATOM3D_ENGINE_INSTANCEMESH_H
#define __ATOM3D_ENGINE_INSTANCEMESH_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "model.h"
#include "components.h"
#include "modelanimationkeyframe.h"

class ATOM_Node;
class ATOM_InstanceSkeleton;
class ATOM_InstanceTrack;
struct ATOM_MeshAnimationContext;

class ATOM_ENGINE_API ATOM_InstanceMesh: public ATOM_Drawable, public ATOM_ReferenceObj
{
public:
	ATOM_InstanceMesh (ATOM_Node *node, ATOM_Mesh *mesh);
	virtual ~ATOM_InstanceMesh (void);

public:
	bool setupParameters (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material, bool skinning);
	virtual bool draw(ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material);

public:
	void setComponents (ATOM_Components components);
	ATOM_Components getComponents (void) const;
	ATOM_Mesh *getMesh (void) const;
	void setId (int id);
	int getId (void) const;
	void setIndex (int index);
	int getIndex (void) const;
	void setUVArray (ATOM_VertexArray *array);
	ATOM_VertexArray *getUVArray (void) const;
	void setVertexArray (ATOM_VertexArray *array);
	ATOM_VertexArray *getVertexArray (void) const;
	void setNormalArray (ATOM_VertexArray *array);
	ATOM_VertexArray *getNormalArray (void) const;
	void setBillboardNormal (const ATOM_Vector3f &v);
	const ATOM_Vector3f &getBillboardNormal (void) const;
	void setBillboardXAxis (const ATOM_Vector3f &v);
	const ATOM_Vector3f &getBillboardXAxis (void) const;
	void setBillboardCenter (const ATOM_Vector3f &v);
	const ATOM_Vector3f &getBillboardCenter (void) const;
	void setWorldMatrix (const ATOM_Matrix4x4f &m);
	const ATOM_Matrix4x4f &getWorldMatrix (void) const;
	void setVisible (bool visible);
	bool getVisible (void) const;
	bool mapJoints (ATOM_InstanceTrack *track);
	void invalidateMapJoints (void);
	const ATOM_VECTOR<unsigned> &getJointMap (void) const;
	void setDiffuseColor (const ATOM_Vector4f &color);
	void setAlphaRef (float alpharef);
	void setShininess (float shininess);
	void setGlareMap (ATOM_Texture *texture);
	void setGlareVelocity (const ATOM_Vector4f &v);
	void setGlareRepeat (const ATOM_Vector4f &v);
	void setAlbedoMap (ATOM_Texture *texture);
	void setLastAnimationFrame (float frame);
	float getLastAnimationFrame (void) const;
	void setCurrentTrack (ATOM_InstanceTrack *track);
	ATOM_InstanceTrack *getCurrentTrack (void) const;
	void setCurrentTrackFrame (float frame);
	float getCurrentTrackFrame (void) const;
	bool isContentLost (void) const;
	void setContentLost (bool b);

	const ATOM_Vector4f &getDiffuseColor (void) const;
	float getAlphaRef (void) const;
	float getShininess (void) const;
	ATOM_Texture *getGlareMap (void) const;
	const ATOM_Vector4f &getGlareVelocity (void) const;
	const ATOM_Vector4f &getGlareRepeat (void) const;
	ATOM_Texture *getAlbedoMap (void) const;

	void setUpdateFrameStamp (unsigned val);
	unsigned getUpdateFrameStamp (void) const;

	ATOM_Node *getNode (void) const;
	bool canBatch (void) const;

protected:
	ATOM_AUTOPTR(ATOM_Mesh) _mesh;
	ATOM_Node *_node;
	ATOM_Vector4f _diffuseColor;
	float _alpharef;
	float _shininess;

	bool _visible;
	ATOM_Components _owner;
	int _id;
	int _index;
	ATOM_Vector3f _billboardNormal;
	ATOM_Vector3f _billboardXAxis;
	ATOM_Vector3f _billboardCenter;
	ATOM_Matrix4x4f _worldMatrix;
	ATOM_VECTOR<unsigned> _jointMap;

	ATOM_AUTOREF(ATOM_VertexArray) _vertexArray;
	ATOM_AUTOREF(ATOM_VertexArray) _normalArray;
	ATOM_AUTOREF(ATOM_VertexArray) _uvArray;
	ATOM_AUTOREF(ATOM_VertexArray) _vertexArray2;
	ATOM_AUTOREF(ATOM_VertexArray) _normalArray2;
	ATOM_AUTOREF(ATOM_VertexArray) _uvArray2;

	ATOM_AUTOREF(ATOM_Texture) _albedoTexture;
	ATOM_AUTOREF(ATOM_Texture) _glareTexture;
	ATOM_Vector4f _glareVelocity;
	ATOM_Vector4f _glareRepeat;

	bool _jointMapOk;
	ATOM_InstanceTrack *_currentMapping;
	float _lastAnimationFrame;

	unsigned _updateFrameStamp;
	float _currentTrackFrame;
	ATOM_InstanceTrack *_currentTrack;

	ATOM_MeshAnimationContext *_animationContext;
	ATOM_AUTOPTR(ATOM_ModelAnimationDataCache) _keyframeCache;
};

#endif // __ATOM3D_ENGINE_INSTANCEMESH_H
