#ifndef __ATOM_FONT_ATOM_CC_CP950_H
#define __ATOM_FONT_ATOM_CC_CP950_H

#include "charcodec.h"

int     ATOM_CALL ATOM_CC_GetMaxByteCount_CP950 (void);
bool    ATOM_CALL ATOM_CC_IsLeadByte_CP950 (char ch);
ATOM_UNICC  ATOM_CALL ATOM_CC_ConvertCharToUnicode_CP950 (ATOM_MBCC Char);
int     ATOM_CALL ATOM_CC_ConvertTextToUnicode_CP950 (ATOM_UNICC *dst, const char *src, int SrcBufferLen, int DstBufferLen);
int     ATOM_CALL ATOM_CC_ConvertTextToMultibyte_CP950 (ATOM_MBCC *dst, const char *src, int SrcBufferLen, int DstBufferLen);
const char * ATOM_CALL ATOM_CC_GetNextUNIATOM_CC_CP950 (const char *str, const char *strEnd, ATOM_UNICC *ch);
const char * ATOM_CALL ATOM_CC_GetPrevUNIATOM_CC_CP950 (const char *str, const char *strEnd, ATOM_UNICC *ch);
const char * ATOM_CALL ATOM_CC_GetNextMBATOM_CC_CP950 (const char *str, const char *strEnd, ATOM_MBCC *ch);
const char * ATOM_CALL ATOM_CC_GetPrevMBATOM_CC_CP950 (const char *str, const char *strEnd, ATOM_MBCC *ch);

#endif // __ATOM_FONT_ATOM_CC_CP950_H
