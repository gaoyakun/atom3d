#include <string.h>
#include <ATOM_dbghlp.h>

#include "utils.h"

unsigned bin2txt (char *text, unsigned size)
{
  if (size == 0)
  {
    return 0;
  }

  char *tmp = ATOM_NEW_ARRAY(char, size+1);
  char *dst = tmp;
  const char *src = text;
  unsigned n = 0;
  for (unsigned i = 0; i < size-1; ++i)
  {
    if (src[0] != '\r' || src[1] != '\n')
    {
      ++n;
      *dst++ = *src;
    }
    src++;
  }

  if (*src != '\r')
  {
    n++;
    *dst++ = *src++;
  }

  memcpy (text, tmp, n);
  ATOM_DELETE_ARRAY(tmp);
  return n;
}

