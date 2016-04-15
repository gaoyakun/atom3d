#ifndef __ATOM3D_STUDIO_RENAME_H
#define __ATOM3D_STUDIO_RENAME_H

class RenameDialog: public ATOM_EventTrigger
{
public:
//%%BeginIDList
	enum {
		ID_NEWNAME = 102,
		ID_OK = 103,
		ID_CANCEL = 104,
	};
//%%EndIDList

public:
	RenameDialog (void);
	virtual ~RenameDialog (void);

public:
	const char *prompt (ATOM_Widget *parent, const char *title, const char *oldName);

public:
	void onCommand (ATOM_WidgetCommandEvent *event);

public:
//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_NewName = (ATOM_Edit*)parent->getChildByIdRecursive(ID_NEWNAME);
		m_OK = (ATOM_Button*)parent->getChildByIdRecursive(ID_OK);
		m_Cancel = (ATOM_Button*)parent->getChildByIdRecursive(ID_CANCEL);
	}
//%%EndInitFunc

private:
//%%BeginVarList
	ATOM_Edit *m_NewName;
	ATOM_Button *m_OK;
	ATOM_Button *m_Cancel;
//%%EndVarList

	ATOM_Dialog *_dlg;
	ATOM_STRING _newName;

	ATOM_DECLARE_EVENT_MAP(RenameDialog, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_RENAME_H
