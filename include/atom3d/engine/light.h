#ifndef __ATOM3D_ENGINE_LIGHT_H
#define __ATOM3D_ENGINE_LIGHT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_utils.h"
#include "node.h"

class ATOM_ENGINE_API ATOM_Light: public ATOM_ReferenceObj
{
public:
  enum LightType
  {
    Unknown = -1,
    Directional = 0,
    Point,
    Spot
  };

public:
  ATOM_Light (void);
  ATOM_Light & operator = (const ATOM_Light &other);

  void setColorARGB (const ATOM_ColorARGB &color);
  void setColor4f (const ATOM_Vector4f &color);
  ATOM_ColorARGB getColorARGB (void) const;
  const ATOM_Vector4f &getColor4f (void) const;
  void setCastShadow (int cast);
  int getCastShadow (void) const;
  void setRange (const ATOM_Vector3f &range);
  const ATOM_Vector3f &getRange (void) const;
  void setLightType (ATOM_Light::LightType type);
  ATOM_Light::LightType getLightType (void) const;
  void setInnerCone (float angle);
  float getInnerCone (void) const;
  void setOuterCone (float angle);
  float getOuterCone (void) const;
  void setAttenuation (const ATOM_Vector3f &atten);
  const ATOM_Vector3f &getAttenuation (void) const;
  //---- wangjian added ----//
  void setHasAttenuation(int has);
  bool hasAttenuationParam() const;
  //------------------------//
  const ATOM_Vector3f &getDirection (void) const;
  void setDirection (const ATOM_Vector3f &v);
  const ATOM_Vector3f &getPosition (void) const;
  void setPosition (const ATOM_Vector3f &v);

protected:
  LightType _type;
  int _castShadow;
  ATOM_Vector4f _color;
  ATOM_Vector3f _range;
  float _innerCone;
  float _outerCone;
  ATOM_Vector3f _attenuation;

  // wangjian added
  int			_hasAttenuation;
  
  ATOM_Vector3f _direction;
  ATOM_Vector3f _position;
};

#endif // __ATOM3D_ENGINE_LIGHT_H
