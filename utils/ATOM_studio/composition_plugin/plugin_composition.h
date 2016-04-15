#ifndef __ATOM3D_STUDIO_COMPOSITION_PLUGIN_H
#define __ATOM3D_STUDIO_COMPOSITION_PLUGIN_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "editor.h"

#define CB_TYPE_COMPOSITION_ACTOR	"CompositionActor"

class TimeLine;
class PluginComposition: public AS_Plugin
{
public:
//%%BeginIDList
	enum {
		ID_ACTORCELL = 102,
		ID_ACTORTREE = 103,
	};
//%%EndIDList
public:
	PluginComposition (void);

public:
	virtual ~PluginComposition (void);
	virtual unsigned getVersion (void) const;
	virtual const char *getName (void) const;
	virtual void deleteMe (void);
	virtual bool initPlugin (AS_Editor *editor);
	virtual void donePlugin (void);
	virtual bool beginEdit (const char *filename);
	virtual void endEdit (void);
	virtual void frameUpdate (void);
	virtual void handleEvent (ATOM_Event *event);
	virtual bool saveFile (const char *filename);
	virtual unsigned getMinWindowWidth (void) const;
	virtual unsigned getMinWindowHeight (void) const;
	virtual bool isDocumentModified (void);
	virtual void handleTransformEdited (ATOM_Node *node);
	virtual void handleScenePropChanged (void);
	virtual void changeRenderScheme (void);

private:
	bool initGUI (void);
	void calcUILayout (void);
	void clear (void);
	void setupMenu (void);
	void cleanupMenu (void);
	void refreshActorList (void);
	int refreshActorListT (ATOM_Node *node);
	void selectActorByName (const char *name);
	ATOM_CompositionActor *findActorByName (const char *name) const;
	const char *findActorByNode (ATOM_Node *node) const;
	bool updateActorMap (void);
	bool updateActorMapR (ATOM_CompositionActor *actor);
	void toggleGUI (void);
	void toggleGrid (void);
	void selectScene (void);
	void selectBkImage (void);
	void deleteCurrentActor (void);
	void renameCurrentActor (void);
	ATOM_STRING newName (void) const;
	bool isKeyPropertyPS (const char *name) const;
	bool isKeyPropertyGeode (const char *name) const;
	bool isKeyPropertyShape (const char *name) const;
	bool isKeyPropertyLight (const char *name) const;
	bool isKeyPropertyWeaponTrail (const char *name) const;
	bool isKeyPropertyTrail (const char *name) const;
	bool isKeyPropertyDecal (const char *name) const;
	void setupKeyframeBarPS (ATOM_CompositionActor *actor);
	void setupKeyframeBarGeode (ATOM_CompositionActor *actor);
	void setupKeyframeBarShape (ATOM_CompositionActor *actor);
	void setupKeyframeBarLight (ATOM_CompositionActor *actor);
	void setupKeyframeBarTrail (ATOM_CompositionActor *actor);
	void setupKeyframeBarWeaponTrail (ATOM_CompositionActor *actor);
	void setupKeyframeBarDecal (ATOM_CompositionActor *actor);
	void setupKeyframeBarTransform (ATOM_CompositionActor *actor);
	void updateKeyframeBarTransform (void);
	void setupActorPropertyBar (ATOM_CompositionActor *actor);
	void setupMaterialTweakBar (ATOMX_TweakBar *bar, ATOM_Material *material);
	bool exportCameraPath (const char *filename);
	void testCameraPath (void);

private:
//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_ActorCell = (ATOM_Cell*)parent->getChildByIdRecursive(ID_ACTORCELL);
		m_ActorTree = (ATOM_TreeCtrl*)parent->getChildByIdRecursive(ID_ACTORTREE);
	}
//%%EndInitFunc

private:
	AS_Editor *_editor;
	ATOM_DeferredScene *_scene;
	ATOM_AUTOREF(GridNode) _grid;
	ATOM_AUTOREF(ATOM_BkImage) _bkImage;
	ATOM_AUTOREF(ATOM_CompositionNode) _node;
	ATOM_Widget *_leftPanel;
	TimeLine *_timeLine;
	ATOMX_PropertyTweakBar *_actorPropertyBar;
	ATOMX_PropertyTweakBar *_keyValueBar;
	ATOM_Point2Di _createPosition;
	ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_CompositionActor)> _actorMap;
	ATOM_STRING _currentActor;
	ATOM_VECTOR<AS_AccelKey> _accelKeys;
	bool _guiShown;
	int _transformMode;

//%%BeginVarList
	ATOM_Cell *m_ActorCell;
	ATOM_TreeCtrl *m_ActorTree;
//%%EndVarList
};

#endif // __ATOM3D_STUDIO_COMPOSITION_PLUGIN_H
