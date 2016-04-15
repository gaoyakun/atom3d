#ifndef __ATOM_FONT_CHARCODEC_H
#define __ATOM_FONT_CHARCODEC_H

#include "basedefs.h"

typedef unsigned short ATOM_UNICC;
typedef unsigned short ATOM_MBCC;

enum
{
  ATOM_CC_START      = 0,
  ATOM_CC_ISO8859_1  = 0,
  ATOM_CC_CP936,
  ATOM_CC_CP950,

  // More codecs definition comes here.

  ATOM_CC_END,
  ATOM_CC_UNKNOWN = ATOM_CC_END
};

typedef int (ATOM_CALL *CCFUNC_GetMaxByteCount) (void);
typedef bool (ATOM_CALL *CCFUNC_IsLeadByte) (char ch);
typedef ATOM_UNICC (ATOM_CALL *CCFUNC_ConvertCharToUnicode) (ATOM_MBCC ch);
typedef int (ATOM_CALL *CCFUNC_ConvertTextToUnicode) (ATOM_UNICC *dst, const char *src, int SrcBufferLen, int DstBufferLen);
typedef int (ATOM_CALL *CCFUNC_ConvertTextToMultiByte) (ATOM_MBCC *dst, const char *src, int SrcBufferLen, int DstBufferLen);
typedef const char * (ATOM_CALL *CCFUNC_GetNextChar) (const char *strStart, const char *strEnd, ATOM_UNICC *ch);

struct ATOM_CCFunctions
{
  CCFUNC_GetMaxByteCount GetMaxByteCount;
  CCFUNC_IsLeadByte IsLeadByte;
  CCFUNC_ConvertCharToUnicode ConvertCharToUnicode;
  CCFUNC_ConvertTextToUnicode ConvertTextToUnicode;
  CCFUNC_ConvertTextToMultiByte ConvertTextToMultiByte;
  CCFUNC_GetNextChar GetNextUNICC;
  CCFUNC_GetNextChar GetPrevUNICC;
  CCFUNC_GetNextChar GetNextMBCC;
  CCFUNC_GetNextChar GetPrevMBCC;
};

extern"C" 
{
  ATOM_FONT_API int    ATOM_CALL ATOM_CC_GetMaxByteCount      (int cc);
  ATOM_FONT_API bool   ATOM_CALL ATOM_CC_IsLeadByte           (int cc, char ch);
  ATOM_FONT_API ATOM_UNICC ATOM_CALL ATOM_CC_ConvertCharToUnicode (int ccSrc, ATOM_MBCC Char);
  ATOM_FONT_API int    ATOM_CALL ATOM_CC_ConvertTextToUnicode (int ccSrc, ATOM_UNICC *dst, const char *src, int SrcBufferLen, int DstBufferLen);
  ATOM_FONT_API int    ATOM_CALL ATOM_CC_ConvertTextToMultibyte (int ccSrc, ATOM_MBCC *dst, const char *src, int SrcBufferLen, int DstBufferLen);
  ATOM_FONT_API const char *ATOM_CALL ATOM_CC_GetNextUNICC (int ccSrc, const char *strStart, const char *strEnd, ATOM_UNICC *ch);
  ATOM_FONT_API const char *ATOM_CALL ATOM_CC_GetPrevUNICC (int ccSrc, const char *strStart, const char *strEnd, ATOM_UNICC *ch);
  ATOM_FONT_API const char *ATOM_CALL ATOM_CC_GetNextMBCC (int ccSrc, const char *strStart, const char *strEnd, ATOM_MBCC *ch);
  ATOM_FONT_API const char *ATOM_CALL ATOM_CC_GetPrevMBCC (int ccSrc, const char *strStart, const char *strEnd, ATOM_MBCC *ch);
  ATOM_FONT_API const ATOM_CCFunctions * ATOM_CALL ATOM_CC_GetFunctions (int ccSrc);
} // "C"

#endif // __ATOM_FONT_CHARCODEC_H
