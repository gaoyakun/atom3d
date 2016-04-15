#include "vertexarray.h"

// inline member functions
ATOM_VertexArray::ATOM_VertexArray(void) 
{
	_attributes = 0;
	_vertexSize = 0;
    _numVertices = 0;
	_capacity = 0;
    _usage = Static;
	_contentLost = false;
}

bool ATOM_VertexArray::isContentLost (void) const
{
	return _contentLost;
}

void ATOM_VertexArray::setContentLost (bool b)
{
	_contentLost = b;
}

unsigned ATOM_VertexArray::getAttributes(void) const 
{
	return _attributes;
}

ATOM_VertexArray::Usage ATOM_VertexArray::getUsage (void) const 
{
	return _usage;
}

unsigned ATOM_VertexArray::getVertexStride(void) const 
{
	return _vertexSize;
}

unsigned ATOM_VertexArray::getNumVertices (void) const 
{
	return _numVertices;
}

