#include "stdafx.h"

typedef ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > ObjectVector;

ATOM_ScriptVar::ATOM_ScriptVar (void) 
{
	_M_type = TYPE_NONE;
	_M_integer = 0;
}

ATOM_ScriptVar::~ATOM_ScriptVar (void) 
{
	deleteStorage ();
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_ScriptVar &rhs) 
{
	_M_type = rhs._M_type;

	switch (_M_type)
	{
	case TYPE_INT: 
	case TYPE_INT_ARRAY:
		_M_integer = ATOM_NEW(ATOM_VECTOR<int>, *rhs._M_integer); 
		break;
	case TYPE_FLOAT: 
	case TYPE_FLOAT_ARRAY:
		_M_float = ATOM_NEW(ATOM_VECTOR<float>, *rhs._M_float); 
		break;
	case TYPE_STRING: 
	case TYPE_STRING_ARRAY:
		_M_string = ATOM_NEW(ATOM_VECTOR<ATOM_STRING>, *rhs._M_string); 
		break;
	case TYPE_VEC2: 
	case TYPE_VEC2_ARRAY:
		_M_vec2 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector2f>, *rhs._M_vec2); 
		break;
	case TYPE_VEC3: 
	case TYPE_VEC3_ARRAY:
		_M_vec3 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector3f>, *rhs._M_vec3); 
		break;
	case TYPE_VEC4: 
	case TYPE_VEC4_ARRAY:
		_M_vec4 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector4f>, *rhs._M_vec4); 
		break;
	case TYPE_MAT33: 
	case TYPE_MAT33_ARRAY:
		_M_mat33 = ATOM_NEW(ATOM_VECTOR<ATOM_Matrix3x3f>, *rhs._M_mat33); 
		break;
	case TYPE_MAT44: 
	case TYPE_MAT44_ARRAY:
		_M_mat44 = ATOM_NEW(ATOM_VECTOR<ATOM_Matrix4x4f>, *rhs._M_mat44); 
		break;
	case TYPE_OBJECT: 
	case TYPE_OBJECT_ARRAY:
		_M_object = ATOM_NEW(ObjectVector, *rhs._M_object); 
		break;
	default:
		break;
	}
}

ATOM_ScriptVar::ATOM_ScriptVar (int value) 
{
	_M_type = TYPE_INT;
	_M_integer = ATOM_NEW(ATOM_VECTOR<int>);
	_M_integer->push_back (value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_VECTOR<int> &value) 
{
	_M_type = TYPE_INT_ARRAY;
	_M_integer = ATOM_NEW(ATOM_VECTOR<int>, value);
}

ATOM_ScriptVar::ATOM_ScriptVar (float value) 
{
	_M_type = TYPE_FLOAT;
	_M_float = ATOM_NEW(ATOM_VECTOR<float>);
	_M_float->push_back (value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_VECTOR<float> &value) 
{
	_M_type = TYPE_FLOAT_ARRAY;
	_M_float = ATOM_NEW(ATOM_VECTOR<float>, value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_STRING &value) 
{
	_M_type = TYPE_STRING;
	_M_string = ATOM_NEW(ATOM_VECTOR<ATOM_STRING>);
	_M_string->push_back (value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_VECTOR<ATOM_STRING> &value) 
{
	_M_type = TYPE_STRING_ARRAY;
	_M_string = ATOM_NEW( ATOM_VECTOR<ATOM_STRING>, value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_Vector2f &value) 
{
	_M_type = TYPE_VEC2;
	_M_vec2 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector2f>);
	_M_vec2->push_back (value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_Vector3f &value) 
{
	_M_type = TYPE_VEC3;
	_M_vec3 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector3f>);
	_M_vec3->push_back (value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Vector2f> &value) 
{
	_M_type = TYPE_VEC2_ARRAY;
	_M_vec2 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector2f>, value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Vector3f> &value) 
{
	_M_type = TYPE_VEC3_ARRAY;
	_M_vec3 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector3f>, value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_Vector4f &value) 
{
	_M_type = TYPE_VEC4;
	_M_vec4 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector4f>);
	_M_vec4->push_back (value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Vector4f> &value) 
{
	_M_type = TYPE_VEC4_ARRAY;
	_M_vec4 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector4f>, value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_Matrix3x3f &value) 
{
	_M_type = TYPE_MAT33;
	_M_mat33 = ATOM_NEW(ATOM_VECTOR<ATOM_Matrix3x3f>);
	_M_mat33->push_back (value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Matrix3x3f> &value) 
{
	_M_type = TYPE_MAT33_ARRAY;
	_M_mat33 = ATOM_NEW(ATOM_VECTOR<ATOM_Matrix3x3f>, value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_Matrix4x4f &value) 
{
	_M_type = TYPE_MAT44;
	_M_mat44 = ATOM_NEW(ATOM_VECTOR<ATOM_Matrix4x4f>);
	_M_mat44->push_back (value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Matrix4x4f> &value) 
{
	_M_type = TYPE_MAT44_ARRAY;
	_M_mat44 = ATOM_NEW(ATOM_VECTOR<ATOM_Matrix4x4f>, value);
}

ATOM_ScriptVar::ATOM_ScriptVar (ATOM_AUTOREF(ATOM_Object) value) 
{
	_M_type = TYPE_OBJECT;
	_M_object = ATOM_NEW(ObjectVector);
	_M_object->push_back (value);
}

ATOM_ScriptVar::ATOM_ScriptVar (const ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > &value) 
{
	_M_type = TYPE_OBJECT_ARRAY;
	_M_object = ATOM_NEW(ObjectVector, value);
}

ATOM_ScriptVar & ATOM_ScriptVar::operator = (const ATOM_ScriptVar &rhs) 
{
	ATOM_ScriptVar tmp(rhs);
	swap (tmp);
	return *this;
}

void ATOM_ScriptVar::deleteStorage (void) 
{
	switch (_M_type)
	{
	case TYPE_INT: 
	case TYPE_INT_ARRAY:
		ATOM_DELETE(_M_integer); 
		break;
	case TYPE_FLOAT: 
	case TYPE_FLOAT_ARRAY:
		ATOM_DELETE(_M_float); 
		break;
	case TYPE_STRING: 
	case TYPE_STRING_ARRAY:
		ATOM_DELETE(_M_string); 
		break;
	case TYPE_VEC2: 
	case TYPE_VEC2_ARRAY:
		ATOM_DELETE(_M_vec2); 
		break;
	case TYPE_VEC3: 
	case TYPE_VEC3_ARRAY:
		ATOM_DELETE(_M_vec3); 
		break;
	case TYPE_VEC4: 
	case TYPE_VEC4_ARRAY:
		ATOM_DELETE(_M_vec4); 
		break;
	case TYPE_MAT33: 
	case TYPE_MAT33_ARRAY:
		ATOM_DELETE(_M_mat33); 
		break;
	case TYPE_MAT44: 
	case TYPE_MAT44_ARRAY:
		ATOM_DELETE(_M_mat44); 
		break;
	case TYPE_OBJECT: 
	case TYPE_OBJECT_ARRAY:
		ATOM_DELETE(_M_object); 
		break;
	default:
		break;
	}
}

void ATOM_ScriptVar::createStorage (void) 
{
	switch (_M_type)
	{
	case TYPE_INT: 
		_M_integer = ATOM_NEW(ATOM_VECTOR<int>, 1); 
		break;
	case TYPE_INT_ARRAY:
		_M_integer = ATOM_NEW(ATOM_VECTOR<int>); 
		break;
	case TYPE_FLOAT: 
		_M_float = ATOM_NEW(ATOM_VECTOR<float>, 1); 
		break;
	case TYPE_FLOAT_ARRAY:
		_M_float = ATOM_NEW(ATOM_VECTOR<float>); 
		break;
	case TYPE_STRING: 
		_M_string = ATOM_NEW(ATOM_VECTOR<ATOM_STRING>, 1); 
		break;
	case TYPE_STRING_ARRAY:
		_M_string = ATOM_NEW(ATOM_VECTOR<ATOM_STRING>); 
		break;
	case TYPE_VEC2: 
		_M_vec2 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector2f>, 1); 
		break;
	case TYPE_VEC2_ARRAY:
		_M_vec2 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector2f>); 
		break;
	case TYPE_VEC3: 
		_M_vec3 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector3f>, 1); 
		break;
	case TYPE_VEC3_ARRAY:
		_M_vec3 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector3f>); 
		break;
	case TYPE_VEC4: 
		_M_vec4 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector4f>, 1); 
		break;
	case TYPE_VEC4_ARRAY:
		_M_vec4 = ATOM_NEW(ATOM_VECTOR<ATOM_Vector4f>); 
		break;
	case TYPE_MAT33: 
		_M_mat33 = ATOM_NEW(ATOM_VECTOR<ATOM_Matrix3x3f>, 1); 
		break;
	case TYPE_MAT33_ARRAY:
		_M_mat33 = ATOM_NEW(ATOM_VECTOR<ATOM_Matrix3x3f>); 
		break;
	case TYPE_MAT44: 
		_M_mat44 = ATOM_NEW(ATOM_VECTOR<ATOM_Matrix4x4f>, 1); 
		break;
	case TYPE_MAT44_ARRAY:
		_M_mat44 = ATOM_NEW(ATOM_VECTOR<ATOM_Matrix4x4f>); 
		break;
	case TYPE_OBJECT: 
		_M_object = ATOM_NEW(ObjectVector, 1); 
		break;
	case TYPE_OBJECT_ARRAY:
		_M_object = ATOM_NEW(ObjectVector); 
		break;
	default:
		break;
	}
}

void ATOM_ScriptVar::reallocStorage (int type) 
{
	if (type != _M_type)
	{
		deleteStorage ();
		_M_type = type;
		createStorage ();
	}
}

const int & ATOM_ScriptVar::asInteger (void) const 
{
	ATOM_ASSERT (_M_integer && !_M_integer->empty());
	return _M_integer->front();
}

const ATOM_VECTOR<int> & ATOM_ScriptVar::asIntegerArray (void) const 
{
	ATOM_ASSERT (_M_integer);
	return *_M_integer;
}

const float & ATOM_ScriptVar::asFloat(void) const 
{
	ATOM_ASSERT (_M_float && !_M_float->empty());
	return _M_float->front();
}

const ATOM_VECTOR<float> & ATOM_ScriptVar::asFloatArray (void) const 
{
	ATOM_ASSERT (_M_float);
	return *_M_float;
}

const ATOM_STRING & ATOM_ScriptVar::asString (void) const 
{
	ATOM_ASSERT (_M_string && !_M_string->empty());
	return _M_string->front();
}

const ATOM_VECTOR<ATOM_STRING> & ATOM_ScriptVar::asStringArray (void) const 
{
	ATOM_ASSERT (_M_string);
	return *_M_string;
}

const ATOM_Vector2f & ATOM_ScriptVar::asVector2 (void) const 
{
	ATOM_ASSERT (_M_vec2 && !_M_vec2->empty());
	return _M_vec2->front();
}

const ATOM_Vector3f & ATOM_ScriptVar::asVector3 (void) const 
{
	ATOM_ASSERT (_M_vec3 && !_M_vec3->empty());
	return _M_vec3->front();
}

const ATOM_VECTOR<ATOM_Vector2f> & ATOM_ScriptVar::asVector2Array (void) const 
{
	ATOM_ASSERT (_M_vec2);
	return *_M_vec2;
}

const ATOM_VECTOR<ATOM_Vector3f> & ATOM_ScriptVar::asVector3Array (void) const 
{
	ATOM_ASSERT (_M_vec3);
	return *_M_vec3;
}

const ATOM_Vector4f & ATOM_ScriptVar::asVector4 (void) const 
{
	ATOM_ASSERT (_M_vec4 && !_M_vec4->empty());
	return _M_vec4->front();
}

const ATOM_VECTOR<ATOM_Vector4f> & ATOM_ScriptVar::asVector4Array (void) const 
{
	ATOM_ASSERT (_M_vec4);
	return *_M_vec4;
}

const ATOM_Matrix3x3f & ATOM_ScriptVar::asMatrix33 (void) const 
{
	ATOM_ASSERT (_M_mat33 && !_M_mat33->empty());
	return _M_mat33->front();
}

const ATOM_VECTOR<ATOM_Matrix3x3f> & ATOM_ScriptVar::asMatrix33Array(void) const 
{
	ATOM_ASSERT (_M_mat33);
	return *_M_mat33;
}

const ATOM_Matrix4x4f & ATOM_ScriptVar::asMatrix44 (void) const 
{
	ATOM_ASSERT (_M_mat44 && !_M_mat44->empty());
	return _M_mat44->front();
}

const ATOM_VECTOR<ATOM_Matrix4x4f> & ATOM_ScriptVar::asMatrix44Array (void) const 
{
	ATOM_ASSERT (_M_mat44);
	return *_M_mat44;
}

const ATOM_AUTOREF(ATOM_Object) & ATOM_ScriptVar::asObject (void) const 
{
	ATOM_ASSERT (_M_object && !_M_object->empty());
	return _M_object->front();
}

const ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > & ATOM_ScriptVar::asObjectArray (void) const 
{
	ATOM_ASSERT (_M_object);
	return *_M_object;
}

int & ATOM_ScriptVar::asInteger (void) 
{
	ATOM_ASSERT (_M_integer && !_M_integer->empty());
	return _M_integer->front();
}

ATOM_VECTOR<int> & ATOM_ScriptVar::asIntegerArray (void) 
{
	ATOM_ASSERT (_M_integer);
	return *_M_integer;
}

float & ATOM_ScriptVar::asFloat(void) 
{
	ATOM_ASSERT (_M_float && !_M_float->empty());
	return _M_float->front();
}

ATOM_VECTOR<float> & ATOM_ScriptVar::asFloatArray (void) 
{
	ATOM_ASSERT (_M_float);
	return *_M_float;
}

ATOM_STRING & ATOM_ScriptVar::asString (void) 
{
	ATOM_ASSERT (_M_string && !_M_string->empty());
	return _M_string->front();
}

ATOM_VECTOR<ATOM_STRING> & ATOM_ScriptVar::asStringArray (void) 
{
	ATOM_ASSERT (_M_string);
	return *_M_string;
}

ATOM_Vector2f & ATOM_ScriptVar::asVector2 (void) 
{
	ATOM_ASSERT (_M_vec2 && !_M_vec2->empty());
	return _M_vec2->front();
}

ATOM_VECTOR<ATOM_Vector2f> & ATOM_ScriptVar::asVector2Array (void) 
{
	ATOM_ASSERT (_M_vec2);
	return *_M_vec2;
}

ATOM_Vector3f & ATOM_ScriptVar::asVector3 (void) 
{
	ATOM_ASSERT (_M_vec3 && !_M_vec3->empty());
	return _M_vec3->front();
}

ATOM_VECTOR<ATOM_Vector3f> & ATOM_ScriptVar::asVector3Array (void) 
{
	ATOM_ASSERT (_M_vec3);
	return *_M_vec3;
}

ATOM_Vector4f & ATOM_ScriptVar::asVector4 (void) 
{
	ATOM_ASSERT (_M_vec4 && !_M_vec4->empty());
	return _M_vec4->front();
}

ATOM_VECTOR<ATOM_Vector4f> & ATOM_ScriptVar::asVector4Array (void) 
{
	ATOM_ASSERT (_M_vec4);
	return *_M_vec4;
}

ATOM_Matrix3x3f & ATOM_ScriptVar::asMatrix33 (void) 
{
	ATOM_ASSERT (_M_mat33 && !_M_mat33->empty());
	return _M_mat33->front();
}

ATOM_VECTOR<ATOM_Matrix3x3f> & ATOM_ScriptVar::asMatrix33Array(void) 
{
	ATOM_ASSERT (_M_mat33);
	return *_M_mat33;
}

ATOM_Matrix4x4f & ATOM_ScriptVar::asMatrix44 (void) 
{
	ATOM_ASSERT (_M_mat44 && !_M_mat44->empty());
	return _M_mat44->front();
}

ATOM_VECTOR<ATOM_Matrix4x4f> & ATOM_ScriptVar::asMatrix44Array (void) 
{
	ATOM_ASSERT (_M_mat44);
	return *_M_mat44;
}

ATOM_AUTOREF(ATOM_Object) & ATOM_ScriptVar::asObject (void) 
{
	ATOM_ASSERT (_M_object && !_M_object->empty());
	return _M_object->front();
}

ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > & ATOM_ScriptVar::asObjectArray (void) 
{
	ATOM_ASSERT (_M_object);
	return *_M_object;
}

template <class T>
bool _VectorEqual (const T &v1, const T &v2) 
{
	if (v1.size() != v2.size())
		return false;

	for (unsigned i = 0; i < v1.size(); ++i)
	{
		if (v1[i] != v2[i])
			return false;
	}
	return true;
}

bool ATOM_ScriptVar::operator == (const ATOM_ScriptVar &rhs) const 
{
	if (_M_type != rhs._M_type)
		return false;

	switch (_M_type)
	{
	case TYPE_INT:          return asInteger() == rhs.asInteger();
	case TYPE_INT_ARRAY:    return _VectorEqual (*_M_integer, *rhs._M_integer);
	case TYPE_FLOAT:        return asFloat() == rhs.asFloat();
	case TYPE_FLOAT_ARRAY:  return _VectorEqual (*_M_float, *rhs._M_float);
	case TYPE_STRING:       return asString() == rhs.asString ();
	case TYPE_STRING_ARRAY: return _VectorEqual (*_M_string, *rhs._M_string);
	case TYPE_VEC2:         return asVector2() == rhs.asVector2 ();
	case TYPE_VEC2_ARRAY:   return _VectorEqual (*_M_vec2, *rhs._M_vec2);
	case TYPE_VEC3:         return asVector3() == rhs.asVector3 ();
	case TYPE_VEC3_ARRAY:   return _VectorEqual (*_M_vec3, *rhs._M_vec3);
	case TYPE_VEC4:         return asVector4() == rhs.asVector4 ();
	case TYPE_VEC4_ARRAY:   return _VectorEqual (*_M_vec4, *rhs._M_vec4);
	case TYPE_MAT33:        return asMatrix33() == rhs.asMatrix33();
	case TYPE_MAT33_ARRAY:  return _VectorEqual (*_M_mat33, *rhs._M_mat33);
	case TYPE_MAT44:        return asMatrix44() == rhs.asMatrix44();
	case TYPE_MAT44_ARRAY:  return _VectorEqual (*_M_mat44, *rhs._M_mat44);
	case TYPE_OBJECT:       return asObject() == rhs.asObject();
	case TYPE_OBJECT_ARRAY: return _VectorEqual (*_M_object, *rhs._M_object);
	default: return false;
	}
}

bool ATOM_ScriptVar::operator != (const ATOM_ScriptVar &rhs) const 
{
	return ! operator == (rhs);
}

int ATOM_ScriptVar::getInteger (void) const
{
	if (_M_type == TYPE_INT)
	{
		return asInteger();
	}
	else if (_M_type == TYPE_FLOAT)
	{
		return asFloat();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return 0;
	}
}

float ATOM_ScriptVar::getFloat (void) const
{
	if (_M_type == TYPE_INT)
	{
		return asInteger();
	}
	else if (_M_type == TYPE_FLOAT)
	{
		return asFloat();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return 0.f;
	}
}

ATOM_STRING ATOM_ScriptVar::getString (void) const
{
	if (_M_type == TYPE_STRING)
	{
		return asString();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return "";
	}
}

ATOM_Vector2f ATOM_ScriptVar::getVector2 (void) const
{
	if (_M_type == TYPE_VEC2)
	{
		return asVector2();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_Vector2f(0.f);
	}
}

ATOM_Vector3f ATOM_ScriptVar::getVector3 (void) const
{
	if (_M_type == TYPE_VEC3)
	{
		return asVector3();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_Vector3f(0.f);
	}
}

ATOM_Vector4f ATOM_ScriptVar::getVector4 (void) const
{
	if (_M_type == TYPE_VEC4)
	{
		return asVector4();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_Vector4f(0.f);
	}
}

ATOM_Matrix3x3f ATOM_ScriptVar::getMatrix33 (void) const
{
	if (_M_type == TYPE_MAT33)
	{
		return asMatrix33();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_Matrix3x3f::getIdentityMatrix();
	}
}

ATOM_Matrix4x4f ATOM_ScriptVar::getMatrix44 (void) const
{
	if (_M_type == TYPE_MAT44)
	{
		return asMatrix44();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_Matrix4x4f::getIdentityMatrix();
	}
}

ATOM_AUTOREF(ATOM_Object) ATOM_ScriptVar::getObject (void) const
{
	if (_M_type == TYPE_OBJECT)
	{
		return asObject();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return nullptr;
	}
}

ATOM_VECTOR<int> ATOM_ScriptVar::getIntegerArray (void) const
{
	if (_M_type == TYPE_INT_ARRAY)
	{
		return asIntegerArray();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_VECTOR<int>();
	}
}

ATOM_VECTOR<float> ATOM_ScriptVar::getFloatArray (void) const
{
	if (_M_type == TYPE_FLOAT_ARRAY)
	{
		return asFloatArray();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_VECTOR<float>();
	}
}

ATOM_VECTOR<ATOM_STRING> ATOM_ScriptVar::getStringArray (void) const
{
	if (_M_type == TYPE_STRING_ARRAY)
	{
		return asStringArray();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_VECTOR<ATOM_STRING>();
	}
}

ATOM_VECTOR<ATOM_Vector2f> ATOM_ScriptVar::getVector2Array (void) const
{
	if (_M_type == TYPE_VEC2_ARRAY)
	{
		return asVector2Array();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_VECTOR<ATOM_Vector2f>();
	}
}

ATOM_VECTOR<ATOM_Vector3f> ATOM_ScriptVar::getVector3Array (void) const
{
	if (_M_type == TYPE_VEC3_ARRAY)
	{
		return asVector3Array();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_VECTOR<ATOM_Vector3f>();
	}
}

ATOM_VECTOR<ATOM_Vector4f> ATOM_ScriptVar::getVector4Array (void) const
{
	if (_M_type == TYPE_VEC4_ARRAY)
	{
		return asVector4Array();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_VECTOR<ATOM_Vector4f>();
	}
}

ATOM_VECTOR<ATOM_Matrix3x3f> ATOM_ScriptVar::getMatrix33Array (void) const
{
	if (_M_type == TYPE_MAT33_ARRAY)
	{
		return asMatrix33Array();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_VECTOR<ATOM_Matrix3x3f>();
	}
}

ATOM_VECTOR<ATOM_Matrix4x4f> ATOM_ScriptVar::getMatrix44Array (void) const
{
	if (_M_type == TYPE_MAT44_ARRAY)
	{
		return asMatrix44Array();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_VECTOR<ATOM_Matrix4x4f>();
	}
}

ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > ATOM_ScriptVar::getObjectArray (void) const
{
	if (_M_type == TYPE_OBJECT_ARRAY)
	{
		return asObjectArray();
	}
	else
	{
		ATOM_LOGGER::error ("Bad variant cast\n");
		return ATOM_VECTOR<ATOM_AUTOREF(ATOM_Object)>();
	}
}

int ATOM_ScriptVar::getType (void) const 
{
	return _M_type;
}

void ATOM_ScriptVar::setType (int type) 
{
	reallocStorage (type);
}

void ATOM_ScriptVar::swap (ATOM_ScriptVar &rhs) 
{
	std::swap (_M_type, rhs._M_type);
	std::swap (_M_integer, rhs._M_integer);
}
