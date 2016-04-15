#include "stdafx.h"
#include "text.h"
#include "textrenderer.h"
#include "textmapmngr.h"

extern ATOM_RenderDevice *GetGlobalDevice (void);

class ATOM_RenderDatasGlyph
{
  friend class ATOM_Text;
  friend class ATOM_SegmentRenderCallback;
  struct glyph
  {
    short x1, y1, x2, y2;
    float u1, v1, u2, v2;
  };
public:
  ATOM_RenderDatasGlyph (ATOM_Text *segment);
  void setGlyphCount (int n);
  int getGlyphCount () const;
  void setGlyph (int n, short x1, short y1, short x2, short y2, float u1, float v1, float u2, float v2);
  void setSize (int size);
public:
  void render(ATOM_TextRenderer *renderer, ATOM_RenderDevice* device, ATOM_Texture *texture, int vw, int vh);
public:
  bool _realize(int vw, int vh);
  bool _M_dirty;
  int _M_size;
  ATOM_Text *_M_segment;
  ATOM_VECTOR<ATOM_Text::GlyphVertex> _M_vertices;
  ATOM_VECTOR<ATOM_Text::GlyphVertex> _M_vertices_outline;
  ATOM_VECTOR<glyph> _M_glyphs;
};

static inline void DestroyRenderDatasGlyph (ATOM_RenderDatasGlyph *rd_glyph)
{
  ATOM_DELETE(rd_glyph);
}

ATOM_RenderDatasGlyph::ATOM_RenderDatasGlyph (ATOM_Text *segment)
{
  _M_size = 0;
  _M_dirty = false;
  _M_segment = segment;
}

inline int ATOM_RenderDatasGlyph::getGlyphCount () const
{
  return _M_glyphs.size();
}

inline void ATOM_RenderDatasGlyph::setGlyphCount (int n)
{
  if (_M_size != n)
  {
    _M_size = n;
    _M_glyphs.resize (n);
    _M_vertices.resize (n * 4);
	if (_M_segment->_M_outline_width != 0.f)
	{
		_M_vertices_outline.resize (n * 4 * 4);
	}
	else
	{
		_M_vertices_outline.resize (0);
	}

    _M_dirty = true;
  }
}

inline void ATOM_RenderDatasGlyph::setGlyph (int n, short x1, short y1, short x2, short y2, float u1, float v1, float u2, float v2)
{
  ATOM_ASSERT (n < _M_glyphs.size());
  glyph &g = _M_glyphs[n];

  g.x1 = x1;
  g.y1 = y1;
  g.x2 = x2;
  g.y2 = y2;
  g.u1 = u1;
  g.v1 = v1;
  g.u2 = u2;
  g.v2 = v2;

  _M_dirty = true;
}

inline void ATOM_RenderDatasGlyph::setSize (int size)
{
  setGlyphCount (size);
}

bool ATOM_RenderDatasGlyph::_realize(int vw, int vh)
{
  ATOM_STACK_TRACE(ATOM_RenderDatasGlyph::_realize);
  if (_M_size == 0)
    return false;

  ATOM_ASSERT (_M_glyphs.size() * 4 == _M_vertices.size());

  for (int i = 0; i < _M_glyphs.size(); ++i)
  {
    glyph &g = _M_glyphs[i];
    ATOM_Text::GlyphVertex *v = &_M_vertices[i * 4];
    v->x = g.x1;
    v->y = g.y1;
    v->z = 0.f;
    v->u = g.u1;
    v->v = g.v1;
    v++;
    v->x = g.x1;
    v->y = g.y2;
    v->z = 0.f;
    v->u = g.u1;
    v->v = g.v2;
    v++;
    v->x = g.x2;
    v->y = g.y2;
    v->z = 0.f;
    v->u = g.u2;
    v->v = g.v2;
    v++;
    v->x = g.x2;
    v->y = g.y1;
    v->z = 0.f;
    v->u = g.u2;
    v->v = g.v1;
  }

  if (!_M_vertices_outline.empty ())
  {
	  float outline_offset_x[4] = {
		  -_M_segment->_M_outline_width,
		  0.f,
		  _M_segment->_M_outline_width,
		  0.f
	  };
	  float outline_offset_y[4] = {
		  0.f,
		  -_M_segment->_M_outline_width,
		  0.f,
		  _M_segment->_M_outline_width
	  };

	  for (int n = 0; n < 4; ++n)
	  {
		  for (int i = 0; i < _M_glyphs.size(); ++i)
		  {
			glyph &g = _M_glyphs[i];
			ATOM_Text::GlyphVertex *v = &_M_vertices_outline[n * _M_glyphs.size() * 4 + i * 4];
			v->x = g.x1 + outline_offset_x[n];
			v->y = g.y1 + outline_offset_y[n];
			v->z = 0.f;
			v->u = g.u1;
			v->v = g.v1;
			v++;
			v->x = g.x1 + outline_offset_x[n];
			v->y = g.y2 + outline_offset_y[n];
			v->z = 0.f;
			v->u = g.u1;
			v->v = g.v2;
			v++;
			v->x = g.x2 + outline_offset_x[n];
			v->y = g.y2 + outline_offset_y[n];
			v->z = 0.f;
			v->u = g.u2;
			v->v = g.v2;
			v++;
			v->x = g.x2 + outline_offset_x[n];
			v->y = g.y1 + outline_offset_y[n];
			v->z = 0.f;
			v->u = g.u2;
			v->v = g.v1;
		  }
	  }
  }

  return true;
}

void ATOM_RenderDatasGlyph::render (ATOM_TextRenderer *renderer, ATOM_RenderDevice *device, ATOM_Texture *texture, int vw, int vh)
{
  ATOM_STACK_TRACE(ATOM_RenderDatasGlyph::render);
  if (_M_dirty)
  {
    _M_dirty = false;

    if (!_realize (vw, vh))
    {
      return;
    }
  }

  renderer->AddText (texture, &_M_vertices[0], _M_vertices_outline.empty() ? 0 : &_M_vertices_outline[0], _M_vertices.size());
}

class ATOM_SegmentRenderCallback: public ATOM_FontRenderCallback
  {
  public:
    ATOM_SegmentRenderCallback (ATOM_Text *segment)
      :_M_segment(segment)
    {
    }

    virtual void render (int w, int h, int pitch, const unsigned char *alpha, int x, int y, bool mono, void **userdata);

  private:
    ATOM_Text *_M_segment;
  };

//struct SkinCache
//{
//  ATOM_VECTOR< ATOM_AUTOREF(Skin) > skins;

//  Skin *get (ITexMap *texmap)
//  {
//    for (unsigned i = 0; i < skins.size(); ++i)
//    {
//      if (skins[i]->GetGroup(0)->layers[0] == texmap)
//      {
//        return skins[i].get();
//      }
//    }

//    Skin *skin = ATOM_NEW(Skin);
//    skin->SetTexMapGroupCount(1);
//    skin->GetGroup(0)->SetLayerCount(1);
//    skin->GetGroup(0)->SetLayer(0, texmap);
//    skins.push_back (skin);
//
//    return skin;
//  }
//};

void ATOM_SegmentRenderCallback::render (int w, int h, int pitch, const unsigned char *alpha, int x, int y, bool mono, void **userdata)
{
  ATOM_STACK_TRACE(ATOM_SegmentRenderCallback::render);
  //static SkinCache skinCache;

  ATOM_ASSERT (_M_segment);

  ATOM_TextGlyph *textglyph;

  if (!*userdata)
  {
    if (!TextMapMngr::GetInstance()->UploadGlyph (GetGlobalDevice(), _M_segment->_M_font, w, h, pitch, alpha, x, y, mono, &textglyph))
    {
      return;
    }

    *userdata = textglyph;
  }
  else
  {
    textglyph = (ATOM_TextGlyph*)(*userdata);
  }

  ATOM_RenderDatasGlyph *rdg = 0;
  ATOM_Texture *texmap = 0;

  for (int i = 0; i < _M_segment->_M_texmaps.size(); ++i)
  {
    if (_M_segment->_M_texmaps[i].get() == textglyph->texmap)
    {
      rdg = _M_segment->_M_renderdatas[i];
      texmap = textglyph->texmap.get();
      break;
    }
  }

  if (!texmap)
  {
    _M_segment->_M_texmaps.push_back (textglyph->texmap);

    if (_M_segment->_M_texmaps.size() > _M_segment->_M_renderdatas.size())
    {
      rdg = _M_segment->createRenderDatasGlyph(_M_segment);
      _M_segment->_M_renderdatas.push_back(rdg);
    }
    else
    {
      rdg = _M_segment->_M_renderdatas[_M_segment->_M_texmaps.size() - 1];
    }
  }

  rdg->setSize (rdg->getGlyphCount()+1);
  rdg->setGlyph (rdg->getGlyphCount()-1, x, y, x + w, y + h, textglyph->u1, textglyph->v1, textglyph->u2, textglyph->v2);
}

ATOM_Text::ATOM_Text (void)
{
  ATOM_STACK_TRACE(ATOM_Text::ATOM_Text);
  _M_zvalue = 0.f;
  _M_font = 0;
  _M_margin = 0;
  _M_left = 0;
  _M_top = 0;
  _M_width = 0;
  _M_height = 0;
  _M_limit = -1;
  _M_render_mode = ATOM_RENDER_DEFAULT;
  _M_display_mode = DISPLAYMODE_UNKNOWN;
  _M_saved_viewport[0] = 0;
  _M_saved_viewport[1] = 0;
  _M_bounds_dirty = false;
  _M_dirty = false;
  _M_outline_width = 0;
  _M_renderer = ATOM_NEW(ATOM_TextRenderer);

  setDisplayMode (DISPLAYMODE_2D);
}

ATOM_Text::~ATOM_Text ()
{
  ATOM_STACK_TRACE(ATOM_Text::~ATOM_Text);
  clearBuffers ();

  for (int i = 0; i < _M_renderdatas.size(); ++i)
  {
    DestroyRenderDatasGlyph (_M_renderdatas[i]);
  }

  ATOM_DELETE(_M_renderer);
}

void ATOM_Text::clearBuffers ()
{
  ATOM_STACK_TRACE(ATOM_Text::clearBuffers);
  for (int i = 0; i < _M_texmaps.size(); ++i)
  {
    if (i < _M_renderdatas.size())
      _M_renderdatas[i]->setSize (0);
  }
  _M_texmaps.resize(0);
}

const char *ATOM_Text::getString() const {
  return _M_string.c_str();
}

void ATOM_Text::setString(const char *str)
{
  str = str ? str : "";

  if (_M_string == str)
    return;

  _M_string = str;
  _M_dirty = true;
  _M_bounds_dirty = true;
}

void ATOM_Text::setZValue(float z)
{
	_M_zvalue = z;
}

float ATOM_Text::getZValue() const
{
  return _M_zvalue;
}

ATOM_FontHandle ATOM_Text::getFont () const
{
  return _M_font;
}

void ATOM_Text::setFont (ATOM_FontHandle font)
{
  _M_font = font;
  _M_dirty = true;
  _M_bounds_dirty = true;
}

ATOM_RenderDatasGlyph *ATOM_Text::createRenderDatasGlyph (ATOM_Text *segment) const
{
  ATOM_STACK_TRACE(ATOM_Text::createRenderDatasGlyph);
  return ATOM_NEW(ATOM_RenderDatasGlyph, segment);
}

static ATOM_Texture *GetDefaultTextTexMap (ATOM_RenderDevice *pDevice) {
  ATOM_STACK_TRACE(GetDefaultTextTexMap);
  static const unsigned c = 0xFFFFFFFF;
  ATOM_AUTOREF(ATOM_Texture) texture = pDevice->allocTexture(0, &c, 1, 1, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
  ATOM_ASSERT(texture);
  return texture.get();
}

bool ATOM_Text::realize (void)
{
  ATOM_STACK_TRACE(ATOM_Text::realize);
  if (_M_font == 0) return false;

  int x, y, w, h;
  ATOM_RenderDevice *device = GetGlobalDevice();
  device->getViewport (device->getCurrentView(), &x, &y, &w, &h);
  _M_saved_viewport[0] = w;
  _M_saved_viewport[1] = h;

  if (_M_dirty )
  {
    _M_dirty = false;

    clearBuffers ();
    _M_renderer->Clear ();

    if (!_M_string.empty())
    {
      ATOM_SetCharMargin (_M_margin);
      ATOM_SetRenderMode (_M_render_mode);
      ATOM_SegmentRenderCallback callback(this);
	  int len = _M_string.length();
      ATOM_RenderString (_M_font, _M_string.c_str(), (_M_limit<0?len:ATOM_min2(_M_limit,len)), &callback);

      internalRender (w, h);
    }
  }

  return true;
}

void ATOM_Text::setLimit (int limit)
{
	if (_M_limit != limit)
	{
		_M_limit = limit;
		_M_dirty = true;
		_M_bounds_dirty = true;
	}
}

void ATOM_Text::setCharMargin (int margin)
{
  if (_M_margin != margin)
  {
    _M_margin = margin;
    _M_dirty = true;
    _M_bounds_dirty = true;
  }
}

int ATOM_Text::getCharMargin (void) const
{
  return _M_margin;
}

void ATOM_Text::getBounds (int *left, int *top, int *width, int *height)
{
  ATOM_STACK_TRACE(ATOM_Text::getBounds);
  if (_M_bounds_dirty)
  {
    if (!_M_string.empty())
    {
      ATOM_SetCharMargin (_M_margin);
	  int len = _M_string.length();
      ATOM_CalcStringBounds (_M_font, _M_string.c_str(), (_M_limit<0?len:ATOM_min2(_M_limit,len)), &_M_left, &_M_top, &_M_width, &_M_height);
    }
    else
    {
      _M_left = 0;
      _M_top = 0;
      _M_width = 0;
      _M_height = 0;
    }

    _M_bounds_dirty = false;
  }

  if (left)
    *left = _M_left;
  if (top)
    *top = _M_top;
  if (width)
    *width = _M_width;
  if (height)
    *height = _M_height;
}

void ATOM_Text::setRenderMode (int mode)
{
  if (mode == ATOM_RENDER_MONO || mode == ATOM_RENDER_DEFAULT)
  {
    _M_render_mode = mode;
    _M_dirty = true;
  }
}

int ATOM_Text::getRenderMode (void) const
{
  return _M_render_mode;
}

void ATOM_Text::setDisplayMode (int mode)
{
  if (_M_display_mode != mode)
  {
    _M_dirty = true;
    _M_display_mode = mode;
  }
}

int ATOM_Text::getDisplayMode (void) const
{
  return _M_display_mode;
}

void ATOM_Text::internalRender (int vw, int vh)
{
  ATOM_STACK_TRACE(ATOM_Text::internalRender);
  ATOM_RenderDevice *device = GetGlobalDevice();
  ATOM_ASSERT(device);

  for (int i = 0; i < _M_texmaps.size(); ++i)
  {
    _M_renderdatas[i]->render (_M_renderer, device, _M_texmaps[i].get(), vw, vh);
  }
}

void ATOM_Text::render(int x, int y, ATOM_ColorARGB color)
{
  ATOM_STACK_TRACE(ATOM_Text::render);
  if (realize ())
  {
	ATOM_Vector4f c(color.getFloatR(), color.getFloatG(), color.getFloatB(), color.getFloatA());

	_M_renderer->Render (GetGlobalDevice(), _M_display_mode, c, c, x, y, _M_zvalue, _M_saved_viewport[0], _M_saved_viewport[1], false);
  }
}

void ATOM_Text::renderOutline(int x, int y, ATOM_ColorARGB color, ATOM_ColorARGB colorOutline)
{
	ATOM_STACK_TRACE(ATOM_Text::renderOutline);
	if (1.f != _M_outline_width)
	{
		_M_outline_width = 1.f;
		_M_dirty = true;
	}

	if (realize ())
	{
		ATOM_Vector4f c(color.getFloatR(), color.getFloatG(), color.getFloatB(), color.getFloatA());
		ATOM_Vector4f c2(colorOutline.getFloatR(), colorOutline.getFloatG(), colorOutline.getFloatB(), colorOutline.getFloatA());

		_M_renderer->Render (GetGlobalDevice(), _M_display_mode, c, c2, x, y, _M_zvalue, _M_saved_viewport[0], _M_saved_viewport[1], true);
	}
}

void ATOM_Text::renderShadow(int x, int y, int offsetx, int offsety, ATOM_ColorARGB color, ATOM_ColorARGB colorShadow)
{
	ATOM_STACK_TRACE(ATOM_Text::renderShadow);
	render (x + offsetx, y + offsety, colorShadow);
	render (x, y, color);
}


