#ifndef __ATOM3D_STUDIO_DLGLOADNPC_H
#define __ATOM3D_STUDIO_DLGLOADNPC_H

class HeightFieldBBoxTree;

class DlgLoadNPC: public ATOM_EventTrigger
{
public:
//%%BeginIDList
	enum {
		ID_EDNPCCONFIG = 102,
		ID_BTNCHOOSENPCCONFIG = 103,
		ID_EDMAPCONFIG = 105,
		ID_BTNCHOOSEMAPCONFIG = 106,
		ID_EDMODELCONFIG = 108,
		ID_BTNCHOOSEMODELCONFIG = 109,
		ID_BTNOK = 110,
		ID_BTNCANCEL = 111,
		ID_EDMONSTERCONFIG = 104,
		ID_BTNCHOOSEMONSTERCONFIG = 107,
	};
//%%EndIDList

public:
//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_edNPCConfig = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDNPCCONFIG);
		m_btnChooseNPCConfig = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCHOOSENPCCONFIG);
		m_edMapConfig = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDMAPCONFIG);
		m_btnChooseMapConfig = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCHOOSEMAPCONFIG);
		m_edModelConfig = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDMODELCONFIG);
		m_btnChooseModelConfig = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCHOOSEMODELCONFIG);
		m_btnOK = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNOK);
		m_btnCancel = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCANCEL);
		m_edMonsterConfig = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDMONSTERCONFIG);
		m_btnChooseMonsterConfig = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCHOOSEMONSTERCONFIG);
	}
//%%EndInitFunc

public:
	DlgLoadNPC (const char *sceneFileName, ATOM_Widget *parent, ATOM_Node *npcRoot);
	virtual ~DlgLoadNPC (void);

public:
	void showModal (HeightFieldBBoxTree *hf);
	void onCommand (ATOM_WidgetCommandEvent *event);
	bool save (void);

private:
	bool getModelFileName (int modelId, ATOM_STRING &s) const;
	int getCurrentSceneId (void) const;
	bool getMonsterInfo (int monsterId, int &modelId, float &scale) const;
	ATOM_Node *findNPC (int npcId);
	void load (const char *npcConfigFileName, int sceneId);
	ATOM_Node *loadModel (const char *filename, ATOM_Node *parent) const;

private:
//%%BeginVarList
	ATOM_Edit *m_edNPCConfig;
	ATOM_Button *m_btnChooseNPCConfig;
	ATOM_Edit *m_edMapConfig;
	ATOM_Button *m_btnChooseMapConfig;
	ATOM_Edit *m_edModelConfig;
	ATOM_Button *m_btnChooseModelConfig;
	ATOM_Button *m_btnOK;
	ATOM_Button *m_btnCancel;
	ATOM_Edit *m_edMonsterConfig;
	ATOM_Button *m_btnChooseMonsterConfig;
//%%EndVarList

	ATOM_STRING _npcConfigFileName;
	ATOM_AUTOREF(ATOM_Node) _npcRoot;
	ATOM_STRING _sceneFileName;
	HeightFieldBBoxTree *_heightField;
	ATOM_Dialog *_dlgLoadNPC;

	ATOM_DECLARE_EVENT_MAP(DlgLoadNPC, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_DLGLOADNPC_H
