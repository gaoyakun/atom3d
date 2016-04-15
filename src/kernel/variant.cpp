#include "StdAfx.h"
#include "variant.h"

static inline void _cleanVariantInternal (ATOM_Variant::VariantInternal *p)
{
  ATOM_ASSERT(p);
  if ((p->type & 0x80000000) != 0 && p->dummy)
  {
    ATOM_FREE(p->dummy);
	p->dummy = 0;
  }
  else if (p->type == ATOM_Variant::STRING)
  {
    ATOM_FREE(p->s);
	p->s = 0;
  }
  p->memsize = 0;
  p->dummy = 0;
  p->type = ATOM_Variant::NONE;
  p->count = 0;
}

static inline void _resizeVariantInternal (ATOM_Variant::VariantInternal *p, unsigned size)
{
  ATOM_ASSERT(p);

  switch (p->type)
  {
  case ATOM_Variant::STRING:
    ATOM_FREE(p->s);
	p->memsize = 0;
	p->s = 0;
    break;
  default:
    break;
  }

  if (p->memsize < size)
  {
    if ((p->type & 0x80000000) != 0)
    {
      ATOM_FREE(p->dummy);
	  p->dummy = 0;
    }

    p->dummy = ATOM_MALLOC(size);
    p->memsize = size;
  }
}

static inline void _VariantInternalSetV (ATOM_Variant::VariantInternal *p, const ATOM_Vector4f &v)
{
  ATOM_ASSERT(p);
  _cleanVariantInternal (p);
  memcpy (p->v, v.xyzw, 4 * sizeof(float));
  p->count = 0;
  p->type = ATOM_Variant::VECTOR4;
}

static inline void _VariantInternalSetM (ATOM_Variant::VariantInternal *p, const ATOM_Matrix4x4f &m)
{
  ATOM_ASSERT(p);
  _cleanVariantInternal (p);
  memcpy (p->m, m.m, 16 * sizeof(float));
  p->count = 0;
  p->type = ATOM_Variant::MATRIX44;
}

static inline void _VariantInternalSetI (ATOM_Variant::VariantInternal *p, int i)
{
  ATOM_ASSERT(p);
  _cleanVariantInternal (p);
  p->i = i;
  p->count = 0;
  p->type = ATOM_Variant::INT;
}

static inline void _VariantInternalSetIA (ATOM_Variant::VariantInternal *p, const int *i, unsigned count)
{
  ATOM_ASSERT(p);

  if (count == 0)
  {
    _cleanVariantInternal (p);
  }
  else
  {
    ATOM_ASSERT(i);
    _resizeVariantInternal (p, count * sizeof(int));
    memcpy (p->i_array, i, count * sizeof(int));
  }

  p->type = ATOM_Variant::INT_ARRAY;
  p->count = count;
}

static inline void _VariantInternalSetF (ATOM_Variant::VariantInternal *p, float f)
{
  ATOM_ASSERT(p);
  _cleanVariantInternal (p);
  p->f = f;
  p->count = 0;
  p->type = ATOM_Variant::FLOAT;
}

static inline void _VariantInternalSetFA (ATOM_Variant::VariantInternal *p, const float *f, unsigned count)
{
  ATOM_ASSERT(p);

  if (count == 0)
  {
    _cleanVariantInternal (p);
  }
  else
  {
    ATOM_ASSERT(f);
    _resizeVariantInternal (p, count * sizeof(float));
    memcpy (p->f_array, f, count * sizeof(float));
  }

  p->type = ATOM_Variant::FLOAT_ARRAY;
  p->count = count;
}

static inline void _VariantInternalSetS (ATOM_Variant::VariantInternal *p, const char *s)
{
  ATOM_ASSERT(p);
  _cleanVariantInternal (p);

  if (s)
  {
    p->s = (char*)ATOM_MALLOC(strlen(s)+1);
    strcpy (p->s, s);
  }
  else
  {
    p->s = 0;
  }

  p->count = 0;
  p->type = ATOM_Variant::STRING;
}

static inline void _VariantInternalSetP (ATOM_Variant::VariantInternal *p, void *ptr)
{
  ATOM_ASSERT(p);
  _cleanVariantInternal (p);
  p->p = ptr;
  p->count = 0;
  p->type = ATOM_Variant::POINTER;
}

static inline void _cloneVariantInternal (ATOM_Variant::VariantInternal *r, const ATOM_Variant::VariantInternal *p)
{
  ATOM_ASSERT(p);
  switch (p->type)
  {
  case ATOM_Variant::INT:
    _VariantInternalSetI (r, p->i);
    break;
  case ATOM_Variant::INT_ARRAY:
    _VariantInternalSetIA (r, p->i_array, p->count);
    break;
  case ATOM_Variant::FLOAT:
    _VariantInternalSetF (r, p->f);
    break;
  case ATOM_Variant::FLOAT_ARRAY:
    _VariantInternalSetFA (r, p->f_array, p->count);
    break;
  case ATOM_Variant::STRING:
    _VariantInternalSetS (r, p->s);
    break;
  case ATOM_Variant::POINTER:
    _VariantInternalSetP (r, p->p);
    break;
  case ATOM_Variant::VECTOR4:
    _VariantInternalSetV (r, ATOM_Vector4f(p->v[0], p->v[1], p->v[2], p->v[3]));
	break;
  case ATOM_Variant::MATRIX44:
    _VariantInternalSetM (r, ATOM_Matrix4x4f(p->m[0], p->m[1], p->m[2], p->m[3], p->m[4], p->m[5], p->m[6], p->m[7], p->m[8], p->m[9], p->m[10], p->m[11], p->m[12], p->m[13], p->m[14], p->m[15]));
	break;
  default:
    break;
  }

  r->type = p->type;
}

static inline int _CmpStr (const char *s1, const char *s2)
{
  if (s1 == s2)
  {
    return 0;
  }

  if (!s1)
  {
    return -1;
  }

  if (!s2)
  {
    return 1;
  }

  return strcmp (s1, s2);
}

ATOM_Variant::ATOM_Variant (void)
{
	_data.type = NONE;
}

ATOM_Variant::ATOM_Variant (int i)
{
  _VariantInternalSetI (&_data, i);
}

ATOM_Variant::ATOM_Variant (const int *i, unsigned count)
{
  _VariantInternalSetIA (&_data, i, count);
}

ATOM_Variant::ATOM_Variant (float f)
{
  _VariantInternalSetF (&_data, f);
}

ATOM_Variant::ATOM_Variant (const float *f, unsigned count)
{
  _VariantInternalSetFA (&_data, f, count);
}

ATOM_Variant::ATOM_Variant (const char *s)
{
  _VariantInternalSetS (&_data, s);
}

ATOM_Variant::ATOM_Variant (void *p)
{
  _VariantInternalSetP (&_data, p);
}

ATOM_Variant::ATOM_Variant (const ATOM_Vector4f &v)
{
	_VariantInternalSetV (&_data, v);
}

ATOM_Variant::ATOM_Variant (const ATOM_Matrix4x4f &m)
{
	_VariantInternalSetM (&_data, m);
}

ATOM_Variant::ATOM_Variant (const ATOM_Variant &rhs)
{
  _cloneVariantInternal (&_data, &rhs._data);
}

ATOM_Variant::~ATOM_Variant (void)
{
  _cleanVariantInternal (&_data);
}

ATOM_Variant &ATOM_Variant::operator = (const ATOM_Variant &rhs)
{
  if (this != &rhs)
  {
	_cloneVariantInternal (&_data, &rhs._data);
  }
  return *this;
}

void ATOM_Variant::swap (ATOM_Variant &rhs)
{
	VariantInternal tmp = rhs._data;
	_data = rhs._data;
	rhs._data = tmp;
}

void ATOM_Variant::invalidate (void)
{
  _cleanVariantInternal (&_data);
}

bool ATOM_Variant::isArray (void) const
{
  return (_data.type & 0x80000000) != 0;
}

bool ATOM_Variant::isEmpty (void) const
{
  return _data.type == ATOM_Variant::NONE;
}

unsigned ATOM_Variant::getArraySize (void) const
{
  return _data.count;
}

int ATOM_Variant::getI (void) const
{
  return _data.i;
}

int ATOM_Variant::getI (unsigned index) const
{
  return _data.i_array[index];
}

const int *ATOM_Variant::getIntArray (void) const
{
  return _data.i_array;
}

void ATOM_Variant::setI (int i)
{
  _VariantInternalSetI (&_data, i);
}

void ATOM_Variant::setIntArray (const int *i, unsigned count)
{
  _VariantInternalSetIA (&_data, i, count);
}

float ATOM_Variant::getF (void) const
{
  return _data.f;
}

float ATOM_Variant::getF (unsigned index) const
{
  return _data.f_array[index];
}

const float *ATOM_Variant::getFloatArray (void) const
{
  return _data.f_array;
}

void ATOM_Variant::setF (float f)
{
  _VariantInternalSetF (&_data, f);
}

void ATOM_Variant::setFloatArray (const float *f, unsigned count)
{
  _VariantInternalSetFA (&_data, f, count);
}

const char *ATOM_Variant::getS (void) const
{
  return _data.s;
}

void ATOM_Variant::setS (const char *s)
{
  _VariantInternalSetS (&_data, s);
}

void *ATOM_Variant::getP (void) const
{
  return _data.p;
}

void ATOM_Variant::setP (void *p)
{
  _VariantInternalSetP (&_data, p);
}

void ATOM_Variant::setV (const ATOM_Vector4f &v)
{
	_VariantInternalSetV (&_data, v);
}

const float *ATOM_Variant::getV (void) const
{
	return _data.v;
}

void ATOM_Variant::setM (const ATOM_Matrix4x4f &m)
{
	_VariantInternalSetM (&_data, m);
}

const float *ATOM_Variant::getM (void) const
{
	return _data.m;
}

ATOM_Variant::Type ATOM_Variant::getType (void) const
{
  return _data.type;
}

ATOM_KERNEL_API bool ATOM_CALL operator == (const ATOM_Variant &left, const ATOM_Variant &right)
{
  if (left._data.type == right._data.type)
  {
    switch (left._data.type)
    {
    case ATOM_Variant::NONE:
      return true;
    case ATOM_Variant::INT:
      return left._data.i == right._data.i;
    case ATOM_Variant::INT_ARRAY:
      return left._data.count == right._data.count && (left._data.count == 0 || !memcmp(left._data.i_array, right._data.i_array, left._data.count * sizeof(int)));
    case ATOM_Variant::FLOAT:
      return left._data.f == right._data.f;
    case ATOM_Variant::FLOAT_ARRAY:
      return left._data.count == right._data.count && (left._data.count == 0 || !memcmp(left._data.f_array, right._data.f_array, left._data.count * sizeof(float)));
	case ATOM_Variant::VECTOR4:
	  return !memcmp (left._data.v, right._data.v, 4 * sizeof(float));
	case ATOM_Variant::MATRIX44:
	  return !memcmp (left._data.v, right._data.v, 16 * sizeof(float));
    case ATOM_Variant::STRING:
      return _CmpStr (left._data.s, right._data.s) == 0;
    case ATOM_Variant::POINTER:
      return left._data.p == right._data.p;
    }
  }
  return false;
}

ATOM_KERNEL_API bool ATOM_CALL operator != (const ATOM_Variant &left, const ATOM_Variant &right)
{
  return ! operator == (left, right);
}

