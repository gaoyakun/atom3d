#ifndef __ATOM3D_STUDIO_SCENE_PLUGIN_H
#define __ATOM3D_STUDIO_SCENE_PLUGIN_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "editor.h"

class EditorRenderSchemeFactory;
class DlgHeightGen;
class DlgEditBlocks;
class DlgLoadNPC;
class DlgTerrainTools;
class DlgRegionEdit;
class DlgPathEditor;

class PluginScene: public AS_Plugin
{
	friend class DlgRegionEdit;

	enum EditMode
	{
		EM_SELECT,
		EM_MOVE,
		EM_ROTATE
	};

public:
	PluginScene (void);

public:
	virtual ~PluginScene (void);
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

public:
	void lookAtNode (ATOM_Node *node);
	void selectNode (ATOM_Node *node);
	ATOM_Node *getSelectedNode (void) const;
	void deleteSelectedNode (void);
	ATOM_Scene *getScene (void) const;
	void changeBlockType (int type);
	void clearBlocks (bool set);
	void clearBlockLessAngle (int angle, bool set);
	void clearBlockGreaterAngle (int angle, bool set);
	ATOM_Terrain *findTerrain (void) const;
	AS_Editor *getEditor (void) const;
	void beginAddRegion (ATOM_AUTOREF(ATOM_Decal) decal);
	void endAddRegion (ATOM_Decal *decal, bool cancel);
	void beginAddPath (ATOM_AUTOREF(ATOM_LineTrail) trail);
	void endAddPath (ATOM_LineTrail *trail, bool cancel);
	void refreshPathList (void);
	ATOM_Node *getSceneRoot (void) const;
	ATOM_ShapeNode *getCurrentHeightIndicator (void) const;
	bool showHeightGrid (bool show);

	//--- wangjian added ---//
	void selectPostEffect(ATOM_PostEffect* pe);
	//----------------------//

private:
	void clear (void);
	ATOM_BBox calcSceneBoundingbox (void);
	void createTweakBars (void);
	void createSceneGraphTree (ATOM_RealtimeCtrl *parent);
	void createPostEffectList (ATOM_RealtimeCtrl *parent);
	void calcUILayout (void);
	void beginCreateAsset (ATOM_Node *asset, bool replace);
	void endCreateAsset (bool cancel, bool localAppend);
	void updateAssetTransform (void);
	void updateRegionTransform (void);
	void updatePathTransform (void);
	void updateNodeTransformFromCursor (ATOM_Node *node);
	void updateNodeTransformFromCursorHF (ATOM_Node *node);
	void reparentNode (ATOM_Node *childNode, ATOM_Node *parentNode);
	void cloneSelectedNode (void);
	bool saveSelectedNode (const char *filename);
	void toggleLightNodeDisplay (void);
	void toggleGrid (void);
	//--- wangjian added ---//
	void toggleSceneNode(bool bTerrain=false);
	void setPickFilterType(ATOM_STRING filterClassName);
	bool checkPickFilterType(ATOM_STRING strFilterClassName);
	void getCubeMapVector( unsigned face, ATOM_Vector3f & vLookDir, ATOM_Vector3f & vUpDir );
	void packDepthEnvMap();
	void genEnvMap(ATOM_Vector3f cameraPos, float cullradius, unsigned cubeMapSize, bool bGlobalEnv=true,bool bSaveToFile=false, const char* filename = 0 );
	//----------------------//
	void toggleLockCamera (void);
	void alignCameraToSelectedNode (void);
	void applyCameraToSelectedNode (void);
	void toggleGUI (void);
	void checkDuplicatedGeodes (const char *filename) const;
	void setupMenu (void);
	void cleanupMenu (void);
	void addLightGeodeToScene (void);
	void detachDummyNodes (void);
	void attachDummyNodes (void);
	void beginEditNodeTranslation (void);
	void beginEditNodeRotation (void);
	void beginEditNodeScale (void);
	void endEditNodeTransform (void);
	void startTimePreview (void);
	void endTimePreview (void);
	bool isHeightGridShown (void);
	void beginEditHeight (void);
	void endEditHeight (void);
	void beginEditTerrain (void);
	void endEditTerrain (void);
	void editColorGradingEffect (ATOM_ColorGradingEffect *effect);
	void generateTerrainShadowMap (void);
	void setupMaterialTweakBar (ATOMX_TweakBar *bar, ATOM_Material *material);
	void showHeightmapInfo (float x, float y, float w, float h);
	DlgHeightGen *getDlgHeightGen (void);
	DlgEditBlocks *getDlgEditBlocks (void);
	DlgLoadNPC *getDlgLoadNPC (void);
	DlgRegionEdit *getDlgRegionEdit (void);
	DlgPathEditor *getDlgPathEditor (void);

	static void ATOMX_CALL NodeSelectionEditCallback(void *);
	static void ATOMX_CALL NodeTranslationEditCallback(void *);
	static void ATOMX_CALL NodeRotationEditCallback(void *);
	static void ATOMX_CALL NodeScaleEditCallback(void *);
	static void ATOMX_CALL NodeDeleteCallback(void *);
	static void ATOMX_CALL NodeCloneCallback(void *);
	static void ATOMX_CALL NodeSaveCallback(void *);

private:
	ATOM_TreeCtrl *_tcSceneGraph;
	ATOM_ListBox *_postEffectList;
	ATOM_PopupMenu *_postEffectListMenu;
	ATOM_PopupMenu *_nodeMenu;
	ATOMX_PropertyTweakBar *_tbNodeProperties;
	ATOMX_TweakBar *_tbTimePreview;
	ATOM_DeferredScene *_scene;
	ATOM_AUTOREF(ATOM_Node) _selectedNode;
	ATOM_PopupMenu *_pickedNodeList;
	ATOM_Node *_cloneSrc;
	ATOM_PickVisitor _pickVisitor;
	EditMode _editMode;
	AS_Editor *_editor;
	DlgHeightGen *_dlgHeightGen;
	DlgEditBlocks *_dlgEditBlocks;
	DlgLoadNPC *_dlgLoadNPC;
	DlgTerrainTools *_dlgTerrainTools;
	DlgRegionEdit *_dlgRegionEdit;
	DlgPathEditor *_dlgPathEditor;
	int _editingHeightX;
	int _editingHeightY;
	bool _isEditingHeight;
	bool _isGUIShown;
	bool _isCreatingAsset;
	bool _replaceAsset;
	bool _displayLightGeode;
	//--- wangjian added ---//
	bool							_hideAllSceneNode;
	ATOM_PopupMenu *				_nodePickFilterMenu;
	ATOM_HASHSET<ATOM_STRING>		_pickFilterSet;
	ATOM_PopupMenu *				_genEnvMapMenu;
	ATOM_AUTOREF(ATOM_PostEffect)	_selectPostEffect;
	//----------------------//
	float _assetRotateX;
	float _assetRotateY;
	float _assetRotateZ;
	float _assetOffsetY;
	float _assetOffsetY2;
	int _savedTime;
	AS_CameraModal *_oldCameraModal;
	AS_CameraModal *_lockedCameraModal;
	ATOM_AUTOREF(ATOM_Node) _nodeToBeReplaced;
	ATOM_AUTOREF(ATOM_ShapeNode) _heightEditingPoint;
	ATOM_STRING _currentCreateAssetFileName;
	ATOM_STRING _currentCreateAssetName;
	ATOM_AUTOREF(ATOM_Node) _currentCreateAsset;
	ATOM_AUTOREF(GridNode) _grid;
	ATOM_AUTOREF(ATOM_Node) _sceneRoot;
	ATOM_AUTOREF(ATOM_Node) _sceneNPC;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> _focusedNode;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> _pickedNodes;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> _tempNodesParent;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> _tempNodesChild;
	ATOM_AUTOREF(ATOM_Decal) _currentRegion;
	ATOM_AUTOREF(ATOM_LineTrail) _currentPath;
	ATOM_AUTOREF(ATOM_ShapeNode) _heightModIndicator;
	ATOM_AUTOREF(ATOM_Decal) _heightModDecal;

	bool _beginCreatingRegion;
	bool _beginCreatePath;

	ATOM_Matrix4x4f _nodeTransformBeforeEdit;
	ATOM_VECTOR<AS_AccelKey> _accelKeys;

	EditorRenderSchemeFactory *_editorRenderSchemeFactory;
};

#endif // __ATOM3D_STUDIO_SCENE_PLUGIN_H
