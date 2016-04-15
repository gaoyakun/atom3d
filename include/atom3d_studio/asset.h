/**	\file asset.h
 *	AS_Asset类的声明.
 *
 *	\author 高雅昆
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
//! 节点资源类型的基类.
//! 可以通过扩展组件来创建新的资源类型，这些类型统一由Asset面板来进行操作
//! \author 高雅昆
//! \ingroup studio
class AS_Asset
{
public:
	//! 析构函数
	virtual ~AS_Asset (void) {}

	//! 通过文件来创建
	//! \param filename 文件名
	//! \return true成功 false失败
	virtual bool loadFromFile (const char *filename) = 0;

	//! 删除自身.
	//! 该函数的实现取决于扩展组件中如何管理此资源
	virtual void deleteMe (void) = 0;

	//! 获取场景节点指针
	//! \return 节点指针
	virtual ATOM_Node *getNode (void) = 0;

	//! 该资源在场景编辑器中是否允许被点选
	//! \return true允许 false不允许
	virtual bool isPickable (void) const = 0;

	//! 该资源是否支持场景刷子
	//! \return true支持 false不支持
	virtual bool supportBrushing (void) const = 0;
};

#endif // __ATOM3D_STUDIO_ASSET_H

/*! @} */
