#include "statecache.h"
#include "gfxdriver.h"

ATOM_StateCache::ATOM_StateCache (ATOM_GfxDriver *driver)
{
	_driver = driver;

	_defaultStates = driver->allocRenderStateSet (0);
	_currentStates = driver->allocRenderStateSet (0);
	_desiredStates = driver->allocRenderStateSet (0);
	_currentPass = 0;

	loadDefaults ();
}

ATOM_RenderStateSet *ATOM_StateCache::getDesiredStates (void) const
{
	return _desiredStates.get();
}

ATOM_RenderStateSet *ATOM_StateCache::getCurrentStates (void) const
{
	return _currentStates.get();
}

ATOM_RenderStateSet *ATOM_StateCache::getDefaultStates (void) const
{
	return _defaultStates.get();
}

bool ATOM_StateCache::loadDefaults (void)
{
	_defaultStates->useAlphaBlending (0)->setDefaults ();
	_defaultStates->useColorWrite (0)->setDefaults ();
	_defaultStates->useDepth (0)->setDefaults ();
	_defaultStates->useStencil (0)->setDefaults ();
	_defaultStates->useRasterizer (0)->setDefaults ();
	_defaultStates->useFog (0)->setDefaults ();
	_defaultStates->useMultisample (0)->setDefaults ();
	_defaultStates->useAlphaTest (0)->setDefaults ();
	for (unsigned i = 0; i < 16; ++i)
	{
		_defaultStates->useSampler (i, 0)->setDefaults ();
		_defaultStates->useTextureStage (i, 0)->setDefaults ();
	}

	_currentStates->useAlphaBlending (0)->setDefaults ();
	_currentStates->useColorWrite (0)->setDefaults ();
	_currentStates->useDepth (0)->setDefaults ();
	_currentStates->useStencil (0)->setDefaults ();
	_currentStates->useRasterizer (0)->setDefaults ();
	_currentStates->useFog (0)->setDefaults ();
	_currentStates->useMultisample (0)->setDefaults ();
	_currentStates->useAlphaTest (0)->setDefaults ();
	for (unsigned i = 0; i < 16; ++i)
	{
		_currentStates->useSampler (i, 0)->setDefaults ();
		_currentStates->useTextureStage (i, 0)->setDefaults ();
	}

	return true;
}

bool ATOM_StateCache::sync (void)
{
	return _desiredStates ? _desiredStates->apply (_currentPass, false) : false;
}

ATOM_GfxDriver *ATOM_StateCache::getGfxDriver (void) const
{
	return _driver;
}

