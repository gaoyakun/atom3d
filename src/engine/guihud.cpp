#include "StdAfx.h"



ATOM_GuiHud::ATOM_GuiHud (void)
{
	ATOM_STACK_TRACE(ATOM_GuiHud::ATOM_GuiHud);

	_enableGeoClipping = false;
	_userZValueEnabled = false;
	_guiRenderer = ATOM_NEW(ATOM_GUIRenderer, false);
	_guiRenderer->set3DMode (true);
	_guiRenderer->getDesktop()->setClearBackground (false);

	_load_flag = LOAD_ALLFINISHED;
}

ATOM_GuiHud::~ATOM_GuiHud (void)
{
	ATOM_STACK_TRACE(ATOM_GuiHud::~ATOM_GuiHud);

	ATOM_DELETE(_guiRenderer);
}

void ATOM_GuiHud::accept (ATOM_Visitor &visitor)
{
	ATOM_STACK_TRACE(ATOM_GuiHud::accept);

	visitor.visit (*this);
}

void ATOM_GuiHud::update (const ATOM_Rect2Di &viewport, const ATOM_Matrix4x4f &mvp)
{
	ATOM_STACK_TRACE(ATOM_GuiHud::update);

	ATOM_Hud::update (viewport, mvp);

	int x, y;

	if (isScreenSpace ())
	{
		getPosition (x, y);
	}
	else
	{
		const ATOM_Vector3f &c = getCenterPosition ();
		x = ATOM_ftol (c.x);
		y = ATOM_ftol (c.y);
	}

	x += viewport.point.x;
	y += viewport.point.y;

	if (_alignment & ALIGN_LEFT)
	{
		//
	}
	else if (_alignment & ALIGN_RIGHT)
	{
		x -= getWidth();
	}
	else
	{
		x -= getWidth() / 2;
	}

	if (_alignment & ALIGN_TOP)
	{
		//
	}
	else if (_alignment & ALIGN_BOTTOM)
	{
		y -= getHeight();
	}
	else
	{
		y -= getHeight() / 2;
	}

	_guiRenderer->setViewport (ATOM_Rect2Di(x, y, getWidth(), getHeight()));
}

bool ATOM_GuiHud::draw (ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material)
{
	ATOM_STACK_TRACE(ATOM_GuiHud::render);

	_guiRenderer->ignoreFog (isIgnoreFog () != 0);
	_guiRenderer->setZValue (_userZValueEnabled ? getZ() : getCenterPosition().z);
	_guiRenderer->render ();

	return true;
}

ATOM_GUIRenderer *ATOM_GuiHud::getGuiRenderer (void) const
{
	return _guiRenderer;
}

void ATOM_GuiHud::enableUserZValue (bool enable)
{
	_userZValueEnabled = enable;
}

