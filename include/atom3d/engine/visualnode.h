/**	\file engine/visualnode.h
 *	可视节点基类.
 *
 *	\author 高雅昆
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_VISUALNODE_H
#define __ATOM3D_ENGINE_VISUALNODE_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "basedefs.h"
#include "node.h"

//! \class ATOM_VisualNode
//! 可视节点基类.
//! \author 高雅昆
//! \ingroup engine
class ATOM_ENGINE_API ATOM_VisualNode: public ATOM_Node
{
public:
	//! \copydoc ATOM_Node::accept
	virtual void accept (ATOM_Visitor &visitor);

public:
	//! 装填渲染队列
	//! \param visitor 裁剪访问器指针
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor) = 0;

	//! 射线求交
	//! \param camera 当前摄像机
	//! \param ray 射线结构
	//! \param len 如果相交，返回交点到射线原点的距离
	//! \return true 相交 false 不相交
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;

	//! 更新
	//! \param camera 摄像机
	virtual void update (ATOM_Camera *camera);
};

#endif // __ATOM3D_ENGINE_VISUALNODE_H
/*! @} */
