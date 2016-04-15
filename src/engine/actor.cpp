#include "StdAfx.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ATOM_ActorPart
///////////////////////////////////////////////////////////////////////////////////////////////////////

const int ATOM_ActorPart::invalid_Id = -1;
const int ATOM_ActorPart::any_Id = -1;

ATOM_ActorPart::ATOM_ActorPart (void)
{
	_actor = 0;
	_currentCandidate = invalid_Id;
	_userData = nullptr;
}

ATOM_ActorPart::~ATOM_ActorPart (void)
{
}

void ATOM_ActorPart::setActor (ATOM_Actor *actor)
{
	_actor = actor;
}

ATOM_Actor *ATOM_ActorPart::getActor (void) const
{
	return _actor;
}

void ATOM_ActorPart::setUserData (void *userData)
{
	_userData = userData;
}

void *ATOM_ActorPart::getUserData (void) const
{
	return _userData;
}

bool ATOM_ActorPart::setName (const char *name)
{
	if (name && name[0] && _name != name)
	{
		_name = name;

		if (_actor)
		{
			_actor->notifyPartChange();
		}
		return true;
	}
	return false;
}

const char *ATOM_ActorPart::getName (void) const
{
	return _name.c_str();
}

bool ATOM_ActorPart::setCurrentCandidate (CandidateId id)
{
	if (id == _currentCandidate)
	{
		return true;
	}

	if (onCandidateChange (_currentCandidate, id))
	{
		_currentCandidate = id;
		return true;
	}

	return false;
}

ATOM_ActorPart::CandidateId ATOM_ActorPart::generateId (void) const
{
	ATOM_ActorPart::CandidateId n = 1;

	for (;;)
	{
		bool found = true;
		for (unsigned i = 0; i < _candidates.size(); ++i)
		{
			if (_candidates[i].id == n)
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return n;
		}
		++n;
	}
}

int ATOM_ActorPart::getCandidateIndexById (CandidateId id) const
{
	for (int i = 0; i < _candidates.size(); ++i)
	{
		if (_candidates[i].id == id)
		{
			return i;
		}
	}
	return -1;
}

ATOM_ActorPart::CandidateId ATOM_ActorPart::addCandidate (const char *desc)
{
	CandidateId id = generateId ();

	_candidates.resize (_candidates.size() + 1);
	_candidates.back().desc = desc;
	_candidates.back().id = id;

	return id;
}

ATOM_ActorPart::CandidateId ATOM_ActorPart::addCandidate (CandidateId id, const char *desc)
{
	if (!desc || !desc[0] || id < 0 || getCandidateIndexById (id) >= 0)
	{
		return invalid_Id;
	}

	_candidates.resize (_candidates.size() + 1);
	_candidates.back().desc = desc;
	_candidates.back().id = id;

	return id;
}

bool ATOM_ActorPart::removeCandidate (ATOM_ActorPart::CandidateId id)
{
	if (id >= 0)
	{
		int idx = getCandidateIndexById (id);
		if (idx >= 0)
		{
			_candidates.erase (_candidates.begin() + idx);
			if (id == _currentCandidate)
			{
				setCurrentCandidate (invalid_Id);
			}
			return true;
		}
	}
	return false;
}

void ATOM_ActorPart::clearCandidates (void)
{
	setCurrentCandidate (invalid_Id);
	_candidates.clear ();
}

unsigned ATOM_ActorPart::getNumCandidates (void) const
{
	return _candidates.size();
}

ATOM_ActorPart::CandidateId ATOM_ActorPart::getCandidateId (unsigned index) const
{
	if (index < _candidates.size())
	{
		return _candidates[index].id;
	}
	return invalid_Id;
}

bool ATOM_ActorPart::setCandidateId (unsigned index, CandidateId id)
{
	if (id < 0)
	{
		return false;
	}

	if (index >= _candidates.size())
	{
		return false;
	}

	if (id == _candidates[index].id)
	{
		return true;
	}

	if (getCandidateIndexById (id) >= 0)
	{
		return false;
	}

	if (_currentCandidate == _candidates[index].id)
	{
		_currentCandidate = id;
	}

	_candidates[index].id = id;

	return true;
}

ATOM_ActorPart::CandidateId ATOM_ActorPart::getCurrentCandidate (void) const
{
	return _currentCandidate;
}

const char *ATOM_ActorPart::getCandidateDesc (ATOM_ActorPart::CandidateId id) const
{
	if (id >= 0)
	{
		int index = getCandidateIndexById (id);
		if (index >= 0)
		{
			return _candidates[index].desc.c_str();
		}
	}
	return 0;
}

void ATOM_ActorPart::setCandidateDesc (ATOM_ActorPart::CandidateId id, const char *desc)
{
	if (id >= 0)
	{
		int index = getCandidateIndexById (id);
		if (index >= 0)
		{
			if (_candidates[index].desc != desc)
			{
				_candidates[index].desc = desc;

				if (_currentCandidate == _candidates[index].id)
				{
					onCandidateChange (_currentCandidate, _currentCandidate);
				}
			}
		}
	}
}

//////////////////////////////////////
/*
	<ActorPart Type="ATOM_ActorComponentsPart" Name="head" Current="0">
		<Candidate Desc="/1.nm" Id="100" />
		<Candidate Desc="/2.nm" Id="120" />
		<Candidate Desc="/3.nm" Id="120" />
	</ComponentsPart>


	<ActorPart Type="ATOM_ActorColorPart" Name="head" Current="0">
		<Candidate Desc="155,100,255" Id="100" />
		<Candidate Desc="255,0,255" Id="120" />
		<Candidate Desc="0,0,100.nm" Id="120" />
	</ComponentsPart>
*/
//////////////////////////////////////
bool ATOM_ActorPart::loadFromXml (ATOM_TiXmlElement *xmlElement)
{
	clearCandidates ();

	int id = 0;
	xmlElement->QueryIntAttribute ("Current", &id);

	const char *name = xmlElement->Attribute ("Name");
	if (!name)
	{
		return false;
	}
	_name = name;

	ATOM_TiXmlElement *eCandidate = xmlElement->FirstChildElement ("Candidate");
	while (eCandidate)
	{
		const char *desc = eCandidate->Attribute ("Desc");
		int candidateId = 0;
		eCandidate->QueryIntAttribute ("Id", &candidateId);
		if (desc && desc[0] && candidateId >= 0)
		{
			addCandidate (candidateId, desc);
		}
		eCandidate = eCandidate->NextSiblingElement ("Candidate");
	}

	if (!getCandidateDesc (id))
	{
		setCurrentCandidate (invalid_Id);
	}
	else
	{
		setCurrentCandidate (id);
	}

	return true;
}

void ATOM_ActorPart::writeToXml (ATOM_TiXmlElement *xmlElement)
{
	if (_currentCandidate != invalid_Id)
	{
		xmlElement->SetAttribute ("Current", _currentCandidate);
	}

	xmlElement->SetAttribute ("Name", _name.c_str());
	xmlElement->SetAttribute ("Type", getClassName());

	for (unsigned i = 0; i < _candidates.size(); ++i)
	{
		ATOM_TiXmlElement eCandidate("Candidate");
		eCandidate.SetAttribute ("Desc", _candidates[i].desc.c_str());
		eCandidate.SetAttribute ("Id", _candidates[i].id);
		xmlElement->InsertEndChild (eCandidate);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ATOM_ActorModifierPart
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATOM_ActorModifierPart::loadFromXml (ATOM_TiXmlElement *xmlElement)
{
	if (!ATOM_ActorPart::loadFromXml (xmlElement))
	{
		return false;
	}

	_affectParts.resize (0);

	ATOM_TiXmlElement *parts = xmlElement->FirstChildElement ("AffectParts");
	if (parts)
	{
		ATOM_TiXmlElement *part = parts->FirstChildElement ("Part");
		while (part)
		{
			const char *name = part->Attribute ("Name");
			if (!name)
			{
				ATOM_LOGGER::error("%s(0x%08X) NAME attribute not found.\n", __FUNCTION__, xmlElement);
			}
			else
			{
				if (!addPart (name))
				{
					ATOM_LOGGER::error("%s(0x%08X) duplicate name <%s>.\n", __FUNCTION__, xmlElement, name);
				}
			}
			part = part->NextSiblingElement ("Part");
		}
	}

	return true;
}

void ATOM_ActorModifierPart::writeToXml (ATOM_TiXmlElement *xmlElement)
{
	ATOM_ActorPart::writeToXml (xmlElement);

	if (_affectParts.size() > 0)
	{
		ATOM_TiXmlElement parts("AffectParts");

		for (unsigned i = 0; i < _affectParts.size(); ++i)
		{
			ATOM_TiXmlElement part ("Part");
			part.SetAttribute ("Name", _affectParts[i].c_str());
			parts.InsertEndChild (part);
		}

		xmlElement->InsertEndChild (parts);
	}
}

void ATOM_ActorModifierPart::sync (void)
{
	_parts.resize (0);

	ATOM_ActorPartDefine &partDefine = getActor()->getPartDefine ();

	for (unsigned i = 0; i < partDefine.getNumParts(); ++i)
	{
		ATOM_ActorPart *part = partDefine.getPart (i);

		for (unsigned n = 0; n < getNumParts(); ++n)
		{
			if (!strcmp (part->getName(), getPart(n)))
			{
				_parts.push_back (part);
				break;
			}
		}
	}
}

bool ATOM_ActorModifierPart::addPart (const char *partName)
{
	if (!partName || !partName[0])
	{
		return false;
	}

	if (getPartIndex (partName) >= 0)
	{
		return false;
	}

	_affectParts.push_back (partName);
	getActor()->notifyPartChange ();

	return true;
}

void ATOM_ActorModifierPart::removePart (const char *partName)
{
	int index = getPartIndex (partName);
	if (index >= 0)
	{
		_affectParts.erase (_affectParts.begin() + index);
		getActor()->notifyPartChange ();
	}
}

void ATOM_ActorModifierPart::clearAllParts (void)
{
	_affectParts.clear ();
	getActor()->notifyPartChange ();
}

unsigned ATOM_ActorModifierPart::getNumParts (void) const
{
	return _affectParts.size();
}

const char *ATOM_ActorModifierPart::getPart (unsigned index) const
{
	if (index < _affectParts.size())
	{
		return _affectParts[index].c_str();
	}
	return 0;
}

int ATOM_ActorModifierPart::getPartIndex (const char *name) const
{
	if (!name || !name[0])
	{
		return -1;
	}

	for (int i = 0; i < _affectParts.size(); ++i)
	{
		if (_affectParts[i] == name)
		{
			return i;
		}
	}
	return -1;
}

void ATOM_ActorModifierPart::setMeshProperty (const char *property)
{
	_meshProperty = property ? property : "";
}

const char *ATOM_ActorModifierPart::getMeshProperty (void) const
{
	return _meshProperty.empty() ? 0 : _meshProperty.c_str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ATOM_ActorComponentsPart
///////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_ActorComponentsPart::~ATOM_ActorComponentsPart (void)
{
	ATOM_Actor *actor = getActor ();
	if (actor && _currentComponents.isValid ())
	{
		actor->getInternalGeode ()->removeComponents (_currentComponents);
	}
}

void ATOM_ActorComponentsPart::apply (void)
{
}

void ATOM_ActorComponentsPart::sync (void)
{
}

void ATOM_ActorComponentsPart::setColor (const ATOM_Vector4f &color, const char *property)
{
	/*
	if (_currentComponents.isValid ())
	{
#if 1
		getActor()->getInternalGeode ()->getAnimation()->enableColorOverriden (_currentComponents, true);
		getActor()->getInternalGeode ()->getAnimation()->setOverrideColor (_currentComponents, color);
#else
		ATOM_Geode *geode = getActor()->getInternalGeode ();
		for (unsigned i = 0; i < geode->getNumMeshes(); ++i)
		{
			ATOM_InstanceMesh *mesh = geode->getAnimation()->getMesh (i);
			if (mesh->getComponents() == _currentComponents)
			{
				if (!property || mesh->getMesh()->getProperty(property))
				{
					mesh->setDiffuseColor (color);
				}
			}
		}
#endif
	}
	*/
}

void ATOM_ActorComponentsPart::setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property)
{
	/*
	if (_currentComponents.isValid ())
	{
#if 1
		getActor()->getInternalGeode ()->getAnimation()->enableTextureOverriden (_currentComponents, true);
		getActor()->getInternalGeode ()->getAnimation()->setOverrideTexture(_currentComponents, albedo);
#else
		ATOM_Geode *geode = getActor()->getInternalGeode ();
		for (unsigned i = 0; i < geode->getNumMeshes(); ++i)
		{
			ATOM_InstanceMesh *mesh = geode->getAnimation()->getMesh (i);
			if (mesh->getComponents() == _currentComponents)
			{
				if (!property || mesh->getMesh()->getProperty(property))
				{
					mesh->setAlbedoMap (albedo);
				}
			}
		}
#endif
	}
	*/
}

ATOM_Components ATOM_ActorComponentsPart::getCurrentComponents (void) const
{
	return _currentComponents;
}

void ATOM_ActorComponentsPart::setCurrentComponents (ATOM_Components part)
{
	if (part != _currentComponents)
	{
		setCurrentCandidate (any_Id);

		_currentComponents = part;

		if (!_currentComponents.isValid ())
		{
			setCurrentCandidate (invalid_Id);
		}
	}
}

bool ATOM_ActorComponentsPart::onCandidateChange (ATOM_ActorPart::CandidateId oldId, ATOM_ActorPart::CandidateId newId)
{
	if (newId >= 0)
	{
		const char *desc = getCandidateDesc (newId);

		if (!desc)
		{
			return false;
		}

		//--- wangjian modified ---//
		// 异步加载
#if 0
		ATOM_Components comp (desc, false);
#else

		int loadPriority = getActor() ? getActor()->getLoadPriority() : ATOM_LoadPriority_IMMEDIATE;

		// 加载模型
		ATOM_Components comp (desc, loadPriority);

		// 是否开启资源异步加载模式
		bool bMT = ( ATOM_AsyncLoader::IsRun() );// && ( loadPriority != ATOM_LoadPriority_IMMEDIATE );

		if( comp.isValid() && bMT && ( !comp.getModel()->getAsyncLoader()->IsLoadAllFinished() ) )
		{
			// 加载优先级
			loadPriority = ( loadPriority == ATOM_LoadPriority_IMMEDIATE ) ? comp.getModel()->getAsyncLoader()->priority :
																			 loadPriority;
			// 添加资源请求
			ATOM_LOADRequest_Event * event_load( ATOM_NEW(	ATOM_LOADRequest_Event,
															desc,
															getActor()->getInternalGeode(),
															loadPriority,
															comp.getModel()->getAsyncLoader(),
															ATOM_LOADRequest_Event::FLAG_SHOW ) );
		}
#endif
		//-------------------------//

		if (!comp.isValid())
		{
			return false;
		}

		//---------------------------------------------------------------//
		// wangjian modified
#if 0
		getActor()->getInternalGeode()->addComponents (comp);
#else
		getActor()->getInternalGeode()->addComponents (comp,loadPriority);
#endif
		//---------------------------------------------------------------//

		if (_currentComponents.isValid ())
		{
			getActor()->getInternalGeode()->removeComponents (_currentComponents);
		}
		_currentComponents = comp;
	}
	else if (_currentComponents.isValid ())
	{
		getActor()->getInternalGeode()->removeComponents (_currentComponents);

		_currentComponents.reset();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ATOM_ActorColorPart
///////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_ActorColorPart::ATOM_ActorColorPart (void)
{
	_currentColor.set (1.f, 1.f, 1.f, 1.f);
}

ATOM_ActorColorPart::~ATOM_ActorColorPart (void)
{
}

void ATOM_ActorColorPart::apply (void)
{
	const char *prop = getMeshProperty();

	for (unsigned i = 0; i < _parts.size(); ++i)
	{
		_parts[i]->setColor (_currentColor, getMeshProperty());
	}
}

void ATOM_ActorColorPart::setColor (const ATOM_Vector4f &color, const char *property)
{
}

void ATOM_ActorColorPart::setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property)
{
}

ATOM_Vector4f ATOM_ActorColorPart::getColorById (ATOM_ActorPart::CandidateId id) const
{
	const char *desc = getCandidateDesc (id);

	if (desc)
	{
		int r = 255, g = 255, b = 255, a = 255;
		sscanf (desc, "%d,%d,%d,%d", &r, &g, &b, &a);
		return ATOM_Vector4f (r/255.f, g/255.f, b/255.f, a/255.f);
	}
	return ATOM_Vector4f (1.f, 1.f, 1.f, 1.f);
}

const ATOM_Vector4f & ATOM_ActorColorPart::getCurrentColor (void) const
{
	return _currentColor;
}

void ATOM_ActorColorPart::setCurrentColor (const ATOM_Vector4f &color)
{
	_currentColor = color;

	setCurrentCandidate (ATOM_ActorPart::any_Id);
}

bool ATOM_ActorColorPart::onCandidateChange (CandidateId oldId, CandidateId newId)
{
	if (newId >= 0)
	{
		const char *desc = getCandidateDesc (newId);

		int r = 255, g = 255, b = 255, a = 255;

		if (desc)
		{
			sscanf (desc, "%d,%d,%d,%d", &r, &g, &b, &a);
		}

		_currentColor.set (r/255.f, g/255.f, b/255.f, a/255.f);
	}
	else if (newId == ATOM_ActorPart::invalid_Id)
	{
		_currentColor.set (1.f, 1.f, 1.f, 1.f);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ATOM_ActorTexturePart
///////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_ActorTexturePart::ATOM_ActorTexturePart (void)
{
}

ATOM_ActorTexturePart::~ATOM_ActorTexturePart (void)
{
}

void ATOM_ActorTexturePart::apply (void)
{
	for (unsigned i = 0; i < _parts.size(); ++i)
	{
		_parts[i]->setTextures (_currentAlbedo.get(), _currentNormalMap.get(), getMeshProperty());
	}
}

void ATOM_ActorTexturePart::setColor (const ATOM_Vector4f &color, const char *property)
{
}

void ATOM_ActorTexturePart::setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property)
{
}

ATOM_STRING ATOM_ActorTexturePart::getAlbedoFileNameById (ATOM_ActorPart::CandidateId id) const
{
	ATOM_STRING filenameAlbedo;
	const char *desc = getCandidateDesc (id);
	if (desc)
	{
		const char *sep = strchr (desc, '|');
		if (!sep)
		{
			filenameAlbedo = desc;
		}
		else
		{
			filenameAlbedo = ATOM_STRING(desc, sep - desc);
		}
	}
	return filenameAlbedo;
}

ATOM_STRING ATOM_ActorTexturePart::getNormalMapFileNameById (ATOM_ActorPart::CandidateId id) const
{
	ATOM_STRING filenameNormalMap;
	const char *desc = getCandidateDesc (id);
	if (desc)
	{
		const char *sep = strchr (desc, '|');
		if (sep)
		{
			filenameNormalMap = sep + 1;
		}
	}
	return filenameNormalMap;
}

ATOM_Texture *ATOM_ActorTexturePart::getCurrentAlbedo (void) const
{
	return _currentAlbedo.get();
}

ATOM_Texture *ATOM_ActorTexturePart::getCurrentNormalMap (void) const
{
	return _currentNormalMap.get();
}

void ATOM_ActorTexturePart::setCurrentTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap)
{
	_currentAlbedo = albedo;
	_currentNormalMap = normalmap;

	setCurrentCandidate (ATOM_ActorPart::any_Id);
}

bool ATOM_ActorTexturePart::onCandidateChange (CandidateId oldId, CandidateId newId)
{
	if (newId >= 0)
	{
		const char *desc = getCandidateDesc (newId);

		ATOM_STRING filenameAlbedo;
		ATOM_STRING filenameNormalMap;

		if (desc)
		{
			const char *sep = strchr (desc, '|');

			if (!sep)
			{
				filenameAlbedo = desc;
			}
			else
			{
				filenameNormalMap = sep + 1;
				filenameAlbedo = ATOM_STRING(desc, sep - desc);
			}

			if (!filenameAlbedo.empty ())
			{
				_currentAlbedo = ATOM_CreateTextureResource (filenameAlbedo.c_str());
			}

			if (!filenameNormalMap.empty ())
			{
				_currentNormalMap = ATOM_CreateTextureResource (filenameNormalMap.c_str());
			}
		}
		else
		{
			_currentAlbedo = 0;
			_currentNormalMap = 0;
		}
	}
	else if (newId == ATOM_ActorPart::invalid_Id)
	{
		_currentAlbedo = 0;
		_currentNormalMap = 0;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ATOM_ActorTransformPart
///////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_ActorTransformPart::ATOM_ActorTransformPart (void)
{
	_matrix.makeIdentity ();
}

ATOM_ActorTransformPart::~ATOM_ActorTransformPart (void)
{
	if (getActor ())
	{
		getActor()->getInternalGeode()->setO2T (ATOM_Matrix4x4f::getIdentityMatrix ());
	}
}

void ATOM_ActorTransformPart::apply (void)
{
	getActor()->getInternalGeode()->setO2T (_matrix);
}

void ATOM_ActorTransformPart::sync (void)
{
}

void ATOM_ActorTransformPart::setColor (const ATOM_Vector4f &color, const char *property)
{
}

void ATOM_ActorTransformPart::setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property)
{
}

void ATOM_ActorTransformPart::setCurrentTransform (const ATOM_Matrix4x4f &matrix)
{
	_matrix = matrix;

	apply ();

	setCurrentCandidate (ATOM_ActorPart::any_Id);
}

const ATOM_Matrix4x4f &ATOM_ActorTransformPart::getCurrentTransform (void) const
{
	return _matrix;
}

bool ATOM_ActorTransformPart::onCandidateChange (CandidateId oldId, CandidateId newId)
{
	if (newId >= 0)
	{
		const char *desc = getCandidateDesc (newId);

		ATOM_Matrix4x4f matrix;
		if (!parseDesc (desc, matrix))
		{
			return false;
		}

		_matrix = matrix;

		apply ();
	}
	else if (newId == ATOM_ActorPart::invalid_Id)
	{
		_matrix.makeIdentity ();

		apply ();
	}

	return true;
}

bool ATOM_ActorTransformPart::parseDesc (const char *desc, ATOM_Matrix4x4f &matrix) const
{
	matrix.makeIdentity ();

	return 16 == sscanf (desc, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
		&matrix.m00, &matrix.m01, &matrix.m02, &matrix.m03,
		&matrix.m10, &matrix.m11, &matrix.m12, &matrix.m13,
		&matrix.m20, &matrix.m21, &matrix.m22, &matrix.m23,
		&matrix.m30, &matrix.m31, &matrix.m32, &matrix.m33);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ATOM_ActorBindingPart
///////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_ActorBindingPart::ATOM_ActorBindingPart (void)
{
	_bindMatrix.makeIdentity ();
}

ATOM_ActorBindingPart::~ATOM_ActorBindingPart (void)
{
}

void ATOM_ActorBindingPart::apply (void)
{
}

void ATOM_ActorBindingPart::sync (void)
{
	if (_currentBinding && _currentBinding->getParent() == getActor()->getInternalGeode())
	{
		getActor()->getInternalGeode()->detach (_currentBinding.get());
	}

	if (_currentBinding)
	{
		getActor()->getInternalGeode()->attach (_boneName.c_str(), _currentBinding.get());
	}
}

void ATOM_ActorBindingPart::setColor (const ATOM_Vector4f &color, const char *property)
{
}

void ATOM_ActorBindingPart::setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property)
{
}

void ATOM_ActorBindingPart::setBoneName (const char *boneName)
{
	if (_currentBinding)
	{
		getActor()->getInternalGeode()->detach (_currentBinding.get());
	}

	_boneName = boneName ? boneName : "";

	if (_currentBinding && !_boneName.empty ())
	{
		getActor()->getInternalGeode()->attach (_boneName.c_str(), _currentBinding.get());
	}
}

const char *ATOM_ActorBindingPart::getBoneName (void) const
{
	return _boneName.c_str();
}

void ATOM_ActorBindingPart::setCurrentNode (ATOM_Node *node)
{
	if (!node)
	{
		if (_currentBinding)
		{
			getActor()->getInternalGeode()->detach (_currentBinding.get());
			_currentBinding = 0;
		}
		setCurrentCandidate (ATOM_ActorPart::invalid_Id);
	}
	else
	{
		ATOM_AUTOREF(ATOM_Node) newNode = node;

		if (_currentBinding)
		{
			_currentBinding->clearChildren();
		}
		else
		{
			_currentBinding = ATOM_HARDREF(ATOM_Node)();
		}
		newNode->setO2T (_bindMatrix);
		_currentBinding->appendChild (newNode.get());

		setCurrentCandidate (ATOM_ActorPart::any_Id);
	}

}

ATOM_Node *ATOM_ActorBindingPart::getCurrentNode (void) const
{
	return (_currentBinding && _currentBinding->getNumChildren() == 1) ? _currentBinding->getChild(0) : 0;
}

void ATOM_ActorBindingPart::setCurrentBindMatrix (const ATOM_Matrix4x4f &matrix)
{
	_bindMatrix = matrix;

	if (_currentBinding && _currentBinding->getNumChildren() == 1)
	{
		_currentBinding->getChild(0)->setO2T (_bindMatrix);
	}

	setCurrentCandidate (_currentBinding ? ATOM_ActorPart::any_Id : ATOM_ActorPart::invalid_Id);
}

const ATOM_Matrix4x4f &ATOM_ActorBindingPart::getCurrentBindMatrix (void) const
{
	return _bindMatrix;
}

bool ATOM_ActorBindingPart::parseDesc (const char *desc, ATOM_STRING &filename, ATOM_STRING &boneName, ATOM_Matrix4x4f &matrix) const
{
	filename.clear ();
	boneName.clear ();
	matrix.makeIdentity ();

	if (!desc)
	{
		return false;
	}

	const char *sep = strchr (desc, '|');

	if (!sep)
	{
		return false;
	}
	else
	{
		filename = ATOM_STRING(desc, sep - desc);

		sep++;

		const char *sep2 = strchr (sep, '|');
		if (!sep2)
		{
			boneName = sep;
			matrix.makeIdentity ();
		}
		else
		{
			boneName = ATOM_STRING (sep, sep2 - sep);
			const char *strMatrix = sep2 + 1;
			sscanf (strMatrix, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f"
				, &matrix.m00, &matrix.m01, &matrix.m02, &matrix.m03
				, &matrix.m10, &matrix.m11, &matrix.m12, &matrix.m13
				, &matrix.m20, &matrix.m21, &matrix.m22, &matrix.m23
				, &matrix.m30, &matrix.m31, &matrix.m32, &matrix.m33);
		}

		return true;
	}
}

bool ATOM_ActorBindingPart::getBindingInfoById (CandidateId id, BindingInfo &info) const
{
	if (parseDesc (getCandidateDesc (id), info.nodeFileName, info.boneName, info.bindMatrix))
	{
		return true;
	}
	return false;
}

bool ATOM_ActorBindingPart::onCandidateChange (CandidateId oldId, CandidateId newId)
{
	if (newId >= 0)
	{
		const char *desc = getCandidateDesc (newId);

		ATOM_STRING nodeFilename;
		ATOM_STRING boneName;
		ATOM_Matrix4x4f o2t = ATOM_Matrix4x4f::getIdentityMatrix();

		if (!parseDesc (getCandidateDesc (newId), nodeFilename, boneName, o2t))
		{
			return false;
		}

		//--- wangjian modified ---//
		// 异步加载
		ATOM_AUTOREF(ATOM_Node) nodeAttach = ATOM_Node::loadNodeFromFile ( nodeFilename.c_str(), 
																		   getActor() ? getActor()->getLoadPriority() : ATOM_LoadPriority_IMMEDIATE );
		//-------------------------//
		if (!nodeAttach)
		{
			return false;
		}

		if (_currentBinding)
		{
			getActor()->getInternalGeode()->detach (_currentBinding.get());
			_currentBinding->clearChildren();
		}
		else
		{
			_currentBinding = ATOM_HARDREF(ATOM_Node)();
		}

		nodeAttach->setO2T (o2t);
		_currentBinding->appendChild (nodeAttach.get());
		_boneName = boneName;
		_bindMatrix = o2t;
		getActor()->getInternalGeode()->attach (_boneName.c_str(), _currentBinding.get());
	}
	else if (newId == ATOM_ActorPart::invalid_Id)
	{
		if (_currentBinding)
		{
			getActor()->getInternalGeode()->detach (_currentBinding.get());
			_currentBinding = 0;
		}
		_bindMatrix.makeIdentity ();
		_boneName.clear ();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ATOM_ActorPartDefine
///////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_ActorPartDefine::ATOM_ActorPartDefine (ATOM_Actor *actor)
{
	_actor = actor;
}

ATOM_ActorPartDefine::~ATOM_ActorPartDefine (void)
{
}

///////////////////////////////////
/*
	<ActorPartDefine>
		<ActorPart Type="ATOM_ActorComponentsPart" Name="head" Current="0">
			<Candidate Desc="/1.nm" Id="100" />
			<Candidate Desc="/2.nm" Id="120" />
			<Candidate Desc="/3.nm" Id="120" />
		</ActorPart>
		<ActorPart Type="Components" Name="body" Current="0">
			<Candidate Desc="/1.nm" Id="100" />
			<Candidate Desc="/2.nm" Id="120" />
			<Candidate Desc="/3.nm" Id="120" />
		</ActorPart>
	</ActorPartDefine>
*/
///////////////////////////////////
bool ATOM_ActorPartDefine::loadFromXml (ATOM_TiXmlElement *xmlElement)
{
	ATOM_TiXmlElement *ePart = xmlElement->FirstChildElement ("ActorPart");
	while (ePart)
	{
		const char *className = ePart->Attribute ("Type");
		ATOM_AUTOREF(ATOM_ActorPart) part = ATOM_CreateObject (className, 0);
		if (part)
		{
			part->setActor (_actor);
			if (part->loadFromXml (ePart))
			{
				addPart (part.get());
			}
		}
		ePart = ePart->NextSiblingElement ("ActorPart");
	}

	_actor->notifyPartChange ();

	return true;
}

void ATOM_ActorPartDefine::writeToXml (ATOM_TiXmlElement *xmlElement)
{
	for (unsigned i = 0; i < _parts.size(); ++i)
	{
		ATOM_TiXmlElement ePart("ActorPart");
		_parts[i]->writeToXml (&ePart);
		xmlElement->InsertEndChild (ePart);
	}
}

ATOM_ActorComponentsPart *ATOM_ActorPartDefine::newComponentsPart (const char *name)
{
	ATOM_AUTOREF(ATOM_ActorComponentsPart) part = ATOM_CreateObject (ATOM_ActorComponentsPart::_classname(), 0);

	if (part && part->setName (name) && addPart (part.get()))
	{
		_actor->notifyPartChange ();
		return part.get();
	}

	return 0;
}

bool ATOM_ActorPartDefine::addPart (ATOM_ActorPart *part)
{
	if (part && !getPartByName (part->getName()))
	{
		part->setActor (_actor);
		_parts.push_back (part);
		_actor->notifyPartChange ();
		return true;
	}
	return false;
}

void ATOM_ActorPartDefine::removePartByName (const char *name)
{
	if (name && name[0])
	{
		for (unsigned i = 0; i < _parts.size(); ++i)
		{
			if (!strcmp (_parts[i]->getName(), name))
			{
				_parts.erase (_parts.begin() + i);
				_actor->notifyPartChange ();
				return;
			}
		}
	}
}

void ATOM_ActorPartDefine::removeAllParts (void)
{
	_parts.clear ();
	_actor->notifyPartChange ();
}

void ATOM_ActorPartDefine::removePart (ATOM_ActorPart *part)
{
	for (unsigned i = 0; i < _parts.size(); ++i)
	{
		if (_parts[i] == part)
		{
			_parts.erase (_parts.begin() + i);
			_actor->notifyPartChange ();
			return;
		}
	}
}

ATOM_ActorPart *ATOM_ActorPartDefine::getPartByName (const char *name) const
{
	if (name && name[0])
	{
		for (unsigned i = 0; i < _parts.size(); ++i)
		{
			if (!strcmp (_parts[i]->getName(), name))
			{
				return _parts[i].get();
			}
		}
	}
	return 0;
}

unsigned ATOM_ActorPartDefine::getNumParts (void) const
{
	return _parts.size();
}

const char *ATOM_ActorPartDefine::getPartName (unsigned index) const
{
	if (index < _parts.size())
	{
		return _parts[index]->getName();
	}
	return 0;
}

ATOM_ActorPart *ATOM_ActorPartDefine::getPart (unsigned index) const
{
	return index < _parts.size() ? _parts[index].get() : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ATOM_Actor
///////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Actor)
	ATOM_ATTRIBUTES_BEGIN(ATOM_Actor)
		ATOM_ATTRIBUTE_PERSISTENT (ATOM_Actor, "动作", getAction, setAction, "", "group=ATOM_Actor;desc='动作'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Actor, ATOM_VisualNode)

ATOM_IMPLEMENT_NODE_FILE_TYPE(ATOM_Actor)

ATOM_Actor::ATOM_Actor (void)
{
	_partChange = false;
	_geode = ATOM_HARDREF(ATOM_Geode)();
	_geode->setPrivate (1);
	_geode->setShow (ATOM_Node::INHERIT);
	_geode->setPickable (ATOM_Node::INHERIT);
	_partDefine = ATOM_NEW(ATOM_ActorPartDefine, this);
}

ATOM_Actor::~ATOM_Actor (void)
{
	ATOM_DELETE(_partDefine);
}

void ATOM_Actor::accept (ATOM_Visitor &visitor)
{
	visitor.visit (*this);
}

bool ATOM_Actor::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	return _geode->rayIntersectionTest (camera, ray, len);
}

bool ATOM_Actor::supportMTLoading (void)
{
	return false;
}

const ATOM_BBox& ATOM_Actor::getBoundingbox(void) const
{
	_boundingBox = _geode->getBoundingbox();
	_boundingBox.transform (_geode->getO2T());
	return _boundingBox;
}

const ATOM_BBox& ATOM_Actor::getWorldBoundingbox (void) const
{
	//---------------------------------------------------------------------------------------------//
	// wangjian modifeid
	if( /*_load_priority == ATOM_LoadPriority_IMMEDIATE || */_geode->_load_flag == LOAD_ALLFINISHED )
		return _geode->getWorldBoundingbox ();
	else
		return ATOM_Node::getWorldBoundingbox ();
	//---------------------------------------------------------------------------------------------//
}

void ATOM_Actor::buildBoundingbox (void) const
{
}

void ATOM_Actor::notifyPartChange (void)
{
	_partChange = true;
}

void ATOM_Actor::saveToXML (ATOM_TiXmlDocument &doc) const
{
	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement eRoot("Root");

	ATOM_TiXmlElement ePartDefine ("ActorPartDefine");
	getPartDefine().writeToXml (&ePartDefine);
	eRoot.InsertEndChild (ePartDefine);

	if (!_properties.empty ())
	{
		ATOM_TiXmlElement eProp("Properties");
		writeProperties (&eProp);
		eRoot.InsertEndChild (eProp);
	}

	if (!_action.empty ())
	{
		ATOM_TiXmlElement eAction("Action");
		eAction.SetAttribute ("Name", _action.c_str());
		eRoot.InsertEndChild (eAction);
	}

	doc.InsertEndChild (eRoot);
}

bool ATOM_Actor::saveToFile (const char *filename) const
{
	char nativeFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, nativeFileName);
	ATOM_TiXmlDocument doc (nativeFileName);

	saveToXML (doc);

	return doc.SaveFile ();
}

//=======================================================
// wangjian modified 
//=======================================================
bool ATOM_Actor::onLoad(ATOM_RenderDevice *device)
{
	//--- wangjian added ---//
	_geode->setLoadPriority(getLoadPriority());
	_geode->_load_flag = _load_flag;
	//----------------------//

#if 0

	if (!getNodeFileName().empty ())
	{
		ATOM_AutoFile f(getNodeFileName().c_str(), ATOM_VFS::read|ATOM_VFS::text);
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
			ATOM_LOGGER::error ("%s(0x%08X) %s\n", __FUNCTION__, device, doc.ErrorDesc());
			return false;
		}

		ATOM_TiXmlElement *root = doc.RootElement ();
		if (!root)
		{
			return false;
		}

		ATOM_TiXmlElement *element = root->FirstChildElement ("ActorPartDefine");
		if (!element)
		{
			return false;
		}

		if (!_partDefine->loadFromXml (element))
		{
			return false;
		}

		ATOM_TiXmlElement *elementProp = root->FirstChildElement ("Properties");
		if (elementProp)
		{
			loadProperties (elementProp);
		}

		ATOM_TiXmlElement *elementAction = root->FirstChildElement ("Action");
		const char *actionName = 0;
		if (elementAction)
		{
			actionName = elementAction->Attribute ("Name");
		}

		if (actionName && _action.empty())
		{
			_action = actionName; 
		}
	}

	appendChild (_geode.get());

	//--- wangjian added ---//
	onLoadFinished();
	bool bMT = ( ATOM_AsyncLoader::IsRun() );// && ( _load_priority != ATOM_LoadPriority_IMMEDIATE );
	if( !bMT )
		_geode->onLoadFinished();
	//----------------------//

	if (!_action.empty())
	{
		this->doAction (_action.c_str(), ATOM_Geode::ACTIONFLAGS_UPSIDE|ATOM_Geode::ACTIONFLAGS_DOWNSIDE);
	}
	else
	{
		this->doAction (0, ATOM_Geode::ACTIONFLAGS_UPSIDE|ATOM_Geode::ACTIONFLAGS_DOWNSIDE);
	}

	return true;

#else

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	if ( !getNodeFileName().empty () )
	{
		// 是否开启了异步加载模式
		bool bMT = ( ATOM_AsyncLoader::IsRun() );// && (_load_priority != ATOM_LoadPriority_IMMEDIATE);

		//==============================================================
		// 如果直接加载
		//==============================================================
		if( !bMT )
		{
			bool createNew = false;
			_attribFile = ATOM_LookupOrCreateObject ( ATOM_NodeAttributeFile::_classname(), getNodeFileName().c_str(), &createNew );
			return loadSync();
		}
		//==============================================================
		// 如果异步加载 
		//==============================================================
		else
		{
			const char *fileName = getNodeFileName().c_str();

			//---------------------------------------------
			// 查找该节点属性文件对象
			//---------------------------------------------
			bool createNew = false;
			_attribFile = ATOM_LookupOrCreateObject (ATOM_NodeAttributeFile::_classname(), fileName, &createNew);

			//---------------------------------------------
			// 如果该节点属性文件对象还不存在 且 
			// 优先级为ATOM_LoadPriority_IMMEDIATE
			//---------------------------------------------
			if( createNew && _load_priority == ATOM_LoadPriority_IMMEDIATE )
			{
				// 直接加载
				return loadSync();
			}
			//---------------------------------------------
			// 如果该节点属性文件对象不存在 或 
			// 该模型资源尚未加载完成
			// 使用异步加载
			//---------------------------------------------
			else if( createNew || ( !_attribFile->getAsyncLoader()->IsLoadAllFinished() ) )
			{
				// 如果节点属性文件对象存在 且 优先级为ATOM_LoadPriority_IMMEDIATE 则 使用该节点属性文件对象的加载对象的优先级
				// 否则 使用此节点的优先级
				int loadPrioriy = ( false == createNew && _load_priority == ATOM_LoadPriority_IMMEDIATE ) ?	_attribFile->getAsyncLoader()->priority : 
																											_load_priority;

				// 添加资源请求
				ATOM_LOADRequest_Event* event_load( ATOM_NEW(	ATOM_LOADRequest_Event,
																fileName,
																this,
																loadPrioriy,
																_attribFile ? _attribFile->getAsyncLoader() : 0,
																ATOM_LOADRequest_Event::/*FLAG_SHOW*/FLAG_NOT_SHOW ) );

				if( ATOM_AsyncLoader::isEnableLog() )
				{
					ATOM_LOGGER::debug( "<%s> : submit load request for attribute file %s of Composition node [ Address : %x, priority %d ]\n", 
										__FUNCTION__,
										fileName,
										_attribFile ? (unsigned)(_attribFile->getAsyncLoader()) : 0,
										loadPrioriy );
				}

				// 如果是新创建的对象 异步加载之
				if( createNew )
				{
					return _attribFile->loadAsync(fileName,loadPrioriy);
				}
				// 否则 如果该对象尚未加载完成且是被放弃的对象 重新处理该对象
				else
				{
					if( _attribFile->getAsyncLoader()->abandoned )
						ATOM_AsyncLoader::ReProcessLoadObject( _attribFile->getAsyncLoader() );
				}

				return true;
			}
			//---------------------------------------------
			// 如果该属性文件对象存在 且 已经加载完成
			//---------------------------------------------
			else
			{
				_load_flag = LOAD_ATTRIBUTE_LOADED;

				ATOM_ASSERT(_attribFile.get());

				// 如果出错 设置加载完成 退出
				if( _attribFile->getAsyncLoader()->IsLoadFailed() )
				{
					_load_flag = LOAD_ALLFINISHED;
					return false;
				}

				ATOM_TiXmlElement * root = _attribFile->getDocument().RootElement();
				if (!root)
				{
					return false;
				}

				ATOM_TiXmlElement *element = root->FirstChildElement ("ActorPartDefine");
				if (!element)
				{
					return false;
				}

				if (!_partDefine->loadFromXml (element))
				{
					return false;
				}

				ATOM_TiXmlElement *elementProp = root->FirstChildElement ("Properties");
				if (elementProp)
				{
					loadProperties (elementProp);
				}

				ATOM_TiXmlElement *elementAction = root->FirstChildElement ("Action");
				const char *actionName = 0;
				if (elementAction)
				{
					actionName = elementAction->Attribute ("Name");
				}

				if (actionName && _action.empty())
				{
					_action = actionName; 
				}
			
				//// 添加子节点
				//appendChild (_geode.get());

				//if (!_action.empty())
				//{
				//	this->doAction (_action.c_str(), ATOM_Geode::ACTIONFLAGS_UPSIDE|ATOM_Geode::ACTIONFLAGS_DOWNSIDE);
				//}
				//else
				//{
				//	this->doAction (0, ATOM_Geode::ACTIONFLAGS_UPSIDE|ATOM_Geode::ACTIONFLAGS_DOWNSIDE);
				//}

				//// 设置加载完成
				//_load_flag = LOAD_ALLFINISHED;

				//return true;
			}
		}
	}

	appendChild (_geode.get());

	//--- wangjian added ---//
	bool bMT = ( ATOM_AsyncLoader::IsRun() );// && ( _load_priority != ATOM_LoadPriority_IMMEDIATE );
	if( !bMT )
		_geode->onLoadFinished();
	//----------------------//

	if (!_action.empty())
	{
		this->doAction (_action.c_str(), ATOM_Geode::ACTIONFLAGS_UPSIDE|ATOM_Geode::ACTIONFLAGS_DOWNSIDE);
	}
	else
	{
		this->doAction (0, ATOM_Geode::ACTIONFLAGS_UPSIDE|ATOM_Geode::ACTIONFLAGS_DOWNSIDE);
	}

	// 设置加载完成标记
	_load_flag = LOAD_ALLFINISHED;

#endif

	return true;

}

void ATOM_Actor::assign(ATOM_Node *other) const
{

}

void ATOM_Actor::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	syncParts ();
}

void ATOM_Actor::skipClipTest (bool b)
{
	_geode->skipClipTest (b);
}

bool ATOM_Actor::isSkipClipTest (void) const
{
	return _geode->isSkipClipTest ();
}

ATOM_ActorPartDefine & ATOM_Actor::getPartDefine (void) const
{
	return *_partDefine;
}

ATOM_ActorPart::CandidateId ATOM_Actor::setPart (const char *partName, ATOM_ActorPart::CandidateId id)
{
	ATOM_ActorPart *part = _partDefine->getPartByName(partName);

	if (part)
	{
		return part->setCurrentCandidate (id);
	}

	return ATOM_ActorPart::invalid_Id;
}

ATOM_ActorPart::CandidateId ATOM_Actor::getPart (const char *partName) const
{
	ATOM_ActorPart *part = _partDefine->getPartByName(partName);

	if (part)
	{
		return part->getCurrentCandidate ();
	}

	return ATOM_ActorPart::invalid_Id;
}

bool ATOM_Actor::doAction (const char *actionName, unsigned flags, unsigned loopCount, bool forceReset, unsigned fadeTime, float speed)
{
	return _geode->doAction (actionName,flags, loopCount, forceReset, fadeTime, speed);
}

void ATOM_Actor::setDefaultActionSpeed (unsigned flags, float speed)
{
	_geode->setDefaultActionSpeed (flags, speed);
}

void ATOM_Actor::setCurrentActionSpeed (unsigned flags, float speed)
{
	_geode->setCurrentActionSpeed (flags, speed);
}

ATOM_Geode *ATOM_Actor::getInternalGeode (void) const
{
	return _geode.get();
}

void ATOM_Actor::syncParts (void)
{
	if (_partChange)
	{
		/*
#if 1
		for (unsigned i = 0; i < _geode->getNumComponents(); ++i)
		{
			_geode->getAnimation()->enableColorOverriden(_geode->getComponents(i), false);
			_geode->getAnimation()->enableTextureOverriden(_geode->getComponents(i), false);
		}
#else
		for (unsigned i = 0; i < _geode->getNumMeshes(); ++i)
		{
			ATOM_InstanceMesh *mesh = _geode->getAnimation()->getMesh(i);
			mesh->setDiffuseColor (mesh->getMesh()->diffuseColor);
			mesh->setAlbedoMap (mesh->getMesh()->diffuseTexture.get());
		}
#endif
		*/
		for (unsigned i = 0; i < _partDefine->getNumParts(); ++i)
		{
			_partDefine->getPart(i)->sync ();
		}
		_partChange = false;
	}

	for (unsigned i = 0; i < _partDefine->getNumParts(); ++i)
	{
		_partDefine->getPart(i)->apply ();
	}
}

static const ATOM_Variant invalidVariant;

void ATOM_Actor::setProperty (const char *name, const ATOM_Variant &value)
{
	if (name)
	{
		_properties[name] = value;
	}
}

const ATOM_Variant &ATOM_Actor::getProperty (const char *name)
{
	ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::const_iterator it = _properties.find (name);

	return it == _properties.end () ? invalidVariant : it->second;
}

bool ATOM_Actor::loadProperties (ATOM_TiXmlElement *element)
{
	ATOM_TiXmlElement *eProperty = element->FirstChildElement ("Property");
	while (eProperty)
	{
		const char *name = eProperty->Attribute ("Name");
		if (name)
		{
			const char *type = eProperty->Attribute ("Type");
			if (type)
			{
				if (!stricmp (type, "int"))
				{
					int value = 0;

					if (eProperty->QueryIntAttribute ("Value", &value) == ATOM_TIXML_SUCCESS)
					{
						setProperty (name, value);
					}
				}
				else if (!stricmp (type, "float"))
				{
					float value = 0;

					if (eProperty->QueryFloatAttribute ("Value", &value) == ATOM_TIXML_SUCCESS)
					{
						setProperty (name, value);
					}
				}
				else if (!stricmp (type, "vector"))
				{
					const char *vec = eProperty->Attribute ("Value");
					ATOM_Vector4f v;

					if (vec && 4 == sscanf (vec, "%f,%f,%f,%f", &v.x, &v.y, &v.z, &v.w))
					{
						setProperty (name, v);
					}
				}
				else if (!stricmp (type, "string"))
				{
					const char *s = eProperty->Attribute ("Value");
					if (s)
					{
						setProperty (name, s);
					}
				}
			}
		}
		eProperty = eProperty->NextSiblingElement ("Property");
	}

	return true;
}

void ATOM_Actor::writeProperties (ATOM_TiXmlElement *element) const
{
	ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::const_iterator it = _properties.begin ();

	for (; it != _properties.end (); ++it)
	{
		ATOM_TiXmlElement eProperty("Property");

		switch (it->second.getType())
		{
		case ATOM_Variant::INT:
			{
				eProperty.SetAttribute ("Name", it->first.c_str());
				eProperty.SetAttribute ("Type", "int");
				eProperty.SetAttribute ("Value", it->second.getI());
				break;
			}
		case ATOM_Variant::FLOAT:
			{
				eProperty.SetAttribute ("Name", it->first.c_str());
				eProperty.SetAttribute ("Type", "float");
				eProperty.SetDoubleAttribute ("Value", it->second.getF());
				break;
			}
		case ATOM_Variant::STRING:
			{
				eProperty.SetAttribute ("Name", it->first.c_str());
				eProperty.SetAttribute ("Type", "string");
				eProperty.SetAttribute ("Value", it->second.getS());
				break;
			}
		case ATOM_Variant::VECTOR4:
			{
				char buffer[128];
				const float *v = it->second.getV ();
				sprintf (buffer, "%f,%f,%f,%f", v[0], v[1], v[2], v[3]);

				eProperty.SetAttribute ("Name", it->first.c_str());
				eProperty.SetAttribute ("Type", "vector");
				eProperty.SetAttribute ("Value", buffer);
				break;
			}
		default:
			continue;
			break;
		}

		element->InsertEndChild (eProperty);
	}
}

unsigned ATOM_Actor::getNumProperties (void) const
{
	return _properties.size();
}

const char *ATOM_Actor::getPropertyName (unsigned index) const
{
	if (index < _properties.size())
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::const_iterator it = _properties.begin ();
		std::advance (it, index);
		return it->first.c_str();
	}
	return 0;
}

const ATOM_Variant &ATOM_Actor::getPropertyValue (unsigned index) const
{
	if (index < _properties.size())
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::const_iterator it = _properties.begin ();
		std::advance (it, index);
		return it->second;
	}
	return invalidVariant;
}

void ATOM_Actor::removeAllProperties (void)
{
	_properties.clear ();
}

void ATOM_Actor::resetCurrentAction (unsigned flags)
{
	if (_geode)
	{
		_geode->resetActions (flags);
	}
}

void ATOM_Actor::setAction (const ATOM_STRING &actionName)
{
	_action = actionName;

	if (_geode)
	{
		if (_action.empty ())
		{
			_geode->stopAction ();
		}
		else
		{
			_geode->doAction (_action.c_str(), ATOM_Geode::ACTIONFLAGS_UPSIDE|ATOM_Geode::ACTIONFLAGS_DOWNSIDE);
		}
	}
}

const ATOM_STRING &ATOM_Actor::getAction (void) const
{
	return _action;
}

//=========================================================================
// wangjian added
void ATOM_Actor::onLoadFinished()
{
	ATOM_ASSERT(_attribFile.get());

	// 如果出错 设置加载完成 退出
	if( _attribFile->getAsyncLoader()->IsLoadFailed() )
	{
		_load_flag = LOAD_ALLFINISHED;
		return;
	}

	_load_flag = LOAD_ATTRIBUTE_LOADED;

	ATOM_TiXmlElement * root = _attribFile->getDocument().RootElement();
	if (!root)
	{
		ATOM_LOGGER::error("<%s> : missing XML root element\n",__FUNCTION__);
		return;
	}

	ATOM_TiXmlElement *element = root->FirstChildElement ("ActorPartDefine");
	if (!element)
	{
		ATOM_LOGGER::error("<%s> : missing XML ActorPartDefine element\n",__FUNCTION__);
		return;
	}

	if (!_partDefine->loadFromXml (element))
	{
		return;
	}

	ATOM_TiXmlElement *elementProp = root->FirstChildElement ("Properties");
	if (elementProp)
	{
		loadProperties (elementProp);
	}

	ATOM_TiXmlElement *elementAction = root->FirstChildElement ("Action");
	const char *actionName = 0;
	if (elementAction)
	{
		actionName = elementAction->Attribute ("Name");
	}

	if (actionName && _action.empty())
	{
		_action = actionName; 
	}

	appendChild (_geode.get());

	bool bMT = ( ATOM_AsyncLoader::IsRun() );// && ( _load_priority != ATOM_LoadPriority_IMMEDIATE );
	if( !bMT )
		_geode->onLoadFinished();

	if (!_action.empty())
	{
		this->doAction (_action.c_str(), ATOM_Geode::ACTIONFLAGS_UPSIDE|ATOM_Geode::ACTIONFLAGS_DOWNSIDE);
	}
	else
	{
		this->doAction (0, ATOM_Geode::ACTIONFLAGS_UPSIDE|ATOM_Geode::ACTIONFLAGS_DOWNSIDE);
	}

	// 设置加载完成标记
	_load_flag = LOAD_ALLFINISHED;

}
// 直接加载接口
bool ATOM_Actor::loadSync()
{
	// 如果属性文件不存在 退出
	if( !_attribFile )
		return false;

	// 如果属性文件加载失败 退出
	if( _attribFile->getAsyncLoader()->IsLoadFailed() )
		return false;

	// 如果属性文件还未加载完成
	if( !_attribFile->getAsyncLoader()->IsLoadAllFinished() )
	{
		// 直接读取属性文件
		if( false == _attribFile->loadSync(getNodeFileName().c_str()) )
			return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ATOM_TiXmlElement *root = _attribFile->getDocument().RootElement ();
	if (!root)
	{
		return false;
	}

	ATOM_TiXmlElement *element = root->FirstChildElement ("ActorPartDefine");
	if (!element)
	{
		return false;
	}

	if (!_partDefine->loadFromXml (element))
	{
		return false;
	}

	ATOM_TiXmlElement *elementProp = root->FirstChildElement ("Properties");
	if (elementProp)
	{
		loadProperties (elementProp);
	}

	ATOM_TiXmlElement *elementAction = root->FirstChildElement ("Action");
	const char *actionName = 0;
	if (elementAction)
	{
		actionName = elementAction->Attribute ("Name");
	}

	if (actionName && _action.empty())
	{
		_action = actionName; 
	}

	appendChild (_geode.get());

	bool bMT = ( ATOM_AsyncLoader::IsRun() );// && ( _load_priority != ATOM_LoadPriority_IMMEDIATE );
	if( !bMT )
		_geode->onLoadFinished();

	if (!_action.empty())
	{
		this->doAction (_action.c_str(), ATOM_Geode::ACTIONFLAGS_UPSIDE|ATOM_Geode::ACTIONFLAGS_DOWNSIDE);
	}
	else
	{
		this->doAction (0, ATOM_Geode::ACTIONFLAGS_UPSIDE|ATOM_Geode::ACTIONFLAGS_DOWNSIDE);
	}

	// 设置加载完成标记
	_load_flag = LOAD_ALLFINISHED;

	return true;
}
//=========================================================================

