#include "StdAfx.h"
#include "blockeditor.h"
#include "atom3d_studio.h"
#include "plugin.h"
#include "gridnode.h"
#include "plugin_scene.h"

ATOM_BEGIN_EVENT_MAP(DlgEditBlocks, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(DlgEditBlocks, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(DlgEditBlocks, ATOM_ComboBoxSelectionChangedEvent, onBlockTypeChanged)
ATOM_END_EVENT_MAP

DlgEditBlocks::DlgEditBlocks (PluginScene *plugin, ATOM_Widget *parent)
{
	_dialog = (ATOM_Dialog*)parent->loadHierarchy ("/editor/ui/editblocks.ui");
	_plugin = plugin;

	if (_dialog)
	{
		_dialog->show (ATOM_Widget::Hide);
		_dialog->setEventTrigger (this);
		initControls (_dialog);

		m_cbBlockTypes->getDropDownList()->addItem ("ÕÏ°­", BT_Block);
		m_cbBlockTypes->getDropDownList()->addItem ("·Ç°²È«Çø", BT_Unsafe);
		m_cbBlockTypes->getDropDownList()->addItem ("°ÚÌ¯Çø", BT_Stall);
		m_cbBlockTypes->select (0);

		m_cbBrushMode->getDropDownList()->addItem ("Ìî³ä", BM_FILL);
		m_cbBrushMode->getDropDownList()->addItem ("²Á³ý", BM_CLEAR);
		m_cbBrushMode->select (0);
	}
}

DlgEditBlocks::~DlgEditBlocks (void)
{
	ATOM_DELETE(_dialog);
}

BlockType DlgEditBlocks::getBlockType (void) const
{
	int selected = m_cbBlockTypes->getSelected ();
	if (selected >= 0 && selected < m_cbBlockTypes->getDropDownList()->getItemCount())
	{
		return (BlockType)(int)m_cbBlockTypes->getDropDownList()->getItemData (selected);
	}
	return BT_None;
}

int DlgEditBlocks::getBrushSize (void) const
{
	return m_sbBrushSize->getPosition () * 2 + 1;
}

ATOM_ColorARGB DlgEditBlocks::getBrushColor (BlockType type) const
{
	switch (type)
	{
	case BT_None:
		return ATOM_ColorARGB(0.f, 1.f, 0.f, 1.f);

	case BT_Block:
		return ATOM_ColorARGB(1.f, 0.f, 0.f, 1.f);

	case BT_Unsafe:
		return ATOM_ColorARGB(1.f, 0.5f, 0.f, 1.f);

	case BT_Stall:
		return ATOM_ColorARGB(0.f, 1.f, 1.f, 1.f);

	default:
		return ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f);
	}
}

BrushMode DlgEditBlocks::getBrushMode (void) const
{
	return (BrushMode)m_cbBrushMode->getDropDownList()->getItemData (m_cbBrushMode->getSelected());
}

int DlgEditBlocks::show (bool b)
{
	if (_dialog)
	{
		_dialog->show (b ? ATOM_Widget::ShowNormal : ATOM_Widget::Hide);
	}

	return 0;
}

bool DlgEditBlocks::isShown (void) const
{
	return _dialog && _dialog->getShowState() != ATOM_Widget::Hide;
}

void DlgEditBlocks::onCommand (ATOM_WidgetCommandEvent *event)
{
	bool set = (int)m_cbBrushMode->getDropDownList()->getItemData (m_cbBrushMode->getSelected()) == BM_FILL;

	if (event->id == ID_BTNCLOSE)
	{
		_dialog->show (ATOM_Widget::Hide);
	}
	else if (event->id == ID_BTNCLEAR)
	{
		_plugin->clearBlocks (set);
	}
	else if (event->id == ID_BTNCLEARANGLEL)
	{
		_plugin->clearBlockLessAngle (m_sbAngle->getPosition(), set);
	}
	else if (event->id == ID_BTNCLEARANGLEG)
	{
		_plugin->clearBlockGreaterAngle (m_sbAngle->getPosition(), set);
	}
}

void DlgEditBlocks::onBlockTypeChanged (ATOM_ComboBoxSelectionChangedEvent *event)
{
	if (event->id == ID_CBBLOCKTYPES)
	{
		_plugin->changeBlockType (getBlockType ());
	}
}

