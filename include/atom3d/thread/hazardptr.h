#ifndef __ATOM3D_THREAD_HAZARDPTR_H
#define __ATOM3D_THREAD_HAZARDPTR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "cas.h"

// Hazard pointer struct defination
template <class T>
struct HazardPtr
{
  volatile const T *_ptr;
  volatile HazardPtr<T> *_next;
  volatile long _active;
};

template <class T>
struct HazardPtrList
{
  volatile HazardPtr<T> *_head;
  volatile long _length;

  HazardPtr<T> *acquire (void);
  void release (HazardPtr<T> *ptr);

  HazardPtrList (void): _head(0), _length(0) {}
  ~HazardPtrList (void);
};

template <class T>
inline HazardPtrList<T>::~HazardPtrList (void)
{
  HazardPtr<T> *p = (HazardPtr<T> *)_head;

  while (p)
  {
    HazardPtr<T> *tmp = (HazardPtr<T>*)p->_next;
    ATOM_DELETE(p);
    p = tmp;
  }
}

template <class T>
inline HazardPtr<T> *HazardPtrList<T>::acquire (void)
{
  register HazardPtr<T> *p = (HazardPtr<T> *)_head;

  for (; p; p = (HazardPtr<T> *)p->_next)
  {
    if (p->_active || !dhCAS (&p->_active, 0L, 1L))
    {
      continue;
    }
    // Got one!
    return (HazardPtr<T>*)p;
  }

  p = ATOM_NEW(HazardPtr<T>);
  p->_active = 1L;
  p->_ptr = 0;

  HazardPtr<T> *oldhead;
  int oldlen;
  do
  {
    oldhead = (HazardPtr<T>*)_head;
    oldlen = _length;
    p->_next = oldhead;
  }
  while (!dhCAS2((long volatile*)&_head, (long)oldhead, oldlen, (long)p, oldlen+1));

  return (HazardPtr<T>*)p;
}

template <class T>
inline void HazardPtrList<T>::release (HazardPtr<T> *p)
{
  p->_active = 0L;
  p->_ptr = 0;
}

template <class T>
struct HazardPtrHashList
{
  static const int num_buckets = 12289;
  HazardPtrList<T> *_buckets;

  HazardPtrHashList (void);
  ~HazardPtrHashList (void);
  HazardPtr<T> * acquire (const T *ptr);
  void release (HazardPtr<T> *ptr);
  unsigned hash (const T *ptr) const;
  bool check_exists (const T *ptr) const;
};

template <class T>
inline HazardPtrHashList<T>::HazardPtrHashList (void)
{
  _buckets = ATOM_NEW_ARRAY(HazardPtrList<T>, num_buckets);
}

template <class T>
inline HazardPtrHashList<T>::~HazardPtrHashList (void)
{
  ATOM_DELETE_ARRAY(_buckets);
}

template <class T>
inline HazardPtr<T> *HazardPtrHashList<T>::acquire (const T *ptr)
{
  HazardPtrList<T> &bucket = _buckets[hash(ptr)];
  HazardPtr<T> *hp = bucket.acquire ();
  hp->_ptr = ptr;
  return hp;
}

template <class T>
inline void HazardPtrHashList<T>::release (HazardPtr<T> *ptr)
{
  HazardPtrList<T> &bucket = _buckets[hash((T*)ptr->_ptr)];
  bucket.release (ptr);
}

template <class T>
inline unsigned HazardPtrHashList<T>::hash (const T *ptr) const
{
  return (((unsigned long)ptr) >> 2) % num_buckets;
}

template <class T>
inline bool HazardPtrHashList<T>::check_exists (const T *ptr) const
{
  const HazardPtrList<T> &bucket = _buckets[hash(ptr)];
  HazardPtr<T> *p = (HazardPtr<T> *)bucket._head;
  while (p)
  {
    if (p->_ptr == ptr)
    {
      return true;
    }
    p = (HazardPtr<T> *)p->_next;
  }
  return false;
}

#endif // __ATOM3D_THREAD_HAZARDPTR_H
