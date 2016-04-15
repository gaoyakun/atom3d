/**	\file animatedtexturechannel.h
 *	设备相关资源基类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GLRENDER_GFXRESOURCE_H
#define __ATOM_GLRENDER_GFXRESOURCE_H

#include "../ATOM_kernel.h"
#include "basedefs.h"

class ATOM_GfxResource;
class ATOM_RenderDevice;

//! \class ATOM_GfxResource
//! 渲染设备相关资源基类
//! \author 高雅昆
class ATOM_GfxResource: public ATOM_Object
{
	friend class ATOM_RenderDevice;
	friend class ATOM_D3D9Device;

public:
	//! 资源的载入状态
	enum LOADINGSTATE
	{
		//! 未载入
		LS_NOTLOADED,
		//! 已载入
		LS_LOADED,
		//! 载入失败
		LS_LOADFAILED,
		//! 正在载入
		LS_LOADING
	};

protected:
	//! 构造函数
	ATOM_GfxResource (void);

	//! 析构函数
	virtual ~ATOM_GfxResource (void);

public:
	//! 删除资源.
	//! 当设备丢失此函数会被调用
	//! \param needRestore true表示需要重建此资源 false表示不需要重建
	virtual void invalidate (bool needRestore);

	//! 重新创建此资源.
	//! 当设备复位时此函数会被调用
	virtual void restore (void);

	//! 获得和此资源相关的渲染设备.
	//! \return 渲染设备，如果此资源未初始化或者已删除则返回NULL
	virtual ATOM_RenderDevice *getRenderDevice (void) const;

	//! 设置关联的渲染设备.
	//! 资源初始化后需要调用此函数关联设备
	//! \param device 渲染设备
	virtual void setRenderDevice (ATOM_RenderDevice *device);

	//! 创建资源
	//! \return true 成功 false 失败
	virtual bool realize (void);

	//! 设置载入状态
	//! \param val 状态
	virtual void setLoadingState (LOADINGSTATE val);

	//! 获取载入状态
	//! \return 载入状态
	virtual LOADINGSTATE getLoadingState (void) const;

public:
	bool needRestore (void) const;
	void setNeedReset (bool b);
	bool needReset (void) const;
	ATOM_GfxResource *getNext (void) const;
	ATOM_GfxResource *getPrev (void) const;

protected:
	virtual void invalidateImpl (bool needRestore);
	virtual void restoreImpl (void);

protected:
	ATOM_RenderDevice *_M_device;
	ATOM_GfxResource *_M_prev;
	ATOM_GfxResource *_M_next;
	unsigned _M_last_used_tick;
	bool _M_invalid;
	bool _M_need_reset;
	LOADINGSTATE _M_loading_state;
};

#endif // __ATOM_GLRENDER_GFXRESOURCE_H
/*! @} */
