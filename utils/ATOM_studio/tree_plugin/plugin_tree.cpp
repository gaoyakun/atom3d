#include "StdAfx.h"

#if defined(SUPPORT_BILLBOARD_TREE)

#include "atom3d_studio.h"
#include "plugin.h"
#include "treedata.h"
#include "trunk.h"
#include "leaf.h"
#include "tree.h"
#include "plugin_tree.h"
#include "editor.h"
#include "camera_modal.h"
#include "gridnode.h"

#define PARAM_ID_SHAPE				1
#define PARAM_ID_ADDLEVEL			2
#define PARAM_ID_REMOVELEVEL		3
#define PARAM_ID_STEMTEXTURE		4
#define PARAM_ID_LEAFTEXTURE		5
#define PARAM_ID_REGENERATE			6
#define PARAM_ID_ADDLEAFCUSTER		7
#define PARAM_ID_TOGGLECLUSTER		8
#define PARAM_ID_TOGGLEVIEWMODE		10
#define PARAM_ID_TOGGLEBBOX			11
#define PARAM_ID_EXPORT				12
#define PARAM_ID_IMPORT_TRUNKMODEL	13
#define PARAM_ID_LEAFCARDNUM_BASE	400

PluginTree::PluginTree (void)
{
	_paramTweakBar = 0;
	_viewTweakBar = 0;
	_lastUpdateTick = 0;
	_trunkTriangleCount = 0;
	_leafTriangleCount = 0;
	_FPS = 0.f;
	_updateInterval = 1000;
	_isEditSphereTransform = false;
	_drawExternTrunk = 0;
	_drawLeaves = 1;
	_drawNativeTrunk = 1;
}

PluginTree::~PluginTree (void) 
{
}

unsigned PluginTree::getVersion (void) const
{
	return AS_VERSION;
}

const char *PluginTree::getName (void) const
{
	return "Tree editor";
}

void PluginTree::deleteMe (void)
{
	ATOM_DELETE(this);
}

bool PluginTree::initPlugin (AS_Editor *editor)
{
	editor->registerFileType (this, "xml", "ATOM3D 树", AS_FILETYPE_CANEDIT);

	_editor = editor;

	return true;
}

void PluginTree::donePlugin (void)
{
}

bool PluginTree::beginEdit (const char *filename)
{
	_tree = ATOM_HARDREF(TreeNode)();
	_tree->setFileName (filename ? filename : "");
	_tree->setPickable (0);

	if (!_tree->load (ATOM_GetRenderDevice()))
	{
		return false;
	}

	_tree->setDrawBoundingbox (0);
	_tree->setRenderMode (TreeNode::DRAW_AUTO);
	_scene = ATOM_NEW(ATOM_DeferredScene);
	ATOM_RenderScheme *renderScheme = _editor->getRenderScheme ();
	if (!renderScheme)
	{
		return false;
	}
	_scene->setRenderScheme (renderScheme);
	_editor->getRealtimeCtrl()->setScene (_scene);
	_scene->getRootNode()->appendChild (_tree.get());
	_editor->lookAtNode (_tree.get());

	ATOM_HARDREF(ATOM_LightNode) lightNode;
	lightNode->setLightType (ATOM_Light::Directional);
	lightNode->setLightColor (ATOM_Vector4f(1.f,1.f,1.f,1.f));
	lightNode->getLight()->setDirection (ATOM_Vector3f(1.f, -1.f, 1.f));
	_scene->getRootNode()->appendChild (lightNode.get());

	ATOM_HARDREF(GridNode) grid;
	grid->setPickable (0);
	grid->setSize (100, 100);
	grid->load (ATOM_GetRenderDevice());
	grid->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(100.f, 100.f, 100.f)));
	_scene->getRootNode()->appendChild (grid.get());

	_treeData = _tree->getTrunk().getTreeData();

	_currentLeafParam._flipLeaves = _tree->getLeaves().getFlip() ? 1 : 0;
	_currentLeafParam._leafHeight = _tree->getLeaves().getHeight();
	_currentLeafParam._leafWidth = _tree->getLeaves().getWidth();
	_currentLeafParam._leafSizeV = _tree->getLeaves().getSizeV();
	_currentLeafParam._leafRandomRange = _tree->getLeaves().getRandomRange();
	_currentLeafParam._numLeafCards = _tree->getLeaves().getDensity();
	_desiredLeafParam = _currentLeafParam;
	_lodDistance = _tree->getLODDistance ();
	_drawNativeTrunk = _tree->getTrunk().isVisible() ? 1 : 0;
	_drawExternTrunk = _tree->getTrunk().getDrawAlternate () ? 1 : 0;
	_drawLeaves = _tree->getLeaves().isVisible() ? 1 : 0;

	createParamTweakBar ();
	createViewTweakBar ();
	calcUILayout ();

	_treeDirty = true;

	showClusters (false);

	return true;
}

void PluginTree::endEdit (void)
{
	_editor->getRealtimeCtrl()->setScene (0);
	_tree = 0;

	ATOM_DELETE(_scene);
	ATOM_DELETE(_paramTweakBar);
	ATOM_DELETE(_viewTweakBar);
}

void PluginTree::frameUpdate (void)
{
	_tree->setLODDistance (_lodDistance);
	_tree->getTrunk().setTreeData (_treeData, _tree->getRandomSeedTrunk());
	_tree->getTrunk().setVisible (_drawNativeTrunk != 0);
	_tree->getTrunk().setDrawAlternate (_drawExternTrunk != 0);
	_tree->getLeaves().setVisible (_drawLeaves != 0);
	_tree->getLeaves().setRandomRange (_desiredLeafParam._leafRandomRange);
	_tree->getLeaves().setSizeV (_desiredLeafParam._leafSizeV);
	_tree->getLeaves().setDensity (_desiredLeafParam._numLeafCards);
	_tree->getLeaves().setFlip (_desiredLeafParam._flipLeaves != 0);
	_tree->getLeaves().setWidth (_desiredLeafParam._leafWidth);
	_tree->getLeaves().setHeight (_desiredLeafParam._leafHeight);
	_tree->checkBillboardValid ();

	_trunkTriangleCount = _tree->getTrunk().getFaceCount();
	_leafTriangleCount = _tree->getLeaves().getFaceCount();
	_FPS = ATOM_APP->getFrameStamp().FPS;
}

void PluginTree::selectSphereNode (ATOM_Node *node)
{
	if (node != _selectedSphereNode.get())
	{
		if (_selectedSphereNode)
		{
			_selectedSphereNode->setDrawBoundingbox (false);
		}

		_selectedSphereNode = node;

		if (node)
		{
			node->setDrawBoundingbox (true);
		}
	}
}

void PluginTree::handleEvent (ATOM_Event *event)
{
	int eventId = event->getEventTypeId ();

	if (eventId == ATOM_WidgetLButtonUpEvent::eventTypeId ())
	{
		ATOM_WidgetLButtonUpEvent *e = (ATOM_WidgetLButtonUpEvent *)event;
		if (!_isEditSphereTransform)
		{
			ATOM_PickVisitor v;
			_editor->getRealtimeCtrl()->pick (e->x, e->y, v);
			if (v.getNumPicked ())
			{
				v.sortResults ();
				ATOM_Node *node = v.getPickResult (0).node.get();
				ATOM_ShapeNode *shape = dynamic_cast<ATOM_ShapeNode*>(node);
				if (shape)
				{
					selectSphereNode (shape);
				}
				else
				{
					selectSphereNode (0);
				}
			}
		}
	}
	else if (eventId == ATOM_WidgetKeyDownEvent::eventTypeId ())
	{
		ATOM_WidgetKeyDownEvent *e = (ATOM_WidgetKeyDownEvent*)event;
		if (_selectedSphereNode)
		{
			switch (e->key)
			{
			case KEY_t:
				{
					_selectedSphereNode->setDrawBoundingbox (false);
					beginTranslateSphere (_selectedSphereNode.get());
					_isEditSphereTransform = true;
					break;
				}
			case KEY_s:
				{
					_selectedSphereNode->setDrawBoundingbox (false);
					beginScaleSphere (_selectedSphereNode.get());
					_isEditSphereTransform = true;
					break;
				}
			case KEY_ESCAPE:
				{
					if (_isEditSphereTransform)
					{
						_isEditSphereTransform = false;
						endEditSphereTransform (_selectedSphereNode.get());
						_selectedSphereNode->setDrawBoundingbox (true);
					}
					break;
				}
			case KEY_DELETE:
				{
					if (_isEditSphereTransform)
					{
						_isEditSphereTransform = false;
						endEditSphereTransform (_selectedSphereNode.get());
						_selectedSphereNode->setDrawBoundingbox (true);
					}
					_editor->doNodeDeleteOp (_selectedSphereNode.get(), _selectedSphereNode->getParent());
					_tree->regenerateLeaves(false);
					break;
				}
			}
		}
	}
	else if (eventId == ATOMX_TWValueChangedEvent::eventTypeId ())
	{
		ATOMX_TWValueChangedEvent *e = (ATOMX_TWValueChangedEvent*)event;
		if (e->bar == _viewTweakBar)
		{
			if (e->id == PARAM_ID_TOGGLEVIEWMODE)
			{
				TreeNode::RenderMode newMode = (TreeNode::RenderMode)e->newValue.getI();
				_tree->setRenderMode (newMode);
			}
		}
		else if (e->bar == _paramTweakBar)
		{
			if (e->id == PARAM_ID_SHAPE)
			{
				_treeData.shape = e->newValue.getI ();
			}
			else if (_treeData.levels < 1)
			{
				_treeData.levels = 1;
			}
			else if (_treeData.levels > 4)
			{
				_treeData.levels = 4;
			}
		}
	}
	else if (eventId == ATOMX_TWCommandEvent::eventTypeId ())
	{
		ATOMX_TWCommandEvent *e = (ATOMX_TWCommandEvent*)event;
		switch (e->id)
		{
		case PARAM_ID_ADDLEVEL:
			{
				if (_treeData.levels < 4)
				{
					_treeData.levels++;
					_treeData.level[_treeData.levels - 1].levelNumber = _treeData.levels - 1;
					_treeData.level[_treeData.levels - 1].branchDist = 1.f;
					_treeData.level[_treeData.levels - 1].branches = _treeData.levels * 5;
					_treeData.level[_treeData.levels - 1].curve = 0.f;
					_treeData.level[_treeData.levels - 1].curveBack = 0.f;
					_treeData.level[_treeData.levels - 1].curveRes = 6.f;
					_treeData.level[_treeData.levels - 1].curveV = 0.f;
					_treeData.level[_treeData.levels - 1].downAngle = 60.f;
					_treeData.level[_treeData.levels - 1].downAngleV = 30.f;
					_treeData.level[_treeData.levels - 1].length = 1.f;
					_treeData.level[_treeData.levels - 1].lengthV = 0.f;
					_treeData.level[_treeData.levels - 1].rotate = 0.f;
					_treeData.level[_treeData.levels - 1].rotateV = 360.f;
					_treeData.level[_treeData.levels - 1].segSplits = 0.f;
					_treeData.level[_treeData.levels - 1].splitAngle = 0.f;
					_treeData.level[_treeData.levels - 1].splitAngleV = 0.f;
					_treeData.level[_treeData.levels - 1].taper = 1.f;

					ATOM_DELETE(_paramTweakBar);
					createParamTweakBar ();
					calcUILayout ();

					_treeDirty = true;
				}
				break;
			}
		case PARAM_ID_REMOVELEVEL:
			{
				int levels = _treeData.levels + 1;
				if (_treeData.levels > 1)
				{
					_treeData.levels--;

					ATOM_DELETE(_paramTweakBar);
					createParamTweakBar ();
					calcUILayout ();

					_treeDirty = true;
				}
				break;
			}
		case PARAM_ID_STEMTEXTURE:
			{
				unsigned n = _editor->getOpenImageFileNames (false, false);
				if ( n == 1)
				{
					const char *imageFileName = _editor->getOpenedImageFileName (0);
					_tree->getTrunk().setTextureFileName (imageFileName);
					_treeDirty = true;
				}
				break;
			}
		case PARAM_ID_LEAFTEXTURE:
			{
				unsigned n = _editor->getOpenImageFileNames (false, false);
				if ( n == 1)
				{
					const char *imageFileName = _editor->getOpenedImageFileName (0);
					_tree->getLeaves().setTextureFileName (imageFileName);
				}
				break;
			}
		case PARAM_ID_REGENERATE:
			{
				_tree->regenerateTrunk (true);
				_tree->regenerateLeaves (true);
				break;
			}
		case PARAM_ID_ADDLEAFCUSTER:
			{
				showClusters (true);

				ATOM_HARDREF(ATOM_ShapeNode) shape;
				shape->setType (ATOM_ShapeNode::SPHERE);
				shape->setMaterialFileName ("/materials/builtin/shape.mat");
				ATOM_Material *m = shape->getMaterial();
				if (m)
				{
					float randomR = (float)rand() / float(RAND_MAX);
					float randomG = (float)rand() / float(RAND_MAX);
					float randomB = (float)rand() / float(RAND_MAX);
					m->getParameterTable ()->setVector ("diffuseColor", ATOM_Vector4f(randomR, randomG, randomB, 1.f));
				}
				shape->setO2T (ATOM_Matrix4x4f::getScaleMatrix (ATOM_Vector3f(10.f,10.f,10.f)));
				_tree->appendChild (shape.get());

				selectSphereNode (shape.get());
				_selectedSphereNode->setDrawBoundingbox (false);
				beginTranslateSphere (_selectedSphereNode.get());
				_isEditSphereTransform = true;
				_tree->regenerateLeaves(false);

				break;
			}
		case PARAM_ID_TOGGLECLUSTER:
			{
				showClusters (!_clusterShown);
				break;
			}
		case PARAM_ID_TOGGLEBBOX:
			{
				_tree->setDrawBoundingbox (!_tree->getDrawBoundingbox() ? 1 : 0);
				break;
			}
		case PARAM_ID_EXPORT:
			{
				ATOM_FileDlg dlg(ATOM_FileDlg::MODE_SAVEFILE|ATOM_FileDlg::MODE_OVERWRITEPROMPT, NULL, "atr", NULL, "ATOM3D树模型(*.atr)|*.atr|", _editor->getRenderWindow()->getWindowInfo()->handle);
				if (dlg.doModal () == IDOK)
				{
					const char *filename = dlg.getSelectedFileName (0);
					if (!_tree->exportTreeModel (filename))
					{
						::MessageBoxA (_editor->getRenderWindow()->getWindowInfo()->handle, "导出失败!", "ATOM Studio", MB_OK|MB_ICONHAND);
					}
				}
				break;
			}
		case PARAM_ID_IMPORT_TRUNKMODEL:
			{
				ATOM_FileDlg dlg(0, NULL, "atr", NULL, "WaveFront Object(*.obj)|*.obj|3ds Max 3DS(*.3ds)|*.3ds|3ds Max ASE(*.ase)|*.ase|Collada(*.dae)|*.dae|Stanford Polygon Library(*.ply)|*.ply|AutoCAD DXF(*.dxf)|*.dxf|LightWave(*.lwo)|*.lwo|Modo(*.lxo)|*.lxo|TrueSpace(*.cob)|*.cob|Valve Model(*.smd)|*.smd|DirectX X(*.x)|*.x|Ogre Mesh(*.mesh)|*.mesh|Ogre XML(*.xml)|*.xml|Irrlicht Mesh(*.irrmesh)|*.irrmesh|", _editor->getRenderWindow()->getWindowInfo()->handle);
				if (dlg.doModal () == IDOK)
				{
					const char *filename = dlg.getSelectedFileName (0);
					importTrunkModel (filename);
				}
				break;
			}
		default:
			break;
		};
	}
	else if (eventId == ATOM_WidgetResizeEvent::eventTypeId())
	{
		calcUILayout ();

		_tree->invalidateBillboard ();
	}
}

bool PluginTree::saveFile (const char *filename)
{
	return _tree->save (filename);
}

unsigned PluginTree::getMinWindowWidth (void) const
{
	return 600;
}

unsigned PluginTree::getMinWindowHeight (void) const
{
	return 450;
}

bool PluginTree::isDocumentModified (void)
{
	return _editor->isDocumentModified ();
}

void PluginTree::createViewTweakBar (void)
{
	_viewTweakBar = ATOM_NEW(ATOMX_TweakBar, "View Parameters");
	_viewTweakBar->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
	_viewTweakBar->setBarMovable (false);
	_viewTweakBar->setBarResizable (false);
	_viewTweakBar->setBarIconifiable (false);

	_viewTweakBar->addImmediateVariable ("Trunk triangles", ATOMX_TBTYPE_UINT32, &_trunkTriangleCount, true, "Statistics");
	_viewTweakBar->addImmediateVariable ("Leaf triangles", ATOMX_TBTYPE_UINT32, &_leafTriangleCount, true, "Statistics");
	_viewTweakBar->addImmediateVariable ("FPS", ATOMX_TBTYPE_FLOAT, &_FPS, true, "Statistics");

	_viewTweakBar->addButton ("Toggle cluster", PARAM_ID_TOGGLECLUSTER, "Toggle cluster", "View");
	_viewTweakBar->addButton ("Toggle BBox", PARAM_ID_TOGGLEBBOX, "Toggle BBox", "View");
	_viewTweakBar->addEnum ("View mode", PARAM_ID_TOGGLEVIEWMODE, _tree->getRenderMode(), false, "View", 0, "Auto", TreeNode::DRAW_AUTO, "3D", TreeNode::DRAW_NORMAL, "Billboard", TreeNode::DRAW_BILLBOARD, 0);
}

void PluginTree::createParamTweakBar (void)
{
	_paramTweakBar = ATOM_NEW(ATOMX_TweakBar, "Tree Parameters");
	_paramTweakBar->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
	_paramTweakBar->setBarMovable (false);
	_paramTweakBar->setBarResizable (false);
	_paramTweakBar->setBarIconifiable (false);

	_paramTweakBar->addButton ("Add level", PARAM_ID_ADDLEVEL, "Add level", "Tree Operations");
	_paramTweakBar->addButton ("Remove level", PARAM_ID_REMOVELEVEL, "Remove level", "Tree Operations");
	_paramTweakBar->addButton ("Add cluster", PARAM_ID_ADDLEAFCUSTER, "Add leaf cluster", "Tree Operations");
	_paramTweakBar->addButton ("Stem texture", PARAM_ID_STEMTEXTURE, "Stem texture..", "Tree Operations");
	_paramTweakBar->addButton ("Leaf texture", PARAM_ID_LEAFTEXTURE, "Leaf texture..", "Tree Operations");
	_paramTweakBar->addButton ("Regenerate", PARAM_ID_REGENERATE, "Regenerate", "Tree Operations");
	_paramTweakBar->addButton ("Export", PARAM_ID_EXPORT, "Export..", "Tree Operations");
	_paramTweakBar->addButton ("Import model", PARAM_ID_IMPORT_TRUNKMODEL, "Import model..", "Tree Operations");
	_paramTweakBar->addImmediateVariable ("External Trunk", ATOMX_TBTYPE_BOOL, &_drawExternTrunk, false, "Tree Operations");
	_paramTweakBar->addImmediateVariable ("Native Trunk", ATOMX_TBTYPE_BOOL, &_drawNativeTrunk, false, "Tree Operations");
	_paramTweakBar->addImmediateVariable ("Leaves", ATOMX_TBTYPE_BOOL, &_drawLeaves, false, "Tree Operations");

	_paramTweakBar->addImmediateVariable ("Width", ATOMX_TBTYPE_FLOAT, &_desiredLeafParam._leafWidth, false, "Leaf Params");
	_paramTweakBar->addImmediateVariable ("Height", ATOMX_TBTYPE_FLOAT, &_desiredLeafParam._leafHeight, false, "Leaf Params");
	_paramTweakBar->addImmediateVariable ("SizeV", ATOMX_TBTYPE_FLOAT, &_desiredLeafParam._leafSizeV, false, "Leaf Params");
	_paramTweakBar->addImmediateVariable ("Density", ATOMX_TBTYPE_UINT32, &_desiredLeafParam._numLeafCards, false, "Leaf Params");
	_paramTweakBar->addImmediateVariable ("RandomRange", ATOMX_TBTYPE_FLOAT, &_desiredLeafParam._leafRandomRange, false, "Leaf Params");
	_paramTweakBar->addImmediateVariable ("Flip", ATOMX_TBTYPE_BOOL, &_desiredLeafParam._flipLeaves, false, "Leaf Params");

	_paramTweakBar->addImmediateVariable ("trunkScale", ATOMX_TBTYPE_FLOAT, &_treeData.trunk.scale, false, "Trunk Params");
	_paramTweakBar->setVarLabel ("trunkScale", "Scale");
	_paramTweakBar->addImmediateVariable ("trunkScaleV", ATOMX_TBTYPE_FLOAT, &_treeData.trunk.scaleV, false, "Trunk Params");
	_paramTweakBar->setVarLabel ("trunkScaleV", "ScaleV");
	_paramTweakBar->addImmediateVariable ("baseSplits", ATOMX_TBTYPE_FLOAT, &_treeData.trunk.baseSplits, false, "Trunk Params");
	_paramTweakBar->addImmediateVariable ("dist", ATOMX_TBTYPE_FLOAT, &_treeData.trunk.dist, false, "Trunk Params");

	if (_treeData.levels > 0)
	{
		_paramTweakBar->addImmediateVariable ("downAngle_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].downAngle, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("downAngle_0", "downAngle");
		_paramTweakBar->addImmediateVariable ("downAngleV_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].downAngleV, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("downAngleV_0", "downAngleV");
		_paramTweakBar->addImmediateVariable ("rotate_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].rotate, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("rotate_0", "rotate");
		_paramTweakBar->addImmediateVariable ("rotateV_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].rotateV, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("rotateV_0", "rotateV");
		_paramTweakBar->addImmediateVariable ("branches_0", ATOMX_TBTYPE_INT32, &_treeData.level[0].branches, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("branches_0", "branches");
		_paramTweakBar->addImmediateVariable ("branchDist_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].branchDist, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("branchDist_0", "branchDist");
		_paramTweakBar->addImmediateVariable ("length_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].length, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("length_0", "length");
		_paramTweakBar->addImmediateVariable ("lengthV_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].lengthV, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("lengthV_0", "lengthV");
		_paramTweakBar->addImmediateVariable ("taper_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].taper, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("taper_0", "taper");
		//_paramTweakBar->addImmediateVariable ("segSplits_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].segSplits, false, "Level0 Params");
		//_paramTweakBar->setVarLabel ("segSplits_0", "segSplits");
		_paramTweakBar->addImmediateVariable ("splitAngle_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].splitAngle, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("splitAngle_0", "splitAngle");
		_paramTweakBar->addImmediateVariable ("splitAngleV_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].splitAngleV, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("splitAngleV_0", "splitAngleV");
		_paramTweakBar->addImmediateVariable ("curveRes_0", ATOMX_TBTYPE_INT32, &_treeData.level[0].curveRes, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("curveRes_0", "curveRes");
		_paramTweakBar->addImmediateVariable ("curve_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].curve, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("curve_0", "curve");
		_paramTweakBar->addImmediateVariable ("curveBack_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].curveBack, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("curveBack_0", "curveBack");
		_paramTweakBar->addImmediateVariable ("curveV_0", ATOMX_TBTYPE_FLOAT, &_treeData.level[0].curveV, false, "Level0 Params");
		_paramTweakBar->setVarLabel ("curveV_0", "curveV");
	}

	if (_treeData.levels > 1)
	{
		_paramTweakBar->addImmediateVariable ("downAngle_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].downAngle, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("downAngle_1", "downAngle");
		_paramTweakBar->addImmediateVariable ("downAngleV_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].downAngleV, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("downAngleV_1", "downAngleV");
		_paramTweakBar->addImmediateVariable ("rotate_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].rotate, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("rotate_1", "rotate");
		_paramTweakBar->addImmediateVariable ("rotateV_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].rotateV, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("rotateV_1", "rotateV");
		_paramTweakBar->addImmediateVariable ("branches_1", ATOMX_TBTYPE_INT32, &_treeData.level[1].branches, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("branches_1", "branches");
		_paramTweakBar->addImmediateVariable ("branchDist_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].branchDist, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("branchDist_1", "branchDist");
		_paramTweakBar->addImmediateVariable ("length_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].length, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("length_1", "length");
		_paramTweakBar->addImmediateVariable ("lengthV_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].lengthV, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("lengthV_1", "lengthV");
		_paramTweakBar->addImmediateVariable ("taper_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].taper, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("taper_1", "taper");
		//_paramTweakBar->addImmediateVariable ("segSplits_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].segSplits, false, "Level1 Params");
		//_paramTweakBar->setVarLabel ("segSplits_1", "segSplits");
		_paramTweakBar->addImmediateVariable ("splitAngle_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].splitAngle, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("splitAngle_1", "splitAngle");
		_paramTweakBar->addImmediateVariable ("splitAngleV_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].splitAngleV, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("splitAngleV_1", "splitAngleV");
		_paramTweakBar->addImmediateVariable ("curveRes_1", ATOMX_TBTYPE_INT32, &_treeData.level[1].curveRes, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("curveRes_1", "curveRes");
		_paramTweakBar->addImmediateVariable ("curve_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].curve, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("curve_1", "curve");
		_paramTweakBar->addImmediateVariable ("curveBack_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].curveBack, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("curveBack_1", "curveBack");
		_paramTweakBar->addImmediateVariable ("curveV_1", ATOMX_TBTYPE_FLOAT, &_treeData.level[1].curveV, false, "Level1 Params");
		_paramTweakBar->setVarLabel ("curveV_1", "curveV");
	}

	if (_treeData.levels > 2)
	{
		_paramTweakBar->addImmediateVariable ("downAngle_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].downAngle, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("downAngle_2", "downAngle");
		_paramTweakBar->addImmediateVariable ("downAngleV_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].downAngleV, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("downAngleV_2", "downAngleV");
		_paramTweakBar->addImmediateVariable ("rotate_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].rotate, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("rotate_2", "rotate");
		_paramTweakBar->addImmediateVariable ("rotateV_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].rotateV, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("rotateV_2", "rotateV");
		_paramTweakBar->addImmediateVariable ("branches_2", ATOMX_TBTYPE_INT32, &_treeData.level[2].branches, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("branches_2", "branches");
		_paramTweakBar->addImmediateVariable ("branchDist_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].branchDist, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("branchDist_2", "branchDist");
		_paramTweakBar->addImmediateVariable ("length_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].length, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("length_2", "length");
		_paramTweakBar->addImmediateVariable ("lengthV_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].lengthV, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("lengthV_2", "lengthV");
		_paramTweakBar->addImmediateVariable ("taper_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].taper, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("taper_2", "taper");
		//_paramTweakBar->addImmediateVariable ("segSplits_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].segSplits, false, "Level2 Params");
		//_paramTweakBar->setVarLabel ("segSplits_2", "segSplits");
		_paramTweakBar->addImmediateVariable ("splitAngle_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].splitAngle, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("splitAngle_2", "splitAngle");
		_paramTweakBar->addImmediateVariable ("splitAngleV_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].splitAngleV, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("splitAngleV_2", "splitAngleV");
		_paramTweakBar->addImmediateVariable ("curveRes_2", ATOMX_TBTYPE_INT32, &_treeData.level[2].curveRes, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("curveRes_2", "curveRes");
		_paramTweakBar->addImmediateVariable ("curve_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].curve, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("curve_2", "curve");
		_paramTweakBar->addImmediateVariable ("curveBack_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].curveBack, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("curveBack_2", "curveBack");
		_paramTweakBar->addImmediateVariable ("curveV_2", ATOMX_TBTYPE_FLOAT, &_treeData.level[2].curveV, false, "Level2 Params");
		_paramTweakBar->setVarLabel ("curveV_2", "curveV");
	}

	if (_treeData.levels > 3)
	{
		_paramTweakBar->addImmediateVariable ("downAngle_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].downAngle, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("downAngle_3", "downAngle");
		_paramTweakBar->addImmediateVariable ("downAngleV_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].downAngleV, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("downAngleV_3", "downAngleV");
		_paramTweakBar->addImmediateVariable ("rotate_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].rotate, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("rotate_3", "rotate");
		_paramTweakBar->addImmediateVariable ("rotateV_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].rotateV, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("rotateV_3", "rotateV");
		_paramTweakBar->addImmediateVariable ("branches_3", ATOMX_TBTYPE_INT32, &_treeData.level[3].branches, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("branches_3", "branches");
		_paramTweakBar->addImmediateVariable ("branchDist_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].branchDist, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("branchDist_3", "branchDist");
		_paramTweakBar->addImmediateVariable ("length_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].length, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("length_3", "length");
		_paramTweakBar->addImmediateVariable ("lengthV_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].lengthV, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("lengthV_3", "lengthV");
		_paramTweakBar->addImmediateVariable ("taper_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].taper, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("taper_3", "taper");
		//_paramTweakBar->addImmediateVariable ("segSplits_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].segSplits, false, "Level3 Params");
		//_paramTweakBar->setVarLabel ("segSplits_3", "segSplits");
		_paramTweakBar->addImmediateVariable ("splitAngle_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].splitAngle, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("splitAngle_3", "splitAngle");
		_paramTweakBar->addImmediateVariable ("splitAngleV_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].splitAngleV, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("splitAngleV_3", "splitAngleV");
		_paramTweakBar->addImmediateVariable ("curveRes_3", ATOMX_TBTYPE_INT32, &_treeData.level[3].curveRes, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("curveRes_3", "curveRes");
		_paramTweakBar->addImmediateVariable ("curve_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].curve, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("curve_3", "curve");
		_paramTweakBar->addImmediateVariable ("curveBack_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].curveBack, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("curveBack_3", "curveBack");
		_paramTweakBar->addImmediateVariable ("curveV_3", ATOMX_TBTYPE_FLOAT, &_treeData.level[3].curveV, false, "Level3 Params");
		_paramTweakBar->setVarLabel ("curveV_3", "curveV");
	}

	if (_treeData.levels > 4)
	{
		_paramTweakBar->addImmediateVariable ("downAngle_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].downAngle, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("downAngle_4", "downAngle");
		_paramTweakBar->addImmediateVariable ("downAngleV_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].downAngleV, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("downAngleV_4", "downAngleV");
		_paramTweakBar->addImmediateVariable ("rotate_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].rotate, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("rotate_4", "rotate");
		_paramTweakBar->addImmediateVariable ("rotateV_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].rotateV, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("rotateV_4", "rotateV");
		_paramTweakBar->addImmediateVariable ("branches_4", ATOMX_TBTYPE_INT32, &_treeData.level[4].branches, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("branches_4", "branches");
		_paramTweakBar->addImmediateVariable ("branchDist_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].branchDist, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("branchDist_4", "branchDist");
		_paramTweakBar->addImmediateVariable ("length_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].length, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("length_4", "length");
		_paramTweakBar->addImmediateVariable ("lengthV_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].lengthV, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("lengthV_4", "lengthV");
		_paramTweakBar->addImmediateVariable ("taper_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].taper, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("taper_4", "taper");
		//_paramTweakBar->addImmediateVariable ("segSplits_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].segSplits, false, "Level4 Params");
		//_paramTweakBar->setVarLabel ("segSplits_4", "segSplits");
		_paramTweakBar->addImmediateVariable ("splitAngle_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].splitAngle, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("splitAngle_4", "splitAngle");
		_paramTweakBar->addImmediateVariable ("splitAngleV_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].splitAngleV, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("splitAngleV_4", "splitAngleV");
		_paramTweakBar->addImmediateVariable ("curveRes_4", ATOMX_TBTYPE_INT32, &_treeData.level[4].curveRes, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("curveRes_4", "curveRes");
		_paramTweakBar->addImmediateVariable ("curve_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].curve, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("curve_4", "curve");
		_paramTweakBar->addImmediateVariable ("curveBack_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].curveBack, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("curveBack_4", "curveBack");
		_paramTweakBar->addImmediateVariable ("curveV_4", ATOMX_TBTYPE_FLOAT, &_treeData.level[4].curveV, false, "Level4 Params");
		_paramTweakBar->setVarLabel ("curveV_4", "curveV");
	}

	_paramTweakBar->addEnum ("shape", PARAM_ID_SHAPE, _treeData.shape, false, "Tree Params", 0, "Conical", TD_CONICAL, "Spherical", TD_SPHERICAL, "HemiSpherical", TD_HEMISPHERICAL, "Cylindrical", TD_CYLINDRICAL, "TaperedCylindrical", TD_TAPERED_CYLINDRICAL, "Flame", TD_FLAME, "InverseConical", TD_INVERSE_CONICAL, "TendFlame", TD_TEND_FLAME, "Envelope", TD_ENVELOPE, 0);
	_paramTweakBar->addImmediateVariable ("baseSize", ATOMX_TBTYPE_FLOAT, &_treeData.baseSize, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("scale", ATOMX_TBTYPE_FLOAT, &_treeData.scale, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("scaleV", ATOMX_TBTYPE_FLOAT, &_treeData.scaleV, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("ratio", ATOMX_TBTYPE_FLOAT, &_treeData.ratio, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("ratioPower", ATOMX_TBTYPE_FLOAT, &_treeData.ratioPower, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("lobes", ATOMX_TBTYPE_INT32, &_treeData.lobes, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("lobeDepth", ATOMX_TBTYPE_FLOAT, &_treeData.lobeDepth, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("flare", ATOMX_TBTYPE_FLOAT, &_treeData.flare, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("attractionUp", ATOMX_TBTYPE_FLOAT, &_treeData.attractionUp, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("pruneRatio", ATOMX_TBTYPE_FLOAT, &_treeData.pruneRatio, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("prunePowerLow", ATOMX_TBTYPE_FLOAT, &_treeData.prunePowerLow, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("prunePowerHigh", ATOMX_TBTYPE_FLOAT, &_treeData.prunePowerHigh, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("pruneWidth", ATOMX_TBTYPE_FLOAT, &_treeData.pruneWidth, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("pruneWidthPeak", ATOMX_TBTYPE_FLOAT, &_treeData.pruneWidthPeak, false, "Tree Params");
	_paramTweakBar->addImmediateVariable ("LOD distance", ATOMX_TBTYPE_FLOAT, &_lodDistance, false, "Tree Operations");
}

void PluginTree::calcUILayout (void)
{
	ATOM_Rect2Di rc = _editor->getRealtimeCtrl()->getClientRect();
	ATOM_Point2Di pt(rc.size.w - 220, 0);
	_editor->getRealtimeCtrl()->clientToGUI (&pt);

	_viewTweakBar->setBarPosition (pt.x, pt.y);
	_viewTweakBar->setBarSize (220, 200);

	_paramTweakBar->setBarPosition (pt.x, pt.y + 200);
	_paramTweakBar->setBarSize (220, rc.size.h - 200);
}

void PluginTree::beginTranslateSphere (ATOM_Node *node)
{
	_editor->beginEditNodeTranslation (node);
}

void PluginTree::beginScaleSphere (ATOM_Node *node)
{
	_editor->beginEditNodeScaling (node, true);
}

void PluginTree::endEditSphereTransform (ATOM_Node *node)
{
	_editor->endEditNodeTransform ();
	_tree->regenerateLeaves (false);
}

void PluginTree::showClusters (bool show)
{
	for (unsigned i = 0; i < _tree->getNumChildren (); ++i)
	{
		_tree->getChild(i)->setShow (show ? ATOM_Node::SHOW : ATOM_Node::HIDE);
	}
	_clusterShown = show;
}

bool PluginTree::importTrunkModel (const char *filename)
{
	if (_tree->getTrunk().setAlternateModel (filename))
	{
		_tree->getTrunk().setDrawAlternate(true);
		return true;
	}
	return false;
}

void PluginTree::handleTransformEdited (ATOM_Node *node)
{
}

void PluginTree::handleScenePropChanged (void)
{
}

void PluginTree::changeRenderScheme (void)
{
	_scene->setRenderScheme (_editor->getRenderScheme());
}

#endif
