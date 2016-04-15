#ifndef __ATOM_DEBUGHELP_NEW_H__
#define __ATOM_DEBUGHELP_NEW_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include "mem.h"
#include "placementnew.h"
#include "meta/typetraits.h"

#if ATOM3D_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4127)
#pragma warning(disable:4267)
#endif

#if defined(ATOM_NO_DEBUGHELP_MEM_MNGR)

# define ATOM_NEW(classname, ...) (new classname(__VA_ARGS__))
# define ATOM_TRACE_NEW(classname, ...) (new classname(__VA_ARGS__))
# define ATOM_TAG_NEW(classname, tag, ...) (new classname(__VA_ARGS__))
# define ATOM_TAG_TRACE_NEW(classname, tag, ...) (new classname(__VA_ARGS__))
# define ATOM_NEW_ARRAY(classname, count) (new classname[count])
# define ATOM_TRACE_NEW_ARRAY(classname, count) (new classname[count])
# define ATOM_TAG_NEW_ARRAY(classname, tag, count) (new classname[count])
# define ATOM_TAG_TRACE_NEW_ARRAY(classname, tag, count) (new classname[count])
# define ATOM_DELETE(ptr) delete (ptr)
# define ATOM_DELETE_ARRAY(ptr) delete [] (ptr)

#else

#if ATOM3D_COMPILER_GCC
# define ATOM_NEW(classname, ...) ATOM_new< classname,ATOM_NEW_MARK >()(__FILE__, __LINE__, 1, ##__VA_ARGS__)
# define ATOM_TRACE_NEW(classname, ...) ATOM_NEW(classname, ##__VA_ARGS__)
# define ATOM_TAG_NEW(classname, tag, ...) ATOM_NEW(classname, ##__VA_ARGS__)
# define ATOM_TAG_TRACE_NEW(classname, tag, ...) ATOM_NEW(classname, ##__VA_ARGS__)
# define ATOM_NEW_ARRAY(classname, count) ATOM_new< classname,ATOM_NEWARRAY_MARK >()(__FILE__, __LINE__, count)
# define ATOM_TRACE_NEW_ARRAY(classname, count) ATOM_NEW_ARRAY(classname, count)
# define ATOM_TAG_NEW_ARRAY(classname, tag, count) ATOM_NEW_ARRAY(classname, count)
# define ATOM_TAG_TRACE_NEW_ARRAY(classname, tag, count) ATOM_NEW_ARRAY(classname, count)
# define ATOM_DELETE(ptr) ATOM_delete(ptr, MarkWrapper<ATOM_NEW_MARK>(), MarkWrapper<ATOM_NEWARRAY_MARK>(), __FILE__, __LINE__)
# define ATOM_DELETE_ARRAY(ptr) ATOM_delete(ptr, MarkWrapper<ATOM_NEWARRAY_MARK>(), MarkWrapper<ATOM_NEW_MARK>(), __FILE__, __LINE__)
#else
# define ATOM_NEW(classname, ...) ATOM_new< classname,ATOM_NEW_MARK >()(__FILE__, __LINE__, 1, __VA_ARGS__)
# define ATOM_TRACE_NEW(classname, ...) ATOM_NEW(classname, __VA_ARGS__)
# define ATOM_TAG_NEW(classname, tag, ...) ATOM_NEW(classname, __VA_ARGS__)
# define ATOM_TAG_TRACE_NEW(classname, tag, ...) ATOM_NEW(classname, __VA_ARGS__)
# define ATOM_NEW_ARRAY(classname, count) ATOM_new< classname,ATOM_NEWARRAY_MARK >()(__FILE__, __LINE__, count)
# define ATOM_TRACE_NEW_ARRAY(classname, count) ATOM_NEW_ARRAY(classname, count)
# define ATOM_TAG_NEW_ARRAY(classname, tag, count) ATOM_NEW_ARRAY(classname, count)
# define ATOM_TAG_TRACE_NEW_ARRAY(classname, tag, count) ATOM_NEW_ARRAY(classname, count)
# define ATOM_DELETE(ptr) ATOM_delete(ptr, MarkWrapper<ATOM_NEW_MARK>(), MarkWrapper<ATOM_NEWARRAY_MARK>(), __FILE__, __LINE__)
# define ATOM_DELETE_ARRAY(ptr) ATOM_delete(ptr, MarkWrapper<ATOM_NEWARRAY_MARK>(), MarkWrapper<ATOM_NEW_MARK>(), __FILE__, __LINE__)
#endif

#endif

#define ATOM_NEW_MARK ATOM_FOURCC('N','0','0','1')
#define ATOM_NEWARRAY_MARK ATOM_FOURCC('A','0','0','1')

template <unsigned Mark>
struct MarkWrapper
{
  static const unsigned mark = Mark;
};

template <class T>
struct ATOM_MallocTraits
{
	void *alloc(unsigned size, unsigned align, const char *filename, unsigned lineno)
	{
		return ATOM_AlignedMalloc(size, align, filename, lineno);
	}

	void dealloc (void *p, const char *filename, unsigned lineno)
	{
		ATOM_AlignedFree(p, filename, lineno);
	}
};

template <class T, unsigned Mark>
class ATOM_new
{
private:
  T *internalAlloc (const char *filename, int lineno, size_t count)
  {
    size_t *c = (size_t*)ATOM_MallocTraits<T>().alloc(count * sizeof(T) + 4 * sizeof(size_t), 16, filename, lineno);

    if (c)
    {
      *c++ = count;
      *c++ = sizeof(T);
      *c++ = Mark;
      c++;
      return reinterpret_cast<T*>(c);
    }

    return 0;
  }

public:
  T * operator () (const char *filename, int lineno, size_t count)
  {
    T *p = internalAlloc (filename, lineno, count);

    if (p && !ATOM_IsPOD<T>::result)
    {
      for (size_t i = 0; i < count; ++i)
      {
        ATOM_placement_new (p+i);
      }
    }

    return p;
  }

  template <class A1>
  T *operator () (const char *filename, int lineno, size_t count, A1 a1)
  {
    T *p = internalAlloc (filename, lineno, count);

    if (p)
    {
      for (size_t i = 0; i < count; ++i)
      {
        ATOM_placement_new (p+i, a1);
      }
    }

    return p;
  }

  template <class A1, class A2>
  T *operator () (const char *filename, int lineno, size_t count, A1 a1, A2 a2)
  {
    T *p = internalAlloc (filename, lineno, count);

    if (p)
    {
      for (size_t i = 0; i < count; ++i)
      {
        ATOM_placement_new (p+i, a1, a2);
      }
    }

    return p;
  }

  template <class A1, class A2, class A3>
  T *operator () (const char *filename, int lineno, size_t count, A1 a1, A2 a2, A3 a3)
  {
    T *p = internalAlloc (filename, lineno, count);

    if (p)
    {
      for (size_t i = 0; i < count; ++i)
      {
        ATOM_placement_new (p+i, a1, a2, a3);
      }
    }

    return p;
  }


  template <class A1, class A2, class A3, class A4>
  T *operator () (const char *filename, int lineno, size_t count, A1 a1, A2 a2, A3 a3, A4 a4)
  {
    T *p = internalAlloc (filename, lineno, count);

    if (p)
    {
      for (size_t i = 0; i < count; ++i)
      {
        ATOM_placement_new (p+i, a1, a2, a3, a4);
      }
    }

    return p;
  }

  template <class A1, class A2, class A3, class A4, class A5>
  T *operator () (const char *filename, int lineno, size_t count, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
  {
    T *p = internalAlloc (filename, lineno, count);

    if (p)
    {
      for (size_t i = 0; i < count; ++i)
      {
        ATOM_placement_new (p+i, a1, a2, a3, a4, a5);
      }
    }

    return p;
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6>
  T *operator () (const char *filename, int lineno, size_t count, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
  {
    T *p = internalAlloc (filename, lineno, count);

    if (p)
    {
      for (size_t i = 0; i < count; ++i)
      {
        ATOM_placement_new (p+i, a1, a2, a3, a4, a5, a6);
      }
    }

    return p;
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
  T *operator () (const char *filename, int lineno, size_t count, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
  {
    T *p = internalAlloc (filename, lineno, count);

    if (p)
    {
      for (size_t i = 0; i < count; ++i)
      {
        ATOM_placement_new (p+i, a1, a2, a3, a4, a5, a6, a7);
      }
    }

    return p;
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
  T *operator () (const char *filename, int lineno, size_t count, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
  {
    T *p = internalAlloc (filename, lineno, count);

    if (p)
    {
      for (size_t i = 0; i < count; ++i)
      {
        ATOM_placement_new (p+i, a1, a2, a3, a4, a5, a6, a7, a8);
      }
    }

    return p;
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
  T *operator () (const char *filename, int lineno, size_t count, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
  {
    T *p = internalAlloc (filename, lineno, count);

    if (p)
    {
      for (size_t i = 0; i < count; ++i)
      {
        ATOM_placement_new (p+i, a1, a2, a3, a4, a5, a6, a7, a8, a9);
      }
    }

    return p;
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
  T *operator () (const char *filename, int lineno, size_t count, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
  {
    T *p = internalAlloc (filename, lineno, count);

    if (p)
    {
      for (size_t i = 0; i < count; ++i)
      {
        ATOM_placement_new (p+i, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
      }
    }

    return p;
  }
};

template <class T, unsigned MarkCorrect, unsigned MarkWrong>
class ATOM_DeleteT
{
public:
  void operator () (T *p, const char *filename, int lineno)
  {
    if (p)
    {
      size_t *c = reinterpret_cast<size_t*>(p);

      if (c[-2] != MarkCorrect)
      {
        if (c[-2] == MarkWrong)
        {
          ATOM_GetErrorHandler()("ATOM_NEW/ATOM_NEW_ARRAY mismatch with ATOM_DELETE/ATOM_DELETE_ARRAY", filename, lineno, false);
        }
        else
        {
          ATOM_GetErrorHandler()("DebugHelp memory function version mismatch", filename, lineno, true);
          return;
        }
      }

      if (!ATOM_IsPOD<T>::result)
      {
        size_t count = *(c-4);
        size_t size = *(c-3);
        for (size_t i = 0; i < count; ++i)
        {
          p->~T ();
          p = reinterpret_cast<T*>(reinterpret_cast<char*>(p) + size);
        }
      }
      ATOM_MallocTraits<T>().dealloc(c-4, filename, lineno);
    }
  }
};

template <class T, class MarkCorrect, class MarkWrong>
void ATOM_delete (T *p, MarkCorrect, MarkWrong, const char *filename, int lineno)
{
  ATOM_DeleteT<T, MarkCorrect::mark, MarkWrong::mark>() (p, filename, lineno);
}

#if ATOM3D_COMPILER_MSVC
#pragma warning(pop)
#endif

#endif // __ATOM_DEBUGHELP_NEW_H__
