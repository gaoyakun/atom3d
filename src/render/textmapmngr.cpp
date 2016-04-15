#include "stdafx.h"
#include "textmapmngr.h"
#include "textmap.h"

TextMapMngr *TextMapMngr::GetInstance ()
{
  static TextMapMngr instance;
  return &instance;
}

TextMapMngr::TextMapMngr ()
{
}

TextMapMngr::~TextMapMngr ()
{
  for (TextMapDictIter it = _M_textmaps.begin(); it != _M_textmaps.end(); ++it)
    for (unsigned n = 0; n < it->second.size(); ++n)
      ATOM_DELETE(it->second[n]);
}

const ATOM_VECTOR<TextMap *> * TextMapMngr::LookupTextMap (const char *facename, int size) const
{
  if (!facename)
    return 0;

  std::pair<TextMapDictConstIter, TextMapDictConstIter> r = _M_textmaps.equal_range(facename);
  for (TextMapDictConstIter it = r.first; it != r.second; ++it)
  {
    ATOM_ASSERT (!it->second.empty());
    if (it->second[0]->GetSize() == size)
      return &it->second;
  }

  return 0;
}

ATOM_VECTOR<TextMap *> * TextMapMngr::LookupTextMap (const char *facename, int size)
{
  if (!facename)
    return 0;

  std::pair<TextMapDictIter, TextMapDictIter> r = _M_textmaps.equal_range(facename);
  for (TextMapDictIter it = r.first; it != r.second; ++it)
  {
    ATOM_ASSERT (!it->second.empty());
    if (it->second[0]->GetSize() == size)
      return &it->second;
  }

  return 0;
}

ATOM_VECTOR<TextMap*> * TextMapMngr::LookupOrInsertTextMap (ATOM_RenderDevice *device, ATOM_FontHandle font)
{
#if 1
  const char *facename = ATOM_GetFontName (font);
#else
  ATOM_STRING facename = ATOM_GetFontFamilyName(font);
  facename += ' ';
  facename += ATOM_GetFontStyleName(font);
#endif

  int size = ATOM_GetFontSize(font);

  std::pair<TextMapDictIter, TextMapDictIter> r = _M_textmaps.equal_range(facename);
  for (TextMapDictIter it = r.first; it != r.second; ++it)
  {
    ATOM_ASSERT (!it->second.empty());
    if (it->second[0]->GetSize() == size)
      return &it->second;
  }

  TextMap *textmap = ATOM_NEW(TextMap, device, font);
  ATOM_VECTOR<TextMap*> v;
  v.push_back(textmap);
  return &_M_textmaps.insert(std::pair<ATOM_STRING, ATOM_VECTOR<TextMap*> >(facename, v))->second;
}

bool TextMapMngr::UploadGlyph (ATOM_RenderDevice *device, ATOM_FontHandle font, int w, int h, int pitch, const unsigned char *alpha, int x, int y, bool mono, ATOM_TextGlyph **textglyph)
{
  ATOM_VECTOR<TextMap*> * v = LookupOrInsertTextMap (device, font);
  if (v == 0)
    return false;

  for (int i = 0; i < v->size(); ++i)
  {
    int ret = (*v)[i]->SmartUploadGlyph(w, h, pitch, alpha, x, y, mono, textglyph);

    switch (ret)
    {
    case TextMap::UPLOAD_OK:
      return true;

    case TextMap::UPLOAD_ERR_NOSPACE:
      break;

    case TextMap::UPLOAD_ERR_UNKNOWN:
    default:
      return false;
    }
  }

  TextMap *textmap = ATOM_NEW(TextMap, device, font);
  v->push_back(textmap);
  return TextMap::UPLOAD_OK == textmap->SmartUploadGlyph(w, h, pitch, alpha, x, y, mono, textglyph);
}

unsigned TextMapMngr::GetNumTextMaps (void) const
{
	return _M_textmaps.size();
}

