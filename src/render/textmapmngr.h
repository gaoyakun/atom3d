#ifndef __ATOM_RENDER_TEXTMAPMNGR_H
#define __ATOM_RENDER_TEXTMAPMNGR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>
#include <ATOM_utils.h>
#include <ATOM_kernel.h>

#include "textglyph.h"

class TextMap;
class ATOM_RenderDevice;

class TextMapMngr
  {
    typedef ATOM_HASHMULTIMAP<ATOM_STRING, ATOM_VECTOR<TextMap*> > TextMapDict;
    typedef TextMapDict::iterator TextMapDictIter;
    typedef TextMapDict::const_iterator TextMapDictConstIter;
  public:
    enum { DEFAULT_TEXTMAP_SIZE = 512 };
  private:
    TextMapMngr ();
    ~TextMapMngr ();
  public:
    static TextMapMngr *GetInstance ();
  public:
    const ATOM_VECTOR<TextMap*> * LookupTextMap (const char *facename, int size) const;
    ATOM_VECTOR<TextMap*> * LookupTextMap (const char *facename, int size);
    ATOM_VECTOR<TextMap*> * LookupOrInsertTextMap (ATOM_RenderDevice *device, ATOM_FontHandle font);
    bool UploadGlyph (ATOM_RenderDevice *device, ATOM_FontHandle font, int w, int h, int pitch, const unsigned char *alpha, int x, int y, bool mono, ATOM_TextGlyph **textglyph);
	unsigned GetNumTextMaps (void) const;
  private:
    TextMapDict _M_textmaps;
  };

#endif // __ATOM_RENDER_TEXTMAPMNGR_H
