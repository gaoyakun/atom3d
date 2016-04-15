#ifndef __DEBUGHELP_STATICPTR_H__
#define __DEBUGHELP_STATICPTR_H__

namespace
{

  template <class T, bool Destruct>
  struct _dhSP_DestructImpl
  {
    void operator () (T *p) const
    {
      p->~T();
    }
  };

  template <class T>
  struct _dhSP_DestructImpl<T, false>
  {
    void operator () (T *p) const
    {
    }
  };

  struct _dhSP_Dummy;
}

template <class T, bool Destruct>
class ATOM_StaticPtr
{
public:
  ATOM_StaticPtr (void)
  {
    new ((T*)_instance) T;
  }

  template <class A1> 
  ATOM_StaticPtr (const A1& a1)
  {
    new ((T*)_instance) T(a1);
  }

  template <class A1,class A2> 
  ATOM_StaticPtr (const A1& a1, const A2& a2)
  {
    new ((T*)_instance) T(a1,a2);
  }

  template <class A1,class A2,class A3> 
  ATOM_StaticPtr (const A1& a1, const A2& a2, const A3& a3)
  {
    new ((T*)_instance) T(a1,a2,a3);
  }

  template <class A1,class A2,class A3,class A4> 
  ATOM_StaticPtr (const A1& a1, const A2& a2, const A3& a3, const A4& a4)
  {
    new ((T*)_instance) T(a1,a2,a3,a4);
  }

  template <class A1,class A2,class A3,class A4,class A5> 
  ATOM_StaticPtr (const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
  {
    new ((T*)_instance) T(a1,a2,a3,a4,a5);
  }

  template <class A1,class A2,class A3,class A4,class A5,class A6> 
  ATOM_StaticPtr (const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
  {
    new ((T*)_instance) T(a1,a2,a3,a4,a5,a6);
  }

  template <class A1,class A2,class A3,class A4,class A5,class A6,class A7> 
  ATOM_StaticPtr (const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
  {
    new ((T*)_instance) T(a1,a2,a3,a4,a5,a6,a7);
  }

  template <class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8> 
  ATOM_StaticPtr (const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
  {
    new ((T*)_instance) T(a1,a2,a3,a4,a5,a6,a7,a8);
  }

  template <class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9> 
  ATOM_StaticPtr (const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
  {
    new ((T*)_instance) T(a1,a2,a3,a4,a5,a6,a7,a8,a9);
  }

  template <class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9,class A10> 
  ATOM_StaticPtr (const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
  {
    new ((T*)_instance) T(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
  }

  ~ATOM_StaticPtr (void)
  {
    _dhSP_DestructImpl<T, Destruct>()((T*)_instance);
  }

  T * operator ->()
  {
    return (T*)_instance;
  }

  const T * operator ->() const
  {
    return (const T*)_instance;
  }

  T & operator *()
  {
    return *((T*)_instance);
  }

  const T & operator *() const
  {
    return *((const T*)_instance);
  }

  bool operator !() const
  {
    return false;
  }

  operator const struct _dhSP_Dummy * () const
  {
    return (const struct _dhSP_Dummy *)_instance;
  }

private:
  void operator = (const ATOM_StaticPtr &);

private:
  char _instance[sizeof(T)];
  T *_ptr;
};

#endif // __DEBUGHELP_STATICPTR_H__
