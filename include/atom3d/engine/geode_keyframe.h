#ifndef __ATOM3D_ENGINE_GEODE_KEYFRAME_H
#define __ATOM3D_ENGINE_GEODE_KEYFRAME_H

#include "composition_keyframevalue.h"
#include "composition_io.h"

struct ATOM_GeodeKeyFrameData
{
	int actionIndex;
	int loop;
	unsigned fadeTime;
	float speed;
	float transparency;
	//--- wangjian added ---//
	float colorMultiplier;

	ATOM_GeodeKeyFrameData (void)
	{
		actionIndex = -1;
		loop = 0;
		fadeTime = 500;
		speed = 1.f;
		transparency = 1.f;
		//--- wangjian added ---//
		colorMultiplier = 1.0f;
	}

	ATOM_GeodeKeyFrameData (float)
	{
	}

	ATOM_GeodeKeyFrameData & operator /= (float)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_GeodeKeyFrameData & operator -= (const ATOM_GeodeKeyFrameData &)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_GeodeKeyFrameData operator - (void) const
	{
		ATOM_ASSERT(0);
		return ATOM_GeodeKeyFrameData(*this);
	}

	ATOM_GeodeKeyFrameData operator / (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_GeodeKeyFrameData(*this);
	}

	ATOM_GeodeKeyFrameData operator * (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_GeodeKeyFrameData(*this);
	}

	ATOM_GeodeKeyFrameData operator * (const ATOM_GeodeKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_GeodeKeyFrameData(*this);
	}

	ATOM_GeodeKeyFrameData operator + (const ATOM_GeodeKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_GeodeKeyFrameData(*this);
	}

	ATOM_GeodeKeyFrameData operator - (const ATOM_GeodeKeyFrameData&) const
	{
		ATOM_ASSERT(0);
		return ATOM_GeodeKeyFrameData(*this);
	}
};

template <>
struct KeyValueXMLWriter<ATOM_GeodeKeyFrameData>
{
	void operator ()(long x, const ATOM_GeodeKeyFrameData &val, ATOM_TiXmlElement *xml) const
	{
		xml->SetAttribute ("Time", x);
		xml->SetAttribute ("Action", val.actionIndex);
		xml->SetAttribute ("FadeTime", val.fadeTime);
		xml->SetDoubleAttribute ("Speed", val.speed);
		xml->SetAttribute ("Loop", val.loop);
		xml->SetDoubleAttribute ("Transparency", val.transparency);
		//--- wangjian added ---//
		xml->SetDoubleAttribute ("ColorMultiplier", val.colorMultiplier);
	}
};

template <>
struct KeyValueXMLLoader<ATOM_GeodeKeyFrameData>
{
	bool operator ()(long &x, ATOM_GeodeKeyFrameData &val, const ATOM_TiXmlElement *xml) const
	{
		if (xml->QueryIntAttribute ("Time", (int*)&x) != ATOM_TIXML_SUCCESS)
			x = 0;

		if (xml->QueryIntAttribute ("Action", (int*)&val.actionIndex) != ATOM_TIXML_SUCCESS)
			val.actionIndex = -1;

		if (xml->QueryIntAttribute ("FadeTime", (int*)&val.fadeTime) != ATOM_TIXML_SUCCESS)
			val.fadeTime = 500;

		if (xml->QueryFloatAttribute ("Speed", &val.speed) != ATOM_TIXML_SUCCESS)
			val.speed = 1.f;

		if (xml->QueryIntAttribute ("Loop", &val.loop) != ATOM_TIXML_SUCCESS)
			val.loop = 0;

		if (xml->QueryFloatAttribute ("Transparency", &val.transparency) != ATOM_TIXML_SUCCESS)
			val.transparency = 1.f;

		//--- wangjian added ---//
		if (xml->QueryFloatAttribute ("ColorMultiplier", &val.colorMultiplier) != ATOM_TIXML_SUCCESS)
			val.colorMultiplier = 1.f;

		return true;
	}
};

struct GeodeLinearPolicy
{
	ATOM_GeodeKeyFrameData ipol (const ATOM_GeodeKeyFrameData &val1, const ATOM_GeodeKeyFrameData &val2, float t) const
	{
		ATOM_GeodeKeyFrameData result;

		result.actionIndex = t < 1.f ? val1.actionIndex : val2.actionIndex;
		result.fadeTime = t < 1.f ? val1.fadeTime : val2.fadeTime;
		result.loop = t < 1.f ? val1.loop : val2.loop;
		result.speed = t < 1.f ? val1.speed : val2.speed;
		result.transparency = val1.transparency + (val2.transparency - val1.transparency) * t;
		//--- wangjian added ---//
		result.colorMultiplier = val1.colorMultiplier + (val2.colorMultiplier - val1.colorMultiplier) * t;

		return result;
	}
};

class ATOM_ENGINE_API ATOM_GeodeKeyFrame: public ATOM_CompositionKeyFrameT<ATOM_GeodeKeyFrameData>
{
public:
	typedef GeodeLinearPolicy linear_policy_type;

public:
	ATOM_GeodeKeyFrame (void);
	ATOM_GeodeKeyFrame (const ATOM_GeodeKeyFrameData &data);

	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_Geode &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_GeodeKeyFrame, ATOM_GeodeKeyFrameData)

class ATOM_ENGINE_API ATOM_GeodeKeyFrameValue: public ATOM_CompositionKeyFrameValue
{
public:
	void setData (const ATOM_GeodeKeyFrameData &data);
	const ATOM_GeodeKeyFrameData & getData (void) const;

private:
	ATOM_GeodeKeyFrameData _data;
};

#endif // __ATOM3D_ENGINE_GEODE_KEYFRAME_H

