#include "StdAfx.h"
#include "chunkio.h"

ATOM_ChunkStream::ATOM_ChunkStream (void)
{
	_data = 0;
	_pos = 0;
	_size = 0;
	_cap = 0;
	_error = false;
}

ATOM_ChunkStream::~ATOM_ChunkStream (void)
{
	ATOM_FREE(_data);
}

void ATOM_ChunkStream::read (void *buffer, unsigned size)
{
	_error = false;
	unsigned sizeRemain = _size - (_pos - _data);

	if (sizeRemain < size)
	{
		_error = true;
		size = sizeRemain;
	}

	if (buffer)
	{
		memcpy (buffer, _pos, size);
	}
	_pos += size;
}

void ATOM_ChunkStream::write (const void *buffer, unsigned size)
{
	_error = false;
	unsigned size_all = size + (_pos - _data);

	if (size_all > _cap)
	{
		unsigned size_alloc = size_all - _cap;
		if (size_alloc < 1024)
		{
			size_alloc = 1024;
		}
		_cap += size_alloc + _cap;

		char *new_data = (char*)ATOM_REALLOC (_data, _cap);
		_pos = new_data + (_pos - _data);
		_data = new_data;
	}

	if (size_all > _size)
	{
		_size = size_all;
	}

	if (buffer)
	{
		memcpy (_pos, buffer, size);
	}

	_pos += size;
}

float ATOM_ChunkStream::readFloat (void)
{
	float val;
	read (&val, sizeof(float));
	if (_error)
	{
		val = 0.f;
	}
	return val;
}

void ATOM_ChunkStream::writeFloat (float value)
{
	write (&value, sizeof(float));
}

int ATOM_ChunkStream::readInt (void)
{
	int val;
	read (&val, sizeof(int));
	if (_error)
	{
		val = 0;
	}
	return val;
}

void ATOM_ChunkStream::writeInt (int value)
{
	write (&value, sizeof(int));
}

char ATOM_ChunkStream::readChar (void)
{
	char val;
	read (&val, sizeof(char));
	if (_error)
	{
		val = 0;
	}
	return val;
}

void ATOM_ChunkStream::writeChar (char value)
{
	write (&value, sizeof(char));
}

short ATOM_ChunkStream::readShort (void)
{
	short val;
	read (&val, sizeof(short));
	if (_error)
	{
		val = 0;
	}
	return val;
}

void ATOM_ChunkStream::writeShort (short value)
{
	write (&value, sizeof(short));
}

long ATOM_ChunkStream::readLong (void)
{
	long val;
	read (&val, sizeof(long));
	if (_error)
	{
		val = 0;
	}
	return val;
}

void ATOM_ChunkStream::writeLong (long value)
{
	write (&value, sizeof(long));
}

long long ATOM_ChunkStream::readLongLong (void)
{
	long long val;
	read (&val, sizeof(long long));
	if (_error)
	{
		val = 0;
	}
	return val;
}

void ATOM_ChunkStream::writeLongLong (long long value)
{
	write (&value, sizeof(long long));
}

double ATOM_ChunkStream::readDouble (void)
{
	double val;
	read (&val, sizeof(double));
	if (_error)
	{
		val = 0.0;
	}
	return val;
}

void ATOM_ChunkStream::writeDouble (double value)
{
	write (&value, sizeof(double));
}

int ATOM_ChunkStream::readCString (char *value)
{
	int num_chars = 0;

	for (;;)
	{
		char ch = readChar ();
		if (_error)
		{
			return num_chars;
		}
		++num_chars;

		if (value)
		{
			*value++ = ch;
		}

		if (ch == '\0')
		{
			break;
		}
	}

	return num_chars;
}

void ATOM_ChunkStream::writeCString (const char *value)
{
	if (value)
	{
		write (value, strlen(value)+1);
	}
}

bool ATOM_ChunkStream::error (void) const
{
	return _error;
}

bool ATOM_ChunkStream::eof (void) const
{
	return _pos == _data;
}

void ATOM_ChunkStream::seek (int offset)
{
	char *new_pos = _pos + offset;
	if (new_pos < _data || new_pos > _data+_size)
	{
		_error = true;
	}
	else
	{
		_pos = new_pos;
	}
}

void ATOM_ChunkStream::resize (unsigned size)
{
	if (size > _cap)
	{
		unsigned size_alloc = size - _cap;
		if (size_alloc < 1024)
		{
			size_alloc = 1024;
		}
		_cap += size_alloc;

		char *new_data = (char*)ATOM_REALLOC(_data, _cap);
		_pos = new_data + (_pos - _data);
		_data = new_data;

		if (_pos > _data + size)
		{
			_pos = _data + size;
		}
	}

	_size = size;
}

int ATOM_ChunkStream::position (void) const
{
	return _pos - _data;
}

unsigned ATOM_ChunkStream::getSize (void) const
{
	return _size;
}

void *ATOM_ChunkStream::getBuffer (void) const
{
	return _data;
}

ATOM_ChunkIO::Chunk::Chunk (void)
{
	_sig = 0;
	_parent = 0;
	_firstChild = 0;
	_nextSibling = 0;
	_stream = ATOM_NEW(ATOM_ChunkStream);
}

ATOM_ChunkIO::Chunk::~Chunk (void)
{
	clearChildren ();
	ATOM_DELETE(_stream);
}

void ATOM_ChunkIO::Chunk::setSig (unsigned sig)
{
	_sig = sig;
}

unsigned ATOM_ChunkIO::Chunk::getSig (void) const
{
	return _sig;
}

ATOM_ChunkIO::Chunk *ATOM_ChunkIO::Chunk::getFirstChild (void) const
{
	return _firstChild;
}

ATOM_ChunkIO::Chunk *ATOM_ChunkIO::Chunk::getFirstChild (unsigned sig) const
{
	return (_firstChild && _firstChild->getSig() != sig) ? _firstChild->getNextSibling (sig) : _firstChild;
}

ATOM_ChunkIO::Chunk *ATOM_ChunkIO::Chunk::getNextSibling (void) const
{
	return _nextSibling;
}

ATOM_ChunkIO::Chunk *ATOM_ChunkIO::Chunk::getNextSibling (unsigned sig) const
{
	Chunk *result = _nextSibling;
	while (result)
	{
		if (result->getSig() == sig)
		{
			break;
		}
		result = result->_nextSibling;
	}
	return result;
}

ATOM_ChunkIO::Chunk *ATOM_ChunkIO::Chunk::getParent (void) const
{
	return _parent;
}

ATOM_ChunkIO::Chunk * ATOM_ChunkIO::Chunk::prependChild (void)
{
	ATOM_ChunkIO::Chunk *child = ATOM_NEW(ATOM_ChunkIO::Chunk);

	child->_nextSibling = _firstChild;
	child->_parent = this;
	_firstChild = child;

	return child;
}

ATOM_ChunkIO::Chunk * ATOM_ChunkIO::Chunk::appendChild (void)
{
	if (!_firstChild)
	{
		return prependChild ();
	}
	else
	{
		ATOM_ChunkIO::Chunk *child = ATOM_NEW(ATOM_ChunkIO::Chunk);
		Chunk *last = _firstChild;
		while (last->_nextSibling)
		{
			last = last->_nextSibling;
		}
		last->_nextSibling = child;
		child->_parent = this;
		child->_nextSibling = 0;
		return child;
	}
}

void ATOM_ChunkIO::Chunk::removeChild (ATOM_ChunkIO::Chunk *child)
{
	if (child && child->_parent == this)
	{
		Chunk *prev = 0;
		Chunk *test = _firstChild;
		while (test)
		{
			if (test == child)
			{
				if (prev)
				{
					prev->_nextSibling = child->_nextSibling;
				}
				else
				{
					_firstChild = child->_nextSibling;
				}
				ATOM_DELETE(child);
				return;
			}
			prev = test;
			test = prev->_nextSibling;
		}
	}
}

void ATOM_ChunkIO::Chunk::clearChildren (void)
{
	while (_firstChild)
	{
		removeChild (_firstChild);
	}
}

unsigned ATOM_ChunkIO::Chunk::getSize (void) const
{
	return _stream->getSize();
}

unsigned ATOM_ChunkIO::Chunk::getInclusiveSize (void) const
{
	unsigned inclusiveSize = getSize();

	for (Chunk *child = _firstChild; child; child = child->_nextSibling)
	{
		inclusiveSize += child->getInclusiveSize ();
		inclusiveSize += 3 * sizeof(unsigned);
	}

	return inclusiveSize;
}

ATOM_ChunkStream *ATOM_ChunkIO::Chunk::getChunkStream (void) const
{
	return _stream;
}

ATOM_ChunkIO::ATOM_ChunkIO (void)
{
	_topChunk = ATOM_NEW(Chunk);
	_ioError = false;
	_fp = 0;
}

ATOM_ChunkIO::~ATOM_ChunkIO (void)
{
	ATOM_DELETE(_topChunk);
	if (_fp)
	{
		ATOM_CloseFile (_fp);
		_fp = 0;
	}
}

void ATOM_ChunkIO::read (void *buffer, unsigned size)
{
	if (size)
	{
		if (buffer)
		{
			if (_read (buffer, size) != size)
			{
				markError ();
			}
		}
		else
		{
			if (!_seek (size))
			{
				markError ();
			}
		}
	}
}

void ATOM_ChunkIO::write (const void *buffer, unsigned size)
{
	if (size)
	{
		if (buffer)
		{
			if (_write (buffer, size) != size)
			{
				markError ();
			}
		}
		else
		{
			char ch = 0;
			while (size)
			{
				if (_write (&ch, 1) != 1)
				{
					markError ();
					return;
				}
				size--;
			}
		}
	}
}

template <class T>
T ChunkIO_ReadValue (ATOM_ChunkIO *chunkio)
{
	T val;
	chunkio->read (&val, sizeof(T));
	if (chunkio->ioError())
	{
		val = static_cast<T>(0);
	}
	return val;
}

template <class T>
void ChunkIO_WriteValue (ATOM_ChunkIO *chunkio, const T &val)
{
	chunkio->write (&val, sizeof(T));
}

int ATOM_ChunkIO::readInt (void)
{
	return ChunkIO_ReadValue<int>(this);
}

void ATOM_ChunkIO::writeInt (int value)
{
	ChunkIO_WriteValue<int>(this, value);
}

char ATOM_ChunkIO::readChar (void)
{
	return ChunkIO_ReadValue<char>(this);
}

void ATOM_ChunkIO::writeChar (char value)
{
	ChunkIO_WriteValue<char>(this, value);
}

short ATOM_ChunkIO::readShort (void)
{
	return ChunkIO_ReadValue<short>(this);
}

void ATOM_ChunkIO::writeShort (short value)
{
	ChunkIO_WriteValue<short>(this, value);
}

long ATOM_ChunkIO::readLong (void)
{
	return ChunkIO_ReadValue<long>(this);
}

void ATOM_ChunkIO::writeLong (long value)
{
	ChunkIO_WriteValue<long>(this, value);
}

long long ATOM_ChunkIO::readLongLong (void)
{
	return ChunkIO_ReadValue<long long>(this);
}

void ATOM_ChunkIO::writeLongLong (long long value)
{
	ChunkIO_WriteValue<long long>(this, value);
}

double ATOM_ChunkIO::readDouble (void)
{
	return ChunkIO_ReadValue<double>(this);
}

void ATOM_ChunkIO::writeDouble (double value)
{
	ChunkIO_WriteValue<double>(this, value);
}

int ATOM_ChunkIO::readCString (char *value)
{
	ATOM_VECTOR<char> s;
	for (;;)
	{
		char ch = readChar ();
		if (ioError ())
		{
			return -1;
		}
		s.push_back (ch);
		if (ch == '\0')
		{
			break;
		}
	}
	if (value)
	{
		strcpy (value, &s[0]);
	}
	return s.size();
}

void ATOM_ChunkIO::writeCString (const char *value)
{
	if (value)
	{
		write (value, strlen(value)+1);
	}
}

int ATOM_ChunkIO::readPascalString (char *value)
{
	int numBytes = readInt ();
	if (numBytes < 0)
	{
		return -1;
	}

	if (ioError ())
	{
		return -1;
	}

	if (!value)
	{
		return numBytes+1;
	}

	read (value, numBytes);
	if (ioError ())
	{
		return -1;
	}

	value[numBytes] = '\0';
	return numBytes+1;
}

void ATOM_ChunkIO::writePascalString (const char *value)
{
	if (value)
	{
		int len = strlen(value);
		write (&len, sizeof(int));
		if (!ioError ())
		{
			write (value, len);
		}
	}
}

bool ATOM_ChunkIO::ioError (void) const
{
	return _ioError;
}

void ATOM_ChunkIO::markError (void)
{
	_ioError = true;
}

ATOM_ChunkIO::Chunk *ATOM_ChunkIO::getTopChunk (void) const
{
	return _topChunk;
}

unsigned ATOM_ChunkIO::_writeChunksR (Chunk *parentChunk)
{
	unsigned sizeWritten = 0;

	Chunk *chunk = parentChunk->getFirstChild ();
	while (chunk)
	{
		writeInt (chunk->getSig ());
		if (ioError ())
		{
			return sizeWritten;
		}
		sizeWritten += sizeof(int);

		writeInt (chunk->getInclusiveSize ());
		if (ioError ())
		{
			return sizeWritten;
		}
		sizeWritten += sizeof(int);

		writeInt (chunk->getSize ());
		if (ioError ())
		{
			return sizeWritten;
		}
		sizeWritten += sizeof(int);

		write (chunk->getChunkStream()->getBuffer(), chunk->getChunkStream()->getSize());
		if (ioError ())
		{
			return sizeWritten;
		}
		sizeWritten += chunk->getSize ();
		sizeWritten += _writeChunksR (chunk);

		chunk = chunk->getNextSibling ();
	}

	return sizeWritten;
}

unsigned ATOM_ChunkIO::_readChunksR (Chunk *parent, unsigned size)
{
	unsigned sizeRead = 0;
	while (sizeRead < size)
	{
		int sig = readInt ();
		if (ioError ())
		{
			return sizeRead;
		}
		sizeRead += sizeof(int);

		int inclusiveSize = readInt ();
		if (ioError ())
		{
			return sizeRead;
		}
		sizeRead += sizeof(int);

		int exclusiveSize = readInt ();
		if (ioError ())
		{
			return sizeRead;
		}
		sizeRead += sizeof(int);

		if (inclusiveSize < exclusiveSize)
		{
			return false;
		}

		Chunk *subChunk = parent->appendChild ();
		if (!subChunk)
		{
			return false;
		}

		subChunk->setSig (sig);
		subChunk->getChunkStream()->resize (exclusiveSize);
		subChunk->getChunkStream()->seek (-subChunk->getChunkStream()->position());
		read (subChunk->getChunkStream()->getBuffer(), exclusiveSize);
		subChunk->getChunkStream()->seek (-exclusiveSize);
		if (ioError ())
		{
			parent->removeChild (subChunk);
			return sizeRead;
		}
		sizeRead += exclusiveSize;

		if (inclusiveSize > exclusiveSize)
		{
			unsigned subChunkTotalSize = inclusiveSize - exclusiveSize;
			unsigned subChunkSizeRead = _readChunksR (subChunk, subChunkTotalSize);
			sizeRead += subChunkSizeRead;

			if (subChunkTotalSize != subChunkSizeRead)
			{
				return sizeRead;
			}
		}
	}
	return sizeRead;
}

bool ATOM_ChunkIO::load (const char *file)
{
	_fp = ATOM_OpenFile (file, ATOM_VFS::read);
	if (_fp)
	{
		unsigned size = _fp->size ();
		_topChunk->clearChildren ();
		unsigned sizeRead = _readChunksR (_topChunk, size);
		ATOM_CloseFile (_fp);
		_fp = 0;
		return sizeRead == size;
	}

	return false;
}

bool ATOM_ChunkIO::save (const char *file)
{
	_fp = ATOM_OpenFile (file, ATOM_VFS::write);
	if (_fp)
	{
		unsigned size = _fp->size ();
		unsigned sizeWritten = _writeChunksR (_topChunk);
		ATOM_CloseFile (_fp);
		_fp = 0;
		return !ioError();
	}

	return false;
}

unsigned ATOM_ChunkIO::_read (void *buffer, unsigned size)
{
	return _fp->read (buffer, size);
}

unsigned ATOM_ChunkIO::_write (const void *buffer, unsigned size)
{
	return _fp->write (buffer, size);
}

bool ATOM_ChunkIO::_seek (int offset)
{
	return _fp->seek (offset, ATOM_VFS::current);
}

bool ATOM_ChunkIO::_iseof (void)
{
	return _fp->eof ();
}

