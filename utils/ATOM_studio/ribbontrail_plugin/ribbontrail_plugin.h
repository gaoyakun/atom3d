#ifndef __ATOM3D_STUDIO_RIBBONTRAIL_PLUGIN_H
#define __ATOM3D_STUDIO_RIBBONTRAIL_PLUGIN_H

#if _MSC_VER > 1000
# pragma once
#endif

class PluginRibbonTrail: public AS_Plugin
{
public:
	PluginRibbonTrail (void);

public:
	virtual ~PluginRibbonTrail (void);
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
	void setupMenu (void);
	void cleanupMenu (void);
	void createPropertyBar (void);
	void setupPropertyBar (void);
	void setupProperty (const char *name);
	void setupIntProperty (const char *name, const ATOM_Variant &value);
	void setupFloatProperty (const char *name, const ATOM_Variant &value);
	void setupStringProperty (const char *name, const ATOM_Variant &value);
	void setupVectorProperty (const char *name, const ATOM_Variant &value);
	void setupColorProperty (const char *name, const ATOM_Variant &value);
	void setupSwitchProperty (const char *name, const ATOM_Variant &value);
	void clear (void);
	void calcUILayout (void);

private:
	AS_Editor *_editor;
	ATOM_DeferredScene *_scene;
	ATOM_AUTOREF(GridNode) _grid;
	ATOM_AUTOREF(ATOM_RibbonTrail) _trail;
	ATOM_AUTOREF(ATOM_ShapeNode) _target;

	ATOMX_PropertyTweakBar *_tbProperties;
};

#endif // __ATOM3D_STUDIO_RIBBONTRAIL_PLUGIN_H
