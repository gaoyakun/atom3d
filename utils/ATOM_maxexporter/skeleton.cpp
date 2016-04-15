#include <windows.h>

#include "skeleton.h"

struct CRC32
{
	unsigned crc_table[256];

	CRC32 (void)
	{
		unsigned poly = 0xEDB88320;
		for (unsigned i = 0; i < 256; ++i)
		{
			unsigned crc = i;
			for (unsigned j = 8; j > 0; --j)
			{
				if (crc & 1)
				{
					crc = (crc >> 1) ^ poly;
				}
				else
				{
					crc >>= 1;
				}
			}
			crc_table[i] = crc;
		}
	}

	unsigned generate (const char *block, unsigned len) const
	{
		register unsigned crc = 0xFFFFFFFF;

		for (unsigned i = 0; i < len; ++i)
		{
			crc = ((crc >> 8) & 0x00FFFFFF) ^ crc_table[(crc ^ *block++) & 0xFF];
		}

		return crc ^ 0xFFFFFFFF;
	}
};

#define PRECISION 0.0001

unsigned ATOM_Skeleton::calcHash (const char *s, unsigned len)
{
	CRC32 crc32;
	return crc32.generate (s, len);
}

ATOM_Skeleton::ATOM_Skeleton()
{
	m_joints.clear();
	m_hash = 0;
}


ATOM_Skeleton::~ATOM_Skeleton()
{
	clear();
}


void ATOM_Skeleton::clear()
{
	m_joints.clear();
}

// Get joint list
std::vector<ATOM_Skeleton::joint>& ATOM_Skeleton::getJoints()
{
	return m_joints;
}

unsigned ATOM_Skeleton::getHash (void) const
{
	if (!m_hash)
	{
		std::string jointNameChain;
		for (unsigned i = 0; i < m_joints.size(); ++i)
		{
			jointNameChain += m_joints[i].name.c_str();
		}

		CRC32 crc32;
		m_hash = crc32.generate (jointNameChain.c_str(), jointNameChain.length());
	}

	return m_hash;
}

bool ATOM_Skeleton::isIdentical (const ATOM_Skeleton &other) const
{
	if (m_joints.size() != other.m_joints.size())
	{
		return false;
	}

	for (unsigned i = 0 ;i < m_joints.size(); ++i)
	{
		if (m_joints[i].name != other.m_joints[i].name)
		{
			return false;
		}

		if (m_joints[i].parentIndex != other.m_joints[i].parentIndex)
		{
			return false;
		}
	}

	return true;
}

void ATOM_Skeleton::remapJoint (void)
{
	m_remapOldToNew.resize (m_joints.size());
	m_remapNewToOld.resize (attachPoints.size());

	for (unsigned i = 0; i < attachPoints.size(); ++i)
	{
		m_remapOldToNew[attachPoints[i]] = i;
		m_remapNewToOld[i] = attachPoints[i];
	}
}

