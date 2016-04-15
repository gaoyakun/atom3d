#include "StdAfx.h"
#include "editor.h"
#include "editor_impl.h"
#include "colorgrading_editor.h"

enum
{
	CGE_MENUID_OPEN,
	CGE_MENUID_SAVE,
	CGE_MENUID_SAVEAS,
	CGE_MENUID_EXPORT,
	CGE_MENUID_EXIT,
	CGE_MENUID_RGB,
	CGE_MENUID_RED,
	CGE_MENUID_GREEN,
	CGE_MENUID_BLUE,
	CGE_MENUID_TEXTURE_SIZE_4,
	CGE_MENUID_TEXTURE_SIZE_8,
	CGE_MENUID_TEXTURE_SIZE_16,
	CGE_MENUID_TEXTURE_SIZE_32,
	CGE_MENUID_LINEAR_FILTER,
	CGE_MENUID_POINT_FILTER,
	CGE_MENUID_INTERP_SPLINE,
	CGE_MENUID_INTERP_LINEAR,
	CGE_MENUID_INTERP_STEP
};

#define CURVEEDITOR_ID	1

ATOM_BEGIN_EVENT_MAP(ColorGradingEditor, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(ColorGradingEditor, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(ColorGradingEditor, ATOM_TopWindowCloseEvent, onClose)
	ATOM_EVENT_HANDLER(ColorGradingEditor, ATOM_CurveChangedEvent, onCurveChanged)
ATOM_END_EVENT_MAP

ColorGradingEditor::ColorGradingEditor (EditorImpl *editorImpl, ATOM_ColorGradingEffect *effect)
{
	_editorImpl = editorImpl;
	_colorGradingEffect = effect;
	_dialog = 0;
	_menuBar = 0;
	_curveEditor = 0;
	_currentChannel = Channel_None;
	_textureDimension = 0;
	_linearFilter = true;

	_rgbCurve.insertKey (0, 0);
	_rgbCurve.insertKey (1000, 1.f);
	_rgbCurve.setAddress (ATOM_CURVE_ADDRESS_CLAMP);
	_rgbCurve.setWaveType (ATOM_WAVE_TYPE_SPLINE);
	_redCurve.insertKey (0, 0);
	_redCurve.insertKey (1000, 1.f);
	_redCurve.setAddress (ATOM_CURVE_ADDRESS_CLAMP);
	_redCurve.setWaveType (ATOM_WAVE_TYPE_SPLINE);
	_greenCurve.insertKey (0, 0);
	_greenCurve.insertKey (1000, 1.f);
	_greenCurve.setAddress (ATOM_CURVE_ADDRESS_CLAMP);
	_greenCurve.setWaveType (ATOM_WAVE_TYPE_SPLINE);
	_blueCurve.insertKey (0, 0);
	_blueCurve.insertKey (1000, 1.f);
	_blueCurve.setAddress (ATOM_CURVE_ADDRESS_CLAMP);
	_blueCurve.setWaveType (ATOM_WAVE_TYPE_SPLINE);
}

ColorGradingEditor::~ColorGradingEditor (void)
{
	ATOM_DELETE(_dialog);
	ATOM_DELETE(_menuBar);
}

bool ColorGradingEditor::showEditor (void)
{
	if (!_dialog)
	{
		unsigned _dialogWidth = 400;
		unsigned _dialogHeight = 300;
		unsigned _dialogLeft = (_editorImpl->getRealtimeCtrl()->getClientRect().size.w - _dialogWidth) / 2;
		unsigned _dialogTop = (_editorImpl->getRealtimeCtrl()->getClientRect().size.h - _dialogHeight) / 2;

		_dialog = ATOM_NEW(ATOM_Dialog, _editorImpl->getRealtimeCtrl(), ATOM_Rect2Di(_dialogLeft, _dialogTop, _dialogWidth, _dialogHeight));

		_menuBar = ATOM_NEW(ATOM_MenuBar, _editorImpl->getGUIRenderer());
		_menuBar->appendMenuItem ("文件", 0);
		_menuBar->appendMenuItem ("编辑", 0);
		_menuBar->appendMenuItem ("贴图选项", 0);
		_menuBar->appendMenuItem ("曲线选项", 0);

		ATOM_PopupMenu *fileMenu = _menuBar->createSubMenu (0);
		fileMenu->appendMenuItem ("打开...", CGE_MENUID_OPEN);
		fileMenu->appendMenuItem ("保存", CGE_MENUID_SAVE);
		fileMenu->appendMenuItem ("另存...", CGE_MENUID_SAVEAS);
		fileMenu->appendMenuItem ("导出", CGE_MENUID_EXPORT);
		fileMenu->appendMenuItem ("退出", CGE_MENUID_EXIT);

		ATOM_PopupMenu *editMenu = _menuBar->createSubMenu (1);
		editMenu->appendMenuItem ("红绿蓝", CGE_MENUID_RGB);
		editMenu->appendMenuItem ("红", CGE_MENUID_RED);
		editMenu->appendMenuItem ("绿", CGE_MENUID_GREEN);
		editMenu->appendMenuItem ("蓝", CGE_MENUID_BLUE);

		ATOM_PopupMenu *optionMenu = _menuBar->createSubMenu (2);
		optionMenu->appendMenuItem ("体积纹理大小", 0);
		optionMenu->appendMenuItem ("纹理过滤", 0);
		ATOM_PopupMenu *sizeMenu = optionMenu->createSubMenu (0);
		sizeMenu->appendMenuItem ("4x4x4", CGE_MENUID_TEXTURE_SIZE_4);
		sizeMenu->appendMenuItem ("8x8x8", CGE_MENUID_TEXTURE_SIZE_8);
		sizeMenu->appendMenuItem ("16x16x16", CGE_MENUID_TEXTURE_SIZE_16);
		sizeMenu->appendMenuItem ("32x32x32", CGE_MENUID_TEXTURE_SIZE_32);
		ATOM_PopupMenu *filterMenu = optionMenu->createSubMenu (1);
		filterMenu->appendMenuItem ("线性过滤", CGE_MENUID_LINEAR_FILTER);
		filterMenu->appendMenuItem ("无", CGE_MENUID_POINT_FILTER);

		ATOM_PopupMenu *curveMenu = _menuBar->createSubMenu (3);
		curveMenu->appendMenuItem ("插值", 0);
		ATOM_PopupMenu *interpMenu = curveMenu->createSubMenu (0);
		interpMenu->appendMenuItem ("样条曲线", CGE_MENUID_INTERP_SPLINE);
		interpMenu->appendMenuItem ("线性", CGE_MENUID_INTERP_LINEAR);
		interpMenu->appendMenuItem ("阶跃", CGE_MENUID_INTERP_STEP);

		_dialog->setMenuBar (_menuBar);

		_curveEditor = ATOM_NEW(ATOM_CurveEditor, _dialog, ATOM_Rect2Di(ATOM_Point2Di(0,0), _dialog->getClientRect().size), ATOM_Widget::Border|ATOM_Widget::Control, CURVEEDITOR_ID, ATOM_Widget::ShowNormal);
		_curveEditor->setMenuBar (0);
		_curveEditor->setOriginTimeAxis (0.f);
		_curveEditor->setOriginValueAxis (float(_curveEditor->getClientRect().size.h-1)/float(_curveEditor->getClientRect().size.h));
		_curveEditor->setValueAxisUnitResolution (_curveEditor->getClientRect().size.h-2);
		_curveEditor->setTimeAxisUnitResolution (_curveEditor->getClientRect().size.w);
		_curveEditor->setMinTimeRange (0);
		_curveEditor->setMaxTimeRange (1000);
		_curveEditor->setMaxValue (1.f);
		_curveEditor->setMinValue (0.f);
		_curveEditor->enableScroll (true);

		setChannel (Channel_RGB);
		setTextureDimension (16);
		enableLinearFilter (true);

		_dialog->setEventTrigger (this);
	}

	_dialog->show (ATOM_Widget::ShowNormal);

	return true;
}

void ColorGradingEditor::hideEditor (void)
{
	if (_dialog)
	{
		_dialog->show (ATOM_Widget::Hide);
	}
}

void ColorGradingEditor::setChannel (int channel)
{
	if (channel != _currentChannel)
	{
		switch (_currentChannel)
		{
		case Channel_RGB:
			_rgbCurve = _curveEditor->getCurve();
			break;
		case Channel_R:
			_redCurve = _curveEditor->getCurve();
			break;
		case Channel_G:
			_greenCurve = _curveEditor->getCurve();
			break;
		case Channel_B:
			_blueCurve = _curveEditor->getCurve();
			break;
		}

		switch (channel)
		{
		case Channel_RGB:
			_curveEditor->getCurve() = _rgbCurve;
			_curveEditor->setCurveColor (ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f));
			_menuBar->getMenuItem (1).submenu->setCheck (0, true);
			_menuBar->getMenuItem (1).submenu->setCheck (1, false);
			_menuBar->getMenuItem (1).submenu->setCheck (2, false);
			_menuBar->getMenuItem (1).submenu->setCheck (3, false);
			break;
		case Channel_R:
			_curveEditor->getCurve() = _redCurve;
			_curveEditor->setCurveColor (ATOM_ColorARGB(1.f, 0.f, 0.f, 1.f));
			_menuBar->getMenuItem (1).submenu->setCheck (0, false);
			_menuBar->getMenuItem (1).submenu->setCheck (1, true);
			_menuBar->getMenuItem (1).submenu->setCheck (2, false);
			_menuBar->getMenuItem (1).submenu->setCheck (3, false);
			break;
		case Channel_G:
			_curveEditor->getCurve() = _greenCurve;
			_curveEditor->setCurveColor (ATOM_ColorARGB(0.f, 1.f, 0.f, 1.f));
			_menuBar->getMenuItem (1).submenu->setCheck (0, false);
			_menuBar->getMenuItem (1).submenu->setCheck (1, false);
			_menuBar->getMenuItem (1).submenu->setCheck (2, true);
			_menuBar->getMenuItem (1).submenu->setCheck (3, false);
			break;
		case Channel_B:
			_curveEditor->getCurve() = _blueCurve;
			_curveEditor->setCurveColor (ATOM_ColorARGB(0.f, 0.f, 1.f, 1.f));
			_menuBar->getMenuItem (1).submenu->setCheck (0, false);
			_menuBar->getMenuItem (1).submenu->setCheck (1, false);
			_menuBar->getMenuItem (1).submenu->setCheck (2, false);
			_menuBar->getMenuItem (1).submenu->setCheck (3, true);
			break;
		}

		_currentChannel = channel;
	}
}

int ColorGradingEditor::getChannel (void) const
{
	return _currentChannel;
}

void ColorGradingEditor::onCommand (ATOM_WidgetCommandEvent *event)
{
	switch (event->id)
	{
	case CGE_MENUID_OPEN:
		{
			HWND hwndOwner = _editorImpl->getRenderWindow()->getWindowInfo()->handle;
			if (_editorImpl->getOpenFileNames("lut", "颜色曲线文件(*.lut)|*.lut|", false, false, "选择颜色曲线文件") == 1)
			{
				const char *filename = _editorImpl->getOpenedFileName (0);

				if (!loadFromFile (filename))
				{
					HWND hwndOwner = _editorImpl->getRenderWindow()->getWindowInfo()->handle;
					::MessageBoxA (hwndOwner, "载入文件失败!", "错误", MB_OK|MB_ICONHAND);
				}
				else
				{
					ATOM_CurveChangedEvent event(_curveEditor->getId());
					onCurveChanged (&event);
				}
			}
			break;
		}
	case CGE_MENUID_SAVE:
	case CGE_MENUID_SAVEAS:
		{
			if (_editorImpl->getOpenFileNames("lut", "颜色曲线文件(*.lut)|*.lut|", false, true, "选择颜色曲线文件") == 1)
			{
				const char *filename = _editorImpl->getOpenedFileName (0);

				if (!saveToFile (filename))
				{
					HWND hwndOwner = _editorImpl->getRenderWindow()->getWindowInfo()->handle;
					::MessageBoxA (hwndOwner, "保存文件失败!", "错误", MB_OK|MB_ICONHAND);
				}
			}
			break;
		}
	case CGE_MENUID_EXPORT:
		{
			HWND hwndOwner = _editorImpl->getRenderWindow()->getWindowInfo()->handle;
			if (_editorImpl->getOpenFileNames("DDS", "DDS贴图(*.DDS)|*.DDS|", false, true, "导出校色贴图") == 1)
			{
				const char *filename = _editorImpl->getOpenedFileName (0);

				if (!exportTexture (filename, _textureDimension))
				{
					::MessageBoxA (hwndOwner, "导出失败!", "错误", MB_OK|MB_ICONHAND);
				}
				else
				{
					_colorGradingEffect->setLUTTextureFileName(filename);
				}
			}
			break;
		}
	case CGE_MENUID_EXIT:
		{
			_dialog->show(ATOM_Widget::Hide);
			break;
		}
	case CGE_MENUID_RGB:
		{
			if (getChannel() != Channel_RGB)
			{
				setChannel (Channel_RGB);
				ATOM_CurveChangedEvent event(_curveEditor->getId());
				onCurveChanged (&event);
			}
			break;
		}
	case CGE_MENUID_RED:
		{
			if (getChannel() != Channel_R)
			{
				setChannel (Channel_R);
				ATOM_CurveChangedEvent event(_curveEditor->getId());
				onCurveChanged (&event);
			}
			break;
		}
	case CGE_MENUID_GREEN:
		{
			if (getChannel() != Channel_G)
			{
				setChannel (Channel_G);
				ATOM_CurveChangedEvent event(_curveEditor->getId());
				onCurveChanged (&event);
			}
			break;
		}
	case CGE_MENUID_BLUE:
		{
			if (getChannel() != Channel_B)
			{
				setChannel (Channel_B);
				ATOM_CurveChangedEvent event(_curveEditor->getId());
				onCurveChanged (&event);
			}
			break;
		}
	case CGE_MENUID_INTERP_SPLINE:
		{
			setWaveType (ATOM_WAVE_TYPE_SPLINE);
			ATOM_CurveChangedEvent event(_curveEditor->getId());
			onCurveChanged (&event);
			break;
		}
	case CGE_MENUID_INTERP_LINEAR:
		{
			setWaveType (ATOM_WAVE_TYPE_LINEAR);
			ATOM_CurveChangedEvent event(_curveEditor->getId());
			onCurveChanged (&event);
			break;
		}
	case CGE_MENUID_INTERP_STEP:
		{
			setWaveType (ATOM_WAVE_TYPE_STEP);
			ATOM_CurveChangedEvent event(_curveEditor->getId());
			onCurveChanged (&event);
			break;
		}
	case CGE_MENUID_TEXTURE_SIZE_4:
		{
			if (getTextureDimension () != 4)
			{
				setTextureDimension (4);
				ATOM_CurveChangedEvent event(_curveEditor->getId());
				onCurveChanged (&event);
			}
			break;
		}
	case CGE_MENUID_TEXTURE_SIZE_8:
		{
			if (getTextureDimension () != 8)
			{
				setTextureDimension (8);
				ATOM_CurveChangedEvent event(_curveEditor->getId());
				onCurveChanged (&event);
			}
			break;
		}
	case CGE_MENUID_TEXTURE_SIZE_16:
		{
			if (getTextureDimension () != 16)
			{
				setTextureDimension (16);
				ATOM_CurveChangedEvent event(_curveEditor->getId());
				onCurveChanged (&event);
			}
			break;
		}
	case CGE_MENUID_TEXTURE_SIZE_32:
		{
			if (getTextureDimension () != 32)
			{
				setTextureDimension (32);
				ATOM_CurveChangedEvent event(_curveEditor->getId());
				onCurveChanged (&event);
			}
			break;
		}
	case CGE_MENUID_LINEAR_FILTER:
		{
			enableLinearFilter (true);
			break;
		}
	case CGE_MENUID_POINT_FILTER:
		{
			enableLinearFilter (false);
			break;
		}
	}
}

void ColorGradingEditor::onClose (ATOM_TopWindowCloseEvent *event)
{
	_dialog->show (ATOM_Widget::Hide);
}

void ColorGradingEditor::onCurveChanged (ATOM_CurveChangedEvent *event)
{
	switch (_currentChannel)
	{
	case Channel_RGB:
		_rgbCurve = _curveEditor->getCurve();
		break;
	case Channel_R:
		_redCurve = _curveEditor->getCurve();
		break;
	case Channel_G:
		_greenCurve = _curveEditor->getCurve();
		break;
	case Channel_B:
		_blueCurve = _curveEditor->getCurve();
	}

	if (_currentChannel == Channel_RGB)
	{
		updateColorGradingTexture (_rgbCurve, _rgbCurve, _rgbCurve, _textureDimension);
	}
	else
	{
		updateColorGradingTexture (_redCurve, _greenCurve, _blueCurve, _textureDimension);
	}
}

bool ColorGradingEditor::saveToFile (const char *filename)
{
	char nativeFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, nativeFileName);
	ATOM_TiXmlDocument doc (nativeFileName);
	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement eRoot("ColorLUT");
	eRoot.SetAttribute ("Active", int(_currentChannel));
	eRoot.SetAttribute ("LinearFilter", _linearFilter?1:0);
	eRoot.SetAttribute ("TextureSize", _textureDimension);

	ATOM_TiXmlElement eLUT("LUT_RGB");
	eLUT.SetAttribute ("WaveType", int(_rgbCurve.getWaveType()));
	for (unsigned i = 0; i < _rgbCurve.getNumKeys(); ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		eKey.SetAttribute ("Key", _rgbCurve[i].x);
		eKey.SetDoubleAttribute ("Value", _rgbCurve[i].value());
		eLUT.InsertEndChild (eKey);
	}
	eRoot.InsertEndChild (eLUT);

	ATOM_TiXmlElement eLUT_R("LUT_R");
	eLUT_R.SetAttribute ("WaveType", int(_redCurve.getWaveType()));
	for (unsigned i = 0; i < _redCurve.getNumKeys(); ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		eKey.SetAttribute ("Key", _redCurve[i].x);
		eKey.SetDoubleAttribute ("Value", _redCurve[i].value());
		eLUT_R.InsertEndChild (eKey);
	}
	eRoot.InsertEndChild (eLUT_R);

	ATOM_TiXmlElement eLUT_G("LUT_G");
	eLUT_G.SetAttribute ("WaveType", int(_greenCurve.getWaveType()));
	for (unsigned i = 0; i < _greenCurve.getNumKeys(); ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		eKey.SetAttribute ("Key", _greenCurve[i].x);
		eKey.SetDoubleAttribute ("Value", _greenCurve[i].value());
		eLUT_G.InsertEndChild (eKey);
	}
	eRoot.InsertEndChild (eLUT_G);

	ATOM_TiXmlElement eLUT_B("LUT_B");
	eLUT_B.SetAttribute ("WaveType", int(_blueCurve.getWaveType()));
	for (unsigned i = 0; i < _blueCurve.getNumKeys(); ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		eKey.SetAttribute ("Key", _blueCurve[i].x);
		eKey.SetDoubleAttribute ("Value", _blueCurve[i].value());
		eLUT_B.InsertEndChild (eKey);
	}
	eRoot.InsertEndChild (eLUT_B);

	doc.InsertEndChild (eRoot);

	if (!doc.SaveFile ())
	{
		return false;
	}

	return true;
}

bool ColorGradingEditor::loadFromFile (const char *filename)
{
	char nativeFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, nativeFileName);
	ATOM_TiXmlDocument doc (nativeFileName);
	if (!doc.LoadFile ())
	{
		return false;
	}

	ATOM_FloatAnimationCurve rgbCurve;
	ATOM_FloatAnimationCurve redCurve;
	ATOM_FloatAnimationCurve greenCurve;
	ATOM_FloatAnimationCurve blueCurve;

	ATOM_TiXmlElement *eRoot = doc.RootElement ();
	int channel = Channel_RGB;
	int textureDimension = 16;	
	int linearFilter = 1;
	int waveTypeRGB = ATOM_WAVE_TYPE_SPLINE;
	int waveTypeR = ATOM_WAVE_TYPE_SPLINE;
	int waveTypeG = ATOM_WAVE_TYPE_SPLINE;
	int waveTypeB = ATOM_WAVE_TYPE_SPLINE;

	eRoot->QueryIntAttribute ("Active", &channel);
	eRoot->QueryIntAttribute ("LinearFilter", &linearFilter);
	eRoot->QueryIntAttribute ("TextureSize", &textureDimension);

	ATOM_TiXmlElement *eLUT_RGB = eRoot->FirstChildElement ("LUT_RGB");
	if (!eLUT_RGB)
	{
		return false;
	}
	eLUT_RGB->QueryIntAttribute ("WaveType", &waveTypeRGB);
	rgbCurve.setWaveType ((ATOM_WaveType)waveTypeRGB);

	ATOM_TiXmlElement *eKey = eLUT_RGB->FirstChildElement ("Key");
	while (eKey)
	{
		int time;
		float value;
		if (ATOM_TIXML_NO_ATTRIBUTE == eKey->QueryIntAttribute ("Key", &time))
		{
			return false;
		}
		if (ATOM_TIXML_NO_ATTRIBUTE == eKey->QueryFloatAttribute ("Value", &value))
		{
			return false;
		}
		rgbCurve.insertKey (time, value);
		eKey = eKey->NextSiblingElement ("Key");
	}

	ATOM_TiXmlElement *eLUT_R = eRoot->FirstChildElement ("LUT_R");
	if (!eLUT_R)
	{
		return false;
	}
	eLUT_R->QueryIntAttribute ("WaveType", &waveTypeR);
	redCurve.setWaveType ((ATOM_WaveType)waveTypeR);

	eKey = eLUT_R->FirstChildElement ("Key");
	while (eKey)
	{
		int time;
		float value;
		if (ATOM_TIXML_NO_ATTRIBUTE == eKey->QueryIntAttribute ("Key", &time))
		{
			return false;
		}
		if (ATOM_TIXML_NO_ATTRIBUTE == eKey->QueryFloatAttribute ("Value", &value))
		{
			return false;
		}
		redCurve.insertKey (time, value);
		eKey = eKey->NextSiblingElement ("Key");
	}

	ATOM_TiXmlElement *eLUT_G = eRoot->FirstChildElement ("LUT_G");
	if (!eLUT_G)
	{
		return false;
	}
	eLUT_G->QueryIntAttribute ("WaveType", &waveTypeR);
	greenCurve.setWaveType ((ATOM_WaveType)waveTypeR);

	eKey = eLUT_G->FirstChildElement ("Key");
	while (eKey)
	{
		int time;
		float value;
		if (ATOM_TIXML_NO_ATTRIBUTE == eKey->QueryIntAttribute ("Key", &time))
		{
			return false;
		}
		if (ATOM_TIXML_NO_ATTRIBUTE == eKey->QueryFloatAttribute ("Value", &value))
		{
			return false;
		}
		greenCurve.insertKey (time, value);
		eKey = eKey->NextSiblingElement ("Key");
	}

	ATOM_TiXmlElement *eLUT_B = eRoot->FirstChildElement ("LUT_B");
	if (!eLUT_B)
	{
		return false;
	}
	eLUT_B->QueryIntAttribute ("WaveType", &waveTypeR);
	blueCurve.setWaveType ((ATOM_WaveType)waveTypeR);

	eKey = eLUT_B->FirstChildElement ("Key");
	while (eKey)
	{
		int time;
		float value;
		if (ATOM_TIXML_NO_ATTRIBUTE == eKey->QueryIntAttribute ("Key", &time))
		{
			return false;
		}
		if (ATOM_TIXML_NO_ATTRIBUTE == eKey->QueryFloatAttribute ("Value", &value))
		{
			return false;
		}
		blueCurve.insertKey (time, value);
		eKey = eKey->NextSiblingElement ("Key");
	}

	_rgbCurve = rgbCurve;
	_redCurve = redCurve;
	_greenCurve = greenCurve;
	_blueCurve = blueCurve;

	setChannel (channel);

	switch (_currentChannel)
	{
	case Channel_RGB:
		_curveEditor->getCurve() = _rgbCurve;
		break;
	case Channel_R:
		_curveEditor->getCurve() = _redCurve;
		break;
	case Channel_G:
		_curveEditor->getCurve() = _greenCurve;
		break;
	case Channel_B:
		_curveEditor->getCurve() = _blueCurve;
		break;
	}

	setTextureDimension (textureDimension);	
	enableLinearFilter (linearFilter != 0);

	return true;
}

bool ColorGradingEditor::exportTexture (const char *filename, int dim)
{
	if (_currentChannel == Channel_RGB)
	{
		updateColorGradingTexture (_rgbCurve, _rgbCurve, _rgbCurve, dim);
	}
	else
	{
		updateColorGradingTexture (_redCurve, _greenCurve, _blueCurve, dim);
	}

	ATOM_AUTOREF(ATOM_Texture) colorGradingTexture = _colorGradingEffect->getLUT ();
	if (!colorGradingTexture)
	{
		return false;
	}
	else if (!colorGradingTexture->saveToFileEx (filename, ATOM_PIXEL_FORMAT_BGRA8888))
	{
		return false;
	}

	return true;
}

void ColorGradingEditor::setTextureDimension (int dim)
{
	_textureDimension = dim;

	switch (dim)
	{
	case 4:
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (0, true);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (1, false);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (2, false);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (3, false);
		break;
	case 8:
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (0, false);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (1, true);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (2, false);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (3, false);
		break;
	case 16:
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (0, false);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (1, false);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (2, true);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (3, false);
		break;
	case 32:
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (0, false);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (1, false);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (2, false);
		_menuBar->getMenuItem (2).submenu->getMenuItem(0).submenu->setCheck (3, true);
		break;
	}
}

int ColorGradingEditor::getTextureDimension (void) const
{
	return _textureDimension;
}

void ColorGradingEditor::enableLinearFilter (bool enable)
{
	_linearFilter = enable;

	if (enable)
	{
		_menuBar->getMenuItem (2).submenu->getMenuItem(1).submenu->setCheck (0, true);
		_menuBar->getMenuItem (2).submenu->getMenuItem(1).submenu->setCheck (1, false);
	}
	else
	{
		_menuBar->getMenuItem (2).submenu->getMenuItem(1).submenu->setCheck (0, false);
		_menuBar->getMenuItem (2).submenu->getMenuItem(1).submenu->setCheck (1, true);
	}
}

bool ColorGradingEditor::isLinearFilterEnabled (void) const
{
	return _linearFilter;
}

void ColorGradingEditor::setWaveType (ATOM_WaveType waveType)
{
	_curveEditor->getCurve().setWaveType (waveType);

	switch (waveType)
	{
	case ATOM_WAVE_TYPE_SPLINE:
		_menuBar->getMenuItem (3).submenu->getMenuItem(0).submenu->setCheck (0, true);
		_menuBar->getMenuItem (3).submenu->getMenuItem(0).submenu->setCheck (1, false);
		_menuBar->getMenuItem (3).submenu->getMenuItem(0).submenu->setCheck (2, false);
		break;
	case ATOM_WAVE_TYPE_LINEAR:
		_menuBar->getMenuItem (3).submenu->getMenuItem(0).submenu->setCheck (0, false);
		_menuBar->getMenuItem (3).submenu->getMenuItem(0).submenu->setCheck (1, true);
		_menuBar->getMenuItem (3).submenu->getMenuItem(0).submenu->setCheck (2, false);
		break;
	case ATOM_WAVE_TYPE_STEP:
		_menuBar->getMenuItem (3).submenu->getMenuItem(0).submenu->setCheck (0, false);
		_menuBar->getMenuItem (3).submenu->getMenuItem(0).submenu->setCheck (1, false);
		_menuBar->getMenuItem (3).submenu->getMenuItem(0).submenu->setCheck (2, true);
		break;
	}
}

ATOM_WaveType ColorGradingEditor::getWaveType (void) const
{
	return _curveEditor->getCurve().getWaveType ();
}

bool ColorGradingEditor::updateColorGradingTexture (const ATOM_FloatAnimationCurve &redCurve, const ATOM_FloatAnimationCurve &greenCurve, const ATOM_FloatAnimationCurve &blueCurve, int dim)
{
	ATOM_AUTOREF(ATOM_Image) image = createColorGradingLUTImage (dim, dim, dim);
	ATOM_ColorARGB *pixels = (ATOM_ColorARGB*)image->getData();

	for (unsigned b = 0; b < dim; ++b)
	{
		for (unsigned g = 0; g < dim; ++g)
		{
			for (unsigned r = 0; r < dim; ++r)
			{
				long red = 1000 * pixels->getFloatR();
				long green = 1000 * pixels->getFloatG();
				long blue = 1000 * pixels->getFloatB();
				unsigned char red2 = ATOM_max2(ATOM_min2(redCurve.eval (red), 1.0), 0.0) * 255;
				unsigned char green2 = ATOM_max2(ATOM_min2(greenCurve.eval (green), 1.0), 0.0) * 255;
				unsigned char blue2 = ATOM_max2(ATOM_min2(blueCurve.eval (blue), 1.0), 0.0) * 255;
				pixels->setBytes (red2, green2, blue2, 255);

				pixels++;
			}
		}
	}

	ATOM_AUTOREF(ATOM_Texture) colorGradingTexture = _colorGradingEffect->getLUT ();
	if (!colorGradingTexture)
	{
		colorGradingTexture = ATOM_GetRenderDevice()->allocVolumeTexture (0, image->getData(), dim, dim, dim, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::NOMIPMAP|ATOM_Texture::NOCOMPRESS|ATOM_Texture::TEXTURE3D);
		_colorGradingEffect->setLUT (colorGradingTexture.get());
	}
	colorGradingTexture->loadTexImageFromMemory (image->getData(), dim, dim, dim, ATOM_PIXEL_FORMAT_BGRA8888, 1);

	return true;
}

ATOM_AUTOREF(ATOM_Image) ColorGradingEditor::createColorGradingLUTImage (unsigned w, unsigned h, unsigned d)
{
	ATOM_HARDREF(ATOM_Image) image;

	if (!image)
	{
		return 0;
	}

	image->init (w, h * d, ATOM_PIXEL_FORMAT_BGRA8888);

	ATOM_ColorARGB *colors = (ATOM_ColorARGB*)image->getData ();
	unsigned rScale = 255/(w-1);
	unsigned gScale = 255/(h-1);
	unsigned bScale = 255/(d-1);

	for (unsigned i = 0; i < h * d; ++i)
	{
		for (unsigned j = 0; j < w; ++j)
		{
			unsigned r = j % w;
			unsigned g = i % h;
			unsigned b = i / d;
			colors->setBytes (r*rScale, g*gScale, b*bScale, 255);
			colors++;
		}
	}

	ATOM_AUTOREF(ATOM_Image) ret = image.get();

	return ret;
}
