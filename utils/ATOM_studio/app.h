#ifndef __ATOM_STUDIO_APP_H
#define __ATOM_STUDIO_APP_H

#if _MSC_VER > 1000
# pragma once
#endif

enum
{
	MENUID_OPEN,
	MENUID_CLOSE,
	MENUID_SAVE,
	MENUID_SAVEAS,
	MENUID_CHOOSE_VFSROOT,
	MENUID_EXIT,

	MENUID_UNDO,
	MENUID_REDO,
	MENUID_COPY,
	MENUID_CUT,
	MENUID_PASTE,

	MENUID_CAMERA_PARAMS,
	MENUID_TIMESCALE_32,
	MENUID_TIMESCALE_16,
	MENUID_TIMESCALE_8,
	MENUID_TIMESCALE_4,
	MENUID_TIMESCALE_2,
	MENUID_TIMESCALE_1,
	MENUID_TIMESCALE_1_2,
	MENUID_TIMESCALE_1_4,
	MENUID_TIMESCALE_1_8,
	MENUID_TIMESCALE_1_16,
	MENUID_TIMESCALE_1_32,
	MENUID_RENDER_SCHEME_DEFERRED,
	MENUID_RENDER_SCHEME_FORWARD,
	MENUID_DEFERRED_RENDERING,
	MENUID_DEFERRED_RENDERING_SHOWNORMALS,
	MENUID_DEFERRED_RENDERING_SHOWCOLORS,
	MENUID_DEFERRED_RENDERING_SHOWDEPTH,
	MENUID_DEFERRED_RENDERING_SHOWLIGHTBUFFER,
	MENUID_DEFERRED_RENDERING_SHOWSHADOWMAP,
	MENUID_DEFERRED_RENDERING_SHOWSHADOWMASK,
	MENUID_DEFERRED_RENDERING_SHOWHALFLAMBERT,		// wangjian added HalfLambert相关
	MENUID_DEFERRED_RENDERING_SHOWHDRLUMIN,			// wangjian added hdr luminance相关
	MENUID_DEFERRED_RENDERING_SHOWBLOOM,			// wangjian added bloom相关
	MENUID_FF_RENDERING,
	MENUID_RENDER_ATMOSPHERE,
	MENUID_RENDER_AERIALPERSPECTIVE,
	MENUID_RENDER_SSAO,
	MENUID_RENDER_SSAODOWNSAMPLE,
	MENUID_RENDER_SSAOBLUR,
	MENUID_RENDER_SHADOW,
	MENUID_RENDER_SHADOWQUALITY_HIGH,
	MENUID_RENDER_SHADOWQUALITY_LOW,
	MENUID_RENDER_SHADOWQUALITY_DISABLE,

	//--- wangjian modified ---//
	MENUID_RENDER_POSTEFFECT,
	MENUID_RENDER_PE_FXAA,
	MENUID_RENDER_PE_BLOOM,
	MENUID_RENDER_PE_WEATHER,

	MENUID_RENDER_WATERQUALITY_LOW,					// WATER QUALITY
	MENUID_RENDER_WATERQUALITY_MEDIUM,
	MENUID_RENDER_WATERQUALITY_HIGH,

	MENUID_RENDER_LIGHT_DIRECTIONAL,				// LIGHT TOGGLE
	MENUID_RENDER_LIGHT_POINT,

	MENUID_RENDER_TERRAINDETAIL_LOW,				// TERRAIN texture QUALITY
	MENUID_RENDER_TERRAINDETAIL_MEDIUM,
	MENUID_RENDER_TERRAINDETAIL_HIGH,

	MENUID_RENDER_TEXTUREQUALITY_LOW,
	MENUID_RENDER_TEXTUREQUALITY_MEDIUM,
	MENUID_RENDER_TEXTUREQUALITY_HIGH,
	MENUID_RENDER_TEXTUREQUALITY_ULTRA_HIGH,
	MENUID_TOGGLE_WIREFRAME,

	MENUID_TOGGLE_TONEMAPPING,

	MENUID_TOGGLE_SSS,								// wangjian added
	MENUID_TOGGLE_STATESORTING,						// wangjian added
	MENUID_TOGGLE_RENDERQUEUE_CACHE,				// wangjian added
	MENUID_TOGGLE_DUMPSCENESTATISTICS,				// wangjian added

	MENUID_DDSOPT,
	MENUID_VIEWTTF,
	MENUID_INSTALL_ESPLUGIN,
	MENUID_EDIT_SCRIPT,
	MENUID_CURVE_EDITOR,
	MENUID_DUMP_CAMERA,
	MENUID_SAVE_SCENE_FILELIST,						// wangjian added : 导出当前场景非异步加载文件列表
	MENUID_SAVE_ALL_SCENE_FILELIST,					// wangjian added : 导出所有场景非异步加载文件列表
	MENUID_MERGE_NORMAL_GLOSS_MAP,					// wangjian added :
	MENUID_REGEN_ALL_NM2,							// wangjian added : 重新生成该目录下所有的NM2文件
	MENUID_COMPRESS_ALL_NM2,						// wangjian added : 压缩该目录下所有的NM2文件

	MENUID_VIEW_CHANGELOG,
	MENUID_VIEW_D3DCAPS,
	MENUID_ABOUT,

	MENUID_NEW_SUBID = 100
};


class PluginScene;
class PluginModel;
class PluginGUI;
class PluginTree;
class PluginActor;
class PluginRibbonTrail;
class PluginComposition;
class EditorImpl;
class RealtimeEventTrigger;

class StudioApp: public ATOM_Application
{
public:
	StudioApp (void);
	virtual ~StudioApp (void);

public:
	virtual void onReportError (ATOM_AppErrorHandler::ErrorType errorType);

public:
	void onIdle (ATOM_AppIdleEvent *event);
	void onInit (ATOM_AppInitEvent *event);
	void onExit (ATOM_AppExitEvent *event);
	void onDeviceLost (ATOM_DeviceLostEvent *event);
	void onDeviceReset (ATOM_DeviceResetEvent *event);
	void onWMEvent (ATOM_AppWMEvent *event);
	void onTWCommand (ATOMX_TWCommandEvent *event);
	void onTWValueChanged (ATOMX_TWValueChangedEvent *event);
	void onTWAddVariable (ATOMX_TWAddVariableEvent *event);
	void onUnhandled (ATOM_Event *event);
	void onWindowClose (ATOM_WindowCloseEvent *event);
	void onWindowResize (ATOM_WindowResizeEvent *event);
	void onMouseMove (ATOM_MouseMoveEvent *event);
	void onChar (ATOM_CharEvent *event);
	void onKeyDown (ATOM_KeyDownEvent *event);
	void onKeyUp (ATOM_KeyUpEvent *event);

public:
	int getNumDrawCalls (void) const;
	int getNumPrimitivesDrawn (void) const;
	void *getDevice (void) const;
	bool setProjectPath (const char *path);
	bool chooseAndSetProject (void);

private:
	bool initGraphics (void);
	bool initFont (void);
	bool initMainUI (void);
	void loadPlugins (void);
	void loadPlugin (const char *filename);
	void setDefaultFont(const char * fontXml);
	void render (void);
	void initVFS (int argc, char **argv);

private:
	ATOM_GUIRenderer _guiRenderer;
	ATOM_RealtimeCtrl *_realtimeCtrl;
	ATOM_GUIFont::handle _font;
	ATOM_AUTOREF(ATOM_RenderWindow) _renderWindow;
	EditorImpl *_editorImpl;
	PluginScene *_scenePlugin;
	PluginModel *_modelPlugin;
#if defined(SUPPORT_BILLBOARD_TREE)
	PluginTree *_treePlugin;
#endif
	PluginGUI *_guiPlugin;
	PluginActor *_actorPlugin;
	PluginComposition *_compositionPlugin;
	PluginRibbonTrail *_ribbontrailPlugin;
	RealtimeEventTrigger *_realtimeTrigger;
	ATOM_RenderDevice *_device;
	bool _pauseRender;
	bool _eatKeyEvent;
	bool _eatCharEvent;
	int _numDrawCalls;
	int _numPrimitivesDrawn;
	ATOM_STRING _currentProject;
	ATOM_STRING _currentDocument;
	bool _firstCreated;
	
	ATOM_DECLARE_EVENT_MAP(StudioApp, ATOM_Application)
};

#endif // __ATOM_STUDIO_APP_H
