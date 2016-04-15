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

#ifndef __TREEVIEW_H_
#define __TREEVIEW_H_

#include <opentree/opentree.h>

#include "opentree/utils/otutils.h"

#include <renderer/gamelet.h>
#include <renderer/mesh.h>
#include <renderer/tangentlist.h>
#include <renderer/texture.h>
#include <renderer/vertices.h>
#include <renderer/vertexlist.h>
#include <renderer/indexlist.h>
#include <renderer/shader_cg.h>

#include <opentree/mesher/treemesher.h>
#include <opentree/mesher/leafmesher.h>

class TreeView : public Gamelet
{
private:
  opentree::MesherTree* tree;

  VertexList vertices;

  IndexList trunk_triangle;
  IndexList branch_triangle;
  IndexList subbranch_triangle;
  IndexList leaves_triangle;

  TangentList trunk_tangents;

  Texture* stem_tex;
  Texture* stem_tex_n;
  Texture* stem_tex_h;
  Texture* branch_tex;
  Texture* branch_tex_n;
  Texture* branch_tex_h;
  Texture* leaves_tex;

  unsigned int vbo_data;

  ShaderCG normal_shader;
  ShaderCG bump_shader;
  ShaderCG leaf_shader;

  opentree::otMatrix camera;

  bool normals, light, vbo, bumpmap, leaves;
  
public:
  TreeView(const char* type, const char* algo, const char* treefile);
  ~TreeView();
  
  void init();

  void handleEvent(Events ev, int a=0, int b=0, int c=0);

  void draw();
};

#endif //__TREEVIEW_H_
