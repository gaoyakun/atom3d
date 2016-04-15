#ifndef __ATOM3D_STUDIO_CURVED_H__
#define __ATOM3D_STUDIO_CURVED_H__

class AS_Editor;

class CurveEd: public ATOM_EventTrigger
{
private:
	//%%BeginIDList
	enum {
		ID_CECURVE = 101,
		ID_BTNSAVE = 102,
		ID_BTNCLOSE = 103,
		ID_BTNLOAD = 104,
	};
//%%EndIDList

	//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_ceCurve = parent ? (ATOM_CurveEditor*)parent->getChildByIdRecursive(ID_CECURVE) : 0;
		m_btnSave = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNSAVE) : 0;
		m_btnClose = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCLOSE) : 0;
		m_btnLoad = parent ? (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNLOAD) : 0;
	}
//%%EndInitFunc

	//%%BeginVarList
	ATOM_CurveEditor *m_ceCurve;
	ATOM_Button *m_btnSave;
	ATOM_Button *m_btnClose;
	ATOM_Button *m_btnLoad;
//%%EndVarList

private:
	ATOM_Dialog *_dialog;
	AS_Editor *_editor;

public:
	CurveEd (AS_Editor *editor, ATOM_Widget *parent);
	virtual ~CurveEd (void);

	void show (void);
	void hide (void);
	const ATOM_FloatAnimationCurve &getCurve (void) const;
	void setCurve (const ATOM_FloatAnimationCurve &curve);
	bool save (const char *filename);
	bool load (const char *filename);

	void onCommand (ATOM_WidgetCommandEvent *event);

	ATOM_DECLARE_EVENT_MAP(CurveEd, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_CURVED_H__
