#include "StdAfx.h"
#include "components.h"
#include "model.h"

struct ComponentsData
{
	int refcount;
	ATOM_AUTOREF(ATOM_SharedModel) model;
};

ATOM_Components::ATOM_Components (void): _internalData (0)
{
	ATOM_STACK_TRACE(ATOM_Components::ATOM_Components);
}

//--- wangjian modified ---//
//  异步加载 ： 设置加载优先级
#if 0
ATOM_Components::ATOM_Components (const char *filename, bool mtload, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group, ATOM_LoadInterface *owner): _internalData (0)
{
	ATOM_STACK_TRACE(ATOM_Components::ATOM_Components);
	loadComponents (filename, mtload, flags, callback, userData, group, owner);
}

#else

ATOM_Components::ATOM_Components (const char *filename, int loadPriority, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group, ATOM_LoadInterface *owner): _internalData (0)
{
	ATOM_STACK_TRACE(ATOM_Components::ATOM_Components);
	loadComponents (filename, loadPriority, flags, callback, userData, group, owner);
}

#endif
//----------------------//

ATOM_Components::ATOM_Components (ATOM_SharedModel *model): _internalData (0)
{
	ATOM_STACK_TRACE(ATOM_Components::ATOM_Components);
	setModel (model);
}

ATOM_Components::~ATOM_Components (void)
{
	ATOM_STACK_TRACE(ATOM_Components::~ATOM_Components);
	reset ();
}

ATOM_Components::ATOM_Components (const ATOM_Components &other): _internalData(other._internalData)
{
	ATOM_STACK_TRACE(ATOM_Components::ATOM_Components);
	if (_internalData)
	{
		_internalData->refcount++;
	}
}

ATOM_Components & ATOM_Components::operator = (const ATOM_Components &other)
{
	if (this != &other && _internalData != other._internalData)
	{
		if (other._internalData)
		{
			other._internalData->refcount++;
		}

		reset ();

		_internalData = other._internalData;
	}

	return *this;
}

bool operator == (const ATOM_Components &c1, const ATOM_Components &c2)
{
	if (c1._internalData == c2._internalData)
	{
		return true;
	}

	if (c1._internalData && c2._internalData && c1._internalData->model == c2._internalData->model)
	{
		return true;
	}

	return false;
}

bool operator != (const ATOM_Components &c1, const ATOM_Components &c2)
{
	return ! operator == (c1, c2);
}

unsigned ATOM_Components::getNumMeshes (void) const
{
	ATOM_STACK_TRACE(ATOM_Components::getNumMeshes);
	ATOM_SharedModel *model = _internalData ? _internalData->model.get() : 0;
	return model ? model->getNumMeshes(): 0;
}

unsigned ATOM_Components::getNumTracks (void) const
{
	ATOM_STACK_TRACE(ATOM_Components::getNumTracks);
	ATOM_SharedModel *model = _internalData ? _internalData->model.get() : 0;
	return model ? model->getNumTracks () : 0;
}

ATOM_SharedMesh *ATOM_Components::getMesh (unsigned index) const
{
	ATOM_STACK_TRACE(ATOM_Components::getMesh);
	return (_internalData && _internalData->model) ? _internalData->model->getMesh (index) : 0;
}

ATOM_JointAnimationTrack *ATOM_Components::getTrack (unsigned index) const
{
	ATOM_STACK_TRACE(ATOM_Components::getTrack);
	return (_internalData && _internalData->model) ? _internalData->model->getTrack (index) : 0;
}

ATOM_Skeleton *ATOM_Components::getSkeleton (void) const
{
	ATOM_STACK_TRACE(ATOM_Components::getSkeleton);
	return (_internalData && _internalData->model) ? _internalData->model->getSkeleton () : 0;
}

ATOM_SharedModel *ATOM_Components::getModel (void) const
{
	ATOM_STACK_TRACE(ATOM_Components::getModel);
	return _internalData ? _internalData->model.get() : 0;
}

ATOM_AUTOREF(ATOM_SharedModel) ATOM_Components::findModel (const char *filename)
{
	ATOM_STACK_TRACE(ATOM_Components::findModel);
	if (filename)
	{
		char buffer[ATOM_VFS::max_filename_length];
		if (!ATOM_CompletePath (filename, buffer))
		{
			return false;
		}
		_strlwr (buffer);

		return ATOM_LookupObject (ATOM_SharedModel::_classname(), buffer);
	}
	return 0;
}

ATOM_AUTOREF(ATOM_SharedModel) ATOM_Components::newModel (const char *filename, bool &createNew)
{
	ATOM_STACK_TRACE(ATOM_Components::newModel);
	if (filename)
	{
		char buffer[ATOM_VFS::max_filename_length];
		if (!ATOM_CompletePath (filename, buffer))
		{
			return false;
		}
		_strlwr (buffer);

		return ATOM_LookupOrCreateObject (ATOM_SharedModel::_classname(), buffer, &createNew);
	}

	createNew = false;
	return 0;
}

static void __cdecl defaultModelLoadingCallback (ATOM_LoadingRequest *request)
{
	ATOM_DELETE(request);
}

//--- wangjian modified ---//
// 异步加载：设置加载优先级
#if 0

bool ATOM_Components::loadComponents (const char *filename, bool mtload, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group, ATOM_LoadInterface *owner)
{
	ATOM_STACK_TRACE(ATOM_Components::loadComponents);
	if (filename)
	{
		char buffer[ATOM_VFS::max_filename_length];
		if (!ATOM_CompletePath (filename, buffer))
		{
			return false;
		}
		_strlwr (buffer);

		bool createNew = false;
		ATOM_AUTOREF(ATOM_SharedModel) model = ATOM_LookupOrCreateObject (ATOM_SharedModel::_classname(), buffer, &createNew);
#if 0
		if (mtload)
		{
			model->getLoadInterface()->setFileName (filename);
			model->getLoadInterface()->mtload (flags, group, (callback ? callback : &defaultModelLoadingCallback), userData);
		}
		else 
#endif

		// 需要加载
		if (createNew)
		{
			//--- wangjian modified ---//
			// 异步加载相关 ：添加异步加载标记
			model->load (ATOM_GetRenderDevice(), buffer, mtload ? 0 : ATOM_LoadPriority_IMMEDIATE);
			//-------------------------//
#if 0
			if (model->getLoadInterface()->getLoadingState() != ATOM_LoadInterface::LS_LOADED)
			{
				if (callback)
				{
					callback (0);
				}
				return false;
			}
			else if (callback)
			{
				callback (0);
			}
#endif
		}
		else
		{
#if 0
			if (model->getLoadInterface()->getLoadingState() == ATOM_LoadInterface::LS_LOADING)
			{
				ATOM_ContentStream::waitForInterfaceDone (model->getLoadInterface());

				if (model->getLoadInterface()->getLoadingState () == ATOM_LoadInterface::LS_LOADED)
				{
					if (callback)
					{
						callback (0);
					}
				}
				else
				{
					if (callback)
					{
						callback (0);
					}
					return false;
				}
			}
#endif
		}

		setModel (model.get());
		return true;
	}

	return false;
}

#else 

bool ATOM_Components::loadComponents (const char *filename, int loadPriority, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group, ATOM_LoadInterface *owner)
{
	ATOM_STACK_TRACE(ATOM_Components::loadComponents);

	if (filename)
	{
		char buffer[ATOM_VFS::max_filename_length];
		if (!ATOM_CompletePath (filename, buffer))
		{
			return false;
		}
		_strlwr (buffer);

		// 检查是否需要加载
		bool createNew = false;
		// 如果模型已经存在 则不需要再次加载
		ATOM_AUTOREF(ATOM_SharedModel) model = ATOM_LookupOrCreateObject (ATOM_SharedModel::_classname(), buffer, &createNew);

		// 如果该模型已经在异步加载中 此时如果直接加载该模型 会出现问题
#if 0
		// 但是如果是直接加载标记 且该模型还未加载完成 则还是直接加载
		if( loadPriority == ATOM_LoadPriority_IMMEDIATE && 
			( false == model->getAsyncLoader()->IsLoadAllFinished() )/*model->getAsyncLoader()->GetLoadStage() != ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED*/ )
			createNew = true;
#endif
	
		///////////////////////////////////////////////////////////////////////////////////
#if 1
		// 如果存在
		if( !createNew )
		{
			if( model->getAsyncLoader()->priority != ATOM_LoadPriority_IMMEDIATE )
			{
				if( loadPriority > model->getAsyncLoader()->priority )
					model->getAsyncLoader()->priority = loadPriority;

				if( model->getAsyncLoader()->abandoned )
					ATOM_AsyncLoader::ReProcessLoadObject( model->getAsyncLoader() );
			}
		}
#endif
		///////////////////////////////////////////////////////////////////////////////////

#if 0
		if (mtload)
		{
			model->getLoadInterface()->setFileName (filename);
			model->getLoadInterface()->mtload (flags, group, (callback ? callback : &defaultModelLoadingCallback), userData);
		}
		else 
#endif
			// 加载是否成功
			bool bLoadSuccess = true;
			if (createNew)
			{
				//--- wangjian modified ---//
				// 异步加载相关 ：添加异步加载标记
#if 1
				bLoadSuccess = model->load(ATOM_GetRenderDevice(), buffer, loadPriority );
#else
				bLoadSuccess = model->load_half(ATOM_GetRenderDevice(), buffer, loadPriority );
#endif
				//-------------------------//
#if 0
				if (model->getLoadInterface()->getLoadingState() != ATOM_LoadInterface::LS_LOADED)
				{
					if (callback)
					{
						callback (0);
					}
					return false;
				}
				else if (callback)
				{
					callback (0);
				}
#endif
			}
			else
			{
#if 0
				if (model->getLoadInterface()->getLoadingState() == ATOM_LoadInterface::LS_LOADING)
				{
					ATOM_ContentStream::waitForInterfaceDone (model->getLoadInterface());

					if (model->getLoadInterface()->getLoadingState () == ATOM_LoadInterface::LS_LOADED)
					{
						if (callback)
						{
							callback (0);
						}
					}
					else
					{
						if (callback)
						{
							callback (0);
						}
						return false;
					}
				}
#endif
			}

			// 加载成功 设置模型
			if( bLoadSuccess )
			{
				setModel (model.get());
				return true;
			}
	}

	return false;
}

#endif
//---------------------------------------------------------------------//


/*
bool ATOM_Components::loadComponentsFromMemory (const void *mem, unsigned size)
{
	ATOM_STACK_TRACE(ATOM_Components::loadComponentsFromMemory);
	ATOM_HARDREF(ATOM_Model) model;
	if (!model->memload (ATOM_GetRenderDevice(), mem, size, true, true))
	{
		return false;
	}

	if (!model->initMaterials (true, false))
	{
		return false;
	}

	ATOM_ModelLockStruct lockStruct;

	if (model->lock ())
	{
		bool ret = model->realize ();

		model->unlock ();

		if (ret)
		{
			setModel (model.get());

			return true;
		}
	}

	return false;
}
*/
void ATOM_Components::setModel (ATOM_SharedModel *model)
{
	ATOM_STACK_TRACE(ATOM_Components::setModel);
	if (!_internalData || _internalData->model != model)
	{
		reset ();

		_internalData = ATOM_NEW(ComponentsData);
		_internalData->refcount = 1;
		_internalData->model = model;
	}
}

void ATOM_Components::reset (void)
{
	ATOM_STACK_TRACE(ATOM_Components::reset);
	if (_internalData)
	{
		_internalData->refcount--;

		if (0 == _internalData->refcount)
		{
			ATOM_DELETE(_internalData);
		}

		_internalData = 0;
	}
}

bool ATOM_Components::isValid (void) const
{
	ATOM_STACK_TRACE(ATOM_Components::isValid);
	return _internalData && _internalData->model;
}


