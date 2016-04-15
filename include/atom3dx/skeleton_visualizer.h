#ifndef __ATOM3DX_SKELETON_VISUALIZER_H
#define __ATOM3DX_SKELETON_VISUALIZER_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "../atom3d/ATOM_engine.h"
#include "basedefs.h"
#include "axisnode.h"

class ATOMX_API ATOMX_SkeletonVisualizer: private ATOM_Noncopyable
{
public:
	ATOMX_SkeletonVisualizer (void);
	virtual ~ATOMX_SkeletonVisualizer (void);

public:
	void setSource (ATOM_Geode *source);
	void updateVertices (void);
	void draw (ATOM_Material *material);
	ATOM_Material *getMaterial (void) const;

private:
	bool prepareForRender (void);

private:
	ATOM_AUTOREF(ATOM_VertexArray) _vertices;
	ATOM_AUTOREF(ATOM_IndexArray) _indices;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_WeakPtrT<ATOM_Geode> _source;
	ATOM_VertexDecl _vertexDecl;
	float _jointRadius;
};

class SkeletonVisualizerRenderDatas;
class ATOMX_API ATOMX_SkeletonVisualizerNode: public ATOM_VisualNode
{
	ATOM_CLASS(ATOMX, ATOMX_SkeletonVisualizerNode, ATOMX_SkeletonVisualizerNode)

public:
	ATOMX_SkeletonVisualizerNode (void);
	virtual ~ATOMX_SkeletonVisualizerNode (void);

public:
	virtual void buildBoundingbox (void) const;
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual void skipClipTest (bool b);
	virtual bool isSkipClipTest (void) const;
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;
	virtual void update (ATOM_Camera *camera);

public:
	void setSource (ATOM_Geode *geode);

private:
	ATOM_WeakPtrT<ATOM_Geode> _source;
	SkeletonVisualizerRenderDatas *_renderDatas;
};

#endif // __ATOM3DX_SKELETON_VISUALIZER_H
