#ifndef __ATOM_UTILS_AUTOREF_H
#define __ATOM_UTILS_AUTOREF_H

#include <cassert>
#include <cstddef>

#if (_MSC_VER <= 1300)
# define POLICY_DCL(_TYPE) class _TYPE
# define POLICY(T1, T2) T1
#else
# define POLICY_DCL(_TYPE) template <class> class _TYPE
# define POLICY(T1, T2) T1<T2>
#endif

template <class Type,POLICY_DCL(ReferencePolicy)>
class ATOM_AutoRef
  {
  public:
    struct _Dummy;

    typedef POLICY(ReferencePolicy, Type) policy_type;
  public:
    inline ATOM_AutoRef();
    inline ATOM_AutoRef(const char *classname, const char *objectname);
    inline ATOM_AutoRef(const ATOM_AutoRef& rhs);
    template <class T,POLICY_DCL(P)>
    inline ATOM_AutoRef(const ATOM_AutoRef<T,P>& rhs);
    inline ATOM_AutoRef(Type* p);
    template <class T>
    inline ATOM_AutoRef(T* p);
    inline ~ATOM_AutoRef();

  public:
    inline ATOM_AutoRef& operator =(const ATOM_AutoRef& rhs);
    template <class T,POLICY_DCL(P)>
    inline ATOM_AutoRef& operator =(const ATOM_AutoRef<T,P>& rhs) {
      Type *p = rhs.pointer ? dynamic_cast<Type*>(rhs.pointer) : 0;
      assert (p || !rhs.pointer);

      if (p)
        typename ATOM_AutoRef<T,P>::policy_type().acquire(rhs.pointer);

      if ( pointer)
        POLICY(ReferencePolicy, Type)().release(pointer);

      pointer = p;
      return *this;
    }

    inline Type *get () const { return pointer; }

    inline ATOM_AutoRef& operator =(Type* p);
    inline Type* operator ->() const;
    inline Type& operator *() const;
    inline bool operator !() const;
    inline operator _Dummy * () const;

    Type* pointer;
  };

// inline member function of ATOM_AutoRef
template <class Type,POLICY_DCL(ReferencePolicy)>
inline ATOM_AutoRef<Type, ReferencePolicy>::ATOM_AutoRef()
  : pointer(POLICY(ReferencePolicy, Type)().init()) {
}

template <class Type,POLICY_DCL(ReferencePolicy)>
inline ATOM_AutoRef<Type, ReferencePolicy>::ATOM_AutoRef(const char *classname, const char *objectname)
  : pointer(POLICY(ReferencePolicy, Type)().init(classname, objectname)) {
}

template <class Type,POLICY_DCL(ReferencePolicy)>
inline ATOM_AutoRef<Type, ReferencePolicy>::ATOM_AutoRef(Type* p)
  : pointer(p) {
  if ( pointer)
    POLICY(ReferencePolicy, Type)().acquire(pointer);
}

template <class Type,POLICY_DCL(ReferencePolicy)>
template <class T>
inline ATOM_AutoRef<Type, ReferencePolicy>::ATOM_AutoRef(T* p) {
  pointer = p ? dynamic_cast<Type*>(p) : 0;
  assert (pointer || !p);
  if ( pointer)
    POLICY(ReferencePolicy, Type)().acquire(pointer);
}

template <class Type,POLICY_DCL(ReferencePolicy)>
inline ATOM_AutoRef<Type, ReferencePolicy>::ATOM_AutoRef(const ATOM_AutoRef& rhs)
  : pointer(rhs.pointer) {
  if ( pointer)
    POLICY(ReferencePolicy, Type)().acquire(pointer);
}

template <class Type,POLICY_DCL(ReferencePolicy)>
template <class T,POLICY_DCL(P)>
inline ATOM_AutoRef<Type, ReferencePolicy>::ATOM_AutoRef(const ATOM_AutoRef<T,P>& rhs) {
  pointer = rhs.pointer ? dynamic_cast<Type*>(rhs.pointer) : 0;
  assert (pointer || !rhs.pointer);
  if ( pointer)
    POLICY(ReferencePolicy, Type)().acquire(pointer);
}

template <class Type,POLICY_DCL(ReferencePolicy)>
inline ATOM_AutoRef<Type, ReferencePolicy>::~ATOM_AutoRef() {
  if ( pointer)
  {
    POLICY(ReferencePolicy, Type)().release(pointer);
  }
}

template <class Type,POLICY_DCL(ReferencePolicy)>
inline ATOM_AutoRef<Type, ReferencePolicy>& 
ATOM_AutoRef<Type, ReferencePolicy> ::operator =(const ATOM_AutoRef& rhs) {
  if ( rhs.pointer)
  {
    POLICY(ReferencePolicy, Type)().acquire(rhs.pointer);
  }
  if ( pointer)
  {
    POLICY(ReferencePolicy, Type)().release(pointer);
  }
  pointer = rhs.pointer;
  return *this;
}

template <class Type,POLICY_DCL(ReferencePolicy)>
inline ATOM_AutoRef<Type, ReferencePolicy> ::operator _Dummy * () const {
  return (_Dummy*)pointer;
}

template <class Type,POLICY_DCL(ReferencePolicy)>
inline ATOM_AutoRef<Type, ReferencePolicy>& ATOM_AutoRef<Type, ReferencePolicy> ::operator =(Type* p) {
  if (p)
    POLICY(ReferencePolicy, Type)().acquire(p);

  if ( pointer)
    POLICY(ReferencePolicy, Type)().release(pointer);

  pointer = p;

  return *this;
}

template <class Type,POLICY_DCL(ReferencePolicy)>
inline Type *ATOM_AutoRef<Type, ReferencePolicy> ::operator ->() const {
  return pointer;
}

template <class Type,POLICY_DCL(ReferencePolicy)>
inline Type &ATOM_AutoRef<Type, ReferencePolicy> ::operator *() const {
  return *pointer;
}

template <class Type,POLICY_DCL(ReferencePolicy)>
inline bool ATOM_AutoRef<Type, ReferencePolicy> ::operator !() const {
  return pointer == 0;
}

template <class Type1,POLICY_DCL(ReferencePolicy1), class Type2, POLICY_DCL(ReferencePolicy2) >
inline bool operator ==(const ATOM_AutoRef<Type1,ReferencePolicy1>& ref1, const ATOM_AutoRef<Type2,ReferencePolicy2>& ref2) {
  return ref1.pointer == ref2.pointer;
}

template <class Type1,class Type2, POLICY_DCL(ReferencePolicy)>
inline bool operator ==(const Type1 *p, const ATOM_AutoRef<Type2,ReferencePolicy>& ref) {
  return p == ref.pointer;
}

template <class Type1,class Type2, POLICY_DCL(ReferencePolicy)>
inline bool operator ==(const ATOM_AutoRef<Type1,ReferencePolicy>& ref, const Type2 *p) {
  return p == ref.pointer;
}

template <class Type1,POLICY_DCL(ReferencePolicy1), class Type2, POLICY_DCL(ReferencePolicy2) >
inline bool operator !=(const ATOM_AutoRef<Type1,ReferencePolicy1>& ref1, const ATOM_AutoRef<Type2,ReferencePolicy2>& ref2) {
  return !operator == (ref1, ref2);
}

template <class Type1, class Type2, POLICY_DCL(ReferencePolicy)>
inline bool operator !=(const Type1 *p, const ATOM_AutoRef<Type2,ReferencePolicy>& ref) {
  return !operator == (p, ref);
}

template <class Type1, class Type2, POLICY_DCL(ReferencePolicy)>
inline bool operator !=(const ATOM_AutoRef<Type1,ReferencePolicy>& ref, const Type2 *p) {
  return !operator == (ref, p);
}

template <class Type1,POLICY_DCL(ReferencePolicy1), class Type2, POLICY_DCL(ReferencePolicy2) >
inline bool operator >(const ATOM_AutoRef<Type1,ReferencePolicy1>& ref1, const ATOM_AutoRef<Type2,ReferencePolicy2>& ref2) {
  return ref1.pointer > ref2.pointer;
}

template <class Type1,class Type2, POLICY_DCL(ReferencePolicy)>
inline bool operator >(const Type1 *p, const ATOM_AutoRef<Type2,ReferencePolicy>& ref) {
  return p > ref.pointer;
}

template <class Type1,class Type2,POLICY_DCL(ReferencePolicy)>
inline bool operator >(const ATOM_AutoRef<Type1,ReferencePolicy>& ref, const Type2 *p) {
  return ref.pointer > p;
}

template <class Type1,POLICY_DCL(ReferencePolicy1), class Type2, POLICY_DCL(ReferencePolicy2) >
inline bool operator <(const ATOM_AutoRef<Type1,ReferencePolicy1>& ref1, const ATOM_AutoRef<Type2,ReferencePolicy2>& ref2) {
  return ref1.pointer < ref2.pointer;
}

template <class Type1,class Type2, POLICY_DCL(ReferencePolicy)>
inline bool operator <(const Type1 *p, const ATOM_AutoRef<Type2,ReferencePolicy>& ref) {
  return p < ref.pointer;
}

template <class Type1,class Type2,POLICY_DCL(ReferencePolicy)>
inline bool operator <(const ATOM_AutoRef<Type1,ReferencePolicy>& ref, const Type2 *p) {
  return ref.pointer < p;
}

template <class Type1,POLICY_DCL(ReferencePolicy1), class Type2, POLICY_DCL(ReferencePolicy2) >
inline bool operator >=(const ATOM_AutoRef<Type1,ReferencePolicy1>& ref1, const ATOM_AutoRef<Type2,ReferencePolicy2>& ref2) {
  return ref1.pointer >= ref2.pointer;
}

template <class Type1,class Type2, POLICY_DCL(ReferencePolicy)>
inline bool operator >=(const Type1 *p, const ATOM_AutoRef<Type2,ReferencePolicy>& ref) {
  return p >= ref.pointer;
}

template <class Type1,class Type2,POLICY_DCL(ReferencePolicy)>
inline bool operator >=(const ATOM_AutoRef<Type1,ReferencePolicy>& ref, const Type2 *p) {
  return ref.pointer >= p;
}

template <class Type1,POLICY_DCL(ReferencePolicy1), class Type2, POLICY_DCL(ReferencePolicy2) >
inline bool operator <=(const ATOM_AutoRef<Type1,ReferencePolicy1>& ref1, const ATOM_AutoRef<Type2,ReferencePolicy2>& ref2) {
  return ref1.pointer <= ref2.pointer;
}

template <class Type1,class Type2, POLICY_DCL(ReferencePolicy)>
inline bool operator <=(const Type1 *p, const ATOM_AutoRef<Type2,ReferencePolicy>& ref) {
  return p <= ref.pointer;
}

template <class Type1,class Type2, POLICY_DCL(ReferencePolicy)>
inline bool operator <=(const ATOM_AutoRef<Type1,ReferencePolicy>& ref, const Type2 *p) {
  return ref.pointer <= p;
}

#endif // __ATOM_UTILS_AUTOREF_H
