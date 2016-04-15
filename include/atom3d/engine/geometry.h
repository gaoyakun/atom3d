#ifndef __ATOM3D_ENGINE_GEOMETRY_H
#define __ATOM3D_ENGINE_GEOMETRY_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_kernel.h"
#include "../ATOM_render.h"
#include "basedefs.h"

enum ATOM_GeometryType
{
	GT_MULTISTREAM,
	GT_INTERLEAVEDSTREAM,
	GT_HWINSTANCING
};

class ATOM_ENGINE_API ATOM_Geometry
{
public:
	virtual ~ATOM_Geometry (void) {}
	virtual ATOM_Geometry *clone (void) const = 0;

public:
	void setIndices (ATOM_IndexArray *indexArray);
	ATOM_IndexArray *getIndices (void) const;

public:
	virtual bool draw (ATOM_RenderDevice *device, int type, int prim_count, int offset) = 0;
	virtual bool createVertexDecl (void) = 0;
	virtual ATOM_GeometryType getType (void) const = 0;

protected:
	ATOM_AUTOREF(ATOM_IndexArray) _indices;
};

class ATOM_ENGINE_API ATOM_MultiStreamGeometry: public ATOM_Geometry
{
	struct StreamInfo
	{
		ATOM_AUTOREF(ATOM_VertexArray) stream;
		unsigned attrib;
		unsigned offset;
	};

public:
	ATOM_MultiStreamGeometry (void);
	virtual ~ATOM_MultiStreamGeometry (void);
	virtual ATOM_Geometry *clone (void) const;

public:
	void addStream (ATOM_VertexArray *vertexArray, unsigned offset = 0, unsigned attrib = 0);
	void clear (void);
	void removeStream (ATOM_VertexArray *vertexArray);
	ATOM_VertexArray *getStream (int attrib) const;

public:
	virtual bool draw (ATOM_RenderDevice *device, int type, int prim_count, int offset);
	virtual bool createVertexDecl (void);
	virtual ATOM_GeometryType getType (void) const;

protected:
	ATOM_VertexDecl _vertexDecl;
	unsigned _numVerts;
	StreamInfo _streams[6 + ATOM_MAX_TEXTURE_LAYER_COUNT];
};

class ATOM_ENGINE_API ATOM_InterleavedStreamGeometry: public ATOM_Geometry
{
public:
	ATOM_InterleavedStreamGeometry (void);
	virtual ~ATOM_InterleavedStreamGeometry (void);
	virtual ATOM_Geometry *clone (void) const;

public:
	// wangjian modified
	void setStream (ATOM_VertexArray *vertexArray, unsigned attrib = 0, unsigned attribFlag = 0);
	ATOM_VertexArray *getStream (void) const;
	unsigned getStreamAttrib (void) const;

public:
	virtual bool draw (ATOM_RenderDevice *device, int type, int prim_count, int offset);
	virtual bool createVertexDecl (void);
	virtual ATOM_GeometryType getType (void) const;

protected:
	ATOM_VertexDecl _vertexDecl;
	ATOM_AUTOREF(ATOM_VertexArray) _stream;
	unsigned _attrib;
	// wangjian added
	unsigned _attrib_flag;
};

class ATOM_ENGINE_API ATOM_HWInstancingGeometry: public ATOM_InterleavedStreamGeometry
{
public:
	ATOM_HWInstancingGeometry (void);
	virtual ~ATOM_HWInstancingGeometry (void);
	virtual ATOM_Geometry *clone (void) const;

public:
	void setInstanceDataAttributes (unsigned attrib);
	void *addInstance (void);
	void clearInstances (void);
	unsigned getNumInstances (void) const;
	void enableInstancing (bool b);
	bool instancingEnabled (void) const;

public:
	virtual bool draw (ATOM_RenderDevice *device, int type, int prim_count, int offset);
	virtual bool createVertexDecl (void);
	virtual ATOM_GeometryType getType (void) const;

protected:
	unsigned _numInstances;
	unsigned _instanceDataStreamAttrib;
	unsigned _instanceDataSize;
	int _lockedStreamIndex;
	void *_lockPtr;
	bool _enableInstancing;
	ATOM_VertexDecl _instancedVertexDecl;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_VertexArray)> _instanceDataStreams;

	//--- wangjian added ---//
public:
	void *addInstance (unsigned numInstance);
	void setInstanceCount(unsigned numInstance);
protected:
	bool createStaticInstanceBuffer();

	static ATOM_AUTOREF(ATOM_VertexArray)	_instanceData_staticbuffer;
	//----------------------//
};

#endif // __ATOM3D_ENGINE_GEOMETRY_H
