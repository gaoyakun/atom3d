#include "StdAfx.h"
#include "waterrendertask.h"



ATOM_WaterRenderTask::ATOM_WaterRenderTask (ATOM_Water *waterNode)
:	_waterNode (waterNode)
{
	ATOM_STACK_TRACE(ATOM_WaterRenderTask::ATOM_WaterRenderTask);
}

ATOM_WaterRenderTask::~ATOM_WaterRenderTask (void)
{
	ATOM_STACK_TRACE(ATOM_WaterRenderTask::~ATOM_WaterRenderTask);
}

void ATOM_WaterRenderTask::render (ATOM_Scene *scene, bool clear)
{
	//ATOM_RenderDevice *device = ATOM_GetRenderDevice();

	//ATOM_AUTOREF(ATOM_Texture) oldRenderTarget = device->getRenderTarget ();
	//ATOM_AUTOREF(ATOM_Texture) oldRenderDepth = device->GetRenderDepth ();

	//const int *viewport = scene->getCamera()->getViewport();
	//if (!_renderTarget || _renderTarget->getWidth() != viewport[2] || _renderTarget->getHeight() != viewport[3])
	//{
	//	_renderTarget = device->allocTexture (0, 0, viewport[2], viewport[3], PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	//}

	//if (!oldRenderDepth && (!_renderDepth || _renderDepth->getWidth() != viewport[2] || _renderDepth->getHeight() != viewport[3]))
	//{
	//	_renderDepth = device->allocTexture (0, 0, viewport[2], viewport[3], PIXEL_FORMAT_DEPTH24, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
 // 	}

	//_waterNode->setShow (0);

	//ATOM_Texture *refractionRT = oldRenderTarget ? oldRenderTarget.get() : _renderTarget.get();
	//ATOM_Texture *refractionRD = oldRenderDepth ? oldRenderDepth.get() : _renderDepth.get();
	//if (refractionRT != oldRenderTarget.get() || refractionRD != oldRenderDepth.get())
	//{
	//	device->SetRenderTarget (refractionRT, refractionRD);
	//}

	//scene->getDefaultRenderTask()->render (scene, clear, forceMaterial, layerMask & ~ATOM_RenderQueue::BLEND_LAYERS);

	//ATOM_Texture *composeRT = oldRenderTarget ? _renderTarget.get() : 0;
	//device->SetRenderTarget (composeRT, refractionRD);

	//ATOM_WaterMaterialObject *
}


