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

#ifndef __RENDERER_H_
#define __RENDERER_H_

#include <opentree/mesher/meshtypes.h>
#include "vertices.h"
#include "triangles.h"
#include <opentree/utils/otmatrix.h>

class Tangent;
class Texture;
class Triangle;
class Vertex;
class Light;

/// \addtogroup Renderer
/// @{

class Renderer
{
private:
  bool hasInited, hasVboExt;
  unsigned last, fps;
  bool wireframe, textured, normals, points;
  const Vertex* vlist;
  const Tangent* tlist;
  char title[256];

public:
  Renderer();
  ~Renderer();
  void init();
  void setViewport(int x, int y, int w, int h);
  void getViewport(int& x, int& y, int& w, int& h);
  void drawMesh(unsigned num_triangles, const Triangle* triangles);
  void setupArray(const Vertex* vertices, const Tangent* tangents = 0);
  void setupVBOArray(unsigned int vertices);
  void clear(float r,float g,float b,float a);
  void blit();
  void setPerspective(float fov, float ratio, float near, float far);
  void render2Texture(unsigned tex, unsigned x, unsigned y);
  void renderDepth2Texture(unsigned tex, unsigned x, unsigned y);
  void enableTransparency(bool state);
  void enableAlphaTest(bool state);
  void enableLight(bool state);
  void enablePointSprites(bool state);
  void sleep(unsigned time);
  void updateFPS(const char* title, unsigned vert, unsigned tria);
  void setCamera(opentree::otMatrix view);
  unsigned int createVBO(const Vertex* vertices, unsigned int count);
  Texture* createTexture(unsigned x, unsigned y);
  Texture* loadTexture(const char* filename);
  void setTexture(Texture* tex, unsigned int layer=0);
  void showWireFrame(bool enable) { wireframe = enable; }
  void showTextures(bool enable) { textured = enable; }
  void showNormals(bool enable) { normals = enable; }
  void getLight(Light* light, unsigned int number);
  void setLight(Light* light, unsigned int number);
};

/// }@

#endif //__RENDERER_H_
