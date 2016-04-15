#ifndef __ATOM3D_STUDIO_DLG_NEW_PARTICLESYS_H
#define __ATOM3D_STUDIO_DLG_NEW_PARTICLESYS_H

class DlgNewParticleSystem: public ATOM_EventTrigger
{
public:
//%%BeginIDList
	enum {
		ID_NAME = 103,
		ID_FILENAME = 108,
		ID_LOADFILE = 104,
		ID_OK = 102,
		ID_CANCEL = 105,
	};
//%%EndIDList
public:
	DlgNewParticleSystem (AS_Editor *editor);
	~DlgNewParticleSystem (void);

public:
	bool prompt (ATOM_Widget *parent);
	const char *getFileName (void) const;
	const char *getName (void) const;
	ATOM_ParticleSystem *getParticleSystemNode (void) const;

private:
//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_Name = (ATOM_Edit*)parent->getChildByIdRecursive(ID_NAME);
		m_FileName = (ATOM_Edit*)parent->getChildByIdRecursive(ID_FILENAME);
		m_LoadFile = (ATOM_Button*)parent->getChildByIdRecursive(ID_LOADFILE);
		m_Ok = (ATOM_Button*)parent->getChildByIdRecursive(ID_OK);
		m_Cancel = (ATOM_Button*)parent->getChildByIdRecursive(ID_CANCEL);
	}
//%%EndInitFunc

public:
	void onCommand (ATOM_WidgetCommandEvent *event);

private:
	AS_Editor *_editor;
	ATOM_STRING _fileName;
	ATOM_STRING _name;
	ATOM_AUTOREF(ATOM_ParticleSystem) _node;

//%%BeginVarList
	ATOM_Edit *m_Name;
	ATOM_Edit *m_FileName;
	ATOM_Button *m_LoadFile;
	ATOM_Button *m_Ok;
	ATOM_Button *m_Cancel;
//%%EndVarList

	ATOM_DECLARE_EVENT_MAP(DlgNewParticleSystem, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_DLG_NEW_PARTICLESYS_H
