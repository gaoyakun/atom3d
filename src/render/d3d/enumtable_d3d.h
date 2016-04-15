#ifndef __ATOM3D_RENDER_ENUMTABLE_D3D9_H
#define __ATOM3D_RENDER_ENUMTABLE_D3D9_H

#include <d3d9.h>
#include "stateset.h"

extern DWORD primitiveTypeLookupTable[ATOM_RenderAttributes::PrimitiveType_Count];
extern DWORD shadeModeLookupTable[ATOM_RenderAttributes::ShadeMode_Count];
extern DWORD blendFuncLookupTable[ATOM_RenderAttributes::BlendFunc_Count];
extern DWORD blendOpLookupTable[ATOM_RenderAttributes::BlendOp_Count];
extern DWORD compareFuncLookupTable[ATOM_RenderAttributes::CompareFunc_Count];
extern DWORD stencilOpLookupTable[ATOM_RenderAttributes::StencilOp_Count];
extern DWORD fillModeLookupTable[ATOM_RenderAttributes::FillMode_Count];
extern DWORD samplerFilterLookupTable[ATOM_RenderAttributes::SamplerFilter_Count][3];
extern DWORD samplerAddressLookupTable[ATOM_RenderAttributes::SamplerAddress_Count];
extern DWORD textureOpLookupTable[ATOM_RenderAttributes::TextureOp_Count];
extern DWORD textureArgLookupTable[ATOM_RenderAttributes::TextureArg_Count];
extern DWORD textureCoordGenLookupTable[ATOM_RenderAttributes::TexCoordGen_Count];
extern DWORD fogModeLookupTable[ATOM_RenderAttributes::FogMode_Count];

#endif // __ATOM3D_RENDER_ENUMTABLE_D3D9_H
