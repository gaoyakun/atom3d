#include "StdAfx.h"
#include "skeleton.h"



ATOM_Skeleton::ATOM_Skeleton (void)
{
	ATOM_STACK_TRACE(ATOM_Skeleton::ATOM_Skeleton);
	_hashCode = 0;
}

unsigned ATOM_Skeleton::getHashCode (void) const
{
	return _hashCode;
}

void ATOM_Skeleton::setHashCode (unsigned code)
{
	_hashCode = code;
}

bool ATOM_Skeleton::isUpside (const char *name) const
{
	return !strnicmp(name, "pu_", 3);
}

bool ATOM_Skeleton::isUpside (unsigned joint) const
{
	return std::find (_upsideMask.begin(), _upsideMask.end(), joint) != _upsideMask.end();
}

void ATOM_Skeleton::addAttachPoint (const char *name, unsigned jointIndex)
{
	ATOM_STACK_TRACE(ATOM_Skeleton::addAttachPoint);
	ATOM_ASSERT(name);
	_attachPoints[name] = jointIndex;

	if (!strcmp (name, "u-Bip01 Pelvis") || !strcmp(name, "u-Bip01") || !strcmp(name, "u-Bip01 Spine"))
	{
		_downsideMask.push_back (jointIndex);
	}
	else if (!strnicmp (name, "pu_", 3) || !strnicmp(name, "u-", 2))
	{
		_upsideMask.push_back (jointIndex);
	}
	else// if (!strnicmp (name, "pd_", 3))
	{
		_downsideMask.push_back (jointIndex);
	}
}

void ATOM_Skeleton::removeAttachPoint (const char *name)
{
	ATOM_STACK_TRACE(ATOM_Skeleton::removeAttachPoint);
	ATOM_ASSERT(name);
	ATOM_HASHMAP<ATOM_STRING, unsigned>::iterator it = _attachPoints.find (name);
	if (it != _attachPoints.end ())
	{
		unsigned jointIndex = it->second;
		_attachPoints.erase (it);

		ATOM_VECTOR<unsigned>::iterator itMask = std::find (_upsideMask.begin(), _upsideMask.end(), jointIndex);
		if (itMask != _upsideMask.end ())
		{
			_upsideMask.erase (itMask);
		}
		else
		{
			itMask = std::find (_downsideMask.begin(), _downsideMask.end(), jointIndex);
			if (itMask != _downsideMask.end ())
			{
				_downsideMask.erase (itMask);
			}
		}
	}
}

void ATOM_Skeleton::clearAttachPoints (void)
{
	ATOM_STACK_TRACE(ATOM_Skeleton::clearAttachPoints);
	_attachPoints.clear ();
	_upsideMask.clear ();
	_downsideMask.clear ();
}

unsigned ATOM_Skeleton::getNumAttachPoints (void) const
{
	return _attachPoints.size();
}

const ATOM_HASHMAP<ATOM_STRING, unsigned> &ATOM_Skeleton::getAttachPoints (void) const
{
	return _attachPoints;
}

unsigned ATOM_Skeleton::getAttachPointIndex (const char *name) const
{
	if (name)
	{
		ATOM_HASHMAP<ATOM_STRING, unsigned>::const_iterator it = _attachPoints.find (name);
		if (it != _attachPoints.end ())
		{
			return it->second;
		}
	}
	return (unsigned)-1;
}

const char *ATOM_Skeleton::getAttachPointName (unsigned index) const
{
	for (ATOM_HASHMAP<ATOM_STRING, unsigned>::const_iterator it = _attachPoints.begin(); it != _attachPoints.end(); ++it)
	{
		if (it->second == index)
		{
			return it->first.c_str();
		}
	}
	return 0;
}

unsigned ATOM_Skeleton::getNumJoints (void) const
{
	return _jointBindMatrices.size();
}

void ATOM_Skeleton::setNumJoints (unsigned num)
{
	ATOM_STACK_TRACE(ATOM_Skeleton::setNumJoints);
	_jointBindMatrices.resize (num);
	_jointParents.resize (num);
	ATOM_LOGGER::debug("Joint count: %d\n", num);
}

void ATOM_Skeleton::transformJoints (const ATOM_Matrix4x4f &matrix)
{
	for (int i = 0; i < _jointBindMatrices.size(); ++i)
	{
		if (_jointParents[i] < 0)
		{
			ATOM_Matrix4x4f m;
			_jointBindMatrices[i].toMatrix44(m);
			_jointBindMatrices[i] = matrix >> m;
		}
	}
	_jointInverseBindMatricesWorld.resize (0);
	_jointBindMatricesWorld.resize (0);
}

void ATOM_Skeleton::setJointBindMatrix (unsigned joint, const ATOM_Matrix3x4f &matrix)
{
	ATOM_STACK_TRACE(ATOM_Skeleton::setJointBindMatrix);
	_jointBindMatrices[joint] = matrix;
}

const ATOM_Matrix3x4f &ATOM_Skeleton::getJointBindMatrix (unsigned joint) const
{
	return _jointBindMatrices[joint];
}

ATOM_VECTOR<ATOM_Matrix3x4f> &ATOM_Skeleton::getJointMatrices (void)
{
	return _jointBindMatrices;
}

const ATOM_VECTOR<ATOM_Matrix3x4f> &ATOM_Skeleton::getJointMatrices (void) const
{
	return _jointBindMatrices;
}

const ATOM_VECTOR<unsigned> &ATOM_Skeleton::getUpsideMask (void) const
{
	return _upsideMask;
}

const ATOM_VECTOR<unsigned> &ATOM_Skeleton::getDownsideMask (void) const
{
	return _downsideMask;
}

ATOM_VECTOR<int> &ATOM_Skeleton::getJointParents (void)
{
	return _jointParents;
}

const ATOM_VECTOR<int> &ATOM_Skeleton::getJointParents (void) const
{
	return _jointParents;
}

const ATOM_VECTOR<ATOM_Matrix3x4f> &ATOM_Skeleton::getInverseJointMatricesWorld (void) const
{
	ATOM_STACK_TRACE(ATOM_Skeleton::getInverseJointMatricesWorld);
	if (_jointInverseBindMatricesWorld.empty ())
	{
		_jointInverseBindMatricesWorld.resize (_jointBindMatrices.size());

		if (_jointBindMatricesWorld.empty ())
		{
			_jointBindMatricesWorld.resize (_jointBindMatrices.size());

			for (unsigned i = 0; i < _jointBindMatrices.size(); ++i)
			{
				if (_jointParents[i] >= 0)
				{
					_jointBindMatricesWorld[i] = _jointBindMatricesWorld[_jointParents[i]] >> _jointBindMatrices[i];
				}
				else
				{
					_jointBindMatricesWorld[i] = _jointBindMatrices[i];
				}
			}
		}

		for (unsigned i = 0; i < _jointInverseBindMatricesWorld.size(); ++i)
		{
			ATOM_Matrix4x4f m;
			_jointBindMatricesWorld[i].toMatrix44(m);
			m.invertAffine ();
			_jointInverseBindMatricesWorld[i] = m;
		}
	}
	return _jointInverseBindMatricesWorld;
}

const ATOM_VECTOR<ATOM_Matrix3x4f> &ATOM_Skeleton::getJointMatricesWorld (void) const
{
	ATOM_STACK_TRACE(ATOM_Skeleton::getJointMatricesWorld);
	if (_jointBindMatricesWorld.empty ())
	{
		_jointBindMatricesWorld.resize (_jointBindMatrices.size());

		for (unsigned i = 0; i < _jointBindMatrices.size(); ++i)
		{
			if (_jointParents[i] >= 0)
			{
				_jointBindMatricesWorld[i] = _jointBindMatricesWorld[_jointParents[i]] >> _jointBindMatrices[i];
			}
			else
			{
				_jointBindMatricesWorld[i] = _jointBindMatrices[i];
			}
		}
	}

	return _jointBindMatricesWorld;
}

void ATOM_Skeleton::dump (void) const
{
	printf ("JointCount=%d\n", getNumJoints());
	printf ("AttachPointCount=%d\n", getNumAttachPoints());
	for (ATOM_HASHMAP<ATOM_STRING, unsigned>::const_iterator it = _attachPoints.begin(); it != _attachPoints.end(); it++)
	{
		unsigned index = it->second;
		printf ("--------%s--------\n", it->first.c_str());
		printf ("parent:%d\n", _jointParents[index]);
		printf ("%f,%f,%f,%f\n", _jointBindMatrices[index].m00,_jointBindMatrices[index].m01,_jointBindMatrices[index].m02,_jointBindMatrices[index].m03);
		printf ("%f,%f,%f,%f\n", _jointBindMatrices[index].m10,_jointBindMatrices[index].m11,_jointBindMatrices[index].m12,_jointBindMatrices[index].m13);
		printf ("%f,%f,%f,%f\n", _jointBindMatrices[index].m20,_jointBindMatrices[index].m21,_jointBindMatrices[index].m22,_jointBindMatrices[index].m23);
	}
}

