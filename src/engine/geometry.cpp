#include "StdAfx.h"

#define MAX_BATCH_COUNT 500

void ATOM_Geometry::setIndices (ATOM_IndexArray *indexArray)
{
	ATOM_STACK_TRACE(ATOM_Geometry::setIndices);

	_indices = indexArray;
}

ATOM_IndexArray *ATOM_Geometry::getIndices (void) const
{
	ATOM_STACK_TRACE(ATOM_Geometry::getIndices);

	return _indices.get();
}

ATOM_MultiStreamGeometry::ATOM_MultiStreamGeometry (void)
{
	ATOM_STACK_TRACE(ATOM_MultiStreamGeometry::ATOM_MultiStreamGeometry);

	_vertexDecl = 0;
	_numVerts = 0;

	for (unsigned i = 0; i < 6 + ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
	{
		_streams[i].attrib = 0;
	}
}

ATOM_MultiStreamGeometry::~ATOM_MultiStreamGeometry (void)
{
	ATOM_STACK_TRACE(ATOM_MultiStreamGeometry::~ATOM_MultiStreamGeometry);

	if (_vertexDecl)
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice();
		if (device)
		{
			device->destroyVertexDecl (_vertexDecl);
		}
	}
}

ATOM_Geometry *ATOM_MultiStreamGeometry::clone (void) const
{
	ATOM_MultiStreamGeometry *p = ATOM_NEW(ATOM_MultiStreamGeometry);
	for (unsigned i = 0; i < 6 + ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
	{
		p->_streams[i] = _streams[i];
	}
	p->_numVerts = _numVerts;
	p->_vertexDecl = 0;
	p->_indices = this->_indices;

	return p;
}

ATOM_VertexArray *ATOM_MultiStreamGeometry::getStream (int attrib) const
{
	int streamIndex = -1;

	switch (attrib)
	{
	case ATOM_VERTEX_ATTRIB_COORD:
		{
			streamIndex = 0;
			break;
		}
	case ATOM_VERTEX_ATTRIB_NORMAL:
		{
			streamIndex = 1;
			break;
		}
	case ATOM_VERTEX_ATTRIB_PSIZE:
		{
			streamIndex = 2;
			break;
		}
	case ATOM_VERTEX_ATTRIB_PRIMARY_COLOR:
		{
			streamIndex = 3;
			break;
		}
	case ATOM_VERTEX_ATTRIB_TANGENT:
		{
			streamIndex = 4;
			break;
		}
	case ATOM_VERTEX_ATTRIB_BINORMAL:
		{
			streamIndex = 5;
			break;
		}
	default:
		{
			for (unsigned i = 0; i < ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
			{
				if (attrib & ATOM_VERTEX_ATTRIB_TEX_MASK(i))
				{
					streamIndex = 6 + i;
					break;
				}
			}
			break;
		}
	}

	return (streamIndex == -1) ? 0 : _streams[streamIndex].stream.get();
}

void ATOM_MultiStreamGeometry::addStream (ATOM_VertexArray *vertexArray, unsigned offset, unsigned attrib)
{
	ATOM_STACK_TRACE(ATOM_MultiStreamGeometry::addStream);

	if (vertexArray)
	{
		unsigned att = attrib ? attrib : vertexArray->getAttributes();
		unsigned streamIndex = 0;
		if (att & ATOM_VERTEX_ATTRIB_COORD_MASK)
		{
			streamIndex = 0;
			_numVerts = vertexArray->getNumVertices();
		}
		else if (att & ATOM_VERTEX_ATTRIB_NORMAL)
		{
			streamIndex = 1;
		}
		else if (att & ATOM_VERTEX_ATTRIB_PSIZE)
		{
			streamIndex = 2;
		}
		else if (att & ATOM_VERTEX_ATTRIB_PRIMARY_COLOR)
		{
			streamIndex = 3;
		}
		else if (att & ATOM_VERTEX_ATTRIB_TANGENT)
		{
			streamIndex = 4;
		}
		else if (att & ATOM_VERTEX_ATTRIB_BINORMAL)
		{
			streamIndex = 5;
		}
		else
		{
			for (unsigned i = 0; i < ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
			{
				if (att & ATOM_VERTEX_ATTRIB_TEX_MASK(i))
				{
					streamIndex = 6 + i;
				}
			}
		}

		_streams[streamIndex].stream = vertexArray;
		_streams[streamIndex].offset = offset;

		if (att != _streams[streamIndex].attrib)
		{
			_streams[streamIndex].attrib = att;

			if (_vertexDecl)
			{
				ATOM_GetRenderDevice()->destroyVertexDecl (_vertexDecl);
				_vertexDecl = 0;
			}
		}
	}
}

void ATOM_MultiStreamGeometry::clear (void)
{
	ATOM_STACK_TRACE(ATOM_MultiStreamGeometry::clear);

	for (unsigned i = 0; i < 6 + ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
	{
		_streams[i].attrib = 0;
		_streams[i].stream = 0;
	}

	if (_vertexDecl)
	{
		ATOM_GetRenderDevice()->destroyVertexDecl (_vertexDecl);
		_vertexDecl = 0;
	}

	_numVerts = 0;
}

void ATOM_MultiStreamGeometry::removeStream (ATOM_VertexArray *vertexArray)
{
	ATOM_STACK_TRACE(ATOM_MultiStreamGeometry::removeStream);

	if (vertexArray)
	{
		for (unsigned i = 0; i < 6 + ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
		{
			if (_streams[i].stream.get() == vertexArray)
			{
				if (_streams[i].attrib & ATOM_VERTEX_ATTRIB_COORD_MASK)
				{
					_numVerts = 0;
				}

				_streams[i].stream = 0;
				_streams[i].attrib = 0;
				_streams[i].offset = 0;

				if (_vertexDecl)
				{
					ATOM_GetRenderDevice()->destroyVertexDecl (_vertexDecl);
					_vertexDecl = 0;
				}

				break;
			}
		}
	}
}

bool ATOM_MultiStreamGeometry::createVertexDecl (void)
{
	ATOM_STACK_TRACE(ATOM_MultiStreamGeometry::createVertexDecl);

	if (!_vertexDecl)
	{
		unsigned attrib = 0;

		// wangjian modified for test float16
		unsigned attributeflags = 0;

		for (unsigned i = 0; i < 6 + ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
		{
			attrib |= _streams[i].attrib;
			if( _streams[i].stream )
				attributeflags |= _streams[i].stream->getAttribFlags();
		}

		_vertexDecl = attrib ? ATOM_GetRenderDevice()->createMultiStreamVertexDeclaration (attrib, attributeflags) : 0;
	}

	return _vertexDecl != 0;
}

bool ATOM_MultiStreamGeometry::draw (ATOM_RenderDevice *device, int type, int prim_count, int offset)
{
	ATOM_STACK_TRACE(ATOM_MultiStreamGeometry::draw);

	if (_numVerts)
	{
		if (!_vertexDecl)
		{
			createVertexDecl ();
		}

		if (_vertexDecl)
		{
			device->setVertexDecl (_vertexDecl);
			unsigned stream = 0;

			for (unsigned i = 0; i < 6 + ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
			{
				if (_streams[i].stream)
				{
					device->setStreamSource (stream++, _streams[i].stream.get(), _streams[i].offset);
				}
			}

			if (_indices)
			{
				device->renderStreamsIndexed(_indices.get(), type, prim_count, offset);
			}
			else
			{
				device->renderStreams (type, prim_count);
			}
		}

		return true;
	}

	return false;
}

ATOM_GeometryType ATOM_MultiStreamGeometry::getType (void) const
{
	return GT_MULTISTREAM;
}

ATOM_InterleavedStreamGeometry::ATOM_InterleavedStreamGeometry (void)
{
	ATOM_STACK_TRACE(ATOM_InterleavedStreamGeometry::ATOM_InterleavedStreamGeometry);

	_vertexDecl = 0;
	_attrib = 0;

	// wangjian added 
	_attrib_flag = 0;
}

ATOM_InterleavedStreamGeometry::~ATOM_InterleavedStreamGeometry (void)
{
	ATOM_STACK_TRACE(ATOM_InterleavedStreamGeometry::~ATOM_InterleavedStreamGeometry);

	if (_vertexDecl)
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice();
		if (device)
		{
			device->destroyVertexDecl (_vertexDecl);
		}
	}
}

ATOM_Geometry *ATOM_InterleavedStreamGeometry::clone (void) const
{
	ATOM_InterleavedStreamGeometry *p = ATOM_NEW(ATOM_InterleavedStreamGeometry);
	p->_stream = _stream;
	p->_attrib = _attrib;
	p->_vertexDecl = 0;
	//--- wangjian added ---//
	p->_indices = this->_indices;
	p->_attrib_flag = _attrib_flag;
	//----------------------//

	return p;
}

// wangjian modified
void ATOM_InterleavedStreamGeometry::setStream (ATOM_VertexArray *vertexArray, unsigned attrib/* = 0*/, unsigned attribFlag/* = 0*/)
{
	ATOM_STACK_TRACE(ATOM_InterleavedStreamGeometry::setStream);

	_stream = vertexArray;

	if (_stream)
	{
		unsigned newAttrib		= attrib ? attrib : _stream->getAttributes();
		unsigned newAttribFlag	= attribFlag ? attribFlag : _stream->getAttribFlags();		// wangjian added

		// wangjian modified 
		if ( ( newAttrib != _attrib || newAttribFlag != _attrib_flag ) && _vertexDecl)
		{
			ATOM_GetRenderDevice()->destroyVertexDecl (_vertexDecl);
			_vertexDecl = 0;
		}

		_attrib			= newAttrib;
		_attrib_flag	= newAttribFlag;		// wangjian added
	}
}

ATOM_VertexArray *ATOM_InterleavedStreamGeometry::getStream (void) const
{
	return _stream.get();
}

unsigned ATOM_InterleavedStreamGeometry::getStreamAttrib (void) const
{
	return _attrib;
}

bool ATOM_InterleavedStreamGeometry::createVertexDecl (void)
{
	ATOM_STACK_TRACE(ATOM_InterleavedStreamGeometry::createVertexDecl);

	if (!_vertexDecl && _stream)
	{
		_vertexDecl = ATOM_GetRenderDevice()->createInterleavedVertexDeclaration (_attrib, 0, _attrib_flag);	// wangjian modified
	}

	return _vertexDecl != 0;
}

bool ATOM_InterleavedStreamGeometry::draw (ATOM_RenderDevice *device, int type, int prim_count, int offset)
{
	ATOM_STACK_TRACE(ATOM_InterleavedStreamGeometry::draw);

	if (_stream)
	{
		if (!_vertexDecl)
		{
			createVertexDecl ();
		}

		if (_vertexDecl)
		{
			device->setVertexDecl (_vertexDecl);
			device->setStreamSource (0, _stream.get());

			if (_indices)
			{
				device->renderStreamsIndexed(_indices.get(), type, prim_count, offset);
			}
			else
			{
				device->renderStreams (type, prim_count);
			}
		}

		return true;
	}

	return false;
}

ATOM_GeometryType ATOM_InterleavedStreamGeometry::getType (void) const
{
	return GT_INTERLEAVEDSTREAM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_AUTOREF(ATOM_VertexArray)	ATOM_HWInstancingGeometry::_instanceData_staticbuffer = 0;

ATOM_HWInstancingGeometry::ATOM_HWInstancingGeometry (void)
{
	ATOM_STACK_TRACE(ATOM_HWInstancingGeometry::ATOM_HWInstancingGeometry);

	_numInstances = 0;
	_instanceDataStreamAttrib = 0;
	_instanceDataSize = 0;
	_lockedStreamIndex = -1;
	_lockPtr = 0;
	_enableInstancing = false;
	_instancedVertexDecl = 0;
}

ATOM_HWInstancingGeometry::~ATOM_HWInstancingGeometry (void)
{
	ATOM_STACK_TRACE(ATOM_HWInstancingGeometry::~ATOM_HWInstancingGeometry);

	if (_lockPtr)
	{
		_instanceDataStreams.back()->unlock ();
		_lockPtr = 0;
	}

	if (_instancedVertexDecl)
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice();
		if (device)
		{
			device->destroyVertexDecl (_instancedVertexDecl);
		}
	}
}

ATOM_Geometry *ATOM_HWInstancingGeometry::clone (void) const
{
	ATOM_HWInstancingGeometry *p = ATOM_NEW(ATOM_HWInstancingGeometry);
	p->_stream = _stream;
	p->_attrib = _attrib;
	p->_vertexDecl = 0;
	//--- wangjian added ---//
	p->_indices = this->_indices;
	//----------------------//
	p->setInstanceDataAttributes (_instanceDataStreamAttrib);

	return p;
}

void ATOM_HWInstancingGeometry::setInstanceDataAttributes (unsigned attrib)
{
	ATOM_STACK_TRACE(ATOM_HWInstancingGeometry::setInstanceDataAttributes);

	if (attrib != _instanceDataStreamAttrib)
	{
		clearInstances ();

		if (_instancedVertexDecl)
		{
			ATOM_GetRenderDevice()->destroyVertexDecl (_instancedVertexDecl);
			_instancedVertexDecl = 0;
		}
		_instanceDataStreamAttrib = attrib;
		_instanceDataSize = ATOM_GetVertexSize (attrib);
	}
}

void *ATOM_HWInstancingGeometry::addInstance (void)
{
	ATOM_STACK_TRACE(ATOM_HWInstancingGeometry::addInstance);

	//--- wangjian added ---//
	if( ATOM_RenderSettings::isUseInstancingStaticBuffer() )
		return 0;
	//----------------------//

	if (_instanceDataStreamAttrib == 0)
	{
		return 0;
	}

	int streamIndex = _numInstances / MAX_BATCH_COUNT;
	if (streamIndex != _lockedStreamIndex)
	{
		if (_lockedStreamIndex >= 0)
		{
			_instanceDataStreams[_lockedStreamIndex]->unlock ();
			_lockPtr = 0;
		}

		if (streamIndex >= _instanceDataStreams.size())
		{
			_instanceDataStreams.resize (streamIndex+1);
		}

		if (!_instanceDataStreams[streamIndex])
		{
			_instanceDataStreams[streamIndex] = ATOM_GetRenderDevice()->allocVertexArray (_instanceDataStreamAttrib, ATOM_USAGE_DYNAMIC, MAX_BATCH_COUNT);
			if (!_instanceDataStreams[streamIndex])
			{
				return 0;
			}
		}

		_lockPtr = _instanceDataStreams[streamIndex]->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		if (!_lockPtr)
		{
			return 0;
		}

		_lockedStreamIndex = streamIndex;
	}

	ATOM_ASSERT(_lockPtr);
	char *data = (char*)_lockPtr;

	_lockPtr = data + _instanceDataSize;
	_numInstances++;

	return data;
}

//--- wangjian added ---//
void * ATOM_HWInstancingGeometry::addInstance (unsigned numInstance)
{
	ATOM_STACK_TRACE(ATOM_HWInstancingGeometry::addInstance);

	//--- wangjian added ---//
	if( ATOM_RenderSettings::isUseInstancingStaticBuffer() )
		return 0;
	//----------------------//

	if (_instanceDataStreamAttrib == 0)
	{
		return 0;
	}

	if (numInstance <= 0)
	{
		return 0;
	}

	int streamIndex = _numInstances / MAX_BATCH_COUNT;
	if (streamIndex != _lockedStreamIndex)
	{
		if (_lockedStreamIndex >= 0)
		{
			_instanceDataStreams[_lockedStreamIndex]->unlock ();
			_lockPtr = 0;
		}

		if (streamIndex >= _instanceDataStreams.size())
		{
			_instanceDataStreams.resize (streamIndex+1);
		}

		if (!_instanceDataStreams[streamIndex])
		{
			_instanceDataStreams[streamIndex] = ATOM_GetRenderDevice()->allocVertexArray (	_instanceDataStreamAttrib, 
																							ATOM_USAGE_DYNAMIC, 
																							MAX_BATCH_COUNT	);
			if (!_instanceDataStreams[streamIndex])
			{
				return 0;
			}
		}

		_lockPtr = _instanceDataStreams[streamIndex]->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		if (!_lockPtr)
		{
			return 0;
		}

		_lockedStreamIndex = streamIndex;
	}

	ATOM_ASSERT(_lockPtr);
	char *data = (char*)_lockPtr;

	_lockPtr = data + _instanceDataSize * numInstance;
	_numInstances+=numInstance;

	return data;
}

void ATOM_HWInstancingGeometry::setInstanceCount(unsigned numInstance)
{
	_numInstances = numInstance;
}
//----------------------//

unsigned ATOM_HWInstancingGeometry::getNumInstances (void) const
{
	return _numInstances;
}

void ATOM_HWInstancingGeometry::clearInstances (void)
{
	ATOM_STACK_TRACE(ATOM_HWInstancingGeometry::clearInstances);

	if (_lockPtr)
	{
		_instanceDataStreams[_lockedStreamIndex]->unlock ();
		_lockedStreamIndex = -1;
		_lockPtr = 0;
	}

	_numInstances = 0;
}

bool ATOM_HWInstancingGeometry::draw (ATOM_RenderDevice *device, int type, int prim_count, int offset)
{
	ATOM_STACK_TRACE(ATOM_HWInstancingGeometry::draw);

	if (!_enableInstancing)
	{
		return ATOM_InterleavedStreamGeometry::draw (device, type, prim_count, offset);
	}

	if (_lockPtr)
	{
		_instanceDataStreams[_lockedStreamIndex]->unlock ();
		_lockedStreamIndex = -1;
		_lockPtr = 0;
	}

	if (_numInstances && _stream && _indices)
	{
		if (!_instancedVertexDecl)
		{
			createVertexDecl ();
		}

		if (_instancedVertexDecl)
		{
			device->setStreamSource (0, _stream.get());

			//--- wangjian modified ---//
#if 0
			if (_numInstances && _instanceDataStreams.size() > 0)
			{
				unsigned rem = _numInstances;
				unsigned stream = 0;
				device->setVertexDecl (_instancedVertexDecl);
				while (rem)
				{
					if (_instanceDataStreams[stream])
					{
						unsigned count = (rem > MAX_BATCH_COUNT) ? MAX_BATCH_COUNT : rem;
						device->setStreamSourceFreq (0, ATOM_STREAMSOURCE_INDEXEDDATA|count);
						device->setStreamSource (1, _instanceDataStreams[stream].get());
						device->setStreamSourceFreq (1, ATOM_STREAMSOURCE_INSTANCEDATA|1);
						device->renderStreamsIndexed(_indices.get(), type, prim_count, offset);
						rem -= count;
					}
					++stream;
				}
				device->setStreamSourceFreq (0, 1);
				device->setStreamSourceFreq (1, 1);
			}
#else

			// use static instance buffer
			if( ATOM_RenderSettings::isUseInstancingStaticBuffer() )
			{
				if( !_instanceData_staticbuffer )
					createStaticInstanceBuffer();

				if( _instanceData_staticbuffer )
				{
					device->setVertexDecl (_instancedVertexDecl);
					ATOM_ASSERT( _numInstances <= ATOM_RenderSettings::getInstancingSBCountThreshold() );
					device->setStreamSourceFreq (0, ATOM_STREAMSOURCE_INDEXEDDATA|_numInstances);
					device->setStreamSource (1, _instanceData_staticbuffer.get());
					device->setStreamSourceFreq (1, ATOM_STREAMSOURCE_INSTANCEDATA|1);
					device->renderStreamsIndexed(_indices.get(), type, prim_count, offset);
				}
			}
			else
			{
				if (_instanceDataStreams.size() > 0)
				{
					unsigned rem = _numInstances;
					unsigned stream = 0;
					device->setVertexDecl (_instancedVertexDecl);
					while (rem)
					{
						if (_instanceDataStreams[stream])
						{
							unsigned count = (rem > MAX_BATCH_COUNT) ? MAX_BATCH_COUNT : rem;
							device->setStreamSourceFreq (0, ATOM_STREAMSOURCE_INDEXEDDATA|count);
							device->setStreamSource (1, _instanceDataStreams[stream].get());
							device->setStreamSourceFreq (1, ATOM_STREAMSOURCE_INSTANCEDATA|1);
							device->renderStreamsIndexed(_indices.get(), type, prim_count, offset);
							rem -= count;
						}
						++stream;
					}
						
				}
			}

			device->setStreamSourceFreq (0, 1);
			device->setStreamSourceFreq (1, 1);
			device->setStreamSource (0, 0);
			device->setStreamSource (1, 0);
#endif
			//---------------------------------------------------------------------//

		}

		_numInstances = 0;

		return true;
	}

	return false;
}

bool ATOM_HWInstancingGeometry::createVertexDecl (void)
{
	ATOM_STACK_TRACE(ATOM_HWInstancingGeometry::createVertexDecl);

	if (!ATOM_InterleavedStreamGeometry::createVertexDecl ())
	{
		return false;
	}

	if (!_instancedVertexDecl && _stream)
	{
		_instancedVertexDecl = ATOM_GetRenderDevice()->createInterleavedVertexDeclaration (_attrib, _instanceDataStreamAttrib, _attrib_flag);
	}

	return _instancedVertexDecl != 0;
}

ATOM_GeometryType ATOM_HWInstancingGeometry::getType (void) const
{
	return GT_HWINSTANCING;
}

void ATOM_HWInstancingGeometry::enableInstancing (bool b)
{
	_enableInstancing = b;
}

bool ATOM_HWInstancingGeometry::instancingEnabled (void) const
{
	return _enableInstancing;
}

bool ATOM_HWInstancingGeometry::createStaticInstanceBuffer()
{
	if( !_instanceData_staticbuffer )
	{
		_instanceData_staticbuffer = ATOM_GetRenderDevice()->allocVertexArray(	_instanceDataStreamAttrib, 
																				ATOM_USAGE_STATIC, 
																				ATOM_RenderSettings::getInstancingSBCountThreshold(), 
																				true	);
		if (!_instanceData_staticbuffer)
		{
			return false;
		}
		float *p = (float*)_instanceData_staticbuffer->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		if (!p)
		{
			_instanceData_staticbuffer = 0;
			return false;
		}
		for (unsigned i = 0; i < _instanceData_staticbuffer->getNumVertices(); ++i)
		{
			p[i] = float(i);
		}
		_instanceData_staticbuffer->unlock ();
	}
	
	return true;
}
