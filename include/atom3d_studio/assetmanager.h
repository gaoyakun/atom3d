/**	\file assetmanager.h
 *	AS_AssetManager�������.
 *
 *	\author ������
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3D_STUDIO_ASSETMANAGER_H
#define __ATOM3D_STUDIO_ASSETMANAGER_H

#if _MSC_VER > 1000
# pragma once
#endif

class AS_Asset;
class AS_Editor;

//! \class AS_AssetManager
//! �ڵ���Դ���������͵Ļ���.
//! ���������������ض�����Դ����������չ�����ʵ���µ���Դ��������ע�ᡣ
//! \author ������
//! \ingroup studio
class AS_AssetManager
{
public:
	//! ��������
	virtual ~AS_AssetManager (void) {}

	//! ����һ����Դʵ��
	//! \return ��Դʵ��ָ��
	virtual AS_Asset *createAsset (void) = 0;

	//! ֧�ֶ������ļ�����
	//! \return �ļ����͸���
	virtual unsigned getNumFileExtensions (void) const = 0;

	//! ���ͨ���ļ�����������Դ�����ظ������ļ�����չ��(������)
	//! \param index ĳ���ļ����͵�����
	//! \return �ļ���չ��
	virtual const char * getFileExtension (unsigned index) const = 0;

	//! ���ͨ���ļ�����������Դ�����ظ������ļ���˵����Ϣ
	//! \param index ĳ���ļ����͵�����
	//! \return �ļ�˵����Ϣ
	virtual const char * getFileDesc (unsigned index) const = 0;

	//! ���ñ༭��ָ��
	//! \param editor �༭��ָ��
	virtual void setEditor (AS_Editor *editor) = 0;
};

#endif // __ATOM3D_STUDIO_ASSETMANAGER_H

/*! @} */
