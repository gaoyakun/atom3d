#ifndef __ATOM3D_ENGINE_STATESET_IO_H
#define __ATOM3D_ENGINE_STATESET_IO_H

#include <ATOM_render.h>

class ATOM_MaterialEffect;
class ATOM_MaterialPass;
class ATOM_MaterialTextureParam;

bool loadAlphaBlendStates (ATOM_RenderDevice *device, ATOM_AlphaBlendAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect);
bool loadColorWriteStates (ATOM_RenderDevice *device, ATOM_ColorWriteAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect);
bool loadDepthStates (ATOM_RenderDevice *device, ATOM_DepthAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect);
bool loadStencilStates (ATOM_RenderDevice *device, ATOM_StencilAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect);
bool loadRasterizerStates (ATOM_RenderDevice *device, ATOM_RasterizerAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect);
bool loadMultisampleStates (ATOM_RenderDevice *device, ATOM_MultisampleAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect);
bool loadSamplerStates (ATOM_RenderDevice *device, ATOM_SamplerAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect, ATOM_MaterialTextureParam **texParam);
bool loadAlphaTestStates (ATOM_RenderDevice *device, ATOM_AlphaTestAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect);

#endif // __ATOM3D_ENGINE_STATESET_IO_H
