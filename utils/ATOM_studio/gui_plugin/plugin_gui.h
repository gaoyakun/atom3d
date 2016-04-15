#ifndef __ATOM3D_STUDIO_GUI_PLUGIN_H
#define __ATOM3D_STUDIO_GUI_PLUGIN_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "editor.h"
#include "plugin.h"
#include "proxy.h"
#include "component_proxy.h"
#include "widgetparameters.h"

class EditorForm;
class WidgetParameters;

#define ATOM3D_UI_VERSION 1

class PluginGUI: public AS_Plugin
{
public:
	struct FontInfo
	{
		ATOM_STRING name;
		ATOM_STRING filename;
		ATOM_STRING comment;
		int size;
		int charset;
		int margin;
		ATOM_GUIFont::handle handle;
	};
	typedef ATOM_HashMap<ATOM_STRING, FontInfo> FontInfoMap;

	typedef ATOM_Set<ATOM_STRING> NameSet;

public:
	PluginGUI (void);
	void updateWidgetTree (bool rebuild);

public:
	virtual ~PluginGUI (void);
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
	FontInfoMap *getFontInfoMap (void);
	const FontInfoMap *getFontInfoMap (void) const;
	ATOMX_TweakBar *getWidgetPropertyBar (void) const;
	AS_Editor *getEditor (void) const;
	void updateImageList (void);
	int editColorImage (int id, WidgetParameters::ImageInfo *image);
	int editTextureImage (int id, WidgetParameters::ImageInfo *image);
	int editMaterialImage (int id, WidgetParameters::ImageInfo *image);
	void showThumb (bool show);
	bool validateProxyName (const char *name) const;
	bool changeProxyName (ControlProxy *proxy, const char *newName);
	void addProxyName (const char *name);
	void removeProxyName (const char *name);
	ATOM_Widget *getUIRoot (void) const;
	void generateControlIDs (void);
	bool exportSourceCode (void);
	void *getCreateParameter (void) const;

private:
	void setupMenu (void);
	void cleanupMenu (void);
	int generateImageId (void) const;
	void calcUILayout (void);
	bool loadFontSet (const char *filename);
	bool saveFontSet (const char *filename) const;
	void updateFontList (void);
	void updateWidgetTreeR (ATOM_TreeItem *parentItem);
	ATOM_STRING showNewFontDialog (void);
	void notifyFontChange (const char *name);
	void notifyFontChangeR (ControlProxy *proxy, const char *name);
	ATOM_GUIFont::handle createFont (const char *fontFilename, int size, int charset, int margin);
	bool saveSelected (ATOM_TiXmlElement *parent);
	bool saveAll (ATOM_TiXmlElement *parent);
	bool load (ATOM_TiXmlElement *root);
	bool saveHeaderFile (ControlProxy *proxy, const char *filename) const;
	void showGUI (bool show);
	void toggleGUI (void);
	void generateControlIDs_R (ControlProxy *proxy);
	bool generateSource_R (ControlProxy *rootProxy, ControlProxy *proxy, const char *idindent, ATOM_STRING &idlist, const char *varindent, ATOM_STRING &varlist, const char *initindent, ATOM_STRING &initlist, const char *parentWindowName);

	struct ArrayInfo
	{
		int dimension;
		ATOM_SET<int> elements;
		ATOM_STRING widgetType;
	};
	bool generateArrayInfo_R (ATOM_MAP<ATOM_STRING, ArrayInfo> &arrayMap, ControlProxy *rootProxy, ControlProxy *proxy);

private:
	static void ATOMX_CALL FontFileCallback (void *userData);
	static void ATOMX_CALL FontDeleteCallback (void *userData);
	static void ATOMX_CALL FontListLoad (void *userData);
	static void ATOMX_CALL FontListSave (void *userData);
	static void ATOMX_CALL FontListSaveAs (void *userData);
	static void ATOMX_CALL FontListClear (void *userData);
	static void ATOMX_CALL FontListNew (void *userData);
	static void ATOMX_CALL ToolboxCreateWidget (void *userData);

private:
	ATOM_AUTOPTR(ATOM_GUIImageList) _pluginImages;
	ATOM_TreeCtrl *_uiHerichy;
	ATOM_ListBox *_imageList;
	ATOM_Widget *_thumb;
	ATOMX_TweakBar *_fontList;
	ATOMX_TweakBar *_widgetProp;
	ATOMX_TweakBar *_toolbox;
	ATOMX_TweakBar *_customProp;
	ATOM_PopupMenu *_imageListMenu;
	EditorForm *_editorForm;
	FontInfoMap _fontInfoMap;
	ATOM_Set<ATOM_STRING> _installedFonts;
	NameSet _proxyNameSet;
	ATOM_STRING _fontSetFile;
	AS_Editor *_editor;
	bool _guiShown;
	void *_creationParam;
	ATOM_VECTOR<AS_AccelKey> _accelKeys;

	//--- wangjian added ---//
	bool _asyncload;
	//----------------------//
};

#endif // __ATOM3D_STUDIO_GUI_PLUGIN_H
