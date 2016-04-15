/**	\file atomint.h
 *	ATOM_AtomInt类的声明.
 *
 *	\author 高雅昆
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
//! 原子整数操作封装类.
//! \author 高雅昆
class ATOM_AtomInt
{
private:
  long value;

public:
  //! 构造函数
  ATOM_AtomInt (void);

  //! 构造函数
  //! \param val 初始值
  ATOM_AtomInt (int val);

  //! 整数转换操作符重载
  //! \return 整数值
  operator int (void) const;

  //! 赋值操作符重载
  ATOM_AtomInt & operator = (int val);

  //! 自增操作符重载
  ATOM_AtomInt & operator ++ (void);

  //! 自增操作符重载
  int operator ++ (int);

  //! 自减操作符重载
  ATOM_AtomInt & operator -- (void);

  //! 自减操作符重载
  int operator -- (int);

  //! 自增操作符重载
  ATOM_AtomInt & operator += (int val);

  //! 自减操作符重载
  ATOM_AtomInt & operator -= (int val);

  //! 自乘操作符重载
  ATOM_AtomInt & operator *= (int val);

  //! 自除操作符重载
  ATOM_AtomInt & operator /= (int val);

  //! %=操作符重载
  ATOM_AtomInt & operator %= (int val);

  //! &=操作符重载
  ATOM_AtomInt & operator &= (int val);

  //! ^=操作符重载
  ATOM_AtomInt & operator ^= (int val);

  //! |=操作符重载
  ATOM_AtomInt & operator |= (int val);

  //! >>=操作符重载
  ATOM_AtomInt & operator >>= (int val);

  //! <<=操作符重载
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
