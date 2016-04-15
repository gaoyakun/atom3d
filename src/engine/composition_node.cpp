#include "StdAfx.h"
#include "composition_node.h"

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_CompositionNode)
	ATOM_ATTRIBUTES_BEGIN(ATOM_CompositionNode)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_CompositionNode, "CompositionFileName",	getCompositionFileName, setCompositionFileName, "", "group=ATOM_CompositionNode;type=vfilename;desc='特效文件名'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_CompositionNode, "Duration", getTimeLineDuration, setTimeLineDuration, 3000, "group=ATOM_CompositionNode")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_CompositionNode, "Speed", getSpeed, setSpeed, 1.f, "group=ATOM_CompositionNode")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_CompositionNode, "Playing", att_getPlaying, att_setPlaying, 0, "group=ATOM_CompositionNode;type=bool")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_CompositionNode, ATOM_VisualNode)

ATOM_IMPLEMENT_NODE_FILE_TYPE(ATOM_CompositionNode)

ATOM_CompositionNode::ATOM_CompositionNode (void)
{
	_actorParent = ATOM_HARDREF(ATOM_Node)();
	_actorParent->setPrivate (1); 
	_actorParent->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());
	_actorParent->setPickable (ATOM_Node::INHERIT);
	//appendChild (_actorParent.get());

	_timeDuration = 1;
	_currentPosition = 0;
	_playTick = 0;
	_playing = false;
	_speed = 1.f;
	_attribLoaded = false;
	_autoRemove = false;
	_loopCount = 0;
	_desiredLoopCount = 0;
	_groupDirty = false;
	_loadRef = false;

	//--- wangjian added ---//
	_updateCache = 0;
	//----------------------//
}

ATOM_CompositionNode::~ATOM_CompositionNode (void)
{
}

void ATOM_CompositionNode::buildBoundingbox (void) const
{
	int numChildren = _actorParent->getNumChildren();
	if (numChildren == 0)
	{
		_boundingBox.setMin(0.f);
		_boundingBox.setMax(0.f);
	}
	else
	{
		_boundingBox.beginExtend();
		for (int i = 0; i < numChildren; ++i)
		{
			ATOM_Node *childNode = _actorParent->getChild(i);

			ATOM_BBox bbox;
			childNode->getCombinedBoundingBox(bbox);
			bbox.transform (childNode->getO2T());

			_boundingBox.extend (bbox.getMin());
			_boundingBox.extend (bbox.getMax());
		}
	}
}

void ATOM_CompositionNode::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	//const ATOM_Matrix4x4f &matWorld = getWorldMatrix();
	unsigned flags = visitor->getFlags();
	visitor->setFlags (flags|ATOM_CullVisitor::CULL_NO_CLIP_TEST);
	//getActorParentNode()->setO2T (matWorld);
	cullGroups (visitor);
	visitor->setFlags (flags);
}

void ATOM_CompositionNode::accept (ATOM_Visitor &visitor)
{
	visitor.visit (*this);
}

bool ATOM_CompositionNode::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
#if 1
	float d;
	if (ray.intersectionTestEx (getBoundingbox(), d) && d >= 0.f)
	{
		if (len)
		{
			*len = d;
		}
		return true;
	}
	return false;
#else
	ATOM_PickVisitor v;
	v.setCamera (camera);
	v.setRay (ray);
	v.setFlags (0);
	v.traverse (*_actorParent);

	if (v.getNumPicked ())
	{
		v.sortResults ();

		if (len)
		{
			*len = v.getPickResult (0).distance;
		}

		return true;
	}

	return false;
#endif
}

static void _gatherActorNodes (ATOM_CompositionActor *actor, ATOM_VECTOR<ATOM_CompositionActor*> &actorVector)
{
	actorVector.push_back (actor);
	for (int i = 0; i < actor->getNumChildActors(); ++i)
	{
		_gatherActorNodes (actor->getChildActor(i), actorVector);
	}
}

struct SortPred
{
	bool operator () (ATOM_CompositionActor *a1, ATOM_CompositionActor *a2)
	{
		ATOM_STRING s1 = a1->getName();
		ATOM_STRING s2 = a2->getName();
		return s1 < s2;
	}
};

void ATOM_CompositionNode::updateGroups (void)
{
	_groupNodes.resize (0);
	_groupNumbers.resize (0);

	ATOM_VECTOR<ATOM_CompositionActor*> actorVector;
	for (int i = 0; i < getNumTopActors(); ++i)
	{
		_gatherActorNodes (getTopActor (i), actorVector);
	}

	if (!actorVector.empty ())
	{
		std::sort (actorVector.begin(), actorVector.end(), SortPred());
	}

	ATOM_STRING lastName("!不要起这个名字!");
	for (int i = 0; i < actorVector.size(); ++i)
	{
		ATOM_CompositionActor *actor = actorVector[i];
		_groupNodes.push_back (actor->getNode());

		const char *name = actor->getName ();
		const char *p = strrchr (name, '@');
		ATOM_STRING name_prefix = p ? ATOM_STRING(name, p-name) : name;
		if (lastName != name_prefix)
		{
			_groupNumbers.push_back (1);
			lastName = name_prefix;
		}
		else
		{
			_groupNumbers.back()++;
		}
	}
}

void ATOM_CompositionNode::updateR (ATOM_CompositionActor *actor, ATOM_Camera *camera)
{
	actor->update (_currentPosition);

	for (unsigned i = 0; i < actor->getNumChildActors(); ++i)
	{
		updateR (actor->getChildActor(i), camera);
	}

	//--- wangjian modified ---//
	if( _updateCache == 3 )
	{
		_updateCache = 0;
		invalidateBoundingbox ();
	}
	++_updateCache;
	//-------------------------//
}

unsigned ATOM_CompositionNode::getStartTick (void) const
{
	return _startTick;
}

void ATOM_CompositionNode::update (ATOM_Camera *camera)
{
	//--- wangjian added ---//
	// 异步加载：如果没有加载完成
	if( ! checkAsyncLoadFlag() )
		return;
	// 如果加载属性文件失败 
	if( _attribFile && _attribFile->getAsyncLoader()->IsLoadFailed() )
		return;
	//----------------------//

	if (_groupDirty)
	{
		_groupDirty = false;
		updateGroups ();
	}

	getActorParentNode()->setO2T (getWorldMatrix());

	if (_playing)
	{
#if 1
		unsigned currentTick = ATOM_APP->getFrameStamp().currentTick;
		_currentPosition = currentTick - _playTick;
		if (_currentPosition > _timeDuration)
		{
			++_loopCount;
			if (_desiredLoopCount && _loopCount >= _desiredLoopCount)
			{
				stop ();
				if (_autoRemove && _parent)
				{
					_autoRemove = false;
					_parent->removeChild (this);
				}
				return;
			}

			unsigned t = _timeDuration * (_currentPosition / _timeDuration);
			_playTick += t;
			_currentPosition -= t;

			for (unsigned i = 0; i < _actors.size(); ++i)
			{
				_actors[i]->reset ();
			}		
		}
#else
		_currentPosition += ATOM_APP->getFrameStamp().elapsedTick;

		if (_currentPosition > _timeDuration)
		{
			_currentPosition %= _timeDuration;
		}
#endif
	}

	for (unsigned i = 0; i < _actors.size(); ++i)
	{
		updateR (_actors[i].get(), camera);
	}
}

bool ATOM_CompositionNode::loadAttribute(const ATOM_TiXmlElement *xmlelement)
{
	if (!_attribLoaded)
	{
		_attribLoaded = true;
		return ATOM_VisualNode::loadAttribute (xmlelement);
	}
	return true;
}

bool ATOM_CompositionNode::writeAttribute(ATOM_TiXmlElement *xmlelement)
{
	return ATOM_VisualNode::writeAttribute (xmlelement);
}

bool ATOM_CompositionNode::onLoad(ATOM_RenderDevice *device)
{
	ATOM_STRING s = _compositionFileName;
	if (s.empty ())
	{
		s = _filenameToBeLoad;
	}

	if (s.empty ())
	{
		return true;
	}

#if 0

	ATOM_AutoFile f(s.c_str(), ATOM_VFS::read|ATOM_VFS::text);
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

	if (strcmp (root->Value(), "Composition"))
	{
		return false;
	}

	//--- wangjian modified ---//
	// 异步加载
	// 先保存加载优先级
	// 因为loadAttribute会改变_load_priority的值
	int pri_saved = _load_priority;

	if (!loadAttribute (root))
	{
		return false;
	}
	
	// 恢复加载优先级
	_load_priority = pri_saved;
	//----------------------//

	_compositionFileName = s;
	_filenameToBeLoad = s;

	if (loadFromXML (device, root))
	{
		updateGroups();
		_groupDirty = false;
		return true;
	}

	return false;

#else

	// 是否开启异步加载模式
	bool bMT = ATOM_AsyncLoader::IsRun();// && _load_priority != ATOM_LoadPriority_IMMEDIATE;

	//==============================================================
	// 如果直接加载
	//==============================================================
	if( !bMT )
	{
		bool createNew = false;
		_attribFile = ATOM_LookupOrCreateObject (ATOM_NodeAttributeFile::_classname(), s.c_str(), &createNew);
		return loadSync(s);
	}
	//==============================================================
	// 如果异步加载 
	//==============================================================
	else
	{
		//---------------------------------------------
		// 查找该节点属性文件对象
		//---------------------------------------------
		bool createNew = false;
		_attribFile = ATOM_LookupOrCreateObject (ATOM_NodeAttributeFile::_classname(), s.c_str(), &createNew);

		//---------------------------------------------
		// 如果该节点属性文件对象还不存在 且 
		// 优先级为ATOM_LoadPriority_IMMEDIATE
		//---------------------------------------------
		if( createNew && _load_priority == ATOM_LoadPriority_IMMEDIATE )
		{
			// 直接加载
			return loadSync(s);
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
															s.c_str(),
															this, 
															loadPrioriy,
															_attribFile ? _attribFile->getAsyncLoader() : 0,
															ATOM_LOADRequest_Event::/*FLAG_SHOW*/FLAG_NOT_SHOW ) );

			if( ATOM_AsyncLoader::isEnableLog() )
			{
				ATOM_LOGGER::debug( "<%s>: submit load request for attribute file %s of Composition node [ Address : %x, priority %d ]\n", 
									__FUNCTION__,
									s.c_str(),
									_attribFile ? (unsigned)(_attribFile->getAsyncLoader()) : 0,
									loadPrioriy );
			}

			// 如果是新创建的对象 异步加载之
			if( createNew )
			{
				return _attribFile->loadAsync( s.c_str(),loadPrioriy );
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

			const ATOM_TiXmlElement * root = _attribFile->getDocument().RootElement();
			if (!root)
			{
				return false;
			}
			if ( strcmp (root->Value(), "Composition") )
			{
				return false;
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			// 如果当前是否有变换，保存当前的变换[因为loadAttribute方法可能会重设变换]
			bool bHasTransform = false;
			ATOM_Transformf temp_transform;
			if( !_transform.getO2T().almostEqual( ATOM_Matrix4x4f::getIdentityMatrix() ) )
			{
				temp_transform = _transform;
				bHasTransform = true;
			}
			bool bPickable = getPickable2();
			int pri_saved = _load_priority;	// 先保存加载优先级 因为loadAttribute会改变_load_priority的值
			//////////////////////////////////////////////////////////////////////////////////////////////

			if (!loadAttribute (root))
			{
				return false;
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			_load_priority = pri_saved;		// 恢复加载优先级
			setPickable(bPickable);
			// 如果有变换，则设置回原来的变换
			if( bHasTransform )
			{
				setO2T(temp_transform.getO2T());
			}
			//////////////////////////////////////////////////////////////////////////////////////////////

			_compositionFileName	= s;
			_filenameToBeLoad		= s;

			// 加载XML
			if (loadFromXML (device, root))
			{
				updateGroups();
				_groupDirty = false;
				return true;
			}

			return false;
		}
	}

	return true;

#endif
}

bool ATOM_CompositionNode::isActorNode (ATOM_Node *node) const
{
	return node->getParent() == _actorParent.get();
}

void ATOM_CompositionNode::setTimeLineDuration (unsigned duration)
{
	_timeDuration = duration;
	if (_timeDuration <= 0)
	{
		_timeDuration = 1;
	}

	if (_currentPosition > _timeDuration)
	{
		_currentPosition = _timeDuration;
	}
}

unsigned ATOM_CompositionNode::getTimeLineDuration (void) const
{
	return _timeDuration;
}

bool ATOM_CompositionNode::seekTimeLine (unsigned position)
{
	_currentPosition = ATOM_min2 (position, _timeDuration);

	return true;
}

unsigned ATOM_CompositionNode::getTimeLinePosition (void) const
{
	return _currentPosition;
}

void ATOM_CompositionNode::reset (void)
{
	seekTimeLine (0);
	_playTick = 0;
	_startTick = 0;

	for (unsigned i = 0; i < _actors.size(); ++i)
	{
		_actors[i]->reset ();
	}
}

void ATOM_CompositionNode::play (unsigned loopCount, bool autoRemove)
{
	if (!_playing)
	{
		reset ();

		_playTick = ATOM_APP->getFrameStamp().currentTick;
		_startTick = _playTick;
		_loopCount = 0;
		_desiredLoopCount = loopCount;
		_autoRemove = autoRemove;
		_playing = true;
	}
}

void ATOM_CompositionNode::stop (void)
{
	_playing = false;
}

bool ATOM_CompositionNode::isPlaying (void) const
{
	return _playing;
}

void ATOM_CompositionNode::setSpeed (float speed)
{
	_speed = speed;
}

float ATOM_CompositionNode::getSpeed (void) const
{
	return _speed;
}

void ATOM_CompositionNode::addActor (ATOM_CompositionActor *actor)
{
	if (actor && !isActorNode(actor->getNode ()))
	{
		_actors.push_back (actor);
		_actorParent->appendChild (actor->getNode ());
		actor->setCompositionNode (this);
		_groupDirty = true;
	}
}

unsigned ATOM_CompositionNode::getNumTopActors (void) const
{
	return _actors.size();
}

ATOM_CompositionActor *ATOM_CompositionNode::getTopActor (unsigned index) const
{
	return _actors[index].get();
}

ATOM_CompositionActor *ATOM_CompositionNode::getActorByName (const char *name) const
{
	for (int i = 0; i < _actors.size(); ++i)
	{
		ATOM_CompositionActor *actor = _actors[i]->getActorByName (name);
		if (actor)
		{
			return actor;
		}
	}
	return 0;
}

void ATOM_CompositionNode::removeActor (ATOM_CompositionActor *actor)
{
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_CompositionActor)>::iterator it = std::find(_actors.begin(), _actors.end(), actor);
	if (it != _actors.end ())
	{
		ATOM_ASSERT(actor->getNode()->getParent() == _actorParent.get());
		actor->setCompositionNode (nullptr);
		_actorParent->removeChild (actor->getNode());
		_actors.erase (it);
		_groupDirty = true;
	}
}

void ATOM_CompositionNode::invalidateGroup (void)
{
	_groupDirty = true;
}

void ATOM_CompositionNode::cullGroups (ATOM_CullVisitor *v)
{
	float d = v->getDistanceOverrideValue ();
	int nodeIndex = 0;
	for (int i = 0; i < _groupNumbers.size(); ++i)
	{
		v->setDistanceOverrideValue ((_groupNodes[nodeIndex]->getWorldMatrix().getRow3(3) - v->getCamera()->getViewMatrix().getRow3(3)).getSquaredLength());
		for (int j = 0; j < _groupNumbers[i]; ++j)
		{
			_groupNodes[nodeIndex++]->accept (*v);
		}
	}
	v->setDistanceOverrideValue(d);
}

ATOM_Node *ATOM_CompositionNode::getActorParentNode (void) const
{
	return _actorParent.get();
}

bool ATOM_CompositionNode::loadFromXML (ATOM_RenderDevice *device, const ATOM_TiXmlElement *xml)
{
	//--- wangjian added ---//
	// 异步加载
	//bool bMT =	( ATOM_AsyncLoader::IsRun() ) && ( _load_priority != ATOM_LoadPriority_IMMEDIATE );

	for (const ATOM_TiXmlElement *eActor = xml->FirstChildElement ("Actor"); eActor; eActor = eActor->NextSiblingElement ("Actor"))
	{
		//--- wangjian modified ---//
		// 异步加载：添加异步加载优先级
		ATOM_AUTOPTR(ATOM_CompositionActor) actor = ATOM_LoadActorFromXML ( device, eActor, _loadRef, _load_priority );
		//-------------------------//

		if (actor)
		{
			addActor (actor.get());
		}
	}

	//--- wangjian added ---//
	if( !ATOM_AsyncLoader::IsRun()/* || _load_priority == ATOM_LoadPriority_IMMEDIATE*/ )
		_load_flag = LOAD_ALLFINISHED;
	//----------------------//

	return true;
}

bool ATOM_CompositionNode::saveToXML (ATOM_TiXmlElement *xml)
{
	xml->SetValue ("Composition");

	bool bHasPredefineBBox = false;
	ATOM_BBox _temp_bbox;
	_temp_bbox.beginExtend();

	for (unsigned i = 0; i < _actors.size(); ++i)
	{
		ATOM_TiXmlElement e("Actor");
		ATOM_SaveActorToXML (_actors[i].get(), &e);
		xml->InsertEndChild (e);

		//------- wangjian added -------//
		ATOM_CompositionActor * actor = _actors[i].get();
		if( !actor->getNode() )
			continue;

		if( stricmp( actor->getNode()->getClassName(), "ATOM_ShapeNode" ) == 0 )
		{
			if( stricmp( actor->getName(),"bbox_min" ) == 0 )
			{
				// 必须是ref
				if( !actor->isReference() )
				{
					return false;
				}
				// 必须有TranslateTrack
				if( !_actors[i].get()->getTranslateTrack() || _actors[i].get()->getTranslateTrack()->getNumKeys() == 0 )
				{
					return false;
				}

				const ATOM_Vector3f & position = _actors[i].get()->getTranslateTrack()->getKeyValueByIndex(0);
				_temp_bbox.extend(position);

				bHasPredefineBBox = true;
			}
			else if( stricmp( actor->getName(),"bbox_max" ) == 0 )
			{
				// 必须是ref
				if( !actor->isReference() )
				{
					return false;
				}
				// 必须有TranslateTrack
				if( !_actors[i].get()->getTranslateTrack() || _actors[i].get()->getTranslateTrack()->getNumKeys() == 0 )
				{
					return false;
				}

				const ATOM_Vector3f & position = _actors[i].get()->getTranslateTrack()->getKeyValueByIndex(0);
				_temp_bbox.extend(position);

				bHasPredefineBBox = true;
			}
		}
		//------------------------------//
	}

	if( bHasPredefineBBox )
	{
		_predefinedBBox = _temp_bbox;
	}

	writeAttribute (xml);

	return true;
}

void ATOM_CompositionNode::setCompositionFileName (const ATOM_STRING &filename)
{
	_compositionFileName = filename;
}

const ATOM_STRING &ATOM_CompositionNode::getCompositionFileName (void) const
{
	return _compositionFileName;
}

void ATOM_CompositionNode::att_setPlaying (int play)
{
	//-----------------------------------------------//
	// wangjian added 
	// 当前已经在播放 && 节点当前还未加载完成 不停止播放
	if( _playing && !play && !checkAsyncLoadFlag() )
		return;
	//-----------------------------------------------//

	play ? this->play(0) : this->stop ();	
}

int ATOM_CompositionNode::att_getPlaying (void) const
{
	return isPlaying () ? 1 : 0;
}

bool ATOM_CompositionNode::isLoadReferenceActors (void) const
{
	return _loadRef;
}

void ATOM_CompositionNode::setLoadReferenceActors (bool enable)
{
	_loadRef = enable;
}

//--- wangjian added ---//

bool ATOM_CompositionNode::loadSync(ATOM_STRING & file)
{
	// 设置加载完成标记
	//_load_flag = LOAD_ALLFINISHED;

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
		if( false == _attribFile->loadSync(file.c_str()) )
			return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	ATOM_TiXmlElement *root = _attribFile->getDocument().RootElement ();
	if (!root)
	{
		return false;
	}

	if (strcmp (root->Value(), "Composition"))
	{
		return false;
	}

	if (!loadAttribute (root))
	{
		return false;
	}

	_load_priority = ATOM_LoadPriority_IMMEDIATE;

	_compositionFileName = file;
	_filenameToBeLoad = file;

	if (loadFromXML (ATOM_GetRenderDevice(), root))
	{
		updateGroups();
		_groupDirty = false;
		return true;
	}

	return false;
}
bool ATOM_CompositionNode::checkNodeAllFinshed()
{
	// 如果开启场景异步加载 且 当前加载标记为'加载尚未开始'
	if( ATOM_AsyncLoader::isEnableSceneMTLoad() && _load_flag < LOAD_LOADSTARTED )
		return true;

	return	checkAsyncLoadFlag();
}

// 异步加载：检查异步加载完成标记
bool ATOM_CompositionNode::checkAsyncLoadFlag()
{
	
	if( !ATOM_AsyncLoader::IsRun() )					// 如果没有开启异步加载
		return true;
	
	//if( _load_priority == ATOM_LoadPriority_IMMEDIATE )	// 如果加载优先级为立即加载
	//	return true;
	
	if( _load_flag == LOAD_ALLFINISHED )				// 如果已经加载完成
		return true;

	if( !getNumTopActors() )
		return false;

	// 如果有一个没有加载完成就不显示。
	//（如果有一个加载完成了就显示？有加载失败的情况：eg.文件不存在）
	for( int i = 0; i < _actors.size(); ++i )
	{
		ATOM_CompositionActor * actor = getTopActor(i);
		if( actor )
		{			
			//if( !actor->getNode()->checkNodeAllFinshed() )
			if( !actor->checkNodeAllLoadFinshed() )
			{
				if( ATOM_AsyncLoader::IsRun() && ATOM_AsyncLoader::isEnableLog() )
					ATOM_LOGGER::log(	"%s : %s %s %s %d %d \n",
										__FUNCTION__,
										getCompositionFileName().c_str(),
										actor->getNode()->getClassName(),
										actor->getName(),
										actor->getNode()->_load_flag,
										actor->getNode()->_load_priority );
				return false;
			}
		}
	}

	// 设置加载完成标记
	_load_flag = LOAD_ALLFINISHED;

	return true;
}

// 属性文件加载请求完成
void ATOM_CompositionNode::onLoadFinished()
{
	ATOM_ASSERT(_attribFile.get());

	if( _attribFile->getAsyncLoader()->IsLoadFailed() )
	{
		_load_flag = LOAD_ALLFINISHED;
		return;
	}

	_load_flag = LOAD_ATTRIBUTE_LOADED;

	const ATOM_TiXmlElement * root = _attribFile->getDocument().RootElement();
	if (!root)
	{
		ATOM_LOGGER::error("<%s> : XML file has no root element!\n");
		return;
	}
	if ( strcmp (root->Value(), "Composition") )
	{
		ATOM_LOGGER::error("<%s> : this is not a valid composition node file!\n");
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// 如果当前是否有变换，保存当前的变换[因为loadAttribute方法可能会重设变换]
	bool bHasTransform = false;
	ATOM_Transformf temp_transform;
	if( !_transform.getO2T().almostEqual( ATOM_Matrix4x4f::getIdentityMatrix() ) )
	{
		temp_transform = _transform;
		bHasTransform = true;
	}
	bool bPickable = getPickable2();
	int pri_saved = _load_priority;	// 先保存加载优先级 因为loadAttribute会改变_load_priority的值
	//////////////////////////////////////////////////////////////////////////////////////////////

	if ( !loadAttribute (root) )
	{
		ATOM_LOGGER::error("<%s> : load attribute failed!\n");
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	_load_priority = pri_saved;		// 恢复加载优先级
	setPickable(bPickable);
	// 如果有变换，则设置回原来的变换
	if( bHasTransform )
	{
		setO2T(temp_transform.getO2T());
	}
	//////////////////////////////////////////////////////////////////////////////////////////////

	_compositionFileName	= _attribFile->getAsyncLoader()->filename;
	_filenameToBeLoad		= _attribFile->getAsyncLoader()->filename;

	// 加载XML属性文件 提交子节点的加载请求
	if ( loadFromXML (ATOM_GetRenderDevice(), root) )
	{
		updateGroups();
		_groupDirty = false;
	}
}

void ATOM_CompositionNode::resetMaterialDirtyFlag_impl()
{
	if( _actorParent.get() )
		_actorParent->resetMaterialDirtyFlag();
}
//----------------------//