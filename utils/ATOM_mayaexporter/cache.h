#ifndef __ATOM_MAYAEXPORTER_CACHE_H
#define __ATOM_MAYAEXPORTER_CACHE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>
#include <ATOM_math.h>

struct JointTransform
{
	ATOM_Vector3f scale;
	ATOM_Vector3f translation;
	ATOM_Quaternion rotate;

	friend inline bool operator == (const JointTransform &jt1, const JointTransform &jt2)
	{
		return jt1.scale == jt2.scale && jt1.translation == jt2.translation && jt1.rotate == jt2.rotate;
	}

	friend inline bool operator != (const JointTransform &jt1, const JointTransform &jt2)
	{
		return ! operator == (jt1, jt2);
	}
};


template <class T>
struct Cache
{
	std::vector<T> elements;

	int add (const T &value)
	{
		for (unsigned i = 0; i < elements.size(); ++i)
		{
			if (elements[i] == value)
			{
				return (int)i;
			}
		}
		elements.resize (elements.size() + 1);
		elements.back () = value;
		return elements.size() - 1;
	}

	int getIndex (const T &value) const
	{
		for (unsigned i = 0; i < elements.size(); ++i)
		{
			if (elements[i] == value)
			{
				return (int)i;
			}
		}
		return -1;
	}

	const T &getElement (int index) const
	{
		return elements[index];
	}

	unsigned getNumElements (void) const
	{
		return elements.size();
	}
};

typedef Cache<std::vector<ATOM_Vector3f> > Vector3Cache;
typedef Cache<std::vector<ATOM_Vector2f> > Vector2Cache;
typedef Cache<ATOM_Matrix4x4f> Matrix44Cache;
typedef Cache<ATOM_BBox> ATOM_BBoxCache;
typedef Cache<std::vector<JointTransform> > JointTransformCache;

#endif // __ATOM_MAYAEXPORTER_CACHE_H