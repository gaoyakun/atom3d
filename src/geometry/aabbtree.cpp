#include "aabbtree.h"
#include "ray.h"

ATOM_AABBTree::ATOM_AABBTree (void)
{
	_primitiveType = PT_TRIANGLES;
}

ATOM_AABBTree::~ATOM_AABBTree (void)
{
}

void ATOM_AABBTree::getPrimitive (unsigned index, const unsigned short *indices, PrimitiveType primitiveType, unsigned short *out) const
{
	switch (primitiveType)
	{
	case PT_TRIANGLES:
		out[0] = indices[index * 3];
		out[1] = indices[index * 3 + 1];
		out[2] = indices[index * 3 + 2];
		break;
	case PT_TRIANGLEFAN:
		out[0] = indices[0];
		out[1] = indices[index + 1];
		out[2] = indices[index + 2];
		break;
	case PT_TRIANGLESTRIP:
		out[0] = indices[((index + 1) / 2) * 2];
		out[1] = indices[1 + (index / 2) * 2];
		out[2] = indices[index + 2];
		break;
	case PT_LINES:
		out[0] = indices[index * 2];
		out[1] = indices[index * 2 + 1];
		break;
	case PT_LINESTRIP:
		out[0] = indices[index];
		out[1] = indices[index + 1];
		break;
	case PT_POINTS:
		out[0] = indices[index];
		break;
	}
}

void ATOM_AABBTree::save (void *handle) const
{
	writeFile (handle, &_primitiveType, sizeof(_primitiveType));

	unsigned numNodes = _nodes.size();
	writeFile (handle, &numNodes, sizeof(unsigned));
	if (numNodes)
	{
		writeFile (handle, &_nodes[0], numNodes * sizeof(AABBTreeNode));
	}

	unsigned numVerts = _vertices.size();
	writeFile (handle, &numVerts, sizeof(unsigned));
	if (numVerts)
	{
		writeFile (handle, &_vertices[0], numVerts * sizeof(ATOM_Vector3f));
	}

	unsigned numIndices = _indices.size();
	writeFile (handle, &numIndices, sizeof(unsigned));
	if (numIndices)
	{
		writeFile (handle, &_indices[0], numIndices * sizeof(unsigned short));
	}

	unsigned numPrimList = _leafPrimitiveLists.size();
	writeFile (handle, &numPrimList, sizeof(unsigned));
	if (numPrimList)
	{
		writeFile (handle, &_leafPrimitiveLists[0], numPrimList * sizeof(unsigned));
	}
}

void ATOM_AABBTree::load (void *handle)
{
	readFile (handle, &_primitiveType, sizeof(_primitiveType));

	unsigned numNodes;
	readFile (handle, &numNodes, sizeof(unsigned));
	_nodes.resize(numNodes);
	if (numNodes)
	{
		readFile (handle, &_nodes[0], numNodes * sizeof(AABBTreeNode));
	}

	unsigned numVerts;
	readFile (handle, &numVerts, sizeof(unsigned));
	_vertices.resize (numVerts);
	if (numVerts)
	{
		readFile (handle, &_vertices[0], numVerts * sizeof(ATOM_Vector3f));
	}

	unsigned numIndices;
	readFile (handle, &numIndices, sizeof(unsigned));
	_indices.resize (numIndices);
	if (numIndices)
	{
		readFile (handle, &_indices[0], numIndices* sizeof(unsigned short));
	}

	unsigned numPrimList;
	readFile (handle, &numPrimList, sizeof(unsigned));
	_leafPrimitiveLists.resize (numPrimList);
	if (numPrimList)
	{
		readFile (handle, &_leafPrimitiveLists[0], numPrimList * sizeof(unsigned));
	}
}

unsigned ATOM_AABBTree::getStoreSize (void) const
{
	unsigned size = 0;

	size += sizeof(_primitiveType);

	size += sizeof(unsigned);
	size += _nodes.size() * sizeof(AABBTreeNode);

	size += sizeof(unsigned);
	size += _vertices.size() * sizeof(ATOM_Vector3f);

	size += sizeof(unsigned);
	size += _indices.size() * sizeof(unsigned short);

	size += sizeof(unsigned);
	size += _leafPrimitiveLists.size() * sizeof(unsigned);

	return size;
}

ATOM_AABBTree::PrimitiveType ATOM_AABBTree::getPrimitiveType (void) const
{
	return _primitiveType;
}

const std::vector<ATOM_AABBTree::AABBTreeNode> & ATOM_AABBTree::getNodes (void) const
{
	return _nodes;
}

const std::vector<ATOM_Vector3f> & ATOM_AABBTree::getVertices (void) const
{
	return _vertices;
}

const std::vector<unsigned> & ATOM_AABBTree::getLeafPrimitiveList (void) const
{
	return _leafPrimitiveLists;
}

int ATOM_AABBTree::rayIntersect (const ATOM_Ray &ray, float &d, bool boxTestOnly) const
{
	if (_nodes.empty ())
	{
		return -1;
	}

	d = FLT_MAX;
	int t = -1;
	rayIntersectR (0, ray, d, t, false, boxTestOnly);

	return t;
}

bool ATOM_AABBTree::rayIntersectionTest (const ATOM_Ray &ray, bool boxTestOnly) const
{
	if (_nodes.empty ())
	{
		return false;
	}

	float d = FLT_MAX;
	int t = -1;
	rayIntersectR (0, ray, d, t, true, boxTestOnly);

	return t != -1;
}

void ATOM_AABBTree::rayIntersectR (unsigned node, const ATOM_Ray &ray, float &d, int &t, bool testOnly, bool boxTestOnly, int *indexOut) const
{
	const AABBTreeNode &theNode = _nodes[node];
	float dist = 0.f;

	bool boxTest = testOnly ? ray.intersectionTest (theNode.bbox) : ray.intersectionTestEx (theNode.bbox, dist);
	if (!boxTest)
	{
		return;
	}

	if (theNode.left == -1 && theNode.right == -1)
	{
		if (boxTestOnly)
		{
			if (testOnly)
			{
				t = 0;
				return;
			}
			else if (dist > 0.f && dist < d)
			{
				d = dist;
			}
		}
		else
		{
			for (unsigned i = 0; i < theNode.numPrimitives; ++i)
			{
				unsigned primitive = _leafPrimitiveLists[theNode.primitive + i];
				unsigned short indices[3];
				getPrimitive (primitive, &_indices[0], _primitiveType, indices);
				const ATOM_Vector3f &v0 = _vertices[indices[0]];
				const ATOM_Vector3f &v1 = _vertices[indices[1]];
				const ATOM_Vector3f &v2 = _vertices[indices[2]];
				float u, v;
				if (ray.intersectionTest (v0, v1, v2, false, u, v, dist) && dist > 0.f && dist < d)
				{
					t = primitive;

					if (testOnly)
					{
						return;
					}

					if (indexOut)
					{
						indexOut[0] = indices[0];
						indexOut[1] = indices[1];
						indexOut[2] = indices[2];
					}
					d = dist;
				}
			}
		}
	}
	else
	{
		if (theNode.left != -1)
		{
			rayIntersectR (theNode.left, ray, d, t, testOnly, boxTestOnly, indexOut);

			if (testOnly && t != -1)
			{
				return;
			}
		}

		if (theNode.right != -1)
		{
			rayIntersectR (theNode.right, ray, d, t, testOnly, boxTestOnly, indexOut);

			if (testOnly && t != -1)
			{
				return;
			}
		}
	}
}

unsigned ATOM_AABBTree::writeFile (void *handle, const void *buffer, unsigned size) const
{
	return 0;
}

unsigned ATOM_AABBTree::readFile (void *handle, void *buffer, unsigned size) const
{
	return 0;
}

