#include "StdAfx.h"
#include "editor.h"
#include "dlg_new_particlesys.h"

ATOM_BEGIN_EVENT_MAP(DlgNewParticleSystem, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(DlgNewParticleSystem, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

DlgNewParticleSystem::DlgNewParticleSystem (AS_Editor *editor)
{
	_editor = editor;
}

DlgNewParticleSystem::~DlgNewParticleSystem (void)
{
}

bool DlgNewParticleSystem::prompt (ATOM_Widget *parent)
{
	ATOM_Dialog *dialog = (ATOM_Dialog*)parent->loadHierarchy ("/editor/ui/composition_create_with_filename.ui");

	if (dialog)
	{
		unsigned w1 = parent->getWidgetRect().size.w;
		unsigned h1 = parent->getWidgetRect().size.h;
		unsigned w2 = dialog->getWidgetRect().size.w;
		unsigned h2 = dialog->getWidgetRect().size.h;
		unsigned x = (w1 - w2) / 2;
		unsigned y = (h1 - h2) / 2;

		dialog->moveTo (x, y);
		dialog->setEventTrigger (this);
		initControls (dialog);

		if (dialog->showModal () == DlgNewParticleSystem::ID_OK)
		{
			_fileName = m_FileName->getString ();
			_name = m_Name->getString ();

			_node = ATOM_HARDREF(ATOM_ParticleSystem)();

			if (!_fileName.empty ())
			{
				_node->loadFromFile (ATOM_GetRenderDevice(), _fileName.c_str());
			}
			else
			{
				_node->loadAttribute (NULL);
				_node->load (ATOM_GetRenderDevice());
			}
		}

		dialog->setEventTrigger (NULL);
		ATOM_DELETE(dialog);

		return _node.get() != NULL;
	}

	return false;
}

const char *DlgNewParticleSystem::getFileName (void) const
{
	return _fileName.c_str();
}

const char *DlgNewParticleSystem::getName (void) const
{
	return _name.c_str();
}

ATOM_ParticleSystem *DlgNewParticleSystem::getParticleSystemNode (void) const
{
	return _node.get();
}

void DlgNewParticleSystem::onCommand (ATOM_WidgetCommandEvent *event)
{
	ATOM_Dialog *dialog = (ATOM_Dialog*)getHost();

	if (event->id == ID_OK)
	{
		ATOM_STRING name = m_Name->getString ();
		if (!name.empty ())
		{
			dialog->endModal (ID_OK);
		}
	}
	else if (event->id == ID_CANCEL)
	{
		dialog->endModal (ID_CANCEL);
	}
}

