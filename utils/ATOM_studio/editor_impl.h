#ifndef __ATOM3D_STUDIO_EDITOR_IMPL_H
#define __ATOM3D_STUDIO_EDITOR_IMPL_H

#if _MSC_VER > 1000
# pragma once
#endif

class AS_CameraModal;
class AS_Asset;
class AS_AssetManager;
class AS_Clipboard;
class AssetEditor;
class ColorGradingEditor;
class CurveEd;
class ScenePropEditor;

class LoadableAssetManager;
class LightAssetManager;
class DecalAssetManager;
class BkImageAssetManager;
class ShapeAssetManager;
class HudAssetManager;
class TerrainAssetManager;
class ParticleSysAssetManager;
class NodeAssetManager;
class WeaponTrailAssetManager;

struct AccelKeyInfo;

enum RenderMode
{
	RM_DEFERRED,
	RM_DEFERRED_NORMAL,
	RM_DEFERRED_COLOR,
	RM_DEFERRED_DEPTH,
	RM_DEFERRED_LIGHTBUFFER,
	RM_DEFERRED_SHADOWMAP,
	RM_DEFERRED_SHADOWMASK,
	//--- wangjian added ---//
	// * 用以调试GBUFFER RT中保存的HalfLambert.
	RM_DEFERRED_HALFLAMBERT,
	RM_HDR_LUMIN,
	RM_BLOOM,
	//----------------------//

};

enum SchemeMode
{
	SCHEME_DEFERRED,
	SCHEME_FORWARD
};

class EditorImpl;

class CapsViewCommandCallback: public ATOMX_TweakBar::CommandCallback
{
	EditorImpl *_editorImpl;

public:
	CapsViewCommandCallback (EditorImpl *editorImpl): _editorImpl(editorImpl)
	{
	}

	void callback (ATOMX_TWCommandEvent *event);
};

class EditorImpl: public AS_Editor
{
public:
	EditorImpl (void);
	virtual ~EditorImpl (void);

public:
	virtual unsigned getVersion (void) const;
	virtual ATOM_RenderWindow *getRenderWindow (void) const;
	virtual ATOM_GUIRenderer *getGUIRenderer (void) const;
	virtual ATOM_RealtimeCtrl *getRealtimeCtrl (void) const;
	virtual ATOM_MenuBar *getMenuBar (void) const;
	virtual AS_CameraModal *getCameraModal (void) const;
	virtual void setCameraModal (AS_CameraModal *cameraModal);
	virtual bool registerFileType (AS_Plugin *plugin, const char *ext, const char *desc, unsigned editFlags);
	virtual bool registerRenderManager (AS_RenderManager *manager, const char *name);
	virtual bool registerAssetManager (AS_AssetManager *manager, const char *name);
	virtual AS_Asset *createAsset (const char *name);
	virtual AS_Asset *createAssetByFileName (const char *filename);
	virtual unsigned getOpenAssetFileNames (bool multi, bool save, const char *title);
	virtual const char *getOpenedAssetFileName (unsigned index);
	virtual unsigned getOpenImageFileNames (bool multi, bool save, const char *title);
	virtual const char *getOpenedImageFileName (unsigned index);
	virtual unsigned getOpenFileNames (const char *ext, const char *filter, bool multi, bool save, const char *title);
	virtual const char *getOpenedFileName (unsigned index);
	virtual bool setRenderManager (const char *name);
	virtual const char *getRenderManager (void);
	virtual void setCapture (void);
	virtual void releaseCapture (void);
	virtual void showAssetEditor (bool show);
	virtual void setAssetEditorPosition (int x, int y, int w, int h);
	virtual void showScenePropEditor (bool show);
	virtual void setScenePropEditorPosition (int x, int y, int w, int h);
	virtual void setScenePropEditorTarget (ATOM_DeferredScene *scene);
	virtual void resetScenePropEditor (void);
	virtual void refreshScenePropEditor (void);
	virtual void showColorGradingEditor (bool show, ATOM_ColorGradingEffect *effect);
	virtual void handleEvent (ATOM_Event *event);
	virtual ATOM_Point2Di getMousePosition (void) const;
	virtual const char *getDocumentFileName (void) const;
	virtual void setDocumentModified (bool modified);
	virtual bool isDocumentModified (void) const;
	virtual void doEditOp (AS_Operation *op);
	virtual void doObjectAttribModifyOp (ATOM_Object *object, const char *propName, const ATOM_ScriptVar &oldValue, const ATOM_ScriptVar &newValue);
	virtual void doNodeTransformOp (ATOM_Node *node, const ATOM_Matrix4x4f &oldMatrix, const ATOM_Matrix4x4f &newMatrix);
	virtual void doNodeCreateOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent);
	virtual void doNodeDeleteOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent);
	virtual bool canUndo (void) const;
	virtual bool undo (void);
	virtual bool canRedo (void) const;
	virtual bool redo (void);
	virtual AS_AccelKey registerAccelKeyCommand (ATOM_Key key, int keymod, int commandId);
	virtual void unregisterAccelKeyCommand (AS_AccelKey accelKey);
	virtual void markDummyNodeType (const char *classname);
	virtual bool isDummyNodeType (const char *classname);
	virtual bool beginEditNodeTranslation (ATOM_Node *node);
	virtual bool beginEditNodeRotation (ATOM_Node *node);
	virtual bool beginEditNodeScaling (ATOM_Node *node, bool forceUniformScale);
	virtual bool isEditingNodeTranslation (void) const;
	virtual bool isEditingNodeRotation (void) const;
	virtual bool isEditingNodeScaling (void) const;
	virtual bool isEditingNodeTransform (void) const;
	virtual void endEditNodeTransform (void);
	virtual unsigned getMinWindowWidth (void) const;
	virtual unsigned getMinWindowHeight (void) const;
	virtual float measureScreenDistance (const ATOM_Vector3f &locationWorld, float distanceWorld) const;
	virtual void showNodeAxis (ATOM_Node *node, bool show, float minAxisLength);
	virtual void renderScene (void);
	virtual void lookAtNode (ATOM_Node *node);
	virtual int allocClipboardContentType (const char *name);
	virtual int getClipboardContentType (const char *name) const;
	virtual const char *getClipboardContentName (int type) const;
	virtual bool setClipboardData (int contentType, const void *data, unsigned dataSize);
	virtual bool setClipboardDataByName (const char *name, const void *data, unsigned dataSize);
	virtual unsigned getClipboardDataLength (void) const;
	virtual int getClipboardData (void *data) const;
	virtual void emptyClipboard (void);
	virtual bool showRenameDialog (ATOM_STRING &name);
	virtual bool showNewPropertyDialog (AS_PropertyInfo *info);

	// wangjian modified
#if 0
	virtual ATOM_RenderScheme *getRenderScheme (void) const;
#else
	virtual ATOM_RenderScheme *getRenderScheme (void);
#endif

public:
	struct PluginInfo
	{
		AS_Plugin *plugin;
		ATOM_STRING desc;
		ATOM_STRING ext;
		unsigned editFlags;
	};

	void init (ATOM_RenderWindow *renderWindow, ATOM_GUIRenderer *guiRenderer, ATOM_RealtimeCtrl *realtimeCtrl);
	ATOM_STRING buildFileFilterString (void) const;
	ATOM_STRING buildAssetFileFilterString (void) const;
	void newDocument (unsigned pluginInfoIndex);
	void openDocument (void);
	void openDocument (const char *filename, const char *desc);
	bool saveDocument (void);
	bool saveDocumentAs (void);
	void closeDocument (void);
	bool isDocumentEditing (void);
	bool exit (void);
	void frameUpdate (void);
	void resize (void);
	void setMousePosition (int x, int y);
	void updateStudioCaption (bool modified);
	bool checkAndFireAccelKeyCommand (ATOM_Key key, int keymod);
	void addPlugin (AS_Plugin *plugin);
	void doExit (void);
	const ATOM_VECTOR<PluginInfo> &getPluginList (void) const;
	const ATOM_HASHMAP<ATOM_STRING, AS_AssetManager*> &getAssetManagerMap (void) const;
	void render (void);
	void setRenderMode (RenderMode mode);
	void setSchemeMode (SchemeMode mode);
	void toggleWireFrame (void);
	void showD3DCaps (void);
	void editCamera (void);
	void initAssets (void);
	void initMenuStates (ATOM_MenuPopupEvent *event);
	void emergencySave (void);
	void showCurveEditor (bool show);
	void showCameraParams (void);

private:
	void clearUndoList (void);
	void checkForWindowSize (AS_Plugin *plugin);
	void createAxisNode (void);
	void updateNodeAxis (void);
	void updateAxises (void);
	void createTrackBallNode (void);
	void updateNodeTrackball (void);
	bool onTranslationEditLButtonDown (int x, int y);
	void onTranslationEditLButtonUp (void);
	bool isTranslating (void) const;
	bool onScaleEditLButtonDown (int x, int y, bool uniformScale);
	void onScaleEditLButtonUp (void);
	bool isScaling (void) const;
	bool onRotationEditLButtonDown (int x, int y);
	void onRotationEditLButtonUp (void);
	bool isRotating (void) const;
	bool needPromptForSave (void) const;
	ATOM_RenderScheme *getRenderSchemeDeferred (void) const;
	ATOM_RenderScheme *getRenderSchemeForward (void) const;

private:
	ATOM_AUTOREF(ATOM_RenderWindow) _renderWindow;
	ATOM_GUIRenderer *_guiRenderer;
	ATOM_RealtimeCtrl *_realtimeCtrl;
	ATOM_VECTOR<AS_Plugin*> _plugins;
	ATOM_VECTOR<PluginInfo> _pluginInfos;
	ATOM_HASHMAP<ATOM_STRING, AS_RenderManager*> _renderManagerMap;
	ATOM_HASHMAP<ATOM_STRING, AS_AssetManager*> _assetManagerMap;
	ATOM_VECTOR<ATOM_STRING> _openedAssetFileNames;
	ATOM_VECTOR<ATOM_STRING> _openedImageFileNames;
	ATOM_VECTOR<ATOM_STRING> _openedFileNames;
	const char *_currentRenderManagerName;
	AssetEditor *_assetEditor;
	ScenePropEditor *_scenePropEditor;
	ColorGradingEditor *_colorGradingEditor;
	int _captureCounter;
	AS_Clipboard *_clipboard; 
	AS_Plugin *_currentEditingPlugin;
	AS_CameraModal *_cameraModal;
	AS_CameraModal *_defaultCameraModal;
	LoadableAssetManager *_loadableAssetManager;
	LightAssetManager *_lightAssetManager;
	DecalAssetManager *_decalAssetManager;
	BkImageAssetManager *_bkImageAssetManager;
	ShapeAssetManager *_shapeAssetManager;
	HudAssetManager *_hudAssetManager;
	TerrainAssetManager *_terrainAssetManager;
	ParticleSysAssetManager *_particleSysAssetManager;
	NodeAssetManager *_nodeAssetManager;
	WeaponTrailAssetManager *_weaponTrailAssetManager;
	ATOMX_TweakBar *_D3DCapsView;
	ATOMX_TweakBar *_cameraOptionView;
	CurveEd *_curveEditor;
	float _cameraFOV;
	float _cameraNear;
	float _cameraFar;
	ATOM_Vector3f _cameraPosition;
	CapsViewCommandCallback *_CapsViewCommandCallback;

	int _currentMouseX;
	int _currentMouseY;
	ATOM_STRING _currentFile;
	ATOM_STRING _currentExt;
	ATOM_STRING _currentDesc;
	bool _documentModified;

	RenderMode _renderMode;
	SchemeMode _schemeMode;

	ATOM_VECTOR<AS_Operation*> _editOps;
	unsigned _currentOp;

	ATOM_VECTOR<AccelKeyInfo*> _accelKeys;
	ATOM_HASHSET<ATOM_STRING> _dummyNodeTypes;

	ATOM_AUTOREF(ATOMX_AxisNode) _axis;
	ATOM_AUTOREF(ATOMX_TrackBallNode) _trackball;
	ATOM_AUTOREF(ATOM_Node) _editingNode;
	bool _isEditingNodeTranslation;
	bool _isEditingNodeRotation;
	bool _isEditingNodeScaling;
	bool _isUniformScaling;
	bool _forceUniformScale;
	int _axisType;

	ATOM_Vector3f _trackballPoint;
	ATOM_Vector3f _projectPlane;
	ATOM_Vector3f _projectPoint;
	ATOM_Vector3f _lastRotatePoint;
	ATOM_Matrix4x4f _transformBeforeEdit;
	ATOM_Matrix4x4f _transformBeforeEditInv;
	ATOM_Matrix4x4f _savedNodeWorldMatrix;
	ATOM_Matrix4x4f _savedNodeO2T;

	struct AxisNodeInfo
	{
		ATOM_AUTOREF(ATOM_Node) node;
		ATOM_AUTOREF(ATOMX_AxisNode) axis;
		float minLength;
	};
	ATOM_VECTOR<AxisNodeInfo> _nodesWithAxis;

	mutable ATOM_DeferredRenderScheme *_renderSchemeDeferred;
	mutable ATOM_CustomRenderScheme *_renderSchemeForward;
};

#endif // __ATOM3D_STUDIO_EDITOR_IMPL_H