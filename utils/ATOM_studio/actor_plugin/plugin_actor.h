#ifndef __ATOM3D_STUDIO_ACTOR_PLUGIN_H
#define __ATOM3D_STUDIO_ACTOR_PLUGIN_H

#if _MSC_VER > 1000
# pragma once
#endif

class PluginActor: public AS_Plugin
{
public:
	PluginActor (void);

public:
	virtual ~PluginActor (void);
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
	void createCandidatesBar (void);
	void createPropertyBar (void);
	void setupPropertyBar (void);
	void setupProperty (const char *name);
	void setupIntProperty (const char *name, const ATOM_Variant &value);
	void setupFloatProperty (const char *name, const ATOM_Variant &value);
	void setupStringProperty (const char *name, const ATOM_Variant &value);
	void setupVectorProperty (const char *name, const ATOM_Variant &value);
	void setupColorProperty (const char *name, const ATOM_Variant &value);
	void setupSwitchProperty (const char *name, const ATOM_Variant &value);
	void setupCandidatesBar (ATOM_ActorPart *part);
	void setupComponentsCandidateBar (ATOM_ActorComponentsPart *part);
	void setupColorCandidateBar (ATOM_ActorColorPart *part);
	void setupTextureCandidateBar (ATOM_ActorTexturePart *part);
	void setupBindingCandidateBar (ATOM_ActorBindingPart *part);
	void setupTransformCandidateBar (ATOM_ActorTransformPart *part);
	void createPartListBox (void);
	void createPointList (void);
	ATOM_STRING generatePointName (void);
	void newPointShape (const char *name, const ATOM_Matrix4x4f &matrix);
	void refreshPointList (void);
	void renameCurrentPoint (void);
	void setCurrentPointToCamera (void);
	void setCameraToCurrentPoint (void);
	void deleteCurrentPoint (void);
	void clearPoints (void);
	void clear (void);
	void calcUILayout (void);
	void editAffectParts (void);
	int newProperty (ATOM_STRING &name) const;
	void savePropertyTypes (ATOM_TiXmlElement &xmlElement) const;
	bool loadPropertyTypes (ATOM_TiXmlElement &xmlElement);
	bool importCP (const char *filename);
	bool exportCP (const char *filename);

public:
	void onNewPart (void);
	void onDeletePart (void);
	void onSelectPart (void);

private:
	ATOM_STRING genPartName (void) const;

public:
	struct PropertyInfo
	{
		ATOM_Variant value;
		int type;
	};

private:
	AS_Editor *_editor;
	ATOM_DeferredScene *_scene;
	ATOM_AUTOREF(GridNode) _grid;
	ATOM_AUTOREF(ATOM_Actor) _actor;
	ATOM_AUTOREF(ATOMX_SkeletonVisualizerNode) _skeletonVisualizer;

	ATOMX_TweakBar *_tbPartCandidates;
	ATOMX_TweakBar *_tbProperties;
	ATOM_ListBox *_partListBox;
	ATOM_ListBox *_pointList;
	ATOM_PopupMenu *_pointListMenu;
	ATOM_PopupMenu *_actionMenuUp;
	ATOM_PopupMenu *_actionMenuDown;
	ATOM_PopupMenu *_jointMenu;
	ATOM_ActorPart *_currentPart;

	bool _editingPoint;

	ATOM_VECTOR<AS_AccelKey> _accelKeys;
	ATOM_HASHMAP<ATOM_STRING, PropertyInfo> _propertyTypes;
	ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)> _points;
};

#endif // __ATOM3D_STUDIO_ACTOR_PLUGIN_H
