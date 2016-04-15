#include "StdAfx.h"
#include "asset.h"
#include "editor.h"
#include "assetmanager.h"
#include "asset_terrain.h"

AssetTerrain::AssetTerrain (ATOM_Terrain *node)
{
	_terrainNode = node;
	_terrainNode->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());
}

bool AssetTerrain::loadFromFile (const char *filename)
{
	return true;
}

void AssetTerrain::deleteMe (void)
{
	ATOM_DELETE(this);
}

ATOM_Node *AssetTerrain::getNode (void)
{
	return _terrainNode.get();
}

bool AssetTerrain::isPickable (void) const
{
	return true;
}

bool AssetTerrain::supportBrushing (void) const
{
	return false;
}

class DlgNewTerrain: public ATOM_EventTrigger
{
//%%BeginIDList
	enum {
		ID_EDDIR = 102,
		ID_BTNSELECTDIR = 103,
		ID_CBWIDTH = 104,
		ID_CBHEIGHT = 101,
		ID_EDSCALE = 108,
		ID_EDMAXHEIGHT = 107,
		ID_BTNOK = 105,
		ID_BTNCANCEL = 106,
		ID_LABEL6 = 109,
		ID_EDEFAULTHEIGHT = 110,
	};
//%%EndIDList

public:
//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_edDir = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDDIR);
		m_btnSelectDir = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNSELECTDIR);
		m_cbWidth = (ATOM_ComboBox*)parent->getChildByIdRecursive(ID_CBWIDTH);
		m_cbHeight = (ATOM_ComboBox*)parent->getChildByIdRecursive(ID_CBHEIGHT);
		m_edScale = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDSCALE);
		m_edMaxHeight = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDMAXHEIGHT);
		m_btnOk = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNOK);
		m_btnCancel = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCANCEL);
		m_label6 = (ATOM_Label*)parent->getChildByIdRecursive(ID_LABEL6);
		m_eDefaultHeight = (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDEFAULTHEIGHT);
	}
//%%EndInitFunc

public:
	DlgNewTerrain (ATOM_Widget *parent)
	{
		_dialog = (ATOM_Dialog*)parent->loadHierarchy ("/editor/ui/newterrain.ui");
		if (_dialog)
		{
			initControls (_dialog);
			_dialog->setEventTrigger (this);

			m_cbWidth->getDropDownList()->addItem ("65", 65);
			m_cbWidth->getDropDownList()->addItem ("129", 129);
			m_cbWidth->getDropDownList()->addItem ("257", 257);
			m_cbWidth->getDropDownList()->addItem ("513", 513);
			m_cbWidth->getDropDownList()->addItem ("1025", 1025);
			m_cbWidth->select(2);

			m_cbHeight->getDropDownList()->addItem ("65", 65);
			m_cbHeight->getDropDownList()->addItem ("129", 129);
			m_cbHeight->getDropDownList()->addItem ("257", 257);
			m_cbHeight->getDropDownList()->addItem ("513", 513);
			m_cbHeight->getDropDownList()->addItem ("1025", 1025);
			m_cbHeight->select(2);
		}
	}

	virtual ~DlgNewTerrain (void)
	{
		ATOM_DELETE(_dialog);
	}

public:
	bool showModal (void)
	{
		return _dialog ? _dialog->showModal() == ID_BTNOK : false;
	}

	ATOM_STRING getDir (void) const
	{
		return m_edDir->getString ();
	}

	int getWidth (void) const
	{
		int sel = m_cbWidth->getSelected ();
		ATOM_ASSERT(sel >= 0);
		return 64 * (1 << sel) + 1;
	}

	int getHeight (void) const
	{
		int sel = m_cbHeight->getSelected ();
		ATOM_ASSERT(sel >= 0);
		return 64 * (1 << sel) + 1;
	}

	ATOM_Vector3f getScale (void) const
	{
		ATOM_STRING s = m_edScale->getString ();
		float h = s.empty () ? 0.f : atof (s.c_str());

		s = m_edMaxHeight->getString ();
		float v = s.empty () ? 0.f : atof (s.c_str());

		return ATOM_Vector3f(h, v/65536.f, h);
	}
	float getDefaultHeight(void)const
	{
		ATOM_STRING str = m_eDefaultHeight->getString();
		return (str.empty()?0.f:atof(str.c_str()));
	}

public:
	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		switch (event->id)
		{
		case ID_BTNSELECTDIR:
			{
				ATOM_FileDlg dlg(ATOM_FileDlg::MODE_OPENDIR);
				if (IDOK == dlg.doModal ())
				{
					m_edDir->setString (dlg.getSelectedFileName(0));
				}
				break;
			}
		case ID_BTNOK:
			{
				int w = getWidth ();
				int h = getHeight ();
				if (w != h)
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "目前不支持宽高不等的地形!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
					return;
				}
				ATOM_Vector3f s = getScale ();
				if (s.x <= 0.f || s.y <= 0.f || s.z <= 0.f)
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "缩放值错误!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
					return;
				}
				ATOM_STRING dir = m_edDir->getString ();
				if (dir.empty ())
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "请选择存放目录!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
					return;
				}
				float defaultHeight = getDefaultHeight();
				float maxHeight = atof(m_edMaxHeight->getString ().c_str());
				if(defaultHeight > maxHeight)
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "初始高度不能大于最大高度!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
					return;
				}
				if (!ATOM_IsDirectory (dir.c_str()))
				{
					if (IDYES !=::MessageBoxA (ATOM_APP->getMainWindow(), "目录不存在,是否创建目录?", "ATOM3D编辑器", MB_YESNO|MB_ICONQUESTION))
					{
						return;
					}
					if (!ATOM_CreateDirectory (dir.c_str()))
					{
						::MessageBoxA (ATOM_APP->getMainWindow(), "创建目录失败!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
						return;
					}
				}
				_dialog->endModal (ID_BTNOK);
				break;
			}
		case ID_BTNCANCEL:
			_dialog->endModal (ID_BTNCANCEL);
			break;
		}
	}

private:
//%%BeginVarList
	ATOM_Edit *m_edDir;
	ATOM_Button *m_btnSelectDir;
	ATOM_ComboBox *m_cbWidth;
	ATOM_ComboBox *m_cbHeight;
	ATOM_Edit *m_edScale;
	ATOM_Edit *m_edMaxHeight;
	ATOM_Button *m_btnOk;
	ATOM_Button *m_btnCancel;
	ATOM_Label *m_label6;
	ATOM_Edit *m_eDefaultHeight;
//%%EndVarList

	ATOM_Dialog *_dialog;

	ATOM_DECLARE_EVENT_MAP(DlgNewTerrain, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(DlgNewTerrain, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(DlgNewTerrain, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

AS_Asset *TerrainAssetManager::createAsset (void)
{
	DlgNewTerrain dlgNewTerrain(_editor->getRealtimeCtrl());
	if (dlgNewTerrain.showModal ())
	{
		int w = dlgNewTerrain.getWidth();
		int h = dlgNewTerrain.getHeight();
		ATOM_STRING dir = dlgNewTerrain.getDir ();
		if (dir[dir.length()-1] != '/')
			dir += '/';
		ATOM_Vector3f scale = dlgNewTerrain.getScale ();
		ATOM_STRING hf = dir + "heightmap16bit.raw";
		{
			ATOM_AutoFile f(hf.c_str(), ATOM_VFS::write);
			if (f)
			{
				float defaultHeight = dlgNewTerrain.getDefaultHeight();
				int iHeight = (defaultHeight / scale.y);
				unsigned short usHeight = iHeight>65535?65535:iHeight;
				void *heights = ATOM_CALLOC (w * h, sizeof(unsigned short));
				unsigned short * temp = (unsigned short*)heights;
				for(int i = 0; i < w * h;++i,++temp)
					*temp= usHeight;
				f->write (heights, w * h * sizeof(unsigned short));
			}
		}

		
		ATOM_STRING tf = dir + "terrain.xml";
		char buf[ATOM_VFS::max_filename_length];
		ATOM_GetNativePathName (tf.c_str(), buf);

		ATOM_TiXmlDocument doc(buf);
		ATOM_TiXmlDeclaration decl("1.0", "gb2312", "");
		doc.InsertEndChild (decl);

		ATOM_TiXmlElement eTerrain("Terrain");
		eTerrain.SetAttribute ("HeightMap", hf.c_str());
		eTerrain.SetAttribute ("AlphaMapWidth", 512);
		eTerrain.SetAttribute ("AlphaMapHeight", 512);
		eTerrain.SetDoubleAttribute ("ScaleX", scale.x);
		eTerrain.SetDoubleAttribute ("ScaleY", scale.y);
		eTerrain.SetDoubleAttribute ("ScaleZ", scale.z);
		eTerrain.SetAttribute ("PatchSize", 33);
		doc.InsertEndChild (eTerrain);

		if (!doc.SaveFile ())
		{
			return 0;
		}

		ATOM_HARDREF(ATOM_Terrain) terrain;
		terrain->loadAttribute (NULL);
		terrain->setTerrainFileName (tf.c_str());
		if (!terrain->load (ATOM_GetRenderDevice()))
		{
			return 0;
		}

		return ATOM_NEW(AssetTerrain, terrain.get());
	}

	return NULL;
}

unsigned TerrainAssetManager::getNumFileExtensions (void) const
{
	return 1;
}

const char * TerrainAssetManager::getFileExtension (unsigned) const
{
	return NULL;
}

const char * TerrainAssetManager::getFileDesc (unsigned) const
{
	return "Terrain";
}

void TerrainAssetManager::setEditor (AS_Editor *editor)
{
	_editor = editor;
}

