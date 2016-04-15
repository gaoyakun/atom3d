#ifndef __ATOM3D_STUDIO_TERRAIN_TOOLS_DETAIL_NORMAL_H
#define __ATOM3D_STUDIO_TERRAIN_TOOLS_DETAIL_NORMAL_H

#if _MSC_VER > 1000
# pragma once
#endif

class DlgTerrainDetailNormal: public ATOM_EventTrigger
{
	//%%BeginIDList
	enum {
		ID_EDFILE = 102,
		ID_BTNSELECTFILE = 103,
		ID_EDPOWER = 104,
		ID_EDGLOSS = 105,
		ID_BTNOK = 106,
		ID_BTNCANCEL = 107,
	};
	//%%EndIDList

public:
	//%%BeginInitFunc
	void initControls(ATOM_Widget *parent);
	//%%EndInitFunc

public:
	DlgTerrainDetailNormal ( ATOM_Widget *parent, ATOM_AUTOREF(ATOM_Terrain) terrain );
	virtual ~DlgTerrainDetailNormal (void);

public:
	//bool showModal (void);
	int show (bool b, int detailId);
	bool isShow();

	ATOM_STRING getFile (void) const;
	float getPower (void) const;
	float getGloss (void) const;

public:
	void onCommand (ATOM_WidgetCommandEvent *event);

private:
	//%%BeginVarList
	ATOM_Edit	*	m_edFile;
	ATOM_Button *	m_btnSelectFile;
	ATOM_Edit	*	m_edPower;
	ATOM_Edit	*	m_edGloss;
	ATOM_Button *	m_btnOk;
	ATOM_Button *	m_btnCancel;
	//%%EndVarList

	ATOM_Dialog *				_dialog;
	ATOM_AUTOREF(ATOM_Terrain)	_terrain;
	int							_detailId;

	bool						_isShow;

	ATOM_DECLARE_EVENT_MAP(DlgTerrainDetailNormal, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_ASSET_TERRAIN_H
