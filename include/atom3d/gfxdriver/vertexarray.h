/**	\file vertexarray.h
 *	顶点数组类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GFXDRIVER_VERTEXARRAY_H
#define __ATOM_GFXDRIVER_VERTEXARRAY_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "gfxresource.h"

class ATOM_VertexArray: public ATOM_GfxResource
{
public:
	enum LockMode
	{
		ReadOnly,
		WriteOnly,
		ReadWrite
	};

	enum Usage
	{
		Static,
		Dynamic
	};

protected:
	ATOM_VertexArray(void);

public:
	virtual unsigned getAttributes(void) const;
	virtual Usage getUsage (void) const;
	virtual unsigned getVertexStride(void) const;
	virtual unsigned getNumVertices(void) const;

public:
	virtual void* lock(LockMode mode, unsigned offset, unsigned size, bool overwrite) = 0;
	virtual void unlock(void) = 0;
	virtual bool isLocked (void) const = 0;
	virtual bool updateData (const void *data, unsigned bytes) = 0;
	virtual bool isContentLost (void) const;
	virtual void setContentLost (bool b);

protected:
	unsigned _attributes;
	unsigned _vertexSize;
	unsigned _capacity;
	unsigned _numVertices;
	Usage _usage;
	LockMode _lockMode;
	bool _contentLost;
};

#endif // __ATOM_GFXDRIVER_VERTEXARRAY_H
/*! @} */
