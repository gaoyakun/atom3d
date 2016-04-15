#ifndef __ATOM3D_STUDIO_SCENE_PATHEDITOR_H
#define __ATOM3D_STUDIO_SCENE_PATHEDITOR_H

class PluginScene;

class DlgPathEditor: public ATOM_EventTrigger
{
//%%BeginIDList
	enum {
		ID_CBPATHLIST = 101,
		ID_BTNNEWPATH = 102,
		ID_BTNDELETEPATH = 103,
		ID_EDPATHNAME = 111,
		ID_EDROUNDRADIUS = 106,
		ID_EDSMOOTHNESS = 108,
		ID_BTNNEWCTLPOINT = 104,
		ID_BTNREMOVECTLPOINT = 105,
		ID_BTNSHOWHIDE = 107,
	};
//%%EndIDList

//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_cbPathList = parent ? (ATOM_ComboBox*)parent->getChildByIdRecursive(ID_CBPATHLIST) : 0;
		m_btnNewPath = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNNEWPATH) : 0;
		m_btnDeletePath = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNDELETEPATH) : 0;
		m_edPathName = parent ? (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDPATHNAME) : 0;
		m_edRoundRadius = parent ? (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDROUNDRADIUS) : 0;
		m_edSmoothness = parent ? (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDSMOOTHNESS) : 0;
		m_btnNewCtlPoint = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNNEWCTLPOINT) : 0;
		m_btnRemoveCtlPoint = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNREMOVECTLPOINT) : 0;
		m_btnShowHide = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNSHOWHIDE) : 0;
	}
//%%EndInitFunc

//%%BeginVarList
	ATOM_ComboBox *m_cbPathList;
	ATOM_Button *m_btnNewPath;
	ATOM_Button *m_btnDeletePath;
	ATOM_Edit *m_edPathName;
	ATOM_Edit *m_edRoundRadius;
	ATOM_Edit *m_edSmoothness;
	ATOM_Button *m_btnNewCtlPoint;
	ATOM_Button *m_btnRemoveCtlPoint;
	ATOM_Button *m_btnShowHide;
//%%EndVarList

public:
	DlgPathEditor (ATOM_Widget *parent, PluginScene *plugin);
	virtual ~DlgPathEditor (void);

	void show (bool bShow);
	void addPath (ATOM_LineTrail *lineTrail);
	void addPath (const char *name, const ATOM_Path3D &path);
	int getNumPaths (void) const;
	void deletePath (int index);
	void deletePathByName (const char *pathName);
	void clear (void);
	int getPathByCtlPointNode (ATOM_Node *node) const;
	int getPathByName (const char *pathName) const;
	const char *getPathName (int path) const;
	float getPathRoundRadius (int path) const;
	int getPathSmoothness (int path) const;
	void refreshPathTrail (int path);
	ATOM_Dialog *getDialog (void) const;
	void selectPath (int path);
	void setCurrentCtlPoint (ATOM_Node *node);
	void addCtlPoint (void);
	void removeCtlPoint (void);
	void showHidePath (void);
	void createPath (int index, ATOM_Path3D *path) const;

public:
	void onCommand (ATOM_WidgetCommandEvent *event);
	void onPathSelected (ATOM_ComboBoxSelectionChangedEvent *event);
	void onEditTextChanged (ATOM_EditTextChangedEvent *event);

private:
	ATOM_STRING newPathName (void) const;

private:
	struct PathInfo
	{
		ATOM_STRING pathName;
		float roundRadius;
		int smoothness;
		bool show;
		ATOM_AUTOREF(ATOM_LineTrail) pathTrail;
		ATOM_VECTOR<ATOM_AUTOREF(ATOM_ShapeNode)> controlPoints;
		ATOM_VECTOR<ATOM_Vector3f> lookDirs;
	};
	ATOM_VECTOR<PathInfo> _pathList;
	ATOM_AUTOREF(ATOM_Node) _pathParentNode;

	PluginScene *_plugin;
	int _currentPath;
	int _currentPoint;
	ATOM_Dialog *_dlg;

	ATOM_DECLARE_EVENT_MAP(DlgPathEditor, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_SCENE_PATHEDITOR_H
