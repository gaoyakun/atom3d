/**	\file engine/actor.h
 *	��ɫ�ڵ����.
 *
 *	\author ������
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_ACTOR_H
#define __ATOM3D_ENGINE_ACTOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "../ATOM_kernel.h"
#include "../ATOM_render.h"

#include "basedefs.h"
#include "components.h"

class ATOM_Actor;

//! \class ATOM_ActorPart
//! ��������.
//! ��ɫ�еĲ������ͻ���
//! \author ������
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorPart: public ATOM_Object
{
public:
	//! ��ѡ����������
	typedef int CandidateId;
	static const int invalid_Id;
	static const int any_Id;

private:
	struct CandidateInfo
	{
		ATOM_STRING desc;
		CandidateId id;
	};

public:
	//! ���캯��
	ATOM_ActorPart (void);

	//! ��������
	virtual ~ATOM_ActorPart (void);

	//! ִ��׼������.
	//! �˺���Ϊ�ڲ�ʹ�ã���Ⱦǰ��ɫ�е�ÿ����������ô˺���
	virtual void apply (void) = 0;

	//! ���ô˲�������ɫ.
	//! \param color ��ɫ
	//! \param property ��������
	virtual void setColor (const ATOM_Vector4f &color, const char *property) = 0;

	//! ���ô˲�������ͼ.
	//! \param albedo ��ɫ��ͼ
	//! \param normalmap ������ͼ
	//! \param property ��������
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property) = 0;

	//! ͬ���˲���.
	virtual void sync (void) = 0;

	//! ͨ��XML�ļ�����
	//! ��������˲��������к�ѡ�б�(������ѡ���ļ���������ֵ)�Լ���ǰ�ĺ�ѡ
	//! \param xmlElement XmlElement
	//! \return true �ɹ� false ʧ��
	virtual bool loadFromXml (ATOM_TiXmlElement *xmlElement);

	//! д��XML�ļ�
	//! ����д��˲��������к�ѡ�б�(������ѡ���ļ���������ֵ)�Լ���ǰ�ĺ�ѡ
	//! \param xmlElement XmlElement
	virtual void writeToXml (ATOM_TiXmlElement *xmlElement);

public:
	//! ���õ�ǰ��������
	//! \param id �������������Ϊinvalid_Id��������ǰ��ѡ�����Ϊany_Id����Ե�ǰ��ѡ����Ӱ�죬�������õ�ǰ����ֵ
	//! \return true �ɹ� false ʧ�ܣ�ͨ������Ϊ��������Ч������ֵ
	bool setCurrentCandidate (CandidateId id);

	//! ����һ����ѡ��
	//! \param desc ��ѡ������
	//! \return ���ӵĲ������������ʧ�ܷ���invalid_Id
	CandidateId addCandidate (const char *desc);

	//! ����һ����ѡ�ߣ��ֹ���������
	//! \param id ��ѡ������
	//! \param desc ��ѡ������
	//! \return ���ӵĲ�������������ɹ����ص�ֵ�Ͳ���id��ͬ�����ʧ�ܷ���invalid_Id
	CandidateId addCandidate (CandidateId id, const char *desc);

	//! ɾ��һ����ѡ��
	//! \param id ��ѡ������
	//! \return true �ɹ� false �����ڶ�Ӧid�ĺ�ѡ��
	bool removeCandidate (CandidateId id);

	//! ɾ�����к�ѡ��
	void clearCandidates (void);

	//! ��ȡ��ѡ������
	//! \return ��ѡ������
	unsigned getNumCandidates (void) const;

	//! ��ȡĳ����ѡ�ߵ�����
	//! \param index ��ѡ�������������μ�getNumCandidates
	//! \return ��ѡ��������ʧ�ܷ���invalid_Id
	CandidateId getCandidateId (unsigned index) const;

	//! ����ĳ����ѡ�ߵ�����
	//! \param index ��ѡ�������������μ�getNumCandidates
	//! \param id �µ�����
	//! \return true �ɹ� false indexԽ����µ�id�Ѿ���ʹ����
	bool setCandidateId (unsigned index, CandidateId id);

	//! ��ȡĳ����ѡ�ߵ���������
	//! \param id �ú�ѡ�ߵ�Id
	//! \return �ú�ѡ�ߵ���������
	int getCandidateIndexById (CandidateId id) const;

	//! ��ȡ��ǰ��ѡ������
	//! \return ��ǰ��ѡ�������������ǰ��ѡ��δ�����򷵻�invalid_Id
	CandidateId getCurrentCandidate (void) const;

	//! ��ȡĳ����ѡ�ߵ�����
	//! \param id �ú�ѡ�ߵ�����
	//! \return �ú�ѡ�ߵ��ļ���
	const char *getCandidateDesc (CandidateId id) const;

	//! ����ĳ����ѡ�ߵ�����
	//! \param id ��ѡ������
	//! \param desc �µ�������
	void setCandidateDesc (CandidateId id, const char *desc);

	//! ��������
	//! \param name Ҫ���õ�����
	bool setName (const char *name);

	//! ��ȡ����
	//! \return ������������
	const char *getName (void) const;

	//! ����������ɫ
	//! \param actor ��ɫʵ��
	void setActor (ATOM_Actor *actor);

	//! ��ȡ������ɫ
	//! \return ������ɫʵ��
	ATOM_Actor *getActor (void) const;

	//! �����Զ�������
	//! \param userdata �Զ�������
	void setUserData (void *userData);

	//! ��ȡ�Զ�������
	//! \return �Զ�������
	void *getUserData (void) const;

protected:
	//! ����ѡ�߸ı�ʱ����
	//! ������Ӧ���ش˷�����ִ�о����������Դ����
	//! \param oldId �ɵĺ�ѡ������
	//! \param newId �µĺ�ѡ������
	//! \return true �ɹ� false ʧ��
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId) = 0;

private:
	CandidateId generateId (void) const;

private:
	ATOM_VECTOR<CandidateInfo> _candidates;
	ATOM_STRING _name;
	ATOM_Actor *_actor;
	CandidateId _currentCandidate;
	void *_userData;
};

//! \class ATOM_ActorModifierPart
//! �޸�����������.
//! ���ֲ�����������������ʵ�岿���ģ�����ɫ����ͼ�����ʣ����α任�ȵ�
//! \author ������
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorModifierPart: public ATOM_ActorPart
{
public:
	//! \copydoc ATOM_ActorPart::loadFromXml
	virtual bool loadFromXml (ATOM_TiXmlElement *xmlElement);

	//! \copydoc ATOM_ActorPart::writeToXml
	virtual void writeToXml (ATOM_TiXmlElement *xmlElement);

	//! \copydoc ATOM_ActorPart::sync
	virtual void sync (void);

public:
	//! ������Ӱ��Ĳ���
	//! \param partName ��Ӱ��Ĳ�����
	//! \return true �ɹ� false ���partNameΪ�ջ����Ѵ�����ͬ���ƵĲ�����ʧ��
	bool addPart (const char *partName);

	//! ��Ӱ�첿���б���ɾ��
	//! \param partName Ҫɾ���Ĳ�����
	void removePart (const char *partName);

	//! ���Ӱ�첿���б�
	void clearAllParts (void);

	//! ��ȡӰ�첿���б�������
	//! \return ����
	unsigned getNumParts (void) const;

	//! ��ȡĳ��Ӱ�첿������
	//! \param index Ӱ�첿���б�������
	//! \return ��������
	const char *getPart (unsigned index) const;

	//! ������������.
	//! ֻ�и����Ա����õ�����Ż��ܵ����޸���Ӱ��
	//! \param property ��������
	void setMeshProperty (const char *property);

	//! ��ȡ��������
	const char *getMeshProperty (void) const;

private:
	int getPartIndex (const char *name) const;

protected:
	ATOM_VECTOR<ATOM_STRING> _affectParts;
	ATOM_VECTOR<ATOM_ActorPart*> _parts;
	ATOM_STRING _meshProperty;
};

//! \class ATOM_ActorComponentsPart
//! Mesh��������������.
//! \author ������
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorComponentsPart: public ATOM_ActorPart
{
	ATOM_CLASS(engine, ATOM_ActorComponentsPart, ATOM_ActorComponentsPart)

public:
	//! ��������
	virtual ~ATOM_ActorComponentsPart (void);

	//! \copydoc ATOM_ActorPart::apply
	virtual void apply (void);

	//! \copydoc ATOM_ActorPart::sync
	virtual void sync (void);

	//! \copydoc ATOM_ActorPart::setColor
	virtual void setColor (const ATOM_Vector4f &color, const char *property);

	//! \copydoc ATOM_ActorPart::setTextures
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property);

public:
	//! ��ȡ��ǰ�Ĳ���ʵ��
	//! \return ��ǰ����ʵ��
	ATOM_Components getCurrentComponents (void) const;

	//! ���õ�ǰ����ʵ��
	//! �˷���ֱ�����ò���ʵ����������ͨ����ѡ����
	//! \param part ����ʵ���������ΪNULL��Ὣ��ǰ������������Ϊany_Id����������Ϊinvalid_Id
	void setCurrentComponents (ATOM_Components part);

protected:
	//! \copydoc ATOM_ActorPart::onCandidateChange
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId);

private:
	ATOM_Components _currentComponents;
};

//! \class ATOM_ActorColorPart
//! ��ɫ��������.
//! ���ڿɻ�ɫ�Ĳ���
//! \author ������
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorColorPart: public ATOM_ActorModifierPart
{
	ATOM_CLASS(engine, ATOM_ActorColorPart, ATOM_ActorColorPart)

public:
	//! ���캯��
	ATOM_ActorColorPart (void);

	//! ��������
	virtual ~ATOM_ActorColorPart (void);

public:
	// \copydoc ATOM_ActorPart::apply
	virtual void apply (void);

	// \copydoc ATOM_ActorPart::setColor
	virtual void setColor (const ATOM_Vector4f &color, const char *property);

	// \copydoc ATOM_ActorPart::setTextures
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property);

public:
	//! ��ȡĳ����ѡ��ɫ
	//! \param id ��ѡ������
	//! \return ��ɫ
	ATOM_Vector4f getColorById (ATOM_ActorPart::CandidateId id) const;

	//! ��ȡ��ǰ��ɫֵ
	//! \return ��ǰ��ɫֵ
	const ATOM_Vector4f & getCurrentColor (void) const;

	//! ���õ�ǰ��ɫֵ
	//! �˷���ֱ��������ɫֵ��������ͨ����ѡ��������ǰ�������ᱻ����Ϊany_Id
	//! \param color ��ɫֵ
	void setCurrentColor (const ATOM_Vector4f &color);

protected:
	//! \copydoc ATOM_ActorPart::onCandidateChange
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId);

private:
	ATOM_Vector4f _currentColor;
};

//! \class ATOM_ActorTexturePart
//! ��ͼ��������.
//! ���ڿɻ���ͼ�Ĳ���
//! \author ������
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorTexturePart: public ATOM_ActorModifierPart
{
	ATOM_CLASS(engine, ATOM_ActorTexturePart, ATOM_ActorTexturePart)

public:
	//! ���캯��
	ATOM_ActorTexturePart (void);

	//! ��������
	virtual ~ATOM_ActorTexturePart (void);

public:
	//! \copydoc ATOM_ActorPart::apply
	virtual void apply (void);

	//! \copydoc ATOM_ActorPart::setColor
	virtual void setColor (const ATOM_Vector4f &color, const char *property);

	//! \copydoc ATOM_ActorPart::setTextures
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property);

public:
	//! ��ȡĳ����ѡ�ߵ���ɫ��ͼ�ļ���
	//! \param id ��ѡ������
	//! \return ��ɫ��ͼ�ļ���
	ATOM_STRING getAlbedoFileNameById (ATOM_ActorPart::CandidateId id) const;

	//! ��ȡĳ����ѡ�ߵ���ɫ������ͼ�ļ���
	//! \param id ��ѡ������
	//! \return ������ͼ�ļ���
	ATOM_STRING getNormalMapFileNameById (ATOM_ActorPart::CandidateId id) const;

	//! ��ȡ��ǰ��ɫ��ͼ
	//! \return ��ɫ��ͼָ��
	ATOM_Texture *getCurrentAlbedo (void) const;

	//! ��ȡ��ǰ������ͼ
	//! \return ��ǰ������ͼָ��
	ATOM_Texture *getCurrentNormalMap (void) const;

	//! ���õ�ǰ��ͼ
	//! �˷���ֱ�����õ�ǰ��ͼ��������ʹ�ú�ѡ����������ǰ��ѡ���������ᱻ����Ϊany_Id
	//! \param albedo ��ɫ��ͼ
	//! \param normalmap ������ͼ
	void setCurrentTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap);

protected:
	//! \copydoc ATOM_ActorPart::onCandidateChange
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId);

private:
	ATOM_AUTOREF(ATOM_Texture) _currentAlbedo;
	ATOM_AUTOREF(ATOM_Texture) _currentNormalMap;
};

//! \class ATOM_ActorTransformPart
//! ����任��������
//! \author ������
//! \ingroup engine
class ATOM_ENGINE_API ATOM_ActorTransformPart: public ATOM_ActorPart
{
	ATOM_CLASS(engine, ATOM_ActorTransformPart, ATOM_ActorTransformPart)

public:
	//! ���캯��
	ATOM_ActorTransformPart (void);

	//! ��������
	virtual ~ATOM_ActorTransformPart (void);

public:
	//! \copydoc ATOM_ActorPart::apply
	virtual void apply (void);

	//! \copydoc ATOM_ActorPart::sync
	virtual void sync (void);

	//! \copydoc ATOM_ActorPart::setColor
	virtual void setColor (const ATOM_Vector4f &color, const char *property);

	//! \copydoc ATOM_ActorPart::setTextures
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property);

public:
	//! ���ñ任����
	//! �˷���ֱ�����ñ任���󣬶�����ͨ����ѡ��������ǰ��ѡ���������ᱻ����Ϊany_Id
	//! \param matrix �任����
	void setCurrentTransform (const ATOM_Matrix4x4f &matrix);

	//! ��ȡ��ǰ�任����
	//! \return ��ǰ�任����
	const ATOM_Matrix4x4f &getCurrentTransform (void) const;

protected:
	//! \copydoc ATOM_ActorPart::onCandidateChange
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId);

private:
	bool parseDesc (const char *desc, ATOM_Matrix4x4f &matrix) const;

private:
	ATOM_Matrix4x4f _matrix;
};

//! \class ATOM_ActorBindingPart
//! �����󶨲�������.
//! \author ������
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorBindingPart: public ATOM_ActorPart
{
	ATOM_CLASS(engine, ATOM_ActorBindingPart, ATOM_ActorBindingPart)

public:
	struct BindingInfo
	{
		ATOM_STRING nodeFileName;
		ATOM_STRING boneName;
		ATOM_Matrix4x4f bindMatrix;
	};

public:
	//! ���캯��
	ATOM_ActorBindingPart (void);

	//! ��������
	virtual ~ATOM_ActorBindingPart (void);

public:
	//! \copydoc ATOM_ActorPart::apply
	virtual void apply (void);

	//! \copydoc ATOM_ActorPart::sync
	virtual void sync (void);

	//! \copydoc ATOM_ActorPart::setColor
	virtual void setColor (const ATOM_Vector4f &color, const char *property);

	//! \copydoc ATOM_ActorPart::setTextures
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property);

public:
	//! ���ð󶨹�����
	//! \param boneName �󶨹�����
	void setBoneName (const char *boneName);

	//! ��ȡ�󶨹�����
	//! \return �󶨹�����
	const char *getBoneName (void) const;

	//! ���ð󶨽ڵ�.
	//! �˷���ֱ�����ð󶨽ڵ㣬������ͨ����ѡ����
	//! \param node �ڵ�ָ�룬���ΪNULL��ǰ��ѡ�������ᱻ���ó�invalid_Id����������Ϊany_Id
	void setCurrentNode (ATOM_Node *node);

	//! ��ȡ��ǰ�󶨽ڵ�
	//! \return ��ǰ�󶨽ڵ�
	ATOM_Node *getCurrentNode (void) const;

	//! ���ð󶨱任
	//! �˷���ֱ�����ð󶨽ڵ�任��������ͨ����ѡ����
	//! \param matrix �任����
	void setCurrentBindMatrix (const ATOM_Matrix4x4f &matrix);

	//! ��ȡ��ǰ�󶨱任
	//! \return ��ǰ�󶨱任
	const ATOM_Matrix4x4f &getCurrentBindMatrix (void) const;

	//! ��ȡָ��Id�ĺ�ѡ�ߵİ���Ϣ
	//! \param id ��ѡ��Id
	//! \param info ����ɹ����������Ϣ�ṹ�ᱻ���
	//! \return true �ɹ� false ʧ��
	bool getBindingInfoById (CandidateId id, BindingInfo &info) const;

protected:
	//! \copydoc ATOM_ActorPart::onCandidateChange
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId);

private:
	bool parseDesc (const char *desc, ATOM_STRING &filename, ATOM_STRING &boneName, ATOM_Matrix4x4f &matrix) const;

private:
	ATOM_STRING _boneName;
	ATOM_Matrix4x4f _bindMatrix;
	ATOM_AUTOREF(ATOM_Node) _currentBinding;
};

//! \class ATOM_ActorPartDefine
//! ��ɫ�Ĳ���ϵͳ����
//! ���������һϵ�еĲ�������
//! \author ������
//! \ingroup engine
class ATOM_ENGINE_API ATOM_ActorPartDefine
{
public:
	//! ���캯��
	ATOM_ActorPartDefine (ATOM_Actor *actor);

	//! ��������
	~ATOM_ActorPartDefine (void);

public:
	//! ͨ��xml��ȡ
	//! \param xmlElement XmlElement
	//! \return true �ɹ� false ʧ��
	bool loadFromXml (ATOM_TiXmlElement *xmlElement);

	//! д�뵽xml
	//! \param xmlElement XmlElement
	void writeToXml (ATOM_TiXmlElement *xmlElement);

public:
	//! �����µ�mesh��������
	//! \param name ������������
	//! \return �ɹ����ز���ָ�룬����������ѱ�ʹ�÷���NULL
	ATOM_ActorComponentsPart *newComponentsPart (const char *name);

#if 0
	//! �����µ���ɫ��������
	//! \param name ������������
	//! \return �ɹ����ز���ָ�룬����������ѱ�ʹ�÷���NULL
	ATOM_ActorColorPart *newColorPart (const char *name);

	//! �����µ���ͼ��������
	//! \param name ������������
	//! \return �ɹ����ز���ָ�룬����������ѱ�ʹ�÷���NULL
	ATOM_ActorTexturePart *newTexturePart (const char *name);

	//! �����µĹ����󶨲�������
	//! \param name ������������
	//! \return �ɹ����ز���ָ�룬����������ѱ�ʹ�÷���NULL
	ATOM_ActorBindingPart *newBindingPart (const char *name);
#endif

	//! ���һ����������
	//! \param part Ҫ��ӵĲ�������
	//! \return true �ɹ� false ���partΪ�ջ��Ѿ�������ͬ���Ƶ�part��ʧ��
	bool addPart (ATOM_ActorPart *part);

	//! ɾ��һ����������
	//! \param name Ҫɾ��������������
	void removePartByName (const char *name);

	//! ɾ��һ����������
	//! \param part Ҫɾ����������ָ��
	void removePart (ATOM_ActorPart *part);

	//! ɾ�����в�������
	void removeAllParts (void);

	//! ͨ�����Ʋ���һ����������ָ��
	//! \param name Ҫ���ҵĲ�����������
	ATOM_ActorPart *getPartByName (const char *name) const;

	//! ��ȡ�����в������͵�����
	//! \return ����
	unsigned getNumParts (void) const;

	//! ��ȡĳ���������͵�����
	//! \param index ���������б��������μ�getNumParts
	//! \return �����������ƣ����index��Ч����NULL
	const char *getPartName (unsigned index) const;

	//! ��ȡĳ���������͵�ָ��
	//! \param index �����б��������μ�getNumParts
	//! \return ��������ָ�룬���index��Ч����NULL
	ATOM_ActorPart *getPart (unsigned index) const;

private:
	ATOM_Actor *_actor;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_ActorPart)> _parts;
};

//! \class ATOM_Actor
//! ��ɫ�ڵ����.
//! ������Ϸ�еĽ�ɫ��֧�ָ��������������󶨺Ͷ���.�������ʹ��ATOM_Actor����л���
//! \code
//!		Actor->setPart ("head", 34); // ʹ��34��ͷ��ģ��
//!		Actor->setPart ("bodycolor", 12);	// ʹ��12��������ɫ
//!	\endcode
//! \author ������
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_Actor: public ATOM_VisualNode
{
public:
	//! ���캯��
	ATOM_Actor (void);

	//! ��������
	virtual ~ATOM_Actor (void);

public:
	//! \copydoc ATOM_Node::accept
	virtual void accept (ATOM_Visitor &visitor);

	//! \copydoc ATOM_VisualNode::rayIntersectionTest
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;

	//! \copydoc ATOM_Node::supportMTLoading
	virtual bool supportMTLoading (void);

	//! \copydoc ATOM_Node::getBoundingbox
	virtual const ATOM_BBox& getBoundingbox(void) const;

	//! \copydoc ATOM_Node::getWorldBoundingbox
	virtual const ATOM_BBox& getWorldBoundingbox (void) const;

protected:
	//! \copydoc ATOM_Node::buildBoundingbox
	virtual void buildBoundingbox (void) const;

	//! \copydoc ATOM_Node::onLoad
	virtual bool onLoad(ATOM_RenderDevice *device);

	//! \copydoc ATOM_Node::assign
	virtual void assign(ATOM_Node *other) const;

public:
	//! \copydoc ATOM_VisualNode::setupRenderQueue
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);

	//! \copydoc ATOM_VisualNode::skipClipTest
	virtual void skipClipTest (bool b);

	//! \copydoc ATOM_VisualNode::isSkipClipTest
	virtual bool isSkipClipTest (void) const;

public:
	//! ��ȡ����ϵͳ������ʵ��
	//! \return ����ϵͳ������ʵ��
	ATOM_ActorPartDefine & getPartDefine (void) const;

	//! ����ĳ���������͵ĵ�ǰ��ѡ������
	//! \param partName ������������
	//! \param id ��ѡ������
	//! \return ����֮ǰ�ĵ�ǰ��ѡ������
	ATOM_ActorPart::CandidateId setPart (const char *partName, ATOM_ActorPart::CandidateId id);

	//! ��ȡĳ���������͵ĵ�ǰ��ѡ������
	//! \param partName ������������
	//! \return ��ѡ������
	ATOM_ActorPart::CandidateId getPart (const char *partName) const;

	//! ���Ŷ���
	//! Ҫ����ȷ���Ŷ�����������Ӷ�������
	//! \param actionName ������
	//! \param flags ����
	//! \param loopCount ѭ��������Ϊ0����ʼ��ѭ��
	//! \param forceReset ���Ϊtrue�Ļ���������ͬ�Ķ�������ǿ�ƴ�ͷ����
	//! \param fadeTime �����ںϽ����ĺ�����
	//! \param speed ���Ŵ˶��������ʣ����Ϊ0�򲻸��ĵ�ǰ���ʣ�ʹ��Ĭ�ϵ��ٶ�ֵ
	//! \return ��������
	bool doAction (const char *actionName, unsigned flags, unsigned loopCount = 0, bool forceReset = false, unsigned fadeTime = 300, float speed = 0.f);

	//! �趨������Ĭ�ϲ�������.
	//! �˷��������趨�����doAction����ʱ�����õ�Ĭ������(doAction������speed����Ϊ0.0)
	//! \param flags ����
	//! \param speed ����, 1.0Ϊ��������
	void setDefaultActionSpeed (unsigned flags, float speed);

	//! �趨��ǰ�����Ĳ�������.
	//! �˷��������趨��ǰ���ڲ��ŵĶ������ʣ�����Ӱ��Ĭ�ϲ�������
	//! \param flags ����
	//! \param speed ����, 1.0Ϊ��������
	void setCurrentActionSpeed (unsigned flags, float speed);

	//! ��ǰ�������Ž��ȹ���
	void resetCurrentAction (unsigned flags);

	//! ��ȡ�ڲ�Geode�ڵ�
	//! \return �ڵ�ָ��
	ATOM_Geode *getInternalGeode (void) const;

	//! ͬ������״̬.
	//! �˺����ڲ�ʹ��
	void syncParts (void);

	//! ֪ͨ��ɫʵ��������Ҫ��ͬ��
	void notifyPartChange (void);

	//! ���浽�ļ�.
	//! \param filename �ļ���
	//! \return true �ɹ� false ʧ��
	bool saveToFile (const char *filename) const;

	//! ���浽XML�ĵ�
	//! \param doc XML�ĵ�
	void saveToXML (ATOM_TiXmlDocument &doc) const;

public:
	//! ��������ֵ
	//! \param name ��������
	//! \param value ����ֵ
	void setProperty (const char *name, const ATOM_Variant &value);

	//! ��ѯ����ֵ
	//! \param name ��������
	//! \return ����ֵ�����δ�ҵ�ָ�����Ƶ����ԣ��򷵻�NONE
	const ATOM_Variant &getProperty (const char *name);

	//! ��ȡ��������.
	//! \return ���Եĸ���
	unsigned getNumProperties (void) const;

	//! ��ȡ��������
	//! \param index ���Ե�����
	//! \return ���Ե����ƣ����������Ч����NULL�����ص��ַ���ָ�벢���ܱ�֤������Ч�������Ա����仯(��������ɾ������)�����ַ������ܱ��ͷ�
	const char *getPropertyName (unsigned index) const;

	//! ��ȡ����ֵ
	//! \param index ��������
	//! \return ����ֵ�����������Ч����NONE
	const ATOM_Variant &getPropertyValue (unsigned index) const;

	//! ɾ����������
	void removeAllProperties (void);

public:
	void setAction (const ATOM_STRING &actionName);
	const ATOM_STRING &getAction (void) const;

private:
	bool loadProperties (ATOM_TiXmlElement *element);
	void writeProperties (ATOM_TiXmlElement *element) const;

private:
	ATOM_STRING _action;
	bool _partChange;
	ATOM_AUTOREF(ATOM_Geode) _geode;
	ATOM_ActorPartDefine *_partDefine;
	ATOM_HASHMAP<ATOM_STRING, ATOM_Variant> _properties;

	ATOM_CLASS(engine, ATOM_Actor, ATOM_Actor)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Actor)
	ATOM_DECLARE_NODE_FILE_TYPE(ATOM_Actor, ATOM_Actor, "act", "ATOM3D actor")

	//=========================================================================
	// wangjian added
public:
	virtual void onLoadFinished();
private:
	// ֱ�Ӽ��ؽӿ�
	bool loadSync();
	//=========================================================================
};

#endif // __ATOM3D_ENGINE_ACTOR_H
/*! @} */
