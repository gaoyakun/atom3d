#include "stdafx.h"
#include "textmap.h"
#include "textmapmngr.h"

class TextGlyphAllocator
{
private:
  ATOM_VECTOR<ATOM_TextGlyph*> glyphs;

public:
  ~TextGlyphAllocator (void) {
    for (unsigned i = 0; i < glyphs.size(); ++i)
    {
      ATOM_DELETE(glyphs[i]);
    }
  }

  ATOM_TextGlyph *create (void) { 
    ATOM_TextGlyph *glyph = ATOM_NEW(ATOM_TextGlyph);
    glyphs.push_back (glyph);
    return glyph;
  }
};

static TextGlyphAllocator _TextGlyphAllocator;

TextMap::TextMap (ATOM_RenderDevice * device, ATOM_FontHandle font)
{
  ATOM_ASSERT (device);

  _M_size = ATOM_GetFontSize (font);
  _M_grid_h = (ATOM_GetFontHeight (font) + 7) & ~7;
  _M_margin = 2;
  ATOM_ASSERT (_M_grid_h);
  _M_dim_y = TextMapMngr::DEFAULT_TEXTMAP_SIZE / (_M_grid_h + _M_margin);
  _M_facename = ATOM_GetFontName(font);
  _M_glyphs = ATOM_NEW_ARRAY(ATOM_VECTOR<glyph>, _M_dim_y);

  void *p = ATOM_CALLOC(CACHE_SIZE, 1);
  ATOM_ASSERT (p);
  _M_texmap = device->allocTexture(0, 
                                    p, 
                                    TextMapMngr::DEFAULT_TEXTMAP_SIZE, 
                                    TextMapMngr::DEFAULT_TEXTMAP_SIZE, 
                                    ATOM_PIXEL_FORMAT_A8, 
									ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
  ATOM_FREE (p);
  ATOM_ASSERT (_M_texmap);
}

TextMap::~TextMap ()
{
  ATOM_DELETE_ARRAY(_M_glyphs);
}

ATOM_Texture *TextMap::GetTexMap () const
{
  return _M_texmap.get();
}

int TextMap::SmartUploadGlyph (int w, int h, int pitch, const unsigned char *alpha, int, int, bool mono, ATOM_TextGlyph **textglyph)
{
  // buffer stores the new texture region to be update into _M_texture.
  unsigned char *buffer = const_cast<unsigned char*>(alpha);

  // pitch may be negative
  int upitch = (pitch < 0) ? -pitch : pitch;

  // 8 bytes aligned data (for D3D texture sub-loading)
  int aligned_pitch = mono ? (upitch * 8 + 7) & ~7 : ((upitch + 7) & ~7);

  // v is the target glyph vector to be insert.
  ATOM_VECTOR<TextMap::glyph> *v = 0;

  // x and y stores the position of the new glyph
  int x, y;

  // Find if we have free space to insert the new glyph
  for (int i = 0; i < _M_dim_y; ++i)
  {
    v = &_M_glyphs[i];
    if (v->empty ())
    {
      x = 0;
      y = i * _M_grid_h;
      break;
    }

    x = v->back().startx + v->back().width;
    x = (x + 7) & ~7;
    if (x + aligned_pitch <= TextMapMngr::DEFAULT_TEXTMAP_SIZE)
    {
      y = i * _M_grid_h;
      break;
    }
    v = 0;
  }

  // No free space to insert.
  if (v == 0)
  {
    return UPLOAD_ERR_NOSPACE;
  }

  if (buffer)
  {
    // Texture need to be update.
    if (!mono)
    {
      // The alpha buffer is 8 level gray data.
      // Alpha buffer is not 4 bytes aligned, so we create a new buffer and copy the alpha buffer into it.
      ATOM_ASSERT (h <= _M_grid_h);
      buffer = (unsigned char*)ATOM_STACK_MALLOC(aligned_pitch * _M_grid_h);
      memset (buffer, 0, aligned_pitch * _M_grid_h);
      for (int i = 0; i < h; ++i)
      {
        memcpy (buffer + i * aligned_pitch, alpha + i * pitch, upitch);
      }
      // Update the texture.
      _M_texmap->updateTexImage(buffer, x, y, aligned_pitch, _M_grid_h, ATOM_PIXEL_FORMAT_A8);
    }
    else // mono
    {
      // Alpha buffer is mono buffer, create a new buffer.
      buffer = (unsigned char*)ATOM_STACK_MALLOC(aligned_pitch * _M_grid_h);
      memset (buffer, 0, aligned_pitch * _M_grid_h);
      for (int row = 0; row < h; ++row)
      {
        const unsigned char *src = alpha + row * pitch;
        unsigned char *p = buffer + row * aligned_pitch;
        for (int col = 0; col < upitch; ++col)
        {
          unsigned char ch = *src++;
          for (int i = 0; i < 8; ++i)
          {
            if (ch >> 7)
              *p++ = 255;
            else
              *p++ = 0;
            ch <<= 1;
          }
        }
      }
      // Update the texture and calculate the texture coordinates.
      _M_texmap->updateTexImage (buffer, x, y, aligned_pitch, _M_grid_h, ATOM_PIXEL_FORMAT_A8);
    }
  }

  float u1 = float(x)/float(TextMapMngr::DEFAULT_TEXTMAP_SIZE);
  float v1 = float(y)/float(TextMapMngr::DEFAULT_TEXTMAP_SIZE);
  float u2 = float(x + w)/float(TextMapMngr::DEFAULT_TEXTMAP_SIZE);
  float v2 = float(y + h)/float(TextMapMngr::DEFAULT_TEXTMAP_SIZE);

  if (pitch < 0 && buffer == alpha)
  {
    // It is vertical reversed, adjust the UV coordinates;
    std::swap (v1, v2);
  }

  ATOM_TextGlyph *glf = _TextGlyphAllocator.create ();
  glf->u1 = u1;
  glf->v1 = v1;
  glf->u2 = u2;
  glf->v2 = v2;
  glf->texmap = _M_texmap;

  *textglyph = glf;

  // Insert a new glyph to the vector.
  TextMap::glyph g;
  g.startx = x;
  g.width = w;
  v->push_back (g);

  return UPLOAD_OK;
}

