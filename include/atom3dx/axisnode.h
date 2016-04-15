/**	\file axisnode.h
 *	ATOMX_AxisNode�������.
 *
 *	\author ������
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3DX_AXISNODE_H
#define __ATOM3DX_AXISNODE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class AxisRenderDatas;

//! \class AxisNode
//! ������ڵ�
//! �ýڵ��ھֲ�ԭ�㴦��ʾXYZ���������ᣬ�ú�������ɫ��ʾ
//! \author ������
//! \ingroup studio
class ATOMX_API ATOMX_AxisNode: public ATOM_VisualNode
{
	ATOM_CLASS(ATOMX, ATOMX_AxisNode, ATOMX_AxisNode)

public:
	ATOMX_AxisNode (void);
	virtual ~ATOMX_AxisNode (void);

public:
	virtual void buildBoundingbox (void) const;
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;

public:
	bool setSize (float width, float height);
	float getWidth (void) const;
	float getHeight (void) const;
	int getHitAxis (void) const;
	ATOM_Vector3f getRayProjectPlane (const ATOM_Vector3f &start, const ATOM_Vector3f &direction, int axisType) const;
	ATOM_Vector3f getRayProjectPoint (const ATOM_Vector3f &start, const ATOM_Vector3f &direction, int axisType, const ATOM_Vector3f &plane) const;

protected:
	virtual bool onLoad(ATOM_RenderDevice *device);

private:
	AxisRenderDatas *_renderDatas;
	float _width;
	float _height;
	mutable int _hitAxis;
};

#endif // __ATOM3DX_AXISNODE_H

/*! @} */
