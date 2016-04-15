#include "StdAfx.h"
#include "flash_proxy.h"
#include "plugin_gui.h"

const ATOM_STRING &FlashParameters::SWFFileName (void) const
{
	return _fileName;
}

ATOM_STRING &FlashParameters::SWFFileName (void)
{
	return _fileName;
}

const ATOM_STRING &FlashParameters::initFunc (void) const
{
	return _initFunc;
}

ATOM_STRING &FlashParameters::initFunc (void)
{
	return _initFunc;
}

const ATOM_STRING &FlashParameters::clearFunc (void) const
{
	return _clearFunc;
}

ATOM_STRING &FlashParameters::clearFunc (void)
{
	return _clearFunc;
}

const ATOM_STRING &FlashParameters::activateFunc (void) const
{
	return _activateFunc;
}

ATOM_STRING &FlashParameters::activateFunc (void)
{
	return _activateFunc;
}

const ATOM_STRING &FlashParameters::deactivateFunc (void) const
{
	return _deactivateFunc;
}

ATOM_STRING &FlashParameters::deactivateFunc (void)
{
	return _deactivateFunc;
}

int FlashParameters::FPS (void) const
{
	return _FPS;
}

int &FlashParameters::FPS (void)
{
	return _FPS;
}

void FlashParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	_fileName = "";
	_initFunc = "initSwf";
	_clearFunc = "clearSwf";
	_activateFunc = "showSwf";
	_deactivateFunc = "hideSwf";
	_FPS = 24;
}

WidgetParameters *FlashParameters::clone (void) const
{
	return ATOM_NEW(FlashParameters, *this);
}

void FlashParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addButton ("SWFName", PARAMID_FLASH_FILENAME, "SWF文件", "Flash控件", 0);
	propBar->addStringVar ("加载回调方法", PARAMID_INIT_FUNC, _initFunc.c_str(), false, "Flash控件", 0);
	propBar->addStringVar ("销毁回调方法", PARAMID_CLEAR_FUNC, _clearFunc.c_str(), false, "Flash控件", 0);
	propBar->addStringVar ("激活回调方法", PARAMID_ACTIVATE_FUNC, _activateFunc.c_str(), false, "Flash控件", 0);
	propBar->addStringVar ("禁止回调方法", PARAMID_DEACTIVATE_FUNC, _deactivateFunc.c_str(), false, "Flash控件", 0);
	propBar->addUIntVar ("帧率", PARAMID_FPS, _FPS, false, "Flash控件", 0);
}

void FlashParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	if (!_fileName.empty ())
	{
		ATOM_FlashCtrl *flashCtrl = (ATOM_FlashCtrl*)widget;

		flashCtrl->setInitCallback (_initFunc.c_str());
		flashCtrl->setClearCallback (_clearFunc.c_str());
		flashCtrl->setActivateCallback (_activateFunc.c_str());
		flashCtrl->setDeactivateCallback (_deactivateFunc.c_str());
		flashCtrl->setFPS (_FPS);

		if (_fileName != flashCtrl->getSWFFileName ())
		{
			flashCtrl->loadSWF (_fileName.c_str());
		}
	}
}

void FlashParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	if (!_fileName.empty ())
	{
		xml->SetAttribute ("SWF", _fileName.c_str());
		xml->SetAttribute ("InitCallback", _initFunc.c_str());
		xml->SetAttribute ("ClearCallback", _clearFunc.c_str());
		xml->SetAttribute ("ActivateCallback", _activateFunc.c_str());
		xml->SetAttribute ("DeactivateCallback", _deactivateFunc.c_str());
		xml->SetAttribute ("FPS", _FPS);
	}
}

void FlashParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	const char *filename = xml ? xml->Attribute ("SWF") : 0;
	_fileName = filename ? filename : "";

	const char *initCB = xml ? xml->Attribute ("InitCallback") : 0;
	_initFunc = initCB ? initCB : "initSwf";
	const char *clearCB = xml ? xml->Attribute ("ClearCallback") : 0;
	_clearFunc = clearCB ? clearCB : "clearSwf";
	const char *activateCB = xml ? xml->Attribute ("ActivateCallback") : 0;
	_activateFunc = activateCB ? activateCB : "";
	const char *deactivateCB = xml ? xml->Attribute ("DeactivateCallback") : 0;
	_deactivateFunc = deactivateCB ? deactivateCB : "";

	if (xml)
	{
		xml->QueryIntAttribute ("FPS", &_FPS);
	}
}

bool FlashParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_INIT_FUNC:
		_initFunc = event->newValue.getS();
		return true;
	case PARAMID_CLEAR_FUNC:
		_clearFunc = event->newValue.getS();
		return true;
	case PARAMID_ACTIVATE_FUNC:
		_activateFunc = event->newValue.getS();
		return true;
	case PARAMID_DEACTIVATE_FUNC:
		_deactivateFunc = event->newValue.getS();
		return true;
	case PARAMID_FPS:
		_FPS = event->newValue.getI();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool FlashParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	switch (event->id)
	{
	case PARAMID_FLASH_FILENAME:
		{
			if (1 == getProxy()->getPlugin()->getEditor()->getOpenFileNames ("SWF", "Flash影片(*.SWF)|*.SWF|所有文件(*.*)|*.*|", false, false, "选择Flash影片"))
			{
				_fileName = getProxy()->getPlugin()->getEditor()->getOpenedFileName (0);
			}
			return true;
		}
	default:
		return WidgetParameters::handleBarCommandEvent (event);
	}
}

FlashProxy::FlashProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *FlashProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_FlashCtrl *flashCtrl = ATOM_NEW(ATOM_FlashCtrl, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (flashCtrl);
	return flashCtrl;
}

void FlashProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *FlashProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(FlashParameters, proxy);
}

