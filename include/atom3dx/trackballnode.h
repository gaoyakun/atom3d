/**	\file trackballnode.h
 *	ATOMX_TrackBallNode类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3DX_TRACKBALLNODE_H
#define __ATOM3DX_TRACKBALLNODE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class TrackBallRenderDatas;

//! \class TrackBallNode
//! TrackBall节点
//! 该节点于局部原点处显示跟踪球
//! \author 高雅昆
//! \ingroup studio
class ATOMX_API ATOMX_TrackBallNode: public ATOM_VisualNode
{
	ATOM_CLASS(ATOMX, ATOMX_TrackBallNode, ATOMX_TrackBallNode)

public:
	ATOMX_TrackBallNode (void);
	virtual ~ATOMX_TrackBallNode (void);

public:
	virtual void buildBoundingbox (void) const;
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;

public:
	bool setRadius (float radius);
	float getRadius (void) const;
	int getHitAxis (void) const;
	const ATOM_Vector3f &getHitPoint (void) const;
	const ATOM_Vector3f &getHitPointFar (void) const;
	const ATOM_Vector3f &getHitPointNearest (void) const;
	bool worldRayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray) const;
private:
	float getScreenDistance (ATOM_Camera *camera, const ATOM_Vector3f &worldPos, float distance) const;
	float getScreenDistance (ATOM_Camera *camera, const ATOM_Vector3f &worldPos1, const ATOM_Vector3f &worldPos2) const;

protected:
	virtual bool onLoad(ATOM_RenderDevice *device);

private:
	TrackBallRenderDatas *_renderDatas;
	float _radius;
	mutable int _hitAxis;
	mutable ATOM_Vector3f _hitPoint;
	mutable ATOM_Vector3f _hitPointNearest;
	mutable ATOM_Vector3f _hitPointFar;
};

#endif // __ATOM3DX_TRACKBALLNODE_H

/*! @} */
