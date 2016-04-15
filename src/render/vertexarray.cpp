#include "stdafx.h"

// inline member functions
ATOM_VertexArray::ATOM_VertexArray(void) 
{
	_M_attributes = 0;
	_M_vertex_size = 0;
    _M_num_verts = 0;
	_M_capacity = 0;
    _M_usage = ATOM_USAGE_STATIC;
	_M_content_lost = false;
	_preservedContent = 0;

	//--- wangjian added for test float16 ---//
	_attribFlags = 0;
	//---------------------------------------//
}

bool ATOM_VertexArray::isContentLost (void) const
{
	return _M_content_lost;
}

void ATOM_VertexArray::setContentLost (bool b)
{
	_M_content_lost = b;
}

unsigned ATOM_VertexArray::getAttributes(void) const 
{
	return _M_attributes;
}

int ATOM_VertexArray::getUsage (void) const 
{
	return _M_usage;
}

unsigned ATOM_VertexArray::getVertexStride(void) const 
{
	return _M_vertex_size;
}

unsigned ATOM_VertexArray::getNumVertices (void) const 
{
	return _M_num_verts;
}

//--- wangjian added for test float16 ---//
unsigned ATOM_VertexArray::getAttribFlags() const
{
	return _attribFlags;
}
//---------------------------------------//

