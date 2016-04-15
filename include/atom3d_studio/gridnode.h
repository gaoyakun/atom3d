/**	\file gridnode.h
 *	GridNode�������.
 *
 *	\author ������
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3D_STUDIO_GRIDNODE_H
#define __ATOM3D_STUDIO_GRIDNODE_H

#if _MSC_VER > 1000
# pragma once
#endif

class GridRenderDatas;

//! \class GridNode
//! �༭����������ڵ�
//! \author ������
//! \ingroup studio
class GridNode: public ATOM_VisualNode
{
	ATOM_CLASS(engine, GridNode, GridNode)

public:
	GridNode (void);
	virtual ~GridNode (void);

public:
	virtual void buildBoundingbox (void) const;
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);

public:
	bool setSize (unsigned width, unsigned height);
	unsigned getWidth (void) const;
	unsigned getHeight (void) const;

protected:
	virtual bool onLoad(ATOM_RenderDevice *device);

private:
	GridRenderDatas *_renderDatas;
	unsigned _width;
	unsigned _height;
};

#endif // __ATOM3D_STUDIO_GRIDNODE_H

/*! @} */
