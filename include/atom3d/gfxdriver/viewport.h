#ifndef __ATOM3D_GFXDRIVER_VIEWPORT_H
#define __ATOM3D_GFXDRIVER_VIEWPORT_H

#include "basedefs.h"

class ATOM_Viewport
{
public:
	//! 设置视口范围
	//! \param region 范围
	//! \return true成功 false失败
	virtual bool setRegion (const ATOM_Rect2Di &region) = 0;

	//! 获取视口范围
	//! \return 视口范围
	virtual const ATOM_Rect2Di &getRegion (void) const = 0;

	//! 获取清除色
	//! \return 清除色
	virtual ATOM_ColorARGB getClearColor(void) const = 0;

	//! 设置清除色
	//! \param color 清除色
	virtual void setClearColor(ATOM_ColorARGB color) = 0;

	//! 获取深度缓冲区清除值
	//! \return 深度清除值
	virtual float getClearDepth (void) const = 0;

	//! 设置深度缓冲区清除值
	//! \param depth 清除值，范围0到1
	virtual void setClearDepth(float depth) = 0;

	//! 获取模板缓冲区清除值
	//! \return 模板缓冲区清除值
	virtual int getClearStencil (void) const = 0;

	//! 设置渲染窗口的模板缓冲区清除值
	//! \param stencil 清除值，范围0到255
	virtual void setClearStencil(int stencil) = 0;

	//! 清除后备缓冲区
	//! \param clearColor true清除颜色 false不清除颜色
	//! \param clearDepth true清除深度 false不清除深度
	//! \param clearDepth true清除模板 false不清除模板
	//! \return true成功 false失败
	virtual bool clear (bool clearColor, bool clearDepth, bool clearStencil) = 0;

	//! 设置剪裁区域
	//! \param scissorRect 剪裁区域
	virtual void setScissorRect (const ATOM_Rect2Di &scissorRect);

	//! 获取剪裁范围
	//! \return 剪裁区域
	virtual ATOM_Rect2Di getScissorRect (void) const;

	//! 设置是否允许视口剪裁
	//! \param enable true表示允许，false表示不允许
	virtual void enableScissorTest (bool enable) = 0;

	//! 查询视口剪裁是否打开
	//! \return true 打开 false 未打开
	//! \sa enableScissorTest setScissorRect getScissorRect
	virtual bool isScissorTestEnabled (void) const = 0;

	//! 读取颜色缓冲区内容
	//! \param rc 读取范围
	//! \param format 读取的像素格式
	//! \param pixels 读取缓冲区，必须具有足够的大小
	//! \return true 成功 false 失败
	virtual bool readColorBuffer(const ATOM_Rect2Di &rc, ATOM_PixelFormat format, void *pixels) = 0;

	//! 读取颜色缓冲区内容并保存到图像文件中
	//! \param path 虚拟文件路径
	//! \return true 成功 false 失败
	virtual bool grabScreenToFile (const char *path) = 0;

	//! 读取颜色缓冲区内容并保存到ATOM_Image对象
	//! \param image ATOM_Image类指针
	//! \return true 成功 false 失败
	virtual bool grabScreenToImage (ATOM_Image *image) = 0;

	//! 开始渲染到本视口
	//! \return true成功 false失败
	//! \sa endFrame
	virtual bool beginRender (void) = 0;

	//! 结束本视口的渲染
	//! \sa beginRender
	virtual void endRender (void) = 0;
};

#endif // __ATOM3D_GFXDRIVER_VIEWPORT_H
