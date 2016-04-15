#ifndef __ATOM_FONT_TTF_HANDLE_H
#define __ATOM_FONT_TTF_HANDLE_H

extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
#include FT_CACHE_MANAGER_H
#include FT_STROKER_H
#include FT_SIZES_H
} // extern "C"

#include <ATOM_kernel.h>

#include "charcodec.h"

struct ATOM_FontInfo 
{
  ATOM_STRING name;
  int charset;
  int index;
  int size;
  int reference;
  FT_Face face;
  FT_Int32 loadFlags;
  FT_Render_Mode renderMode;
  CCFUNC_GetNextChar nextCharFunc;
};

inline bool operator == (const ATOM_FontInfo &font1, const ATOM_FontInfo &font2) 
{
  return  font1.size == font2.size && 
          font1.name == font2.name;
}

ATOM_FontInfo * create_font_info (const ATOM_FontInfo &info);
void destroy_font_info (ATOM_FontInfo *info);
bool is_valid_info (ATOM_FontInfo *info);
void purge_all_handles ();

#endif // __ATOM_FONT_TTF_HANDLE_H
