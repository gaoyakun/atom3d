/**	\file archive.h
 *	ZIP档管理类
 *
 *	\author 高雅昆
 *	\ingroup archive
 */
#ifndef __ATOM3D_ARCHIVE_ARCHIVE_H
#define __ATOM3D_ARCHIVE_ARCHIVE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "zip_wrapper.h"

//! \class ATOM_Archive
//! 封装了大多数常用的ZIP操作
//! \author 高雅昆
class ATOM_Archive
{
public:
	//! 构造函数
	ATOM_Archive (void);

	//! 构造函数
	//! \param filename ZIP包的Windows文件名
	//! \param passwd ZIP包的密码
	ATOM_Archive (const char *filename, const char *passwd = 0);

	//! 析构函数
	virtual ~ATOM_Archive (void);

public:
	//! ZIP文件的时间戳结构
	struct FileTime
	{
		unsigned year;
		unsigned month;
		unsigned day;
		unsigned hour;
		unsigned minute;
		unsigned second;
	};

	//! ZIP文件详细信息
	struct FileInfo
	{
		//! 包内的文件名
		const char *fileName;
		//! 文件注释
		const char *fileComment;
		//! 文件压缩后的大小，以字节为单位
		unsigned compressedSize;
		//! 文件压缩前的大小，以字节为单位
		unsigned uncompressedSize;
		//! 文件的CRC校验码
		unsigned long crc32;
		//! 文件的时间戳
		FileTime time;
	};

	//! \class FileIterator
	//! 文件迭代器，通过从此类型继承来迭代访问ZIP包内的每个文件
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
	//! 打开一个ZIP包
	//! \param filename ZIP包的WINDOWS文件名
	//! \param passwd ZIP包的密码
	virtual bool open (const char *filename, const char *passwd = 0);

	//! 关闭当前ZIP包
	virtual void close (void);

	//! 查询ZIP包是否成功被打开
	//! \return true ZIP包被打开 false ZIP包未成功打开
	virtual bool opened (void) const;

	//! 查询包内是否包含指定的文件
	//! \param filename 包内文件名
	//! \return true 包内包含此文件 false 包内无此文件
	virtual bool fileExists (const char *filename) const;

	//! 添加一个磁盘文件到当前包内
	//! \param filenameDst 包内文件名，如果含目录则目录自动被创建
	//! \param filenameSrc 磁盘文件名
	//! \return true 文件添加成功 false 文件添加失败
	virtual bool addFile (const char *filenameDst, const char *filenameSrc);

	//! 添加内存数据到当前包内
	//! \param filenameDst 包内文件名，如果含目录则目录自动被创建
	//! \param mem 内存数据起始地址
	//! \param size 内存数据的字节长度
	//! \return true 文件添加成功 false 文件添加失败
	virtual bool addFile (const char *filenameDst, const void *mem, unsigned size);

	//! 添加文件夹以及文件夹内的所有文件到包内
	//! \param dirDst 包内的文件夹名
	//! \param dirSrc 磁盘的文件夹名，对应包内的文件夹名
	//! \return true 文件夹添加成功 false 文件夹添加失败
	virtual bool addDirectory (const char *dirDst, const char *dirSrc);

	//! 从包内删除文件
	//! \param filenameDst 包内要删除的文件路径名
	//! \return true 删除成功 false 删除失败
	//! \note 此函数内部会创建新的压缩包并执行文件的复制，开销很大，如果需要删除多个文件，考虑使用Archive::removeFiles函数
	//! \sa removeFiles
	virtual bool removeFile (const char *filenameDst);

	//! 从包内删除多个文件
	//! \param filenames 字符串指针数组，每个元素代表一个包内的文件路径
	//! \param size 要删除的文件
	//! \return true 所有文件删除成功 false 并非所有文件删除成功
	//! \sa removeFile
	virtual bool removeFiles (const char * const *filenames, unsigned size);

	//! 从包内解压一个文件到磁盘
	//! \param filenameDst 磁盘文件名，如果这个参数为NULL，则将文件使用包内的文件名解压到当前目录
	//! \param filenameSrc 包内被解压的文件路径
	//! \return true 解压成功 false 解压失败
	virtual bool extractFile (const char *filenameDst, const char *filenameSrc);

	//! 解压一整个目录到磁盘
	//! \param dirDst 磁盘目录名称，如果不存在此目录则被自动创建
	//! \param dirSrc 包内被解压的目录名，对应磁盘的目录名
	//! \return true 所有文件解压成功 false 并非所有文件解压成功
	virtual bool extractDirectory (const char *dirDst, const char *dirSrc);

	//! 迭代访问包内的所有文件
	//! \param iterator 访问器
	//! \return true 退出迭代 false 继续迭代
	virtual bool iterate (FileIterator *iterator) const;

private:
	char *_filename;
	char *_passwd;
};

#endif // __ATOM3D_ARCHIVE_ARCHIVE_H
