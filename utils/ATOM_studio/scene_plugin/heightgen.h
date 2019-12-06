#ifndef __ATOM3D_STUDIO_SCENE_HEIGHT_GEN_H
#define __ATOM3D_STUDIO_SCENE_HEIGHT_GEN_H

class DlgEditBlocks;
class PluginScene;

/*
struct HeightFieldBBoxTreeNode
{
	ATOM_BBox bbox;
	ATOM_Rect2Di rc;
	int v[4];
	HeightFieldBBoxTreeNode *left;
	HeightFieldBBoxTreeNode *right;
};

class HeightFieldBBoxTree: private ATOM_Noncopyable
{
public:
	HeightFieldBBoxTree (int res_x, int res_y, const ATOM_Vector4f *vertices);
	~HeightFieldBBoxTree (void);

	bool pick (const ATOM_Ray &ray, float &distance);
	bool create (int res_x, int res_y, const ATOM_Vector4f *vertices);
	float getHeight (int x, int y) const;
	float getRealHeight (float x, float y) const;
	HeightFieldBBoxTreeNode *getRootNode (void) const;
	float *getHeights (void) const;
	void updateHeightValue (int x, int z, float height);

private:
	void deleteNode (HeightFieldBBoxTreeNode *node);
	bool createChildNode (HeightFieldBBoxTreeNode *node, int x, int y, int w, int h, const ATOM_Vector4f *vertices);
	void updateChildNode (HeightFieldBBoxTreeNode *node, int x, int y, float height);
	HeightFieldBBoxTreeNode *rayIntersectionTestR (HeightFieldBBoxTreeNode *node, const ATOM_Ray &ray, float *d) const;

private:
	int _resX;
	int _resY;
	float *_heights;
	HeightFieldBBoxTreeNode *_rootNode;
};
*/

class HeightGrid: private ATOM_Noncopyable
{
public:
	HeightGrid (ATOM_Scene *scene, DlgEditBlocks *dlgEditBlocks, PluginScene *plugin);
	~HeightGrid (void);

public:
	void setSize (const ATOM_Size2Di &size);
	void setCellSize (float cellSize);
	const ATOM_Size2Di &getSize (void) const;
	ATOM_ShapeNode *getHeightGrid (void) const;
	ATOM_ShapeNode *getBlockGrid (void) const;
	ATOM_Texture *getPositionTexture (void) const;
	ATOM_Texture *getBlockTexture (void) const;
	ATOM_Material *getHeightGridMaterial (void) const;
	ATOM_Material *getBlockGridMaterial (void) const;
	HeightFieldBBoxTree *getBBoxTree(void) const;
	void updateBlockData (int x, int y, int w, int h, int type, bool set);
	void updateBlockTexture (int x, int y, int w, int h, ATOM_ColorARGB color);
	void updateBlockTextureByType (int type);
	void clearBlockTexture (ATOM_ColorARGB color);
	void clearBlocks (int type, bool set);
	void clearBlocksLessAngle (int type, int angle, bool set);
	void clearBlocksGreaterAngle (int type, int angle, bool set);
	DlgEditBlocks *getDlgEditBlocks (void) const;
	void updateHeightValue (int x, int z, float height);
	void updatePositionHeight (int x, int z, float height);

public:
	void showGrid (bool show);
	bool isGridShown (void) const;
	void showBlock (bool show);
	bool isBlockShown (void) const;
	void setVertices (const ATOM_Vector4f *vertices);
	const ATOM_VECTOR<unsigned char> &getBlockData (void) const;
	ATOM_VECTOR<unsigned char> &getBlockData (void);
	void setGridColor (const ATOM_Vector4f &color);

private:
	PluginScene *_plugin;
	DlgEditBlocks *_dlgEditBlocks;
	float _cellSize;
	ATOM_Scene *_scene;
	ATOM_Size2Di _size;
	ATOM_Vector4f _gridColor;
#if 1
	ATOM_HeightField *_heightField;
#else
	HeightFieldBBoxTree *_bboxTree;
#endif
	ATOM_VECTOR<ATOM_ColorARGB> _colorCache;
	ATOM_VECTOR<unsigned char> _blockData;
	ATOM_AUTOPTR(ATOM_Material) _heightGridMaterial;
	ATOM_AUTOPTR(ATOM_Material) _blockGridMaterial;
	ATOM_AUTOPTR(ATOM_Material) _updatePosMaterial;
	ATOM_AUTOREF(ATOM_Texture) _positionTexture;
	ATOM_AUTOREF(ATOM_Texture) _blockTexture;
	ATOM_AUTOREF(ATOM_ShapeNode) _heightGrid;
	ATOM_AUTOREF(ATOM_ShapeNode) _blockGrid;
};

class DlgHeightGen: public ATOM_EventTrigger
{
public:
//%%BeginIDList
	enum {
		ID_EDCELLSIZE = 110,
		ID_EDX = 103,
		ID_EDY = 109,
		ID_EDWIDTH = 102,
		ID_EDHEIGHT = 104,
		ID_EDNEAR = 101,
		ID_EDFAR = 105,
		ID_EDINCLUDEPATTERN = 106,
		ID_BTNOK = 107,
		ID_BTNSUBGEN = 111,
		ID_BTNCANCEL = 108,
		ID_BTNSUBGEN2 = 112,
	};
//%%EndIDList

public:
	DlgHeightGen (ATOM_Widget *parent, ATOM_Scene *scene, DlgEditBlocks *dlgEditBlocks, PluginScene *plugin);
	virtual ~DlgHeightGen (void);

public:
//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_edCellSize = parent ? (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDCELLSIZE) : 0;
		m_edX = parent ? (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDX) : 0;
		m_edY = parent ? (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDY) : 0;
		m_edWidth = parent ? (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDWIDTH) : 0;
		m_edHeight = parent ? (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDHEIGHT) : 0;
		m_edNear = parent ? (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDNEAR) : 0;
		m_edFar = parent ? (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDFAR) : 0;
		m_edIncludePattern = parent ? (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDINCLUDEPATTERN) : 0;
		m_btnOk = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNOK) : 0;
		m_btnSubGen = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNSUBGEN) : 0;
		m_btnCancel = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCANCEL) : 0;
		m_btnSubGen2 = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNSUBGEN2) : 0;
	}
//%%EndInitFunc

public:
	int showModal (void);
	HeightGrid *getGrid (void);
	float getCellSize (void) const;
	const ATOM_Rect2Di &getRegion (void) const;
	bool loadHeightData (const char *filename);
	bool saveHeightData (const char *filename);
	bool exportHeightMap (const char *filename);
	bool loadBlockData (const char *filename);
	bool saveBlockData (const char *filename);
	bool exportBlockMap (const char *filename);
	bool exportSceneMap (const char *filename);
	bool buildHeightGrid (ATOM_ShapeNode *indicator, bool exclude, const ATOM_Rect2Di *subRegion = nullptr);

public:
	void onCommand (ATOM_WidgetCommandEvent *event);

private:

private:
//%%BeginVarList
	ATOM_Edit *m_edCellSize;
	ATOM_Edit *m_edX;
	ATOM_Edit *m_edY;
	ATOM_Edit *m_edWidth;
	ATOM_Edit *m_edHeight;
	ATOM_Edit *m_edNear;
	ATOM_Edit *m_edFar;
	ATOM_Edit *m_edIncludePattern;
	ATOM_Button *m_btnOk;
	ATOM_Button *m_btnSubGen;
	ATOM_Button *m_btnCancel;
	ATOM_Button *m_btnSubGen2;
//%%EndVarList

	ATOM_Dialog *_dialog;
	ATOM_Rect2Df _region;
	ATOM_Point2Di _nearFar;
	float _cellSize;
	PluginScene *_plugin;
	ATOM_Scene *_scene;
	ATOM_BBox _sceneBoundingBox;
	ATOM_ShapeNode *_heightIndicator;
	HeightGrid _heightGrid;

	ATOM_AUTOPTR(ATOM_Material) _heightGenMaterial;

	ATOM_DECLARE_EVENT_MAP(DlgHeightGen, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_SCENE_HEIGHT_GEN_H
