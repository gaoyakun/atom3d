#include "aabbtreebuilder.h"

ATOM_AABBTreeBuilder::ATOM_AABBTreeBuilder (void)
{
}

ATOM_AABBTreeBuilder::~ATOM_AABBTreeBuilder (void)
{
	reset ();
}

void ATOM_AABBTreeBuilder::reset (void)
{
	_vertices.resize (0);
	_indices.resize (0);
	_leafPrimitiveLists.resize (0);
	_extractedIndices.resize (0);
	for (unsigned i = 0; i < _items.size(); ++i)
	{
		delete _items[i];
	}
	_items.resize (0);
}

bool ATOM_AABBTreeBuilder::buildFromPrimitives (ATOM_AABBTree *tree, ATOM_AABBTree::PrimitiveType prim, const ATOM_Vector3f *verts, unsigned numVerts, const unsigned short *indices, unsigned numPrimitives, unsigned maxLeafPrimitiveCount, int maxDepth)
{
	if (!numPrimitives)
	{
		return false;
	}

	reset ();

	_vertices.resize (numVerts);
	memcpy (&_vertices[0], verts, sizeof(ATOM_Vector3f) * numVerts);

	unsigned numIndices = calcIndexCount (prim, numPrimitives);
	_indices.resize (numIndices);
	memcpy (&_indices[0], indices, sizeof(unsigned short) * numIndices);

	_extractedIndices.resize (numPrimitives * 3);

	WorkingItem *item = new WorkingItem;
	item->primitives.resize (numPrimitives);
	item->depth = 0;
	_items.push_back (item);

	for (unsigned i = 0; i < numPrimitives; ++i)
	{
		item->primitives[i] = i;
		getPrimitive (i, indices, prim, &_extractedIndices[i*3]);
	}

	item->left = -1;
	item->right = -1;

	int off = 0;
	for (;;)
	{
		workOnItem (prim, *_items[off++], maxLeafPrimitiveCount, maxDepth - 1);
		if (off == _items.size())
		{
			break;
		}
	}

	return buildFinalTree (tree, prim);
}

bool ATOM_AABBTreeBuilder::buildFinalTree (ATOM_AABBTree *tree, ATOM_AABBTree::PrimitiveType prim)
{
	if (!tree)
	{
		return false;
	}

	tree->_nodes.resize (_items.size ());
	for (unsigned i = 0; i < tree->_nodes.size(); ++i)
	{
		ATOM_AABBTree::AABBTreeNode &node = tree->_nodes[i];
		WorkingItem *item = _items[i];

		node.bbox = item->aabb;
		node.left = item->left;
		node.right = item->right;
		node.primitive = item->prim;
		node.numPrimitives = item->numPrims;
	}

	tree->_vertices = _vertices;
	tree->_indices = _indices;
	tree->_leafPrimitiveLists = _leafPrimitiveLists;
	tree->_primitiveType = prim;

	return true;
}

void ATOM_AABBTreeBuilder::workOnItem (ATOM_AABBTree::PrimitiveType prim, WorkingItem &item, unsigned maxLeafPrimitiveCount, int maxDepth)
{
	unsigned numPrimitives = item.primitives.size();

	item.aabb.beginExtend ();

	for (unsigned i = 0; i < numPrimitives; ++i)
	{
		unsigned primitive = item.primitives[i];
		const unsigned short *idx = &_extractedIndices[primitive*3];
		item.aabb.extend (_vertices[idx[0]]);
		item.aabb.extend (_vertices[idx[1]]);
		item.aabb.extend (_vertices[idx[2]]);
	}

	item.prim = 0;
	item.numPrims = numPrimitives;
	if (numPrimitives <= maxLeafPrimitiveCount || (maxDepth >= 0 && item.depth > maxDepth))
	{
		item.left = -1;
		item.right = -1;
		item.prim = allocPrimitiveList (numPrimitives);

		for (unsigned i = 0; i < numPrimitives; ++i)
		{
			_leafPrimitiveLists[item.prim + i] = item.primitives[i];
		}
		return;
	}

	const ATOM_Vector3f &bboxMin = item.aabb.getMin();
	const ATOM_Vector3f &bboxMax = item.aabb.getMax();
	ATOM_Vector3f bboxCenter = item.aabb.getCenter();

	ATOM_Vector3f extents = item.aabb.getExtents ();
	int axis = 0;
	if (extents[1] > extents[0]) axis = 1;
	if (extents[2] > extents[axis]) axis = 2;

	float middle = calcAvgPoint (item, axis);
	WorkingItem *left = 0;
	WorkingItem *right = 0;

	for (unsigned i = 0; i < numPrimitives; ++i)
	{
		unsigned primitive = item.primitives[i];
		float minval, maxval;
		int result = classifyPrimitive (primitive, axis, middle, minval, maxval);
		if (result == 0)
		{
			if (!left)
			{
				left = new WorkingItem;
				left->depth = item.depth + 1;
				left->aabb.setMin (bboxMin);
				if (axis == 0)
				{
					left->aabb.setMax (ATOM_Vector3f((maxval > bboxCenter.x) ? maxval : bboxCenter.x, bboxMax.y, bboxMax.z));
				}
				else if (axis == 1)
				{
					left->aabb.setMax (ATOM_Vector3f(bboxMin.x, (maxval > bboxCenter.y) ? maxval : bboxCenter.y, bboxMax.z));
				}
				else
				{
					left->aabb.setMax (ATOM_Vector3f(bboxMin.x, bboxMin.y, (maxval > bboxCenter.z) ? maxval : bboxCenter.z));
				}
			}
			left->primitives.push_back (primitive);
		}
		else
		{
			if (!right)
			{
				right = new WorkingItem;
				right->depth = item.depth + 1;
				right->aabb.setMax (bboxMax);
				if (axis == 0)
				{
					right->aabb.setMin (ATOM_Vector3f((minval < bboxCenter.x) ? minval : bboxCenter.x, bboxMin.y, bboxMin.z));
				}
				else if (axis == 1)
				{
					right->aabb.setMin (ATOM_Vector3f(bboxMin.x, (minval < bboxCenter.y) ? minval : bboxCenter.y, bboxMin.z));
				}
				else
				{
					right->aabb.setMin (ATOM_Vector3f(bboxMin.x, bboxMin.y, (minval < bboxCenter.z) ? minval : bboxCenter.z));
				}
			}
			right->primitives.push_back (primitive);
		}
	}

	if (left && !right)
	{
		assert (left->primitives.size() == numPrimitives);
		right = new WorkingItem;
		right->depth = item.depth + 1;
		right->aabb = left->aabb;
		unsigned le = numPrimitives / 2;
		unsigned start = numPrimitives - le;
		right->primitives.resize(le);
		for (unsigned n = 0; n < le; ++n)
		{
			right->primitives[n] = left->primitives[start+n];
		}
		left->primitives.resize (start);
	}
	else if (right && !left)
	{
		assert (right->primitives.size() == numPrimitives);
		left = new WorkingItem;
		left->depth = item.depth + 1;
		left->aabb = right->aabb;
		unsigned le = numPrimitives / 2;
		unsigned start = numPrimitives - le;
		left->primitives.resize (le);
		for (unsigned n = 0; n < le; ++n)
		{
			left->primitives[n] = right->primitives[start+n];
		}
		right->primitives.resize (start);
	}

	if (left)
	{
		item.left = _items.size();
		_items.push_back (left);
	}
	else
	{
		item.left = -1;
	}

	if (right)
	{
		item.right = _items.size();
		_items.push_back (right);
	}
	else
	{
		item.right = -1;
	}
}

int ATOM_AABBTreeBuilder::classifyPrimitive (unsigned primitive, int axis, float middle, float &minval, float &maxval)
{
	takeMinMax (primitive, axis, minval, maxval);
	
	float m = 0.f;
	const unsigned short *idx = &_extractedIndices[primitive * 3];
	//const ATOM_Vector3f &v0 = _vertices[idx[0]];
	//const ATOM_Vector3f &v1 = _vertices[idx[1]];
	//const ATOM_Vector3f &v2 = _vertices[idx[2]];

	m += _vertices[idx[0]][axis];
	m += _vertices[idx[1]][axis];
	m += _vertices[idx[2]][axis];
	m /= 3.f;

	return (m < middle) ? 0 : 1;
}

void ATOM_AABBTreeBuilder::takeMinMax (unsigned primitive, int axis, float &minval, float &maxval)
{
	minval = FLT_MAX;
	maxval = -FLT_MAX;

	const unsigned short *idx = &_extractedIndices[primitive * 3];
	const ATOM_Vector3f &v0 = _vertices[idx[0]];
	const ATOM_Vector3f &v1 = _vertices[idx[1]];
	const ATOM_Vector3f &v2 = _vertices[idx[2]];

	if (v0[axis] < minval) minval = v0[axis];
	if (v0[axis] > maxval) maxval = v0[axis];
	if (v1[axis] < minval) minval = v1[axis];
	if (v1[axis] > maxval) maxval = v1[axis];
	if (v2[axis] < minval) minval = v2[axis];
	if (v2[axis] > maxval) maxval = v2[axis];
}

float ATOM_AABBTreeBuilder::calcAvgPoint (WorkingItem &item, int axis)
{
	float m = 0.f;

	unsigned numPrimitives = item.primitives.size();
	for (unsigned i = 0; i < numPrimitives; ++i)
	{
		unsigned primitive = item.primitives[i];
		const unsigned short *idx = &_extractedIndices[primitive * 3];
		m += _vertices[idx[0]][axis];
		m += _vertices[idx[1]][axis];
		m += _vertices[idx[2]][axis];
	}

	return m / (3 * numPrimitives);
}

unsigned ATOM_AABBTreeBuilder::allocPrimitiveList (unsigned numPrimitives)
{
	_leafPrimitiveLists.resize (_leafPrimitiveLists.size() + numPrimitives);
	return _leafPrimitiveLists.size() - numPrimitives;
}

unsigned ATOM_AABBTreeBuilder::calcIndexCount (ATOM_AABBTree::PrimitiveType primitiveType, unsigned numPrimitives) const
{
	switch (primitiveType)
	{
	case ATOM_AABBTree::PT_TRIANGLES:
		return numPrimitives * 3;
		break;
	case ATOM_AABBTree::PT_TRIANGLEFAN:
	case ATOM_AABBTree::PT_TRIANGLESTRIP:
		return numPrimitives + 2;
		break;
	default:
		assert(0);
		return 0;
	}
}

void ATOM_AABBTreeBuilder::getPrimitive (unsigned index, const unsigned short *indices, ATOM_AABBTree::PrimitiveType primitiveType, unsigned short *out) const
{
	switch (primitiveType)
	{
	case ATOM_AABBTree::PT_TRIANGLES:
		out[0] = indices[index * 3];
		out[1] = indices[index * 3 + 1];
		out[2] = indices[index * 3 + 2];
		break;
	case ATOM_AABBTree::PT_TRIANGLEFAN:
		out[0] = indices[0];
		out[1] = indices[index + 1];
		out[2] = indices[index + 2];
		break;
	case ATOM_AABBTree::PT_TRIANGLESTRIP:
		out[0] = indices[((index + 1) / 2) * 2];
		out[1] = indices[1 + (index / 2) * 2];
		out[2] = indices[index + 2];
		break;
	default:
		assert(0);
		break;
	}
}

