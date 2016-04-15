#include "transform.h"

void ATOM_Transform::reset()
{
  _Mo2t.makeIdentity ();
}

const ATOM_Transform::Matrix44	& ATOM_Transform::getO2T() const {
  return _Mo2t;
}

ATOM_Transform::Vector3 ATOM_Transform::getOrigin() const { 
  ATOM_Transform::Matrix44 o2t = getO2T();
  return ATOM_Transform::Vector3(o2t(3, 0), o2t(3, 1), o2t(3, 2)); 
}

void ATOM_Transform::setO2T(const Matrix44 &Mo2t)
{
  _Mo2t = Mo2t;
}

void ATOM_Transform::setOrigin(const Vector3 &V)
{
  _Mo2t.setRow3 (3, V);
}

void ATOM_Transform::translate(const Vector3 &V)
{
  _Mo2t >>= Matrix44::getTranslateMatrix (V);
}

