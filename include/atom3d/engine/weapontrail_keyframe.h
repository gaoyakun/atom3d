#ifndef __ATOM3D_ENGINE_WEAPONTRAIL_KEYFRAME_H
#define __ATOM3D_ENGINE_WEAPONTRAIL_KEYFRAME_H

#include "composition_keyframevalue.h"
#include "composition_io.h"

struct ATOM_WeaponTrailKeyFrameData
{
	ATOM_Vector4f color;

	ATOM_WeaponTrailKeyFrameData (void)
	{
	}

	ATOM_WeaponTrailKeyFrameData (float)
	{
	}

	ATOM_WeaponTrailKeyFrameData & operator /= (float)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_WeaponTrailKeyFrameData & operator -= (const ATOM_WeaponTrailKeyFrameData &)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_WeaponTrailKeyFrameData operator - (void) const
	{
		ATOM_ASSERT(0);
		return ATOM_WeaponTrailKeyFrameData(*this);
	}

	ATOM_WeaponTrailKeyFrameData operator / (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_WeaponTrailKeyFrameData(*this);
	}

	ATOM_WeaponTrailKeyFrameData operator * (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_WeaponTrailKeyFrameData(*this);
	}

	ATOM_WeaponTrailKeyFrameData operator * (const ATOM_WeaponTrailKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_WeaponTrailKeyFrameData(*this);
	}

	ATOM_WeaponTrailKeyFrameData operator + (const ATOM_WeaponTrailKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_WeaponTrailKeyFrameData(*this);
	}

	ATOM_WeaponTrailKeyFrameData operator - (const ATOM_WeaponTrailKeyFrameData&) const
	{
		ATOM_ASSERT(0);
		return ATOM_WeaponTrailKeyFrameData(*this);
	}
};

template <>
struct KeyValueXMLWriter<ATOM_WeaponTrailKeyFrameData>
{
	void operator ()(long x, const ATOM_WeaponTrailKeyFrameData &val, ATOM_TiXmlElement *xml) const
	{
		xml->SetAttribute ("Time", x);

		char buffer[256];
		sprintf (buffer, "%f,%f,%f,%f", val.color.x, val.color.y, val.color.z, val.color.w);
		xml->SetAttribute ("Color", buffer);
	}
};

template <>
struct KeyValueXMLLoader<ATOM_WeaponTrailKeyFrameData>
{
	bool operator ()(long &x, ATOM_WeaponTrailKeyFrameData &val, const ATOM_TiXmlElement *xml) const
	{
		if (xml->QueryIntAttribute ("Time", (int*)&x) != ATOM_TIXML_SUCCESS)
			x = 0;

		const char *color = xml->Attribute ("Color");
		if (!color || 4 != sscanf(color, "%f,%f,%f,%f", &val.color.x, &val.color.y, &val.color.z, &val.color.w))
			val.color.set (1.f, 1.f, 1.f, 1.f);

		return true;
	}
};

struct WeaponTrailLinearPolicy
{
	ATOM_WeaponTrailKeyFrameData ipol (const ATOM_WeaponTrailKeyFrameData &val1, const ATOM_WeaponTrailKeyFrameData &val2, float t) const
	{
		ATOM_WeaponTrailKeyFrameData result;

		result.color = val1.color + (val2.color - val1.color) * t;

		return result;
	}
};

class ATOM_ENGINE_API ATOM_WeaponTrailKeyFrame: public ATOM_CompositionKeyFrameT<ATOM_WeaponTrailKeyFrameData>
{
public:
	typedef WeaponTrailLinearPolicy linear_policy_type;

public:
	ATOM_WeaponTrailKeyFrame (void);
	ATOM_WeaponTrailKeyFrame (const ATOM_WeaponTrailKeyFrameData &data);

	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_WeaponTrail &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_WeaponTrailKeyFrame, ATOM_WeaponTrailKeyFrameData)

class ATOM_ENGINE_API ATOM_WeaponTrailKeyFrameValue: public ATOM_CompositionKeyFrameValue
{
public:
	void setData (const ATOM_WeaponTrailKeyFrameData &data);
	const ATOM_WeaponTrailKeyFrameData & getData (void) const;

private:
	ATOM_WeaponTrailKeyFrameData _data;
};

#endif // __ATOM3D_ENGINE_WEAPONTRAIL_KEYFRAME_H

