#ifndef __ATOM3D_STUDIO_SCENE_BLOCK_EDITOR_H
#define __ATOM3D_STUDIO_SCENE_BLOCK_EDITOR_H

class PluginScene;

enum BlockType
{
	BT_None = 0,
	BT_Block = (unsigned char)(3<<0),
	BT_Unsafe = (unsigned char)(1<<2),
	BT_Stall = (unsigned char)(1<<3)
};

enum BrushMode
{
	BM_FILL,
	BM_CLEAR
};

class DlgEditBlocks: public ATOM_EventTrigger
{
//%%BeginIDList
	enum {
		ID_CBBLOCKTYPES = 102,
		ID_SBBRUSHSIZE = 104,
		ID_SBANGLE = 107,
		ID_BTNCLEAR = 101,
		ID_BTNCLEARANGLEL = 103,
		ID_BTNCLEARANGLEG = 108,
		ID_BTNCLOSE = 105,
		ID_CBBRUSHMODE = 109,
	};
//%%EndIDList

public:
	DlgEditBlocks (PluginScene *plugin, ATOM_Widget *parent);
	virtual ~DlgEditBlocks (void);

public:
	//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_cbBlockTypes = (ATOM_ComboBox*)parent->getChildByIdRecursive(ID_CBBLOCKTYPES);
		m_sbBrushSize = (ATOM_ScrollBar*)parent->getChildByIdRecursive(ID_SBBRUSHSIZE);
		m_sbAngle = (ATOM_ScrollBar*)parent->getChildByIdRecursive(ID_SBANGLE);
		m_btnClear = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCLEAR);
		m_btnClearAngleL = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCLEARANGLEL);
		m_btnClearAngleG = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCLEARANGLEG);
		m_btnClose = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCLOSE);
		m_cbBrushMode = (ATOM_ComboBox*)parent->getChildByIdRecursive(ID_CBBRUSHMODE);
	}
//%%EndInitFunc

public:
	int show (bool b);
	bool isShown (void) const;
	BlockType getBlockType (void) const;
	BrushMode getBrushMode (void) const;
	int getBrushSize (void) const;
	ATOM_ColorARGB getBrushColor (BlockType type) const;

public:
	void onCommand (ATOM_WidgetCommandEvent *event);
	void onBlockTypeChanged (ATOM_ComboBoxSelectionChangedEvent *event);

private:
	//%%BeginVarList
	ATOM_ComboBox *m_cbBlockTypes;
	ATOM_ScrollBar *m_sbBrushSize;
	ATOM_ScrollBar *m_sbAngle;
	ATOM_Button *m_btnClear;
	ATOM_Button *m_btnClearAngleL;
	ATOM_Button *m_btnClearAngleG;
	ATOM_Button *m_btnClose;
	ATOM_ComboBox *m_cbBrushMode;
//%%EndVarList

	ATOM_Dialog *_dialog;
	PluginScene *_plugin;

	ATOM_DECLARE_EVENT_MAP(DlgEditBlocks, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_SCENE_BLOCK_EDITOR_H
