#ifndef __ATOM3D_ENGINE_LINETRAIL_H
#define __ATOM3D_ENGINE_LINETRAIL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"
#include "visualnode.h"

class ATOM_ENGINE_API ATOM_LineTrail;

class ATOM_ENGINE_API ATOM_LineTrailRenderDatas: public ATOM_Drawable
{
	friend class ATOM_LineTrail;

public:
	ATOM_LineTrailRenderDatas (ATOM_LineTrail *lineTrail);
	virtual ~ATOM_LineTrailRenderDatas (void);

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

private:
	void invalidateVertices (void);
	void updateVertices (ATOM_RenderDevice *device, ATOM_Camera *camera);
	ATOM_Material *getMaterial (void) const;

protected:
	ATOM_LineTrail *_lineTrail;
	ATOM_AUTOREF(ATOM_VertexArray) _vertexArray;

private:
	ATOM_AUTOPTR(ATOM_Material) _material;
	static ATOM_VertexDecl _vertexDecl;
};

class ATOM_ENGINE_API ATOM_LineTrail: public ATOM_VisualNode
{
public:
	static const unsigned LTF_WORLDSPACE;
	static const int BlendMode_Default;
	static const int BlendMode_AlphaAdditive;
	static const int BlendMode_Additive;

	struct InflectionPoint
	{
		ATOM_Vector3f position;
		ATOM_ColorARGB color;
		float width;
		mutable float length;
		mutable bool lengthDirty;
	};

public:
	ATOM_LineTrail (void);
	virtual ~ATOM_LineTrail (void);

public:
	virtual void accept (ATOM_Visitor &visitor);
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;
	virtual bool supportFixedFunction (void) const;

protected:
	virtual bool onLoad(ATOM_RenderDevice *device);
	virtual void assign(ATOM_Node *other) const;
	virtual void buildBoundingbox (void) const;
	virtual void buildWorldBoundingbox (void) const;

public:
	void appendInflectionPoint (const InflectionPoint &p);
	void insertInflectionPoint (unsigned pos, const InflectionPoint &p);
	void removeInflectionPoint (unsigned pos);
	void clearInflectionPoints (void);
	void setNumInflectionPoints (unsigned count);
	unsigned getNumInflectionPoints (void) const;
	const InflectionPoint &inflectionPoint (unsigned pos) const;
	InflectionPoint &inflectionPoint (unsigned pos);
	void setFlags (unsigned flags);
	unsigned getFlags (void) const;
	void setTexture (ATOM_Texture *texture);
	ATOM_Texture *getTexture (void) const;
	void setTextureFileName (const ATOM_STRING &fileName);
	const ATOM_STRING &getTextureFileName (void) const;
	float getSegmentLength (unsigned pos) const;
	float getTrailLength (void) const;
	void setBlendMode (int blendMode);
	int getBlendMode (void) const;

protected:
	void invalidLength (unsigned pos);

private:
	ATOM_AUTOREF(ATOM_Texture) _texture;
	ATOM_STRING _textureFileName;
	ATOM_DEQUE<InflectionPoint> _inflectionPoints;
	ATOM_LineTrailRenderDatas *_renderDatas;
	unsigned _flags;
	int _blendMode;

	ATOM_CLASS(engine, ATOM_LineTrail, ATOM_LineTrail)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_LineTrail)
};

#endif // __ATOM3D_ENGINE_LINETRAIL_H
