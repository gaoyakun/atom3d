/**	\file atomint.h
 *	ATOM_AtomInt�������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_ATOMINT_H
#define __ATOM_KERNEL_ATOMINT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>

//! \class ATOM_AtomInt
//! ԭ������������װ��.
//! \author ������
class ATOM_AtomInt
{
private:
  long value;

public:
  //! ���캯��
  ATOM_AtomInt (void);

  //! ���캯��
  //! \param val ��ʼֵ
  ATOM_AtomInt (int val);

  //! ����ת������������
  //! \return ����ֵ
  operator int (void) const;

  //! ��ֵ����������
  ATOM_AtomInt & operator = (int val);

  //! ��������������
  ATOM_AtomInt & operator ++ (void);

  //! ��������������
  int operator ++ (int);

  //! �Լ�����������
  ATOM_AtomInt & operator -- (void);

  //! �Լ�����������
  int operator -- (int);

  //! ��������������
  ATOM_AtomInt & operator += (int val);

  //! �Լ�����������
  ATOM_AtomInt & operator -= (int val);

  //! �Գ˲���������
  ATOM_AtomInt & operator *= (int val);

  //! �Գ�����������
  ATOM_AtomInt & operator /= (int val);

  //! %=����������
  ATOM_AtomInt & operator %= (int val);

  //! &=����������
  ATOM_AtomInt & operator &= (int val);

  //! ^=����������
  ATOM_AtomInt & operator ^= (int val);

  //! |=����������
  ATOM_AtomInt & operator |= (int val);

  //! >>=����������
  ATOM_AtomInt & operator >>= (int val);

  //! <<=����������
  ATOM_AtomInt & operator <<= (int val);
};

// inline 
inline ATOM_AtomInt::ATOM_AtomInt (void)
{
  value = 0;
}

inline ATOM_AtomInt::ATOM_AtomInt (int val)
{
  value = 0;
  ::InterlockedExchange ((LONG*)&value, val);
}

inline ATOM_AtomInt::operator int (void) const
{
  return value;
}

inline ATOM_AtomInt & ATOM_AtomInt::operator = (int val)
{
  ::InterlockedExchange ((LONG*)&value, val);
  return *this;
}

inline ATOM_AtomInt & ATOM_AtomInt::operator ++ (void)
{
  ::InterlockedIncrement ((LONG*)&value);
  return *this;
}

inline int ATOM_AtomInt::operator ++ (int)
{
  int ret = value;
  ::InterlockedIncrement ((LONG*)&value);
  return ret;
}

inline ATOM_AtomInt & ATOM_AtomInt::operator -- (void)
{
  ::InterlockedDecrement ((LONG*)&value);
  return *this;
}

inline int ATOM_AtomInt::operator -- (int)
{
  int ret = value;
  ::InterlockedDecrement ((LONG*)&value);
  return ret;
}

inline ATOM_AtomInt & ATOM_AtomInt::operator += (int val)
{
  return operator = (value + val);
}

inline ATOM_AtomInt & ATOM_AtomInt::operator -= (int val)
{
  return operator = (value - val);
}

inline ATOM_AtomInt & ATOM_AtomInt::operator *= (int val)
{
  return operator = (value * val);
}

inline ATOM_AtomInt & ATOM_AtomInt::operator /= (int val)
{
  return operator = (value / val);
}

inline ATOM_AtomInt & ATOM_AtomInt::operator %= (int val)
{
  return operator = (value % val);
}

inline ATOM_AtomInt & ATOM_AtomInt::operator &= (int val)
{
  return operator = (value & val);
}

inline ATOM_AtomInt & ATOM_AtomInt::operator ^= (int val)
{
  return operator = (value ^ val);
}

inline ATOM_AtomInt & ATOM_AtomInt::operator |= (int val)
{
  return operator = (value | val);
}

inline ATOM_AtomInt & ATOM_AtomInt::operator >>= (int val)
{
  return operator = (value >> val);
}

inline ATOM_AtomInt & ATOM_AtomInt::operator <<= (int val)
{
  return operator = (value << val);
}

#endif // __ATOM_KERNEL_ATOMINT_H

/*! @} */
