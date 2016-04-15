#ifndef __ATOM3D_ENGINE_TRAIL_KEYFRAME_H
#define __ATOM3D_ENGINE_TRAIL_KEYFRAME_H

#include "composition_keyframevalue.h"
#include "composition_io.h"

struct ATOM_TrailKeyFrameData
{
	ATOM_Vector4f colorStart;
	ATOM_Vector4f colorEnd;

	ATOM_TrailKeyFrameData (void)
	{
	}

	ATOM_TrailKeyFrameData (float)
	{
	}

	ATOM_TrailKeyFrameData & operator /= (float)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_TrailKeyFrameData & operator -= (const ATOM_TrailKeyFrameData &)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_TrailKeyFrameData operator - (void) const
	{
		ATOM_ASSERT(0);
		return ATOM_TrailKeyFrameData(*this);
	}

	ATOM_TrailKeyFrameData operator / (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_TrailKeyFrameData(*this);
	}

	ATOM_TrailKeyFrameData operator * (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_TrailKeyFrameData(*this);
	}

	ATOM_TrailKeyFrameData operator * (const ATOM_TrailKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_TrailKeyFrameData(*this);
	}

	ATOM_TrailKeyFrameData operator + (const ATOM_TrailKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_TrailKeyFrameData(*this);
	}

	ATOM_TrailKeyFrameData operator - (const ATOM_TrailKeyFrameData&) const
	{
		ATOM_ASSERT(0);
		return ATOM_TrailKeyFrameData(*this);
	}
};

template <>
struct KeyValueXMLWriter<ATOM_TrailKeyFrameData>
{
	void operator ()(long x, const ATOM_TrailKeyFrameData &val, ATOM_TiXmlElement *xml) const
	{
		xml->SetAttribute ("Time", x);

		char buffer[256];
		sprintf (buffer, "%f,%f,%f,%f", val.colorStart.x, val.colorStart.y, val.colorStart.z, val.colorStart.w);
		xml->SetAttribute ("ColorStart", buffer);
		sprintf (buffer, "%f,%f,%f,%f", val.colorEnd.x, val.colorEnd.y, val.colorEnd.z, val.colorEnd.w);
		xml->SetAttribute ("ColorEnd", buffer);
	}
};

template <>
struct KeyValueXMLLoader<ATOM_TrailKeyFrameData>
{
	bool operator ()(long &x, ATOM_TrailKeyFrameData &val, const ATOM_TiXmlElement *xml) const
	{
		if (xml->QueryIntAttribute ("Time", (int*)&x) != ATOM_TIXML_SUCCESS)
			x = 0;

		const char *colorStart = xml->Attribute ("ColorStart");
		if (!colorStart || 4 != sscanf(colorStart, "%f,%f,%f,%f", &val.colorStart.x, &val.colorStart.y, &val.colorStart.z, &val.colorStart.w))
			val.colorStart.set (1.f, 1.f, 1.f, 1.f);

		const char *colorEnd = xml->Attribute ("ColorEnd");
		if (!colorEnd || 4 != sscanf(colorEnd, "%f,%f,%f,%f", &val.colorEnd.x, &val.colorEnd.y, &val.colorEnd.z, &val.colorEnd.w))
			val.colorEnd.set (1.f, 1.f, 1.f, 1.f);

		return true;
	}
};

struct TrailLinearPolicy
{
	ATOM_TrailKeyFrameData ipol (const ATOM_TrailKeyFrameData &val1, const ATOM_TrailKeyFrameData &val2, float t) const
	{
		ATOM_TrailKeyFrameData result;

		result.colorStart = val1.colorStart + (val2.colorStart - val1.colorStart) * t;
		result.colorEnd = val1.colorEnd + (val2.colorEnd - val1.colorEnd) * t;

		return result;
	}
};

class ATOM_ENGINE_API ATOM_TrailKeyFrame: public ATOM_CompositionKeyFrameT<ATOM_TrailKeyFrameData>
{
public:
	typedef TrailLinearPolicy linear_policy_type;

public:
	ATOM_TrailKeyFrame (void);
	ATOM_TrailKeyFrame (const ATOM_TrailKeyFrameData &data);

	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_RibbonTrail &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_TrailKeyFrame, ATOM_TrailKeyFrameData)

class ATOM_ENGINE_API ATOM_TrailKeyFrameValue: public ATOM_CompositionKeyFrameValue
{
public:
	void setData (const ATOM_TrailKeyFrameData &data);
	const ATOM_TrailKeyFrameData & getData (void) const;

private:
	ATOM_TrailKeyFrameData _data;
};

#endif // __ATOM3D_ENGINE_TRAIL_KEYFRAME_H

