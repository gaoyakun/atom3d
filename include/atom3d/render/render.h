/**	\file render.h
 *	ATOM_Renderer类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GLRENDER_RENDERER_H
#define __ATOM_GLRENDER_RENDERER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "renderwindow.h"

class ATOM_RenderDevice;

//! \class ATOM_Renderer
//! Renderer管理器 
//! \todo 废弃这个类
//! \author 高雅昆
class ATOM_Renderer: public ATOM_Object
{
	ATOM_CLASS(render, ATOM_Renderer, ATOM_Renderer)

public:
	//! 构造函数
	ATOM_Renderer (void);

	//! 析构函数
	virtual ~ATOM_Renderer (void);

public:
	//! 初始化
	virtual bool initialize (ATOM_DisplayMode *mode, int w, int h, ATOM_RenderWindow::ShowMode showMode, bool naked, bool resizable, int multisample, const char *title, void *windowid = 0, void *parentid = 0, void *iconid = 0, bool contentBackup = false, void *instance = 0);

	//! 释放
	virtual void finalize (void);

	//! 创建渲染窗口
	virtual ATOM_AUTOREF(ATOM_RenderWindow) createRenderWindow(const char *title, int w, int h, bool naked, bool resizable, int multisample, ATOM_RenderWindow::ShowMode showMode, void* windowid = 0, void *parentid = 0, bool contentBackup = false, void *instance = 0);

	//! 获取渲染设备
	//! \return 渲染设备
	virtual ATOM_RenderDevice *getRenderDevice (void) const;

	//! 获取渲染窗口 
	//! \return 渲染窗口 
	virtual ATOM_RenderWindow *getRenderWindow (void) const;

private:
	ATOM_RenderDevice *device;
};

#endif // __ATOM_GLRENDER_RENDERER_H
/*! @} */
