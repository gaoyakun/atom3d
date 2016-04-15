#include <ATOM_dbghlp.h>
#include <ATOM_vfs.h>
#include <ATOM_utils.h>
#include "ttf_impl.h"
#include "ttf_handle.h"
#include "fontapi.h"

#define USE_FT_CACHE 0

#ifdef FT_FLOOR
#  undef FT_FLOOR
#endif
#define FT_FLOOR(X)     (((X) & -64) / 64)

#ifdef FT_CEIL
#  undef FT_CEIL
#endif
#define FT_CEIL(X)      ((((X) + 63) & -64) / 64)

struct glyphinfo {
  FT_BitmapGlyph glyph;
  int size;
  void *userdata;
};

typedef ATOM_HASHMULTIMAP<ATOM_UNICC, glyphinfo> GlyphCache;
typedef GlyphCache::iterator GlyphCacheIter;
typedef GlyphCache::const_iterator GlyphCacheConstIter;

static bool initialized = false;  // Is freetype initialized
static int char_margin = 4;       // The char margin

static FT_Library       library;
static FT_Error  error;

ATOM_HASHMAP<ATOM_STRING, ATOM_File*> fileHandles;

#define MAX_FIXED_SIZE_COUNT 32

// Our face descriptor
struct FaceID
{
  FT_Stream stream;
  int index;
  bool support_unicode;
  int support_charset[ATOM_CC_END];
  int num_fixed_sizes;
  int fixed_sizes[MAX_FIXED_SIZE_COUNT];
  int underline_pos;
  int underline_thickness;
  bool has_kerning;
  ATOM_STRING name;
  GlyphCache *glyph_cache;
};

typedef ATOM_VECTOR<FaceID> FontNameSet;
typedef FontNameSet::iterator FontNameSetIter;
typedef FontNameSet::const_iterator FontNameSetConstIter;

// All fonts currently installed.
static FontNameSet installed_fonts;


// Callback IO function for ATOM_File used in freetype to open a face.
static unsigned long IO_func (FT_Stream stream, unsigned long offset, unsigned char *buffer, unsigned long count) {
  char *p = (char*)stream->descriptor.pointer;
  ATOM_File *file = (ATOM_File*)stream->descriptor.pointer;
  ATOM_ASSERT (file);
  file->seek (offset, SEEK_SET);
  return count ? file->read (buffer, count) : 0;
}

// Callback close function for ATOM_File used in freetype to open a face.
static void close_func (FT_Stream stream)
  {
}

static bool select_unicode_charset (FT_Face face)
{
  return !FT_Select_Charmap (face, FT_ENCODING_UNICODE);
}

static bool select_charset (int charset, FT_Face face)
{
  switch (charset)
  {
  case ATOM_CC_CP936:
    return !FT_Select_Charmap (face, FT_ENCODING_GB2312);
  case ATOM_CC_CP950:
    return !FT_Select_Charmap (face, FT_ENCODING_BIG5);
  default:
    return false;
  }
}

// Our face query callback function for freetype cache manager to lookup a face.
FT_CALLBACK_DEF( FT_Error )
my_face_requester(FTC_FaceID  face_id,
                  FT_Library  lib,
                  FT_Pointer  request_data,
                  FT_Face*    aface )
{
  int index = (int)face_id;
  ATOM_ASSERT (index >= 0 && index < int(installed_fonts.size()));

  FT_UNUSED (request_data);

  const FaceID &id = installed_fonts[index];

  FT_Open_Args_ open_args;
  memset (&open_args, 0, sizeof(open_args));
  open_args.flags = FT_OPEN_STREAM;
  open_args.stream = id.stream;

  return FT_Open_Face (library, &open_args, 0, aface);
}

#if USE_FT_CACHE
// Get the bitmap of a glyph
static unsigned char * glyph_to_bitmap (FT_Glyph glyf, int *width, int *height, int *pitch, 
                                        int *left, int *top, int *advance_x, int *advance_y, FT_Pointer *ptr)
{
  ATOM_ASSERT (width);
  ATOM_ASSERT (height);
  ATOM_ASSERT (pitch);
  ATOM_ASSERT (left);
  ATOM_ASSERT (top);
  ATOM_ASSERT (advance_x);
  ATOM_ASSERT (advance_y);

  FT_BitmapGlyph bitmap;
  FT_Bitmap *source;

  if (glyf->format == ft_glyph_format_outline)
  {
    error = FT_Glyph_To_Bitmap (&glyf, mono ? FT_RENDER_MODE_MONO : FT_RENDER_MODE_NORMAL, 0, 0);
    if (error)
      return 0;
  }

  ATOM_ASSERT (glyf->format == FT_GLYPH_FORMAT_BITMAP);

  *ptr = glyf;
  bitmap = (FT_BitmapGlyph)glyf;

  source = &bitmap->bitmap;

  *height = source->rows;
  *width = source->width;
  *pitch = source->pitch;
  *left = bitmap->left;
  *top = bitmap->top;
  *advance_x = (glyf->advance.x + 0x8000) >> 16;
  *advance_y = (glyf->advance.y + 0x8000) >> 16;

  return source->buffer;
}

// Get the bitmap of a character index
static unsigned char *get_glyph_bitmap (FT_ULong index, int *width, int *height, int *pitch, 
                                        int *left, int *top, int *advance_x, int *advance_y, FT_Pointer *ptr)
{
  index = FTC_CMapCache_Lookup( cmap_cache, (FTC_FaceID)current_font_itr.face_id, 0, index );
  
  if (current_font_itr.width < 48 && current_font_itr.height < 48)
  {
    FTC_SBit sbit;

    error = FTC_SBitCache_Lookup (sbits_cache, &current_font_itr, index, &sbit, 0);
    if (error)
      return 0;

    if (sbit->buffer)
    {
      *height = sbit->height;
      *width = sbit->width;
      *pitch = sbit->pitch;
      *left = sbit->left;
      *top = sbit->top;
      *advance_x = sbit->xadvance;
      *advance_y = sbit->yadvance;

      return sbit->buffer;
    }
  }

  FT_Glyph glyf;
  error = FTC_ImageCache_Lookup (image_cache, &current_font_itr, index, &glyf, 0);
  return error ? 0 :glyph_to_bitmap (glyf, width, height, pitch, left, top, advance_x, advance_y, ptr);
}
#endif
// Initialize the freetype font system.
bool init_font_system () {
  if (!initialized)
  {
    error = FT_Init_FreeType (&library);
    ATOM_ASSERT (!error);

    initialized = true;
  }

  return initialized;
}

// Shutdown the freetype font system.
void shutdown_font_system () {
  if (initialized)
  {
    for (unsigned i = 0; i < installed_fonts.size(); ++i)
	{
      for (GlyphCacheIter it = installed_fonts[i].glyph_cache->begin(); it != installed_fonts[i].glyph_cache->end(); ++it)
	  {
        FT_Done_Glyph ((FT_Glyph)it->second.glyph);
	  }
	}
    FT_Done_FreeType (library);		

    for (unsigned i = 0; i < installed_fonts.size(); ++i)
    {			
		ATOM_DELETE(installed_fonts[i].stream);
		ATOM_DELETE(installed_fonts[i].glyph_cache);
    }
    installed_fonts.clear ();

	for (ATOM_HASHMAP<ATOM_STRING, ATOM_File*>::iterator it = fileHandles.begin(); it != fileHandles.end(); ++it)
	{
		ATOM_CloseFile (it->second);
	}
	fileHandles.clear ();

    initialized = false;
  }
}

// Install a certern font from stream.
bool install_font (const char *name, ATOM_File *fontfile) 
{
	if (!fontfile) 
	{
		ATOM_LOGGER::error ("[install_font] Invalid font file: 0x%08X.\n", fontfile);
		return false;
	}

	FT_StreamRec *stream = ATOM_NEW(FT_StreamRec);
	FT_Open_Args_ open_args;

	memset (stream, 0, sizeof(*stream));
	stream->size = fontfile->size();
	stream->pos = 0;
	stream->descriptor.pointer = fontfile;
	stream->read = IO_func;
	stream->close = close_func;

	memset (&open_args, 0, sizeof(open_args));
	open_args.flags = FT_OPEN_STREAM;
	open_args.stream = stream;

	FT_Face face;

	// Try opening the first face in the font stream.
	error = FT_Open_Face (library, &open_args, 0, &face);
	if (error)
	{
		ATOM_LOGGER::error ("[install_font] Couldn't open font.\n");
		return false;
	}

	FaceID face_id;

    if (select_unicode_charset (face))
    {
		face_id.support_unicode = true;
    }

    // The font must be scalable.
    if (!FT_IS_SCALABLE (face))
    {
		ATOM_LOGGER::error ("[install_font] Font is not scalable.\n");
		return false;
    }

    for (int charset = 0; charset < ATOM_CC_END; ++charset)
    {
		face_id.support_charset[charset] = select_charset (charset, face);
    }

	face_id.num_fixed_sizes = face->num_fixed_sizes;
	if (face_id.num_fixed_sizes > MAX_FIXED_SIZE_COUNT)
	{
		face_id.num_fixed_sizes = MAX_FIXED_SIZE_COUNT;
	}
	for (int s = 0; s < face_id.num_fixed_sizes; ++s)
	{
		face_id.fixed_sizes[s] = face->available_sizes[s].height;
	}

    // Store this font.
    face_id.name = name ? name : "";
    face_id.index = 0;
	face_id.underline_pos = face->underline_position;
	face_id.underline_thickness = face->underline_thickness;
    face_id.stream = stream;
	face_id.has_kerning = FT_HAS_KERNING(face);
    face_id.glyph_cache = ATOM_NEW(GlyphCache);
    installed_fonts.push_back (face_id);

    FT_Done_Face (face);

	return true;
}

// Install a certern font from stream.
bool install_font (const char *name, const char *filename) {
  if (!filename) 
  {
    ATOM_LOGGER::error ("[install_font] Invalid font file: 0x%08X.\n", filename);
    return false;
  }

  ATOM_File *fontFile = 0;
  bool newFile = false;
  char ident[ATOM_VFS::max_filename_length];
  if (filename[1] == ':')
  {
	  strcpy (ident, filename);
  }
  else
  {
	ATOM_CompletePath (filename, ident);
  }
  _strlwr (ident);
  ATOM_HASHMAP<ATOM_STRING, ATOM_File*>::iterator it = fileHandles.find (ident);
  if (it != fileHandles.end ())
  {
	  fontFile = it->second;
  }
  else
  {
	fontFile = ATOM_OpenFile (ident, ATOM_VFS::read|ATOM_VFS::binary);
	if (!fontFile)
	{
		ATOM_LOGGER::error ("Open file <%s> failed!\n", ident);
		return false;
	}
	newFile = true;
  }

  if (install_font (name, fontFile))
  {
	  if (newFile)
	  {
		fileHandles[ident] = fontFile;
	  }
	  return true;
  }

  return false;
}

// Get how many fonts currently installed.
int get_num_fonts () {
  return installed_fonts.size();
}

bool create_font (ATOM_FontInfo *info)
{
	if (!info)
	{
		return false;
	}

	int index = find_face_index (info->name.c_str());
	if (index < 0)
	{
		return false;
	}

	const FaceID &id = installed_fonts[index];

	FT_Open_Args_ open_args;
	memset (&open_args, 0, sizeof(open_args));
	open_args.flags = FT_OPEN_STREAM;
	open_args.stream = id.stream;

	int error = FT_Open_Face (library, &open_args, 0, &info->face);
	if (error)
	{
		ATOM_LOGGER::error ("Create font <%s> failed\n", info->name);
		return false;
	}

	bool fixed_size = false;
	for (int i = 0; i < id.num_fixed_sizes; ++i)
	{
		if (id.fixed_sizes[i] == info->size)
		{
			FT_Select_Size (info->face, i);
			fixed_size = true;
			info->loadFlags = FT_LOAD_NO_AUTOHINT|FT_LOAD_MONOCHROME;
			info->renderMode = FT_RENDER_MODE_MONO;
			break;
		}
	}

	if (!fixed_size)
	{
		FT_Set_Pixel_Sizes (info->face, 0, info->size);
		info->loadFlags = FT_LOAD_DEFAULT;
		info->renderMode = FT_RENDER_MODE_NORMAL;
	}

	if (installed_fonts[index].support_unicode)
	{
		info->nextCharFunc = ATOM_CC_GetFunctions(info->charset)->GetNextUNICC;
		select_unicode_charset (info->face);
	}
	else if (installed_fonts[index].support_charset[info->charset])
	{
		info->nextCharFunc = ATOM_CC_GetFunctions(info->charset)->GetNextMBCC;
		select_charset (info->charset, info->face);
	}
	else
	{
		ATOM_LOGGER::error ("Create font failed because char set <%d> is not supported\n", info->charset);
		return false;
	}

	info->index = index;
	return true;
}

// Find the face index witch matches the given name.
int find_face_index (const char *name)
{
  ATOM_ASSERT (name);
  for (unsigned i = 0; i < installed_fonts.size(); ++i)
  {
    if( !_stricmp(installed_fonts[i].name.c_str(), name) )
    {
      return int(i);
    }
  }
  return -1;
}

// Get the font name of a given font index.
const char *get_name (int font) {
  ATOM_ASSERT (font >= 0 && font < int(installed_fonts.size()));
  return installed_fonts[font].name.c_str();
}

void get_underline (ATOM_FontInfo *info, int *pos, int *thickness)
{
  *pos = ((FT_MulFix(info->face->underline_position, info->face->size->metrics.y_scale)) >> 6);
  *thickness = (FT_MulFix(info->face->underline_thickness, info->face->size->metrics.y_scale)) >> 6;
  if (*thickness < 1)
  {
	  *thickness = 1;
  }
}

glyphinfo *find_glyph (int face, const ATOM_UNICC &ch, int size)
{
  ATOM_ASSERT (face >= 0 && face < int(installed_fonts.size()));
  std::pair<GlyphCacheIter, GlyphCacheIter> r = installed_fonts[face].glyph_cache->equal_range(ch);
  for (GlyphCacheIter it = r.first; it != r.second; ++it)
  {
    glyphinfo *g = &it->second;
    if (g->size == size)
      return g;
  }
  return 0;
}

void cache_glyph (int face, const ATOM_UNICC &ch, int size, FT_BitmapGlyph glyph, void *userdata)
{
  ATOM_ASSERT (face >= 0 && face < int(installed_fonts.size()));
  glyphinfo g;
  g.glyph = glyph;
  g.size = size;
  g.userdata = userdata;
  installed_fonts[face].glyph_cache->insert(std::pair<ATOM_UNICC, glyphinfo>(ch, g));
}

// This function renders a unicode string using a callback function.
void render_string (ATOM_FontInfo *info, const char *str, int len, int x, int y, ATOM_FontRenderCallback *callback) 
{
  int left, top, width, height, pitch, advance_x, advance_y;
  int startx = x;
  int starty = y;

  FT_Face face = info->face;
  const bool hasKerning = installed_fonts[info->index].has_kerning;

  FT_ULong prev = 0;
  FT_Vector kerning;
  kerning.x = 0;

  const char *strStart = str;
  const char *strEnd = str + len;
  ATOM_UNICC ch;
  int charMargin = 0;

  while (strStart = info->nextCharFunc (strStart, strEnd, &ch))
  {
    FT_ULong index = FT_Get_Char_Index (face, ch);
    if (!index)
    {
      continue;
    }

    glyphinfo *ginfo = find_glyph (info->index, ch, info->size);

    FT_BitmapGlyph bglyf = ginfo ? ginfo->glyph : 0;
    void *userdata = ginfo ? ginfo->userdata : 0;

    bool needCache = false;

    if (!bglyf)
    {
      if (FT_Load_Glyph (face, index, info->loadFlags))
      {
        ATOM_LOGGER::error ("[render_string] Couldn't load glyph %d.\n", index);
        return;
      }

      FT_GlyphSlot glyphslot = face->glyph;
      FT_Glyph glyph;

      if (FT_Render_Glyph (glyphslot, info->renderMode))
      {
        ATOM_LOGGER::error ("[render_string] Couldn't render glyph.\n");
        return;
      }
      
      FT_Get_Glyph (glyphslot, &glyph);
      ATOM_ASSERT (glyph->format == ft_glyph_format_bitmap);
      bglyf = (FT_BitmapGlyph)glyph;

      needCache = true;
    }

    height = bglyf->bitmap.rows;
    width = bglyf->bitmap.width;
    pitch = bglyf->bitmap.pitch;
    left = bglyf->left;
    top = bglyf->top;
    advance_x = (((FT_Glyph)bglyf)->advance.x + 0x8000) >> 16;
    advance_y = (((FT_Glyph)bglyf)->advance.y + 0x8000) >> 16;

    if (hasKerning && prev != 0)
	{
      FT_Get_Kerning (face, prev, index, FT_KERNING_DEFAULT, &kerning);
	}
    prev = index;
	startx += (kerning.x >> 6);
	startx += charMargin;

    callback->render (width, height, pitch, bglyf->bitmap.buffer, startx + left, starty - top, pitch < width, &userdata);

	charMargin = char_margin;
    startx += advance_x;
	starty -= advance_y;

    if (needCache)
    {
      cache_glyph (info->index, ch, info->size, bglyf, userdata);
    }
    else
    {
      ginfo->userdata = userdata;
    }
  }
}

// This function calculate the string bounds
bool get_string_bounds (ATOM_FontInfo *info, const char *str, int len, int *l, int *t, int *w, int *h) {
  int left, top, width, height, pitch, advance_x, advance_y;
  int minx = 0;
  int miny = 0;
  int maxx = 0;
  int maxy = 0;
  int x = 0;
  int z = 0; 
  int prev = 0;
  FT_Vector kerning;
  kerning.x = 0;

  const bool hasKerning = installed_fonts[info->index].has_kerning;
  const char *strStart = str;
  const char *strEnd = str + len;
  ATOM_UNICC ch;
  int charMargin = 0;

  while (strStart = info->nextCharFunc(strStart, strEnd, &ch))
  {
    FT_ULong index = FT_Get_Char_Index (info->face, ch);
    if (!index)
    {
      continue;
    }

    glyphinfo *ginfo = find_glyph (info->index, ch, info->size);
    FT_BitmapGlyph bglyf = ginfo ? ginfo->glyph : 0;

    if (!bglyf)
    {
      if (FT_Load_Glyph (info->face, index, info->loadFlags))
      {
        ATOM_LOGGER::error ("[render_string] Couldn't load glyph %d.\n", index);
        return false;
      }

      FT_GlyphSlot glyphslot = info->face->glyph;
      FT_Glyph glyph;

      if (FT_Render_Glyph (glyphslot, info->renderMode))
      {
        ATOM_LOGGER::error ("[render_string] Couldn't render glyph.\n");
        return false;
      }
      
      FT_Get_Glyph (glyphslot, &glyph);
      ATOM_ASSERT (glyph->format == ft_glyph_format_bitmap);
      bglyf = (FT_BitmapGlyph)glyph;

      cache_glyph (info->index, ch, info->size, bglyf, 0);
    }

    left = bglyf->left;
    top = bglyf->top;
    height = bglyf->bitmap.rows;
    width = bglyf->bitmap.width;
    pitch = bglyf->bitmap.pitch;
    advance_x = (((FT_Glyph)bglyf)->advance.x + 0x8000) >> 16;
    advance_y = (((FT_Glyph)bglyf)->advance.y + 0x8000) >> 16;

    if (hasKerning && prev != 0)
	{
      FT_Get_Kerning (info->face, prev, index, FT_KERNING_DEFAULT, &kerning);
	}

    prev = index;
    z = x + left;
    if (minx > z)
      minx = z;

    x += (kerning.x >> 6);
	x += charMargin;
	charMargin = char_margin;

    x += advance_x;
	if (maxx < x)
		maxx = x;

    top = -top;
    if (top < miny)
      miny = top;

    if (top + height > maxy)
      maxy = top + height;
  }

  if (l) *l = minx;
  if (t) *t = miny;
  if (w) *w = maxx - minx;
  if (h) *h = maxy - miny;

  return true;
}

bool font_system_initialized () {
  return initialized;
}

int get_face_height (ATOM_FontInfo *info) {
  return info->face->size->metrics.height >> 6;
}

int get_face_ascender (ATOM_FontInfo *info) {
	return info->face->size->metrics.ascender >> 6;
}

int get_face_width (ATOM_FontInfo *info) {
  return info->face->size->metrics.max_advance >> 6;
}

void set_char_margin (int margin) {
  char_margin = margin;
}

unsigned clip_string (ATOM_FontInfo *info, unsigned clip_size, const char *str, int *l, int *t, int *w, int *h)
{
  ATOM_StringClipInfo clipinfo;

  if (clip_string2 (info, clip_size, str, &clipinfo))
  {
	  if (l) *l = clipinfo.boundsClippedBefore[0];
	  if (t) *t = clipinfo.boundsClippedBefore[1];
	  if (w) *w = clipinfo.boundsClippedBefore[2];
	  if (h) *h = clipinfo.boundsClippedBefore[3];
	  return clipinfo.clippedBefore;
  }

  return 0;
}

bool clip_string2 (ATOM_FontInfo *info, unsigned clip_size, const char *str, ATOM_StringClipInfo *clipinfo)
{
  if (!clipinfo)
  {
	  return false;
  }

  memset(clipinfo, 0, sizeof(ATOM_StringClipInfo));

  int left, top, width, height, pitch, advance_x, advance_y;
  int minx = 0;
  int miny = 0;
  int maxx = 0;
  int maxy = 0;
  int x = 0;
  int z = 0; 
  int prev = 0;
  FT_Vector kerning;
  kerning.x = 0;

  const bool hasKerning = installed_fonts[info->index].has_kerning;
  const char *strStart = str;
  const char *strEnd = str + strlen(str);
  int _minx = minx;
  int _miny = miny;
  int _maxx = maxx;
  int _maxy = maxy;
  const char *lastStrPos = strStart;
  float frac = 0.f;
  int charMargin = 0;
  int lastWidth = _minx;

  ATOM_UNICC ch;
  while (strStart = info->nextCharFunc(strStart, strEnd, &ch))
  {
    FT_ULong index = FT_Get_Char_Index (info->face, ch);
    if (!index)
    {
      continue;
    }

    glyphinfo *ginfo = find_glyph (info->index, ch, info->size);
    FT_BitmapGlyph bglyf = ginfo ? ginfo->glyph : 0;

    if (!bglyf)
    {
      if (FT_Load_Glyph (info->face, index, info->loadFlags))
      {
        ATOM_LOGGER::error ("[render_string] Couldn't load glyph %d.\n", index);
        return 0;
      }

      FT_GlyphSlot glyphslot = info->face->glyph;
      FT_Glyph glyph;

      if (FT_Render_Glyph (glyphslot, info->renderMode))
      {
        ATOM_LOGGER::error ("[render_string] Couldn't render glyph.\n");
        return 0;
      }
      
      FT_Get_Glyph (glyphslot, &glyph);
      ATOM_ASSERT (glyph->format == ft_glyph_format_bitmap);
      bglyf = (FT_BitmapGlyph)glyph;

      cache_glyph (info->index, ch, info->size, bglyf, 0);
    }

    left = bglyf->left;
    top = bglyf->top;
    height = bglyf->bitmap.rows;
    width = bglyf->bitmap.width;
    pitch = bglyf->bitmap.pitch;
    advance_x = (((FT_Glyph)bglyf)->advance.x + 0x8000) >> 16;
    advance_y = (((FT_Glyph)bglyf)->advance.y + 0x8000) >> 16;

    if (hasKerning && prev != 0)
      FT_Get_Kerning (info->face, prev, index, FT_KERNING_DEFAULT, &kerning);
    prev = index;

    z = x + left;
    if (minx > z)
      minx = z;

    x += (kerning.x >> 6);
	x += charMargin;
	charMargin = char_margin;

	x += advance_x;
	if (x - minx > clip_size)
	{
		clipinfo->clippedBefore = (lastStrPos - str);
		clipinfo->clippedAfter = (strStart - str);
		clipinfo->boundsClippedBefore[0] = _minx;
		clipinfo->boundsClippedBefore[1] = _miny;
		clipinfo->boundsClippedBefore[2] = lastWidth - _minx;
		clipinfo->boundsClippedBefore[3] = _maxy - _miny;
		clipinfo->boundsClippedAfter[0] = _minx;
		clipinfo->boundsClippedAfter[1] = _miny;
		clipinfo->boundsClippedAfter[2] = x - _minx;
		clipinfo->boundsClippedAfter[3] = _maxy - _miny;
		clipinfo->clipRatio = 1.f - float(x - minx - clip_size)/float(advance_x);
		return true;
	}
	else
	{
		lastWidth = x;
	}

    z = x;
    if (maxx < z)
      maxx = z;

    top = -top;
    if (top < miny)
      miny = top;

    if (top + height > maxy)
      maxy = top + height;

	_minx = minx;
	_miny = miny;
	_maxx = maxx;
	_maxy = maxy;
	lastStrPos = strStart;
  }

  clipinfo->clippedBefore = (lastStrPos - str);
  clipinfo->clippedAfter = (lastStrPos - str);
  clipinfo->boundsClippedBefore[0] = _minx;
  clipinfo->boundsClippedBefore[1] = _miny;
  clipinfo->boundsClippedBefore[2] = lastWidth - _minx;
  clipinfo->boundsClippedBefore[3] = _maxy - _miny;
  clipinfo->boundsClippedAfter[0] = _minx;
  clipinfo->boundsClippedAfter[1] = _miny;
  clipinfo->boundsClippedAfter[2] = lastWidth - _minx;
  clipinfo->boundsClippedAfter[3] = _maxy - _miny;
  clipinfo->clipRatio = 0.f;

  return true;
}

int get_char_margin (void)
{
	return char_margin;
}

struct QueryHandle
{
	FT_StreamRec stream;
	FT_Open_Args_ open_args;
	ATOM_File *fp;
	unsigned numFaces;
	unsigned currentFace;
};

static void fill_face_info (FT_Face face, ATOM_FaceInfo *info)
{
	info->numGlyphs = face->num_glyphs;
	info->faceFlags = 0;
	if (FT_HAS_HORIZONTAL (face)) info->faceFlags |= ATOM_FontFaceFlag_Horizontal;
	if (FT_HAS_VERTICAL (face)) info->faceFlags |= ATOM_FontFaceFlag_Vertical;
	if (FT_HAS_KERNING (face)) info->faceFlags |= ATOM_FontFaceFlag_Kerning;
	if (FT_IS_SCALABLE (face)) info->faceFlags |= ATOM_FontFaceFlag_Scalable;
	if (FT_IS_SFNT (face)) info->faceFlags |= ATOM_FontFaceFlag_sFnt;
	if (FT_IS_FIXED_WIDTH (face)) info->faceFlags |= ATOM_FontFaceFlag_FixedWidth;
	if (FT_HAS_FIXED_SIZES (face)) info->faceFlags |= ATOM_FontFaceFlag_FixedSizes;
	if (FT_HAS_GLYPH_NAMES (face)) info->faceFlags |= ATOM_FontFaceFlag_GlyphNames;
	if (FT_HAS_MULTIPLE_MASTERS (face)) info->faceFlags |= ATOM_FontFaceFlag_MultipleMasters;
	info->styleFlags = 0;
	if (face->style_flags & FT_STYLE_FLAG_ITALIC) info->styleFlags |= ATOM_FontFaceStyle_Italic;
	if (face->style_flags & FT_STYLE_FLAG_BOLD) info->styleFlags |= ATOM_FontFaceStyle_Bold;
	strncpy (info->familyName ,face->family_name, sizeof(info->familyName));
	strncpy (info->styleName ,face->style_name, sizeof(info->styleName));
	info->numFixedSizes = face->num_fixed_sizes;
	if (info->numFixedSizes > ATOM_MaxFontFixedSizes) info->numFixedSizes = ATOM_MaxFontFixedSizes;
	memset (info->fixedSizes, 0, sizeof(info->fixedSizes));
	for (unsigned long i = 0; i < info->numFixedSizes; ++i)
	{
		info->fixedSizes[i].width = face->available_sizes[i].width;
		info->fixedSizes[i].height = face->available_sizes[i].height;
		info->fixedSizes[i].size = face->available_sizes[i].size;
	}
	info->numCharmaps = face->num_charmaps;
	if (info->numCharmaps > ATOM_MaxFontCharMaps) info->numCharmaps = ATOM_MaxFontCharMaps;
	memset (info->charmaps, 0, sizeof(info->charmaps));
	for (unsigned long i = 0; i < info->numCharmaps; ++i)
	{
		switch (face->charmaps[i]->encoding)
		{
		case FT_ENCODING_NONE:
			info->charmaps[i] = ATOM_FontEncodingNone;
			break;
		case FT_ENCODING_MS_SYMBOL:
			info->charmaps[i] = ATOM_FontEncodingMsSymbol;
			break;
		case FT_ENCODING_UNICODE:
			info->charmaps[i] = ATOM_FontEncodingUnicode;
			break;
		case FT_ENCODING_SJIS:
			info->charmaps[i] = ATOM_FontEncodingSJIS;
			break;
		case FT_ENCODING_GB2312:
			info->charmaps[i] = ATOM_FontEncodingGB2312;
			break;
		case FT_ENCODING_BIG5:
			info->charmaps[i] = ATOM_FontEncodingBIG5;
			break;
		case FT_ENCODING_WANSUNG:
			info->charmaps[i] = ATOM_FontEncodingWANSUNG;
			break;
		case FT_ENCODING_JOHAB:
			info->charmaps[i] = ATOM_FontEncodingJOHAB;
			break;
		case FT_ENCODING_ADOBE_STANDARD:
			info->charmaps[i] = ATOM_FontEncodingAdobeStandard;
			break;
		case FT_ENCODING_ADOBE_EXPERT:
			info->charmaps[i] = ATOM_FontEncodingAdobeExpert;
			break;
		case FT_ENCODING_ADOBE_CUSTOM:
			info->charmaps[i] = ATOM_FontEncodingAdobeCustom;
			break;
		case FT_ENCODING_ADOBE_LATIN_1:
			info->charmaps[i] = ATOM_FontEncodingAdobeLatin1;
			break;
		case FT_ENCODING_OLD_LATIN_2:
			info->charmaps[i] = ATOM_FontEncodingOldLatin2;
			break;
		case FT_ENCODING_APPLE_ROMAN:
			info->charmaps[i] = ATOM_FontEncodingAppleRoman;
			break;
		default:
			info->charmaps[i] = ATOM_FontEncodingUnknown;
			break;
		}
	}
}

void *begin_query_face_info (const char *fileName, ATOM_FaceInfo *info)
{
	if (!fileName || !info)
	{
		return 0;
	}

	ATOM_File *fp = ATOM_OpenFile (fileName, ATOM_VFS::read|ATOM_VFS::binary);
	if (!fp)
	{
		return 0;
	}

	QueryHandle *handle = ATOM_NEW(QueryHandle);
	handle->fp = fp;
	memset (&handle->stream, 0, sizeof(handle->stream));
	handle->stream.size = fp->size();
	handle->stream.pos = 0;
	handle->stream.descriptor.pointer = fp;
	handle->stream.read = IO_func;
	handle->stream.close = close_func;

	memset (&handle->open_args, 0, sizeof(handle->open_args));
	handle->open_args.flags = FT_OPEN_STREAM;
	handle->open_args.stream = &handle->stream;

	FT_Face face;

	// Try opening the first face in the font stream.
	error = FT_Open_Face (library, &handle->open_args, 0, &face);
	if (error)
	{
		ATOM_CloseFile (fp);
		ATOM_DELETE(handle);
		return 0;
	}

	handle->numFaces = face->num_faces;
	handle->currentFace = 0;
	fill_face_info (face, info);

    FT_Done_Face (face);

	return handle;
}

bool query_next_face_info (void *handle, ATOM_FaceInfo *info)
{
	QueryHandle *h = (QueryHandle *)handle;
	if (!h)
	{
		return false;
	}

	if (h->currentFace >= h->numFaces)
	{
		return false;
	}

	h->currentFace++;
	FT_Face face;

	// Try opening the first face in the font stream.
	error = FT_Open_Face (library, &h->open_args, h->currentFace, &face);
	if (error)
	{
		return false;
	}

	fill_face_info (face, info);
    FT_Done_Face (face);

	return true;
}

void end_query_face_info (void *handle)
{
	QueryHandle *h = (QueryHandle *)handle;

	if (h)
	{
		ATOM_CloseFile (h->fp);
		ATOM_DELETE(h);
	}
}

