#include <stdio.h>
#include "renderer/shader_cg.h"
#include "renderer/renderer.h"
#include "renderer/light.h"

void ErrorCheck()
{
#ifdef HAS_CG
  CGerror error = cgGetError();
  if (error)
    printf("CG Error: %s\n", cgGetErrorString(error));
#endif //HAS_CG
}

ShaderCG::ShaderCG()
{
  use_cg = true;
}

ShaderCG::~ShaderCG()
{
}

bool ShaderCG::init(const char* file_v, const char* file_f)
{
#ifdef HAS_CG
  CGcontext context = cgCreateContext();

  profile_vertex = cgGLGetLatestProfile(CG_GL_VERTEX);;
  profile_fragment = cgGLGetLatestProfile(CG_GL_FRAGMENT);;

  cgGLSetOptimalOptions(profile_vertex);
  cgGLSetOptimalOptions(profile_fragment);

  program_vertex   = cgCreateProgramFromFile(context, CG_SOURCE, file_v, profile_vertex  , 0, 0);
  program_fragment = cgCreateProgramFromFile(context, CG_SOURCE, file_f, profile_fragment, 0, 0);

  if (!program_vertex || !program_fragment)
  {
    ErrorCheck();
    use_cg = false;
    printf("Shaders Disabled\n");
    return false;
  }

  g_modelViewProjMatrix = cgGetNamedParameter(program_vertex, "ModelViewProj");
  g_modelViewMatrix = cgGetNamedParameter(program_vertex, "ModelView");
  g_modelViewMatrixI = cgGetNamedParameter(program_vertex, "ModelViewI");
  g_modelViewMatrixIT = cgGetNamedParameter(program_vertex, "ModelViewIT");

  g_lightPosition = cgGetNamedParameter(program_vertex, "LightPos");
  g_lightDiffuse  = cgGetNamedParameter(program_fragment, "lightcolor");
  //g_lightAmbiente = cgGetNamedParameter(program_vertex, "LightAmbiente");
  //g_lightSpecular = cgGetNamedParameter(program_vertex, "LightSpecular");

  cgGLLoadProgram(program_vertex);
  cgGLLoadProgram(program_fragment);

  ErrorCheck();

  printf("Shaders Enabled\n");

  return true;
#else
  printf("Shaders Disabled\n");
  return false;
#endif // HAS_CG
}

void ShaderCG::preRender(Renderer* renderer)
{
#ifdef HAS_CG
  if (!use_cg) return;

  cgGLEnableProfile(profile_vertex);
  cgGLEnableProfile(profile_fragment);
  cgGLBindProgram(program_vertex);
  cgGLBindProgram(program_fragment);

  if(g_modelViewProjMatrix != NULL)
    cgGLSetStateMatrixParameter(g_modelViewProjMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

  if(g_modelViewMatrix != NULL)
    cgGLSetStateMatrixParameter(g_modelViewMatrix, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);

  if(g_modelViewMatrixI != NULL)
    cgGLSetStateMatrixParameter(g_modelViewMatrixI, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE);

  if(g_modelViewMatrixIT != NULL)
    cgGLSetStateMatrixParameter(g_modelViewMatrixIT, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);

  Light light;
  renderer->getLight(&light, 0);

  if(g_lightDiffuse != NULL)
    cgGLSetParameter4fv(g_lightDiffuse, light.getDiffuse());

  if(g_lightPosition != NULL)
    cgGLSetParameter4fv(g_lightPosition, light.getPos());

/*
  if(g_lightAmbiente != NULL)
    cgGLSetParameter4fv(g_lightAmbiente, light.getAmbiente());

  if(g_lightSpecular != NULL)
    cgGLSetParameter4fv(g_lightSpecular, light.getSpecular());
*/


  ErrorCheck();

  glDisable(GL_LIGHTING);

#endif // HAS_CG
}

void ShaderCG::postRender()
{
#ifdef HAS_CG
  if (!use_cg) return;

  glEnable(GL_LIGHTING);
  cgGLDisableProfile(profile_vertex);
  cgGLDisableProfile(profile_fragment);

  ErrorCheck();
#endif // HAS_CG
}
