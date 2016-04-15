#include "StdAfx.h"
#include "parameter.h"

ATOM_MaterialParam::ATOM_MaterialParam (void)
{
	_valueCallback = 0;
	_editorType = ParamEditorType_Disable;
	_registerIndex = -1;
	_parameterIndex = 0;
	_flags = 0;
	//--- wangjian added for pass test ---//
	//_pass = 0;
	//------------------------------------//
}

ATOM_MaterialParam::~ATOM_MaterialParam (void)
{
}

ATOM_MaterialParam::Type ATOM_MaterialParam::getParamType (void) const
{
	return ParamType_Unknown;
}

unsigned ATOM_MaterialParam::getDimension (void) const
{
	return 1;
}

unsigned ATOM_MaterialParam::getRegisterCount (void)
{
	return 0;
}

void ATOM_MaterialParam::writeRegisterValue (ATOM_Vector4f *buffer)
{
}

void ATOM_MaterialParam::writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc)
{
}

void ATOM_MaterialParam::setValueCallback (ValueCallback callback)
{
	_valueCallback = callback;
}

void ATOM_MaterialParam::setRegisterIndex (int index)
{
	_registerIndex = index;
}

int ATOM_MaterialParam::getRegisterIndex (void) const
{
	return _registerIndex;
}

void ATOM_MaterialParam::setParamIndex (int index)
{
	_parameterIndex = index;
}

int ATOM_MaterialParam::getParamIndex (void) const
{
	return _parameterIndex;
}

void ATOM_MaterialParam::setFlags (unsigned flags)
{
	_flags = flags;
}

unsigned ATOM_MaterialParam::getFlags (void) const
{
	return _flags;
}

void ATOM_MaterialParam::setEditorType (EditorType editorType)
{
	_editorType = editorType;
}

ATOM_MaterialParam::EditorType ATOM_MaterialParam::getEditorType (void) const
{
	return _editorType;
}

ATOM_MaterialParam::ValueCallback ATOM_MaterialParam::getValueCallback (void) const
{
	return _valueCallback;
}

//--------------------------------------------------//
// wangjian modified : 增加一个参数：EFFECT PASS
#if 0 
void ATOM_MaterialParam::addApplyCallback (	ParamCallback callback, void *context, void *context2, ATOM_MaterialEffect *effect )
{
	if (callback)
	{
		_applyCallbacks.resize (_applyCallbacks.size() + 1);
		_applyCallbacks.back().callback = callback;
		_applyCallbacks.back().context = context;
		_applyCallbacks.back().context2 = context2;
		_applyCallbacks.back().effect = effect;
	}
}
#else
void ATOM_MaterialParam::addApplyCallback (	ParamCallback callback, 
											void *context, 
											void *context2, 
											ATOM_MaterialEffect *effect,
											ATOM_MaterialPass *pass)
{
	// wangjian modified 
#if 0
	_applyCallbacks.resize (_applyCallbacks.size() + 1);
	_applyCallbacks.back().callback = callback;
	_applyCallbacks.back().context = context;
	_applyCallbacks.back().context2 = context2;
	_applyCallbacks.back().effect = effect;
	_applyCallbacks.back().pass = pass;
#else
	CallbackInfo info;
	info.callback = callback;
	info.context = context;
	info.context2 = context2;
	info.effect = effect;
	info.pass = pass;
	_applyCallbacks.push_back(info);
#endif
}
#endif
//--------------------------------------------------//


int ATOM_MaterialParam::getNumCallbacks (void) const
{
	return _applyCallbacks.size();
}

const ATOM_MaterialParam::CallbackInfo *ATOM_MaterialParam::getCallback (int i) const
{
	return &_applyCallbacks[i];
}

const ATOM_MaterialParam::CallbackInfo *ATOM_MaterialParam::getCallbackForEffect (ATOM_MaterialEffect *effect) const
{
	for (unsigned i = 0; i < _applyCallbacks.size(); ++i)
	{
		const CallbackInfo &info = _applyCallbacks[i];

		if (!info.effect || effect == info.effect)
		{
			return &info;
		}
	}
	return nullptr;
}

/*
void ATOM_MaterialParam::apply (ATOM_MaterialEffect *effect)
{
	for (unsigned i = 0; i < _applyCallbacks.size(); ++i)
	{
		const CallbackInfo &info = _applyCallbacks[i];

		if (!info.effect || effect == info.effect)
		{
			evalValueCallback ();
			info.callback(this, info.context, info.context2);
		}
	}
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////

ATOM_MaterialFloatParam::ATOM_MaterialFloatParam (void)
{
	_value = 0.f;
}

ATOM_MaterialFloatParam::ATOM_MaterialFloatParam (float value)
{
	_value = value;
}

ATOM_MaterialParam::Type ATOM_MaterialFloatParam::getParamType (void) const
{
	return ParamType_Float;
}

unsigned ATOM_MaterialFloatParam::getValueSize (void) const
{
	return sizeof(float);
}

void *ATOM_MaterialFloatParam::getValuePtr (void)
{
	return &_value;
}

unsigned ATOM_MaterialFloatParam::getRegisterCount (void)
{
	return 1;
}

void ATOM_MaterialFloatParam::writeRegisterValue (ATOM_Vector4f *buffer)
{
	buffer->set (_value, _value, _value, _value);
}

void ATOM_MaterialFloatParam::writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc)
{
	float f = *((const float*)valuePtr);
	buffer->set (f, f, f, f);
}

void ATOM_MaterialFloatParam::setValue (float value)
{
	_value = value;
}

float ATOM_MaterialFloatParam::getValue (void) const
{
	return _value;
}

ATOM_MaterialFloatArrayParam::ATOM_MaterialFloatArrayParam (unsigned dimension)
{
	_value = dimension ? ATOM_NEW_ARRAY(float, dimension) : 0;
	_dimension = dimension;
}

ATOM_MaterialFloatArrayParam::ATOM_MaterialFloatArrayParam (unsigned dimension, const float *value)
{
	_value = dimension ? ATOM_NEW_ARRAY(float, dimension) : 0;
	_dimension = dimension;

	if (value)
	{
		memcpy (_value, value, sizeof(float)*dimension);
	}
}

ATOM_MaterialFloatArrayParam::~ATOM_MaterialFloatArrayParam (void)
{
	ATOM_DELETE_ARRAY(_value);
}

ATOM_MaterialParam::Type ATOM_MaterialFloatArrayParam::getParamType (void) const
{
	return ParamType_FloatArray;
}

unsigned ATOM_MaterialFloatArrayParam::getDimension (void) const
{
	return _dimension;
}

void ATOM_MaterialFloatArrayParam::setDimension (unsigned dimension)
{
	if (_dimension != dimension)
	{
		ATOM_DELETE_ARRAY(_value);
		_value = dimension ? ATOM_NEW_ARRAY(float, dimension) : 0;
		_dimension = dimension;
	}
}

unsigned ATOM_MaterialFloatArrayParam::getValueSize (void) const
{
	return sizeof(float);
}

void *ATOM_MaterialFloatArrayParam::getValuePtr (void)
{
	return _value;
}

unsigned ATOM_MaterialFloatArrayParam::getRegisterCount (void)
{
	return _dimension;
}

void ATOM_MaterialFloatArrayParam::writeRegisterValue (ATOM_Vector4f *buffer)
{
	for (const float *p = _value; p < _value+_dimension; ++p, ++buffer)
	{
		buffer->set (*p, *p, *p, *p);
	}
}

void ATOM_MaterialFloatArrayParam::writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc)
{
	const float *f = (const float*)valuePtr;
	int n = desc->numElements;
	if (n > _dimension)
		n = _dimension;

	for (const float *p = (const float*)f; p < f+n; ++p, ++buffer)
	{
		buffer->set (*p, *p, *p, *p);
	}
}

void ATOM_MaterialFloatArrayParam::setValue (const float *value, unsigned count)
{
	if (value)
	{
		if (count > _dimension)
			count = _dimension;

		memcpy(_value, value, count*sizeof(float));
	}
}

float *ATOM_MaterialFloatArrayParam::getValue (void) const
{
	return _value;
}

void ATOM_MaterialFloatArrayParam::setElement (unsigned index, float value)
{
	if (index < _dimension)
	{
		_value[index] = value;
	}
}

ATOM_MaterialIntParam::ATOM_MaterialIntParam (void)
{
	_value = 0;
}

ATOM_MaterialIntParam::ATOM_MaterialIntParam (int value)
{
	_value = value;
}

ATOM_MaterialParam::Type ATOM_MaterialIntParam::getParamType (void) const
{
	return ParamType_Int;
}

unsigned ATOM_MaterialIntParam::getValueSize (void) const
{
	return sizeof(int);
}

void *ATOM_MaterialIntParam::getValuePtr (void)
{
	return &_value;
}

unsigned ATOM_MaterialIntParam::getRegisterCount (void)
{
	return 1;
}

void ATOM_MaterialIntParam::writeRegisterValue (ATOM_Vector4f *buffer)
{
	buffer->set (_value, _value, _value, _value);
}

void ATOM_MaterialIntParam::writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc)
{
	int i = *((const int*)valuePtr);
	buffer->set (i,i,i,i);
}

void ATOM_MaterialIntParam::setValue (int value)
{
	_value = value;
}

int ATOM_MaterialIntParam::getValue (void) const
{
	return _value;
}

ATOM_MaterialIntArrayParam::ATOM_MaterialIntArrayParam (unsigned dimension)
{
	_value = dimension ? ATOM_NEW_ARRAY(int, dimension) : 0;
	_dimension = dimension;
}

ATOM_MaterialIntArrayParam::ATOM_MaterialIntArrayParam (unsigned dimension, const int *value)
{
	_value = dimension ? ATOM_NEW_ARRAY(int, dimension) : 0;
	_dimension = dimension;

	if (value)
	{
		memcpy (_value, value, sizeof(int)*dimension);
	}
}

ATOM_MaterialIntArrayParam::~ATOM_MaterialIntArrayParam (void)
{
	ATOM_DELETE_ARRAY(_value);
}

ATOM_MaterialParam::Type ATOM_MaterialIntArrayParam::getParamType (void) const
{
	return ParamType_IntArray;
}

unsigned ATOM_MaterialIntArrayParam::getDimension (void) const
{
	return _dimension;
}

void ATOM_MaterialIntArrayParam::setDimension (unsigned dimension)
{
	if (dimension != _dimension)
	{
		ATOM_DELETE_ARRAY(_value);
		_value = dimension ? ATOM_NEW_ARRAY(int, dimension) : 0;
		_dimension = dimension;
	}
}

unsigned ATOM_MaterialIntArrayParam::getRegisterCount (void)
{
	return _dimension;
}

void ATOM_MaterialIntArrayParam::writeRegisterValue (ATOM_Vector4f *buffer)
{
	for (const int *p = _value; p < _value + _dimension; ++p, ++buffer)
	{
		buffer->set (*p, *p, *p, *p);
	}
}

void ATOM_MaterialIntArrayParam::writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc)
{
	const int *i = (const int*)valuePtr;
	int n = desc->numElements;
	if (n > _dimension)
		n = _dimension;

	for (const int *p = i; p < i + n; ++p, ++buffer)
	{
		buffer->set (*p, *p, *p, *p);
	}
}

unsigned ATOM_MaterialIntArrayParam::getValueSize (void) const
{
	return sizeof(int);
}

void *ATOM_MaterialIntArrayParam::getValuePtr (void)
{
	return _value;
}

void ATOM_MaterialIntArrayParam::setValue (const int *value, unsigned count)
{
	if (value)
	{
		if (count > _dimension)
			count = _dimension;

		memcpy(_value, value, count*sizeof(int));
	}
}

int *ATOM_MaterialIntArrayParam::getValue (void) const
{
	return _value;
}

void ATOM_MaterialIntArrayParam::setElement (unsigned index, int value)
{
	if (index < _dimension)
	{
		_value[index] = value;
	}
}

ATOM_MaterialMatrix44Param::ATOM_MaterialMatrix44Param (void)
{
	_value.makeIdentity();
}

ATOM_MaterialMatrix44Param::ATOM_MaterialMatrix44Param (const ATOM_Matrix4x4f &value)
{
	_value = value;
}

ATOM_MaterialParam::Type ATOM_MaterialMatrix44Param::getParamType (void) const
{
	return ParamType_Matrix44;
}

unsigned ATOM_MaterialMatrix44Param::getValueSize (void) const
{
	return sizeof(ATOM_Matrix4x4f);
}

void *ATOM_MaterialMatrix44Param::getValuePtr (void)
{
	return &_value;
}

unsigned ATOM_MaterialMatrix44Param::getRegisterCount (void)
{
	return 4;
}

void ATOM_MaterialMatrix44Param::writeRegisterValue (ATOM_Vector4f *buffer)
{
	// transposed
	buffer[0].set (_value.m00, _value.m10, _value.m20, _value.m30);
	buffer[1].set (_value.m01, _value.m11, _value.m21, _value.m31);
	buffer[2].set (_value.m02, _value.m12, _value.m22, _value.m32);
	buffer[3].set (_value.m03, _value.m13, _value.m23, _value.m33);
}

void ATOM_MaterialMatrix44Param::writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc)
{
	const ATOM_Matrix4x4f *m = (const ATOM_Matrix4x4f*)valuePtr;
	// transposed
	buffer[0].set (m->m00, m->m10, m->m20, m->m30);

	if (desc->registerCount > 1) 
	{
		buffer[1].set (m->m01, m->m11, m->m21, m->m31);
	}
	if (desc->registerCount > 2)
	{
		buffer[2].set (m->m02, m->m12, m->m22, m->m32);
	}
	if (desc->registerCount > 3)
	{
		buffer[3].set (m->m03, m->m13, m->m23, m->m33);
	}
}

void ATOM_MaterialMatrix44Param::setValue (const ATOM_Matrix4x4f &value)
{
	_value = value;
}

const ATOM_Matrix4x4f &ATOM_MaterialMatrix44Param::getValue (void) const
{
	return _value;
}

ATOM_MaterialMatrix44ArrayParam::ATOM_MaterialMatrix44ArrayParam (unsigned dimension)
{
	_value = dimension ? ATOM_NEW_ARRAY(ATOM_Matrix4x4f, dimension) : 0;
	_dimension = dimension;
}

ATOM_MaterialMatrix44ArrayParam::ATOM_MaterialMatrix44ArrayParam (unsigned dimension, const ATOM_Matrix4x4f *value)
{
	_value = dimension ? ATOM_NEW_ARRAY(ATOM_Matrix4x4f, dimension) : 0;
	_dimension = dimension;

	if (value)
	{
		memcpy (_value, value, sizeof(ATOM_Matrix4x4f)*dimension);
	}
}

ATOM_MaterialMatrix44ArrayParam::~ATOM_MaterialMatrix44ArrayParam (void)
{
	ATOM_DELETE_ARRAY (_value);
}

ATOM_MaterialParam::Type ATOM_MaterialMatrix44ArrayParam::getParamType (void) const
{
	return ParamType_Matrix44Array;
}

unsigned ATOM_MaterialMatrix44ArrayParam::getDimension (void) const
{
	return _dimension;
}

void ATOM_MaterialMatrix44ArrayParam::setDimension (unsigned dimension)
{
	if (dimension != _dimension)
	{
		ATOM_DELETE_ARRAY(_value);
		_value = dimension ? ATOM_NEW_ARRAY(ATOM_Matrix4x4f, dimension) : 0;
		_dimension = dimension;
	}
}

unsigned ATOM_MaterialMatrix44ArrayParam::getRegisterCount (void)
{
	return _dimension * 4;
}

void ATOM_MaterialMatrix44ArrayParam::writeRegisterValue (ATOM_Vector4f *buffer)
{
	for (const ATOM_Matrix4x4f *m = _value; m < _value + _dimension; ++m, buffer+=4)
	{
		buffer[0].set (m->m00, m->m10, m->m20, m->m30);
		buffer[1].set (m->m01, m->m11, m->m21, m->m31);
		buffer[2].set (m->m02, m->m12, m->m22, m->m32);
		buffer[3].set (m->m03, m->m13, m->m23, m->m33);
	}
}

void ATOM_MaterialMatrix44ArrayParam::writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc)
{
	const ATOM_Matrix4x4f *m = (const ATOM_Matrix4x4f*)valuePtr;
	int n = desc->numElements;
	if (n > _dimension)
		n = _dimension;

	switch (desc->registerCount)
	{
	case 1:
		{
			for (int i = 0; i < n; ++i, ++m)
			{
				buffer++->set (m->m00, m->m10, m->m20, m->m30);
			}
			break;
		}
	case 2:
		{
			for (int i = 0; i < n; ++i, ++m)
			{
				buffer++->set (m->m00, m->m10, m->m20, m->m30);
				buffer++->set (m->m01, m->m11, m->m21, m->m31);
			}
			break;
		}
	case 3:
		{
			for (int i = 0; i < n; ++i, ++m)
			{
				buffer++->set (m->m00, m->m10, m->m20, m->m30);
				buffer++->set (m->m01, m->m11, m->m21, m->m31);
				buffer++->set (m->m02, m->m12, m->m22, m->m32);
			}
			break;
		}
	case 4:
		{
			for (int i = 0; i < n; ++i, ++m)
			{
				buffer++->set (m->m00, m->m10, m->m20, m->m30);
				buffer++->set (m->m01, m->m11, m->m21, m->m31);
				buffer++->set (m->m02, m->m12, m->m22, m->m32);
				buffer++->set (m->m03, m->m13, m->m23, m->m33);
			}
			break;
		}
	}
}

unsigned ATOM_MaterialMatrix44ArrayParam::getValueSize (void) const
{
	return sizeof(ATOM_Matrix4x4f);
}

void *ATOM_MaterialMatrix44ArrayParam::getValuePtr (void)
{
	return _value;
}

void ATOM_MaterialMatrix44ArrayParam::setValue (const ATOM_Matrix4x4f *value, unsigned count)
{
	if (value)
	{
		if (count > _dimension)
			count = _dimension;

		memcpy(_value, value, count*sizeof(ATOM_Matrix4x4f));
	}
}

ATOM_Matrix4x4f *ATOM_MaterialMatrix44ArrayParam::getValue (void) const
{
	return _value;
}

void ATOM_MaterialMatrix44ArrayParam::setElement (unsigned index, const ATOM_Matrix4x4f &value)
{
	if (index < _dimension)
	{
		_value[index] = value;
	}
}

ATOM_MaterialMatrix43Param::ATOM_MaterialMatrix43Param (void)
{
	_value.makeIdentity();
}

ATOM_MaterialMatrix43Param::ATOM_MaterialMatrix43Param (const ATOM_Matrix3x4f &value)
{
	_value = value;
}

ATOM_MaterialParam::Type ATOM_MaterialMatrix43Param::getParamType (void) const
{
	return ParamType_Matrix43;
}

unsigned ATOM_MaterialMatrix43Param::getValueSize (void) const
{
	return sizeof(ATOM_Matrix3x4f);
}

void *ATOM_MaterialMatrix43Param::getValuePtr (void)
{
	return &_value;
}

unsigned ATOM_MaterialMatrix43Param::getRegisterCount (void)
{
	return 3;
}

void ATOM_MaterialMatrix43Param::writeRegisterValue (ATOM_Vector4f *buffer)
{
	memcpy (buffer, &_value, sizeof(float)*12);
}

void ATOM_MaterialMatrix43Param::writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc)
{
	const ATOM_Vector4f *v = (const ATOM_Vector4f*)valuePtr;

	buffer[0] = v[0];

	if (desc->registerCount > 1)
	{
		buffer[1] = v[1];
	}

	if (desc->registerCount > 2)
	{
		buffer[2] = v[2];
	}
}

void ATOM_MaterialMatrix43Param::setValue (const ATOM_Matrix3x4f &value)
{
	_value = value;
}

const ATOM_Matrix3x4f &ATOM_MaterialMatrix43Param::getValue (void) const
{
	return _value;
}

ATOM_MaterialMatrix43ArrayParam::ATOM_MaterialMatrix43ArrayParam (unsigned dimension)
{
	_value = dimension ? ATOM_NEW_ARRAY(ATOM_Matrix3x4f, dimension) : 0;
	_dimension = dimension;
}

ATOM_MaterialMatrix43ArrayParam::ATOM_MaterialMatrix43ArrayParam (unsigned dimension, const ATOM_Matrix3x4f *value)
{
	_value = dimension ? ATOM_NEW_ARRAY(ATOM_Matrix3x4f, dimension) : 0;
	_dimension = dimension;

	if (value)
	{
		memcpy (_value, value, sizeof(ATOM_Matrix3x4f)*dimension);
	}
}

ATOM_MaterialMatrix43ArrayParam::~ATOM_MaterialMatrix43ArrayParam (void)
{
	ATOM_DELETE_ARRAY (_value);
}

ATOM_MaterialParam::Type ATOM_MaterialMatrix43ArrayParam::getParamType (void) const
{
	return ParamType_Matrix43Array;
}

unsigned ATOM_MaterialMatrix43ArrayParam::getDimension (void) const
{
	return _dimension;
}

void ATOM_MaterialMatrix43ArrayParam::setDimension (unsigned dimension)
{
	if (dimension != _dimension)
	{
		ATOM_DELETE_ARRAY(_value);
		_value = dimension ? ATOM_NEW_ARRAY(ATOM_Matrix3x4f, dimension) : 0;
		_dimension = dimension;
	}
}

unsigned ATOM_MaterialMatrix43ArrayParam::getRegisterCount (void)
{
	return _dimension * 3;
}

void ATOM_MaterialMatrix43ArrayParam::writeRegisterValue (ATOM_Vector4f *buffer)
{
	memcpy (buffer, _value, _dimension*sizeof(float)*12);
}

void ATOM_MaterialMatrix43ArrayParam::writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc)
{
	int n = desc->numElements;
	if (n > _dimension)
	{
		n = _dimension;
	}

	switch (desc->registerCount)
	{
	case 1:
		{
			const ATOM_Vector4f *v = (const ATOM_Vector4f*)valuePtr;
			for (int i = 0; i < n; ++i, v+=3)
			{
				*buffer++ = v[0];
			}
			break;
		}
	case 2:
		{
			const ATOM_Vector4f *v = (const ATOM_Vector4f*)valuePtr;
			for (int i = 0; i < n; ++i, v+=3)
			{
				*buffer++ = v[0];
				*buffer++ = v[1];
			}
			break;
		}
	case 3:
		{
			memcpy (buffer, valuePtr, n*sizeof(float)*12);
			break;
		}
	}
}

unsigned ATOM_MaterialMatrix43ArrayParam::getValueSize (void) const
{
	return sizeof(ATOM_Matrix3x4f);
}

void *ATOM_MaterialMatrix43ArrayParam::getValuePtr (void)
{
	return _value;
}

void ATOM_MaterialMatrix43ArrayParam::setValue (const ATOM_Matrix3x4f *value, unsigned count)
{
	if (value)
	{
		if (count > _dimension)
			count = _dimension;

		memcpy(_value, value, count*sizeof(ATOM_Matrix3x4f));
	}
}

ATOM_Matrix3x4f *ATOM_MaterialMatrix43ArrayParam::getValue (void) const
{
	return _value;
}

void ATOM_MaterialMatrix43ArrayParam::setElement (unsigned index, const ATOM_Matrix3x4f &value)
{
	if (index < _dimension)
	{
		_value[index] = value;
	}
}

ATOM_MaterialVectorParam::ATOM_MaterialVectorParam (void)
{
	_value.set(0.f, 0.f, 0.f, 0.f);
}

ATOM_MaterialVectorParam::ATOM_MaterialVectorParam (const ATOM_Vector4f &value)
{
	_value = value;
}

ATOM_MaterialParam::Type ATOM_MaterialVectorParam::getParamType (void) const
{
	return ParamType_Vector;
}

unsigned ATOM_MaterialVectorParam::getValueSize (void) const
{
	return sizeof(ATOM_Vector4f);
}

void *ATOM_MaterialVectorParam::getValuePtr (void)
{
	return &_value;
}

unsigned ATOM_MaterialVectorParam::getRegisterCount (void)
{
	return 1;
}

void ATOM_MaterialVectorParam::writeRegisterValue (ATOM_Vector4f *buffer)
{
	*buffer = _value;
}

void ATOM_MaterialVectorParam::writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc)
{
	*buffer = *((const ATOM_Vector4f*)valuePtr);
}

void ATOM_MaterialVectorParam::setValue (const ATOM_Vector4f &value)
{
	_value = value;
}

const ATOM_Vector4f &ATOM_MaterialVectorParam::getValue (void) const
{
	return _value;
}

ATOM_MaterialVectorArrayParam::ATOM_MaterialVectorArrayParam (unsigned dimension)
{
	_value = dimension ? ATOM_NEW_ARRAY(ATOM_Vector4f, dimension) : 0;
	_dimension = dimension;
}

ATOM_MaterialVectorArrayParam::ATOM_MaterialVectorArrayParam (unsigned dimension, const ATOM_Vector4f *value)
{
	_value = dimension ? ATOM_NEW_ARRAY(ATOM_Vector4f, dimension) : 0;
	_dimension = dimension;

	if (value)
	{
		memcpy (_value, value, sizeof(ATOM_Vector4f)*dimension);
	}
}

ATOM_MaterialVectorArrayParam::~ATOM_MaterialVectorArrayParam (void)
{
	ATOM_DELETE_ARRAY (_value);
}

ATOM_MaterialParam::Type ATOM_MaterialVectorArrayParam::getParamType (void) const
{
	return ParamType_VectorArray;
}

unsigned ATOM_MaterialVectorArrayParam::getDimension (void) const
{
	return _dimension;
}

void ATOM_MaterialVectorArrayParam::setDimension (unsigned dimension)
{
	if (dimension != _dimension)
	{
		ATOM_DELETE_ARRAY(_value);
		_value = dimension ? ATOM_NEW_ARRAY(ATOM_Vector4f, dimension) : 0;
		_dimension = dimension;
	}
}

unsigned ATOM_MaterialVectorArrayParam::getRegisterCount (void)
{
	return _dimension;
}

void ATOM_MaterialVectorArrayParam::writeRegisterValue (ATOM_Vector4f *buffer)
{
	memcpy (buffer, _value, _dimension*sizeof(float)*4);
}

void ATOM_MaterialVectorArrayParam::writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc)
{
	int n = desc->numElements;
	if (n > _dimension)
	{
		n = _dimension;
	}
	memcpy (buffer, valuePtr, n*sizeof(float)*4);
}

unsigned ATOM_MaterialVectorArrayParam::getValueSize (void) const
{
	return sizeof(ATOM_Vector4f);
}

void *ATOM_MaterialVectorArrayParam::getValuePtr (void)
{
	return _value;
}

void ATOM_MaterialVectorArrayParam::setValue (const ATOM_Vector4f *value, unsigned count)
{
	if (value)
	{
		if (count > _dimension)
			count = _dimension;

		memcpy(_value, value, count*sizeof(ATOM_Vector4f));
	}
}

ATOM_Vector4f *ATOM_MaterialVectorArrayParam::getValue (void) const
{
	return _value;
}

void ATOM_MaterialVectorArrayParam::setElement (unsigned index, const ATOM_Vector4f &value)
{
	if (index < _dimension)
	{
		_value[index] = value;
	}
}

ATOM_MaterialTextureParam::ATOM_MaterialTextureParam (void)
{
}

ATOM_MaterialTextureParam::ATOM_MaterialTextureParam (ATOM_Texture *value)
{
	_value = value;
}

ATOM_MaterialParam::Type ATOM_MaterialTextureParam::getParamType (void) const
{
	return ParamType_Texture;
}

unsigned ATOM_MaterialTextureParam::getValueSize (void) const
{
	return sizeof(ATOM_AUTOREF(ATOM_Texture));
}

void *ATOM_MaterialTextureParam::getValuePtr (void)
{
	return &_value;
}

void ATOM_MaterialTextureParam::setValue (ATOM_Texture *value)
{
	_value = value;
}

ATOM_Texture *ATOM_MaterialTextureParam::getValue (void) const
{
	return _value.get();
}

void ATOM_MaterialTextureParam::setFileName (const char *filename)
{
	_filename = filename ? filename : "";
}

const char *ATOM_MaterialTextureParam::getFileName (void) const
{
	return _filename.c_str();
}

ATOM_MaterialSamplerParam::ATOM_MaterialSamplerParam (void)
{
	_index = 0;
}

ATOM_MaterialParam::Type ATOM_MaterialSamplerParam::getParamType (void) const
{
	return ATOM_MaterialParam::ParamType_Sampler;
}

unsigned ATOM_MaterialSamplerParam::getValueSize (void) const
{
	return 0;
}

void *ATOM_MaterialSamplerParam::getValuePtr (void)
{
	return 0;
}

void ATOM_MaterialSamplerParam::setSamplerStates (ATOM_SamplerAttributes *value)
{
	_samplerStates = value;
}

ATOM_SamplerAttributes *ATOM_MaterialSamplerParam::getSamplerStates (void) const
{
	return _samplerStates.get();
}

void ATOM_MaterialSamplerParam::setIndex (unsigned index)
{
	_index = index;
}

unsigned ATOM_MaterialSamplerParam::getIndex (void) const
{
	return _index;
}

void ATOM_MaterialSamplerParam::setTexParam (ATOM_MaterialTextureParam *param)
{
	_texParam = param;
}

ATOM_MaterialTextureParam *ATOM_MaterialSamplerParam::getTexParam (void) const
{
	return _texParam.get();
}

/*
ATOM_EffectParameterValue::ATOM_EffectParameterValue (void)
{
	type = ATOM_MaterialParam::ParamType_Unknown;
	handle = 0;
	dataSize = 0;
	arraySize = 0;
	p = 0;
}

ATOM_EffectParameterValue::ATOM_EffectParameterValue (ATOM_MaterialParam::Type Type)
{
	type = Type;
	handle = 0;
	dataSize = 0;
	arraySize = 0;
	p = 0;
}
*/
ATOM_EffectParameterValue::ATOM_EffectParameterValue (const char *name, ATOM_MaterialParam *param)
{
	handle = param;
	dimension = 1;
	parameterName = name;
	textureFileName = "";
	//--- wangjian added ---//
	isValidationFlag = 0;
	validationBitIndex = -1;
	//----------------------//

	switch (handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Texture:
		{
			ATOM_MaterialTextureParam *textureParam = (ATOM_MaterialTextureParam*)handle;
			t = textureParam->getValue();
			if (t)
			{
				t->addRef ();
			}
			textureFileName = textureParam->getFileName();
			break;
		}
	case ATOM_MaterialParam::ParamType_Sampler:
		p = 0;
		break;
	default:
		dimension = handle->getDimension ();
		p = ATOM_MALLOC(dimension*handle->getValueSize());
		memcpy (p, handle->getValuePtr(), dimension*handle->getValueSize());
		break;
	}
}

ATOM_EffectParameterValue::ATOM_EffectParameterValue (const ATOM_EffectParameterValue &other)
{
	handle = other.handle;
	parameterName = other.parameterName;

	//--- wangjian added ---//
	isValidationFlag = other.isValidationFlag;
	validationBitIndex = other.validationBitIndex;
	//----------------------//

	switch (handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Texture:
		{
			t = other.t;
			if (t)
			{
				t->addRef ();
			}
			textureFileName = other.textureFileName;
			break;
		}
	case ATOM_MaterialParam::ParamType_Sampler:
		p = 0;
		break;
	default:
		dimension = handle->getDimension();
		p = ATOM_MALLOC(dimension*handle->getValueSize());
		memcpy (p, other.p, dimension*handle->getValueSize());
		break;
	}
}

ATOM_EffectParameterValue::~ATOM_EffectParameterValue (void)
{
	switch (handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Texture:
		{
			if (t)
			{
				ATOM_AUTOREF(ATOM_Texture) tmp = t;
				t->decRef ();
			}
			break;
		}
	case ATOM_MaterialParam::ParamType_Sampler:
		break;
	default:
		ATOM_FREE(p);
		break;
	}
}

void ATOM_EffectParameterValue::swap (ATOM_EffectParameterValue &other)
{
	std::swap (handle, other.handle);
	std::swap (dimension, other.dimension);
	std::swap (parameterName, other.parameterName);
	std::swap (textureFileName, other.textureFileName);
	std::swap (p, other.p);
	//--- wangjian added ---//
	std::swap (isValidationFlag,other.isValidationFlag);
	//----------------------//
}

const char *ATOM_EffectParameterValue::getParameterName (void) const
{
	return parameterName.c_str();
}

ATOM_EffectParameterValue &ATOM_EffectParameterValue::operator = (const ATOM_EffectParameterValue &other)
{
	ATOM_EffectParameterValue tmp(other);
	swap (tmp);
	return *this;
}

bool ATOM_EffectParameterValue::isAutoParameter (void) const
{
	return handle && handle->getValueCallback();
}

bool ATOM_EffectParameterValue::isSameValueAs (const ATOM_EffectParameterValue &other) const
{
	if (this == &other)
	{
		return true;
	}

	int type1 = handle ? handle->getParamType() : -1;
	int type2 = other.handle ? other.handle->getParamType() : -1;
	if (type1 != type2)
	{
		return false;
	}

	//--- wangjian added ---//
	if( isValidationFlag != other.isValidationFlag )
	{
		return false;
	}
	//-----------------------//


	switch (type1)
	{
	case ATOM_MaterialParam::ParamType_Float:
		if (*f != *other.f)
		{
			return false;
		}
		break;
	case ATOM_MaterialParam::ParamType_FloatArray:
		if (dimension != other.dimension)
		{
			return false;
		}
		if (memcmp(f, other.f, sizeof(float)*dimension))
		{
			return false;
		}
		break;
	case ATOM_MaterialParam::ParamType_Int:
		if (*i != *other.i)
		{
			return false;
		}
		break;
	case ATOM_MaterialParam::ParamType_IntArray:
		if (dimension != other.dimension)
		{
			return false;
		}
		if (memcmp(i, other.i, sizeof(int)*dimension))
		{
			return false;
		}
		break;
	case ATOM_MaterialParam::ParamType_Vector:
		if (*v != *other.v)
		{
			return false;
		}
		break;
	case ATOM_MaterialParam::ParamType_VectorArray:
		if (dimension != other.dimension)
		{
			return false;
		}
		if (memcmp(v, other.v, sizeof(ATOM_Vector4f)*dimension))
		{
			return false;
		}
		break;
	case ATOM_MaterialParam::ParamType_Matrix44:
		if (*m44 != *other.m44)
		{
			return false;
		}
		break;
	case ATOM_MaterialParam::ParamType_Matrix44Array:
		if (dimension != other.dimension)
		{
			return false;
		}
		if (memcmp(m44, other.m44, sizeof(ATOM_Matrix4x4f)*dimension))
		{
			return false;
		}
		break;
	case ATOM_MaterialParam::ParamType_Matrix43:
		if (*m43 != *other.m43)
		{
			return false;
		}
		break;
	case ATOM_MaterialParam::ParamType_Matrix43Array:
		if (dimension != other.dimension)
		{
			return false;
		}
		if (memcmp(m43, other.m43, sizeof(ATOM_Matrix3x4f)*dimension))
		{
			return false;
		}
		break;
	case ATOM_MaterialParam::ParamType_Texture:
		if (textureFileName != other.textureFileName)
		{
			return false;
		}
		if (t != other.t)
		{
			return false;
		}
		break;
	}

	return true;
}

bool ATOM_EffectParameterValue::apply (ATOM_CoreMaterial *material)
{
	switch (handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Float:
		material->setFloat (handle, *f);
		break;
	case ATOM_MaterialParam::ParamType_FloatArray:
		material->setFloatArray (handle, f, dimension);
		break;
	case ATOM_MaterialParam::ParamType_Int:
		material->setInt (handle, *i);
		break;
	case ATOM_MaterialParam::ParamType_IntArray:
		material->setIntArray (handle, i, dimension);
		break;
	case ATOM_MaterialParam::ParamType_Vector:
		material->setVector (handle, *v);
		break;
	case ATOM_MaterialParam::ParamType_VectorArray:
		material->setVectorArray (handle, v, dimension);
		break;
	case ATOM_MaterialParam::ParamType_Matrix44:
		material->setMatrix44 (handle, *m44);
		break;
	case ATOM_MaterialParam::ParamType_Matrix44Array:
		material->setMatrix44Array (handle, m44, dimension);
		break;
	case ATOM_MaterialParam::ParamType_Matrix43:
		material->setMatrix43 (handle, *m43);
		break;
	case ATOM_MaterialParam::ParamType_Matrix43Array:
		material->setMatrix43Array (handle, m43, dimension);
		break;
	case ATOM_MaterialParam::ParamType_Texture:
		material->setTexture (handle, t);
		break;
	default:
		return false;
	}

	return true;
}

void ATOM_EffectParameterValue::setFloat (float value)
{
	ATOM_ASSERT (handle->getParamType() == ATOM_MaterialParam::ParamType_Float);
	if (handle->getParamType() == ATOM_MaterialParam::ParamType_Float)
		*f = value;
}

void ATOM_EffectParameterValue::setFloatArray (const float *values, unsigned count)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_FloatArray);
	if (count <= handle->getDimension ())
	{
		memcpy (f, values, count*sizeof(float));
		dimension = count;
	}
}

void ATOM_EffectParameterValue::setInt (int value)
{
	ATOM_ASSERT (handle->getParamType() == ATOM_MaterialParam::ParamType_Int);
	if (handle->getParamType() == ATOM_MaterialParam::ParamType_Int)
		*i = value;
}

void ATOM_EffectParameterValue::setIntArray (const int *values, unsigned count)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_IntArray);
	if (count <= handle->getDimension ())
	{
		memcpy (i, values, count*sizeof(int));
		dimension = count;
	}
}

void ATOM_EffectParameterValue::setVector (const ATOM_Vector4f &value)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_Vector);

	*v = value;
}

void ATOM_EffectParameterValue::setVector (const ATOM_Vector3f &value)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_Vector);

	v->set (value.x, value.y, value.z, 0.f);
}

void ATOM_EffectParameterValue::setVector (const ATOM_Vector2f &value)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_Vector);

	v->set (value.x, value.y, 0.f, 0.f);
}

void ATOM_EffectParameterValue::setVectorArray (const ATOM_Vector4f *values, unsigned count)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_VectorArray);
	ATOM_ASSERT(count <= handle->getDimension ());

	memcpy (v, values, count*sizeof(ATOM_Vector4f));
	dimension = count;
}

void ATOM_EffectParameterValue::setVectorArray (const ATOM_Vector3f *values, unsigned count)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_VectorArray);
	ATOM_ASSERT(count <= handle->getDimension ());
	for (unsigned i = 0; i < count; ++i)
	{
		v[i].set (values[i].x, values[i].y, values[i].z, 0.f);
	}
	dimension = count;
}

void ATOM_EffectParameterValue::setVectorArray (const ATOM_Vector2f *values, unsigned count)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_VectorArray);
	ATOM_ASSERT(count <= handle->getDimension ());
	for (unsigned i = 0; i < count; ++i)
	{
		v[i].set (values[i].x, values[i].y, 0.f, 0.f);
	}
	dimension = count;
}

void ATOM_EffectParameterValue::setMatrix44 (const ATOM_Matrix4x4f &value)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);

	*m44 = value;
}

void ATOM_EffectParameterValue::setMatrix44Array (const ATOM_Matrix4x4f *values, unsigned count)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44Array);
	ATOM_ASSERT(count <= handle->getDimension ());

	memcpy (m44, values, count*sizeof(ATOM_Matrix4x4f));
	dimension = count;
}

void ATOM_EffectParameterValue::setMatrix43 (const ATOM_Matrix3x4f &value)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix43);

	*m43 = value;
}

void ATOM_EffectParameterValue::setMatrix43Array (const ATOM_Matrix3x4f *values, unsigned count)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix43Array);
	ATOM_ASSERT(count <= handle->getDimension ());

	memcpy (m43, values, count*sizeof(ATOM_Matrix3x4f));
	dimension = count;
}

void ATOM_EffectParameterValue::setTexture (ATOM_Texture *value)
{
	ATOM_ASSERT(handle->getParamType() == ATOM_MaterialParam::ParamType_Texture);
	
	if (t)
	{
		ATOM_AUTOREF(ATOM_Texture) tmp = t;
		t->decRef ();
	}

	t = value;

	if (t)
	{
		t->addRef ();
	}

	textureFileName = "";
}
//--- wangjian modified ---//
// 异步加载 ：在该方法中，会有加载纹理的操作，所以加了一个加载优先级（默认为异步加载）
void ATOM_EffectParameterValue::setTexture (const char *filename, int texLoadPriority/* = 0*/)
{
	//--- wangjian modified ---//
	ATOM_STRING textureFile = textureFileName;
	if( filename && filename[0] != '\0' )
		textureFile = filename;

	// 异步加载
	ATOM_AUTOREF(ATOM_Texture) texture = ATOM_CreateTextureResource (textureFile.c_str(),ATOM_PIXEL_FORMAT_UNKNOWN,texLoadPriority);
	//-------------------------//

	setTexture (texture.get());

	textureFileName = textureFile;
}
//------------------------//

