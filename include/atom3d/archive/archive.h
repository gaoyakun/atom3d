/**	\file archive.h
 *	ZIP��������
 *
 *	\author ������
 *	\ingroup archive
 */
#ifndef __ATOM3D_ARCHIVE_ARCHIVE_H
#define __ATOM3D_ARCHIVE_ARCHIVE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "zip_wrapper.h"

//! \class ATOM_Archive
//! ��װ�˴�������õ�ZIP����
//! \author ������
class ATOM_Archive
{
public:
	//! ���캯��
	ATOM_Archive (void);

	//! ���캯��
	//! \param filename ZIP����Windows�ļ���
	//! \param passwd ZIP��������
	ATOM_Archive (const char *filename, const char *passwd = 0);

	//! ��������
	virtual ~ATOM_Archive (void);

public:
	//! ZIP�ļ���ʱ����ṹ
	struct FileTime
	{
		unsigned year;
		unsigned month;
		unsigned day;
		unsigned hour;
		unsigned minute;
		unsigned second;
	};

	//! ZIP�ļ���ϸ��Ϣ
	struct FileInfo
	{
		//! ���ڵ��ļ���
		const char *fileName;
		//! �ļ�ע��
		const char *fileComment;
		//! �ļ�ѹ����Ĵ�С�����ֽ�Ϊ��λ
		unsigned compressedSize;
		//! �ļ�ѹ��ǰ�Ĵ�С�����ֽ�Ϊ��λ
		unsigned uncompressedSize;
		//! �ļ���CRCУ����
		unsigned long crc32;
		//! �ļ���ʱ���
		FileTime time;
	};

	//! \class FileIterator
	//! �ļ���������ͨ���Ӵ����ͼ̳�����������ZIP���ڵ�ÿ���ļ�
	//! \sa Archive::iterate
	class FileIterator
	{
	public:
		virtual bool begin (void) { return true; }
		virtual void end (void) { return; }
		virtual bool rawIterate (void) { return false; }
		virtual bool process (ATOM_UnZip *unzip, const FileInfo *fileInfo) = 0;
	};

private:
	ATOM_Archive (const ATOM_Archive &);
	ATOM_Archive & operator = (const ATOM_Archive&);

public:
	//! ��һ��ZIP��
	//! \param filename ZIP����WINDOWS�ļ���
	//! \param passwd ZIP��������
	virtual bool open (const char *filename, const char *passwd = 0);

	//! �رյ�ǰZIP��
	virtual void close (void);

	//! ��ѯZIP���Ƿ�ɹ�����
	//! \return true ZIP������ false ZIP��δ�ɹ���
	virtual bool opened (void) const;

	//! ��ѯ�����Ƿ����ָ�����ļ�
	//! \param filename �����ļ���
	//! \return true ���ڰ������ļ� false �����޴��ļ�
	virtual bool fileExists (const char *filename) const;

	//! ���һ�������ļ�����ǰ����
	//! \param filenameDst �����ļ����������Ŀ¼��Ŀ¼�Զ�������
	//! \param filenameSrc �����ļ���
	//! \return true �ļ���ӳɹ� false �ļ����ʧ��
	virtual bool addFile (const char *filenameDst, const char *filenameSrc);

	//! ����ڴ����ݵ���ǰ����
	//! \param filenameDst �����ļ����������Ŀ¼��Ŀ¼�Զ�������
	//! \param mem �ڴ�������ʼ��ַ
	//! \param size �ڴ����ݵ��ֽڳ���
	//! \return true �ļ���ӳɹ� false �ļ����ʧ��
	virtual bool addFile (const char *filenameDst, const void *mem, unsigned size);

	//! ����ļ����Լ��ļ����ڵ������ļ�������
	//! \param dirDst ���ڵ��ļ�����
	//! \param dirSrc ���̵��ļ���������Ӧ���ڵ��ļ�����
	//! \return true �ļ�����ӳɹ� false �ļ������ʧ��
	virtual bool addDirectory (const char *dirDst, const char *dirSrc);

	//! �Ӱ���ɾ���ļ�
	//! \param filenameDst ����Ҫɾ�����ļ�·����
	//! \return true ɾ���ɹ� false ɾ��ʧ��
	//! \note �˺����ڲ��ᴴ���µ�ѹ������ִ���ļ��ĸ��ƣ������ܴ������Ҫɾ������ļ�������ʹ��Archive::removeFiles����
	//! \sa removeFiles
	virtual bool removeFile (const char *filenameDst);

	//! �Ӱ���ɾ������ļ�
	//! \param filenames �ַ���ָ�����飬ÿ��Ԫ�ش���һ�����ڵ��ļ�·��
	//! \param size Ҫɾ�����ļ�
	//! \return true �����ļ�ɾ���ɹ� false ���������ļ�ɾ���ɹ�
	//! \sa removeFile
	virtual bool removeFiles (const char * const *filenames, unsigned size);

	//! �Ӱ��ڽ�ѹһ���ļ�������
	//! \param filenameDst �����ļ���������������ΪNULL�����ļ�ʹ�ð��ڵ��ļ�����ѹ����ǰĿ¼
	//! \param filenameSrc ���ڱ���ѹ���ļ�·��
	//! \return true ��ѹ�ɹ� false ��ѹʧ��
	virtual bool extractFile (const char *filenameDst, const char *filenameSrc);

	//! ��ѹһ����Ŀ¼������
	//! \param dirDst ����Ŀ¼���ƣ���������ڴ�Ŀ¼���Զ�����
	//! \param dirSrc ���ڱ���ѹ��Ŀ¼������Ӧ���̵�Ŀ¼��
	//! \return true �����ļ���ѹ�ɹ� false ���������ļ���ѹ�ɹ�
	virtual bool extractDirectory (const char *dirDst, const char *dirSrc);

	//! �������ʰ��ڵ������ļ�
	//! \param iterator ������
	//! \return true �˳����� false ��������
	virtual bool iterate (FileIterator *iterator) const;

private:
	char *_filename;
	char *_passwd;
};

#endif // __ATOM3D_ARCHIVE_ARCHIVE_H
