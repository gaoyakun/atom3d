#include "StdAfx.h"
#include "statecache.h"

ATOM_StateCache::ATOM_StateCache (ATOM_RenderDevice *device)
{
	_device = device;
	_currentPass = 0;

	_defaultStates = device->allocRenderStateSet (0);
	_currentStates = device->allocRenderStateSet (0);
	_desiredStates = device->allocRenderStateSet (0);

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

void ATOM_StateCache::setCurrentPass (int pass)
{
	_currentPass = pass;
}

bool ATOM_StateCache::loadDefaults (void)
{
	_defaultStates->useAlphaBlending (0)->setDefaults ();
	_defaultStates->useColorWrite (0)->setDefaults ();
	_defaultStates->useDepth (0)->setDefaults ();
	_defaultStates->useStencil (0)->setDefaults ();
	_defaultStates->useRasterizer (0)->setDefaults ();
	_defaultStates->useMultisample (0)->setDefaults ();
	_defaultStates->useAlphaTest (0)->setDefaults ();
	for (unsigned i = 0; i < ATOM_RenderAttributes::SamplerCount; ++i)
	{
		_defaultStates->useSampler (i, 0)->setDefaults ();
	}

	_currentStates->useAlphaBlending (0)->setDefaults ();
	_currentStates->useColorWrite (0)->setDefaults ();
	_currentStates->useDepth (0)->setDefaults ();
	_currentStates->useStencil (0)->setDefaults ();
	_currentStates->useRasterizer (0)->setDefaults ();
	_currentStates->useMultisample (0)->setDefaults ();
	_currentStates->useAlphaTest (0)->setDefaults ();
	for (unsigned i = 0; i < ATOM_RenderAttributes::SamplerCount; ++i)
	{
		_currentStates->useSampler (i, 0)->setDefaults ();
	}

	return true;
}

bool ATOM_StateCache::sync (void)
{
	return _desiredStates ? _desiredStates->apply (_currentPass, false) : false;
}

ATOM_RenderDevice *ATOM_StateCache::getDevice (void) const
{
	return _device;
}

