#include "stdafx.h"

static ATOM_STACK<ATOM_STRING> _S_cwd_stack;

static inline void ConvertToNativeString(ATOM_STRING& str) {
#ifdef WIN32
  for ( ATOM_STRING::iterator ch = str.begin(); ch != str.end(); ++ch)
    if ( (*ch) == '/')
      *ch = '\\';
#endif
}

static inline void RemoveEndSlash(ATOM_STRING& str) {
  if ( !str.empty() && ('/' == str[str.length() - 1] || '\\' == str[str.length() - 1]))
  {
    ATOM_STRING s = str.substr(0, str.length() - 1);
    str.swap(s);
  }
}

static inline void DuplicateString(char* dst, unsigned dst_len, const char* src, unsigned src_len) {
  if ( dst_len >= src_len)
    memcpy(dst, src, src_len);
  else
  {
    memcpy(dst, src, dst_len - 1);
    dst[dst_len - 1] = '\0';
  }
}

ATOM_KERNEL_API unsigned ATOM_SysGetTempDir(char* buffer, unsigned size) {
#ifdef WIN32
  if ( buffer == 0)
    return GetTempPathA(0, 0);
  return GetTempPathA(size, buffer);
#else
  if ( buffer == 0)
    return 5;
  else
  {
    if ( size == 0)
      return 0;

    if ( size >= 5)
      strcpy(buffer, "/tmp");
    else
    {
      memcpy(buffer, "/tmp", size - 1);
      buffer[size - 1] = '\0';
    }
  }
#endif

}

ATOM_KERNEL_API const char* ATOM_SysFindFirst(const char* dir, ATOM_FIND_HANDLE* handle, ATOM_DIRECTORY_TYPE* data) {
  if ( dir == 0)
    return 0;

#ifdef WIN32
  ATOM_STRING dirpath(dir);
  if ( dirpath[dirpath.length() - 1] == '/' || dirpath[dirpath.length() - 1] == '\\')
    dirpath += "*";
  else
    dirpath += "/*";
  ATOM_FIND_HANDLE h = FindFirstFileA(dirpath.c_str(), data);
  if ( handle)
    *handle = h;
  if ( h == ATOM_INVALID_FIND_HANDLE_VALUE)
    return 0;
  return data->cFileName;
#else
  const char* dummy_first_name = ".";
  ATOM_FIND_HANDLE dir = opendir(dir); 
  if ( handle)
    *handle = dir;
  return (dir == ATOM_INVALID_FIND_HANDLE_VALUE) ? 0 : dummy_first_name;
#endif
}

ATOM_KERNEL_API const char* ATOM_SysFindNext(ATOM_FIND_HANDLE handle, ATOM_DIRECTORY_TYPE* data) {
  if ( handle == ATOM_INVALID_FIND_HANDLE_VALUE)
    return 0;

#ifdef WIN32
  if ( FindNextFileA(handle, data) == 0)
    return 0;
  return data->cFileName;
#else
  struct dirent* dp;
  if ( (dp = readdir(handle)) == 0)
    return 0;
  return dp->d_name;
#endif
}

ATOM_KERNEL_API void ATOM_SysCloseFind(ATOM_FIND_HANDLE handle) {
#ifdef WIN32
  if ( handle != ATOM_INVALID_FIND_HANDLE_VALUE)
    FindClose(handle);
#else
  if ( handle != ATOM_INVALID_FIND_HANDLE_VALUE)
    closedir(handle);
#endif
}

ATOM_KERNEL_API bool ATOM_SysFileExists(const char* path) {
  if ( path == 0)
    return false;
#ifdef WIN32
  ATOM_STRING s(path);
  ConvertToNativeString(s);
  return GetFileAttributesA(s.c_str()) != 0xFFFFFFFF;
#else
  struct stat path_stat;
  return stat(path, &path_stat) == 0;
#endif
}

ATOM_KERNEL_API bool ATOM_SysIsDirectory(const char* path) {
  if ( path == 0)
    return false;
#ifdef WIN32
  ATOM_STRING s(path);
  ConvertToNativeString(s);
  DWORD attributes = GetFileAttributesA(s.c_str());
  if ( attributes == 0xFFFFFFFF)
    return false;
  return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
  struct stat path_stat;
  if ( stat(path, &path_stat) != 0)
    return false;
  return S_ISDIR(path_stat.st_mode);
#endif
}

static inline bool IsDirEmpty(const char* path) {
  ATOM_FIND_HANDLE h;
  ATOM_DIRECTORY_TYPE data;
  bool isempty = true;
  const char* f = ATOM_SysFindFirst(path, &h, &data);

  while ( f)
  {
    if ( strcmp(f, ".") && strcmp(f, ".."))
    {
      isempty = false;
      break;
    }
    f = ATOM_SysFindNext(h, &data);
  }

  ATOM_ASSERT(h != ATOM_INVALID_FIND_HANDLE_VALUE);
  ATOM_SysCloseFind(h);

  return isempty;
}

ATOM_KERNEL_API bool ATOM_SysIsEmpty(const char* path) {
  if ( path == 0)
    exit(1);
#ifdef WIN32
  WIN32_FILE_ATTRIBUTE_DATA fad;
  if ( !GetFileAttributesExA(path, GetFileExInfoStandard, &fad))
    return false;
  return ((fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
       ? IsDirEmpty(path)
       : (fad.nFileSizeHigh == 0 && fad.nFileSizeLow == 0);
#else
  struct stat path_stat;
  if ( stat(ph.string().c_str(), &path_stat) != 0)
    return false;
  return S_ISDIR(path_stat.st_mode) ? IsDirEmpty(path) : (path_stat.st_size == 0);
#endif
}

ATOM_KERNEL_API bool ATOM_SysCreateDirectory(const char* path) {
  if ( path == 0)
    return false;
#ifdef WIN32
  ATOM_STRING s(path);
  ConvertToNativeString(s);
  RemoveEndSlash(s);
  if ( !CreateDirectoryA(s.c_str(), 0))
    return false;
#else
  if ( mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) != 0)
    return false;
#endif
  return true;
}

ATOM_KERNEL_API bool ATOM_SysRemove(const char* path) {
  if ( path == 0)
    return false;

  if ( ATOM_SysFileExists(path))
  {
#ifdef WIN32
    ATOM_STRING s(path);
    ConvertToNativeString(s);
    if ( ATOM_SysIsDirectory(s.c_str()))
    {
      if ( !RemoveDirectoryA(s.c_str()))
        return false;
    }
    else
    {
      if ( !DeleteFileA(s.c_str()))
        return false;
    }
#else
    if ( remove(path) != 0)
      return false;
#endif
    return true;
  }
  return false;
}

ATOM_KERNEL_API long ATOM_SysRemoveAll(const char* path) {
  if ( path == 0)
    return 0;

  if ( !ATOM_SysFileExists(path))
    return 0;

  long count = 1;
  if ( ATOM_SysIsDirectory(path))
  {
    ATOM_FIND_HANDLE h;
    ATOM_DIRECTORY_TYPE data;

    const char* iter;
    ATOM_STRING s1( path), s2;
    RemoveEndSlash(s1);

    iter = ATOM_SysFindFirst(s1.c_str(), &h, &data);
    while ( iter)
    {
      if ( strcmp(iter, ".") && strcmp(iter, ".."))
      {
        s2 = s1 + "\\" + iter;
        count += ATOM_SysRemoveAll(s2.c_str());
      }
      iter = ATOM_SysFindNext(h, &data);
    }
    if ( h != ATOM_INVALID_FIND_HANDLE_VALUE)
      ATOM_SysCloseFind(h);
  }

  if ( !ATOM_SysRemove(path))
    return count - 1;

  return count;
}

ATOM_KERNEL_API bool ATOM_SysRename(const char* old_path, const char* new_path) {
  if ( old_path == 0 || new_path == 0)
    return false;

#ifdef WIN32
  ATOM_STRING s1( old_path), s2(new_path);
  ConvertToNativeString(s1);
  ConvertToNativeString(s2);
  if ( !MoveFileA(s1.c_str(), s2.c_str()))
    return false;
#else
  if ( ATOM_SysFileExists(new_path) || rename(old_path, new_path) != 0)
    return false;
#endif

  return true;
}

ATOM_KERNEL_API bool ATOM_SysDeepCopy(const char* from, const char* to, bool recurse) {
  if ( from == 0 || to == 0)
  {
    ATOM_LOGGER::error ("[ATOM_SysDeepCopy] Invalid arguments: 0x%08X, 0x%08X, %d.\n", from, to, int(recurse));
    return false;
  }
  if ( !ATOM_SysIsDirectory(from) || !ATOM_SysIsDirectory(to))
  {
    ATOM_LOGGER::error ("[l3AbstractFS::DeepCopy] Invalid path: %s, %s.\n", from, to);
    return false;
  }

  bool succ = true;

  ATOM_FIND_HANDLE handle;
  ATOM_DIRECTORY_TYPE data;
  const char* p = ATOM_SysFindFirst(from, &handle, &data);

  while ( p)
  {
    if ( strcmp(p, ".") && strcmp(p, ".."))
    {
      unsigned n = ATOM_SysMergePath(from, p, 0, 0);
      char* srcbuf = ATOM_NEW_ARRAY(char, n);
      ATOM_SysMergePath(from, p, srcbuf, n);

      n = ATOM_SysMergePath(to, p, 0, 0);
      char* dstbuf = ATOM_NEW_ARRAY(char, n);
      ATOM_SysMergePath(to, p, dstbuf, n);

      if ( ATOM_SysIsDirectory(srcbuf))
      {
        if ( recurse)
        {
          if ( !ATOM_SysCreateDirectory(dstbuf))
            ATOM_LOGGER::error ("[ATOM_SysDeepCopy] Couldn't create destination directory %s.\n", dstbuf);

          if ( ATOM_SysIsDirectory(dstbuf))
          {
            if ( !ATOM_SysDeepCopy(srcbuf, dstbuf, recurse))
              succ = false;
          }
        }
      }
      else
      {
        if ( !ATOM_SysCopyFile(srcbuf, dstbuf))
        {
          ATOM_LOGGER::error ("[ATOM_DeepCopy] Error copying file from %s to %s.\n", srcbuf, dstbuf);
          succ = false;
        }
      }

      ATOM_DELETE(srcbuf);
      ATOM_DELETE(dstbuf);
    }

    p = ATOM_SysFindNext(handle, &data);
  }

  if ( handle)
    ATOM_SysCloseFind(handle);
  return succ;
}

ATOM_KERNEL_API bool ATOM_SysCopyFile(const char* from, const char* to) {
  if ( from == 0 || to == 0)
  {
    ATOM_LOGGER::error ("[ATOM_SysCopyFile] Invalid arguments: 0x%08X, 0x%08X, %d.\n", from, to);
    return false;
  }

#ifdef WIN32
  ATOM_STRING s1( from), s2(to);
  ConvertToNativeString(s1);
  ConvertToNativeString(s2);
  if ( !ATOM_SysFileExists(s1.c_str()))
    return false;

  if ( !CopyFileA(s1.c_str(), s2.c_str(), false))
    return false;
#else
  const std::size_t buf_sz = 32768;
  char* buf = ATOM_NEW_ARRAY(char, buf_sz);
  int infile, outfile = 0;

  if ( (infile = open(from, O_RDONLY)) < 0
    || (outfile = open(to, O_WRONLY | O_CREATE | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
  {
    if ( infile != 0)
      close(infile);
    ATOM_DELETE(buf);
    return false;
  }

  ssize_t sz;
  while ( (sz = read(infile, buf, buf_sz)) > 0 && (sz = write(outfile, buf, sz)) > 0)
  {
  }

  close(infile);
  close(outfile);
  ATOM_DELETE(buf);

  if ( sz != 0)
    return false;
#endif
  return true;
}

ATOM_KERNEL_API void ATOM_SysPushCWD() {
  unsigned n = ATOM_SysGetCWD(0, 0);
  char* p = ATOM_NEW_ARRAY(char, n);
  ATOM_SysGetCWD(p, n);

  _S_cwd_stack.push(p);
  ATOM_DELETE(p);
}

ATOM_KERNEL_API void ATOM_SysPopCWD() {
  if ( !_S_cwd_stack.empty())
  {
    ATOM_SysSetCWD(_S_cwd_stack.top().c_str());
    _S_cwd_stack.pop();
  }
}

ATOM_KERNEL_API unsigned ATOM_SysGetCWD(char* buffer, unsigned size) {
#ifdef WIN32
  DWORD sz;
  if ( (sz = GetCurrentDirectoryA(0, static_cast<char*>(0))) == 0)
    exit(1);

  if ( buffer == 0)
    return sz;

  if ( size >= sz)
  {
    if ( GetCurrentDirectoryA(size, buffer) == 0)
      exit(1);
    return 0;
  }
  else
  {
    if ( size == 0)
      return 0;

    char* buf = ATOM_NEW_ARRAY(char, sz);
    if ( GetCurrentDirectoryA(sz, buf) == 0)
    {
      ATOM_DELETE(buf);
      exit(1);
    }

    memcpy(buffer, buf, size - 1);
    buffer[size - 1] = '\0';

    ATOM_DELETE(buf);
    return 0;
  }
#else
  long path_max = pathconf(".", _PC_PATH_MAX);
  if ( path_max < 1)
    exit(1);

  if ( buffer == 0)
    return path_max;

  if ( size >= path_max)
  {
    if ( getcwd(buffer, size) == 0)
      exit(1);
    return 0;
  }
  else
  {
    if ( size == 0)
      return 0;

    char* buf = ATOM_NEW_ARRAY(char, path_max);
    if ( getcwd(buf, path_max) == 0)
    {
      ATOM_DELETE(buf);
      exit(1);
    }
    memcpy(buffer, buf, size - 1);
    buffer[size - 1] = '\0';

    ATOM_DELETE(buf);
    return 0;
  }
#endif

}

ATOM_KERNEL_API bool ATOM_SysSetCWD(const char* path) {
  if ( path == 0)
    return false;

#ifdef WIN32
  return SetCurrentDirectoryA(path) != FALSE;
#else
  return chdir(path) == 0;
#endif
}

ATOM_KERNEL_API unsigned ATOM_SysMergePath(const char* path, const char* relative, char* buffer, unsigned size) {
  if ( path == 0 || relative == 0)
    return 0;

  unsigned len1 = strlen(path);
  unsigned len2 = strlen(relative);

  if ( len1 == 0 || (path[len1 - 1] != '/' && path[len1 - 1] != '\\'))
    len1++;
  if ( len2 > 0 && (relative[0] == '/' || relative[0] == '\\'))
    len2--;

  if ( buffer == 0)
    return len1 + len2 + 1;

  if ( size == 0)
    return 0;

  if ( size >= len1 + len2 + 1)
  {
    strcpy(buffer, path);
    if ( len1 > strlen(path))
    {
#ifdef WIN32
      buffer[len1 - 1] = '\\';
#else
      buffer[len1 - 1] = '/';
#endif
      buffer[len1] = '\0';
    }
    if ( len2 < strlen(relative))
      relative++;
    strcat(buffer, relative);
  }
  else
  {
    char* p = ATOM_NEW_ARRAY(char, len1 + len2 + 1);
    strcpy(p, path);
    if ( len1 > strlen(path))
    {
#ifdef WIN32
      p[len1 - 1] = '\\';
#else
      p[len1 - 1] = '/';
#endif
      p[len1] = '\0';
    }
    if ( len2 < strlen(relative))
      relative++;
    strcat(p, relative);
    memcpy(buffer, p, size - 1);
    buffer[size - 1] = '\0';
    ATOM_DELETE(p);
  }
#ifdef WIN32
  for (unsigned i = 0; i < strlen(buffer); ++i)
    if (buffer[i] == '/')
      buffer[i] = '\\';
#endif
  return 0;
}
