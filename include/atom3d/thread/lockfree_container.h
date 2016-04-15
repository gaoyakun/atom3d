#ifndef __ATOM3D_THREAD_LOCKFREE_CONTAINER_H
#define __ATOM3D_THREAD_LOCKFREE_CONTAINER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>
#include <algorithm>
#include <functional>
#include "tls.h"
#include "cas.h"
#include "hazardptr.h"
#include "node.h"
#include "lockfree_stack.h"
#include "lockfree_freelist.h"
#include "staticptr.h"
#include "mutex.h"

#define ATOM_TLS_OUT_OF_INDEXES 0xFFFFFFFF

template <class T>
class ATOM_LockFreeContainer
{
  struct priv_data
  {
    T *container;
    long changes;
  };

  struct tls_data
  {
	lf_Node<std::vector<T*> > *_tls_retire_list;
    HazardPtr<T> *_tls_reading_hp;
    T *_tls_writing;
    T *_tls_old_write;
    long _tls_old_changes;

    tls_data (void)
    {
      _tls_retire_list = 0;
      _tls_reading_hp = 0;
      _tls_writing = 0;
      _tls_old_write = 0;
      _tls_old_changes = 0;
    }
  };

  static const unsigned invalid_tls_index = 0xFFFFFFFF;
  typedef std::vector<tls_data> tls_data_vector;
  typedef typename tls_data_vector::iterator tls_data_iter;

public:
  ATOM_LockFreeContainer (void);
  ~ATOM_LockFreeContainer (void);

public:
  const T *acquire_read (void);
  void release_read (void);
  T *acquire_write (void);
  bool release_write (void);

private:
  tls_data *get_tls_data (void);
  unsigned new_tls_index (void);
  void free_tls_index (unsigned index);
  void retire (T *c);
  void scan (tls_data *tlsdata);
  static HazardPtrHashList<T> _hazardptr_list;

  ATOM_LockFreeStack<std::vector<T*> > _retirelists;
  static volatile unsigned _tlsdata;
  static ATOM_LockFreeFreeList<T> _freelist;
  static volatile unsigned _max_tls_index;

  static inline unsigned acquire_tls_index (void)
  {
    static volatile unsigned max_index = 0;
    unsigned index;
    do
    {
      index = max_index;
    }
    while (!ATOM_CAS((long volatile*)&max_index, index, index+1));
    return index;
  }

  static inline ATOM_FastMutex & get_tlsindex_mutex (void)
  {
    static ATOM_StaticPtr<ATOM_FastMutex, false> mutex;
    return *mutex;
  }

  static inline std::vector<unsigned> & get_tls_free_indexes (void)
  {
	static ATOM_StaticPtr<std::vector<unsigned>, false> indexes;
    return *indexes;
  }

private:
  volatile priv_data _data;
  unsigned _tls_index;
};

template <class T>
volatile unsigned ATOM_LockFreeContainer<T>::_max_tls_index = 0;

template <class T>
HazardPtrHashList<T> ATOM_LockFreeContainer<T>::_hazardptr_list;

template <class T>
ATOM_LockFreeFreeList<T> ATOM_LockFreeContainer<T>::_freelist;

template <class T>
volatile unsigned ATOM_LockFreeContainer<T>::_tlsdata = 0;

template <class T>
inline ATOM_LockFreeContainer<T>::ATOM_LockFreeContainer (void)
{
  _data.container = _freelist.alloc();
  _data.changes = 0;

  if (_tlsdata == 0)
  {
    unsigned tlsindex = ATOM_TlsAlloc ();
    if (tlsindex == ATOM_TLS_OUT_OF_INDEXES)
    {
      throw std::runtime_error("Couldn't allocate TLS index");
    }

    if (!ATOM_CAS((volatile long*)&_tlsdata, 0, tlsindex))
    {
      ATOM_TlsFree (tlsindex);
    }
  }

  _tls_index = new_tls_index();
}

template <class T>
inline ATOM_LockFreeContainer<T>::~ATOM_LockFreeContainer (void)
{
  for(;;)
  {
	lf_Node<std::vector<T*> > *retirelist = _retirelists.pop ();
    if (!retirelist)
    {
      break;
    }
    delete retirelist;
  }

  if (_tls_index != invalid_tls_index)
  {
    tls_data_vector *datavec = (tls_data_vector*)ATOM_TlsGetValue(_tlsdata);
    if (datavec)
    {
      delete (*datavec)[_tls_index]._tls_retire_list;
    }
  }

  free_tls_index (_tls_index);
}

template <class T>
inline unsigned ATOM_LockFreeContainer<T>::new_tls_index(void)
{
  ATOM_FastMutex::ScopeMutex l(get_tlsindex_mutex());
  std::vector<unsigned> &free_indexes = get_tls_free_indexes ();
  if (!free_indexes.empty())
  {
    unsigned ret = free_indexes.back();
    free_indexes.pop_back ();
    return ret;
  }
  return acquire_tls_index ();
}

template <class T>
inline void ATOM_LockFreeContainer<T>::free_tls_index (unsigned index)
{
  ATOM_FastMutex::ScopeMutex l(get_tlsindex_mutex());
  get_tls_free_indexes().push_back (index);
}

template <class T>
inline typename ATOM_LockFreeContainer<T>::tls_data *ATOM_LockFreeContainer<T>::get_tls_data(void)
{
  tls_data_vector *datavec = (tls_data_vector*)ATOM_TlsGetValue(_tlsdata);
  if (!datavec)
  {
    datavec = new tls_data_vector;
    ATOM_TlsSetValue(_tlsdata, datavec);
  }

  if (datavec->size() <= _tls_index)
  {
    datavec->resize(_tls_index+1);
  }

  return &(*datavec)[_tls_index];
}

template <class T>
inline void ATOM_LockFreeContainer<T>::scan (tls_data *tlsdata)
{
  std::vector<T*> *retirelist = tlsdata->_tls_retire_list ? &tlsdata->_tls_retire_list->_value : 0;
  if (!retirelist || retirelist->empty())
  {
    return;
  }

  std::vector<T*>::iterator i = retirelist->begin();
  while (i != retirelist->end())
  {
    if (!_hazardptr_list.check_exists(*i))
    {
      _freelist.garbage(*i);

      if (&(*i) != &retirelist->back())
      {
        *i = retirelist->back ();
      }
      
      retirelist->pop_back ();
    }
    else
    {
      ++i;
    }
  }
}

template <class T>
inline void ATOM_LockFreeContainer<T>::retire (T *c)
{
  tls_data *tlsdata = get_tls_data ();
  lf_Node<std::vector<T*> > *retirelist = tlsdata->_tls_retire_list;
  if (!retirelist)
  {
    retirelist = new lf_Node<std::vector<T*> >;
    _retirelists.push (retirelist);
    tlsdata->_tls_retire_list = retirelist;  
  }

  retirelist->_value.push_back (c);
  
  if (retirelist->_value.size())
  {
    scan (tlsdata);
  }
}

template <class T>
inline const T *ATOM_LockFreeContainer<T>::acquire_read (void)
{
  HazardPtr<T> *p = 0;
  T *ptr;
  long counter;

  do
  {
    if (p)
    {
      _hazardptr_list.release (p);
    }
    ptr = (T*)_data.container;
    counter = _data.changes;
    p = _hazardptr_list.acquire (ptr);
  }
  while (_data.container != ptr || _data.changes != counter);

  tls_data *tlsdata = get_tls_data ();
  tlsdata->_tls_reading_hp = p;

  return (const T*)ptr;
}

template <class T>
inline void ATOM_LockFreeContainer<T>::release_read (void)
{
  tls_data *tlsdata = get_tls_data();
  _hazardptr_list.release (tlsdata->_tls_reading_hp);
}

template <class T>
inline T *ATOM_LockFreeContainer<T>::acquire_write (void)
{
  tls_data *tlsdata = get_tls_data();
  T *pNew = tlsdata->_tls_writing;
  if (pNew)
  {
    _freelist.garbage(pNew);
  }

  HazardPtr<T> *p = 0;
  T *pOld;
  long counter;

  do
  {
    if (p)
    {
      _hazardptr_list.release (p);
    }
    pOld = (T*)_data.container;
    counter = _data.changes;
    p = _hazardptr_list.acquire (pOld);
  }
  while (_data.container != pOld || _data.changes != counter);

  //if (pOld != _data.container)
  //{
  //  __asm int 3
  //}

  pNew = _freelist.alloc(*pOld);

  _hazardptr_list.release (p);

  //release_read ();

  tlsdata->_tls_writing = pNew;
  tlsdata->_tls_old_write = pOld;
  tlsdata->_tls_old_changes = counter;

  return pNew;
}

template <class T>
inline bool ATOM_LockFreeContainer<T>::release_write (void)
{
  tls_data *tlsdata = get_tls_data();

  T *pNew = tlsdata->_tls_writing;
  T *pOld = tlsdata->_tls_old_write;
  long changes = tlsdata->_tls_old_changes;

  bool ret = ATOM_CAS2((long volatile*)&_data.container, (long)pOld, changes, (long)pNew, changes+1);
  if (ret)
  {
    retire (pOld);
    tlsdata->_tls_writing = 0;
  }
  return ret;
}

#endif // __ATOM3D_THREAD_LOCKFREE_CONTAINER_H

