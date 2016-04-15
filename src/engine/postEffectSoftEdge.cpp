#include "StdAfx.h"
#include "postEffectSoftEdge.h"

ATOM_SoftEdgeEffect::ATOM_SoftEdgeEffect (void)
{
	_materialError = false;
	_material = 0;
	_softEdgeMaskTexture = "";
	_softTexture = 0;

	_gammaFlag = AFTER_GAMMA_CORRECT;
}

ATOM_SoftEdgeEffect::~ATOM_SoftEdgeEffect (void)
{
	//_chain = 0;
}

void ATOM_SoftEdgeEffect::setSoftEdgeTexture(const char* file)
{
	if( !file )
		return;

	if( _softEdgeMaskTexture == file )
		return;

	_softEdgeMaskTexture = file;

	_softTexture = ATOM_CreateTextureResource(	_softEdgeMaskTexture.c_str(),
												ATOM_PIXEL_FORMAT_UNKNOWN,
												ATOM_LoadPriority_IMMEDIATE	);
}

bool ATOM_SoftEdgeEffect::render (ATOM_RenderDevice *device)
{
	if( !_chain )
		return false;

	ATOM_PostEffect *prevEffect = getPreviousEffect ();
	ATOM_Texture *inputTexture = prevEffect ? prevEffect->getRenderTarget() : _chain->getInputTexture();
	if (!inputTexture)
	{
		return false;
	}

	if (!_material)
	{
		if (_materialError)
		{
			return false;
		}

		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/pp_soft_edge.mat");
		if (!_material)
		{
			_materialError = true;
			return false;
		}
		_material->setActiveEffect ("default");
	}
	if( !_softTexture )
	{
		_softEdgeMaskTexture = "/textures/soft_edge_mask.png";
		_softTexture = ATOM_CreateTextureResource("/textures/soft_edge_mask.png",ATOM_PIXEL_FORMAT_UNKNOWN,ATOM_LoadPriority_IMMEDIATE);
	}

	_material->getParameterTable()->setTexture ("inputTexture", inputTexture);
	_material->getParameterTable()->setTexture ("softEdgeTexture", _softTexture.get());

	//device->setRenderTarget (0, getRenderTarget());
	//device->setViewport (0, ATOM_Rect2Di(500, 300, 100, 100));

	// ¿ªÆôSRGBÐ´RT
	//device->enableSRGBWrite(true);

	drawTexturedFullscreenQuad (device, _material.get(), inputTexture->getWidth(), inputTexture->getHeight());

	// ¹Ø±ÕSRGBÐ´RT
	//device->enableSRGBWrite(false);

	return true;
}