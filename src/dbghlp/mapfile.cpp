#include "StdAfx.h"
#include <sstream>
#include <fstream>
#include <io.h>
#include <fcntl.h>
#include "mapfile.h"
#include "debugutils.h"
#include "dbghelpapi.h"

static inline void remove_head_non_spaces(std::string &str) {
  int n = 0;
  while ( n < int(str.length()) && str[n] != ' ' && str[n] != '\t')
    ++n;
  str.erase(0, n);
}

static inline void remove_tail_non_spaces(std::string &str) {
  if (str.length() == 0)
    return;

  int n = str.length() - 1;
  while (n >= 0 && str[n] != ' ' && str[n] != '\t')
    --n;
  str.erase(n);
}

static inline void remove_head_tail_spaces(std::string& str) {
  int n = 0;
  while ( n < int(str.length()) && (str[n] == ' ' || str[n] == '\t'))
    ++n;
  str.erase(0, n);

  if ( str.length() > 0)
  {
    n = int(str.length() - 1);
    while ( n >= 0 && (str[n] == ' ' || str[n] == '\t'))
      --n;
    str.erase(n + 1);
  }
}

static inline bool getline_from_stream (std::istream &in, std::string &out)
{
  static char buffer[2048];
  while (!in.eof())
  {
    in.getline (buffer, 2048);
    out = buffer;
    remove_head_tail_spaces (out);
    if (!out.empty())
    {
      return true;
    }
  }
  return false;
}

static bool _GetFunctionTraceInfoFromStream (std::istream &s, ATOM_FunctionTraceInfo *info)
{
  std::string temp;
  ATOM_ULongLong preferred_load_address = 0;

  // Skip module name
  if (!getline_from_stream (s, temp))
  {
    return false;
  }

  // Skip timestamp
  if (!getline_from_stream (s, temp))
  {
    return false;
  }

  // This is the preferred load address
  if (!getline_from_stream (s, temp))
  {
    return false;
  }
  if (1 != sscanf (temp.c_str(), "Preferred load address is %I64X", &preferred_load_address))
  {
    return false;
  }

  // Skip "Start Length Name Class"
  if (!getline_from_stream (s, temp))
  {
    return false;
  }

  // Skip segment descriptions
  ATOM_ULongLong codesection_section = 0;
  ATOM_ULongLong codesection_offset = 0;
  bool codesection_offset_calculated = false;

  for(;;)
  {
    if (!getline_from_stream (s, temp))
      return false;

    if (!_strnicmp ("Address", temp.c_str(), 7))
      break;

    if (!codesection_offset_calculated)
    {
      // get xxxx:xxxxxxxx
      std::string start = temp.substr(0,13);
      temp.erase(0, 13);
      remove_head_tail_spaces(temp);

      // skip length XXXXXXXXH
      temp.erase(0, 9);
      remove_head_tail_spaces(temp);

      // skip Name
      remove_head_non_spaces(temp);
      remove_head_tail_spaces(temp);

      if (temp == "CODE")
      {
        sscanf(start.c_str(), "%I64X:%I64X", &codesection_section, &codesection_offset);
        codesection_offset_calculated = true;
      }
    }
  }

  ATOM_ULongLong function_section = 0;
  ATOM_ULongLong function_offset = 0;
  ATOM_ULongLong line_offset = 0;
  info->funcname[0] = '\0';
  info->filename[0] = '\0';

  // Now start for functions
  for (;;)
  {
    if (!getline_from_stream (s, temp))
      return true;

    if (!_strnicmp("entry point", temp.c_str(), 11))
    {
      break;
    }

    // skip xxxx:xxxxxxxx
    std::string function_start = temp.substr(0, 13);
    temp.erase (0, 13);
    remove_head_tail_spaces(temp);

    // get function name
    const char *p = temp.c_str();
    while ((*p) != ' ' && (*p) != '\t' && (*p) != '\0')
      p++;
    size_t len = p - temp.c_str();
    std::string function_name = temp.substr(0, len);
    temp.erase (0, len);
    remove_head_tail_spaces(temp);

    // get function offset
    p = temp.c_str();
    while ((*p) != ' ' && (*p) != '\t' && (*p) != '\0')
      p++;
    len = p - temp.c_str();
    std::string offset_str = temp.substr(0, len);
    if (1 != sscanf (offset_str.c_str(), "%I64X", &function_offset))
      return false;

    if (function_offset > preferred_load_address)
    {
      ATOM_ULongLong rva2 = function_offset - preferred_load_address;
      if (rva2 > info->offset - info->base_of_image)
      {
        line_offset = info->offset - info->base_of_image - 0x1000 - codesection_offset;
        sscanf (function_start.c_str(), "%I64X:%I64X", &function_section, &function_offset);
        if (info->funcname[0])
        {
          char buffer[512];
          ATOM_UndecorateFunctionName (info->funcname, buffer, 512);
          buffer[511] = '\0';
          strcpy (info->funcname, buffer);
        }
        break;
      }
      else
      {
        strcpy (info->funcname, function_name.c_str());
      }
    }
  }

  if (!info->funcname[0])
  {
    return false;
  }

  for (;;)
  {
    if (!getline_from_stream (s, temp))
    {
      info->lineno = -1;
      return true;
    }

    if (temp.length() > 16 && !_strnicmp (temp.c_str(), "Line numbers for", 16))
    {
      break;
    }
  }

  ATOM_ULongLong min_offset = 0;
  int function_line = 0;
  std::string filename;
  std::string fn;

  for(;;)
  {
    if (temp == "Exports")
    {
      break;
    }

    if (temp.length() > 16 && !_strnicmp (temp.c_str(), "Line numbers for", 16))
    {
      remove_tail_non_spaces (temp);
      remove_tail_non_spaces (temp);
      remove_head_tail_spaces (temp);
      int i = temp.length() - 1;
      if (temp[i] != ')')
        return false;

      i--;
      int r = 1;
      for (; i >= 0; --i)
      {
        if (temp[i] == ')')
          ++r;
        else if (temp[i] == '(')
          --r;

        if (r == 0)
          break;
      }

      if (r != 0)
      {
        return false;
      }

      fn = temp.substr(i+1, temp.length()-i-2);
    }
    else
    {
      struct StartAddr {
        ATOM_ULongLong section;
        ATOM_ULongLong offset;
        int lineno;
      } addr[4];

      int n = sscanf (temp.c_str(), "%d %I64X:%I64X %d %I64X:%I64X %d %I64X:%I64X %d %I64X:%I64X",
        &addr[0].lineno, &addr[0].section, &addr[0].offset,
        &addr[1].lineno, &addr[1].section, &addr[1].offset,
        &addr[2].lineno, &addr[2].section, &addr[2].offset,
        &addr[3].lineno, &addr[3].section, &addr[3].offset);

      for (int i = 0; i < n/3; ++i)
      {
        if (addr[i].section != function_section)
        {
          continue;
        }

        if (addr[i].offset < line_offset && addr[i].offset > min_offset)
        {
          min_offset = addr[i].offset;
          function_line = addr[i].lineno;
          filename = fn;
        }
      }
    }

    if (!getline_from_stream (s, temp))
    {
      info->lineno = -1;
      break;
    }
  }

  if (!filename.empty())
  {
    info->lineno = function_line;
    strcpy (info->filename, filename.c_str());
  }

  return true;
}

ATOM_DBGHLP_API bool ATOM_GetFunctionTraceInfoFromMapFile (const char *mapfile, ATOM_FunctionTraceInfo *info)
{
  std::ifstream s(mapfile, std::ios_base::in);
  if (s.good())
  {
    return _GetFunctionTraceInfoFromStream (s, info);
  }
  return false;
}

ATOM_DBGHLP_API bool ATOM_GetFunctionTraceInfoFromMapFileInMemory (const char *content, ATOM_FunctionTraceInfo *info)
{
  if (content && info)
  {
    std::stringstream s(content);
    return _GetFunctionTraceInfoFromStream (s, info);
  }
  return false;
}

ATOM_DBGHLP_API bool ATOM_UndecorateFunctionName (const char *in, char *out, size_t size)
{
  HMODULE hDbgHlp = ATOM_LoadDebugHelpDll ();
  if (hDbgHlp)
  {
    // UnDecorateSymbolName()
    typedef DWORD (WINAPI *tUDSN)( PCSTR, PSTR, DWORD, DWORD );
    tUDSN pUDSN = (tUDSN) GetProcAddress(hDbgHlp, "UnDecorateSymbolName" );
    if (pUDSN)
    {
      pUDSN( in, out, size, UNDNAME_NAME_ONLY );
      return true;
    }
  }
  return false;
}

