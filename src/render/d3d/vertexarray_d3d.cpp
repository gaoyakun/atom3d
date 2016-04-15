#include "stdafx.h"
#include "d3d9device.h"
#include "vertexarray_d3d.h"



static const D3DVERTEXELEMENT9 _CoordElement = { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 };
static const D3DVERTEXELEMENT9 _CoordXYZWElement = { 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 };
static const D3DVERTEXELEMENT9 _CoordRHWElement = { 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 };
static const D3DVERTEXELEMENT9 _ColorElement = { 0, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 };
static const D3DVERTEXELEMENT9 _NormalElement = { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 };
static const D3DVERTEXELEMENT9 _PSizeElement = { 0, 0, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_PSIZE, 0 };
static const D3DVERTEXELEMENT9 _TexCoordElement = { 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 };
static const D3DVERTEXELEMENT9 _TangentElement = { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 };
static const D3DVERTEXELEMENT9 _BinormalElement = { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 };

//------------------------------------------------------------------------------------------------------------------------------------//
// wangjian ADDED for test float16
static const D3DVERTEXELEMENT9 _CoordCompressedElement = { 0, 0, D3DDECLTYPE_FLOAT16_4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 };
static const D3DVERTEXELEMENT9 _NormalCompressedElement = { 0, 0, D3DDECLTYPE_FLOAT16_4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 };
static const D3DVERTEXELEMENT9 _NormalCompressedBumpMappingElement = { 0, 0, D3DDECLTYPE_FLOAT16_4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 };
static const D3DVERTEXELEMENT9 _TangentCompressedElement = { 0, 0, D3DDECLTYPE_FLOAT16_4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 };
static const D3DVERTEXELEMENT9 _BinormalCompressedElement = { 0, 0, D3DDECLTYPE_FLOAT16_4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 };
static const D3DVERTEXELEMENT9 _TexCoordCompressedElement = { 0, 0, D3DDECLTYPE_FLOAT16_2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 };
//------------------------------------------------------------------------------------------------------------------------------------//

#define LOCK_READ  1L
#define LOCK_WRITE 2L

ATOM_D3DVertexArray::ATOM_D3DVertexArray(void) 
{
  ATOM_STACK_TRACE(ATOM_D3DVertexArray::ATOM_D3DVertexArray);
  _M_vertexbuffer = 0;
  _M_locked = 0;
  _M_size_bytes = 0;
}

ATOM_D3DVertexArray::~ATOM_D3DVertexArray (void)
{
  ATOM_STACK_TRACE(ATOM_D3DVertexArray::~ATOM_D3DVertexArray);
  setRenderDevice (0);
}

void ATOM_D3DVertexArray::invalidateImpl (bool needRestore)
{
  ATOM_STACK_TRACE(ATOM_D3DVertexArray::invalidateImpl);
  if (_M_vertexbuffer && (!needRestore || ATOM_USAGE_DYNAMIC == _M_usage))
  {
    _M_vertexbuffer->Release();
    _M_vertexbuffer = 0;
	_M_locked = 0;

	if (_preservedContent)
	{
		ATOM_FREE(_preservedContent);
		_preservedContent = 0;
	}
  }
}

bool ATOM_D3DVertexArray::_internalCreateVertexBuffer (unsigned bytes) {
  ATOM_STACK_TRACE(ATOM_D3DVertexArray::_internalCreateVertexBuffer);
  if (_M_device && _M_device->isDeviceReady ())
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

    HRESULT hr = device->getD3DDevice()->CreateVertexBuffer (bytes, usage, 0, pool, &_M_vertexbuffer, 0);

    if (FAILED(hr))
    {
		if (pool == D3DPOOL_DEFAULT && hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			ATOM_LOGGER::error ("%s(%d) CreateVertexBuffer() failed because there is not enough video memory, evicting managed resources and try again...\n", __FUNCTION__, bytes);
			ATOM_CHECK_D3D_RESULT(device->getD3DDevice()->EvictManagedResources ());
			hr = device->getD3DDevice()->CreateVertexBuffer (bytes, usage, 0, pool, &_M_vertexbuffer, 0);
			if (FAILED(hr))
			{
				ATOM_LOGGER::error ("CreateVertexBuffer() still failed!\n");
				ATOM_CHECK_D3D_RESULT(hr);
				return false;
			}
		}
		else
		{
			ATOM_LOGGER::error ("%s(%d) CreateVertexBuffer() failed!\n", __FUNCTION__, bytes);
			ATOM_CHECK_D3D_RESULT(hr);
			return false;
		}

		return false;
    }

    ATOM_ASSERT (_M_vertexbuffer);
	_M_locked = 0;

	if (getRenderDevice()->isContentBackupEnabled())
	{
		_preservedContent = ATOM_MALLOC(bytes);
	}

    return true;
  }

  return false;
}

bool ATOM_D3DVertexArray::updateData (const void *data, unsigned bytes) 
{
  ATOM_STACK_TRACE(ATOM_D3DVertexArray::updateData);

  if (!_M_device || !_M_device->isDeviceReady ())
  {
	  return false;
  }

  if (!_M_vertexbuffer)
  {
	  _M_num_verts = bytes / _M_vertex_size;
	  _M_size_bytes = bytes;

	  if (!_internalCreateVertexBuffer (bytes))
	  {
		return false;
	  }
  }

  if (_M_locked)
  {
	  ATOM_LOGGER::error ("%s(0x%08X, %u) VertexArray is locked!\n", __FUNCTION__, data, bytes);
	  return false;
  }

  if (data)
  {
	void *p = lock (ATOM_LOCK_WRITEONLY, 0, bytes, true);
	if (!p)
	{
		return false;
	}
	memcpy (p, data, bytes);
	unlock ();
  }

  return true;
}

//--- wangjian modified for compressed attributes ---//
bool ATOM_D3DVertexArray::getVertexElement (unsigned attrib, D3DVERTEXELEMENT9 *element, unsigned attribFlags/* = 0*/) 
{
  ATOM_STACK_TRACE(ATOM_D3DVertexArray::getVertexElement);
  if (element)
  {
    switch (attrib)
    {
    case ATOM_VERTEX_ATTRIB_COORD:
		{
			if( attribFlags & ATTRIBUTE_FLAG_COMPRESSED )
				*element = _CoordCompressedElement;
			else
				*element = _CoordElement;
		}
		break;
    case ATOM_VERTEX_ATTRIB_COORD_XYZW:
		*element = _CoordXYZWElement;
		break;
	case ATOM_VERTEX_ATTRIB_COORD_XYZRHW:
		*element = _CoordRHWElement;
		break;
    case ATOM_VERTEX_ATTRIB_NORMAL:
		{
			if( attribFlags & ATTRIBUTE_FLAG_COMPRESSED )
			{
				if( attribFlags & ATTRIBUTE_FLAG_BUMPMAPPING )		
					*element = _NormalCompressedBumpMappingElement;			// 如果使用法线映射
				else
					*element = _NormalCompressedElement;
			}
			else
				*element = _NormalElement;
		}
		break;
	case ATOM_VERTEX_ATTRIB_PSIZE:
		*element = _PSizeElement;
		break;
    case ATOM_VERTEX_ATTRIB_PRIMARY_COLOR:
		*element = _ColorElement;
		break;
    case ATOM_VERTEX_ATTRIB_TANGENT:
		{
			if( attribFlags & ATTRIBUTE_FLAG_COMPRESSED )
				*element = _TangentCompressedElement;
			else
				*element = _TangentElement;
		}
		break;
	case ATOM_VERTEX_ATTRIB_BINORMAL:
		{
			if( attribFlags & ATTRIBUTE_FLAG_COMPRESSED )
				*element = _BinormalCompressedElement;
			else
				*element = _BinormalElement;
		}
		break;
    default:
		{
			for (unsigned i = 0;i < ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
			{
				if (attrib & ATOM_VERTEX_ATTRIB_TEX_MASK(i))
				{
					if( attribFlags & ATTRIBUTE_FLAG_COMPRESSED )
					{
						*element = _TexCoordCompressedElement;
						element->UsageIndex = i;
						switch (ATOM_VERTEX_ATTRIB_TEX_COMPONENTS(attrib, i))
						{
						case 1:
							element->Type = D3DDECLTYPE_FLOAT1;
							break;
						case 2:
							element->Type = D3DDECLTYPE_FLOAT16_2;
							break;
						case 3:
							element->Type = D3DDECLTYPE_FLOAT3;
							break;
						case 4:
							element->Type = D3DDECLTYPE_FLOAT16_4;
							break;
						default:
							return false;
							break;
						}
					}
					else
					{
						*element = _TexCoordElement;
						element->UsageIndex = i;
						switch (ATOM_VERTEX_ATTRIB_TEX_COMPONENTS(attrib, i))
						{
						case 1:
							element->Type = D3DDECLTYPE_FLOAT1;
							break;
						case 2:
							element->Type = D3DDECLTYPE_FLOAT2;
							break;
						case 3:
							element->Type = D3DDECLTYPE_FLOAT3;
							break;
						case 4:
							element->Type = D3DDECLTYPE_FLOAT4;
							break;
						default:
							return false;
							break;
						}
					}
					

					return true;
				}
			}

			return false;
		}
    }

    return true;
  }

  return false;
}

bool ATOM_D3DVertexArray::realize (void) {
  ATOM_STACK_TRACE(ATOM_D3DVertexArray::realize);
  if (!_M_vertexbuffer)
  {
    if (!_internalCreateVertexBuffer (_M_size_bytes))
    {
      ATOM_LOGGER::error ("%s() Create vertex buffer failed!\n", __FUNCTION__);
      return false;
    }
    ATOM_ASSERT (_M_vertexbuffer);
  }
  return true;
}

void ATOM_D3DVertexArray::restoreImpl (void) {
  ATOM_STACK_TRACE(ATOM_D3DVertexArray::restoreImpl);
  if (_M_size_bytes && ATOM_USAGE_DYNAMIC == _M_usage)
  {
    realize();
	setContentLost (true);
  }
}

void *ATOM_D3DVertexArray::lock (ATOM_LockMode mode, unsigned offset, unsigned size, bool overwrite) {
  ATOM_STACK_TRACE(ATOM_D3DVertexArray::lock);
  if (_M_locked)
  {
	  ATOM_LOGGER::error ("%s(%d, %u, %u, %d) VertexArray is locked!\n", __FUNCTION__, mode, offset, size, overwrite ? 1 : 0);
	  return 0;
  }

  if (_M_usage == ATOM_USAGE_DYNAMIC)
  {
	  _M_lock_flags = overwrite ? D3DLOCK_DISCARD : D3DLOCK_NOOVERWRITE;
  }
  else if (mode == ATOM_LOCK_READONLY)
  {
	  _M_lock_flags = D3DLOCK_READONLY;
  }
  else
  {
	  _M_lock_flags = 0;
  }

  _M_lock_offset = offset;
  _M_lock_size = size;
  _M_locked = mode == ATOM_LOCK_READONLY ? LOCK_READ : LOCK_WRITE;

  if (_preservedContent)
  {
	  return (char*)_preservedContent + offset;
  }
  else if (_M_vertexbuffer)
  {
    void *Data = 0;

    HRESULT hr = _M_vertexbuffer->Lock (_M_lock_offset, _M_lock_size, &Data, _M_lock_flags);
    if (FAILED(hr))
    {
	  ATOM_LOGGER::error ("%s(%d, %d, %d, %d) Lock failed!\n", __FUNCTION__, mode, offset, size, overwrite);
	  ATOM_CHECK_D3D_RESULT(hr);
      return 0;
    }

    return Data;
  }

  return 0;
}

void ATOM_D3DVertexArray::unlock (void)
{
	if (!_M_locked)
	{
		return;
	}

	if (_M_vertexbuffer)
	{
		HRESULT hr = S_OK;

		if (_preservedContent)
		{
			if (_M_locked != LOCK_READ)
			{
				void *Data = 0;

				hr = _M_vertexbuffer->Lock (_M_lock_offset, _M_lock_size, &Data, _M_lock_flags);
				if (FAILED(hr))
				{
					ATOM_LOGGER::error ("%s() unlock failed!\n", __FUNCTION__);
					ATOM_CHECK_D3D_RESULT(hr);
				}

				memcpy (Data, (char*)_preservedContent+_M_lock_offset, _M_lock_size?_M_lock_size:getVertexStride()*getNumVertices());
				hr = _M_vertexbuffer->Unlock ();
			}
		}
		else
		{
			hr = _M_vertexbuffer->Unlock ();
		}
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s() Unlock failed!\n", __FUNCTION__);
			ATOM_CHECK_D3D_RESULT(hr);
		}
	}

	_M_locked = 0;
}

bool ATOM_D3DVertexArray::isLocked (void) const
{
	return _M_locked;
}


