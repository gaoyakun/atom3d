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

#ifndef __MATERIAL_H_
#define __MATERIAL_H_

#include <renderer/renderer.h>
#include <renderer/texture.h>

#include "vertexlist.h"
#include "indexlist.h"
#include "shader.h"

#include <opentree/mesher/meshtypes.h>

/// \addtogroup Renderer
/// @{

class Material
{
public:
  //Materials can have maximally 8 textures. Just because I say so :-)
  Texture* textures[8];

  Shader* shader;

  char* name;

public:
  Material() : shader(0), name(0)
  {
    for (int i=0; i<8; i++)
      textures[i] = 0;
  }
  ~Material() 
  { 
    delete [] name; 
  }

  const char* getName()
  {
    return name;
  }

  void setName(const char* name)
  {
    this->name = new char[strlen(name)+1];
    strcpy(this->name,name);
  }

  void setToRenderer(Renderer* renderer)
  {
    for (int i=0; i<8; i++)
    {
      if (textures[i] != 0)
      {
        renderer->setTexture(textures[i], i);
      }
    }
  }

  void enableShader(Renderer* renderer)
  {
    if (shader)
      shader->preRender(renderer);
  }

  void disableShader()
  {
    if (shader)
      shader->postRender();
  }

  void setShader(Shader* shader)
  {
    this->shader = shader;
  }

  void setTexture(Texture* tex, unsigned int number)
  {
    textures[number] = tex;
  }
  
  Texture* getTexture(unsigned int number)
  {
    return textures[number];
  }
};

/// }@

#endif // __MATERIAL_H_
