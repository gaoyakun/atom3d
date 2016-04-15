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

#ifndef __LIGHT_H_
#define __LIGHT_H_

/// \addtogroup Renderer
/// @{

class Light
{
private:
  float ambiente[4];
  float diffuse[4];
  float specular[4];
  float pos[4];

public:
  Light() {}
  ~Light() {}

  float* getAmbiente()
  {
    return ambiente;
  }

  void setAmbiente(float color[4])
  {
    ambiente[0] = color[0];
    ambiente[1] = color[1];
    ambiente[2] = color[2];
    ambiente[3] = color[3];
  }

  float* getDiffuse()
  {
    return diffuse;
  }

  void setDiffuse(float color[4])
  {
    diffuse[0] = color[0];
    diffuse[1] = color[1];
    diffuse[2] = color[2];
    diffuse[3] = color[3];
  }

  float* getSpecular()
  {
    return specular;
  }

  void setSpecular(float color[4])
  {
    specular[0] = color[0];
    specular[1] = color[1];
    specular[2] = color[2];
    specular[3] = color[3];
  }

  float* getPos()
  {
    return pos;
  }

  void setPos(float position[4])
  {
    pos[0] = position[0];
    pos[1] = position[1];
    pos[2] = position[2];
    pos[3] = position[3];
  }

};

/// }@

#endif // __LIGHT_H_
