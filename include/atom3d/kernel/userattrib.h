#ifndef __ATOM3D_KERNEL_WIDGETATTRIB_H
#define __ATOM3D_KERNEL_WIDGETATTRIB_H

#include "../ATOM_dbghlp.h"
#include "variant.h"

class ATOM_KERNEL_API ATOM_UserAttributes
{
public:
	virtual bool setAttribute (const char *name, const ATOM_Variant &value);
	virtual const ATOM_Variant &getAttribute (const char *name) const;
	virtual void clearAttributes (void);
	virtual unsigned getNumAttributes (void) const;
	virtual const ATOM_Variant &getAttributeValue (int index) const;
	virtual const char *getAttributeName (int index) const;

private:
	ATOM_HASHMAP<ATOM_STRING, ATOM_Variant> _attributes;
};

#endif // __ATOM3D_KERNEL_WIDGETATTRIB_H
