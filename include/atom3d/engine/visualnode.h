/**	\file engine/visualnode.h
 *	���ӽڵ����.
 *
 *	\author ������
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
//! ���ӽڵ����.
//! \author ������
//! \ingroup engine
class ATOM_ENGINE_API ATOM_VisualNode: public ATOM_Node
{
public:
	//! \copydoc ATOM_Node::accept
	virtual void accept (ATOM_Visitor &visitor);

public:
	//! װ����Ⱦ����
	//! \param visitor �ü�������ָ��
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor) = 0;

	//! ������
	//! \param camera ��ǰ�����
	//! \param ray ���߽ṹ
	//! \param len ����ཻ�����ؽ��㵽����ԭ��ľ���
	//! \return true �ཻ false ���ཻ
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;

	//! ����
	//! \param camera �����
	virtual void update (ATOM_Camera *camera);
};

#endif // __ATOM3D_ENGINE_VISUALNODE_H
/*! @} */
