#include <ATOM.h>

#include "misc.h"

struct DeviceStateBlock
{
	ATOM_AUTOREF(ATOM_Texture) renderTarget;
	int viewport[4];
	float clearColor[4];
	unsigned flags;
};

static ATOM_VECTOR<DeviceStateBlock> stateBlockStack;

void pushDeviceStates (unsigned stateFlags)
{
	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();

	stateBlockStack.resize (stateBlockStack.size() + 1);
	DeviceStateBlock &newBlock = stateBlockStack.back ();
	newBlock.flags = stateFlags;

	if (stateFlags & DEVICESTATE_REDNERTARGET)
	{
		newBlock.renderTarget = device->getRenderTarget (0);
	}

	if (stateFlags & DEVICESTATE_VIEWPORT)
	{
		device->getViewport (device->getCurrentView(), &newBlock.viewport[0], &newBlock.viewport[1], &newBlock.viewport[2], &newBlock.viewport[3]);
	}

	if (stateFlags & DEVICESTATE_CLEARVALUE)
	{
		device->getClearColor (device->getCurrentView(), newBlock.clearColor[0], newBlock.clearColor[1], newBlock.clearColor[2], newBlock.clearColor[3]);
	}

	if (stateFlags & DEVICESTATE_TRANSFORM)
	{
		device->pushMatrix (MATRIXMODE_WORLD);
		device->pushMatrix (MATRIXMODE_VIEW);
		device->pushMatrix (MATRIXMODE_PROJECTION);
	}
}

void popDeviceStates (void)
{
	if (stateBlockStack.size() > 0)
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice ();

		const DeviceStateBlock &theBlock = stateBlockStack.back ();
		unsigned stateFlags = theBlock.flags;

		if (stateFlags & DEVICESTATE_REDNERTARGET)
		{
			device->setRenderTarget (0, theBlock.renderTarget.get());
		}

		if (stateFlags & DEVICESTATE_VIEWPORT)
		{
			device->setViewport (device->getCurrentView(), theBlock.viewport[0], theBlock.viewport[1], theBlock.viewport[2], theBlock.viewport[3]);
		}

		if (stateFlags & DEVICESTATE_CLEARVALUE)
		{
			device->setClearColor (device->getCurrentView(), theBlock.clearColor[0], theBlock.clearColor[1], theBlock.clearColor[2], theBlock.clearColor[3]);
		}

		if (stateFlags & DEVICESTATE_TRANSFORM)
		{
			device->popMatrix (MATRIXMODE_WORLD);
			device->popMatrix (MATRIXMODE_VIEW);
			device->popMatrix (MATRIXMODE_PROJECTION);
		}
	}
}

