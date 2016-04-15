#include "StdAfx.h"
#include "treeinstance.h"
#include "treebatch.h"

ATOM_TreeInstance::ATOM_TreeInstance (void)
{
	ATOM_STACK_TRACE(ATOM_TreeInstance::ATOM_TreeInstance);

	_batch = ATOM_NEW(ATOM_TreeBatch, this);
}

ATOM_TreeInstance::~ATOM_TreeInstance (void)
{
	ATOM_STACK_TRACE(ATOM_TreeInstance::~ATOM_TreeInstance);

	ATOM_DELETE(_batch);
}

bool ATOM_TreeInstance::load(ATOM_RenderDevice *device, const char *filename)
{
	ATOM_STACK_TRACE(ATOM_TreeInstance::load);

	ATOM_AutoFile f(filename, ATOM_VFS::binary|ATOM_VFS::read);
	if (!f)
	{
		return false;
	}

	unsigned sig;
	f->read(&sig, sizeof(sig));
	if (sig != ATOM_MAKE_FOURCC('A','T','T','R'))
	{
		// signature mismatch
		return false;
	}

	unsigned version;
	f->read(&version, sizeof(unsigned));
	if (version != 2)
	{
		// incorrect version
		return false;
	}

	f->read(&_boundingBox, sizeof(ATOM_BBox));
	f->read(&_trunkBBox, sizeof(ATOM_BBox));
	f->read(&_leafBBox, sizeof(ATOM_BBox));

	unsigned numTrunkVertices;
	f->read (&numTrunkVertices, sizeof(unsigned));
	_trunkVertexArray = 0;

	if (numTrunkVertices && !ATOM_RenderSettings::isFixedFunction())
	{
		_trunkVertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL|ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_STATIC, numTrunkVertices, true);
		if (!_trunkVertexArray)
		{
			return false;
		}
		void *p = _trunkVertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		f->read (p, numTrunkVertices * sizeof(float) * 8);
		_trunkVertexArray->unlock ();
	}
	else
	{
		f->seek (numTrunkVertices * sizeof(float) * 8, ATOM_VFS::current);
	}

	unsigned numTrunkIndices;
	f->read (&numTrunkIndices, sizeof(unsigned));
	_trunkIndexArray = 0;
	if (numTrunkIndices && !ATOM_RenderSettings::isFixedFunction())
	{
		_trunkIndexArray = device->allocIndexArray (ATOM_USAGE_STATIC, numTrunkIndices, false, true);
		if (!_trunkIndexArray)
		{
			return false;
		}
		void *p = _trunkIndexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		f->read (p, numTrunkIndices * sizeof(unsigned short));
		_trunkIndexArray->unlock ();
	}
	else
	{
		f->seek (numTrunkIndices * sizeof(unsigned short), ATOM_VFS::current);
	}

	char buffer[ATOM_VFS::max_filename_length];
	unsigned trunkTextureFileNameLen;
	f->read (&trunkTextureFileNameLen, sizeof(unsigned));
	f->read (buffer, trunkTextureFileNameLen);
	buffer[trunkTextureFileNameLen] = '\0';
	if (!ATOM_RenderSettings::isFixedFunction())
	{
		_trunkTexture = ATOM_CreateTextureResource (buffer);
	}

	unsigned numLeafCardVertices;
	f->read (&numLeafCardVertices, sizeof(unsigned));
	_leafCardVertexArray = 0;
	if (!ATOM_RenderSettings::isFixedFunction() && numLeafCardVertices)
	{
		_leafCardVertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_TEX1_4, ATOM_USAGE_STATIC, numLeafCardVertices, true);
		if (!_leafCardVertexArray)
		{
			return false;
		}
		void *p = _leafCardVertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		f->read (p, numLeafCardVertices * sizeof(float) * 8);
		_leafCardVertexArray->unlock ();
	}
	else
	{
		f->seek (numLeafCardVertices * sizeof(float) * 8, ATOM_VFS::current);
	}

	unsigned numLeafCardIndices;
	f->read (&numLeafCardIndices, sizeof(unsigned));
	_leafCardIndexArray = 0;
	if (!ATOM_RenderSettings::isFixedFunction() && numLeafCardIndices)
	{
		_leafCardIndexArray = device->allocIndexArray (ATOM_USAGE_STATIC, numLeafCardIndices, false, true);
		if (!_leafCardIndexArray)
		{
			return false;
		}
		void *p = _leafCardIndexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		f->read (p, numLeafCardIndices * sizeof(unsigned short));
		_leafCardIndexArray->unlock ();
	}
	else
	{
		f->seek (numLeafCardIndices * sizeof(unsigned short), ATOM_VFS::current);
	}

	unsigned leafTextureFileNameLen;
	f->read (&leafTextureFileNameLen, sizeof(unsigned));
	f->read (buffer, leafTextureFileNameLen);
	buffer[leafTextureFileNameLen] = '\0';
	if (!ATOM_RenderSettings::isFixedFunction())
	{
		_leafCardTexture = ATOM_CreateTextureResource (buffer);
	}

	f->read (_billboardInfo, 4 * sizeof(BillboardInfo));
	unsigned numBillboardVertices;
	f->read (&numBillboardVertices, sizeof(unsigned));
	_billboardVertexArray = 0;
	if (!ATOM_RenderSettings::isFixedFunction() && numBillboardVertices)
	{
		_billboardVertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_STATIC, numBillboardVertices, true);
		if (!_billboardVertexArray)
		{
			return false;
		}
		void *p = _billboardVertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		f->read (p, numBillboardVertices * sizeof(float) * 6);
		_billboardVertexArray->unlock ();
	}
	else
	{
		f->seek (numBillboardVertices * sizeof(float) * 6, ATOM_VFS::current);
	}

	unsigned numBillboardIndices;
	f->read (&numBillboardIndices, sizeof(unsigned));
	_billboardIndexArray = 0;
	if (!ATOM_RenderSettings::isFixedFunction() && numBillboardIndices)
	{
		_billboardIndexArray = device->allocIndexArray (ATOM_USAGE_STATIC, numBillboardIndices, false, true);
		if (!_billboardIndexArray)
		{
			return false;
		}
		void *p = _billboardIndexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		f->read (p, numBillboardIndices * sizeof(unsigned short));
		_billboardIndexArray->unlock ();
	}
	else
	{
		f->seek (numBillboardIndices * sizeof(unsigned short), ATOM_VFS::current);
	}

	unsigned billboardColorTextureSize;
	f->read (&billboardColorTextureSize, sizeof(unsigned));
	_billboardTexture = 0;
	if (billboardColorTextureSize)
	{
		void *p = ATOM_MALLOC(billboardColorTextureSize);
		f->read (p, billboardColorTextureSize);
		_billboardTexture = device->allocTexture (0, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP);
		if (!_billboardTexture->loadTexImageFromFileInMemory (p, billboardColorTextureSize, 0, 0, ATOM_PIXEL_FORMAT_UNKNOWN))
		{
			ATOM_FREE(p);
			return false;
		}
		ATOM_FREE(p);
	}

	unsigned billboardNormalTextureSize;
	f->read (&billboardNormalTextureSize, sizeof(unsigned));
	_billboardNormalTexture = 0;
	if (!ATOM_RenderSettings::isFixedFunction() && billboardNormalTextureSize)
	{
		void *p = ATOM_MALLOC(billboardNormalTextureSize);
		f->read (p, billboardNormalTextureSize);
		_billboardNormalTexture = device->allocTexture (0, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP);
		if (!_billboardNormalTexture->loadTexImageFromFileInMemory (p, billboardNormalTextureSize, 0, 0, ATOM_PIXEL_FORMAT_UNKNOWN))
		{
			ATOM_FREE(p);
			return false;
		}
		ATOM_FREE(p);
	}
	else
	{
		f->seek (billboardNormalTextureSize, ATOM_VFS::current);
	}

	f->read (&_LODdistance, sizeof(float));

	return true;
}

bool ATOM_TreeInstance::rayIntersectionTest (const ATOM_Ray &ray, float *len) const
{
	float d1 = FLT_MAX, d2 = FLT_MAX;
	bool trunkPick = ray.intersectionTestEx (_trunkBBox, d1) && d1 >= 0.f;

	bool leafPick = (_leafCardVertexArray && _leafCardIndexArray) ? (ray.intersectionTestEx (_leafBBox, d2) && d2 >= 0.f) : false;
	if (trunkPick || leafPick)
	{
		if (len)
		{
			*len = ATOM_min2(d1, d2);
		}
		return true;
	}
	return false;
}

const ATOM_TreeInstance::BillboardInfo &ATOM_TreeInstance::getBillboardInfo (int index) const
{
	return _billboardInfo[index];
}

ATOM_Texture *ATOM_TreeInstance::getTrunkTexture (void) const
{
	return _trunkTexture.get();
}

ATOM_Texture *ATOM_TreeInstance::getLeafTexture (void) const
{
	return _leafCardTexture.get();
}

ATOM_Texture *ATOM_TreeInstance::getBillboardTexture (void) const
{
	return _billboardTexture.get();
}

ATOM_Texture *ATOM_TreeInstance::getBillboardNormalMap (void) const
{
	return _billboardNormalTexture.get();
}

ATOM_VertexArray *ATOM_TreeInstance::getTrunkVertexArray (void) const
{
	return _trunkVertexArray.get();
}

ATOM_IndexArray *ATOM_TreeInstance::getTrunkIndexArray (void) const
{
	return _trunkIndexArray.get();
}

ATOM_VertexArray *ATOM_TreeInstance::getLeafCardVertexArray (void) const
{
	return _leafCardVertexArray.get();
}

ATOM_IndexArray *ATOM_TreeInstance::getLeafCardIndexArray (void) const
{
	return _leafCardIndexArray.get();
}

ATOM_VertexArray *ATOM_TreeInstance::getBillboardVertexArray (void) const
{
	return _billboardVertexArray.get();
}

ATOM_IndexArray *ATOM_TreeInstance::getBillboardIndexArray (void) const
{
	return _billboardIndexArray.get();
}

ATOM_TreeBatch *ATOM_TreeInstance::getBatch (void) const
{
	return _batch;
}

const ATOM_BBox &ATOM_TreeInstance::getBoundingbox (void) const
{
	return _boundingBox;
}

float ATOM_TreeInstance::getLODDistance (void) const
{
	return _LODdistance;
}

const ATOM_BBox &ATOM_TreeInstance::getTrunkBoundingbox (void) const
{
	return _trunkBBox;
}

const ATOM_BBox &ATOM_TreeInstance::getLeafBoundingbox (void) const
{
	return _leafBBox;
}

bool ATOM_TreeInstance::hasTrunk (void) const
{
	return _trunkVertexArray && _trunkIndexArray;
}

bool ATOM_TreeInstance::hasLeaves (void) const
{
	return _leafCardVertexArray && _leafCardIndexArray;
}

