#include <string.h>
#include "basedefs.h"
#include "iso8859_1.h"

#include "iso8859_1.i"

int ATOM_CALL ATOM_CC_GetMaxByteCount_8859_1 (void) 
{
	return MaxByteCount ();
}

bool ATOM_CALL ATOM_CC_IsLeadByte_8859_1 (char ch) 
{
	return IsLeadByte (ch);
}

ATOM_UNICC ATOM_CALL ATOM_CC_ConvertCharToUnicode_8859_1 (ATOM_MBCC Char) 
{
	return ToUnicode((unsigned char*)&Char);
}

int ATOM_CALL ATOM_CC_ConvertTextToUnicode_8859_1 (ATOM_UNICC *dst, const char *src, int SrcBufferLen, int DstBufferLen) 
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
					*dst++ = ATOM_CC_ConvertCharToUnicode_8859_1 (*((ATOM_MBCC*)src_buffer));
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
				*dst++ = ATOM_CC_ConvertCharToUnicode_8859_1 (*((ATOM_MBCC*)src_buffer));
				++src_buffer;
				--count_conv;
				--DstBufferLen;
			}
		}

		return dst - s;
	}
}

int ATOM_CALL ATOM_CC_ConvertTextToMultibyte_8859_1 (ATOM_MBCC *dst, const char *src, int SrcBufferLen, int DstBufferLen) 
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

const char * ATOM_CALL ATOM_CC_GetNextUNIATOM_CC_8859_1 (const char *str, const char *strEnd, ATOM_UNICC *ch)
{
	if (str == strEnd)
	{
		return 0;
	}
	else
	{
		*ch = *str;
		return str + 1;
	}
}

const char * ATOM_CALL ATOM_CC_GetPrevUNIATOM_CC_8859_1 (const char *str, const char *strEnd, ATOM_UNICC *ch)
{
	if (str >= strEnd)
	{
		return 0;
	}
	else
	{
		*ch = *(strEnd-1);
		return strEnd-1;
	}
}

const char * ATOM_CALL ATOM_CC_GetNextMBATOM_CC_8859_1 (const char *str, const char *strEnd, ATOM_MBCC *ch)
{
	if (str >= strEnd)
	{
		return 0;
	}
	else
	{
		*ch = *str;
		return str + 1;
	}
}

const char * ATOM_CALL ATOM_CC_GetPrevMBATOM_CC_8859_1 (const char *str, const char *strEnd, ATOM_MBCC *ch)
{
	if (str >= strEnd)
	{
		return 0;
	}
	else
	{
		*ch = *(strEnd-1);
		return strEnd-1;
	}
}

