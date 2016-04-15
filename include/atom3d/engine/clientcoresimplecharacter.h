/********************************************************************
	created:	2008/12/30
	created:	30:12:2008   15:53
	filename: 	ClientCoreSimpleCharacter.h
	author:		Dashi Bai

	history:
*********************************************************************/
#pragma once

#include "basedefs.h"

//! 动作类型
enum ANIMATIONTYPE
{
	ANIMATIONTYPE_CYCLE,
	ANIMATIONTYPE_ONCE,
};

//! 部件信息
struct CoreSimpleComponentInfo
{
	int						m_nId;			//!< ID
	ATOM_STRING				m_sMeshFile;	//!< 模型文件
	ATOM_VECTOR<ATOM_STRING>	m_arMaterial;	//!< 材质
};

//! 装备信息
struct CoreSimpleEquipInfo
{
	int						m_nId;			//!< ID
	ATOM_STRING				m_sBone;		//!< 骨头名
	ATOM_STRING				m_sModelFile;	//!< 模型信息
};

//! 动作信息
struct CoreSimpleAnimationInfo
{
	int						m_nId;
	ATOM_STRING				m_sFile;
	unsigned char			m_nType;
};

//! 角色信息
class ATOM_ENGINE_API ClientCoreSimpleCharacterInfo
{
public:
	bool									m_bMayaTrans;	//!< 是否为MAYA导出（暂时）
	ATOM_STRING								m_sSkeleton;	//!< 骨骼文件
	ATOM_VECTOR<CoreSimpleComponentInfo>		m_arComponent;	//!< 部件信息
	ATOM_VECTOR<CoreSimpleEquipInfo>			m_arEquip;		//!< 装备信息
	ATOM_MAP<int, CoreSimpleAnimationInfo>	m_vslAnimation;	//!< 动作信息

	//! 取得角色信息
	static ClientCoreSimpleCharacterInfo* FindCharacterInfo(LPCTSTR pFileName);

private:
	typedef ATOM_MAP<ATOM_STRING, ClientCoreSimpleCharacterInfo> InfoVsl;
	static InfoVsl m_vslInfo;
};


