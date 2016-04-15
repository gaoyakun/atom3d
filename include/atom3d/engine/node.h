/**	\file engine/node.h
 *	SceneGraph�ڵ����.
 *
 *	\author ������
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_NODE_H
#define __ATOM3D_ENGINE_NODE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_geometry.h"
#include "../ATOM_kernel.h"
#include "../ATOM_render.h"

#include "camera.h"
#include "spatialcell.h"

#define ATOM_DECLARE_NODE_FILE_TYPE(classname, name, ext, desc)	\
	struct classname##_AutoRegisterFileType	\
	{	\
		classname##_AutoRegisterFileType(void)	\
		{	\
			ATOM_Node::registerNodeFileType (#name, ext, desc);	\
		}	\
	};	\
	static classname##_AutoRegisterFileType __classname##_auto_register_file_type;	\

#define ATOM_IMPLEMENT_NODE_FILE_TYPE(classname)	\
	classname::classname##_AutoRegisterFileType classname::__classname##_auto_register_file_type;	\

class ATOM_GameEntity;
class ATOM_Visitor;

//! \class ATOM_Node
//! SceneGraph�ڵ����.
//! \author ������
//! \todo ��Ҫ����
//! \ingroup engine
class ATOM_ENGINE_API ATOM_Node : public ATOM_Object
{
	ATOM_CLASS(engine, ATOM_Node, Node)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Node)
	friend class ATOM_SpatialCell;
	friend class ATOM_BaseNodeLoader;
	friend class BaseNodeCreator;

public:
	enum
	{
		HIDE = 0,
		SHOW = 1,
		HIDE_PICKABLE = 3
	};

	enum 
	{
		NONPICKABLE = 0,
		PICKABLE = 1
	};

	enum
	{
		NONTOPMOST = 0,
		TOPMOST = 1
	};

	enum
	{
		NONPROJECTABLE = 0,
		PROJECTABLE = 1
	};

	enum
	{
		NONANTITOPMOST = 0,
		ANTITOPMOST = 1
	};

	enum
	{
		INHERIT = -1
	};

public:
	//! ���캯��.
	ATOM_Node (void);

	//! ��������.
	virtual ~ATOM_Node (void);

public:
	//! ��ȡ�Լ��ĸ��ڵ�.
	//! \return ���ڵ㣬���û�и��ڵ㷵��NULL
	ATOM_Node *getParent(void) const;

	//! ��������.
	//! \param str ����
	void setDescribe (const ATOM_STRING &str);

	//! ��ȡ����.
	//! \return ����
	const ATOM_STRING &getDescribe (void) const;

	//! ֪ͨ�ڵ��Χ����Ҫ���¼���.
	void invalidateBoundingbox (void);

	//! ��ȡ�ýڵ�����任�仯��ʱ�����ÿ�θýڵ������任�����仯��ʱ��������.
	//! \return ����任�仯��ʱ���
	unsigned getChangeStamp (void) const;

	//! ��ȡ�ýڵ������Ŀռ�ָ�ڵ�ṹ.
	//! �ռ�ָ�ڵ�Ϊ�˲����ڵ㣬�������ڵ��,Ŀǰֻ��ʵ�ְ˲���
	//! \return �ռ�ָ�ڵ�ṹָ��
	//! \note �˺�����ҪΪ�ڲ�ʹ�ã��ⲿ�������
	//! \sa ATOM_SpatialCell::NodeInfo setCellNodeInfo
	ATOM_SpatialCell::NodeInfo *getCellNodeInfo (void) const;

	//! ���øýڵ������Ŀռ�ָ�ڵ�ṹ.
	//! �ռ�ָ�ڵ�Ϊ�˲����ڵ㣬�������ڵ��,Ŀǰֻ��ʵ�ְ˲���
	//! \param cellNodeInfo �ռ�ָ�ڵ�ṹָ��
	//! \note �˺�����ҪΪ�ڲ�ʹ�ã��ⲿ�������
	//! \sa ATOM_SpatialCell::NodeInfo getCellNodeInfo
	void setCellNodeInfo (ATOM_SpatialCell::NodeInfo *cellNodeInfo);

	//! ����һ����Ϸ��Ԫ���˽ڵ�.
	//! ��Ҫ���ڽ�����������Ⱦ�ĳ����ڵ����Ϸ�����������
	//! \param entity ��Ϸ����
	//! \sa ATOM_GameEntity getGameEntity
	void setGameEntity (ATOM_GameEntity *entity);

	//! ��ȡ���ӵ��˽ڵ����Ϸ��Ԫ.
	//! \return ��Ϸ����
	//! \sa ATOM_GameEntity setGameEntity
	ATOM_GameEntity *getGameEntity (void) const;

	//! װ�ش˽ڵ�.
	//! װ�ز���ͨ�������ڽڵ��ڲ������Ѿ����趨���֮��, һ����˵��Ҫ���ش˺�����Ӧ������onLoad����
	//! \param pDevice ��Ⱦ�豸
	//! \return true�ɹ� falseʧ��
	bool load(ATOM_RenderDevice *pDevice);

	//! ͨ���ļ�װ�ش˽ڵ�.
	//! װ�ز���ͨ�������ڽڵ��ڲ������Ѿ����趨���֮��, һ����˵��Ҫ���ش˺�����Ӧ������onLoad����
	//! \param pDevice ��Ⱦ�豸
	//! \param filename �ļ���
	//! \return true�ɹ� falseʧ��
	bool loadFromFile(ATOM_RenderDevice *pDevice, const char *filename, int loadPriority = ATOM_LoadPriority_IMMEDIATE);	// wangjian modified : �첽���� ��Ӽ������ȼ���Ĭ��Ϊ���첽��

	//! �첽װ�ش˽ڵ�.
	//! װ�ز���ͨ�������ڽڵ��ڲ������Ѿ����趨���֮��, һ����˵��Ҫ���ش˺���.
	//! \param pDevice ��Ⱦ�豸
	//! \param flags ����������
	//! \param callback �ص���������������ɺ󱻵��á����ΪNULL���ʹ��Ĭ�ϵĻص�
	//! \param userData �Զ������ݣ��ڻص���ʹ��
	//! \param group ����������
	//! \return true�ɹ� falseʧ��
	bool mtload(ATOM_RenderDevice *pDevice, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group);

	//! �첽ͨ���ļ�װ�ش˽ڵ�.
	//! װ�ز���ͨ�������ڽڵ��ڲ������Ѿ����趨���֮��, һ����˵��Ҫ���ش˺���.
	//! \param pDevice ��Ⱦ�豸
	//! \param filename �ļ���
	//! \param flags ����������
	//! \param callback �ص���������������ɺ󱻵��á����ΪNULL���ʹ��Ĭ�ϵĻص�
	//! \param userData �Զ������ݣ��ڻص���ʹ��
	//! \param group ����������
	//! \return true�ɹ� falseʧ��
	bool mtloadFromFile(ATOM_RenderDevice *pDevice, const char *filename, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group);

	//! �����Ƿ�������߳�����.
	//! \param enable �Ƿ�����
	void enableMTLoading (bool enable);

	//! ��ѯ�Ƿ�������߳�����.
	//! \return true��ʾ����false��ʾ������
	bool isMTLoadingEnabled (void) const;

public:
	//! ����һ��ATOM_Visitor�����Լ�.
	//! �ڵ����ı���������ATOM_Visitor���ģʽ
	//! \param visitor ATOM_Visitor����
	virtual void accept (ATOM_Visitor &visitor);

	//! �����Ϣ��log.���ڵ���
	//! \param indent ����
	virtual void dump(const char *indent);

	//! ��������
	//! \return ���ƵĽڵ㣬����ʧ�ܷ���NULL
	virtual ATOM_AUTOREF(ATOM_Node) clone (void) const;

	//! ��ѯ�Լ��Ƿ�֧�ֶ��߳�����.
	//! \return ֧�ַ���true�����򷵻�false
	virtual bool supportMTLoading (void);

	//! ��ȡ�ڵ����Լ��ֲ�����ϵ�еİ�Χ��.
	//! \return ��Χ��
	//! \sa getWorldBoundingbox buildBoundingbox buildWorldBoundingbox
	virtual const ATOM_BBox& getBoundingbox(void) const;

	//! ��ȡ�ڵ��ڵ�ǰ��������ϵ�µİ�Χ��.
	//! \return ��Χ��
	//! \sa getBoundingbox buildBoundingbox buildWorldBoundingbox
	virtual const ATOM_BBox& getWorldBoundingbox(void) const;

	//! ��ȡ�ڵ㵱ǰ������任����.
	//! �˱任���������и��ڵ�һֱ�����ڵ�ı任��������
	//! \param projectionMatrix ��ǰ��ͶӰ�任������������������任
	//! \param viewMatrix ��ǰ��������任������������������任
	//! \return ����任����
	//! \sa getInvWorldMatrix getO2T setO2T
	virtual const ATOM_Matrix4x4f & getWorldMatrix (void) const;

	//! ��ȡ�ڵ㵱ǰ������任����������.
	//! \param projectionMatrix ��ǰ��ͶӰ�任������������������任
	//! \param viewMatrix ��ǰ��������任������������������任
	//! \return ����任����������
	//! \sa getWorldMatrix getO2T setO2T
	virtual const ATOM_Matrix4x4f & getInvWorldMatrix (void) const;

	//! �����Ƿ��ֹ���μ���
	//! \param b true ��ֹ���μ��� false �����μ���
	virtual void skipClipTest (bool b);

	//! ��ѯ�Ƿ��ֹ���μ���
	//! \return true ��ֹ���μ��� false �����μ���
	virtual bool isSkipClipTest (void) const;

	//! �Ƿ�֧�̶ֹ�������Ⱦ
	//! \return true ֧�� false ��֧��
	virtual bool supportFixedFunction (void) const;

public:
	//! ���һ���ڵ�Ϊ�Լ����ӽڵ�.
	//! \param node Ҫ��ӵĽڵ�
	//! \return true��ʾ�ɹ���false��ʾʧ�ܣ�ʧ��һ������Ϊ����ڵ����Ϊ��ָ����ߴ���Ľڵ��Ѿ����Լ����ӽڵ���
	virtual bool appendChild(ATOM_Node *node);

	//! ɾ���Լ���һ���ӽڵ�.
	//! \param node Ҫɾ���Ľڵ�
	//! \return true��ʾ�ɹ���false��ʾʧ�ܣ�ʧ��һ������Ϊ����ڵ����Ϊ��ָ����ߴ���Ľڵ㲢�����Լ����ӽڵ�
	virtual bool removeChild(ATOM_Node *node);

	//! ��ȡ�Լ����ӽڵ�����.
	//! \return �ӽڵ�����
	virtual unsigned getNumChildren (void) const;

	//! ��ȡָ��������һ���ӽڵ�.
	//! \param index �ӽڵ�������ȡֵ��ΧΪ0��getNumChildren()-1
	virtual ATOM_Node *getChild (unsigned index) const;

	//! ɾ�������ӽڵ�.
	virtual void clearChildren(void);

	//! ��ѯһ���ڵ��Ƿ����Լ����ӽڵ�.
	//! \param node Ҫ��ѯ�Ľڵ�
	//! \return true��ʾ���Լ����ӽڵ㣬�������Լ����ӽڵ�
	virtual bool hasChild(ATOM_Node *node) const;

	//! ��ѯָ�����ƵĲο���
	//! \param name �ο��������
	//! \param matrix �������, ����òο���ı任����
	//! \return true����ָ���ο��� false������
	virtual bool queryReferencePoint (const char *name, ATOM_Matrix4x4f *matrix) const;

	//! ��ȡ���вο����б�
	virtual void getReferencePointList (ATOM_VECTOR<ATOM_STRING> &pointlist) const;

public:
	//! ��ȡ�Լ���Ը��ڵ�ı任����.
	//! \return ��Ը��ڵ�ı任����
	//! \sa setO2T getWorldMatrix getInvWorldMatrix
	virtual const ATOM_Matrix4x4f & getO2T(void) const;

	//! �����Լ���Ը��ڵ�ı任����.
	//! \param Mo2t ��Ը��ڵ�ı任����
	//! \sa getO2T getWorldMatrix getInvWorldMatrix
	virtual void setO2T(const ATOM_Matrix4x4f &Mo2t);

	//! ��ȡ����ӿ�
	//! \return ����ӿ�
	virtual ATOM_LoadInterface *getLoadInterface (void) const;

	//virtual ATOM_Vector3f getOrigin(void) const;
	//virtual ATOM_Vector3f this2Other(const ATOM_Vector3f &V) const;
	//virtual ATOM_Vector3f other2This(const ATOM_Vector3f &V) const;
	//virtual void setOrigin(const ATOM_Vector3f &V);
	//virtual void translate(const ATOM_Vector3f &V);
	//virtual void otherRotate(const ATOM_Vector3f &Axis, float fAngle);
	//virtual void otherRotate(const ATOM_Vector3f &Axis, float tSin, float tCos);	//�������CAMERA�ϣ����Գ���������ת��Ч��
	//virtual void rotateOther(const ATOM_Vector3f &Axis, float fAngle);
	//virtual void rotateOther(const ATOM_Vector3f &Axis, float tSin, float tCos);
	//virtual void rotateThis(const ATOM_Vector3f &Axis, float fAngle);
	//virtual void rotateThis(const ATOM_Vector3f &Axis, float tSin, float tCos);
	//virtual void lookAt(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz);
	//virtual void setTransform(const ATOM_Vector3f &Origin, const ATOM_Vector3f& Axis, float tSin, float tCos);
	//virtual void setTransform(const ATOM_Vector3f &Origin, const ATOM_Vector3f& Axis, float fAngle);

protected:
	//! �����Լ��İ�Χ��.
	//! ÿ�ֽڵ㶼�������ش˺���ִ�о���ļ���
	//! \sa getBoundingbox getWorldBoundingbox buildWorldBoundingbox
	virtual void buildBoundingbox (void) const;

	//! �����Լ�������ռ�İ�Χ��.
	//! ����Ѿ�ʵ����buildBoundingbox��Ҳ���Բ������ش˺������˺������Զ��任�ֲ�����ϵ��Χ�е���������ϵ
	//! \param projectionMatrix ��ǰ��ͶӰ����
	//! \param viewMatrix ��ǰ�����������
	//! \sa getBoundingbox getWorldBoundingbox buildBoundingbox
	virtual void buildWorldBoundingbox (void) const;

	//! �����Լ�������任����.
	//! һ�㲻��Ҫ����
	//! \param projectionMatrix ��ǰ��ͶӰ�任
	//! \param viewMatrix ��ǰ��������任
	//! \sa getWorldMatrix getInvWorldMatrix buildInvWorldMatrix
	virtual void buildWorldMatrix (void) const;

	//! �����Լ�������任����������.
	//! һ�㲻��Ҫ����
	//! \param projectionMatrix ��ǰ��ͶӰ�任
	//! \param viewMatrix ��ǰ��������任
	//! \sa getWorldMatrix getInvWorldMatrix buildWorldMatrix
	virtual void buildInvWorldMatrix (void) const;

	//! ���ڵ�ı任�������˱仯����ô˺���.
	//! һ�㲻��Ҫ����
	virtual void onTransformChanged (void);

	//! ִ�о��������.
	//! \param device ��Ⱦ�豸
	//! \return true�ɹ� falseʧ��
	//! \sa load
	virtual bool onLoad (ATOM_RenderDevice *device);

	//! ִ�о�����첽����.
	//! \param device ��Ⱦ�豸
	//! \param flags ����������
	//! \param callback �첽������ɺ��֪ͨ�ص�����
	//! \param userData �Զ������ݣ����ڻص�������ȡ��
	//! \param group ����������
	//! \return true�ɹ� falseʧ��
	//! \sa mtload
	virtual bool onMtLoad (ATOM_RenderDevice *device, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group);

	//! ���߳����������´�Ӳ�̶�ȡ��ʵ�֣��˲�����IO�߳�ִ��.
	//! \return �ɹ�����true�����򷵻�false
	virtual bool mtLoadFromDisk (ATOM_BaseResourceLoader *loader);

	//! ���߳����������������豸��ض��󣬴˲�����ͼ���̣߳�һ����˵�����̣߳�ִ��.
	//! \param device ��Ⱦ�豸
	//! \param loader loaderָ��
	//! \return �ɹ�����true�����򷵻�false
	virtual bool mtLock (ATOM_RenderDevice *device, ATOM_BaseResourceLoader *loader);

	//! ���߳����������������豸���ݣ��˲�����mtLockִ�гɹ��Ժ��ڵ����������߳���ִ��.
	//! \return �ɹ�����true�����򷵻�false
	virtual bool mtUploadDeviceData (ATOM_BaseResourceLoader *loader);

	//! ���߳����������½��������豸��ض��󣬴˲�����mtUploadDeviceDataִ�������ͼ���̣߳�һ����˵�����̣߳���ִ��.
	//! \param device ��Ⱦ�豸
	//! \param loader loader����ָ��
	//! \return �ɹ�����LOADINGSTAGE_FINISH��ʧ�ܷ���LOADINGSTAGE_NOTLOADED���ȴ�����LOADINGSTAGE_TRYAGAIN
	virtual ATOM_LoadingStage mtUnlock (ATOM_RenderDevice *device, ATOM_BaseResourceLoader *loader);

	//! �ȴ������������
	virtual void mtWait (void);

	//! ����
	//! \param other ����Ŀ��
	virtual void assign (ATOM_Node *other) const;

public:
	int getShow (void) const;
	int getPickable (void) const;

public:
	static bool registerNodeFileType (const char *classname, const char *fileext, const char *desc);
	static unsigned getNumRegisteredFileTypes (void);
	static void getRegisteredFileInfo (unsigned index, ATOM_STRING &fileext, ATOM_STRING &filedesc);
	//--- wangjian modified ---//
	// �첽���أ��������첽�������ȼ���Ĭ��Ϊ���첽��
	static ATOM_AUTOREF(ATOM_Node) loadNodeFromFile ( const char *filename, 
													  int loadPriority = ATOM_LoadPriority_IMMEDIATE,
													  int loadFlag = 0 );
	//-------------------------//

public:
	int getPickable2 (void) const;
	void setPickable (int pickable);
	void setPickableRecursive (int enable);
	int getShow2(void) const;
	void setShow(int show);
	void setShowRecursive (int show);
	int isContributeCullingEnabled (void) const;
	void enableContributeCulling (int b);
	void enableContributeCullingRecursive (int b);
	const ATOM_STRING &getLayer (void) const;
	void setLayer (const ATOM_STRING &layer);
	void ignoreFog (int ignore);
	void ignoreFogRecursive (int ignore);
	int isIgnoreFog (void) const;
	void setDrawBoundingbox (int bDraw);
	int getDrawBoundingbox (void) const;
	void setNoOccludedFrameStamp (unsigned frameStamp);
	unsigned getNoOccludedFrameStamp (void) const;
	void setRotation (const ATOM_Vector4f &quaterion);
	const ATOM_Vector4f &getRotation (void) const;
	void setTranslation (const ATOM_Vector3f &translation);
	const ATOM_Vector3f &getTranslation (void) const;
	void setScale (const ATOM_Vector3f &scale);
	const ATOM_Vector3f &getScale (void) const;
	const ATOM_Vector3f &getWorldTranslation (void) const;
	const ATOM_Vector3f &getWorldScale (void) const;
	const ATOM_Vector4f &getWorldRotation (void) const;
	void setNodeFileName (const ATOM_STRING &filename);
	const ATOM_STRING &getNodeFileName (void) const;
	void setTopMost (int bTopMost);
	int getTopMost (void) const;
	int getTopMost2 (void) const;
	void setAntiTopMost (int bAntiTopMost);
	int getAntiTopMost (void) const;
	int getAntiTopMost2 (void) const;
	void setProjectable (int bProjectable);
	int getProjectable (void) const;
	unsigned getVisibleStamp (void) const;
	void setVisibleStamp (unsigned value);
	void setPrivate (int bPrivate);
	int getPrivate (void) const;
	void getCombinedBoundingBox (ATOM_BBox &bbox) const;
	const ATOM_Vector3f &getPredefinedBBoxMin (void) const;
	void setPredefinedBBoxMin (const ATOM_Vector3f &v);
	const ATOM_Vector3f &getPredefinedBBoxMax (void) const;
	void setPredefinedBBoxMax (const ATOM_Vector3f &v);
	void enableGeometryClipping (bool enable);
	bool isGeometryClippingEnabled (void) const;


private:
	void updateDecomposition (void) const;
	void updateComposition (void);
	void updateWorldDecomposition (void) const;

protected:
	ATOM_Node *_parent;
	int _pickable;
	int _showMode;
	int _projectable;
	int _contributeCulling;
	int _ignoreFog;
	int _drawBoundingbox;
	int _topMost;
	int _antiTopMost;
	int _private;
	bool _noOccludedFrameStamp;
	bool _enableMTLoading;
	bool _skipClipTest;
	bool _enableGeoClipping;
	ATOM_BBox _predefinedBBox;

	mutable unsigned _changeStamp;
	mutable unsigned _worldMatrixStamp;
	mutable unsigned _iworldMatrixStamp;
	mutable unsigned _bboxStamp;
	mutable unsigned _wbboxStamp;

	mutable ATOM_Matrix4x4f _worldMatrix;
	mutable ATOM_Matrix4x4f _iworldMatrix;
	mutable ATOM_BBox _boundingBox;
	mutable ATOM_BBox _boundingBoxW;
	mutable bool _decomposeDirty;
	mutable ATOM_Matrix4x4f _rotation;
	mutable ATOM_Vector3f _translation;
	mutable ATOM_Vector3f _scale;
	mutable ATOM_Vector4f _quat;
	mutable ATOM_Vector3f _scaleW;
	mutable ATOM_Vector3f _translationW;
	mutable ATOM_Vector4f _rotationW;
	mutable bool _worldTransformDirty;
	mutable unsigned _visibleStamp;

	ATOM_Transformf _transform;

	typedef ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> NodeVector;
	typedef NodeVector::iterator NodeVectorIter;
	typedef NodeVector::const_iterator NodeVectorConstIter;
	NodeVector _children;

	ATOM_GameEntity *_entity;
	ATOM_STRING _layer;
	ATOM_STRING _describe;
	ATOM_STRING _filenameToBeLoad;

	ATOM_SpatialCell::NodeInfo *_cellNodeInfo;

	mutable ATOM_AUTOPTR(ATOM_LoadInterface) _loadInterface;

	//--- wangjian added ---//
	// �첽���أ���غ����ͳ�Ա
public:
	enum 
	{
		LOAD_NOTLOAD = 0,		// ������δ��ʼ
		LOAD_LOADSTARTED,		// �����Ѿ���ʼ
		LOAD_ATTRIBUTE_LOADED,	// �����ļ����ؽ׶�
		LOAD_LOADED,
		LOAD_ALLFINISHED,		// 

		LOAD_DELAYLOAD = 255,
	};

	void				setLoadFlag(unsigned flag);
	unsigned			getLoadFlag(void) const;					
	void				setLoadPriority(int pri);
	int					getLoadPriority (void) const;
	bool				checkNodeLoadStarted() const;
	bool				checkNodeLoadFinished();
	virtual bool		checkNodeAllFinshed();
	virtual void		onLoadFinished();

	// ���ò����������
	void				resetMaterialDirtyFlag();

	unsigned			_load_flag;
	int					_load_priority;

	// �ڵ������ļ��첽���ض���
	/*ATOM_AsyncableLoadAttributeFile					* getAttribFileLoader( const char* filename = 0 );
	ATOM_AUTOPTR(ATOM_AsyncableLoadAttributeFile)	_attribFileLoader;*/
	
protected:
	virtual void		resetMaterialDirtyFlag_impl();

	ATOM_AUTOREF(ATOM_NodeAttributeFile)	_attribFile;
	//----------------------//
};

#endif //__ATOM3D_ENGINE_NODE_H
/*! @} */
