#ifndef __ATOM_GLRENDER_VERTEXARRAY_D3D_H
#define __ATOM_GLRENDER_VERTEXARRAY_D3D_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include <d3d9.h>
#include "gfxtypes.h"
#include "vertexarray.h"

class ATOM_D3DVertexArray: public ATOM_VertexArray
{
	ATOM_CLASS(render, ATOM_D3DVertexArray, ATOM_D3DVertexArray)

public:
	ATOM_D3DVertexArray(void);
	virtual ~ATOM_D3DVertexArray (void);

public:
	virtual void* lock(ATOM_LockMode mode, unsigned offset, unsigned size, bool overwrite);
	virtual void unlock(void);
	virtual bool isLocked (void) const;
	virtual bool updateData (const void *data, unsigned bytes);
	virtual bool realize (void);

protected:
	virtual void invalidateImpl (bool needRestore);
	virtual void restoreImpl (void);

public:
	IDirect3DVertexBuffer9 *getD3DVertexBuffer (void) const;

private:
	bool _internalCreateVertexBuffer (unsigned bytes);

public:
	static bool getVertexElement (unsigned attrib, D3DVERTEXELEMENT9 *element, unsigned attribFlags = 0);

private:
	IDirect3DVertexBuffer9 *_M_vertexbuffer;
	DWORD _M_locked;
	DWORD _M_size_bytes;
	DWORD _M_lock_offset;
	DWORD _M_lock_size;
	DWORD _M_lock_flags;
};

// inline
inline IDirect3DVertexBuffer9 *ATOM_D3DVertexArray::getD3DVertexBuffer(void) const
{
  return _M_vertexbuffer;
}

#endif // __ATOM_GLRENDER_VERTEXARRAY_D3D_H
