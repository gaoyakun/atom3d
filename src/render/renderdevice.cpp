#include "stdafx.h"
#include "textmapmngr.h"
#include "texturemanager.h"
#include "vbmanager.h"
#include "ibmanager.h"
#include "defaultcursor.h"
#include "stateset_base.h"

ATOM_RenderDeviceCaps device_caps;

ATOM_CursorInfo::ATOM_CursorInfo(void)
{
	show = true;
	hotspot[0] = 0;
	hotspot[1] = 0;
	region[0] = 0;
	region[1] = 0;
	region[2] = 0;
	region[3] = 0;
	hcursor = 0;
}

ATOM_RenderDevice::ATOM_RenderDevice(void)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::ATOM_RenderDevice);

  _M_view_matrix.makeIdentity();
  _M_world_matrix.makeIdentity();
  _M_projection_matrix.makeIdentity();
  _M_model_view_matrix.makeIdentity();
  _M_mvp_matrix.makeIdentity();
  _M_inv_model_view_matrix.makeIdentity();
  _M_inv_view_matrix.makeIdentity();
  _M_inv_world_matrix.makeIdentity();
  _M_inv_projection_matrix.makeIdentity();
  _M_inv_mvp_matrix.makeIdentity();

  for (unsigned i = 0; i < ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
    _M_texture_matrix[i].makeIdentity();

  _M_mvp_dirty = false;
  _M_model_view_dirty = false;
  _M_inv_world_dirty = false;
  _M_inv_model_view_dirty = false;
  _M_inv_projection_dirty = false;
  _M_inv_mvp_dirty = false;

  _viewportScaleX = 1.f;
  _viewportScaleY = 1.f;
  _viewportTranslateX = 0.f;
  _viewportTranslateY = 0.f;
  _needAdjustProjectionMatrix = false;

  _M_texture_manager = 0;
  _M_vb_manager = 0;
  _M_ib_manager = 0;
  _M_enable_hardware_cursor = true;
  _M_auto_apply_a2c = false;

  _M_resource_list = 0;

  _M_reset_display = false;
  _M_enable_content_backup = false;
  _M_device_active = true;

  _M_resource_dead_tick = 30 * 1000; // 30 seconds
  _M_current_view = 0;
}

ATOM_RenderDevice::~ATOM_RenderDevice() {
  ATOM_STACK_TRACE(ATOM_RenderDevice::~ATOM_RenderDevice);

  _M_render_window = 0;

  if (_M_reset_display)
  {
    ::ChangeDisplaySettings (0, 0);
  }
}

ATOM_AUTOREF(ATOM_RenderWindow) ATOM_RenderDevice::getWindow (void) const 
{
  return _M_render_window;
}

ATOM_RenderWindow *ATOM_RenderDevice::getCurrentView (void) 
{
  return _M_current_view ? _M_current_view->window.get() : 0;
}

ATOM_CursorInfo *ATOM_RenderDevice::getCurrentCursorInfo (void) 
{
  return _M_current_view ? &_M_current_view->cursor_info : 0;
}

void ATOM_RenderDevice::setViewCursor (ATOM_RenderWindow *view) {
  ATOM_STACK_TRACE(ATOM_RenderDevice::setViewCursor);

  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  if (v)
  {
    ATOM_CursorInfo *ci = &v->cursor_info;
    setCustomCursor(view, ci->image.get(), ci->hotspot[0], ci->hotspot[1], ci->region);
    _M_cursor_window = view;
  }
}

void ATOM_RenderDevice::_present_Impl (void) {
  ATOM_STACK_TRACE(ATOM_RenderDevice::_present_Impl);

  ATOM_AUTOREF(ATOM_RenderWindow) w = getCurrentView();
  if (w)
  {
    w->present();
  }
}

void ATOM_RenderDevice::present() {
  ATOM_STACK_TRACE(ATOM_RenderDevice::present);

  _present_Impl ();
}

void ATOM_RenderDevice::setDefaultCursor (ATOM_RenderWindow *view)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::setDefaultCursor);

  ATOM_AUTOREF(ATOM_Image) img = ATOM_HARDREF(ATOM_Image)();
  ATOM_ASSERT (img);
  img->init (defaultCursorWidth, defaultCursorHeight, defaultCursorFormat, defaultCursorBits);
  setCustomCursor (view, img.get(), defaultCursorHotspotX, defaultCursorHotspotY);
}

bool ATOM_RenderDevice::setCustomCursor (ATOM_RenderWindow *view, HCURSOR hCursor)
{
	return false;
}

bool ATOM_RenderDevice::setCustomCursor (ATOM_RenderWindow *view, ATOM_Image *image, int hotspot_x, int hotspot_y, const int *region) {
  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  if (!v)
  {
    return false;
  }
  ATOM_AUTOREF(ATOM_RenderWindow) win = view;
  ATOM_CursorInfo *ci = &v->cursor_info;

  if (image)
  {
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
      ci->region[2] = 0;
      ci->region[3] = 0;
    }

    ci->hotspot[0] = hotspot_x;
    ci->hotspot[1] = hotspot_y;
  }
  else
  {
    setDefaultCursor (view);
  }

  return true;
}

bool ATOM_RenderDevice::setCustomCursor (ATOM_RenderWindow *view, ATOM_Texture *image, int hotspot_x, int hotspot_y, const int *region) {
  return false;
}

void ATOM_RenderDevice::setCursorPosition (int x, int y)
{
  ::SetCursorPos (x, y);
}

ATOM_GfxResource *ATOM_RenderDevice::getResourceList (void) const
{
	return _M_resource_list;
}

void ATOM_RenderDevice::insertResource (ATOM_GfxResource *resource)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::insertResource);

  ATOM_Mutex::ScopeMutex sm(_M_lock);

  if (resource)
  {
    if (!_M_resource_list)
    {
      _M_resource_list = resource;
      resource->_M_prev = resource;
      resource->_M_next = resource;
    }
    else if (_M_resource_list->_M_next == _M_resource_list)
    {
      resource->_M_next = _M_resource_list;
      resource->_M_prev = _M_resource_list;
      _M_resource_list->_M_prev = resource;
      _M_resource_list->_M_next = resource;
      _M_resource_list = resource;
    }
    else
    {
      resource->_M_next = _M_resource_list;
      resource->_M_prev = _M_resource_list->_M_prev;
      _M_resource_list->_M_prev->_M_next = resource;
      _M_resource_list->_M_prev = resource;
      _M_resource_list = resource;
    }
  }
}

void ATOM_RenderDevice::removeAllResources (void)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::removeAllResources);

	ATOM_Mutex::ScopeMutex sm(_M_lock);

  while (_M_resource_list)
    removeResource (_M_resource_list);
}

void ATOM_RenderDevice::removeResource (ATOM_GfxResource *resource)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::removeResource);

	ATOM_Mutex::ScopeMutex sm(_M_lock);

  if (resource && resource->_M_next && resource->_M_prev)
  {
    if (_M_resource_list == resource && _M_resource_list->_M_next == _M_resource_list)
    {
      _M_resource_list = 0;
    }
    else
    {
      resource->_M_prev->_M_next = resource->_M_next;
      resource->_M_next->_M_prev = resource->_M_prev;
      if (resource == _M_resource_list)
      {
        _M_resource_list = _M_resource_list->_M_next;
      }
    }

    resource->_M_prev = 0;
    resource->_M_next = 0;
  }
}

void ATOM_RenderDevice::freeResource (ATOM_GfxResource *resource)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::freeResource);

  if (resource)
  {
    resource->invalidate (false);
    removeResource (resource);
  }
}

void ATOM_RenderDevice::freeAllResources (void)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::freeAllResources);
  ATOM_Mutex::ScopeMutex sm(_M_lock);
  while (_M_resource_list)
  {
    _M_resource_list->setRenderDevice(0);
  }
}

bool ATOM_RenderDevice::screenShot (ATOM_RenderWindow *view, const char *sPath)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::screenShot);

  HCURSOR cursor = ::GetCursor ();
  ::SetCursor (::LoadCursor (NULL, IDC_WAIT));
#if 0
  {
	ATOM_Mutex::ScopeMutex sm(_M_lock);

    ATOM_GfxResource *p = _M_resource_list;
    unsigned n = 0;
    char filename[260];
    ATOM_MountedPathToReal ("/data/texture_dump", filename);
    ::CreateDirectory(filename, 0);

    while (p)
    {
      ATOM_AUTOREF(ATOM_Texture) texture = dynamic_cast<ATOM_Texture*>(p);
      if (texture)
      {
        ATOM_BaseImage image(texture->getWidth(), texture->getHeight(), ATOM_PIXEL_FORMAT_RGBA8888, 0, 1);
        texture->GetTexImage (ATOM_PIXEL_FORMAT_RGBA8888, image.getData());
        sprintf (filename, "/data/texture_dump/texture%d.png", n++);
        image.Save (ATOM_AutoFile(filename, ), ATOM_PIXEL_FORMAT_RGBA8888);
      }
      p = p->_M_next;
      if (p == _M_resource_list)
      {
        break;
      }
    }
    return;
  }
#endif

  char sBuf[512] = {0}; 
  if (sPath == NULL)
  {
    time_t long_time;
    time (&long_time);                

    tm *pTime = localtime (&long_time); 
    sprintf (sBuf, "/screenshot/%d%d%d_%d%d%d.png", pTime->tm_year+1900, pTime->tm_mon+1, 
        pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
  }
  else
  {
    memcpy (sBuf, sPath, sizeof(sBuf));
  }

  ATOM_AUTOREF(ATOM_Image) pImage = ATOM_CreateObject(ATOM_Image::_classname(), 0);
  ATOM_ASSERT (pImage);

  if (!screenShot (view, pImage))
  {
	SetCursor (cursor);
	return false;
  }

  pImage->convertFormat (ATOM_PIXEL_FORMAT_BGR888);
  ATOM_CreateDirectory ("screenshot");
  bool ret = pImage->writeToFile (ATOM_AutoFile(sBuf, ATOM_VFS::write), ATOM_PIXEL_FORMAT_BGR888);

  SetCursor (cursor);

  return ret;
}

void ATOM_RenderDevice::setScissorRect (ATOM_RenderWindow *view, const ATOM_Rect2Di &rc)
{
	ATOM_STACK_TRACE(ATOM_RenderDevice::setScissorRect);

	setScissorRect (view, rc.point.x, rc.point.y, rc.size.w, rc.size.h);
}

void ATOM_RenderDevice::setScissorRect (ATOM_RenderWindow *view, int x, int y, int w, int h)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::setScissorRect);

  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  if (v)
  {
    if (strcmp (getRenderer(), "D3D9") != 0)
    {
      y = v->viewport[3] - y - h;
    }

	int l = x;
	int t = y;
	int r = x + w;
	int b = y + h;

	int vl = 0;
	int vt = 0;
	int vr = v->window->getWindowHints().bounds.first;
	int vb = v->window->getWindowHints().bounds.second;

	if (l < vl) l = vl;
	if (r > vr) r = vr;
	if (t < vt) t = vt;
	if (b > vb) b = vb;

    v->scissor[0] = l;
    v->scissor[1] = t;
    v->scissor[2] = r-l;
    v->scissor[3] = b-t;
	v->scissor2[0] = x;
	v->scissor2[1] = y;
	v->scissor2[2] = w;
	v->scissor2[3] = h;

    if (v == _M_current_view)
    {
      _setScissorRect_Impl (v->scissor);
    }
  }
}

ATOM_Rect2Di ATOM_RenderDevice::getScissorRect (ATOM_RenderWindow *view) const
{
	ATOM_STACK_TRACE(ATOM_RenderDevice::getScissorRect);

	ATOM_Rect2Di ret;
	getScissorRect (view, &ret.point.x, &ret.point.y, &ret.size.w, &ret.size.h);
	return ret;
}

void ATOM_RenderDevice::getScissorRect (ATOM_RenderWindow *view, int *x, int *y, int *w, int *h) const
{
	ATOM_STACK_TRACE(ATOM_RenderDevice::getScissorRect);

	ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
	if (v)
	{
		if (x) *x = v->scissor2[0];
		if (y) *y = v->scissor2[1];
		if (w) *w = v->scissor2[2];
		if (h) *h = v->scissor2[3];
	}
}

void ATOM_RenderDevice::setClearColor(ATOM_RenderWindow *view, float r, float g, float b, float a) {
  ATOM_STACK_TRACE(ATOM_RenderDevice::setClearColor);

  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  if (v)
  {
    v->clearcolor[0] = r;
    v->clearcolor[1] = g;
    v->clearcolor[2] = b;
    v->clearcolor[3] = a;
    v->clearcolor_dirty = true;
  }
}

void ATOM_RenderDevice::getClearColor (ATOM_RenderWindow *view, float &r, float &g, float &b,float &a) const {
  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  if (v)
  {
    r = v->clearcolor[0];
    g = v->clearcolor[1];
    b = v->clearcolor[2];
    a = v->clearcolor[3];
  }
}

ATOM_ColorARGB ATOM_RenderDevice::getClearColor (ATOM_RenderWindow *view) const {
  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  if (v)
  {
	  return ATOM_ColorARGB(v->clearcolor[0], v->clearcolor[1], v->clearcolor[2], v->clearcolor[3]);
  }
  return 0;
}

void ATOM_RenderDevice::setClearColor(ATOM_RenderWindow *view, ATOM_ColorARGB color) 
{
	setClearColor(view, color.getFloatR(), color.getFloatG(), color.getFloatB(), color.getFloatA());
}

void ATOM_RenderDevice::setClearDepth(ATOM_RenderWindow *view, float d) {
  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  if (v)
  {
    v->cleardepth = d;
    v->cleardepth_dirty = true;
  }
}

void ATOM_RenderDevice::setClearStencil(ATOM_RenderWindow *view, int s) {
  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  if (v)
  {
    v->clearstencil = s;
    v->clearstencil_dirty = true;
  }
}

void ATOM_RenderDevice::clear(bool color, bool depth, bool stencil) {
  ATOM_STACK_TRACE(ATOM_RenderDevice::clear);

  if (!_M_current_view)
  {
    return;
  }

  if ( color)
  {
    if ( _M_current_view->clearcolor_dirty)
    {
      _setClearColor_Impl (_M_current_view->clearcolor);
      _M_current_view->clearcolor_dirty = false;
    }
  }

  if ( depth)
  {
    if ( _M_current_view->cleardepth_dirty)
    {
      _setClearDepth_Impl (_M_current_view->cleardepth);
      _M_current_view->cleardepth_dirty = false;
    }
  }

  if ( stencil)
  {
    if ( _M_current_view->clearstencil_dirty)
    {
      _setClearStencil_Impl (_M_current_view->clearstencil);
      _M_current_view->clearstencil_dirty = false;
    }
  }

  _clear_Impl (color, depth, stencil);
}

void ATOM_RenderDevice::setTextureTransform(int stage, const ATOM_Matrix4x4f &matrix) 
{
	ATOM_STACK_TRACE(ATOM_RenderDevice::setTextureTransform);

      _M_texture_matrix[stage] = matrix;
      _setTextureMatrix_Impl (stage, &matrix);
}

void ATOM_RenderDevice::getTextureTransform(int stage, ATOM_Matrix4x4f &matrix) 
{
      matrix = _M_texture_matrix[stage];
}

void ATOM_RenderDevice::setTransform(int mode, const ATOM_Matrix4x4f &matrix) {
  ATOM_STACK_TRACE(ATOM_RenderDevice::setTransform);

  switch ( mode)
  {
  case ATOM_MATRIXMODE_VIEW:
    _M_view_matrix = matrix;
    _M_inv_view_matrix.invertAffineFrom (_M_view_matrix);
	_M_model_view_dirty = true;
    //_M_model_view_matrix = _M_inv_view_matrix >> _M_world_matrix;
	_M_view_projection_matrix = _M_projection_matrix >> _M_inv_view_matrix;
    _M_inv_model_view_dirty = true;
    _M_mvp_dirty = true;
    _M_inv_mvp_dirty = true;
    _setModelViewMatrix_Impl (0, &_M_inv_view_matrix);
    break;

  case ATOM_MATRIXMODE_WORLD:
    _M_world_matrix = matrix;
	_M_model_view_dirty = true;
    //_M_model_view_matrix = _M_inv_view_matrix >> _M_world_matrix;
    _M_inv_world_dirty = true;
    _M_inv_model_view_dirty = true;
    _M_mvp_dirty = true;
    _M_inv_mvp_dirty = true;
    _setModelViewMatrix_Impl (&_M_world_matrix, 0);
    break;

  case ATOM_MATRIXMODE_PROJECTION:
    _M_projection_matrix = matrix;
	if (_needAdjustProjectionMatrix)
	{
		_M_projection_matrix.m00 *= _viewportScaleX;
		_M_projection_matrix.m11 *= _viewportScaleY;
		_M_projection_matrix.m20 += _viewportTranslateX;
		_M_projection_matrix.m21 += _viewportTranslateY;
	}
	_M_view_projection_matrix = _M_projection_matrix >> _M_inv_view_matrix;
    _M_inv_projection_dirty = true;
    _M_mvp_dirty = true;
    _M_inv_mvp_dirty = true;
    _setProjectionMatrix_Impl (&_M_projection_matrix);
    break;

  default:
    ATOM_LOGGER::error ("[ATOM_RenderDevice::setTransform] Invalid mode: %d.\n", mode);
  }
}

void ATOM_RenderDevice::getTransform(int mode, ATOM_Matrix4x4f &matrix) 
{
	ATOM_STACK_TRACE(ATOM_RenderDevice::getTransform);

	switch ( mode)
	{
	case ATOM_MATRIXMODE_WORLD:
		matrix = _M_world_matrix;
		break;

	case ATOM_MATRIXMODE_VIEW:
		matrix = _M_view_matrix;
		break;

	case ATOM_MATRIXMODE_PROJECTION:
		matrix = _M_projection_matrix;
		break;

	case ATOM_MATRIXMODE_INV_VIEW:
		matrix = _M_inv_view_matrix;
		break;

	case ATOM_MATRIXMODE_INV_WORLD:
		if ( _M_inv_world_dirty)
		{
			_M_inv_world_matrix.invertAffineFrom (_M_world_matrix);
			_M_inv_world_dirty = false;
		}
		matrix = _M_inv_world_matrix;
		break;

	case ATOM_MATRIXMODE_INV_PROJECTION:
		if ( _M_inv_projection_dirty)
		{
			_M_inv_projection_matrix.invertFrom (_M_projection_matrix);
			_M_inv_projection_dirty = false;
		}
		matrix = _M_inv_projection_matrix;
		break;

	case ATOM_MATRIXMODE_MVP:
		if ( _M_mvp_dirty)
		{
			_M_mvp_matrix = _M_view_projection_matrix >> _M_world_matrix;
			_M_mvp_dirty = false;
		}
		matrix = _M_mvp_matrix;
		break;

	case ATOM_MATRIXMODE_INV_MVP:
		if ( _M_inv_mvp_dirty)
		{
			if ( _M_mvp_dirty)
			{
				_M_mvp_matrix = _M_view_projection_matrix >> _M_world_matrix;
				_M_mvp_dirty = false;
			}
			_M_inv_mvp_matrix.invertFrom (_M_mvp_matrix);
			_M_inv_mvp_dirty = false;
		}
		matrix = _M_inv_mvp_matrix;
		break;

	case ATOM_MATRIXMODE_INV_MODELVIEW:
		if ( _M_inv_model_view_dirty)
		{
			if (_M_model_view_dirty)
			{
				_M_model_view_matrix = _M_inv_view_matrix >> _M_world_matrix;
				_M_model_view_dirty = false;
			}
			_M_inv_model_view_matrix.invertAffineFrom (_M_model_view_matrix);
			_M_inv_model_view_dirty = false;
		}
		matrix = _M_inv_model_view_matrix;
		break;

	case ATOM_MATRIXMODE_VIEWPROJ:
		matrix = _M_view_projection_matrix;
		break;

	case ATOM_MATRIXMODE_MODELVIEW:
		if (_M_model_view_dirty)
		{
			_M_model_view_matrix = _M_inv_view_matrix >> _M_world_matrix;
			_M_model_view_dirty = false;
		}
		matrix = _M_model_view_matrix;
		break;

	default:
		ATOM_LOGGER::error ("[ATOM_RenderDevice::getTransform] Invalid mode: %d.\n", mode);
	}
}

void ATOM_RenderDevice::pushMatrix(int mode) {
  ATOM_STACK_TRACE(ATOM_RenderDevice::pushMatrix);

  switch ( mode)
  {
  case ATOM_MATRIXMODE_WORLD:
    _M_world_matrix_stack.push_back(_M_world_matrix);
    break;
  case ATOM_MATRIXMODE_VIEW:
    _M_view_matrix_stack.push_back(_M_view_matrix);
    break;
  case ATOM_MATRIXMODE_PROJECTION:
    _M_proj_matrix_stack.push_back(_M_projection_matrix);
    break;
  default:
    ATOM_LOGGER::error("[ATOM_RenderDevice::pushMatrix] Invalid argments: %d.\n", mode);
    break;
  }
}

void ATOM_RenderDevice::popMatrix(int mode) {
  ATOM_STACK_TRACE(ATOM_RenderDevice::popMatrix);

  switch ( mode)
  {
  case ATOM_MATRIXMODE_WORLD:
    if ( _M_world_matrix_stack.empty())
      ATOM_LOGGER::error("[ATOM_RenderDevice::popMatrix] Matrix stack is empty.\n");
    else
    {
      setTransform(mode, _M_world_matrix_stack.back());
      _M_world_matrix_stack.pop_back();
    }
    break;
  case ATOM_MATRIXMODE_VIEW:
    if ( _M_view_matrix_stack.empty())
      ATOM_LOGGER::error("[ATOM_RenderDevice::popMatrix] Matrix stack is empty.\n");
    else
    {
      setTransform(mode, _M_view_matrix_stack.back());
      _M_view_matrix_stack.pop_back();
    }
    break;
  case ATOM_MATRIXMODE_PROJECTION:
    if ( _M_proj_matrix_stack.empty())
    {
      ATOM_LOGGER::error("[ATOM_RenderDevice::popMatrix] Matrix stack is empty.\n");
    }
    else
    {
      setTransform(mode, _M_proj_matrix_stack.back());
      _M_proj_matrix_stack.pop_back();
    }
    break;
  default:
    ATOM_LOGGER::error("[ATOM_RenderDevice::popMatrix] Invalid argments: %d.\n", mode);
    break;
  }
}

void ATOM_RenderDevice::loadIdentity(int mode) {
  setTransform (mode, ATOM_Matrix4x4f::getIdentityMatrix());
}

ATOM_AUTOREF(ATOM_RenderStateSet) ATOM_RenderDevice::allocRenderStateSet (const char *name)
{
	ATOM_AUTOREF(ATOM_RenderStateSetBase) stateset = ATOM_CreateObject (ATOM_RenderStateSetBase::_classname(), name);
	stateset->setDevice (this);
	return stateset;
}

ATOM_AUTOREF(ATOM_RenderStateSet) ATOM_RenderDevice::lookupRenderStateSet (const char *name)
{
	return ATOM_LookupObject (ATOM_RenderStateSetBase::_classname(), name);
}

ATOM_AUTOPTR(ATOM_RenderAttributes) ATOM_RenderDevice::allocRenderAttributes (ATOM_RenderAttributes::RenderState stateType)
{
	ATOM_AUTOPTR(ATOM_RenderAttributes) ret;

	switch (stateType)
	{
	case ATOM_RenderAttributes::RS_AlphaBlending:
		ret = ATOM_NEW(ATOM_AlphaBlendAttributesBase);
		break;
	case ATOM_RenderAttributes::RS_AlphaTest:
		ret = ATOM_NEW(ATOM_AlphaTestAttributesBase);
		break;
	case ATOM_RenderAttributes::RS_ColorWrite:
		ret = ATOM_NEW(ATOM_ColorWriteAttributesBase);
		break;
	case ATOM_RenderAttributes::RS_Depth:
		ret = ATOM_NEW(ATOM_DepthAttributesBase);
		break;
	case ATOM_RenderAttributes::RS_Multisample:
		ret = ATOM_NEW(ATOM_MultisampleAttributesBase);
		break;
	case ATOM_RenderAttributes::RS_Rasterizer:
		ret = ATOM_NEW(ATOM_RasterizerAttributesBase);
		break;
	case ATOM_RenderAttributes::RS_Sampler:
		ret = ATOM_NEW(ATOM_SamplerAttributesBase);
		break;
	case ATOM_RenderAttributes::RS_Stencil:
		ret = ATOM_NEW(ATOM_StencilAttributesBase);
		break;
	default:
		break;
	}

	return ret;
}

ATOM_AUTOREF(ATOM_Texture) ATOM_RenderDevice::allocTexture(const char* name, unsigned flags) 
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::allocTexture);

  if (name && lookupTexture(name))
    return 0;

  ATOM_AUTOREF(ATOM_Texture) pTex;

  if (_M_texture_manager)
    pTex = _M_texture_manager->AllocateTexture(flags, name);

  if (pTex)
  {
    pTex->setRenderDevice(this);
  }

  return pTex;
}

ATOM_AUTOREF(ATOM_Texture) ATOM_RenderDevice::allocTexture(const char* name,
                                  const void* image,
                                  int w,
                                  int h,
                                  ATOM_PixelFormat format,
                                  unsigned flags) 
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::allocTexture);

  if (name && lookupTexture(name))
    return 0;

  ATOM_AUTOREF(ATOM_Texture) pTex;

  if (_M_texture_manager)
    pTex = _M_texture_manager->AllocateTexture(image, w, h, format, flags, name);

  if (pTex)
  {
    pTex->setRenderDevice(this);
    pTex->realize ();
  }

  return pTex;
}

ATOM_AUTOREF(ATOM_Texture) ATOM_RenderDevice::allocVolumeTexture(const char* name,
                                  const void* image,
                                  int w,
                                  int h,
								  int d,
                                  ATOM_PixelFormat format,
                                  unsigned flags) 
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::allocVolumeTexture);

  if (name && lookupTexture(name))
    return 0;

  ATOM_AUTOREF(ATOM_Texture) pTex;

  if (_M_texture_manager)
    pTex = _M_texture_manager->AllocateVolumeTexture(image, w, h, d, format, flags, name);

  if (pTex)
  {
    pTex->setRenderDevice(this);
    pTex->realize ();
  }

  return pTex;
}

ATOM_AUTOREF(ATOM_Texture) ATOM_RenderDevice::allocCubeTexture (const char* name, const void* image[6], int size, 
                                                      ATOM_PixelFormat format, unsigned flags)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::allocCubeTexture);

  if (name && lookupTexture(name))
    return ATOM_AUTOREF(ATOM_Texture)();

  ATOM_AUTOREF(ATOM_Texture) pTex;

  if (_M_texture_manager)
    pTex = _M_texture_manager->AllocateCubeTexture (image, size, format, flags, name);

  if (pTex)
  {
	  pTex->setRenderDevice (this);
	  pTex->realize();
  }

  return pTex;
}

//--- wangjian added for DX9 Depth stencil format ---//
// * 新增参数 bool bUseDX9DSFormat
// * 用以某些特殊情况时 需要生成DX9 DEPTH STENCIL FORMAT类型的DEPTH TEXTURE.
// * 现在用于shadow mapping的硬件PCF.
ATOM_AUTOREF(ATOM_Texture) ATOM_RenderDevice::allocDepthTexture (unsigned width, unsigned height, bool bUseDX9DSFormat/*=false*//*wangjian added*/)
{
  ATOM_AUTOREF(ATOM_Texture) pTex;

  if (_M_texture_manager)
    pTex = _M_texture_manager->AllocateDepthTexture (width, height,bUseDX9DSFormat);

  if (pTex)
  {
	  pTex->setRenderDevice (this);
	  pTex->realize();
  }

  return pTex;
}
//-------------------------------------------------//

ATOM_AUTOREF(ATOM_Texture) ATOM_RenderDevice::lookupTexture(const char* name) {
  ATOM_STACK_TRACE(ATOM_RenderDevice::lookupTexture);

  return _M_texture_manager ? _M_texture_manager->LookupTexture(name) : ATOM_AUTOREF(ATOM_Texture)();
}

void ATOM_RenderDevice::setViewport(ATOM_RenderWindow *view, int x, int y, int w, int h, float depthMin, float depthMax) {
  ATOM_STACK_TRACE(ATOM_RenderDevice::setViewport);

  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  if (v)
  {
    v->viewport[0] = x;
    v->viewport[1] = y;
    v->viewport[2] = w;
    v->viewport[3] = h;
	v->depthMin = depthMin;
	v->depthMax = depthMax;

    if (v == _M_current_view)
    {
      _setViewport_Impl (v->viewport, depthMin, depthMax);
    }
  }
}

void ATOM_RenderDevice::setViewport(ATOM_RenderWindow *view, const ATOM_Rect2Di &viewport, float depthMin, float depthMax) {
	setViewport (view, viewport.point.x, viewport.point.y, viewport.size.w, viewport.size.h, depthMin, depthMax);
}

void ATOM_RenderDevice::getViewport(ATOM_RenderWindow *view, int* x, int* y, int* w, int* h, float *depthMin, float *depthMax) const {
  ATOM_STACK_TRACE(ATOM_RenderDevice::getViewport);

  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  if (v)
  {
    if (x) *x = v->viewport[0];
    if (y) *y = v->viewport[1];
    if (w) *w = v->viewport[2];
    if (h) *h = v->viewport[3];
	if (depthMin) *depthMin = v->depthMin;
	if (depthMax) *depthMax = v->depthMax;
  }
}

ATOM_Rect2Di ATOM_RenderDevice::getViewport(ATOM_RenderWindow *view, float *depthMin, float *depthMax) const {
	ATOM_STACK_TRACE(ATOM_RenderDevice::getViewport);

	ATOM_Rect2Di rc;
	getViewport (view, &rc.point.x, &rc.point.y, &rc.size.w, &rc.size.h, depthMin, depthMax);
	return rc;
}

bool ATOM_RenderDevice::beginFrame() 
{
  return true;
}

void ATOM_RenderDevice::endFrame() {
}

bool ATOM_RenderDevice::showCustomCursor (ATOM_RenderWindow *view, bool show)
{
  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  if (!v)
  {
    return false;
  }
  ATOM_CursorInfo *ci = &v->cursor_info;
  bool ret = ci->show;
  ci->show = show;
  return ret;
}

bool ATOM_RenderDevice::isCustomCursorShown (ATOM_RenderWindow *view) const {
  ATOM_RenderView *v = view ? view->getRenderView() : _M_current_view;
  return v ? v->cursor_info.show : false;
}

unsigned ATOM_RenderDevice::getNumDrawCalls () const 
{
  return 0;
}

unsigned ATOM_RenderDevice::getNumPrimitivesDrawn (void) const
{
	return 0;
}

ATOM_AUTOREF(ATOM_VertexArray) ATOM_RenderDevice::allocVertexArray(unsigned attrib, int usage, unsigned size, bool realize, unsigned attribFlags/*=0*/) {
  ATOM_STACK_TRACE(ATOM_RenderDevice::allocVertexArray);

  ATOM_AUTOREF(ATOM_VertexArray) v;
  
  if (_M_vb_manager)
    v = _M_vb_manager->allocVertexArray(attrib, usage, size, attribFlags);

  if (v && realize && !v->realize())
  {
    return 0;
  }

  return v;
}

ATOM_AUTOREF(ATOM_IndexArray) ATOM_RenderDevice::allocIndexArray(int usage, unsigned size, bool use32bit, bool realize) {
  ATOM_STACK_TRACE(ATOM_RenderDevice::allocIndexArray);

  ATOM_AUTOREF(ATOM_IndexArray) i;
  
  if (_M_ib_manager)
    i = _M_ib_manager->allocIndexArray(usage, size, use32bit);

  if (i && realize)
    i->realize ();

  return i;
}

const ATOM_RenderDeviceCaps & ATOM_RenderDevice::getCapabilities () const {
  return device_caps;
}

void ATOM_RenderDevice::syncRenderStates(void) {
}

void ATOM_RenderDevice::evictManagedResource (void) 
{
  return;
}

bool ATOM_RenderDevice::addView (ATOM_RenderWindow *window)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::addView);

  if (window)
  {
    if (window->getRenderView())
    {
      return true;
    }

    if (window->attachDevice(this))
    {
      ATOM_RenderView *rv = ATOM_NEW(ATOM_RenderView);
      rv->window = window;
	  rv->viewport[0] = 0;
	  rv->viewport[1] = 0;
	  rv->viewport[2] = window->getWindowHints().bounds.first;
	  rv->viewport[3] = window->getWindowHints().bounds.second;
	  rv->depthMin = 0.f;
	  rv->depthMax = 1.f;
      window->_M_view = rv;

      _M_views.push_back (rv);

      if (!_M_current_view)
      {
        setCurrentView (window);
      }

      return true;
    }
  }

  return false;
}

void ATOM_RenderDevice::removeView (ATOM_RenderWindow *window)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::removeView);

  ATOM_AUTOREF(ATOM_RenderWindow) w = window;

  for (unsigned i = 0; i < _M_views.size(); ++i)
  {
    if (_M_views[i]->window.get() == window)
    {
      window->detachDevice();

      if (getCurrentView() == window)
      {
        _M_current_view = 0;
      }

      ATOM_DELETE(_M_views[i]);

      _M_views.erase (_M_views.begin() + i);

      if (!_M_current_view && !_M_views.empty())
      {
        setCurrentView (_M_views[0]->window.get());
      }

      window->_M_view = 0;

      return;
    }
  }
}

void ATOM_RenderDevice::setCurrentView (ATOM_RenderWindow *window)
{
  ATOM_STACK_TRACE(ATOM_RenderDevice::setCurrentView);

  if (window)
  {
    ATOM_RenderView *v = window->getRenderView();
    if (!v)
    {
      return;
    }

    if (!getRenderTarget(0))
    {
      window->setCurrent();
    }

    _M_current_view = v;

    _setClearColor_Impl (v->clearcolor);
    v->clearcolor_dirty = false;

    _setClearDepth_Impl (v->cleardepth);
    v->cleardepth_dirty = false;

    _setClearStencil_Impl (v->clearstencil);
    v->clearstencil_dirty = false;

	_setViewport_Impl (v->viewport, v->depthMin, v->depthMax);
    _setScissorRect_Impl (v->scissor);

    enableScissorTest (window, v->enable_scissor_test);
 }
}

bool ATOM_RenderDevice::isMultisampleSupported(void)
{
  return _M_render_window ? _M_render_window->isMultisampleSupported() : false;
}

bool ATOM_RenderDevice::isSampleSupported(unsigned sample)
{
  return _M_render_window ? _M_render_window->isSampleSupported(sample) : false;
}

bool ATOM_RenderDevice::setup (ATOM_DisplayMode *fullscreen_mode, const char *title, int w, int h, ATOM_RenderWindow::ShowMode showMode, bool naked, bool resizable, int multisample, void *windowid, void *parentid, void *iconid, bool contentBackup, void *instance)
{
	ATOM_STACK_TRACE(ATOM_RenderDevice::setup);

	if (fullscreen_mode)
	{
		DEVMODE desktopMode;
		::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &desktopMode);

		if (desktopMode.dmPelsWidth != fullscreen_mode->width
			|| desktopMode.dmPelsHeight != fullscreen_mode->height
			|| desktopMode.dmBitsPerPel < fullscreen_mode->depth
			|| desktopMode.dmDisplayFrequency < fullscreen_mode->freq)
		{
			DEVMODE mode;
			DEVMODE sel;
			int i = 0;
			memset(&sel, 0, sizeof(sel));

			while ( ::EnumDisplaySettings(0, i++, &mode))
			{
				if ( mode.dmPelsWidth == fullscreen_mode->width && mode.dmPelsHeight == fullscreen_mode->height)
				{
					if (mode.dmBitsPerPel < fullscreen_mode->depth || mode.dmDisplayFrequency < fullscreen_mode->freq)
					{
						continue;
					}

					if (mode.dmDisplayFrequency >= sel.dmDisplayFrequency)
					{
						sel = mode;

						if (mode.dmDisplayFrequency == fullscreen_mode->freq)
						{
							break;
						}
					}
				}
			}

			if (sel.dmPelsWidth == 0)
			{
				return false;
			}

			sel.dmSize = sizeof(sel);
			sel.dmDriverExtra = 0;
			sel.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

			if ( DISP_CHANGE_SUCCESSFUL != ChangeDisplaySettings(&sel, CDS_FULLSCREEN | CDS_SET_PRIMARY))
			{
				sel.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				if ( DISP_CHANGE_SUCCESSFUL != ChangeDisplaySettings(&mode, CDS_FULLSCREEN | CDS_SET_PRIMARY))
				{
					return false;
				}
			}

			_M_reset_display = true;
		}
	}

	_M_enable_content_backup = contentBackup;
	return true;
}

void ATOM_RenderDevice::getViewPoint (float *v) const
{
	v[0] = _M_view_matrix.m30;
	v[1] = _M_view_matrix.m31;
	v[2] = _M_view_matrix.m32;
}

bool ATOM_RenderDevice::dumpTextureStats (const char *filename)
{
	return _M_texture_manager ? _M_texture_manager->DumpTextureStats (filename) : false;
}

unsigned ATOM_RenderDevice::getNumTextMaps (void) const
{
	return TextMapMngr::GetInstance()->GetNumTextMaps();
}

bool ATOM_RenderDevice::isContentBackupEnabled (void) const
{
	return _M_enable_content_backup;
}

void ATOM_RenderDevice::activate (bool active)
{
	_M_device_active = active;
}

bool ATOM_RenderDevice::isActive (void) const
{
	return _M_device_active;
}


