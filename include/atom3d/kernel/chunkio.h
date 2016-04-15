#ifndef __ATOM3D_KERNEL_CHUNKIO_H
#define __ATOM3D_KERNEL_CHUNKIO_H

#include "basedefs.h"

class ATOM_KERNEL_API ATOM_ChunkStream
{
public:
	ATOM_ChunkStream (void);
	~ATOM_ChunkStream (void);

public:
	void read (void *buffer, unsigned size);
	void write (const void *buffer, unsigned size);
	float readFloat (void);
	void writeFloat (float value);
	int readInt (void);
	void writeInt (int value);
	char readChar (void);
	void writeChar (char value);
	short readShort (void);
	void writeShort (short value);
	long readLong (void);
	void writeLong (long value);
	long long readLongLong (void);
	void writeLongLong (long long value);
	double readDouble (void);
	void writeDouble (double value);
	int readCString (char *value);
	void writeCString (const char *value);
	void seek (int offset);
	int position (void) const;
	bool error (void) const;
	bool eof (void) const;
	void resize (unsigned size);
	unsigned getSize (void) const;
	void *getBuffer (void) const;

private:
	char *_data;
	char *_pos;
	unsigned _size;
	unsigned _cap;
	bool _error;
};

class ATOM_KERNEL_API ATOM_ChunkIO
{
public:
	class ATOM_KERNEL_API Chunk
	{
	public:
		Chunk (void);
		virtual ~Chunk (void);

	public:
		void setSig (unsigned sig);
		unsigned getSig (void) const;
		Chunk *getFirstChild (void) const;
		Chunk *getFirstChild (unsigned sig) const;
		Chunk *getNextSibling (void) const;
		Chunk *getNextSibling (unsigned sig) const;
		Chunk *getParent (void) const;
		Chunk *prependChild (void);
		Chunk *appendChild (void);
		void removeChild (Chunk *child);
		void clearChildren (void);
		unsigned getSize (void) const;
		unsigned getInclusiveSize (void) const;
		ATOM_ChunkStream *getChunkStream (void) const;

	private:
		unsigned _sig;
		Chunk *_parent;
		Chunk *_firstChild;
		Chunk *_nextSibling;
		ATOM_ChunkStream *_stream;
	};

public:
	ATOM_ChunkIO (void);
	virtual ~ATOM_ChunkIO (void);

protected:
	virtual unsigned _read (void *buffer, unsigned size);
	virtual unsigned _write (const void *buffer, unsigned size);
	virtual bool _seek (int offset);
	virtual bool _iseof (void);

public:
	void read (void *buffer, unsigned size);
	void write (const void *buffer, unsigned size);
	int readInt (void);
	void writeInt (int value);
	char readChar (void);
	void writeChar (char value);
	short readShort (void);
	void writeShort (short value);
	long readLong (void);
	void writeLong (long value);
	long long readLongLong (void);
	void writeLongLong (long long value);
	double readDouble (void);
	void writeDouble (double value);
	int readCString (char *value);
	void writeCString (const char *value);
	int readPascalString (char *value);
	void writePascalString (const char *value);
	bool ioError (void) const;
	Chunk *getTopChunk (void) const;

private:
	unsigned _readChunksR (Chunk *parent, unsigned size);
	unsigned _writeChunksR (Chunk *chunk);

public:
	virtual bool load (const char *file);
	virtual bool save (const char *file);
	virtual void markError (void);
	
private:
	Chunk *_topChunk;
	bool _ioError;
	ATOM_File *_fp;
};

#endif // __ATOM3D_KERNEL_CHUNKIO_H
