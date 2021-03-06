/**	\file animatedtexturechannel.h
 *	索引缓冲区类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GLRENDER_IINDEXARRAY_H
#define __ATOM_GLRENDER_IINDEXARRAY_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#include "gfxtypes.h"
#include "gfxresource.h"

//! \class ATOM_IndexArray
//! 索引缓冲区类
//! \author 高雅昆
class ATOM_IndexArray: public ATOM_GfxResource
{
protected:
    ATOM_IndexArray(void);

public:
	//! 获取索引的数量
	//! \return 索引的数量
	virtual unsigned getNumIndices (void) const;

	//! 锁定索引缓冲区以更新数据
	//! \param mode 锁定旗标
	//! \param offset 锁定位置相对于起始位置的字节偏移量
	//! \param size 锁定的大小字节数
	//! \param overwrite false表示锁定范围不会和本缓冲区内正在用于渲染的索引范围重叠
	//! \return 锁定的索引缓冲区指针，失败返回NULL
    virtual void * lock(ATOM_LockMode mode, unsigned offset, unsigned size, bool overwrite) = 0;

	//! 完成数据更新索引缓冲区解锁
    virtual void unlock(void) = 0;

	//! 查询缓冲区内的数据是否因为设备丢失而失效
	//! \return true 已失效 false 未失效
	virtual bool isContentLost (void) const;

	//! 设置数据的失效标记
	//! \param b true表示失效 false表示未失效
	virtual void setContentLost (bool b);

protected:
    virtual void invalidateImpl (bool needRestore) = 0;

protected:
	unsigned _M_num_indices;
	int _M_usage;
	bool _M_32bit;
	bool _M_content_lost;
	void *_preservedContent;
};

#endif // __ATOM_GLRENDER_IINDEXARRAY_H
/*! @} */
