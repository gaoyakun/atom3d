/**	\file render.h
 *	ATOM_Renderer�������.
 *
 *	\author ������
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
//! Renderer������ 
//! \todo ���������
//! \author ������
class ATOM_Renderer: public ATOM_Object
{
	ATOM_CLASS(render, ATOM_Renderer, ATOM_Renderer)

public:
	//! ���캯��
	ATOM_Renderer (void);

	//! ��������
	virtual ~ATOM_Renderer (void);

public:
	//! ��ʼ��
	virtual bool initialize (ATOM_DisplayMode *mode, int w, int h, ATOM_RenderWindow::ShowMode showMode, bool naked, bool resizable, int multisample, const char *title, void *windowid = 0, void *parentid = 0, void *iconid = 0, bool contentBackup = false, void *instance = 0);

	//! �ͷ�
	virtual void finalize (void);

	//! ������Ⱦ����
	virtual ATOM_AUTOREF(ATOM_RenderWindow) createRenderWindow(const char *title, int w, int h, bool naked, bool resizable, int multisample, ATOM_RenderWindow::ShowMode showMode, void* windowid = 0, void *parentid = 0, bool contentBackup = false, void *instance = 0);

	//! ��ȡ��Ⱦ�豸
	//! \return ��Ⱦ�豸
	virtual ATOM_RenderDevice *getRenderDevice (void) const;

	//! ��ȡ��Ⱦ���� 
	//! \return ��Ⱦ���� 
	virtual ATOM_RenderWindow *getRenderWindow (void) const;

private:
	ATOM_RenderDevice *device;
};

#endif // __ATOM_GLRENDER_RENDERER_H
/*! @} */
