#ifndef __ATOM3D_MATH_ANIMATION_CURVE_H
#define __ATOM3D_MATH_ANIMATION_CURVE_H

#include <cassert>
#include <vector>
#include <iterator>
#include <algorithm>

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

enum ATOM_WaveType
{
	ATOM_WAVE_TYPE_SPLINE = 0,
	ATOM_WAVE_TYPE_LINEAR = 1,
	ATOM_WAVE_TYPE_STEP = 2
};

enum ATOM_CurveAddress
{
	ATOM_CURVE_ADDRESS_CONTINUE = 0,
	ATOM_CURVE_ADDRESS_WRAP = 1,
	ATOM_CURVE_ADDRESS_CLAMP = 2
};

template <class ValueType>
struct LinearInterpolationPolicy
{
	ValueType ipol (const ValueType &val1, const ValueType &val2, float t)
	{
		return val1 + (val2 - val1) * t;
	}
};

template <class ValueType, class ValueOwner>
struct ATOM_CurveValuePolicy
{
	const ValueType & operator () (const ValueOwner &owner) const
	{
		return owner;
	}
};

template <class ValueType, class ValueOwner = ValueType, class LinearPolicy = LinearInterpolationPolicy<ValueType> >
class ATOM_AnimationCurve
{
public:
	struct KeyPoint
	{
		long x;
		ValueOwner val;

		KeyPoint(void) {}
		KeyPoint(long _x, const ValueOwner &_val): x(_x), val(_val) {}

		void set(long _x, const ValueOwner &_val) { x = _x; val = _val; }
		bool operator <(long time) const { return x < time; }
		bool operator <(const KeyPoint &other) const { return x < other.x; }
		const ValueType & value(void) const { return ATOM_CurveValuePolicy<ValueType, ValueOwner>()(val); }
	};

	class KeyPointList
	{
	public:
		typedef std::vector<KeyPoint> point_list;

	public:
		unsigned getCount() const;
		const KeyPoint& operator [](unsigned index) const;
		KeyPoint& operator [](unsigned index);
		unsigned insert(long x, const ValueOwner &y);
		void remove(unsigned index);
		void clear(void);
		unsigned getPeriod (void) const;

	private:
		point_list _points;
	};

public:
    ATOM_AnimationCurve (void);

public:
    unsigned insertKey(long x, const ValueOwner &y);
	unsigned findKey (long x) const;
    void removeKey(unsigned index);
    void removeAllKeys (void);
	const typename ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPoint& operator [](unsigned index) const;
	typename ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPoint& operator [](unsigned index);
    unsigned getNumKeys (void) const;
	void setWaveType (ATOM_WaveType waveType);
	ATOM_WaveType getWaveType (void) const;
	void setAddress (ATOM_CurveAddress address);
	ATOM_CurveAddress getAddress (void) const;
    ValueType evalSpline (long x) const;
    ValueType evalLinear (long x) const;
    ValueType evalStep (long x) const;
	ValueType eval(long x) const;

private:
    void computeSpline(void) const;
    void solveTridiag(std::vector<float>& sub, std::vector<float>& diag, std::vector<float>& sup) const;
    int getSegment(long x) const;

private:
    KeyPointList _keyPoints;
	ATOM_WaveType _waveType;
	ATOM_CurveAddress _address;
	mutable std::vector<ValueType> _a;
	mutable std::vector<long> _h;
    mutable bool _keyChanged;
};

#include "animation_curve.inl"

// explicit instantiation
template class ATOM_AnimationCurve<float>;
template class ATOM_AnimationCurve<double>;
template class ATOM_AnimationCurve<ATOM_Vector2f>;
template class ATOM_AnimationCurve<ATOM_Vector3f>;
template class ATOM_AnimationCurve<ATOM_Vector4f>;

// 
typedef ATOM_AnimationCurve<float> ATOM_FloatAnimationCurve;
typedef ATOM_AnimationCurve<double> ATOM_DoubleAnimationCurve;
typedef ATOM_AnimationCurve<ATOM_Vector2f> ATOM_Vector2DAnimationCurve;
typedef ATOM_AnimationCurve<ATOM_Vector3f> ATOM_Vector3DAnimationCurve;
typedef ATOM_AnimationCurve<ATOM_Vector4f> ATOM_Vector4DAnimationCurve;

#endif // __NEW3D_RENDER_INTERPOLATOR_H
