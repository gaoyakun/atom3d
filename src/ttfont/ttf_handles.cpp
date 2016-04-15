#include <algorithm>
#include <vector>
#include <ATOM_dbghlp.h>
#include "ttf_handle.h"
#include "ttf_impl.h"
#include "charcodec.h"

static ATOM_VECTOR<ATOM_FontInfo*> font_handles_pool;

ATOM_FontInfo * create_font_info (const ATOM_FontInfo &info) 
{
  int slot = -1;
  for (unsigned i = 0; i < font_handles_pool.size(); ++i)
  {
    if (!font_handles_pool[i])
      slot = int(i);
    else if (info == *font_handles_pool[i])
    {
      font_handles_pool[i]->reference++;
      return font_handles_pool[i];
    }
  }
  if (slot < 0)
  {
    font_handles_pool.push_back (ATOM_NEW(ATOM_FontInfo, info));
    font_handles_pool.back()->reference = 1;
    return font_handles_pool.back();
  }
  else
  {
    font_handles_pool[slot] = ATOM_NEW(ATOM_FontInfo, info);
    font_handles_pool[slot]->reference = 1;
    return font_handles_pool[slot];
  }
}

void destroy_font_info (ATOM_FontInfo *info) 
{
  for (unsigned i = 0; i < font_handles_pool.size(); ++i)
  {
    if (font_handles_pool[i] == info)
    {
      --info->reference;
      if (!info->reference)
      {
        ATOM_DELETE(info);
        font_handles_pool[i] = 0;
      }
    }
  }
}

bool is_valid_info (ATOM_FontInfo *info) 
{
  return info ? std::find (font_handles_pool.begin(), font_handles_pool.end(), info) != font_handles_pool.end() : false;
}

void purge_all_handles (void) 
{
  for (unsigned i = 0; i < font_handles_pool.size(); ++i)
    ATOM_DELETE(font_handles_pool[i]);

  font_handles_pool.clear();
}

