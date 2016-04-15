#include "StdAfx.h"
#include "rename.h"

ATOM_BEGIN_EVENT_MAP(RenameDialog, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(RenameDialog, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

RenameDialog::RenameDialog (void)
{
	_dlg = 0;
}

RenameDialog::~RenameDialog (void)
{
	if (_dlg)
	{
		_dlg->setEventTrigger (0);
		ATOM_DELETE(_dlg);
	}
}

const char *RenameDialog::prompt (ATOM_Widget *parent, const char *title, const char *oldName)
{
	if (!_dlg)
	{
		_dlg = (ATOM_Dialog*)parent->loadHierarchy ("/editor/ui/rename.ui");
	}

	if (_dlg)
	{
		initControls (_dlg);

		_dlg->setEventTrigger (this);
		int x = (parent->getWidgetRect().size.w - _dlg->getWidgetRect().size.w)/2;
		int y = (parent->getWidgetRect().size.h - _dlg->getWidgetRect().size.h)/2;
		_dlg->moveTo (x, y);
		m_NewName->setString (oldName ? oldName : "");

		if (ID_OK == _dlg->showModal ())
		{
			_newName = m_NewName->getString();

			return _newName.c_str();
		}
	}

	return 0;
}

void RenameDialog::onCommand (ATOM_WidgetCommandEvent *event)
{
	_dlg->endModal (event->id);
}

