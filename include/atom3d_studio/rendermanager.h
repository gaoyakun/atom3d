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
	//! ��������
	virtual ~AS_RenderManager (void) {}

	//! �Զ�����Ⱦ����
	//! \param editor �༭�����ܽӿ�ָ��
	//! \param scene Ҫ��Ⱦ�ĳ���
	virtual void renderScene (AS_Editor *editor, ATOM_Scene *scene) = 0;
};

#endif // __ATOM3D_STUDIO_RENDERMANAGER_H
