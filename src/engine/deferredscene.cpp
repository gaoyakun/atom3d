#include "StdAfx.h"
#include "deferredscene.h"
//#include "deferred_lighting.h"
#include "stdshadowmap_manager.h"
#include "cascadedshadowmap_manager.h"
#include "posteffect_water.h"

//static ATOM_DeferredLighting _deferredLighting;

//ATOM_AUTOREF(ATOM_Texture) ATOM_DeferredScene::_sceneRenderTarget;
//ATOM_AUTOREF(ATOM_DepthBuffer) ATOM_DeferredScene::_sceneDepthBuffer;
//ATOM_AUTOREF(ATOM_Texture) ATOM_DeferredScene::_positionMap;
//ATOM_AUTOREF(ATOM_Texture) ATOM_DeferredScene::_colorMap;
//ATOM_AUTOREF(ATOM_Texture) ATOM_DeferredScene::_normalMap;
//ATOM_AUTOREF(ATOM_Texture) ATOM_DeferredScene::_finalcolorMap;
//ATOM_AUTOREF(ATOM_Texture) ATOM_DeferredScene::_shadowMask;
//ATOM_AUTOREF(ATOM_Texture) ATOM_DeferredScene::_downsampledDepthBuffer;
//ATOM_AUTOREF(ATOM_Texture) ATOM_DeferredScene::_ssaoMap[2];
//ATOM_DeferredLighting *ATOM_DeferredScene::_lighting = &_deferredLighting;

ATOM_DeferredScene::ATOM_DeferredScene (void)
{
	ATOM_STACK_TRACE(ATOM_DeferredScene::ATOM_DeferredScene);

//	_enableDeferredRendering = true;
}

ATOM_DeferredScene::~ATOM_DeferredScene (void)
{
	ATOM_STACK_TRACE(ATOM_DeferredScene::~ATOM_DeferredScene);
}

/*
void ATOM_DeferredScene::addLight (ATOM_Light *light)
{
	ATOM_STACK_TRACE(ATOM_DeferredScene::addLight);

	switch (light->getLightType())
	{
	case ATOM_Light::Point:
		_omniLights.push_back (light);
		break;
	case ATOM_Light::Directional:
		_directionalLights.push_back (light);
		break;
	case ATOM_Light::Spot:
		_spotLights.push_back (light);
		break;
	default:
		break;
	}
}

void ATOM_DeferredScene::clearLights (void)
{
	ATOM_STACK_TRACE(ATOM_DeferredScene::clearLight);

	_omniLights.resize (0);
	_directionalLights.resize (0);
	_spotLights.resize (0);
}

void ATOM_DeferredScene::addDecal (ATOM_Decal *decal)
{
	_decals.push_back (decal);
}

void ATOM_DeferredScene::clearDecals (void)
{
	_decals.resize (0);
}

void ATOM_DeferredScene::setAtmosphere (ATOM_Atmosphere *atmosphere)
{
	_atmosphere = atmosphere;
}

ATOM_Atmosphere *ATOM_DeferredScene::getAtmosphere (void) const
{
	return _atmosphere.get();
}

*/