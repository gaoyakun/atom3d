#include "stdafx.h"
#include "d3d9device.h"
#include "indexarray_d3d.h"

ATOM_D3D9IndexArray::ATOM_D3D9IndexArray (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9IndexArray::ATOM_D3D9IndexArray);

  _M_indexbuffer = 0;
  _M_size_bytes = 0;
}

ATOM_D3D9IndexArray::~ATOM_D3D9IndexArray (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9IndexArray::~ATOM_D3D9IndexArray);

  setRenderDevice (0);
}

void ATOM_D3D9IndexArray::invalidateImpl (bool needRestore)
{
  ATOM_STACK_TRACE(ATOM_D3D9IndexArray::invalidateImpl);

  if (_M_indexbuffer && (!needRestore || ATOM_USAGE_DYNAMIC == _M_usage))
  {
    _M_indexbuffer->Release();
    _M_indexbuffer = 0;
  }
}

void ATOM_D3D9IndexArray::restoreImpl (void)
{
	ATOM_STACK_TRACE(ATOM_D3D9IndexArray::restoreImpl);

	if (_M_size_bytes && ATOM_USAGE_DYNAMIC == _M_usage)
	{
		realize (_M_usage, _M_num_indices, _M_32bit);
		setContentLost (true);
	}
}

bool ATOM_D3D9IndexArray::realize (int usage, unsigned length, bool use32bit)
{
	ATOM_STACK_TRACE(ATOM_D3D9IndexArray::realize);

	if (length == 0 || _M_indexbuffer)
	{
		return false;
	}

	if (!_M_device || !_M_device->isDeviceReady ())
	{
		return 0;
	}


	_M_num_indices = length;
	_M_32bit = use32bit;
	_M_usage = usage;
	_M_size_bytes = length * (use32bit ? 4 : 2);

	if (!_internalCreateIndexBuffer (_M_size_bytes))
	{
		ATOM_LOGGER::error ("ATOM_D3D9IndexArray::Realize: Create index buffer failed.\n");
		return false;
	}
	ATOM_ASSERT (_M_indexbuffer);

	return true;
}

bool ATOM_D3D9IndexArray::_internalCreateIndexBuffer (unsigned bytes)
{
	ATOM_STACK_TRACE(ATOM_D3D9IndexArray::_internalCreateIndexBuffer);

	if (_M_device && _M_device->isDeviceReady())
	{
		ATOM_D3D9Device *device = (ATOM_D3D9Device*)_M_device;
		if (!device->getD3DDevice())
			return false;

		DWORD usage = D3DUSAGE_WRITEONLY;
		D3DPOOL pool = D3DPOOL_MANAGED;

		switch (_M_usage)
		{
		case ATOM_USAGE_STATIC:
			usage = D3DUSAGE_WRITEONLY;
			break;
		case ATOM_USAGE_DYNAMIC:
			usage = D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC;
			pool = D3DPOOL_DEFAULT;
			break;
		case ATOM_USAGE_STOREONLY:
			return false;
			break;
		}

		ATOM_CHECK_D3D_RESULT(device->getD3DDevice()->CreateIndexBuffer (bytes, usage, _M_32bit ? D3DFMT_INDEX32 : D3DFMT_INDEX16, pool, &_M_indexbuffer, 0));

		return _M_indexbuffer != NULL;
	}

	return false;
}

void * ATOM_D3D9IndexArray::lock(ATOM_LockMode mode, unsigned offset, unsigned size, bool overwrite) 
{
	ATOM_STACK_TRACE(ATOM_D3D9IndexArray::lock);

	if (!_M_device || !_M_device->isDeviceReady ())
	{
		return 0;
	}

	if (_M_indexbuffer)
	{
		void *buf = 0;
		DWORD lockFlags;
		if (_M_usage == ATOM_USAGE_DYNAMIC)
		{
			lockFlags = overwrite ? D3DLOCK_DISCARD : D3DLOCK_NOOVERWRITE;
		}
		else if (mode == ATOM_LOCK_READONLY)
		{
			lockFlags = D3DLOCK_READONLY;
		}
		else
		{
			lockFlags = 0;
		}

		ATOM_CHECK_D3D_RESULT(_M_indexbuffer->Lock (offset, size, &buf, lockFlags));
		return buf;
	}
	return 0;
}

void ATOM_D3D9IndexArray::unlock(void)
{
	ATOM_STACK_TRACE(ATOM_D3D9IndexArray::unlock);

	if (!_M_device || !_M_device->isDeviceReady ())
	{
		return;
	}

	if (_M_indexbuffer)
	{
		ATOM_CHECK_D3D_RESULT(_M_indexbuffer->Unlock ());
	}
}

