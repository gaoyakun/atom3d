#include <ATOM_dbghlp.h>
#include <ATOM_kernel.h>
#include "fontapi.h"
#include "ttf_handle.h"
#include "charcodec.h"
#include "ttf_impl.h"

ATOM_FONT_API bool ATOM_CALL ATOM_Initialized (void) 
{
  return font_system_initialized ();
}

ATOM_FONT_API ATOM_FontHandle ATOM_CALL ATOM_CreateFont (const char *name, int size, int charset) 
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_CreateFont]: Font system not initialized.\n");
    return 0;
  }
  if (!name || size <= 0 || size > 0xFFFF) 
  {
    ATOM_LOGGER::error ("[ATOM_CreateFont] Invalid arguments: 0x%08X, %d.\n", name, size);
    return 0;
  }

  int faceIndex = find_face_index (name);
  if (faceIndex < 0)
  {
    ATOM_LOGGER::error ("[ATOM_CreateFont] No such font.\n");
    return 0;
  }

  ATOM_FontInfo info;
  info.name = name;
  info.charset = charset;
  info.size = size;
  info.reference = 0;

  if (!create_font (&info))
  {
	  return 0;
  }

  return create_font_info (info);
}

ATOM_FONT_API void ATOM_CALL ATOM_ReleaseFont (ATOM_FontHandle font) 
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_ReleaseFont]: Font system not initialized.\n");
    return;
  }
  destroy_font_info ((ATOM_FontInfo*)font);
}

ATOM_FONT_API bool ATOM_CALL ATOM_LoadFont (const char *name, const char *filename) 
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_LoadFont]: Font system not initialized.\n");
    return false;
  }
  return install_font (name, filename);
}

ATOM_FONT_API int ATOM_CALL ATOM_GetNumFontsInstalled (void) 
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_GetNumFontsInstalled]: Font system not initialized.\n");
    return 0;
  }
  return get_num_fonts ();
}

ATOM_FONT_API int ATOM_CALL ATOM_GetFontHeight (ATOM_FontHandle font) 
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_GetFontHeight]: Font system not initialized.\n");
    return 0;
  }
  ATOM_FontInfo *info = (ATOM_FontInfo*)font;
  if (!is_valid_info (info))
  {
    ATOM_LOGGER::error ("[ATOM_GetFontHeight] Invalidate font handle: 0x%08X.\n", font);
    return 0;
  }
  int face = find_face_index (info->name.c_str());
  ATOM_ASSERT (face >= 0);

  return get_face_height (info);
}

ATOM_FONT_API int ATOM_CALL ATOM_GetFontWidth (ATOM_FontHandle font) 
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_GetFontWidth]: Font system not initialized.\n");
    return 0;
  }
  ATOM_FontInfo *info = (ATOM_FontInfo*)font;
  if (!is_valid_info (info))
  {
    ATOM_LOGGER::error ("[ATOM_GetFontWidth] Invalidate font handle: 0x%08X.\n", font);
    return 0;
  }

  return get_face_width (info);
}

ATOM_FONT_API int ATOM_CALL ATOM_GetFontAscender (ATOM_FontHandle font) 
{
	if (!ATOM_Initialized() && !init_font_system())
	{
		ATOM_LOGGER::error ("[ATOM_GetFontWidth]: Font system not initialized.\n");
		return 0;
	}
	ATOM_FontInfo *info = (ATOM_FontInfo*)font;
	if (!is_valid_info (info))
	{
		ATOM_LOGGER::error ("[ATOM_GetFontWidth] Invalidate font handle: 0x%08X.\n", font);
		return 0;
	}

	return get_face_ascender (info);
}

ATOM_FONT_API void ATOM_CALL ATOM_SetRenderMode (int mode) 
{
	/*
  if (!ATOM_Initialized())
  {
    ATOM_LOGGER::error ("[ATOM_SetRenderMode]: Font system not initialized.\n");
    return;
  }
  switch (mode)
  {
  case ATOM_RENDER_DEFAULT:
    set_antialiased (true);
    break;
  case ATOM_RENDER_MONO:
    set_antialiased (false);
    break;
  }
  */
}

ATOM_FONT_API void ATOM_CALL ATOM_RenderString (ATOM_FontHandle font, const char *str, int len, ATOM_FontRenderCallback *callback) 
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_RenderUnicodeString]: Font system not initialized.\n");
    return;
  }
  ATOM_FontInfo *info = (ATOM_FontInfo*)font;
  if (!is_valid_info (info))
  {
    ATOM_LOGGER::error ("[ATOM_RenderUnicodeString] Invalidate font handle: 0x%08X.\n", font);
    return;
  }

  if (!str || !len || !callback)
    return;

//  set_font_pixel_size (info->size);
  //set_current_font (find_face_index (info->name.c_str()), info->size);

  render_string (info, str, len, 0, 0, callback);
}

ATOM_FONT_API bool ATOM_CALL ATOM_CalcStringBounds (ATOM_FontHandle font, const char *str, int len, int *l, int *t, int *w, int *h) 
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_CalcUnicodeStringBounds]: Font system not initialized.\n");
    return false;
  }
  ATOM_FontInfo *info = (ATOM_FontInfo*)font;
  if (!is_valid_info (info))
  {
    ATOM_LOGGER::error ("[ATOM_CalcUnicodeStringBounds] Invalidate font handle: 0x%08X.\n", font);
    return false;
  }

  if (!str || !len)
    return false;

  //set_font_pixel_size (info->size);
  //set_current_font (find_face_index (info->name.c_str()), info->size);
  return get_string_bounds (info, str, len, l, t, w, h);
}

ATOM_FONT_API int ATOM_CALL ATOM_GetFontSize (ATOM_FontHandle font) 
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_GetFontSize]: Font system not initialized.\n");
    return 0;
  }
  if (!is_valid_info ((ATOM_FontInfo*)font))
  {
    ATOM_LOGGER::error ("[ATOM_GetFontSize] Invalidate font handle: 0x%08X.\n", font);
    return 0;
  }
  return ((ATOM_FontInfo*)font)->size;
}

ATOM_FONT_API unsigned ATOM_CALL ATOM_ClipString (ATOM_FontHandle font, const char *str, unsigned clip_size, int *l, int *t, int *w, int *h)
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_CalcUnicodeStringBounds]: Font system not initialized.\n");
    return 0;
  }
  ATOM_FontInfo *info = (ATOM_FontInfo*)font;
  if (!is_valid_info (info))
  {
    ATOM_LOGGER::error ("[ATOM_CalcUnicodeStringBounds] Invalidate font handle: 0x%08X.\n", font);
    return 0;
  }

  if (!str)
    return 0;

  //set_font_pixel_size (info->size);
  //set_current_font (find_face_index (info->name.c_str()), info->size);
  return clip_string (info, clip_size, str, l, t, w, h);
}

ATOM_FONT_API bool ATOM_CALL ATOM_ClipStringEx (ATOM_FontHandle font, const char *str, unsigned clip_size, ATOM_StringClipInfo *clipinfo)
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_CalcUnicodeStringBounds]: Font system not initialized.\n");
    return 0;
  }
  ATOM_FontInfo *info = (ATOM_FontInfo*)font;
  if (!is_valid_info (info))
  {
    ATOM_LOGGER::error ("[ATOM_CalcUnicodeStringBounds] Invalidate font handle: 0x%08X.\n", font);
    return 0;
  }

  if (!str)
    return 0;

  //set_font_pixel_size (info->size);
  //set_current_font (find_face_index (info->name.c_str()), info->size);
  return clip_string2 (info, clip_size, str, clipinfo);
}

ATOM_FONT_API bool ATOM_CALL ATOM_GetUnderLineInfo (ATOM_FontHandle font, ATOM_UnderLineInfo *underLineInfo)
{
  if (!ATOM_Initialized() && !init_font_system())
	{
		return false;
	}

	if (!ATOM_Initialized())
	{
		ATOM_LOGGER::error ("[ATOM_CalcUnicodeStringBounds]: Font system not initialized.\n");
		return false;
	}

	ATOM_FontInfo *info = (ATOM_FontInfo*)font;
	if (!is_valid_info (info))
	{
		ATOM_LOGGER::error ("[ATOM_CalcUnicodeStringBounds] Invalidate font handle: 0x%08X.\n", font);
		return false;
	}

	get_underline (info, &underLineInfo->underLinePosition, &underLineInfo->underLineThickness);
	return true;
}

ATOM_FONT_API const char * ATOM_CALL ATOM_GetFontName (ATOM_FontHandle font) 
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_GetFontFamilyName]: Font system not initialized.\n");
    return 0;
  }
  if (!is_valid_info ((ATOM_FontInfo*)font))
  {
    ATOM_LOGGER::error ("[ATOM_GetFontFamilyName] Invalidate font handle: 0x%08X.\n", font);
    return 0;
  }
  return ((ATOM_FontInfo*)font)->name.c_str();
}

ATOM_FONT_API void ATOM_CALL ATOM_SetCharMargin (int margin)
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_SetCharMargin]: Font system not initialized.\n");
    return;
  }
  set_char_margin (margin);
}

ATOM_FONT_API int ATOM_CALL ATOM_GetCharMargin (void)
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_SetCharMargin]: Font system not initialized.\n");
    return 0;
  }

  return get_char_margin ();
}

ATOM_FONT_API void *ATOM_CALL ATOM_BeginQueryFaceInfo (const char *fileName, ATOM_FaceInfo *info)
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_BeginQueryFaceInfo]: Font system not initialized.\n");
    return 0;
  }

  return begin_query_face_info (fileName, info);
}

ATOM_FONT_API bool ATOM_CALL ATOM_QueryNextFaceInfo (void *handle, ATOM_FaceInfo *info)
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_QueryNextFaceInfo]: Font system not initialized.\n");
    return false;
  }

  return query_next_face_info (handle, info);
}

ATOM_FONT_API void ATOM_CALL ATOM_EndQueryFaceInfo (void *handle)
{
  if (!ATOM_Initialized() && !init_font_system())
  {
    ATOM_LOGGER::error ("[ATOM_EndQueryFaceInfo]: Font system not initialized.\n");
	return;
  }

  end_query_face_info (handle);
}

