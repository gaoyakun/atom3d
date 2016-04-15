#include "StdAfx.h"
#include "resource.h"
#include "twpropbar.h"
#include "tw_events.h"

ATOM_BEGIN_EVENT_MAP(ATOMX_PropertyTweakBar, ATOMX_TweakBar)
	ATOM_EVENT_HANDLER(ATOMX_PropertyTweakBar, ATOMX_TWValueChangedEvent, onValueChanged)
	ATOM_EVENT_HANDLER(ATOMX_PropertyTweakBar, ATOMX_TWCommandEvent, onCommand)
ATOM_END_EVENT_MAP

#define BUTTON_ID_RETURN	-1234

void ATOMX_CALL writeToBarFunc (const char *name, ATOMX_TBValue &value, void *userData)
{
	ATOMX_PropertyTweakBar *bar = (ATOMX_PropertyTweakBar*)userData;
	ATOM_Object *object = bar->getObject ();
	ATOM_ScriptVar var;
	ATOM_GetAttribValue (object, name, var);

	ATOMX_TBType type = value.getType ();
	switch (type)
	{
	case ATOMX_TBTYPE_BOOL:
		value.setI (type, var.getInteger() ? 1 : 0);
		break;
	case ATOMX_TBTYPE_CHAR:
	case ATOMX_TBTYPE_INT8:
		value.setI (type, char(var.getInteger()));
		break;
	case ATOMX_TBTYPE_UINT8:
		value.setU (type, (unsigned char)(var.getInteger()));
		break;
	case ATOMX_TBTYPE_INT16:
		value.setI (type, short(var.getInteger()));
		break;
	case ATOMX_TBTYPE_UINT16:
		value.setU (type, (unsigned short)(var.getInteger()));
		break;
	case ATOMX_TBTYPE_INT32:
		value.setI (type, var.getInteger ());
		break;
	case ATOMX_TBTYPE_UINT32:
		value.setU (type, unsigned(var.getInteger()));
		break;
	case ATOMX_TBTYPE_FLOAT:
		value.setF (type, var.getFloat());
		break;
	case ATOMX_TBTYPE_COLOR3F:
	case ATOMX_TBTYPE_DIR3F:
	case ATOMX_TBTYPE_VECTOR3F:
		value.set3F (type, var.getVector3().x, var.getVector3().y, var.getVector3().z);
		break;
	case ATOMX_TBTYPE_COLOR4F:
	case ATOMX_TBTYPE_QUAT4F:
	case ATOMX_TBTYPE_VECTOR4F:
		value.set4F (type, var.getVector4().x, var.getVector4().y, var.getVector4().z, var.getVector4().w);
		break;
	case ATOMX_TBTYPE_STRING:
		value.setS (type, var.getString ().c_str());
		break;
	default:
		break;
	}
}

void ATOMX_CALL WriteToBarFunc (ATOMX_TBValue &value, void *userData)
{
}

ATOMX_PropertyTweakBar::ATOMX_PropertyTweakBar (const char *name)
: ATOMX_TweakBar (name)
{
}

ATOMX_PropertyTweakBar::ATOMX_PropertyTweakBar (const char *name, ATOM_Object *object)
: ATOMX_TweakBar (name)
{
	pushObject (object);
}

ATOM_Object *ATOMX_PropertyTweakBar::getObject (void) const
{
	return _objects.empty() ? 0 : _objects.back().get();
}

void ATOMX_PropertyTweakBar::setObject (ATOM_Object *object, bool clearBeforeSet)
{
	if (clearBeforeSet)
	{
		ATOMX_TweakBar::clear ();
		_varDetails.resize (0);
	}

	_objects.clear ();
	pushObject (object);
}

void ATOMX_PropertyTweakBar::clear (void)
{
	ATOMX_TweakBar::clear ();
	_varDetails.resize (0);
	_objects.resize (0);
}

void ATOMX_PropertyTweakBar::onValueChanged (ATOMX_TWValueChangedEvent *event)
{
	int id = event->id;

	if (id >= _varDetails.size())
	{
		callParentHandler(event);
		return;
	}

	ATOM_Object *_object = getObject();
	if (!_object)
	{
		callParentHandler(event);
		return;
	}

	const char *attribName = ATOM_GetAttribName(_object->getClassName(), _varDetails[id].attribIndex);

	switch (event->newValue.getType())
	{
	case ATOMX_TBTYPE_INT32:
	case ATOMX_TBTYPE_UINT32:
	case ATOMX_TBTYPE_INT16:
	case ATOMX_TBTYPE_UINT16:
	case ATOMX_TBTYPE_INT8:
	case ATOMX_TBTYPE_UINT8:
		{
			ATOM_ScriptVar v(event->newValue.getI());
			ATOM_SetAttribValue (_object, attribName, v);
			break;
		}
	case ATOMX_TBTYPE_BOOL:
		{
			ATOM_ScriptVar v(event->newValue.getB() ? 1 : 0);
			ATOM_SetAttribValue (_object, attribName, v);
			break;
		}
	case ATOMX_TBTYPE_FLOAT:
		{
			ATOM_ScriptVar v(event->newValue.getF());
			ATOM_SetAttribValue (_object, attribName, v);
			break;
		}
	case ATOMX_TBTYPE_STRING:
		{
			ATOM_ScriptVar v(ATOM_STRING(event->newValue.getS()));
			ATOM_SetAttribValue (_object, attribName, v);
			break;
		}
	case ATOMX_TBTYPE_VECTOR3F:
	case ATOMX_TBTYPE_COLOR3F:
	case ATOMX_TBTYPE_DIR3F:
		{
			ATOM_Vector3f v3(event->newValue.get3F()[0], event->newValue.get3F()[1], event->newValue.get3F()[2]);
			ATOM_ScriptVar v(v3);
			ATOM_SetAttribValue (_object, attribName, v);
			break;
		}
	case ATOMX_TBTYPE_VECTOR4F:
	case ATOMX_TBTYPE_COLOR4F:
	case ATOMX_TBTYPE_QUAT4F:
		{
			ATOM_Vector4f v4(event->newValue.get4F()[0], event->newValue.get4F()[1], event->newValue.get4F()[2], event->newValue.get4F()[3]);
			ATOM_ScriptVar v(v4);
			ATOM_SetAttribValue (_object, attribName, v);
			break;
		}
	}

	callParentHandler (event);
}

static char gRootDir[MAX_PATH];

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT msg, LPARAM lp, LPARAM pData)
{
	if (msg == BFFM_INITIALIZED && gRootDir[0])
	{
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)gRootDir);
	}
	return   0;
}

static bool chooseDir (HWND hWndParent, const char *title, char *initialDir)
{
	BROWSEINFO bi;
	char folderName[MAX_PATH];
	memset(&bi, 0, sizeof(bi));
	memset(folderName, 0, sizeof(folderName));
	bi.hwndOwner = hWndParent;
	bi.pszDisplayName = 0;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_STATUSTEXT|BIF_USENEWUI|BIF_RETURNONLYFSDIRS;
	bi.lpfn = &BrowseCallbackProc;

	strcpy (gRootDir, initialDir);
	LPITEMIDLIST itemId = SHBrowseForFolder (&bi);
	if (itemId)
	{
		SHGetPathFromIDList (itemId, folderName);
		GlobalFreePtr (itemId);
		strcpy (initialDir, folderName);
		return true;
	}
	return false;
}

void ATOMX_PropertyTweakBar::onCommand (ATOMX_TWCommandEvent *event)
{
	int id = event->id;

	if (id == BUTTON_ID_RETURN)
	{
		popObject ();
		return;
	}

	if (id >= _varDetails.size())
	{
		callParentHandler (event);
		return;
	}

	ATOM_Object *_object = getObject();
	if (!_object)
	{
		callParentHandler (event);
		return;
	}

	const char *attribName = ATOM_GetAttribName(_object->getClassName(), _varDetails[id].attribIndex);
	HWND hwndOwner = (HWND)ATOM_GetRenderDevice()->getWindow()->getWindowInfo()->handle;
	ATOM_STRING sResult;

	if (_varDetails[id].isObject)
	{
		switch (event->name[0])
		{
		case '#':
			//View
			pushObject (_varDetails[id].value.asObject ().get());
			return;
		case '@':
			//Clear
			ATOM_SetAttribValue (_object, attribName, ATOM_ScriptVar((ATOM_Object*)0));
			ATOMX_TweakBar::clear ();
			_varDetails.resize (0);
			setupPropertyVars (_objects.size() > 1);
			return;
		case '&':
			//New
			ATOM_STRING className = askClassName ();
			if (!className.empty())
			{
				ATOM_AUTOREF(ATOM_Object) obj = ATOM_CreateObject(className.c_str(), 0);
				if (obj)
				{
					ATOM_SetAttribValue (_object, attribName, obj);
					ATOMX_TweakBar::clear ();
					_varDetails.resize (0);
					setupPropertyVars (_objects.size() > 1);
				}
			}
			return;
		}
	}

	if (_varDetails[id].isFileName)
	{
		char filename[260];
		ATOM_ScriptVar var;
		ATOM_GetAttribValue (_object, attribName, var);
		strcpy (filename, var.getString().c_str());

		OPENFILENAMEA ofn;
		memset(&ofn, 0, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwndOwner;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = sizeof(filename);
		ofn.lpstrFilter = _varDetails[id].filenameFilter.empty() ? "All\0*.*\0" : _varDetails[id].filenameFilter.c_str();
		ofn.nFilterIndex = 0;
		ofn.lpstrFileTitle = 0;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = 0;
		ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;

		if (::GetOpenFileNameA (&ofn))
		{
			sResult = ofn.lpstrFile;
			setVarLabel (attribName, ofn.lpstrFile);
		}
	}
	else if (_varDetails[id].isVfsFileName)
	{
		const char *filter = _varDetails[id].filenameFilter.empty() ? "All(*.*)|*.*|" : _varDetails[id].filenameFilter.c_str();
		ATOM_FileDlg dlg(0, "/", NULL, NULL, filter, hwndOwner);
		if (dlg.doModal () == IDOK)
		{
			sResult = dlg.getSelectedFileName(0);
			setVarLabel (attribName, sResult.c_str());
		}
	}
	else if (_varDetails[id].isVfsDirName)
	{
		const char *filter = _varDetails[id].filenameFilter.empty() ? "All(*.*)|*.*|" : _varDetails[id].filenameFilter.c_str();
		ATOM_FileDlg dlg(ATOM_FileDlg::MODE_OPENDIR, "/", NULL, NULL, NULL, hwndOwner);
		if (dlg.doModal () == IDOK)
		{
			sResult = dlg.getSelectedFileName(0);
			setVarLabel (attribName, sResult.c_str());
		}
	}
	else if (_varDetails[id].isDirName)
	{
		char filename[MAX_PATH];
		filename[0] = '\0';
		if (chooseDir (hwndOwner, "ÇëÑ¡ÔñÄ¿Â¼", filename))
		{
			sResult = filename;
			setVarLabel (attribName, filename);
		}
	}

	if (!sResult.empty ())
	{
		ATOM_ScriptVar v;
		ATOM_GetAttribValue (_object, attribName, v);

		ATOMX_TWValueChangedEvent e;
		e.name = attribName;
		e.userdata = 0;
		e.id = event->id;
		e.oldValue.setS (ATOMX_TBTYPE_STRING, v.asString().c_str());
		e.newValue.setS (ATOMX_TBTYPE_STRING, sResult.c_str());
		e.bar = this;
		onValueChanged (&e);
	}
}

void ATOMX_PropertyTweakBar::setupPropertyVars (bool addReturnButton)
{
	ATOM_Object *object = getObject ();
	if (!object)
	{
		return;
	}

	if (addReturnButton)
	{
		addButton ("%%pop", BUTTON_ID_RETURN, "Return", 0);
	}

	for (int i = 0; i < ATOM_GetAttribCount (object->getClassName()); ++i)
	{
		const char *name = ATOM_GetAttribName (object->getClassName(), i);
		setupPropertyVar (name, i);
	}
}

static const char *findNextValidChar (const char *current)
{
	const char *strEnd = current + strlen(current);
	const char *p = current;

	while (p < strEnd)
	{
		if (*p == ' ' || *p == '\t')
		{
			++p;
			continue;
		}
		return p;
	}
	return 0;
}

static const char *findNextValidChar (const char *current, char ch)
{
	const char *strEnd = current + strlen(current);
	const char *p = current;

	while (p < strEnd)
	{
		if (*p != ch)
		{
			++p;
			continue;
		}
		return p;
	}
	return 0;
}

static const char *findPrevValidChar (const char *first, const char *current)
{
	const char *p = current - 1;

	while (p >= first)
	{
		if (*p == ' ' || *p == '\t')
		{
			--p;
			continue;
		}
		return p;
	}
	return 0;
}

static ATOM_STRING parseCommentKeyword (const char *comment, const char *keyword)
{
	const char *str = comment;
	const char *strEnd = comment + strlen(comment);
	unsigned nKeywordLen = strlen(keyword);

	for (;;)
	{
		const char *p = strstr (str, keyword);
		if (!p)
		{
			return "";
		}

		p += nKeywordLen;
		str = p;

		const char *nextChar = findNextValidChar (p);
		if (!nextChar || *nextChar != '=')
		{
			continue;
		}
		nextChar = findNextValidChar (nextChar + 1);
		if (*nextChar == '\'' || *nextChar == '"')
		{
			const char *s = findNextValidChar (nextChar + 1, *nextChar);
			if (!s)
			{
				continue;
			}
			const char *s2 = findNextValidChar (s + 1);
			if (s2 && *s2 != ';')
			{
				continue;
			}
			return ATOM_STRING(nextChar + 1, s - nextChar - 1);
		}
		else
		{
			const char *s = findNextValidChar (nextChar + 1, ';');
			if (s)
			{
				const char *prev = findPrevValidChar (nextChar, s);
				return ATOM_STRING(nextChar, prev + 1 - nextChar);
			}
			else
			{
				const char *prev = findPrevValidChar (nextChar, strEnd);
				return ATOM_STRING(nextChar, prev + 1 - nextChar);
			}
		}
	}
}

bool ATOMX_PropertyTweakBar::parsePropDetail (ATOMX_PropertyTweakBar *bar, ATOM_Object *object, const char *propName, const char *propComment, VarDetail *detail)
{
	detail->type = ATOMX_TBTYPE_UNKNOWN;
	detail->minValue = 0.f;
	detail->maxValue = 0.f;
	detail->isEnum = false;
	detail->hasMinMax = false;
	detail->isObject = false;
	detail->isVfsFileName = false;
	detail->isFileName = false;
	detail->isVfsDirName = false;
	detail->isDirName = false;
	detail->isReadonly = false;
	detail->hasStep = false;
	detail->hasPrecision = false;
	detail->attribIndex = 0;
	detail->group = parseCommentKeyword (propComment, "group");
	detail->desc = parseCommentKeyword (propComment, "desc");

	detail->isReadonly = ATOM_IsAttribReadonly (object->getClassName(), propName);
	int valType = ATOM_GetAttribType(object->getClassName(), propName);
	if (valType == TYPE_OBJECT)
	{
		detail->isObject = true;
		ATOM_GetAttribValue (object, propName, detail->value);
		return true;
	}

	ATOM_STRING typeStr = parseCommentKeyword (propComment, "type");
	ATOM_STRING minValue = parseCommentKeyword (propComment, "min");
	ATOM_STRING maxValue = parseCommentKeyword (propComment, "max");
	ATOM_STRING enums = parseCommentKeyword (propComment, "enum");
	ATOM_STRING step = parseCommentKeyword (propComment, "step");
	ATOM_STRING precision = parseCommentKeyword (propComment, "precision");

	switch (valType)
	{
	case TYPE_INT:
		detail->type = ATOMX_TBTYPE_INT32;
		break;
	case TYPE_FLOAT:
		detail->type = ATOMX_TBTYPE_FLOAT;
		break;
	case TYPE_STRING:
		{
			if (!stricmp(typeStr.c_str(), "vfilename"))
			{
				detail->isVfsFileName = true;
			}
			else if (!stricmp(typeStr.c_str(), "filename"))
			{
				detail->isFileName = true;
			}
			else if (!stricmp(typeStr.c_str(), "vdirname"))
			{
				detail->isVfsDirName = true;
			}
			else if (!stricmp(typeStr.c_str(), "dirname"))
			{
				detail->isDirName = true;
			}
			detail->type = ATOMX_TBTYPE_STRING;
			break;
		}
	case TYPE_VEC2:
		detail->type = ATOMX_TBTYPE_VECTOR2F;
		break;
	case TYPE_VEC3:
		detail->type = ATOMX_TBTYPE_VECTOR3F;
		break;
	case TYPE_VEC4:
		detail->type = ATOMX_TBTYPE_VECTOR4F;
		break;
	default:
		return false;
	}

	if (detail->isVfsFileName || detail->isFileName)
	{
		detail->filenameFilter = parseCommentKeyword (propComment, "filterStr");
	}

	const struct
	{
		const char *typeStr;
		ATOMX_TBType type;
	}
	typeTable[] = {
		{ "rgb", ATOMX_TBTYPE_COLOR3F },
		{ "rgba", ATOMX_TBTYPE_COLOR4F },
		{ "bool", ATOMX_TBTYPE_BOOL	},
		{ "quat", ATOMX_TBTYPE_QUAT4F },
		{ "dir", ATOMX_TBTYPE_DIR3F },
		{ "int32", ATOMX_TBTYPE_INT32 },
		{ "uint32", ATOMX_TBTYPE_UINT32 },
		{ "int16", ATOMX_TBTYPE_INT16 },
		{ "uint16", ATOMX_TBTYPE_UINT16 },
		{ "int8", ATOMX_TBTYPE_INT8 },
		{ "uint8", ATOMX_TBTYPE_UINT8 }
	};

	const unsigned numTableEntries = sizeof(typeTable)/sizeof(typeTable[0]);
	for (unsigned i = 0; i < numTableEntries; ++i)
	{
		if (!stricmp (typeTable[i].typeStr, typeStr.c_str()))
		{
			switch (typeTable[i].type)
			{
			case ATOMX_TBTYPE_INT32:
			case ATOMX_TBTYPE_UINT32:
			case ATOMX_TBTYPE_INT16:
			case ATOMX_TBTYPE_UINT16:
			case ATOMX_TBTYPE_INT8:
			case ATOMX_TBTYPE_UINT8:
			case ATOMX_TBTYPE_BOOL:
				if (valType == TYPE_INT)
				{
					detail->type = typeTable[i].type;
				}
				break;
			case ATOMX_TBTYPE_COLOR3F:
			case ATOMX_TBTYPE_DIR3F:
				if (valType == TYPE_VEC3)
				{
					detail->type = typeTable[i].type;
				}
				break;
			case ATOMX_TBTYPE_QUAT4F:
			case ATOMX_TBTYPE_COLOR4F:
				if (valType == TYPE_VEC4)
				{
					detail->type = typeTable[i].type;
				}
				break;
			default:
				return false;
			}
		}
	}

	switch (detail->type)
	{
	case ATOMX_TBTYPE_VECTOR2F:
		detail->memberNames.push_back (".x");
		detail->memberNames.push_back (".y");
		break;
	case ATOMX_TBTYPE_VECTOR3F:
	case ATOMX_TBTYPE_DIR3F:
		detail->memberNames.push_back (".x");
		detail->memberNames.push_back (".y");
		detail->memberNames.push_back (".z");
		break;
	case ATOMX_TBTYPE_VECTOR4F:
	case ATOMX_TBTYPE_QUAT4F:
		detail->memberNames.push_back (".x");
		detail->memberNames.push_back (".y");
		detail->memberNames.push_back (".z");
		detail->memberNames.push_back (".w");
		break;
	}

	if (!minValue.empty() && !maxValue.empty())
	{
		detail->hasMinMax = true;
		detail->minValue = atof (minValue.c_str());
		detail->maxValue = atof (maxValue.c_str());
	}

	if (!step.empty ())
	{
		float fStep = atof (step.c_str());
		if (fStep > 0.f)
		{
			detail->hasStep = true;
			detail->step = fStep;
		}
	}

	if (!precision.empty ())
	{
		int n = atoi (precision.c_str());
		if (n > 0)
		{
			detail->hasPrecision = true;
			detail->precision = n;
		}
	}

	if (!enums.empty () && TYPE_INT == valType)
	{
		detail->isEnum = true;

		ATOM_STRING enumInfo[2];
		unsigned currentIdx = 0;
		bool ignoreSpace = false;
		for (unsigned i = 0; i < enums.length(); ++i)
		{
			char ch = enums.c_str()[i];
			if (ch == ' ' || ch == '\t')
			{
				if (!ignoreSpace)
				{
					if (currentIdx == 1)
					{
						int value = atoi (enumInfo[1].c_str());
						detail->propEnumValue.addEnum (enumInfo[0].c_str(), value);
						enumInfo[0].clear();
						enumInfo[1].clear();
					}
					currentIdx = 1 - currentIdx;

					ignoreSpace = true;
				}
			}
			else
			{
				if (ignoreSpace)
				{
					ignoreSpace = false;
				}
				enumInfo[currentIdx].push_back (ch);
			}
		}
		if (currentIdx == 1)
		{
			int value = atoi (enumInfo[1].c_str());
			detail->propEnumValue.addEnum (enumInfo[0].c_str(), value);
		}
	}

	ATOM_GetAttribValue (object, propName, detail->value);
	if (!detail->isEnum)
	{
		switch (detail->type)
		{
		case ATOMX_TBTYPE_INT32:
		case ATOMX_TBTYPE_UINT32:
		case ATOMX_TBTYPE_INT16:
		case ATOMX_TBTYPE_UINT16:
		case ATOMX_TBTYPE_INT8:
		case ATOMX_TBTYPE_UINT8:
			detail->propValue.setI (detail->type, detail->value.getInteger());
			break;
		case ATOMX_TBTYPE_BOOL:
			detail->propValue.setB (detail->type, detail->value.getInteger() != 0);
			break;
		case ATOMX_TBTYPE_FLOAT:
			detail->propValue.setF (detail->type, detail->value.getFloat() != 0);
			break;
		case ATOMX_TBTYPE_STRING:
			detail->propValue.setS (detail->type, detail->value.getString().c_str());
			break;
		case ATOMX_TBTYPE_COLOR3F:
		case ATOMX_TBTYPE_DIR3F:
		case ATOMX_TBTYPE_VECTOR3F:
			detail->propValue.set3F (detail->type, detail->value.getVector3().x, detail->value.getVector3().y, detail->value.getVector3().z);
			break;
		case ATOMX_TBTYPE_COLOR4F:
		case ATOMX_TBTYPE_QUAT4F:
		case ATOMX_TBTYPE_VECTOR4F:
			detail->propValue.set4F (detail->type, detail->value.getVector4().x, detail->value.getVector4().y, detail->value.getVector4().z, detail->value.getVector4().w);
			break;
		default:
			return false;
		}
	}

	return true;
}

void ATOMX_PropertyTweakBar::setupPropertyVar (const char *prop, int index)
{
	ATOM_Object *_object = getObject ();
	if (!_object)
	{
		return;
	}

	const char *classname = _object->getClassName();
	int type = ATOM_GetAttribType (classname, prop);
	const char *comment = ATOM_GetAttribComment (classname, prop);

	_varDetails.resize (_varDetails.size() + 1);
	VarDetail &vd = _varDetails.back();
	parsePropDetail (this, _object, prop, comment, &vd);
	vd.attribIndex = index;
	int id = _varDetails.size() - 1;
	const char *group = vd.group.empty () ? 0 : vd.group.c_str();

	/*
	if (!vd.desc.empty())
	{
		ATOM_STRING name = prop;
		name += "_Desc";
		addText (name.c_str(), vd.desc.c_str(), group);
	}
	*/

	if (vd.isObject)
	{
		ATOM_STRING prefix[3] = { "#", "@", "&" };
		if (vd.value.asObject())
		{
			addButton ((prefix[0] + prop).c_str(), id, "View", prop);
			addButton ((prefix[1] + prop).c_str(), id, "Clear", prop);
		}
		else
		{
			addButton ((prefix[2] + prop).c_str(), id, "New..", prop);
		}
	}
	else if (vd.isFileName || vd.isVfsFileName || vd.isVfsDirName || vd.isDirName)
	{
		/*
		if (vd.desc.empty())
		{
			ATOM_STRING name = prop;
			name += "_Desc";
			addText (name.c_str(), name.c_str(), group);
		}
		*/
		const char *l = vd.propValue.getS();
		if (!l[0])
		{
			l = "(null)";
		}
		addButton (prop, id, l, group);
		if (!vd.desc.empty())
		{
			setVarLabel (prop, vd.desc.c_str());
		}
	}
	else if (vd.isEnum)
	{
		addEnum (prop, id, vd.value.getInteger(), vd.propEnumValue, vd.isReadonly, group);
		if (!vd.desc.empty ())
		{
			setVarLabel (prop, vd.desc.c_str());
		}
		setVarWriteToBarFunc (prop, &writeToBarFunc, this);
	}
	else
	{
		addVariable (prop, id, vd.propValue, vd.isReadonly, group);
		setVarWriteToBarFunc (prop, &writeToBarFunc, this);
		if (!vd.desc.empty ())
		{
			setVarLabel (prop, vd.desc.c_str());
		}

		if (!vd.memberNames.empty())
		{
			for (unsigned i = 0; i < vd.memberNames.size(); ++i)
			{
				ATOM_STRING s = prop;
				s += vd.memberNames[i];

				if (vd.hasStep)
				{
					setVarStep (s.c_str(), vd.step);
				}

				if (vd.hasMinMax)
				{
					setVarMinMax (s.c_str(), vd.minValue, vd.maxValue);
				}

				if (vd.hasPrecision)
				{
					setVarPrecision (s.c_str(), vd.precision);
				}
			}
		}
		else
		{
			if (vd.hasStep)
			{
				setVarStep (prop, vd.step);
			}

			if (vd.hasMinMax)
			{
				setVarMinMax (prop, vd.minValue, vd.maxValue);
			}

			if (vd.hasPrecision)
			{
				setVarPrecision (prop, vd.precision);
			}
		}
	}
}

void ATOMX_PropertyTweakBar::pushObject (ATOM_Object *object)
{
	_objects.push_back (object);

	ATOMX_TweakBar::clear ();
	_varDetails.resize (0);
	setupPropertyVars (_objects.size() > 1);
}

void ATOMX_PropertyTweakBar::popObject (void)
{
	ATOMX_TweakBar::clear ();
	_varDetails.resize (0);

	if (_objects.size () > 0)
	{
		_objects.pop_back ();
		setupPropertyVars (_objects.size() > 1);
	}
}

static INT_PTR CALLBACK DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lparam)
{
	static char *buffer = 0;
	switch (message)
	{
	case WM_INITDIALOG:
		{
			buffer = (char*)lparam;
			HWND hwndEdit = ::GetDlgItem (hDlg, IDC_CLASSNAME);
			::SetFocus (hwndEdit);
			return FALSE;
		}
	case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK)
			{
				HWND hwndEdit = ::GetDlgItem (hDlg, IDC_CLASSNAME);
				::GetWindowTextA (hwndEdit, buffer, 255);
				EndDialog (hDlg, IDOK);
			}
			else if (LOWORD(wParam) == IDCANCEL)
			{
				EndDialog (hDlg, IDCANCEL);
			}
			return TRUE;
		}
	case WM_CLOSE:
		{
			EndDialog (hDlg, IDCANCEL);
			return TRUE;
		}
	default:
		{
			return FALSE;
		}
	}
}

ATOM_STRING ATOMX_PropertyTweakBar::askClassName (void) const
{
	HMODULE hInstance = ::GetModuleHandleA ("ATOMX.DLL");
	char buffer[256] = "";

	if (IDOK == ::DialogBoxParamA (hInstance, MAKEINTRESOURCE(IDD_ASKCLASSNAME), ATOM_GetRenderDevice()->getWindow()->getWindowInfo()->handle, &DlgProc, (LPARAM)buffer))
	{
		return buffer;
	}

	return "";
}

