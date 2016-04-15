#include "StdAfx.h"
#include "userattrib.h"

bool ATOM_UserAttributes::setAttribute (const char *name, const ATOM_Variant &value)
{
	if (!name)
	{
		return false;
	}

	switch (value.getType())
	{
	case ATOM_Variant::NONE:
		{
			// remove the attribute
			ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::iterator it = _attributes.find (name);
			if (it == _attributes.end ())
			{
				return false;
			}
			_attributes.erase (it);
			break;
		}
	case ATOM_Variant::INT:
	case ATOM_Variant::FLOAT:
	case ATOM_Variant::VECTOR4:
	case ATOM_Variant::STRING:
	case ATOM_Variant::MATRIX44:
		{
			_attributes[name] = value;
			break;
		}
	default:
		return false;
	}

	return true;
}

const ATOM_Variant &ATOM_UserAttributes::getAttribute (const char *name) const
{
	static const ATOM_Variant emptyValue;

	if (name)
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::const_iterator it = _attributes.find (name);
		if (it != _attributes.end ())
		{
			return it->second;
		}
	}

	return emptyValue;
}

void ATOM_UserAttributes::clearAttributes (void)
{
	_attributes.clear ();
}

unsigned ATOM_UserAttributes::getNumAttributes (void) const
{
	return _attributes.size();
}

const ATOM_Variant &ATOM_UserAttributes::getAttributeValue (int index) const
{
	static const ATOM_Variant nullVar;
	if (index >= 0 && index < _attributes.size())
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::const_iterator it = _attributes.begin();
		std::advance (it, index);
		return it->second;
	}
	return nullVar;
}

const char *ATOM_UserAttributes::getAttributeName (int index) const
{
	if (index >= 0 && index < _attributes.size())
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::const_iterator it = _attributes.begin();
		std::advance (it, index);
		return it->first.c_str();
	}
	return 0;
}

