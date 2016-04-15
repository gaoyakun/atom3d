#ifndef __ATOM_FONT_FONTAPI_H
#define __ATOM_FONT_FONTAPI_H

#include "../ATOM_kernel.h"
#include "basedefs.h"
#include "charcodec.h"
#include "rendercallback.h"

#define ATOM_RENDER_DEFAULT 0
#define ATOM_RENDER_MONO    1

typedef void *  ATOM_FontHandle;
struct ATOM_StringClipInfo
{
	int clippedBefore;
	int boundsClippedBefore[4];
	int boundsClippedAfter[5];
	int clippedAfter;
	float clipRatio;
};

struct ATOM_UnderLineInfo
{
	int underLinePosition;
	int underLineThickness;
};

#define ATOM_MaxFontCharMaps 16

enum ATOM_FontEncoding
{
	ATOM_FontEncodingNone = 0,
	ATOM_FontEncodingMsSymbol = 1,
	ATOM_FontEncodingUnicode = 2,
	ATOM_FontEncodingSJIS = 3,
	ATOM_FontEncodingGB2312 = 4,
	ATOM_FontEncodingBIG5 = 5,
	ATOM_FontEncodingWANSUNG = 6,
	ATOM_FontEncodingJOHAB = 7,
	ATOM_FontEncodingAdobeStandard = 8,
	ATOM_FontEncodingAdobeExpert = 9,
	ATOM_FontEncodingAdobeCustom = 10,
	ATOM_FontEncodingAdobeLatin1 = 11,
	ATOM_FontEncodingOldLatin2 = 12,
	ATOM_FontEncodingAppleRoman = 13,
	ATOM_FontEncodingUnknown = 14
};

enum 
{
	ATOM_FontFaceFlag_Scalable			= ( 1L <<  0 ),
	ATOM_FontFaceFlag_FixedSizes		= ( 1L <<  1 ),
	ATOM_FontFaceFlag_FixedWidth		= ( 1L <<  2 ),
	ATOM_FontFaceFlag_sFnt				= ( 1L <<  3 ),
	ATOM_FontFaceFlag_Horizontal		= ( 1L <<  4 ),
	ATOM_FontFaceFlag_Vertical			= ( 1L <<  5 ),
	ATOM_FontFaceFlag_Kerning			= ( 1L <<  6 ),
	ATOM_FontFaceFlag_MultipleMasters	= ( 1L <<  8 ),
	ATOM_FontFaceFlag_GlyphNames		= ( 1L <<  9 ),
	ATOM_FontFaceFlag_ExternalStream	= ( 1L << 10 ),
	ATOM_FontFaceFlag_Hinter			= ( 1L << 11 )
};

enum
{
	ATOM_FontFaceStyle_Italic	= (1L << 0),
	ATOM_FontFaceStyle_Bold		= (1L << 1)
};

struct ATOM_FontBitmapSize
{
	unsigned short width;
	unsigned short height;
	long size;
};

#define ATOM_MaxFontFixedSizes 32

struct ATOM_FaceInfo
{
	unsigned long numGlyphs;
	unsigned long faceFlags;
	unsigned long styleFlags;
	char familyName[256];
	char styleName[256];
	unsigned long numFixedSizes;
	ATOM_FontBitmapSize fixedSizes[ATOM_MaxFontFixedSizes];
	unsigned long numCharmaps;
	ATOM_FontEncoding charmaps[ATOM_MaxFontCharMaps];
};

ATOM_FONT_API bool         ATOM_CALL ATOM_Initialized (void);
ATOM_FONT_API ATOM_FontHandle   ATOM_CALL ATOM_CreateFont (const char *fontName, int size, int charset);
ATOM_FONT_API void         ATOM_CALL ATOM_ReleaseFont (ATOM_FontHandle font);
ATOM_FONT_API bool         ATOM_CALL ATOM_LoadFont (const char *fontName, const char *filename);
ATOM_FONT_API int          ATOM_CALL ATOM_GetNumFontsInstalled (void);
ATOM_FONT_API int          ATOM_CALL ATOM_GetFontHeight (ATOM_FontHandle font);
ATOM_FONT_API int          ATOM_CALL ATOM_GetFontWidth (ATOM_FontHandle font);
ATOM_FONT_API int		   ATOM_CALL ATOM_GetFontAscender (ATOM_FontHandle font);
ATOM_FONT_API void         ATOM_CALL ATOM_SetRenderMode (int mode);
ATOM_FONT_API int          ATOM_CALL ATOM_GetFontSize (ATOM_FontHandle font);
ATOM_FONT_API const char * ATOM_CALL ATOM_GetFontName (ATOM_FontHandle font);
ATOM_FONT_API void         ATOM_CALL ATOM_RenderString (ATOM_FontHandle font, const char *str, int len, ATOM_FontRenderCallback *callback);
ATOM_FONT_API bool         ATOM_CALL ATOM_CalcStringBounds (ATOM_FontHandle font, const char *str, int len, int *l, int *t, int *w, int *h);
ATOM_FONT_API void         ATOM_CALL ATOM_SetCharMargin (int margin);
ATOM_FONT_API int		   ATOM_CALL ATOM_GetCharMargin (void);
ATOM_FONT_API bool		  ATOM_CALL ATOM_GetUnderLineInfo (ATOM_FontHandle font, ATOM_UnderLineInfo *info);
ATOM_FONT_API unsigned	  ATOM_CALL ATOM_ClipString (ATOM_FontHandle font, const char *str, unsigned clip_size, int *l, int *t, int *w, int *h);
ATOM_FONT_API bool		  ATOM_CALL ATOM_ClipStringEx (ATOM_FontHandle font, const char *str, unsigned clip_size, ATOM_StringClipInfo *info);

ATOM_FONT_API void *ATOM_CALL ATOM_BeginQueryFaceInfo (const char *fileName, ATOM_FaceInfo *info);
ATOM_FONT_API bool ATOM_CALL ATOM_QueryNextFaceInfo (void *handle, ATOM_FaceInfo *info);
ATOM_FONT_API void ATOM_CALL ATOM_EndQueryFaceInfo (void *handle);

#endif // __ATOM_FONT_FONTAPI_H
