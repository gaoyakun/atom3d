#include "stdafx.h"
#include "d3d9device.h"
#include "d3d9window.h"
#include "texturemanager_d3d.h"
#include "vbmanager_d3d.h"
#include "ibmanager_d3d.h"
#include "vertexarray_d3d.h"
#include "indexarray_d3d.h"
#include "depthbuffer_d3d.h"
#include "texture_d3d.h"
#include "initializeD3D9.h"
#include "text.h"
#include "statecache.h"
#include "vertexshader_d3d.h"
#include "pixelshader_d3d.h"
#include "gpuprogram_d3d.h"
#include "enumtable_d3d.h"
#include "stateset_base.h"
//--- wangjian added ---//
#include "rendertargetsurface_d3d.h"
//----------------------//

#pragma comment(lib, "dxguid")

#define INTERMEDIATE_VERTEXBUFFER_SIZE (2048 * 2048)
#define INTERMEDIATE_INDEXBUFFER_SIZE (1024 * 1024)
#define USE_FAST_FTOL

# define ATOM_D3DCOLOR_COLORVALUE(r, g, b, a) D3DCOLOR_RGBA(ATOM_ftol((r)*255.f), ATOM_ftol((g)*255.f), ATOM_ftol((b)*255.f), ATOM_ftol((a)*255.f))

ATOM_D3D9Device *sRenderDevice = 0;

long TextureCount = 0;
long SurfaceCount = 0;
int BeginSceneCount = 0;

unsigned const PrimTypeTable[] = {
  D3DPT_TRIANGLELIST,
  D3DPT_TRIANGLESTRIP,
  D3DPT_TRIANGLEFAN,
  D3DPT_LINELIST,
  D3DPT_LINESTRIP,
  D3DPT_POINTLIST
};

ATOM_RenderDevice *GetGlobalDevice (void)
{
  return sRenderDevice;
}

extern ATOM_RenderDeviceCaps device_caps;

struct SamplerStateInfo
{
  int minFilter;
  int magFilter;
  int wrapModeU;
  int wrapModeV;
  int wrapModeW;
  bool anisotropicFilter;
  int coordsrc;
  bool transform;
  unsigned bordercolor;
  ATOM_AUTOREF(ATOM_D3DTexture) texture;

  SamplerStateInfo (void) {
    init ();
  }

  void init (void) {
    minFilter = -1;
    magFilter = -1;
    wrapModeU = -1;
    wrapModeV = -1;
    wrapModeW = -1;
    coordsrc = -1;
    transform = false;
    anisotropicFilter = false;
    bordercolor = 0;
  }
};

static inline unsigned GetStencilBits (D3DFORMAT format)
{
  switch (format)
  {
  case D3DFMT_D15S1:
    return 1;
  case D3DFMT_D24S8:
    return 8;
  case D3DFMT_D24X4S4:
    return 4;
  case D3DFMT_D24FS8:
    return 8;
  default:
    return 0;
  }
}

void ATOM_D3D9Device::setInitialStates (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::setInitialStates);

  if (!_M_d3ddevice)
    return;

  //_DeviceGraphicTest ();
  _M_d3ddevice->SetRenderState(D3DRS_LIGHTING, FALSE);

  ATOM_AUTOREF(ATOM_D3D9Window) window = _M_render_window;
  ATOM_ASSERT (window);

  _M_has_zbuffer = true;

  D3DFORMAT depthFormat = ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->presentParams.AutoDepthStencilFormat;
  _M_has_stencilbuffer = GetStencilBits (depthFormat) != 0;

  _stateCache->loadDefaults();
  _stateCache->getDefaultStates()->apply(0, true);
}

bool ATOM_D3D9Device::canAutoGenMipmap (D3DFORMAT format) const
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::canAutoGenMipmap);

  if (device_caps.mipmap_generation)
  {
    ATOM_AUTOREF(ATOM_D3D9Window) window = _M_render_window;
    HRESULT hr = _M_d3d->CheckDeviceFormat(
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->adapter,
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->devtype,
	  ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->presentParams.BackBufferFormat,
      D3DUSAGE_AUTOGENMIPMAP,
      D3DRTYPE_TEXTURE,
      format);

    return hr == D3D_OK;
  }

  return false;
}

ATOM_D3D9Device::ATOM_D3D9Device()
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::ATOM_D3D9Device);

  ATOM_ASSERT (!sRenderDevice);
  sRenderDevice = this;
  _M_d3ddevice = 0;
  _M_d3d = 0;

  _M_current_clientstate_mask = 0;
  _M_current_pointer_mask = 0;
  _M_need_adjustlights = false;
  _M_interleaved_stream = false;
  _M_num_vertices = 0;

  _M_texture_manager = ATOM_NEW(ATOM_ID3DTextureManager, this);
  _M_vb_manager = ATOM_NEW(ATOM_ID3DVBManager, this);
  _M_ib_manager = ATOM_NEW(ATOM_D3DIBManager, this);

  //memset (_M_texture_lod, 0, sizeof(_M_texture_lod));

  _M_has_zbuffer = false;
  _M_has_stencilbuffer = false;
  _M_current_query = 0;
  _M_intermediate_vertexbuffer = 0;
  _M_intermediate_vertexbuffer_size = 0;
  _M_intermediate_vertexbuffer_pos = 0;
  _M_intermediate_indexbuffer = 0;
  _M_intermediate_indexbuffer_size = 0;
  _M_intermediate_indexbuffer_pos = 0;
  _M_current_decl = 0;
  _M_texture_quality = TQ_LOW;

  _M_device_ready = false;
  _stateCache = 0;
  _M_taa_fourcc = 0;
  _M_best_HDR_format = ATOM_PIXEL_FORMAT_UNKNOWN;
  _M_best_HDR_format_rg = ATOM_PIXEL_FORMAT_UNKNOWN;
  _M_best_HDR_format_r = ATOM_PIXEL_FORMAT_UNKNOWN;

  _M_default_backbuffer = 0;
  _M_default_depthbuffer = 0;

  _depthTextureFormat = D3DFMT_UNKNOWN;
  _M_frame_stamp = 0;
  _M_num_drawcalls = 0;
  _M_num_primitives = 0;
  _M_beginframe_counter = 0;
}

ATOM_D3D9Device::~ATOM_D3D9Device() {
  ATOM_STACK_TRACE(ATOM_D3D9Device::~ATOM_D3D9Device);

  while (_M_views.size() > 0)
  {
    removeView (_M_views[0]->window.get());
  }
  _M_render_window = 0;
  _M_current_view = 0;
  sRenderDevice = 0;
  shutdown ();

  for (unsigned i = 0; i < _M_views.size(); ++i)
  {
    _M_views[i]->window->detachDevice();
  }

  ATOM_DELETE(_M_texture_manager);
  _M_texture_manager = 0;

  ATOM_DELETE(_M_vb_manager);
  _M_vb_manager = 0;

  ATOM_DELETE(_M_ib_manager);
  _M_ib_manager = 0;

  ATOM_DELETE(_stateCache);
}

void ATOM_D3D9Device::shutdown (void) {
  ATOM_STACK_TRACE(ATOM_D3D9Device::shutdown);

  invalidateAllResources ();
  freeAllResources ();

  for (ATOM_SET<ATOM_VertexDecl>::iterator it = _M_decls.begin(); it != _M_decls.end(); ++it)
  {
	  IDirect3DVertexDeclaration9 *decl = (IDirect3DVertexDeclaration9*)(*it);
	  decl->Release ();
  }
  _M_decls.clear ();

  if (_M_d3ddevice)
  {
	ATOM_CHECK_D3D_RESULT(_M_d3ddevice->EvictManagedResources ());
    _M_d3ddevice->Release();
    _M_d3ddevice = 0;
  }

  if (_M_d3d)
  {
    _M_d3d->Release();
    _M_d3d = 0;
  }

  _M_device_ready = false;
}

const char *ATOM_D3D9Device::getVendor (void) const {
  return "Microsoft";
}

const char *ATOM_D3D9Device::getRenderer (void) const {
  return "D3D9";
}

unsigned ATOM_D3D9Device::requireIntermediateVertexBuffer (unsigned size)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::requireIntermediateVertexBuffer);

  if (!_M_d3ddevice)
    return 0;

  if (!_M_intermediate_vertexbuffer || size > _M_intermediate_vertexbuffer_size)
  {
    if (_M_intermediate_vertexbuffer)
    {
      _M_intermediate_vertexbuffer->Release();
      _M_intermediate_vertexbuffer = 0;
    }

    unsigned vbsize = ATOM_max2(size, INTERMEDIATE_VERTEXBUFFER_SIZE);
	HRESULT hr = _M_d3ddevice->CreateVertexBuffer(vbsize, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &_M_intermediate_vertexbuffer , 0);
	if (FAILED(hr))
	{
		if (hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			ATOM_LOGGER::warning ("%s(%d) CreateVertexBuffer() failed due to insufficient video memory, all managed resource will be evicted.\n", __FUNCTION__, size);
			ATOM_CHECK_D3D_RESULT(_M_d3ddevice->EvictManagedResources ());
			hr = _M_d3ddevice->CreateVertexBuffer(vbsize, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &_M_intermediate_vertexbuffer , 0);
			if (FAILED(hr))
			{
				_M_intermediate_vertexbuffer_size = 0;
				_M_intermediate_vertexbuffer_pos = 0;
				ATOM_LOGGER::error ("%s(%d) CreateVertexBuffer() failed.\n", __FUNCTION__, size);
				ATOM_CHECK_D3D_RESULT(hr);
				return 0;
			}
		}
		else
		{
			_M_intermediate_vertexbuffer_size = 0;
			_M_intermediate_vertexbuffer_pos = 0;
			ATOM_LOGGER::error ("%s(%d) CreateVertexBuffer() failed.\n", __FUNCTION__, size);
			ATOM_CHECK_D3D_RESULT(hr);
			return 0;
		}
	}

    _M_intermediate_vertexbuffer_size = vbsize;
    _M_intermediate_vertexbuffer_pos = 0;
  }

  return 1;
}

unsigned ATOM_D3D9Device::requireIntermediateIndexBuffer (unsigned size)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::requireIntermediateIndexBuffer);

  if (!_M_d3ddevice)
    return 0;

  if (!_M_intermediate_indexbuffer || size > _M_intermediate_indexbuffer_size)
  {
    if (_M_intermediate_indexbuffer)
    {
      _M_intermediate_indexbuffer->Release();
      _M_intermediate_indexbuffer = 0;
    }

    unsigned ibsize = ATOM_max2(size, INTERMEDIATE_INDEXBUFFER_SIZE);
	HRESULT hr = _M_d3ddevice->CreateIndexBuffer(ibsize, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &_M_intermediate_indexbuffer , 0);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%d) CreateIndexBuffer failed.\n", __FUNCTION__, size);
		ATOM_CHECK_D3D_RESULT(hr);
		return 0;
	}

    _M_intermediate_indexbuffer_size = ibsize;
    _M_intermediate_indexbuffer_pos = 0;
  }

  return 1;
}

bool ATOM_D3D9Device::getBackbufferSize (int *w, int *h) const
{
	if (w)
	{
		*w = _backbufferW;
	}

	if (h)
	{
		*h = _backbufferH;
	}

	return true;
}


bool ATOM_D3D9Device::getBackbufferDimension (int *w, int *h) const
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::getBackbufferDimension);

	if (w) *w = 0;
	if (h) *h = 0;

	if (!_M_default_backbuffer)
	{
		return false;
	}

	D3DSURFACE_DESC surfaceDesc;
	HRESULT hr = _M_default_backbuffer->GetDesc(&surfaceDesc);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X, 0x%08X) GetDesc() failed.\n", __FUNCTION__, w, h);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	if (w) *w = surfaceDesc.Width;
	if (h) *h = surfaceDesc.Height;

	return true;
}

bool ATOM_D3D9Device::readColorBuffer(ATOM_RenderWindow *view, int x, int y, int w, int h, ATOM_PixelFormat format, void *pixels)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::readColorBuffer);

	if (!_M_d3ddevice)
	{
		return false;
	}

	if (!_M_default_backbuffer)
	{
		return false;
	}

	D3DFORMAT d3dfmt;
	switch (format)
	{
	case ATOM_PIXEL_FORMAT_BGRA8888:
		d3dfmt = D3DFMT_A8R8G8B8;
		break;
	case ATOM_PIXEL_FORMAT_BGRX8888:
		d3dfmt = D3DFMT_X8R8G8B8;
		break;
	case ATOM_PIXEL_FORMAT_BGR888:
		d3dfmt = D3DFMT_R8G8B8;
		break;
	default:
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d, %d, 0x%08X) Invalid format.\n", __FUNCTION__, view, x, y, w, h, format, pixels);
		return false;
	}

	D3DSURFACE_DESC surfaceDesc;
	HRESULT hr;
	hr = _M_default_backbuffer->GetDesc(&surfaceDesc);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d, %d, 0x%08X) GetDesc() failed.\n", __FUNCTION__, view, x, y, w, h, format, pixels);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	IDirect3DSurface9 *intermediateTarget = 0;
	hr = _M_d3ddevice->CreateRenderTarget(surfaceDesc.Width, surfaceDesc.Height, surfaceDesc.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &intermediateTarget, NULL);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d, %d, 0x%08X) GetDesc() failed.\n", __FUNCTION__, view, x, y, w, h, format, pixels);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	hr = _M_d3ddevice->StretchRect(_M_default_backbuffer, NULL, intermediateTarget, NULL, D3DTEXF_NONE);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d, %d, 0x%08X) StretchRect() failed.\n", __FUNCTION__, view, x, y, w, h, format, pixels);
		intermediateTarget->Release();
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	IDirect3DSurface9 *systemSurface = 0;
	hr = _M_d3ddevice->CreateOffscreenPlainSurface(surfaceDesc.Width, surfaceDesc.Height, surfaceDesc.Format, D3DPOOL_SYSTEMMEM, &systemSurface, NULL);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d, %d, 0x%08X) CreateOffscreenPlainSurface() failed.\n", __FUNCTION__, view, x, y, w, h, format, pixels);
		intermediateTarget->Release();
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	hr = _M_d3ddevice->GetRenderTargetData (intermediateTarget, systemSurface);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d, %d, 0x%08X) GetRenderTargetData() failed.\n", __FUNCTION__, view, x, y, w, h, format, pixels);
		intermediateTarget->Release();
		systemSurface->Release();
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	intermediateTarget->Release ();

	IDirect3DSurface9 *destSurface = 0;
	hr = _M_d3ddevice->CreateOffscreenPlainSurface(surfaceDesc.Width, surfaceDesc.Height, d3dfmt, D3DPOOL_SCRATCH, &destSurface, NULL);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d, %d, 0x%08X) CreateOffscreenPlainSurface() failed.\n", __FUNCTION__, view, x, y, w, h, format, pixels);
		systemSurface->Release();
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	//--- wangjian commented ---//
	// 去除D3DX依赖：使用LOCK/UNLOCK进行拷贝
	hr = D3DXLoadSurfaceFromSurface(destSurface, NULL, NULL, systemSurface, NULL, NULL, D3DX_FILTER_NONE, 0);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d, %d, 0x%08X) D3DXLoadSurfaceFromSurface() failed.\n", __FUNCTION__, view, x, y, w, h, format, pixels);
		systemSurface->Release();
		destSurface->Release();
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	systemSurface->Release();

    D3DLOCKED_RECT LockRect;

    RECT rc;
    rc.left   = x;
    rc.right  = x + w;
    rc.top    = y;
    rc.bottom = y + h;

	hr = destSurface->LockRect(&LockRect, &rc, D3DLOCK_READONLY);
    if(FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d, %d, 0x%08X) LockRect() failed.\n", __FUNCTION__, view, x, y, w, h, format, pixels);
		destSurface->Release();
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

    memcpy (pixels, LockRect.pBits, LockRect.Pitch * h);

	destSurface->UnlockRect ();
	destSurface->Release ();

	return true;
}

bool ATOM_D3D9Device::readDepthBuffer(ATOM_RenderWindow *view, int x, int y, int w, int h, void *pixels) {
  return false;
}

bool ATOM_D3D9Device::readStencilBuffer (ATOM_RenderWindow *view, int x, int y, int w, int h, void *pixels) {
  return false;
}

void ATOM_D3D9Device::enableScissorTest (ATOM_RenderWindow *view, bool enable)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::enableScissorTest);

  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;

  if (_M_d3ddevice && v)
  {
    v->enable_scissor_test = enable;

    if (v == _M_current_view)
    {
		_M_d3ddevice->SetRenderState(D3DRS_SCISSORTESTENABLE, enable);
    }
  }
}

bool ATOM_D3D9Device::isScissorTestEnabled (ATOM_RenderWindow *view) const
{
  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  return v->enable_scissor_test;
}

bool ATOM_D3D9Device::setStreamSource(unsigned stream, ATOM_VertexArray* vertex_array, unsigned offset)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::setStreamSource);

	ATOM_D3DVertexArray *va = (ATOM_D3DVertexArray*)vertex_array;
	HRESULT hr = _M_d3ddevice->SetStreamSource (stream, (va ? va->getD3DVertexBuffer() : 0), offset, (va ? va->getVertexStride() : 0));
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%d, 0x%08X) SetStreamSource() failed.\n", __FUNCTION__, stream, vertex_array);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	static const unsigned coordAttribMask = ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_COORD_XYZRHW;
	if (va && (va->getAttributes() & coordAttribMask))
	{
		_M_num_vertices = va->getNumVertices();
	}

	return true;
}

bool ATOM_D3D9Device::setStreamSourceFreq (unsigned stream, unsigned freq)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::setStreamSourceFreq);

	if (device_caps.shader_model < 3)
	{
		return false;
	}

	HRESULT hr = _M_d3ddevice->SetStreamSourceFreq (stream, freq);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%d, %d) SetStreamSourceFreq() failed.\n", __FUNCTION__, stream, freq);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	return true;
}

void ATOM_D3D9Device::setClientStates(unsigned streams) {
}

void ATOM_D3D9Device::renderStreams(int type, int prim_count) {
  ATOM_STACK_TRACE(ATOM_D3D9Device::renderStreams);

  if (_M_d3ddevice)
  {
    if ( prim_count == 0)
      return;

    ATOM_ASSERT (ATOM_GFX_IS_VALID_PRIMITIVE_TYPE(type));
    D3DPRIMITIVETYPE primType = (D3DPRIMITIVETYPE)ATOM_GFX_MAP_TO_D3D_PRIMITIVE_TYPE(type);

	_stateCache->sync ();
    ATOM_CHECK_D3D_RESULT(_M_d3ddevice->DrawPrimitive(primType, 0, prim_count));

    _M_num_drawcalls++;
	_M_num_primitives += prim_count;
  }
}

void ATOM_D3D9Device::renderStreamsIndexed(ATOM_IndexArray* index_array, int type, int prim_count, int offset)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::renderStreamsIndexed);

  if (_M_d3ddevice)
  {
    if (!index_array || !index_array->realize())
    {
      return;
    }

    ATOM_D3D9IndexArray *ib = (ATOM_D3D9IndexArray*)index_array;
    ATOM_ASSERT (ib);

    if ( prim_count == 0)
      return;

    ATOM_ASSERT (ATOM_GFX_IS_VALID_PRIMITIVE_TYPE(type));
    D3DPRIMITIVETYPE primType = (D3DPRIMITIVETYPE)ATOM_GFX_MAP_TO_D3D_PRIMITIVE_TYPE(type);

	HRESULT hr;

	hr = _M_d3ddevice->SetIndices (ib->getD3DIndexBuffer());
	if (FAILED(hr))
	{
		ATOM_CHECK_D3D_RESULT(hr);
		return;
	}

	_stateCache->sync ();
	ATOM_CHECK_D3D_RESULT(_M_d3ddevice->DrawIndexedPrimitive(primType, 0, 0, _M_num_vertices, offset, prim_count));

    _M_num_drawcalls++;
	_M_num_primitives += prim_count;
  }
}

void ATOM_D3D9Device::renderInterleavedMemoryStreamsIndexed (int type, int vertex_count, int vertex_range, unsigned attrib, unsigned stride, const void *stream, const unsigned short *indices) {
  ATOM_STACK_TRACE(ATOM_D3D9Device::renderInterleavedMemoryStreamsIndexed);

  if (_M_d3ddevice)
  {
    unsigned prim_count = 0;
    switch ( type)
    {
    case ATOM_PRIMITIVE_TRIANGLES:
      prim_count = vertex_count / 3;
      break;
    case ATOM_PRIMITIVE_TRIANGLE_STRIP:
    case ATOM_PRIMITIVE_TRIANGLE_FAN:
      prim_count = vertex_count - 2;
      break;
    case ATOM_PRIMITIVE_LINES:
      prim_count = vertex_count / 2 ;
      break;
    case ATOM_PRIMITIVE_LINE_STRIP:
      prim_count = vertex_count - 1;
      break;
    case ATOM_PRIMITIVE_POINTS:
      prim_count = vertex_count;
      break;
    default:
      return;
    }

    if (0 == prim_count)
    {
      return;
    }

    unsigned texcoordcmp[ATOM_MAX_TEXTURE_LAYER_COUNT];
    unsigned numtexlayers = 0;

    for (unsigned i = 0; i < ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
    {
      if (attrib & ATOM_VERTEX_ATTRIB_TEX_MASK(i))
      {
        texcoordcmp[i] = ATOM_VERTEX_ATTRIB_TEX_COMPONENTS(attrib, i);
        ++numtexlayers;
        continue;
      }
      break;
    }

    unsigned lockSize = vertex_range * stride;
    if (!requireIntermediateVertexBuffer (lockSize))
	{
		return;
	}

    unsigned OffsetVertex;
    DWORD lockFlags;
    if (lockSize > _M_intermediate_vertexbuffer_size - _M_intermediate_vertexbuffer_pos)
    {
      OffsetVertex = 0;
      lockFlags = D3DLOCK_DISCARD;
    }
    else
    {
      OffsetVertex = _M_intermediate_vertexbuffer_pos;
      lockFlags = D3DLOCK_NOOVERWRITE;
    }
    _M_intermediate_vertexbuffer_pos = OffsetVertex + lockSize;

    HRESULT hr;

    char *lockedPtr = 0;
	hr = _M_intermediate_vertexbuffer->Lock (OffsetVertex, lockSize, (void**)&lockedPtr, lockFlags);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%d,%d,%d,%u,%u,0x%08X,0x%08X) Lock vertex buffer failed.\n", type, vertex_count, vertex_range, attrib, stride, stream, indices);
		ATOM_CHECK_D3D_RESULT(hr);
		return;
	}
    memcpy (lockedPtr, stream, lockSize);
    ATOM_CHECK_D3D_RESULT(_M_intermediate_vertexbuffer->Unlock ());

    IDirect3DVertexDeclaration9 *decl = 0;
    ATOM_HASHMAP<unsigned, IDirect3DVertexDeclaration9*>::iterator it = _M_immediately_vertex_decl.find (attrib);
    if (it == _M_immediately_vertex_decl.end())
    {
      decl = createImmediatelyVertexDeclaration (attrib, numtexlayers, texcoordcmp);
      ATOM_ASSERT (decl);
      _M_immediately_vertex_decl[attrib] = decl;
    }
    else
    {
      decl = it->second;
    }

	hr = _M_d3ddevice->SetStreamSource (0, _M_intermediate_vertexbuffer, OffsetVertex, stride);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%d, %d, %d, 0x%08X, %d, 0x%08X, 0x%08X) SetStreamSource() failed()\n", type, vertex_count, vertex_range, attrib, stride, stream, indices) ;
		ATOM_CHECK_D3D_RESULT(hr);
		return;
	}

    hr = _M_d3ddevice->SetVertexDeclaration(decl);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%d, %d, %d, 0x%08X, %d, 0x%08X, 0x%08X) SetVertexDeclaration() failed()\n", type, vertex_count, vertex_range, attrib, stride, stream, indices) ;
		ATOM_CHECK_D3D_RESULT(hr);
		return;
	}

    unsigned OffsetIndex;
    lockSize = vertex_count * sizeof(unsigned short);
    if (!requireIntermediateIndexBuffer (lockSize))
	{
		return;
	}

    if (lockSize > _M_intermediate_indexbuffer_size - _M_intermediate_indexbuffer_pos)
    {
      OffsetIndex = 0;
      lockFlags = D3DLOCK_DISCARD;
    }
    else
    {
      OffsetIndex = _M_intermediate_indexbuffer_pos;
      lockFlags = D3DLOCK_NOOVERWRITE;
    }
    _M_intermediate_indexbuffer_pos = OffsetIndex + lockSize;

	lockedPtr = 0;
    hr = _M_intermediate_indexbuffer->Lock (OffsetIndex, lockSize, (void**)&lockedPtr, lockFlags);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%d,%d,%d,%u,%u,0x%08X,0x%08X) Lock index buffer failed.\n", type, vertex_count, vertex_range, attrib, stride, stream, indices);
		ATOM_CHECK_D3D_RESULT(hr);
		return;
	}
    memcpy (lockedPtr, indices, lockSize);
    ATOM_CHECK_D3D_RESULT(_M_intermediate_indexbuffer->Unlock ());
    ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetIndices(_M_intermediate_indexbuffer));

    ATOM_ASSERT (ATOM_GFX_IS_VALID_PRIMITIVE_TYPE(type));
    D3DPRIMITIVETYPE primType = (D3DPRIMITIVETYPE)ATOM_GFX_MAP_TO_D3D_PRIMITIVE_TYPE(type);

	_stateCache->sync ();
    ATOM_CHECK_D3D_RESULT(_M_d3ddevice->DrawIndexedPrimitive(primType, 0, 0, vertex_range, OffsetIndex/sizeof(unsigned short), prim_count));

    _M_num_drawcalls++;
	_M_num_primitives += prim_count;
  }
}
//-- wangjian added for test ---//
void ATOM_D3D9Device::renderStreamsImmediateRESZ( ATOM_Texture * texture_resz )
{
	// Bind depth stencil texture to texture sampler 0 
	ATOM_D3DTexture * pDeviceTex = (ATOM_D3DTexture *)texture_resz;
	if( !pDeviceTex )
		return;

	_M_d3ddevice->SetTexture(0, pDeviceTex->getD3DTexture() ); 

	_M_d3ddevice->SetVertexShader(NULL);
	_M_d3ddevice->SetPixelShader(NULL);

	_M_d3ddevice->SetFVF(D3DFVF_XYZ);

	D3DXVECTOR3 vDummyPoint(0,0,0);
	_M_d3ddevice->SetRenderState(D3DRS_ZENABLE, FALSE); 
	_M_d3ddevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE); 
	_M_d3ddevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0); 
	_M_d3ddevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, vDummyPoint,  
		sizeof(D3DXVECTOR3)); 
	_M_d3ddevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); 
	_M_d3ddevice->SetRenderState(D3DRS_ZENABLE, TRUE); 
	_M_d3ddevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0F);

	#define RESZ_CODE 0x7fa05000
	_M_d3ddevice->SetRenderState(D3DRS_POINTSIZE, RESZ_CODE);

	// This hack to fix resz hack, has been found by Maksym Bezus!!!
	// Without this line resz will be resolved only for first frame
	_M_d3ddevice->SetRenderState(D3DRS_POINTSIZE, 0); // TROLOLO!!!
}

void ATOM_D3D9Device::applySlopeScaleBias( float fDepthBias/* = .0f*/, float fSlopeScaleBias/* = 0.0f*/ )
{
	_M_d3ddevice->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fDepthBias);
	_M_d3ddevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&fSlopeScaleBias);
	DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Depth);
}

void ATOM_D3D9Device::enableSRGBWrite(bool enable/* = false*/)
{
	_M_d3ddevice->SetRenderState (D3DRS_SRGBWRITEENABLE, enable);
	DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_SRGB);
}
//------------------------------//
ATOM_AUTOREF(ATOM_Shader) ATOM_D3D9Device::allocVertexShader (const char *name)
{
	ATOM_AUTOREF(ATOM_Shader) shader = ATOM_CreateObject(ATOM_VertexShaderD3D9::_classname(), name);
	if (shader)
	{
		shader->setRenderDevice (this);
	}
	return shader;
}

ATOM_AUTOREF(ATOM_Shader) ATOM_D3D9Device::allocPixelShader (const char *name)
{
	ATOM_AUTOREF(ATOM_Shader) shader = ATOM_CreateObject(ATOM_PixelShaderD3D9::_classname(), name);
	if (shader)
	{
		shader->setRenderDevice (this);
	}
	return shader;
}

ATOM_AUTOREF(ATOM_GPUProgram) ATOM_D3D9Device::allocGPUProgram (const char *name)
{
	ATOM_AUTOREF(ATOM_GPUProgramD3D9) program = ATOM_CreateObject(ATOM_GPUProgramD3D9::_classname(), name);
	if (program)
	{
		program->setDevice (this);
	}
	return program.get();
}

ATOM_AUTOREF(ATOM_Shader) ATOM_D3D9Device::lookupVertexShader (const char *name)
{
	return ATOM_LookupObject (ATOM_VertexShaderD3D9::_classname(), name);
}

ATOM_AUTOREF(ATOM_Shader) ATOM_D3D9Device::lookupPixelShader (const char *name)
{
	return ATOM_LookupObject (ATOM_PixelShaderD3D9::_classname(), name);
}

ATOM_AUTOREF(ATOM_GPUProgram) ATOM_D3D9Device::lookupGPUProgram (const char *name)
{
	return ATOM_LookupObject (ATOM_GPUProgramD3D9::_classname(), name);
}

IDirect3DVertexDeclaration9 *ATOM_D3D9Device::createImmediatelyVertexDeclaration (unsigned attrib, unsigned nlayers, const unsigned *cmps) const {
  ATOM_STACK_TRACE(ATOM_D3D9Device::createImmediatelyVertexDeclaration);

  if (_M_d3ddevice)
  {
    static const D3DVERTEXELEMENT9 ElementEnd = D3DDECL_END();
    D3DVERTEXELEMENT9 elements[16];
    D3DVERTEXELEMENT9 *pElement = elements;

	unsigned offset;

	if (attrib & ATOM_VERTEX_ATTRIB_COORD)
	{
	  ATOM_D3DVertexArray::getVertexElement(ATOM_VERTEX_ATTRIB_COORD, pElement++);
	  offset = 3 * sizeof(float);
	}
	else if (attrib & ATOM_VERTEX_ATTRIB_COORD_XYZW)
	{
	  ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_COORD_XYZW, pElement++);
	  offset = 4 * sizeof(float);
	}
	else if (attrib & ATOM_VERTEX_ATTRIB_COORD_XYZRHW)
	{
	  ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_COORD_XYZRHW, pElement++);
	  offset = 4 * sizeof(float);
	}
	else
	{
	  return 0;
	}

    if (attrib & ATOM_VERTEX_ATTRIB_NORMAL)
    {
      ATOM_D3DVertexArray::getVertexElement(ATOM_VERTEX_ATTRIB_NORMAL, pElement);
      pElement->Stream = 0;
      pElement->Offset = offset;
      offset += 3 * sizeof(float);
      pElement++;
    }

	if (attrib & ATOM_VERTEX_ATTRIB_PSIZE)
	{
	  ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_PSIZE, pElement);
	  pElement->Stream = 0;
	  pElement->Offset = offset;
	  offset += sizeof(float);
	  pElement++;
	}

    if (attrib & ATOM_VERTEX_ATTRIB_PRIMARY_COLOR)
    {
      ATOM_D3DVertexArray::getVertexElement(ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, pElement);
      pElement->Stream = 0;
      pElement->Offset = offset;
      offset += sizeof(unsigned);
      pElement++;
    }

	if (attrib & ATOM_VERTEX_ATTRIB_TANGENT)
	{
      ATOM_D3DVertexArray::getVertexElement(ATOM_VERTEX_ATTRIB_TANGENT, pElement);
      pElement->Stream = 0;
      pElement->Offset = offset;
      offset += 3 * sizeof(float);
      pElement++;
	}

	if (attrib & ATOM_VERTEX_ATTRIB_BINORMAL)
	{
      ATOM_D3DVertexArray::getVertexElement(ATOM_VERTEX_ATTRIB_BINORMAL, pElement);
      pElement->Stream = 0;
      pElement->Offset = offset;
      offset += 3 * sizeof(float);
      pElement++;
	}

    for (unsigned i = 0; i < nlayers; ++i)
    {
      ATOM_D3DVertexArray::getVertexElement((attrib & ATOM_VERTEX_ATTRIB_TEX_MASK(i)), pElement);
      pElement->Stream = 0;
      pElement->Offset = offset;
      offset += cmps[i] * sizeof(float);
      pElement++;
    }

    *pElement = ElementEnd;

    IDirect3DVertexDeclaration9 *decl = 0;
    ATOM_CHECK_D3D_RESULT(_M_d3ddevice->CreateVertexDeclaration (elements, &decl));

    return decl;
  }

  return 0;
}

void ATOM_D3D9Device::outputText (ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color, const char *text, ...)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::outputText);

  char buffer[1024];
  va_list args;
  va_start(args, text);
  vsnprintf(buffer, 1024, text, args);
  buffer[1023] = '\0';
  va_end(args);

  ATOM_Text t;
  t.setString (text);
  t.setZValue (1.f);
  t.setFont (font);
  t.setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
  t.render (x, y, color);
}

void ATOM_D3D9Device::outputTextOutline (ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color, ATOM_ColorARGB colorOutline, const char *text, ...)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::outputTextOutline);

  char buffer[1024];
  va_list args;
  va_start(args, text);
  vsnprintf(buffer, 1024, text, args);
  buffer[1023] = '\0';
  va_end(args);

  ATOM_Text t;
  t.setString (text);
  t.setZValue (1.f);
  t.setFont (font);
  t.setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
  t.renderOutline (x, y, color, colorOutline);
}

void ATOM_D3D9Device::outputTextShadow (ATOM_FontHandle font, int x, int y, int offsetx, int offsety, ATOM_ColorARGB color, ATOM_ColorARGB colorShadow, const char *text, ...)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::outputTextShadow);

  char buffer[1024];
  va_list args;
  va_start(args, text);
  vsnprintf(buffer, 1024, text, args);
  buffer[1023] = '\0';
  va_end(args);

  ATOM_Text t;
  t.setString (text);
  t.setZValue (1.f);
  t.setFont (font);
  t.setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
  t.renderShadow (x, y, offsetx, offsety, color, colorShadow);
}

void ATOM_D3D9Device::setConfigure (const char *entry, int value)
{
}

bool ATOM_D3D9Device::screenShot (ATOM_RenderWindow *view, ATOM_AUTOREF(ATOM_Image) pImage)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::screenShot);

  if (_M_d3ddevice)
  {
    if (!pImage)
	{
		return false;
	}

    ATOM_PixelFormat nPixelFmt = ATOM_PIXEL_FORMAT_BGRA8888;
    ATOM_PixelFormat nImageFmt = ATOM_PIXEL_FORMAT_BGRX8888;

	int w, h;
	if (!this->getBackbufferSize (&w, &h))
	{
		return false;
	}

	pImage->init (w, h, nImageFmt);
	return readColorBuffer (view, 0, 0, w, h, nPixelFmt, pImage->getData());
  }

  return false;
}

void ATOM_D3D9Device::_setScissorRect_Impl (const int *scissor)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::_setScissorRect_Impl);

  if (_M_d3ddevice)
  {
    RECT rect;
    rect.left   = scissor[0];
    rect.right  = scissor[0] + scissor[2];
    rect.top    = scissor[1];
    rect.bottom = scissor[1] + scissor[3];

	if (rect.left < 0) rect.left = 0;
	if (rect.top < 0) rect.top = 0;
	if (rect.right < rect.left) rect.right = rect.left;
	if (rect.bottom < rect.top) rect.bottom = rect.top;
	ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetScissorRect (&rect));
  }
}

void ATOM_D3D9Device::_setClearColor_Impl (const float *rgba) {
}

void ATOM_D3D9Device::_setClearDepth_Impl (float depth) {
}

void ATOM_D3D9Device::_setClearStencil_Impl (int stencil) {
}

void ATOM_D3D9Device::_clear_Impl(bool color, bool depth, bool stencil) {
  ATOM_STACK_TRACE(ATOM_D3D9Device::_clear_Impl);

  if (_M_d3ddevice && _M_current_view)
  {
    DWORD flags = 0;

    if (color)
    {
      flags |= D3DCLEAR_TARGET;
    }

    if (depth && _M_has_zbuffer)
    {
      flags |= D3DCLEAR_ZBUFFER;
    }

    if (stencil)
    {
      if (_M_has_stencilbuffer)
      {
        flags |= D3DCLEAR_STENCIL;
      }
    }

    ATOM_CHECK_D3D_RESULT(_M_d3ddevice->Clear(0, 0, flags, ATOM_D3DCOLOR_COLORVALUE(_M_current_view->clearcolor[0], _M_current_view->clearcolor[1], _M_current_view->clearcolor[2], _M_current_view->clearcolor[3]), _M_current_view->cleardepth, _M_current_view->clearstencil));
  }
}

void ATOM_D3D9Device::_setModelViewMatrix_Impl (const ATOM_Matrix4x4f *world, const ATOM_Matrix4x4f *view) {
  ATOM_STACK_TRACE(ATOM_D3D9Device::_setModelViewMatrix_Impl);

  if (_M_d3ddevice)
  {
    if (world)
    {
      ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetTransform (D3DTS_WORLD, (const D3DMATRIX*)world));
    }
    if (view)
    {
      ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetTransform (D3DTS_VIEW, (const D3DMATRIX*)view));
    }
  }
}

void ATOM_D3D9Device::_setProjectionMatrix_Impl (const ATOM_Matrix4x4f *proj) {
	ATOM_STACK_TRACE(ATOM_D3D9Device::_setProjectionMatrix_Impl);

	if (_M_d3ddevice)
	{
		ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetTransform (D3DTS_PROJECTION, (const D3DMATRIX*)proj));
	}
}

void ATOM_D3D9Device::_setTextureMatrix_Impl (int stage, const ATOM_Matrix4x4f *matrix) {
  ATOM_STACK_TRACE(ATOM_D3D9Device::_setTextureMatrix_Impl);

  if (_M_d3ddevice)
  {
    ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetTransform ((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + stage), (const D3DMATRIX*)matrix));
  }
}

void ATOM_D3D9Device::_setViewport_Impl (const int *xywh, float depthMin, float depthMax) {
	ATOM_STACK_TRACE(ATOM_D3D9Device::_setViewport_Impl);

	if (_M_d3ddevice)
	{
		bool adjust = _needAdjustProjectionMatrix;

		int backbufferW, backbufferH;

		//--- wangjian modified ---//
		// 在渲染阴影时，如果设备支持NULL RENDER TARGET,那么在设置生成阴影的RT时是使用setRenderTargetSurface直接设置NULL RT的surface，
		// 而不是使用setRenderTarget设置纹理的surface。生成阴影时 _setViewport_Impl 正好会在 setRenderTargetSurface 之后进行一次调用，
		// 此时如果getRenderTarget的话得到的将是一个空的RT，那么backbufferW和backbufferH 的值就会出现错误，在getRenderTarget函数中增加
		// 两个参数( int * rt_width , int * rt_height )用于在该情况下获得NULL RT的surface的大小。所以这里需要做相应修改。
		int rt_width = 0;
		int rt_height = 0;
		ATOM_Texture *texture = getRenderTarget ( 0, &rt_width, &rt_height );
		if (texture)
		{
			backbufferW = texture->getWidth ();
			backbufferH = texture->getHeight ();
		}
		else if( rt_width != 0 && rt_height != 0 )			// 得到NULL RT的surface大小
		{
			backbufferW = rt_width;						
			backbufferH = rt_height;
		}
		else
		{
			getBackbufferSize (&backbufferW, &backbufferH);
		}
		//-----------------------//

		int vx = xywh[0];
		int vy = xywh[1];
		int vw = xywh[2];
		int vh = xywh[3];

		if (vx < 0)
		{
			vw += vx;
			vx = 0;
			_needAdjustProjectionMatrix = true;
		}
		if (vy < 0)
		{
			vh += vy;
			vy = 0;
			_needAdjustProjectionMatrix = true;
		}
		if (vx + vw > backbufferW)
		{
			vw = backbufferW - vx;
			_needAdjustProjectionMatrix = true;
		}
		if (vy + vh > backbufferH)
		{
			vh = backbufferH - vy;
			_needAdjustProjectionMatrix = true;
		}

		D3DVIEWPORT9 vp;
		vp.X = vx;
		vp.Y = vy;
		vp.Width = vw;
		vp.Height = vh;
		vp.MinZ = depthMin;
		vp.MaxZ = depthMax;

		if (vp.Width>0 && vp.Height>0)
		{
			ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetViewport (&vp));
		}

		if (adjust)
		{
			_M_projection_matrix.m00 /= _viewportScaleX;
			_M_projection_matrix.m11 /= _viewportScaleY;
			_M_projection_matrix.m20 -= _viewportTranslateX;
			_M_projection_matrix.m21 -= _viewportTranslateY;
		}

		if (_needAdjustProjectionMatrix)
		{
			_viewportScaleX = float(xywh[2])/float(vw);
			_viewportScaleY = float(xywh[3])/float(vh);
			_viewportTranslateX = float(2*xywh[0]+xywh[2]-2*vx-vw)/float(vw);
			_viewportTranslateY = -float(2*xywh[1]+xywh[3]-2*vy-vh)/float(vh);
		}

		if (adjust || _needAdjustProjectionMatrix)
		{
			setTransform (ATOM_MATRIXMODE_PROJECTION, _M_projection_matrix);
		}
	}
}

unsigned ATOM_D3D9Device::getNumTextures (void) const
{
	return TextureCount;
}

void ATOM_D3D9Device::_present_Impl (void)
{
  if (_M_d3ddevice && _M_current_view)
  {
    _M_current_view->window->present ();
  }

  int numTargets = ATOM_min2 (getCapabilities().max_rendertarget_count, 4);
  for (unsigned i = 0; i < numTargets; ++i)
  {
	setRenderTarget (i, 0);
  }
  setDepthBuffer (0);
}

bool ATOM_D3D9Device::supportSRGBWrite (D3DFORMAT format) const
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::supportSRGBWrite);

  if (_M_d3ddevice)
  {
    ATOM_AUTOREF(ATOM_D3D9Window) window = _M_render_window;
    ATOM_ASSERT (window);

    HRESULT hr;
	hr = _M_d3d->CheckDeviceFormat(
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->adapter,
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->devtype,
	  ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->presentParams.BackBufferFormat,
      D3DUSAGE_QUERY_SRGBWRITE,
      D3DRTYPE_TEXTURE,
      format);

	if (SUCCEEDED(hr))
	{
		return true;
	}
  }

  return false;
}

bool ATOM_D3D9Device::isTextureFormatOk (D3DFORMAT format) const
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::isTextureFormatOk);

  if (_M_d3ddevice)
  {
    ATOM_AUTOREF(ATOM_D3D9Window) window = _M_render_window;
    ATOM_ASSERT (window);

    HRESULT hr;
	hr = _M_d3d->CheckDeviceFormat(
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->adapter,
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->devtype,
	  ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->presentParams.BackBufferFormat,
      0,
      D3DRTYPE_TEXTURE,
      format);

	if (FAILED(hr))
	{
		return false;
	}

    hr = _M_d3d->CheckDeviceFormat(
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->adapter,
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->devtype,
	  ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->presentParams.BackBufferFormat,
      0,
      D3DRTYPE_CUBETEXTURE,
      format);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
  }

  return false;
}

bool ATOM_D3D9Device::isRenderTargetFormatOk (D3DFORMAT format) const
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::isRenderTargetFormatOk);

  if (_M_d3ddevice)
  {
    ATOM_AUTOREF(ATOM_D3D9Window) window = _M_render_window;
    ATOM_ASSERT (window);

    HRESULT hr;
	hr = _M_d3d->CheckDeviceFormat(
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->adapter,
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->devtype,
	  ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->presentParams.BackBufferFormat,
      D3DUSAGE_RENDERTARGET,
      D3DRTYPE_TEXTURE,
      format);

	if (FAILED(hr))
	{
		return false;
	}

	hr = _M_d3d->CheckDeviceFormat(
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->adapter,
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->devtype,
	  ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->presentParams.BackBufferFormat,
      D3DUSAGE_RENDERTARGET,
      D3DRTYPE_CUBETEXTURE,
      format);

	if (FAILED(hr))
	{
		return false;
	}

    return true;
  }

  return false;
}

bool ATOM_D3D9Device::isDepthStencilTextureFormatOk (D3DFORMAT format) const
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::isDepthStencilTextureFormatOk);

  if (_M_d3ddevice)
  {
    ATOM_AUTOREF(ATOM_D3D9Window) window = _M_render_window;
    ATOM_ASSERT (window);

    HRESULT hr = _M_d3d->CheckDeviceFormat(
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->adapter,
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->devtype,
	  ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->presentParams.BackBufferFormat,
      D3DUSAGE_DEPTHSTENCIL,
      D3DRTYPE_TEXTURE,
      format);

    return SUCCEEDED(hr);
  }

  return false;
}

bool ATOM_D3D9Device::doesTextureFormatSupportSRGB (D3DFORMAT format) const
{
  if (_M_d3ddevice)
  {
    ATOM_AUTOREF(ATOM_D3D9Window) window = _M_render_window;
    ATOM_ASSERT (window);

    HRESULT hr = _M_d3d->CheckDeviceFormat(
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->adapter,
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->devtype,
	  ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->presentParams.BackBufferFormat,
      D3DUSAGE_QUERY_SRGBREAD,
      D3DRTYPE_TEXTURE,
      format);

    return SUCCEEDED(hr);
  }

  return false;
}

bool ATOM_D3D9Device::isDepthStencilFormatOk (D3DFORMAT format) const
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::isDepthStencilFormatOk);

  if (_M_d3ddevice)
  {
    ATOM_AUTOREF(ATOM_D3D9Window) window = _M_render_window;
    ATOM_ASSERT (window);

    HRESULT hr = _M_d3d->CheckDeviceFormat(
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->adapter,
      ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->devtype,
	  ((ATOM_D3D9WindowInfo*)window->getWindowInfo())->presentParams.BackBufferFormat,
      D3DUSAGE_DEPTHSTENCIL,
      D3DRTYPE_SURFACE,
      format);

    return SUCCEEDED(hr);
  }

  return false;
}

bool ATOM_D3D9Device::beginFrame (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::beginFrame);

  if (_M_d3ddevice)
  {
    if (!testResetDevice())
    {
      return false;
    }

	++_M_beginframe_counter;

	if (_M_beginframe_counter > 1)
	{
	  return true;
	}

	++_M_frame_stamp;

	DBG_CLEAR_STATE_CHANGE(_stateChangeInfo);

    if (!ATOM_RenderDevice::beginFrame ())
    {
	  --_M_beginframe_counter;
      return false;
    }

    HRESULT hr = _M_d3ddevice->BeginScene();

    if (FAILED(hr))
    {
		ATOM_LOGGER::error ("%s() BeginScene() failed.\n", __FUNCTION__);
		ATOM_CHECK_D3D_RESULT(hr);
		--_M_beginframe_counter;
		return false;
    }

	_M_num_drawcalls = 0;
	_M_num_primitives = 0;

    return true;
  }

  return false;
}

void ATOM_D3D9Device::endFrame (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::endFrame);

  if (_M_beginframe_counter == 0)
  {
	  return;
  }

  --_M_beginframe_counter;

  if (_M_beginframe_counter > 0)
  {
	  return;
  }

  if (_M_d3ddevice)
  {
    ATOM_RenderDevice::endFrame();

	for (unsigned i = 0; i < 16; ++i)
	{
		setStreamSource (i, 0, 0);
	}

    ATOM_CHECK_D3D_RESULT(_M_d3ddevice->EndScene());
    //printf ("%d triangles rendered\n", _M_primitive_rendered);
  }
}

unsigned ATOM_D3D9Device::getNumDrawCalls (void) const
{
	return _M_num_drawcalls;
}

unsigned ATOM_D3D9Device::getNumPrimitivesDrawn (void) const
{
	return _M_num_primitives;
}

void ATOM_D3D9Device::invalidateAllResources (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::invalidateAllResources);

  if (_M_default_backbuffer)
  {
	  _M_default_backbuffer->Release ();
	  _M_default_backbuffer = 0;
  }

  if (_M_default_depthbuffer)
  {
	  _M_default_depthbuffer->Release ();
	  _M_default_depthbuffer = 0;
  }

  if (_M_texture_manager)
  {
    ATOM_ID3DTextureManager *tm = (ATOM_ID3DTextureManager*)_M_texture_manager;
    tm->Invalidate();
  }

  invalidateResources ();

  ATOM_AUTOREF(ATOM_D3D9Window) win = _M_render_window;

  if (win)
  {
    ATOM_D3D9WindowInfo *info = (ATOM_D3D9WindowInfo*)win->getWindowInfo();

    if (info && _M_d3d)
    {
      D3DFORMAT dispFormat;
      D3DFORMAT backFormat;
      D3DFORMAT zbufFormat;
      FindBufferFormats (
        _M_d3d,
        info->adapter,
        info->devtype,
        dispFormat,
        backFormat,
        zbufFormat,
        info->bpp,
        &info->depth,
        &info->stencil
      );

      if (backFormat == D3DFMT_UNKNOWN || zbufFormat == D3DFMT_UNKNOWN)
      {
        return;
      }

      info->presentParams.BackBufferFormat = backFormat;
      info->presentParams.AutoDepthStencilFormat = zbufFormat;
    }
  }
}

void ATOM_D3D9Device::restoreAllResources (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::restoreAllResources);

  ATOM_CHECK_D3D_RESULT(_M_d3ddevice->GetRenderTarget (0, &_M_default_backbuffer));
  ATOM_CHECK_D3D_RESULT(_M_d3ddevice->GetDepthStencilSurface (&_M_default_depthbuffer));
  getBackbufferDimension (&_backbufferW, &_backbufferH);

  restoreResources ();

  setInitialStates ();
  syncRenderStates ();
  //_M_state_cache->syncRenderStates ();

  for (unsigned i = 0; i < _M_views.size(); ++i)
  {
    ATOM_AUTOREF(ATOM_RenderWindow) w = _M_views[i]->window;
    ATOM_CursorInfo *ci = &_M_views[i]->cursor_info;
    if (ci->image)
    {
      setCustomCursor (w.get(), ci->image.get(), ci->hotspot[0], ci->hotspot[1], &ci->region[0]);
    }
	else if (ci->hcursor)
	{
		setCustomCursor (w.get(), (HCURSOR)ci->hcursor);
	}
  }

  if (_M_cursor_window)
  {
    setViewCursor (_M_cursor_window.get());
  }
}

bool ATOM_D3D9Device::resizeDevice (unsigned w, unsigned h)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::resizeDevice);

	if (w == 0 || h == 0)
	{
		return false;
	}

	int width, height;
	getBackbufferSize (&width, &height);
	if (w != width || h != height)
	{
		ATOM_AUTOREF(ATOM_D3D9Window) win = _M_render_window;
		ATOM_ASSERT(win);
		ATOM_D3D9WindowInfo *info = (ATOM_D3D9WindowInfo*)win->getWindowInfo();
		info->presentParams.BackBufferWidth = w;
		info->presentParams.BackBufferHeight = h;
		ATOM_DeviceLostEvent lostEvent;
		ATOM_APP->handleEvent (&lostEvent);
		invalidateAllResources ();
		for (unsigned i = 0; i < _M_views.size(); ++i)
		{
			_M_views[i]->window->detachDevice();
		}
		ATOM_CHECK_D3D_RESULT(_M_d3ddevice->Reset (&info->presentParams));
		restoreAllResources ();
		ATOM_DeviceResetEvent resetEvent(&info->presentParams);
		ATOM_APP->handleEvent (&resetEvent);
		for (unsigned i = 0; i < _M_views.size(); ++i)
		{
			ATOM_RenderView *w = _M_views[i];
			if (w->window->attachDevice(this) && w == _M_current_view)
			{
				w->window->setCurrent ();
			}
		}
	}

	return true;
}

bool ATOM_D3D9Device::testResetDevice (void)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::testResetDevice);

	_M_device_ready = false;

	if (_M_d3ddevice)
	{
		HRESULT hr = _M_d3ddevice->TestCooperativeLevel();
		if (SUCCEEDED(hr))
		{
			_M_device_ready = true;
		}
		else if (D3DERR_DEVICENOTRESET == hr)
		{
			ATOM_DeviceLostEvent lostEvent;
			ATOM_APP->handleEvent (&lostEvent);

			invalidateAllResources ();

			ATOM_AUTOREF(ATOM_D3D9Window) win = _M_render_window;
			ATOM_ASSERT(win);
			ATOM_D3D9WindowInfo *info = (ATOM_D3D9WindowInfo*)win->getWindowInfo();

			for (unsigned i = 0; i < _M_views.size(); ++i)
			{
				_M_views[i]->window->detachDevice();
			}

			D3DDISPLAYMODE desktopMode;
			_M_d3d->GetAdapterDisplayMode(info->adapter, &desktopMode);
			info->presentParams.BackBufferWidth = win->getWindowHints().bounds.first;
			info->presentParams.BackBufferHeight = win->getWindowHints().bounds.second;
			info->presentParams.BackBufferFormat = desktopMode.Format;

			hr = _M_d3ddevice->Reset (&info->presentParams);
			if (FAILED(hr))
			{
				ATOM_LOGGER::error ("%s() Reset() failed.\n", __FUNCTION__);
				ATOM_CHECK_D3D_RESULT(hr);
				_M_device_ready = false;
			}
			else
			{
				_M_device_ready = true;

				restoreAllResources ();
				setupCaps ();

				ATOM_DeviceResetEvent resetEvent(&info->presentParams);
				ATOM_APP->handleEvent (&resetEvent);

				for (unsigned i = 0; i < _M_views.size(); ++i)
				{
					ATOM_RenderView *w = _M_views[i];
					if (w->window->attachDevice(this) && w == _M_current_view)
					{
						w->window->setCurrent ();
					}
				}
			}
		}
		else
		{
			_M_device_ready = false;
		}
	}

	return _M_device_ready;
}

void ATOM_D3D9Device::invalidateResources (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::invalidateResources);

  ATOM_HASHMAP<unsigned, IDirect3DVertexDeclaration9*>::iterator it;
  for (it = _M_immediately_vertex_decl.begin(); it != _M_immediately_vertex_decl.end(); ++it)
  {
    it->second->Release();
    it->second = 0;
  }
  _M_immediately_vertex_decl.clear();

  for (unsigned i = 0; i < _M_occ_queries.size(); ++i)
  {
    if (_M_occ_queries[i])
    {
      _M_occ_queries[i]->Release();
      _M_occ_queries[i] = 0;
    }
  }

  if (_M_intermediate_vertexbuffer)
  {
    _M_intermediate_vertexbuffer->Release();
    _M_intermediate_vertexbuffer = 0;
    _M_intermediate_vertexbuffer_size = 0;
    _M_intermediate_vertexbuffer_pos = 0;
  }

  if (_M_intermediate_indexbuffer)
  {
    _M_intermediate_indexbuffer->Release();
    _M_intermediate_indexbuffer = 0;
    _M_intermediate_indexbuffer_size = 0;
    _M_intermediate_indexbuffer_pos = 0;
  }

  _M_current_decl = 0;

  ATOM_GfxResource *p = _M_resource_list;

  if (p)
  {
    for (;;)
    {
      if (!p->needRestore())
      {
        p->invalidate (true);
        p->setNeedReset(true);
      }

      p = p->_M_next;
      if (p == _M_resource_list)
      {
        break;
      }
    }
  }
}

void ATOM_D3D9Device::restoreResources (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::restoreResources);

  ATOM_GfxResource *p = _M_resource_list;

  if (p)
  {
    for (;;)
    {
      if (p->needReset())
      {
        p->restore ();
        p->setNeedReset(false);
      }

      p = p->_M_next;
      if (p == _M_resource_list)
      {
        break;
      }
    }
  }

  for (unsigned i = 0; i < _M_occ_queries.size(); ++i)
  {
    ATOM_CHECK_D3D_RESULT(_M_d3ddevice->CreateQuery (D3DQUERYTYPE_OCCLUSION, &_M_occ_queries[i]));
  }

  ATOM_CursorInfo *ci = getCurrentCursorInfo ();
  if (_M_enable_hardware_cursor && ci && ci->image)
  {
    IDirect3DSurface9 *surface = imageToCursorSurface (ci->image.get(), 0);
    if (surface)
    {
      ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetCursorProperties (ci->hotspot[0], ci->hotspot[1], surface));
      surface->Release ();
    }
  }
}

unsigned ATOM_D3D9Device::allocOcclusionQuery (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::allocOcclusionQuery);

  if (_M_d3ddevice)
  {
    unsigned index = 0;

    for (; index < _M_occ_queries.size(); ++index)
    {
      if (!_M_occ_queries[index])
      {
        break;
      }
    }

    if (index == _M_occ_queries.size())
    {
      _M_occ_queries.push_back (0);
    }

    HRESULT hr = _M_d3ddevice->CreateQuery (D3DQUERYTYPE_OCCLUSION, &_M_occ_queries[index]);
    if (FAILED(hr))
    {
		ATOM_LOGGER::error ("%s() CreateQuery() failed.\n", __FUNCTION__);
		ATOM_CHECK_D3D_RESULT(hr);
		return 0;
    }
    ATOM_ASSERT (_M_occ_queries[index]);

    return index + 1;
  }

  return 0;
}

void ATOM_D3D9Device::freeOcclusionQuery (unsigned query)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::freeOcclusionQuery);

  if (query > 0 && query <= _M_occ_queries.size() && _M_occ_queries[query-1])
  {
    _M_occ_queries[query-1]->Release();
    _M_occ_queries[query-1] = 0;
  }

  if (_M_current_query == query)
  {
    _M_current_query = 0;
  }
}

bool ATOM_D3D9Device::beginOcclusionQuery (unsigned query)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::beginOcclusionQuery);

  if (_M_d3ddevice)
  {
    if (_M_current_query)
    {
      return false;
    }

    if (query == 0 || query > _M_occ_queries.size() || !_M_occ_queries[query-1])
    {
      return false;
    }

    IDirect3DQuery9 *q = _M_occ_queries[query-1];
    ATOM_ASSERT (q);
    HRESULT hr = q->Issue (D3DISSUE_BEGIN);
    if (FAILED(hr))
    {
		ATOM_LOGGER::error ("%s(%d) Issue() failed.\n", __FUNCTION__, query);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
    }

    _M_current_query = query;

    return true;
  }

  return false;
}

void ATOM_D3D9Device::endOcclusionQuery (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::endOcclusionQuery);

  if (_M_current_query)
  {
    IDirect3DQuery9 *q = _M_occ_queries[_M_current_query-1];
    ATOM_ASSERT (q);
    q->Issue (D3DISSUE_END);
    _M_current_query = 0;
  }
}

unsigned ATOM_D3D9Device::getOcclusionQueryResult (unsigned query)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::getOcclusionQueryResult);

  if (query == 0 || query > _M_occ_queries.size() || !_M_occ_queries[query-1])
  {
    return 0;
  }

  DWORD numPixelsDrawn = 0;

  while (S_FALSE == _M_occ_queries[query-1]->GetData (&numPixelsDrawn, sizeof(DWORD), D3DGETDATA_FLUSH))
    ;

  return numPixelsDrawn;
}

bool ATOM_D3D9Device::setCustomCursor (ATOM_RenderWindow *view, HCURSOR hCursor)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::setCustomCursor);

	if (!_M_d3ddevice)
	{
		return false;
	}

    if (!_M_enable_hardware_cursor)
    {
		return ATOM_RenderDevice::setCustomCursor(view, hCursor);
    }

    ATOM_CursorInfo *ci = 0;
	if (view == 0)
	{
		view = getCurrentView();
	}

    for (unsigned i = 0; i < _M_views.size(); ++i)
    {
      if (_M_views[i]->window.get() == view)
      {
        ci = &_M_views[i]->cursor_info;
        break;
      }
    }

    if (!ci)
    {
      return false;
    }

	if (!isActive ())
	{
		ci->hcursor = hCursor;
		return true;
	}

    HRESULT hr = E_FAIL;
    ICONINFO iconinfo;
    BOOL bBWCursor;
    LPDIRECT3DSURFACE9 pCursorSurface = NULL;
    HDC hdcColor = NULL;
    HDC hdcMask = NULL;
    HDC hdcScreen = NULL;
    BITMAP bm;
    DWORD dwWidth;
    DWORD dwHeightSrc;
    DWORD dwHeightDest;
    COLORREF crColor;
    COLORREF crMask;
    UINT x;
    UINT y;
    BITMAPINFO bmi;
    COLORREF* pcrArrayColor = NULL;
    COLORREF* pcrArrayMask = NULL;
    DWORD* pBitmap;
    HGDIOBJ hgdiobjOld;

    ZeroMemory( &iconinfo, sizeof(iconinfo) );
    if( !GetIconInfo( hCursor, &iconinfo ) )
        goto End;

    if (0 == GetObject((HGDIOBJ)iconinfo.hbmMask, sizeof(BITMAP), (LPVOID)&bm))
        goto End;
    dwWidth = bm.bmWidth;
    dwHeightSrc = bm.bmHeight;

    if( iconinfo.hbmColor == NULL )
    {
        bBWCursor = TRUE;
        dwHeightDest = dwHeightSrc / 2;
    }
    else
    {
        bBWCursor = FALSE;
        dwHeightDest = dwHeightSrc;
    }

    // Create a surface for the fullscreen cursor
    if( FAILED( hr = _M_d3ddevice->CreateOffscreenPlainSurface( dwWidth, dwHeightDest,
        D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pCursorSurface, NULL ) ) )
    {
        goto End;
    }

    pcrArrayMask = ATOM_NEW_ARRAY( DWORD, dwWidth * dwHeightSrc);

    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = dwWidth;
    bmi.bmiHeader.biHeight = dwHeightSrc;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hdcScreen = GetDC( NULL );
    hdcMask = CreateCompatibleDC( hdcScreen );
    if( hdcMask == NULL )
    {
        hr = E_FAIL;
        goto End;
    }
    hgdiobjOld = SelectObject(hdcMask, iconinfo.hbmMask);
    GetDIBits(hdcMask, iconinfo.hbmMask, 0, dwHeightSrc,
        pcrArrayMask, &bmi, DIB_RGB_COLORS);
    SelectObject(hdcMask, hgdiobjOld);

    if (!bBWCursor)
    {
        pcrArrayColor = ATOM_NEW_ARRAY (DWORD, dwWidth * dwHeightDest);
        hdcColor = CreateCompatibleDC( hdcScreen );
        if( hdcColor == NULL )
        {
            hr = E_FAIL;
            goto End;
        }
        SelectObject(hdcColor, iconinfo.hbmColor);
        GetDIBits(hdcColor, iconinfo.hbmColor, 0, dwHeightDest,
            pcrArrayColor, &bmi, DIB_RGB_COLORS);
    }

    // Transfer cursor image into the surface
    D3DLOCKED_RECT lr;
    if (FAILED(hr = pCursorSurface->LockRect( &lr, NULL, 0 )))
	{
		goto End;
	}

    pBitmap = (DWORD*)lr.pBits;
    for( y = 0; y < dwHeightDest; y++ )
    {
        for( x = 0; x < dwWidth; x++ )
        {
            if (bBWCursor)
            {
                crColor = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
                crMask = pcrArrayMask[dwWidth*(dwHeightSrc-1-y) + x];
            }
            else
            {
                crColor = pcrArrayColor[dwWidth*(dwHeightDest-1-y) + x];
                crMask = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
            }
            if (crMask == 0)
                pBitmap[dwWidth*y + x] = 0xff000000 | crColor;
            else
                pBitmap[dwWidth*y + x] = 0x00000000;

#if 0
            // It may be helpful to make the D3D cursor look slightly
            // different from the Windows cursor so you can distinguish
            // between the two when developing/testing code.  When
            // bAddWatermark is TRUE, the following code adds some
            // small grey "D3D" characters to the upper-left corner of
            // the D3D cursor image.
            if( bAddWatermark && x < 12 && y < 5 )
            {
                // 11.. 11.. 11.. .... CCC0
                // 1.1. ..1. 1.1. .... A2A0
                // 1.1. .1.. 1.1. .... A4A0
                // 1.1. ..1. 1.1. .... A2A0
                // 11.. 11.. 11.. .... CCC0

                const WORD wMask[5] = { 0xccc0, 0xa2a0, 0xa4a0, 0xa2a0, 0xccc0 };
                if( wMask[y] & (1 << (15 - x)) )
                {
                    pBitmap[dwWidth*y + x] |= 0xff808080;
                }
            }
#endif
        }
    }
    pCursorSurface->UnlockRect();

    // Set the device cursor
    if( FAILED( hr = _M_d3ddevice->SetCursorProperties( iconinfo.xHotspot,
        iconinfo.yHotspot, pCursorSurface ) ) )
    {
        goto End;
    }

    hr = S_OK;

End:
    if( iconinfo.hbmMask != NULL )
        DeleteObject( iconinfo.hbmMask );
    if( iconinfo.hbmColor != NULL )
        DeleteObject( iconinfo.hbmColor );
    if( hdcScreen != NULL )
        ReleaseDC( NULL, hdcScreen );
    if( hdcColor != NULL )
        DeleteDC( hdcColor );
    if( hdcMask != NULL )
        DeleteDC( hdcMask );

    ATOM_DELETE_ARRAY( pcrArrayColor );
    ATOM_DELETE_ARRAY( pcrArrayMask );

	if (pCursorSurface)
	{
		pCursorSurface->Release();
	}

	if (SUCCEEDED(hr))
	{
		ci->hcursor = hCursor;
	}
	else
	{
		ci->hcursor = 0;
	}

    return ATOM_CHECK_D3D_RESULT(hr);
}

bool ATOM_D3D9Device::setCustomCursor (ATOM_RenderWindow *view, ATOM_Texture *pTex, int hotspot_x, int hotspot_y, const int *region)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::setCustomCursor);

  if (_M_d3ddevice)
  {
    if (_M_enable_hardware_cursor)
    {
      if (pTex)
      {
        ATOM_CursorInfo *ci = 0;
		if (!view)
		{
			view = getCurrentView();
		}

        for (unsigned i = 0; i < _M_views.size(); ++i)
        {
          if (_M_views[i]->window.get() == view)
          {
            ci = &_M_views[i]->cursor_info;
            break;
          }
        }

        if (!ci)
        {
          return false;
        }

        IDirect3DSurface9 *surface = textureToCursorSurface (pTex, region, ci->image);
        if (surface)
        {
			if (isActive())
			{
				unsigned w = region ? region[2] : pTex->getWidth ();
				unsigned h = region ? region[3] : pTex->getHeight ();
				if (w != 32)
				{
					hotspot_x = 32 * (float(hotspot_x)/float(w));
				}
				if (h != 32)
				{
					hotspot_y = 32 * (float(hotspot_y)/float(h));
				}
				ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetCursorProperties (hotspot_x, hotspot_y, surface));
			}
			surface->Release();
        }

        showCustomCursor (view, ci->show);

        ci->hotspot[0] = hotspot_x;
        ci->hotspot[1] = hotspot_y;
		ci->region[0] = 0;
		ci->region[1] = 0;
		ci->region[2] = 32;
		ci->region[3] = 32;
		ci->hcursor = 0;

        return true;
      }
      else
      {
        setDefaultCursor (view);

        return true;
      }
    }
    else
    {
      return ATOM_RenderDevice::setCustomCursor(view, pTex, hotspot_x, hotspot_y, region);
    }
  }

  return false;
}

bool ATOM_D3D9Device::setCustomCursor (ATOM_RenderWindow *view, ATOM_Image *image, int hotspot_x, int hotspot_y, const int *region)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::setCustomCursor);

  if (_M_d3ddevice)
  {
    if (image)
    {
      if (_M_enable_hardware_cursor)
      {
        ATOM_CursorInfo *ci = 0;
		if (!view)
		{
			view = getCurrentView();
		}

        for (unsigned i = 0; i < _M_views.size(); ++i)
        {
          if (_M_views[i]->window.get() == view)
          {
            ci = &_M_views[i]->cursor_info;
            break;
          }
        }

        if (!ci)
        {
          return false;
        }

        ci->image = image;

		if (region)
		{
			ci->region[0] = region[0];
			ci->region[1] = region[1];
			ci->region[2] = region[2];
			ci->region[3] = region[3];
		}
		else
		{
			ci->region[0] = 0;
			ci->region[1] = 0;
			ci->region[2] = image->getWidth();
			ci->region[3] = image->getHeight();
		}

		if (isActive ())
		{
			IDirect3DSurface9 *surface = imageToCursorSurface (image, ci->region);

			if (surface)
			{
			  ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetCursorProperties (hotspot_x * 32 / ci->region[2], hotspot_y * 32 / ci->region[3], surface));
			  surface->Release ();
			}
		}
		
        showCustomCursor (view, ci->show);

        ci->hotspot[0] = hotspot_x;
        ci->hotspot[1] = hotspot_y;
		ci->hcursor = 0;

        return true;
      }
    }
    else
    {
      setDefaultCursor (view);

      return true;
    }
  }

  return false;
}

void ATOM_D3D9Device::setViewCursor (ATOM_RenderWindow *view)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::setViewCursor);

  for (unsigned i = 0; i < _M_views.size(); ++i)
  {
    if (_M_views[i]->window.get() == view)
    {
      ATOM_CursorInfo *ci = &_M_views[i]->cursor_info;
	  if (ci->hcursor)
	  {
		setCustomCursor (view, (HCURSOR)ci->hcursor);
	  }
	  else
	  {
		setCustomCursor(view, ci->image.get(), ci->hotspot[0], ci->hotspot[1], ci->region);
	  }
      _M_cursor_window = view;
      return;
    }
  }
}

bool ATOM_D3D9Device::showCustomCursor (ATOM_RenderWindow *view, bool show)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::showCustomCursor);

  ATOM_CursorInfo *ci = 0;
  for (unsigned i = 0; i < _M_views.size(); ++i)
  {
    if (_M_views[i]->window.get() == view)
    {
      ci = &_M_views[i]->cursor_info;
      break;
    }
  }

  if (ci && _M_d3ddevice && _M_enable_hardware_cursor)
  {
    bool ret = ci->show;
    ci->show = show;

    if (_M_cursor_window.get() == view && isActive())
    {
      ATOM_CHECK_D3D_RESULT(_M_d3ddevice->ShowCursor(show ? TRUE : FALSE));
    }

    return ret;
  }

  return false;
}

void ATOM_D3D9Device::setCursorPosition (int x, int y)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::setCursorPosition);

  if (_M_d3ddevice && _M_enable_hardware_cursor)
  {
    _M_d3ddevice->SetCursorPosition (x, y, 0);
  }
}

void ATOM_D3D9Device::_enableHardwareCursor_Impl (bool enable)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::_enableHardwareCursor_Impl);

  ATOM_CursorInfo *ci = 0;
  for (unsigned i = 0; i < _M_views.size(); ++i)
  {
    if (_M_views[i]->window == _M_cursor_window)
    {
      ci = &_M_views[i]->cursor_info;
      break;
    }
  }

  if (ci)
  {
    if (enable)
    {
		if (ci->hcursor)
		{
			setCustomCursor (_M_cursor_window.get(), (HCURSOR)ci->hcursor);
		}
		else
		{
			setCustomCursor (_M_cursor_window.get(), ci->image.get(), ci->hotspot[0], ci->hotspot[1], (ci->region[2] == 0) ? 0 : ci->region);
		}
    }
  }
}

void ATOM_D3D9Device::evictManagedResource (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::evictManagedResource);

  if (_M_d3ddevice)
  {
    ATOM_CHECK_D3D_RESULT(_M_d3ddevice->EvictManagedResources ());
  }
}

bool ATOM_D3D9Device::initialize (ATOM_RenderWindow *window, int multisample)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::initialize);

  ATOM_D3D9Window *d3dwin = (ATOM_D3D9Window *)window;

  if (d3dwin)
  {
    invalidateAllResources ();

    if (d3dwin->initDevice (this, multisample))
    {
      setD3DDevice (((ATOM_D3D9WindowInfo*)d3dwin->getWindowInfo())->device);
      setD3D (((ATOM_D3D9WindowInfo*)d3dwin->getWindowInfo())->d3d9);
      _M_render_window = window;

      if (!isTextureFormatOk (D3DFMT_A8R8G8B8) ||
          !isTextureFormatOk (D3DFMT_X8R8G8B8) ||
          !isRenderTargetFormatOk (D3DFMT_X8R8G8B8))
      {
        return false;
      }

	  setupCaps ();

	  _M_best_HDR_format = ATOM_PIXEL_FORMAT_UNKNOWN;
	  _M_best_HDR_format_rg = ATOM_PIXEL_FORMAT_UNKNOWN;
	  _M_best_HDR_format_r = ATOM_PIXEL_FORMAT_UNKNOWN;

	  D3DFORMAT hdrFormat = findBestHDRFormat (&_M_HDR_format_support_filtering, D3DFMT_A16B16G16R16F, D3DFMT_A32B32G32R32F);
	  switch (hdrFormat)
	  {
	  case D3DFMT_A16B16G16R16F:
		  _M_best_HDR_format = ATOM_PIXEL_FORMAT_RGBA16F;
		  break;
	  case D3DFMT_A32B32G32R32F:
		  _M_best_HDR_format = ATOM_PIXEL_FORMAT_RGBA32F;
		  break;
	  }

	  D3DFORMAT hdrFormatRG = findBestHDRFormat (&_M_HDR_format_rg_support_filtering, D3DFMT_G16R16F, D3DFMT_G32R32F);
	  switch (hdrFormatRG)
	  {
	  case D3DFMT_G16R16F:
		  _M_best_HDR_format_rg = ATOM_PIXEL_FORMAT_RG16F;
		  break;
	  case D3DFMT_G32R32F:
		  _M_best_HDR_format_rg = ATOM_PIXEL_FORMAT_RG32F;
		  break;
	  }

	  D3DFORMAT hdrFormatR = findBestHDRFormat (&_M_HDR_format_r_support_filtering, D3DFMT_R16F, D3DFMT_R32F);
	  switch (hdrFormatR)
	  {
	  case D3DFMT_R16F:
		  _M_best_HDR_format_rg = ATOM_PIXEL_FORMAT_R16F;
		  break;
	  case D3DFMT_R32F:
		  _M_best_HDR_format_rg = ATOM_PIXEL_FORMAT_R32F;
		  break;
	  }

      restoreAllResources ();


      return true;
    }
  }

  return false;
}

static bool supportFiltering (IDirect3D9 *d3d, UINT adapter, D3DDEVTYPE devtype, D3DFORMAT adapterFormat, D3DFORMAT format)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::supportFiltering);

	return SUCCEEDED (d3d->CheckDeviceFormat(adapter, devtype, adapterFormat, D3DUSAGE_QUERY_FILTER | D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, format));
}

static bool supportPostPixelShaderBlending (IDirect3D9 *d3d, UINT adapter, D3DDEVTYPE devtype, D3DFORMAT adapterFormat, D3DFORMAT format)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::supportPostPixelShaderBlending);

	return SUCCEEDED (d3d->CheckDeviceFormat(adapter, devtype, adapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING | D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, format));
}

D3DFORMAT toD3DFormat (int format)
{
	switch (format)
	{
	case ATOM_PIXEL_FORMAT_RGBA8888:
		return D3DFMT_A8B8G8R8;
	case ATOM_PIXEL_FORMAT_RGBX8888:
		return D3DFMT_X8B8G8R8;
	case ATOM_PIXEL_FORMAT_BGRA8888:
		return D3DFMT_A8R8G8B8;
	case ATOM_PIXEL_FORMAT_BGRX8888:
		return D3DFMT_X8R8G8B8;
	case ATOM_PIXEL_FORMAT_BGR888:
		return D3DFMT_R8G8B8;
	case ATOM_PIXEL_FORMAT_BGRA4444:
		return D3DFMT_A4R4G4B4;
	case ATOM_PIXEL_FORMAT_BGRX4444:
		return D3DFMT_X4R4G4B4;
	case ATOM_PIXEL_FORMAT_BGR565:
		return D3DFMT_R5G6B5;
	case ATOM_PIXEL_FORMAT_A8:
		return D3DFMT_A8;
	case ATOM_PIXEL_FORMAT_GREY8:
		return D3DFMT_L8;
	case ATOM_PIXEL_FORMAT_GREY8A8:
		return D3DFMT_A8L8;
	case ATOM_PIXEL_FORMAT_RGBA16:
		return D3DFMT_A16B16G16R16;
	case ATOM_PIXEL_FORMAT_RGBA32F:
		return D3DFMT_A32B32G32R32F;
	case ATOM_PIXEL_FORMAT_R32F:
		return D3DFMT_R32F;
	case ATOM_PIXEL_FORMAT_RG32F:
		return D3DFMT_G32R32F;
	case ATOM_PIXEL_FORMAT_RGBA16F:
		return D3DFMT_A16B16G16R16F;
	case ATOM_PIXEL_FORMAT_R16F:
		return D3DFMT_R16F;
	case ATOM_PIXEL_FORMAT_RG16F:
		return D3DFMT_G16R16F;
	case ATOM_PIXEL_FORMAT_DXT1:
		return D3DFMT_DXT1;
	case ATOM_PIXEL_FORMAT_DXT3:
		return D3DFMT_DXT3;
	case ATOM_PIXEL_FORMAT_DXT5:
		return D3DFMT_DXT5;

	default:
		return D3DFMT_UNKNOWN;
	}
}

bool ATOM_D3D9Device::supportRenderTarget (ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::supportRenderTarget);

	D3DFORMAT d3dFmt = toD3DFormat (format);
	if (d3dFmt != D3DFMT_UNKNOWN)
	{
		return isRenderTargetFormatOk (d3dFmt);
	}
	return false;
}

bool ATOM_D3D9Device::supportRenderTargetPostPixelShaderBlending (ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::supportRenderTargetPostPixelShaderBlending);

	D3DFORMAT d3dFmt = toD3DFormat (format);
	if (d3dFmt != D3DFMT_UNKNOWN)
	{
		ATOM_D3D9Window *d3dwin = (ATOM_D3D9Window *)_M_render_window.get();
		ATOM_D3D9WindowInfo* info = (ATOM_D3D9WindowInfo*)d3dwin->getWindowInfo();
		UINT adapter = info->adapter;
		D3DDEVTYPE devtype = info->devtype;
		D3DFORMAT adpterformat = info->presentParams.BackBufferFormat;
		return supportPostPixelShaderBlending (_M_d3d, adapter, devtype, adpterformat, d3dFmt);
	}
	return false;
}

bool ATOM_D3D9Device::supportRenderTargetFiltering (ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::supportRenderTargetFiltering);

	D3DFORMAT d3dFmt = toD3DFormat (format);
	if (d3dFmt != D3DFMT_UNKNOWN)
	{
		ATOM_D3D9Window *d3dwin = (ATOM_D3D9Window *)_M_render_window.get();
		ATOM_D3D9WindowInfo* info = (ATOM_D3D9WindowInfo*)d3dwin->getWindowInfo();
		UINT adapter = info->adapter;
		D3DDEVTYPE devtype = info->devtype;
		D3DFORMAT adpterformat = info->presentParams.BackBufferFormat;
		return supportFiltering (_M_d3d, adapter, devtype, adpterformat, d3dFmt);
	}
	return false;
}

D3DFORMAT ATOM_D3D9Device::findBestHDRFormat (int *f, D3DFORMAT format1, D3DFORMAT format2) const
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::findBestHDRFormat);

	ATOM_D3D9Window *d3dwin = (ATOM_D3D9Window *)_M_render_window.get();
	ATOM_D3D9WindowInfo* info = (ATOM_D3D9WindowInfo*)d3dwin->getWindowInfo();
	UINT adapter = info->adapter;
	D3DDEVTYPE devtype = info->devtype;
	D3DFORMAT format = info->presentParams.BackBufferFormat;

	if (supportPostPixelShaderBlending (_M_d3d, adapter, devtype, format, format1))
	{
		if (supportFiltering (_M_d3d, adapter, devtype, format, format1))
		{
			*f = 1;
			return format1;
		}
		else
		{
			if (supportPostPixelShaderBlending (_M_d3d, adapter, devtype, format, format2))
			{
				if (supportFiltering (_M_d3d, adapter, devtype, format, format2))
				{
					*f = 1;
					return format2;
				}
			}

			*f = 0;
			return format1;
		}
	}
	else
	{
		if (supportPostPixelShaderBlending (_M_d3d, adapter, devtype, format, format2))
		{
			if (supportFiltering (_M_d3d, adapter, devtype, format, format2))
			{
				*f = 1;
			}
			else
			{
				*f = 0;
			}
			return format2;
		}
		else
		{
			*f = 0;
			return D3DFMT_UNKNOWN;
		}
	}
}

ATOM_AUTOREF(ATOM_Image) ATOM_D3D9Device::cursorSurfaceToImage (IDirect3DSurface9 *surface) {
  ATOM_STACK_TRACE(ATOM_D3D9Device::cursorSurfaceToImage);

  HRESULT hr;

  if (surface)
  {
    D3DSURFACE_DESC desc;
    hr = surface->GetDesc(&desc);
    if (FAILED(hr))
    {
		ATOM_LOGGER::error ("%s(0x%08X) GetDesc() failed.\n", __FUNCTION__, surface);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
    }

    ATOM_ASSERT (desc.Format == D3DFMT_A8R8G8B8);
    unsigned w = desc.Width;
    unsigned h = desc.Height;

    D3DLOCKED_RECT lrc;
    hr = surface->LockRect (&lrc, 0, D3DLOCK_READONLY);
    if (FAILED(hr))
    {
		ATOM_LOGGER::error ("%s(0x%08X) LockRect() failed.\n", __FUNCTION__, surface);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
    }

    ATOM_HARDREF(ATOM_Image) img;
    img->init (w, h, ATOM_PIXEL_FORMAT_BGRA8888);

    unsigned char *data = img->getData();
    unsigned char *bits = (unsigned char*)lrc.pBits;
    unsigned src_pitch = w * 4;
    unsigned dst_pitch = lrc.Pitch;

    for (unsigned i = 0; i < h; ++i)
    {
      memcpy (data + i * src_pitch, bits + i * dst_pitch, src_pitch);
    }

    surface->UnlockRect ();
    return img;
  }

  return 0;
}

IDirect3DSurface9 *ATOM_D3D9Device::textureToCursorSurface (ATOM_Texture *texture, const int *region, ATOM_AUTOREF(ATOM_Image) &image)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::textureToCursorSurface);

	image = 0;

	if (texture)
	{
		HRESULT hr;

		ATOM_D3DTexture *d3dtex = (ATOM_D3DTexture*)texture;

		IDirect3DSurface9 *surface = 0;
		hr = d3dtex->getSurfaceLevel(0, &surface);
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(0x%08X, 0x%08X) GetSurfaceLevel() failed.\n", __FUNCTION__, texture, region);
			ATOM_CHECK_D3D_RESULT(hr);
			return 0;
		}

		ATOM_ASSERT (surface);
		D3DSURFACE_DESC desc;
		hr = d3dtex->getLevelDesc (0, &desc);
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(0x%08X, 0x%08X) GetLevelDesc() failed.\n", __FUNCTION__, texture, region);
			surface->Release ();
			ATOM_CHECK_D3D_RESULT(hr);
			return 0;
		}

		int l = region ? region[0] : 0;
		int t = region ? region[1] : 0;
		int w = region ? region[2] : desc.Width;
		int h = region ? region[3] : desc.Height;
		if (w == 0 || h == 0)
		{
			ATOM_LOGGER::error ("%s(0x%08X, 0x%08X) Invalid region(%d,%d,%d,%d).\n", __FUNCTION__, texture, l, t, w, h);
			surface->Release ();
			return 0;
		}

		if (desc.Format==D3DFMT_A8R8G8B8 && l==0 && t==0 && w==32 && h==32)
		{
			image = cursorSurfaceToImage (surface);
			return surface;
		}

		IDirect3DSurface9 *cursorSurface = 0;
		hr = _M_d3ddevice->CreateOffscreenPlainSurface(w, h, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &cursorSurface, NULL);
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(0x%08X, 0x%08X) CreateOffscreenPlainSurface() failed.\n", __FUNCTION__, texture, region);
			surface->Release();
			ATOM_CHECK_D3D_RESULT(hr);
			return 0;
		}

		RECT rc;
		rc.left = l;
		rc.top = t;
		rc.right = l + w;
		rc.bottom = t + h;

		//--- wangjian commented ---//
		// 去除D3DX依赖：使用LOCK/UNLOCK进行拷贝

		hr = D3DXLoadSurfaceFromSurface (cursorSurface, 0, 0, surface, 0, &rc, D3DX_FILTER_NONE, 0);
		surface->Release();
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(0x%08X, 0x%08X) D3DXLoadSurfaceFromSurface() failed.\n", __FUNCTION__, texture, region);
			ATOM_CHECK_D3D_RESULT(hr);
			cursorSurface->Release();
			return 0;
		}

		image = cursorSurfaceToImage (cursorSurface);
		if (!image)
		{
			return 0;
		}

		if (w == 32 && h == 32)
		{
			return cursorSurface;
		}
		cursorSurface->Release ();
		image->convertFormat (ATOM_PIXEL_FORMAT_BGRA8888);
		image->resize (32, 32);
		return imageToCursorSurface (image.get(), 0);
  }

  return 0;
}

IDirect3DSurface9 *ATOM_D3D9Device::imageToCursorSurface (ATOM_Image *image, const int *region) {
  ATOM_STACK_TRACE(ATOM_D3D9Device::imageToCursorSurface);

  if (image)
  {
    HRESULT hr;

    int l = region ? region[0] : 0;
    int t = region ? region[1] : 0;
    int w = region ? region[2] : image->getWidth();
    int h = region ? region[3] : image->getHeight();

    IDirect3DSurface9 *surface = 0;
    hr = _M_d3ddevice->CreateOffscreenPlainSurface(32, 32, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &surface, NULL);
    if (FAILED(hr))
    {
		ATOM_LOGGER::error ("%s(0x%08X, 0x%08X) CreateOffscreenPlainSurface() failed.\n", __FUNCTION__, image, region);
		ATOM_CHECK_D3D_RESULT(hr);
		return 0;
    }

    if (image)
    {
      image->convertFormat (ATOM_PIXEL_FORMAT_BGRA8888);
    }

    unsigned char *data = ATOM_NEW_ARRAY(unsigned char, w * h * 4);
    for (int i = 0; i < h; ++i)
    {
      memcpy (data + i * w * 4, image->getData() + ((t + i) * image->getWidth() + l) * 4, w * 4);
    }

	ATOM_BaseImage tmpImage(w, h, ATOM_PIXEL_FORMAT_BGRA8888, data, 1);

	if (w != 32 || h != 32)
	{
		tmpImage.resize (32, 32);
	}

    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = 32;
    rc.bottom = 32;

	//--- wangjian commented ---//
	// 去除D3DX依赖：使用LOCK/UNLOCK进行拷贝
    hr = D3DXLoadSurfaceFromMemory(surface, 0, 0, tmpImage.getData(), D3DFMT_A8R8G8B8, 4 * 32, 0, &rc, D3DX_FILTER_NONE, 0);

    ATOM_DELETE_ARRAY(data);

    if (FAILED(hr))
    {
		ATOM_LOGGER::error ("%s(0x%08X, 0x%08X) D3DXLoadSurfaceFromMemory() failed.\n", __FUNCTION__, image, region);
		surface->Release ();
		ATOM_CHECK_D3D_RESULT(hr);
		return 0;
    }

    return surface;
  }

  return 0;
}

void ATOM_D3D9Device::setupCaps (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::setupCaps);

  _depthTextureFormat = determinDepthTextureFormat();

  ATOM_D3D9WindowInfo *info = (ATOM_D3D9WindowInfo*)_M_render_window->getWindowInfo();

  D3DCAPS9 caps;
  ATOM_CHECK_D3D_RESULT(_M_d3ddevice->GetDeviceCaps (&caps));

  device_caps.hardware_accel = (caps.DevCaps & D3DDEVCAPS_HWRASTERIZATION) != 0 && (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
  device_caps.index_32bit = caps.MaxVertexIndex > 0x0000FFFF;
  device_caps.fsaa = false;
  device_caps.blend_color = (caps.SrcBlendCaps & D3DBLEND_BLENDFACTOR) && (caps.DestBlendCaps & D3DBLEND_BLENDFACTOR);
  device_caps.mipmap_generation = (caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP) != 0;
  device_caps.alpha_to_coverage = _M_d3d->CheckDeviceFormat(info->adapter, info->devtype, info->presentParams.BackBufferFormat, 0, D3DRTYPE_SURFACE, (D3DFORMAT)MAKEFOURCC('S','S','A','A')) == S_OK;
  if (device_caps.alpha_to_coverage)
  {
	  _M_taa_fourcc = MAKEFOURCC('S','S','A','A');
  }
  else
  {
	  device_caps.alpha_to_coverage = _M_d3d->CheckDeviceFormat(info->adapter, info->devtype, info->presentParams.BackBufferFormat, 0, D3DRTYPE_SURFACE, (D3DFORMAT)MAKEFOURCC('A','T','O','C')) == S_OK;
	if (device_caps.alpha_to_coverage)
	{
		_M_taa_fourcc = MAKEFOURCC('A','T','O','C');
	}
  }
  device_caps.anisotropic_filter = (caps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY) != 0;
  device_caps.texture_stream_write = (caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) != 0;
  device_caps.texture_cubemap = (caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP) != 0;
  device_caps.texture_s3tc_compress = isTextureFormatOk(D3DFMT_DXT1) && isTextureFormatOk(D3DFMT_DXT3) && isTextureFormatOk(D3DFMT_DXT5);
  device_caps.texture_sRGB_read = doesTextureFormatSupportSRGB(D3DFMT_DXT1)
	  && doesTextureFormatSupportSRGB(D3DFMT_DXT3)
	  && doesTextureFormatSupportSRGB(D3DFMT_DXT5)
	  && doesTextureFormatSupportSRGB(D3DFMT_X8R8G8B8)
	  && doesTextureFormatSupportSRGB(D3DFMT_A8R8G8B8);
  device_caps.framebuffer_sRGB_write = supportSRGBWrite (info->presentParams.BackBufferFormat);
  device_caps.texture_3d = (caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP) != 0;
  device_caps.texture_npo2 = (caps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0 && (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) == 0;
  device_caps.texture_npo2_c = (caps.TextureCaps & D3DPTEXTURECAPS_POW2) != 0 && (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0;
  device_caps.texture_depth = _depthTextureFormat != D3DFMT_UNKNOWN;
  device_caps.texture_packed_depth = _depthTextureFormat == MAKEFOURCC('R','A','W','Z');
  device_caps.texture_float32 = isTextureFormatOk (D3DFMT_A32B32G32R32F)
	  && isTextureFormatOk (D3DFMT_R32F)
	  && isTextureFormatOk (D3DFMT_G32R32F);
  device_caps.texture_float16 = isTextureFormatOk (D3DFMT_A16B16G16R16F)
	  && isTextureFormatOk (D3DFMT_R16F)
	  && isTextureFormatOk (D3DFMT_G16R16F);
  device_caps.rendertarget_float32 = isRenderTargetFormatOk (D3DFMT_A32B32G32R32F)
	  && isRenderTargetFormatOk (D3DFMT_R32F)
	  && isRenderTargetFormatOk (D3DFMT_G32R32F);
  device_caps.rendertarget_float16 = isRenderTargetFormatOk (D3DFMT_A16B16G16R16F)
	  && isRenderTargetFormatOk (D3DFMT_R16F)
	  && isRenderTargetFormatOk (D3DFMT_G16R16F);
  device_caps.shadowmap = false;
  device_caps.per_vertex_point_size = (caps.FVFCaps & D3DFVFCAPS_PSIZE) != 0;
  device_caps.vs_dynamic_branching = (caps.VS20Caps.DynamicFlowControlDepth > 0);
  device_caps.ps_dynamic_branching = (caps.PS20Caps.DynamicFlowControlDepth > 0);
  device_caps.mrt_independent_bit_depth = (caps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS) != 0;
  device_caps.mrt_independent_write_masks = (caps.PrimitiveMiscCaps & D3DPMISCCAPS_INDEPENDENTWRITEMASKS) != 0;
  device_caps.mrt_post_blending = (caps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING) != 0;
  device_caps.max_clip_planes = caps.MaxUserClipPlanes;
  device_caps.max_texture_size = caps.MaxTextureWidth > caps.MaxTextureHeight ? caps.MaxTextureHeight : caps.MaxTextureWidth;
  device_caps.max_texture_size_3d = caps.MaxVolumeExtent;
  device_caps.max_texture_size_cubemap = device_caps.max_texture_size;
  device_caps.max_texture_unit_count = caps.MaxSimultaneousTextures;
  device_caps.max_rendertarget_count = caps.NumSimultaneousRTs;
  if (device_caps.max_texture_unit_count > ATOM_MAX_TEXTURE_LAYER_COUNT)
  {
    device_caps.max_texture_unit_count = ATOM_MAX_TEXTURE_LAYER_COUNT;
  }
  device_caps.max_anisotropy = caps.MaxAnisotropy;
  device_caps.max_point_size = caps.MaxPointSize;
  device_caps.occlusion_query = true;
  device_caps.max_vertex_program_uniforms = caps.MaxVertexShaderConst;
  device_caps.max_fragment_program_uniforms = 0;
  device_caps.major_version = 9;
  device_caps.minor_version = 0;
  device_caps.micro_version = 0;
  device_caps.version = 9.0f;

  //--- wangjian commented ---//
  // 去除对D3DX依赖：得到VS和PS的版本可以使用caps.PixelShaderVersion/VertexShaderVersion.
  const char *vsProfile = D3DXGetVertexShaderProfile (_M_d3ddevice);
  ATOM_LOGGER::log ("VertexShader Profile: %s\n", vsProfile);
  const char *psProfile = D3DXGetPixelShaderProfile (_M_d3ddevice);
  ATOM_LOGGER::log ("PixelShader Profile: %s\n", psProfile);

  if (!vsProfile || !psProfile)
  {
	  device_caps.shader_model = 0;
	  device_caps.shader_model_ext = 0;

	  ATOM_LOGGER::log ("Shader Model: Unknown\n");
  }
  else if (!strcmp (vsProfile, "vs_3_0") && !strcmp(psProfile, "ps_3_0"))
  {
	  device_caps.shader_model = 3;
	  device_caps.shader_model_ext = 0;

	  ATOM_LOGGER::log ("Shader Model: 3.0\n");
  }
  else
  {
	  bool vs_1_1 = false;
	  bool vs_2_0 = false;
	  bool vs_2_a = false;
	  bool ps_2_0 = false;
	  bool ps_2_a = false;
	  bool ps_2_b = false;

	  if (!strcmp (vsProfile, "vs_2_a"))
	  {
		  vs_2_a = true;
		  vs_2_0 = true;
		  vs_1_1 = true;
	  }
	  else if (!strcmp (vsProfile, "vs_2_0"))
	  {
		  vs_2_0 = true;
		  vs_1_1 = true;
	  }
	  else if (!strcmp (vsProfile, "vs_1_1"))
	  {
		  vs_1_1 = true;
	  }

	  if (!strcmp (psProfile, "ps_2_b"))
	  {
		  ps_2_b = true;
		  ps_2_a = true;
		  ps_2_0 = true;
	  }
	  else if (!strcmp (psProfile, "ps_2_a"))
	  {
		  ps_2_a = true;
		  ps_2_0 = true;
	  }
	  else if (!strcmp (psProfile, "ps_2_0"))
	  {
		  ps_2_0 = true;
	  }

	  if (vs_2_0 && ps_2_a)
	  {
		  device_caps.shader_model = 2;
		  device_caps.shader_model_ext = 1;
	  }
	  //--------------------------------------//
	  // wangjian modified : if vs_2_0 or ps_2_0 
#if 0
	  else if (vs_2_0 && ps_2_0)
#else
	  else if (vs_2_0 || ps_2_0)
#endif
	  //--------------------------------------//
	  {
		  device_caps.shader_model = 2;
		  device_caps.shader_model_ext = 0;

		  ATOM_LOGGER::log ("Shader Model: 2.0\n");
	  }
	  else
	  {
		  device_caps.shader_model = 0;
		  device_caps.shader_model_ext = 0;

		  ATOM_LOGGER::log ("Shader Model: Unknown\n");
	  }
  }
  device_caps.max_sampler_count = (device_caps.shader_model >= 2) ? 16 : device_caps.max_texture_unit_count;

#if 0
  device_caps.hardware_instancing = false;
#else
  device_caps.hardware_instancing = device_caps.shader_model >= 3;
#endif

  //--- wangjian added for some special caps ---//
  // 1. 设置对硬件PCF的支持（用于shadow map的生成 )
  // hardware pcf
  device_caps.ati_fetch4 = _M_d3d->CheckDeviceFormat( info->adapter, info->devtype, info->presentParams.BackBufferFormat,
													  D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, (D3DFORMAT)MAKEFOURCC('D','F','2','4') ) == S_OK;
 
  device_caps.hardware_pcf = _M_d3d->CheckDeviceFormat(	info->adapter, info->devtype, info->presentParams.BackBufferFormat,
															D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, D3DFMT_D24S8 ) == S_OK;
  //device_caps.hardware_pcf = false;
  if( device_caps.hardware_pcf )
	  ATOM_LOGGER::log ("Native Shadow Mapping ( Sample depth buffer directly as shadow map ) IS support!\n");

  // 2. 硬件设备对NRT的支持（用于shadow map的生成 )
  // render target format : NULL
  device_caps.null_rtf = _M_d3d->CheckDeviceFormat(	info->adapter, info->devtype, info->presentParams.BackBufferFormat, 
													0, D3DRTYPE_SURFACE, (D3DFORMAT)MAKEFOURCC('N','U','L','L')	) == S_OK;
  //device_caps.null_rtf = false;
  if( device_caps.null_rtf )
	  ATOM_LOGGER::log ("NULL RT ( Dummy render target surface that does not consume video memory ) IS support!\n");

  // 3. 硬件设备对RESZ的支持（暂时未用到）
  // RESZ： Resolve Z depth
  bool bRESZSupported = _M_d3d->CheckDeviceFormat(	info->adapter, info->devtype, info->presentParams.BackBufferFormat, 
													D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, 
													(D3DFORMAT)MAKEFOURCC('R','E','S','Z')	) == S_OK;
  if( bRESZSupported )
	  ATOM_LOGGER::log ("RESZ ( Resolve MSAA’d depth stencil surface into non-MSAA’d depth texture ) IS support!\n");

  // 4. 支持half float顶点layout
  device_caps.declTypes_float16 = caps.DeclTypes & D3DDTCAPS_FLOAT16_4 ? true : false;
  if( device_caps.declTypes_float16 )
	  ATOM_LOGGER::log ("HALF float vertex declaration IS support!\n");
  //--------------------------------------------//
}

bool ATOM_D3D9Device::setup (ATOM_DisplayMode *fullscreen_mode, const char *title, int w, int h, ATOM_RenderWindow::ShowMode showMode, bool naked, bool resizable, int multisample, void *windowid, void *parentid, void *iconid, bool contentBackup, void *instance)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::setup);

  if (!ATOM_RenderDevice::setup (fullscreen_mode, title, w, h, showMode, naked, resizable, multisample, windowid, parentid, iconid, contentBackup, instance))
  {
    return false;
  }

  ATOM_HARDREF(ATOM_D3D9Window) d3dwindow;
  ATOM_ASSERT(d3dwindow);

  ATOM_RenderWindow::Hints hints;
  hints.accel = true;
  hints.bounds.first = w;
  hints.bounds.second = h;
  hints.resizable = resizable;
  hints.naked = naked;
  hints.bpp = 32;
  hints.fullscreen = (fullscreen_mode != NULL);
  hints.samples = multisample;

  if (multisample)
  {
	ATOM_LOGGER::log ("Setting up display mode: %d x %d %s %dx FSAA \n", w, h, fullscreen_mode ? "[fullscreen]" : "", multisample);
  }
  else
  {
	ATOM_LOGGER::log ("Setting up display mode: %d x %d %s \n", w, h, fullscreen_mode ? "[fullscreen]" : "");
  }

  if (!d3dwindow->open(this, title ? title : "", &hints, showMode, windowid, parentid, iconid, instance))
  {
    return false;
  }

  d3dwindow->_M_is_primary = true;

  ATOM_D3D9WindowInfo *wi = (ATOM_D3D9WindowInfo*)d3dwindow->getWindowInfo();
  ATOM_ASSERT(wi);

  _M_d3d = wi->d3d9;
  _M_d3ddevice = wi->device;
  _M_d3dpp = wi->presentParams;
  _M_render_window = d3dwindow;

  ATOM_APP->setMainWindow (wi->handle);

  // Get device capabilities
  setupCaps ();

  // Find the best HDR format
  _M_best_HDR_format = ATOM_PIXEL_FORMAT_UNKNOWN;
  _M_best_HDR_format_rg = ATOM_PIXEL_FORMAT_UNKNOWN;
  _M_best_HDR_format_r = ATOM_PIXEL_FORMAT_UNKNOWN;

  D3DFORMAT hdrFormat = findBestHDRFormat (&_M_HDR_format_support_filtering, D3DFMT_A16B16G16R16F, D3DFMT_A32B32G32R32F);
  switch (hdrFormat)
  {
  case D3DFMT_A16B16G16R16F:
	  _M_best_HDR_format = ATOM_PIXEL_FORMAT_RGBA16F;
	  break;
  case D3DFMT_A32B32G32R32F:
	  _M_best_HDR_format = ATOM_PIXEL_FORMAT_RGBA32F;
	  break;
  }

  D3DFORMAT hdrFormatRG = findBestHDRFormat (&_M_HDR_format_rg_support_filtering, D3DFMT_G16R16F, D3DFMT_G32R32F);
  switch (hdrFormatRG)
  {
  case D3DFMT_G16R16F:
	  _M_best_HDR_format_rg = ATOM_PIXEL_FORMAT_RG16F;
	  break;
  case D3DFMT_G32R32F:
	  _M_best_HDR_format_rg = ATOM_PIXEL_FORMAT_RG32F;
	  break;
  }

  D3DFORMAT hdrFormatR = findBestHDRFormat (&_M_HDR_format_r_support_filtering, D3DFMT_R16F, D3DFMT_R32F);
  switch (hdrFormatR)
  {
  case D3DFMT_R16F:
	  _M_best_HDR_format_r = ATOM_PIXEL_FORMAT_R16F;
	  break;
  case D3DFMT_R32F:
	  _M_best_HDR_format_r = ATOM_PIXEL_FORMAT_R32F;
	  break;
  }

  // Set default render states
  hints = d3dwindow->getWindowHints();

  ATOM_Matrix4x4f mat = ATOM_Matrix4x4f::getPerspectiveFovLHMatrix(3.1415926f / 3.f, float(hints.bounds.first) / float(hints.bounds.second), 1.f, 1000.f);
  setTransform(ATOM_MATRIXMODE_PROJECTION, mat);
  mat.makeIdentity ();
  setTransform(ATOM_MATRIXMODE_WORLD, mat);
  setTransform(ATOM_MATRIXMODE_VIEW, mat);

  _M_render_window = d3dwindow;
  d3dwindow->_M_device = this;

  if (!_stateCache)
  {
	  _stateCache = ATOM_NEW(ATOM_StateCache, this);
  }

  syncRenderStates ();
  setInitialStates ();

  ATOM_CHECK_D3D_RESULT(_M_d3ddevice->GetRenderTarget (0, &_M_default_backbuffer));
  ATOM_CHECK_D3D_RESULT(_M_d3ddevice->GetDepthStencilSurface (&_M_default_depthbuffer));

  getBackbufferDimension (&_backbufferW, &_backbufferH);

  IDirect3DStateBlock9 *sb = 0;
  ATOM_CHECK_D3D_RESULT(_M_d3ddevice->CreateStateBlock(D3DSBT_ALL, &sb));
  sb->Capture ();
  sb->Apply ();
  sb->Release ();

  addView (_M_render_window.get());

  setDefaultCursor (nullptr);

  _M_device_ready = true;

  return true;
}

void ATOM_D3D9Device::syncRenderStates(void)
{
	_stateCache->loadDefaults ();
	_stateCache->getDefaultStates ()->apply (0, true);
}

IDirect3DSurface9 *ATOM_D3D9Device::getBackBuffer (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::getBackBuffer);

  ATOM_AUTOREF(ATOM_D3D9Window) w = _M_current_view ? _M_current_view->window : 0;
  return w ? w->getBackBuffer() : 0;
}

IDirect3DSurface9 *ATOM_D3D9Device::getDepthStencilBuffer (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::getDepthStencilBuffer);

  ATOM_AUTOREF(ATOM_D3D9Window) w = _M_current_view ? _M_current_view->window : 0;
  return w ? w->getDepthStencilBuffer() : 0;
}

IDirect3DSurface9 *ATOM_D3D9Device::getDepthStencilSurface (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::getDepthStencilSurface);

  if (_M_d3ddevice)
  {
    IDirect3DSurface9 *surface;
    ATOM_CHECK_D3D_RESULT(_M_d3ddevice->GetDepthStencilSurface(&surface));
    return surface;
  }
  return 0;
}

bool ATOM_D3D9Device::setRenderTarget (int index, ATOM_Texture *target, unsigned face/* = -1*/)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::setRenderTarget);

	//--- wangjian added ---//
	// 当_M_current_rtss设置了之后，
	// 需要在其他 setRenderTarget 被调用之前通过 setRenderTargetSurface(index,0) 将 RenderTargetSurface 重置为 NULL
	if( _M_current_rtss[index] )
	{
		ATOM_LOGGER::warning("%s(%d, 0x%08X) setRenderTargetSurface(%d,NULL) should be called before set new Render Target.\n", __FUNCTION__, index, target, index);
		setRenderTargetSurface(index,NULL);
	}
	//----------------------//

	HRESULT hr;

	ATOM_RenderStateSetBase *states = (ATOM_RenderStateSetBase*)_stateCache->getCurrentStates();

	for (unsigned stageIndex = 0; stageIndex < 16; ++stageIndex)
	{
		_M_d3ddevice->SetTexture (stageIndex, NULL);
		if (states->_samplerAttributes[stageIndex])
		{
			states->_samplerAttributes[stageIndex]->setTexture (NULL);
		}
	}

	if (index == 0)
	{
		if (!target)
		{
			ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetRenderTarget (0, _M_default_backbuffer));
		}
		else
		{
			//------- wangjian added for cube map ----------//
			if( (target->getFlags() & ATOM_Texture::CUBEMAP) && face == -1 )
			{
				face = target->getRenderTargetFace();
				if( face == -1 )
				{
					ATOM_LOGGER::error("%s(%d, 0x%08X) the face of cubemap should be >= 0 \n", __FUNCTION__, index, target, index);
					return false;
				}
			}
			//---------------------------------------------//

			ATOM_D3DTexture *tex = (ATOM_D3DTexture*)target;			
			IDirect3DSurface9 *colorRT = 0;
			hr = tex->getSurfaceLevel(0, &colorRT, face);		
			if (FAILED(hr))
			{
				ATOM_LOGGER::error ("%s(%d, 0x%08X) GetSurfaceLevel() failed.\n", __FUNCTION__, index, target);
				ATOM_CHECK_D3D_RESULT(hr);
				return false;
			}

			hr = _M_d3ddevice->SetRenderTarget (0, colorRT);
			colorRT->Release ();

			if (FAILED(hr))
			{
				ATOM_LOGGER::error ("%s(%d, 0x%08X) SetRenderTarget() failed.\n", __FUNCTION__, index, target);
				ATOM_CHECK_D3D_RESULT(hr);
				return false;
			}	
		}
	}
	else
	{
		if (index >= 4 || index >= getCapabilities().max_rendertarget_count)
		{
			ATOM_LOGGER::error ("%s(%d, 0x%08X) Invalid rendertarget index.\n", __FUNCTION__, index, target);
			return false;
		}

		if (!target)
		{
			hr = _M_d3ddevice->SetRenderTarget (index, 0);
			if (FAILED(hr))
			{
				ATOM_LOGGER::error ("%s(%d, 0x%08X) SetRenderTarget() failed.\n", __FUNCTION__, index, target);
				ATOM_CHECK_D3D_RESULT(hr);
				return false;
			}
		}
		else
		{
			//------- wangjian added for cube map ----------//
			if( (target->getFlags() & ATOM_Texture::CUBEMAP) && face == -1 )
			{
				face = target->getRenderTargetFace();
				if( face == -1 )
				{
					ATOM_LOGGER::error("%s(%d, 0x%08X) the face of cubemap should be >= 0 \n", __FUNCTION__, index, target, index);
					return false;
				}
			}
			//---------------------------------------------//

			ATOM_D3DTexture *tex = (ATOM_D3DTexture*)target;
			IDirect3DSurface9 *colorRT = 0;
			hr = tex->getSurfaceLevel(0, &colorRT, face);	// wangjian modified for cube map
			if (FAILED(hr))
			{
				ATOM_LOGGER::error ("%s(%d, 0x%08X) GetSurfaceLevel() failed.\n", __FUNCTION__, index, target);
				ATOM_CHECK_D3D_RESULT(hr);
				return false;
			}
			hr = _M_d3ddevice->SetRenderTarget (index, colorRT);
			colorRT->Release ();

			if (FAILED(hr))
			{
				ATOM_LOGGER::error ("%s(%d, 0x%08X) SetRenderTarget() failed.\n", __FUNCTION__, index, target);
				ATOM_CHECK_D3D_RESULT(hr);
				return false;
			}
		}
	}

	_M_current_rendertargets[index] = target;

	//--- wangjian added ---//
	if( target && (target->getFlags() & ATOM_Texture::CUBEMAP) && face != -1 )
		target->setRenderTargetFace(face);
	//----------------------//

	return true;
}

//--- wangjian added ---//
// * 新增的API：用于直接设置通过CreateRenderTarget函数直接生成的RT surface作为 Render Target *//
// * 主要用于在生成shadow map时使用NULL RENDER TARGET时，设置生成的DUMMY COLOR RT。
// * 注意：在渲染到RT之后马上通过 setRenderTargetSurface(index,NULL)将RT重置。
bool ATOM_D3D9Device::setRenderTargetSurface (int index, ATOM_RenderTargetSurface *rts)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::setRenderTargetSurface);

	HRESULT hr;

	ATOM_RenderStateSetBase *states = (ATOM_RenderStateSetBase*)_stateCache->getCurrentStates();

	for (unsigned stageIndex = 0; stageIndex < 16; ++stageIndex)
	{
		_M_d3ddevice->SetTexture (stageIndex, NULL);
		if (states->_samplerAttributes[stageIndex])
		{
			states->_samplerAttributes[stageIndex]->setTexture (NULL);
		}
	}

	if (index == 0)
	{
		if (!rts)
		{
			ATOM_CHECK_D3D_RESULT(_M_d3ddevice->SetRenderTarget (0, _M_default_backbuffer));
		}
		else
		{
			ATOM_RenderTargetSurfaceD3D *rts_d3d = (ATOM_RenderTargetSurfaceD3D*)rts;
			IDirect3DSurface9 *colorRT = rts_d3d->getD3DSurface();
			if (!colorRT)
			{
				ATOM_LOGGER::error ("%s(%d, 0x%08X) has no d3d surface.\n", __FUNCTION__, index, rts);
				return false;
			}

			hr = _M_d3ddevice->SetRenderTarget (0, colorRT);

			if (FAILED(hr))
			{
				ATOM_LOGGER::error ("%s(%d, 0x%08X) SetRenderTarget() failed.\n", __FUNCTION__, index, rts);
				ATOM_CHECK_D3D_RESULT(hr);
				return false;
			}
		}
	}
	else
	{
		if (index >= 4 || index >= getCapabilities().max_rendertarget_count)
		{
			ATOM_LOGGER::error ("%s(%d, 0x%08X) Invalid rendertarget index.\n", __FUNCTION__, index, rts);
			return false;
		}

		if (!rts)
		{
			hr = _M_d3ddevice->SetRenderTarget (index, 0);
			if (FAILED(hr))
			{
				ATOM_LOGGER::error ("%s(%d, 0x%08X) SetRenderTarget() failed.\n", __FUNCTION__, index, rts);
				ATOM_CHECK_D3D_RESULT(hr);
				return false;
			}
		}
		else
		{
			ATOM_RenderTargetSurfaceD3D *rts_d3d = (ATOM_RenderTargetSurfaceD3D*)rts;
			IDirect3DSurface9 *colorRT = rts_d3d->getD3DSurface();
			if (!colorRT)
			{
				ATOM_LOGGER::error ("%s(%d, 0x%08X) has no d3d surface.\n", __FUNCTION__, index, rts);
				return false;
			}
			hr = _M_d3ddevice->SetRenderTarget (index, colorRT);

			if (FAILED(hr))
			{
				ATOM_LOGGER::error ("%s(%d, 0x%08X) SetRenderTarget() failed.\n", __FUNCTION__, index, rts);
				ATOM_CHECK_D3D_RESULT(hr);
				return false;
			}
		}
	}

	_M_current_rtss[index] = rts;
	_M_current_rendertargets[index] = 0;

	return true;
}
//----------------------//

//--- wangjian modified ---//
// * 新增两个参数：用于通过 setRenderTargetSurface 函数设置RT之后，需要获取RT的表面大小时 *//
ATOM_Texture *ATOM_D3D9Device::getRenderTarget (int index, int * rt_width, int * rt_height) const
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::getRenderTarget);

	if (index >= 4)
	{
		return 0;
	}

	//--- wangjian added ---//
	// 当通过 setRenderTargetSurface 设置RT后，获得的是该surface的大小，此时_M_current_rendertargets[index]为NULL
	// 注意：rt_width 和 rt_height的值需要在getRenderTarget定义和初始化为0。
	if( _M_current_rtss[index].get() && rt_width && rt_height )
	{
		*rt_width = _M_current_rtss[index].get()->getWidth();
		*rt_height = _M_current_rtss[index].get()->getHeight();
	}
	//----------------------//

	return _M_current_rendertargets[index].get();
}

struct ATOM_D3D9WindowInfo *ATOM_D3D9Device::getD3DWindowInfo (void) const
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::getD3DWindowInfo);

  return _M_render_window ? (ATOM_D3D9WindowInfo*)_M_render_window->getWindowInfo() : 0;
}

ATOM_RenderDevice * ATOM_CreateRenderDevice (ATOM_DisplayMode *mode, int w, int h, ATOM_RenderWindow::ShowMode showMode, bool naked, bool resizable, int multisample, const char *title, void *windowid, void *parentid, void *iconid, bool contentBackup,void *instance)
{
	ATOM_STACK_TRACE(ATOM_CreateRenderDevice);

	ATOM_RenderDevice *device = ATOM_NEW(ATOM_D3D9Device);

	if (device)
	{
		if (device->setup (mode, title, w, h, showMode, naked, resizable, multisample, windowid, parentid, iconid, contentBackup, instance))
		{
			return device;
		}
		else
		{
			ATOM_DELETE(device);
		}
	}

	return 0;
}

void ATOM_DestroyRenderDevice (ATOM_RenderDevice *device)
{
  ATOM_STACK_TRACE(ATOM_DestroyRenderDevice);

  ATOM_DELETE (device);
}

//--- wangjian MODIFIED for Multithreaded ---//
// 异步加载 ： 添加加载优先级
ATOM_AUTOREF(ATOM_Texture) ATOM_D3D9Device::createTextureResource (	const char *filename, 
																	unsigned width, 
																	unsigned height, 
																	ATOM_PixelFormat format,
																	int loadPriority	) 
{
  ATOM_STACK_TRACE(ATOM_D3D9Device::createTextureResource);

  //--- wangjian MODIFIED for Multithreaded ---//
  // 异步加载 ： 添加加载优先级
  return _M_texture_manager->AllocateTexture (filename, width, height, true, format, loadPriority );
  //-------------------------------------------//
}

ATOM_AUTOREF(ATOM_Texture) ATOM_D3D9Device::findTextureResource (const char *filename) {
	ATOM_STACK_TRACE(ATOM_D3D9Device::findTextureResource);

	return _M_texture_manager->LookupTexture (filename);
}

ATOM_AUTOREF(ATOM_Texture) ATOM_D3D9Device::createTexture (const char *filename, unsigned width, unsigned height, ATOM_PixelFormat format) {
  ATOM_STACK_TRACE(ATOM_D3D9Device::createTexture);

  return _M_texture_manager->AllocateTexture (filename, width, height, false, format);
}

bool ATOM_D3D9Device::saveImageToFile (int w, int h, const void *rawdata, ATOM_PixelFormat format, const char *filename)
{
	return false;
}

void *ATOM_D3D9Device::getRenderContext (void) const
{
	return _M_d3ddevice;
}

unsigned ATOM_D3D9Device::getAvailableTextureMem (void) const
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::getAvailableTextureMem);

	return _M_d3ddevice->GetAvailableTextureMem ();
}

ATOM_PixelFormat ATOM_D3D9Device::getBestHDRFormatRGBA (int *supportFiltering) const
{
	if (supportFiltering)
	{
		*supportFiltering = _M_HDR_format_support_filtering;
	}
	return _M_best_HDR_format;
}

ATOM_PixelFormat ATOM_D3D9Device::getBestHDRFormatRG (int *supportFiltering) const
{
	if (supportFiltering)
	{
		*supportFiltering = _M_HDR_format_rg_support_filtering;
	}
	return _M_best_HDR_format_rg;
}

ATOM_PixelFormat ATOM_D3D9Device::getBestHDRFormatR (int *supportFiltering) const
{
	if (supportFiltering)
	{
		*supportFiltering = _M_HDR_format_r_support_filtering;
	}
	return _M_best_HDR_format_r;
}

bool ATOM_D3D9Device::getImageFileInfoInMemory (const void *data, unsigned size, ATOM_ImageFileInfo *fileInfo) const
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::getImageFileInfoInMemory);

	//--- wangjian commented ---//
	// 去除D3DX依赖：使用ATOM_IMAGE类加载图片资源 同时获得图片的信息。
	D3DXIMAGE_INFO info;
	HRESULT hr = D3DXGetImageInfoFromFileInMemory (data, size, &info);
	if (FAILED(hr))
	{
		return false;
	}

	if (fileInfo)
	{
		fileInfo->width = info.Width;
		fileInfo->height = info.Height;
		fileInfo->depth = info.Depth;
		fileInfo->numMips = info.MipLevels;

		switch (info.ImageFileFormat)
		{
		case D3DXIFF_JPG:
			fileInfo->type = ATOM_ImageCodec::JPEG;
			break;
		case D3DXIFF_PNG:
			fileInfo->type = ATOM_ImageCodec::PNG;
			break;
		case D3DXIFF_DDS:
			fileInfo->type = ATOM_ImageCodec::DDS;
			break;
		case D3DXIFF_BMP:
			fileInfo->type = ATOM_ImageCodec::BMP;
			break;
		default:
			return false;
		}

		switch (info.ResourceType)
		{
		case D3DRTYPE_SURFACE:
		case D3DRTYPE_TEXTURE:
			fileInfo->textureType = ATOM_Texture::TEXTURE2D;
			break;
		case D3DRTYPE_VOLUMETEXTURE:
			fileInfo->textureType = ATOM_Texture::TEXTURE3D;
			break;
		case D3DRTYPE_CUBETEXTURE:
			fileInfo->textureType = ATOM_Texture::CUBEMAP;
			break;
		default:
			return false;
		}


		switch (info.Format)
		{
		case D3DFMT_A8R8G8B8:
			fileInfo->format = ATOM_PIXEL_FORMAT_BGRA8888;
			break;
		case D3DFMT_X8R8G8B8:
			fileInfo->format = ATOM_PIXEL_FORMAT_BGRX8888;
			break;
		case D3DFMT_R8G8B8:
			fileInfo->format = ATOM_PIXEL_FORMAT_BGR888;
			break;
		case D3DFMT_DXT1:
			fileInfo->format = ATOM_PIXEL_FORMAT_DXT1;
			break;
		case D3DFMT_DXT3:
			fileInfo->format = ATOM_PIXEL_FORMAT_DXT3;
			break;
		case D3DFMT_DXT5:
			fileInfo->format = ATOM_PIXEL_FORMAT_DXT5;
			break;

		case D3DFMT_R32F:
			fileInfo->format = ATOM_PIXEL_FORMAT_R32F;
			break;
		case D3DFMT_G32R32F:
			fileInfo->format = ATOM_PIXEL_FORMAT_RG32F;
			break;
		case D3DFMT_A32B32G32R32F:
			fileInfo->format = ATOM_PIXEL_FORMAT_RGBA32F;
			break;
		case D3DFMT_R16F:
			fileInfo->format = ATOM_PIXEL_FORMAT_R16F;
			break;
		case D3DFMT_G16R16F:
			fileInfo->format = ATOM_PIXEL_FORMAT_RG16F;
			break;
		case D3DFMT_A16B16G16R16F:
			fileInfo->format = ATOM_PIXEL_FORMAT_RGBA16F;
			break;
		default:
			return false;
		}
	}
	return true;
}

static const D3DVERTEXELEMENT9 ElementEnd = D3DDECL_END();

ATOM_VertexDecl ATOM_D3D9Device::createMultiStreamVertexDeclarationEx (unsigned attrib0, ...)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::createMultiStreamVertexDeclarationEx);

	va_list arg_ptr;
	va_start (arg_ptr, attrib0);
	unsigned attrib = attrib0;

	D3DVERTEXELEMENT9 vertexelements[32];
    D3DVERTEXELEMENT9 *elements = vertexelements;
    WORD stream = 0;
	unsigned offset = 0;

	for (;;)
	{
		if (attrib == 0)
		{
			break;
		}

		if (attrib & ATOM_VERTEX_ATTRIB_COORD)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_COORD, elements);
			elements->Stream = stream;
			elements->Offset = 0;
			elements++;
			offset += 3 * sizeof(float);
		}
		else if (attrib & ATOM_VERTEX_ATTRIB_COORD_XYZW)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_COORD_XYZW, elements);
			elements->Stream = stream;
			elements->Offset = 0;
			elements++;
			offset += 4 * sizeof(float);
		}
		else if (attrib & ATOM_VERTEX_ATTRIB_COORD_XYZRHW)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_COORD_XYZRHW, elements);
			elements->Stream = stream;
			elements->Offset = 0;
			elements++;
			offset += 4 * sizeof(float);
		}

		if (attrib & ATOM_VERTEX_ATTRIB_NORMAL)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_NORMAL, elements);
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;
			offset += 3 * sizeof(float);
		}

		if (attrib & ATOM_VERTEX_ATTRIB_PSIZE)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_PSIZE, elements);
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;
			offset += sizeof(float);
		}

		if (attrib & ATOM_VERTEX_ATTRIB_PRIMARY_COLOR)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, elements);
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;
			offset += sizeof(DWORD);
		}

		if (attrib & ATOM_VERTEX_ATTRIB_TANGENT)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_TANGENT, elements);
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;
			offset += 3 * sizeof(float);
		}

		if (attrib & ATOM_VERTEX_ATTRIB_BINORMAL)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_BINORMAL, elements);
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;
			offset += 3 * sizeof(float);
		}

		for (unsigned i = 0; i < ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
		{
			unsigned layerMask = ATOM_VERTEX_ATTRIB_TEX_MASK(i);
			unsigned layerAttrib = attrib & layerMask;

			if (layerAttrib)
			{
				ATOM_D3DVertexArray::getVertexElement (layerAttrib, elements);
				elements->Stream = stream;
				elements->Offset = offset;
				elements++;
				unsigned numComps = 0;
				switch (elements->Type)
				{
				case D3DDECLTYPE_FLOAT1: numComps = 1; break;
				case D3DDECLTYPE_FLOAT2: numComps = 2; break;
				case D3DDECLTYPE_FLOAT3: numComps = 3; break;
				case D3DDECLTYPE_FLOAT4: numComps = 4; break;
				};
				offset += numComps * sizeof(float);
			}
		}

		attrib = va_arg (arg_ptr, unsigned);

		stream++;
	}

    *elements = ElementEnd;

	IDirect3DVertexDeclaration9 *decl = 0;
	HRESULT hr = getD3DDevice()->CreateVertexDeclaration (vertexelements, &decl);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%d) CreateVertexDeclaration() failed.\n", __FUNCTION__, attrib0);
		ATOM_CHECK_D3D_RESULT(hr);
		return 0;
	}

	_M_decls.insert (decl);

	return decl;
}


// wangjian modified for test float16
ATOM_VertexDecl ATOM_D3D9Device::createMultiStreamVertexDeclaration (unsigned attrib, unsigned attribFlags/* = 0*/)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::createMultiStreamVertexDeclaration);

	D3DVERTEXELEMENT9 vertexelements[32];
    D3DVERTEXELEMENT9 *elements = vertexelements;
    WORD stream = 0;

	if (attrib & ATOM_VERTEX_ATTRIB_COORD)
	{
		ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_COORD, elements, attribFlags);

		elements->Stream = stream++;
		elements++;
	}
	else if (attrib & ATOM_VERTEX_ATTRIB_COORD_XYZW)
	{
		ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_COORD_XYZW, elements, attribFlags);
		elements->Stream = stream++;
		elements++;
	}
	else if (attrib & ATOM_VERTEX_ATTRIB_COORD_XYZRHW)
	{
		ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_COORD_XYZRHW, elements, attribFlags);
		elements->Stream = stream++;
		elements++;
	}

	if (attrib & ATOM_VERTEX_ATTRIB_NORMAL)
	{
		ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_NORMAL, elements, attribFlags);

		elements->Stream = stream++;
		elements++;
	}

	if (attrib & ATOM_VERTEX_ATTRIB_PSIZE)
	{
		ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_PSIZE, elements, attribFlags);
		elements->Stream = stream++;
		elements++;
	}

	if (attrib & ATOM_VERTEX_ATTRIB_PRIMARY_COLOR)
	{
		ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, elements, attribFlags);
		elements->Stream = stream++;
		elements++;
	}

	if (attrib & ATOM_VERTEX_ATTRIB_TANGENT)
	{
		ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_TANGENT, elements, attribFlags);
		elements->Stream = stream++;
		elements++;
	}

	if (attrib & ATOM_VERTEX_ATTRIB_BINORMAL)
	{
		ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_BINORMAL, elements, attribFlags);
		elements->Stream = stream++;
		elements++;
	}

	for (unsigned i = 0; i < ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
	{
		unsigned layerMask = ATOM_VERTEX_ATTRIB_TEX_MASK(i);
		unsigned layerAttrib = attrib & layerMask;

		if (layerAttrib)
		{
			ATOM_D3DVertexArray::getVertexElement (layerAttrib, elements, attribFlags);
			elements->Stream = stream++;
			elements++;
		}
	}

    if (stream == 0)
    {
      return 0;
    }

    *elements = ElementEnd;

	IDirect3DVertexDeclaration9 *decl = 0;
	HRESULT hr = getD3DDevice()->CreateVertexDeclaration (vertexelements, &decl);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%d) CreateVertexDeclaration() failed.\n", __FUNCTION__, attrib);
		ATOM_CHECK_D3D_RESULT(hr);
		return 0;
	}

	_M_decls.insert (decl);

	return decl;
}

// wangjian modified
ATOM_VertexDecl ATOM_D3D9Device::createInterleavedVertexDeclaration (unsigned attribGeometry, unsigned attribInstanceData, unsigned attribFlags/* = 0*/)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::createInterleavedVertexDeclaration);

	if (!attribGeometry)
	{
		return 0;
	}

	D3DVERTEXELEMENT9 vertexelements[32];
    D3DVERTEXELEMENT9 *elements = vertexelements;

	unsigned attribs[2] = { attribGeometry, attribInstanceData };

	for (unsigned i = 0; i < 2; ++i)
	{
		WORD stream = i;
		unsigned offset = 0;

		unsigned attrib = attribs[i];
		if (!attrib)
		{
			break;
		}

		unsigned attrib_flags = attribFlags;
		if( i == 1 )
			attrib_flags &= ~ATTRIBUTE_FLAG_COMPRESSED;

		if (attrib & ATOM_VERTEX_ATTRIB_COORD)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_COORD, elements, attrib_flags);	// wangjian modified
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;

			// wangjian modified
			if( attribFlags & ATTRIBUTE_FLAG_COMPRESSED )
				offset += 2 * sizeof(float);
			else
				offset += 3 * sizeof(float);
		}
		else if (attrib & ATOM_VERTEX_ATTRIB_COORD_XYZW)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_COORD_XYZW, elements, attrib_flags);	// wangjian modified
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;

			offset += 4 * sizeof(float);
		}
		else if (attrib & ATOM_VERTEX_ATTRIB_COORD_XYZRHW)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_COORD_XYZRHW, elements, attrib_flags);	// wangjian modified
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;

			offset += 4 * sizeof(float);
		}

		if (attrib & ATOM_VERTEX_ATTRIB_NORMAL)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_NORMAL, elements, attrib_flags);	// wangjian modified
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;

			// wangjian modified
			if( attribFlags & ATTRIBUTE_FLAG_COMPRESSED )
				offset += 2 * sizeof(float);
			else
				offset += 3 * sizeof(float);
		}

		if (attrib & ATOM_VERTEX_ATTRIB_PSIZE)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_PSIZE, elements, attrib_flags);	// wangjian modified
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;

			offset += sizeof(float);
		}

		if (attrib & ATOM_VERTEX_ATTRIB_PRIMARY_COLOR)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, elements, attrib_flags);	// wangjian modified
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;

			offset += sizeof(unsigned);
		}

		if (attrib & ATOM_VERTEX_ATTRIB_TANGENT)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_TANGENT, elements, attrib_flags);	// wangjian modified
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;

			// wangjian modified
			if( attribFlags & ATTRIBUTE_FLAG_COMPRESSED )
				offset += 2 * sizeof(float);
			else
				offset += 3 * sizeof(float);
		}

		if (attrib & ATOM_VERTEX_ATTRIB_BINORMAL)
		{
			ATOM_D3DVertexArray::getVertexElement (ATOM_VERTEX_ATTRIB_BINORMAL, elements, attrib_flags);	// wangjian modified
			elements->Stream = stream;
			elements->Offset = offset;
			elements++;

			// wangjian modified
			if( attribFlags & ATTRIBUTE_FLAG_COMPRESSED )
				offset += 2 * sizeof(float);
			else
				offset += 3 * sizeof(float);
		}

		for (unsigned i = 0; i < ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
		{
			unsigned layerMask = ATOM_VERTEX_ATTRIB_TEX_MASK(i);
			unsigned layerAttrib = attrib & layerMask;

			if (layerAttrib)
			{
				ATOM_D3DVertexArray::getVertexElement (layerAttrib, elements, attrib_flags);	// wangjian modified
				elements->Stream = stream;
				elements->Offset = offset;
				elements++;

				// wangjian modified
				if( attrib_flags & ATTRIBUTE_FLAG_COMPRESSED )
					offset += ATOM_VERTEX_ATTRIB_TEX_COMPONENTS(attrib, i) * sizeof(float) / 2;
				else
					offset += ATOM_VERTEX_ATTRIB_TEX_COMPONENTS(attrib, i) * sizeof(float);
			}
		}
	}

    *elements = ElementEnd;

	IDirect3DVertexDeclaration9 *decl = 0;
	HRESULT hr = getD3DDevice()->CreateVertexDeclaration (vertexelements, &decl);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X, 0x%08X) CreateVertexDeclaration() failed.\n", __FUNCTION__, attribGeometry, attribInstanceData);
		ATOM_CHECK_D3D_RESULT(hr);
		return 0;
	}

	_M_decls.insert (decl);

	return decl;
}

void ATOM_D3D9Device::destroyVertexDecl (ATOM_VertexDecl vd)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::destroyVertexDecl);

	if (vd)
	{
		ATOM_SET<ATOM_VertexDecl>::iterator it = _M_decls.find (vd);

		if (it != _M_decls.end ())
		{
			_M_decls.erase (it);

			IDirect3DVertexDeclaration9 *decl = (IDirect3DVertexDeclaration9*)vd;
			decl->Release ();
		}
	}
}

bool ATOM_D3D9Device::setVertexDecl(ATOM_VertexDecl vd)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::setVertexDecl);

	if (vd)
	{
		HRESULT hr = _M_d3ddevice->SetVertexDeclaration ((IDirect3DVertexDeclaration9*)vd);
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(0x%08X) SetVertexDeclaration() failed.\n", __FUNCTION__, vd);
			ATOM_CHECK_D3D_RESULT(hr);
			return false;
		}
		return true;
	}

	return false;
}

bool ATOM_D3D9Device::setTexture (int sampler, ATOM_Texture *texture)
{
	ATOM_SamplerAttributes *currentStates = _stateCache->getCurrentStates ()->useSampler (sampler, 0);

	if (texture != currentStates->getTexture ())
	{
		if (texture && texture->isLocked ())
		{
			texture = NULL;
		}
		_M_d3ddevice->SetTexture (sampler, texture ? ((ATOM_D3DTexture*)texture)->getD3DTexture () : NULL);
		currentStates->setTexture (texture);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Texture);
	}

	return true;
}

unsigned ATOM_D3D9Device::getFrameStamp (void) const
{
	return _M_frame_stamp;
}

IDirect3DDevice9 *ATOM_D3D9Device::getD3DDevice(void) const
{
	return _M_d3ddevice;
}

IDirect3D9 *ATOM_D3D9Device::getD3D(void) const
{
	return _M_d3d;
}

const D3DPRESENT_PARAMETERS *ATOM_D3D9Device::getD3DPresentParams (void) const
{
	return &_M_d3dpp;
}

void ATOM_D3D9Device::setD3DDevice(IDirect3DDevice9 *device)
{
	_M_d3ddevice = device;
}

void ATOM_D3D9Device::setD3D(IDirect3D9 *d3d)
{
	_M_d3d = d3d;
}

bool ATOM_D3D9Device::pushRenderStates(void)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::pushRenderStates);

	IDirect3DStateBlock9 *stateBlock = 0;
	HRESULT hr = _M_d3ddevice->CreateStateBlock (D3DSBT_ALL, &stateBlock);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() CreateStateBlock() failed.\n", __FUNCTION__);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	hr = stateBlock->Capture ();
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() Capture() failed.\n", __FUNCTION__);
		ATOM_CHECK_D3D_RESULT(hr);
		stateBlock->Release ();
		return false;
	}

	_M_statestack.push_back (stateBlock);
	return true;
}

bool ATOM_D3D9Device::popRenderStates(void)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::popRenderStates);

	if (_M_statestack.empty ())
	{
		ATOM_LOGGER::error ("%s() State capturing stack is empty.\n", __FUNCTION__);
		return false;
	}

	IDirect3DStateBlock9 *stateBlock = _M_statestack.back ();
	_M_statestack.pop_back ();

	HRESULT hr = stateBlock->Apply ();
	stateBlock->Release ();
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() Apply() failed.\n", __FUNCTION__);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	return true;
}

bool ATOM_D3D9Device::isRenderTargetFormat (ATOM_PixelFormat format) const
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::isRenderTargetFormat);

	D3DFORMAT d3dformat;

	switch (format)
	{
	case ATOM_PIXEL_FORMAT_RGBA8888:
		d3dformat = D3DFMT_A8B8G8R8;
		break;
	case ATOM_PIXEL_FORMAT_BGRA8888:
		d3dformat = D3DFMT_A8R8G8B8;
		break;
	case ATOM_PIXEL_FORMAT_RGBX8888:
		d3dformat = D3DFMT_X8B8G8R8;
		break;
	case ATOM_PIXEL_FORMAT_BGRX8888:
		d3dformat = D3DFMT_X8R8G8B8;
		break;
	case ATOM_PIXEL_FORMAT_BGR888:
		d3dformat = D3DFMT_R8G8B8;
		break;
	case ATOM_PIXEL_FORMAT_A8:
		d3dformat = D3DFMT_A8;
		break;
	case ATOM_PIXEL_FORMAT_GREY8:
		d3dformat = D3DFMT_L8;
		break;
	case ATOM_PIXEL_FORMAT_DXT1:
		d3dformat = D3DFMT_DXT1;
		break;
	case ATOM_PIXEL_FORMAT_DXT3:
		d3dformat = D3DFMT_DXT3;
		break;
	case ATOM_PIXEL_FORMAT_DXT5:
		d3dformat = D3DFMT_DXT5;
		break;

	case ATOM_PIXEL_FORMAT_R32F:
		d3dformat = D3DFMT_R32F;
		break;
	case ATOM_PIXEL_FORMAT_RG32F:
		d3dformat = D3DFMT_G32R32F;
		break;
	case ATOM_PIXEL_FORMAT_RGBA32F:
		d3dformat = D3DFMT_A32B32G32R32F;
		break;
	case ATOM_PIXEL_FORMAT_R16F:
		d3dformat = D3DFMT_R16F;
		break;
	case ATOM_PIXEL_FORMAT_RG16F:
		d3dformat = D3DFMT_G16R16F;
		break;
	case ATOM_PIXEL_FORMAT_RGBA16F:
		d3dformat = D3DFMT_A16B16G16R16F;
		break;
	default:
		return false;
	}

	return isRenderTargetFormatOk (d3dformat);
}

ATOM_AUTOREF(ATOM_DepthBuffer) ATOM_D3D9Device::allocDepthBuffer (unsigned width, unsigned height)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::allocDepthBuffer);

	ATOM_HARDREF(ATOM_DepthBufferD3D) depthbuffer;
	depthbuffer->setRenderDevice (this);
	if (depthbuffer->create (width, height))
	{
		return depthbuffer;
	}
	return 0;
}

bool ATOM_D3D9Device::setDepthBuffer (ATOM_DepthBuffer *depthBuffer)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::setDepthBuffer);

	if (depthBuffer)
	{
		//--- wangjian added ---//
		/*if( _M_currentDepthBuffer == depthBuffer )
			return true;*/
		//----------------------//

		ATOM_DepthBufferD3D *p = (ATOM_DepthBufferD3D*)depthBuffer;
		IDirect3DSurface9 *surface = p->getDepthStencilSurface();
		if (surface)
		{
			HRESULT hr = _M_d3ddevice->SetDepthStencilSurface (surface);
			if (FAILED(hr))
			{
				ATOM_LOGGER::error ("%s(0x%08X) SetDepthStencilSurface() failed.\n", __FUNCTION__, depthBuffer);
				ATOM_CHECK_D3D_RESULT(hr);
				return false;
			}
			_M_currentDepthBuffer = depthBuffer;
			return true;
		}
		return false;
	}
	else
	{
		HRESULT hr = _M_d3ddevice->SetDepthStencilSurface (_M_default_depthbuffer);
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(0x%08X) SetDepthStencilSurface() failed.\n", __FUNCTION__, depthBuffer);
			ATOM_CHECK_D3D_RESULT(hr);
			return false;
		}
		_M_currentDepthBuffer = 0;
		return true;
	}
}

ATOM_DepthBuffer *ATOM_D3D9Device::getDepthBuffer (void) const
{
	return _M_currentDepthBuffer.get();
}

//--- wangjian added ---//
// * 新增API, 用以分配一个 Render Target Surface 资源
// * 该API主要用于某些特殊使用情况，比如在shadow map渲染时，如果设备支持NRT，那么在使用NULL RENDER TARGET时需要直接生成
// * RT surface,而非通过生成纹理得到RT surface.
// * 在渲染到纹理时，直接绑定该RT的surface.
ATOM_AUTOREF(ATOM_RenderTargetSurface) ATOM_D3D9Device::allocRenderTargetSurface (unsigned width, unsigned height,ATOM_RenderTargetSurface::ATOM_RTSURFACE_FORMAT fmt)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::allocRenderTargetSurface);

	ATOM_HARDREF(ATOM_RenderTargetSurfaceD3D) rt_surface;
	rt_surface->setRenderDevice (this);
	if (rt_surface->create (width, height,fmt))
	{
		return rt_surface;
	}
	return 0;
}
//----------------------//

void ATOM_D3D9Device::setTextureQuality (ATOM_RenderDevice::TextureQuality quality)
{
#if 0
	ATOM_STACK_TRACE(ATOM_D3D9Device::setTextureQuality);

	int anisotropy;

	switch (quality)
	{
	case TQ_LOW:
		anisotropy = 1;
		_M_state_cache->enableAnisotropicFilter (false);
		break;

	case TQ_MEDIUM:
		anisotropy = ATOM_min2 (4, device_caps.max_anisotropy);
		_M_state_cache->enableAnisotropicFilter (true);
		break;

	case TQ_HIGH:
		anisotropy = ATOM_min2 (8, device_caps.max_anisotropy);
		_M_state_cache->enableAnisotropicFilter (true);
		break;

	case TQ_ULTRA_HIGH:
		anisotropy = device_caps.max_anisotropy;
		_M_state_cache->enableAnisotropicFilter (true);
		break;

	default:
		return;
	}

	for (unsigned i = 0; i < device_caps.max_texture_unit_count; ++i)
	{
		_M_state_cache->SetSamplerState (i, D3DSAMP_MAXANISOTROPY, anisotropy);
	}

	_M_texture_quality = quality;
#endif
}

ATOM_RenderDevice::TextureQuality ATOM_D3D9Device::getTextureQuality (void) const
{
	return _M_texture_quality;
}

bool ATOM_D3D9Device::setLight (unsigned index, const ATOM_VertexLight *light)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::setLight);

	if (!light)
	{
		ATOM_LOGGER::error ("%s(%d, 0x%08X) Invalid argument.\n", __FUNCTION__, index, light);
		return false;
	}

	D3DLIGHT9 l;
	memset (&l, 0, sizeof(D3DLIGHT9));

	l.Diffuse.r = light->diffuse.x;
	l.Diffuse.g = light->diffuse.y;
	l.Diffuse.b = light->diffuse.z;
	l.Diffuse.a = light->diffuse.w;
	l.Specular.r = light->specular.x;
	l.Specular.g = light->specular.y;
	l.Specular.b = light->specular.z;
	l.Specular.a = light->specular.w;
	l.Ambient.r = light->ambient.x;
	l.Ambient.g = light->ambient.y;
	l.Ambient.b = light->ambient.z;
	l.Ambient.a = light->ambient.w;

	switch (light->type)
	{
	case ATOM_VertexLight::VLT_DIRECTIONAL:
		{
			l.Type = D3DLIGHT_DIRECTIONAL;
			l.Direction.x = light->direction.x;
			l.Direction.y = light->direction.y;
			l.Direction.z = light->direction.z;
			l.Range = 1000000.f;
			break;
		}
	case ATOM_VertexLight::VLT_POINT:
		{
			l.Type = D3DLIGHT_POINT;
			l.Range = light->range;
			l.Attenuation0 = light->attenuation0;
			l.Attenuation1 = light->attenuation1;
			l.Attenuation2 = light->attenuation2;
			l.Position.x = light->position.x;
			l.Position.y = light->position.y;
			l.Position.z = light->position.z;
			break;
		}
	case ATOM_VertexLight::VLT_SPOT:
		{
			l.Type = D3DLIGHT_SPOT;
			l.Range = light->range;
			l.Attenuation0 = light->attenuation0;
			l.Attenuation1 = light->attenuation1;
			l.Attenuation2 = light->attenuation2;
			l.Position.x = light->position.x;
			l.Position.y = light->position.y;
			l.Position.z = light->position.z;
			l.Direction.x = light->direction.x;
			l.Direction.y = light->direction.y;
			l.Direction.z = light->direction.z;
			l.Falloff = light->falloff;
			l.Theta = light->theta;
			l.Phi = light->phi;
			break;
		}
	default:
		{
			ATOM_LOGGER::error ("%s(%d, 0x%08X) Invalid argument.\n", __FUNCTION__, index, light);
			return false;
		}
	}

	HRESULT hr = _M_d3ddevice->SetLight (index, &l);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%d, 0x%08X) SetLight() failed.\n", __FUNCTION__, index, light);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	return true;
}

bool ATOM_D3D9Device::enableLight (unsigned index, bool enable)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::enableLight);

	HRESULT hr = _M_d3ddevice->LightEnable (index, enable ? TRUE : FALSE);

	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%d, %s) LightEnable() failed.\n", __FUNCTION__, index, enable ? "True" : "False");
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	return true;
}

bool ATOM_D3D9Device::setAmbientColor (ATOM_ColorARGB color)
{
#if 1
	return false;
#else
	ATOM_STACK_TRACE(ATOM_D3D9Device::setAmbientColor);

	HRESULT hr = _M_state_cache->SetRenderState (D3DRS_AMBIENT, color);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X) SetRenderState(D3DRS_AMBIENT) failed.\n", __FUNCTION__, color);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}
	return true;
#endif
}

bool ATOM_D3D9Device::setMaterial (const ATOM_VertexLightMaterial *material)
{
	ATOM_STACK_TRACE(ATOM_D3D9Device::setMaterial);

	if (!material)
	{
		ATOM_LOGGER::error ("%s(0x%08X) Invalid argument.\n", __FUNCTION__, material);
		return false;
	}

	D3DMATERIAL9 mat;
	mat.Diffuse.r = material->diffuse.x;
	mat.Diffuse.g = material->diffuse.y;
	mat.Diffuse.b = material->diffuse.z;
	mat.Diffuse.a = material->diffuse.w;
	mat.Specular.r = material->specular.x;
	mat.Specular.g = material->specular.y;
	mat.Specular.b = material->specular.z;
	mat.Specular.a = material->specular.w;
	mat.Ambient.r = material->ambient.x;
	mat.Ambient.g = material->ambient.y;
	mat.Ambient.b = material->ambient.z;
	mat.Ambient.a = material->ambient.w;
	mat.Emissive.r = material->emissive.x;
	mat.Emissive.g = material->emissive.y;
	mat.Emissive.b = material->emissive.z;
	mat.Emissive.a = material->emissive.w;
	mat.Power = material->power;

	HRESULT hr = _M_d3ddevice->SetMaterial (&mat);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(0x%08X) SetMaterial() failed.\n", __FUNCTION__, material);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	return true;
}

bool ATOM_D3D9Device::enableVertexLighting (bool enable)
{
#if 1
	return false;
#else
	ATOM_STACK_TRACE(ATOM_D3D9Device::enableVertexLighting);

	HRESULT hr = _M_state_cache->SetRenderState (D3DRS_LIGHTING, enable ? TRUE : FALSE);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s(%s) SetRenderState(D3DRS_LIGHTING) failed.\n", __FUNCTION__, enable ? "True" : "False");
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	return true;
#endif
}

D3DFORMAT ATOM_D3D9Device::determinDepthTextureFormat (void) const
{

	const D3DFORMAT fmt[4] = {
		(D3DFORMAT)MAKEFOURCC('I', 'N', 'T', 'Z'),
		(D3DFORMAT)MAKEFOURCC('D', 'F', '2', '4'),
		(D3DFORMAT)MAKEFOURCC('R', 'A', 'W', 'Z'),
		(D3DFORMAT)MAKEFOURCC('D', 'F', '1', '6')
	};

	//--- wangjian added for debug ---//
	const ATOM_STRING depthTextureFormat[4] = {
		"INTZ",
		"DF24"	
		"RAWZ",
		"DF16"
	};
	//--------------------------------//
	for (int i = 0; i < 4; ++i)
	{
		if (isDepthStencilFormatOk (fmt[i]))
		{
			//--- wangjian added for debug ---//
			ATOM_LOGGER::log( "Depth Texture ( Format : %s ) IS Supported.\n", depthTextureFormat[i].c_str() );
			//--------------------------------//
			return fmt[i];
		}
	}

	//--- wangjian added for debug ---//
	ATOM_LOGGER::log( "Depth Buffer as Texture is NOT Supported. \n" );
	//--------------------------------//

	return D3DFMT_UNKNOWN;
}

D3DFORMAT ATOM_D3D9Device::getDepthTextureFormat(void) const
{
	return _depthTextureFormat;
}

ATOM_StateCache *ATOM_D3D9Device::getStateCache (void) const
{
	return _stateCache;
}

static inline int _stateValue (int desiredValue, int currentValue, int defaultValue, int pass)
{
	return desiredValue == 0 ? (pass > 0 ? currentValue : defaultValue) : desiredValue;
}

bool ATOM_D3D9Device::applyAlphaBlendState (ATOM_AlphaBlendAttributes *state, int pass, bool force)
{
	ATOM_AlphaBlendAttributes *currentStates = _stateCache->getCurrentStates ()->useAlphaBlending (0);
	ATOM_AlphaBlendAttributes *defaultStates = pass > 0 ? currentStates : _stateCache->getDefaultStates ()->useAlphaBlending (0);

	bool enableBlending = state->isAlphaBlendingEnabled ();
	if (force || currentStates->isAlphaBlendingEnabled() != enableBlending)
	{
		_M_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, enableBlending?TRUE:FALSE);
		currentStates->enableAlphaBlending (enableBlending);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_AlphaBlending);
	}

	if (force || enableBlending)
	{
		ATOM_RenderAttributes::BlendFunc srcBlend = state->getSrcBlend();
		if (srcBlend == ATOM_RenderAttributes::BlendFunc_Unknown)
			srcBlend = defaultStates->getSrcBlend();

		ATOM_RenderAttributes::BlendFunc destBlend = state->getDestBlend();
		if (destBlend == ATOM_RenderAttributes::BlendFunc_Unknown)
			destBlend = defaultStates->getDestBlend();

		if (force || srcBlend != currentStates->getSrcBlend())
		{
			_M_d3ddevice->SetRenderState(D3DRS_SRCBLEND, blendFuncLookupTable[srcBlend]);
			currentStates->setSrcBlend (srcBlend);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_AlphaBlending);
		}
		if (force || destBlend != currentStates->getDestBlend())
		{
			_M_d3ddevice->SetRenderState(D3DRS_DESTBLEND, blendFuncLookupTable[destBlend]);
			currentStates->setDestBlend (destBlend);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_AlphaBlending);
		}

		if (force || (srcBlend >= ATOM_RenderAttributes::BlendFunc_Constant || destBlend >= ATOM_RenderAttributes::BlendFunc_Constant))
		{
			_M_d3ddevice->SetRenderState(D3DRS_BLENDFACTOR, state->getBlendConstant ());
			currentStates->setBlendConstant(state->getBlendConstant());
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_AlphaBlending);
		}
	}

	return true;
}

bool ATOM_D3D9Device::applyColorWriteState (ATOM_ColorWriteAttributes *state, int pass, bool force)
{
	ATOM_ColorWriteAttributes *currentStates = _stateCache->getCurrentStates ()->useColorWrite (0);

	unsigned char writeMask = state->getColorWriteMask ();
	if (force || currentStates->getColorWriteMask () != writeMask)
	{
		DWORD mask = 0;
		if (writeMask&ATOM_RenderAttributes::ColorMask_Red) mask |= D3DCOLORWRITEENABLE_RED;
		if (writeMask&ATOM_RenderAttributes::ColorMask_Green) mask |= D3DCOLORWRITEENABLE_GREEN;
		if (writeMask&ATOM_RenderAttributes::ColorMask_Blue) mask |= D3DCOLORWRITEENABLE_BLUE;
		if (writeMask&ATOM_RenderAttributes::ColorMask_Alpha) mask |= D3DCOLORWRITEENABLE_ALPHA;
		_M_d3ddevice->SetRenderState(D3DRS_COLORWRITEENABLE, mask);

		currentStates->setColorWriteMask (writeMask);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_ColorWrite);
	}

	return true;
}

bool ATOM_D3D9Device::applyDepthState (ATOM_DepthAttributes *state, int pass, bool force)
{
	ATOM_DepthAttributes *currentStates = _stateCache->getCurrentStates ()->useDepth (0);
	ATOM_DepthAttributes *defaultStates = pass > 0 ? currentStates : _stateCache->getDefaultStates ()->useDepth (0);

	bool enableDepthWrite = state->isDepthWriteEnabled ();
	if (force || currentStates->isDepthWriteEnabled() != enableDepthWrite)
	{
		_M_d3ddevice->SetRenderState(D3DRS_ZWRITEENABLE, enableDepthWrite?TRUE:FALSE);
		currentStates->enableDepthWrite(enableDepthWrite);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Depth);
	}

	ATOM_RenderAttributes::CompareFunc depthFunc = state->getDepthFunc ();
	if (depthFunc == ATOM_RenderAttributes::CompareFunc_Unknown)
		depthFunc = defaultStates->getDepthFunc ();

	if (force || currentStates->getDepthFunc() != depthFunc)
	{
		_M_d3ddevice->SetRenderState(D3DRS_ZFUNC, compareFuncLookupTable[depthFunc]);
		currentStates->setDepthFunc(depthFunc);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Depth);
	}

	return true;
}

bool ATOM_D3D9Device::applyStencilState (ATOM_StencilAttributes *state, int pass, bool force)
{
	ATOM_StencilAttributes *currentStates = _stateCache->getCurrentStates ()->useStencil (0);
	ATOM_StencilAttributes *defaultStates = pass > 0 ? currentStates : _stateCache->getDefaultStates ()->useStencil (0);

	bool enableStencil = state->isStencilEnabled();
	if (force || currentStates->isStencilEnabled() != enableStencil)
	{
		_M_d3ddevice->SetRenderState(D3DRS_STENCILENABLE, enableStencil?TRUE:FALSE);
		currentStates->enableStencil (enableStencil);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
	}

	if (force || enableStencil)
	{
		bool twosided = state->isTwoSidedStencilEnabled ();
		if (force || currentStates->isTwoSidedStencilEnabled() != twosided)
		{
			_M_d3ddevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, twosided?TRUE:FALSE);
			currentStates->enableTwoSidedStencil (twosided);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
		}

		unsigned readmask = state->getStencilReadMask ();
		if (force || readmask != currentStates->getStencilReadMask ())
		{
			_M_d3ddevice->SetRenderState(D3DRS_STENCILMASK, readmask);
			currentStates->setStencilReadMask (readmask);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
		}

		unsigned writemask = state->getStencilWriteMask ();
		if (force || writemask != currentStates->getStencilWriteMask())
		{
			_M_d3ddevice->SetRenderState(D3DRS_STENCILWRITEMASK, writemask);
			currentStates->setStencilWriteMask(writemask);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
		}

		int stencilRef = state->getStencilRef ();
		if (force || stencilRef != currentStates->getStencilRef ())
		{
			_M_d3ddevice->SetRenderState(D3DRS_STENCILREF, stencilRef);
			currentStates->setStencilRef (stencilRef);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
		}

		ATOM_RenderAttributes::StencilOp failOpFront = state->getStencilFailOpFront ();
		if (failOpFront == ATOM_RenderAttributes::StencilOp_Unknown)
			failOpFront = defaultStates->getStencilFailOpFront ();

		if (force || failOpFront != currentStates->getStencilFailOpFront())
		{
			_M_d3ddevice->SetRenderState(D3DRS_STENCILFAIL, stencilOpLookupTable[failOpFront]);
			currentStates->setStencilFailOpFront (failOpFront);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
		}

		ATOM_RenderAttributes::StencilOp depthFailOpFront = state->getStencilDepthFailOpFront();
		if (depthFailOpFront == ATOM_RenderAttributes::StencilOp_Unknown)
			depthFailOpFront = defaultStates->getStencilDepthFailOpFront ();

		if (force || depthFailOpFront != currentStates->getStencilDepthFailOpFront())
		{
			_M_d3ddevice->SetRenderState(D3DRS_STENCILZFAIL, stencilOpLookupTable[depthFailOpFront]);
			currentStates->setStencilDepthFailOpFront(depthFailOpFront);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
		}

		ATOM_RenderAttributes::StencilOp passOpFront = state->getStencilPassOpFront ();
		if (passOpFront == ATOM_RenderAttributes::StencilOp_Unknown)
			passOpFront = defaultStates->getStencilPassOpFront ();

		if (force || passOpFront != currentStates->getStencilPassOpFront())
		{
			_M_d3ddevice->SetRenderState(D3DRS_STENCILPASS, stencilOpLookupTable[passOpFront]);
			currentStates->setStencilPassOpFront(passOpFront);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
		}

		ATOM_RenderAttributes::CompareFunc stencilFuncFront = state->getStencilFuncFront ();
		if (stencilFuncFront == ATOM_RenderAttributes::CompareFunc_Unknown)
			stencilFuncFront = defaultStates->getStencilFuncFront ();

		if (force || stencilFuncFront != currentStates->getStencilFuncFront())
		{
			_M_d3ddevice->SetRenderState(D3DRS_STENCILFUNC, compareFuncLookupTable[stencilFuncFront]);
			currentStates->setStencilFuncFront (stencilFuncFront);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
		}

		if (force || twosided)
		{
			ATOM_RenderAttributes::StencilOp failOpBack = state->getStencilFailOpBack ();
			if (failOpBack == ATOM_RenderAttributes::StencilOp_Unknown)
				failOpBack = defaultStates->getStencilFailOpBack ();

			if (force || failOpBack != currentStates->getStencilFailOpBack ())
			{
				_M_d3ddevice->SetRenderState(D3DRS_CCW_STENCILFAIL, stencilOpLookupTable[failOpBack]);
				currentStates->setStencilFailOpBack(failOpBack);
				DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
			}

			ATOM_RenderAttributes::StencilOp depthFailOpBack = state->getStencilDepthFailOpBack ();
			if (depthFailOpBack == ATOM_RenderAttributes::StencilOp_Unknown)
				depthFailOpBack = defaultStates->getStencilDepthFailOpBack ();

			if (force || depthFailOpBack != currentStates->getStencilDepthFailOpBack())
			{
				_M_d3ddevice->SetRenderState(D3DRS_CCW_STENCILZFAIL, stencilOpLookupTable[depthFailOpBack]);
				currentStates->setStencilDepthFailOpBack(depthFailOpBack);
				DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
			}

			ATOM_RenderAttributes::StencilOp passOpBack = state->getStencilPassOpBack ();
			if (passOpBack == ATOM_RenderAttributes::StencilOp_Unknown)
				passOpBack = defaultStates->getStencilPassOpBack ();

			if (force || passOpBack != currentStates->getStencilPassOpBack())
			{
				_M_d3ddevice->SetRenderState(D3DRS_CCW_STENCILPASS, stencilOpLookupTable[passOpBack]);
				currentStates->setStencilPassOpBack (passOpBack);
				DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
			}

			ATOM_RenderAttributes::CompareFunc stencilFuncBack = state->getStencilFuncBack ();
			if (stencilFuncBack == ATOM_RenderAttributes::CompareFunc_Unknown)
				stencilFuncBack = defaultStates->getStencilFuncBack ();

			if (force || stencilFuncBack != currentStates->getStencilFuncBack ())
			{
				_M_d3ddevice->SetRenderState(D3DRS_CCW_STENCILFUNC, compareFuncLookupTable[stencilFuncBack]);
				currentStates->setStencilFuncBack (stencilFuncBack);
				DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Stencil);
			}
		}
	}

	return true;
}

bool ATOM_D3D9Device::applyRasterizerState (ATOM_RasterizerAttributes *state, int pass, bool force)
{
	ATOM_RasterizerAttributes *currentStates = _stateCache->getCurrentStates()->useRasterizer (0);
	ATOM_RasterizerAttributes *defaultStates = pass > 0 ? currentStates : _stateCache->getDefaultStates()->useRasterizer (0);

	ATOM_RenderAttributes::FillMode fillMode = state->getFillMode ();
	if (fillMode == ATOM_RenderAttributes::FillMode_Unknown)
		fillMode = defaultStates->getFillMode ();

	if (force || fillMode != currentStates->getFillMode ())
	{
		_M_d3ddevice->SetRenderState(D3DRS_FILLMODE, fillModeLookupTable[fillMode]);
		currentStates->setFillMode (fillMode);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Rasterizer);
	}

	ATOM_RenderAttributes::FrontFace frontFace = state->getFrontFace();
	if (frontFace == ATOM_RenderAttributes::FrontFace_Unknown)
		frontFace = defaultStates->getFrontFace ();

	ATOM_RenderAttributes::CullMode cullMode = state->getCullMode();
	if (cullMode == ATOM_RenderAttributes::CullMode_Unknown)
		cullMode = defaultStates->getCullMode ();

	DWORD d3dcull = 0;
	if (cullMode == ATOM_RenderAttributes::CullMode_None)
	{
		d3dcull = D3DCULL_NONE;
	}
	else
	{
		switch (frontFace)
		{
		case ATOM_RenderAttributes::FrontFace_CCW:
			switch (cullMode)
			{
			case ATOM_RenderAttributes::CullMode_Back:
				d3dcull = D3DCULL_CW;
				break;
			case ATOM_RenderAttributes::CullMode_Front:
				d3dcull = D3DCULL_CCW;
				break;
			}
			break;

		case ATOM_RenderAttributes::FrontFace_CW:
			switch (cullMode)
			{
			case ATOM_RenderAttributes::CullMode_Back:
				d3dcull = D3DCULL_CCW;
				break;
			case ATOM_RenderAttributes::CullMode_Front:
				d3dcull = D3DCULL_CW;
				break;
			}
			break;
		}
	}

	_M_d3ddevice->SetRenderState(D3DRS_CULLMODE, d3dcull);
	currentStates->setFrontFace (frontFace);
	currentStates->setCullMode (cullMode);
	DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Rasterizer);

	return true;
}

bool ATOM_D3D9Device::applyMultisampleState (ATOM_MultisampleAttributes *state, int pass, bool force)
{
	ATOM_MultisampleAttributes *currentStates = _stateCache->getCurrentStates ()->useMultisample (0);

	bool enableMultisample = state->isMultisampleEnabled ();
	if (force || enableMultisample != currentStates->isMultisampleEnabled())
	{
		_M_d3ddevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, enableMultisample?TRUE:FALSE);
		currentStates->enableMultisample (enableMultisample);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Multisample);
	}

	return true;
}

bool ATOM_D3D9Device::applySamplerState (ATOM_SamplerAttributes *state, int index, int pass, bool force)
{
	int stageIndex = index < ATOM_RenderAttributes::DMapStage_0 ? index : D3DVERTEXTEXTURESAMPLER0 + (index - ATOM_RenderAttributes::DMapStage_0);

	ATOM_SamplerAttributes *currentStates = _stateCache->getCurrentStates ()->useSampler (index, 0);
	ATOM_SamplerAttributes *defaultStates = pass > 0 ? currentStates : _stateCache->getDefaultStates ()->useSampler (index, 0);

	ATOM_D3DTexture *texture = (ATOM_D3DTexture*)state->getTexture();
	if (texture != currentStates->getTexture ())
	{
		if (texture && texture->isLocked ())
		{
			texture = NULL;
		}
		_M_d3ddevice->SetTexture (stageIndex, texture ? texture->getD3DTexture () : NULL);
		currentStates->setTexture (texture);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_Texture);
	}

	if (!texture)
	{
		return true;
	}

	ATOM_RenderAttributes::SamplerFilter filter = state->getFilter ();
	if (filter == ATOM_RenderAttributes::SamplerFilter_Unknown)
		filter = defaultStates->getFilter ();

	if (force || currentStates->getFilter() != filter)
	{
		_M_d3ddevice->SetSamplerState(stageIndex, D3DSAMP_MINFILTER, samplerFilterLookupTable[filter][0]);
		_M_d3ddevice->SetSamplerState(stageIndex, D3DSAMP_MAGFILTER, samplerFilterLookupTable[filter][1]);
		_M_d3ddevice->SetSamplerState(stageIndex, D3DSAMP_MIPFILTER, samplerFilterLookupTable[filter][2]);
		currentStates->setFilter (filter);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_TextureFilter);
	}

	ATOM_RenderAttributes::SamplerAddress addressU = state->getAddressU ();
	if (addressU == ATOM_RenderAttributes::SamplerAddress_Unknown)
		addressU = defaultStates->getAddressU ();

	if (force || addressU != currentStates->getAddressU())
	{
		_M_d3ddevice->SetSamplerState(stageIndex, D3DSAMP_ADDRESSU, samplerAddressLookupTable[addressU]);
		currentStates->setAddressU (addressU);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_TextureAddress);
	}

	ATOM_RenderAttributes::SamplerAddress addressV = state->getAddressV ();
	if (addressV == ATOM_RenderAttributes::SamplerAddress_Unknown)
		addressV = defaultStates->getAddressV ();

	if (force || addressV != currentStates->getAddressV())
	{
		_M_d3ddevice->SetSamplerState(stageIndex, D3DSAMP_ADDRESSV, samplerAddressLookupTable[addressV]);
		currentStates->setAddressV (addressV);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_TextureAddress);
	}

	ATOM_RenderAttributes::SamplerAddress addressW = state->getAddressW ();
	if (addressW == ATOM_RenderAttributes::SamplerAddress_Unknown)
		addressW = defaultStates->getAddressW ();

	if (force || addressW != currentStates->getAddressW())
	{
		_M_d3ddevice->SetSamplerState(stageIndex, D3DSAMP_ADDRESSW, samplerAddressLookupTable[addressW]);
		currentStates->setAddressW (addressW);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_TextureAddress);
	}

	ATOM_ColorARGB borderColor = state->getBorderColor ();
	if (force || borderColor != currentStates->getBorderColor())
	{
		_M_d3ddevice->SetSamplerState(stageIndex, D3DSAMP_BORDERCOLOR, borderColor);
		currentStates->setBorderColor (borderColor);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_SamplerBorderColor);
	}

	bool srgbTexture = state->isSRGBTextureEnabled ();
	if (force || (srgbTexture ^ currentStates->isSRGBTextureEnabled ()))
	{
		_M_d3ddevice->SetSamplerState(stageIndex, D3DSAMP_SRGBTEXTURE, srgbTexture);
		currentStates->enableSRGBTexture (srgbTexture);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_SRGB);
	}

	int MaxLOD = state->getMaxLOD ();
	if (force || MaxLOD != currentStates->getMaxLOD ())
	{
		_M_d3ddevice->SetSamplerState (stageIndex, D3DSAMP_MAXMIPLEVEL, MaxLOD);
		currentStates->setMaxLOD (MaxLOD);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_SamplerMaxLOD);
	}

	unsigned maxAnisotropic = state->getMaxAnisotropic ();
	if (force || maxAnisotropic != currentStates->getMaxAnisotropic ())
	{
		_M_d3ddevice->SetSamplerState (stageIndex, D3DSAMP_MAXANISOTROPY, maxAnisotropic);
		currentStates->setMaxAnisotropic (maxAnisotropic);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_MaxAnisotropic);
	}

	float lodBias = state->getMipmapLodBias ();
	if (force || lodBias != currentStates->getMipmapLodBias ())
	{
		//=== WANGJIAN MODIFIED FOR ATI FETCH4 ===//
		// 如果设备支持使用ATI的FETCH4 [注意：暂时未使用]

#ifndef ATI_FETCH4_VALUE
#define ATI_FETCH4_VALUE 100
#endif //defined(ATI_FETCH4_VALUE)
		if( lodBias == ATI_FETCH4_VALUE )
		{
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
	((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)
		#define FETCH4_ENABLE  ((DWORD)MAKEFOURCC('G', 'E', 'T', '4'))
			_M_d3ddevice->SetSamplerState (stageIndex, D3DSAMP_MIPMAPLODBIAS, FETCH4_ENABLE);
		}
		else
		//==================================//
			_M_d3ddevice->SetSamplerState (stageIndex, D3DSAMP_MIPMAPLODBIAS, *((DWORD*)&lodBias));

		currentStates->setMipmapLodBias (lodBias);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_MipmapLODBias);
	}

	return true;
}

bool ATOM_D3D9Device::applyAlphaTestState (ATOM_AlphaTestAttributes *state, int pass, bool force)
{
	ATOM_AlphaTestAttributes *currentStates = _stateCache->getCurrentStates ()->useAlphaTest (0);
	ATOM_AlphaTestAttributes *defaultStates = pass > 0 ? currentStates : _stateCache->getDefaultStates ()->useAlphaTest (0);

	bool enableAlphaTest = state->isAlphaTestEnabled();
	if (force || enableAlphaTest != currentStates->isAlphaTestEnabled ())
	{
		_M_d3ddevice->SetRenderState(D3DRS_ALPHATESTENABLE, enableAlphaTest?TRUE:FALSE);
		currentStates->enableAlphaTest (enableAlphaTest);
		DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_AlphaTest);
	}

	if (force || enableAlphaTest)
	{
		ATOM_RenderAttributes::CompareFunc alphaFunc = state->getAlphaFunc ();
		if (alphaFunc == ATOM_RenderAttributes::CompareFunc_Unknown)
			alphaFunc = defaultStates->getAlphaFunc ();

		if (force || alphaFunc != currentStates->getAlphaFunc ())
		{
			_M_d3ddevice->SetRenderState(D3DRS_ALPHAFUNC, compareFuncLookupTable[alphaFunc]);
			currentStates->setAlphaFunc (alphaFunc);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_AlphaTest);
		}

		float alphaRef = state->getAlphaRef ();
		if (force || alphaRef != currentStates->getAlphaRef ())
		{
			_M_d3ddevice->SetRenderState(D3DRS_ALPHAREF, 255*alphaRef);
			currentStates->setAlphaRef (alphaRef);
			DBG_MARK_STATE_CHANGE(_stateChangeInfo, StateChange_AlphaTest);
		}
	}

	return true;
}

void ATOM_D3D9Device::loadDefaultRenderStates (void)
{
	getStateCache()->getDesiredStates()->replace (getStateCache()->getDefaultStates());
}

bool ATOM_D3D9Device::isDeviceReady (void) const
{
	return true;//_M_device_ready;
}

void *ATOM_D3D9Device::getInternalDevice (void) const
{
	return _M_d3ddevice;
}

bool ATOM_D3D9Device::setConstantsVS (unsigned startRegister, const void *data, unsigned regCount)
{
	if (_M_d3ddevice)
	{
		HRESULT hr = _M_d3ddevice->SetVertexShaderConstantF (startRegister, (const float*)data, regCount);
		if (SUCCEEDED (hr))
		{
			return true;
		}
	}
	return false;
}

bool ATOM_D3D9Device::setConstantsPS (unsigned startRegister, const void *data, unsigned regCount)
{
	if (_M_d3ddevice)
	{
		HRESULT hr = _M_d3ddevice->SetPixelShaderConstantF (startRegister, (const float*)data, regCount);
		if (SUCCEEDED (hr))
		{
			return true;
		}
	}
	return false;
}
