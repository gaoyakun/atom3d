#ifndef __ATOM3D_STUDIO_RENDERMANAGER_H
#define __ATOM3D_STUDIO_RENDERMANAGER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../atom3d/ATOM_engine.h"

class AS_Editor;

class AS_RenderManager
{
public:
	//! 析构函数
	virtual ~AS_RenderManager (void) {}

	//! 自定义渲染过程
	//! \param editor 编辑器功能接口指针
	//! \param scene 要渲染的场景
	virtual void renderScene (AS_Editor *editor, ATOM_Scene *scene) = 0;
};

#endif // __ATOM3D_STUDIO_RENDERMANAGER_H
