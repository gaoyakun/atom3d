#ifndef __ATOM_FONT_ATOM_CC_CP936_H
#define __ATOM_FONT_ATOM_CC_CP936_H

#include "charcodec.h"

int     ATOM_CALL ATOM_CC_GetMaxByteCount_CP936 (void);
bool    ATOM_CALL ATOM_CC_IsLeadByte_CP936 (char ch);
ATOM_UNICC  ATOM_CALL ATOM_CC_ConvertCharToUnicode_CP936 (ATOM_MBCC Char);
int     ATOM_CALL ATOM_CC_ConvertTextToUnicode_CP936 (ATOM_UNICC *dst, const char *src, int SrcBufferLen, int DstBufferLen);
int     ATOM_CALL ATOM_CC_ConvertTextToMultibyte_CP936 (ATOM_MBCC *dst, const char *src, int SrcBufferLen, int DstBufferLen);
const char * ATOM_CALL ATOM_CC_GetNextUNIATOM_CC_CP936 (const char *str, const char *strEnd, ATOM_UNICC *ch);
const char * ATOM_CALL ATOM_CC_GetPrevUNIATOM_CC_CP936 (const char *str, const char *strEnd, ATOM_UNICC *ch);
const char * ATOM_CALL ATOM_CC_GetNextMBATOM_CC_CP936 (const char *str, const char *strEnd, ATOM_MBCC *ch);
const char * ATOM_CALL ATOM_CC_GetPrevMBATOM_CC_CP936 (const char *str, const char *strEnd, ATOM_MBCC *ch);

#endif // __ATOM_FONT_ATOM_CC_CP936_H
