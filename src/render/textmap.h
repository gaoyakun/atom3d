#ifndef __ATOM_RENDER_TEXTMAP_H
#define __ATOM_RENDER_TEXTMAP_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "textmapmngr.h"
#include "textglyph.h"

class ATOM_RenderDevice;

class TextMap
  {
    friend class FontTextureCallback;
    enum { CACHE_SIZE = TextMapMngr::DEFAULT_TEXTMAP_SIZE * TextMapMngr::DEFAULT_TEXTMAP_SIZE };
    struct glyph {
      short startx;
      short width;
    };
  public:
    enum {
      UPLOAD_OK,
      UPLOAD_ERR_NOSPACE,
      UPLOAD_ERR_UNKNOWN
    };
  public:
    TextMap (ATOM_RenderDevice *device, ATOM_FontHandle font);
    virtual ~TextMap ();
  public:
    inline int GetSize () const;
  public:
    ATOM_Texture *GetTexMap () const;
    int SmartUploadGlyph (int w, int h, int pitch, const unsigned char *alpha, int x, int y, bool mono, ATOM_TextGlyph **textglyph);

  private:
    int _M_size;
    int _M_grid_h;
    int _M_dim_y;
    int _M_margin;
    float _M_tc_offset;
    ATOM_STRING _M_facename;
    ATOM_VECTOR<glyph> *_M_glyphs;
    ATOM_AUTOREF(ATOM_Texture) _M_texmap;
  };

// inline member functions
inline int TextMap::GetSize() const {
  return _M_size;
}

#endif // __ATOM_RENDER_TEXTMAP_H
