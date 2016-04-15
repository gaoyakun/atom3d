#include "StdAfx.h"
#include "cell_proxy.h"

CellParameters::CellParameters (ControlProxy *proxy): WidgetParameters(proxy)
{
	_sizeX = 0;
	_sizeY = 0;
	_cellWidth = 0;
	_cellHeight = 0;
	_alignmentX = 1;
	_alignmentY = 1;
	_textOffsetX = 0;
	_textOffsetY = 0;
	_cooldownImage = -1;
	_selectedImage = -1;
	_frameImage = -1;
	_frameOffset = 0;
	_growmode = ATOM_Cell::GM_DOWN;
	_spaceX = 0;
	_spaceY = 0;
	_dragdrop = 0;
	_subscriptImageId = ATOM_INVALID_IMAGEID;
	_subscriptImageRect = ATOM_Rect2Di(0,0,0,0);
	_subscriptTextPosition = ATOM_Point2Di(0,0);
}

void CellParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	rect().size.w = 200;
	rect().size.h = 160;
	borderMode() = ATOM_Widget::Drop;
	style() |= ATOM_Widget::Border;
	imageId() = ATOM_IMAGEID_CONTROL_BKGROUND;

	_sizeX = 0;
	_sizeY = 0;
	_cellWidth = 0;
	_cellHeight = 0;
	_alignmentX = 1;
	_alignmentY = 1;
	_textOffsetX = 0;
	_textOffsetY = 0;
	_cooldownImage = -1;
	_selectedImage = -1;
	_frameImage = -1;
	_frameOffset = 0;
	_spaceX = 0;
	_spaceY = 0;
	_dragdrop = 0;
	_subscriptImageId = ATOM_INVALID_IMAGEID;
	_subscriptImageRect = ATOM_Rect2Di(0,0,0,0);
	_subscriptTextPosition = ATOM_Point2Di(0,0);
	_growmode = ATOM_Cell::GM_DOWN;
}

WidgetParameters *CellParameters::clone (void) const
{
	return ATOM_NEW(CellParameters, *this);
}

void CellParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addIntVar ("��������", PARAMID_CELL_SIZEX, _sizeX, false, "����", 0);
	propBar->addIntVar ("��������", PARAMID_CELL_SIZEY, _sizeY, false, "����", 0);
	propBar->addIntVar ("���ӿ��", PARAMID_CELL_CELLWIDTH, _cellWidth, false, "����", 0);
	propBar->addIntVar ("���Ӹ߶�", PARAMID_CELL_CELLHEIGHT, _cellHeight, false, "����", 0);
	propBar->addBoolVar ("���ӿ��Ϸ�", PARAMID_CELL_DRAGDROP, _dragdrop != 0, false, "����", 0);
	ATOMX_TBEnum growEnum;
	growEnum.addEnum ("����", ATOM_Cell::GM_UP);
	growEnum.addEnum ("����", ATOM_Cell::GM_DOWN);
	propBar->addEnum ("����", PARAMID_CELL_GROWMODE, _growmode, growEnum, false, "����", 0);
	ATOMX_TBEnum alignEnum;
	alignEnum.addEnum ("��", 0);
	alignEnum.addEnum ("��", 1);
	alignEnum.addEnum ("��", 2);
	propBar->addEnum ("�������", PARAMID_CELL_XALIGNMENT, _alignmentX, alignEnum, false, "����", 0);
	propBar->addEnum ("�������", PARAMID_CELL_YALIGNMENT, _alignmentY, alignEnum, false, "����", 0);
	propBar->addIntVar ("���ֺ���ƫ��", PARAMID_CELL_TEXTOFFSETX, _textOffsetX, false, "����", 0);
	propBar->addIntVar ("��������ƫ��", PARAMID_CELL_TEXTOFFSETY, _textOffsetY, false, "����", 0);
	propBar->addIntVar ("CDͼƬ", PARAMID_CELL_CD_IMAGE, _cooldownImage, false, "����", 0);
	propBar->addIntVar ("ѡ��ͼƬ", PARAMID_CELL_SELECTED_IMAGE, _selectedImage, false, "����", 0);
	propBar->addIntVar ("�߿�ͼƬ", PARAMID_CELL_FRAME_IMAGE, _frameImage, false, "����", 0);
	propBar->addIntVar ("�߿�ƫ��", PARAMID_CELL_FRAME_OFFSET, _frameOffset, false, "����", 0);
	propBar->addIntVar ("������", PARAMID_CELL_SPACE_X, _spaceX, false, "����", 0);
	propBar->addIntVar ("������", PARAMID_CELL_SPACE_Y, _spaceY, false, "����", 0);
	propBar->addIntVar ("�±�ͼƬ", PARAMID_CELL_SUBSCRIPT_IMAGE, _subscriptImageId, false, "����", 0);
	propBar->addIntVar ("�±�ͼƬ����ƫ��", PARAMID_CELL_SUBSCRIPT_IMAGEOFFSETX, _subscriptImageRect.point.x, false, "����", 0);
	propBar->addIntVar ("�±�ͼƬ����ƫ��", PARAMID_CELL_SUBSCRIPT_IMAGEOFFSETY, _subscriptImageRect.point.y, false, "����", 0);
	propBar->addIntVar ("�±�ͼƬ���", PARAMID_CELL_SUBSCRIPT_IMAGEWIDTH,  _subscriptImageRect.size.w, false, "����", 0);
	propBar->addIntVar ("�±�ͼƬ�߶�", PARAMID_CELL_SUBSCRIPT_IMAGEHEIGHT, _subscriptImageRect.size.h, false, "����", 0);
	propBar->addIntVar ("�±����ֺ���ƫ��", PARAMID_CELL_SUBSCRIPT_TEXTOFFSETX, _subscriptTextPosition.x, false, "����", 0);
	propBar->addIntVar ("�±���������ƫ��", PARAMID_CELL_SUBSCRIPT_TEXTOFFSETY, _subscriptTextPosition.y, false, "����", 0);
}

void CellParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	ATOM_Cell *cell = (ATOM_Cell*)widget;
	cell->setCellWidth (_cellWidth, _cellHeight);

	int alignment = 0;
	switch (_alignmentX)
	{
	case 0: alignment |= ATOM_Widget::AlignX_Left; break;
	case 1: alignment |= ATOM_Widget::AlignX_Middle; break;
	case 2: alignment |= ATOM_Widget::AlignX_Right; break;
	}
	switch (_alignmentY)
	{
	case 0: alignment |= ATOM_Widget::AlignY_Top; break;
	case 1: alignment |= ATOM_Widget::AlignY_Middle; break;
	case 2: alignment |= ATOM_Widget::AlignY_Bottom; break;
	}
	cell->setAlign (alignment);
	cell->setOffset (ATOM_Point2Di(_textOffsetX, _textOffsetY));
	cell->setCooldownImageId (_cooldownImage);
	cell->setSelectImageId (_selectedImage);
	cell->setFrameImageId (_frameImage);
	cell->setFrameOffset (_frameOffset);
	cell->setSpaceX (_spaceX);
	cell->setSpaceY (_spaceY);
	cell->setSubscriptTextPosition(_subscriptTextPosition);
	cell->setSubscriptImageRect(_subscriptImageRect);
	cell->setSubscriptImageId(_subscriptImageId);
	cell->setCellSize (_sizeX, _sizeY);
	cell->setGrowMode ((ATOM_Cell::GrowMode)_growmode);
	cell->allowCellDragDrop (_dragdrop != 0);

}

void CellParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("sizex", _sizeX);
	xml->SetAttribute ("sizey", _sizeY);
	xml->SetAttribute ("cellw", _cellWidth);
	xml->SetAttribute ("cellh", _cellHeight);
	xml->SetAttribute ("alignx", _alignmentX);
	xml->SetAttribute ("aligny", _alignmentY);
	xml->SetAttribute ("offsetx", _textOffsetX);
	xml->SetAttribute ("offsetY", _textOffsetY);
	xml->SetAttribute ("cdimage", _cooldownImage);
	xml->SetAttribute ("selimage", _selectedImage);
	xml->SetAttribute ("frameimage", _frameImage);
	xml->SetAttribute ("frameoffset", _frameOffset);
	xml->SetAttribute ("spacex", _spaceX);
	xml->SetAttribute ("spacey", _spaceY);
	xml->SetAttribute ("celldragdrop", _dragdrop);
	xml->SetAttribute ("growmode", _growmode);
	xml->SetAttribute("subtxtOffsetx",_subscriptTextPosition.x);
	xml->SetAttribute("subtxtOffsety",_subscriptTextPosition.y);
	xml->SetAttribute("subimgoffsetx",_subscriptImageRect.point.x);
	xml->SetAttribute("subimgoffsety",_subscriptImageRect.point.y);
	xml->SetAttribute("subimgw",_subscriptImageRect.size.w);
	xml->SetAttribute("subimgh",_subscriptImageRect.size.h);
	xml->SetAttribute("subimg",_subscriptImageId);
}

void CellParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("sizex", &_sizeX);
		xml->QueryIntAttribute ("sizey", &_sizeY);
		xml->QueryIntAttribute ("cellw", &_cellWidth);
		xml->QueryIntAttribute ("cellh", &_cellHeight);
		xml->QueryIntAttribute ("alignx", &_alignmentX);
		xml->QueryIntAttribute ("aligny", &_alignmentY);
		xml->QueryIntAttribute ("offsetx", &_textOffsetX);
		xml->QueryIntAttribute ("offsetY", &_textOffsetY);
		xml->QueryIntAttribute ("cdimage", &_cooldownImage);
		xml->QueryIntAttribute ("selimage", &_selectedImage);
		xml->QueryIntAttribute ("frameimage", &_frameImage);
		xml->QueryIntAttribute ("frameoffset", &_frameOffset);
		xml->QueryIntAttribute ("spacex", &_spaceX);
		xml->QueryIntAttribute ("spacey", &_spaceY);
		xml->QueryIntAttribute ("celldragdrop", &_dragdrop);
		xml->QueryIntAttribute ("growmode", &_growmode);
		xml->QueryIntAttribute("subtxtOffsetx", &_subscriptTextPosition.x);
		xml->QueryIntAttribute("subtxtOffsety", &_subscriptTextPosition.y);
		xml->QueryIntAttribute("subimgoffsetx", &_subscriptImageRect.point.x);
		xml->QueryIntAttribute("subimgoffsety", &_subscriptImageRect.point.y);
		xml->QueryIntAttribute("subimgw", &_subscriptImageRect.size.w);
		xml->QueryIntAttribute("subimgh", &_subscriptImageRect.size.h);
		xml->QueryIntAttribute("subimg", &_subscriptImageId);
	}
}

bool CellParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_CELL_SIZEX:
		_sizeX = event->newValue.getI();
		return true;
	case PARAMID_CELL_SIZEY:
		_sizeY = event->newValue.getI();
		return true;
	case PARAMID_CELL_CELLWIDTH:
		_cellWidth = event->newValue.getI();
		return true;
	case PARAMID_CELL_CELLHEIGHT:
		_cellHeight = event->newValue.getI();
		return true;
	case PARAMID_CELL_XALIGNMENT:
		_alignmentX = event->newValue.getI();
		return true;
	case PARAMID_CELL_YALIGNMENT:
		_alignmentY = event->newValue.getI();
		return true;
	case PARAMID_CELL_TEXTOFFSETX:
		_textOffsetX = event->newValue.getI();
		return true;
	case PARAMID_CELL_TEXTOFFSETY:
		_textOffsetY = event->newValue.getI();
		return true;
	case PARAMID_CELL_CD_IMAGE:
		_cooldownImage = event->newValue.getI();
		return true;
	case PARAMID_CELL_SELECTED_IMAGE:
		_selectedImage = event->newValue.getI();
		return true;
	case PARAMID_CELL_FRAME_IMAGE:
		_frameImage = event->newValue.getI();
		return true;
	case PARAMID_CELL_FRAME_OFFSET:
		_frameOffset = event->newValue.getI();
		return true;
	case PARAMID_CELL_SPACE_X:
		_spaceX = event->newValue.getI();
		return true;
	case PARAMID_CELL_SPACE_Y:
		_spaceY = event->newValue.getI();
		return true;
	case PARAMID_CELL_DRAGDROP:
		_dragdrop = event->newValue.getB() ? 1 : 0;
		return true;
	case PARAMID_CELL_GROWMODE:
		_growmode = event->newValue.getI();
		return true;
	case PARAMID_CELL_SUBSCRIPT_IMAGE:
		_subscriptImageId =  event->newValue.getI();
		return true;
	case PARAMID_CELL_SUBSCRIPT_IMAGEOFFSETX:
		_subscriptImageRect.point.x =  event->newValue.getI();
		return true;
	case PARAMID_CELL_SUBSCRIPT_IMAGEOFFSETY:
		_subscriptImageRect.point.y =  event->newValue.getI();
		return true;
	case PARAMID_CELL_SUBSCRIPT_IMAGEWIDTH:
		_subscriptImageRect.size.w =  event->newValue.getI();
		return true;
	case PARAMID_CELL_SUBSCRIPT_IMAGEHEIGHT:
		_subscriptImageRect.size.h =  event->newValue.getI();
		return true;
	case PARAMID_CELL_SUBSCRIPT_TEXTOFFSETX:
		_subscriptTextPosition.x =  event->newValue.getI();
		return true;
	case PARAMID_CELL_SUBSCRIPT_TEXTOFFSETY:
		_subscriptTextPosition.y =  event->newValue.getI();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool CellParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

//////////////////////////////////////////////////////////////////////////////////////

CellProxy::CellProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *CellProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_Cell *cell = ATOM_NEW(ATOM_Cell, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (cell);
	return cell;
}

void CellProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *CellProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(CellParameters, proxy);
}

