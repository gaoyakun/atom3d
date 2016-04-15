#ifndef __ATOM3D_ENGINE_DECAL_KEYFRAME_H
#define __ATOM3D_ENGINE_DECAL_KEYFRAME_H

#include "composition_keyframevalue.h"
#include "composition_io.h"

struct ATOM_DecalKeyFrameData
{
	ATOM_Vector4f color;

	ATOM_DecalKeyFrameData (void)
	{
	}

	ATOM_DecalKeyFrameData (float)
	{
	}

	ATOM_DecalKeyFrameData & operator /= (float)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_DecalKeyFrameData & operator -= (const ATOM_DecalKeyFrameData &)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_DecalKeyFrameData operator - (void) const
	{
		ATOM_ASSERT(0);
		return ATOM_DecalKeyFrameData(*this);
	}

	ATOM_DecalKeyFrameData operator / (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_DecalKeyFrameData(*this);
	}

	ATOM_DecalKeyFrameData operator * (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_DecalKeyFrameData(*this);
	}

	ATOM_DecalKeyFrameData operator * (const ATOM_DecalKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_DecalKeyFrameData(*this);
	}

	ATOM_DecalKeyFrameData operator + (const ATOM_DecalKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_DecalKeyFrameData(*this);
	}

	ATOM_DecalKeyFrameData operator - (const ATOM_DecalKeyFrameData&) const
	{
		ATOM_ASSERT(0);
		return ATOM_DecalKeyFrameData(*this);
	}
};

template <>
struct KeyValueXMLWriter<ATOM_DecalKeyFrameData>
{
	void operator ()(long x, const ATOM_DecalKeyFrameData &val, ATOM_TiXmlElement *xml) const
	{
		char buffer[256];

		xml->SetAttribute ("Time", x);

		sprintf (buffer, "%f,%f,%f,%f", val.color.x, val.color.y, val.color.z, val.color.w);
		xml->SetAttribute ("Color", buffer);
	}
};

template <>
struct KeyValueXMLLoader<ATOM_DecalKeyFrameData>
{
	bool operator ()(long &x, ATOM_DecalKeyFrameData &val, const ATOM_TiXmlElement *xml) const
	{
		if (xml->QueryIntAttribute ("Time", (int*)&x) != ATOM_TIXML_SUCCESS)
			x = 0;

		const char *color = xml->Attribute ("Color");
		if (!color || 4 != sscanf (color, "%f,%f,%f,%f", &val.color.x, &val.color.y, &val.color.z, &val.color.w))
			val.color.set (1.f, 1.f, 1.f, 1.f);

		return true;
	}
};

struct DecalLinearPolicy
{
	ATOM_DecalKeyFrameData ipol (const ATOM_DecalKeyFrameData &val1, const ATOM_DecalKeyFrameData &val2, float t) const
	{
		ATOM_DecalKeyFrameData result;

		result.color = val1.color + (val2.color - val1.color) * t;

		return result;
	}
};

class ATOM_ENGINE_API ATOM_DecalKeyFrame: public ATOM_CompositionKeyFrameT<ATOM_DecalKeyFrameData>
{
public:
	typedef DecalLinearPolicy linear_policy_type;

public:
	ATOM_DecalKeyFrame (void);
	ATOM_DecalKeyFrame (const ATOM_DecalKeyFrameData &data);

	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_Decal &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_DecalKeyFrame, ATOM_DecalKeyFrameData)

class ATOM_ENGINE_API ATOM_DecalKeyFrameValue: public ATOM_CompositionKeyFrameValue
{
public:
	void setData (const ATOM_DecalKeyFrameData &data);
	const ATOM_DecalKeyFrameData & getData (void) const;

private:
	ATOM_DecalKeyFrameData _data;
};

#endif // __ATOM3D_ENGINE_DECAL_KEYFRAME_H

