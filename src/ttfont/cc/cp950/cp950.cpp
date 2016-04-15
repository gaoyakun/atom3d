#include <string.h>
#include "basedefs.h"
#include "cp950.h"

#include "cp950.i"

int ATOM_CALL ATOM_CC_GetMaxByteCount_CP950 ()
{
return MaxByteCount ();
}

bool ATOM_CALL ATOM_CC_IsLeadByte_CP950 (char ch)
{
return IsLeadByte (ch);
}

ATOM_UNICC ATOM_CALL ATOM_CC_ConvertCharToUnicode_CP950 (ATOM_MBCC Char)
{
return ToUnicode((unsigned char*)&Char);
}

int ATOM_CALL ATOM_CC_ConvertTextToUnicode_CP950 (ATOM_UNICC *dst, const char *src, 
                                 int SrcBufferLen, int DstBufferLen)
{
if (src == 0)
  return 0;

const char *src_buffer = src;
int count_conv;

if (SrcBufferLen ==0)
  {
    SrcBufferLen = strlen (src_buffer);
    count_conv = SrcBufferLen + 1;
  }
else
  {
    count_conv = SrcBufferLen;
  }

if (dst == 0)
  {
    // compute the required dest buffer length.
    int len = 0;

    while (count_conv)
      {
        // Loop while not finished.

        if (IsLeadByte (*src_buffer))
          {
            // This MBCC takes two bytes.
            if (count_conv > 1)
              {
                ++len;
                src_buffer += 2;
                count_conv -= 2;
              }
            else
              {
                // The MBCC was truncated by string's boundary,
                // just skip it.
                ++src_buffer;
                --count_conv;
              }
          }
        else
          {
            // This MBCC takes one byte.
            ++len;
            ++src_buffer;
            --count_conv;
          }
      }

    return len * sizeof(ATOM_UNICC);
  }
else
  {
    DstBufferLen /= sizeof(ATOM_UNICC);
    ATOM_UNICC *s = dst;

    while (count_conv && DstBufferLen)
      {
        // Loop while source string processed all dest buffer full.

        if (IsLeadByte (*src_buffer))
          {
            // This MBCC takes two bytes.
            if (count_conv > 1)
              {
                *dst++ = ATOM_CC_ConvertCharToUnicode_CP950 (*((ATOM_MBCC*)src_buffer));
                src_buffer += 2;
                count_conv -= 2;
                --DstBufferLen;
              }
            else
              {
                // The MBCC was truncated by string's boundary,
                // just skip it.
                ++src_buffer;
                --count_conv;
              }
          }
        else
          {
            // This MBCC takes one byte.
            *dst++ = ATOM_CC_ConvertCharToUnicode_CP950 (*((ATOM_MBCC*)src_buffer));
            ++src_buffer;
            --count_conv;
            --DstBufferLen;
          }
      }

    return dst - s;
  }
}

static inline ATOM_MBCC swap_byte (ATOM_MBCC src)
{
return (src << 8) + (src >> 8);
}

int ATOM_CALL ATOM_CC_ConvertTextToMultibyte_CP950 (ATOM_MBCC *dst, const char *src, 
                                 int SrcBufferLen, int DstBufferLen)
{
if (src == 0)
  return 0;

const char *src_buffer = src;
int count_conv;

if (SrcBufferLen ==0)
  {
    SrcBufferLen = strlen (src_buffer);
    count_conv = SrcBufferLen + 1;
  }
else
  {
    count_conv = SrcBufferLen;
  }

if (dst == 0)
  {
    // compute the required dest buffer length.
    int len = 0;

    while (count_conv)
      {
        // Loop while not finished.

        if (IsLeadByte (*src_buffer))
          {
            // This MBCC takes two bytes.
            if (count_conv > 1)
              {
                ++len;
                src_buffer += 2;
                count_conv -= 2;
              }
            else
              {
                // The MBCC was truncated by string's boundary,
                // just skip it.
                ++src_buffer;
                --count_conv;
              }
          }
        else
          {
            // This MBCC takes one byte.
            ++len;
            ++src_buffer;
            --count_conv;
          }
      }

    return len * sizeof(ATOM_UNICC);
  }
else
  {
    DstBufferLen /= sizeof(ATOM_UNICC);
    ATOM_UNICC *s = (ATOM_UNICC*)dst;

    while (count_conv && DstBufferLen)
      {
        // Loop while source string processed all dest buffer full.

        if (IsLeadByte (*src_buffer))
          {
            // This MBCC takes two bytes.
            if (count_conv > 1)
              {
                *dst++ = *((ATOM_MBCC*)src_buffer);
                src_buffer += 2;
                count_conv -= 2;
                --DstBufferLen;
              }
            else
              {
                // The MBCC was truncated by string's boundary,
                // just skip it.
                ++src_buffer;
                --count_conv;
              }
          }
        else
          {
            // This MBCC takes one byte.
            *dst++ = *src_buffer++;
            --count_conv;
            --DstBufferLen;
          }
      }

    return dst - (ATOM_MBCC*)s;
  }
}

const char * ATOM_CALL ATOM_CC_GetNextUNIATOM_CC_CP950 (const char *str, const char *strEnd, ATOM_UNICC *ch)
{
  static const ATOM_UNICC uniccUnknown = ATOM_CC_ConvertCharToUnicode_CP950 ('?');

  if (str == strEnd)
  {
    return 0;
  }
  else
  {
    if (IsLeadByte (*str))
    {
      if (str + 1 < strEnd)
      {
         *ch = ATOM_CC_ConvertCharToUnicode_CP950 (*((ATOM_MBCC*)str));
         return str + 2;
      }
      else
      {
        *ch = L'?';
        return str + 1;
      }
    }
    else
    {
      *ch = *str;
      return str + 1;
    }
  }
}

const char * ATOM_CALL ATOM_CC_GetPrevUNIATOM_CC_CP950 (const char *str, const char *strEnd, ATOM_UNICC *ch)
{
  static const ATOM_UNICC uniccUnknown = ATOM_CC_ConvertCharToUnicode_CP950 ('?');

  if (str >= strEnd)
  {
    return 0;
  }
  else
  {
    if (str + 1 == strEnd || !IsLeadByte (*(strEnd-2)))
	{
		*ch = ATOM_CC_ConvertCharToUnicode_CP950 (*(strEnd-1));
		return strEnd - 1;
	}
	else
	{
		*ch = ATOM_CC_ConvertCharToUnicode_CP950 (*((ATOM_MBCC*)(strEnd-2)));
		return strEnd - 2;
	}
  }
}

const char * ATOM_CALL ATOM_CC_GetNextMBATOM_CC_CP950 (const char *str, const char *strEnd, ATOM_MBCC *ch)
{
  if (str >= strEnd)
  {
    return 0;
  }
  else
  {
    if (IsLeadByte (*str))
    {
      if (str + 1 < strEnd)
      {
         *ch = swap_byte(*((ATOM_MBCC*)str));
         return str + 2;
      }
      else
      {
        *ch = '?';
        return str + 1;
      }
    }
    else
    {
      *ch = *str;
      return str + 1;
    }
  }
}

const char * ATOM_CALL ATOM_CC_GetPrevMBATOM_CC_CP950 (const char *str, const char *strEnd, ATOM_MBCC *ch)
{
  if (str >= strEnd)
  {
    return 0;
  }
  else
  {
    if (str + 1 == strEnd || !IsLeadByte (*(strEnd-2)))
	{
		*ch = *(strEnd-1);
		return strEnd - 1;
	}
	else
	{
		*ch = swap_byte (*((ATOM_MBCC*)(strEnd-2)));
		return strEnd - 2;
	}
  }
}

