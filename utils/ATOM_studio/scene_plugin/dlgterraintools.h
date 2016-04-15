#ifndef __ATOM3D_STUDIO_SCENE_DLG_TERRAIN_TOOLS_H
#define __ATOM3D_STUDIO_SCENE_DLG_TERRAIN_TOOLS_H

class PluginScene;

class DlgTerrainDetailNormal;	// wangjian added

class DlgTerrainTools: public ATOM_EventTrigger
{
//%%BeginIDList
	enum {
		ID_BTNHEIGHTTOOL = 101,
		ID_BTNTEXTURETOOL = 102,
		ID_BTNGRASSTOOL = 132,
		ID_VSTOOLS = 103,
		ID_PNLHEIGHTTOOL = 104,
		ID_HTRAISE = 105,
		ID_HTLOWER = 106,
		ID_HTSMOOTH = 107,
		ID_HTRAMP = 108,
		ID_HTERODE = 109,
		ID_HTLEVEL = 110,
		ID_PNLTEXTURETOOL = 111,
		ID_BTNPREVTEXBRUSH = 117,
		ID_LBTEXBRUSH1 = 121,
		ID_LBTEXBRUSH2 = 123,
		ID_LBTEXBRUSH3 = 124,
		ID_LBTEXBRUSH4 = 125,
		ID_BTNNEXTTEXBRUSH = 126,
		ID_EDTILEU = 128,
		ID_EDTILEV = 129,
		ID_BTNNEWTEXTURE = 127,
		ID_BTNDELETETEXTURE = 130,
		ID_PNLGRASSTOOL = 131,
		ID_BTNPREVGRASS = 133,
		ID_LBGRASS1 = 134,
		ID_LBGRASS2 = 135,
		ID_LBGRASS3 = 136,
		ID_LBGRASS4 = 137,
		ID_BTNNEXTGRASS = 138,

		ID_BTNOPENNORMAL = 139,			// wangjian added : 切换地形法线贴图

		ID_EDGRASSWIDTH = 141,
		ID_EDGRASSHEIGHT = 143,
		ID_BTNNEWGRASS = 144,
		ID_BTNDELETEGRASS = 145,
		ID_BTNPREVBRUSH = 112,
		ID_LBBRUSH1 = 114,
		ID_LBBRUSH2 = 115,
		ID_LBBRUSH3 = 118,
		ID_LBBRUSH4 = 120,
		ID_BTNNEXTBRUSH = 113,
		ID_SBRADIUS = 116,
		ID_SBSTRENGTH = 119,
		ID_SBFALLOFF = 122,
	};
//%%EndIDList

	enum Tool
	{
		TOOL_RAISE,
		TOOL_LOWER,
		TOOL_SMOOTH,
		TOOL_RAMP,
		TOOL_ERODE,
		TOOL_LEVEL
	};

public:
	DlgTerrainTools (PluginScene *plugin, ATOM_Terrain *terrain, ATOM_Widget *parent);
	virtual ~DlgTerrainTools (void);

public:
//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_btnHeightTool = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNHEIGHTTOOL);
		m_btnTextureTool = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNTEXTURETOOL);
		m_btnGrassTool = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNGRASSTOOL);
		m_vsTools = (ATOM_ViewStack*)parent->getChildByIdRecursive(ID_VSTOOLS);
		m_pnlHeightTool = (ATOM_Panel*)parent->getChildByIdRecursive(ID_PNLHEIGHTTOOL);
		m_htRaise = (ATOM_Button*)parent->getChildByIdRecursive(ID_HTRAISE);
		m_htLower = (ATOM_Button*)parent->getChildByIdRecursive(ID_HTLOWER);
		m_htSmooth = (ATOM_Button*)parent->getChildByIdRecursive(ID_HTSMOOTH);
		m_htRamp = (ATOM_Button*)parent->getChildByIdRecursive(ID_HTRAMP);
		m_htErode = (ATOM_Button*)parent->getChildByIdRecursive(ID_HTERODE);
		m_htLevel = (ATOM_Button*)parent->getChildByIdRecursive(ID_HTLEVEL);
		m_pnlTextureTool = (ATOM_Panel*)parent->getChildByIdRecursive(ID_PNLTEXTURETOOL);
		m_btnPrevTexBrush = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNPREVTEXBRUSH);
		m_lbTexBrush1 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBTEXBRUSH1);
		m_lbTexBrush2 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBTEXBRUSH2);
		m_lbTexBrush3 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBTEXBRUSH3);
		m_lbTexBrush4 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBTEXBRUSH4);
		m_btnNextTexBrush = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNNEXTTEXBRUSH);
		m_edTileU = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDTILEU);
		m_edTileV = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDTILEV);
		m_btnNewTexture = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNNEWTEXTURE);
		m_btnDeleteTexture = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNDELETETEXTURE);
		m_pnlGrassTool = (ATOM_Panel*)parent->getChildByIdRecursive(ID_PNLGRASSTOOL);
		m_btnPrevGrass = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNPREVGRASS);
		m_lbGrass1 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBGRASS1);
		m_lbGrass2 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBGRASS2);
		m_lbGrass3 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBGRASS3);
		m_lbGrass4 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBGRASS4);
		m_btnNextGrass = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNNEXTGRASS);
		m_edGrassWidth = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDGRASSWIDTH);
		m_edGrassHeight = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDGRASSHEIGHT);
		m_btnNewGrass = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNNEWGRASS);
		m_btnDeleteGrass = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNDELETEGRASS);
		m_btnPrevBrush = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNPREVBRUSH);
		m_lbBrush1 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBBRUSH1);
		m_lbBrush2 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBBRUSH2);
		m_lbBrush3 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBBRUSH3);
		m_lbBrush4 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBBRUSH4);
		m_btnNextBrush = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNNEXTBRUSH);
		m_sbRadius = (ATOM_ScrollBar*)parent->getChildByIdRecursive(ID_SBRADIUS);
		m_sbStrength = (ATOM_ScrollBar*)parent->getChildByIdRecursive(ID_SBSTRENGTH);
		m_sbFalloff = (ATOM_ScrollBar*)parent->getChildByIdRecursive(ID_SBFALLOFF);

		m_btnNormalOpen = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNOPENNORMAL);		// wangjian added
	}
//%%EndInitFunc

public:
	int show (bool b);
	void beginEdit (void);
	void endEdit (void);
	bool isShown (void) const;
	void applyRaiseBrush (int x, int y, float orientation);
	void applyLowerBrush (int x, int y, float orientation);
	void applySmoothBrush (int x, int y, float orientation);
	void applyLevelBrush (int x, int y, float orientation, float height);
	void applyTextureBrush (float x, float y, float orientation);
	void applyGrassBrush (const ATOM_Vector3f &pos, float orientation);
	void beginBrushing (bool revert);
	void updateBrushing (int x, int y, float fx, float fy);
	void endBrushing (void);
	bool isBrushing (void) const;
	void restoreHeights (void);
	void restoreAlphas (void);
	void calcNormalMap (void);
	ATOM_Terrain *getTerrain (void) const;
	void updateBaseMap (void);
	void updateBrushIndicator (void);
	void beginOrientation (void);
	void updateOrientation (int deltaMouse);
	void endOrientation (void);
	bool isEditingOrientation (void) const;
	void setHitTerrain (bool hit, const ATOM_Vector3f &hitPos);

public:
	void onCommand (ATOM_WidgetCommandEvent *event);
	void onClose (ATOM_TopWindowCloseEvent *event);
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	//void onRButtonDown(ATOM_WidgetRButtonDownEvent *event);	// wangjian added
	void onEditChanged (ATOM_EditTextChangedEvent *event);

private:
	void loadBrushes (void);
	void setHeightBrushPaletteIndex (int index);
	void selectHeightBrush (int index);
	void setTextureBrushPaletteIndex (int index);
	void selectTextureBrush (int index);
	void setGrassBrushPaletteIndex (int index);
	void selectGrassBrush (int index);
	ATOM_Label *getHeightBrushLabel (int index) const;
	ATOM_Label *getTextureBrushLabel (int index) const;
	ATOM_Label *getGrassBrushLabel (int index) const;
	void applyCurrentHeightBrush (int x, int y, int w, int h, float orientation, float strength, ATOM_Material *material);
	void backupHeights (void);
	void backupAlphas (void);
	void drawQuad (ATOM_RenderDevice *device, ATOM_Material *material, int w, int h);
	void drawRegion (ATOM_RenderDevice *device, ATOM_Material *material, int x, int y, int w, int h, float orientation, int vpW, int vpH);
	void updateHeightField (void);
	void checkHeightTool (int id);

private:
//%%BeginVarList
	ATOM_Button *m_btnHeightTool;
	ATOM_Button *m_btnTextureTool;
	ATOM_Button *m_btnGrassTool;
	ATOM_ViewStack *m_vsTools;
	ATOM_Panel *m_pnlHeightTool;
	ATOM_Button *m_htRaise;
	ATOM_Button *m_htLower;
	ATOM_Button *m_htSmooth;
	ATOM_Button *m_htRamp;
	ATOM_Button *m_htErode;
	ATOM_Button *m_htLevel;
	ATOM_Panel *m_pnlTextureTool;
	ATOM_Button *m_btnPrevTexBrush;
	ATOM_Label *m_lbTexBrush1;
	ATOM_Label *m_lbTexBrush2;
	ATOM_Label *m_lbTexBrush3;
	ATOM_Label *m_lbTexBrush4;
	ATOM_Button *m_btnNextTexBrush;
	ATOM_Edit *m_edTileU;
	ATOM_Edit *m_edTileV;
	ATOM_Button *m_btnNewTexture;
	ATOM_Button *m_btnDeleteTexture;
	ATOM_Panel *m_pnlGrassTool;
	ATOM_Button *m_btnPrevGrass;
	ATOM_Label *m_lbGrass1;
	ATOM_Label *m_lbGrass2;
	ATOM_Label *m_lbGrass3;
	ATOM_Label *m_lbGrass4;
	ATOM_Button *m_btnNextGrass;
	ATOM_Edit *m_edGrassWidth;
	ATOM_Edit *m_edGrassHeight;
	ATOM_Button *m_btnNewGrass;
	ATOM_Button *m_btnDeleteGrass;
	ATOM_Button *m_btnPrevBrush;
	ATOM_Label *m_lbBrush1;
	ATOM_Label *m_lbBrush2;
	ATOM_Label *m_lbBrush3;
	ATOM_Label *m_lbBrush4;
	ATOM_Button *m_btnNextBrush;
	ATOM_ScrollBar *m_sbRadius;
	ATOM_ScrollBar *m_sbStrength;
	ATOM_ScrollBar *m_sbFalloff;

	ATOM_Button *	m_btnNormalOpen;		// wangjian added : 切换法线
//%%EndVarList

	ATOM_Dialog *_dialog;
	PluginScene *_plugin;
	ATOM_AUTOREF(ATOM_Terrain) _terrain;

	int _currentFirstHeightBrush;
	int _numHeightBrushes;
	ATOM_GUIImage *_currentSelectedHeightBrush;

	int _currentFirstTextureBrush;
	int _currentSelectedTextureBrushIndex;
	ATOM_GUIImage *_currentSelectedTextureBrush;
	ATOM_VECTOR<int> _textureBrushImageIDs;
	//--------- wangjian added -----------------//
	DlgTerrainDetailNormal * _detailNormalDlg;
	//------------------------------------------//

	int _currentFirstGrassBrush;
	int _currentSelectedGrassBrushIndex;
	ATOM_GUIImage *_currentSelectedGrassBrush;
	ATOM_VECTOR<int> _grassBrushImageIDs;

	int _currentHeightmap;
	ATOM_AUTOREF(ATOM_Texture) _heightmapPingPong[2];
	ATOM_AUTOREF(ATOM_Texture) _normalMap;
	ATOM_VECTOR<float> _backupHeights;
	ATOM_VECTOR<ATOM_BaseImage*> _backupAlphas;
	ATOM_AUTOREF(ATOM_DepthBuffer) _depthbuffer;
	ATOM_AUTOREF(ATOM_DepthBuffer) _depthbufferAlpha;

	ATOM_AUTOPTR(ATOM_Material) _copyMaterial;
	ATOM_AUTOPTR(ATOM_Material) _raiseMaterial;
	ATOM_AUTOPTR(ATOM_Material) _smoothMaterial;
	ATOM_AUTOPTR(ATOM_Material) _levelMaterial;
	ATOM_AUTOPTR(ATOM_Material) _normalgenMaterial;
	ATOM_AUTOPTR(ATOM_Material) _textureMaterial;
	ATOM_AUTOPTR(ATOM_Material) _basemapMaterial;
	ATOM_AUTOREF(ATOM_Decal) _brushIndicator;

	bool _isBrushing;
	Tool _currentTool;
	bool _isBrushingTexture;
	bool _isBrushingGrass;
	int _savedPixelError;
	float _savedGrassDistance;
	float _orientation;
	bool _isEditingOrientation;
	bool _isTerrainHit;
	bool _revert;
	ATOM_Vector3f _hitPosSaved;
	//--- wangjian added ---//
	float _hitHeightForLevel;
	//----------------------//
	int _nextTextureBrushImageId;
	ATOM_VECTOR<ATOM_Vector3f> _normals;

	ATOM_DECLARE_EVENT_MAP(DlgTerrainTools, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_SCENE_DLG_TERRAIN_TOOLS_H
