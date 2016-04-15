#ifndef __ATOM3D_KERNEL_BITARRAY_H
#define __ATOM3D_KERNEL_BITARRAY_H

#if _MSC_VER > 100
#pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "basedefs.h"

class ATOM_KERNEL_API ATOM_BitArray
{
public:
	ATOM_BitArray (void);
	ATOM_BitArray (unsigned size);

public:
	bool testBit (unsigned index) const;
	void setBit (unsigned index, bool b);
	void setBitOn (unsigned index);
	void setBitOff (unsigned index);
	void resize (unsigned size);
	unsigned size (void) const;
	bool empty (void) const;
	void clear (void);
	void setAll (bool b);
	void reserve (unsigned size);

private:
	unsigned _size;
	ATOM_VECTOR<unsigned char> _array;
};

#endif // __ATOM3D_KERNEL_BITARRAY_H
