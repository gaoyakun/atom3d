/********************************************************************
	created:	2008/12/30
	created:	30:12:2008   15:53
	filename: 	ClientCoreSimpleCharacter.h
	author:		Dashi Bai

	history:
*********************************************************************/
#pragma once

#include "basedefs.h"

//! ��������
enum ANIMATIONTYPE
{
	ANIMATIONTYPE_CYCLE,
	ANIMATIONTYPE_ONCE,
};

//! ������Ϣ
struct CoreSimpleComponentInfo
{
	int						m_nId;			//!< ID
	ATOM_STRING				m_sMeshFile;	//!< ģ���ļ�
	ATOM_VECTOR<ATOM_STRING>	m_arMaterial;	//!< ����
};

//! װ����Ϣ
struct CoreSimpleEquipInfo
{
	int						m_nId;			//!< ID
	ATOM_STRING				m_sBone;		//!< ��ͷ��
	ATOM_STRING				m_sModelFile;	//!< ģ����Ϣ
};

//! ������Ϣ
struct CoreSimpleAnimationInfo
{
	int						m_nId;
	ATOM_STRING				m_sFile;
	unsigned char			m_nType;
};

//! ��ɫ��Ϣ
class ATOM_ENGINE_API ClientCoreSimpleCharacterInfo
{
public:
	bool									m_bMayaTrans;	//!< �Ƿ�ΪMAYA��������ʱ��
	ATOM_STRING								m_sSkeleton;	//!< �����ļ�
	ATOM_VECTOR<CoreSimpleComponentInfo>		m_arComponent;	//!< ������Ϣ
	ATOM_VECTOR<CoreSimpleEquipInfo>			m_arEquip;		//!< װ����Ϣ
	ATOM_MAP<int, CoreSimpleAnimationInfo>	m_vslAnimation;	//!< ������Ϣ

	//! ȡ�ý�ɫ��Ϣ
	static ClientCoreSimpleCharacterInfo* FindCharacterInfo(LPCTSTR pFileName);

private:
	typedef ATOM_MAP<ATOM_STRING, ClientCoreSimpleCharacterInfo> InfoVsl;
	static InfoVsl m_vslInfo;
};


