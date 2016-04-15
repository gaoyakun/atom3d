#include "stdafx.h"
#include "ClientCoreSimpleCharacter.h"

TiXmlElement* LoadXmlFile(TiXmlDocument& doc, const char* pFileName)
{
	ATOM_STACK_TRACE(LoadXmlFile);

	ATOM_AutoFile file(pFileName, ATOM_VFS::text|ATOM_VFS::read);
	if(file == NULL)
	{
		ATOM_LOGGER::warning ("Create failed, open %s failed", pFileName);
		return FALSE;
	}
	int nLen = file->size();
	char* pBuffer = ATOM_NEW_ARRAY(char, nLen+1);
	assert(pBuffer);
	int nRead = file->read(pBuffer, nLen);
	pBuffer[nRead] = 0;
	doc.Parse(pBuffer);
	ATOM_DELETE_ARRAY(pBuffer);

	return doc.RootElement();
}


ClientCoreSimpleCharacterInfo::InfoVsl ClientCoreSimpleCharacterInfo::m_vslInfo;

// 取得模型信息
ClientCoreSimpleCharacterInfo* ClientCoreSimpleCharacterInfo::FindCharacterInfo(LPCTSTR pFileName)
{
	ATOM_STACK_TRACE(ClientCoreSimpleCharacterInfo::FindCharacterInfo);

	InfoVsl::iterator iter = m_vslInfo.find(pFileName);
	if(iter != m_vslInfo.end())
	{
		return &iter->second;
	}
	
	// 读取XML
	TiXmlDocument doc;
	TiXmlElement* pRoot = LoadXmlFile(doc, pFileName);
	if(NULL == pRoot)
	{
		return NULL;
	}

	// 基本信息
	const char* pValue;
	int nValue;
	ClientCoreSimpleCharacterInfo* pInfo = &m_vslInfo[pFileName];
	pValue =  pRoot->Attribute("skeleton");
	pInfo->m_sSkeleton = pValue ? pValue : "";
	pInfo->m_bMayaTrans = pRoot->Attribute("maya", &nValue) && nValue > 0;

	// 部件信息
	pInfo->m_arComponent.clear();
	CoreSimpleComponentInfo component;
	TiXmlElement* pElemComponent = pRoot->FirstChildElement("component");
	for(; pElemComponent; pElemComponent = pElemComponent->NextSiblingElement("component"))
	{
		component.m_nId = pElemComponent->Attribute("id", &nValue) ? nValue : 0;
		pValue = pElemComponent->Attribute("file");
		component.m_sMeshFile = pValue ? pValue : "";

		component.m_arMaterial.clear();
		TiXmlElement* pElemMaterial = pElemComponent->FirstChildElement("material");
		for(; pElemMaterial; pElemMaterial = pElemMaterial->NextSiblingElement("material"))
		{
			pValue = pElemMaterial->Attribute("file");
			component.m_arMaterial.push_back(pValue);					
		}

		ATOM_ASSERT(component.m_nId == pInfo->m_arComponent.size());
		pInfo->m_arComponent.push_back(component);
	}

	// 装备信息
	pInfo->m_arEquip.clear();
	CoreSimpleEquipInfo equip;
	TiXmlElement* pElemEquip = pRoot->FirstChildElement("equip");
	for(; pElemEquip; pElemEquip = pElemEquip->NextSiblingElement("equip"))
	{
		equip.m_nId = pElemEquip->Attribute("id", &nValue) ? nValue : 0;
		pValue = pElemEquip->Attribute("bone");
		equip.m_sBone = pValue ? pValue : "";
		pValue = pElemEquip->Attribute("file");
		equip.m_sModelFile = pValue;

		ATOM_ASSERT(equip.m_nId == pInfo->m_arEquip.size());
		pInfo->m_arEquip.push_back(equip);
	}

	// 动作信息
	pInfo->m_vslAnimation.clear();
	CoreSimpleAnimationInfo ani;
	TiXmlElement* pElemAnimation = pRoot->FirstChildElement("animation");
	for(; pElemAnimation; pElemAnimation = pElemAnimation->NextSiblingElement("animation"))
	{
		ani.m_nId = pElemAnimation->Attribute("id", &nValue) ? nValue : 0;
		pValue = pElemAnimation->Attribute("file");
		ani.m_sFile = pValue ? pValue : "";
		ani.m_nType = pElemAnimation->Attribute("type", &nValue) ? nValue : ANIMATIONTYPE_CYCLE;
		pInfo->m_vslAnimation[ani.m_nId] = ani;
	}

	return pInfo;
}
