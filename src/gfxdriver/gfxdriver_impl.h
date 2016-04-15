#ifndef __ATOM3D_GFXDRIVER_GFXDRIVER_IMPL_H
#define __ATOM3D_GFXDRIVER_GFXDRIVER_IMPL_H

#include "basedefs.h"
#include "gfxdriver.h"

class ATOM_GfxWindow;

class ATOM_GfxDriverImpl: public ATOM_GfxDriver
{
public:
	ATOM_GfxDriverImpl (void);
	virtual ~ATOM_GfxDriverImpl (void);

public:
	virtual ATOM_StateCache *getStateCache (void) const;
	virtual bool setTransform (TransformMode mode, const ATOM_Matrix4x4f &matrix);
	virtual const ATOM_Matrix4x4f & getTransform (TransformMode mode) const;
	virtual const ATOM_GfxDriverCaps *getCapabilities (void) const;
	virtual bool begin (void);
	virtual void end (void);
	virtual bool pushMatrix (TransformMode mode);
	virtual bool popMatrix (TransformMode mode);
	virtual void loadDefaultRenderStates (void);
    virtual void syncRenderStates (void);
	virtual void renderStreams(int type, int prim_count);
	virtual void renderStreamsIndexed(ATOM_IndexArray* index_array, int type, int prim_count, int offset);
	virtual void renderInterleavedMemoryStreamsIndexed (int type, int index_count, int vertex_range, unsigned attrib, unsigned stride, const void *stream, const unsigned short *indices);

protected:
	virtual void _syncTransformStates (void) = 0;
	virtual void _renderStreams(int type, int prim_count);
	virtual void _renderStreamsIndexed(ATOM_IndexArray* index_array, int type, int prim_count, int offset);
	virtual void _renderInterleavedMemoryStreamsIndexed (int type, int index_count, int vertex_range, unsigned attrib, unsigned stride, const void *stream, const unsigned short *indices);

private:
	bool applyStateChanges (void);

private:
	struct TransformMatrix
	{
		ATOM_Matrix4x4f matrix;
		int dirtyFlag;
	};
	mutable TransformMatrix _transformMatrices[14][4];
	mutable bool _transformChanged;
	ATOM_VECTOR<ATOM_Matrix4x4f> _transformStacks[11];
	ATOM_StateCache *_stateCache;
};

#endif // __ATOM3D_GFXDRIVER_GFXDRIVER_IMPL_H
