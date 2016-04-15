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

#ifndef _FOREST_XMLPARSER_H_
#define _FOREST_XMLPARSER_H_

#include "forest.h"
#include "loader.h"

#include <opentree/utils/otvector3.h>

/// \addtogroup Forest
/// @{

class TreeFactory;

class xmlParserForest
{
private:
  Loader* loader;

public:
  xmlParserForest(Loader* loader) : loader(loader)
  {
  }
  ~xmlParserForest() {}

  bool parse(const char* filename)
  {
    TiXmlDocument doc(filename);
    if (!doc.LoadFile())
      return false;
      
    TiXmlHandle docHandle(&doc);
    TiXmlNode* resources = docHandle.FirstChild("forest").FirstChild("resources").Node();
    TiXmlNode* factories = docHandle.FirstChild("forest").FirstChild("factories").Node();
    TiXmlNode* objects = docHandle.FirstChild("forest").FirstChild("objects").Node();

    printf("Loading Resources ");
    parseResources(resources);
    printf("[done]\n");
    printf("Loading Factories ");
    parseFactories(factories);
    printf("[done]\n");
    printf("Loading Objects ");
    parseObjects(objects);
    printf("[done]\n");

    return true;
  }

  void parseResources(TiXmlNode* resources)
  {
    for( TiXmlNode* child = resources->FirstChild(); child; child = child->NextSibling() )
    {
      printf(".");

      if (!strncmp(child->Value(), "weberparams", 11))
        parseWeberParams(child);
      else if (!strncmp(child->Value(), "texture", 7))
        parseTexture(child);
      else if (!strncmp(child->Value(), "vertexlist", 10))
       loader->loadVertexList(child->FirstChild()->Value());
    }
  }

  void parseTexture(TiXmlNode* texture)
  {
    const char* name = texture->ToElement()->Attribute("name");
    if (!strncmp(texture->FirstChild()->Value(),"file", 4))
    {
      const char* file = texture->FirstChild()->FirstChild()->Value();
      loader->loadFileTexture(name, file);
    }
    else if (!strncmp(texture->FirstChild()->Value(),"new", 3))
    {
      int x, y;
      texture->FirstChildElement()->Attribute("x", &x);
      texture->FirstChildElement()->Attribute("y", &y);
      loader->loadNewTexture(name, x, y);
    }
  }

  void parseWeberParams(TiXmlNode* weberparams)
  {
    const char* name = weberparams->ToElement()->Attribute("name");
    const char* format = weberparams->FirstChild("format")->FirstChild()->Value();
    const char* file = weberparams->FirstChild("file")->FirstChild()->Value();
    loader->loadWeberParams(name, format, file);
  }

  void parseFactories(TiXmlNode* factories)
  {
    for( TiXmlNode* child = factories->FirstChild(); child; child = child->NextSibling() )
    {
      if (child->Type() != TiXmlNode::ELEMENT) continue;
      const char* factory = child->ToElement()->Attribute("name");
      loader->loadFacotry(factory);
      for( TiXmlNode* fact_child = child->FirstChild(); fact_child; fact_child = fact_child->NextSibling() )
      {
        const char* fact_child_name = fact_child->Value();
        if (!strncmp(fact_child_name, "generator", 9))
          parseGenerator(fact_child, factory);
        else if (!strncmp(fact_child_name, "mesh", 4))
          parseMesh(fact_child, factory);
      }
    }
  }

  void parseGenerator(TiXmlNode* gen, const char* factory)
  {
    printf(".");
    const char* name = gen->ToElement()->Attribute("name");
    const char* params = gen->FirstChild("params")->FirstChild()->Value();
    const char* seed = gen->FirstChild("seed")->FirstChild()->Value();
    loader->loadGenerator(factory, name, params, atoi(seed));
  }

  void parseMesh(TiXmlNode* mesh, const char* factory)
  {
    if (mesh->Type() != TiXmlNode::ELEMENT) return;

    const char* name = mesh->ToElement()->Attribute("name");
    const char* from = mesh->FirstChild("from")->FirstChild()->Value();
    const char* to = mesh->FirstChild("to")->FirstChild()->Value();
    const char* vertexlist = mesh->FirstChild("vertexlist")->FirstChild()->Value();

    TiXmlNode* type = mesh->FirstChild("type");

    const char* type_name = type->ToElement()->Attribute("name");
    if (!strncmp(type_name, "leaves", 6))
    {
      printf("l");
      const char* texture = type->FirstChild("texture")->FirstChild()->Value();
      loader->loadLeafMesh(factory, name, vertexlist, (float)atof(from), (float)atof(to), texture);
    }
    else if (!strncmp(type_name, "stem", 4))
    {
      const char* level = type->FirstChild("level")->FirstChild()->Value();
      const char* texture = type->FirstChild("texture")->FirstChild()->Value();
      const char* curveres = type->FirstChild("curveres")->FirstChild()->Value();
      const char* circleres = type->FirstChild("circleres")->FirstChild()->Value();
      printf("s%d", atoi(level));
      loader->loadStemMesh(factory, name, vertexlist, (float)atof(from),
        (float)atof(to), atoi(level), atoi(curveres), atoi(circleres), texture);
    }
    else if (!strncmp(type_name, "billboard", 9))
    {
      printf("b");
      opentree::otArray<const char*> sources;
      for( TiXmlNode* child = type->FirstChild("source"); child; child = child->NextSibling("source") )
      {
        sources.add(child->FirstChild()->Value());
      }
      const char* texture = type->FirstChild("texture")->FirstChild()->Value();
      const char* normaltexture = type->FirstChild("normals")->FirstChild()->Value();
      const char* depthtexture = type->FirstChild("depth")->FirstChild()->Value();
      int u0, u1, v0, v1;
      type->FirstChildElement("uv")->Attribute("u0", &u0);
      type->FirstChildElement("uv")->Attribute("u1", &u1);
      type->FirstChildElement("uv")->Attribute("v0", &v0);
      type->FirstChildElement("uv")->Attribute("v1", &v1);
      opentree::otMatrix trf = parseTransform(type->FirstChildElement("transform"));
      loader->loadBillboardMesh(factory, name, vertexlist, (float)atof(from), (float)atof(to), trf, texture, normaltexture, depthtexture, u0, u1, v0, v1, sources.getRawArray(), sources.getCount());
    }
    else if (!strncmp(type_name, "billcloud", 9))
    {
      printf("c");
      printf("\nBillboard Clouds have not been implemented yet\n");
    }
  }

  void parseObjects(TiXmlNode* objects)
  {
    for( TiXmlNode* child = objects->FirstChild(); child; child = child->NextSibling() )
    {
      if (child->Type() != TiXmlNode::ELEMENT) continue;
      printf(".");
      const char* name = child->ToElement()->Attribute("name");
      const char* factory = child->FirstChild("factory")->FirstChild()->Value();
      TiXmlNode* transform = child->FirstChild("transform");
      opentree::otMatrix trf = parseTransform(transform);
      loader->loadObjects(name, factory, trf);
    }
  }

  opentree::otMatrix parseTransform(TiXmlNode* transform)
  {
    opentree::otMatrix trf;
    for( TiXmlNode* trf_child = transform->FirstChild(); trf_child; trf_child = trf_child->NextSibling() )
    {
      if (!strncmp(trf_child->Value(), "translate", 9))
      {
        double x, y, z;
        trf_child->ToElement()->Attribute("x", &x);
        trf_child->ToElement()->Attribute("y", &y);
        trf_child->ToElement()->Attribute("z", &z);
        trf.translate((float)x,(float)y,(float)z);
      }
      if (!strncmp(trf_child->Value(), "rot", 3))
      {
        double x, y, z;
        trf_child->ToElement()->Attribute("x", &x);
        trf_child->ToElement()->Attribute("y", &y);
        trf_child->ToElement()->Attribute("z", &z);
        if (x) trf.grotXDegree((float)x);
        if (y) trf.grotYDegree((float)y);
        if (z) trf.grotZDegree((float)z);
      }
    }
    return trf;
  }
};

/// }@

#endif // _FOREST_XMLPARSER_H_
