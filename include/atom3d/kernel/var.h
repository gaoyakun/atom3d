/**	\file kernel/var.h
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_VAR_H
#define __ATOM_KERNEL_VAR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "../ATOM_dbghlp.h"
#include "kernel.h"

//! \class ATOM_ScriptVar
//! �ű����ݷ�װ��.
//! ��װ�˲���������������
//! \todo ��ATOM_Variant��ͳһ
//! \author ������
class ATOM_KERNEL_API ATOM_ScriptVar
{
public:
	//! ���캯��
	ATOM_ScriptVar (void);

	//! ��������
	~ATOM_ScriptVar (void);

	//! �������캯��
	//! \param rhs ��������
	ATOM_ScriptVar (const ATOM_ScriptVar &rhs);

	//! ������ֵ����
	//! \param value ��ʼֵ
	ATOM_ScriptVar (int value);

	//! ���������鹹��
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_VECTOR<int> &value);

	//! �Ӹ���ֵ����
	//! \param value ��ʼֵ
	ATOM_ScriptVar (float value);

	//! �Ӹ������鹹��
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_VECTOR<float> &value);

	//! ���ַ���ֵ����
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_STRING &value);

	//! ���ַ������鹹��
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_STRING> &value);

	//! �Ӷ�ά����ֵ����
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_Vector2f &value);

	//! �Ӷ�ά�������鹹��
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Vector2f> &value);

	//! ����ά����ֵ����
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_Vector3f &value);

	//! ����ά�������鹹��
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Vector3f> &value);

	//! ����ά��������
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_Vector4f &value);

	//! ����ά�������鹹��
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Vector4f> &value);

	//! ��3��3������
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_Matrix3x3f &value);

	//! ��3��3�������鹹��
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Matrix3x3f> &value);

	//! ��4��4������
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_Matrix4x4f &value);

	//! ��4��4�������鹹��
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Matrix4x4f> &value);

	//! �Ӳ��������
	//! \param value ��ʼֵ
	ATOM_ScriptVar (ATOM_AUTOREF(ATOM_Object) value);

	//! �Ӳ���������鹹��
	//! \param value ��ʼֵ
	ATOM_ScriptVar (const ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > &value);

public:
	//! ��ֵ����������
	//! \param rhs ��ֵ����
	ATOM_ScriptVar & operator = (const ATOM_ScriptVar &rhs);

	//! ��ȡ������ֵ
	//! \return ����ֵ
	const int & asInteger (void) const;

	//! ��ȡ����������ֵ
	//! \return ����ֵ
	const ATOM_VECTOR<int> & asIntegerArray (void) const;

	//! ��ȡ������ֵ
	//! \return ����ֵ
	const float & asFloat(void) const;

	//! ��ȡ����������ֵ
	//! \return ����ֵ
	const ATOM_VECTOR<float> & asFloatArray (void) const;

	//! ��ȡ�ַ�����ֵ
	//! \return �ַ���ֵ
	const ATOM_STRING & asString (void) const;

	//! ��ȡ�ַ���������ֵ
	//! \return ����ֵ
	const ATOM_VECTOR<ATOM_STRING> & asStringArray (void) const;

	//! ��ȡ��ά������ֵ
	//! \return ��ά����ֵ
	const ATOM_Vector2f & asVector2 (void) const;

	//! ��ȡ��ά����������ֵ
	//! \return ����ֵ
	const ATOM_VECTOR<ATOM_Vector2f> & asVector2Array (void) const;

	//! ��ȡ��ά������ֵ
	//! \return ��ά����ֵ
	const ATOM_Vector3f & asVector3 (void) const;

	//! ��ȡ��ά����������ֵ
	//! \return ����ֵ
	const ATOM_VECTOR<ATOM_Vector3f> & asVector3Array (void) const;

	//! ��ȡ��ά������ֵ
	//! \return ��ά����ֵ
	const ATOM_Vector4f & asVector4 (void) const;

	//! ��ȡ��ά����������ֵ
	//! \return ����ֵ
	const ATOM_VECTOR<ATOM_Vector4f> & asVector4Array (void) const;

	//! ��ȡ3��3������ֵ
	//! \return 3��3����ֵ
	const ATOM_Matrix3x3f & asMatrix33 (void) const;

	//! ��ȡ3��3����������ֵ
	//! \return ����ֵ
	const ATOM_VECTOR<ATOM_Matrix3x3f> & asMatrix33Array(void) const;

	//! ��ȡ4��4������ֵ
	//! \return 4��4����ֵ
	const ATOM_Matrix4x4f & asMatrix44 (void) const;

	//! ��ȡ4��4����������ֵ
	//! \return ����ֵ
	const ATOM_VECTOR<ATOM_Matrix4x4f> & asMatrix44Array (void) const;

	//! ��ȡ���������ֵ
	//! \return �������ֵ
	const ATOM_AUTOREF(ATOM_Object) & asObject (void) const;

	//! ��ȡ�������������ֵ
	//! \return ����ֵ
	const ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > & asObjectArray (void) const;

	//! ��ȡ������ֵ
	//! \return ����ֵ
	int & asInteger (void);

	//! ��ȡ����������ֵ
	//! \return ����ֵ
	ATOM_VECTOR<int> & asIntegerArray (void);

	//! ��ȡ������ֵ
	//! \return ����ֵ
	float & asFloat(void);

	//! ��ȡ����������ֵ
	//! \return ����ֵ
	ATOM_VECTOR<float> & asFloatArray (void);

	//! ��ȡ�ַ�����ֵ
	//! \return �ַ���ֵ
	ATOM_STRING& asString (void);

	//! ��ȡ�ַ���������ֵ
	//! \return ����ֵ
	ATOM_VECTOR<ATOM_STRING> & asStringArray (void);

	//! ��ȡ��ά������ֵ
	//! \return ��ά����ֵ
	ATOM_Vector2f & asVector2 (void);

	//! ��ȡ��ά����������ֵ
	//! \return ����ֵ
	ATOM_VECTOR<ATOM_Vector2f> & asVector2Array (void);

	//! ��ȡ��ά������ֵ
	//! \return ��ά����ֵ
	ATOM_Vector3f & asVector3 (void);

	//! ��ȡ��ά����������ֵ
	//! \return ����ֵ
	ATOM_VECTOR<ATOM_Vector3f> & asVector3Array (void);

	//! ��ȡ��ά������ֵ
	//! \return ��ά����ֵ
	ATOM_Vector4f & asVector4 (void);

	//! ��ȡ��ά����������ֵ
	//! \return ����ֵ
	ATOM_VECTOR<ATOM_Vector4f> & asVector4Array (void);

	//! ��ȡ3��3������ֵ
	//! \return 3��3����ֵ
	ATOM_Matrix3x3f & asMatrix33 (void);

	//! ��ȡ3��3����������ֵ
	//! \return ����ֵ
	ATOM_VECTOR<ATOM_Matrix3x3f> & asMatrix33Array(void);

	//! ��ȡ4��4������ֵ
	//! \return 4��4����ֵ
	ATOM_Matrix4x4f & asMatrix44 (void);

	//! ��ȡ4��4����������ֵ
	//! \return ����ֵ
	ATOM_VECTOR<ATOM_Matrix4x4f> & asMatrix44Array (void);

	//! ��ȡ���������ֵ
	//! \return �������ֵ
	ATOM_AUTOREF(ATOM_Object) & asObject (void);

	//! ��ȡ�������������ֵ
	//! \return ����ֵ
	ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > & asObjectArray (void);

public:
	//! ��ȡ����ֵ.
	//! �������������Ͳ��������򸡵������׳��쳣
	//! \return ����ֵ
	int getInteger (void) const;

	//! ��ȡ����ֵ.
	//! �������������Ͳ��������򸡵������׳��쳣
	//! \return ����ֵ
	float getFloat (void) const;

	//! ��ȡ�ַ���ֵ.
	//! �������������Ͳ����ַ������׳��쳣
	//! \return �ַ���ֵ
	ATOM_STRING getString (void) const;

	//! ��ȡ��ά����ֵ.
	//! �������������Ͳ��Ƕ�ά�������׳��쳣
	//! \return ��ά����ֵ
	ATOM_Vector2f getVector2 (void) const;

	//! ��ȡ��ά����ֵ.
	//! �������������Ͳ�����ά�������׳��쳣
	//! \return ��ά����ֵ
	ATOM_Vector3f getVector3 (void) const;

	//! ��ȡ��ά����ֵ.
	//! �������������Ͳ�����ά�������׳��쳣
	//! \return ��ά����ֵ
	ATOM_Vector4f getVector4 (void) const;

	//! ��ȡ3��3����ֵ.
	//! �������������Ͳ���3��3������׳��쳣
	//! \return 3��3����ֵ
	ATOM_Matrix3x3f getMatrix33 (void) const;

	//! ��ȡ4��4����ֵ.
	//! �������������Ͳ���4��4������׳��쳣
	//! \return 4��4����ֵ
	ATOM_Matrix4x4f getMatrix44 (void) const;

	//! ��ȡ�������ֵ.
	//! �������������Ͳ��ǲ��������׳��쳣
	//! \return �������ֵ
	ATOM_AUTOREF(ATOM_Object) getObject (void) const;

	//! ��ȡ��������.
	//! �������������Ͳ�������������׳��쳣
	//! \return ��������
	ATOM_VECTOR<int> getIntegerArray (void) const;

	//! ��ȡ��������.
	//! �������������Ͳ��Ǹ���������׳��쳣
	//! \return ��������
	ATOM_VECTOR<float> getFloatArray (void) const;

	//! ��ȡ�ַ�������.
	//! �������������Ͳ����ַ���������׳��쳣
	//! \return �ַ�������
	ATOM_VECTOR<ATOM_STRING> getStringArray (void) const;

	//! ��ȡ��ά��������.
	//! �������������Ͳ��Ƕ�ά����������׳��쳣
	//! \return ��ά��������
	ATOM_VECTOR<ATOM_Vector2f> getVector2Array (void) const;

	//! ��ȡ��ά��������.
	//! �������������Ͳ�����ά����������׳��쳣
	//! \return ��ά��������
	ATOM_VECTOR<ATOM_Vector3f> getVector3Array (void) const;

	//! ��ȡ��ά��������.
	//! �������������Ͳ�����ά����������׳��쳣
	//! \return ��ά��������
	ATOM_VECTOR<ATOM_Vector4f> getVector4Array (void) const;

	//! ��ȡ3��3��������.
	//! �������������Ͳ���3��3����������׳��쳣
	//! \return 3��3��������
	ATOM_VECTOR<ATOM_Matrix3x3f> getMatrix33Array (void) const;

	//! ��ȡ4��4��������.
	//! �������������Ͳ���4��4����������׳��쳣
	//! \return 4��4��������
	ATOM_VECTOR<ATOM_Matrix4x4f> getMatrix44Array (void) const;

	//! ��ȡ�����������.
	//! �������������Ͳ��ǲ������������׳��쳣
	//! \return �����������
	ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > getObjectArray (void) const;

public:
	//! ��Ȳ���
	//! \param rhs ���Զ���
	bool operator == (const ATOM_ScriptVar &rhs) const;

	//! ���Ȳ���
	//! \param rhs ���Զ���
	bool operator != (const ATOM_ScriptVar &rhs) const;

private:
	void deleteStorage (void);
	void createStorage (void);
	void reallocStorage (int type);

public:
	//! ��ȡ�ڲ���������
	//! \return ��������
	int getType (void) const;

	//! �����ڲ���������
	//! \param type ��������
	void setType (int type);

	//! �����ڲ�����
	//! \param rhs ��������
	void swap (ATOM_ScriptVar &rhs);

private:
	int _M_type;
	union 
	{
		ATOM_VECTOR<int> *_M_integer;
		ATOM_VECTOR<float> *_M_float;
		ATOM_VECTOR<ATOM_STRING> *_M_string;
		ATOM_VECTOR<ATOM_Vector2f> *_M_vec2;
		ATOM_VECTOR<ATOM_Vector3f> *_M_vec3;
		ATOM_VECTOR<ATOM_Vector4f> *_M_vec4;
		ATOM_VECTOR<ATOM_Matrix3x3f> *_M_mat33;
		ATOM_VECTOR<ATOM_Matrix4x4f> *_M_mat44;
		ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > *_M_object;
	};
};

#endif // __ATOM_KERNEL_VAR_H
/*! @} */
