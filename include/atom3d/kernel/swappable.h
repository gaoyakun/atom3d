#ifndef __ATOM3D_KERNEL_SWAPPABLE_H
#define __ATOM3D_KERNEL_SWAPPABLE_H

#if _MSC_VER > 1000
# pragma once
#endif

#if 0

#include <windows.h>
#include "ATOM_utils.h"
#include "refobj.h"

class ATOM_KERNEL_API ATOM_Swappable: private ATOM_Noncopyable
{
	friend class SwappableManager;

public:
	ATOM_Swappable (void);
	virtual ~ATOM_Swappable (void);

public:
	virtual unsigned getSize (void) const = 0;
	virtual bool isLocked (void) const;
	virtual void *lock (void);
	virtual bool unlock (void);
	virtual bool isDataValid (void) const = 0;

protected:
	virtual bool write (void *buffer) = 0;
	virtual bool read (const void *buffer) = 0;
	virtual void invalidate (void) = 0;
	virtual bool swapToDisk (void);
	virtual void loadFromDisk (void);

public:
	void setSlot (int slot);
	int getSlot (void) const;
	void setOrder (int order);
	int getOrder (void) const;
	void setSlice (void *slice);
	void *getSlice (void) const;
	unsigned getLastLockTime (void) const;
	void *getDataPtr (void) const;

protected:
	int _lockCount;
	void *_ptr;
	void *_mapPtr;
	void *_slice;
	int _slot;
	int _order;
	unsigned _lastLockTime;
};

class ATOM_KERNEL_API ATOM_SwappableRawMemory: public ATOM_Swappable
{
public:
	ATOM_SwappableRawMemory (unsigned size);
	virtual ~ATOM_SwappableRawMemory (void);

public:
	virtual unsigned getSize (void) const;
	virtual bool isDataValid (void) const;
	virtual ATOM_SwappableRawMemory *clone (void);

protected:
	virtual bool write (void *buffer);
	virtual bool read (const void *buffer);
	virtual void invalidate (void);

private:
	unsigned _size;
};

class MemMapSection;

class SwappableManager
{
public:
	SwappableManager (void);
	~SwappableManager (void);
	void registerSwappable (ATOM_Swappable *swappable, bool allocFileMapping = true);
	void unregisterSwappable (ATOM_Swappable *swappable);
	void process (unsigned tick, unsigned tick_max, unsigned mem_size, unsigned mem_size_max);

private:
	ATOM_VECTOR<MemMapSection*> _sections;
	ATOM_VECTOR<ATOM_Swappable*> _swappableVector;
	ATOM_VECTOR<ATOM_Swappable*> _swapList;
	ATOM_VECTOR<unsigned> _freeSlots;
};

class MemMapSection
{
public:
	struct Slice
	{
		unsigned offset;
		unsigned size;
		unsigned stride;
		void *mapPtr;
		MemMapSection *section;
		bool free;
		Slice *prev;
		Slice *next;
	};

public:
	MemMapSection (unsigned size);
	~MemMapSection (void);

public:
	static const unsigned min_tolerence = 1024;
	Slice *allocSlice (unsigned size);
	void freeSlice (Slice *slice);
	void *mapSlice (Slice *slice, unsigned size);
	void unmapSlice (Slice *slice);

private:
	unsigned _totalSize;
	Slice *_sliceList;
	HANDLE _mapHandle;
};

class ATOM_KERNEL_API ATOM_SwappableManager
{
public:
	static SwappableManager _manager;
	static unsigned _memoryLimitInBytes;
	static unsigned _expiredTime;
	static unsigned _lastProcessTime;
	static unsigned _processInterval;
	static void registerSwappable (ATOM_Swappable *swappable, bool allocFileMapping);
	static void unregisterSwappable (ATOM_Swappable *swappable);
	static void setMemoryLimitValue (unsigned numBytes);
	static void setExpiredTime (unsigned tick);
	static void setProcessInterval (unsigned tick);
	static void checkAndSwap (unsigned currentTick);
	static void *mapSwappable (ATOM_Swappable *swappable);
	static void unmapSwappable (ATOM_Swappable *swappable);
};

#endif

#endif // __ATOM3D_KERNEL_SWAPPABLE_H
