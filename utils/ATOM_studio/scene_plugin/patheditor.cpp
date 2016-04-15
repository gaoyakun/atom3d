#include "StdAfx.h"
#include "atom3d_studio.h"
#include "plugin.h"
#include "gridnode.h"
#include "patheditor.h"
#include "plugin_scene.h"

ATOM_BEGIN_EVENT_MAP(DlgPathEditor, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(DlgPathEditor, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(DlgPathEditor, ATOM_EditTextChangedEvent, onEditTextChanged)
	ATOM_EVENT_HANDLER(DlgPathEditor, ATOM_ComboBoxSelectionChangedEvent, onPathSelected)
ATOM_END_EVENT_MAP

DlgPathEditor::DlgPathEditor (ATOM_Widget *parent, PluginScene *plugin)
{
	_plugin = plugin;
	_currentPath = -1;
	_currentPoint = -1;
	_dlg = (ATOM_Dialog*)parent->loadHierarchy ("/editor/ui/patheditor.ui");
	if (_dlg)
	{
		initControls (_dlg);
		_dlg->show (ATOM_Widget::Hide);
		_dlg->setEventTrigger (this);
	}
}

DlgPathEditor::~DlgPathEditor (void)
{
	if (_dlg)
	{
		_dlg->setEventTrigger (nullptr);
		ATOM_DELETE(_dlg);
		_dlg = nullptr;
	}
}

void DlgPathEditor::show (bool bShow)
{
	if (_dlg)
	{
		_dlg->show (bShow?ATOM_Widget::ShowNormal:ATOM_Widget::Hide);
	}
}

void DlgPathEditor::addPath (const char *name, const ATOM_Path3D &path)
{
	_pathList.resize (_pathList.size() + 1);
	PathInfo &info = _pathList.back();
	info.show = true;
	info.pathName = name;
	info.roundRadius = path.getRoundRadius();
	info.smoothness = path.getSmoothness();
	info.pathTrail = ATOM_HARDREF(ATOM_LineTrail)();
	info.pathTrail->loadAttribute (nullptr);
	info.pathTrail->load (ATOM_GetRenderDevice ());
	info.pathTrail->setFlags (0);
	info.pathTrail->setPrivate (1);
	info.pathTrail->setO2T (path.getWorldMatrix());
	_plugin->getSceneRoot()->appendChild (info.pathTrail.get());

	for (int i = 0; i < path.getNumControlPoints(); ++i)
	{
		const ATOM_Matrix4x4f &m = path.getControlPoint (i);
		ATOM_HARDREF(ATOM_ShapeNode) shapeNode;
		shapeNode->loadAttribute (NULL);
		shapeNode->load (ATOM_GetRenderDevice());
		shapeNode->setO2T (m);
		shapeNode->setScale (ATOM_Vector3f(3.f, 3.f, 3.f));
		shapeNode->setPrivate (1);
		shapeNode->setDescribe ("路径控制点");
		info.controlPoints.push_back (shapeNode);
		info.pathTrail->appendChild (shapeNode.get());
		ATOM_LineTrail::InflectionPoint lp;
		lp.position = m.getRow3(3);
		lp.color.setRaw (0xFFFFFFFF);
		lp.width = 2.f;
		lp.length = 0.f;
		lp.lengthDirty = false;
		info.pathTrail->appendInflectionPoint(lp);
	}
	m_cbPathList->insertItem (m_cbPathList->getDropDownList()->getItemCount(), info.pathName.c_str(), 0xFFFFFFFF);
	refreshPathTrail (_pathList.size() - 1);
	selectPath (_pathList.size() - 1);
}

void DlgPathEditor::addPath (ATOM_LineTrail *trail)
{
	_pathList.resize (_pathList.size() + 1);
	_pathList.back().pathName = newPathName();
	_pathList.back().roundRadius = 1.f;
	_pathList.back().smoothness = 3;
	_pathList.back().pathTrail = trail;
	_pathList.back().show = true;
	for (int i = 0; i < trail->getNumInflectionPoints(); ++i)
	{
		ATOM_HARDREF(ATOM_ShapeNode) shapeNode;
		shapeNode->loadAttribute (NULL);
		shapeNode->load (ATOM_GetRenderDevice());
		shapeNode->setO2T (ATOM_Matrix4x4f::getTranslateMatrix(trail->inflectionPoint(i).position));
		shapeNode->setScale (ATOM_Vector3f(3.f, 3.f, 3.f));
		shapeNode->setPrivate (1);
		shapeNode->setDescribe ("路径控制点");
		trail->appendChild (shapeNode.get());
		_pathList.back().controlPoints.push_back (shapeNode);
	}
	m_cbPathList->insertItem (m_cbPathList->getDropDownList()->getItemCount(), _pathList.back().pathName.c_str(), 0xFFFFFFFF);
	selectPath (_pathList.size() - 1);
}

int DlgPathEditor::getNumPaths (void) const
{
	return _pathList.size();
}

void DlgPathEditor::deletePath (int index)
{
	if (index >= 0 && index < _pathList.size())
	{
		_plugin->selectNode (nullptr);

		PathInfo &info = _pathList[index];
		if (info.pathTrail)
		{
			info.pathTrail->getParent()->removeChild(info.pathTrail.get());
			info.pathTrail = nullptr;
		}
		for (int i = 0; i < info.controlPoints.size(); ++i)
		{
			if (info.controlPoints[i])
			{
				info.controlPoints[i]->getParent()->removeChild(info.controlPoints[i].get());
			}
		}
		info.controlPoints.clear ();

		_pathList.erase (_pathList.begin() + index);
		
		selectPath (-1);
		setCurrentCtlPoint (nullptr);
	}
}

void DlgPathEditor::deletePathByName (const char *pathName)
{
	deletePath (getPathByName (pathName));
}

void DlgPathEditor::clear (void)
{
	while (_pathList.size() > 0)
	{
		deletePath (_pathList.size() - 1);
	}
}

int DlgPathEditor::getPathByCtlPointNode (ATOM_Node *node) const
{
	for (int i = 0; i < _pathList.size(); ++i)
	{
		const PathInfo &info = _pathList[i];

		if (info.pathTrail.get() == node)
		{
			return i;
		}

		for (int j = 0; j < info.controlPoints.size(); ++j)
		{
			if (info.controlPoints[j].get() == node)
			{
				return i;
			}
		}
	}
	return -1;
}

int DlgPathEditor::getPathByName (const char *pathName) const
{
	for (int i = 0; i < _pathList.size(); ++i)
	{
		if (_pathList[i].pathName == pathName)
		{
			return i;
		}
	}
	return -1;
}

const char *DlgPathEditor::getPathName (int path) const
{
	return _pathList[path].pathName.c_str();
}

float DlgPathEditor::getPathRoundRadius (int path) const
{
	return _pathList[path].roundRadius;
}

int DlgPathEditor::getPathSmoothness (int path) const
{
	return _pathList[path].smoothness;
}

void DlgPathEditor::refreshPathTrail (int path)
{
	ATOM_LineTrail *trail = _pathList[path].pathTrail.get();
	trail->clearInflectionPoints ();

	for (int i = 0; i < _pathList[path].controlPoints.size(); ++i)
	{
		_pathList[path].controlPoints[i]->setColor (i==0?ATOM_Vector4f(1.f,0.f,0.f,1.f):ATOM_Vector4f(0.f,1.f,1.f,1.f));
	}

	if (_pathList[path].smoothness == 0)
	{
		for (int i = 0; i < _pathList[path].controlPoints.size(); ++i)
		{
			ATOM_LineTrail::InflectionPoint lp;
			lp.position = _pathList[path].controlPoints[i]->getO2T().getRow3(3);
			lp.color.setRaw (0xFFFFFFFF);
			lp.width = 2.f;
			lp.length = 0.f;
			lp.lengthDirty = false;
			trail->appendInflectionPoint(lp);
		}
	}
	else
	{
		ATOM_VECTOR<ATOM_Vector3f> vecPos[2];
		ATOM_VECTOR<ATOM_Vector3f> vecLook[2];
		int current = 0;
		float radius = ATOM_saturate(_pathList[path].roundRadius) / 3.f;

		for (int i = 0; i < _pathList[path].controlPoints.size(); ++i)
		{
			const ATOM_Matrix4x4f &m = _pathList[path].controlPoints[i]->getO2T();
			ATOM_Vector3f eye, to , up;
			m.decomposeLookatLH (eye, to, up);
			vecPos[current].push_back (eye);
			vecLook[current].push_back (to - eye);
		}

		for (int smoothness = 0; smoothness < _pathList[path].smoothness; ++smoothness)
		{
			int next = 1 - current;
			vecPos[next].resize (0);
			vecLook[next].resize (0);

			vecPos[next].push_back (vecPos[current][0]);
			vecLook[next].push_back (vecLook[current][0]);
			for (int n = 1; n < vecPos[current].size()-1; ++n)
			{
				ATOM_Vector3f vPrev = vecPos[current][n-1];
				ATOM_Vector3f vThis = vecPos[current][n];
				ATOM_Vector3f vNext = vecPos[current][n+1];
				ATOM_Vector3f v1 = vThis + (vPrev - vThis) * radius;
				ATOM_Vector3f v2 = vThis + (vNext - vThis) * radius;
				vecPos[next].push_back (v1);
				vecPos[next].push_back (v2);

				ATOM_Vector3f lPrev = vecLook[current][n-1];
				ATOM_Vector3f lThis = vecLook[current][n];
				ATOM_Vector3f lNext = vecLook[current][n+1];
				ATOM_Vector3f l1 = lThis + (lPrev - lThis) * radius;
				ATOM_Vector3f l2 = lThis + (lNext - lThis) * radius;
				vecLook[next].push_back (l1);
				vecLook[next].push_back (l2);
			}
			vecPos[next].push_back (vecPos[current].back());
			vecLook[next].push_back (vecLook[current].back());
			current = next;
		}

		for (int i = 0; i < vecPos[current].size(); ++i)
		{
			ATOM_LineTrail::InflectionPoint lp;
			lp.position = vecPos[current][i];
			lp.color.setRaw (0xFFFFFFFF);
			lp.width = 2.f;
			lp.length = 0.f;
			lp.lengthDirty = false;
			trail->appendInflectionPoint(lp);
		}
	}
}

void DlgPathEditor::onCommand (ATOM_WidgetCommandEvent *event)
{
	switch (event->id)
	{
	case ID_BTNNEWPATH:
		{
			ATOM_HARDREF(ATOM_LineTrail) trail;
			trail->loadAttribute (nullptr);
			trail->load (ATOM_GetRenderDevice ());
			ATOM_LineTrail::InflectionPoint lp;
			lp.position.set(0.f, 0.f, 0.f);
			lp.color.setRaw (0xFFFFFFFF);
			lp.width = 2.f;
			lp.length = 0.f;
			lp.lengthDirty = false;
			trail->appendInflectionPoint(lp);
			lp.position.set(100.f, 0.f, 0.f);
			trail->appendInflectionPoint(lp);
			trail->setFlags (0);
			trail->setPrivate (1);
			trail->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());

			_plugin->beginAddPath (trail);

			break;
		}
	case ID_BTNDELETEPATH:
		{
			deletePath (_currentPath);
			break;
		}
	case ID_BTNNEWCTLPOINT:
		{
			addCtlPoint ();
			break;
		}
	case ID_BTNREMOVECTLPOINT:
		{
			removeCtlPoint ();
			break;
		}
	case ID_BTNSHOWHIDE:
		{
			showHidePath ();
			break;
		}
	}
}

void DlgPathEditor::onPathSelected (ATOM_ComboBoxSelectionChangedEvent *event)
{
	selectPath (event->index);
}


void DlgPathEditor::onEditTextChanged (ATOM_EditTextChangedEvent *event)
{
	if (_currentPath < 0)
	{
		return;
	}

	switch (event->id)
	{
	case ID_EDPATHNAME:
		{
			if (event->text != _pathList[_currentPath].pathName)
			{
				_pathList[_currentPath].pathName = event->text;
				m_cbPathList->setItemText (_currentPath, event->text.c_str());
			}
			break;
		}
	case ID_EDROUNDRADIUS:
		{
			float f = atof (event->text.c_str());
			if (f != _pathList[_currentPath].roundRadius)
			{
				_pathList[_currentPath].roundRadius = f;
				refreshPathTrail (_currentPath);
			}
			break;
		}
	case ID_EDSMOOTHNESS:
		{
			int n = atoi (event->text.c_str());
			if (n != _pathList[_currentPath].smoothness)
			{
				_pathList[_currentPath].smoothness = n;
				refreshPathTrail (_currentPath);
			}
			break;
		}
	}
}

ATOM_STRING DlgPathEditor::newPathName (void) const
{
	char buffer[256];
	int i = 1;
	for (;;)
	{
		sprintf (buffer, "路径%d", i++);
		if (getPathByName (buffer) < 0)
		{
			return buffer;
		}
	}
}

void DlgPathEditor::selectPath (int path)
{
	if (path != _currentPath)
	{
		_currentPath = path;
		_currentPoint = -1;

		if (path < 0)
		{
			m_edPathName->enable (false);
			m_edPathName->setString ("");

			m_edRoundRadius->enable (false);
			m_edRoundRadius->setString ("");

			m_edSmoothness->enable (false);
			m_edSmoothness->setString ("");
		}
		else
		{
			char buffer[256];

			m_edPathName->enable (true);
			m_edPathName->setString (_pathList[path].pathName.c_str());

			m_edRoundRadius->enable (true);
			sprintf (buffer, "%f", _pathList[path].roundRadius);
			m_edRoundRadius->setString (buffer);

			m_edSmoothness->enable (true);
			sprintf (buffer, "%d", _pathList[path].smoothness);
			m_edSmoothness->setString (buffer);
		}

		if (path != m_cbPathList->getSelected())
		{
			m_cbPathList->select(path, false);
		}
	}
}

ATOM_Dialog *DlgPathEditor::getDialog (void) const
{
	return _dlg;
}

void DlgPathEditor::setCurrentCtlPoint (ATOM_Node *node)
{
	if (node && _currentPath >= 0)
	{
		for (int i = 0; i < _pathList[_currentPath].controlPoints.size(); ++i)
		{
			if (_pathList[_currentPath].controlPoints[i].get() == node)
			{
				_currentPoint = i;
				break;
			}
		}
	}
	else
	{
		_currentPoint = -1;
	}
}

void DlgPathEditor::addCtlPoint (void)
{
	if (_currentPoint < 0 || _currentPath < 0)
	{
		return;
	}

	ATOM_Vector3f position;
	ATOM_HARDREF(ATOM_ShapeNode) shapeNode;
	shapeNode->loadAttribute (NULL);
	shapeNode->load (ATOM_GetRenderDevice());

	if (_currentPoint == _pathList[_currentPath].controlPoints.size()-1)
	{
		position = _pathList[_currentPath].controlPoints.back()->getO2T().getRow3(3) + ATOM_Vector3f (0.f, 0.f, 10.f);
		_pathList[_currentPath].controlPoints.push_back (shapeNode);
	}
	else
	{
		position = (_pathList[_currentPath].controlPoints[_currentPoint]->getO2T().getRow3(3) + _pathList[_currentPath].controlPoints[_currentPoint+1]->getO2T().getRow3(3)) * 0.5f;
		_pathList[_currentPath].controlPoints.insert (_pathList[_currentPath].controlPoints.begin()+(_currentPoint+1), shapeNode);
	}

	shapeNode->setO2T (ATOM_Matrix4x4f::getTranslateMatrix(position));
	shapeNode->setScale (ATOM_Vector3f(3.f, 3.f, 3.f));
	shapeNode->setPrivate (1);
	shapeNode->setDescribe ("路径控制点");
	_pathList[_currentPath].pathTrail->appendChild (shapeNode.get());

	refreshPathTrail(_currentPath);
}

void DlgPathEditor::removeCtlPoint (void)
{
	if (_currentPoint >= 0 && _currentPath >= 0)
	{
		ATOM_Node *ctlPointNode = _pathList[_currentPath].controlPoints[_currentPoint].get();
		ATOM_ASSERT(_plugin->getSelectedNode() == ctlPointNode);
		_pathList[_currentPath].controlPoints.erase (_pathList[_currentPath].controlPoints.begin()+_currentPoint);
		ctlPointNode->getParent()->removeChild (ctlPointNode);
		refreshPathTrail (_currentPath);
	}
}

void DlgPathEditor::showHidePath (void)
{
	if (_currentPath >= 0)
	{
		_pathList[_currentPath].show = !_pathList[_currentPath].show;
		_pathList[_currentPath].pathTrail->setShow (_pathList[_currentPath].show?ATOM_Node::SHOW:ATOM_Node::HIDE);
	}
}

void DlgPathEditor::createPath (int index, ATOM_Path3D *path) const
{
	const PathInfo &info = _pathList[index];
	path->setRoundRadius (info.roundRadius);
	path->setSmoothness (info.smoothness);
	path->setWorldMatrix (info.pathTrail->getWorldMatrix ());
	for (int i = 0; i < info.controlPoints.size(); ++i)
	{
		path->appendControlPoint (info.controlPoints[i]->getO2T ());
	}
}

