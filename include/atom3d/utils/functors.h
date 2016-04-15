#ifndef __ATOM_UTILS_FUNCTORS_H
#define __ATOM_UTILS_FUNCTORS_H

#include <cstdlib>

template <class T>
struct Newer
{
  T *operator () () const { return new T; }
  T *operator () (const T &rhs) const { return new T(rhs); }
};

template <class T>
struct Deletor
{
  void operator () (T *p) const { delete p; }
};

struct mallocator
{
  void * operator () (unsigned size) { return malloc(size); }
};

struct Freer
{
  void operator () (void *p) const { free (p); }
};

#endif // __ATOM_UTILS_FUNCTORS_H


