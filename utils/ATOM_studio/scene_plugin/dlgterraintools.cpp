#include "StdAfx.h"
#include "dlgterraintools.h"
#include "atom3d_studio.h"
#include "plugin.h"
#include "gridnode.h"
#include "plugin_scene.h"
#include "dlgterraindetailnormal.h"	// wangjian added

ATOM_BEGIN_EVENT_MAP(DlgTerrainTools, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(DlgTerrainTools, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(DlgTerrainTools, ATOM_TopWindowCloseEvent, onClose)
	ATOM_EVENT_HANDLER(DlgTerrainTools, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	//ATOM_EVENT_HANDLER(DlgTerrainTools, ATOM_WidgetRButtonDownEvent, onRButtonDown)	// wangjian added
	ATOM_EVENT_HANDLER(DlgTerrainTools, ATOM_EditTextChangedEvent, onEditChanged)	
ATOM_END_EVENT_MAP

#define FIRST_HEIGHT_BRUSH_ID	3000
#define FIRST_TEXTURE_BRUSH_ID	4000

DlgTerrainTools::DlgTerrainTools (PluginScene *plugin, ATOM_Terrain *terrain, ATOM_Widget *parent)
{
	_plugin = plugin;
	_terrain = terrain;
	_currentFirstHeightBrush = -1;
	_currentFirstTextureBrush = -1;
	_currentFirstGrassBrush = -1;
	_currentHeightmap = 0;
	_isBrushingTexture = false;
	_isBrushingGrass = false;
	_heightmapPingPong[_currentHeightmap] = terrain->getHeightmap ();
	_heightmapPingPong[1-_currentHeightmap] = ATOM_GetRenderDevice()->allocTexture (NULL, NULL, terrain->getHeightmap()->getWidth(), terrain->getHeightmap()->getHeight(), ATOM_PIXEL_FORMAT_R32F, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::RENDERTARGET);
	_normalMap = ATOM_GetRenderDevice()->allocTexture (NULL, NULL, terrain->getHeightmap()->getWidth(), terrain->getHeightmap()->getHeight(), ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::RENDERTARGET);
	_depthbuffer = ATOM_GetRenderDevice()->allocDepthBuffer(terrain->getHeightmap()->getWidth(), terrain->getHeightmap()->getHeight());
	_depthbufferAlpha = ATOM_GetRenderDevice()->allocDepthBuffer(_terrain->getAlphaMapWidth(), _terrain->getAlphaMapHeight());
	_currentSelectedHeightBrush = 0;
	_currentSelectedTextureBrushIndex = -1;
	_currentSelectedTextureBrush = 0;
	_currentSelectedGrassBrushIndex = -1;
	_currentSelectedGrassBrush = 0;
	_numHeightBrushes = 0;
	_isBrushing = false;
	_currentTool = TOOL_RAISE;
	_savedPixelError = 0;
	_savedGrassDistance = 0;
	_isTerrainHit = false;
	_revert = false;
	//--- wangjian added ---//
	_hitHeightForLevel = 0.0f;
	//----------------------//
	_isEditingOrientation = false;
	_orientation = 0.f;
	_nextTextureBrushImageId = FIRST_TEXTURE_BRUSH_ID;
	_brushIndicator = ATOM_HARDREF(ATOM_Decal)();
	_brushIndicator->loadAttribute (NULL);
	_brushIndicator->load (ATOM_GetRenderDevice());
	ATOM_AUTOPTR(ATOM_Material) m = ATOM_MaterialManager::createMaterialFromCore(ATOM_GetRenderDevice(), "/editor/materials/decal.mat");
	m->getParameterTable()->setInt ("isMask", 1);
	m->getParameterTable()->setInt ("isArrow", 0);
	_brushIndicator->setMaterial (m.get());
	_brushIndicator->enableBlend (true);
	_brushIndicator->setSrcBlend (ATOM_RenderAttributes::BlendFunc_SrcAlpha);
	_brushIndicator->setDestBlend (ATOM_RenderAttributes::BlendFunc_InvSrcAlpha);
	_brushIndicator->setPrivate (1);

	_backupHeights.resize (terrain->getHeightmap()->getWidth() * terrain->getHeightmap()->getHeight());
	backupHeights ();
	backupAlphas ();

	// wangjian modified
#if 0
	_dialog = (ATOM_Dialog*)parent->loadHierarchy ("/editor/ui/terraheighttool.ui");
#else
	_dialog = (ATOM_Dialog*)parent->loadHierarchy ("/editor/ui/terraheighttool_new.ui");
#endif
	if (_dialog)
	{
		_dialog->show (ATOM_Widget::Hide);
		_dialog->setEventTrigger (this);
		initControls (_dialog);

		m_vsTools->setActivePage (ID_PNLHEIGHTTOOL);
		m_lbBrush1->setEventTrigger (this);
		m_lbBrush2->setEventTrigger (this);
		m_lbBrush3->setEventTrigger (this);
		m_lbBrush4->setEventTrigger (this);
		m_lbTexBrush1->setEventTrigger (this);
		m_lbTexBrush2->setEventTrigger (this);
		m_lbTexBrush3->setEventTrigger (this);
		m_lbTexBrush4->setEventTrigger (this);
		m_lbGrass1->setEventTrigger (this);
		m_lbGrass2->setEventTrigger (this);
		m_lbGrass3->setEventTrigger (this);
		m_lbGrass4->setEventTrigger (this);

		m_sbRadius->setRange (1.f, 256.f);
		m_sbRadius->setPosition (32.f);
		m_sbRadius->setStep (1.f);
		m_sbStrength->setRange (0.f, 100.f);
		m_sbStrength->setPosition (0.f);
		m_sbStrength->setStep (0.1f);
		m_sbFalloff->setRange (0.f, 1.f);
		m_sbFalloff->setPosition (0.f);
		m_sbFalloff->setStep (0.1f);

		loadBrushes ();

		if (_numHeightBrushes > 0)
		{
			setHeightBrushPaletteIndex (0);
		}

		_copyMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/editor/materials/tt_copy.mat");
		ATOM_ASSERT(_copyMaterial);
		_copyMaterial->setActiveEffect ("default");

		_raiseMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/editor/materials/tt_raise.mat");
		ATOM_ASSERT(_raiseMaterial);
		_raiseMaterial->setActiveEffect ("default");

		_smoothMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/editor/materials/tt_smooth.mat");
		ATOM_ASSERT(_smoothMaterial);
		_smoothMaterial->setActiveEffect ("default");

		_levelMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/editor/materials/tt_level.mat");
		ATOM_ASSERT(_levelMaterial);
		_levelMaterial->setActiveEffect ("default");

		_normalgenMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/editor/materials/tt_normalgen.mat");
		ATOM_ASSERT(_normalgenMaterial);
		_normalgenMaterial->setActiveEffect ("default");

		_textureMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/editor/materials/tt_texture.mat");
		ATOM_ASSERT(_textureMaterial);

		_basemapMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/editor/materials/tt_basemap.mat");
		ATOM_ASSERT(_basemapMaterial);
		_basemapMaterial->setActiveEffect ("default");
	}

	_detailNormalDlg = 0;			// wangjian added
}

DlgTerrainTools::~DlgTerrainTools (void)
{
	//--- wangjian added ---//
	if( _detailNormalDlg )
		ATOM_DELETE(_detailNormalDlg);
	//--------------------------------//

	if (_dialog)
	{
		_dialog->setEventTrigger (NULL);
		ATOM_DELETE(_dialog);
	}

	for (unsigned i = 0; i < _backupAlphas.size(); ++i)
	{
		ATOM_DELETE(_backupAlphas[i]);
	}
	_backupAlphas.clear ();

	if (_brushIndicator && _plugin->getScene())
	{
		_plugin->getScene()->getRootNode()->removeChild (_brushIndicator.get());
	}
}

int DlgTerrainTools::show (bool b)
{
	if (_dialog)
	{
		_dialog->show (b ? ATOM_Widget::ShowNormal : ATOM_Widget::Hide);

		if (b)
		{
			_plugin->getScene()->getRootNode()->appendChild (_brushIndicator.get());
		}
		else
		{
			_plugin->getScene()->getRootNode()->removeChild (_brushIndicator.get());
		}
	}
	return 0;
}

bool DlgTerrainTools::isShown (void) const
{
	return _dialog && _dialog->getShowState() != ATOM_Widget::Hide;
}

static void setHeightToolsCheckColor (ATOM_Button *button, bool check)
{
	ATOM_ColorARGB c = check ? ATOM_ColorARGB(0.8f, 0.8f, 0.8f, 1.f) : ATOM_ColorARGB(1.f, 1.f, 1.f, 1.f);
	ATOM_GUIImage *image = button->getValidImage(button->getClientImageId());
	if (image)
	{
		image->setColor (WST_NORMAL, c);
		image->setColor (WST_HOVER, c);
		image->setColor (WST_HOLD, c);
		image->setColor (WST_DISABLED, c);
	}
}

void DlgTerrainTools::checkHeightTool (int id)
{
	m_htRaise->setBorderMode (ATOM_Widget::Raise);
	setHeightToolsCheckColor (m_htRaise, false);
	m_htErode->setBorderMode (ATOM_Widget::Raise);
	setHeightToolsCheckColor (m_htErode, false);
	m_htLevel->setBorderMode (ATOM_Widget::Raise);
	setHeightToolsCheckColor (m_htLevel, false);
	m_htSmooth->setBorderMode (ATOM_Widget::Raise);
	setHeightToolsCheckColor (m_htSmooth, false);
	m_htRamp->setBorderMode (ATOM_Widget::Raise);
	setHeightToolsCheckColor (m_htRamp, false);
	m_htLower->setBorderMode (ATOM_Widget::Raise);
	setHeightToolsCheckColor (m_htLower, false);

	switch (id)
	{
	case ID_HTRAISE: 
		m_htRaise->setBorderMode (ATOM_Widget::Drop); 
		setHeightToolsCheckColor (m_htRaise, true);
		break;
	case ID_HTLOWER: 
		m_htLower->setBorderMode (ATOM_Widget::Drop); 
		setHeightToolsCheckColor (m_htLower, true);
		break;
	case ID_HTLEVEL: 
		m_htLevel->setBorderMode (ATOM_Widget::Drop); 
		setHeightToolsCheckColor (m_htLevel, true);
		break;
	case ID_HTSMOOTH: 
		m_htSmooth->setBorderMode (ATOM_Widget::Drop); 
		setHeightToolsCheckColor (m_htSmooth, true);
		break;
	case ID_HTRAMP: 
		m_htRamp->setBorderMode (ATOM_Widget::Drop); 
		setHeightToolsCheckColor (m_htRamp, true);
		break;
	case ID_HTERODE: 
		m_htErode->setBorderMode (ATOM_Widget::Drop); 
		setHeightToolsCheckColor (m_htErode, true);
		break;
	}
}

void DlgTerrainTools::onCommand (ATOM_WidgetCommandEvent *event)
{
	switch (event->id)
	{
	case ID_BTNPREVBRUSH:
		setHeightBrushPaletteIndex (_currentFirstHeightBrush-1);
		break;
	case ID_BTNNEXTBRUSH:
		setHeightBrushPaletteIndex (_currentFirstHeightBrush+1);
		break;
	case ID_BTNPREVTEXBRUSH:
		setTextureBrushPaletteIndex (_currentFirstTextureBrush-1);
		break;
	case ID_BTNNEXTTEXBRUSH:
		setTextureBrushPaletteIndex (_currentFirstTextureBrush+1);
		break;
	case ID_BTNPREVGRASS:
		setGrassBrushPaletteIndex (_currentFirstGrassBrush-1);
		break;
	case ID_BTNNEXTGRASS:
		setGrassBrushPaletteIndex (_currentFirstGrassBrush+1);
		break;
	case ID_HTRAISE:
		_currentTool = TOOL_RAISE;
		checkHeightTool (ID_HTRAISE);
		break;
	case ID_HTLOWER:
		_currentTool = TOOL_LOWER;
		checkHeightTool (ID_HTLOWER);
		break;
	case ID_HTSMOOTH:
		_currentTool = TOOL_SMOOTH;
		checkHeightTool (ID_HTSMOOTH);
		break;
	case ID_HTRAMP:
		_currentTool = TOOL_RAMP;
		checkHeightTool (ID_HTRAMP);
		break;
	case ID_HTERODE:
		_currentTool = TOOL_ERODE;
		checkHeightTool (ID_HTERODE);
		break;
	case ID_HTLEVEL:
		_currentTool = TOOL_LEVEL;
		checkHeightTool (ID_HTLEVEL);
		break;
	case ID_BTNHEIGHTTOOL:
		m_vsTools->setActivePage (ID_PNLHEIGHTTOOL);
		_isBrushingTexture = false;
		_isBrushingGrass = false;
		break;
	case ID_BTNTEXTURETOOL:
		m_vsTools->setActivePage (ID_PNLTEXTURETOOL);
		_isBrushingTexture = true;
		_isBrushingGrass = false;
		break;
	case ID_BTNGRASSTOOL:
		m_vsTools->setActivePage (ID_PNLGRASSTOOL);
		_isBrushingTexture = false;
		_isBrushingGrass = true;
		break;
	case ID_BTNNEWGRASS:
		{
			if (_plugin->getEditor()->getOpenImageFileNames (false, false) == 1)
			{
				const char *filename = _plugin->getEditor()->getOpenedImageFileName (0);
				_grassBrushImageIDs.push_back (_nextTextureBrushImageId++);
				_dialog->getImageList ()->newTextureImage (_grassBrushImageIDs.back(), filename, NULL, NULL, NULL);
				setGrassBrushPaletteIndex (int(_grassBrushImageIDs.size())-4);
				ATOM_Terrain::VegetationInfo info;
				info.width = 20.f;
				info.height = 20.f;
				info.textureFileName = filename;
				_terrain->appendGrassLayer (info);
				selectTextureBrush ((int(_grassBrushImageIDs.size())-1-_currentFirstGrassBrush) % 4);
			}
			break;
		}
	case ID_BTNDELETEGRASS:
		{
			if (_currentSelectedGrassBrushIndex >= 0)
			{
				_terrain->removeGrassLayer (_currentSelectedGrassBrushIndex);
				_dialog->getImageList()->removeImage (_grassBrushImageIDs[_currentSelectedGrassBrushIndex]);
				_grassBrushImageIDs.erase (_grassBrushImageIDs.begin() + _currentSelectedGrassBrushIndex);
				setGrassBrushPaletteIndex (_currentFirstGrassBrush);
				selectGrassBrush (-1);
			}
			break;
		}
	case ID_BTNNEWTEXTURE:
		{
			if (_plugin->getEditor()->getOpenImageFileNames (false, false) == 1)
			{
				const char *filename = _plugin->getEditor()->getOpenedImageFileName (0);
				_textureBrushImageIDs.push_back (_nextTextureBrushImageId++);
				_dialog->getImageList ()->newTextureImage (_textureBrushImageIDs.back(), filename, NULL, NULL, NULL);
				setTextureBrushPaletteIndex (int(_textureBrushImageIDs.size())-4);
				_terrain->addDetailMapFileName (filename, ATOM_Vector2f(20,20));

				selectTextureBrush ((int(_textureBrushImageIDs.size())-1-_currentFirstTextureBrush) % 4);
				backupAlphas ();
			}
			break;
		}
	case ID_BTNDELETETEXTURE:
		{
			if (_currentSelectedTextureBrushIndex >= 0)
			{
				_terrain->removeDetailMap (_currentSelectedTextureBrushIndex);
				_dialog->getImageList()->removeImage (_textureBrushImageIDs[_currentSelectedTextureBrushIndex]);
				_textureBrushImageIDs.erase (_textureBrushImageIDs.begin() + _currentSelectedTextureBrushIndex);
				setTextureBrushPaletteIndex (_currentFirstTextureBrush);
				selectTextureBrush (-1);
				backupAlphas ();
			}
			break;
		}
	case ID_BTNOPENNORMAL:
		{
			if (_currentSelectedTextureBrushIndex >= 0)
			{
				if( !_detailNormalDlg )
					_detailNormalDlg = ATOM_NEW(DlgTerrainDetailNormal,_dialog,_terrain);
				if(_detailNormalDlg)
					_detailNormalDlg->show(true,_currentSelectedTextureBrushIndex);
			}
			break;
		}
	default:
		break;
	}
}

void DlgTerrainTools::onClose (ATOM_TopWindowCloseEvent *event)
{
	_plugin->getScene()->getRootNode()->removeChild (_brushIndicator.get());
}

void DlgTerrainTools::onEditChanged (ATOM_EditTextChangedEvent *event)
{
	if ((event->id == ID_EDTILEU || event->id == ID_EDTILEV) && _currentSelectedTextureBrushIndex >= 0)
	{
		float tileU = atof (m_edTileU->getString().c_str());
		float tileV = atof (m_edTileV->getString().c_str());
		_terrain->setDetailScale (_currentSelectedTextureBrushIndex, ATOM_Vector2f(tileU, tileV));
	}
	else if ((event->id == ID_EDGRASSWIDTH || event->id == ID_EDGRASSHEIGHT) && _currentSelectedGrassBrushIndex >= 0)
	{
		float w = atof (m_edGrassWidth->getString().c_str());
		float h = atof (m_edGrassHeight->getString().c_str());
		if (w < 1.f) w = 1.f;
		_terrain->setGrassLayerSize (_currentSelectedGrassBrushIndex, w, h);
	}
}

void DlgTerrainTools::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	void *host = getHost ();
	if (host == m_lbBrush1)
	{
		selectHeightBrush (0);
	}
	else if (host == m_lbBrush2)
	{
		selectHeightBrush (1);
	}
	else if (host == m_lbBrush3)
	{
		selectHeightBrush (2);
	}
	else if (host == m_lbBrush4)
	{
		selectHeightBrush (3);
	}
	else if (host == m_lbTexBrush1)
	{
		selectTextureBrush (0);
	}
	else if (host == m_lbTexBrush2)
	{
		selectTextureBrush (1);
	}
	else if (host == m_lbTexBrush3)
	{
		selectTextureBrush (2);
	}
	else if (host == m_lbTexBrush4)
	{
		selectTextureBrush (3);
	}
	else if (host == m_lbGrass1)
	{
		selectGrassBrush (0);
	}
	else if (host == m_lbGrass2)
	{
		selectGrassBrush (1);
	}
	else if (host == m_lbGrass3)
	{
		selectGrassBrush (2);
	}
	else if (host == m_lbGrass4)
	{
		selectGrassBrush (3);
	}
}

//void DlgTerrainTools::onRButtonDown (ATOM_WidgetRButtonDownEvent *event)
//{
//	void *host = getHost ();
//	
//	if (host == m_lbTexBrush1)
//	{
//		openPopupMenuForDetailTexture (0);
//	}
//	else if (host == m_lbTexBrush2)
//	{
//		openPopupMenuForDetailTexture (1);
//	}
//	else if (host == m_lbTexBrush3)
//	{
//		openPopupMenuForDetailTexture (2);
//	}
//	else if (host == m_lbTexBrush4)
//	{
//		openPopupMenuForDetailTexture (3);
//	}
//}
//-------------------------------------//

void DlgTerrainTools::loadBrushes (void)
{
	ATOM_GUIImageList *imageList = _dialog->getImageList ();
	ATOM_ASSERT(imageList);
	int id = FIRST_HEIGHT_BRUSH_ID;
	_numHeightBrushes = 0;

	ATOM_VFS::finddata_t *fd = ATOM_FindFirst ("/editor/brushes");
	if (fd)
	{
		do
		{
			const char *ext = strrchr (fd->filename, '.');
			if (ext && !stricmp (ext, ".png"))
			{
				imageList->newTextureImage (id++, fd->filename, NULL, NULL, NULL);
				_numHeightBrushes++;
			}
		}
		while (ATOM_FindNext (fd));

		ATOM_CloseFind (fd);
	}

	_textureBrushImageIDs.clear ();
	for (unsigned i = 0; i < _terrain->getNumDetailMaps(); ++i)
	{
		_textureBrushImageIDs.push_back (_nextTextureBrushImageId++);
		_dialog->getImageList ()->newTextureImage (_textureBrushImageIDs.back(), _terrain->getDetailMapFileName(i), NULL, NULL, NULL);
	}
	setTextureBrushPaletteIndex (0);

	_grassBrushImageIDs.clear ();
	for (unsigned i = 0; i < _terrain->getNumGrassLayers(); ++i)
	{
		_grassBrushImageIDs.push_back (_nextTextureBrushImageId++);
		_dialog->getImageList ()->newTextureImage (_grassBrushImageIDs.back(), _terrain->getGrassLayerInfo(i).textureFileName.c_str(), NULL, NULL, NULL);
	}
	setGrassBrushPaletteIndex (0);
}

void DlgTerrainTools::setTextureBrushPaletteIndex (int index)
{
	int numBrushes = _textureBrushImageIDs.size();
	if (numBrushes == 0)
	{
		_currentFirstTextureBrush = -1;
		m_lbTexBrush1->setClientImageId (ATOM_INVALID_IMAGEID);
		m_lbTexBrush2->setClientImageId (ATOM_INVALID_IMAGEID);
		m_lbTexBrush3->setClientImageId (ATOM_INVALID_IMAGEID);
		m_lbTexBrush4->setClientImageId (ATOM_INVALID_IMAGEID);
		return;
	}

	if (index > numBrushes - 4)
	{
		index = numBrushes - 4;
	}

	if (index < 0)
	{
		index = 0;
	}

	_currentFirstTextureBrush = index;

	if (index < numBrushes)
	{
		m_lbTexBrush1->setClientImageId (_textureBrushImageIDs[index]);
		index++;
	}
	else
	{
		m_lbTexBrush1->setClientImageId (ATOM_INVALID_IMAGEID);
	}

	if (index < numBrushes)
	{
		m_lbTexBrush2->setClientImageId (_textureBrushImageIDs[index]);
		index++;
	}
	else
	{
		m_lbTexBrush2->setClientImageId (ATOM_INVALID_IMAGEID);
	}

	if (index < numBrushes)
	{
		m_lbTexBrush3->setClientImageId (_textureBrushImageIDs[index]);
		index++;
	}
	else
	{
		m_lbTexBrush3->setClientImageId (ATOM_INVALID_IMAGEID);
	}

	if (index < numBrushes)
	{
		m_lbTexBrush4->setClientImageId (_textureBrushImageIDs[index]);
		index++;
	}
	else
	{
		m_lbTexBrush4->setClientImageId (ATOM_INVALID_IMAGEID);
	}
}

void DlgTerrainTools::setGrassBrushPaletteIndex (int index)
{
	int numBrushes = _grassBrushImageIDs.size();
	if (numBrushes == 0)
	{
		_currentFirstGrassBrush = -1;
		m_lbGrass1->setClientImageId (ATOM_INVALID_IMAGEID);
		m_lbGrass2->setClientImageId (ATOM_INVALID_IMAGEID);
		m_lbGrass3->setClientImageId (ATOM_INVALID_IMAGEID);
		m_lbGrass4->setClientImageId (ATOM_INVALID_IMAGEID);
		return;
	}

	if (index > numBrushes - 4)
	{
		index = numBrushes - 4;
	}

	if (index < 0)
	{
		index = 0;
	}

	_currentFirstGrassBrush = index;

	if (index < numBrushes)
	{
		m_lbGrass1->setClientImageId (_grassBrushImageIDs[index]);
		index++;
	}
	else
	{
		m_lbGrass1->setClientImageId (ATOM_INVALID_IMAGEID);
	}

	if (index < numBrushes)
	{
		m_lbGrass2->setClientImageId (_grassBrushImageIDs[index]);
		index++;
	}
	else
	{
		m_lbGrass2->setClientImageId (ATOM_INVALID_IMAGEID);
	}

	if (index < numBrushes)
	{
		m_lbGrass3->setClientImageId (_grassBrushImageIDs[index]);
		index++;
	}
	else
	{
		m_lbGrass3->setClientImageId (ATOM_INVALID_IMAGEID);
	}

	if (index < numBrushes)
	{
		m_lbGrass4->setClientImageId (_grassBrushImageIDs[index]);
		index++;
	}
	else
	{
		m_lbGrass4->setClientImageId (ATOM_INVALID_IMAGEID);
	}
}

void DlgTerrainTools::selectTextureBrush (int index)
{
	ATOM_GUIImage *newImage = NULL;

	if (index >= 0)
	{
		ATOM_ASSERT(index < 4);

		ATOM_Label *brushLabel = getTextureBrushLabel(index);
		if (brushLabel)
		{
			int imageId = brushLabel->getClientImageId ();
			if (imageId != ATOM_INVALID_IMAGEID)
			{
				newImage = _dialog->getImageList()->getImage (imageId);
			}
		}
	}

	if (newImage != _currentSelectedTextureBrush)
	{
		if (_currentSelectedTextureBrush)
		{
			_currentSelectedTextureBrush->setColor (WST_NORMAL, 0xFFFFFFFF);
			_currentSelectedTextureBrush->setColor (WST_HOVER, 0xFFFFFFFF);
			_currentSelectedTextureBrush->setColor (WST_HOLD, 0xFFFFFFFF);
			_currentSelectedTextureBrush->setColor (WST_DISABLED, 0xFFFFFFFF);
		}

		if (newImage)
		{
			newImage->setColor (WST_NORMAL, 0xFFFF00FF);
			newImage->setColor (WST_HOVER, 0xFFFF00FF);
			newImage->setColor (WST_HOLD, 0xFFFF00FF);
			newImage->setColor (WST_DISABLED, 0xFFFF00FF);
		}

		_currentSelectedTextureBrush = newImage;
	}
	
	if (newImage)
	{
		int detailIndex = _currentFirstTextureBrush+index;
		ATOM_Vector4f scale = _terrain->getDetailScale (detailIndex);
		char buffer[256];
		sprintf (buffer, "%f", scale.x);
		m_edTileU->setString (buffer);
		sprintf (buffer, "%f", scale.y);
		m_edTileV->setString (buffer);
		_currentSelectedTextureBrushIndex = _currentFirstTextureBrush + index;
	}
	else
	{
		m_edTileU->setString ("");
		m_edTileV->setString ("");
		_currentSelectedTextureBrushIndex = -1;
	}
	
	// wangjian added : 如果法线对话框当前显示，则刷新到当前的细节纹理
	if( _detailNormalDlg && _detailNormalDlg->isShow() )
		_detailNormalDlg->show(true,_currentSelectedTextureBrushIndex);

}

void DlgTerrainTools::selectGrassBrush (int index)
{
	ATOM_GUIImage *newImage = NULL;

	if (index >= 0)
	{
		ATOM_ASSERT(index < 4);

		ATOM_Label *brushLabel = getGrassBrushLabel(index);
		if (brushLabel)
		{
			int imageId = brushLabel->getClientImageId ();
			if (imageId != ATOM_INVALID_IMAGEID)
			{
				newImage = _dialog->getImageList()->getImage (imageId);
			}
		}
	}

	if (newImage != _currentSelectedGrassBrush)
	{
		if (_currentSelectedGrassBrush)
		{
			_currentSelectedGrassBrush->setColor (WST_NORMAL, 0xFFFFFFFF);
			_currentSelectedGrassBrush->setColor (WST_HOVER, 0xFFFFFFFF);
			_currentSelectedGrassBrush->setColor (WST_HOLD, 0xFFFFFFFF);
			_currentSelectedGrassBrush->setColor (WST_DISABLED, 0xFFFFFFFF);
		}

		if (newImage)
		{
			newImage->setColor (WST_NORMAL, 0xFFFF00FF);
			newImage->setColor (WST_HOVER, 0xFFFF00FF);
			newImage->setColor (WST_HOLD, 0xFFFF00FF);
			newImage->setColor (WST_DISABLED, 0xFFFF00FF);
		}

		_currentSelectedGrassBrush = newImage;
	}

	if (newImage)
	{
		int grassIndex = _currentFirstGrassBrush+index;
		float w = _terrain->getGrassLayerInfo(grassIndex).width;
		float h = _terrain->getGrassLayerInfo(grassIndex).height;
		char buffer[256];
		sprintf (buffer, "%f", w);
		m_edGrassWidth->setString (buffer);
		sprintf (buffer, "%f", h);
		m_edGrassHeight->setString (buffer);
		_currentSelectedGrassBrushIndex = _currentFirstGrassBrush + index;
	}
	else
	{
		m_edGrassWidth->setString ("");
		m_edGrassHeight->setString ("");
		_currentSelectedGrassBrushIndex = -1;
	}
}

void DlgTerrainTools::setHeightBrushPaletteIndex (int index)
{
	if (_numHeightBrushes == 0)
	{
		return;
	}

	if (index > _numHeightBrushes - 4)
	{
		index = _numHeightBrushes - 4;
	}

	if (index < 0)
	{
		index = 0;
	}

	_currentFirstHeightBrush = index;

	if (index < _numHeightBrushes)
	{
		m_lbBrush1->setClientImageId (index + FIRST_HEIGHT_BRUSH_ID);
		index++;
	}
	else
	{
		m_lbBrush1->setClientImageId (ATOM_INVALID_IMAGEID);
	}

	if (index < _numHeightBrushes)
	{
		m_lbBrush2->setClientImageId (index + FIRST_HEIGHT_BRUSH_ID);
		index++;
	}
	else
	{
		m_lbBrush2->setClientImageId (ATOM_INVALID_IMAGEID);
	}

	if (index < _numHeightBrushes)
	{
		m_lbBrush3->setClientImageId (index + FIRST_HEIGHT_BRUSH_ID);
		index++;
	}
	else
	{
		m_lbBrush3->setClientImageId (ATOM_INVALID_IMAGEID);
	}

	if (index < _numHeightBrushes)
	{
		m_lbBrush4->setClientImageId (index + FIRST_HEIGHT_BRUSH_ID);
		index++;
	}
	else
	{
		m_lbBrush4->setClientImageId (ATOM_INVALID_IMAGEID);
	}
}

void DlgTerrainTools::selectHeightBrush (int index)
{
	ATOM_GUIImage *newImage = NULL;

	if (index >= 0)
	{
		ATOM_ASSERT(index < 4);

		ATOM_Label *brushLabel = getHeightBrushLabel (index);
		if (brushLabel)
		{
			int imageId = brushLabel->getClientImageId ();
			if (imageId != ATOM_INVALID_IMAGEID)
			{
				newImage = _dialog->getImageList()->getImage (imageId);
			}
		}
	}

	if (newImage != _currentSelectedHeightBrush)
	{
		if (_currentSelectedHeightBrush)
		{
			_currentSelectedHeightBrush->setColor (WST_NORMAL, 0xFFFFFFFF);
			_currentSelectedHeightBrush->setColor (WST_HOVER, 0xFFFFFFFF);
			_currentSelectedHeightBrush->setColor (WST_HOLD, 0xFFFFFFFF);
			_currentSelectedHeightBrush->setColor (WST_DISABLED, 0xFFFFFFFF);
		}

		if (newImage)
		{
			newImage->setColor (WST_NORMAL, 0xFFFF00FF);
			newImage->setColor (WST_HOVER, 0xFFFF00FF);
			newImage->setColor (WST_HOLD, 0xFFFF00FF);
			newImage->setColor (WST_DISABLED, 0xFFFF00FF);
		}

		_currentSelectedHeightBrush = newImage;
	}
}

ATOM_Label *DlgTerrainTools::getHeightBrushLabel (int index) const
{
	switch (index)
	{
	case 0: return m_lbBrush1;
	case 1: return m_lbBrush2;
	case 2: return m_lbBrush3;
	case 3: return m_lbBrush4;
	default: return NULL;
	}
}

ATOM_Label *DlgTerrainTools::getTextureBrushLabel (int index) const
{
	switch (index)
	{
	case 0: return m_lbTexBrush1;
	case 1: return m_lbTexBrush2;
	case 2: return m_lbTexBrush3;
	case 3: return m_lbTexBrush4;
	default: return NULL;
	}
}

ATOM_Label *DlgTerrainTools::getGrassBrushLabel (int index) const
{
	switch (index)
	{
	case 0: return m_lbGrass1;
	case 1: return m_lbGrass2;
	case 2: return m_lbGrass3;
	case 3: return m_lbGrass4;
	default: return NULL;
	}
}

void DlgTerrainTools::applyCurrentHeightBrush (int x, int y, int w, int h, float orientation, float strength, ATOM_Material *material)
{
	if (!_currentSelectedHeightBrush)
	{
		return;
	}

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	
	ATOM_AUTOREF(ATOM_DepthBuffer) oldDepth = device->getDepthBuffer ();
	ATOM_AUTOREF(ATOM_Texture) oldRT = device->getRenderTarget (0);
	ATOM_Rect2Di oldVP = device->getViewport (NULL);

	ATOM_Texture *target = _heightmapPingPong[1-_currentHeightmap].get();
	int vpW = target->getWidth();
	int vpH = target->getHeight();
	device->setRenderTarget(0, target);
	device->setViewport (NULL, ATOM_Rect2Di(0, 0, vpW, vpH));
	device->setDepthBuffer (_depthbuffer.get());
	device->beginFrame ();

	// copy old value
	_copyMaterial->getParameterTable()->setTexture ("heightTexture", _heightmapPingPong[_currentHeightmap].get());
	drawQuad (device, _copyMaterial.get(), vpW, vpH);

	// apply height filter
	material->getParameterTable()->setTexture ("heightTexture", _heightmapPingPong[_currentHeightmap].get());
	material->getParameterTable()->setTexture ("brushTexture", _currentSelectedHeightBrush->getTexture(WST_NORMAL));
	material->getParameterTable()->setFloat ("brushStrength", strength);
	drawRegion (device, material, x, y, w, h, orientation, vpW, vpH);

	device->endFrame ();

	device->setDepthBuffer (oldDepth.get());
	device->setRenderTarget (0, oldRT.get());
	device->setViewport (NULL, oldVP);

	_terrain->setHeightmap (target);
	_currentHeightmap = 1 - _currentHeightmap;
}

void DlgTerrainTools::applyRaiseBrush (int x, int y, float orientation)
{
	float strength = m_sbStrength->getPosition()*0.2f;
	int radius = m_sbRadius->getPosition();

	applyCurrentHeightBrush (x - radius, y - radius, radius*2, radius*2, orientation, strength, _raiseMaterial.get());
}

void DlgTerrainTools::applyLowerBrush (int x, int y, float orientation)
{
	int radius = m_sbRadius->getPosition();

	float strength = -m_sbStrength->getPosition()*0.2f;
	applyCurrentHeightBrush (x - radius, y - radius, radius*2, radius*2, orientation, strength, _raiseMaterial.get());
}

void DlgTerrainTools::applySmoothBrush (int x, int y, float orientation)
{
	int radius = m_sbRadius->getPosition();

	float strength = m_sbStrength->getPosition()/m_sbStrength->getRangeMax();
	ATOM_Vector4f invTextureSize(1.f/_terrain->getHeightmap()->getWidth(), 1.f/_terrain->getHeightmap()->getHeight(), 0.f, 0.f);
	_smoothMaterial->getParameterTable()->setVector ("invTextureSize", invTextureSize);
	applyCurrentHeightBrush (x - radius, y - radius, radius*2, radius*2, orientation, strength, _smoothMaterial.get());
}

void DlgTerrainTools::applyLevelBrush (int x, int y, float orientation, float height)
{
	int radius = m_sbRadius->getPosition();

	float strength = m_sbStrength->getPosition()/m_sbStrength->getRangeMax();
	_levelMaterial->getParameterTable()->setFloat ("level", height);
	applyCurrentHeightBrush (x - radius, y - radius, radius*2, radius*2, orientation, strength, _levelMaterial.get());
}

void DlgTerrainTools::applyGrassBrush (const ATOM_Vector3f &pos, float orientation)
{
	if (_currentSelectedGrassBrush)
	{
		int radius = m_sbRadius->getPosition();
		float strength = m_sbStrength->getPosition()/m_sbStrength->getRangeMax();
		int grassCount = strength * radius * radius;

		if (_revert)
		{
			_terrain->removeRandomGrassObjects (_currentSelectedGrassBrushIndex, pos, radius, grassCount);
		}
		else
		{
			_terrain->addRandomGrassObjects (_currentSelectedGrassBrushIndex, pos, radius, grassCount, &_normals[0]);
		}
	}
}

void DlgTerrainTools::applyTextureBrush (float x, float y, float orientation)
{
	if (!_currentSelectedHeightBrush)
	{
		return;
	}
	float radiusX = m_sbRadius->getPosition() * float(_terrain->getAlphaMapWidth())/float(_terrain->getHeightmap()->getWidth());
	float radiusY = m_sbRadius->getPosition() * float(_terrain->getAlphaMapHeight())/float(_terrain->getHeightmap()->getHeight());
	float fx = x * _terrain->getAlphaMapWidth() - radiusX;//float(_terrain->getAlphaMapWidth())/float(_terrain->getHeightmap()->getWidth());
	float fy = y * _terrain->getAlphaMapHeight() - radiusY;//float(_terrain->getAlphaMapHeight())/float(_terrain->getHeightmap()->getHeight());
	int ix = int(fx + 0.5f);
	int iy = int(fy + 0.5f);
	int w = int(radiusX * 2.f + 0.5f);
	int h = int(radiusY * 2.f + 0.5f);
	
	float strength = m_sbStrength->getPosition()/m_sbStrength->getRangeMax();

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	int alphaIndex = _currentSelectedTextureBrushIndex / 4;
	int alphaChannel = _currentSelectedTextureBrushIndex % 4;

	ATOM_AUTOREF(ATOM_DepthBuffer) oldDepth = device->getDepthBuffer ();
	ATOM_AUTOREF(ATOM_Texture) oldRT = device->getRenderTarget (0);
	ATOM_Rect2Di oldVP = device->getViewport (NULL);

	device->beginFrame ();
	device->setDepthBuffer (_depthbufferAlpha.get());

	_textureMaterial->setActiveEffect ("Modulate");
	_textureMaterial->getParameterTable()->setTexture ("brushTexture", _currentSelectedHeightBrush->getTexture(WST_NORMAL));
	_textureMaterial->getParameterTable()->setFloat ("brushStrength", strength);

	// apply alpha modulate to all alpha maps
	for (int i = 0; i < _terrain->getNumAlphaMaps(); ++i)
	{
		ATOM_Texture *target = _terrain->getAlphaMap(i);
		int vpW = target->getWidth();
		int vpH = target->getHeight();
		device->setRenderTarget(0, target);
		device->setViewport (NULL, ATOM_Rect2Di(0, 0, vpW, vpH));

		drawRegion (device, _textureMaterial.get(), ix, iy, w, h, orientation, vpW, vpH);
	}

	// apply alpha addition to the channel
	ATOM_Texture *target = _terrain->getAlphaMap(alphaIndex);
	int vpW = target->getWidth();
	int vpH = target->getHeight();
	switch (alphaChannel)
	{
	case 0: _textureMaterial->setActiveEffect ("Add2"); break;
	case 1: _textureMaterial->setActiveEffect ("Add1"); break;
	case 2: _textureMaterial->setActiveEffect ("Add0"); break;
	case 3: _textureMaterial->setActiveEffect ("Add3"); break;
	}
	device->setRenderTarget(0, target);
	device->setViewport (NULL, ATOM_Rect2Di(0, 0, vpW, vpH));
	drawRegion (device, _textureMaterial.get(), ix, iy, w, h, orientation, vpW, vpH);

	device->endFrame ();

	device->setDepthBuffer (oldDepth.get());
	device->setRenderTarget (0, oldRT.get());
	device->setViewport (NULL, oldVP);
}

void DlgTerrainTools::beginBrushing (bool revert)
{
	if (!_isBrushing )
	{
		if (_isBrushingGrass)
		{
			if (!_currentSelectedGrassBrush)
			{
				return;
			}

			if (_normals.empty ())
			{
				_normals.resize (_terrain->getHeightField()->getSizeX() * _terrain->getHeightField()->getSizeZ());
				_terrain->getHeightField()->computeNormals (&_normals[0]);
			}
		}
		else if (_isBrushingTexture)
		{
			if (!_currentSelectedTextureBrush)
			{
				return;
			}
		}
		else
		{
			if (!_currentSelectedHeightBrush)
			{
				return;
			}
		}

		_isBrushing = true;
		_revert = revert;
	}
}

void DlgTerrainTools::updateBrushIndicator (void)
{
	if (_isTerrainHit)
	{
		float radius = m_sbRadius->getPosition();
		float scaleX = radius * _terrain->getScale().x * 2.f;
		float scaleZ = radius * _terrain->getScale().z * 2.f;
		float orientation = _isBrushingGrass ? 0.f : -_orientation;
		ATOM_Matrix4x4f matrix = ATOM_Matrix4x4f::getRotateYMatrix (orientation);
		matrix.setRow3 (3, _hitPosSaved);
		matrix >>= ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(scaleX, 1000.f, scaleZ));
		_brushIndicator->setO2T (matrix);

		if (_isBrushingGrass)
		{
			_brushIndicator->setTexture (_currentSelectedGrassBrush?ATOM_GetColorTexture(ATOM_ColorARGB(0.f,1.f,0.f,1.f)):ATOM_GetColorTexture(0));
			_brushIndicator->getMaterial()->getParameterTable()->setTexture ("decalMask", ATOM_GetColorTexture(0xFFFFFFFF));
			_brushIndicator->setColor (ATOM_Vector4f(1.f,1.f,1.f,0.8f));
		}
		else if (_isBrushingTexture)
		{
			ATOM_Texture *maskTexture = _currentSelectedHeightBrush?_currentSelectedHeightBrush->getTexture(WST_NORMAL):ATOM_GetColorTexture(0);
			_brushIndicator->setTexture (_currentSelectedTextureBrush?_currentSelectedTextureBrush->getTexture(WST_NORMAL):ATOM_GetColorTexture(0));
			_brushIndicator->getMaterial()->getParameterTable()->setTexture ("decalMask", maskTexture);
			_brushIndicator->setColor (ATOM_Vector4f(1.f,1.f,1.f,1.f));
		}
		else
		{
			ATOM_Texture *maskTexture = _currentSelectedHeightBrush?_currentSelectedHeightBrush->getTexture(WST_NORMAL):ATOM_GetColorTexture(0);
			_brushIndicator->setTexture (maskTexture);
			_brushIndicator->getMaterial()->getParameterTable()->setTexture ("decalMask", maskTexture);
			_brushIndicator->setColor (ATOM_Vector4f(1.f,0.f,0.f,0.7f));
		}
	}
}

void DlgTerrainTools::updateBrushing (int x, int y, float fx, float fy)
{
	if (_isBrushingTexture)
	{
		applyTextureBrush (fx, fy, _orientation);
	}
	else if (_isBrushingGrass)
	{
		applyGrassBrush (_hitPosSaved, _orientation);
	}
	else
	{
		switch (_currentTool)
		{
		case TOOL_RAISE:
			applyRaiseBrush (x, y, _orientation);
			break;
		case TOOL_LOWER:
			applyLowerBrush (x, y, _orientation);
			break;
		case TOOL_SMOOTH:
			applySmoothBrush (x, y, _orientation);
			break;
		case TOOL_LEVEL:
			applyLevelBrush (x, y, _orientation, _hitHeightForLevel/*_hitPosSaved.y*/);// wangjian modified
			break;
		default:
			return;
		}
		calcNormalMap ();
		_normals.resize (0);
	}
}

void DlgTerrainTools::endBrushing (void)
{
	_isBrushing = false;

	if (_isBrushingTexture)
	{
		backupAlphas ();
	}
	else if (_isBrushingGrass)
	{
	}
	else
	{
		backupHeights ();
		updateHeightField ();
	}
	//_terrain->drawWireframe (false);
}

bool DlgTerrainTools::isBrushing (void) const
{
	return _isBrushing;
}

void DlgTerrainTools::backupHeights (void)
{
	_heightmapPingPong[_currentHeightmap]->getTexImage (ATOM_PIXEL_FORMAT_R32F, &_backupHeights[0]);
}

void DlgTerrainTools::restoreHeights (void)
{
	_heightmapPingPong[_currentHeightmap]->updateTexImage (&_backupHeights[0], 0, 0, _heightmapPingPong[_currentHeightmap]->getWidth(), _heightmapPingPong[_currentHeightmap]->getHeight(), ATOM_PIXEL_FORMAT_R32F);
	//--- wangjian added ---//
	// 重新设置地形的高度图
	_terrain->setHeightmap(_heightmapPingPong[_currentHeightmap].get());
}

void DlgTerrainTools::backupAlphas (void)
{
	unsigned numAlphas = _terrain->getNumAlphaMaps();
	unsigned numAlphasBk = _backupAlphas.size();
	if (numAlphasBk > numAlphas)
	{
		for (unsigned i = numAlphas; i < numAlphasBk; ++i)
		{
			ATOM_DELETE(_backupAlphas[i]);
		}
		_backupAlphas.resize (numAlphas);
	}
	else
	{
		_backupAlphas.resize (numAlphas);
		for (unsigned i = numAlphasBk; i < numAlphas; ++i)
		{
			_backupAlphas[i] = NULL;
		}
	}

	for (unsigned i = 0; i < numAlphas; ++i)
	{
		ATOM_Texture *alphaMap = _terrain->getAlphaMap (i);
		ATOM_ASSERT(alphaMap);

		if (!_backupAlphas[i])
		{
			_backupAlphas[i] = ATOM_NEW(ATOM_BaseImage);
		}
		_backupAlphas[i]->init (alphaMap->getWidth(), alphaMap->getHeight(), ATOM_PIXEL_FORMAT_BGRA8888, NULL, 1);
		alphaMap->getTexImage (ATOM_PIXEL_FORMAT_BGRA8888, _backupAlphas[i]->getData());
	}
}

void DlgTerrainTools::restoreAlphas (void)
{
	unsigned numAlphas = _terrain->getNumAlphaMaps();
	unsigned numAlphasBk = _backupAlphas.size();
	ATOM_ASSERT(numAlphas == numAlphasBk);

	for (unsigned i = 0; i < numAlphas; ++i)
	{
		ATOM_Texture *alphaMap = _terrain->getAlphaMap (i);
		ATOM_ASSERT(alphaMap);

		alphaMap->updateTexImage (_backupAlphas[i]->getData(), 0, 0, alphaMap->getWidth(), alphaMap->getHeight(), ATOM_PIXEL_FORMAT_BGRA8888);
	}
}

ATOM_Terrain *DlgTerrainTools::getTerrain (void) const
{
	return _terrain.get();
}

void DlgTerrainTools::updateHeightField (void)
{
	float *heights = ATOM_NEW_ARRAY(float, _heightmapPingPong[_currentHeightmap]->getWidth() * _heightmapPingPong[_currentHeightmap]->getHeight());
	_heightmapPingPong[_currentHeightmap]->getTexImage (ATOM_PIXEL_FORMAT_R32F, heights);
	_terrain->getHeightField()->resetHeights (heights);
	_terrain->invalidateBoundingbox ();
	ATOM_DELETE_ARRAY(heights);
}

void DlgTerrainTools::calcNormalMap (void)
{
	ATOM_RenderDevice *device = ATOM_GetRenderDevice();

	ATOM_AUTOREF(ATOM_DepthBuffer) oldDepth = device->getDepthBuffer ();
	ATOM_AUTOREF(ATOM_Texture) oldRT = device->getRenderTarget (0);
	ATOM_Rect2Di oldVP = device->getViewport (NULL);

	device->setRenderTarget (0, _normalMap.get());
	device->setDepthBuffer (_depthbuffer.get());
	device->setViewport (NULL, ATOM_Rect2Di(0, 0, _normalMap->getWidth(), _normalMap->getHeight()));
	device->beginFrame ();

	_normalgenMaterial->getParameterTable()->setTexture ("heightTexture", _heightmapPingPong[_currentHeightmap].get());
	_normalgenMaterial->getParameterTable()->setVector ("invHeightMapSize", ATOM_Vector4f(1.f/_terrain->getHeightmap()->getWidth(), 1.f/_terrain->getHeightmap()->getHeight(), 0.f, 0.f));
	_normalgenMaterial->getParameterTable()->setFloat ("normalScale", _terrain->getScale().x);
	drawQuad (device, _normalgenMaterial.get(), _normalMap->getWidth(), _normalMap->getHeight());

	device->endFrame ();
	device->setRenderTarget (0, oldRT.get());
	device->setDepthBuffer (oldDepth.get());
	device->setViewport (NULL, oldVP);

	_terrain->setNormalMap (_normalMap.get());
}

void DlgTerrainTools::drawQuad (ATOM_RenderDevice *device, ATOM_Material *material, int w, int h)
{
	float deltax = (float)0.5f/(float)w;
	float deltay = (float)0.5f/(float)h;

	// copy old value
	{
		float vertices[4 * 5] = {
			-1.f - deltax, -1.f + deltay, 0.f, 0.f, 1.f,
			1.f - deltax, -1.f + deltay, 0.f, 1.f, 1.f,
			1.f - deltax,  1.f + deltay, 0.f, 1.f, 0.f,
			-1.f - deltax,  1.f + deltay, 0.f, 0.f, 0.f
		};
		unsigned short indices[4] = {
			0, 1, 2, 3
		};

		ATOM_RenderDevice *device = ATOM_GetRenderDevice();
		unsigned numPasses = material->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (material->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);
				material->endPass (device, pass);
			}
		}
		material->end (device);
	}
}

struct BrushVertex
{
	float x, y, z, w;
	float u, v;
	float u2, v2;
};

static BrushVertex makeBrushVertex (int x, int y, int w, int h)
{
	BrushVertex v;
	v.x = x - 0.5f;
	v.y = y - 0.5f;
	v.z = 0.f;
	v.w = 1.f;
	v.u2 = float(x)/float(w);
	v.v2 = float(y)/float(h);
	return v;
}

void DlgTerrainTools::drawRegion (ATOM_RenderDevice *device, ATOM_Material *material, int x, int y, int w, int h, float orientation, int vpW, int vpH)
{
	float cx = x + w * 0.5f;
	float cy = y + h * 0.5f;
	float s, c;
	ATOM_sincos(orientation, &s, &c);
	float tx = cx * (1.f - c) + cy * s;
	float ty = cy * (1.f - c) - cx * s;

	int l = x;
	int t = y;
	int r = x + w;
	int b = y + h;

	BrushVertex vertices[4];

	vertices[0] = makeBrushVertex(l * c - t * s + tx, l * s + t * c + ty, vpW, vpH);
	vertices[0].u = 0.f;
	vertices[0].v = 1.f;
	vertices[1] = makeBrushVertex(r * c - t * s + tx, r * s + t * c + ty, vpW, vpH);
	vertices[1].u = 1.f;
	vertices[1].v = 1.f;
	vertices[2] = makeBrushVertex(r * c - b * s + tx, r * s + b * c + ty, vpW, vpH);
	vertices[2].u = 1.f;
	vertices[2].v = 0.f;
	vertices[3] = makeBrushVertex(l * c - b * s + tx, l * s + b * c + ty, vpW, vpH);
	vertices[3].u = 0.f;
	vertices[3].v = 0.f;

	unsigned short indices[4] = { 0, 1, 2, 3 };

	unsigned numPasses = material->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (material->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_TEX1_2|ATOM_VERTEX_ATTRIB_TEX2_2, sizeof(BrushVertex), vertices, indices);
			material->endPass (device, pass);
		}
	}
	material->end (device);
}

void DlgTerrainTools::beginEdit (void)
{
	_savedPixelError = _terrain->getMaxPixelError ();
	_terrain->setMaxPixelError (0);

	_savedGrassDistance = _terrain->getGrassFadeOutDistance ();
	_terrain->setGrassFadeOutDistance (20000.f);
}

void DlgTerrainTools::endEdit (void)
{
	_terrain->setMaxPixelError (_savedPixelError);
	_terrain->setGrassFadeOutDistance (_savedGrassDistance);
}

void DlgTerrainTools::updateBaseMap (void)
{
	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	
	ATOM_AUTOREF(ATOM_Texture) tex = _terrain->getBasemap ();

	//---------------------wangjian modified-------------------------//
	int width = 2048;
	int	height = 2048;

#if 1
	// 根据地形实际尺寸确定basemap的尺寸[ 如果按2560 X 2560的地形使用2048X2048的basemap , texel ratio : 2048 / 2560 = 0.8 ]
	const float ratio = 1.6f;
	const ATOM_BBox & bbox = _terrain->getWorldBoundingbox();
	const ATOM_Vector3f extent_terrain = bbox.getMax() - bbox.getMin();
	const int width_terrain = (int)extent_terrain.x;
	const int height_terrain = (int)extent_terrain.z;
	width = width_terrain * ratio;
	height = height_terrain * ratio;
#endif

	if (!tex)
	{
		tex = device->allocTexture (NULL, NULL, width, height, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
		_terrain->setBaseMap (tex.get());
	}
	else
	{
#if 0
		width = tex->getWidth();
		height = tex->getHeight();
#endif
	}
	//------------------------------------------------------------//

	ATOM_AUTOREF(ATOM_DepthBuffer) depth = device->allocDepthBuffer (width, height);
	if (tex && depth)
	{

		ATOM_AUTOREF(ATOM_DepthBuffer) oldDepth = device->getDepthBuffer ();
		ATOM_AUTOREF(ATOM_Texture) oldRT = device->getRenderTarget (0);
		ATOM_Rect2Di oldVP = device->getViewport (NULL);

		int vpW = width;
		int vpH = height;
		device->setRenderTarget(0, tex.get());
		device->setViewport (NULL, ATOM_Rect2Di(0, 0, vpW, vpH));
		device->setDepthBuffer (depth.get());
		device->beginFrame ();

		char buffer[256];
		for (int i = 0; i < 12; ++i)
		{
			sprintf (buffer, "detail%d", i);
			ATOM_Texture *detailMap = i < _terrain->getNumDetailMaps() ? _terrain->getDetailMap (i) : ATOM_GetColorTexture(0);
			_basemapMaterial->getParameterTable()->setTexture (buffer, detailMap);

			sprintf (buffer, "detail%d_uvscale", i);
			ATOM_Vector4f detailUVScale = i < _terrain->getNumDetailMaps() ? _terrain->getDetailScale(i) : ATOM_Vector4f(1,1,1,1);
			_basemapMaterial->getParameterTable()->setVector (buffer, detailUVScale);
		}
		for (int i = 0; i < 3; ++i)
		{
			sprintf (buffer, "alpha%d", i);
			ATOM_Texture *alphaMap = i < _terrain->getNumAlphaMaps() ? _terrain->getAlphaMap (i) : ATOM_GetColorTexture(0);
			_basemapMaterial->getParameterTable()->setTexture (buffer, alphaMap);
		}

		drawQuad (device, _basemapMaterial.get(), vpW, vpH);

		device->endFrame ();

		device->setDepthBuffer (oldDepth.get());
		device->setRenderTarget (0, oldRT.get());
		device->setViewport (NULL, oldVP);
	}
}

void DlgTerrainTools::beginOrientation (void)
{
	if (_isTerrainHit)
	{
		_isEditingOrientation = true;
	}
}

void DlgTerrainTools::updateOrientation (int deltaMouse)
{
	_orientation += deltaMouse * 0.01f;
}

void DlgTerrainTools::endOrientation (void)
{
	_isEditingOrientation = false;
}

bool DlgTerrainTools::isEditingOrientation (void) const
{
	return _isEditingOrientation;
}

void DlgTerrainTools::setHitTerrain (bool hit, const ATOM_Vector3f &hitPos)
{
	//--- wangjian added ---//
	if( _currentTool == TOOL_LEVEL )
	{
		if( hit && ( _hitHeightForLevel == 0.0f ) && _isBrushing )
			_hitHeightForLevel = hitPos.y;
		else if( !_isBrushing)
			_hitHeightForLevel = 0.0f;
	}
	//-------------------------//

	_isTerrainHit = hit;
	
	if (hit && !_isEditingOrientation)
	{
		_hitPosSaved = hitPos;
	}
}

