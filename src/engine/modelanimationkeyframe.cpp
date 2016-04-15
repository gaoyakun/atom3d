#include "StdAfx.h"
#include "modelanimationkeyframe.h"
#include "skeleton.h"

ATOM_ModelAnimationDataCache::ATOM_ModelAnimationDataCache (void)
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationDataCache::ATOM_ModelAnimationDataCache);

	animationData = 0;
	headerOffset = 0;
}

ATOM_ModelAnimationDataCache::~ATOM_ModelAnimationDataCache (void)
{
	ATOM_FREE(animationData);
}

unsigned ATOM_ModelAnimationDataCache::getNumJointFrames (void) const 
{
	char *p = animationData;
	p += headerOffset;
	unsigned n = *((unsigned*)p);
	return n;
}

unsigned ATOM_ModelAnimationDataCache::getJointCount (unsigned frame) const 
{ 
	char *p = animationData;
	p += headerOffset;
	p += 4 * sizeof(unsigned);
	p += frame * 2 * sizeof(unsigned);
	unsigned n = *((unsigned*)p);
	return n; 
}
const ATOM_JointTransformInfo *ATOM_ModelAnimationDataCache::getJoints (unsigned frame) const 
{ 
	char *p0 = animationData;
	char *p =p0 + headerOffset;
	p += 4 * sizeof(unsigned);
	p += frame * 2 * sizeof(unsigned);
	p += sizeof(unsigned);
	unsigned offset = *((unsigned*)p);
	p0 += offset;
	return (const ATOM_JointTransformInfo*)p0;
}
ATOM_JointTransformInfo *ATOM_ModelAnimationDataCache::getJoints (unsigned frame)
{ 
	char *p0 = animationData;
	char *p =p0 + headerOffset;
	p += 4 * sizeof(unsigned);
	p += frame * 2 * sizeof(unsigned);
	p += sizeof(unsigned);
	unsigned offset = *((unsigned*)p);
	p0 += offset;
	return (ATOM_JointTransformInfo*)p0;
}
unsigned ATOM_ModelAnimationDataCache::getNumUVFrames (void) const 
{ 
	char *p = animationData;
	p += headerOffset;
	p += sizeof(unsigned);
	unsigned n = *((unsigned*)p);
	return n;
}
unsigned ATOM_ModelAnimationDataCache::getUVCount (unsigned frame) const 
{ 
	char *p = animationData;
	p += headerOffset;
	unsigned numJointFrames = *((unsigned*)p);
	p += 4 * sizeof(unsigned);
	p += numJointFrames * 2 * sizeof(unsigned);
	p += frame * 2 * sizeof(unsigned);
	unsigned n = *((unsigned*)p);
	return n; 
}
const ATOM_Vector2f *ATOM_ModelAnimationDataCache::getUVs (unsigned frame) const 
{ 
	char *p0 = animationData;
	char *p =p0 + headerOffset;
	unsigned numJointFrames = *((unsigned*)p);
	p += 4 * sizeof(unsigned);
	p += numJointFrames * 2 * sizeof(unsigned);
	p += frame * 2 * sizeof(unsigned);
	p += sizeof(unsigned);
	unsigned offset = *((unsigned*)p);
	p0 += offset;
	return (const ATOM_Vector2f*)p0;
}
unsigned ATOM_ModelAnimationDataCache::getNumVertexFrames (void) const 
{ 
	char *p = animationData;
	p += headerOffset;
	p += 2 * sizeof(unsigned);
	unsigned n = *((unsigned*)p);
	return n;
}
unsigned ATOM_ModelAnimationDataCache::getVertexCount (unsigned frame) const 
{ 
	char *p = animationData;
	p += headerOffset;
	unsigned numJointFrames = *((unsigned*)p);
	p += sizeof(unsigned);
	unsigned numUVFrames = *((unsigned*)p);
	p += 3 * sizeof(unsigned);
	p += numJointFrames * 2 * sizeof(unsigned);
	p += numUVFrames * 2 * sizeof(unsigned);
	p += frame * 2 * sizeof(unsigned);
	unsigned n = *((unsigned*)p);
	return n; 
}
const ATOM_Vector3f *ATOM_ModelAnimationDataCache::getVertices (unsigned frame) const 
{ 
	char *p0 = animationData;
	char *p =p0 + headerOffset;
	unsigned numJointFrames = *((unsigned*)p);
	p += sizeof(unsigned);
	unsigned numUVFrames = *((unsigned*)p);
	p += 3 * sizeof(unsigned);
	p += numJointFrames * 2 * sizeof(unsigned);
	p += numUVFrames * 2 * sizeof(unsigned);
	p += frame * 2 * sizeof(unsigned);
	p += sizeof(unsigned);
	unsigned offset = *((unsigned*)p);
	p0 += offset;
	return (const ATOM_Vector3f*)p0;
}
unsigned ATOM_ModelAnimationDataCache::getNumTransforms (void) const 
{ 
	char *p = animationData;
	p += headerOffset;
	p += 3 * sizeof(unsigned);
	unsigned n = *((unsigned*)p);
	return n;
}
const ATOM_Matrix4x4f *ATOM_ModelAnimationDataCache::getTransforms (void) const 
{ 
	char *p0 = animationData;
	char *p =p0 + headerOffset;
	unsigned numJointFrames = *((unsigned*)p);
	p += sizeof(unsigned);
	unsigned numUVFrames = *((unsigned*)p);
	p += sizeof(unsigned);
	unsigned numVertexFrames = *((unsigned*)p);
	p += 2 * sizeof(unsigned);
	p += numJointFrames * 2 * sizeof(unsigned);
	p += numUVFrames * 2 * sizeof(unsigned);
	p += numVertexFrames * 2 * sizeof(unsigned);
	unsigned offset = *((unsigned*)p);
	p0 += offset;
	return (const ATOM_Matrix4x4f*)p0;
}

void ATOM_ModelAnimationDataCache::create (	unsigned numJointFrames, 
				const ATOM_VECTOR<unsigned> &jointCounts,
				const ATOM_VECTOR<const void*> &jointStreams,
				unsigned numUVFrames,
				const ATOM_VECTOR<unsigned> &uvCounts,
				const ATOM_VECTOR<const void*> &uvStreams,
				unsigned numVertexFrames,
				const ATOM_VECTOR<unsigned> &vertexCounts,
				const ATOM_VECTOR<const void*> &vertexStreams,
				unsigned numTransforms,
				const ATOM_Matrix4x4f *transformStream)
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationDataCache::create);

	unsigned headerSize = 4 * sizeof(unsigned) + 2 * sizeof(unsigned) * (numVertexFrames + numUVFrames + numJointFrames) + sizeof(unsigned);
	unsigned dataSize = 0;
	for (unsigned i = 0; i < jointCounts.size(); ++i)
	{
		dataSize += jointCounts[i] * sizeof(ATOM_JointTransformInfo);
	}
	for (unsigned i = 0; i < vertexCounts.size(); ++i)
	{
		dataSize += vertexCounts[i] * sizeof(float) * 3;
	}
	for (unsigned i = 0; i < uvCounts.size(); ++i)
	{
		dataSize += uvCounts[i] * sizeof(float) * 2;
	}
	dataSize += numTransforms * sizeof(ATOM_Matrix4x4f);

	animationData = (char*)ATOM_MALLOC(dataSize + headerSize);
	headerOffset = dataSize;
	char *pData = animationData;
	unsigned *pHeader = (unsigned*)(pData + dataSize);

	*pHeader++ = numJointFrames;
	*pHeader++ = numUVFrames;
	*pHeader++ = numVertexFrames;
	*pHeader++ = numTransforms;

	unsigned offset = 0;

	for (unsigned i = 0; i < jointCounts.size(); ++i)
	{
		*pHeader++ = jointCounts[i];
		*pHeader++ = offset;
		unsigned sz = jointCounts[i] * sizeof(ATOM_JointTransformInfo);
		struct _jointInfo
		{
			ATOM_Vector3f scale;
			ATOM_Vector3f translate;
			ATOM_Quaternion rotate;
		};
		_jointInfo *src = (_jointInfo*)jointStreams[i];
		ATOM_JointTransformInfo *dst = (ATOM_JointTransformInfo *)(pData + offset);
		for (unsigned j = 0; j < jointCounts[i]; ++j)
		{
			dst[j].rotate = src[j].rotate; 
			dst[j].translate.set (src[j].translate.x, src[j].translate.y, src[j].translate.z, 1.f);
			dst[j].scale.set (src[j].scale.x, src[j].scale.y, src[j].scale.z, 1.f);
		}
		offset += sz;
	}

	for (unsigned i = 0; i < uvCounts.size(); ++i)
	{
		*pHeader++ = uvCounts[i];
		*pHeader++ = offset;
		unsigned sz = uvCounts[i] * sizeof(float) * 2;
		memcpy (pData + offset, uvStreams[i], sz);
		offset += sz;
	}

	for (unsigned i = 0; i < vertexCounts.size(); ++i)
	{
		*pHeader++ = vertexCounts[i];
		*pHeader++ = offset;
		unsigned sz = vertexCounts[i] * sizeof(float) * 3;
		memcpy (pData + offset, vertexStreams[i], sz);
		offset += sz;
	}

	*pHeader++ = offset;
	if (numTransforms > 0)
	{
		memcpy (pData + offset, transformStream, numTransforms * sizeof(ATOM_Matrix4x4f));
	}
}

