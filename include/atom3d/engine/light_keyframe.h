#ifndef __ATOM3D_ENGINE_LIGHT_KEYFRAME_H
#define __ATOM3D_ENGINE_LIGHT_KEYFRAME_H

#include "composition_keyframevalue.h"
#include "composition_io.h"

struct ATOM_LightKeyFrameData
{
	int type;
	ATOM_Vector4f color;
	ATOM_Vector3f atten;

	ATOM_LightKeyFrameData (void)
	{
	}

	ATOM_LightKeyFrameData (float)
	{
	}

	ATOM_LightKeyFrameData & operator /= (float)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_LightKeyFrameData & operator -= (const ATOM_LightKeyFrameData &)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_LightKeyFrameData operator - (void) const
	{
		ATOM_ASSERT(0);
		return ATOM_LightKeyFrameData(*this);
	}

	ATOM_LightKeyFrameData operator / (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_LightKeyFrameData(*this);
	}

	ATOM_LightKeyFrameData operator * (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_LightKeyFrameData(*this);
	}

	ATOM_LightKeyFrameData operator * (const ATOM_LightKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_LightKeyFrameData(*this);
	}

	ATOM_LightKeyFrameData operator + (const ATOM_LightKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_LightKeyFrameData(*this);
	}

	ATOM_LightKeyFrameData operator - (const ATOM_LightKeyFrameData&) const
	{
		ATOM_ASSERT(0);
		return ATOM_LightKeyFrameData(*this);
	}
};

template <>
struct KeyValueXMLWriter<ATOM_LightKeyFrameData>
{
	void operator ()(long x, const ATOM_LightKeyFrameData &val, ATOM_TiXmlElement *xml) const
	{
		char buffer[256];

		xml->SetAttribute ("Time", x);
		xml->SetAttribute ("LightType", val.type);

		sprintf (buffer, "%f,%f,%f,%f", val.color.x, val.color.y, val.color.z, val.color.w);
		xml->SetAttribute ("Color", buffer);

		sprintf (buffer, "%f,%f,%f", val.atten.x, val.atten.y, val.atten.z);
		xml->SetAttribute ("Atten", buffer);
	}
};

template <>
struct KeyValueXMLLoader<ATOM_LightKeyFrameData>
{
	bool operator ()(long &x, ATOM_LightKeyFrameData &val, const ATOM_TiXmlElement *xml) const
	{
		if (xml->QueryIntAttribute ("Time", (int*)&x) != ATOM_TIXML_SUCCESS)
			x = 0;

		if (xml->QueryIntAttribute ("LightType", (int*)&val.type) != ATOM_TIXML_SUCCESS)
			val.type = ATOM_Light::Point;

		const char *color = xml->Attribute ("Color");
		if (!color || 4 != sscanf (color, "%f,%f,%f,%f", &val.color.x, &val.color.y, &val.color.z, &val.color.w))
			val.color.set (1.f, 1.f, 1.f, 1.f);

		const char *atten = xml->Attribute ("Atten");
		if (!atten || 3 != sscanf (atten, "%f,%f,%f", &val.atten.x, &val.atten.y, &val.atten.z))
			val.atten.set (1.f, 0.1f, 0.001f);

		return true;
	}
};

struct LightLinearPolicy
{
	ATOM_LightKeyFrameData ipol (const ATOM_LightKeyFrameData &val1, const ATOM_LightKeyFrameData &val2, float t) const
	{
		ATOM_LightKeyFrameData result;

		result.type = t < 1.f ? val1.type : val2.type;
		result.color = val1.color + (val2.color - val1.color) * t;
		result.atten = val1.atten + (val2.atten - val1.atten) * t;

		return result;
	}
};

class ATOM_ENGINE_API ATOM_LightKeyFrame: public ATOM_CompositionKeyFrameT<ATOM_LightKeyFrameData>
{
public:
	typedef LightLinearPolicy linear_policy_type;

public:
	ATOM_LightKeyFrame (void);
	ATOM_LightKeyFrame (const ATOM_LightKeyFrameData &data);

	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_LightNode &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_LightKeyFrame, ATOM_LightKeyFrameData)

class ATOM_ENGINE_API ATOM_LightKeyFrameValue: public ATOM_CompositionKeyFrameValue
{
public:
	void setData (const ATOM_LightKeyFrameData &data);
	const ATOM_LightKeyFrameData & getData (void) const;

private:
	ATOM_LightKeyFrameData _data;
};

#endif // __ATOM3D_ENGINE_LIGHT_KEYFRAME_H

