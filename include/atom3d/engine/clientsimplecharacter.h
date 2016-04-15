/********************************************************************
	created:	2008/12/30
	created:	30:12:2008   15:52
	filename: 	ClientSimpleCharacter.h
	author:		Dashi Bai
	
	history:	
*********************************************************************/
#pragma once

#include "basedefs.h"

//! 多部件的模型类
class ATOM_ENGINE_API ClientSimpleCharacter : public ATOM_Geode
{
	ATOM_CLASS(engine, ClientSimpleCharacter, ClientSimpleCharacter)
	ATOM_DECLARE_SCRIPT_INTERFACE(ClientSimpleCharacter)

public:
	//! 构造函数
	ClientSimpleCharacter();
	//--- wangjian added ---//
	virtual ~ClientSimpleCharacter();
	//----------------------//

	//! \copydoc ATOM_Object::loadAttribute
	virtual bool loadAttribute(const TiXmlElement *xmlelement);

	//! \copydoc ATOM_Object::writeAttribute
	virtual bool writeAttribute(TiXmlElement *xmlelement);
	
	//! \copydoc ATOM_Node::accept
	virtual void accept (ATOM_Visitor &visitor);

	//! \copydoc ATOM_Node::onLoad
	virtual bool onLoad(ATOM_RenderDevice *device);

	//! \copydoc ATOM_Node::onMtLoad
	virtual bool onMtLoad (ATOM_RenderDevice *device, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group);

	//! \copydoc ATOM_Node::queryReferencePoint
	virtual bool queryReferencePoint (const char *name, ATOM_Matrix4x4f *matrix) const;

	//! \copydoc ATOM_Node::getReferencePointList
	virtual void getReferencePointList (ATOM_VECTOR<ATOM_STRING> &pointlist) const;

public:
	//! \copydoc ATOM_Node::getBoundingbox
	virtual const ATOM_BBox& getBoundingbox(void) const;

	//! \copydoc ATOM_Node::getWorldBoundingbox
	virtual const ATOM_BBox& getWorldBoundingbox (void) const;

	//! \copydoc ATOM_VisualNode::rayIntersectionTest
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;

	//! \copydoc ATOM_Node::removeChild
	virtual bool removeChild (ATOM_Node *node);

public:
	ATOM_Node* attach (const char *attachPoint, const char *filename);

	virtual void detach (ATOM_Node *node);

	int getEquipCount() const;

	const char* getEquipFilename(int index) const;

	const char* getEquipAttachPoint(int index) const;

	ATOM_Node* getEquipNode(int index);

	void setAction(const ATOM_STRING& name);

	const ATOM_STRING& getAction() const;

	ATOM_ColorARGB getBindLightColor (void) const;
	const ATOM_Vector3f &getBindLightDir (void) const;
	ATOM_ColorARGB getBindAmbientColor (void) const;
	float getBindFOV (void) const;
	float getBindLightIntensity (void) const;

protected:
	virtual void assign (ATOM_Node *other) const;

protected:
	void loadCompnent();

protected:
	// 绑定装备数据
	struct EquipInfo
	{
		int					_skeletonIndex;
		ATOM_STRING			_attachPoint;
		ATOM_STRING			_filename;
		ATOM_AUTOREF(ATOM_Node)	_node;
	};

	struct AvatarInfo
	{
		int	meshId;
		ATOM_STRING materialFileName;
	};

protected:
	ATOM_VECTOR<EquipInfo>	_attachInfoList;
	ATOM_VECTOR<AvatarInfo> _avatarInfoList;
	ATOM_STRING				_actionName;
	ATOM_Vector3f			_bindLightDir;
	ATOM_ColorARGB			_bindLightColor;
	ATOM_ColorARGB			_bindAmbient;
	float					_bindFOV;
	float					_bindLightIntensity;
	ATOM_HASHMAP<ATOM_STRING, ATOM_Matrix4x4f> _referencePointMap;

	ATOM_DECLARE_NODE_FILE_TYPE(ClientSimpleCharacter, ClientSimpleCharacter, "csp", "Fengshen Client Simple Character")

	//--- wangjian added ---//
	// 异步加载
public:
	virtual void onLoadFinished();
private:
	struct sMaterialDependInfo
	{
		int							_meshid;
		ATOM_AUTOPTR(ATOM_Material) _material_dependent;
	};
	ATOM_VECTOR<sMaterialDependInfo> _materialDependInfoList;

	// 直接加载接口
	bool loadSync();

	// wangjian added : 固定特效
	struct sFixedEffect
	{
		ATOM_STRING	_effect;
		int			_noRotate;
	};
	struct sFixedEffectContainer
	{
		ATOM_VECTOR<sFixedEffect>			_effects;
	};
	ATOM_MAP<ATOM_STRING,sFixedEffectContainer>			_fixedEffectList;
	void processFixedEffects();
	//----------------------//

};

