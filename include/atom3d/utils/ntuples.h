#ifndef __ATOM_UTILS_NTUPLES_H
#define __ATOM_UTILS_NTUPLES_H

#if _MSC_VER > 1000
# pragma once
#endif

template <typename Type,unsigned N>
struct ATOM_NTuples
  {
    typedef Type element_type;
    static const unsigned dimesion = N;

    Type elements[N];

    Type operator [](int index) const {
      return elements[index];
    }

    Type& operator [](int index) {
      return elements[index];
    }
  };

template <typename T1,typename T2,unsigned N>
static inline bool operator ==(const ATOM_NTuples<T1,N>& t1, const ATOM_NTuples<T2,N>& t2) {
  for ( int i = 0; i < N; ++i)
    if ( t1[i] != t2[i])
      return false;
  return true;
}

template <typename T1,typename T2,unsigned N>
static inline bool operator !=(const ATOM_NTuples<T1,N>& t1, const ATOM_NTuples<T2,N>& t2) {
  return !(t1 == t2);
}

#endif // __ATOM_UTILS_NTUPLES_H

