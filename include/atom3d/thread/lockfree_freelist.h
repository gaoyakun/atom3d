#ifndef __ATOM3D_THREAD_LOCKFREE_FREELIST_H
#define __ATOM3D_THREAD_LOCKFREE_FREELIST_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "cas.h"
#include "node.h"
#include "lockfree_stack.h"

template<class T> 
class ATOM_LockFreeFreeList 
{
  ATOM_LockFreeStack<T> _freelist;

public:
  ~ATOM_LockFreeFreeList()
  {
    for (;;)
    {
      T *instance = (T*)_freelist.pop ();

      if (!instance)
      {
        break;
      }

      free (instance);
    }
  }

  T * alloc(void)
  {
    lf_Node<T> * pInstance = _freelist.pop();
    if (!pInstance)
    {
      pInstance = (lf_Node<T>*)malloc(sizeof(lf_Node<T>));
    }
    if (pInstance)
    {
      pInstance->_next = 0;
      return new (&pInstance->_value) T;
    }
    return 0;
  }

  template <class A1>
  T * alloc (const A1 &a1)
  {
    lf_Node<T> * pInstance = _freelist.pop();
    if (!pInstance)
    {
      pInstance = (lf_Node<T>*)malloc(sizeof(lf_Node<T>));
    }
    if (pInstance)
    {
      pInstance->_next = 0;
      return new (&pInstance->_value) T(a1);
    }
    return 0;
  }

  template <class A1, class A2>
  T * alloc (const A1 &a1, const A2 &a2)
  {
    lf_Node<T> * pInstance = _freelist.pop();
    if (!pInstance)
    {
      pInstance = (lf_Node<T>*)malloc(sizeof(lf_Node<T>));
    }
    if (pInstance)
    {
      pInstance->_next = 0;
      return new (&pInstance->_value) T(a1, a2);
    }
    return 0;
  }

  template <class A1, class A2, class A3>
  T * alloc (const A1 &a1, const A2 &a2, const A3 &a3)
  {
    lf_Node<T> * pInstance = _freelist.pop();
    if (!pInstance)
    {
      pInstance = (lf_Node<T>*)malloc(sizeof(lf_Node<T>));
    }
    if (pInstance)
    {
      pInstance->_next = 0;
      return new (&pInstance->_value) T(a1, a2, a3);
    }
    return 0;
  }

  template <class A1, class A2, class A3, class A4>
  T * alloc (const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
  {
    lf_Node<T> * pInstance = _freelist.pop();
    if (!pInstance)
    {
      pInstance = (lf_Node<T>*)malloc(sizeof(lf_Node<T>));
    }
    if (pInstance)
    {
      pInstance->_next = 0;
      return new (&pInstance->_value) T(a1, a2, a3, a4);
    }
    return 0;
  }

  template <class A1, class A2, class A3, class A4, class A5>
  T * alloc (const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5)
  {
    lf_Node<T> * pInstance = _freelist.pop();
    if (!pInstance)
    {
      pInstance = (lf_Node<T>*)malloc(sizeof(lf_Node<T>));
    }
    if (pInstance)
    {
      pInstance->_next = 0;
      return new (&pInstance->_value) T(a1, a2, a3, a4, a5);
    }
    return 0;
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6>
  T * alloc (const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6)
  {
    lf_Node<T> * pInstance = _freelist.pop();
    if (!pInstance)
    {
      pInstance = (lf_Node<T>*)malloc(sizeof(lf_Node<T>));
    }
    if (pInstance)
    {
      pInstance->_next = 0;
      return new (&pInstance->_value) T(a1, a2, a3, a4, a5, a6);
    }
    return 0;
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
  T * alloc (const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7)
  {
    lf_Node<T> * pInstance = _freelist.pop();
    if (!pInstance)
    {
      pInstance = (lf_Node<T>*)malloc(sizeof(lf_Node<T>));
    }
    if (pInstance)
    {
      pInstance->_next = 0;
      return new (&pInstance->_value) T(a1, a2, a3, a4, a5, a6, a7);
    }
    return 0;
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
  T * alloc (const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8)
  {
    lf_Node<T> * pInstance = _freelist.pop();
    if (!pInstance)
    {
      pInstance = (lf_Node<T>*)malloc(sizeof(lf_Node<T>));
    }
    if (pInstance)
    {
      pInstance->_next = 0;
      return new (&pInstance->_value) T(a1, a2, a3, a4, a5, a6, a7, a8);
    }
    return 0;
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
  T * alloc (const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9)
  {
    lf_Node<T> * pInstance = _freelist.pop();
    if (!pInstance)
    {
      pInstance = (lf_Node<T>*)malloc(sizeof(lf_Node<T>));
    }
    if (pInstance)
    {
      pInstance->_next = 0;
      return new (&pInstance->_value) T(a1, a2, a3, a4, a5, a6, a7, a8, a9);
    }
    return 0;
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
  T * alloc (const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9, const A10 &a10)
  {
    lf_Node<T> * pInstance = _freelist.pop();
    if (!pInstance)
    {
      pInstance = (lf_Node<T>*)malloc(sizeof(lf_Node<T>));
    }
    if (pInstance)
    {
      pInstance->_next = 0;
      return new (&pInstance->_value) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }
    return 0;
  }

  void garbage(T * pInstance)
  {
    pInstance->~T();
    _freelist.push(reinterpret_cast<lf_Node<T> *>(pInstance));
  }

  void destroy (T *pInstance)
  {
    pInstance->~T();
    free (pInstance);
  }
};

#endif // __ATOM3D_THREAD_LOCKFREE_FREELIST_H
