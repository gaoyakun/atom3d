/*
  Copyright (c) 2004 Pascal Kirchdorfer, Seth Yastrov

  This software is provided 'as-is', without any express or implied warranty. In
  no event will the authors be held liable for any damages arising from the use
  of this software.

  Permission is granted to anyone to use this software for any purpose, including
  commercial applications, and to alter it and redistribute it freely, subject to
  the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim
       that you wrote the original software. If you use this software in a product,
       an acknowledgment in the product documentation would be appreciated but is
       not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#ifdef WIN32
#include <windows.h>
#endif //WIN32

#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "renderer/light.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
#include "renderer/triangles.h"
#include "renderer/vertices.h"
#include "renderer/tangents.h"

// ARB_multitexture
#define GL_ACTIVE_TEXTURE_ARB               0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB            0x84E2
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2

#ifndef GL_ARB_point_sprite
#define GL_POINT_SPRITE_ARB                 0x8861
#define GL_COORD_REPLACE_ARB                0x8862

#define GL_POINT_SIZE_MIN_ARB             0x8126
#define GL_POINT_SIZE_MAX_ARB             0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_ARB  0x8128
#define GL_POINT_DISTANCE_ATTENUATION_ARB 0x8129
#endif

// VBO Extension Definitions
#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4

typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum target);

typedef void (APIENTRY * PFNGLPOINTPARAMETERFARBPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFVARBPROC) (GLenum pname, const GLfloat *params);

typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);

#ifndef GL_ARB_multitexture
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
#endif

// PointSprite Extenstion Function Pointers
PFNGLPOINTPARAMETERFARBPROC  glPointParameterfARB  = NULL;
PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = NULL;

// VBO Extension Function Pointers
PFNGLGENBUFFERSARBPROC glGenBuffersARB = 0;
PFNGLBINDBUFFERARBPROC glBindBufferARB = 0;
PFNGLBUFFERDATAARBPROC glBufferDataARB = 0;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = 0;

Renderer::Renderer()
: hasInited(false), hasVboExt(false), last(0), fps(0), wireframe(false), textured(true),
  normals(false), points(false), vlist(0), tlist(0)
{
}

Renderer::~Renderer()
{
  if (hasInited)
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Renderer::init()
{
  SDL_InitSubSystem(SDL_INIT_VIDEO);

  SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 ); 

  const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo( );

  int videoFlags = 0;
  videoFlags  = SDL_OPENGL;          //Enable OpenGL
  videoFlags |= SDL_GL_DOUBLEBUFFER; //Enable double buffering

  if ( videoInfo->hw_available )
      videoFlags |= SDL_HWSURFACE;   //Hardware Surface
  else
      videoFlags |= SDL_SWSURFACE;   //Software Surface

  if ( videoInfo->blit_hw )
      videoFlags |= SDL_HWACCEL;     //Hardware Acceleration

  //Set double buffering
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  SDL_SetVideoMode( 1024, 768, 32, videoFlags );

  hasInited = true;

  //Enable Texture Mapping
  glEnable( GL_TEXTURE_2D );

  //Enable Coloring
  //glColorMaterial( GL_FRONT, GL_DIFFUSE );
  //glEnable( GL_COLOR_MATERIAL );

  //Enable smooth shading
  glShadeModel( GL_SMOOTH );

  //Set the background black
  glClearColor( 0.3f, 0.1f, 0.0f, 0.0f );

  //Depth buffer setup
  glClearDepth( 1.0f );

  //Enables Depth Testing
  glEnable( GL_DEPTH_TEST );

  //The Type Of Depth Test To Do
  glDepthFunc( GL_LEQUAL );

  //Really Nice Perspective Calculations
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
  glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
  
  //Enable Light One
  glEnable( GL_LIGHT0 );
  glEnable( GL_LIGHTING );

  //Setup The Ambient Light
  float ambiente[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  glLightfv( GL_LIGHT0, GL_AMBIENT, ambiente );

  //Setup The Diffuse Light
  float diffuse[] = { 0.8f, 0.8f, 0.8f, 0.8f };
  glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );

  //Setup The Diffuse Light
  float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glLightfv( GL_LIGHT0, GL_SPECULAR, specular );

  //Position The Light
  float pos[] = { -1.5f, 1.0f, -4.0f, 1.0f };
  glLightfv( GL_LIGHT0, GL_POSITION, pos );

  float mcolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);

  float specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);

  glMateriali(GL_FRONT, GL_SHININESS, 96);

  //Blending Function For Translucency Based On Source Alpha Value
  glEnable(GL_BLEND);	
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  //Enable Face Culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnableClientState (GL_VERTEX_ARRAY);
  glEnableClientState (GL_NORMAL_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glEnableClientState (GL_TEXTURE_COORD_ARRAY);

  glAlphaFunc(GL_GEQUAL,0.9f);
  glEnable(GL_ALPHA_TEST);

  //glEnable(GL_NORMALIZE);

#ifndef GL_ARB_multitexture
  // Multitexure extension
  glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) SDL_GL_GetProcAddress("glActiveTextureARB");
  glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) SDL_GL_GetProcAddress("glClientActiveTextureARB");

  if (!glActiveTextureARB)
  {
    printf ("Multitexture extensions not supported");
  }
#endif

  glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC) SDL_GL_GetProcAddress("glPointParameterfARB");
  glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC) SDL_GL_GetProcAddress("glPointParameterfvARB");

  if (glPointParameterfARB)
  {
    // This is how will our point sprite's size will be modified by 
    // distance from the viewer
    float quadratic[] =  { 1.0f, 0.0f, 0.01f };
    glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

    // Query for the max point size supported by the hardware
    float maxSize = 0.0f;
    glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );

    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f );

    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, maxSize );

    // Specify point sprite texture coordinate replacement mode for each 
    // texture unit
    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );

    glPointSize( 10 );
  }
  else
  {
    printf ("Point spite extensions not supported\n");
  }

  // VBO Extension
  glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) SDL_GL_GetProcAddress("glGenBuffersARB");
  glBindBufferARB = (PFNGLBINDBUFFERARBPROC) SDL_GL_GetProcAddress("glBindBufferARB");
  glBufferDataARB = (PFNGLBUFFERDATAARBPROC) SDL_GL_GetProcAddress("glBufferDataARB");
  glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) SDL_GL_GetProcAddress("glDeleteBuffersARB");

  if (glGenBuffersARB)
  {
    hasVboExt = true;
  }
  else
  {
    hasVboExt = false;
    printf("Vertex buffer object extensions not supported\n");
  }
}

void Renderer::getViewport(int& x, int& y, int& w, int& h)
{
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  x = viewport[0];
  y = viewport[1];
  w = viewport[2];
  h = viewport[3];
}

void Renderer::setViewport(int x, int y, int w, int h)
{
  glViewport(x,y,w,h);
}

void Renderer::drawMesh(unsigned num_triangles, const Triangle* triangles)
{
  if (textured)
  {
    glDepthFunc( GL_LEQUAL );
    glPolygonMode(GL_FRONT, GL_FILL);
    glDrawElements(GL_TRIANGLES, num_triangles*3, GL_UNSIGNED_INT, triangles);
  }

  if (wireframe)
  {
    glColor3f(0.5f,0.4f,0.1f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, num_triangles*3, GL_UNSIGNED_INT, triangles);
  }

  if (points)
  {
    glColor3f(0.5f,0.4f,0.1f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glDrawElements(GL_TRIANGLES, num_triangles*3, GL_UNSIGNED_INT, triangles);
  }

  for (GLenum x = glGetError(); x != 0; x=glGetError())
    printf("%s\n",(const char*)glGetString(x));
}

void Renderer::setupArray(const Vertex* vertices, const Tangent* tangents)
{
  if (vlist != vertices)
  {
    vlist = vertices;

    if (vertices !=0)
      setupVBOArray(0);

    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vertices[0].x);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), &vertices[0].nx);
    //glColorPointer(3, GL_FLOAT, sizeof(Vertex), &vertices[0].r);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].u);

  }
  if (tangents)
  {
    if (tlist != tangents)
    {
      tlist = tangents;
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glEnableClientState (GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, sizeof(Tangent), &tangents[0].nsx);
      glClientActiveTextureARB(GL_TEXTURE2_ARB);
      glEnableClientState (GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, sizeof(Tangent), &tangents[0].ntx);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
    }
  }
  else
  {
    tlist = 0;
    glClientActiveTextureARB(GL_TEXTURE1_ARB);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glClientActiveTextureARB(GL_TEXTURE2_ARB);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glClientActiveTextureARB(GL_TEXTURE0_ARB);
  }
}

void Renderer::setupVBOArray(unsigned int vertices)
{
  if (!hasVboExt)
    return;

  if (vertices != 0)
    setupArray(0,0);

  glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertices );
  glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), (char *) 0 );
  glNormalPointer(GL_FLOAT, sizeof(Vertex), (char *) (sizeof(float) * 3));
  glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), (char *) (sizeof(float) * 9) );
}

unsigned int Renderer::createVBO(const Vertex* vertices, unsigned int count)
{
  if (!hasVboExt)
    return 0;

  unsigned int buffer;
  
  // Generate and bind the Vertex Buffer
  glGenBuffersARB( 1, &buffer );
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, buffer );

  // Load the data
  glBufferDataARB( GL_ARRAY_BUFFER_ARB, count*sizeof(Vertex), &vertices[0].x, GL_STATIC_DRAW_ARB );

  return buffer;
}

void Renderer::setLight(Light* light, unsigned int number)
{
  if (light)
  {
    //Enable Light
    glEnable( GL_LIGHT0 + number );

    //Setup The Ambient Light
    glLightfv( GL_LIGHT0 + number, GL_AMBIENT, light->getAmbiente() );

    //Setup The Diffuse Light
    glLightfv( GL_LIGHT0 + number, GL_DIFFUSE, light->getDiffuse() );

    //Setup The Diffuse Light
    glLightfv( GL_LIGHT0 + number, GL_SPECULAR, light->getSpecular() );

    //Position The Light
    glLightfv( GL_LIGHT0 + number, GL_POSITION, light->getPos() );
  }
  else
  {
    glDisable( GL_LIGHT0 + number );
  }
}

void Renderer::getLight(Light* light, unsigned int number)
{
  if (light)
  {
    float ambiente[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glGetLightfv(GL_LIGHT0 + number, GL_AMBIENT, ambiente);
    light->setAmbiente(ambiente);

    float diffuse[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glGetLightfv(GL_LIGHT0 + number, GL_DIFFUSE, diffuse);
    light->setDiffuse(diffuse);

    float specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glGetLightfv(GL_LIGHT0 + number, GL_SPECULAR, specular);
    light->setSpecular(specular);

    float pos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glGetLightfv(GL_LIGHT0 + number, GL_POSITION, pos);
    light->setPos(pos);
  }
}

void Renderer::setTexture(Texture* tex, unsigned int layer)
{
  glActiveTextureARB(GL_TEXTURE0_ARB+layer);
  glBindTexture(GL_TEXTURE_2D, tex->id);
}

Texture* Renderer::createTexture(unsigned x, unsigned y)
{
  Texture* tex = new Texture();
  tex->width = x;
  tex->height = y;

  unsigned int txtnumber;

  glGenTextures(1, &txtnumber);
  glBindTexture(GL_TEXTURE_2D, txtnumber);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  tex->id = txtnumber;
  return tex;
}

void Renderer::render2Texture(unsigned tex, unsigned x, unsigned y)
{
  glBindTexture(GL_TEXTURE_2D,tex);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, x, y, 0);
}

void Renderer::renderDepth2Texture(unsigned tex, unsigned x, unsigned y)
{
  glBindTexture(GL_TEXTURE_2D,tex);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, x, y, 0);
}

void Renderer::clear(float r,float g,float b,float a)
{
  glClearColor(r, g, b, a);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Renderer::blit()
{
  SDL_GL_SwapBuffers();
}

void Renderer::enableLight(bool state)
{
  if (state)
    glEnable(GL_LIGHTING);
  else
    glDisable(GL_LIGHTING);
}

void Renderer::enableTransparency(bool state)
{
  if (state) glEnable(GL_BLEND);
  else glDisable(GL_BLEND);
}

void Renderer::enableAlphaTest(bool state)
{
  if (state) glEnable(GL_ALPHA_TEST);
  else glDisable(GL_ALPHA_TEST);
}

void Renderer::enablePointSprites(bool state)
{
  if (state)
  {
    points = true;
    textured = false;
    glEnable(GL_POINT_SPRITE_ARB);
  }
  else 
  {
    points = false;
    textured = true;
    glDisable(GL_POINT_SPRITE_ARB);
  }
}

void Renderer::sleep(unsigned time)
{
  SDL_Delay(time);
}

void Renderer::updateFPS(const char* name, unsigned vert, unsigned tria)
{
  fps++;

  unsigned now = SDL_GetTicks();

  if( SDL_GetTicks() - last > 1000 )
  {
    //titel is limited to 256 (char length
    sprintf(title,"%s FPS: %.2f Vertices: %i Faces: %i", name, fps/(float)(now - last)*1000, vert, tria);
    SDL_WM_SetCaption(title,title);
    last = SDL_GetTicks();
    fps = 0;
  }
}

void Renderer::setPerspective(float fov, float ratio, float nearplane, float farplane)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, ratio, nearplane, farplane);
}

void Renderer::setCamera(opentree::otMatrix view)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLoadMatrixf(view);
}

Texture* Renderer::loadTexture(const char* filename)
{
  Texture* tex = new Texture();

  SDL_Surface *img = IMG_Load(filename);

  unsigned texture = 0;

  if(!img) 
  {
    return 0;
  }

  glGenTextures(1, &texture);

  glBindTexture(GL_TEXTURE_2D, texture);

  tex->width = img->w;
  tex->height = img->h;

  if (img->format->BytesPerPixel == 3) // RGB 24bit
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->w, img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);
  }
  else if (img->format->BytesPerPixel == 4) // RGBA 32bit
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
  }
  else
  {
    SDL_FreeSurface(img);
    return 0;
  }

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  SDL_FreeSurface(img);

  tex->id = texture;
  return tex;
}
