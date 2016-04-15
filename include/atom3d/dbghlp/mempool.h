#ifndef __ATOM3D_DBGHLP_MEMPOOL_H
#define __ATOM3D_DBGHLP_MEMPOOL_H

#include <vector>
#include "../ATOM_thread.h"
#include "stl.h"

class ATOM_MallocBlockSet;

class ATOM_MemoryPool
{
public:
	ATOM_MemoryPool (unsigned trunkSize);
	~ATOM_MemoryPool (void);

public:
	void *alloc (unsigned size);
	void dealloc (void *p);
	void *aligned_alloc (unsigned size, unsigned align);
	void aligned_dealloc (void *p);

private:
	unsigned _size;
	ATOM_Mutex _lock;
	std::vector<ATOM_MallocBlockSet*> _blockset;
};

#endif // __ATOM3D_DBGHLP_MEMPOOL_H
