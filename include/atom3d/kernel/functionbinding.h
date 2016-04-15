/**	\file functionbinding.h
 *
 *	\author ¸ßÑÅÀ¥
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_FUNCTIONBINDING_H
#define __ATOM_KERNEL_FUNCTIONBINDING_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "kernel.h"

#define ATOM_PARAM_NOCVREF(t, index) ATOM_PARAM(ATOM_RemoveConstVolatile< typename ATOM_RemoveReference< t >::type >::type, index)
#define ATOM_RESULT_NOCVREF(t) ATOM_RESULT(ATOM_RemoveConstVolatile< typename ATOM_RemoveReference< t >::type >::type)

enum {
  TYPE_NONE,
  TYPE_VOID,
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_STRING,
  TYPE_VEC2,
  TYPE_VEC3,
  TYPE_VEC4,
  TYPE_MAT33,
  TYPE_MAT44,
  TYPE_OBJECT,
  TYPE_INT_ARRAY,
  TYPE_FLOAT_ARRAY,
  TYPE_STRING_ARRAY,
  TYPE_VEC2_ARRAY,
  TYPE_VEC3_ARRAY,
  TYPE_VEC4_ARRAY,
  TYPE_MAT33_ARRAY,
  TYPE_MAT44_ARRAY,
  TYPE_OBJECT_ARRAY
};

static inline int MakeArrayType (int t) {
  switch (t)
  {
  case TYPE_NONE: return TYPE_NONE;
  case TYPE_VOID: return TYPE_VOID;
  case TYPE_INT: return TYPE_INT_ARRAY;
  case TYPE_FLOAT: return TYPE_FLOAT_ARRAY;
  case TYPE_STRING: return TYPE_STRING_ARRAY;
  case TYPE_VEC2: return TYPE_VEC2_ARRAY;
  case TYPE_VEC3: return TYPE_VEC3_ARRAY;
  case TYPE_VEC4: return TYPE_VEC4_ARRAY;
  case TYPE_MAT33: return TYPE_MAT33_ARRAY;
  case TYPE_MAT44: return TYPE_MAT44_ARRAY;
  case TYPE_OBJECT: return TYPE_OBJECT_ARRAY;
  default: return t;
  }
}

struct ATOM_FuncDescGeneratorBase {
  ATOM_RuntimeFunc desc;
};

template <class T> struct ATOM_FuncDescGenerator;
template <class T> struct ATOM_FuncDescGenerator;
template <class T> struct ATOM_AttribDescGenerator;
template <class T> struct ATOM_DataTypeGenerator;
template <class T> struct ATOM_DataTypeGeneratorNoCVREF;

template <> struct ATOM_DataTypeGeneratorNoCVREF<void> { 
  static const int type = TYPE_VOID; 
  static const char *getTypeDesc () { return "void"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<int> { 
  static const int type = TYPE_INT; 
  static const char *getTypeDesc () { return "int"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<unsigned> { 
  static const int type = TYPE_INT; 
  static const char *getTypeDesc () { return "int"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<float> { 
  static const int type = TYPE_FLOAT; 
  static const char *getTypeDesc () { return "float"; }
};
template <class E, class A> struct ATOM_DataTypeGeneratorNoCVREF<std::basic_string<E, std::char_traits<E>, A> > { 
  static const int type = TYPE_STRING; 
  static const char *getTypeDesc () { return "string"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_Vector2f> { 
  static const int type = TYPE_VEC2; 
  static const char *getTypeDesc () { return "vec2"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_Vector3f> { 
  static const int type = TYPE_VEC3; 
  static const char *getTypeDesc () { return "vec3"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_Vector4f> { 
  static const int type = TYPE_VEC4; 
  static const char *getTypeDesc () { return "vec4"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_Matrix3x3f> { 
  static const int type = TYPE_MAT33; 
  static const char *getTypeDesc () { return "mat33"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_Matrix4x4f> { 
  static const int type = TYPE_MAT44; 
  static const char *getTypeDesc () { return "mat44"; }
};
template <class T> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_AUTOREF(T)> { 
  static const int type = TYPE_OBJECT; 
  static const char *getTypeDesc () { return T::_classname(); }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_VECTOR<int> > { 
  static const int type = TYPE_INT_ARRAY; 
  static const char *getTypeDesc () { return "int"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_VECTOR<float> > { 
  static const int type = TYPE_FLOAT_ARRAY; 
  static const char *getTypeDesc () { return "float"; }
};
template <class E, class A1> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_VECTOR<std::basic_string<E, std::char_traits<E>, A1> > > { 
  static const int type = TYPE_STRING_ARRAY; 
  static const char *getTypeDesc () { return "string"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_VECTOR<ATOM_Vector2f> > { 
  static const int type = TYPE_VEC2_ARRAY; 
  static const char *getTypeDesc () { return "vec2"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_VECTOR<ATOM_Vector3f> > { 
  static const int type = TYPE_VEC3_ARRAY; 
  static const char *getTypeDesc () { return "vec3"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_VECTOR<ATOM_Vector4f> > { 
  static const int type = TYPE_VEC4_ARRAY; 
  static const char *getTypeDesc () { return "vec4"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_VECTOR<ATOM_Matrix3x3f> > { 
  static const int type = TYPE_MAT33_ARRAY; 
  static const char *getTypeDesc () { return "mat33"; }
};
template <> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_VECTOR<ATOM_Matrix4x4f> > { 
  static const int type = TYPE_MAT44_ARRAY; 
  static const char *getTypeDesc () { return "mat44"; }
};
template <class T> struct ATOM_DataTypeGeneratorNoCVREF<ATOM_VECTOR<ATOM_AUTOREF(T)> > { 
  static const int type = TYPE_OBJECT_ARRAY; 
  static const char *getTypeDesc () { return T::_classname(); }
};

template <class T>
struct ATOM_DataTypeGenerator: public ATOM_DataTypeGeneratorNoCVREF<
  typename ATOM_RemoveConstVolatile<typename ATOM_RemoveReference<T>::type>::type>
{
};

template <class T>
struct ATOM_AttribDescGenerator
{
  ATOM_RuntimeAttrib attrib;
  ATOM_AttribDescGenerator(const char *name, const char *comment) {
    attrib.name = name;
    attrib.type = ATOM_DataTypeGenerator<T>::type;
    attrib.typedesc = ATOM_DataTypeGenerator<T>::getTypeDesc();
	attrib.comment = comment ? comment : "";
  }
};

template <class T>
struct ATOM_FuncDescGenerator<T (*)(void)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T>::getTypeDesc();
    desc.paramtypes.clear();
    desc.paramtypedescs.clear();
  }
};

template <class T1, class T2>
struct ATOM_FuncDescGenerator<T1 (*)(T2)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(1);
    desc.paramtypedescs.resize(1);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
  }
};

template <class T1, class T2, class T3>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(2);
    desc.paramtypedescs.resize(2);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(3);
    desc.paramtypedescs.resize(3);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(4);
    desc.paramtypedescs.resize(4);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5, T6)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(5);
    desc.paramtypedescs.resize(5);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypes[4] = ATOM_DataTypeGenerator<T6>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
    desc.paramtypedescs[4] = ATOM_DataTypeGenerator<T6>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5, T6, T7)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(6);
    desc.paramtypedescs.resize(6);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypes[4] = ATOM_DataTypeGenerator<T6>::type;
    desc.paramtypes[5] = ATOM_DataTypeGenerator<T7>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
    desc.paramtypedescs[4] = ATOM_DataTypeGenerator<T6>::getTypeDesc();
    desc.paramtypedescs[5] = ATOM_DataTypeGenerator<T7>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5, T6, T7, T8)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(7);
    desc.paramtypedescs.resize(7);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypes[4] = ATOM_DataTypeGenerator<T6>::type;
    desc.paramtypes[5] = ATOM_DataTypeGenerator<T7>::type;
    desc.paramtypes[6] = ATOM_DataTypeGenerator<T8>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
    desc.paramtypedescs[4] = ATOM_DataTypeGenerator<T6>::getTypeDesc();
    desc.paramtypedescs[5] = ATOM_DataTypeGenerator<T7>::getTypeDesc();
    desc.paramtypedescs[6] = ATOM_DataTypeGenerator<T8>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5, T6, T7, T8, T9)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(8);
    desc.paramtypedescs.resize(8);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypes[4] = ATOM_DataTypeGenerator<T6>::type;
    desc.paramtypes[5] = ATOM_DataTypeGenerator<T7>::type;
    desc.paramtypes[6] = ATOM_DataTypeGenerator<T8>::type;
    desc.paramtypes[7] = ATOM_DataTypeGenerator<T9>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
    desc.paramtypedescs[4] = ATOM_DataTypeGenerator<T6>::getTypeDesc();
    desc.paramtypedescs[5] = ATOM_DataTypeGenerator<T7>::getTypeDesc();
    desc.paramtypedescs[6] = ATOM_DataTypeGenerator<T8>::getTypeDesc();
    desc.paramtypedescs[7] = ATOM_DataTypeGenerator<T9>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5, T6, T7, T8, T9, T10)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(9);
    desc.paramtypedescs.resize(9);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypes[4] = ATOM_DataTypeGenerator<T6>::type;
    desc.paramtypes[5] = ATOM_DataTypeGenerator<T7>::type;
    desc.paramtypes[6] = ATOM_DataTypeGenerator<T8>::type;
    desc.paramtypes[7] = ATOM_DataTypeGenerator<T9>::type;
    desc.paramtypes[8] = ATOM_DataTypeGenerator<T10>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
    desc.paramtypedescs[4] = ATOM_DataTypeGenerator<T6>::getTypeDesc();
    desc.paramtypedescs[5] = ATOM_DataTypeGenerator<T7>::getTypeDesc();
    desc.paramtypedescs[6] = ATOM_DataTypeGenerator<T8>::getTypeDesc();
    desc.paramtypedescs[7] = ATOM_DataTypeGenerator<T9>::getTypeDesc();
    desc.paramtypedescs[8] = ATOM_DataTypeGenerator<T10>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10,
          class T11>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(10);
    desc.paramtypedescs.resize(10);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypes[4] = ATOM_DataTypeGenerator<T6>::type;
    desc.paramtypes[5] = ATOM_DataTypeGenerator<T7>::type;
    desc.paramtypes[6] = ATOM_DataTypeGenerator<T8>::type;
    desc.paramtypes[7] = ATOM_DataTypeGenerator<T9>::type;
    desc.paramtypes[8] = ATOM_DataTypeGenerator<T10>::type;
    desc.paramtypes[9] = ATOM_DataTypeGenerator<T11>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
    desc.paramtypedescs[4] = ATOM_DataTypeGenerator<T6>::getTypeDesc();
    desc.paramtypedescs[5] = ATOM_DataTypeGenerator<T7>::getTypeDesc();
    desc.paramtypedescs[6] = ATOM_DataTypeGenerator<T8>::getTypeDesc();
    desc.paramtypedescs[7] = ATOM_DataTypeGenerator<T9>::getTypeDesc();
    desc.paramtypedescs[8] = ATOM_DataTypeGenerator<T10>::getTypeDesc();
    desc.paramtypedescs[9] = ATOM_DataTypeGenerator<T11>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10,
          class T11, class T12>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(11);
    desc.paramtypedescs.resize(11);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypes[4] = ATOM_DataTypeGenerator<T6>::type;
    desc.paramtypes[5] = ATOM_DataTypeGenerator<T7>::type;
    desc.paramtypes[6] = ATOM_DataTypeGenerator<T8>::type;
    desc.paramtypes[7] = ATOM_DataTypeGenerator<T9>::type;
    desc.paramtypes[8] = ATOM_DataTypeGenerator<T10>::type;
    desc.paramtypes[9] = ATOM_DataTypeGenerator<T11>::type;
    desc.paramtypes[10] = ATOM_DataTypeGenerator<T12>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
    desc.paramtypedescs[4] = ATOM_DataTypeGenerator<T6>::getTypeDesc();
    desc.paramtypedescs[5] = ATOM_DataTypeGenerator<T7>::getTypeDesc();
    desc.paramtypedescs[6] = ATOM_DataTypeGenerator<T8>::getTypeDesc();
    desc.paramtypedescs[7] = ATOM_DataTypeGenerator<T9>::getTypeDesc();
    desc.paramtypedescs[8] = ATOM_DataTypeGenerator<T10>::getTypeDesc();
    desc.paramtypedescs[9] = ATOM_DataTypeGenerator<T11>::getTypeDesc();
    desc.paramtypedescs[10] = ATOM_DataTypeGenerator<T12>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10,
          class T11, class T12, class T13>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
	desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(12);
    desc.paramtypedescs.resize(12);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypes[4] = ATOM_DataTypeGenerator<T6>::type;
    desc.paramtypes[5] = ATOM_DataTypeGenerator<T7>::type;
    desc.paramtypes[6] = ATOM_DataTypeGenerator<T8>::type;
    desc.paramtypes[7] = ATOM_DataTypeGenerator<T9>::type;
    desc.paramtypes[8] = ATOM_DataTypeGenerator<T10>::type;
    desc.paramtypes[9] = ATOM_DataTypeGenerator<T11>::type;
    desc.paramtypes[10] = ATOM_DataTypeGenerator<T12>::type;
    desc.paramtypes[11] = ATOM_DataTypeGenerator<T13>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
    desc.paramtypedescs[4] = ATOM_DataTypeGenerator<T6>::getTypeDesc();
    desc.paramtypedescs[5] = ATOM_DataTypeGenerator<T7>::getTypeDesc();
    desc.paramtypedescs[6] = ATOM_DataTypeGenerator<T8>::getTypeDesc();
    desc.paramtypedescs[7] = ATOM_DataTypeGenerator<T9>::getTypeDesc();
    desc.paramtypedescs[8] = ATOM_DataTypeGenerator<T10>::getTypeDesc();
    desc.paramtypedescs[9] = ATOM_DataTypeGenerator<T11>::getTypeDesc();
    desc.paramtypedescs[10] = ATOM_DataTypeGenerator<T12>::getTypeDesc();
    desc.paramtypedescs[11] = ATOM_DataTypeGenerator<T13>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10,
          class T11, class T12, class T13, class T14>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
    desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(13);
    desc.paramtypedescs.resize(13);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypes[4] = ATOM_DataTypeGenerator<T6>::type;
    desc.paramtypes[5] = ATOM_DataTypeGenerator<T7>::type;
    desc.paramtypes[6] = ATOM_DataTypeGenerator<T8>::type;
    desc.paramtypes[7] = ATOM_DataTypeGenerator<T9>::type;
    desc.paramtypes[8] = ATOM_DataTypeGenerator<T10>::type;
    desc.paramtypes[9] = ATOM_DataTypeGenerator<T11>::type;
    desc.paramtypes[10] = ATOM_DataTypeGenerator<T12>::type;
    desc.paramtypes[11] = ATOM_DataTypeGenerator<T13>::type;
    desc.paramtypes[12] = ATOM_DataTypeGenerator<T14>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
    desc.paramtypedescs[4] = ATOM_DataTypeGenerator<T6>::getTypeDesc();
    desc.paramtypedescs[5] = ATOM_DataTypeGenerator<T7>::getTypeDesc();
    desc.paramtypedescs[6] = ATOM_DataTypeGenerator<T8>::getTypeDesc();
    desc.paramtypedescs[7] = ATOM_DataTypeGenerator<T9>::getTypeDesc();
    desc.paramtypedescs[8] = ATOM_DataTypeGenerator<T10>::getTypeDesc();
    desc.paramtypedescs[9] = ATOM_DataTypeGenerator<T11>::getTypeDesc();
    desc.paramtypedescs[10] = ATOM_DataTypeGenerator<T12>::getTypeDesc();
    desc.paramtypedescs[11] = ATOM_DataTypeGenerator<T13>::getTypeDesc();
    desc.paramtypedescs[12] = ATOM_DataTypeGenerator<T14>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10,
          class T11, class T12, class T13, class T14, class T15>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
    desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(14);
    desc.paramtypedescs.resize(14);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypes[4] = ATOM_DataTypeGenerator<T6>::type;
    desc.paramtypes[5] = ATOM_DataTypeGenerator<T7>::type;
    desc.paramtypes[6] = ATOM_DataTypeGenerator<T8>::type;
    desc.paramtypes[7] = ATOM_DataTypeGenerator<T9>::type;
    desc.paramtypes[8] = ATOM_DataTypeGenerator<T10>::type;
    desc.paramtypes[9] = ATOM_DataTypeGenerator<T11>::type;
    desc.paramtypes[10] = ATOM_DataTypeGenerator<T12>::type;
    desc.paramtypes[11] = ATOM_DataTypeGenerator<T13>::type;
    desc.paramtypes[12] = ATOM_DataTypeGenerator<T14>::type;
    desc.paramtypes[13] = ATOM_DataTypeGenerator<T15>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
    desc.paramtypedescs[4] = ATOM_DataTypeGenerator<T6>::getTypeDesc();
    desc.paramtypedescs[5] = ATOM_DataTypeGenerator<T7>::getTypeDesc();
    desc.paramtypedescs[6] = ATOM_DataTypeGenerator<T8>::getTypeDesc();
    desc.paramtypedescs[7] = ATOM_DataTypeGenerator<T9>::getTypeDesc();
    desc.paramtypedescs[8] = ATOM_DataTypeGenerator<T10>::getTypeDesc();
    desc.paramtypedescs[9] = ATOM_DataTypeGenerator<T11>::getTypeDesc();
    desc.paramtypedescs[10] = ATOM_DataTypeGenerator<T12>::getTypeDesc();
    desc.paramtypedescs[11] = ATOM_DataTypeGenerator<T13>::getTypeDesc();
    desc.paramtypedescs[12] = ATOM_DataTypeGenerator<T14>::getTypeDesc();
    desc.paramtypedescs[13] = ATOM_DataTypeGenerator<T15>::getTypeDesc();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10,
          class T11, class T12, class T13, class T14, class T15, class T16>
struct ATOM_FuncDescGenerator<T1 (*)(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>: public ATOM_FuncDescGeneratorBase
{
  ATOM_FuncDescGenerator(const char *name, const char *comment) {
    desc.name = name;
    desc.comment = comment ? comment : "";
    desc.returntype = ATOM_DataTypeGenerator<T1>::type;
    desc.returntypedesc = ATOM_DataTypeGenerator<T1>::getTypeDesc();
    desc.paramtypes.resize(15);
    desc.paramtypedescs.resize(15);
    desc.paramtypes[0] = ATOM_DataTypeGenerator<T2>::type;
    desc.paramtypes[1] = ATOM_DataTypeGenerator<T3>::type;
    desc.paramtypes[2] = ATOM_DataTypeGenerator<T4>::type;
    desc.paramtypes[3] = ATOM_DataTypeGenerator<T5>::type;
    desc.paramtypes[4] = ATOM_DataTypeGenerator<T6>::type;
    desc.paramtypes[5] = ATOM_DataTypeGenerator<T7>::type;
    desc.paramtypes[6] = ATOM_DataTypeGenerator<T8>::type;
    desc.paramtypes[7] = ATOM_DataTypeGenerator<T9>::type;
    desc.paramtypes[8] = ATOM_DataTypeGenerator<T10>::type;
    desc.paramtypes[9] = ATOM_DataTypeGenerator<T11>::type;
    desc.paramtypes[10] = ATOM_DataTypeGenerator<T12>::type;
    desc.paramtypes[11] = ATOM_DataTypeGenerator<T13>::type;
    desc.paramtypes[12] = ATOM_DataTypeGenerator<T14>::type;
    desc.paramtypes[13] = ATOM_DataTypeGenerator<T15>::type;
    desc.paramtypes[14] = ATOM_DataTypeGenerator<T16>::type;
    desc.paramtypedescs[0] = ATOM_DataTypeGenerator<T2>::getTypeDesc();
    desc.paramtypedescs[1] = ATOM_DataTypeGenerator<T3>::getTypeDesc();
    desc.paramtypedescs[2] = ATOM_DataTypeGenerator<T4>::getTypeDesc();
    desc.paramtypedescs[3] = ATOM_DataTypeGenerator<T5>::getTypeDesc();
    desc.paramtypedescs[4] = ATOM_DataTypeGenerator<T6>::getTypeDesc();
    desc.paramtypedescs[5] = ATOM_DataTypeGenerator<T7>::getTypeDesc();
    desc.paramtypedescs[6] = ATOM_DataTypeGenerator<T8>::getTypeDesc();
    desc.paramtypedescs[7] = ATOM_DataTypeGenerator<T9>::getTypeDesc();
    desc.paramtypedescs[8] = ATOM_DataTypeGenerator<T10>::getTypeDesc();
    desc.paramtypedescs[9] = ATOM_DataTypeGenerator<T11>::getTypeDesc();
    desc.paramtypedescs[10] = ATOM_DataTypeGenerator<T12>::getTypeDesc();
    desc.paramtypedescs[11] = ATOM_DataTypeGenerator<T13>::getTypeDesc();
    desc.paramtypedescs[12] = ATOM_DataTypeGenerator<T14>::getTypeDesc();
    desc.paramtypedescs[13] = ATOM_DataTypeGenerator<T15>::getTypeDesc();
    desc.paramtypedescs[14] = ATOM_DataTypeGenerator<T16>::getTypeDesc();
  }
};

struct ATOM_AttribAccessorBase {
  ATOM_RuntimeAttrib attrib;
  virtual void Set (ATOM_Object *, const void*) = 0;
  virtual bool GetDefaultValue (void *) = 0;
  virtual void Get (const ATOM_Object *, void *) = 0;
  virtual bool IsReadonly () = 0;
  virtual bool IsPersistent () = 0;
  virtual bool HasDefaultValue () = 0;
};

struct ATOM_VariableAccessorBase {
  ATOM_RuntimeAttrib attrib;
  virtual const void * Get () const = 0;
};

template <class T> struct ATOM_VariableAccessorBaseT: public ATOM_VariableAccessorBase {
  T variable;
  ATOM_VariableAccessorBaseT () {}
  ATOM_VariableAccessorBaseT (const char *name, const char *comment, T var = T()): variable(var) { 
    attrib = ATOM_AttribDescGenerator<T>(name, comment).attrib; 
  }
  virtual const void * Get () const { return &variable; }
};

template <class Tc, class T1, class T2>
struct ATOM_AttribAccessor: public ATOM_AttribAccessorBase {
  typedef typename ATOM_RemoveConstVolatile<typename ATOM_RemoveReference<T1>::type>::type _T1;
  typedef typename ATOM_RemoveConstVolatile<typename ATOM_RemoveReference<T2>::type>::type _T2;
  ATOM_STATIC_ASSERT((ATOM_IsSame<_T1,_T2>::result));

  void (Tc::*SetFunc)(T1);
  T2 (Tc::*GetFunc)() const;

  ATOM_AttribAccessor (const char *name, void (Tc::*sf)(T1), T2 (Tc::*gf)() const, const char *comment): SetFunc(sf),GetFunc(gf) {
    attrib.name = name;
	attrib.comment = comment ? comment : "";
    attrib.type = ATOM_DataTypeGenerator<_T1>::type;
    attrib.typedesc = ATOM_DataTypeGenerator<_T1>::getTypeDesc();
  }

  virtual void Set (ATOM_Object *p, const void *value) {
    if (SetFunc) {
      Tc *pc = dynamic_cast<Tc*>(p);
      (pc->*SetFunc)(*((const _T1*)value));
    }
  }

  virtual bool GetDefaultValue (void *value) {
    return false;
  }

  virtual void Get (const ATOM_Object *p, void *value) {
    ATOM_ASSERT (GetFunc);
    const Tc *pc = dynamic_cast<const Tc*>(p);
    *((_T2*)value) = (pc->*GetFunc)();
  }

  virtual bool IsReadonly () {
    return SetFunc == 0;
  }

  virtual bool HasDefaultValue () {
    return false;
  }

  virtual bool IsPersistent () {
    return false;
  }
};

template <class Tc, class T1, class T2, class T3>
struct PersistentAttribAccessor: public ATOM_AttribAccessor<Tc, T1, T2> {
  typedef typename ATOM_AttribAccessor<Tc, T1, T2>::_T1 _T1;
  typedef typename ATOM_AttribAccessor<Tc, T1, T2>::_T2 _T2;
  typedef typename ATOM_RemoveConstVolatile<typename ATOM_RemoveReference<T3>::type>::type _T3;
//      ATOM_STATIC_ASSERT((ATOM_IsSame<_T1,_T3>::result));
  _T1 defaultValue;
  bool hasDefaultValue;

  PersistentAttribAccessor (const char *name, void (Tc::*sf)(T1), T2 (Tc::*gf)() const, const _T3 *dv, const char *comment)
    : ATOM_AttribAccessor<Tc, T1, T2>(name, sf, gf, comment), hasDefaultValue(true), defaultValue(*dv) {
  }

  PersistentAttribAccessor (const char *name, void (Tc::*sf)(T1), T2 (Tc::*gf)() const, const char *comment)
    : ATOM_AttribAccessor<Tc, T1, T2>(name, sf, gf, comment), hasDefaultValue(false) {
  }

  virtual bool GetDefaultValue (void *value) {
    if (hasDefaultValue)
    {
      *((_T2*)value) = defaultValue;
      return true;
    }
    return false;
  }

  virtual bool HasDefaultValue () {
    return hasDefaultValue;
  }

  virtual bool IsPersistent () {
    return true;
  }
};

template <class Tc, class T1, class T2>
ATOM_AttribAccessorBase *ATOM_AttribAccessorGenerate (const char *name, void (Tc::*sf)(T1), T2 (Tc::*gf)() const, const char *comment) {
  typedef ATOM_AttribAccessor <Tc, T1, T2> C;
  return ATOM_NEW(C, name, sf, gf, comment);
}

template <class Tc, class T>
ATOM_AttribAccessorBase *ATOM_ReadonlyAttribAccessorGenerate (const char *name, T (Tc::*gf)() const, const char *comment) {
  typedef ATOM_AttribAccessor <Tc, T, T> C;
  void (Tc::*sf) (T) = 0;
  return ATOM_NEW(C, name, sf, gf, comment);
}

template <class Tc, class T1, class T2, class T3>
ATOM_AttribAccessorBase *ATOM_PersistentAttribAccessorGenerate (const char *name, void (Tc::*sf)(T1), T2 (Tc::*gf)() const, const T3 &dv, const char *comment) {
  typedef PersistentAttribAccessor <Tc, T1, T2, T3> C;
  return ATOM_NEW(C, name, sf, gf, &dv, comment);
}

template <class Tc, class T1, class T2>
ATOM_AttribAccessorBase *ATOM_NoDefaultPersistentAttribAccessorGenerate (const char *name, void (Tc::*sf)(T1), T2 (Tc::*gf)() const, const char *comment) {
  typedef PersistentAttribAccessor <Tc, T1, T2, T2> C;
  return ATOM_NEW(C, name, sf, gf, comment);
}

template <class Tc, class T1, class T2>
ATOM_AttribAccessorBase *ATOM_ReadonlyPersistentAttribAccessorGenerate (const char *name, T1 (Tc::*gf)() const, const T2 &dv, const char *comment) {
  typedef PersistentAttribAccessor <Tc, T1, T1, T2> C;
  void (Tc::*sf)(T1) = 0;
  return ATOM_NEW(C, name, sf, gf, &dv, comment);
}

template <class Tc, class T1>
ATOM_AttribAccessorBase *ATOM_NoDefaultReadonlyPersistentAttribAccessorGenerate (const char *name, T1 (Tc::*gf)() const, const char *comment) {
  typedef PersistentAttribAccessor <Tc, T1, T1, T1> C;
  void (Tc::*sf)(T1) = 0;
  return ATOM_NEW(C, name, sf, gf, comment);
}

#endif // __ATOM_KERNEL_FUNCTIONBINDING_H
/*! @} */
