#ifndef __ATOM3D_STUDIO_TREE_PLUGIN_H
#define __ATOM3D_STUDIO_TREE_PLUGIN_H

#if defined(SUPPORT_BILLBOARD_TREE)

#if _MSC_VER > 1000
# pragma once
#endif

class AS_Editor;

class PluginTree: public AS_Plugin
{
public:
	PluginTree (void);

public:
	virtual ~PluginTree (void);
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
	void createParamTweakBar (void);
	void createViewTweakBar (void);
	void calcUILayout (void);
	void showClusters (bool show);
	void selectSphereNode (ATOM_Node *node);
	void beginTranslateSphere (ATOM_Node *node);
	void beginScaleSphere (ATOM_Node *node);
	void endEditSphereTransform (ATOM_Node *node);
	bool importTrunkModel (const char *filename);

private:
	struct LeafParams
	{
		unsigned _numLeafCards;
		float _leafRandomRange;
		float _leafWidth;
		float _leafHeight;
		float _leafSizeV;
		int _flipLeaves;
	};

	AS_Editor *_editor;
	ATOMX_TweakBar *_paramTweakBar;
	ATOMX_TweakBar *_viewTweakBar;
	ATOM_DeferredScene *_scene;
	ATOM_AUTOREF(TreeNode) _tree;
	ATOM_AUTOREF(ATOM_Node) _selectedSphereNode;
	ATOM_AUTOPTR(ATOM_Material) _clusterMaterial;
	bool _isEditSphereTransform;
	TreeData _treeData;
	bool _treeModified;
	unsigned _lastUpdateTick;
	unsigned _updateInterval;
	LeafParams _currentLeafParam;
	LeafParams _desiredLeafParam;
	bool _treeDirty;
	bool _clusterShown;
	int _drawExternTrunk;
	int _drawLeaves;
	int _drawNativeTrunk;
	unsigned _trunkTriangleCount;
	unsigned _leafTriangleCount;
	float _FPS;
	float _lodDistance;
};

#endif

#endif // __ATOM3D_STUDIO_TREE_PLUGIN_H
