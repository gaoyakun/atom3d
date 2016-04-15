#ifndef __ATOM_GLRENDER_INDEXARRAY_D3D_H
#define __ATOM_GLRENDER_INDEXARRAY_D3D_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <d3d9.h>
#include "gfxtypes.h"
#include "indexarray.h"

class ATOM_D3D9IndexArray: public ATOM_IndexArray
{
	ATOM_CLASS(render, ATOM_D3D9IndexArray, ATOM_D3D9IndexArray)

public:
	ATOM_D3D9IndexArray(void);
	virtual ~ATOM_D3D9IndexArray(void);

public:
	virtual bool realize (int usage, unsigned length, bool use32bit);
    virtual void * lock(ATOM_LockMode mode, unsigned offset, unsigned size, bool overwrite);
    virtual void unlock(void);

public:
	IDirect3DIndexBuffer9 *getD3DIndexBuffer (void) const;

protected:
	virtual void invalidateImpl (bool needRestore);
	virtual void restoreImpl (void);

private:
	bool _setIndexData (const void *data, unsigned bytes);
	bool _internalCreateIndexBuffer (unsigned bytes);

private:
	IDirect3DIndexBuffer9 *_M_indexbuffer;
	DWORD _M_size_bytes;
};

  // inline
inline IDirect3DIndexBuffer9 *ATOM_D3D9IndexArray::getD3DIndexBuffer (void) const
{
  return _M_indexbuffer;
}

#endif // __ATOM_GLRENDER_INDEXARRAY_D3D_H
