#include "StdAfx.h"
#include "dlgterraindetailnormal.h"

ATOM_BEGIN_EVENT_MAP(DlgTerrainDetailNormal, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(DlgTerrainDetailNormal, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

DlgTerrainDetailNormal::DlgTerrainDetailNormal(ATOM_Widget *parent,ATOM_AUTOREF(ATOM_Terrain) terrain)
{
	if( terrain )
		_terrain = terrain;
	
	_detailId = -1;

	_dialog = (ATOM_Dialog*)parent->loadHierarchy ("/editor/ui/terrain_detail_normal.ui");
	if (_dialog)
	{
		initControls (_dialog);
		_dialog->setEventTrigger (this);
	}

	_isShow = false;
}
DlgTerrainDetailNormal::~DlgTerrainDetailNormal (void)
{
	if(_dialog )
	{
		_dialog->setEventTrigger(NULL);
		ATOM_DELETE(_dialog);
	}

	_terrain = 0;
}
void DlgTerrainDetailNormal::initControls(ATOM_Widget *parent) 
{
	m_edFile		= (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDFILE);
	m_edFile->setString (_terrain->getDetailNormalMap(_detailId).c_str());

	m_btnSelectFile = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNSELECTFILE);

	m_edPower		= (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDPOWER);
	m_edPower->setString("64.0");

	m_edGloss		= (ATOM_Edit*)parent->getChildByIdRecursive(ID_EDGLOSS);
	m_edGloss->setString("0.03");

	m_btnOk			= (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNOK);

	m_btnCancel		= (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNCANCEL);
}

//bool DlgTerrainDetailNormal::showModal (void)
//{
//	return _dialog ? _dialog->showModal() == ID_BTNOK : false;
//}

int DlgTerrainDetailNormal::show (bool b, int detailId)
{
	if( _isShow && b && _detailId==detailId )
		return 0;

	if( !_isShow && !b)
		return 0;

	if( b )
	{
		_detailId = detailId;
		m_edFile->setString (_terrain->getDetailNormalMap(_detailId).c_str());

		char buffer_p[64] ={0};
		sprintf(buffer_p,"%.1f",_terrain->getDetailSpecPower(_detailId));
		m_edPower->setString(buffer_p);

		char buffer_g[64] ={0};
		sprintf(buffer_g,"%.4f",_terrain->getDetailGloss(_detailId));
		m_edGloss->setString(buffer_g);
	}
	else
	{
		_detailId = -1;
	}
	
	if (_dialog)
	{
		_dialog->show ( b ? ATOM_Widget::ShowNormal : ATOM_Widget::Hide);
	}

	_isShow = b;

	return 0;
}
bool DlgTerrainDetailNormal::isShow()
{
	return _isShow;
}
ATOM_STRING DlgTerrainDetailNormal::getFile (void) const
{
	return m_edFile->getString ();
}
float DlgTerrainDetailNormal::getPower (void) const
{
	ATOM_STRING p = m_edPower->getString ();
	float power = p.empty () ? 64.f : atof (p.c_str());
	return power;
}
float DlgTerrainDetailNormal::getGloss (void) const
{
	ATOM_STRING g = m_edGloss->getString ();
	float gloss = g.empty () ? 0.0f : atof (g.c_str());
	return gloss;
}
void DlgTerrainDetailNormal::onCommand (ATOM_WidgetCommandEvent *event)
{
	switch (event->id)
	{
	case ID_BTNSELECTFILE:
		{
			//ATOM_FileDlg dlg(ATOM_FileDlg::MODE_OPENDIR);

			const char *str = "DDS文件|*.dds|JPG文件|*.jpg|PNG文件|*.png|TGA文件|*.tga|所有文件|*.*|";
			int mode = 0;
			ATOM_FileDlg dlg(mode, 0, "dds", 0, str);	
			dlg.setTitle ("请选择法线贴图");

			if (IDOK == dlg.doModal ())
			{
				m_edFile->setString (dlg.getSelectedFileName(0));
			}
			break;
		}
	case ID_BTNOK:
		{
			ATOM_STRING file = m_edFile->getString ();
			_terrain->setDetailNormalMap(file,_detailId);

			float power = getPower ();
			if ( power < 0.f || power > 255 )
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "Power值错误(应在0-255)!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				return;
			}
			_terrain->setDetailSpecPower(power,_detailId);

			float gloss = getGloss();
			if ( gloss < 0.f )
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "Gloss值错误(应大于0)!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				return;
			}
			_terrain->setDetailGloss(gloss,_detailId);
			
			//_dialog->endModal (ID_BTNOK);
			this->show(false,-1);

			break;
		}
	case ID_BTNCANCEL:
		{
			//_dialog->endModal (ID_BTNCANCEL);
			this->show(false,-1);

			break;
		}
	}
}

/*
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
*/

