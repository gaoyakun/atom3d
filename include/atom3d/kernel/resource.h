/**	\file kernel/resource.h
 *	��Դ�������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_RESOURCE_H
#define __ATOM_KERNEL_RESOURCE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <string>
#include "functionbinding.h"

class ATOM_File;

//! \class ATOM_Resource
//! ��Դ�����
//! ���дӴ��������Ķ����֧�ִ�ĳ���ļ����빦��
//! \todo �Ƴ��˽ӿڣ�ʹ�ø���ѧ�ķ���������Դ
//! \author ������
class ATOM_KERNEL_API ATOM_Resource : public ATOM_Object
{
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Resource)

protected:
	ATOM_Resource (void);
	virtual ~ATOM_Resource (void);

protected:
	//! ���ļ��ж�ȡ��Դ
	//! \param file �����ļ�
	//! \param userdata �Զ������ݣ�����Զ���������ATOM_LoadResource�����д���
	//! \return true �ɹ� false ���ɹ�
	virtual bool readFromFile(ATOM_File* file, void *userdata = 0) = 0;

public:
	//! ���ļ��ж�ȡ��Դ
	//! \param file �����ļ�
	//! \param userdata �Զ������ݣ�����Զ���������ATOM_LoadResource�����д���
	//! \return true �ɹ� false ���ɹ�
	//! \sa ATOM_LoadResource
	bool loadFromFile (ATOM_File *file, void *userdata = 0);
};

#endif // __ATOM_KERNEL_RESOURCE_H
/*! @} */
