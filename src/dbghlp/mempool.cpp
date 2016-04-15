#include "StdAfx.h"
#include "mempool.h"
#include "new_wrapper.h"
#include "logger.h"

#define MIN_BLOCK_SIZE 64

#if ATOM3D_PLATFORM_MINGW
# define _aligned_malloc __mingw_aligned_malloc
# define _aligned_free __mingw_aligned_free
#endif

#pragma pack(push, 16)

class ATOM_MallocBlockSet;

#define BLOCK_SIG 0xABCDABCD

struct BlockHeader
{
	unsigned sig;
	int free;
	unsigned size;
	BlockHeader *next;
	BlockHeader *prev;
	BlockHeader *next_free;
	BlockHeader *prev_free;
	ATOM_MallocBlockSet *blockset;
};

#pragma pack(pop)

class ATOM_MallocBlockSet
{
public:
	ATOM_MallocBlockSet (unsigned sz);
	~ATOM_MallocBlockSet (void);

private:
	ATOM_MallocBlockSet (const ATOM_MallocBlockSet&);
	void operator = (const ATOM_MallocBlockSet&);

public:
	void *alloc (unsigned size);
	void dealloc (void *p);
	unsigned getSize (void) const 
	{ 
		return size; 
	}

private:
	void insertFreeBlock (BlockHeader *header);
	void removeFreeBlock (BlockHeader *header);

private:
	BlockHeader *head;
	BlockHeader *head_free;
	unsigned size;
};

ATOM_MallocBlockSet::ATOM_MallocBlockSet (unsigned sz)
{
	sz += 15;
	sz &= ~15;
	head = (BlockHeader*)_aligned_malloc(sz + sizeof(BlockHeader), 16);
	head->sig = BLOCK_SIG;
	head->blockset = this;
	head->free = 1;
	head->next = 0;
	head->prev = 0;
	head->size = sz;

	head_free = head;
	head_free->next_free = 0;
	head_free->prev_free = 0;

	size = sz;
}

ATOM_MallocBlockSet::~ATOM_MallocBlockSet (void)
{
	_aligned_free(head);
}

void *ATOM_MallocBlockSet::alloc (unsigned size)
{
	BlockHeader *pBlock = head_free;
	size += 15;
	size &= ~15;
	
	while (pBlock)
	{
		if (pBlock->size >= size)
		{
			BlockHeader *next = pBlock->next;
			unsigned remain = pBlock->size - size;
			if (remain >= sizeof(BlockHeader)+MIN_BLOCK_SIZE)
			{
				pBlock->size = size;
				pBlock->next = (BlockHeader*)(((char*)pBlock)+sizeof(BlockHeader)+size);
				pBlock->next->sig = BLOCK_SIG;
				pBlock->next->blockset = this;
				pBlock->next->free = 1;
				pBlock->next->size = remain-sizeof(BlockHeader);
				pBlock->next->next = next;
				pBlock->next->prev = pBlock;

				if (next)
				{
					next->prev = pBlock->next;
				}

				insertFreeBlock (pBlock->next);
			}
			pBlock->free = 0;

			removeFreeBlock (pBlock);

			break;
		}
		else
		{
			pBlock = pBlock->next_free;
		}
	}

	return pBlock ? pBlock+1 : 0;
}

void ATOM_MallocBlockSet::dealloc (void *p)
{
	if (!p)
	{
		return;
	}

	BlockHeader *pBlock = (BlockHeader*)(((char*)p) - sizeof(BlockHeader));
	BlockHeader *next = pBlock->next;
	BlockHeader *prev = pBlock->prev;

	pBlock->free = 1;

	if (next && next->free)
	{
		removeFreeBlock (next);

		pBlock->next = next->next;
		pBlock->size += sizeof(BlockHeader);
		pBlock->size += next->size;

		if (pBlock->next)
		{
			pBlock->next->prev = pBlock;
		}
	}

	if (prev && prev->free)
	{
		prev->next = pBlock->next;
		prev->size += sizeof(BlockHeader);
		prev->size += pBlock->size;

		if (prev->next)
		{
			prev->next->prev = prev;
		}
	}
	else
	{
		insertFreeBlock (pBlock);
	}

	pBlock->free = 1;
}

void ATOM_MallocBlockSet::insertFreeBlock (BlockHeader *header)
{
	if (head_free)
	{
		head_free->prev_free = header;
	}

	header->prev_free = 0;
	header->next_free = head_free;
	head_free = header;
}

void ATOM_MallocBlockSet::removeFreeBlock (BlockHeader *header)
{
	if (header->prev_free)
	{
		header->prev_free->next_free = header->next_free;
	}
	if (header->next_free)
	{
		header->next_free->prev_free = header->prev_free;
	}

	if (head_free == header)
	{
		head_free = header->next_free;
	}
}

ATOM_MemoryPool::ATOM_MemoryPool (unsigned trunkSize)
{
	_blockset.push_back (new ATOM_MallocBlockSet(trunkSize));
	_size = trunkSize;
}

ATOM_MemoryPool::~ATOM_MemoryPool (void)
{
	for (unsigned i = 0; i < _blockset.size(); ++i)
	{
		delete(_blockset[i]);
	}
}

void *ATOM_MemoryPool::alloc (unsigned size)
{
	ATOM_Mutex::ScopeMutex autoLock(_lock);

	for (unsigned i = 0; i < _blockset.size(); ++i)
	{
		if (size > _blockset[i]->getSize())
		{
			continue;
		}

		void *p = _blockset[i]->alloc (size);
		if (p)
		{
			return p;
		}
	}

	_blockset.push_back (new ATOM_MallocBlockSet((size > _size) ? size : _size));

	return _blockset.back()->alloc (size);
}

void ATOM_MemoryPool::dealloc (void *p)
{
	ATOM_Mutex::ScopeMutex autoLock(_lock);

	BlockHeader *header = ((BlockHeader*)p) - 1;
	if (header->sig != BLOCK_SIG)
	{
		ATOM_LOGGER::fatal("Invalid block: 0x%08X\n", p);
	}
	else
	{
		header->blockset->dealloc (p);
	}
}

static inline size_t _getNextAlignment (size_t size, size_t alignment)
{
  return (size + alignment - 1) & ~(alignment-1);
}

void *ATOM_MemoryPool::aligned_alloc (unsigned size, unsigned align)
{
	ATOM_Mutex::ScopeMutex autoLock(_lock);

	size_t newsize = align - 1 + sizeof(void*) + size;
	char *p = (char*)alloc(newsize);
	if (p)
	{
		char *aligned = (char*)_getNextAlignment((size_t)(p+sizeof(void*)), align);
		void **origin = (void**)(aligned - sizeof(void*));
		*origin = p;
		return aligned;
	}
	return 0;
}

void ATOM_MemoryPool::aligned_dealloc (void *p)
{
	ATOM_Mutex::ScopeMutex autoLock(_lock);

	if (p)
	{
		char *ptr = (char*)p;
		void **origin_ptr = (void**)(ptr - sizeof(void*));
		dealloc(*origin_ptr);
	}
}

