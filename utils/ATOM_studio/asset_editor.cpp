#include "StdAfx.h"
#include "asset_editor.h"
#include "editor.h"
#include "events.h"
#include "assetmanager.h"
#include "axisnode.h"
#include "trackballnode.h"
#include "editor_impl.h"

class AssetEditorCommandCallback: public ATOMX_TweakBar::CommandCallback
{
	AssetEditor *_assetEditor;

public:
	AssetEditorCommandCallback (AssetEditor *editor): _assetEditor(editor)
	{
	}

	void callback (ATOMX_TWCommandEvent *event)
	{
		switch (event->id)
		{
		case ASSETEDITOR_COMMAND_LOAD:
			{
				_assetEditor->load ();
				break;
			}
		case ASSETEDITOR_COMMAND_SAVE:
			{
				_assetEditor->save ();
				break;
			}
		case ASSETEDITOR_COMMAND_SAVEAS:
			{
				_assetEditor->saveAs ();
				break;
			}
		case ASSETEDITOR_COMMAND_NEW:
			{
				_assetEditor->newAsset ();
				break;
			}
		case ASSETEDITOR_COMMAND_CLEAR:
			{
				_assetEditor->deleteAll ();
				break;
			}
		default:
			{
				if (event->id <= ASSETEDITOR_COMMAND_CREATE_ADDITIONAL)
				{
					int index = ASSETEDITOR_COMMAND_CREATE_ADDITIONAL - event->id;
					const char *name = _assetEditor->getAssetManagerName (index);
					const char *desc = _assetEditor->getAssetManagerDesc (index);
					AS_AssetCreateEvent e(name, 0, desc, 0.f, 0.f, 0.f, 0.f);
					_assetEditor->getEditor()->handleEvent (&e);
				}
				else if (event->id >= 0 && event->id < _assetEditor->getNumAssets())
				{
					if (!strnicmp (event->name.c_str(), "Delete", 6))
					{
						_assetEditor->deleteAsset (event->id);
					}
					else if (!strnicmp (event->name.c_str(), "Create", 6))
					{
						AS_AssetCreateEvent e(
							_assetEditor->getAssetName(event->id), 
							_assetEditor->getAssetFileName(event->id), 
							_assetEditor->getAssetDesc(event->id),
							_assetEditor->getAssetRotateX(event->id),
							_assetEditor->getAssetRotateY(event->id),
							_assetEditor->getAssetRotateZ(event->id),
							_assetEditor->getAssetOffsetY(event->id)
							);
						_assetEditor->getEditor()->handleEvent (&e);
					}
					else if (!strnicmp (event->name.c_str(), "Replace", 6))
					{
						AS_AssetReplaceEvent e(
							_assetEditor->getAssetName(event->id), 
							_assetEditor->getAssetFileName(event->id), 
							_assetEditor->getAssetDesc(event->id)
							);
						_assetEditor->getEditor()->handleEvent (&e);
					}
				}
				break;
			}
		}
	}
};


AssetEditor::AssetEditor (EditorImpl *editor)
: _bar(0)
, _editor (editor)
{
	const ATOM_HASHMAP<ATOM_STRING, AS_AssetManager*> &managerMap = _editor->getAssetManagerMap ();
	for (ATOM_HASHMAP<ATOM_STRING, AS_AssetManager*>::const_iterator it = managerMap.begin(); it != managerMap.end(); ++it)
	{
		for (unsigned i = 0; i < it->second->getNumFileExtensions(); ++i)
		{
			if (!it->second->getFileExtension (i))
			{
				_additionalAssetManagers.push_back (std::pair<ATOM_STRING,ATOM_STRING>(it->first, it->second->getFileDesc(i)));
			}
		}
	}

	createBar ();
}

AssetEditor::~AssetEditor (void)
{
	if (_bar)
	{
		ATOM_DELETE(_bar->getCommandCallback());
		ATOM_DELETE(_bar);
		_bar = 0;
	}
}

void AssetEditor::clear (void)
{
	if (_bar)
	{
		_bar->clear ();
	}

	_currentFileName.clear ();

	for (unsigned i = 0; i < _assets.size(); ++i)
	{
		ATOM_DELETE(_assets[i]);
	}
	_assets.clear ();
}

bool AssetEditor::load (void)
{
	ATOM_FileDlg dlg(0, "/", NULL, NULL, "ass文件|*.ass|", _editor->getRenderWindow()->getWindowInfo()->handle);
	if (dlg.doModal () == IDOK)
	{
		return load (dlg.getSelectedFileName(0));
	}
	return false;
}

bool AssetEditor::load (const char *filename)
{
	clear ();

	ATOM_VECTOR<char> buffer;
	if (!ATOMX_ReadFile (filename, true, buffer))
	{
		return false;
	}

	ATOM_TiXmlDocument doc;
	doc.Parse (&buffer[0]);
	if (doc.Error())
	{
		return false;
	}

	ATOM_TiXmlElement *elementRoot = doc.RootElement ();
	ATOM_TiXmlElement *elementAsset = elementRoot->FirstChildElement ("Asset");
	while (elementAsset)
	{
		const char *assetName = elementAsset->Attribute ("Name");
		const char *assetFileName = elementAsset->Attribute ("File");
		const char *assetDesc = elementAsset->Attribute ("Desc");
		double rotateX = 0;
		double rotateY = 0;
		double rotateZ = 0;
		double offsetY = 0;
		elementAsset->Attribute ("RotateX", &rotateX);
		elementAsset->Attribute ("RotateY", &rotateY);
		elementAsset->Attribute ("RotateZ", &rotateZ);
		elementAsset->Attribute ("OffsetY", &offsetY);

		if (assetName && assetFileName)
		{
			_assets.resize (_assets.size() + 1);
			_assets.back() = ATOM_NEW(AssetInfo);
			_assets.back()->filename = assetFileName;
			_assets.back()->name = assetName;
			_assets.back()->desc = assetDesc ? assetDesc : assetName;
			_assets.back()->rotateX = rotateX;
			_assets.back()->rotateY = rotateY;
			_assets.back()->rotateZ = rotateZ;
			_assets.back()->offsetY = offsetY;
		}
		elementAsset = elementAsset->NextSiblingElement ("Asset");
	}

	_currentFileName = filename;

	if (_bar)
	{
		_bar->clear ();
	}

	createBar ();

	return true;
}

bool AssetEditor::save (void)
{
	if (_currentFileName.empty ())
	{
		return saveAs ();
	}
	else
	{
		return save (_currentFileName.c_str());
	}
}

bool AssetEditor::saveAs (void)
{
	ATOM_FileDlg dlg(ATOM_FileDlg::MODE_SAVEFILE|ATOM_FileDlg::MODE_OVERWRITEPROMPT, NULL, "ass", NULL, "ass文件|*.ass|", _editor->getRenderWindow()->getWindowInfo()->handle);
	if (dlg.doModal () == IDOK)
	{
		_currentFileName = dlg.getSelectedFileName(0);
		return save (_currentFileName.c_str());
	}

	return false;
}

bool AssetEditor::save (const char *filename)
{
	if (!filename)
	{
		return false;
	}

	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, buffer);

	ATOM_TiXmlDocument doc(buffer);
	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement eRoot("Assets");
	for (unsigned i = 0; i < _assets.size(); ++i)
	{
		ATOM_TiXmlElement eAsset("Asset");
		eAsset.SetAttribute ("Name", _assets[i]->name.c_str());
		eAsset.SetAttribute ("File", _assets[i]->filename.c_str());
		eAsset.SetAttribute ("Desc", _assets[i]->desc.c_str());
		eAsset.SetDoubleAttribute ("RotateX", _assets[i]->rotateX);
		eAsset.SetDoubleAttribute ("RotateY", _assets[i]->rotateY);
		eAsset.SetDoubleAttribute ("RotateZ", _assets[i]->rotateZ);
		eAsset.SetDoubleAttribute ("OffsetY", _assets[i]->offsetY);
		eRoot.InsertEndChild (eAsset);
	}

	doc.InsertEndChild (eRoot);

	return doc.SaveFile ();
}

void AssetEditor::setPosition (int x, int y, int w, int h)
{
	if (!_bar)
	{
		createBar ();
	}

	_bar->setBarPosition (x, y);
	_bar->setBarSize (w, h);
}

unsigned AssetEditor::getNumAssets (void) const
{
	return _assets.size ();
}

const char *AssetEditor::getAssetName (unsigned index) const
{
	return _assets[index]->name.c_str();
}

const char *AssetEditor::getAssetFileName (unsigned index) const
{
	return _assets[index]->filename.c_str();
}

const char *AssetEditor::getAssetDesc (unsigned index) const
{
	return _assets[index]->desc.c_str();
}

float AssetEditor::getAssetRotateX (unsigned index) const
{
	return _assets[index]->rotateX;
}

float AssetEditor::getAssetRotateY (unsigned index) const
{
	return _assets[index]->rotateY;
}

float AssetEditor::getAssetRotateZ (unsigned index) const
{
	return _assets[index]->rotateZ;
}

float AssetEditor::getAssetOffsetY (unsigned index) const
{
	return _assets[index]->offsetY;
}

void AssetEditor::newAsset (void)
{
	unsigned num = _editor->getOpenAssetFileNames (true, false, "选择游戏资源");
	for (unsigned i = 0; i < num; ++i)
	{
		const char *filename = _editor->getOpenedAssetFileName(i);

		bool exists = false;
		for (unsigned n = 0; n < _assets.size(); ++n)
		{
			if (!stricmp (_assets[n]->filename.c_str(), filename))
			{
				exists = true;
				break;
			}
		}

		if (!exists)
		{
			_assets.resize (_assets.size() + 1);
			_assets.back() = ATOM_NEW(AssetInfo);
			_assets.back()->filename = filename;

			const char *p = strrchr(filename, '/');
			if (p)
			{
				_assets.back()->name = p + 1;
			}
			else
			{
				_assets.back()->name = filename;
			}
			_assets.back()->desc = _assets.back()->name;
			_assets.back()->offsetY = 0.f;
			_assets.back()->rotateX = 0.f;
			_assets.back()->rotateY = 0.f;
			_assets.back()->rotateZ = 0.f;
			addAssetToBar (_assets.size() - 1);
		}
	}
}

void AssetEditor::deleteAsset (unsigned index)
{
	if (index < _assets.size())
	{
		ATOM_DELETE(_assets[index]);
		_assets.erase (_assets.begin() + index);

		if (_bar)
		{
			_bar->clear ();
		}

		createBar ();
	}
}

void AssetEditor::deleteAll (void)
{
	_assets.resize (0);

	if (_bar)
	{
		_bar->clear ();
	}

	createBar ();
}

void AssetEditor::createBar (void)
{
	if (!_bar)
	{
		_bar = ATOM_NEW(ATOMX_TweakBar, "Asset");
		_bar->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
		_bar->setBarMovable (false);
		_bar->setBarResizable (false);
		_bar->setBarIconifiable (false);
		_bar->setCommandCallback (ATOM_NEW(AssetEditorCommandCallback, this));
	}

	_bar->addButton ("load", ASSETEDITOR_COMMAND_LOAD, "Load..", "File management");
	_bar->addButton ("save", ASSETEDITOR_COMMAND_SAVE, "Save", "File management");
	_bar->addButton ("saveas", ASSETEDITOR_COMMAND_SAVEAS, "Save As..", "File management");
	_bar->addButton ("new", ASSETEDITOR_COMMAND_NEW, "New Asset..", "File management");
	_bar->addButton ("clear", ASSETEDITOR_COMMAND_CLEAR, "Clear", "File management");

	for (int i = 0; i < _additionalAssetManagers.size(); ++i)
	{
		const ATOM_STRING &str = _additionalAssetManagers[i].first;

		_bar->addButton (str.c_str(), ASSETEDITOR_COMMAND_CREATE_ADDITIONAL - i, str.c_str(), "Additional Assets");
	}

	for (int i = 0; i < _assets.size(); ++i)
	{
		addAssetToBar (i);
	}
}

void AssetEditor::addAssetToBar (unsigned index)
{
	AssetInfo *info = _assets[index];
	char buffer[64];

	sprintf (buffer, "Desc%d", index);
	_bar->addStringVar (buffer, index, info->desc.c_str(), false, info->filename.c_str());
	_bar->setVarLabel (buffer, "Desc");

	sprintf (buffer, "Delete%d", index);
	_bar->addButton (buffer, index, "Delete", info->filename.c_str());

	sprintf (buffer, "Create%d", index);
	_bar->addButton (buffer, index, "Create", info->filename.c_str());

	sprintf (buffer, "Replace%d", index);
	_bar->addButton (buffer, index, "Replace", info->filename.c_str());

	sprintf (buffer, "RotateX%d", index);
	_bar->addImmediateVariable (buffer, ATOMX_TBTYPE_FLOAT, &info->rotateX, false, info->filename.c_str());
	_bar->setVarLabel (buffer, "RotateX");
	_bar->setVarMinMax (buffer, 0.f, ATOM_TwoPi);
	_bar->setVarStep (buffer, 0.01f);
	_bar->setVarPrecision (buffer, 2);

	sprintf (buffer, "RotateY%d", index);
	_bar->addImmediateVariable (buffer, ATOMX_TBTYPE_FLOAT, &info->rotateY, false, info->filename.c_str());
	_bar->setVarLabel (buffer, "RotateY");
	_bar->setVarMinMax (buffer, 0.f, ATOM_TwoPi);
	_bar->setVarStep (buffer, 0.01f);
	_bar->setVarPrecision (buffer, 2);

	sprintf (buffer, "RotateZ%d", index);
	_bar->addImmediateVariable (buffer, ATOMX_TBTYPE_FLOAT, &info->rotateZ, false, info->filename.c_str());
	_bar->setVarLabel (buffer, "RotateZ");
	_bar->setVarMinMax (buffer, 0.f, ATOM_TwoPi);
	_bar->setVarStep (buffer, 0.01f);
	_bar->setVarPrecision (buffer, 2);

	sprintf (buffer, "OffsetY%d", index);
	_bar->addImmediateVariable (buffer, ATOMX_TBTYPE_FLOAT, &info->offsetY, false, info->filename.c_str());
	_bar->setVarLabel (buffer, "OffsetY");
	_bar->setVarMinMax (buffer, 0.f, 100.f);
	_bar->setVarStep (buffer, 0.01f);
	_bar->setVarPrecision (buffer, 2);

	_bar->setGroupOpenState (info->filename.c_str(), false);
}

void AssetEditor::show (bool b)
{
	if (!b)
	{
		if (_bar)
		{
			_bar->setBarVisible (false);
		}
	}
	else
	{
		if (!_bar)
		{
			createBar ();
		}

		_bar->setBarVisible (true);
	}
}

EditorImpl *AssetEditor::getEditor (void) const
{
	return _editor;
}

const char *AssetEditor::getAssetManagerName (int index) const
{
	return _additionalAssetManagers[index].first.c_str();
}

const char *AssetEditor::getAssetManagerDesc (int index) const
{
	return _additionalAssetManagers[index].second.c_str();
}

