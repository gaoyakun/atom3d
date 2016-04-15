#ifndef __ATOM_GEOMETRY_VCACHE_H
#define __ATOM_GEOMETRY_VCACHE_H

#if _MSC_VER > 1000
# pragma once
#endif

void optimizeFacesLRU (unsigned numFaces, const unsigned short *indices, unsigned short *newIndices, unsigned cacheSize);
unsigned optimizeVertices (unsigned numFaces, const unsigned short *indices, unsigned *vertexRemap);
void remapVertexArray (void *arrayToBeRemapped, unsigned elementSize, unsigned numElements, unsigned *remap);
void remapIndexArray (unsigned short *arrayToBeRemapped, unsigned *remap, unsigned numIndices);

#endif // __ATOM_GEOMETRY_VCACHE_H
