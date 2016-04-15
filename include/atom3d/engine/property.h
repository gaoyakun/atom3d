#ifndef __ATOM3D_ENGINE_PROPERTY_H
#define __ATOM3D_ENGINE_PROPERTY_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "basedefs.h"

class ATOM_PropertySet
{
public:
	void clear (void);
	unsigned getNumProperties (void) const;
	const char *getValue (const char *key) const;
	void setValue (const char *key, const char *value);
	const ATOM_HASHMAP<ATOM_STRING, ATOM_STRING> &getProperties (void) const;
	ATOM_HASHMAP<ATOM_STRING, ATOM_STRING> &getProperties (void);

private:
	ATOM_HASHMAP<ATOM_STRING, ATOM_STRING> _props;
};

class ATOM_PropertySetCache: public ATOM_ReferenceObj
{
public:
	void setNumPropertySets (unsigned size);
	unsigned getNumPropertySets (void) const;
	const ATOM_PropertySet &getPropertySet (unsigned index) const;
	ATOM_PropertySet &getPropertySet (unsigned index);

private:
	ATOM_VECTOR<ATOM_PropertySet> _propSets;
};

#endif // __ATOM3D_ENGINE_PROPERTY_H
