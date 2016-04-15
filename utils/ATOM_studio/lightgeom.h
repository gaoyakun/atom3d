#ifndef __ATOM3D_STUDIO_LIGHTGEOM_H
#define __ATOM3D_STUDIO_LIGHTGEOM_H

#if _MSC_VER > 1000
# pragma once
#endif

class LightGeomRenderDatas: public ATOM_Drawable
{
public:
	LightGeomRenderDatas (void);

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

public:
	bool create (ATOM_RenderDevice *device, int type);
	void setColor (const ATOM_Vector4f &color);
	ATOM_BBox getBoundingbox (void) const;
	ATOM_Material *getMaterial (void) const;

private:
	ATOM_Vector4f _color;
	ATOM_AUTOREF(ATOM_VertexArray) _vertexArray;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_VertexDecl _vertexDecl;
};

class LightGeode: public ATOM_VisualNode
{
	ATOM_CLASS(engine, LightGeode, LightGeode)

public:
	LightGeode (void);
	virtual ~LightGeode (void);

public:
	virtual void buildBoundingbox (void) const;
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual void setLightType (int type);
	virtual int getLightType (void) const;
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;

public:
	static void setDisplay (bool b);
	static bool getDisplay (void);

protected:
	virtual bool onLoad(ATOM_RenderDevice *device);
	virtual void buildWorldMatrix (void) const;

private:
	LightGeomRenderDatas *_renderDatas;
	int _lightType;
};

#endif // __ATOM3D_STUDIO_LIGHTGEOM_H
