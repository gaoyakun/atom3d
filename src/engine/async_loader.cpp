#include "StdAfx.h"
#include <ddraw.h>
#include "async_loader.h"
#include "loadinterface.h"


ATOM_ModelLoader::ATOM_ModelLoader (void)
{
	ATOM_STACK_TRACE(ATOM_ModelLoader::ATOM_ModelLoader);
}

ATOM_ModelLoader::ATOM_ModelLoader (const char *filename): ATOM_BaseResourceLoader (filename)
{
	ATOM_STACK_TRACE(ATOM_ModelLoader::ATOM_ModelLoader);
}

void ATOM_ModelLoader::setModel (ATOM_Model *model)
{
	ATOM_STACK_TRACE(ATOM_ModelLoader::setModel);

	_model = model;
}

ATOM_BaseResourceLoader::LOADRESULT ATOM_ModelLoader::loadFromDisk (void)
{
	ATOM_STACK_TRACE(ATOM_ModelLoader::loadFromDisk);
	ATOM_BaseResourceLoader::LOADRESULT result = ATOM_BaseResourceLoader::loadFromDisk ();
	if (result != ATOM_BaseResourceLoader::LOADERR_OK)
	{
		return result;
	}

	if (!_model->memload (ATOM_GetRenderDevice(), getData(), getDataSize(), false, false))
	{
		return ATOM_BaseResourceLoader::LOADERR_FAILED;
	}

	return ATOM_BaseResourceLoader::LOADERR_OK;
}

ATOM_Model *ATOM_ModelLoader::getModel (void) const
{
	ATOM_STACK_TRACE(ATOM_ModelLoader::getModel);
	return _model.get();
}

bool ATOM_ModelLoader::lock (void)
{
	ATOM_STACK_TRACE(ATOM_ModelLoader::lock);
	if (!_model->lock ())
	{
		return false;
	}

	return true;
}

int ATOM_ModelLoader::unlock (void)
{
	ATOM_STACK_TRACE(ATOM_ModelLoader::unlock);
	ATOM_ModelLockStruct *s = _model->getLockStruct ();
	ATOM_ASSERT(s);

	if (!s->wait)
	{
		s->wait = true;
		_model->unlock ();
		_model->initMaterials (true, true);
	}

	bool loadFailed = false;

	for (unsigned i = 0; i < s->depends.size(); ++i)
	{
		ATOM_LoadInterface::LoadingState state = s->depends[i]->getLoadingState();
		if (state == ATOM_LoadInterface::LS_LOADING)
		{
			return LOADINGSTAGE_TRYAGAIN;
		}
		else if (state != ATOM_LoadInterface::LS_LOADED)
		{
			ATOM_TextureLoadInterface *textureLoadInterface = dynamic_cast<ATOM_TextureLoadInterface*>(s->depends[i]);
			if (textureLoadInterface)
			{
				ATOM_LOGGER::error ("Texture <%s> load failed\n", textureLoadInterface->getFileName());
				textureLoadInterface->setTexture (ATOM_GetColorTexture (0xFFFFFFFF));
				textureLoadInterface->setLoadingState (ATOM_LoadInterface::LS_LOADED);
			}
			else
			{
				loadFailed = true;
			}
		}
	}

	s->depends.clear ();
	return loadFailed ? LOADINGSTAGE_NOTLOADED : LOADINGSTAGE_FINISH;
}

bool ATOM_ModelLoader::realize (void)
{
	ATOM_STACK_TRACE(ATOM_ModelLoader::realize);
	return _model->realize ();
}

ATOM_BaseNodeLoader::ATOM_BaseNodeLoader (void)
{
	ATOM_STACK_TRACE(ATOM_BaseNodeLoader::ATOM_BaseNodeLoader);
}

ATOM_BaseNodeLoader::ATOM_BaseNodeLoader (const char *filename): ATOM_BaseResourceLoader (filename)
{
	ATOM_STACK_TRACE(ATOM_BaseNodeLoader::ATOM_BaseNodeLoader);
}

void ATOM_BaseNodeLoader::setNode (ATOM_Node *node)
{
	ATOM_STACK_TRACE(ATOM_BaseNodeLoader::setNode);
	_node = node;
}

ATOM_BaseResourceLoader::LOADRESULT ATOM_BaseNodeLoader::loadFromDisk (void)
{
	ATOM_STACK_TRACE(ATOM_BaseNodeLoader::loadFromDisk);
	return (_node && _node->mtLoadFromDisk(this)) ? ATOM_BaseResourceLoader::LOADERR_OK : ATOM_BaseResourceLoader::LOADERR_FAILED;
}

ATOM_Node *ATOM_BaseNodeLoader::getNode (void) const
{
	ATOM_STACK_TRACE(ATOM_BaseNodeLoader::getNode);
	return _node.get();
}

bool ATOM_BaseNodeLoader::lock (void)
{
	ATOM_STACK_TRACE(ATOM_BaseNodeLoader::lock);
	return _node && _node->mtLock (ATOM_GetRenderDevice(), this);
}

int ATOM_BaseNodeLoader::unlock (void)
{
	ATOM_STACK_TRACE(ATOM_BaseNodeLoader::unlock);
	return _node ? _node->mtUnlock (ATOM_GetRenderDevice(), this) : LOADINGSTAGE_NOTLOADED;
}

bool ATOM_BaseNodeLoader::realize (void)
{
	ATOM_STACK_TRACE(ATOM_BaseNodeLoader::realize);
	return _node && _node->mtUploadDeviceData (this);
}

ATOM_GeodeLoader::ATOM_GeodeLoader (void)
{
	ATOM_STACK_TRACE(ATOM_GeodeLoader::ATOM_GeodeLoader);
}

void ATOM_GeodeLoader::addModelRequest (ATOM_Model *model)
{
	ATOM_STACK_TRACE(ATOM_GeodeLoader::addModelRequest);
	_modelRequests.push_back (model);
}

unsigned ATOM_GeodeLoader::getNumModelRequests (void) const
{
	ATOM_STACK_TRACE(ATOM_GeodeLoader::getNumModelRequests);
	return _modelRequests.size();
}

ATOM_LoadInterface *ATOM_GeodeLoader::getModelRequest (unsigned index) const
{
	ATOM_STACK_TRACE(ATOM_GeodeLoader::getModelRequest);
	return _modelRequests[index]->getLoadInterface();
}


