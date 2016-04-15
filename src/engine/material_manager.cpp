#include "StdAfx.h"
#include "material_manager.h"
//--- wangjian added for profile shader compiling ： 测试性能用，不影响引擎---//
//#include "kernel/profiler.h"

//--------------------- wangjian added --------------------//
unsigned							ATOM_MaterialManager::g_matId = 0;
ATOM_HASHMAP<ATOM_STRING,unsigned>	ATOM_MaterialManager::g_matIdHashMap;
ATOM_VECTOR<ATOM_AUTOREF(ATOM_CoreMaterial)> g_coreMaterialUsed;
//---------------------------------------------------------//

//----------------------------------------------------------------------------//

ATOM_AUTOREF(ATOM_CoreMaterial) ATOM_MaterialManager::createCoreMaterial (void)
{
	return ATOM_CreateObject(ATOM_CoreMaterial::_classname(), 0);
}

ATOM_AUTOREF(ATOM_CoreMaterial) ATOM_MaterialManager::loadCoreMaterialFromString (ATOM_RenderDevice *device, const char *str)
{
	if (!str)
	{
		return 0;
	}

	ATOM_AUTOREF(ATOM_CoreMaterial) material = ATOM_CreateObject(ATOM_CoreMaterial::_classname(), 0);
	if (!material->loadString (device, str))
	{
		return 0;
	}

	return material;
}

ATOM_AUTOREF(ATOM_CoreMaterial) ATOM_MaterialManager::loadCoreMaterial (ATOM_RenderDevice *device, const char *materialFileName)
{
	//--- wangjian modified ---//
	if (!materialFileName || '\0' == materialFileName[0] )
	{
		return 0;
	}
	//-------------------------//

	//--- wangjian modified ---//
	ATOM_STRING coreMaterialName = ATOM_RenderSettings::getCoreMaterialName( materialFileName );
	if( coreMaterialName.empty() )
		return 0;

	char buffer[ATOM_VFS::max_filename_length] = {0};
	if (!ATOM_CompletePath (coreMaterialName.c_str(), buffer))
	{
		return 0;
	}
	_strlwr (buffer);
	//-------------------------//

	ATOM_AUTOREF(ATOM_CoreMaterial) material = ATOM_LookupObject (ATOM_CoreMaterial::_classname(), buffer);
	if (!material)
	{
		material = ATOM_CreateObject(ATOM_CoreMaterial::_classname(), buffer);

		//--- wangjian added for profile shader compiling ：测试性能用 不影响引擎 ---//
		//ATOM_Profiler profiler(materialFileName);
		//---------------------------------------------------------------------------//

		//--- wangjian modified ---//
		// 检查该材质资源是否是需要立即加载的
		/*if( ATOM_AsyncLoader::IsRun() && 
		false == ATOM_AsyncLoader::CheckImmediateResource( buffer ) )
		{
		material->getAsyncLoader()->SetAttribute( material.get() );
		material->getAsyncLoader()->Start( buffer,0 );
		}
		else*/
		{
			if (!material->loadXML (device, buffer))
			{
				return 0;
			}
		}

		//-------------------------------------//
		// wangjian added
		g_coreMaterialUsed.push_back(material);
		//-------------------------------------//
	}

	return material;
}

ATOM_AUTOPTR(ATOM_Material) ATOM_MaterialManager::createMaterialFromCore (ATOM_RenderDevice *device, const char *coreMaterialFileName)
{
	

	ATOM_AUTOREF(ATOM_CoreMaterial) coreMaterial = loadCoreMaterial (device, coreMaterialFileName);
	if (coreMaterial)
	{
		ATOM_AUTOPTR(ATOM_Material) m = ATOM_NEW(ATOM_Material);
		m->setCoreMaterial (coreMaterial.get());
		m->setCoreMaterialFileName (coreMaterialFileName);
		return m;
	}
	return 0;
}

ATOM_AUTOPTR(ATOM_Material) ATOM_MaterialManager::createMaterialFromCoreString (ATOM_RenderDevice *device, const char *str)
{
	ATOM_AUTOREF(ATOM_CoreMaterial) coreMaterial = loadCoreMaterialFromString (device, str);
	if (coreMaterial)
	{
		ATOM_AUTOPTR(ATOM_Material) m = ATOM_NEW(ATOM_Material);
		m->setCoreMaterial (coreMaterial.get());
		return m;
	}
	return 0;
}

ATOM_AUTOPTR(ATOM_Material) ATOM_MaterialManager::createMaterialFromFile (ATOM_RenderDevice *device, const char *filename)
{
	if (filename)
	{
		ATOM_AUTOPTR(ATOM_Material) m = ATOM_NEW(ATOM_Material);
		if (m->loadXML (device, filename))
		{
			return m;
		}
	}
	return 0;
}

//--- wangjian added ---//
unsigned ATOM_MaterialManager::appendMaterialId(const char * strMatId)
{
	unsigned id;

	ATOM_HASHMAP<ATOM_STRING, unsigned>::const_iterator iter = g_matIdHashMap.find(strMatId);
	if ( iter == g_matIdHashMap.end () )
	{
		id = g_matId;
		g_matIdHashMap[strMatId] = g_matId++;
	}
	else
	{
		id = iter->second;
	}

	return id;
}
//---------------------//

