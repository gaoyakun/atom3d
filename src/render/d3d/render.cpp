#include "StdAfx.h"
#include "render.h"
#include "d3d9device.h"
#include "d3d9window.h"

ATOM_Renderer::ATOM_Renderer (void)
{
	device = 0;
}

bool ATOM_Renderer::initialize (ATOM_DisplayMode *mode, int w, int h, ATOM_RenderWindow::ShowMode showMode, bool naked, bool resizable, int multisample, const char *title, void *windowid, void *parentid, void *iconid, bool contentBackup, void *instance)
{
	ATOM_STACK_TRACE(ATOM_Renderer::initialize);
	device = ATOM_CreateRenderDevice(mode, w, h, showMode, naked, resizable, multisample, title, windowid, parentid, iconid, contentBackup, instance);
	return device != 0;
}

void ATOM_Renderer::finalize (void)
{
	ATOM_STACK_TRACE(ATOM_Renderer::finalize);
	if (device)
	{
		ATOM_DestroyRenderDevice (device);
		device = 0;
	}
}

ATOM_Renderer::~ATOM_Renderer (void)
{
	ATOM_STACK_TRACE(ATOM_Renderer::~ATOM_Renderer);
	finalize ();
}

ATOM_AUTOREF(ATOM_RenderWindow) ATOM_Renderer::createRenderWindow(const char *title, int w, int h, bool naked, bool resizable, int multisample, ATOM_RenderWindow::ShowMode showMode, void* windowid, void *parentid, bool contentBackup, void *instance)
{
	ATOM_STACK_TRACE(ATOM_Renderer::createRenderWindow);
	if (!device)
	{
		return 0;
	}

	ATOM_HARDREF(ATOM_D3D9Window) window;
	ATOM_ASSERT(window);

	ATOM_RenderWindow::Hints hints;
	hints.accel = true;
	hints.bounds.first = w;
	hints.bounds.second = h;
	hints.resizable = resizable;
	hints.naked = naked;
	hints.bpp = 32;
	hints.samples = multisample;

	if (!window->open (device, title ? title : "", &hints, showMode, windowid, parentid, 0, instance))
	{
		return 0;
	}

	return window;
}

ATOM_RenderDevice *ATOM_Renderer::getRenderDevice (void) const
{
	return device;
}

ATOM_RenderWindow *ATOM_Renderer::getRenderWindow (void) const
{
	return device ? device->getWindow().get() : 0;
}

