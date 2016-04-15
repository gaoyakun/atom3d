#include "stdafx.h"
#include "basedefs.h"
#include "texture_d3d.h"
#include "d3d9window.h"
#include "d3d9device.h"
#include "depthbuffer_d3d.h"
#include "indexarray_d3d.h"
#include "vertexarray_d3d.h"
#include "render.h"
#include "vertexshader_d3d.h"
#include "pixelshader_d3d.h"
#include "gpuprogram_d3d.h"
#include "stateset_base.h"
#include "text.h"
//--- wangjian added ---//
// 新增一个render target surface的资源类型
#include "rendertargetsurface_d3d.h"
//----------------------//

ATOM_PLUGIN_BEGIN(Render, "atom.render", 0, 0, 1)
  ATOM_PLUGIN_DEF(Render, ATOM_Renderer)
  ATOM_PLUGIN_DEF(Render, ATOM_D3D9Window)
  ATOM_PLUGIN_DEF(Render, ATOM_D3DTexture)
  ATOM_PLUGIN_DEF(Render, ATOM_DepthBufferD3D)
  ATOM_PLUGIN_DEF(Render, ATOM_D3D9IndexArray)
  ATOM_PLUGIN_DEF(Render, ATOM_D3DVertexArray)
  ATOM_PLUGIN_DEF(Render, ATOM_VertexShaderD3D9)
  ATOM_PLUGIN_DEF(Render, ATOM_PixelShaderD3D9)
  ATOM_PLUGIN_DEF(Render, ATOM_GPUProgramD3D9)
  ATOM_PLUGIN_DEF(Render, ATOM_RenderStateSetBase)
  ATOM_PLUGIN_DEF(Render, ATOM_Text)
  //--- wangjian added ---//
  // 新增一个render target surface的资源类型
  ATOM_PLUGIN_DEF(Render, ATOM_RenderTargetSurfaceD3D)
  //----------------------//
ATOM_PLUGIN_END

