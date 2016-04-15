#include "stdafx.h"
#include "visitor.h"
#include "loadinterface.h"

// disable stack trace
#ifndef ATOM_NO_STACK_TRACE
#define ATOM_NO_STACK_TRACE
#endif

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Node)
  ATOM_ATTRIBUTES_BEGIN(ATOM_Node)
	ATOM_ATTRIBUTE_PERSISTENT_NODEFAULT	(ATOM_Node, "Describe", getDescribe, setDescribe, "group=ATOM_Node;desc='描述'")
	ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "NodeFileName", getNodeFileName, setNodeFileName, "", "group=ATOM_Node;type=vfilename;desc='节点文件名'")
	ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "Visible", getShow2, setShow, 1, "group=ATOM_Node;type=int32;enum='Hide 0 Show 1 HideAll 2 Hide_Pickable 3 Inherit -1';desc='可视状态'")
	ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "Pickable", getPickable2,	setPickable,			1, "group=ATOM_Node;type=int32;enum='NonPickable 0 Pickable 1 Inherit -1';desc='可点选状态'" )
    ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "ContribCulling", isContributeCullingEnabled,	enableContributeCulling,1, "group=ATOM_Node;type=bool;desc='屏幕大小剪裁'")
    ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "matrix", getO2T,	setO2T, ATOM_Matrix4x4f::getIdentityMatrix(), "group=ATOM_Node;desc='局部变换矩阵'")
	ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "IgnoreFog", isIgnoreFog,	ignoreFogRecursive,	0, "group=ATOM_Node;type=bool;desc='忽略雾效'")
	ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "TopMost", getTopMost2, setTopMost, 0, "group=ATOM_Node;type=int32;enum='NonTopMost 0 TopMost 1 Inherit -1';desc='穿透效果'")
	ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "AntiTopMost", getAntiTopMost2, setAntiTopMost, 0, "group=ATOM_Node;type=int32;enum='NonAntiTopMost 0 AntiTopMost 1 Inherit -1';desc='阻止穿透'")
	ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "Projectable", getProjectable, setProjectable, 0, "group=ATOM_Node;type=int32;enum='NonProjectable 0 Projectable 1 Inherit -1';desc='可投射'")
	ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "Private", getPrivate, setPrivate, 0, "group=ATOM_Node;type=bool;desc='内部节点'")
	ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "BBoxMin", getPredefinedBBoxMin, setPredefinedBBoxMin, ATOM_Vector3f(0.f), "group=ATOM_Node;desc='预定义包围盒极小点'")
	ATOM_ATTRIBUTE_PERSISTENT (ATOM_Node, "BBoxMax", getPredefinedBBoxMax, setPredefinedBBoxMax, ATOM_Vector3f(0.f), "group=ATOM_Node;desc='预定义包围盒极大点'")
	ATOM_ATTRIBUTE (ATOM_Node, "Scale", getScale, setScale,	"group=ATOM_Node;desc='节点缩放'")
	ATOM_ATTRIBUTE (ATOM_Node, "Position", getTranslation, setTranslation, "group=ATOM_Node;desc='节点位置'")
	ATOM_ATTRIBUTE (ATOM_Node, "Rotation", getRotation,	setRotation,	"group=ATOM_Node;type=quat;desc='节点旋转'")
	ATOM_ATTRIBUTE_READONLY (ATOM_Node, "WorldScale", getWorldScale, "group=ATOM_Node;desc='节点缩放(世界)'")
	ATOM_ATTRIBUTE_READONLY (ATOM_Node, "WorldPosition", getWorldTranslation, "group=ATOM_Node;desc='节点位置(世界)'")
	ATOM_ATTRIBUTE_READONLY (ATOM_Node, "WorldRotation", getWorldRotation, "group=ATOM_Node;type=quat;desc='节点旋转(世界)'")
	//--- wangjian added ---//
	// 异步加载 ：异步加载标记
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Geode, "AsyncLoad", getLoadPriority, setLoadPriority, 1, "group=ATOM_Node;desc='异步加载'")
	//----------------------//
  ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Node, ATOM_Object)

ATOM_Node::ATOM_Node (void)
{
	ATOM_STACK_TRACE(ATOM_Node::ATOM_Node);

	_showMode = SHOW;
	_pickable = PICKABLE;
	_cellNodeInfo = 0;
	_contributeCulling = 1;
	_parent = 0;
	_changeStamp = 0;
	_worldMatrixStamp = 0xFFFFFFFF;
	_iworldMatrixStamp = 0xFFFFFFFF;
	_bboxStamp = 0xFFFFFFFF;
	_wbboxStamp = 0xFFFFFFFF;
	_topMost = NONTOPMOST;
	_antiTopMost = NONANTITOPMOST;
	_projectable = NONPROJECTABLE;
	_visibleStamp = 0;
	_private = 0;
	_skipClipTest = false;
	_enableGeoClipping = true;

	_entity = 0;
	_ignoreFog = 0;
	_drawBoundingbox = 0;
	_noOccludedFrameStamp = 0;
	_enableMTLoading = true;
	_decomposeDirty = true;
	_worldTransformDirty = true;

	//--- wangjian added for async loading test ---//
	// 异步加载
	_load_flag		= LOAD_NOTLOAD;
	_load_priority	= ATOM_LoadPriority_ASYNCBASE;
	_predefinedBBox.setMin(0.f);
	_predefinedBBox.setMax(0.f);
	//_attribFileLoader = 0;
	_attribFile = 0;
	//---------------------------------------------//
}

ATOM_Node::~ATOM_Node (void)
{
	ATOM_STACK_TRACE(ATOM_Node::~ATOM_Node);

	ATOM_ASSERT(!_parent);

    if (_cellNodeInfo)
    {
	    _cellNodeInfo->cell->detachNode (this);
    }

	clearChildren ();

	// wangjian added 异步加载相关 节点属性文件加载对象
	//ATOM_AsyncLoader::DestroyAttribFileLoader(_attribFileLoader.get());
}

ATOM_Node *ATOM_Node::getParent(void) const 
{ 
	ATOM_STACK_TRACE(ATOM_Node::getParent);

	return _parent;	
}

void ATOM_Node::setDescribe (const ATOM_STRING &str)
{
	_describe = str;
}

const ATOM_STRING &ATOM_Node::getDescribe (void) const
{
	return _describe;
}

ATOM_SpatialCell::NodeInfo *ATOM_Node::getCellNodeInfo (void) const 
{ 
	return _cellNodeInfo; 
}

void ATOM_Node::setCellNodeInfo (ATOM_SpatialCell::NodeInfo *cellNodeInfo) 
{ 
	_cellNodeInfo = cellNodeInfo; 
}

void ATOM_Node::setGameEntity (ATOM_GameEntity *entity) 
{ 
	_entity = entity; 
}

ATOM_GameEntity *ATOM_Node::getGameEntity (void) const 
{ 
	return _entity; 
}

bool ATOM_Node::load (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_Node::load);

	//_filenameToBeLoad = "";

	if (ATOM_RenderSettings::isFixedFunction() && !supportFixedFunction ())
	{
		//ATOM_LOGGER::warning ("[%s::load]: This type node cannot be used in Fixed-Function pipeline\n", getClassName());
		return false;
	}

	ATOM_LoadInterface::LoadingState state = getLoadInterface()->getLoadingState ();

	if (state == ATOM_LoadInterface::LS_NOTLOADED)
	{
		//--- wangjian added ---//
		// 如果设置为延迟加载（在cullvisit时进行 ）
		if( _load_flag == LOAD_DELAYLOAD )
		{
			_load_flag = LOAD_NOTLOAD;
			return true;
		}

		// 加载开始
		if( _load_flag != LOAD_ALLFINISHED )
			_load_flag = LOAD_LOADSTARTED;
		//----------------------//

		bool ret = onLoad (device);

		if (!ret)
		{
			ATOM_LOGGER::error("[%s::load]: load failed\n", getClassName());
			dumpAttributes ("  ");
		}
		else
		{
			getLoadInterface()->setLoadingState (ATOM_LoadInterface::LS_LOADED);
		}

		return ret;
	}
	else if (state == ATOM_LoadInterface::LS_LOADED)
	{
		return true;
	}
	else if (state == ATOM_LoadInterface::LS_LOADING)
	{
		ATOM_ContentStream::waitForInterfaceDone (getLoadInterface());

		return getLoadInterface()->getLoadingState() == ATOM_LoadInterface::LS_LOADED;
	}
	else
	{
		return false;
	}
}
//--- wangjian modified ---//
// 异步加载 ： 添加异步加载优先级(默认为非异步加载）
bool ATOM_Node::loadFromFile (ATOM_RenderDevice *device, const char *filename, int loadPriority /*= ATOM_LoadPriority_IMMEDIATE*/)
{
	ATOM_STACK_TRACE(ATOM_Node::loadFromFile);

	_filenameToBeLoad = filename ? filename : "";

	if (ATOM_RenderSettings::isFixedFunction() && !supportFixedFunction ())
	{
		ATOM_LOGGER::error ("[%s::load]: This type node cannot be used in Fixed-Function pipeline\n", getClassName());
		return false;
	}

	ATOM_LoadInterface::LoadingState state = getLoadInterface()->getLoadingState ();

	//--- wangjian added ---//
	// 异步加载 ：设置加载优先级
	_load_priority = loadPriority;
	//----------------------//

	if (state == ATOM_LoadInterface::LS_NOTLOADED)
	{
		//--- wangjian added ---//
		// 如果设置为延迟加载（在cullvisit时进行 ）
		if( _load_flag == LOAD_DELAYLOAD )
		{
			_load_flag = LOAD_NOTLOAD;
			return true;
		}

		// 加载开始
		//ATOM_ASSERT(_load_flag<LOAD_LOADSTARTED);
		if( _load_flag != LOAD_ALLFINISHED )
			_load_flag = LOAD_LOADSTARTED;
		//----------------------//

		bool ret = onLoad (device);

		if (!ret)
		{
			ATOM_LOGGER::error("[%s::load]: load failed\n", getClassName());
			dumpAttributes ("  ");
		}
		else
		{
			getLoadInterface()->setLoadingState (ATOM_LoadInterface::LS_LOADED);
		}

		return ret;
	}
	else if (state == ATOM_LoadInterface::LS_LOADED)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ATOM_Node::mtload (ATOM_RenderDevice *device, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group)
{
	ATOM_STACK_TRACE(ATOM_Node::mtload);

	_filenameToBeLoad = "";

	if (ATOM_RenderSettings::isFixedFunction() && !supportFixedFunction ())
	{
		ATOM_LOGGER::error ("[%s::load]: This type node cannot be used in Fixed-Function pipeline\n", getClassName());
		return false;
	}

	return onMtLoad (device, flags, callback, userData, group);
}

bool ATOM_Node::mtloadFromFile (ATOM_RenderDevice *device, const char *filename, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group)
{
	ATOM_STACK_TRACE(ATOM_Node::mtloadFromFile);

	_filenameToBeLoad = filename ? filename : "";

	if (ATOM_RenderSettings::isFixedFunction() && !supportFixedFunction ())
	{
		ATOM_LOGGER::error ("[%s::load]: This type node cannot be used in Fixed-Function pipeline\n", getClassName());
		return false;
	}

	return onMtLoad (device, flags, callback, userData, group);
}

bool ATOM_Node::onMtLoad (ATOM_RenderDevice *device, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group)
{
	ATOM_STACK_TRACE(ATOM_Node::onMtLoad);

	bool ret;

	if (_enableMTLoading && supportMTLoading ())
	{
		return getLoadInterface ()->mtload (flags, group, callback, userData) == ATOM_LoadInterface::LS_LOADING;
	}
	else
	{
		ret = getLoadInterface ()->load () == ATOM_LoadInterface::LS_LOADED;

		if (callback)
		{
			ATOM_LoadingRequest *request = ATOM_NEW(ATOM_LoadingRequest);
			request->loadInterface = getLoadInterface();
			request->flags = flags;
			request->callback = callback;
			request->userData = userData;
			request->stage = !ret ? LOADINGSTAGE_NOTLOADED : LOADINGSTAGE_FINISH;

			callback (request);
		}
	}

	return ret;
}

bool ATOM_Node::onLoad(ATOM_RenderDevice *pDevice)
{
	return true;
}

const ATOM_BBox& ATOM_Node::getBoundingbox(void) const
{
	ATOM_STACK_TRACE(ATOM_Node::getBoundingbox);

	if (_bboxStamp != _changeStamp)
    {
	    buildBoundingbox ();

	    _bboxStamp = _changeStamp;
    }
    return _boundingBox;
}

const ATOM_BBox & ATOM_Node::getWorldBoundingbox (void) const
{
	ATOM_STACK_TRACE(ATOM_Node::getWorldBoundingbox);

	if (_wbboxStamp != _changeStamp)
	{
		buildWorldBoundingbox ();
		_wbboxStamp = _changeStamp;
	}

	return _boundingBoxW;
}

void ATOM_Node::invalidateBoundingbox (void)
{
	onTransformChanged ();
	//++_changeStamp ();
	//_bboxStamp = _wbboxStamp = _changeStamp - 1;
}

unsigned ATOM_Node::getChangeStamp (void) const
{ 
	return _changeStamp; 
}

void ATOM_Node::buildBoundingbox(void) const
{
  _boundingBox.setMin(ATOM_Vector3f(0,0,0));
  _boundingBox.setMax(ATOM_Vector3f(0,0,0));
}

int ATOM_Node::getPickable2 (void) const
{
	return _pickable;
}

int ATOM_Node::getPickable (void) const
{
	if (_pickable == INHERIT)
	{
		return getParent() ? getParent()->getPickable() : PICKABLE;
	}
	else
	{
		return _pickable;
	}
}

void ATOM_Node::setPickable (int pickable)
{
	_pickable = pickable;
}

void ATOM_Node::setPickableRecursive (int pickable)
{
	setPickable (pickable);

	for (unsigned i = 0; i < _children.size(); ++i)
	{
		_children[i]->setPickableRecursive (pickable);
	}
}

int ATOM_Node::getShow2(void) const
{
    return _showMode;
}

int ATOM_Node::getShow (void) const
{
	if (_showMode == INHERIT)
	{
		return getParent() ? getParent()->getShow() : SHOW;
	}
	else
	{
		return _showMode;
	}
}

void ATOM_Node::setShow(int show)
{
    _showMode = show;
}

void ATOM_Node::setShowRecursive (int show)
{
	setShow (show);

	for (unsigned i = 0; i < _children.size(); ++i)
	{
		_children[i]->setShowRecursive (show);
	}
}

const ATOM_STRING &ATOM_Node::getLayer (void) const
{
	return _layer;
}

void ATOM_Node::setLayer (const ATOM_STRING &layer)
{
	_layer = layer;
}

int ATOM_Node::isContributeCullingEnabled (void) const
{
	return _contributeCulling;
}

void ATOM_Node::enableContributeCulling (int b)
{
	_contributeCulling = b;
}

void ATOM_Node::enableContributeCullingRecursive (int b)
{
	enableContributeCulling (b);

	for (unsigned i = 0; i < _children.size(); ++i)
	{
		_children[i]->enableContributeCulling (b);
	}
}

void ATOM_Node::dump (const char *indent) {
  dumpAttributes (indent);
}

bool ATOM_Node::appendChild(ATOM_Node *node)
{
	if (node && !node->getParent() && !hasChild (node))
	{
		_children.push_back (node);
		node->_parent = this;
		node->onTransformChanged ();
		invalidateBoundingbox ();
		return true;
	}
	return false;
}

static void detachCell_R (ATOM_Node *node)
{
	if (node->getCellNodeInfo())
	{
		node->getCellNodeInfo()->cell->detachNode (node);
	}

	for (unsigned i = 0; i < node->getNumChildren(); ++i)
	{
		detachCell_R (node->getChild (i));
	}
}

bool ATOM_Node::removeChild(ATOM_Node *node)
{
	ATOM_STACK_TRACE(ATOM_Node::removeChild);

	NodeVectorIter iter = std::find (_children.begin(), _children.end(), node);
	if (iter != _children.end ())
	{
		node->_parent = 0;
		detachCell_R (node);
		_children.erase (iter);

		invalidateBoundingbox ();
		return true;
	}
	return false;
}

unsigned ATOM_Node::getNumChildren (void) const
{
  return _children.size ();
}

ATOM_Node *ATOM_Node::getChild (unsigned index) const
{
	if( !_children.empty() && index >= 0 && index < _children.size() )
	//ATOM_LOGGER::log("ATOM_Node::getChild : OBJECT : %s has child : %d \n", _M_object_name.c_str(), _children.empty() ? 0 : _children.size() );
		return _children[index].get();
	return 0;
}

void ATOM_Node::clearChildren()
{
	ATOM_STACK_TRACE(ATOM_Node::clearChildren);

	while (_children.size())
	{
		removeChild (_children.begin()->get());
	}
}

void ATOM_Node::getCombinedBoundingBox (ATOM_BBox &bbox) const
{
	bbox = getBoundingbox ();

	for (unsigned i = 0; i < _children.size(); ++i)
	{
		ATOM_BBox bboxChild;
		_children[i]->getCombinedBoundingBox(bboxChild);
		bboxChild.transform (_children[i]->getO2T());

		bbox.extend (bboxChild.getMin());
		bbox.extend (bboxChild.getMax());
	}
}

const ATOM_Vector3f &ATOM_Node::getPredefinedBBoxMin (void) const
{
	return _predefinedBBox.getMin();
}

void ATOM_Node::setPredefinedBBoxMin (const ATOM_Vector3f &v)
{
	_predefinedBBox.setMin (v);
}

const ATOM_Vector3f &ATOM_Node::getPredefinedBBoxMax (void) const
{
	return _predefinedBBox.getMax();
}

void ATOM_Node::setPredefinedBBoxMax (const ATOM_Vector3f &v)
{
	_predefinedBBox.setMax (v);
}

void ATOM_Node::enableGeometryClipping (bool enable)
{
	_enableGeoClipping = enable;
}

bool ATOM_Node::isGeometryClippingEnabled (void) const
{
	return _enableGeoClipping;
}

bool ATOM_Node::hasChild(ATOM_Node *node) const
{
  return std::find (_children.begin(), _children.end(), node) != _children.end ();
}

bool ATOM_Node::queryReferencePoint (const char *name, ATOM_Matrix4x4f *matrix) const
{
	return false;
}

void ATOM_Node::getReferencePointList (ATOM_VECTOR<ATOM_STRING> &pointlist) const
{
	pointlist.resize (0);
}

const ATOM_Matrix4x4f & ATOM_Node::getO2T(void) const {
  return _transform.getO2T ();
}

void ATOM_Node::setO2T (const ATOM_Matrix4x4f &matrix)
{
	_transform.setO2T(matrix);
	_decomposeDirty = true;
	onTransformChanged ();
}

//void ATOM_Node::setOrigin(const ATOM_Vector3f &V) {
//  _transform.setOrigin(V);
//  onTransformChanged ();
//}

//void ATOM_Node::translate(const ATOM_Vector3f &V) {
//  _transform.translate(V);
//  onTransformChanged ();
//}

//void ATOM_Node::otherRotate(const ATOM_Vector3f& Axis, float fAngle) {
//  _transform.otherRotate(Axis, fAngle);
//  onTransformChanged ();
//}

//void ATOM_Node::otherRotate(const ATOM_Vector3f &Axis, float tSin, float tCos) {
//  _transform.otherRotate(Axis, tSin, tCos);
//  onTransformChanged ();
//}

//void ATOM_Node::rotateOther(const ATOM_Vector3f& Axis, float fAngle) {
//  _transform.rotateOther(Axis, fAngle);
//  onTransformChanged ();
//}

//void ATOM_Node::rotateOther(const ATOM_Vector3f &Axis, float tSin, float tCos) {
//  _transform.rotateOther(Axis, tSin, tCos);
//  onTransformChanged ();
//}

//void ATOM_Node::rotateThis(const ATOM_Vector3f& Axis, float fAngle) {
//  _transform.rotateThis(Axis, fAngle);
//  onTransformChanged ();
//}

//void ATOM_Node::rotateThis(const ATOM_Vector3f& Axis, float tSin, float tCos) {
//  _transform.rotateThis(Axis, tSin, tCos);
//  onTransformChanged ();
//}

//void ATOM_Node::lookAt(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz) {
//  _transform.lookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
//  onTransformChanged ();
//}

//void ATOM_Node::setTransform(const ATOM_Vector3f &Origin, const ATOM_Vector3f& Axis, float tSin, float tCos) {
//  _transform.setTransform(Origin, Axis, tSin, tCos);
//  onTransformChanged ();
//}

//void ATOM_Node::setTransform(const ATOM_Vector3f &Origin, const ATOM_Vector3f& Axis, float fAngle) {
//  _transform.setTransform(Origin, Axis, fAngle);
//  onTransformChanged ();
//}

//ATOM_Vector3f ATOM_Node::getOrigin() const {
//  return _transform.getOrigin();
//}

//ATOM_Vector3f ATOM_Node::this2Other(const ATOM_Vector3f &V) const {
//  return _transform.this2Other(V);
//}

//ATOM_Vector3f ATOM_Node::other2This(const ATOM_Vector3f& V) const {
//  return _transform.other2This(V);
//}

void ATOM_Node::accept (ATOM_Visitor &visitor)
{
	ATOM_STACK_TRACE(ATOM_Node::accept);

	visitor.visit (*this);
}

const ATOM_Matrix4x4f &ATOM_Node::getWorldMatrix (void) const 
{
	ATOM_STACK_TRACE(ATOM_Node::getWorldMatrix);

	if (_worldMatrixStamp != _changeStamp)
	{
		buildWorldMatrix ();
		_worldMatrixStamp = _changeStamp;
	}
	return _worldMatrix;
}

const ATOM_Matrix4x4f &ATOM_Node::getInvWorldMatrix (void) const 
{
	ATOM_STACK_TRACE(ATOM_Node::getInvWorldMatrix);

	if (_iworldMatrixStamp != _changeStamp)
	{
		buildInvWorldMatrix ();
		_iworldMatrixStamp = _changeStamp;
	}
	return _iworldMatrix;
}

void ATOM_Node::buildWorldMatrix (void) const
{
	_worldMatrix = _parent ? (_parent->getWorldMatrix() >> getO2T ()) : getO2T ();
}

void ATOM_Node::buildInvWorldMatrix (void) const
{
	_iworldMatrix.invertAffineFrom (getWorldMatrix ());
}

void ATOM_Node::buildWorldBoundingbox (void) const
{
	//--- wangjian modified ---//
	// 检查节点是否加载完成
#if 0
	if( _load_priority == ATOM_LoadPriority_IMMEDIATE || _load_flag == LOAD_ALLFINISHED )
#else
	if( _load_flag == LOAD_ALLFINISHED )
#endif
	{
		_boundingBoxW = getBoundingbox ();
	}
	else
	{
		_boundingBoxW = _predefinedBBox;
	}
	//------------------------//
	
	_boundingBoxW.transform (getWorldMatrix());
}

void ATOM_Node::onTransformChanged (void)
{
	++_changeStamp;
	_worldTransformDirty = true;

	for (unsigned i = 0; i < getNumChildren(); ++i)
	{
		getChild(i)->onTransformChanged ();
	}
}

void ATOM_Node::ignoreFog (int ignore)
{
	_ignoreFog = ignore;
}

int ATOM_Node::isIgnoreFog (void) const
{
	return _ignoreFog;
}

void ATOM_Node::ignoreFogRecursive (int ignore)
{
	ignoreFog (ignore);

	for (unsigned i = 0; i < _children.size(); ++i)
	{
		_children[i]->ignoreFogRecursive (ignore);
	}
}

void ATOM_Node::setDrawBoundingbox (int bDraw)
{
	_drawBoundingbox = bDraw;
}

int ATOM_Node::getDrawBoundingbox (void) const
{
	return _drawBoundingbox;
}

void ATOM_Node::setNoOccludedFrameStamp (unsigned frameStamp)
{
	_noOccludedFrameStamp = frameStamp;
}

unsigned ATOM_Node::getNoOccludedFrameStamp (void) const
{
	return _noOccludedFrameStamp;
}

bool ATOM_Node::mtLoadFromDisk (ATOM_BaseResourceLoader *loader)
{
	return true;
}

bool ATOM_Node::mtLock (ATOM_RenderDevice *device, ATOM_BaseResourceLoader *loader)
{
	return true;
}

bool ATOM_Node::mtUploadDeviceData (ATOM_BaseResourceLoader *loader)
{
	return true;
}

ATOM_LoadingStage ATOM_Node::mtUnlock (ATOM_RenderDevice *device, ATOM_BaseResourceLoader *loader)
{
	return LOADINGSTAGE_FINISH;
}

void ATOM_Node::mtWait (void)
{
	return;
}

void ATOM_Node::enableMTLoading (bool enable)
{
	_enableMTLoading = enable;
}

bool ATOM_Node::isMTLoadingEnabled (void) const
{
	return _enableMTLoading;
}

bool ATOM_Node::supportMTLoading (void)
{
	return false;
}

ATOM_LoadInterface *ATOM_Node::getLoadInterface (void) const
{
	ATOM_STACK_TRACE(ATOM_Node::getLoadInterface);

	if (!_loadInterface)
	{
		_loadInterface = ATOM_NEW(ATOM_NodeLoadInterface, const_cast<ATOM_Node*>(this));
	}
	return _loadInterface.get();
}

void ATOM_Node::updateDecomposition (void) const
{
	_rotation.makeIdentity ();
	getO2T().decompose (_translation, _rotation, _scale);
	ATOM_Quaternion q;
	ATOM_Matrix4x4f m;
	q.fromMatrix (_rotation);
	_quat.set (q.x, q.y, q.z, q.w);
	_decomposeDirty = false;
}

void ATOM_Node::updateComposition (void)
{
	ATOM_Matrix4x4f m = _rotation;
	m.m30 = _translation.x;
	m.m31 = _translation.y;
	m.m32 = _translation.z;
	setO2T (m >> ATOM_Matrix4x4f::getScaleMatrix(_scale));
}

void ATOM_Node::setRotation (const ATOM_Vector4f &quaternion)
{
	if (_decomposeDirty)
	{
		updateDecomposition ();
	}

	_quat = quaternion;
	ATOM_Quaternion q(_quat.x, _quat.y, _quat.z, _quat.w);
	_rotation.makeIdentity ();
	q.toMatrix (_rotation);

	updateComposition ();
}

const ATOM_Vector4f &ATOM_Node::getRotation (void) const
{
	if (_decomposeDirty)
	{
		updateDecomposition ();
	}
	return _quat;
}

void ATOM_Node::setTranslation (const ATOM_Vector3f &translation)
{
	if (_decomposeDirty)
	{
		updateDecomposition ();
	}

	_translation = translation;

	updateComposition ();
}

const ATOM_Vector3f &ATOM_Node::getTranslation (void) const
{
	if (_decomposeDirty)
	{
		updateDecomposition ();
	}
	return _translation;
}

void ATOM_Node::setScale (const ATOM_Vector3f &scale)
{
	if (_decomposeDirty)
	{
		updateDecomposition ();
	}

	_scale = scale;

	updateComposition ();
}

const ATOM_Vector3f &ATOM_Node::getScale (void) const
{
	if (_decomposeDirty)
	{
		updateDecomposition ();
	}
	return _scale;
}

void ATOM_Node::assign (ATOM_Node *other) const
{
	copyAttributesTo (other);

	other->setO2T (getO2T ());
}

ATOM_AUTOREF(ATOM_Node) ATOM_Node::clone (void) const
{
	ATOM_AUTOREF(ATOM_Node) other = ATOM_CreateObject (getClassName(), 0);

	assign (other.get());

	if (other->getLoadInterface()->getLoadingState() == ATOM_LoadInterface::LS_LOADED || other->load (ATOM_GetRenderDevice()))
	{
		return other;
	}

	return 0;
}

void ATOM_Node::updateWorldDecomposition (void) const
{
	const ATOM_Matrix4x4f &worldMatrix = getWorldMatrix ();
	ATOM_Matrix4x4f r;
	worldMatrix.decompose (_translationW, r, _scaleW);
	ATOM_Quaternion q;
	q.fromMatrix (r);
	_rotationW.set (q.x, q.y, q.z, q.w);
}

const ATOM_Vector3f &ATOM_Node::getWorldTranslation (void) const
{
	if (_worldTransformDirty)
	{
		updateWorldDecomposition ();
	}
	return _translationW;
}

const ATOM_Vector4f &ATOM_Node::getWorldRotation (void) const
{
	if (_worldTransformDirty)
	{
		updateWorldDecomposition ();
	}
	return _rotationW;
}

const ATOM_Vector3f &ATOM_Node::getWorldScale (void) const
{
	if (_worldTransformDirty)
	{
		updateWorldDecomposition ();
	}
	return _scaleW;
}

bool ATOM_Node::supportFixedFunction (void) const
{
	return true;
}

void ATOM_Node::setNodeFileName (const ATOM_STRING &filename)
{
	_filenameToBeLoad = filename;
}

const ATOM_STRING &ATOM_Node::getNodeFileName (void) const
{
	return _filenameToBeLoad;
}

static ATOM_HASHMAP<ATOM_STRING, std::pair<ATOM_STRING, ATOM_STRING> > &getFileExtensionMap (void)
{
	static ATOM_HASHMAP<ATOM_STRING, std::pair<ATOM_STRING, ATOM_STRING> > fileExtMap;

	return fileExtMap;
}

bool ATOM_Node::registerNodeFileType (const char *classname, const char *fileext, const char *desc)
{
	if (!classname || !classname[0] || !fileext || !fileext[0])
	{
		return false;
	}

	char buffer[16];
	const char *ext = fileext;

	for (;;)
	{
		const char *ext_sep = strchr (ext, ';');
		if (ext_sep)
		{
			memcpy (buffer, ext, ext_sep - ext);
			buffer[ext_sep - ext] = '\0';
		}
		else
		{
			strcpy (buffer, ext);
		}

		ATOM_HASHMAP<ATOM_STRING, std::pair<ATOM_STRING, ATOM_STRING> >::const_iterator it = getFileExtensionMap().find (buffer);
		if (it != getFileExtensionMap().end ())
		{
			ATOM_LOGGER::warning("%s(%s,%s) file name extension <%s> was already registered for class <%s>\n", __FUNCTION__, classname, fileext, buffer, it->second.first.c_str());
		}

		std::pair<ATOM_STRING, ATOM_STRING> &pair = getFileExtensionMap()[buffer];
		pair.first = classname;
		pair.second = desc;

		if (ext_sep)
		{
			ext = ext_sep + 1;
		}
		else
		{
			break;
		}
	}

	return true;
}

unsigned ATOM_Node::getNumRegisteredFileTypes (void)
{
	return getFileExtensionMap().size();
}

void ATOM_Node::getRegisteredFileInfo (unsigned index, ATOM_STRING &fileext, ATOM_STRING &filedesc)
{
	ATOM_HASHMAP<ATOM_STRING, std::pair<ATOM_STRING, ATOM_STRING> >::const_iterator it = getFileExtensionMap().begin();

	std::advance (it, index);

	fileext = it->first;
	filedesc = it->second.second;
}
//--- wangjian modified ---//
// 异步加载 ：添加异步加载优先级
ATOM_AUTOREF(ATOM_Node) ATOM_Node::loadNodeFromFile (	const char *filename, 
														int loadPriority/* = ATOM_LoadPriority_IMMEDIATE*/,	
														int loadFlag/* = 0*/	)
{
	ATOM_STACK_TRACE(ATOM_Node::loadNodeFromFile);

	if (filename)
	{
		const char *p = strrchr (filename, '.');

		if (!p)
		{
			return 0;
		}

		p++;

		ATOM_HASHMAP<ATOM_STRING, std::pair<ATOM_STRING, ATOM_STRING> >::const_iterator it = getFileExtensionMap().find (p);
		if (it == getFileExtensionMap().end ())
		{
			return 0;
		}

		ATOM_AUTOREF(ATOM_Node) node = ATOM_CreateObject (it->second.first.c_str(), 0);

		if (!node)
		{
			return 0;
		}

		

		//--- wangjian modified ---//

		node->setLoadFlag(loadFlag);

		// 异步加载 ：添加异步加载优先级
		if (!node->loadFromFile (ATOM_GetRenderDevice(), filename, loadPriority))
		{
			return 0;
		}

		//-------------------------//

		return node;
	}

	return 0;
}

void ATOM_Node::setTopMost (int bTopMost)
{
	_topMost = bTopMost;
}

int ATOM_Node::getTopMost (void) const
{
	if (_topMost == INHERIT)
	{
		return getParent() ? getParent()->getTopMost() : NONTOPMOST;
	}
	else
	{
		return _topMost;
	}
}

int ATOM_Node::getTopMost2 (void) const
{
	return _topMost;
}

void ATOM_Node::setAntiTopMost (int bAntiTopMost)
{
	_antiTopMost = bAntiTopMost;
}

int ATOM_Node::getAntiTopMost (void) const
{
	if (_antiTopMost == INHERIT)
	{
		return getParent() ? getParent()->getAntiTopMost() : NONANTITOPMOST;
	}
	else
	{
		return _antiTopMost;
	}
}

int ATOM_Node::getAntiTopMost2 (void) const
{
	return _antiTopMost;
}

void ATOM_Node::setProjectable (int projectable)
{
	_projectable = projectable;
}

int ATOM_Node::getProjectable (void) const
{
	if (_projectable == INHERIT)
	{
		return getParent() ? getParent()->getProjectable() : PROJECTABLE;
	}
	else
	{
		return _projectable;
	}
}

unsigned ATOM_Node::getVisibleStamp (void) const
{
	return _visibleStamp;
}

void ATOM_Node::setVisibleStamp (unsigned value)
{
	_visibleStamp = value;
}

void ATOM_Node::setPrivate (int bPrivate)
{
	_private = bPrivate;
}

int ATOM_Node::getPrivate (void) const
{
	return _private;
}

//--- wangjian added ---//
void ATOM_Node::setLoadFlag(unsigned flag)
{
	_load_flag = flag;
}
unsigned ATOM_Node::getLoadFlag(void) const
{
	return _load_flag;
}
// 异步加载 ：添加异步加载标志
void ATOM_Node::setLoadPriority(int pri)
{
#if 0
	if( ATOM_AsyncLoader::IsRun() )
		_load_priority = pri;
	else
		_load_priority = ATOM_LoadPriority_IMMEDIATE;
#else
	_load_priority = pri;
#endif
}
int ATOM_Node::getLoadPriority(void) const
{
#if 0
	if( ATOM_AsyncLoader::IsRun() )
		return _load_priority;
	else
		return ATOM_LoadPriority_IMMEDIATE;
#else
	return _load_priority;
#endif
}
void ATOM_Node::onLoadFinished()
{
	_load_flag = LOAD_ALLFINISHED;
}

// 检查节点是否已经开始加载（加载请求已经提交）
bool ATOM_Node::checkNodeLoadStarted() const
{
	return ( _load_flag > LOAD_NOTLOAD );
}
bool ATOM_Node::checkNodeLoadFinished()
{
	bool loaded = ( _load_flag == LOAD_LOADED );

	/*if( loaded && !_children.empty() )
	{
	NodeVectorIter iter = _children.begin();
	for(;iter!=_children.end();++iter)
	{
	ATOM_Node * child = (*iter).get();
	if( child )
	{
	if( ATOM_Geode * go = dynamic_cast<ATOM_Geode*>(child) )
	loaded &= child->CheckNodeLoadFinished();
	}
	}

	}*/
	
	return loaded;
}

// 检查节点是否完全加载完成了
bool ATOM_Node::checkNodeAllFinshed()
{
	// 如果没有开启异步加载 返回TRUE
	if( !ATOM_AsyncLoader::IsRun() )
		return true;


	// 如果开启场景异步加载 且 当前加载标记为'加载尚未开始'
	if( ATOM_AsyncLoader::isEnableSceneMTLoad() && _load_flag < LOAD_LOADSTARTED )
		return true;


	// 如果加载优先级为立即加载 或 加载标记为'加载已经完成'
	return ( /*_load_priority == ATOM_LoadPriority_IMMEDIATE || */_load_flag == LOAD_ALLFINISHED );
}

// 节点属性文件异步加载对象
//ATOM_AsyncableLoadAttributeFile * ATOM_Node::getAttribFileLoader( const char* filename )
//{
//	if( !_attribFileLoader && filename && filename[0]!='\0' )
//	{
//		_attribFileLoader = ATOM_AsyncLoader::GetAttribFileLoader(filename);
//	}
//	return _attribFileLoader.get();
//}

// 重置参数表的脏标记
void ATOM_Node::resetMaterialDirtyFlag()
{
	resetMaterialDirtyFlag_impl();

	unsigned num_child = getNumChildren();
	for( int i = 0; i < num_child; ++i )
	{
		ATOM_AUTOREF(ATOM_Node) child = getChild(i);
		if( child.get() )
			child->resetMaterialDirtyFlag();
	}
}
void ATOM_Node::resetMaterialDirtyFlag_impl()
{
	return;
}
//----------------------//

void ATOM_Node::skipClipTest (bool b)
{
	_skipClipTest = b;
}

bool ATOM_Node::isSkipClipTest (void) const
{
	return _skipClipTest;
}

