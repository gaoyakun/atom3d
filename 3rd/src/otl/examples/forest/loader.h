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

#ifndef _FOREST_LOADER_H_
#define _FOREST_LOADER_H_

#include "forest.h"
#include <opentree/mesher/leafmesher.h>
#include <opentree/mesher/billboarder.h>
#include <opentree/mesher/billboxer.h>
#include <opentree/mesher/billcloud.h>
#include <opentree/mesher/treemesher.h>

#include <renderer/shader_cg.h>

#include <opentree/utils/otvector3.h>
#include <helper/xmlparser.h>
#include <helper/xmlparserarbaro.h>

/// \addtogroup Forest
/// @{

class TreeFactory;

class Loader
{
private:
  Forest& forest;
  Renderer* renderer;

public:
  Loader(Forest& forest, Renderer* renderer) 
  : forest(forest), renderer(renderer)
  {
  }

  ~Loader() {}

  void loadFileTexture(const char* name, const char* file)
  {
	  char buffer[MAX_PATH];
	  ::GetCurrentDirectoryA (MAX_PATH, buffer);
    Texture* texture = renderer->loadTexture(file);
    texture->setName(name);
    forest.addTexture(texture);
  }

  void loadNewTexture(const char* name, int x, int y)
  {
    Texture* texture = renderer->createTexture(x, y);
    texture->setName(name);
    forest.addTexture(texture);
  }

  void loadWeberParams(const char* name, const char* format, const char* file)
  {
    TreeParams* params = new TreeParams();
    params->setName(name);
    if(!strncmp(format,"otl-xml",7))
    {      
      opentree::xmlParser parser;
      if (!parser.Load(params->data, file))
        return;
    }
    else if(!strncmp(format,"arbaro-xml",10))
    {      
      opentree::xmlParserArbaro parser;
      if (!parser.Load(params->data, file))
        return;
    }
    forest.addTreeParams(params);
  }

  void loadVertexList(const char* name)
  {
    forest.addVertexList(new VertexList(name));
  }

  void loadFacotry(const char* name)
  {
    TreeFactory* factory = new TreeFactory();
    factory->setName(name);
    forest.addFactory(factory);
  }

  void loadGenerator(const char* factory, const char* name, 
                     const char* params, int seed)
  {
    TreeFactory* treefactory = forest.findFactory(factory);
    TreeParams* treeparams = forest.findTreeParams(params);
    if (!treefactory || !treeparams) return;

    if (!strncmp(name, "otl-weber", 9))
    {
      opentree::iWeber* weber = opentree::newWeber();
      weber->setParams(treeparams->data);
      opentree::setRandomSeed(seed);
      opentree::otTree* tree = weber->generate();
      treefactory->setTree(tree);
      delete weber;
    }
    else if (!strncmp(name, "arbaro-weber", 12))
    {
      opentree::iWeber* weber = opentree::newWeberArbaro();
      weber->setParams(treeparams->data);
      opentree::setRandomSeed(seed);
      opentree::otTree* tree = weber->generate();
      treefactory->setTree(tree);
      delete weber;
    }
    else
    {
      printf("unknown generator '%s'\n", name);
    }
  }

  void loadLeafMesh(const char* factory, const char* name, const char* vertexlist, 
                    float from, float to, const char* texture)
  {
    TreeFactory* treefactory = forest.findFactory(factory);
    VertexList* vlist = forest.findVertexList(vertexlist);
    Texture* tex = forest.findTexture(texture);
    if (!treefactory || !tex || !vertexlist) return;

    opentree::otTree* ot_tree = treefactory->getTree();

    opentree::MesherTree leafmesher(ot_tree);

    Material* mat = new Material();
    mat->setTexture(tex, 0);

    Mesh* mesh = new Mesh();
    mesh->setMaterial(mat);
    mesh->setName(name);
    mesh->setVertices(vlist);

    unsigned int offset = vlist->getCount();

    int v_count = 0;
    leafmesher.getLeavesVerticesCount(&v_count);
    vlist->reserveVertices(v_count);
    leafmesher.getLeavesVertices(*vlist);

    int t_count = 0;
    leafmesher.getLeavesIndicesCount(&t_count);
    IndexList* triangles = new IndexList;
    triangles->reserveTriangles(t_count);
    leafmesher.getLeavesIndices(*triangles, offset);
    mesh->setIndices(triangles);
    mesh->hasTransparency = true;
    mesh->hasAlphaTransparency = true;
    mesh->alignToCamera = false;

    treefactory->addMesh(mesh, from, to);
  }

  void loadStemMesh(const char* factory, const char* name, const char* vertexlist,
                    float from, float to, int level, int curveres, int circleres,
                    const char* texture)
  {
    TreeFactory* treefactory = forest.findFactory(factory);
    VertexList* vlist = forest.findVertexList(vertexlist);
    Texture* tex = forest.findTexture(texture);
    if (!treefactory || !tex || !vlist) return;

    opentree::MesherTree treemesher(treefactory->getTree());
    treemesher.setCurveRes(level, curveres);
    treemesher.setCircleRes(level, circleres);

    int v_count=0;
    int t_count=0;

    treemesher.getVerticesCount(level, &v_count);
    treemesher.getIndicesCount(level, &t_count);

    IndexList* indexlist = new IndexList;

    //unsigned int offset = vlist->getCount();

    vlist->reserveVertices(v_count);
    treemesher.getVertices(level, *vlist);

    Material* mat = new Material();
    mat->setTexture(tex, 0);

    indexlist->reserveTriangles(t_count);
    treemesher.getIndices(level, *indexlist, 0);

    Mesh* mesh = new Mesh();
    mesh->setMaterial(mat);
    mesh->setName(name);
    mesh->setVertices(vlist);
    //mesh->vertex_count = v_count;
    mesh->setIndices(indexlist);
    mesh->hasTransparency = false;
    mesh->hasAlphaTransparency = false;
    mesh->alignToCamera = false;
    treefactory->addMesh(mesh, from, to);
  }

  void loadBillboardMesh(const char* factory, const char* name, const char* vertexlist,
                         float from, float to, opentree::otMatrix& trf, const char* tex, 
                         const char* normaltex, const char* depthtex, int u0, int u1, int v0, int v1, 
                         const char** source, int sources)
  {
    Mesh** sourcemeshes = new Mesh*[sources];
    TreeFactory* treefactory = forest.findFactory(factory);
    for (int i=0; i<sources; i++)
      sourcemeshes[i] = treefactory->findMesh(source[i]);

    VertexList* vlist = forest.findVertexList(vertexlist);
    Texture* texture = forest.findTexture(tex);
    Texture* normal_texture = 0;
    Texture* depth_texture = 0;

    if (normaltex) 
      normal_texture = forest.findTexture(normaltex);

    if (depthtex) 
      depth_texture = forest.findTexture(depthtex);

    if (!treefactory || !vertexlist || !texture) return;

    ///TODO: Passing vertexlist to MesherBillboard is wrong! there might be
    ///      other vertices in the list as well.
    unsigned int count = sourcemeshes[0]->vertexlist->getCount();
    opentree::otVector3* vertices = new opentree::otVector3[count];
    const Vertex* vertex = sourcemeshes[0]->vertexlist->getVertices();
    for (unsigned int i=0; i<count; i++)
    {
      vertices[i].set(vertex[i].x, vertex[i].y, vertex[i].z);
    }
    opentree::MesherBillboard billboardmesher(vertices, count, trf);
    delete [] vertices;

    Material* mat = new Material();
    mat->setTexture(texture, 0);
    if (normal_texture) 
      mat->setTexture(normal_texture, 1);
    if (depth_texture) 
      mat->setTexture(depth_texture, 2);

    Mesh* billboard = new Mesh();
    billboard->setMaterial(mat);
    billboard->setName(name);
    billboard->setVertices(vlist);

    unsigned int offset = vlist->getCount();

    billboardmesher.getVertices(*vlist);

    IndexList* indexlist = new IndexList;

    billboardmesher.getIndices(*indexlist, offset);
    billboard->setIndices(indexlist);
    billboard->hasTransparency = true;
    billboard->hasAlphaTransparency = true;
    billboard->alignToCamera = false;

    treefactory->addMesh(billboard, from, to);

    int x, y, w, h;
    renderer->getViewport(x, y, w, h);

    opentree::otVector3 c = -billboardmesher.getBox().getCenter();
    opentree::otVector3 s = billboardmesher.getBox().getSize();

    renderer->setViewport(texture->width*u0, texture->height*v0,
                          texture->width*u1, texture->height*v1);

    opentree::otMatrix camera;

    camera = trf;
    camera.grotXDegree(90);
    camera.grotZDegree(180);

    camera.thistranslate(c);
    camera.translate(0,0,-s.Z);
    renderer->setCamera(camera);
    // Adjust for depth texture. Don't ask... please don't :)
    renderer->setPerspective(60.0f, s.X/s.Z, 9, 11);
    //renderer->setPerspective(60.0f, s.X/s.Z, .25f, 1000.0f * .25f);


    if (normal_texture || depth_texture)
    {
      ShaderCG shader;
      shader.init("data/shaders/normals_v.cg", "data/shaders/normals_f.cg");

      //Draw normals
      renderer->clear(0,0,0,0);
      for (int i=0; i<sources; i++)
      {
        sourcemeshes[i]->getMaterial()->setShader(&shader); //Set normals shader
        sourcemeshes[i]->draw(renderer);
        sourcemeshes[i]->getMaterial()->setShader(0); //Set no shader
      }
      if (normal_texture)
        renderer->render2Texture(normal_texture->id,texture->width, texture->height);
      if (depth_texture)
        renderer->renderDepth2Texture(depth_texture->id,texture->width, texture->height);
      //renderer->blit();
      //renderer->sleep(1000);
      ShaderCG* nmap_shader = new ShaderCG();
      nmap_shader->init("data/shaders/parallax_v.cg", "data/shaders/parallax_f.cg");
      mat->setShader(nmap_shader);
      //mat->setTexture(normal_texture, 0);
    }

    //Draw colors
    renderer->clear(0,0,0,0);
    for (int i=0; i<sources; i++)
      sourcemeshes[i]->draw(renderer);
    //renderer->clear(1,1,1,1);
    renderer->render2Texture(texture->id,texture->width, texture->height);

    renderer->setViewport(x, y, w, h);
    renderer->setPerspective(60.0f, 1.5f, .25f, 1000.0f * .25f);
    //renderer->blit();
    //renderer->sleep(1000);
    billboard->setTangents(new TangentList());
    billboard->createTangents();
  }

  void loadObjects(const char* name, const char* factory, opentree::otMatrix& trf)
  {
    TreeFactory* treefactory = forest.findFactory(factory);
    if (!treefactory) return;
    TreeObject* obj = treefactory->Create();
    obj->setName(name);
    obj->setTransformation(trf);
    forest.addObject(obj);
  }
};

/// }@

#endif // _FOREST_LOADER_H_
