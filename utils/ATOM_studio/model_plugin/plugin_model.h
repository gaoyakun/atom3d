#ifndef __ATOM3D_STUDIO_MODEL_PLUGIN_H
#define __ATOM3D_STUDIO_MODEL_PLUGIN_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "editor.h"
#include "gridnode.h"

class ModelPropEditor;

class PluginModel: public AS_Plugin
{
public:
	PluginModel (void);

public:
	virtual ~PluginModel (void);
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
	void clear (void);
	void createTweakBars (void);
	void createMeshList (ATOM_RealtimeCtrl *parent);
	void createTrackList (ATOM_RealtimeCtrl *parent);
	void createPointList (ATOM_RealtimeCtrl *parent);
	void calcUILayout (void);
	void setupMenu (void);
	void cleanupMenu (void);
	void toggleGUI (void);
	void toggleGrid (void);
	void toggleBBox (void);
	void selectScene (void);
	void selectBkImage (void);
	ATOM_STRING generatePointName (void);
	void newPointShape (const char *name, const ATOM_Matrix4x4f &matrix);
	void refreshPointList (void);
	void renameCurrentPoint (void);
	void setCurrentPointToCamera (void);
	void setCameraToCurrentPoint (void);
	void deleteCurrentPoint (void);
	void clearPoints (void);
	bool transformModel (const ATOM_Matrix4x4f &matrix);
	void beginTranslateModel (void);
	void beginRotateModel (void);
	void beginScaleModel (void);
	void endTransformModel (void);
	void exportAvatar (const char *filename);

	//------------------------------------------------------------------------//
	// wangjian added 
	void genMeshCurvatureMap( ATOM_SharedMesh * mesh, const char* filename );
	bool saveFileIsolation (const char *filename);
	void resetSharedMaterials();
	//------------------------------------------------------------------------//

private:
	ATOM_DeferredScene *_scene;
	ATOM_AUTOREF(GridNode) _grid;
	ATOM_AUTOREF(ATOM_Geode) _geode;
	ATOM_AUTOREF(ATOM_BkImage) _bkImage;
	ATOM_AUTOREF(ATOM_LightNode) _lightNode[3];
	unsigned _lightUpdateStamp;
	float _lightPositionAngle;
	bool _editingPoint;
	bool _editingModel;
	ATOM_STRING _modelFileName;
	ATOM_ListBox *_meshList;
	ATOM_ListBox *_trackList;
	ATOM_ListBox *_pointList;
	ATOM_PopupMenu *_pointListMenu;
	ATOM_PopupMenu *_meshListMenu;
	ModelPropEditor *_edModelProp;
	ATOM_AUTOREF(ATOM_SharedModel) _model;
	ATOM_AUTOREF(ATOM_SharedModel) _skeleton;
	ATOM_AUTOREF(ATOM_SharedModel) _action;
	bool _isGUIShown;
	bool _readonlyMode;
	AS_Editor *_editor;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Material)> _realMaterials;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Material)> _sharedMaterials;
	ATOM_VECTOR<AS_AccelKey> _accelKeys;

	ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)> _points;
};

#endif // __ATOM3D_STUDIO_MODEL_PLUGIN_H
