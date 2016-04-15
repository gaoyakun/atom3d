#ifndef __ATOM_KERNEL_PLACEMENTNEW_H
#define __ATOM_KERNEL_PLACEMENTNEW_H

#if _MSC_VER > 1000
# pragma once
#endif

template <class T>
inline void ATOM_placement_new (T *p)
{
  new (p) T;
}

template <class T, class A1>
inline void ATOM_placement_new (T *p, A1 a1)
{
  new (p) T (a1);
}

template <class T, class A1, class A2>
inline void ATOM_placement_new (T *p, A1 a1, A2 a2)
{
  new (p) T (a1, a2);
}

template <class T, class A1, class A2, class A3>
inline void ATOM_placement_new (T *p, A1 a1, A2 a2, A3 a3)
{
  new (p) T (a1, a2, a3);
}

template <class T, class A1, class A2, class A3, class A4>
inline void ATOM_placement_new (T *p, A1 a1, A2 a2, A3 a3, A4 a4)
{
  new (p) T (a1, a2, a3, a4);
}

template <class T, class A1, class A2, class A3, class A4, class A5>
inline void ATOM_placement_new (T *p, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
{
  new (p) T (a1, a2, a3, a4, a5);
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6>
inline void ATOM_placement_new (T *p, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
  new (p) T (a1, a2, a3, a4, a5, a6);
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline void ATOM_placement_new (T *p, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
{
  new (p) T (a1, a2, a3, a4, a5, a6, a7);
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
inline void ATOM_placement_new (T *p, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
{
  new (p) T (a1, a2, a3, a4, a5, a6, a7, a8);
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline void ATOM_placement_new (T *p, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
{
  new (p) T (a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
inline void ATOM_placement_new (T *p, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
{
  new (p) T (a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

#endif // __ATOM_KERNEL_PLACEMENTNEW_H
