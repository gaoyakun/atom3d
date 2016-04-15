#include "StdAfx.h"
#include "loadinterface.h"

ATOM_ModelLoadInterface::ATOM_ModelLoadInterface (ATOM_Model *owner)
{
	ATOM_STACK_TRACE(ATOM_ModelLoadInterface::ATOM_ModelLoadInterface);

	_model = owner;
	_loader = 0;
}

ATOM_ModelLoadInterface::~ATOM_ModelLoadInterface (void)
{
	ATOM_STACK_TRACE(ATOM_ModelLoadInterface::~ATOM_ModelLoadInterface);

	ATOM_DELETE(_loader);
}

ATOM_LoadInterface::LoadingState ATOM_ModelLoadInterface::load (void)
{
	ATOM_STACK_TRACE(ATOM_ModelLoadInterface::load);

	ATOM_ASSERT(_model);

	_state = _model->load (ATOM_GetRenderDevice()) ? ATOM_LoadInterface::LS_LOADED : ATOM_LoadInterface::LS_LOADFAILED;

	return _state;
}

ATOM_LoadInterface::LoadingState ATOM_ModelLoadInterface::mtload (unsigned flags, long group, ATOM_LoadingRequestCallback callback, void *userData)
{
	ATOM_STACK_TRACE(ATOM_ModelLoadInterface::mtload);

	ATOM_ASSERT(_model);

	if (!_loader)
	{
		_loader = ATOM_NEW(ATOM_ModelLoader, getFileName());
		_loader->setModel (_model);

		if (_state == LS_NOTLOADED)
		{
			_model->initLockStruct (group);
		}
	}

	ATOM_LoadingRequest *request = ATOM_NEW(ATOM_LoadingRequest);
	request->loadInterface = this;
	request->flags = flags;
	request->userData = userData;
	request->callback = callback;
	request->groupId = group;

	_model->addRef ();

	ATOM_ContentStream::appendRequest (request);

	return _state;
}

void ATOM_ModelLoadInterface::unload (void)
{
	ATOM_STACK_TRACE(ATOM_ModelLoadInterface::unload);

	_model->clear ();
}

void ATOM_ModelLoadInterface::finishLoad (void)
{
	ATOM_STACK_TRACE(ATOM_ModelLoadInterface::finishLoad);

	ATOM_AUTOREF(ATOM_Model) refHolder = _model;

	_model->decRef ();

	if (_loader)
	{
		ATOM_DELETE(_loader);
		_loader = 0;
	}
}

void ATOM_ModelLoadInterface::insureLoadDone (void)
{
	ATOM_STACK_TRACE(ATOM_ModelLoadInterface::insureLoadDone);

	_model->wait ();
}

ATOM_BaseResourceLoader *ATOM_ModelLoadInterface::getLoader (void)
{
	return _loader;
}

ATOM_NodeLoadInterface::ATOM_NodeLoadInterface (ATOM_Node *owner)
{
	ATOM_STACK_TRACE(ATOM_NodeLoadInterface::ATOM_NodeLoadInterface);

	_node = owner;
}

ATOM_NodeLoadInterface::~ATOM_NodeLoadInterface (void)
{
	ATOM_STACK_TRACE(ATOM_NodeLoadInterface::~ATOM_NodeLoadInterface);
}

ATOM_LoadInterface::LoadingState ATOM_NodeLoadInterface::load (void)
{
	ATOM_STACK_TRACE(ATOM_NodeLoadInterface::load);

	//--------------------------------------------- wangjian modified ----------------------------------------------------------------//
	bool delayLoad = (_node->getLoadFlag() == ATOM_Node::LOAD_DELAYLOAD );
	_state = _node->load (ATOM_GetRenderDevice()) ? ( delayLoad ? ATOM_LoadInterface::LS_NOTLOADED : ATOM_LoadInterface::LS_LOADED ) : 
													ATOM_LoadInterface::LS_LOADFAILED;
	return delayLoad ? ATOM_LoadInterface::LS_LOADED : _state;
	//--------------------------------------------------------------------------------------------------------------------------------//
}

ATOM_LoadInterface::LoadingState ATOM_NodeLoadInterface::mtload (unsigned flags, long group, ATOM_LoadingRequestCallback callback, void *userData)
{
	ATOM_STACK_TRACE(ATOM_NodeLoadInterface::mtload);

	_state = _node->mtload (ATOM_GetRenderDevice(), flags, callback, userData, group) ? ATOM_LoadInterface::LS_LOADING : ATOM_LoadInterface::LS_LOADFAILED;

	return _state;
}

void ATOM_NodeLoadInterface::unload (void)
{
}

void ATOM_NodeLoadInterface::finishLoad (void)
{
}

ATOM_BaseResourceLoader *ATOM_NodeLoadInterface::getLoader (void)
{
	return 0;
}
/*
ATOM_GeodeLoadInterface::ATOM_GeodeLoadInterface (ATOM_Geode *owner)
{
	ATOM_STACK_TRACE(ATOM_GeodeLoadInterface::ATOM_GeodeLoadInterface);

	_geode = owner;
	_loader = 0;
}

ATOM_GeodeLoadInterface::~ATOM_GeodeLoadInterface (void)
{
	ATOM_STACK_TRACE(ATOM_GeodeLoadInterface::~ATOM_GeodeLoadInterface);

	ATOM_DELETE(_loader);
}

ATOM_LoadInterface::LoadingState ATOM_GeodeLoadInterface::load (void)
{
	ATOM_STACK_TRACE(ATOM_GeodeLoadInterface::load);

	ATOM_ASSERT(_geode);

	_state = _geode->load (ATOM_GetRenderDevice()) ? ATOM_LoadInterface::LS_LOADED : ATOM_LoadInterface::LS_LOADFAILED;

	return _state;
}

static void GeodeModelCallback (ATOM_LoadingRequest *request)
{
	ATOM_STACK_TRACE(GeodeModelCallback);

	ATOM_DELETE(request);
}

ATOM_LoadInterface::LoadingState ATOM_GeodeLoadInterface::mtload (unsigned flags, long group, ATOM_LoadingRequestCallback callback, void *userData)
{
	ATOM_STACK_TRACE(ATOM_GeodeLoadInterface::mtload);

	ATOM_ASSERT(_geode);

	if (!_loader)
	{
		_loader = ATOM_NEW(ATOM_GeodeLoader);
		_loader->setNode (_geode);
	}

	if (!_geode->getNodeFileName().empty ())
	{
		_geode->setModelFileName (_geode->getNodeFileName());
	}

	for (unsigned i = 0; i <= _geode->getModelFileNames().size(); ++i)
	{
		const char *filename = (i < _geode->getModelFileNames().size()) ? _geode->getModelFileNames()[i].c_str() : _geode->getModelFileName().c_str();
		if (!filename || !filename[0])
		{
			continue;
		}

		char buffer[ATOM_VFS::max_filename_length];
		if (!ATOM_CompletePath (filename, buffer))
		{
			continue;
		}
		_strlwr (buffer);
		bool createNew = false;
		ATOM_AUTOREF(ATOM_Model) model = ATOM_LookupOrCreateObject (ATOM_Model::_classname(), buffer, &createNew);
		_loader->addModelRequest (model.get());

		model->getLoadInterface()->setFileName (filename);
		model->getLoadInterface()->mtload (flags & ~ATOM_LoadInterface::LF_ORDERED, 0, &GeodeModelCallback, 0);

		_geode->addComponents (model.get());
	}

	ATOM_LoadingRequest *request = ATOM_NEW(ATOM_LoadingRequest);
	request->loadInterface = this;
	request->flags = flags;
	request->userData = userData;
	request->callback = callback;
	request->groupId = group;

	_geode->addRef ();

	ATOM_ContentStream::appendRequest (request);

	return _state;
}

void ATOM_GeodeLoadInterface::unload (void)
{
	ATOM_STACK_TRACE(ATOM_GeodeLoadInterface::unload);

	_geode->clear ();
}

void ATOM_GeodeLoadInterface::insureLoadDone (void)
{
	ATOM_STACK_TRACE(ATOM_GeodeLoadInterface::insureLoadDone);

	_geode->mtWait ();
}

void ATOM_GeodeLoadInterface::finishLoad (void)
{
	ATOM_STACK_TRACE(ATOM_GeodeLoadInterface::finishLoad);

	ATOM_AUTOREF(ATOM_BaseGeode) refHolder = _geode;

	_geode->decRef ();

	if (_loader)
	{
		ATOM_DELETE(_loader);
		_loader = 0;
	}
}

ATOM_BaseResourceLoader *ATOM_GeodeLoadInterface::getLoader (void)
{
	ATOM_STACK_TRACE(ATOM_GeodeLoadInterface::getLoader);

	return _loader;
}

*/
