#include "StdAfx.h"
#include "assertion.h"
#include "utils.h"

static inline char ByteToChar (unsigned char byte)
{
  ATOM_ASSERT (byte < 16);
  static const char b2c[] = "0123456789ABCDEF";
  return b2c[byte];
}

static inline void ByteToDoubleChar (unsigned char byte, char *p)
{
  p[0] = ByteToChar (byte >> 4);
  p[1] = ByteToChar (byte & 0x0F);
}

static inline void EmitByte (char *s, unsigned char byte)
{
  char c[3];
  ByteToDoubleChar (byte, c);
  c[2] = '\0';
  strcat (s, c);
}

static inline void EmitWord (char *s, const unsigned char *p)
{
  char c[5];
  ByteToDoubleChar (*p++, c);
  ByteToDoubleChar (*p, c+2);
  c[4] = '\0';
  strcat (s, c);
}

static inline void EmitDword (char *s, const unsigned char *p)
{
  char c[9];
  ByteToDoubleChar (*p++, c);
  ByteToDoubleChar (*p++, c+2);
  ByteToDoubleChar (*p++, c+4);
  ByteToDoubleChar (*p, c+6);
  c[8] = '\0';
  strcat (s, c);
}

static inline void EmitSpace (char *s)
{
  strcat (s, " ");
}

static inline void EmitLineAddress (char *s, const void *p)
{
  strcat(s, "0x");
  unsigned n = (unsigned)p;
  char c[9];
  ByteToDoubleChar ((n>>24)&0xFF, c);
  ByteToDoubleChar ((n>>16)&0xFF, c+2);
  ByteToDoubleChar ((n>>8)&0xFF, c+4);
  ByteToDoubleChar ((n>>0)&0xFF, c+6);
  c[8] = '\0';
  strcat (s, c);
}

static inline void EmitHexLineByte (char *s, const void *p, unsigned numBytes)
{
  const unsigned char *data = (const unsigned char*)p;

  for (unsigned i = 0; i < numBytes; ++i)
  {
    EmitByte (s, *data++); // 2 bytes
    EmitSpace (s); // 1 byte
  }
}

static inline void EmitHexLineWord (char *s, const void *p, unsigned numBytes)
{
  const unsigned char *data = (const unsigned char*)p;

  for (unsigned i = 0; i < numBytes/2; ++i)
  {
    EmitWord (s, data); // 4 bytes
    data += 2;
    EmitSpace (s); // 1 byte
  }
}

static inline void EmitHexLineDword (char *s, const void *p, unsigned numBytes)
{
  const unsigned char *data = (const unsigned char*)p;

  for (unsigned i = 0; i < numBytes/4; ++i)
  {
    EmitDword (s, data); // 8 bytes
    data += 4;
    EmitSpace (s); // 1 byte
  }
}

static inline void EmitAsciiLine (char *s, const void *p, unsigned bytesPerLine)
{
  const char *src = (const char *)p;
  char *data = (char*)ATOM_STACK_MALLOC (bytesPerLine+1);
  ATOM_ASSERT (data);
  for (unsigned i = 0; i < bytesPerLine; ++i)
  {
    data[i] = src[i];
    if (data[i] < 32)
    {
      data[i] = '?';
    }
  }
  data[bytesPerLine] = '\0';
  strcat (s, data);
}

static inline void EmitReturn (char *s)
{
  strcat (s, "\r\n");
}

ATOM_DBGHLP_API ATOM_STRING ATOM_DumpMemoryAsByte (const void *p, unsigned totalBytes)
{
  const unsigned buffersize = 5 * totalBytes;
  char *buffer = (char*)ATOM_STACK_ALLOC(buffersize);
  ATOM_DumpMemoryAsByte (p, totalBytes, buffer);
  ATOM_STRING str = buffer;
  ATOM_STACK_FREE(buffer, buffersize);
  return str;
}

ATOM_DBGHLP_API void ATOM_DumpMemoryAsByte (const void *p, unsigned totalBytes, char *buffer)
{
  static const bool dumpAddress = true;
  static const bool dumpHex = true;
  static const bool dumpAscii = true;
  static const unsigned bytesPerLine = 16;

  ATOM_ASSERT(p);
  ATOM_ASSERT(totalBytes);
  ATOM_ASSERT(buffer);

  char *str = buffer;
  str[0] = '\0';

  unsigned lines = totalBytes / bytesPerLine;
  const unsigned bpl = lines ? bytesPerLine : totalBytes;
  if (lines == 0)
    lines = 1;

  const unsigned char *data = (const unsigned char*)p;
  for (unsigned i = 0; i < lines; ++i)
  {
    if (dumpAddress)
    {
      EmitLineAddress (str, data); // 10 bytes
      EmitSpace (str); // 1 byte
    }

    if (dumpHex)
    {
      EmitHexLineByte (str, data, bpl); // bytesPerLine * 3 bytes
    }

    if (dumpAscii)
    {
      EmitAsciiLine (str, data, bpl); // bytesPerLine bytes
    }

    EmitReturn (str); // 1 byte
    data += bpl;
  }

  // The total bytes of the buffer is at least (totalBytes/bytesPerLine) * (10+1+4*bytesPerLine+1) + 1
  // means 5 * totalBytes
};

ATOM_DBGHLP_API ATOM_STRING ATOM_DumpMemoryAsWord (const void *p, unsigned totalBytes)
{
  const unsigned buffersize = 5 * totalBytes;
  char *buffer = (char*)ATOM_STACK_ALLOC(buffersize);
  ATOM_DumpMemoryAsWord (p, totalBytes, buffer);
  ATOM_STRING str = buffer;
  ATOM_STACK_FREE(buffer, buffersize);
  return str;
}

ATOM_DBGHLP_API void ATOM_DumpMemoryAsWord (const void *p, unsigned totalBytes, char *buffer)
{
  static const bool dumpAddress = true;
  static const bool dumpHex = true;
  static const bool dumpAscii = true;
  static const unsigned bytesPerLine = 16;

  ATOM_ASSERT(p);
  ATOM_ASSERT(totalBytes);
  ATOM_ASSERT(buffer);

  unsigned lines = totalBytes / bytesPerLine;
  char *str = buffer;
  const unsigned char *data = (const unsigned char*)p;
  for (unsigned i = 0; i < lines; ++i)
  {
    if (dumpAddress)
    {
      EmitLineAddress (str, data);  // 10 bytes
      EmitSpace (str); // 1 byte
    }

    if (dumpHex)
    {
      EmitHexLineWord (str, data, bytesPerLine); // 5 * bytesPerLine / 2 bytes
    }

    if (dumpAscii)
    {
      EmitAsciiLine (str, data, bytesPerLine); // bytesPerLine bytes
    }

    EmitReturn (str); // 1 byte
    data += bytesPerLine;
  }

  // totalBytes / bytesPerLine * (10+1+2.5*bytesPerLine+bytesPerLine+1)
  // totalBytes / bytesPerLine * (12 + 3.5*bytesPerLine)
  // means 5 * totalBytes
};

ATOM_DBGHLP_API ATOM_STRING ATOM_DumpMemoryAsDword (const void *p, unsigned totalBytes)
{
  const unsigned buffersize = 5 * totalBytes;
  char *buffer = (char*)ATOM_STACK_ALLOC(buffersize);
  ATOM_DumpMemoryAsDword (p, totalBytes, buffer);
  ATOM_STRING str = buffer;
  ATOM_STACK_FREE(buffer, buffersize);
  return str;
}

ATOM_DBGHLP_API void ATOM_DumpMemoryAsDword (const void *p, unsigned totalBytes, char *buffer)
{
  static const bool dumpAddress = true;
  static const bool dumpHex = true;
  static const bool dumpAscii = true;
  static const unsigned bytesPerLine = 16;

  ATOM_ASSERT(p);
  ATOM_ASSERT(totalBytes);
  ATOM_ASSERT(buffer);

  unsigned lines = totalBytes / bytesPerLine;
  char *str = buffer;
  const unsigned char *data = (const unsigned char*)p;
  for (unsigned i = 0; i < lines; ++i)
  {
    if (dumpAddress)
    {
      EmitLineAddress (str, data); // 10 bytes
      EmitSpace (str);  // 1 byte
    }

    if (dumpHex)
    {
      EmitHexLineDword(str, data, bytesPerLine);  // 9 * bytesPerLine / 4 bytes
    }

    if (dumpAscii)
    {
      EmitAsciiLine (str, data, bytesPerLine); // bytesPerLine bytes
    }

    EmitReturn (str); // 1 byte
    data += bytesPerLine;
  }

  // totalBytes/bytesPerLine * (10+1+3.125*bytesPerLine+1)
  // means 5 * totalBytes
};


ATOM_DBGHLP_API ATOM_STRING dhDisassembleMemory (const void *p, unsigned size)
{
#if 1
  return "";
#else
  ATOM_ASSERT(p);

  ud_t ud_obj;
  ud_init (&ud_obj);
  ud_set_mode (&ud_obj, 32);
  ud_set_syntax (&ud_obj, UD_SYN_INTEL);
  ud_set_input_buffer (&ud_obj, (uint8_t*)(const_cast<void*>(p)), size);
  ATOM_STRING s;
  unsigned addr = (unsigned)p;
  char buffer[1024];
  while (ud_disassemble(&ud_obj))
  {
    sprintf (buffer, "%016llx ", addr + ud_insn_off(&ud_obj));
    s += buffer;

    char *hex1 = ud_insn_hex(&ud_obj);
    char *hex2 = hex1 + 16;
    char c = hex1[16];
    hex1[16] = 0;
    sprintf (buffer, "%-16s %-24s", hex1, ud_insn_asm (&ud_obj));
    s += buffer;
    hex1[16] = c;
    if (strlen(hex1) > 16)
    {
      sprintf(buffer, "\n%15s -%-16s", "", hex2);
      s += buffer;
    }
    s += '\n';
  }
  return s;
#endif
}
