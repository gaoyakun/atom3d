#include "StdAfx.h"
#include "property.h"



void ATOM_PropertySet::clear (void)
{
	_props.clear ();
}

unsigned ATOM_PropertySet::getNumProperties (void) const
{
	return _props.size ();
}

const char *ATOM_PropertySet::getValue (const char *key) const
{
	ATOM_HASHMAP<ATOM_STRING, ATOM_STRING>::const_iterator it = _props.find (key);
	return it != _props.end () ? it->second.c_str() : 0;
}

void ATOM_PropertySet::setValue (const char *key, const char *value)
{
	_props[key] = value;
}

const ATOM_HASHMAP<ATOM_STRING, ATOM_STRING> &ATOM_PropertySet::getProperties (void) const
{
	return _props;
}

ATOM_HASHMAP<ATOM_STRING, ATOM_STRING> &ATOM_PropertySet::getProperties (void)
{
	return _props;
}

void ATOM_PropertySetCache::setNumPropertySets (unsigned size)
{
	_propSets.resize (size);
}

unsigned ATOM_PropertySetCache::getNumPropertySets (void) const
{
	return _propSets.size ();
}

const ATOM_PropertySet &ATOM_PropertySetCache::getPropertySet (unsigned index) const
{
	return _propSets[index];
}

ATOM_PropertySet &ATOM_PropertySetCache::getPropertySet (unsigned index)
{
	return _propSets[index];
}


