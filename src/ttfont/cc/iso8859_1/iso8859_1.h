//$Id: iso8859_1.h 4515 2009-04-30 06:13:24Z gyk $

#ifndef __ATOM_FONT_ATOM_CC_ISO8859_1_H
#define __ATOM_FONT_ATOM_CC_ISO8859_1_H

#include "charcodec.h"

int     ATOM_CALL ATOM_CC_GetMaxByteCount_8859_1 (void);
bool    ATOM_CALL ATOM_CC_IsLeadByte_8859_1 (char ch);
ATOM_UNICC  ATOM_CALL ATOM_CC_ConvertCharToUnicode_8859_1 (ATOM_MBCC Char);
int     ATOM_CALL ATOM_CC_ConvertTextToUnicode_8859_1 (ATOM_UNICC *dst, const char *src, int SrcBufferLen, int DstBufferLen);
int     ATOM_CALL ATOM_CC_ConvertTextToMultibyte_8859_1 (ATOM_MBCC *dst, const char *src, int SrcBufferLen, int DstBufferLen);
const char * ATOM_CALL ATOM_CC_GetNextUNIATOM_CC_8859_1 (const char *str, const char *strEnd, ATOM_UNICC *ch);
const char * ATOM_CALL ATOM_CC_GetPrevUNIATOM_CC_8859_1 (const char *str, const char *strEnd, ATOM_UNICC *ch);
const char * ATOM_CALL ATOM_CC_GetNextMBATOM_CC_8859_1 (const char *str, const char *strEnd, ATOM_MBCC *ch);
const char * ATOM_CALL ATOM_CC_GetPrevMBATOM_CC_8859_1 (const char *str, const char *strEnd, ATOM_MBCC *ch);

#endif // __ATOM_FONT_ATOM_CC_ISO8859_1_H
