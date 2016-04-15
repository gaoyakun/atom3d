#ifndef __ATOM3D_STUDIO_SCENE_REGIONEDIT_H
#define __ATOM3D_STUDIO_SCENE_REGIONEDIT_H

class PluginScene;

class DlgRegionEdit: public ATOM_EventTrigger
{
public:
	enum RegionType
	{
		RT_NONE = 0,
		RT_RECT = 1
	};

	enum
	{
		RF_ROTATABLE = (1<<0),
		RF_SCALABLE = (1<<1)
	};

	struct RegionAttribDef
	{
		ATOM_STRING name;
		ATOM_Variant::Type type;
		int defWidth;
	};

	struct RegionTemplate
	{
		ATOM_STRING name;
		int sortName;
		ATOM_STRING head;
		ATOM_STRING sep;
		ATOM_STRING lineTemplate;
	};

	struct RegionDef
	{
		RegionType type;
		ATOM_STRING name;
		int colorKeyIndex;
		ATOM_ColorARGB defColor;
		ATOM_Size2Di defSize;
		float widthScale;
		unsigned flags;
		RegionTemplate templ;
		ATOM_MAP<int, ATOM_ColorARGB> colorMap;
		ATOM_VECTOR<RegionAttribDef> attributes;
		ATOM_VECTOR<ATOM_VECTOR<ATOM_Variant> > regionList;
		ATOM_VECTOR<ATOM_Matrix4x4f> regionTransforms;
	};

private:
//%%BeginIDList
	enum {
		ID_CBREGIONTYPE = 101,
		ID_LVHEADER = 107,
		ID_LVREGIONLIST = 102,
		ID_BTNNEWREGION = 103,
		ID_BTNDELETEREGION = 104,
		ID_BTNREGIONPROPS = 105,
		ID_BTNLOAD = 108,
		ID_BTNSAVE = 109,
		ID_BTNCLOSE = 106,
		ID_BTNEXPORT = 110,
	};
//%%EndIDList

//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_cbRegionType = (ATOM_ComboBox*)parent->getChildByIdRecursive(ID_CBREGIONTYPE);
		m_lvHeader = (ATOM_ListView*)parent->getChildByIdRecursive(ID_LVHEADER);
		m_lvRegionList = (ATOM_ListView*)parent->getChildByIdRecursive(ID_LVREGIONLIST);
		m_btnNewRegion = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNNEWREGION);
		m_btnDeleteRegion = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNDELETEREGION);
		m_btnRegionProps = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNREGIONPROPS);
		m_btnLoad = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNLOAD);
		m_btnSave = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNSAVE);
		m_btnClose = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCLOSE);
		m_btnExport = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNEXPORT);
	}
//%%EndInitFunc

//%%BeginVarList
	ATOM_ComboBox *m_cbRegionType;
	ATOM_ListView *m_lvHeader;
	ATOM_ListView *m_lvRegionList;
	ATOM_Button *m_btnNewRegion;
	ATOM_Button *m_btnDeleteRegion;
	ATOM_Button *m_btnRegionProps;
	ATOM_Button *m_btnLoad;
	ATOM_Button *m_btnSave;
	ATOM_Button *m_btnClose;
	ATOM_Button *m_btnExport;
//%%EndVarList

public:
	DlgRegionEdit (PluginScene *plugin, ATOM_Widget *parent);
	virtual ~DlgRegionEdit (void);

public:
	void show (bool show);
	ATOM_Dialog *getWidget (void) const;
	void addRegion (ATOM_Decal *decal);
	void selectRegion (int index);
	void removeRegion (int defIndex, int regionIndex);
	void removeAllRegions (int defIndex);
	void clear (void);
	unsigned getNumRegions (int regDefIndex) const;
	bool loadRegionFromFile (const char *xmlFile);
	bool saveRegionToFile (const char *xmlFile);
	bool exportRegionToFile (const char *fileName);

private:
	void loadCfg (const char *configFileName);
	void initUI (void);
	void updateUI (void);
	void updateListView (void);
	void updateRegionList (int index);
	void setRegionListHeader (const RegionDef &def);
	void updateCanvas (void);
	ATOM_ColorARGB getDecalColor (const RegionDef &def, int regionIndex);
	bool loadRegions (const ATOM_TiXmlElement *xml);
	bool saveRegions (ATOM_TiXmlElement *xml);
	bool writeHead (ATOM_File *f, const RegionDef &def, const ATOM_Variant &key);
	bool writeRegion (ATOM_File *f, const RegionDef &def, int regionIndex);
	void updateDecalNames (void);
	ATOM_AUTOREF(ATOM_Decal) createDecal (const ATOM_Matrix4x4f &matrix, int regionIndex);

public:
	void onCommand (ATOM_WidgetCommandEvent *event);
	void onEditChanged (ATOM_EditTextChangedEvent *event);
	void onCBChanged (ATOM_ComboBoxSelectionChangedEvent *event);

private:
	PluginScene *_plugin;
	ATOM_Dialog *_dialog;
	ATOM_VECTOR<RegionDef> _regionDefs;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Decal)> regionDecals;
	int _currentDef;
	int _currentRegion;

	ATOM_DECLARE_EVENT_MAP(DlgRegionEdit, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_SCENE_REGIONEDIT_H
