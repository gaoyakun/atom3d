#ifndef __ATOM3D_ENGINE_COMPOSITION_KEYFRAME_H
#define __ATOM3D_ENGINE_COMPOSITION_KEYFRAME_H

#include "../ATOM_kernel.h"
#include "basedefs.h"
#include "visitor.h"

class ATOM_ENGINE_API ATOM_CompositionKeyFrame: public ATOM_Visitor, public ATOM_ReferenceObj
{
public:
	enum InterpolateMode
	{
		LinearInterpolation,
		StepInterpolation,
		SplineInterpolation
	};

public:
	virtual void visit (ATOM_Node &node) = 0;
};

template <class ValueType, class LinearPolicy = LinearInterpolationPolicy<ValueType> >
class ATOM_CompositionKeyFrameT: public ATOM_CompositionKeyFrame
{
public:
	typedef ValueType value_type;
	typedef LinearPolicy linear_policy_type;

public:
	ATOM_CompositionKeyFrameT (void)
	{
	}

	ATOM_CompositionKeyFrameT (const value_type &value)
		: _value(value)
	{
	}

	const value_type &getValue (void) const
	{
		return _value;
	}

	void setValue (const value_type &value)
	{
		_value = value;
	}

private:
	value_type _value;
};

#define ATOM_DECLARE_COMPOSITION_KEYFRAME(KeyFrameType, ValueType) \
template <> struct ATOM_CurveValuePolicy<ValueType, ATOM_AUTOPTR(KeyFrameType) > { \
	const ValueType & operator () (const ATOM_AUTOPTR(KeyFrameType) &owner) const { \
		return owner->getValue(); \
	} \
};

#endif // __ATOM3D_ENGINE_COMPOSITION_KEYFRAME_H
