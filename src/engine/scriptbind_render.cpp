#include "StdAfx.h"
#include "scriptbind_traits.h"

ATOM_DisplayMode DisplayMode_Constructor (int w, int h, int d, int f)
{
	ATOM_DisplayMode dm;
	dm.width = w;
	dm.height = h;
	dm.depth = d;
	dm.freq = f;
	return dm;
}

void RenderDevice_SetClearColor (ATOM_RenderDevice *device, ATOM_RenderWindow *view, float r, float g, float b, float a)
{
	device->setClearColor (view, r, g, b, a);
}

ATOM_ColorARGB RenderDevice_GetClearColor (ATOM_RenderDevice *device, ATOM_RenderWindow *view)
{
	return device->getClearColor (view);
}

void RenderDevice_SetViewport (ATOM_RenderDevice *device, ATOM_RenderWindow *view, int x, int y, int w, int h, float depthMin, float depthMax)
{
	device->setViewport (view, x, y, w, h, depthMin, depthMax);
}

void RenderDevice_SetScissorRect (ATOM_RenderDevice *device, ATOM_RenderWindow *view, int x, int y, int w, int h)
{
	device->setScissorRect (view, x, y, w, h);
}

ATOM_Matrix4x4f RenderDevice_GetTransform (ATOM_RenderDevice *device, int mode)
{
	ATOM_Matrix4x4f m;
	device->getTransform (mode, m);
	return m;
}

ATOM_Matrix4x4f RenderDevice_GetTextureTransform (ATOM_RenderDevice *device, int stage)
{
	ATOM_Matrix4x4f m;
	device->getTextureTransform (stage, m);
	return m;
}

bool RenderDevice_ScreenShot (ATOM_RenderDevice *device, ATOM_RenderWindow *view, const char *filename)
{
	return device->screenShot (view, filename);
}

void RenderDevice_SetCursorHandle (ATOM_RenderDevice *device)
{
	device->setCustomCursor (nullptr, (HCURSOR)::LoadCursorA(NULL, IDC_ARROW));
}

void RenderDevice_DumpResources (ATOM_RenderDevice *device)
{
	int numTextures = 0;
	int numVBs = 0;
	int numIBs = 0;
	int numDepthBuffers = 0;
	int numRTSurfaces = 0;
	int numShaders = 0;

	ATOM_GfxResource *r = device->getResourceList();
	for (;;)
	{
		if (dynamic_cast<ATOM_Texture*>(r))
		{
			++numTextures;
		}
		else if (dynamic_cast<ATOM_VertexArray*>(r))
		{
			++numVBs;
		}
		else if (dynamic_cast<ATOM_IndexArray*>(r))
		{
			++numIBs;
		}
		else if (dynamic_cast<ATOM_DepthBuffer*>(r))
		{
			++numDepthBuffers;
		}
		else if (dynamic_cast<ATOM_RenderTargetSurface*>(r))
		{
			++numRTSurfaces;
		}
		else if (dynamic_cast<ATOM_Shader*>(r))
		{
			++numShaders;
		}

		r = r->getNext();
		if (r == device->getResourceList())
		{
			break;
		}
	}

	ATOM_LOGGER::log ("%d Textures\n%d VBs\n%d IBs\n%d DepthBuffers\n%d RTSurfaces\n%d Shaders\n", numTextures,numVBs,numIBs,numDepthBuffers,numRTSurfaces,numShaders);
}

ATOM_SCRIPT_BEGIN_TYPE_TABLE(RenderTypes)

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_MouseMoveEvent, ATOM_MouseMoveEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_MouseMoveEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_RAW_ATTRIBUTE(x, ATOM_MouseMoveEvent, x, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(y, ATOM_MouseMoveEvent, y, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(xrel, ATOM_MouseMoveEvent, xrel, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(yrel, ATOM_MouseMoveEvent, yrel, int)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_MouseMoveEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_MouseWheelEvent, ATOM_MouseWheelEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_MouseWheelEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_RAW_ATTRIBUTE(x, ATOM_MouseWheelEvent, x, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(y, ATOM_MouseWheelEvent, y, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(delta, ATOM_MouseWheelEvent, delta, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(keymod, ATOM_MouseWheelEvent, keymod, unsigned)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_MouseWheelEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_MouseButtonDownEvent, ATOM_MouseButtonDownEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_MouseButtonDownEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_RAW_ATTRIBUTE(x, ATOM_MouseButtonDownEvent, x, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(y, ATOM_MouseButtonDownEvent, y, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(button, ATOM_MouseButtonDownEvent, button, ATOM_MouseButton)
		ATOM_DECLARE_RAW_ATTRIBUTE(keymod, ATOM_MouseButtonDownEvent, keymod, unsigned)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_MouseButtonDownEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_MouseButtonUpEvent, ATOM_MouseButtonUpEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_MouseButtonUpEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_RAW_ATTRIBUTE(x, ATOM_MouseButtonUpEvent, x, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(y, ATOM_MouseButtonUpEvent, y, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(button, ATOM_MouseButtonUpEvent, button, ATOM_MouseButton)
		ATOM_DECLARE_RAW_ATTRIBUTE(keymod, ATOM_MouseButtonUpEvent, keymod, unsigned)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_MouseButtonUpEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_KeyDownEvent, ATOM_KeyDownEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_KeyDownEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_RAW_ATTRIBUTE(key, ATOM_KeyDownEvent, key, ATOM_Key)
		ATOM_DECLARE_RAW_ATTRIBUTE(keymod, ATOM_KeyDownEvent, keymod, unsigned)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_KeyDownEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_KeyUpEvent, ATOM_KeyUpEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_KeyUpEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_RAW_ATTRIBUTE(key, ATOM_KeyUpEvent, key, ATOM_Key)
		ATOM_DECLARE_RAW_ATTRIBUTE(keymod, ATOM_KeyUpEvent, keymod, unsigned)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_KeyUpEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_WindowResizeEvent, ATOM_WindowResizeEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_WindowResizeEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_RAW_ATTRIBUTE(width, ATOM_WindowResizeEvent, width, unsigned)
		ATOM_DECLARE_RAW_ATTRIBUTE(height, ATOM_WindowResizeEvent, height, unsigned)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_WindowResizeEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_WindowActiveEvent, ATOM_WindowActiveEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_WindowActiveEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_RAW_ATTRIBUTE(active, ATOM_WindowActiveEvent, active, bool)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_WindowActiveEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_CharEvent, ATOM_CharEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_CharEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_RAW_ATTRIBUTE(mbcc, ATOM_CharEvent, mbcc, unsigned short)
		ATOM_DECLARE_RAW_ATTRIBUTE(unicc, ATOM_CharEvent, unicc, unsigned short)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_CharEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_WindowMoveEvent, ATOM_WindowMoveEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_WindowMoveEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_RAW_ATTRIBUTE(x, ATOM_WindowMoveEvent, x, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(y, ATOM_WindowMoveEvent, y, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(xrel, ATOM_WindowMoveEvent, xrel, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(yrel, ATOM_WindowMoveEvent, yrel, int)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_WindowMoveEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_WindowExposeEvent, ATOM_WindowExposeEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_WindowExposeEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_WindowExposeEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_WindowCloseEvent, ATOM_WindowCloseEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_RAW_ATTRIBUTE(win, ATOM_WindowCloseEvent, win, ATOM_RenderWindow*)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_WindowCloseEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_DeviceLostEvent, ATOM_DeviceLostEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_DeviceLostEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_DeviceResetEvent, ATOM_DeviceResetEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_DeviceResetEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(ATOM_DisplayMode, ATOM_DisplayMode)
		ATOM_DECLARE_RAW_ATTRIBUTE(width, ATOM_DisplayMode, width, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(height, ATOM_DisplayMode, height, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(depth, ATOM_DisplayMode, depth, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(freq, ATOM_DisplayMode, freq, int)
		ATOM_DECLARE_CONSTRUCTOR(DisplayMode_Constructor)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS_NOPARENT_NOCONSTRUCTOR(ATOM_RenderDevice, ATOM_RenderDevice)
		ATOM_DECLARE_METHOD(isMultisampleSupported, ATOM_RenderDevice::isMultisampleSupported)
		ATOM_DECLARE_METHOD(isSampleSupported, ATOM_RenderDevice::isSampleSupported)
		ATOM_DECLARE_METHOD(setClearColor, RenderDevice_SetClearColor)
		ATOM_DECLARE_METHOD(getClearColor, RenderDevice_GetClearColor)
		ATOM_DECLARE_METHOD(setClearDepth, ATOM_RenderDevice::setClearDepth)
		ATOM_DECLARE_METHOD(setClearStencil, ATOM_RenderDevice::setClearStencil)
		ATOM_DECLARE_METHOD(setViewport, RenderDevice_SetViewport)
		ATOM_DECLARE_METHOD(setScissorRect, RenderDevice_SetScissorRect)
		ATOM_DECLARE_METHOD(enableScissorTest, ATOM_RenderDevice::enableScissorTest)
		ATOM_DECLARE_METHOD(isScissorTestEnabled, ATOM_RenderDevice::isScissorTestEnabled)
		ATOM_DECLARE_METHOD(screenShot, RenderDevice_ScreenShot)
		ATOM_DECLARE_METHOD(setCursorHandle, RenderDevice_SetCursorHandle)
		ATOM_DECLARE_METHOD(dumpResource, RenderDevice_DumpResources)
//		ATOM_DECLARE_METHOD(enableWireFrameMode, ATOM_RenderDevice::enableWireFrameMode)
//		ATOM_DECLARE_METHOD(isWireFrameMode, ATOM_RenderDevice::isWireFrameMode)
		ATOM_DECLARE_METHOD(getAvailableTextureMem, ATOM_RenderDevice::getAvailableTextureMem)
		ATOM_DECLARE_METHOD(setTransform, ATOM_RenderDevice::setTransform)
		ATOM_DECLARE_METHOD(getTransform, RenderDevice_GetTransform)
		ATOM_DECLARE_METHOD(pushMatrix, ATOM_RenderDevice::pushMatrix)
		ATOM_DECLARE_METHOD(popMatrix, ATOM_RenderDevice::popMatrix)
		ATOM_DECLARE_METHOD(loadIdentity, ATOM_RenderDevice::loadIdentity)
		ATOM_DECLARE_METHOD(setTextureTransform, ATOM_RenderDevice::setTextureTransform)
		ATOM_DECLARE_METHOD(getTextureTransform, RenderDevice_GetTextureTransform)
//		ATOM_DECLARE_METHOD(enableFSAA, ATOM_RenderDevice::enableFSAA)
		ATOM_DECLARE_METHOD(getVendor, ATOM_RenderDevice::getVendor)
		ATOM_DECLARE_METHOD(getRenderer, ATOM_RenderDevice::getRenderer)
//		ATOM_DECLARE_METHOD(revertWindingOrder, ATOM_RenderDevice::revertWindingOrder)
		ATOM_DECLARE_METHOD(evictManagedResources, ATOM_RenderDevice::evictManagedResource)
		ATOM_DECLARE_METHOD(beginFrame, ATOM_RenderDevice::beginFrame)
		ATOM_DECLARE_METHOD(endFrame, ATOM_RenderDevice::endFrame)
		ATOM_DECLARE_METHOD(clear, ATOM_RenderDevice::clear)
//		ATOM_DECLARE_METHOD(setColorWriteEnable, ATOM_RenderDevice::setColorWriteEnable)
//		ATOM_DECLARE_METHOD(fillViewport, ATOM_RenderDevice::fillViewport)
		ATOM_DECLARE_METHOD(present, ATOM_RenderDevice::present)
		ATOM_DECLARE_METHOD(getNumDrawCalls, ATOM_RenderDevice::getNumDrawCalls)
		ATOM_DECLARE_METHOD(getNumPrimitivesDrawn, ATOM_RenderDevice::getNumPrimitivesDrawn)
		ATOM_DECLARE_METHOD(getNumTextMaps, ATOM_RenderDevice::getNumTextMaps)
//		ATOM_DECLARE_METHOD(enableSRGB, ATOM_RenderDevice::enableSRGB)
//		ATOM_DECLARE_METHOD(isSRGBEnabled, ATOM_RenderDevice::isSRGBEnabled)
	ATOM_SCRIPT_END_CLASS ()

ATOM_SCRIPT_END_TYPE_TABLE

void __render_bind_script (ATOM_Script *script)
{
	ATOM_SCRIPT_REGISTER_TYPE_TABLE(script, RenderTypes);
}
