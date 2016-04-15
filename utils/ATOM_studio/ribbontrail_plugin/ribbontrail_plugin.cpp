#include "StdAfx.h"
#include "atom3d_studio.h"
#include "plugin.h"
#include "gridnode.h"
#include "axisnode.h"
#include "ribbontrail_plugin.h"
#include "editor.h"
#include "camera_modal.h"

#define MENU_ITEMID_CONTROL					(PLUGIN_ID_START + 100)
#define ID_REPLAY							(PLUGIN_ID_START + 200)

PluginRibbonTrail::PluginRibbonTrail (void)
{
	_editor = 0;
	_scene = 0;
	_tbProperties = 0;
}

PluginRibbonTrail::~PluginRibbonTrail (void)
{
	clear ();
}

unsigned PluginRibbonTrail::getVersion (void) const
{
	return AS_VERSION;
}

const char *PluginRibbonTrail::getName (void) const
{
	return "Ribbon trail editor";
}

void PluginRibbonTrail::deleteMe (void)
{
	ATOM_DELETE(this);
}

bool PluginRibbonTrail::initPlugin (AS_Editor *editor)
{
	editor->registerFileType (this, "rbt", "ATOM3D 枪线特效", AS_FILETYPE_CANEDIT);

	_editor = editor;

	return true;
}

void PluginRibbonTrail::donePlugin (void)
{
	clear ();
}

bool PluginRibbonTrail::beginEdit (const char *filename)
{
	_scene = ATOM_NEW(ATOM_DeferredScene);
	ATOM_RenderScheme *renderScheme = _editor->getRenderScheme ();
	if (!renderScheme)
	{
		return false;
	}
	_scene->setRenderScheme (renderScheme);

	_grid = ATOM_HARDREF(GridNode)();
	_grid->setPickable (0);
	_grid->setSize (100, 100);
	_grid->load (ATOM_GetRenderDevice());
	_grid->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(100.f, 100.f, 100.f)));
	_scene->getRootNode()->appendChild (_grid.get());

	AS_CameraModal *camera = _editor->getCameraModal();
	camera->setPosition (ATOM_Vector3f(0.f, 500.f, -500.f));
	camera->setDirection (ATOM_Vector3f(0.f, -500.f, 500.f));
	camera->setUpVector (ATOM_Vector3f(0.f, 1.f, 0.f));

	_editor->getRealtimeCtrl()->setScene (_scene);

	_target = ATOM_HARDREF(ATOM_ShapeNode)();
	_target->setType (ATOM_ShapeNode::SPHERE);
	_target->setMaterialFileName ("/materials/builtin/shape.mat");
	if (_target->getMaterial())
	{
		_target->getMaterial()->getParameterTable()->setVector("diffuseColor", ATOM_Vector4f(1.f, 1.f, 0.f, 1.f));
	}
	_target->load (ATOM_GetRenderDevice());
	_target->setO2T (ATOM_Matrix4x4f::getTranslateMatrix (ATOM_Vector3f(0.f, 0.f, 1000.f))>>ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(10.f, 10.f, 10.f)));
	_scene->getRootNode()->appendChild (_target.get());

	_trail = ATOM_HARDREF(ATOM_RibbonTrail)();

	if (filename)
	{
		if (!_trail->loadFromFile (ATOM_GetRenderDevice(), filename))
		{
			return false;
		}
	}
	else
	{
		_trail->loadAttribute (NULL);
		_trail->load (ATOM_GetRenderDevice());
	}

	_trail->setTarget (_target.get(), 1.f);
	_trail->setDrawBoundingbox (0);
	_scene->getRootNode()->appendChild (_trail.get());

	createPropertyBar ();
	setupPropertyBar ();

	calcUILayout ();

	setupMenu ();

	return true;
}

void PluginRibbonTrail::endEdit (void)
{
	cleanupMenu ();

	clear ();
}

void PluginRibbonTrail::frameUpdate (void)
{
}

void PluginRibbonTrail::handleEvent (ATOM_Event *event)
{
	int eventId = event->getEventTypeId ();

	if (eventId == ATOM_WidgetCommandEvent::eventTypeId ())
	{
		ATOM_WidgetCommandEvent *e = (ATOM_WidgetCommandEvent*)event;
		if (e->id == ID_REPLAY)
		{
			_trail->reset ();
		}
	}
	else if (eventId == ATOM_WidgetResizeEvent::eventTypeId())
	{
		calcUILayout ();
	}
}

bool PluginRibbonTrail::saveFile (const char *filename)
{
	return _trail->save (filename);
}

unsigned PluginRibbonTrail::getMinWindowWidth (void) const
{
	return 600;
}

unsigned PluginRibbonTrail::getMinWindowHeight (void) const
{
	return 450;
}

bool PluginRibbonTrail::isDocumentModified (void)
{
	return _editor->isDocumentModified ();
}

void PluginRibbonTrail::setupMenu (void)
{
	ATOM_MenuBar *menubar = _editor->getMenuBar ();
	menubar->insertMenuItem (AS_MENUITEM_CUSTOM, "控制", MENU_ITEMID_CONTROL);
	ATOM_PopupMenu *menuPart = menubar->createSubMenu (AS_MENUITEM_CUSTOM);
	menuPart->appendMenuItem ("重新播放", ID_REPLAY);
}

void PluginRibbonTrail::cleanupMenu (void)
{
	ATOM_MenuBar *menubar = _editor->getMenuBar ();
	menubar->removeMenuItem (AS_MENUITEM_CUSTOM);
}

void PluginRibbonTrail::createPropertyBar (void)
{
	_tbProperties = ATOM_NEW(ATOMX_PropertyTweakBar, "Trail Properties", _trail.get());
	_tbProperties->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
	_tbProperties->setBarMovable (false);
	_tbProperties->setBarIconifiable (false);
	_tbProperties->setBarResizable (false);
	_tbProperties->setBarFontResizable (false);
}

void PluginRibbonTrail::setupPropertyBar (void)
{
}

void PluginRibbonTrail::clear (void)
{
	_editor->getRealtimeCtrl()->setScene (0);

	ATOM_DELETE(_tbProperties);
	_tbProperties = 0;

	ATOM_DELETE(_scene);
	_scene = 0;

	_grid = 0;
	_trail = 0;
}

void PluginRibbonTrail::calcUILayout (void)
{
	ATOM_Rect2Di rc = _editor->getRealtimeCtrl()->getClientRect();

	ATOM_Point2Di pt0(rc.size.w - 200, 0);
	_editor->getRealtimeCtrl()->clientToGUI (&pt0);
	_tbProperties->setBarPosition (pt0.x, pt0.y);
	_tbProperties->setBarSize (200, rc.size.h);
}

void PluginRibbonTrail::handleTransformEdited (ATOM_Node *node)
{
}

void PluginRibbonTrail::handleScenePropChanged (void)
{
}

void PluginRibbonTrail::changeRenderScheme (void)
{
	_scene->setRenderScheme (_editor->getRenderScheme());
}

