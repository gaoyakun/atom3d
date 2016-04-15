#include "StdAfx.h"
#include "clipboard.h"

AS_Clipboard::AS_Clipboard (void)
{
	_nextContentType = CONTENT_TYPE_INVALID + 1;
	_data = 0;

	clear ();
}

AS_Clipboard::~AS_Clipboard (void)
{
	ATOM_FREE(_data);
}

void AS_Clipboard::clear (void)
{
	ATOM_FREE(_data);
	_data = 0;

	_dataSize = 0;
	_contentType = CONTENT_TYPE_INVALID;
}

int AS_Clipboard::allocContentType (const char *name)
{
	if (getContentType (name) != CONTENT_TYPE_INVALID)
	{
		return CONTENT_TYPE_INVALID;
	}

	int type = _nextContentType++;

	_typeMap[name] = type;

	_nameTable.resize (_nextContentType);
	_nameTable[type] = name;

	return type;
}

const char *AS_Clipboard::getContentName (int contentType) const
{
	if (contentType > CONTENT_TYPE_INVALID && contentType < _nameTable.size())
	{
		return _nameTable[contentType].c_str();
	}
	return 0;
}

int AS_Clipboard::getContentType (const char *name) const
{
	if (!name || !name[0])
	{
		return CONTENT_TYPE_INVALID;
	}

	ATOM_HASHMAP<ATOM_STRING, int>::const_iterator it = _typeMap.find (name);
	if (it != _typeMap.end ())
	{
		return it->second;
	}

	return CONTENT_TYPE_INVALID;
}

bool AS_Clipboard::setContent (int type, const void *data, unsigned size)
{
	if (type <= CONTENT_TYPE_INVALID || type >= _nextContentType)
	{
		return false;
	}

	if (data == 0 || size == 0)
	{
		return false;
	}

	clear ();

	_dataSize = size;
	_data = ATOM_MALLOC(size);
	memcpy (_data, data, size);

	_contentType = type;

	return true;
}

bool AS_Clipboard::setContent (const char *name, const void *data, unsigned size)
{
	int type = getContentType (name);
	if (type == CONTENT_TYPE_INVALID)
	{
		return false;
	}

	return setContent (type, data, size);
}

unsigned AS_Clipboard::getContentSize (void) const
{
	return _dataSize;
}

int AS_Clipboard::getContent (void *data) const
{
	if (data && _data)
	{
		memcpy (data, _data, _dataSize);
	}
	return _contentType;
}

