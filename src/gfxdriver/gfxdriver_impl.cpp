#include "gfxdriver_impl.h"
#include "statecache.h"

ATOM_GfxDriverImpl::ATOM_GfxDriverImpl (void)
{
}

ATOM_GfxDriverImpl::~ATOM_GfxDriverImpl (void)
{
}

ATOM_StateCache *ATOM_GfxDriverImpl::getStateCache (void) const
{
	return 0;
}

bool ATOM_GfxDriverImpl::setTransform (TransformMode mode, const ATOM_Matrix4x4f &matrix)
{
	int matrixIndex = mode / 4;

	switch (mode)
	{
	case ATOM_GfxDriver::World:
		{
			_transformMatrices[matrixIndex][0].matrix = matrix;
			for (unsigned i = 0; i < 4; ++i)
			{
				_transformMatrices[matrixIndex][i].dirtyFlag = 1;
				_transformMatrices[ATOM_GfxDriver::WorldView/4][i].dirtyFlag = 1;
				_transformMatrices[ATOM_GfxDriver::WorldViewProjection/4][i].dirtyFlag = 1;
			}
			break;
		}
	case ATOM_GfxDriver::View:
		{
			_transformMatrices[matrixIndex][0].matrix = matrix;
			for (unsigned i = 0; i < 4; ++i)
			{
				_transformMatrices[matrixIndex][i].dirtyFlag = 1;
				_transformMatrices[ATOM_GfxDriver::WorldView/4][i].dirtyFlag = 1;
				_transformMatrices[ATOM_GfxDriver::ViewProjection/4][i].dirtyFlag = 1;
				_transformMatrices[ATOM_GfxDriver::WorldViewProjection/4][i].dirtyFlag = 1;
			}
			break;
		}
	case ATOM_GfxDriver::Projection:
		{
			_transformMatrices[matrixIndex][0].matrix = matrix;
			for (unsigned i = 0; i < 4; ++i)
			{
				_transformMatrices[matrixIndex][i].dirtyFlag = 1;
				_transformMatrices[ATOM_GfxDriver::ViewProjection/4][i].dirtyFlag = 1;
				_transformMatrices[ATOM_GfxDriver::WorldViewProjection/4][i].dirtyFlag = 1;
			}
			break;
		}
	case ATOM_GfxDriver::Texture0:
	case ATOM_GfxDriver::Texture1:
	case ATOM_GfxDriver::Texture2:
	case ATOM_GfxDriver::Texture3:
	case ATOM_GfxDriver::Texture4:
	case ATOM_GfxDriver::Texture5:
	case ATOM_GfxDriver::Texture6:
	case ATOM_GfxDriver::Texture7:
		{
			_transformMatrices[matrixIndex][0].matrix = matrix;
			for (unsigned i = 0; i < 4; ++i)
			{
				_transformMatrices[matrixIndex][i].dirtyFlag = 1;
			}
			break;
		}
	default:
		{
			return false;
		}
	}

	_transformChanged = true;

	return true;
}

const ATOM_Matrix4x4f & ATOM_GfxDriverImpl::getTransform (TransformMode mode) const
{
	int matrixIndex = mode / 4;

	switch (mode)
	{
	case ATOM_GfxDriver::World:
	case ATOM_GfxDriver::View:
	case ATOM_GfxDriver::Projection:
	case ATOM_GfxDriver::Texture0:
	case ATOM_GfxDriver::Texture1:
	case ATOM_GfxDriver::Texture2:
	case ATOM_GfxDriver::Texture3:
	case ATOM_GfxDriver::Texture4:
	case ATOM_GfxDriver::Texture5:
	case ATOM_GfxDriver::Texture6:
	case ATOM_GfxDriver::Texture7:
		{
			return _transformMatrices[matrixIndex][0].matrix;
		}
	case ATOM_GfxDriver::WorldView:
		{
			if (_transformMatrices[matrixIndex][0].dirtyFlag)
			{
				_transformMatrices[matrixIndex][0].matrix = _transformMatrices[ATOM_GfxDriver::View/4][0].matrix >> _transformMatrices[ATOM_GfxDriver::World/4][0].matrix;
				_transformMatrices[matrixIndex][0].dirtyFlag = 0;
			}
			return _transformMatrices[matrixIndex][0].matrix;
		}
	case ATOM_GfxDriver::ViewProjection:
		{
			if (_transformMatrices[matrixIndex][0].dirtyFlag)
			{
				_transformMatrices[matrixIndex][0].matrix = _transformMatrices[ATOM_GfxDriver::Projection/4][0].matrix >> _transformMatrices[ATOM_GfxDriver::View/4][0].matrix;
				_transformMatrices[matrixIndex][0].dirtyFlag = 0;
			}
			return _transformMatrices[matrixIndex][0].matrix;
		}
	case ATOM_GfxDriver::WorldViewProjection:
		{
			if (_transformMatrices[matrixIndex][0].dirtyFlag)
			{
				_transformMatrices[matrixIndex][0].matrix = getTransform(ATOM_GfxDriver::ViewProjection) >> _transformMatrices[ATOM_GfxDriver::World/4][0].matrix;
				_transformMatrices[matrixIndex][0].dirtyFlag = 0;
			}
			return _transformMatrices[matrixIndex][0].matrix;
		}
	default:
		{
			int modifierIndex = mode % 4;
			if (_transformMatrices[matrixIndex][modifierIndex].dirtyFlag)
			{
				switch (modifierIndex)
				{
				case ATOM_GfxDriver::InvMatrixIndex:
					_transformMatrices[matrixIndex][modifierIndex].matrix.invertFrom (_transformMatrices[matrixIndex][0].matrix);
					break;
				case ATOM_GfxDriver::TransposeMatrixIndex:
					_transformMatrices[matrixIndex][modifierIndex].matrix.transposeFrom (_transformMatrices[matrixIndex][0].matrix);
					break;
				case ATOM_GfxDriver::InvTransposeMatrixIndex:
					_transformMatrices[matrixIndex][modifierIndex].matrix.transposeFrom (getTransform((ATOM_GfxDriver::TransformMode)(matrixIndex*4+ATOM_GfxDriver::InvMatrixIndex)));
					break;
				default:
					// Should never reach here
					break;
				}
				_transformMatrices[matrixIndex][modifierIndex].dirtyFlag = 0;
			}
			return _transformMatrices[matrixIndex][modifierIndex].matrix;
		}
	}
}

const ATOM_GfxDriverCaps *ATOM_GfxDriverImpl::getCapabilities (void) const
{
	return 0;
}

bool ATOM_GfxDriverImpl::begin (void)
{
	return true;
}

void ATOM_GfxDriverImpl::end (void)
{
}

bool ATOM_GfxDriverImpl::pushMatrix (TransformMode mode)
{
	switch (mode)
	{
	case ATOM_GfxDriver::World:
		_transformStacks[0].push_back (_transformMatrices[mode/4][0].matrix);
		break;
	case ATOM_GfxDriver::View:
		_transformStacks[1].push_back (_transformMatrices[mode/4][0].matrix);
		break;
	case ATOM_GfxDriver::Projection:
		_transformStacks[2].push_back (_transformMatrices[mode/4][0].matrix);
		break;
	case ATOM_GfxDriver::Texture0:
	case ATOM_GfxDriver::Texture1:
	case ATOM_GfxDriver::Texture2:
	case ATOM_GfxDriver::Texture3:
	case ATOM_GfxDriver::Texture4:
	case ATOM_GfxDriver::Texture5:
	case ATOM_GfxDriver::Texture6:
	case ATOM_GfxDriver::Texture7:
		_transformStacks[3+mode-ATOM_GfxDriver::Texture0].push_back (_transformMatrices[mode/4][0].matrix);
		break;
	default:
		return false;
	}

	return true;
}

bool ATOM_GfxDriverImpl::popMatrix (TransformMode mode)
{
	switch (mode)
	{
	case ATOM_GfxDriver::World:
		{
			if (_transformStacks[0].empty ())
			{
				return false;
			}
			setTransform (mode, _transformStacks[0].back());
			_transformStacks[0].pop_back();
			break;
		}
	case ATOM_GfxDriver::View:
		{
			if (_transformStacks[1].empty ())
			{
				return false;
			}
			setTransform (mode, _transformStacks[1].back());
			_transformStacks[1].pop_back();
			break;
		}
	case ATOM_GfxDriver::Projection:
		{
			if (_transformStacks[2].empty ())
			{
				return false;
			}
			setTransform (mode, _transformStacks[2].back());
			_transformStacks[2].pop_back();
			break;
		}
	case ATOM_GfxDriver::Texture0:
	case ATOM_GfxDriver::Texture1:
	case ATOM_GfxDriver::Texture2:
	case ATOM_GfxDriver::Texture3:
	case ATOM_GfxDriver::Texture4:
	case ATOM_GfxDriver::Texture5:
	case ATOM_GfxDriver::Texture6:
	case ATOM_GfxDriver::Texture7:
		{
			int slot = 3 + mode - ATOM_GfxDriver::Texture0;
			if (_transformStacks[slot].empty ())
			{
				return false;
			}
			setTransform (mode, _transformStacks[slot].back());
			_transformStacks[slot].pop_back();
			break;
		}
	default:
		return false;
	}

	return true;
}

void ATOM_GfxDriverImpl::renderStreams(int type, int prim_count)
{
	if (applyStateChanges ())
	{
		_renderStreams (type, prim_count);
	}
}

void ATOM_GfxDriverImpl::renderStreamsIndexed(ATOM_IndexArray* index_array, int type, int prim_count, int offset)
{
	if (applyStateChanges ())
	{
		_renderStreamsIndexed (index_array, type, prim_count, offset);
	}
}

void ATOM_GfxDriverImpl::renderInterleavedMemoryStreamsIndexed (int type, int index_count, int vertex_range, unsigned attrib, unsigned stride, const void *stream, const unsigned short *indices)
{
	if (applyStateChanges ())
	{
		_renderInterleavedMemoryStreamsIndexed (type, index_count, vertex_range, attrib, stride, stream, indices);
	}
}

void ATOM_GfxDriverImpl::loadDefaultRenderStates (void)
{
	getStateCache()->getDesiredStates()->replace (getStateCache()->getDefaultStates());
}

void ATOM_GfxDriverImpl::syncRenderStates (void)
{
	_stateCache->loadDefaults ();
	_stateCache->getDefaultStates ()->apply (0, true);
}

bool ATOM_GfxDriverImpl::applyStateChanges (void)
{
	if (_transformChanged)
	{
		_syncTransformStates ();
		_transformChanged = false;
	}

	_stateCache->sync ();

	return true;
}
