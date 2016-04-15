#include <string.h>
#include <ATOM_math.h>

#include "vcache.h"

static const int DEFAULT_VCACHE_SIZE = 20;
static const int MAX_VCACHE_SIZE = 64;
static const float FindVertexScore_CacheDecayPower = 1.5f;
static const float FindVertexScore_LastTriScore = 0.75f;
static const float FindVertexScore_ValenceBoostScale = 2.0f;
static const float FindVertexScore_ValenceBoostPower = 0.5f;

static float findVertexScore(unsigned cachePos, unsigned activeFaceCount, unsigned MaxSizeVertexCache)
{
	if (activeFaceCount == 0)
	{
		 // No tri needs this vertex!
		return -1.0f;
	}

	float Score = 0.0f;

	unsigned CachePosition = cachePos;

	if ( CachePosition == 0xFFFFFFFF)
	{
		 // Vertex is not in FIFO cache - no score.
	}
	else
	{
		if ( CachePosition < 3 )
		{
			// This vertex was used in the last triangle,
			// so it has a fixed score, whichever of the three
			// it's in. Otherwise, you can get very different
			// answers depending on whether you add
			// the triangle 1,2,3 or 3,1,2 - which is silly.
			Score = FindVertexScore_LastTriScore;
		}
		else
		{
			assert( CachePosition < MaxSizeVertexCache );

			// Points for being high in the cache.

			const float Scaler = 1.0f / ( MaxSizeVertexCache - 3 );

			Score = 1.0f - ( CachePosition - 3 ) * Scaler;

			Score = ATOM_pow ( Score, FindVertexScore_CacheDecayPower );
		}
   }

	// Bonus points for having a low number of tris still to 
	// use the vert, so we get rid of lone verts quickly.

	float ValenceBoost = ATOM_pow ((float)activeFaceCount,  -FindVertexScore_ValenceBoostPower );

	Score += FindVertexScore_ValenceBoostScale * ValenceBoost;
	return Score;
}

static unsigned findVertexCount (unsigned numFaces, const unsigned short *indices)
{
	if (numFaces == 0)
	{
		return 0;
	}

	unsigned numVertices = 0;
	for (unsigned i = 0; i < 3 * numFaces; ++i)
	{
		unsigned short index = *indices++;
		if (index > numVertices)
		{
			numVertices = index;
		}
	}

	return numVertices + 1;
}

void optimizeFacesLRU (unsigned numFaces, const unsigned short *indices, unsigned short *newIndices, unsigned cacheSize)
{
	assert(cacheSize < MAX_VCACHE_SIZE);

	if (numFaces == 0)
	{
		return;
	}

	unsigned numVertices = findVertexCount (numFaces, indices);
	unsigned *cachePos = new unsigned[numVertices];
	memset (cachePos, 0xFF, numVertices * sizeof(unsigned));

	unsigned cache[MAX_VCACHE_SIZE+3];
	for (unsigned i = 0; i < cacheSize; ++i)
	{
		cache[i] = 0xFFFFFFFF;
	}

	unsigned *faceRemap = new unsigned[numFaces];
	for (unsigned i = 0; i < numFaces; ++i)
	{
		faceRemap[i] = 0xFFFFFFFF;
	}

	unsigned *activeFaceCount = new unsigned[numVertices];
	memset (activeFaceCount, 0, numVertices * sizeof(unsigned));

	for (unsigned i = 0; i < numFaces * 3; ++i)
	{
		activeFaceCount[indices[i]]++;
	}

	float *vertexScore = new float[numVertices];
	for (unsigned i = 0; i < numVertices; ++i)
	{
		vertexScore[i] = findVertexScore (cachePos[i], activeFaceCount[i], cacheSize);
	}

	unsigned count = 0;
	while (count < numFaces)
	{
		unsigned best = 0xFFFFFFFF;
		float bestScore;

		for (unsigned i = 0; i < numFaces; ++i)
		{
			if (faceRemap[i] != 0xFFFFFFFF) 
			{
				continue;
			}
		     
			const unsigned short* tri = &indices[i*3];
		     
			float thisScore = vertexScore[tri[0]] + vertexScore[tri[1]] + vertexScore[tri[2]];
		      
			if (best == 0xFFFFFFFF || thisScore > bestScore)
			{
				best = i;
				bestScore = thisScore;
			} 
		}

		faceRemap[best] = count;

		const unsigned short *tri = &indices[best * 3];
		assert(activeFaceCount[tri[0]] > 0);
		assert(activeFaceCount[tri[1]] > 0);
		assert(activeFaceCount[tri[2]] > 0);
		activeFaceCount[tri[0]]--;
		activeFaceCount[tri[1]]--;
		activeFaceCount[tri[2]]--;

		unsigned cp[3];
		cp[0] = cachePos[tri[0]];
		cp[1] = cachePos[tri[1]];
		cp[2] = cachePos[tri[2]];

		for (unsigned j = 0; j < cacheSize; ++j)
		{
			if (cache[j] == 0xFFFFFFFF)
			{
				continue;
			}

			cachePos[cache[j]] = 0xFFFFFFFF;
		}

		unsigned pushOutCount = 0;
		for (unsigned j = 0; j < 3; ++j)
		{
			if (cp[j] != 0xFFFFFFFF)
			{
				memmove (&cache[cp[j]], &cache[cp[j]+1], (cacheSize + 3 - (cp[j]+1)) * sizeof(unsigned));

				for (unsigned k = j + 1; k < 3; ++k)
				{
					if (cp[k] != 0xFFFFFFFF && cp[k] >= cp[j]+1)
					{
						cp[k]--;
					}
				}
			}
			else
			{
				pushOutCount++;
			}
		}

		memmove (&cache[3], &cache[0], cacheSize * sizeof(unsigned));

		cache[0] = tri[2];
		cache[1] = tri[1];
		cache[2] = tri[0];

		for (unsigned j = 0; j < cacheSize; ++j)
		{
			if (cache[j] == 0xFFFFFFFF)
			{
				continue;
			}

			cachePos[cache[j]] = j;
		}

		for (unsigned j = 0; j < cacheSize + pushOutCount; ++j)
		{
			unsigned index = cache[j];
			if (index == 0xFFFFFFFF)
			{
				continue;
			}
			vertexScore[index] = findVertexScore (cachePos[index], activeFaceCount[index], cacheSize);
		}

		count++;
	}

	for (unsigned i = 0; i < numFaces; ++i)
	{
		newIndices[faceRemap[i]*3] = indices[i*3];
		newIndices[faceRemap[i]*3+1] = indices[i*3+1];
		newIndices[faceRemap[i]*3+2] = indices[i*3+2];
	}

	delete [] faceRemap;
	delete [] cachePos;
	delete [] activeFaceCount;
	delete [] vertexScore;
}

unsigned optimizeVertices (unsigned numFaces, const unsigned short *indices, unsigned *vertexRemap)
{
	unsigned numVertices = findVertexCount (numFaces, indices);
	memset (vertexRemap, 0xFF, numVertices * sizeof(unsigned));

	unsigned count = 0;
	for (unsigned i = 0; i < numFaces * 3; ++i)
	{
		unsigned index = indices[i];

		if (vertexRemap[index] == 0xFFFFFFFF)
		{
			vertexRemap[index] = count;
			++count;
		}
	}

	return count;
}

void remapVertexArray (void *arrayToBeRemapped, unsigned elementSize, unsigned numElements, unsigned *remap)
{
	char *p = (char*)arrayToBeRemapped;

	char *tmp = (char*)malloc(elementSize * numElements);
	memcpy (tmp, arrayToBeRemapped, elementSize * numElements);

	for (unsigned i = 0; i < numElements; ++i)
	{
		if (remap[i] != 0xFFFFFFFF)
		{
			assert(remap[i] < numElements);
			memcpy (p + remap[i] * elementSize, tmp + i * elementSize, elementSize);
		}
	}

	free (tmp);
}

void remapIndexArray (unsigned short *arrayToBeRemapped, unsigned *remap, unsigned numIndices)
{
	for (unsigned i = 0; i < numIndices; ++i)
	{
		arrayToBeRemapped[i] = remap[arrayToBeRemapped[i]];
	}
}

