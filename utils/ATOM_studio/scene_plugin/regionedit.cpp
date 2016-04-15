#include "StdAfx.h"
#include "atom3d_studio.h"
#include "plugin.h"
#include "gridnode.h"
#include "regionedit.h"
#include "plugin_scene.h"

ATOM_BEGIN_EVENT_MAP(DlgRegionEdit, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(DlgRegionEdit, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(DlgRegionEdit, ATOM_EditTextChangedEvent, onEditChanged)
	ATOM_EVENT_HANDLER(DlgRegionEdit, ATOM_ComboBoxSelectionChangedEvent, onCBChanged)
ATOM_END_EVENT_MAP

#define INDEX_COLUMN_WIDTH 30
#define BUTTON_ID_START 3000

DlgRegionEdit::DlgRegionEdit (PluginScene *plugin, ATOM_Widget *parent)
{
	_plugin = plugin;
	_dialog = (ATOM_Dialog*)parent->loadHierarchy("/editor/ui/regionedit.ui");
	if (_dialog)
	{
		initControls (_dialog);
		_dialog->setEventTrigger (this);
		m_lvRegionList->setEventTrigger (this);

		ATOM_Rect2Di rc = _dialog->getWidgetRect();
		const ATOM_Rect2Di &parentRc = parent->getWidgetRect();
		rc.point.x = (parentRc.size.w - rc.size.w) / 2;
		rc.point.y = (parentRc.size.h - rc.size.h) / 2;
		_dialog->resize (rc);

		m_lvRegionList->setItemHeight (20);
	}
	loadCfg ("/editor/conf/regions.xml");
	initUI ();
}

DlgRegionEdit::~DlgRegionEdit (void)
{
	if (_dialog)
	{
		_dialog->setEventTrigger (NULL);
		m_lvRegionList->setEventTrigger (NULL);
		ATOM_DELETE(_dialog);
	}

	for (int i = 0; i < regionDecals.size(); ++i)
	{
		regionDecals[i]->getParent()->removeChild (regionDecals[i].get());
	}
	regionDecals.clear ();
}

void DlgRegionEdit::show (bool show)
{
	if (_dialog)
	{
		_dialog->show (show ? ATOM_Widget::ShowNormal : ATOM_Widget::Hide);

		if (_currentDef >= 0 && _currentDef < _regionDefs.size())
		{
			const RegionDef &def = _regionDefs[_currentDef];
			for (int i = 0; i < regionDecals.size(); ++i)
			{
				regionDecals[i]->setShow (show?ATOM_Node::SHOW:ATOM_Node::HIDE);
			}
		}
	}
}

ATOM_Dialog *DlgRegionEdit::getWidget (void) const
{
	return _dialog;
}

void DlgRegionEdit::loadCfg (const char *configFileName)
{
	ATOM_AutoFile f(configFileName, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return;
	}

	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error ())
	{
		return;
	}

	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
	{
		return;
	}

	for (ATOM_TiXmlElement *regionDef = root->FirstChildElement("Def"); regionDef; regionDef = regionDef->NextSiblingElement("Def"))
	{
		RegionDef def;

		const char *name = regionDef->Attribute ("Name");
		if (!name)
		{
			continue;
		}
		for (int i = 0; i < _regionDefs.size(); ++i)
		{
			if (!stricmp(_regionDefs[i].name.c_str(), name))
			{
				continue;
			}
		}
		def.name = name;

		const char *type = regionDef->Attribute ("Type");
		if (!type)
		{
			continue;
		}
		if (!stricmp (type, "Rect"))
		{
			def.type = RT_RECT;
		}
		else
		{
			continue;
		}

		def.defSize.w = 100;
		def.defSize.h = 100;
		const char *defSize = regionDef->Attribute ("DefSize");
		if (defSize)
		{
			int w, h;
			if (2 == sscanf (defSize, "%d,%d", &w, &h))
			{
				def.defSize.w = w;
				def.defSize.h = h;
			}
		}

		def.flags = 0;
		int rotatable = 1;
		int scalable = 1;
		regionDef->QueryIntAttribute ("Rotatable", &rotatable);
		regionDef->QueryIntAttribute ("Scalable", &scalable);
		if (rotatable)
		{
			def.flags |= RF_ROTATABLE;
		}
		if (scalable)
		{
			def.flags |= RF_SCALABLE;
		}

		def.colorKeyIndex = -1;
		const char *colorKey = regionDef->Attribute ("ColorKey");

		def.defColor.setFloats (1.f, 1.f, 1.f, 1.f);
		const char *defColor = regionDef->Attribute ("DefColor");
		if (defColor && defColor[0] == '#')
		{
			unsigned colorValue;
			if (1 == sscanf (defColor+1, "%x", &colorValue))
			{
				def.defColor.setRaw (colorValue);
			}
		}

		ATOM_GUIFont::handle font = _dialog->getFont ();
		ATOM_FontHandle hFont = ATOM_GUIFont::getFontHandle (font);

		ATOM_TiXmlElement *eColorMap = regionDef->FirstChildElement ("Colors");
		if (eColorMap)
		{
			for (ATOM_TiXmlElement *clr = eColorMap->FirstChildElement ("Color"); clr; clr = clr->NextSiblingElement ("Color"))
			{
				const char *s = clr->Attribute ("Key");
				if (!s)
				{
					continue;
				}

				int key;
				if (1 != sscanf (s, "%d", &key))
				{
					continue;
				}

				s = clr->Attribute ("Value");
				if (!s || s[0] != '#')
				{
					continue;
				}
				int color;
				if (1 != sscanf (s+1, "%x", &color))
				{
					continue;
				}

				def.colorMap[key] = color;
			}
		}

		int totalWidth = 0;
		ATOM_TiXmlElement *eAttrbs = regionDef->FirstChildElement ("Attributes");
		if (eAttrbs)
		{
			for (ATOM_TiXmlElement *attrib = eAttrbs->FirstChildElement ("Attrib"); attrib; attrib = attrib->NextSiblingElement("Attrib"))
			{
				RegionAttribDef attribDef;
				int width;

				const char *name = attrib->Attribute ("Name");
				if (!name)
				{
					continue;
				}
				for (int i = 0; i < def.attributes.size(); ++i)
				{
					if (!stricmp (def.attributes[i].name.c_str(), name))
					{
						//重名的属性跳过
						continue;
					}
				}
				attribDef.name = name;
				ATOM_CalcStringBounds (hFont, name, strlen(name), 0, 0, &attribDef.defWidth, 0);
				attribDef.defWidth += 10;

				const char *type = attrib->Attribute ("Type");
				if (!type)
				{
					continue;
				}
				if (!stricmp (type, "int"))
				{
					attribDef.type = ATOM_Variant::INT;
					const char *number = "00000000";
					ATOM_CalcStringBounds (hFont, number, strlen(number), 0, 0, &width, 0);
					if (width > attribDef.defWidth)
					{
						attribDef.defWidth = width;
					}
				}
				else if (!stricmp (type, "float"))
				{
					attribDef.type = ATOM_Variant::FLOAT;
					const char *number = "00000000";
					ATOM_CalcStringBounds (hFont, number, strlen(number), 0, 0, &width, 0);
					if (width > attribDef.defWidth)
					{
						attribDef.defWidth = width;
					}
				}
				else if (!stricmp (type, "string"))
				{
					attribDef.type = ATOM_Variant::STRING;
					const char *str = "abcdefghijklmnopq";
					ATOM_CalcStringBounds (hFont, str, strlen(str), 0, 0, &width, 0);
					if (width > attribDef.defWidth)
					{
						attribDef.defWidth = width;
					}
				}
				else
				{
					continue;
				}
				def.attributes.push_back (attribDef);

				if (colorKey && attribDef.name == colorKey)
				{
					def.colorKeyIndex = def.attributes.size() - 1;
				}

				totalWidth += attribDef.defWidth;
			}
		}
		const ATOM_Rect2Di &rc = m_lvRegionList->getClientRect();
		def.widthScale = float(rc.size.w - INDEX_COLUMN_WIDTH) / float(totalWidth);

		ATOM_TiXmlElement *eTempl = regionDef->FirstChildElement ("Template");
		if (eTempl)
		{
			const char *name = eTempl->Attribute ("Name");
			def.templ.name = name ? name : "default";
			def.templ.sortName = -1;

			const char *sortName = eTempl->Attribute ("Sort");
			if (sortName)
			{
				for (int i = 0; i < def.attributes.size(); ++i)
				{
					if (def.attributes[i].name == sortName)
					{
						def.templ.sortName = i;
						break;
					}
				}
			}
			
			const char *sep = eTempl->Attribute ("Sep");
			def.templ.sep = sep ? sep : "\t";

			const char *head = eTempl->Attribute ("Head");
			def.templ.head = head ? head : "";

			const char *lineTempl = 0;
			ATOM_TiXmlNode *node = eTempl->FirstChild ();
			if (node)
			{
				lineTempl = node->Value ();
			}
			def.templ.lineTemplate = lineTempl ? lineTempl : "";
		}

		_regionDefs.push_back (def);
	}
}

void DlgRegionEdit::initUI (void)
{
	int sel = -1;

	if (m_cbRegionType)
	{
		if (_regionDefs.size()>0)
		{
			for (int i = 0; i < _regionDefs.size(); ++i)
			{
				m_cbRegionType->getDropDownList()->addItem (_regionDefs[i].name.c_str(), i);
			}
			sel = 0;
		}
		else
		{
			sel = -1;
		}
		m_cbRegionType->select (sel, false);
	}

	_currentDef = sel >= 0 ? (int)m_cbRegionType->getDropDownList()->getItemData (sel) : -1;
	_currentRegion = -1;

	updateUI ();
}

void DlgRegionEdit::setRegionListHeader (const RegionDef &def)
{
	m_lvHeader->setColumnCount (def.attributes.size()+1);
	m_lvHeader->setColumnWidth (0, INDEX_COLUMN_WIDTH);
	for (int i = 0; i < def.attributes.size(); ++i)
	{
		m_lvHeader->setColumnWidth (i+1, def.attributes[i].defWidth*def.widthScale);
	}

	m_lvHeader->addRow ();
	m_lvHeader->setItemHeight (20);
	m_lvHeader->setLabel (0, 0, "序号", ATOM_Widget::AnyId, ATOM_Widget::Border)->setBorderMode(ATOM_Widget::Raise);
	for (int i = 0; i < def.attributes.size(); ++i)
	{
		const RegionAttribDef &attribDef = def.attributes[i];
		m_lvHeader->setLabel (i+1, 0, attribDef.name.c_str(), ATOM_Widget::AnyId, ATOM_Widget::Border)->setBorderMode(ATOM_Widget::Raise);
	}
}

static void Var2Str (const ATOM_Variant &var, char *buff)
{
	switch (var.getType())
	{
	case ATOM_Variant::INT:
		sprintf (buff, "%d", var.getI());
		break;
	case ATOM_Variant::FLOAT:
		sprintf (buff, "%.4f", var.getF());
		break;
	case ATOM_Variant::STRING:
		strcpy (buff, var.getS());
		break;
	default:
		buff[0] = '\0';
		break;
	}
}

static int makeAttribId (int row, int col)
{
	return (row << 16) | (col & 0xFFFF);
}

static int getRowById (int id)
{
	return id >> 16;
}

static int getColById (int id)
{
	return id & 0xFFFF;
}

void DlgRegionEdit::updateUI (void)
{
	m_lvHeader->clear ();
	m_lvRegionList->clear ();

	if (m_lvRegionList && _currentDef >= 0 && _currentDef < _regionDefs.size())
	{
		setRegionListHeader (_regionDefs[_currentDef]);

		m_lvRegionList->setColumnCount (m_lvHeader->getColumnCount());
		for (int i = 0; i < m_lvRegionList->getColumnCount(); ++i)
		{
			m_lvRegionList->setColumnWidth (i, m_lvHeader->getColumnWidth (i));
		}

		updateListView ();
	}
}

void DlgRegionEdit::updateListView (void)
{
	const RegionDef &def = _regionDefs[_currentDef];
	m_lvRegionList->setRowCount (def.regionList.size());
	for (int i = 0; i < def.regionList.size(); ++i)
	{
		char buffer[256];

		int row = i;
		sprintf (buffer, "%d", row+1);
		ATOM_Button *btn = m_lvRegionList->setButton (0, row, buffer, BUTTON_ID_START+row);
		btn->setClientImageId (ATOM_IMAGEID_CONTROL_BKGROUND);
		btn->setFontColor (ATOM_ColorARGB(0.f, 0.f, 1.f, 1.f));

		for (int a = 0; a < def.attributes.size(); ++a)
		{
			ATOM_ASSERT(def.regionList[i][a].getType() == def.attributes[a].type);
			Var2Str (def.regionList[i][a], buffer);
			ATOM_Edit *edit = m_lvRegionList->setEdit (a+1, row, buffer, makeAttribId(row, a), ATOM_Widget::Border);
			edit->setClientImageId(ATOM_IMAGEID_WINDOW_BKGROUND);
			edit->setCursorImageId(ATOM_IMAGEID_EDIT_CURSOR);
			edit->setCursorWidth(2);
			edit->setBorderMode (ATOM_Widget::Flat);
			edit->setFontColor (ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f));
			edit->setString (buffer);
			if (def.attributes[a].type == ATOM_Variant::INT || def.attributes[a].type == ATOM_Variant::FLOAT)
			{
				edit->setEditType (EDITTYPE_NUMBER);
			}
		}
	}
	updateCanvas ();
}

void DlgRegionEdit::selectRegion (int index)
{
	if (index != _currentRegion)
	{
		if (index >= 0)
		{
			ATOM_Button *button = (ATOM_Button*)m_lvRegionList->getWidget (0, index);
			ATOM_ASSERT(button);
			button->setClientImageId (_dialog->getAttributes()->getAttribute ("ButtonHilightImage").getI());
		}

		if (_currentRegion >= 0)
		{
			ATOM_Button *button = (ATOM_Button*)m_lvRegionList->getWidget (0, _currentRegion);
			ATOM_ASSERT(button);
			button->setClientImageId (ATOM_IMAGEID_CONTROL_BKGROUND);
		}

		_currentRegion = index;
	}
}

void DlgRegionEdit::removeRegion (int defIndex, int regionIndex)
{
	_regionDefs[defIndex].regionList.erase (_regionDefs[defIndex].regionList.begin() + regionIndex);
	_regionDefs[defIndex].regionTransforms.erase (_regionDefs[defIndex].regionTransforms.begin() + regionIndex);

	if (defIndex == _currentDef)
	{
		ATOM_Decal *decal = regionDecals[regionIndex].get();
		decal->getParent()->removeChild (decal);
		regionDecals.erase (regionDecals.begin() + regionIndex);

		_currentRegion = -1;
		updateUI ();

		updateDecalNames ();
	}
}

void DlgRegionEdit::removeAllRegions (int defIndex)
{
	while (!_regionDefs[defIndex].regionList.empty ())
	{
		removeRegion (defIndex, 0);
	}
}

void DlgRegionEdit::clear (void)
{
	for (int i = 0; i < _regionDefs.size(); ++i)
	{
		removeAllRegions (i);
	}
}

unsigned DlgRegionEdit::getNumRegions (int regionDefIndex) const
{
	return _regionDefs[regionDefIndex].regionList.size();
}

void DlgRegionEdit::onEditChanged (ATOM_EditTextChangedEvent *event)
{
	if (getHost() == m_lvRegionList)
	{
		int regionIndex = getRowById (event->id);
		int attribIndex = getColById (event->id);
		RegionDef &def = _regionDefs[_currentDef];
		ATOM_Variant &var = def.regionList[regionIndex][attribIndex];

		ATOM_Edit *edit = (ATOM_Edit*)m_lvRegionList->getWidget(attribIndex+1, regionIndex);
		ATOM_STRING s = edit->getString();
		switch (def.attributes[attribIndex].type)
		{
		case ATOM_Variant::INT:
			var.setI (atoi(s.c_str()));
			break;
		case ATOM_Variant::FLOAT:
			var.setF (atof(s.c_str()));
			break;
		case ATOM_Variant::STRING:
			var.setS (s.c_str());
			break;
		}

		if (attribIndex == _regionDefs[_currentDef].colorKeyIndex)
		{
			ATOM_ColorARGB decalColor = getDecalColor (def, regionIndex);
			regionDecals[regionIndex]->setColor (ATOM_Vector4f(decalColor.getFloatR(), decalColor.getFloatG(), decalColor.getFloatB(), 0.8f));
		}
	}
}

void DlgRegionEdit::onCommand (ATOM_WidgetCommandEvent *event)
{
	if (getHost() == _dialog)
	{
		switch (event->id)
		{
		case ID_BTNNEWREGION:
			if (_currentDef >= 0 && _currentDef < _regionDefs.size())
			{
				float xSize = _regionDefs[_currentDef].defSize.w;
				float zSize = _regionDefs[_currentDef].defSize.h;
				ATOM_AUTOREF(ATOM_Decal) decal = createDecal (ATOM_Matrix4x4f::getScaleMatrix (ATOM_Vector3f(xSize, 2000.f, zSize)), _currentRegion);
				_plugin->beginAddRegion (decal);
			}
			break;
		case ID_BTNDELETEREGION:
			if (_currentDef >= 0 && _currentDef < _regionDefs.size() && _currentRegion >= 0 && _currentRegion < getNumRegions(_currentDef))
			{
				char buffer[256];
				sprintf (buffer, "你确定要移除区域%d吗?", _currentRegion+1);
				if (IDYES == ::MessageBoxA (ATOM_APP->getMainWindow(), buffer, "ATOM3D编辑器", MB_YESNO|MB_ICONQUESTION))
				{
					removeRegion (_currentDef, _currentRegion);
				}
			}
			break;
		case ID_BTNCLOSE:
			show (false);
			break;
		case ID_BTNLOAD:
			if (1 == _plugin->getEditor()->getOpenFileNames("xml", "xml文件|*.xml|所有文件|*.*|", false, false, "打开区域数据文件"))
			{
				loadRegionFromFile (_plugin->getEditor()->getOpenedFileName(0));
			}
			break;
		case ID_BTNSAVE:
			if (1 == _plugin->getEditor()->getOpenFileNames("xml", "xml文件|*.xml|所有文件|*.*|", false, true, "保存区域数据文件"))
			{
				saveRegionToFile (_plugin->getEditor()->getOpenedFileName(0));
			}
			break;
		case ID_BTNEXPORT:
			if (1 == _plugin->getEditor()->getOpenFileNames(0, "所有文件|*.*|", false, true, "导出区域"))
			{
				exportRegionToFile (_plugin->getEditor()->getOpenedFileName(0));
			}
		}
	}
	else if (getHost() == m_lvRegionList)
	{
		if (event->id >= BUTTON_ID_START && event->id < BUTTON_ID_START + getNumRegions(_currentDef))
		{
			selectRegion (event->id - BUTTON_ID_START);
			_plugin->lookAtNode (regionDecals[event->id - BUTTON_ID_START].get());
		}
	}
}

void DlgRegionEdit::addRegion (ATOM_Decal *decal)
{
	RegionDef &def = _regionDefs[_currentDef];

	regionDecals.push_back(decal);
	def.regionTransforms.push_back (decal->getWorldMatrix());

	if (def.regionList.empty ())
	{
		def.regionList.resize (def.regionList.size()+1);
		def.regionList.back().resize (def.attributes.size());
		for (int i = 0; i < def.regionList.back().size(); ++i)
		{
			switch (def.attributes[i].type)
			{
			case ATOM_Variant::INT:
				def.regionList.back()[i].setI(0);
				break;
			case ATOM_Variant::FLOAT:
				def.regionList.back()[i].setF(0.f);
				break;
			case ATOM_Variant::STRING:
				def.regionList.back()[i].setS("");
				break;
			default:
				ATOM_ASSERT(0);
			}
		}
	}
	else
	{
		def.regionList.push_back( def.regionList.back());
	}

	updateUI ();
	updateDecalNames ();
}

void DlgRegionEdit::updateCanvas (void)
{
	const ATOM_Size2Di &sz = m_lvRegionList->getCanvasSize();
	const ATOM_Rect2Di &rc = m_lvRegionList->getClientRect();
	if (sz.w > rc.size.w)
	{
		m_lvRegionList->setStyle(m_lvRegionList->getStyle()|ATOM_Widget::HScroll);
	}
	else
	{
		m_lvRegionList->setStyle(m_lvRegionList->getStyle()&~ATOM_Widget::HScroll);
	}
	if (sz.h > rc.size.h)
	{
		m_lvRegionList->setStyle(m_lvRegionList->getStyle()|ATOM_Widget::VScroll);
	}
	else
	{
		m_lvRegionList->setStyle(m_lvRegionList->getStyle()&~ATOM_Widget::VScroll);
	}
}

ATOM_ColorARGB DlgRegionEdit::getDecalColor (const RegionDef &def, int regionIndex)
{
	if (regionIndex >= 0 && regionIndex < def.regionList.size())
	{
		if (def.colorKeyIndex < 0)
		{
			return def.defColor;
		}

		int key = def.regionList[regionIndex][def.colorKeyIndex].getI();
		ATOM_MAP<int, ATOM_ColorARGB>::const_iterator it = def.colorMap.find (key);
		if (it != def.colorMap.end())
		{
			return it->second;
		}
	}

	return def.defColor;
}

bool DlgRegionEdit::loadRegions (const ATOM_TiXmlElement *xml)
{
	clear ();

	for (const ATOM_TiXmlElement *eDef = xml->FirstChildElement("Def"); eDef; eDef = eDef->NextSiblingElement("Def"))
	{
		const char *name = eDef->Attribute("Name");
		int defIndex = -1;
		if (name)
		{
			for (int i = 0; i < _regionDefs.size(); ++i)
			{
				if (_regionDefs[i].name == name)
				{
					defIndex = i;
					break;
				}
			}
		}
		if (defIndex < 0)
		{
			continue;
		}

		RegionDef &def = _regionDefs[defIndex];
		for (const ATOM_TiXmlElement *eRegion = eDef->FirstChildElement("Region"); eRegion; eRegion = eRegion->NextSiblingElement("Region"))
		{
			ATOM_Vector3f t(0.f,0.f,0.f), s(1.f,1.f,1.f);
			ATOM_Quaternion r(0.f,0.f,0.f,1.f);

			const char *translate = eRegion->Attribute ("Translation");
			if (translate && 3 != sscanf(translate, "%f,%f,%f", &t.x, &t.y, &t.z))
			{
				continue;
			}
			const char *scale = eRegion->Attribute ("Scale");
			if (scale && 3 != sscanf(scale, "%f,%f,%f", &s.x, &s.y, &s.z))
			{
				continue;
			}
			const char *rotate = eRegion->Attribute ("Rotation");
			if (rotate && 4 != sscanf(rotate, "%f,%f,%f,%f", &r.x, &r.y, &r.z, &r.w))
			{
				continue;
			}

			ATOM_Matrix4x4f matrix;
			r.toMatrix (matrix);
			matrix.setRow3 (3, t);
			matrix >>= ATOM_Matrix4x4f::getScaleMatrix(s);

			def.regionTransforms.push_back (matrix);
			def.regionList.push_back (ATOM_VECTOR<ATOM_Variant>());
			for (int i = 0; i < def.attributes.size(); ++i)
			{
				const char *attribName = def.attributes[i].name.c_str();
				const char *attribValue = eRegion->Attribute(attribName);
				ATOM_Variant var;
				switch (def.attributes[i].type)
				{
				case ATOM_Variant::INT:
					var.setI (attribValue ? atoi(attribValue) : 0);
					break;
				case ATOM_Variant::FLOAT:
					var.setF (attribValue ? atof(attribValue) : 0.f);
					break;
				case ATOM_Variant::STRING:
					var.setS (attribValue ? attribValue : "");
					break;
				}
				def.regionList.back().push_back(var);
			}
		}
	}

	_currentRegion = -1;
	updateUI ();

	if (_currentDef >= 0 && _currentDef < _regionDefs.size())
	{
		for (int i = 0; i < _regionDefs[_currentDef].regionTransforms.size(); ++i)
		{
			ATOM_Matrix4x4f m = _regionDefs[_currentDef].regionTransforms[i];
			ATOM_Matrix4x4f invP = _plugin->_sceneRoot->getWorldMatrix();
			invP.invertAffine ();

			regionDecals.push_back (createDecal(invP>>m, i));
			_plugin->_sceneRoot->appendChild (regionDecals.back().get());
		}
	}

	updateDecalNames ();

	return true;
}

bool DlgRegionEdit::writeHead (ATOM_File *f, const RegionDef &def, const ATOM_Variant &key)
{
	char buf[256];

	const char *templ = def.templ.head.c_str();
	for (;;)
	{
		const char *tok = strchr(templ, '%');
		if (!tok)
		{
			f->write (templ, strlen(templ));
			break;
		}
		else
		{
			f->write (templ, tok - templ);
			templ = tok;

			if (!strnicmp (templ, "%_Key_%", strlen("%_Key_%")))
			{
				switch (key.getType ())
				{
				case ATOM_Variant::INT:
					sprintf (buf, "%d", key.getI());
					break;
				case ATOM_Variant::FLOAT:
					sprintf (buf, "%.2f", key.getF());
					break;
				case ATOM_Variant::STRING:
					strcpy (buf, key.getS());
					break;
				}
				f->write (buf, strlen(buf));
				templ += strlen("%_Key_%");
			}
		}
	}
	return true;
}

bool DlgRegionEdit::writeRegion (ATOM_File *f, const RegionDef &def, int regionIndex)
{
	char buf[256];

	const ATOM_Matrix4x4f &matrix = def.regionTransforms[regionIndex];
	ATOM_Vector3f t, s;
	ATOM_Matrix4x4f r;
	matrix.decompose (t, r, s);
	ATOM_Quaternion q;
	q.fromMatrix (r);
	ATOM_Vector3f rot = q.toEulerXYZ ();

	const char *regionTempl = def.templ.lineTemplate.c_str();
	for (;;)
	{
		const char *tok = strchr (regionTempl, '%');
		if (!tok)
		{
			f->write (regionTempl, strlen(regionTempl));
			break;
		}
		else
		{
			f->write (regionTempl, tok - regionTempl);
			regionTempl = tok;

			if (!strnicmp (regionTempl, "%_X_%", strlen("%_X_%")))
			{
				sprintf(buf, "%d", int(t.x));
				f->write (buf, strlen(buf));
				regionTempl += strlen("%_X_%");
			}
			else if (!strnicmp (regionTempl, "%_Y_%", strlen("%_Y_%")))
			{
				sprintf(buf, "%d", int(t.y));
				f->write (buf, strlen(buf));
				regionTempl += strlen("%_Y_%");
			}
			else if (!strnicmp (regionTempl, "%_Z_%", strlen("%_Z_%")))
			{
				sprintf(buf, "%d", int(t.z));
				f->write (buf, strlen(buf));
				regionTempl += strlen("%_Z_%");
			}
			else if (!strnicmp (regionTempl, "%_W_%", strlen("%_W_%")))
			{
				sprintf(buf, "%d", int(ATOM_abs(s.x)));
				f->write (buf, strlen(buf));
				regionTempl += strlen("%_W_%");
			}
			else if (!strnicmp (regionTempl, "%_H_%", strlen("%_H_%")))
			{
				sprintf(buf, "%d", int(ATOM_abs(s.z)));
				f->write (buf, strlen(buf));
				regionTempl += strlen("%_H_%");
			}
			else if (!strnicmp (regionTempl, "%_HW_%", strlen("%_HW_%")))
			{
				sprintf(buf, "%d", int(ATOM_abs(s.x)*0.5f));
				f->write (buf, strlen(buf));
				regionTempl += strlen("%_HW_%");
			}
			else if (!strnicmp (regionTempl, "%_HH_%", strlen("%_HH_%")))
			{
				sprintf(buf, "%d", int(ATOM_abs(s.z)*0.5f));
				f->write (buf, strlen(buf));
				regionTempl += strlen("%_HH_%");
			}
			else if (!strnicmp (regionTempl, "%_RX_%", strlen("%_RX_%")))
			{
				sprintf(buf, "%d", int(rot.x * 180.f / ATOM_Pi));
				f->write (buf, strlen(buf));
				regionTempl += strlen("%_RX_%");
			}
			else if (!strnicmp (regionTempl, "%_RY_%", strlen("%_RY_%")))
			{
				sprintf(buf, "%d", int(rot.y * 180.f / ATOM_Pi));
				f->write (buf, strlen(buf));
				regionTempl += strlen("%_RY_%");
			}
			else if (!strnicmp (regionTempl, "%_RZ_%", strlen("%_RZ_%")))
			{
				sprintf(buf, "%d", int(rot.z * 180.f / ATOM_Pi));
				f->write (buf, strlen(buf));
				regionTempl += strlen("%_RZ_%");
			}
			else
			{
				for (int i = 0; i < def.attributes.size(); ++i)
				{
					ATOM_STRING s = "%";
					s += def.attributes[i].name;
					s += "%";

					if (!strnicmp (regionTempl, s.c_str(), s.length()))
					{
						const ATOM_Variant &val = def.regionList[regionIndex][i];
						switch (val.getType ())
						{
						case ATOM_Variant::INT:
							sprintf (buf, "%d", val.getI());
							break;
						case ATOM_Variant::FLOAT:
							sprintf (buf, "%.2f", val.getF());
							break;
						case ATOM_Variant::STRING:
							strcpy (buf, val.getS());
							break;
						}
						f->write (buf, strlen(buf));
						regionTempl += s.length();
						break;
					}
				}
			}
		}
	}
	return true;
}

bool DlgRegionEdit::exportRegionToFile (const char *fileName)
{
	if (_currentDef >= 0 && _currentDef < _regionDefs.size())
	{
		const RegionDef &def = _regionDefs[_currentDef];
		if (def.templ.name.empty ())
		{
			return false;
		}

		ATOM_AutoFile f(fileName, ATOM_VFS::write|ATOM_VFS::text);
		if (!f)
		{
			return false;
		}

		ATOM_VECTOR<ATOM_VECTOR<int> > lineList;
		ATOM_VECTOR<ATOM_Variant> keyList;

		if (def.templ.sortName < 0)
		{
			lineList.resize (def.regionList.size());
			for (int i = 0; i < def.regionList.size(); ++i)
			{
				lineList[i].push_back (i);
			}
		}
		else
		{
			for (int i = 0; i < def.regionList.size(); ++i)
			{
				const ATOM_Variant &val = def.regionList[i][def.templ.sortName];
				bool newLine = true;
				for (int j = 0; j < lineList.size(); ++j)
				{
					const ATOM_Variant &sortValue = def.regionList[lineList[j][0]][def.templ.sortName];
					if (sortValue == val)
					{
						newLine = false;
						lineList[j].push_back (i);
						break;
					}
				}
				if (newLine)
				{
					lineList.resize (lineList.size() + 1);
					lineList.back().push_back (i);
					keyList.push_back (val);
				}
			}
		}

		// write
		for (int i = 0; i < lineList.size(); ++i)
		{
			if (!keyList.empty())
			{
				writeHead (f, def, keyList[i]);
			}

			for (int j = 0; j < lineList[i].size(); ++j)
			{
				writeRegion (f, def, lineList[i][j]);

				if (j < lineList[i].size() - 1)
				{
					f->write (def.templ.sep.c_str(), def.templ.sep.length());
				}
			}

			f->write ("\n", strlen("\n"));
		}
	}

	return true;
}

bool DlgRegionEdit::saveRegions (ATOM_TiXmlElement *xml)
{
	if (_currentDef >= 0 && _currentDef < _regionDefs.size())
	{
		ATOM_ASSERT(regionDecals.size() == _regionDefs[_currentDef].regionList.size());
		ATOM_ASSERT(regionDecals.size() == _regionDefs[_currentDef].regionTransforms.size());
		for (int i = 0; i < regionDecals.size(); ++i)
		{
			_regionDefs[_currentDef].regionTransforms[i] = regionDecals[i]->getWorldMatrix();
		}
	}

	for (int i = 0; i < _regionDefs.size(); ++i)
	{
		ATOM_TiXmlElement def("Def");
		def.SetAttribute ("Name", _regionDefs[i].name.c_str());

		ATOM_ASSERT(_regionDefs[i].regionTransforms.size() == _regionDefs[i].regionList.size());
		for (int j = 0; j < _regionDefs[i].regionList.size(); ++j)
		{
			ATOM_TiXmlElement region("Region");
			const ATOM_Matrix4x4f &matrix = _regionDefs[i].regionTransforms[j];
			ATOM_Vector3f t, s;
			ATOM_Matrix4x4f r;
			matrix.decompose (t, r, s);
			ATOM_Quaternion q;
			q.fromMatrix (r);

			char buffer[256];
			sprintf (buffer, "%f,%f,%f", t.x, t.y, t.z);
			region.SetAttribute("Translation", buffer);
			sprintf (buffer, "%f,%f,%f", s.x, s.y, s.z);
			region.SetAttribute("Scale", buffer);
			sprintf (buffer, "%f,%f,%f,%f", q.x,q.y,q.z,q.w);
			region.SetAttribute("Rotation", buffer);

			for (int a = 0; a < _regionDefs[i].attributes.size(); ++a)
			{
				const ATOM_Variant &var = _regionDefs[i].regionList[j][a];
				const char *attribName = _regionDefs[i].attributes[a].name.c_str();
				switch (var.getType())
				{
				case ATOM_Variant::INT:
					region.SetAttribute(attribName, var.getI());
					break;
				case ATOM_Variant::FLOAT:
					region.SetDoubleAttribute(attribName, var.getF());
					break;
				case ATOM_Variant::STRING:
					region.SetAttribute(attribName, var.getS());
					break;
				}
			}

			def.InsertEndChild (region);
		}

		xml->InsertEndChild (def);
	}

	return true;
}

bool DlgRegionEdit::loadRegionFromFile (const char *xmlFile)
{
	ATOM_AutoFile f(xmlFile, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return false;
	}

	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error ())
	{
		return false;
	}

	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
	{
		return false;
	}

	loadRegions (root);

	return true;
}

bool DlgRegionEdit::saveRegionToFile (const char *xmlFile)
{
	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName(xmlFile, buffer);

	ATOM_TiXmlDocument doc(buffer);
	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement eRoot("Regions");
	saveRegions (&eRoot);
	doc.InsertEndChild (eRoot);

	return doc.SaveFile ();
}

ATOM_AUTOREF(ATOM_Decal) DlgRegionEdit::createDecal (const ATOM_Matrix4x4f &matrix, int regionIndex)
{
	ATOM_AUTOREF(ATOM_Decal) decal = ATOM_HARDREF(ATOM_Decal)();
	decal->loadAttribute (NULL);
	decal->load (ATOM_GetRenderDevice());
	decal->setO2T (matrix);
	ATOM_AUTOPTR(ATOM_Material) m = ATOM_MaterialManager::createMaterialFromCore(ATOM_GetRenderDevice(), "/editor/materials/decal.mat");
	m->getParameterTable()->setInt ("isMask", 0);
	m->getParameterTable()->setInt ("isArrow", 1);
	decal->setMaterial (m.get());
	decal->enableBlend (true);
	decal->setSrcBlend (ATOM_RenderAttributes::BlendFunc_SrcAlpha);
	decal->setDestBlend (ATOM_RenderAttributes::BlendFunc_InvSrcAlpha);
	decal->setTexture (ATOM_GetColorTexture(0xFFFFFFFF));
	ATOM_AUTOREF(ATOM_Texture) arrowTexture = ATOM_CreateTextureResource ("/editor/textures/dir.png",ATOM_PIXEL_FORMAT_UNKNOWN,ATOM_LoadPriority_IMMEDIATE);	// wangjian modified
	decal->getMaterial()->getParameterTable()->setTexture ("decalTexture2", arrowTexture.get());
	ATOM_ColorARGB color = getDecalColor(_regionDefs[_currentDef], regionIndex);
	decal->setColor (ATOM_Vector4f(color.getFloatR(), color.getFloatG(), color.getFloatB(),0.8f));
	decal->setPrivate (1);

	char buffer[256];
	sprintf (buffer, "%s", _regionDefs[_currentDef].name.c_str());
	decal->setDescribe (buffer);

	return decal;
}

void DlgRegionEdit::onCBChanged (ATOM_ComboBoxSelectionChangedEvent *event)
{
	int sel = m_cbRegionType->getSelected ();
	_currentDef = sel >= 0 ? (int)m_cbRegionType->getDropDownList()->getItemData (sel) : -1;
	_currentRegion = -1;

	updateUI ();

	for (int i = 0; i < regionDecals.size(); ++i)
	{
		regionDecals[i]->getParent()->removeChild (regionDecals[i].get());
	}
	regionDecals.clear ();

	if (_currentDef >= 0 && _currentDef < _regionDefs.size())
	{
		for (int i = 0; i < _regionDefs[_currentDef].regionTransforms.size(); ++i)
		{
			regionDecals.push_back (createDecal(_regionDefs[_currentDef].regionTransforms[i], i));
			_plugin->_sceneRoot->appendChild (regionDecals.back().get());
		}
	}

}

void DlgRegionEdit::updateDecalNames (void)
{
	char buffer[256];
	if (_currentDef >= 0 && _currentDef < _regionDefs.size())
	{
		const RegionDef &def = _regionDefs[_currentDef];
		for (int i = 0; i < regionDecals.size(); ++i)
		{
			sprintf (buffer, "%s%d", def.name.c_str(), i+1);
			regionDecals[i]->setDescribe (buffer);
		}
	}
}

