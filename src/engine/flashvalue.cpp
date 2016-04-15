#include "stdafx.h"
#include "flashvalue.h"

template <class T> 
struct IsBool
{
	enum {
		result = false
	};
};

template <>
struct IsBool <bool>
{
	enum {
		result = true
	};
};

template <>
struct IsBool <const bool>
{
	enum {
		result = true
	};
};

template<class NumberType>
inline NumberType toNumber(const ATOM_STRING &numberString)
{
	if(numberString.substr(0, 4).compare("true") == 0) return 1;
	else if(numberString.substr(0, 4).compare("false") == 0) return 0;

	std::istringstream converter(numberString.c_str());
	
	if(IsBool<NumberType>::result)
	{
		int result;
		return (converter >> result).fail() ? false : !!result;
	}

	NumberType result;
	return (converter >> result).fail() ? 0 : result;
}

template<class NumberType>
inline ATOM_STRING numberToString(const NumberType &number)
{
	std::ostringstream converter;

	if(IsBool<NumberType>::result)
	{
		return number ? "true" : "false";
	}

	return (converter << number).fail() ? "" : converter.str().c_str();
}

ATOM_FlashValue::ATOM_FlashValue() : numValue(0), boolValue(0), valueType(FT_NULL)
{
}

ATOM_FlashValue::ATOM_FlashValue(bool booleanValue) : numValue(0), boolValue(booleanValue), valueType(FT_BOOLEAN)
{
}

ATOM_FlashValue::ATOM_FlashValue(int numericValue) : numValue(numericValue), boolValue(0), valueType(FT_NUMBER)
{
}

ATOM_FlashValue::ATOM_FlashValue(float numericValue) : numValue(numericValue), boolValue(0), valueType(FT_NUMBER)
{
}

ATOM_FlashValue::ATOM_FlashValue(const char* stringValue) : strValue(stringValue), numValue(0), boolValue(0), valueType(FT_STRING)
{
}

short ATOM_FlashValue::getType() const
{
	return valueType;
}

bool ATOM_FlashValue::isNull() const
{
	return valueType == FT_NULL;
}

void ATOM_FlashValue::setNull()
{
	strValue.clear();
	numValue = 0;
	boolValue = false;
	valueType = FT_NULL;
}

bool ATOM_FlashValue::getBool() const
{
	if(valueType == FT_BOOLEAN)
		return boolValue;
	else if(valueType == FT_NUMBER)
		return !!((int)numValue);
	else if(valueType == FT_STRING)
		return toNumber<bool>(strValue);

	return false;
}

float ATOM_FlashValue::getNumber() const
{
	if(valueType == FT_NUMBER)
		return numValue;
	else if(valueType == FT_BOOLEAN)
		return (float)boolValue;
	else if(valueType == FT_STRING)
		return toNumber<float>(strValue);

	return 0;
}

ATOM_ColorARGB ATOM_FlashValue::getNumberAsColor() const
{
	if(valueType != FT_NUMBER)
		return 0;

	return (int)numValue;
}

ATOM_STRING ATOM_FlashValue::getString() const
{
	if(valueType == FT_STRING)
		return strValue;
	else if(valueType == FT_BOOLEAN)
		return numberToString<bool>(boolValue);
	else if(valueType == FT_NUMBER)
		return numberToString<float>(numValue);

	return "";
}

ATOM_FlashArgs::ATOM_FlashArgs()
{
}

ATOM_FlashArgs::ATOM_FlashArgs(const ATOM_FlashValue& firstArg) : ATOM_FlashArguments(1, firstArg)
{
}

ATOM_FlashArgs& ATOM_FlashArgs::operator()(const ATOM_FlashValue& newArg) 
{
	this->push_back(newArg);
	return *this;
}