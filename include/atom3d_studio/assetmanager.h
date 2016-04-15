/**	\file assetmanager.h
 *	AS_AssetManager类的声明.
 *
 *	\author 高雅昆
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
//! 节点资源管理器类型的基类.
//! 管理器用来创建特定的资源，可以在扩展组件中实现新的资源管理器并注册。
//! \author 高雅昆
//! \ingroup studio
class AS_AssetManager
{
public:
	//! 析构函数
	virtual ~AS_AssetManager (void) {}

	//! 创建一个资源实例
	//! \return 资源实例指针
	virtual AS_Asset *createAsset (void) = 0;

	//! 支持多少种文件类型
	//! \return 文件类型个数
	virtual unsigned getNumFileExtensions (void) const = 0;

	//! 如果通过文件来创建此资源，返回该类型文件的扩展名(不带点)
	//! \param index 某种文件类型的索引
	//! \return 文件扩展名
	virtual const char * getFileExtension (unsigned index) const = 0;

	//! 如果通过文件来创建此资源，返回该类型文件的说明信息
	//! \param index 某种文件类型的索引
	//! \return 文件说明信息
	virtual const char * getFileDesc (unsigned index) const = 0;

	//! 设置编辑器指针
	//! \param editor 编辑器指针
	virtual void setEditor (AS_Editor *editor) = 0;
};

#endif // __ATOM3D_STUDIO_ASSETMANAGER_H

/*! @} */
