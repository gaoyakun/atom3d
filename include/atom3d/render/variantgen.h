#ifndef __ATOM_ENGINE_VARIANTGEN_H
#define __ATOM_ENGINE_VARIANTGEN_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "../ATOM_kernel.h"

template <class T> struct ATOM_VariantGen;

template <>
struct ATOM_VariantGen<int>
{
  void generate (ATOM_Variant &var, int val) const
  {
    var.setI (val);
  }
};

template <>
struct ATOM_VariantGen<float>
{
  void generate (ATOM_Variant &var, float val) const
  {
    var.setF (val);
  }
};

template <>
struct ATOM_VariantGen<ATOM_Vector3f>
{
  void generate (ATOM_Variant &var, const ATOM_Vector3f &val) const
  {
    var.setFloatArray (val.xyz, 3);
  }
};

template <>
struct ATOM_VariantGen<ATOM_Vector4f>
{
  void generate (ATOM_Variant &var, const ATOM_Vector4f &val) const
  {
    var.setV (val);
  }
};

template <>
struct ATOM_VariantGen<ATOM_Matrix4x4f>
{
  void generate (ATOM_Variant &var, const ATOM_Matrix4x4f &val) const
  {
    var.setM (val);
  }
};
#endif // __ATOM_ENGINE_VARIANTGEN_H
/*! @} */
