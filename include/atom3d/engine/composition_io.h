#ifndef __ATOM3D_ENGINE_COMPOSITION_IO_H
#define __ATOM3D_ENGINE_COMPOSITION_IO_H

#include "../ATOM_kernel.h"

class ATOM_CompositionActor;

template <class ValueType>
struct KeyValueXMLWriter
{
};

template <class ValueType>
struct KeyValueXMLLoader
{
};

template <>
struct KeyValueXMLWriter<int>
{
	void operator ()(long x, int val, ATOM_TiXmlElement *xml) const
	{
		xml->SetAttribute ("time", int(x));
		xml->SetAttribute ("value", val);
	}
};

template <>
struct KeyValueXMLLoader<int>
{
	bool operator ()(long &x, int &val, const ATOM_TiXmlElement *xml) const
	{
		int nx, nval;

		if (xml->QueryIntAttribute ("time", &nx) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryIntAttribute ("value", &nval) != ATOM_TIXML_SUCCESS)
			return false;

		x = nx;
		val = nval;

		return true;
	}
};

template <>
struct KeyValueXMLWriter<float>
{
	void operator ()(long x, float val, ATOM_TiXmlElement *xml) const
	{
		xml->SetAttribute ("time", int(x));
		xml->SetDoubleAttribute( "value", val);
	}
};

template <>
struct KeyValueXMLLoader<float>
{
	bool operator ()(long &x, float &val, const ATOM_TiXmlElement *xml) const
	{
		int nx;
		float fval;

		if (xml->QueryIntAttribute ("time", &nx) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("value", &fval) != ATOM_TIXML_SUCCESS)
			return false;

		x = nx;
		val = fval;

		return true;
	}
};

template <>
struct KeyValueXMLWriter<ATOM_Vector3f>
{
	void operator ()(long x, const ATOM_Vector3f &val, ATOM_TiXmlElement *xml) const
	{
		char buffer[256];
		sprintf (buffer, "%f,%f,%f", val.x, val.y, val.z);
		xml->SetAttribute ("time", int(x));
		xml->SetAttribute( "value", buffer);
	}
};

template <>
struct KeyValueXMLLoader<ATOM_Vector3f>
{
	bool operator ()(long &x, ATOM_Vector3f &val, const ATOM_TiXmlElement *xml) const
	{
		int nx;
		ATOM_Vector3f vval;

		if (xml->QueryIntAttribute ("time", &nx) != ATOM_TIXML_SUCCESS)
			return false;

		const char *s = xml->Attribute ("value");
		if (!s)
			return false;

		if (3 != sscanf (s, "%f,%f,%f", &vval.x, &vval.y, &vval.z))
			return false;

		x = nx;
		val = vval;

		return true;
	}
};

template <>
struct KeyValueXMLWriter<ATOM_Vector4f>
{
	void operator ()(long x, const ATOM_Vector4f &val, ATOM_TiXmlElement *xml) const
	{
		char buffer[256];
		sprintf (buffer, "%f,%f,%f,%f", val.x, val.y, val.z, val.w);
		xml->SetAttribute ("time", int(x));
		xml->SetAttribute( "value", buffer);
	}
};

template <>
struct KeyValueXMLLoader<ATOM_Vector4f>
{
	bool operator ()(long &x, ATOM_Vector4f &val, const ATOM_TiXmlElement *xml) const
	{
		int nx;
		ATOM_Vector4f vval;

		if (xml->QueryIntAttribute ("time", &nx) != ATOM_TIXML_SUCCESS)
			return false;

		const char *s = xml->Attribute ("value");
		if (!s)
			return false;

		if (4 != sscanf (s, "%f,%f,%f,%f", &vval.x, &vval.y, &vval.z, &vval.w))
			return false;

		x = nx;
		val = vval;

		return true;
	}
};

void ATOM_SaveActorToXML (ATOM_CompositionActor *actor, ATOM_TiXmlElement *xml);

//--- wangjian modified ---//
// 异步加载：增加了一个异步加载的优先级（默认非异步）
ATOM_CompositionActor *ATOM_LoadActorFromXML (	ATOM_RenderDevice *device, const ATOM_TiXmlElement *xml, bool loadRef, 
												int loadPriority = ATOM_LoadPriority_IMMEDIATE	);
//-------------------------//

#endif // __ATOM3D_ENGINE_COMPOSITION_IO_H
