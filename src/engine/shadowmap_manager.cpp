#include "StdAfx.h"
#include "shadowmap_manager.h"

ATOM_ShadowMapManager::ATOM_ShadowMapManager (void)
{
	ATOM_STACK_TRACE(ATOM_ShadowMapManager::ATOM_ShadowMapManager);
	_shadowMapWidth = 2048;
	_shadowMapHeight = 2048;
	_shadowDistance = 3000.f;
	_shadowMapSizeChanged = true;
	_shadowDistanceChanged = true;
}

ATOM_ShadowMapManager::~ATOM_ShadowMapManager (void)
{
	ATOM_STACK_TRACE(ATOM_ShadowMapManager::~ATOM_ShadowMapManager);
}

void ATOM_ShadowMapManager::setShadowMapWidth (unsigned width)
{
	if (width != _shadowMapWidth)
	{
		_shadowMapSizeChanged = true;
		_shadowMapWidth = width;
	}
}

unsigned ATOM_ShadowMapManager::getShadowMapWidth (void) const
{
	return _shadowMapWidth;
}

void ATOM_ShadowMapManager::setShadowMapHeight (unsigned height)
{
	if (height != _shadowMapHeight)
	{
		_shadowMapSizeChanged = true;
		_shadowMapHeight = height;
	}
}

unsigned ATOM_ShadowMapManager::getShadowMapHeight (void) const
{
	return _shadowMapHeight;
}

void ATOM_ShadowMapManager::setShadowDistance (float distance)
{
	if (_shadowDistance != distance)
	{
		_shadowDistanceChanged = true;
		_shadowDistance = distance;
	}
}

float ATOM_ShadowMapManager::getShadowDistance (void) const
{
	return _shadowDistance;
}

bool ATOM_ShadowMapManager::renderShadowMap (ATOM_DeferredScene *scene, ATOM_Camera *camera)
{
	ATOM_STACK_TRACE(ATOM_ShadowMapManager::renderShadowMap);
	if (_shadowMapSizeChanged)
	{
		_shadowMapSizeChanged = false;

		if (!initializeShadowMapTexture (_shadowMapWidth, _shadowMapHeight))
		{
			return false;
		}

		if (!onShadowMapSizeChanged ())
		{
			return false;
		}
	}

	return onRenderShadowMap (scene, camera);
}

bool ATOM_ShadowMapManager::createShadowMaskTexture (ATOM_Texture *shadowMaskTexture, ATOM_Texture *gbufferTexture)
{
	ATOM_STACK_TRACE(ATOM_ShadowMapManager::createShadowMaskTexture);
	return onCreateShadowMaskTexture (shadowMaskTexture, gbufferTexture);
}

bool ATOM_ShadowMapManager::onShadowMapSizeChanged (void)
{
	return true;
}

bool ATOM_ShadowMapManager::onShadowDistanceChanged (void)
{
	return true;
}


