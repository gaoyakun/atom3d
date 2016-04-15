#include "StdAfx.h"

#if 0

#include <psapi.h>
#include "swappable.h"

static ATOM_Mutex *mutex = ATOM_NEW(ATOM_Mutex, 2000);

struct AutoLock
{
	AutoLock (void)
	{
		mutex->lock ();
	}

	~AutoLock (void)
	{
		mutex->unlock ();
	}
};

ATOM_Swappable::ATOM_Swappable (void)
{
	ATOM_STACK_TRACE(ATOM_Swappable::ATOM_Swappable);
	AutoLock autoLock;

	_lockCount = 0;
	_ptr = 0;
	_mapPtr = 0;
	_slice = 0;
	_slot = -1;
	_lastLockTime = 0;
}

ATOM_Swappable::~ATOM_Swappable (void)
{
	ATOM_STACK_TRACE(ATOM_Swappable::~ATOM_Swappable);
}

void ATOM_Swappable::loadFromDisk (void)
{
	ATOM_STACK_TRACE(ATOM_Swappable::loadFromDisk);
	AutoLock autoLock;

	if (!_mapPtr)
	{
		_mapPtr = ATOM_SwappableManager::mapSwappable (this);
	}

	if (_mapPtr && read (_mapPtr))
	{
		ATOM_SwappableManager::unmapSwappable (this);
		_mapPtr = 0;
	}

	//ATOM_LOGGER::debug("(0x%08X)%d bytes loaded from disk\n", this, getSize());
}

void *ATOM_Swappable::lock (void)
{
	ATOM_STACK_TRACE(ATOM_Swappable::lock);
	AutoLock autoLock;

	++_lockCount;

	if (_lockCount == 1 && !isDataValid ())
	{
		loadFromDisk ();
	}

	return _ptr;
}

void *ATOM_Swappable::getDataPtr (void) const
{
	ATOM_STACK_TRACE(ATOM_Swappable::getDataPtr);
	AutoLock autoLock;

	return _ptr;
}

bool ATOM_Swappable::swapToDisk (void)
{
	ATOM_STACK_TRACE(ATOM_Swappable::swapToDisk);
	AutoLock autoLock;

	if (!_mapPtr)
	{
		_mapPtr = ATOM_SwappableManager::mapSwappable (this);
		if (!_mapPtr)
		{
			return false;
		}
	}

	write (_mapPtr);
	ATOM_SwappableManager::unmapSwappable (this);
	_mapPtr = 0;
	_lastLockTime = 0;

	invalidate ();

	//ATOM_LOGGER::debug("(0x%08X)Swapped %d bytes to disk\n", this, getSize());

	return true;
}

bool ATOM_Swappable::unlock (void)
{
	ATOM_STACK_TRACE(ATOM_Swappable::unlock);
	AutoLock autoLock;

	if (!isLocked ())
	{
		return false;
	}

	--_lockCount;

	if (_lockCount == 0)
	{
		_lastLockTime = ATOM_APP->getFrameStamp().currentTick;
	}

	return true;
}

bool ATOM_Swappable::isLocked (void) const
{
	ATOM_STACK_TRACE(ATOM_Swappable::isLocked);
	AutoLock autoLock;

	return _lockCount > 0;
}

void ATOM_Swappable::setSlot (int slot)
{
	ATOM_STACK_TRACE(ATOM_Swappable::setSlot);
	AutoLock autoLock;

	_slot = slot;
}

int ATOM_Swappable::getSlot (void) const
{
	ATOM_STACK_TRACE(ATOM_Swappable::getSlot);
	AutoLock autoLock;

	return _slot;
}

void ATOM_Swappable::setSlice (void *slice)
{
	ATOM_STACK_TRACE(ATOM_Swappable::setSlice);
	AutoLock autoLock;

	_slice = slice;
}

void *ATOM_Swappable::getSlice (void) const
{
	ATOM_STACK_TRACE(ATOM_Swappable::getSlice);
	AutoLock autoLock;

	return _slice;
}

unsigned ATOM_Swappable::getLastLockTime (void) const
{
	ATOM_STACK_TRACE(ATOM_Swappable::getLastLockTime);
	AutoLock autoLock;

	return _lastLockTime;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MemMapSection::MemMapSection (unsigned size)
{
	ATOM_STACK_TRACE(MemMapSection::MemMapSection);
	AutoLock autoLock;

	size = (size + 15) & ~15;

	_mapHandle = NULL;
	_totalSize = size;
	_sliceList = 0;

	if (_totalSize)
	{
		_mapHandle = ::CreateFileMapping (INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE|SEC_COMMIT, 0, _totalSize, NULL);

		if (_mapHandle)
		{
			_sliceList = ATOM_NEW(Slice);
			_sliceList->offset = 0;
			_sliceList->size = _totalSize;
			_sliceList->mapPtr = 0;
			_sliceList->free = true;
			_sliceList->prev = 0;
			_sliceList->next = 0;
			_sliceList->section = this;
		}
		else
		{
			char errMsg[256];

			DWORD err = ::GetLastError ();
			::FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, errMsg, sizeof(errMsg), NULL);

			ATOM_AppFatalEvent e(true, true, errMsg);
			ATOM_APP->handleEvent (&e);
		}
	}
}

MemMapSection::~MemMapSection (void)
{
	ATOM_STACK_TRACE(MemMapSection::~MemMapSection);
	AutoLock autoLock;

	while (_sliceList)
	{
		Slice *next = _sliceList->next;
		unmapSlice (_sliceList);
		ATOM_DELETE(_sliceList);
		_sliceList = next;
	}
	::CloseHandle (_mapHandle);
}

MemMapSection::Slice *MemMapSection::allocSlice (unsigned size)
{
	ATOM_STACK_TRACE(MemMapSection::allocSlice);
	AutoLock autoLock;

	static const unsigned unitSize = 64 * 1024;
	unsigned t = size % unitSize;
	if (t > 0)
	{
		size = size + unitSize - t;
	}

	Slice *bestSlice = 0;
	unsigned minSize = _totalSize;

	Slice *pSlice = _sliceList;
	while (pSlice)
	{
		if (pSlice->free && pSlice->size <= minSize && pSlice->size >= size)
		{
			bestSlice = pSlice;
		}
		pSlice = pSlice->next;
	}

	if (bestSlice)
	{
		bestSlice->free = false;

		if (bestSlice->size >= size + unitSize)
		{
			Slice *newSlice = ATOM_NEW(Slice);
			newSlice->free = true;
			newSlice->next = bestSlice->next;
			newSlice->offset = bestSlice->offset + size;
			newSlice->size = bestSlice->size - size;
			newSlice->prev = bestSlice;
			newSlice->section = this;
			newSlice->mapPtr = 0;

			if (bestSlice->next)
			{
				bestSlice->next->prev = newSlice;
			}
			bestSlice->size = size;
			bestSlice->next = newSlice;
		}

		return bestSlice;
	}

	return 0;
}

void MemMapSection::freeSlice (MemMapSection::Slice *slice)
{
	ATOM_STACK_TRACE(MemMapSection::freeSlice);
	AutoLock autoLock;

	unmapSlice (slice);

	slice->free = true;

	Slice *prev = slice->prev;
	Slice *next = slice->next;
	if (prev && prev->free)
	{
		prev->next = next;
		if (next)
		{
			next->prev = prev;
		}
		prev->size += slice->size;
		ATOM_DELETE(slice);
		slice = prev;
	}

	if (next && next->free)
	{
		slice->next = next->next;
		if (next->next)
		{
			next->next->prev = slice;
		}
		slice->size += next->size;
		ATOM_DELETE(next);
	}
}

void *MemMapSection::mapSlice (Slice *slice, unsigned size)
{
	ATOM_STACK_TRACE(MemMapSection::mapSlice);
	AutoLock autoLock;

	if (slice->mapPtr == 0)
	{
		if (_mapHandle)
		{
			slice->mapPtr = ::MapViewOfFile (_mapHandle, FILE_MAP_WRITE, 0, slice->offset, size);

			if (!slice->mapPtr)
			{
				char errMsg[256];

				DWORD err = ::GetLastError ();
				::FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM, NULL, 0, 0, errMsg, sizeof(errMsg), NULL);

				ATOM_AppFatalEvent e(true, true, errMsg);
				ATOM_APP->handleEvent (&e);
			}
		}
		else
		{
			slice->mapPtr = 0;
		}
	}

	if (!slice->mapPtr)
	{
	}

	return slice->mapPtr;
}

void MemMapSection::unmapSlice (Slice *slice)
{
	ATOM_STACK_TRACE(MemMapSection::unmapSlice);
	AutoLock autoLock;

	if (slice->mapPtr)
	{
		if (!::UnmapViewOfFile (slice->mapPtr))
		{
			char errMsg[256];

			DWORD err = ::GetLastError ();
			::FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM, NULL, 0, 0, errMsg, sizeof(errMsg), NULL);

			ATOM_AppFatalEvent e(true, true, errMsg);
			ATOM_APP->handleEvent (&e);
		}

		slice->mapPtr = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

SwappableManager::SwappableManager (void)
{
	ATOM_STACK_TRACE(SwappableManager::SwappableManager);
}

SwappableManager::~SwappableManager (void)
{
	ATOM_STACK_TRACE(SwappableManager::~SwappableManager);
	AutoLock autoLock;

	for (unsigned i = 0; i < _swappableVector.size(); ++i)
	{
		if (_swappableVector[i])
		{
			_swappableVector[i]->setSlot(-1);
		}
	}

	for (unsigned i = 0; i < _sections.size(); ++i)
	{
		ATOM_DELETE(_sections[i]);
	}
}

void SwappableManager::registerSwappable (ATOM_Swappable *swappable, bool allocFileMapping)
{
	ATOM_STACK_TRACE(SwappableManager::registerSwappable);
	AutoLock autoLock;

	if (swappable->getSlot() < 0)
	{
		if (allocFileMapping)
		{
			MemMapSection::Slice *slice = 0;
			for (unsigned i = 0; i < _sections.size() && !slice; ++i)
			{
				slice = _sections[i]->allocSlice (swappable->getSize());
			}

			if (!slice)
			{
				unsigned mapSize = ATOM_max2 (swappable->getSize(), 128 * 1024 * 1024);
				_sections.push_back (ATOM_NEW(MemMapSection, mapSize));
				slice = _sections.back()->allocSlice (swappable->getSize());
			}

			ATOM_ASSERT(slice);

			swappable->setSlice (slice);
		}

		if (_freeSlots.size() > 0)
		{
			int slot = _freeSlots.back();
			_freeSlots.pop_back ();
			_swappableVector[slot] = swappable;
			swappable->setSlot (slot);
		}
		else
		{
			_swappableVector.push_back (swappable);
			swappable->setSlot (_swappableVector.size() - 1);
		}
	}
}

void SwappableManager::unregisterSwappable (ATOM_Swappable *swappable)
{
	ATOM_STACK_TRACE(SwappableManager::unregisterSwappable);
	AutoLock autoLock;

	MemMapSection::Slice *slice = (MemMapSection::Slice *)swappable->getSlice();

	if (slice)
	{
		MemMapSection *section = slice->section;
		section->freeSlice (slice);
	}

	if (swappable->getSlot() >= 0)
	{
		_swappableVector[swappable->getSlot()] = 0;
		_freeSlots.push_back (swappable->getSlot());
		swappable->setSlot(-1);
	}
}

void SwappableManager::process (unsigned tick, unsigned tick_max, unsigned mem_size, unsigned mem_size_max)
{
	ATOM_STACK_TRACE(SwappableManager::process);
	AutoLock autoLock;

	if (mem_size <= mem_size_max)
	{
		return;
	}

	for (unsigned i = 0; i < _swappableVector.size(); ++i)
	{
		ATOM_Swappable *swappable = _swappableVector[i];

		if (swappable && swappable->getLastLockTime() != 0 && !swappable->isLocked() && tick - swappable->getLastLockTime() > tick_max && swappable->isDataValid())
		{
			_swapList.push_back (swappable);
		}
	}

	if (_swapList.size() > 0)
	{
		for (unsigned i = 0; i < _swapList.size(); ++i)
		{
			_swapList[i]->swapToDisk ();
		}
		_swapList.resize (0);
	}
}

SwappableManager ATOM_SwappableManager::_manager;
unsigned ATOM_SwappableManager::_memoryLimitInBytes = 500 * 1024 * 1024;
unsigned ATOM_SwappableManager::_expiredTime = 1000 * 10;
unsigned ATOM_SwappableManager::_lastProcessTime = 0;
unsigned ATOM_SwappableManager::_processInterval = 100;

void ATOM_SwappableManager::registerSwappable (ATOM_Swappable *swappable, bool allocFileMapping)
{
	ATOM_STACK_TRACE(ATOM_SwappableManager::registerSwappable);
	_manager.registerSwappable (swappable, allocFileMapping);
}

void ATOM_SwappableManager::unregisterSwappable (ATOM_Swappable *swappable)
{
	ATOM_STACK_TRACE(ATOM_SwappableManager::unregisterSwappable);
	_manager.unregisterSwappable (swappable);
}

void ATOM_SwappableManager::setMemoryLimitValue (unsigned numBytes)
{
	ATOM_STACK_TRACE(ATOM_SwappableManager::setMemoryLimitValue);
	_memoryLimitInBytes = numBytes;
}

void ATOM_SwappableManager::setExpiredTime (unsigned tick)
{
	_expiredTime = tick;
}

void ATOM_SwappableManager::setProcessInterval (unsigned tick)
{
	_processInterval = tick;
}

void ATOM_SwappableManager::checkAndSwap (unsigned currentTick)
{
	ATOM_STACK_TRACE(ATOM_SwappableManager::checkAndSwap);
	AutoLock autoLock;

	unsigned elapsed = currentTick - _lastProcessTime;
	if (elapsed >= _processInterval)
	{
		PROCESS_MEMORY_COUNTERS pmc;
		::GetProcessMemoryInfo (::GetCurrentProcess(), &pmc, sizeof(pmc));
		_manager.process (currentTick, _expiredTime, pmc.WorkingSetSize, _memoryLimitInBytes);
		_lastProcessTime = currentTick;
	}
}

void *ATOM_SwappableManager::mapSwappable (ATOM_Swappable *swappable)
{
	ATOM_STACK_TRACE(ATOM_SwappableManager::mapSwappable);
	AutoLock autoLock;

	MemMapSection::Slice *slice = (MemMapSection::Slice*)swappable->getSlice();

	return slice ? slice->section->mapSlice (slice, swappable->getSize()) : 0;
}

void ATOM_SwappableManager::unmapSwappable (ATOM_Swappable *swappable)
{
	ATOM_STACK_TRACE(ATOM_SwappableManager::unmapSwappable);
	AutoLock autoLock;

	MemMapSection::Slice *slice = (MemMapSection::Slice*)swappable->getSlice();

	if (slice)
	{
		slice->section->unmapSlice (slice);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_SwappableRawMemory::ATOM_SwappableRawMemory (unsigned size)
{
	_size = size;
	ATOM_SwappableManager::registerSwappable (this, true);
}

ATOM_SwappableRawMemory::~ATOM_SwappableRawMemory (void)
{
	ATOM_SwappableManager::unregisterSwappable (this);
}

unsigned ATOM_SwappableRawMemory::getSize (void) const
{
	return _size;
}

bool ATOM_SwappableRawMemory::write (void *buffer)
{
	return true;
}

bool ATOM_SwappableRawMemory::read (const void *buffer)
{
	_ptr = _mapPtr;

	return false;
}

bool ATOM_SwappableRawMemory::isDataValid (void) const
{
	return _ptr != 0;
}

void ATOM_SwappableRawMemory::invalidate (void)
{
	_ptr = 0;
}

ATOM_SwappableRawMemory *ATOM_SwappableRawMemory::clone (void)
{
	ATOM_SwappableRawMemory *other = ATOM_NEW(ATOM_SwappableRawMemory, getSize());

	void *srcPtr = lock ();
	void *dstPtr = other->lock ();
	memcpy (dstPtr, srcPtr, getSize());
	unlock ();
	other->unlock ();

	return other;
}

#endif
