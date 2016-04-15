/**	\file asset.h
 *	AS_Asset�������.
 *
 *	\author ������
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3D_STUDIO_ASSET_H
#define __ATOM3D_STUDIO_ASSET_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../atom3d/ATOM_engine.h"

//! \class AS_Asset
//! �ڵ���Դ���͵Ļ���.
//! ����ͨ����չ����������µ���Դ���ͣ���Щ����ͳһ��Asset��������в���
//! \author ������
//! \ingroup studio
class AS_Asset
{
public:
	//! ��������
	virtual ~AS_Asset (void) {}

	//! ͨ���ļ�������
	//! \param filename �ļ���
	//! \return true�ɹ� falseʧ��
	virtual bool loadFromFile (const char *filename) = 0;

	//! ɾ������.
	//! �ú�����ʵ��ȡ������չ�������ι������Դ
	virtual void deleteMe (void) = 0;

	//! ��ȡ�����ڵ�ָ��
	//! \return �ڵ�ָ��
	virtual ATOM_Node *getNode (void) = 0;

	//! ����Դ�ڳ����༭�����Ƿ�������ѡ
	//! \return true���� false������
	virtual bool isPickable (void) const = 0;

	//! ����Դ�Ƿ�֧�ֳ���ˢ��
	//! \return true֧�� false��֧��
	virtual bool supportBrushing (void) const = 0;
};

#endif // __ATOM3D_STUDIO_ASSET_H

/*! @} */
