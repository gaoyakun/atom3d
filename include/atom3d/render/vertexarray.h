/**	\file animatedtexturechannel.h
 *	顶点数组类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GLRENDER_IVERTEXARRAY_H
#define __ATOM_GLRENDER_IVERTEXARRAY_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "gfxtypes.h"
#include "gfxresource.h"

class ATOM_VertexArray: public ATOM_GfxResource
{
	friend class IVBManagerVBO;
	friend class IVBManagerDEFAULT;
	friend class ATOM_ID3DVBManager;

protected:
	ATOM_VertexArray(void);

public:
	virtual unsigned getAttributes() const;
	virtual int getUsage () const;
	virtual unsigned getVertexStride() const;
	virtual unsigned getNumVertices() const;

public:
	virtual void* lock(ATOM_LockMode mode, unsigned offset, unsigned size, bool overwrite) = 0;
	virtual void unlock(void) = 0;
	virtual bool isLocked (void) const = 0;
	virtual bool updateData (const void *data, unsigned bytes) = 0;
	virtual bool isContentLost (void) const;
	virtual void setContentLost (bool b);

	//--- wangjian added for test float16 ---//
	unsigned		getAttribFlags() const;
	//---------------------------------------//

protected:
	unsigned _M_attributes;
	unsigned _M_vertex_size;
	unsigned _M_capacity;
	unsigned _M_num_verts;
	int _M_usage;
	int _M_lockmode;
	bool _M_content_lost;
	void *_preservedContent;
	//--- wangjian added for test float16 ---//
	unsigned _attribFlags;
	//---------------------------------------//
};

#endif // __LIEF3D_GLRENDER_IVERTEXARRAY_H
/*! @} */
