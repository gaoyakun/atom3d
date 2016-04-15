/**	\file variant.h
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_VARIANT_H
#define __ATOM_KERNEL_VARIANT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <algorithm>
#include "basedefs.h"

//! \class ATOM_Variant
//! ���ݷ�װ��.
//! \author ������
class ATOM_KERNEL_API ATOM_Variant
{
	friend ATOM_KERNEL_API bool ATOM_CALL operator == (const ATOM_Variant &left, const ATOM_Variant &right);
	friend ATOM_KERNEL_API bool ATOM_CALL operator != (const ATOM_Variant &left, const ATOM_Variant &right);

public:
	//! ��������
	enum Type
	{
		NONE = 0,
		INT = 1,
		INT_ARRAY = ~INT,
		FLOAT = 2,
		FLOAT_ARRAY = ~FLOAT,
		STRING = 3,
		POINTER = 4,
		VECTOR4 = 5,
		MATRIX44 = 6
	};

	struct VariantInternal
	{
		ATOM_Variant::Type type;
		unsigned count;
		unsigned memsize;

		union
		{
			int *i_array;
			float *f_array;
			int i;
			float f;
			char *s;
			void *p;
			void *dummy;
			float v[4];
			float m[16];
		};

		VariantInternal (void): type(ATOM_Variant::NONE), count(0), memsize(0), i_array(0) {}
	};

public:
	//! ���캯��
	ATOM_Variant (void);

	//! ����������
	//! \param i ��ʼֵ
	ATOM_Variant (int i);

	//! ���������鹹��
	//! \param i �����ַ
	//! \param count ���鳤��
	ATOM_Variant (const int *i, unsigned count);

	//! �Ӹ���������
	//! \param f ��ʼֵ
	ATOM_Variant (float f);

	//! �Ӹ������鹹��
	//! \param f �����ַ
	//! \param count ���鳤��
	ATOM_Variant (const float *f, unsigned count);

	//! ���ַ�������
	//! \param s �ַ���
	ATOM_Variant (const char *s);

	//! ��ָ�빹��
	//! \param p ��ʼֵ
	ATOM_Variant (void *p);

	//! ����ά��������
	//! \param v ��ʼֵ
	ATOM_Variant (const ATOM_Vector4f &v);

	//! ��4��4������
	//! \param m ��ʼֵ
	ATOM_Variant (const ATOM_Matrix4x4f &m);

	//! �������캯��
	//! \param rhs ��������
	ATOM_Variant (const ATOM_Variant &rhs);

	//! ��������
	~ATOM_Variant (void);

public:
	//! ��ֵ����������
	//! \param rhs ��ֵ����
	ATOM_Variant &operator = (const ATOM_Variant &rhs);

	//! ����Ϊ������״̬
	void invalidate (void);

	//! �����Ƿ���������
	//! \return true ���������� false ������������
	bool isArray (void) const;

	//! �����Ƿ���������״̬
	//! \return true �� false ����
	bool isEmpty (void) const;

	//! ��ȡ���鳤��
	//! \return ���鳤�ȣ� ������������򷵻�1
	unsigned getArraySize (void) const;

	//! ����
	//! \param rhs ��������
	void swap (ATOM_Variant &rhs);

	//! ��ȡ����ֵ
	//! \return ����ֵ
	int getI (void) const;

	//! ��ȡ���������Ԫ��ֵ
	//! \param index Ԫ������
	//! \return ����ֵ
	int getI (unsigned index) const;

	//! ��ȡ��������
	//! \return �����ַ 
	const int *getIntArray (void) const;

	//! ��������ֵ
	//! \param i ����ֵ
	void setI (int i);

	//! ������������
	//! \param i �����ַ
	//! \param count ���鳤��
	void setIntArray (const int *i, unsigned count);

	//! ��ȡ����ֵ
	//! \return ����ֵ
	float getF (void) const;

	//! ��ȡ���������Ԫ��ֵ
	//! \param index Ԫ������
	//! \return ����ֵ
	float getF (unsigned index) const;

	//! ��ȡ��������
	//! \return �����ַ 
	const float *getFloatArray (void) const;

	//! ���ø���ֵ
	//! \param f ����ֵ
	void setF (float f);

	//! ���ø�������
	//! \param f �����ַ
	//! \param count ���鳤��
	void setFloatArray (const float *f, unsigned count);

	//! ��ȡ�ַ���ֵ
	//! \return �ַ���ֵ
	const char *getS (void) const;

	//! �����ַ���ֵ
	//! \param s �ַ���ֵ
	void setS (const char *s);

	//! ��ȡָ��ֵ
	//! \return ָ��ֵ
	void *getP (void) const;

	//! ����ָ��ֵ
	//! \param p ָ��ֵ
	void setP (void *p);

	//! ������ά����ֵ
	//! \param v ��ά����ֵ
	void setV (const ATOM_Vector4f &v);

	//! ��ȡ��ά����ֵ
	//! \return ��ά����ֵ
	const float *getV (void) const;

	//! ����4��4����ֵ
	//! \param m 4��4����ֵ
	void setM (const ATOM_Matrix4x4f &m);

	//! ��ȡ4��4����ֵ
	//! \return ָ�����ֵ
	const float *getM (void) const;

	//! ��ȡ�ڲ���������
	//! \return �ڲ���������
	ATOM_Variant::Type getType (void) const;

private:
	VariantInternal _data;
};

//! ��������ATOM_Variant�Ƿ����
ATOM_KERNEL_API bool ATOM_CALL operator == (const ATOM_Variant &left, const ATOM_Variant &right);

//! ��������ATOM_Variant�Ƿ����
ATOM_KERNEL_API bool ATOM_CALL operator != (const ATOM_Variant &left, const ATOM_Variant &right);

namespace std
{
  template <>
  inline void swap (ATOM_Variant &left, ATOM_Variant &right)
  {
    left.swap (right);
  }
}

#endif // __CRAZY3D_UTILS_VARIANT_H__
/*! @} */
