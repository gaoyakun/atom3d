
#ifndef ES_API_H
#define ES_API_H

struct TerrainMesh
{
  int vertices;
  int faces;
  float* pVertices;
  int* pIndices;
  float* pNormals;
  float* pUVs;

  TerrainMesh()
  {
    vertices = 0;
    faces = 0;
    pVertices = NULL;
    pIndices = NULL;
    pNormals = NULL;
    pUVs = NULL;
  }
};



//system
ES_API bool   IsESRegistered();
ES_API float  GetESVersionNumber();

//progress bar
ES_API void   ProgressBar_Create();
ES_API void   ProgressBar_Destroy();
// percent is from 0.0f - 100.0f
ES_API void   ProgressBar_Set(float percent); 
ES_API void   ProgressBar_Add(float percent); 

//set the text in the status bar
ES_API void   SystemSetStatusText(char* in_sText);

//gets exact position at x,y (right to the triangle)
ES_API float  GetTerrainHeightAtPosition(float x, float y);
ES_API float  GetTerrainAngleAtPosition(float x, float y);


ES_API void   GetOpimizedTerrainMesh(float lod, TerrainMesh& out_Mesh, bool bNormals, bool bUVs); //create an optimized terrain mesh of current terrain and given LOD
ES_API void   FreeOptimizedMesh(TerrainMesh& in_mesh); //deallocate TerrainMesh

ES_API void   GetTerrainTextures(BYTE* out_pImgBuffer, int in_size, bool in_bColorMap,bool in_bDetailMap,  bool in_bLightMap); 

#endif