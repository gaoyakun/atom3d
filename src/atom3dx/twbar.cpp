#include "StdAfx.h"
#include "twbar.h"
#include "tw_events.h"
#include "tw_wrapper.h"
#include "atom_graph.h"

bool ATOMX_TweakBar::_initialized = false;

ATOM_BEGIN_EVENT_MAP_NOPARENT(ATOMX_TweakBar)
	ATOM_EVENT_HANDLER(ATOMX_TweakBar, ATOMX_TWValueChangedEvent, onValueChanged)
	ATOM_EVENT_HANDLER(ATOMX_TweakBar, ATOMX_TWCommandEvent, onCommand)
ATOM_END_EVENT_MAP

static const TwStructMember vfStructMembers[] = {
	{ "x", TW_TYPE_FLOAT, offsetof(ATOM_Vector4f, x), "" },
	{ "y", TW_TYPE_FLOAT, offsetof(ATOM_Vector4f, y), "" },
	{ "z", TW_TYPE_FLOAT, offsetof(ATOM_Vector4f, z), "" },
	{ "w", TW_TYPE_FLOAT, offsetof(ATOM_Vector4f, w), "" }
};

static void TW_CALL VectorSummaryCallback (char *summaryStr, size_t summaryMaxLength, const void *value, void *clientData)
{
	const float *f = (const float*)value;
	unsigned numElements = (unsigned)clientData;
	switch (numElements)
	{
	case 2:
		_snprintf (summaryStr, summaryMaxLength, "(%.3f, %.3f)", f[0], f[1]);
		break;
	case 3:
		_snprintf (summaryStr, summaryMaxLength, "(%.3f, %.3f, %.3f)", f[0], f[1], f[2]);
		break;
	case 4:
		_snprintf (summaryStr, summaryMaxLength, "(%.3f, %.3f, %.3f, %.3f)", f[0], f[1], f[2], f[3]);
		break;
	default:
		summaryStr[0] = '\0';
		break;
	}
}

static TwType twGetVector2fType (void)
{
	static TwType type = TW_TYPE_UNDEF;
	if (type == TW_TYPE_UNDEF)
	{
		type = TwDefineStruct("Vector2f", vfStructMembers, 2, 2 * sizeof(float), &VectorSummaryCallback, (void*)2);
	}
	return type;
}

static TwType twGetVector3fType (void)
{
	static TwType type = TW_TYPE_UNDEF;
	if (type == TW_TYPE_UNDEF)
	{
		type = TwDefineStruct("Vector3f", vfStructMembers, 3, 3 * sizeof(float), &VectorSummaryCallback, (void*)3);
	}
	return type;
}

static TwType twGetVector4fType (void)
{
	static TwType type = TW_TYPE_UNDEF;
	if (type == TW_TYPE_UNDEF)
	{
		type = TwDefineStruct("Vector4f", vfStructMembers, 4, 4 * sizeof(float), &VectorSummaryCallback, (void*)4);
	}
	return type;
}

ATOMX_TBValue::ATOMX_TBValue (void)
{
	_type = ATOMX_TBTYPE_UNKNOWN;
	_s = 0;
}

ATOMX_TBValue::ATOMX_TBValue (ATOMX_TBType type)
{
	_type = type;
	_s = 0;
}

ATOMX_TBValue::ATOMX_TBValue (const ATOMX_TBValue &rhs)
{
	_type = rhs._type;

	if (_type == ATOMX_TBTYPE_STRING)
	{
		char *src = rhs._s;

		if (src)
		{
			_s = (char*)ATOM_MALLOC(strlen(src) + 1);
			strcpy (_s, src);
		}
		else
		{
			_s = 0;
		}
	}
	else
	{
		memcpy (this, &rhs, sizeof(ATOMX_TBValue));
	}
}

void ATOMX_TBValue::swap (ATOMX_TBValue &rhs)
{
	char buffer[sizeof(ATOMX_TBValue)];
	memcpy (buffer, this, sizeof(ATOMX_TBValue));
	memcpy (this, &rhs, sizeof(ATOMX_TBValue));
	memcpy (&rhs, buffer, sizeof(ATOMX_TBValue));
}

ATOMX_TBValue & ATOMX_TBValue::operator = (const ATOMX_TBValue &rhs)
{
	if (this != &rhs)
	{
		ATOMX_TBValue tmp(rhs);
		swap (tmp);
	}

	return *this;
}

ATOMX_TBValue::~ATOMX_TBValue (void)
{
	clear ();
}

void ATOMX_TBValue::clear (void)
{
	if (_type == ATOMX_TBTYPE_STRING)
	{
		ATOM_FREE(_s);
		_s = 0;
	}
	_type = ATOMX_TBTYPE_UNKNOWN;
}

ATOMX_TBType ATOMX_TBValue::getType (void) const
{
	return _type;
}

void ATOMX_TBValue::setI (ATOMX_TBType type, int value)
{
	clear ();
	_type = type;

	switch (type)
	{
	case ATOMX_TBTYPE_CHAR:
		_c = value;
		break;
	case ATOMX_TBTYPE_INT8:
		_i8 = value;
		break;
	case ATOMX_TBTYPE_UINT8:
		_ui8 = value;
		break;
	case ATOMX_TBTYPE_INT16:
		_i16 = value;
		break;
	case ATOMX_TBTYPE_UINT16:
		_ui16 = value;
		break;
	case ATOMX_TBTYPE_INT32:
		_i32 = value;
		break;
	case ATOMX_TBTYPE_UINT32:
		_ui32 = value;
		break;
	case ATOMX_TBTYPE_FLOAT:
		_f = value;
		break;
	case ATOMX_TBTYPE_BOOL:
		_b = ((value != 0) ? 1 : 0);
		break;
	default:
		throw std::runtime_error("Invalid tweakbar value type");
	}
}

void ATOMX_TBValue::setU (ATOMX_TBType type, unsigned value)
{
	clear ();
	_type = type;

	switch (type)
	{
	case ATOMX_TBTYPE_CHAR:
		_c = value;
		break;
	case ATOMX_TBTYPE_INT8:
		_i8 = value;
		break;
	case ATOMX_TBTYPE_UINT8:
		_ui8 = value;
		break;
	case ATOMX_TBTYPE_INT16:
		_i16 = value;
		break;
	case ATOMX_TBTYPE_UINT16:
		_ui16 = value;
		break;
	case ATOMX_TBTYPE_INT32:
		_i32 = value;
		break;
	case ATOMX_TBTYPE_UINT32:
		_ui32 = value;
		break;
	case ATOMX_TBTYPE_FLOAT:
		_f = value;
		break;
	case ATOMX_TBTYPE_BOOL:
		_b = ((value != 0) ? 1 : 0);
		break;
	default:
		throw std::runtime_error("Invalid tweakbar value type");
	}
}

void ATOMX_TBValue::setB (ATOMX_TBType type, bool b)
{
	clear ();
	_type = type;

	int num = b ? 1 : 0;

	switch (type)
	{
	case ATOMX_TBTYPE_CHAR:
		_c = num;
		break;
	case ATOMX_TBTYPE_INT8:
		_i8 = num;
		break;
	case ATOMX_TBTYPE_UINT8:
		_ui8 = num;
		break;
	case ATOMX_TBTYPE_INT16:
		_i16 = num;
		break;
	case ATOMX_TBTYPE_UINT16:
		_ui16 = num;
		break;
	case ATOMX_TBTYPE_INT32:
		_i32 = num;
		break;
	case ATOMX_TBTYPE_UINT32:
		_ui32 = num;
		break;
	case ATOMX_TBTYPE_FLOAT:
		_f = num;
		break;
	case ATOMX_TBTYPE_BOOL:
		_b = (b ? 1 : 0);
		break;
	default:
		throw std::runtime_error("Invalid tweakbar value type");
	}
}

void ATOMX_TBValue::setF (ATOMX_TBType type, float f)
{
	clear ();
	_type = type;

	switch (type)
	{
	case ATOMX_TBTYPE_CHAR:
		_c = f;
		break;
	case ATOMX_TBTYPE_INT8:
		_i8 = f;
		break;
	case ATOMX_TBTYPE_UINT8:
		_ui8 = f;
		break;
	case ATOMX_TBTYPE_INT16:
		_i16 = f;
		break;
	case ATOMX_TBTYPE_UINT16:
		_ui16 = f;
		break;
	case ATOMX_TBTYPE_INT32:
		_i32 = f;
		break;
	case ATOMX_TBTYPE_UINT32:
		_ui32 = f;
		break;
	case ATOMX_TBTYPE_FLOAT:
		_f = f;
		break;
	case ATOMX_TBTYPE_BOOL:
		_b = ((f != 0) ? 1 : 0);
		break;
	default:
		throw std::runtime_error("Invalid tweakbar value type");
	}
}

void ATOMX_TBValue::set2F (ATOMX_TBType type, float a, float b)
{
	clear ();
	_type = type;

	switch (type)
	{
	case ATOMX_TBTYPE_VECTOR2F:
		_v2f[0] = a;
		_v2f[1] = b;
		break;
	default:
		throw std::runtime_error("Invalid tweakbar value type");
	}
}

void ATOMX_TBValue::set3F (ATOMX_TBType type, float a, float b, float c)
{
	clear ();
	_type = type;

	switch (type)
	{
	case ATOMX_TBTYPE_COLOR3F:
		_c3f[0] = a;
		_c3f[1] = b;
		_c3f[2] = c;
		break;
	case ATOMX_TBTYPE_DIR3F:
		_d3f[0] = a;
		_d3f[1] = b;
		_d3f[2] = c;
		break;
	case ATOMX_TBTYPE_COLOR4F:
		_c4f[0] = a;
		_c4f[1] = b;
		_c4f[2] = c;
		_c4f[3] = 1.f;
		break;
	case ATOMX_TBTYPE_VECTOR3F:
		_v3f[0] = a;
		_v3f[1] = b;
		_v3f[2] = c;
		break;
	default:
		throw std::runtime_error("Invalid tweakbar value type");
	}
}

void ATOMX_TBValue::set4F (ATOMX_TBType type, float a, float b, float c, float d)
{
	clear ();
	_type = type;

	switch (type)
	{
	case ATOMX_TBTYPE_COLOR4F:
		_c4f[0] = a;
		_c4f[1] = b;
		_c4f[2] = c;
		_c4f[3] = d;
		break;
	case ATOMX_TBTYPE_QUAT4F:
		_q4f[0] = a;
		_q4f[1] = b;
		_q4f[2] = c;
		_q4f[3] = d;
		break;
	case ATOMX_TBTYPE_VECTOR4F:
		_v4f[0] = a;
		_v4f[1] = b;
		_v4f[2] = c;
		_v4f[3] = d;
		break;
	default:
		throw std::runtime_error("Invalid tweakbar value type");
	}
}

void ATOMX_TBValue::setS (ATOMX_TBType type, const char *s)
{
	clear ();
	_type = ATOMX_TBTYPE_STRING;

	if (s)
	{
		_s = (char*)ATOM_MALLOC(strlen(s) + 1);
		strcpy (_s, s);
	}
	else
	{
		_s = 0;
	}
}

int ATOMX_TBValue::getI (void) const
{
	switch (_type)
	{
	case ATOMX_TBTYPE_BOOL:	return _b ? 1 : 0;
	case ATOMX_TBTYPE_CHAR:	return _c;
	case ATOMX_TBTYPE_INT8:	return _i8;
	case ATOMX_TBTYPE_UINT8: return _ui8;
	case ATOMX_TBTYPE_INT16: return _i16;
	case ATOMX_TBTYPE_UINT16: return _ui16;
	case ATOMX_TBTYPE_INT32: return _i32;
	case ATOMX_TBTYPE_UINT32: return _ui32;
	case ATOMX_TBTYPE_FLOAT: return _f;
	default: throw std::runtime_error("Invalid tweakbar value type");
	}
}

unsigned ATOMX_TBValue::getU (void) const
{
	switch (_type)
	{
	case ATOMX_TBTYPE_BOOL:	return _b ? 1 : 0;
	case ATOMX_TBTYPE_CHAR:	return _c;
	case ATOMX_TBTYPE_INT8:	return _i8;
	case ATOMX_TBTYPE_UINT8: return _ui8;
	case ATOMX_TBTYPE_INT16: return _i16;
	case ATOMX_TBTYPE_UINT16: return _ui16;
	case ATOMX_TBTYPE_INT32: return _i32;
	case ATOMX_TBTYPE_UINT32: return _ui32;
	case ATOMX_TBTYPE_FLOAT: return _f;
	default: throw std::runtime_error("Invalid tweakbar value type");
	}
}

bool ATOMX_TBValue::getB (void) const
{
	switch (_type)
	{
	case ATOMX_TBTYPE_BOOL:	return _b != 0;
	case ATOMX_TBTYPE_CHAR:	return _c != 0;
	case ATOMX_TBTYPE_INT8:	return _i8 != 0;
	case ATOMX_TBTYPE_UINT8: return _ui8 != 0;
	case ATOMX_TBTYPE_INT16: return _i16 != 0;
	case ATOMX_TBTYPE_UINT16: return _ui16 != 0;
	case ATOMX_TBTYPE_INT32: return _i32 != 0;
	case ATOMX_TBTYPE_UINT32: return _ui32 != 0;
	case ATOMX_TBTYPE_FLOAT: return _f != 0;
	default: throw std::runtime_error("Invalid tweakbar value type");
	}
}

float ATOMX_TBValue::getF (void) const
{
	switch (_type)
	{
	case ATOMX_TBTYPE_BOOL:	return _b ? 1.f : 0.f;
	case ATOMX_TBTYPE_CHAR:	return _c;
	case ATOMX_TBTYPE_INT8:	return _i8;
	case ATOMX_TBTYPE_UINT8: return _ui8;
	case ATOMX_TBTYPE_INT16: return _i16;
	case ATOMX_TBTYPE_UINT16: return _ui16;
	case ATOMX_TBTYPE_INT32: return _i32;
	case ATOMX_TBTYPE_UINT32: return _ui32;
	case ATOMX_TBTYPE_FLOAT: return _f;
	default: throw std::runtime_error("Invalid tweakbar value type");
	}
}

const float *ATOMX_TBValue::get2F (void) const
{
	switch (_type)
	{
	case ATOMX_TBTYPE_VECTOR2F: return _v2f;
	default: throw std::runtime_error("Invalid tweakbar value type");
	}
}

const float *ATOMX_TBValue::get3F (void) const
{
	switch (_type)
	{
	case ATOMX_TBTYPE_COLOR3F:	return _c3f;
	case ATOMX_TBTYPE_COLOR4F:	return _c4f;
	case ATOMX_TBTYPE_DIR3F:	return _d3f;
	case ATOMX_TBTYPE_QUAT4F: return _q4f;
	case ATOMX_TBTYPE_VECTOR3F: return _v3f;
	default: throw std::runtime_error("Invalid tweakbar value type");
	}
}

const float *ATOMX_TBValue::get4F (void) const
{
	switch (_type)
	{
	case ATOMX_TBTYPE_COLOR4F:	return _c4f;
	case ATOMX_TBTYPE_QUAT4F: return _q4f;
	case ATOMX_TBTYPE_VECTOR4F: return _v4f;
	default: throw std::runtime_error("Invalid tweakbar value type");
	}
}

const char *ATOMX_TBValue::getS (void) const
{
	if (_type == ATOMX_TBTYPE_STRING)
	{
		return _s;
	}
	else
	{
		throw std::runtime_error("Invalid tweakbar value type");
	}
}

ATOMX_TBEnum::ATOMX_TBEnum (void)
{
}

int ATOMX_TBEnum::findEnum (int value) const
{
	for (unsigned i = 0; i < _values.size(); ++i)
	{
		if (_values[i].value == value)
		{
			return i;
		}
	}
	return -1;
}

void ATOMX_TBEnum::addEnum (const char *name, int value)
{
	if (findEnum (value) >= 0)
	{
		throw std::runtime_error("Enum value already defined");
	}

	_values.resize (_values.size() + 1);
	_values.back().name = name;
	_values.back().value = value;
}

void ATOMX_TBEnum::removeEnum (int value)
{
	int index = findEnum (value);

	if (index >= 0)
	{
		_values.erase (_values.begin() + index);
	}
}

bool ATOMX_TBEnum::isValidEnum (int value) const
{
	return findEnum (value) >= 0;
}

void ATOMX_TBEnum::clear (void)
{
	_values.resize (0);
}

unsigned ATOMX_TBEnum::getNumEnums (void) const
{
	return _values.size ();
}

int ATOMX_TBEnum::getEnumValue (unsigned index) const
{
	return _values[index].value;
}

const char *ATOMX_TBEnum::getEnumName (unsigned index) const
{
	return _values[index].name.c_str();
}

ATOMX_TweakBar::ATOMX_TweakBar (const char *name)
{
	if (!name)
	{
		name = "";
	}

	_barName = name;
	_bar = 0;
	_enableNotifying = true;
	_valueChangedCallback = 0;
	_commandCallback = 0;

	if (ATOMX_TweakBar::_initialized)
	{
		_bar = TwNewBar (name);

		if (_bar)
		{
			setBarRefreshPeriod (0.1f);
		}
	}
}

ATOMX_TweakBar::~ATOMX_TweakBar (void)
{
	clear ();

	if (_initialized && _bar)
	{
		TwDeleteBar ((TwBar*)_bar);
	}

	_bar = 0;
}

bool ATOMX_TweakBar::isNotifyingEnabled (void) const
{
	return _enableNotifying;
}

void ATOMX_TweakBar::enableNotifying (bool enable)
{
	_enableNotifying = enable;
}

void TW_CALL ButtonCallback (void *clientData)
{
	if (ATOM_APP)
	{
		ATOMX_TweakBar::VariableBase *vb = (ATOMX_TweakBar::VariableBase*)clientData;

		ATOMX_TWCommandEvent e(vb->name.c_str(), vb->id, vb->userdata, vb->bar);
		vb->bar->handleEvent (&e);
	}
}

void TW_CALL setVariableCallback (const void *value, void *clientData)
{
	if (ATOM_APP)
	{
		ATOMX_TweakBar::Variable *var = (ATOMX_TweakBar::Variable*)clientData;

		ATOMX_TBValue &val = var->value;
		ATOMX_TBValue oldVal = val;
		if (var->writeToFunc)
		{
			var->writeToFunc (var->name.c_str(), oldVal, var->userDataWriteTo);
		}

		ATOMX_TBType type = val.getType();
		switch (type)
		{
		case ATOMX_TBTYPE_BOOL:
			{
				val.setB (type, *((int*)value));
				break;
			}
		case ATOMX_TBTYPE_CHAR:
			{
				val.setI (type, *((char*)value));
				break;
			}
		case ATOMX_TBTYPE_INT8:
			{
				val.setI (type, *((char*)value));
				break;
			}
		case ATOMX_TBTYPE_UINT8:
			{
				val.setU (type, *((unsigned char*)value));
				break;
			}
		case ATOMX_TBTYPE_INT16:
			{
				val.setI (type, *((short*)value));
				break;
			}
		case ATOMX_TBTYPE_UINT16:
			{
				val.setU (type, *((unsigned short*)value));
				break;
			}
		case ATOMX_TBTYPE_INT32:
			{
				val.setI (type, *((int*)value));
				break;
			}
		case ATOMX_TBTYPE_UINT32:
			{
				val.setU (type, *((unsigned int*)value));
				break;
			}
		case ATOMX_TBTYPE_FLOAT:
			{
				val.setF (type, *((float*)value));
				break;
			}
		case ATOMX_TBTYPE_STRING:
			{
				val.setS (type, *((char**)value));
				break;
			}
		case ATOMX_TBTYPE_QUAT4F:
		case ATOMX_TBTYPE_VECTOR4F:
		case ATOMX_TBTYPE_COLOR4F:
			{
				float *f = (float*)value;
				val.set4F (type, f[0], f[1], f[2], f[3]);
				break;
			}
		case ATOMX_TBTYPE_DIR3F:
		case ATOMX_TBTYPE_COLOR3F:
		case ATOMX_TBTYPE_VECTOR3F:
			{
				float *f = (float*)value;
				val.set3F (type, f[0], f[1], f[2]);
				break;
			}
		case ATOMX_TBTYPE_VECTOR2F:
			{
				float *f = (float*)value;
				val.set2F(type, f[0], f[1]);
				break;
			}
		default:
			{
				return;
			}
		}

		if (var->readFromFunc)
		{
			var->readFromFunc (var->name.c_str(), var->value, var->userDataReadFrom);
		}

		ATOMX_TWValueChangedEvent e(var->name.c_str(), oldVal, val, var->id, var->userdata, var->bar);
		var->bar->handleEvent (&e);
	}
}

void TW_CALL getVariableCallback (void *value, void *clientData)
{
	ATOMX_TweakBar::Variable *var = (ATOMX_TweakBar::Variable*)clientData;
	ATOMX_TBValue &val = var->value;
	if (var->writeToFunc)
	{
		var->writeToFunc (var->name.c_str(), var->value, var->userDataWriteTo);
	}

	ATOMX_TBType type = val.getType();
	switch (type)
	{
	case ATOMX_TBTYPE_BOOL:
		{
			*((int*)value) = val.getB();
			break;
		}
	case ATOMX_TBTYPE_CHAR:
		{
			*((char*)value) = val.getI();
			break;
		}
	case ATOMX_TBTYPE_INT8:
		{
			*((char*)value) = val.getI();
			break;
		}
	case ATOMX_TBTYPE_UINT8:
		{
			*((unsigned char*)value) = val.getU();
			break;
		}
	case ATOMX_TBTYPE_INT16:
		{
			*((short*)value) = val.getI();
			break;
		}
	case ATOMX_TBTYPE_UINT16:
		{
			*((unsigned short*)value) = val.getU();
			break;
		}
	case ATOMX_TBTYPE_INT32:
		{
			*((int*)value) = val.getI();
			break;
		}
	case ATOMX_TBTYPE_UINT32:
		{
			*((unsigned int*)value) = val.getU();
			break;
		}
	case ATOMX_TBTYPE_FLOAT:
		{
			*((float*)value) = val.getF();
			break;
		}
	case ATOMX_TBTYPE_COLOR3F:
	case ATOMX_TBTYPE_DIR3F:
	case ATOMX_TBTYPE_VECTOR3F:
		{
			float *f = (float*)value;
			const float *src = val.get3F();
			f[0] = src[0];
			f[1] = src[1];
			f[2] = src[2];
			break;
		}
	case ATOMX_TBTYPE_COLOR4F:
	case ATOMX_TBTYPE_QUAT4F:
	case ATOMX_TBTYPE_VECTOR4F:
		{
			float *f = (float*)value;
			const float *src = val.get4F();
			f[0] = src[0];
			f[1] = src[1];
			f[2] = src[2];
			f[3] = src[3];
			break;
		}
	case ATOMX_TBTYPE_VECTOR2F:
		{
			float *f = (float*)value;
			const float *src = val.get2F();
			f[0] = src[0];
			f[1] = src[1];
			break;
		}
	case ATOMX_TBTYPE_STRING:
		{
			TwCopyCDStringToLibrary ((char**)value, val.getS ());
			break;
		}
	default:
		{
			return;
		}
	}
}

static void TW_CALL CopyCDStringToClient(char **destPtr, const char *src)
{
	size_t srcLen = (src!=NULL) ? strlen(src) : 0;
	size_t destLen = (*destPtr!=NULL) ? strlen(*destPtr) : 0;

	// alloc or realloc dest memory block if needed
	if( *destPtr==NULL )
	*destPtr = (char *)ATOM_MALLOC(srcLen+1);
	else if( srcLen>destLen )
	*destPtr = (char *)ATOM_REALLOC(*destPtr, srcLen+1);

	// copy src
	if( srcLen>0 )
	strncpy(*destPtr, src, srcLen);
	(*destPtr)[srcLen] = '\0'; // null-terminated string
}

bool ATOMX_TweakBar::addImmediateVariable (const char *name, ATOMX_TBType varType, void *valueAddr, bool readonly, const char *group)
{
	if (!_bar || !_initialized)
	{
		return false;
	}

	TwType twType;

	switch (varType)
	{
	case ATOMX_TBTYPE_BOOL:
		twType = TW_TYPE_BOOL32;
		break;
	case ATOMX_TBTYPE_CHAR:
		twType = TW_TYPE_CHAR;
		break;
	case ATOMX_TBTYPE_INT8:
		twType = TW_TYPE_INT8;
		break;
	case ATOMX_TBTYPE_UINT8:
		twType = TW_TYPE_UINT8;
		break;
	case ATOMX_TBTYPE_INT16:
		twType = TW_TYPE_INT16;
		break;
	case ATOMX_TBTYPE_UINT16:
		twType = TW_TYPE_UINT16;
		break;
	case ATOMX_TBTYPE_INT32:
		twType = TW_TYPE_INT32;
		break;
	case ATOMX_TBTYPE_UINT32:
		twType = TW_TYPE_UINT32;
		break;
	case ATOMX_TBTYPE_FLOAT:
		twType = TW_TYPE_FLOAT;
		break;
	case ATOMX_TBTYPE_COLOR3F:
		twType = TW_TYPE_COLOR3F;
		break;
	case ATOMX_TBTYPE_COLOR4F:
		twType = TW_TYPE_COLOR4F;
		break;
	case ATOMX_TBTYPE_STRING:
		twType = TW_TYPE_CDSTRING;
		break;
	case ATOMX_TBTYPE_QUAT4F:
		twType = TW_TYPE_QUAT4F;
		break;
	case ATOMX_TBTYPE_DIR3F:
		twType = TW_TYPE_DIR3F;
		break;
	case ATOMX_TBTYPE_VECTOR2F:
		twType = twGetVector2fType ();
		break;
	case ATOMX_TBTYPE_VECTOR3F:
		twType = twGetVector3fType ();
		break;
	case ATOMX_TBTYPE_VECTOR4F:
		twType = twGetVector4fType ();
		break;
	default:
		return false;
	}

	ATOMX_TWAddVariableEvent e(name, -1, group, 0, this, ATOMX_TWAddVariableEvent::VT_VAR, true);
	ATOM_APP->handleEvent (&e);
	if (!e.allow)
	{
		return true;
	}

	char def[1024];
	if (e.group)
	{
		sprintf (def, "group='%s'", e.group);
	}
	else
	{
		def[0] = '\0';
	}

	if (!readonly)
	{
		return TwAddVarRW((TwBar*)_bar, e.name, twType, valueAddr, def) != 0;
	}
	else
	{
		return TwAddVarRO((TwBar*)_bar, e.name, twType, valueAddr, def) != 0;
	}
}

bool ATOMX_TweakBar::addVariable (const char *name, int id, const ATOMX_TBValue &var, bool readonly, const char *group, void *userData)
{
	if (!name || !_bar || !_initialized)
	{
		return false;
	}

	ATOMX_TWAddVariableEvent e(name, id, group, userData, this, ATOMX_TWAddVariableEvent::VT_VAR, true);
	ATOM_APP->handleEvent (&e);
	if (!e.allow)
	{
		return true;
	}

	ATOM_MAP<ATOM_STRING, ATOMX_TweakBar::VariableBase*>::iterator it = _variables.find (e.name);
	if (it != _variables.end ())
	{
		return false;
	}

	Variable *variable = ATOM_NEW(Variable, this);
	variable->name = e.name;
	variable->id = e.id;
	variable->value = var;
	variable->userdata = e.userdata;

	TwType twType;
	switch (var.getType())
	{
	case ATOMX_TBTYPE_BOOL:
		twType = TW_TYPE_BOOL32;
		break;
	case ATOMX_TBTYPE_CHAR:
		twType = TW_TYPE_CHAR;
		break;
	case ATOMX_TBTYPE_INT8:
		twType = TW_TYPE_INT8;
		break;
	case ATOMX_TBTYPE_UINT8:
		twType = TW_TYPE_UINT8;
		break;
	case ATOMX_TBTYPE_INT16:
		twType = TW_TYPE_INT16;
		break;
	case ATOMX_TBTYPE_UINT16:
		twType = TW_TYPE_UINT16;
		break;
	case ATOMX_TBTYPE_INT32:
		twType= TW_TYPE_INT32;
		break;
	case ATOMX_TBTYPE_UINT32:
		twType = TW_TYPE_UINT32;
		break;
	case ATOMX_TBTYPE_FLOAT:
		twType = TW_TYPE_FLOAT;
		break;
	case ATOMX_TBTYPE_COLOR3F:
		twType = TW_TYPE_COLOR3F;
		break;
	case ATOMX_TBTYPE_COLOR4F:
		twType = TW_TYPE_COLOR4F;
		break;
	case ATOMX_TBTYPE_STRING:
		twType = TW_TYPE_CDSTRING;
		break;
	case ATOMX_TBTYPE_QUAT4F:
		twType = TW_TYPE_QUAT4F;
		break;
	case ATOMX_TBTYPE_DIR3F:
		twType = TW_TYPE_DIR3F;
		break;
	case ATOMX_TBTYPE_VECTOR2F:
		twType = twGetVector2fType ();
		break;
	case ATOMX_TBTYPE_VECTOR3F:
		twType = twGetVector3fType ();
		break;
	case ATOMX_TBTYPE_VECTOR4F:
		twType = twGetVector4fType ();
		break;
	default:
		return false;
	}

	char def[1024];
	if (twType == TW_TYPE_DIR3F || twType == TW_TYPE_QUAT4F)
	{
		if (e.group)
		{
			sprintf (def, "group='%s' axisx=x axisy=y axisz=-z", e.group);
		}
		else
		{
			sprintf (def, "axisx=x axisy=y axisz=-z");
		}
	}
	else
	{
		if (e.group)
		{
			sprintf (def, "group='%s'", e.group);
		}
		else
		{
			def[0] = '\0';
		}
	}
	int ret = TwAddVarCB ((TwBar*)_bar, e.name, twType, (readonly ? 0 : &setVariableCallback), &getVariableCallback, variable, def);
	if (ret)
	{
		_variables[e.name] = variable;
		return true;
	}
	else
	{
		ATOM_DELETE(variable);
		return false;
	}
}

bool ATOMX_TweakBar::addIntVar (const char *name, int id, int value, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.setI (ATOMX_TBTYPE_INT32, value);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addUIntVar (const char *name, int id, unsigned int value, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.setI (ATOMX_TBTYPE_UINT32, value);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addShortVar (const char *name, int id, short value, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.setI (ATOMX_TBTYPE_INT16, value);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addUShortVar (const char *name, int id, unsigned short value, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.setI (ATOMX_TBTYPE_UINT16, value);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addByteVar (const char *name, int id, char value, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.setI (ATOMX_TBTYPE_INT8, value);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addUByteVar (const char *name, int id, unsigned char value, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.setI (ATOMX_TBTYPE_UINT8, value);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addCharVar (const char *name, int id, char value, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.setI (ATOMX_TBTYPE_CHAR, value);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addBoolVar (const char *name, int id, bool value, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.setB (ATOMX_TBTYPE_BOOL, value);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addFloatVar (const char *name, int id, float value, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.setF (ATOMX_TBTYPE_FLOAT, value);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addStringVar (const char *name, int id, const char *value, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.setS (ATOMX_TBTYPE_STRING, value);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addVector2fVar (const char *name, int id, float x, float y, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.set2F (ATOMX_TBTYPE_VECTOR2F, x, y);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addVector2fVar (const char *name, int id, const ATOM_Vector2f &v, bool readonly, const char *group, void *userData)
{
	return addVector2fVar (name, id, v.x, v.y, readonly, group, userData);
}

bool ATOMX_TweakBar::addVector3fVar (const char *name, int id, float x, float y, float z, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.set3F (ATOMX_TBTYPE_VECTOR3F, x, y, z);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addVector3fVar (const char *name, int id, const ATOM_Vector3f &v, bool readonly, const char *group, void *userData)
{
	return addVector3fVar (name, id, v.x, v.y, v.z, readonly, group, userData);
}

bool ATOMX_TweakBar::addVector4fVar (const char *name, int id, float x, float y, float z, float w, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.set4F (ATOMX_TBTYPE_VECTOR4F, x, y, z, w);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addVector4fVar (const char *name, int id, const ATOM_Vector4f &v, bool readonly, const char *group, void *userData)
{
	return addVector4fVar (name, id, v.x, v.y, v.z, v.w, readonly, group, userData);
}

bool ATOMX_TweakBar::addRGBVar (const char *name, int id, float r, float g, float b, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.set3F (ATOMX_TBTYPE_COLOR3F, r, g, b);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addRGBAVar (const char *name, int id, float r, float g, float b, float a, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.set4F (ATOMX_TBTYPE_COLOR4F, r, g, b, a);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addQuatVar (const char *name, int id, float x, float y, float z, float w, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.set4F (ATOMX_TBTYPE_QUAT4F, x, y, z, w);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addDirVar (const char *name, int id, float x, float y, float z, bool readonly, const char *group, void *userData)
{
	ATOMX_TBValue val;
	val.set3F (ATOMX_TBTYPE_DIR3F, x, y, z);
	return addVariable (name, id, val, readonly, group, userData);
}

bool ATOMX_TweakBar::addEnum (const char *name, int id, int value, const ATOMX_TBEnum &var, bool readonly, const char *group, void *userData)
{
	if (!name || !_bar || !_initialized)
	{
		return false;
	}

	ATOMX_TWAddVariableEvent e(name, id, group, userData, this, ATOMX_TWAddVariableEvent::VT_VAR, true);
	ATOM_APP->handleEvent (&e);
	if (!e.allow)
	{
		return true;
	}

	ATOM_MAP<ATOM_STRING, ATOMX_TweakBar::VariableBase*>::iterator it = _variables.find (e.name);
	if (it != _variables.end ())
	{
		return false;
	}

	Variable *variable = ATOM_NEW(Variable, this);
	variable->name = e.name;
	variable->id = e.id;
	variable->value.setI (ATOMX_TBTYPE_INT32, value);
	variable->userdata = e.userdata;

	char def[1024];
	if (e.group)
	{
		sprintf (def, "group='%s'", e.group);
	}
	else
	{
		def[0] = '\0';
	}

	TwEnumVal *enumVals = ATOM_NEW_ARRAY(TwEnumVal, var.getNumEnums ());
	for (unsigned i = 0; i < var.getNumEnums (); ++i)
	{
		enumVals[i].Label = var.getEnumName (i);
		enumVals[i].Value = var.getEnumValue (i);
	}
	TwType type = TwDefineEnum (e.name, enumVals, var.getNumEnums ());
	int ret = TwAddVarCB ((TwBar*)_bar, e.name, type, (readonly ? 0 : &setVariableCallback), &getVariableCallback, variable, def);
	if (ret)
	{
		_variables[e.name] = variable;
		return true;
	}
	else
	{
		ATOM_DELETE(variable);
		return false;
	}
}

bool ATOMX_TweakBar::addEnum (const char *name, int id, int value, bool readonly, const char *group, void *userData, ...)
{
	va_list arg_ptr;

	va_start (arg_ptr, userData);

	ATOMX_TBEnum enumValue;
	for (;;)
	{
		const char *name = va_arg (arg_ptr, const char*);

		if (!name)
		{
			break;
		}

		int value = va_arg (arg_ptr, int);

		enumValue.addEnum (name, value);
	}

	va_end (arg_ptr);

	return addEnum (name, id, value, enumValue, readonly, group, userData);
}

void TW_CALL ButtonCallback2 (void *clientData)
{
	ATOMX_TweakBar::ButtonCallbackInfo *callbackInfo = (ATOMX_TweakBar::ButtonCallbackInfo*)clientData;
	if (callbackInfo->callback)
	{
		callbackInfo->callback (callbackInfo->userdata);
	}
}

bool ATOMX_TweakBar::addButton (const char *name, const char *label, const char *group, PFNButtonCallback callback, void *userdata)
{
	if (!name || !_bar || !_initialized)
	{
		return false;
	}

	ATOMX_TWAddVariableEvent e(name, -1, group, userdata, this, ATOMX_TWAddVariableEvent::VT_BUTTON, true);
	ATOM_APP->handleEvent (&e);
	if (!e.allow)
	{
		return true;
	}

	_buttonCallbacks.push_back (ATOM_NEW(ButtonCallbackInfo));
	_buttonCallbacks.back()->callback = callback;
	_buttonCallbacks.back()->userdata = userdata;

	char def[1024];
	if (e.group)
	{
		if (!label || !label[0])
		{
			sprintf (def, "group='%s'", e.group);
		}
		else
		{
			sprintf (def, "group='%s' label='%s'", e.group, label);
		}
	}
	else
	{
		if (!label || !label[0])
		{
			def[0] = '\0';
		}
		else
		{
			sprintf (def, "label='%s'", label);
		}
	}
	int ret = TwAddButton ((TwBar*)_bar, e.name, &ButtonCallback2, _buttonCallbacks.back(), def);
	if (!ret)
	{
		ATOM_DELETE(_buttonCallbacks.back());
		_buttonCallbacks.pop_back ();
		return false;
	}

	return true;
}

bool ATOMX_TweakBar::addButton (const char *name, int id, const char *label, const char *group, void *userData)
{
	if (!name || !_bar || !_initialized)
	{
		return false;
	}

	ATOMX_TWAddVariableEvent e(name, id, group, userData, this, ATOMX_TWAddVariableEvent::VT_BUTTON, true);
	ATOM_APP->handleEvent (&e);
	if (!e.allow)
	{
		return true;
	}

	ATOM_MAP<ATOM_STRING, VariableBase*>::iterator it = _variables.find (e.name);
	if (it != _variables.end ())
	{
		return false;
	}

	VariableBase *vb = ATOM_NEW(VariableBase, this);
	vb->name = e.name;
	vb->id = e.id;
	vb->userdata = e.userdata;

	char def[1024];
	if (e.group)
	{
		if (!label || !label[0])
		{
			sprintf (def, "group='%s'", e.group);
		}
		else
		{
			sprintf (def, "group='%s' label='%s'", e.group, label);
		}
	}
	else
	{
		if (!label || !label[0])
		{
			def[0] = '\0';
		}
		else
		{
			sprintf (def, "label='%s'", label);
		}
	}
	int ret = TwAddButton ((TwBar*)_bar, e.name, &ButtonCallback, vb, def);
	if (ret)
	{
		_variables[e.name] = vb;
		return true;
	}
	else
	{
		ATOM_DELETE(vb);
		return false;
	}
}

bool ATOMX_TweakBar::addText (const char *name, const char *label, const char *group)
{
	if (!name || !_bar || !_initialized)
	{
		return false;
	}

	ATOMX_TWAddVariableEvent e(name, -1, group, 0, this, ATOMX_TWAddVariableEvent::VT_TEXT, true);
	ATOM_APP->handleEvent (&e);
	if (!e.allow)
	{
		return true;
	}

	char def[1024];
	if (e.group)
	{
		sprintf (def, "group='%s'", e.group);
	}
	else
	{
		def[0] = '\0';
	}
	if (TwAddButton ((TwBar*)_bar, e.name, 0, 0, def))
	{
		if (!label)
		{
			label = "";
		}
		TwSetParam ((TwBar*)_bar, e.name, "label", TW_PARAM_CSTRING, 1, label);
		return true;
	}

	return false;
}

bool ATOMX_TweakBar::addSeparator (const char *name, const char *group)
{
	if (!name || !_bar || !_initialized)
	{
		return false;
	}

	ATOMX_TWAddVariableEvent e(name, -1, group, 0, this, ATOMX_TWAddVariableEvent::VT_SEPARATOR, true);
	ATOM_APP->handleEvent (&e);
	if (!e.allow)
	{
		return true;
	}

	char def[1024];
	if (e.group)
	{
		sprintf (def, "group='%s'", e.group);
	}
	else
	{
		def[0] = '\0';
	}

	if (TwAddSeparator ((TwBar*)_bar, e.name, def))
	{
		return true;
	}

	return false;
}

void ATOMX_TweakBar::remove (const char *name)
{
	if (!name)
	{
		return;
	}

	ATOM_MAP<ATOM_STRING, VariableBase*>::iterator it = _variables.find (name);
	if (it != _variables.end ())
	{
		if (_bar && _initialized)
		{
			TwRemoveVar ((TwBar*)_bar, name);
		}

		ATOM_DELETE(it->second);
		_variables.erase (it);
	}
}

void ATOMX_TweakBar::clear (void)
{
	if (_bar && _initialized)
	{
		TwRemoveAllVars ((TwBar*)_bar);

		for (ATOM_MAP<ATOM_STRING, ATOMX_TweakBar::VariableBase*>::iterator it = _variables.begin(); it != _variables.end(); ++it)
		{
			ATOM_DELETE(it->second);
		}
		_variables.clear ();

		for (unsigned i = 0; i < _buttonCallbacks.size(); ++i)
		{
			ATOM_DELETE(_buttonCallbacks[i]);
		}
		_buttonCallbacks.clear ();
	}
}

void ATOMX_TweakBar::setVarUserData (const char *name, void *userdata)
{
	ATOM_MAP<ATOM_STRING, VariableBase*>::iterator it = _variables.find (name);
	if (it != _variables.end ())
	{
		it->second->userdata = userdata;
	}
}

void ATOMX_TweakBar::setVarLabel (const char *name, const char *label)
{
	if (_initialized && _bar)
	{
		TwSetParam ((TwBar*)_bar, name, "label", TW_PARAM_CSTRING, 1, label);
	}
}

void ATOMX_TweakBar::setVarHelp (const char *name, const char *msg)
{
	if (_initialized && _bar)
	{
		TwSetParam ((TwBar*)_bar, name, "help", TW_PARAM_CSTRING, 1, msg);
	}
}

void ATOMX_TweakBar::setVarGroup (const char *name, const char *group)
{
	if (_initialized && _bar)
	{
		TwSetParam ((TwBar*)_bar, name, "group", TW_PARAM_CSTRING, 1, group);
	}
}

void ATOMX_TweakBar::setVarVisible (const char *name, bool visible)
{
	if (_initialized && _bar)
	{
		TwSetParam ((TwBar*)_bar, name, "visible", TW_PARAM_CSTRING, 1, visible ? "true" : "false");
	}
}

void ATOMX_TweakBar::setVarMinMax (const char *name, float minvalue, float maxvalue)
{
	if (_initialized && _bar)
	{
		TwSetParam ((TwBar*)_bar, name, "min", TW_PARAM_FLOAT, 1, &minvalue);
		TwSetParam ((TwBar*)_bar, name, "max", TW_PARAM_FLOAT, 1, &maxvalue);
	}
}

void ATOMX_TweakBar::setVarStep (const char *name, float step)
{
	if (_initialized && _bar)
	{
		TwSetParam ((TwBar*)_bar, name, "step", TW_PARAM_FLOAT, 1, &step);
	}
}

void ATOMX_TweakBar::setVarPrecision (const char *name, int precision)
{
	if (_initialized && _bar)
	{
		TwSetParam ((TwBar*)_bar, name, "precision", TW_PARAM_INT32, 1, &precision);
	}
}

void ATOMX_TweakBar::setVarHex (const char *name, bool hex)
{
	if (_initialized && _bar)
	{
		TwSetParam ((TwBar*)_bar, name, "hexa", TW_PARAM_CSTRING, 1, hex ? "true" : "false");
	}
}

void ATOMX_TweakBar::setVarBoolString (const char *name, const char *trueStr, const char *falseStr)
{
	if (_initialized && _bar)
	{
		TwSetParam ((TwBar*)_bar, name, "true", TW_PARAM_CSTRING, 1, trueStr);
		TwSetParam ((TwBar*)_bar, name, "false", TW_PARAM_CSTRING, 1, falseStr);
	}
}

void ATOMX_TweakBar::setGroupOpenState (const char *name, bool opened)
{
	if (_initialized && _bar)
	{
		TwSetParam ((TwBar*)_bar, name, "opened", TW_PARAM_CSTRING, 1, opened ? "true" : "false");
	}
}

void TW_CALL SetCursorCallback (HCURSOR hCursor)
{
	ATOM_GetRenderDevice()->setCustomCursor (0, hCursor);
}

bool ATOMX_TweakBar::initialize (void *device, int width, int height)
{
	if (_initialized)
	{
		return true;
	}

	if (!ATOMX_LoadTweakBarLib ())
	{
		return false;
	}

	TwGraphCreateFunc (&ATOMGraphCreate, device);

	if (TwInit (TW_DIRECT3D9, device))
	{
		if (TwWindowSize (width, height))
		{
			TwCursorAccessFunc (0, &SetCursorCallback, 0);
			TwCopyCDStringToClientFunc (&CopyCDStringToClient);

			_initialized = true;
			return true;
		}
		else
		{
			TwTerminate ();
		}
	}
	return false;
}

void ATOMX_TweakBar::terminate (void)
{
	if (_initialized)
	{
		_initialized = false;

		ATOMX_UnloadTweakBarLib ();
	}
}

void ATOMX_TweakBar::handleDeviceLost (void)
{
	if (_initialized)
	{
		TwWindowSize (0, 0);
	}
}

void ATOMX_TweakBar::setBarVisible (const char *barName, bool visible)
{
	if (barName)
	{
		TwBar *bar = TwGetBarByName (barName);

		if (bar)
		{
			TwSetParam (bar, 0, "visible", TW_PARAM_CSTRING, 1, visible ? "true" : "false");
		}
	}
}

void ATOMX_TweakBar::handleDeviceReset (int width, int height)
{
	if (_initialized)
	{
		TwWindowSize (width, height);
	}
}

void ATOMX_TweakBar::draw (void)
{
	if (_initialized && TwGetBarCount() > 0)
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice();

		if (device)
		{
			TwDraw ();

			//device->syncRenderStates();
		}
	}
}

bool ATOMX_TweakBar::processWMEvent (ATOM_AppWMEvent *event)
{
	if (_initialized && TwEventWin (event->hWnd, event->msg, event->wParam, event->lParam))
	{
		event->eat = true;
		event->returnValue = 0;
		return true;
	}
	return false;
}

void ATOMX_TweakBar::setBarColor (float r, float g, float b, float a)
{
	if (_bar && _initialized)
	{
		char paramStr[256];

		int red = ATOM_ftol(r * 255);
		int green = ATOM_ftol(g * 255);
		int blue = ATOM_ftol(b * 255);
		sprintf (paramStr, "'%s' color='%d %d %d'", _barName.c_str(), red, green, blue);
		TwDefine (paramStr);

		int alpha = ATOM_ftol(a * 255);
		sprintf (paramStr, "'%s' alpha=%d", _barName.c_str(), alpha);
		TwDefine (paramStr);
	}
}

void ATOMX_TweakBar::setBarPosition (int x, int y)
{
	if (_bar && _initialized)
	{
		char paramStr[256];
		sprintf (paramStr, "'%s' position='%d %d'", _barName.c_str(), x, y);
		TwDefine (paramStr);
	}
}

void ATOMX_TweakBar::setBarSize (int w, int h)
{
	if (_bar && _initialized)
	{
		char paramStr[256];
		sprintf (paramStr, "'%s' size='%d %d'", _barName.c_str(), w, h);
		TwDefine (paramStr);
	}
}

void ATOMX_TweakBar::setBarLabel (const char *label)
{
	if (_bar && _initialized)
	{
		TwSetParam ((TwBar*)_bar, 0, "label", TW_PARAM_CSTRING, 1, label ? label : "");
	}
}

void ATOMX_TweakBar::setBarVisible (bool visible)
{
	if (_bar && _initialized)
	{
		TwSetParam ((TwBar*)_bar, 0, "visible", TW_PARAM_CSTRING, 1, visible ? "true" : "false");
	}
}

void ATOMX_TweakBar::setBarIconifiable (bool b)
{
	if (_bar && _initialized)
	{
		TwSetParam ((TwBar*)_bar, 0, "iconifiable", TW_PARAM_CSTRING, 1, b ? "true" : "false");
	}
}

void ATOMX_TweakBar::setBarMovable (bool b)
{
	if (_bar && _initialized)
	{
		TwSetParam ((TwBar*)_bar, 0, "movable", TW_PARAM_CSTRING, 1, b ? "true" : "false");
	}
}

void ATOMX_TweakBar::setBarResizable (bool b)
{
	if (_bar && _initialized)
	{
		TwSetParam ((TwBar*)_bar, 0, "resizable", TW_PARAM_CSTRING, 1, b ? "true" : "false");
	}
}

void ATOMX_TweakBar::setBarTopMost (bool b)
{
	if (_bar && _initialized)
	{
		TwSetParam ((TwBar*)_bar, 0, "alwaystop", TW_PARAM_CSTRING, 1, b ? "true" : "false");
	}
}

void ATOMX_TweakBar::setBarBottomMost (bool b)
{
	if (_bar && _initialized)
	{
		TwSetParam ((TwBar*)_bar, 0, "alwaysbottom", TW_PARAM_CSTRING, 1, b ? "true" : "false");
	}
}

void ATOMX_TweakBar::setBarFontResizable (bool b)
{
	if (_bar && _initialized)
	{
		TwSetParam ((TwBar*)_bar, 0, "fontresizable", TW_PARAM_CSTRING, 1, b ? "true" : "false");
	}
}

void ATOMX_TweakBar::setBarRefreshPeriod (float sec)
{
	if (_bar && _initialized)
	{
		TwSetParam ((TwBar*)_bar, 0, "refresh", TW_PARAM_FLOAT, 1, &sec);
	}
}

void ATOMX_TweakBar::setTop (void)
{
	if (_bar && _initialized)
	{
		TwSetTopBar ((TwBar*)_bar);
	}
}

void ATOMX_TweakBar::setBottom (void)
{
	if (_bar && _initialized)
	{
		TwSetBottomBar ((TwBar*)_bar);
	}
}

bool ATOMX_TweakBar::isTop (void) const
{
	if (_bar && _initialized)
	{
		return TwGetTopBar () == (TwBar*)_bar;
	}
	return false;
}

bool ATOMX_TweakBar::isBottom (void) const
{
	if (_bar && _initialized)
	{
		return TwGetBottomBar () == (TwBar*)_bar;
	}
	return false;
}

const char *ATOMX_TweakBar::getName (void) const
{
	return _barName.c_str();
}

void ATOMX_TweakBar::refresh (void)
{
	if (_bar && _initialized)
	{
		TwRefreshBar ((TwBar*)_bar);
	}
}

void ATOMX_TweakBar::setI (const char *name, int value, bool post)
{
	ATOM_MAP<ATOM_STRING, VariableBase*>::iterator it = _variables.find (name);
	if (it != _variables.end ())
	{
		switch (it->second->type)
		{
		case VT_VAR:
			{
				Variable *var = (Variable*)it->second;
				if (var->value.getI() != value)
				{
					var->value.setI (var->value.getType(), value);

					if (post)
					{
						ATOMX_TWValueChangedEvent e;
						e.id = var->id;
						e.bar = this;
						e.userdata = var->userdata;
						e.oldValue = var->value;
						e.newValue.setI (var->value.getType(), value);
						handleEvent (&e);
					}
				}
				break;
			}
		};
	}
}

void ATOMX_TweakBar::setU (const char *name, unsigned value, bool post)
{
	ATOM_MAP<ATOM_STRING, VariableBase*>::iterator it = _variables.find (name);
	if (it != _variables.end ())
	{
		switch (it->second->type)
		{
		case VT_VAR:
			{
				Variable *var = (Variable*)it->second;
				if (var->value.getU() != value)
				{
					var->value.setU (var->value.getType(), value);

					if (post)
					{
						ATOMX_TWValueChangedEvent e;
						e.id = var->id;
						e.bar = this;
						e.userdata = var->userdata;
						e.oldValue = var->value;
						e.newValue.setU (var->value.getType(), value);
						handleEvent (&e);
					}
				}
				var->value.setU (var->value.getType(), value);
				break;
			}
		};
	}
}

void ATOMX_TweakBar::setB (const char *name, bool value, bool post)
{
	ATOM_MAP<ATOM_STRING, VariableBase*>::iterator it = _variables.find (name);
	if (it != _variables.end ())
	{
		switch (it->second->type)
		{
		case VT_VAR:
			{
				Variable *var = (Variable*)it->second;
				if (var->value.getB() != value)
				{
					var->value.setB(var->value.getType(), value);

					if (post)
					{
						ATOMX_TWValueChangedEvent e;
						e.id = var->id;
						e.bar = this;
						e.userdata = var->userdata;
						e.oldValue = var->value;
						e.newValue.setB (var->value.getType(), value);
						handleEvent (&e);
					}
				}
				break;
			}
		};
	}
}

void ATOMX_TweakBar::setF (const char *name, float value, bool post)
{
	ATOM_MAP<ATOM_STRING, VariableBase*>::iterator it = _variables.find (name);
	if (it != _variables.end ())
	{
		switch (it->second->type)
		{
		case VT_VAR:
			{
				Variable *var = (Variable*)it->second;
				if (var->value.getF() != value)
				{
					var->value.setF(var->value.getType(), value);

					if (post)
					{
						ATOMX_TWValueChangedEvent e;
						e.id = var->id;
						e.bar = this;
						e.userdata = var->userdata;
						e.oldValue = var->value;
						e.newValue.setF (var->value.getType(), value);
						handleEvent (&e);
					}
				}
				break;
			}
		};
	}
}

void ATOMX_TweakBar::setS (const char *name, const char *value, bool post)
{
	ATOM_MAP<ATOM_STRING, VariableBase*>::iterator it = _variables.find (name);
	if (it != _variables.end ())
	{
		switch (it->second->type)
		{
		case VT_VAR:
			{
				Variable *var = (Variable*)it->second;
				if (strcmp(var->value.getS(), value))
				{
					var->value.setS(var->value.getType(), value);

					if (post)
					{
						ATOMX_TWValueChangedEvent e;
						e.id = var->id;
						e.bar = this;
						e.userdata = var->userdata;
						e.oldValue = var->value;
						e.newValue.setS (var->value.getType(), value);
						handleEvent (&e);
					}
				}
				break;
			}
		};
	}
}

void ATOMX_TweakBar::set2F (const char *name, const ATOM_Vector2f &value, bool post)
{
	ATOM_MAP<ATOM_STRING, VariableBase*>::iterator it = _variables.find (name);
	if (it != _variables.end ())
	{
		switch (it->second->type)
		{
		case VT_VAR:
			{
				Variable *var = (Variable*)it->second;
				const float *f = var->value.get2F();
				if (value.x != f[0] || value.y != f[1])
				{
					var->value.set2F(var->value.getType(), value.x, value.y);

					if (post)
					{
						ATOMX_TWValueChangedEvent e;
						e.id = var->id;
						e.bar = this;
						e.userdata = var->userdata;
						e.oldValue = var->value;
						e.newValue.set2F (var->value.getType(), value.x, value.y);
						handleEvent (&e);
					}
				}
				break;
			}
		};
	}
}

void ATOMX_TweakBar::set3F (const char *name, const ATOM_Vector3f &value, bool post)
{
	ATOM_MAP<ATOM_STRING, VariableBase*>::iterator it = _variables.find (name);
	if (it != _variables.end ())
	{
		switch (it->second->type)
		{
		case VT_VAR:
			{
				Variable *var = (Variable*)it->second;
				const float *f = var->value.get3F();
				if (value.x != f[0] || value.y != f[1] || value.z != f[2])
				{
					var->value.set3F(var->value.getType(), value.x, value.y, value.z);

					if (post)
					{
						ATOMX_TWValueChangedEvent e;
						e.id = var->id;
						e.bar = this;
						e.userdata = var->userdata;
						e.oldValue = var->value;
						e.newValue.set3F (var->value.getType(), value.x, value.y, value.z);
						handleEvent (&e);
					}
				}
				break;
			}
		};
	}
}

void ATOMX_TweakBar::set4F (const char *name, const ATOM_Vector4f &value, bool post)
{
	ATOM_MAP<ATOM_STRING, VariableBase*>::iterator it = _variables.find (name);
	if (it != _variables.end ())
	{
		switch (it->second->type)
		{
		case VT_VAR:
			{
				Variable *var = (Variable*)it->second;
				const float *f = var->value.get4F();
				if (value.x != f[0] || value.y != f[1] || value.z != f[2] || value.w != f[3])
				{
					var->value.set4F(var->value.getType(), value.x, value.y, value.z, value.w);

					if (post)
					{
						ATOMX_TWValueChangedEvent e;
						e.id = var->id;
						e.bar = this;
						e.userdata = var->userdata;
						e.oldValue = var->value;
						e.newValue.set4F (var->value.getType(), value.x, value.y, value.z, value.w);
						handleEvent (&e);
					}
				}
				break;
			}
		};
	}
}

int  ATOMX_TweakBar::getI (const char *name) const
{
	if (name)
	{
		ATOM_MAP<ATOM_STRING, VariableBase*>::const_iterator it = _variables.find (name);
		if (it != _variables.end ())
		{
			switch (it->second->type)
			{
			case VT_VAR:
				{
					Variable *var = (Variable*)it->second;
					return var->value.getI ();
					break;
				}
			};
		}
	}

	return 0;
}

unsigned ATOMX_TweakBar::getU (const char *name) const
{
	if (name)
	{
		ATOM_MAP<ATOM_STRING, VariableBase*>::const_iterator it = _variables.find (name);
		if (it != _variables.end ())
		{
			switch (it->second->type)
			{
			case VT_VAR:
				{
					Variable *var = (Variable*)it->second;
					return var->value.getU ();
					break;
				}
			};
		}
	}
	return 0;
}

bool ATOMX_TweakBar::getB (const char *name) const
{
	if (name)
	{
		ATOM_MAP<ATOM_STRING, VariableBase*>::const_iterator it = _variables.find (name);
		if (it != _variables.end ())
		{
			switch (it->second->type)
			{
			case VT_VAR:
				{
					Variable *var = (Variable*)it->second;
					return var->value.getB ();
					break;
				}
			};
		}
	}
	return false;
}

float ATOMX_TweakBar::getF (const char *name) const
{
	if (name)
	{
		ATOM_MAP<ATOM_STRING, VariableBase*>::const_iterator it = _variables.find (name);
		if (it != _variables.end ())
		{
			switch (it->second->type)
			{
			case VT_VAR:
				{
					Variable *var = (Variable*)it->second;
					return var->value.getF ();
					break;
				}
			};
		}
	}
	return 0.f;
}

const char *ATOMX_TweakBar::getS (const char *name) const
{
	if (name)
	{
		ATOM_MAP<ATOM_STRING, VariableBase*>::const_iterator it = _variables.find (name);
		if (it != _variables.end ())
		{
			switch (it->second->type)
			{
			case VT_VAR:
				{
					Variable *var = (Variable*)it->second;
					return var->value.getS ();
					break;
				}
			};
		}
	}
	return 0;
}

ATOM_Vector3f ATOMX_TweakBar::get3F (const char *name) const
{
	if (name)
	{
		ATOM_MAP<ATOM_STRING, VariableBase*>::const_iterator it = _variables.find (name);
		if (it != _variables.end ())
		{
			switch (it->second->type)
			{
			case VT_VAR:
				{
					Variable *var = (Variable*)it->second;
					const float *f = var->value.get3F ();
					return f ? ATOM_Vector3f(f[0], f[1], f[2]) : ATOM_Vector3f(0.f, 0.f, 0.f);
					break;
				}
			};
		}
	}
	return ATOM_Vector3f(0.f, 0.f, 0.f);
}

ATOM_Vector4f ATOMX_TweakBar::get4F (const char *name) const
{
	if (name)
	{
		ATOM_MAP<ATOM_STRING, VariableBase*>::const_iterator it = _variables.find (name);
		if (it != _variables.end ())
		{
			switch (it->second->type)
			{
			case VT_VAR:
				{
					Variable *var = (Variable*)it->second;
					const float *f = var->value.get4F ();
					return f ? ATOM_Vector4f(f[0], f[1], f[2], f[3]) : ATOM_Vector4f(0.f, 0.f, 0.f, 0.f);
					break;
				}
			};
		}
	}
	return ATOM_Vector4f(0.f, 0.f, 0.f, 0.f);
}

void ATOMX_TweakBar::onValueChanged (ATOMX_TWValueChangedEvent *event)
{
	if (_enableNotifying && ATOM_APP)
	{
		ATOM_APP->queueEvent (ATOM_NEW(ATOMX_TWValueChangedEvent, *event), ATOM_APP);
	}

	if (_valueChangedCallback)
	{
		_valueChangedCallback->callback (event);
	}
}

void ATOMX_TweakBar::onCommand (ATOMX_TWCommandEvent *event)
{
	if (_enableNotifying && ATOM_APP)
	{
		ATOM_APP->queueEvent (ATOM_NEW(ATOMX_TWCommandEvent, *event), ATOM_APP);
	}

	if (_commandCallback)
	{
		_commandCallback->callback (event);
	}
}

void ATOMX_TweakBar::setValueChangedCallback (ATOMX_TweakBar::ValueChangedCallback *callback)
{
	_valueChangedCallback = callback;
}

ATOMX_TweakBar::ValueChangedCallback *ATOMX_TweakBar::getValueChangedCallback (void) const
{
	return _valueChangedCallback;
}

void ATOMX_TweakBar::setCommandCallback (ATOMX_TweakBar::CommandCallback *callback)
{
	_commandCallback = callback;
}

ATOMX_TweakBar::CommandCallback *ATOMX_TweakBar::getCommandCallback (void) const
{
	return _commandCallback;
}

void ATOMX_TweakBar::setVarReadFromBarFunc (const char *name, PFNReadFromBarFunc func, void *userData)
{
	if (name)
	{
		ATOM_MAP<ATOM_STRING, VariableBase*>::const_iterator it = _variables.find (name);
		if (it != _variables.end ())
		{
			switch (it->second->type)
			{
			case VT_VAR:
				{
					Variable *var = (Variable*)it->second;
					var->readFromFunc = func;
					var->userDataReadFrom = userData;
					break;
				}
			};
		}
	}
}

void ATOMX_TweakBar::setVarWriteToBarFunc (const char *name, PFNWriteToBarFunc func, void *userData)
{
	if (name)
	{
		ATOM_MAP<ATOM_STRING, VariableBase*>::const_iterator it = _variables.find (name);
		if (it != _variables.end ())
		{
			switch (it->second->type)
			{
			case VT_VAR:
				{
					Variable *var = (Variable*)it->second;
					var->writeToFunc = func;
					var->userDataWriteTo = userData;
					break;
				}
			};
		}
	}
}

