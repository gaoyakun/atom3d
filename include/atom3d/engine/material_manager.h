#ifndef __ATOM3D_ENGINE_MATERIAL_MANAGER_H
#define __ATOM3D_ENGINE_MATERIAL_MANAGER_H

#include "../ATOM_render.h"
#include "basedefs.h"

class ATOM_Material;
class ATOM_ENGINE_API ATOM_MaterialManager
{
public:
	static ATOM_AUTOREF(ATOM_CoreMaterial) createCoreMaterial (void);
	static ATOM_AUTOREF(ATOM_CoreMaterial) loadCoreMaterial (ATOM_RenderDevice *device, const char *materialFileName);
	static ATOM_AUTOREF(ATOM_CoreMaterial) loadCoreMaterialFromString (ATOM_RenderDevice *device, const char *str);

	static ATOM_AUTOPTR(ATOM_Material) createMaterialFromCore (ATOM_RenderDevice *device, const char *coreMaterialFileName);
	static ATOM_AUTOPTR(ATOM_Material) createMaterialFromCoreString (ATOM_RenderDevice *device, const char *str);
	static ATOM_AUTOPTR(ATOM_Material) createMaterialFromFile (ATOM_RenderDevice *device, const char *filename);

	//--------------------- wangjian added --------------------//
public:
	static unsigned appendMaterialId(const char * strMatId);
private:
	static unsigned								g_matId;
	static ATOM_HASHMAP<ATOM_STRING,unsigned>	g_matIdHashMap;
	//---------------------------------------------------------//
};

#endif // __ATOM3D_ENGINE_MATERIAL_MANAGER_H
