#include "StdAfx.h"
#include "bitarray.h"

ATOM_BitArray::ATOM_BitArray (void)
{
	_size = 0;
}

ATOM_BitArray::ATOM_BitArray (unsigned size)
{
	resize (size);
}

bool ATOM_BitArray::testBit (unsigned index) const
{
	unsigned byteIndex = index / 8;
	unsigned bitMask = 1 << (index - byteIndex * 8);
	return 0 != (_array[byteIndex] & bitMask);
}

void ATOM_BitArray::setBit (unsigned index, bool b)
{
	if (b)
	{
		setBitOn (index);
	}
	else
	{
		setBitOff (index);
	}
}

void ATOM_BitArray::setBitOn (unsigned index)
{
	unsigned byteIndex = index / 8;
	unsigned bitMask = 1 << (index - byteIndex * 8);
	_array[byteIndex] |= bitMask;
}

void ATOM_BitArray::setBitOff (unsigned index)
{
	unsigned byteIndex = index / 8;
	unsigned bitMask = 1 << (index - byteIndex * 8);
	_array[byteIndex] &= ~bitMask;
}

void ATOM_BitArray::resize (unsigned size)
{
	if (size != _size)
	{
		unsigned oldArraySize = _array.size ();
		unsigned newArraySize = (size + 7) / 8;
		_array.resize (newArraySize);
		if (newArraySize > oldArraySize)
		{
			memset (&_array[oldArraySize], 0, newArraySize - oldArraySize);
		}
		_size = size;
	}
}

unsigned ATOM_BitArray::size (void) const
{
	return _size;
}

bool ATOM_BitArray::empty (void) const
{
	return _size == 0;
}

void ATOM_BitArray::clear (void)
{
	_size = 0;
	_array.clear ();
}

void ATOM_BitArray::setAll (bool b)
{
	if (_size)
	{
		const unsigned char p = b ? 0xFF : 0;
		memset (&_array[0], p, _array.size());
	}
}

void ATOM_BitArray::reserve (unsigned size)
{
	if (size > 0)
	{
		_array.reserve ((size + 7) / 8);
	}
}

