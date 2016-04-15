#include "StdAfx.h"
#include "curved.h"
#include "editor.h"

#define CURVEEDITOR_UIFILE "/editor/ui/curveeditor.ui"

ATOM_BEGIN_EVENT_MAP(CurveEd, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(CurveEd, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

CurveEd::CurveEd (AS_Editor *editor, ATOM_Widget *parent)
{
	_editor = editor;
	_dialog = (ATOM_Dialog*)parent->loadHierarchy (CURVEEDITOR_UIFILE);

	if (_dialog)
	{
		const ATOM_Rect2Di &rc = _dialog->getWidgetRect ();
		const ATOM_Rect2Di &parentRc = parent->getWidgetRect ();
		_dialog->moveTo ((parentRc.size.w - rc.size.w) / 2, (parentRc.size.h - rc.size.h) / 2);
		_dialog->setEventTrigger (this);
		hide ();
	}

	initControls (_dialog);
}

CurveEd::~CurveEd (void)
{
	if (_dialog)
	{
		_dialog->setEventTrigger (NULL);
		ATOM_DELETE(_dialog);
	}
}

void CurveEd::show (void)
{
	if (_dialog)
	{
		_dialog->show (ATOM_Widget::ShowNormal);
	}
}

void CurveEd::hide (void)
{
	if (_dialog)
	{
		_dialog->show (ATOM_Widget::Hide);
	}
}

const ATOM_FloatAnimationCurve &CurveEd::getCurve (void) const
{
	static ATOM_FloatAnimationCurve tmpCurve;
	return m_ceCurve ? m_ceCurve->getCurve () : tmpCurve;
}

void CurveEd::setCurve (const ATOM_FloatAnimationCurve &curve)
{
	if (m_ceCurve)
	{
		m_ceCurve->getCurve() = curve; 
	}
}

void CurveEd::onCommand (ATOM_WidgetCommandEvent *event)
{
	switch (event->id)
	{
	case ID_BTNSAVE:
		{
			if (_editor->getOpenFileNames ("xml", "XML文件|*.xml|所有文件|*.*|", false, true, "保存文件") == 1)
			{
				const char *filename = _editor->getOpenedFileName (0);
				if (!save (filename))
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "保存失败!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				}
			}
			break;
		}
	case ID_BTNLOAD:
		{
			if (_editor->getOpenFileNames ("xml", "XML文件|*.xml|所有文件|*.*|", false, false, "载入文件") == 1)
			{
				const char *filename = _editor->getOpenedFileName (0);
				if (!load (filename))
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "打开失败!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				}
			}
			break;
		}
	case ID_BTNCLOSE:
		hide ();
		break;
	}
}

bool CurveEd::save (const char *filename)
{
	char nativeFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, nativeFileName);
	ATOM_TiXmlDocument doc (nativeFileName);
	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	const ATOM_FloatAnimationCurve &curve = m_ceCurve->getCurve();

	ATOM_TiXmlElement eRoot("Curve");

	eRoot.SetAttribute ("WaveType", int(curve.getWaveType()));
	eRoot.SetAttribute ("AddressMode", int(curve.getAddress()));
	for (unsigned i = 0; i < curve.getNumKeys(); ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		eKey.SetAttribute ("Key", curve[i].x);
		eKey.SetDoubleAttribute ("Value", curve[i].value());
		eRoot.InsertEndChild (eKey);
	}

	doc.InsertEndChild (eRoot);

	return doc.SaveFile ();
}

bool CurveEd::load (const char *filename)
{
	char nativeFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, nativeFileName);
	ATOM_TiXmlDocument doc (nativeFileName);
	if (!doc.LoadFile ())
	{
		return false;
	}

	ATOM_FloatAnimationCurve curve;

	ATOM_TiXmlElement *eRoot = doc.RootElement ();

	int waveType = ATOM_WAVE_TYPE_SPLINE;
	eRoot->QueryIntAttribute ("WaveType", &waveType);
	curve.setWaveType ((ATOM_WaveType)waveType);

	int addressMode = ATOM_CURVE_ADDRESS_CLAMP;
	eRoot->QueryIntAttribute ("AddressMode", &addressMode);
	curve.setAddress ((ATOM_CurveAddress)addressMode);

	ATOM_TiXmlElement *eKey = eRoot->FirstChildElement ("Key");
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
		curve.insertKey (time, value);
		eKey = eKey->NextSiblingElement ("Key");
	}

	m_ceCurve->getCurve() = curve;

	return true;
}
