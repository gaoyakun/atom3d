#include "StdAfx.h"
#include "posteffect.h"

ATOM_BasePostEffect::~ATOM_BasePostEffect (void)
{
}

void ATOM_BasePostEffect::setNextEffect (ATOM_BasePostEffect *nextEffect)
{
	_nextEffect = nextEffect;
}

ATOM_BasePostEffect *ATOM_BasePostEffect::getNextEffect (void) const
{
	return _nextEffect.get();
}

void ATOM_BasePostEffect::chain (ATOM_BasePostEffect *tailEffect)
{
	if (!_nextEffect)
	{
		_nextEffect = tailEffect;
	}
	else
	{
		_nextEffect->chain (tailEffect);
	}
}

bool ATOM_BasePostEffect::render (ATOM_RenderDevice *device, ATOM_Texture *inputColor)
{
	ATOM_STACK_TRACE(ATOM_BasePostEffect::render);

	if (_nextEffect && !_outputColor)
	{
		_outputColor = doAllocOutputColorTexture (device, inputColor);

		if (!_outputColor)
		{
			return false;
		}
	}

	ATOM_AUTOREF(ATOM_Texture) oldRenderTarget = device->getRenderTarget (0);
	int vx, vy, vw, vh;

	if (_nextEffect)
	{
		device->getViewport (device->getCurrentView(), &vx, &vy, &vw, &vh);
		oldRenderTarget = device->getRenderTarget (0);

		device->setRenderTarget (0, _outputColor.get());
		device->setViewport (device->getCurrentView(), 0, 0, _outputColor->getWidth(), _outputColor->getHeight());
	}

	bool b = doRender (device, inputColor);

	if (_nextEffect)
	{
		device->setRenderTarget (0, oldRenderTarget.get());
		device->setViewport (device->getCurrentView(), vx, vy, vw, vh);

		if (b)
		{
			b = _nextEffect->render (device, _outputColor.get());
		}
	}

	return b;
}

void ATOM_BasePostEffect::drawTexturedFullscreenQuad (ATOM_RenderDevice *device, unsigned w, unsigned h)
{
	ATOM_STACK_TRACE(ATOM_BasePostEffect::drawTexturedFullscreenQuad);

	device->pushMatrix (ATOM_MATRIXMODE_WORLD);
	device->pushMatrix (ATOM_MATRIXMODE_VIEW);
	device->pushMatrix (ATOM_MATRIXMODE_PROJECTION);
	device->loadIdentity (ATOM_MATRIXMODE_WORLD);
	device->loadIdentity (ATOM_MATRIXMODE_VIEW);
	device->loadIdentity (ATOM_MATRIXMODE_PROJECTION);

	float deltax = (float)0.5f/(float)w;
	float deltay = (float)0.5f/(float)h;

	float vertices[4 * 5] = {
		-1.f - deltax, -1.f + deltay, 0.f, 0.f, 1.f,
		 1.f - deltax, -1.f + deltay, 0.f, 1.f, 1.f,
		 1.f - deltax,  1.f + deltay, 0.f, 1.f, 0.f,
	    -1.f - deltax,  1.f + deltay, 0.f, 0.f, 0.f
	};

	unsigned short indices[4] = {
		0, 1, 2, 3
	};

	device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);

	device->popMatrix (ATOM_MATRIXMODE_WORLD);
	device->popMatrix (ATOM_MATRIXMODE_VIEW);
	device->popMatrix (ATOM_MATRIXMODE_PROJECTION);
}


