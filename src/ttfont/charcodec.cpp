#include "charcodec.h"
#include "cc/iso8859_1/iso8859_1.h"
#include "cc/cp936/cp936.h"
#include "cc/cp950/cp950.h"

static ATOM_CCFunctions _CCFunctions[ATOM_CC_END] = {
  {
    &ATOM_CC_GetMaxByteCount_8859_1,
    &ATOM_CC_IsLeadByte_8859_1,
    &ATOM_CC_ConvertCharToUnicode_8859_1,
    &ATOM_CC_ConvertTextToUnicode_8859_1,
    &ATOM_CC_ConvertTextToMultibyte_8859_1,
    &ATOM_CC_GetNextUNIATOM_CC_8859_1,
	&ATOM_CC_GetPrevUNIATOM_CC_8859_1,
    &ATOM_CC_GetNextMBATOM_CC_8859_1,
	&ATOM_CC_GetPrevMBATOM_CC_8859_1
  },
  {
    &ATOM_CC_GetMaxByteCount_CP936,
    &ATOM_CC_IsLeadByte_CP936,
    &ATOM_CC_ConvertCharToUnicode_CP936,
    &ATOM_CC_ConvertTextToUnicode_CP936,
    &ATOM_CC_ConvertTextToMultibyte_CP936,
    &ATOM_CC_GetNextUNIATOM_CC_CP936,
	&ATOM_CC_GetPrevUNIATOM_CC_CP936,
    &ATOM_CC_GetNextMBATOM_CC_CP936,
	&ATOM_CC_GetPrevMBATOM_CC_CP936
  },
  {
    &ATOM_CC_GetMaxByteCount_CP950,
    &ATOM_CC_IsLeadByte_CP950,
    &ATOM_CC_ConvertCharToUnicode_CP950,
    &ATOM_CC_ConvertTextToUnicode_CP950,
    &ATOM_CC_ConvertTextToMultibyte_CP950,
    &ATOM_CC_GetNextUNIATOM_CC_CP950,
	&ATOM_CC_GetPrevUNIATOM_CC_CP950,
    &ATOM_CC_GetNextMBATOM_CC_CP950,
	&ATOM_CC_GetPrevMBATOM_CC_CP950
  }
};

ATOM_FONT_API const ATOM_CCFunctions * ATOM_CALL ATOM_CC_GetFunctions (int ccSrc)
{
  return &_CCFunctions[ccSrc];
}

ATOM_FONT_API ATOM_UNICC ATOM_CALL ATOM_CC_ConvertCharToUnicode (int ccSrc, ATOM_MBCC Char)
{
  return _CCFunctions[ccSrc].ConvertCharToUnicode (Char);
}

ATOM_FONT_API int ATOM_CALL ATOM_CC_ConvertTextToUnicode (int ccSrc, ATOM_UNICC *dst, const char *src, int SrcBufferLen, int DstBufferLen)
{
  return _CCFunctions[ccSrc].ConvertTextToUnicode (dst, src, SrcBufferLen, DstBufferLen);
}

ATOM_FONT_API int ATOM_CALL ATOM_CC_ConvertTextToMultibyte (int ccSrc, ATOM_MBCC *dst, const char *src, int SrcBufferLen, int DstBufferLen)
{
  return _CCFunctions[ccSrc].ConvertTextToMultiByte (dst, src, SrcBufferLen, DstBufferLen);
}

ATOM_FONT_API int ATOM_CALL ATOM_CC_GetMaxByteCount (int cc)
{
  return _CCFunctions[cc].GetMaxByteCount ();
}

ATOM_FONT_API bool ATOM_CALL ATOM_CC_IsLeadByte (int cc, char ch)
{
  return _CCFunctions[cc].IsLeadByte (ch);
}

ATOM_FONT_API const char *ATOM_CALL ATOM_CC_GetNextUNICC (int ccSrc, const char *strStart, const char *strEnd, ATOM_UNICC *ch)
{
  return _CCFunctions[ccSrc].GetNextUNICC (strStart, strEnd, ch);
}

ATOM_FONT_API const char *ATOM_CALL ATOM_CC_GetPrevUNICC (int ccSrc, const char *strStart, const char *strEnd, ATOM_UNICC *ch)
{
  return _CCFunctions[ccSrc].GetPrevUNICC (strStart, strEnd, ch);
}

ATOM_FONT_API const char *ATOM_CALL ATOM_CC_GetNextMBCC (int ccSrc, const char *strStart, const char *strEnd, ATOM_MBCC *ch)
{
  return _CCFunctions[ccSrc].GetNextMBCC (strStart, strEnd, ch);
}

ATOM_FONT_API const char *ATOM_CALL ATOM_CC_GetPrevMBCC (int ccSrc, const char *strStart, const char *strEnd, ATOM_MBCC *ch)
{
  return _CCFunctions[ccSrc].GetPrevMBCC (strStart, strEnd, ch);
}
