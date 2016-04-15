#ifndef __ATOM_UTILS_CHUNKIO_H
#define __ATOM_UTILS_CHUNKIO_H

#include <stdio.h>

#define ATOM_MAKE_FOURCC(a, b, c, d) \
	( ((unsigned)d) | ( ((unsigned)c) << 8 )  | ( ((unsigned)b) << 16 ) | ( ((unsigned)a) << 24 ) )

template <class writePred>
bool ATOM_ChunkWrite (const void *chunk, unsigned fourcc, unsigned size, writePred pred)
{
  return pred (&fourcc, sizeof(unsigned)) && pred (&size, sizeof(unsigned)) && pred (chunk, size);
}

template <class readPred>
bool ATOM_ChunkRead (void *c, unsigned size, readPred pred)
{
  return pred (c, size);
}

template <class readPred>
class ATOM_ChunkReader
{
public:
	enum ReadOp
	{
		RO_PROCESSED,
		RO_SKIPPED,
		RO_CANCELED
	};

public:
  virtual bool readFile (readPred &pred)
  {
	  return read (pred, 0);
  }

  bool read (readPred &pred, unsigned size = 0)
  {
	unsigned totalsize = size ? size : pred.size ();
	if (totalsize < 2 * sizeof(unsigned)) return false;
	unsigned sizeread = 0;
	while (sizeread < totalsize)
	{
	  unsigned fourcc;
	  unsigned chunksize;
	  if (!pred (&fourcc, sizeof(unsigned)) || !pred(&chunksize, sizeof(unsigned)))
		return false;
	  sizeread += 2 * sizeof(unsigned);

	  ReadOp ro = onChunkFound (fourcc, chunksize, pred);
	  switch(ro)
	  {
	  case RO_CANCELED:
		return false;
		break;
	  case RO_SKIPPED:
		if (!pred.skip (chunksize)) return false;
		break;
	  case RO_PROCESSED:
		break;
	  default:
		  return false;
	  }
	  sizeread += chunksize;
	}
	return true;
  }
  virtual ReadOp onChunkFound (unsigned fourcc, unsigned size, readPred &pred) = 0;
};
			
#endif // __ATOM_UTILS_CHUNKIO_H
