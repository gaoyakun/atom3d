#include "StdAfx.h"
#include "model.h"
#include "modelanimationkeyframe.h"
#include "skeleton.h"
#include "loadinterface.h"
#include "modelanimationtrack.h"
#include "instanceskeleton.h"

unsigned meshCount = 0;

#define ATOM_MAX_NUM_GPUSKIN_BONES(MaxVertexShaderConst) (((MaxVertexShaderConst)-20)/3)
#define ATOM_MAX_NUM_GPUSKIN_BONES_VS20 ATOM_MAX_NUM_GPUSKIN_BONES(256)

ATOM_Mesh::ATOM_Mesh (ATOM_Model *model)
{
	ATOM_STACK_TRACE(ATOM_Mesh::ATOM_Mesh);

	meshCount++;

	owner = model;
	diffuseColor.set (1.f, 1.f, 1.f, 1.f);
	alphablend = false;
	billboard = false;
	sky = false;
	water = false;
	uvAnimationNoIpol = false;
	fixBlending = false;
	alphaTest = false;
	doubleSide = false;
	wireFrameMode = false;
	alpharef = 0.f;
	shininess = 64;
	transparency = 1.f;
	transparencyMode = TM_NORMAL;
	hashcode = 0;
	propertyIndex = -1;
	geometry = 0;
	//vertices_sa = 0;
}

ATOM_Mesh::~ATOM_Mesh (void)
{
	ATOM_STACK_TRACE(ATOM_Mesh::~ATOM_Mesh);

	meshCount--;

	ATOM_DELETE(geometry);
	//ATOM_DELETE(vertices_sa);
}

void ATOM_Mesh::copyfrom (const ATOM_Mesh *other)
{
	ATOM_STACK_TRACE(ATOM_Mesh::copyFrom);

	diffuseColor = other->diffuseColor;
	billboardNormal = other->billboardNormal;
	billboardXAxis = other->billboardXAxis;
	worldMatrix = other->worldMatrix;
	alphaTest = other->alphaTest;
	doubleSide = other->doubleSide;
	wireFrameMode = other->wireFrameMode;
	billboard = other->billboard;
	uvAnimationNoIpol = other->uvAnimationNoIpol;
	fixBlending = other->fixBlending;
	alphablend = other->alphablend;
	alpharef = other->alpharef;
	shininess = other->shininess;
	transparency = other->transparency;
	transparencyMode = other->transparencyMode;

	weights = other->weights;
	vertices = other->vertices;
	vertices_sa = other->vertices_sa;
	//vertices_sa = other->vertices_sa ? other->vertices_sa->clone () : 0;
	colors = other->colors;
	normals = other->normals;
	texcoords = other->texcoords;
	indices = other->indices;
	diffuseTexture = other->diffuseTexture;
}

const char *ATOM_Mesh::getProperty (const char *key) const
{
	ATOM_STACK_TRACE(ATOM_Mesh::getProperty);

	if (propertyIndex == -1 || !propertySetCache)
	{
		return 0;
	}

	const ATOM_PropertySet &propset = propertySetCache->getPropertySet (propertyIndex);

	return propset.getValue (key);	
}

static const char *getMeshProperty (const char *key, ATOM_Mesh *mesh, ATOM_Model *model)
{
	const char *str = mesh->getProperty (key);
	return str ? str : model->getProperty (key);
}

unsigned modelCount = 0;

ATOM_Model::ATOM_Model (void)
{
	ATOM_STACK_TRACE(ATOM_Model::ATOM_Model);

	modelCount++;

	_aabbtree = 0;
	_properties = -1;
	_lockStruct = ATOM_NEW(ATOM_ModelLockStruct);
	_overrideColorEnable = false;
	_overrideTextureEnable = false;
	_overrideColor.set(1.f, 1.f, 1.f, 1.f);
}

ATOM_Model::~ATOM_Model (void)
{
	ATOM_STACK_TRACE(ATOM_Model::~ATOM_Model);

	modelCount--;

	ATOM_DELETE(_aabbtree);
	ATOM_DELETE(_lockStruct);
}

void ATOM_Model::initLockStruct (long group)
{
	_lockStruct->wait = false;
	_lockStruct->group = group;
}

struct VFSReadPred
{
	ATOM_File *_file;
	unsigned long _size;

	VFSReadPred (ATOM_File *file): _file(file), _size(_file->size()) {}

	bool operator () (void *buffer, unsigned long size) const
	{
		return _file && _file->read (buffer, size) == size;
	}

	bool skip (int offset)
	{
		return _file->seek (offset, ATOM_VFS::current);
	}

	unsigned long size () const
	{
		return _size;
	}
};

struct MemReadPred
{
	const char *_mem;
	unsigned long _size;
	mutable unsigned long _pos;

	MemReadPred (const void *mem, unsigned size): _mem((const char*)mem), _size(size), _pos(0) {}

	bool operator () (void *buffer, unsigned long size) const
	{
		if (_size >= _pos + size)
		{
			memcpy (buffer, _mem + _pos, size);
			_pos += size;
			return true;
		}
		return false;
	}

	bool skip (int offset)
	{
		if (offset == 0)
		{
			return true;
		}

		if (offset > 0 && _size >= _pos + offset)
		{
			_pos += offset;
			return true;
		}
		else if (offset < 0 && _pos >= -offset)
		{
			_pos -= (-offset);
			return true;
		}

		return false;
	}

	unsigned long size (void) const
	{
		return _size;
	}
};

class ATOM_MyAABBTree: public ATOM_AABBTree
{
protected:
	virtual unsigned writeFile (void *handle, const void *buffer, unsigned size) const
	{
		// no need to write now
		return 0;
	}

	virtual unsigned readFile (void *handle, void *buffer, unsigned size) const
	{
		MemReadPred *pred = (MemReadPred*)handle;

		return (*pred)(buffer, size);
		//ATOM_File *file = (ATOM_File*)handle;
		//return file->Read (buffer, size);
	}
};

static bool propCmpValue (const char *val1, const char *val2)
{
	return val1 && val2 && !_stricmp(val1, val2);
}

class ModelChunkReader: public ATOM_ChunkReader<MemReadPred>
{
	static const unsigned MeshChunkFourCC = ATOM_MAKE_FOURCC('M','E','S','H');
	static const unsigned MeshChunkFourCC2 = ATOM_MAKE_FOURCC('M','E','S','2');
	static const unsigned MeshChunkFourCC3 = ATOM_MAKE_FOURCC('M','E','S','3');
	static const unsigned MeshChunkFourCC4 = ATOM_MAKE_FOURCC('M','E','S','4');
	static const unsigned MeshChunkFourCC5 = ATOM_MAKE_FOURCC('M','E','S','5');
	static const unsigned MeshChunkFourCC6 = ATOM_MAKE_FOURCC('M','E','S','6');
	static const unsigned MeshChunkFourCC7 = ATOM_MAKE_FOURCC('M','E','S','7');
	static const unsigned MaterialChunkFourCC = ATOM_MAKE_FOURCC('M','T','R','L');
	static const unsigned MaterialChunkFourCC2 = ATOM_MAKE_FOURCC('M','T','R','2');
	static const unsigned MaterialChunkFourCC3 = ATOM_MAKE_FOURCC('M','T','R','3');
	static const unsigned MaterialChunkFourCC4 = ATOM_MAKE_FOURCC('M','T','R','4');
	static const unsigned GeometryDataChunkFourCC = ATOM_MAKE_FOURCC('G','E','O','M');
	static const unsigned MainChunkFourCC = ATOM_MAKE_FOURCC('N','3','M','F');
	static const unsigned BBoxChunkFourCC = ATOM_MAKE_FOURCC('B','B','O','X');
	static const unsigned AABBTreeChunkFourCC = ATOM_MAKE_FOURCC('A', 'B', 'T', '0');
	static const unsigned VertexWeightFourCC = ATOM_MAKE_FOURCC('W', 'G', 'H', 'T');
	static const unsigned VertexWeightFourCC2 = ATOM_MAKE_FOURCC('W', 'G', 'H', '2');
	static const unsigned AnimationCacheFourCC = ATOM_MAKE_FOURCC('A', 'C', 'A', 'C');
	static const unsigned AnimationCacheFourCC2 = ATOM_MAKE_FOURCC('A', 'C', 'A', '2');
	static const unsigned MeshAnimationDataFourCC = ATOM_MAKE_FOURCC('M', 'A', 'D', 'T');
	static const unsigned MeshAnimationDataFourCC2 = ATOM_MAKE_FOURCC('M', 'A', 'D', '2');
	static const unsigned MeshAnimationDataFourCC3 = ATOM_MAKE_FOURCC('M', 'A', 'D', '3');
	static const unsigned MeshAnimationDataFourCC4 = ATOM_MAKE_FOURCC('M', 'A', 'D', '4');
	static const unsigned SkeletonFourCC = ATOM_MAKE_FOURCC('S', 'K', 'L', 'T');
	static const unsigned SkeletonFourCC2 = ATOM_MAKE_FOURCC('S', 'K', 'L', '2');
	static const unsigned SkeletonFourCC3 = ATOM_MAKE_FOURCC('S', 'K', 'L', '3');
	static const unsigned PropertyDataCacheFourCC = ATOM_MAKE_FOURCC('P', 'R', 'P', 'C');
	static const unsigned ModelPropertiesFourCC = ATOM_MAKE_FOURCC('M', 'P', 'R', 'P');

	struct MeshInfo
	{
		int numVertices;
		int numIndices;
		int hasColor;
		int hasNormal;
		int hasTangents;
		int numChannels;
		int vertexOffset;
		int colorOffset;
		int normalOffset;
		int tangentOffset;
		int binormalOffset;
		int texcoordOffset;
		int indexOffset;
	};

	struct MeshInfo2: public MeshInfo
	{
		int isBillboard;
	};

	struct MeshInfo3: public MeshInfo2
	{
		ATOM_Matrix4x4f worldMatrix;
	};

	struct MeshInfo4: public MeshInfo3
	{
		ATOM_Mesh::TransparencyMode transparencyMode;
	};

	struct MeshInfo5: public MeshInfo4
	{
		int uvAnimationNoIpol;
	};

	struct MeshInfo6: public MeshInfo5
	{
		int fixBlending;
	};

	struct MeshInfo7: public MeshInfo6
	{
		int propHandle;
	};

	struct MaterialInfo
	{
		char diffuseMapName[256];
		char tmp[256];
		char normalMapName[256];
		float emissiveColor[3];
		float Kd;
		float Ks;
		float reserved;
		float diffuseColor[3];
		int alphaTest;
		int doubleSide;
		float alpharef;
		float shininess;
		float transparency;
		int diffuseChannel1;
		int diffuseChannel2;
		int normalmapChannel;
	};

	struct MaterialInfo2: public MaterialInfo
	{
		int materialStyle;
	};

	ATOM_Model *_model;
	ATOM_RenderDevice *_device;
	ATOM_VECTOR<unsigned short> _meshBillboardIndices;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Mesh)> _meshes;
	ATOM_VECTOR<MeshInfo7> _meshInfos;
	ATOM_VECTOR<MaterialInfo2> _materialInfos;
	ATOM_VECTOR<ATOM_VECTOR<unsigned> > _meshGroups;
	ATOM_AUTOPTR(ATOM_ModelAnimationDataCache) _animationCache;
	ATOM_AUTOPTR(ATOM_PropertySetCache) _propertyCache;
	char *_geometryBuffer;
	bool _canDoMerge;

	// for multithread loading
	bool _loadtexture;

public:
	ModelChunkReader (ATOM_Model *model, bool loadtexture = true): _model(model), _loadtexture(loadtexture)
	{
		ATOM_STACK_TRACE(ModelChunkReader::ModelChunkReader);

		_device = ATOM_GetRenderDevice();
		_geometryBuffer = 0;
		_canDoMerge = true;
	}

	~ModelChunkReader (void)
	{
		ATOM_STACK_TRACE(ModelChunkReader::~ModelChunkReader);

		ATOM_FREE(_geometryBuffer);
	}

	bool canMerge (unsigned n1, unsigned n2) const
	{
		ATOM_STACK_TRACE(ModelChunkReader::canMerge);

		const MaterialInfo2 &m1 = _materialInfos[n1];
		const MaterialInfo2 &m2 = _materialInfos[n2];
		const MeshInfo7 &mi1 = _meshInfos[n1];
		const MeshInfo7 &mi2 = _meshInfos[n2];

		return !memcmp (&m1, &m2, sizeof(MaterialInfo2)) && mi1.propHandle == mi2.propHandle && mi1.hasColor == mi2.hasColor && mi1.hasNormal == mi2.hasNormal && mi1.hasTangents == mi2.hasTangents && mi1.transparencyMode == mi2.transparencyMode && mi1.numIndices > 0 && mi2.numIndices > 0;
	}

    virtual bool readFile (MemReadPred &pred)
	{
		ATOM_STACK_TRACE(ModelChunkReader::readFile);

		if (ATOM_ChunkReader<MemReadPred>::readFile (pred))
		{
			for (unsigned t = 0; t < _model->getNumTracks (); ++t)
			{
				_model->getTrack (t)->setPropertyCache (_propertyCache.get());
			}

			if (_meshInfos.size() > 0)
			{
				if (_model->getNumTracks() || _model->getSkeleton())
				{
					_canDoMerge = false;
				}

				if (!_canDoMerge)
				{
					_meshGroups.resize (_meshInfos.size());
					for (unsigned i = 0; i < _meshGroups.size(); ++i)
					{
						_meshGroups[i].push_back (i);
					}
				}
				else
				{
					ATOM_ASSERT(_materialInfos.size() == _meshInfos.size());

					for (unsigned i = 0; i < _materialInfos.size(); ++i)
					{
						bool merged = false;

						if (!_meshInfos[i].isBillboard && !_meshInfos[i].uvAnimationNoIpol)
						{
							for (unsigned j = 0; j < _meshGroups.size(); ++j)
							{
								if (_meshInfos[_meshGroups[j][0]].isBillboard)
								{
									continue;
								}

								if (canMerge (i, _meshGroups[j][0]))
								{
									//printf ("Merged!\n");
									_meshGroups[j].push_back (i);
									merged = true;
									break;
								}
							}
						}

						if (!merged)
						{
							_meshGroups.resize (_meshGroups.size() + 1);
							_meshGroups.back().push_back (i);
						}
					}
				}

				_meshes.resize (_meshGroups.size());

				for (unsigned i = 0; i < _meshGroups.size(); ++i)
				{
					ATOM_AUTOPTR(ATOM_Mesh) mesh = _meshes[i];
					_model->addMesh (mesh.get());

					const MeshInfo7 &meshInfo = _meshInfos[_meshGroups[i][0]];
					const MaterialInfo2 &materialInfo = _materialInfos[_meshGroups[i][0]];

					mesh->propertySetCache = _propertyCache;
					mesh->propertyIndex = meshInfo.propHandle;
					mesh->diffuseTextureName = materialInfo.diffuseMapName;

					ATOM_AUTOREF(ATOM_Texture) diffuseTexture;

					if (_device)
					{
						if (materialInfo.diffuseMapName[0])
						{
							// 如果创建纹理（确保异步加载时不会加载纹理）
							if (_loadtexture)
							{
								//--- wangjian : 这里默认是同步加载纹理 ---//
								// 异步加载
								// 因为异步加载模型的时候是不会创建纹理的，如果执行到这里，那肯定是立即加载
								// 异步加载时创建纹理是在LOCK方法中进行。
								diffuseTexture = ATOM_CreateTextureResource (materialInfo.diffuseMapName,ATOM_PIXEL_FORMAT_UNKNOWN,ATOM_LoadPriority_IMMEDIATE);
								//-------------------------------------//

								if (!diffuseTexture)
								{
									ATOM_LOGGER::error ("Load texture failed <%s>.\n", materialInfo.diffuseMapName);
									diffuseTexture = ATOM_GetColorTexture (0xFFFFFFFF);
								}
								mesh->diffuseTexture = diffuseTexture;
							}
						}
					}

					mesh->billboard = meshInfo.isBillboard;
					mesh->uvAnimationNoIpol = meshInfo.uvAnimationNoIpol;
					mesh->fixBlending = meshInfo.fixBlending != 0;
					mesh->worldMatrix = meshInfo.worldMatrix;
					mesh->transparencyMode = meshInfo.transparencyMode;

					if (!mesh->diffuseTextureName.empty())
					{
						mesh->diffuseColor.set(1.f, 1.f, 1.f, 1.f);
					}
					else
					{
						mesh->diffuseColor.set(materialInfo.diffuseColor[0], materialInfo.diffuseColor[1], materialInfo.diffuseColor[2], 1.f);
					}

					if (mesh->diffuseColor.x == 0.f && mesh->diffuseColor.y == 0.f && mesh->diffuseColor.z == 0.f)
					{
						mesh->diffuseColor.x = 1.f;
						mesh->diffuseColor.y = 1.f;
						mesh->diffuseColor.z = 1.f;
					}
					mesh->alphaTest = materialInfo.alphaTest != 0;
					mesh->doubleSide = materialInfo.doubleSide != 0;
					mesh->alpharef = materialInfo.alpharef * 255.f;
					mesh->shininess = materialInfo.shininess;
					mesh->transparency = materialInfo.transparency;

					// load properties
					mesh->sky = propCmpValue (getMeshProperty ("Sky", mesh.get(), _model), "True");
					mesh->water = propCmpValue (getMeshProperty ("Water", mesh.get(), _model), "True");
					mesh->billboard = propCmpValue (getMeshProperty ("Billboard", mesh.get(), _model), "True");
					mesh->uvAnimationNoIpol = propCmpValue (getMeshProperty ("UVNoIpol", mesh.get(), _model), "True");
					mesh->dummy = propCmpValue (getMeshProperty ("Dummy", mesh.get(), _model), "True");
					mesh->castshadow = !propCmpValue (getMeshProperty ("NoCastShadow", mesh.get(), _model), "True");
					mesh->recvshadow = !propCmpValue (getMeshProperty ("NoRecvShadow", mesh.get(), _model), "True");

					const char *tm = getMeshProperty ("BlendMode", mesh.get(), _model);
					if (tm)
					{
						if (propCmpValue (tm, "Add"))
						{
							mesh->transparencyMode = ATOM_Mesh::TM_ADDITIVE;
						}
						else if (propCmpValue (tm, "AlphaAdd"))
						{
							mesh->transparencyMode = ATOM_Mesh::TM_PARTICLE;
						}
						else if (propCmpValue (tm, "Modulate"))
						{
							mesh->transparencyMode = ATOM_Mesh::TM_MODULATE;
						}
						else if (propCmpValue (tm, "ModulateB"))
						{
							mesh->transparencyMode = ATOM_Mesh::TM_MODULATE_B;
						}
						else if (propCmpValue (tm, "InvModulate"))
						{
							mesh->transparencyMode = ATOM_Mesh::TM_IMODULATE;
						}
						else if (propCmpValue (tm, "InvModulateB"))
						{
							mesh->transparencyMode = ATOM_Mesh::TM_IMODULATE_B;
						}
						else if (propCmpValue (tm, "InvNormal"))
						{
							mesh->transparencyMode = ATOM_Mesh::TM_INORMAL;
						}
						else
						{
							mesh->transparencyMode = ATOM_Mesh::TM_NORMAL;
						}
					}
					else
					{
						mesh->transparencyMode = ATOM_Mesh::TM_NORMAL;
					}


					// load vertices
					unsigned totalVertices = 0;
					unsigned totalIndices = 0;
					ATOM_VECTOR<unsigned> vertexCount(_meshGroups[i].size());
					ATOM_VECTOR<unsigned short> indexCount(_meshGroups[i].size());
					for (unsigned j = 0;j < _meshGroups[i].size(); ++j)
					{
						vertexCount[j] = _meshInfos[_meshGroups[i][j]].numVertices;
						totalVertices += vertexCount[j];
						indexCount[j] = _meshInfos[_meshGroups[i][j]].numIndices;
						totalIndices += indexCount[j];
					}

					bool hasWeight = mesh->weights.size() > 0;

					if (hasWeight)
					{
						//mesh->vertices_sa = ATOM_NEW(ATOM_SwappableRawMemory, totalVertices * sizeof(ATOM_Mesh::SkeletonVertex));
						mesh->vertices_sa.resize (totalVertices);
					}

					if (_device)
					{
						mesh->boundingBox.beginExtend();
						mesh->vertexData.resize (totalVertices);
						char *p2 = (char*)&mesh->vertexData[0];
						ATOM_Mesh::SkeletonVertex *sv = hasWeight ? &mesh->vertices_sa[0] : 0;
						ATOM_Mesh::SkeletonVertex *svv = sv;
						for (unsigned j = 0; j < _meshGroups[i].size(); ++j)
						{
							unsigned len = vertexCount[j];
							unsigned lenBytes = len * 3 * sizeof(float);
							unsigned vertexOffset = _meshInfos[_meshGroups[i][j]].vertexOffset;
							const ATOM_Vector3f *verts = (const ATOM_Vector3f*)(_geometryBuffer + vertexOffset);

							memcpy (p2, _geometryBuffer + _meshInfos[_meshGroups[i][j]].vertexOffset, lenBytes);
							p2 += lenBytes;

							if (svv)
							{
								for (unsigned k = 0; k < len; ++k)
								{
									svv->vertex.set(verts[k].x, verts[k].y, verts[k].z, 1.f);
									svv++;
								}
							}

							for (unsigned vert = 0; vert < len; ++vert)
							{
								mesh->boundingBox.extend (verts[vert]);
							}
						}

						if (_meshInfos[_meshGroups[i][0]].hasColor)
						{
							mesh->diffuseData.resize (totalVertices);
							char *p2 = (char*)&mesh->diffuseData[0];
							for (unsigned j = 0; j < _meshGroups[i].size(); ++j)
							{
								unsigned lenBytes = vertexCount[j] * sizeof(unsigned);
								unsigned colorOffset = _meshInfos[_meshGroups[i][j]].colorOffset;
								memcpy (p2, _geometryBuffer+colorOffset, lenBytes);
								p2 += lenBytes;
							}
							bool stripColor = true;
							unsigned *c = (unsigned*)&mesh->diffuseData[0];
							for (unsigned t = 0; t < totalVertices; ++t)
							{
								if (c[t] != 0xFFFFFFFF)
								{
									stripColor = false;
									break;
								}
							}
							if (stripColor)
							{
								_meshInfos[_meshGroups[i][0]].hasColor = false;
								mesh->diffuseData.clear ();
							}
						}

						if (_meshInfos[_meshGroups[i][0]].hasNormal)
						{
							mesh->normalData.resize (totalVertices);


							char *p2 = (char*)&mesh->normalData[0];
							ATOM_Mesh::SkeletonVertex *svn = sv;
							for (unsigned j = 0; j < _meshGroups[i].size(); ++j)
							{
								unsigned len = vertexCount[j];
								unsigned lenBytes = len * sizeof(float) * 3;
								unsigned normalOffset = _meshInfos[_meshGroups[i][j]].normalOffset;
								const ATOM_Vector3f *normals = (const ATOM_Vector3f*)(_geometryBuffer+normalOffset);

								memcpy (p2, normals, lenBytes);
								p2 += lenBytes;
								if (svn)
								{
									for (unsigned k = 0; k < len; ++k)
									{
										svn->normal.set(normals[k].x, normals[k].y, normals[k].z, 0.f);
										svn++;
									}
								}
							}
						}

						if (_meshInfos[_meshGroups[i][0]].hasTangents)
						{
							mesh->tangentData.resize (totalVertices);

							char *p2 = (char*)&mesh->tangentData[0];
							ATOM_Mesh::SkeletonVertex *svn = sv;
							for (unsigned j = 0; j < _meshGroups[i].size(); ++j)
							{
								unsigned len = vertexCount[j];
								unsigned lenBytes = len * sizeof(float) * 3;
								unsigned tangentOffset = _meshInfos[_meshGroups[i][j]].tangentOffset;
								const ATOM_Vector3f *tangents = (const ATOM_Vector3f*)(_geometryBuffer+tangentOffset);

								memcpy (p2, tangents, lenBytes);
								p2 += lenBytes;
								if (svn)
								{
									for (unsigned k = 0; k < len; ++k)
									{
										svn->tangent.set(tangents[k].x, tangents[k].y, tangents[k].z, 0.f);
										svn++;
									}
								}
							}
						}

						if (_meshInfos[_meshGroups[i][0]].hasTangents)
						{
							mesh->binormalData.resize (totalVertices);


							char *p2 = (char*)&mesh->binormalData[0];
							for (unsigned j = 0; j < _meshGroups[i].size(); ++j)
							{
								unsigned len = vertexCount[j];
								unsigned lenBytes = len * sizeof(float) * 3;
								unsigned binormalOffset = _meshInfos[_meshGroups[i][j]].binormalOffset;
								const ATOM_Vector3f *binormals = (const ATOM_Vector3f*)(_geometryBuffer+binormalOffset);

								memcpy (p2, binormals, lenBytes);
								p2 += lenBytes;
							}
						}

						if (meshInfo.numChannels > 0)
						{
							mesh->texcoordData.resize (totalVertices);
							char *p2 = (char*)&mesh->texcoordData[0];
							for (unsigned j = 0; j < _meshGroups[i].size(); ++j)
							{
								unsigned lenBytes = vertexCount[j] * sizeof(float) * 2;
								memcpy (p2, _geometryBuffer+_meshInfos[_meshGroups[i][j]].texcoordOffset, lenBytes);
								p2 += lenBytes;
							}
						}

						if (meshInfo.numIndices > 0)
						{
							mesh->indexData.resize (totalIndices);
							unsigned short *p2 = &mesh->indexData[0];
							unsigned short base = 0;
							for (unsigned j = 0; j < _meshGroups[i].size(); ++j)
							{
								unsigned len = indexCount[j];
								const unsigned short *pIndices = (const unsigned short*)(_geometryBuffer+_meshInfos[_meshGroups[i][j]].indexOffset);
								for (unsigned index = 0; index < indexCount[j]; ++index)
								{
									unsigned short val = (*pIndices++) + base;
									*p2++ = val;
								}
								base += vertexCount[j];
							}
						}
					}

					if (mesh->billboard)
					{
						unsigned short v0 = 0;
						unsigned short v1 = 1;
						unsigned short v2 = 2;
						if (meshInfo.numIndices > 0)
						{
							const unsigned short *indices = (const unsigned short*)(_geometryBuffer+_meshInfos[_meshGroups[i][0]].indexOffset);
							v0 = indices[0];
							v1 = indices[1];
							v2 = indices[2];
						}
						_meshBillboardIndices[3*i+0] = v0;
						_meshBillboardIndices[3*i+1] = v1;
						_meshBillboardIndices[3*i+2] = v2;

						const ATOM_Vector3f *verts = (const ATOM_Vector3f*)(_geometryBuffer + _meshInfos[_meshGroups[i][0]].vertexOffset);

						mesh->billboardNormal = crossProduct (verts[v1]-verts[v0], verts[v2]-verts[v0]);
						mesh->billboardNormal.normalize ();
						mesh->billboardXAxis = verts[v1]-verts[v0];
						mesh->billboardXAxis.normalize ();
						mesh->billboardCenter.set (0.f, 0.f, 0.f);
						for (unsigned vert = 0; vert < meshInfo.numVertices; ++vert)
						{
							mesh->billboardCenter += verts[vert];
						}
						mesh->billboardCenter /= float(meshInfo.numVertices);

						for (unsigned track = 0; track < _model->getNumTracks(); ++track)
						{
							_model->getTrack(track)->computeBillboardNormal (i, &_meshBillboardIndices[i*3], _meshInfos[i].worldMatrix);
						}
					}

					mesh->initJointMask ();

					// prepare for GPU skin animation
					if (!ATOM_RenderSettings::isFixedFunction() && !mesh->weights.empty ())
					{
						int minindex = 9999;
						int maxindex = 0;
						for (int i = 0; i < mesh->weights.size(); ++i)
						{
							const ATOM_Mesh::VertexWeight &w = mesh->weights[i];
							if (w.weight != 0)
							{
								if (w.jointIndex < minindex) minindex = w.jointIndex;
								if (w.jointIndex > maxindex) maxindex = w.jointIndex;
							}
						}

						if (minindex > maxindex)
						{
							minindex = 0;
							maxindex = 0;
						}
						ATOM_VECTOR<int> jointBitArray(maxindex-minindex+1);
						// initialized to -1
						memset (&jointBitArray[0], 0xFF, sizeof(int)*jointBitArray.size());

						int numJoints = 0;
						unsigned numBones = ATOM_MAX_NUM_GPUSKIN_BONES(ATOM_RenderSettings::getRenderCaps().maxVertexShaderConst);
						mesh->jointMap.resize(numBones);
						for (int i = 0; i < mesh->weights.size(); ++i)
						{
							const ATOM_Mesh::VertexWeight &w = mesh->weights[i];
							if (w.weight == 0.f)
							{
								continue;
							}

							if (jointBitArray[w.jointIndex-minindex] < 0)
							{
								mesh->jointMap[numJoints] = w.jointIndex;
								jointBitArray[w.jointIndex-minindex] = numJoints;
								++numJoints;

								if (numJoints >= numBones)
								{
									// Too many joints, cannot use hardware skinning
									mesh->jointMap.clear();
									break;
								}
							}
						}

						if (!mesh->jointMap.empty())
						{
							// Ok, we can use hardware skinning
							mesh->jointMap.resize (numJoints);
							for (int i = 0; i < mesh->weights.size(); ++i)
							{
								if (mesh->weights[i].weight == 0)
								{
									mesh->weights[i].jointIndex = 0;
								}
								else
								{
									mesh->weights[i].jointIndex = jointBitArray[mesh->weights[i].jointIndex-minindex];
								}
							}
						}
					}
				}
			}
			return true;
		}

		return false;
	}

	virtual ReadOp onChunkFound (unsigned fourcc, unsigned size, MemReadPred &pred)
	{
		ATOM_STACK_TRACE(ModelChunkReader::onChunkFound);

		if (!_model) return RO_CANCELED;

		switch (fourcc)
		{
		case MainChunkFourCC:
			{
				int version;
				if (!pred (&version, sizeof(int)))
					return RO_CANCELED;
				if (version > 2)
				{
					ATOM_LOGGER::error ("ERR: Unsupported model version: %d\n", version);
					return RO_CANCELED;
				}
				return read (pred, size - sizeof(int)) ? RO_PROCESSED : RO_CANCELED;
			}
		case SkeletonFourCC3:
			{
				_canDoMerge = false;

				unsigned chunkVersion;
				if (!pred (&chunkVersion, sizeof(unsigned)))
					return RO_CANCELED;

				unsigned numSkeletons;
				if (!pred(&numSkeletons, sizeof(unsigned)))
					return RO_CANCELED;

				for (unsigned i = 0; i < numSkeletons; ++i)
				{
					ATOM_AUTOPTR(ATOM_Skeleton) skeleton = ATOM_NEW(ATOM_Skeleton);
					unsigned hash;
					if (!pred(&hash, sizeof(unsigned)))
						return RO_CANCELED;
					skeleton->setHashCode (hash);

					unsigned numJoints;
					if (!pred(&numJoints, sizeof(unsigned)))
						return RO_CANCELED;

					skeleton->setNumJoints (numJoints);
#if 1
					ATOM_VECTOR<ATOM_Matrix3x4f> &vecMat = skeleton->getJointMatrices();
					ATOM_Matrix4x4f m;
					for (unsigned i = 0; i < numJoints; ++i)
					{
						if (!pred(&m, sizeof(float)*16))
						{
							return RO_CANCELED;
						}
						vecMat[i] = m;
					}
#else
					ATOM_VECTOR<ATOM_Matrix4x4f> &vecMat = skeleton->getJointMatrices();
					if (!pred(&vecMat[0], numJoints * sizeof(float) * 16))
						return RO_CANCELED;
#endif

					ATOM_VECTOR<int> &parents = skeleton->getJointParents();
					if (!pred(&parents[0], numJoints * sizeof(int)))
						return RO_CANCELED;

					for (unsigned j = 0; j < numJoints; ++j)
					{
						char buffer[64];
						if (!pred(buffer, 64))
							return RO_CANCELED;

						skeleton->addAttachPoint (buffer, j);
					}

					_model->setSkeleton (skeleton.get());
				}

				return RO_PROCESSED;
			}
		case SkeletonFourCC:
		case SkeletonFourCC2:
			{
				ATOM_LOGGER::error ("Skeleton format wrong, please re-export!\n");
				return RO_CANCELED;

				_canDoMerge = false;

				unsigned chunkVersion;
				if (!pred (&chunkVersion, sizeof(unsigned)))
					return RO_CANCELED;

				unsigned numSkeletons;
				if (!pred(&numSkeletons, sizeof(unsigned)))
					return RO_CANCELED;

				for (unsigned i = 0; i < numSkeletons; ++i)
				{
					ATOM_AUTOPTR(ATOM_Skeleton) skeleton = ATOM_NEW(ATOM_Skeleton);
					unsigned hash;
					if (!pred(&hash, sizeof(unsigned)))
						return RO_CANCELED;
					skeleton->setHashCode (hash);

					unsigned numJoints;
					if (!pred(&numJoints, sizeof(unsigned)))
						return RO_CANCELED;

					skeleton->setNumJoints (numJoints);
#if 1
					ATOM_VECTOR<ATOM_Matrix3x4f> &vecMat = skeleton->getJointMatrices();
					ATOM_Matrix4x4f m;
					for (unsigned i = 0; i < numJoints; ++i)
					{
						if (!pred(&m, sizeof(float) * 16))
						{
							return RO_CANCELED;
						}
						vecMat[i] = m;
					}
#else
					ATOM_VECTOR<ATOM_Matrix4x4f> &vecMat = skeleton->getJointMatrices();
					if (!pred(&vecMat[0], numJoints * sizeof(float) * 16))
						return RO_CANCELED;
#endif

					unsigned numAttachPoints;
					if (!pred(&numAttachPoints, sizeof(unsigned)))
						return RO_CANCELED;

					for (unsigned j = 0; j < numAttachPoints; ++j)
					{
						char buffer[64];
						if (!pred(buffer, 64))
							return RO_CANCELED;

						unsigned jointIndex;
						if (!pred(&jointIndex, sizeof(unsigned)))
							return RO_CANCELED;

						skeleton->addAttachPoint (buffer, jointIndex);
					}

					unsigned numActionMasks;
					if (!pred(&numActionMasks, sizeof(unsigned)))
						return RO_CANCELED;

					for (unsigned j = 0; j < numActionMasks; ++j)
					{
						char buffer[64];
						if (!pred(buffer, 64))
							return RO_CANCELED;

						ATOM_VECTOR<unsigned> jointIndices;
						//ATOM_VECTOR<unsigned> &jointIndices = skeleton->addActionMask (buffer);

						unsigned numMaskJoints;
						if (!pred(&numMaskJoints, sizeof(unsigned)))
							return RO_CANCELED;

						jointIndices.resize (numMaskJoints);
						if (!pred(&jointIndices[0], numMaskJoints * sizeof(unsigned)))
							return RO_CANCELED;
					}

					_model->setSkeleton (skeleton.get());
				}

				return RO_PROCESSED;
			}
		case AnimationCacheFourCC:
		case AnimationCacheFourCC2:
			{
				_canDoMerge = false;

				_animationCache = ATOM_NEW(ATOM_ModelAnimationDataCache);
				unsigned numJointFrames = 0; 
				ATOM_VECTOR<unsigned> jointCounts;
				ATOM_VECTOR<const void*> jointStreams;
				unsigned numUVFrames = 0;
				ATOM_VECTOR<unsigned> uvCounts;
				ATOM_VECTOR<const void*> uvStreams;
				unsigned numVertexFrames = 0;
				ATOM_VECTOR<unsigned> vertexCounts;
				ATOM_VECTOR<const void*> vertexStreams;
				unsigned numTransforms = 0;
				const ATOM_Matrix4x4f *transformStream = 0;

				if (!pred(&numVertexFrames, sizeof(unsigned)))
					return RO_CANCELED;

				if (numVertexFrames)
				{
					vertexCounts.resize (numVertexFrames);
					vertexStreams.resize (numVertexFrames);
					for (unsigned i = 0; i < numVertexFrames; ++i)
					{
						unsigned count;
						if (!pred(&count, sizeof(unsigned)))
							return RO_CANCELED;

						vertexCounts[i] = count;
						vertexStreams[i] = pred._mem + pred._pos;
						pred._pos += count * sizeof(ATOM_Vector3f);
					}
				}

				if (!pred(&numUVFrames, sizeof(unsigned)))
					return RO_CANCELED;

				if (numUVFrames)
				{
					uvCounts.resize (numUVFrames);
					uvStreams.resize (numUVFrames);
					for (unsigned i = 0; i < numUVFrames; ++i)
					{
						unsigned count;
						if (!pred(&count, sizeof(unsigned)))
							return RO_CANCELED;

						uvCounts[i] = count;
						uvStreams[i] = pred._mem + pred._pos;
						pred._pos += count * sizeof(ATOM_Vector2f);
					}
				}

				if (!pred(&numJointFrames, sizeof(unsigned)))
					return RO_CANCELED;

				if (numJointFrames)
				{
					jointCounts.resize (numJointFrames);
					jointStreams.resize (numJointFrames);
					for (unsigned i = 0; i < numJointFrames; ++i)
					{
						unsigned count;
						if (!pred(&count, sizeof(unsigned)))
							return RO_CANCELED;

						jointCounts[i] = count;
						jointStreams[i] = pred._mem + pred._pos;
						pred._pos += count * sizeof(float) * 10;
					}
				}
				unsigned numBBoxElements;
				if (!pred(&numBBoxElements, sizeof(unsigned)))
					return RO_CANCELED;

				if (numBBoxElements)
				{
					if (!pred.skip (numBBoxElements * sizeof(ATOM_BBox)))
						return RO_CANCELED;
				}

				if (fourcc == AnimationCacheFourCC2)
				{
					if (!pred(&numTransforms, sizeof(unsigned)))
						return RO_CANCELED;

					if (numTransforms)
					{
						transformStream = (const ATOM_Matrix4x4f*)(pred._mem + pred._pos);
						pred._pos += numTransforms * sizeof(ATOM_Matrix4x4f);
					}
				}

				_animationCache->create (numJointFrames, jointCounts, jointStreams, numUVFrames, uvCounts, uvStreams, numVertexFrames, vertexCounts, vertexStreams, numTransforms, transformStream);

				return RO_PROCESSED;
			}
		case MeshAnimationDataFourCC:
		case MeshAnimationDataFourCC2:
		case MeshAnimationDataFourCC3:
		case MeshAnimationDataFourCC4:
			{
				_canDoMerge = false;

				ATOM_HASHMAP<unsigned, unsigned> hashes;
				ATOM_VECTOR<unsigned> hashVec;
				if (fourcc == MeshAnimationDataFourCC3 || fourcc == MeshAnimationDataFourCC4)
				{
					unsigned numJoints;
					if (!pred(&numJoints, sizeof(unsigned)))
						return RO_CANCELED;

					//ATOM_LOGGER::Log ("%d joints\n", numJoints);
					if (numJoints)
					{
						hashVec.resize (numJoints);
						if (!pred(&hashVec[0], sizeof(unsigned) * numJoints))
							return RO_CANCELED;
						for (unsigned n = 0; n < hashVec.size(); ++n)
						{
							hashes[hashVec[n]] = n;
						}
					}
				}

				unsigned numActions;
				if (!pred(&numActions, sizeof(unsigned)))
					return RO_CANCELED;

				unsigned numMeshes;
				if (!pred(&numMeshes, sizeof(unsigned)))
					return RO_CANCELED;

				ATOM_ASSERT(_meshInfos.size() == numMeshes);

				for (unsigned a = 0; a < numActions; ++a)
				{
					ATOM_AUTOPTR(ATOM_ModelAnimationTrack) track = ATOM_NEW(ATOM_ModelAnimationTrack);

					char actionName[64];
					if (!pred(actionName, 64))
						return RO_CANCELED;

					track->setName (actionName);

					unsigned numFrames;
					if (!pred(&numFrames, sizeof(unsigned)))
						return RO_CANCELED;

					track->setNumFrames (numFrames);

					unsigned hashCode;
					if (!pred(&hashCode, sizeof(unsigned)))
						return RO_CANCELED;

					track->setHashCode (hashCode);

					int propIndex = -1;
					if (fourcc == MeshAnimationDataFourCC4)
					{
						if (!pred(&propIndex, sizeof(int)))
							return RO_CANCELED;
					}
					track->setPropertyIndex (propIndex);

					// boundingbox
					int hasBoundingbox = 0;
					if (!pred(&hasBoundingbox, sizeof(int)))
						return RO_CANCELED;
					if (hasBoundingbox)
					{
						ATOM_BBox bbox;
						if (!pred(&bbox, sizeof(bbox)))
							return RO_CANCELED;
						track->setPrecomputedBoundingbox (bbox);
					}

					// TODO: 动作型别信息
					unsigned reserved;
					if (!pred(&reserved, sizeof(unsigned)))
						return RO_CANCELED;

					track->setAnimationDatas (_animationCache.get());

					for (unsigned f = 0; f < numFrames; ++f)
					{
						ATOM_ModelAnimationTrack::KeyFrame &keyframe = track->getFrame (f);
						if (!pred(&keyframe.frameTime, sizeof(unsigned)))
							return RO_CANCELED;

						keyframe.meshKeyFrames.resize (numMeshes);

						if (!pred(&keyframe.jointIndex, sizeof(int)))
							return RO_CANCELED;

						for (unsigned m = 0; m < numMeshes; ++m)
						{
							ATOM_ModelAnimationTrack::MeshKeyFrame &meshKeyFrame = keyframe.meshKeyFrames[m];
							if (!pred(&meshKeyFrame.vertexIndex, sizeof(int)))
								return RO_CANCELED;

							if (!pred(&meshKeyFrame.uvIndex, sizeof(int)))
								return RO_CANCELED;

							if (!pred(&meshKeyFrame.jointIndex, sizeof(int)))
								return RO_CANCELED;

							if (!pred(&meshKeyFrame.bboxIndex, sizeof(int)))
								return RO_CANCELED;

							if (fourcc == MeshAnimationDataFourCC2 || fourcc == MeshAnimationDataFourCC3 || fourcc == MeshAnimationDataFourCC4)
							{
								if (!pred(&meshKeyFrame.transformIndex, sizeof(int)))
									return RO_CANCELED;

								if (!pred(&meshKeyFrame.transparency, sizeof(float)))
									return RO_CANCELED;
							}
							else
							{
								meshKeyFrame.transformIndex = -1;
								meshKeyFrame.transparency = _materialInfos[m].transparency;
							}
						}
					}

					track->setHashes (hashes);
					_model->addTrack (track.get());
				}
				return RO_PROCESSED;
			}
		case VertexWeightFourCC:
		case VertexWeightFourCC2:
			{
				for (unsigned imesh = 0; imesh < _meshes.size(); ++imesh)
				{
					unsigned weightFlag;
					if (!pred(&weightFlag, sizeof(unsigned)))
						return RO_CANCELED;

					unsigned hashCode;
					if (!pred(&hashCode, sizeof(unsigned)))
						return RO_CANCELED;

					ATOM_Mesh *mesh = _meshes[imesh].get();
					mesh->hashcode  = hashCode;

					if (weightFlag)
					{
						_canDoMerge = false;
						mesh->weights.resize (_meshInfos[imesh].numVertices * 4);
						if (!pred(&mesh->weights[0], _meshInfos[imesh].numVertices * 4 * sizeof(ATOM_Mesh::VertexWeight)))
							return RO_CANCELED;
					}
				}

				if (fourcc == VertexWeightFourCC2)
				{
					unsigned numJoints;
					if (!pred(&numJoints, sizeof(unsigned)))
						return RO_CANCELED;

					ATOM_VECTOR<unsigned> hashes(numJoints);
					if (numJoints)
					{
						if (!pred(&hashes[0], sizeof(unsigned) * numJoints))
							return RO_CANCELED;

						for (unsigned imesh = 0; imesh < _meshes.size(); ++imesh)
						{
							_meshes[imesh]->jointHashes = hashes;
						}
					}
				}

				return RO_PROCESSED;
			}
		case MeshChunkFourCC:
		case MeshChunkFourCC2:
		case MeshChunkFourCC3:
		case MeshChunkFourCC4:
		case MeshChunkFourCC5:
		case MeshChunkFourCC6:
		case MeshChunkFourCC7:
			{
				_meshes.push_back (ATOM_NEW(ATOM_Mesh, _model));
				unsigned sz = _meshInfos.size() + 1;
				_meshInfos.resize (sz);
				_meshBillboardIndices.resize (sz * 3);

				MeshInfo7 &meshInfo = _meshInfos.back ();
				unsigned chunkSize = 0;

				if (fourcc == MeshChunkFourCC)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo)))
						return RO_CANCELED;
					meshInfo.isBillboard = 0;
					meshInfo.worldMatrix.makeIdentity ();
					meshInfo.transparencyMode = ATOM_Mesh::TM_NORMAL;
					meshInfo.uvAnimationNoIpol = 0;
					meshInfo.fixBlending = 0;
					meshInfo.propHandle = -1;
					chunkSize = sizeof(MeshInfo);
				}
				else if (fourcc == MeshChunkFourCC2)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo2)))
						return RO_CANCELED;
					meshInfo.worldMatrix.makeIdentity ();
					meshInfo.transparencyMode = ATOM_Mesh::TM_NORMAL;
					meshInfo.uvAnimationNoIpol = 0;
					meshInfo.fixBlending = 0;
					meshInfo.propHandle = -1;
					chunkSize = sizeof(MeshInfo2);
				}
				else if (fourcc == MeshChunkFourCC3)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo3)))
						return RO_CANCELED;
					meshInfo.transparencyMode = ATOM_Mesh::TM_NORMAL;
					meshInfo.uvAnimationNoIpol = 0;
					meshInfo.fixBlending = 0;
					meshInfo.propHandle = -1;
					chunkSize = sizeof(MeshInfo3);
				}
				else if (fourcc == MeshChunkFourCC4)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo4)))
						return RO_CANCELED;
					meshInfo.uvAnimationNoIpol = 0;
					meshInfo.fixBlending = 0;
					meshInfo.propHandle = -1;
					chunkSize = sizeof(MeshInfo4);
				}
				else if (fourcc == MeshChunkFourCC5)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo5)))
						return RO_CANCELED;
					meshInfo.fixBlending = 0;
					meshInfo.propHandle = -1;
					chunkSize = sizeof(MeshInfo5);
				}
				else if (fourcc == MeshChunkFourCC6)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo6)))
						return RO_CANCELED;
					meshInfo.propHandle = -1;
					chunkSize = sizeof(MeshInfo6);
				}
				else
				{
					if (!pred (&meshInfo, sizeof(MeshInfo7)))
						return RO_CANCELED;
					chunkSize = sizeof(MeshInfo7);
				}

				if (meshInfo.isBillboard || meshInfo.uvAnimationNoIpol || meshInfo.fixBlending)
				{
					_canDoMerge = false;
				}

				return read (pred, size - chunkSize) ? RO_PROCESSED : RO_CANCELED;
			}
		case MaterialChunkFourCC:
		case MaterialChunkFourCC2:
		case MaterialChunkFourCC3:
		case MaterialChunkFourCC4:
			{
				_materialInfos.resize (_materialInfos.size() + 1);
				MaterialInfo2 &materialInfo = _materialInfos.back ();

				if (!pred (&materialInfo, size))
					return RO_CANCELED;

				if (size != sizeof(MaterialInfo2))
				{
					if (fourcc == MaterialChunkFourCC)
					{
						materialInfo.alpharef = 0.5f;
					}
				}

				if (fourcc != MaterialChunkFourCC4)
				{
					materialInfo.Kd = 1.f;
					materialInfo.Ks = 1.f;
				}

				return RO_PROCESSED;
			}
		case GeometryDataChunkFourCC:
			{
				_geometryBuffer = (char*)ATOM_MALLOC(size);
				if (!pred (_geometryBuffer, size))
				{
					ATOM_FREE(_geometryBuffer);
					_geometryBuffer = 0;
					return RO_CANCELED;
				}
				return RO_PROCESSED;
			}
		case BBoxChunkFourCC:
			{
				return RO_SKIPPED;
			}
		case AABBTreeChunkFourCC:
			{
				if (!_model->_aabbtree)
				{
					_model->_aabbtree = ATOM_NEW(ATOM_MyAABBTree);
				}

				_model->_aabbtree->load (&pred);
				return RO_PROCESSED;
			}
		case PropertyDataCacheFourCC:
			{
				unsigned numPropSets;
				if (!pred(&numPropSets, sizeof(unsigned)))
					return RO_CANCELED;

				if (numPropSets > 0)
				{
					_propertyCache = ATOM_NEW(ATOM_PropertySetCache);
					_propertyCache->setNumPropertySets (numPropSets);
					ATOM_VECTOR<char> sbuffer1;
					ATOM_VECTOR<char> sbuffer2;

					for (unsigned i = 0; i < numPropSets; ++i)
					{
						unsigned propSetSize;
						if (!pred(&propSetSize, sizeof(unsigned)))
							return RO_CANCELED;

						unsigned numProps;
						if (!pred(&numProps, sizeof(unsigned)))
							return RO_CANCELED;

						if (numProps)
						{
							ATOM_PropertySet &propSet = _propertyCache->getPropertySet (i);
							for (unsigned p = 0; p < numProps; ++p)
							{
								unsigned len;
								if (!pred(&len, sizeof(unsigned)))
									return RO_CANCELED;

								sbuffer1.resize (len);
								if (!pred(&sbuffer1[0], len))
									return RO_CANCELED;

								if (!pred(&len, sizeof(unsigned)))
									return RO_CANCELED;

								sbuffer2.resize (len);
								if (!pred(&sbuffer2[0], len))
									return RO_CANCELED;

								propSet.setValue (&sbuffer1[0], &sbuffer2[0]);
							}
						}
					}
				}

				return RO_PROCESSED;
			}
		case ModelPropertiesFourCC:
			{
				int index;
				if (!pred(&index, sizeof(int)))
					return RO_CANCELED;

				if (index >= 0 && _propertyCache)
				{
					_model->setPropertyCache (_propertyCache.get());
					_model->setPropertyIndex (index);
				}

				return RO_PROCESSED;
			}
		default:
			{
				return RO_SKIPPED;
			}
		}

		return RO_CANCELED;
	}
};

bool ATOM_Model::initMaterials (bool batch, bool mt)
{
	/*
	ATOM_STACK_TRACE(ATOM_Model::initMaterials);

	ATOM_STACK_TRACE(ATOM_Model::initMaterials);

	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		unsigned flags = 0;

		if (_meshes[i]->transparency != 1.f)
		{
			if (_meshes[i]->transparency == 0.f)
			{
				_meshes[i]->transparency = 1.f;
			}
			flags |= ATOM_DefaultMaterialObject::SMOOTH_ALPHA;
			_meshes[i]->diffuseColor.w = _meshes[i]->transparency;
			_meshes[i]->alphablend = true;

			if (_meshes[i]->transparencyMode == ATOM_Mesh::TM_ADDITIVE)
				flags |= ATOM_MaterialObject::BLENDMODE_ADD;
			else if (_meshes[i]->transparencyMode == ATOM_Mesh::TM_PARTICLE)
				flags |= ATOM_MaterialObject::BLENDMODE_BLENDADD;
			else if (_meshes[i]->transparencyMode == ATOM_Mesh::TM_MODULATE)
				flags |= ATOM_MaterialObject::BLENDMODE_MODULATE;
			else if (_meshes[i]->transparencyMode == ATOM_Mesh::TM_MODULATE_B)
				flags |= ATOM_MaterialObject::BLENDMODE_MODULATE_B;
			else if (_meshes[i]->transparencyMode == ATOM_Mesh::TM_IMODULATE)
				flags |= ATOM_MaterialObject::BLENDMODE_IMODULATE;
			else if (_meshes[i]->transparencyMode == ATOM_Mesh::TM_IMODULATE_B)
				flags |= ATOM_MaterialObject::BLENDMODE_IMODULATE_B;
			else if (_meshes[i]->transparencyMode == ATOM_Mesh::TM_INORMAL)
				flags |= ATOM_MaterialObject::BLENDMODE_INORMAL;
		}
		else
		{
			if (_meshes[i]->transparencyMode == ATOM_Mesh::TM_ADDITIVE)
			{
				flags |= ATOM_MaterialObject::BLENDMODE_ADD;
				flags |= ATOM_MaterialObject::SMOOTH_ALPHA;
				_meshes[i]->diffuseColor.w = _meshes[i]->transparency;
				_meshes[i]->alphablend = true;
			}
			else if (_meshes[i]->transparencyMode == ATOM_Mesh::TM_MODULATE)
			{
				flags |= ATOM_MaterialObject::BLENDMODE_MODULATE;
				flags |= ATOM_MaterialObject::SMOOTH_ALPHA;
				_meshes[i]->diffuseColor.w = _meshes[i]->transparency;
				_meshes[i]->alphablend = true;
			}
			else if (_meshes[i]->transparencyMode == ATOM_Mesh::TM_IMODULATE)
			{
				flags |= ATOM_MaterialObject::BLENDMODE_IMODULATE;
				flags |= ATOM_MaterialObject::SMOOTH_ALPHA;
				_meshes[i]->diffuseColor.w = _meshes[i]->transparency;
				_meshes[i]->alphablend = true;
			}
		}

		if (_meshes[i]->alphaTest)
		{
			flags |= ATOM_MaterialObject::ALPHA_TEST;
			_meshes[i]->transparency = 1.f;
			_meshes[i]->alphablend = false;
		}

		if (_meshes[i]->doubleSide)
		{
			flags |= ATOM_MaterialObject::TWOSIDED;
		}

		if (_meshes[i]->sky)
		{
			flags |= ATOM_MaterialObject::DISABLE_ZWRITE;
		}

		const char *nolight = getProperty ("LightingMode");
		if (!nolight || _stricmp (nolight, "None"))
		{
			flags |= ATOM_MaterialObject::LIGHTING;
		}

		if (_meshes[i]->shininess != 0)
		{
			flags |= ATOM_MaterialObject::SPECULAR;
		}

		ATOM_RenderDevice *device = ATOM_GetRenderDevice();

		if (device && !_meshes[i]->initMaterials (this, flags, true, true, batch, true, !_meshes[i]->alphablend, mt ? _lockStruct : 0))
		{
			return false;
		}
	}
	*/
	return true;
}

bool ATOM_Model::memload (ATOM_RenderDevice *device, const void *mem, unsigned size, bool loadtexture, bool batch)
{
	ATOM_STACK_TRACE(ATOM_Model::memload);

	_meshes.clear ();
	_tracks.clear ();
	_skeleton = 0;
	ATOM_DELETE(_aabbtree);
	_aabbtree = 0;
	_properties = -1;
	_propertySetCache = 0;

	MemReadPred pred(mem, size);
	ModelChunkReader reader (this, loadtexture);
	if (!reader.readFile (pred))
	{
		return false;
	}

	return true;
}

bool ATOM_Model::lock (void)
{
	ATOM_STACK_TRACE(ATOM_Model::lock);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	if (!device)
	{
		return false;
	}

	_lockStruct->meshLockStructs.resize (_meshes.size());

	bool hasTrack = getNumTracks () > 0;

	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		ATOM_Mesh *mesh = _meshes[i].get();

		//----------------------------------//
		// wangjian added 
		// NM格式默认不带切线信息
		mesh->tangentData.clear();
		mesh->binormalData.clear();
		//----------------------------------//

		//--- wangjian modified ---//
		// 是否使用HWInstancing Geometry
		//bool useInterleavedArray = false;//!hasTrack && mesh->weights.empty();
#if 0
		bool useInterleavedArray = ATOM_RenderSettings::isUseHWInstancing() && ( !hasTrack && mesh->weights.empty() );
#else
		// 只要设备支持实例化
		bool useInterleavedArray = (ATOM_RenderSettings::getRenderCaps().supportInstancing) && ( !hasTrack && mesh->weights.empty() );
#endif
		//-------------------------//






		ATOM_MeshLockStruct &ms = _lockStruct->meshLockStructs[i];
		memset (&ms, 0, sizeof(ms));

		//--- wangjian addd ---//
		// 异步加载
		// 如果之前在IO线程中没有创建纹理，在这里创建纹理
		if (!mesh->diffuseTextureName.empty ())
		{
			mesh->diffuseTexture = ATOM_FindTextureResource (mesh->diffuseTextureName.c_str());
			if (!mesh->diffuseTexture)
			{
				//--- 异步加载纹理 ---//
				// 使用基本的异步加载优先级
				mesh->diffuseTexture = ATOM_CreateTextureResource (mesh->diffuseTextureName.c_str());
				if (!mesh->diffuseTexture)
				{
					ATOM_LOGGER::error ("Create texture failed <%s>.\n", mesh->diffuseTextureName.c_str());
					mesh->diffuseTexture = ATOM_GetColorTexture (0xFFFFFFFF);
				}
			}
			/*mesh->diffuseTexture = ATOM_FindTextureResource (mesh->diffuseTextureName.c_str());
			if (!mesh->diffuseTexture)
			{
				mesh->diffuseTexture = ATOM_CreateEmptyTextureResource (mesh->diffuseTextureName.c_str());
			}

			const char *mipfilter = getMeshProperty ("MipmapMode", mesh, this);
			bool skipMipmap = propCmpValue (mipfilter, "None");

			_lockStruct->depends.push_back (mesh->diffuseTexture->getLoadInterface());

			mesh->diffuseTexture->getLoadInterface()->setFileName (mesh->diffuseTextureName.c_str());
			mesh->diffuseTexture->getLoadInterface()->setSkipMipMap (skipMipmap);
			mesh->diffuseTexture->getLoadInterface()->mtload (0, _lockStruct->group, &ATOM_Model::finishRequest, _lockStruct);*/
		}
		//-----------------------//

		unsigned totalVertices = mesh->vertexData.size();


		if (useInterleavedArray)
		{
			unsigned attrib = ATOM_VERTEX_ATTRIB_COORD;

			if (!mesh->diffuseData.empty())
			{
				attrib |= ATOM_VERTEX_ATTRIB_PRIMARY_COLOR;
			}

			if (!mesh->normalData.empty())
			{
				attrib |= ATOM_VERTEX_ATTRIB_NORMAL;
			}

			if (!mesh->tangentData.empty())
			{
				attrib |= ATOM_VERTEX_ATTRIB_TANGENT;
			}

			if (!mesh->binormalData.empty())
			{
				attrib |= ATOM_VERTEX_ATTRIB_BINORMAL;
			}

			if (!mesh->texcoordData.empty())
			{
				attrib |= ATOM_VERTEX_ATTRIB_TEX1_2;
			}

			mesh->interleavedarray = device->allocVertexArray (attrib, ATOM_USAGE_STATIC, totalVertices, true);
			if (mesh->interleavedarray)
			{
				ms.interleaved_vertices = mesh->interleavedarray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			}
			if (!mesh->interleavedarray || !ms.interleaved_vertices)
			{
				return false;
			}
		}
		else
		{
			mesh->vertices = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_STATIC, totalVertices, true);
			if (mesh->vertices)
			{
				ms.vertices = mesh->vertices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			}
			if (!mesh->vertices || !ms.vertices)
			{
				return false;
			}

			if (!mesh->diffuseData.empty())
			{
				mesh->colors = device->allocVertexArray(ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, ATOM_USAGE_STATIC, totalVertices, true);
				if (mesh->colors)
				{
					ms.colors = mesh->colors->lock (ATOM_LOCK_WRITEONLY, 0, totalVertices * sizeof(unsigned), true);
				}
				if (!mesh->colors || !ms.colors)
				{
					return false;
				}
			}

			if (!mesh->normalData.empty ())
			{
				mesh->normals = device->allocVertexArray(ATOM_VERTEX_ATTRIB_NORMAL, ATOM_USAGE_STATIC, totalVertices, true);
				if (mesh->normals)
				{
					ms.normals = mesh->normals->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				}
				if (!mesh->normals || !ms.normals)
				{
					return false;
				}
			}

			if (!mesh->tangentData.empty ())
			{
				mesh->tangents = device->allocVertexArray(ATOM_VERTEX_ATTRIB_TANGENT, ATOM_USAGE_STATIC, totalVertices, true);
				if (mesh->tangents)
				{
					ms.tangents = mesh->tangents->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				}
				if (!mesh->tangents || !ms.tangents)
				{
					return false;
				}
			}

			if (!mesh->binormalData.empty ())
			{
				mesh->binormals = device->allocVertexArray(ATOM_VERTEX_ATTRIB_BINORMAL, ATOM_USAGE_STATIC, totalVertices, true);
				if (mesh->binormals)
				{
					ms.binormals = mesh->binormals->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				}
				if (!mesh->binormals || !ms.binormals)
				{
					return false;
				}
			}

			if (!mesh->texcoordData.empty ())
			{
				mesh->texcoords = device->allocVertexArray(ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_STATIC, totalVertices, true);
				if (mesh->texcoords)
				{
					ms.texcoords = mesh->texcoords->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				}
				if (!mesh->texcoords || !ms.texcoords)
				{
					return false;
				}
			}

			if (!mesh->jointMap.empty ())
			{
				// use hardware skinning
				mesh->blendweights = device->allocVertexArray(ATOM_VERTEX_ATTRIB_TEX2_4, ATOM_USAGE_STATIC, totalVertices, true);
				if (mesh->blendweights)
				{
					ms.blendweights = mesh->blendweights->lock(ATOM_LOCK_WRITEONLY, 0, 0, true);
				}
				if (!mesh->blendweights || !ms.blendweights)
				{
					return false;
				}

				mesh->blendindices = device->allocVertexArray(ATOM_VERTEX_ATTRIB_TEX3_4, ATOM_USAGE_STATIC, totalVertices, true);
				if (mesh->blendindices)
				{
					ms.blendindices = mesh->blendindices->lock(ATOM_LOCK_WRITEONLY, 0, 0, true);
				}
				if (!mesh->blendindices || !ms.blendindices)
				{
					return false;
				}
			}
		}

		if (!mesh->indexData.empty())
		{
			mesh->indices = device->allocIndexArray(ATOM_USAGE_STATIC, mesh->indexData.size(), false, true);
			if (mesh->indices)
			{
				ms.indices = (unsigned short*)mesh->indices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			}
			if (!mesh->indices || !ms.indices)
			{
				return false;
			}
		}
	}

	return true;
}

bool ATOM_Model::realize (void)
{
	ATOM_STACK_TRACE(ATOM_Model::realize);

	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		ATOM_Mesh *mesh = _meshes[i].get();
		ATOM_MeshLockStruct &ms = _lockStruct->meshLockStructs[i];

		if (ms.interleaved_vertices)
		{
			unsigned attrib = mesh->interleavedarray->getAttributes ();

			unsigned char *dst = (unsigned char*)ms.interleaved_vertices;
			for (unsigned i = 0; i < mesh->vertexData.size(); ++i)
			{
				*((ATOM_Vector3f*)dst) = mesh->vertexData[i];
				dst += sizeof(ATOM_Vector3f);

				if (attrib & ATOM_VERTEX_ATTRIB_NORMAL)
				{
					*((ATOM_Vector3f*)dst) = mesh->normalData[i];
					dst += sizeof(ATOM_Vector3f);
				}

				if (attrib & ATOM_VERTEX_ATTRIB_PRIMARY_COLOR)
				{
					*((unsigned*)dst) = mesh->diffuseData[i];
					dst += sizeof(unsigned);
				}

				if (attrib & ATOM_VERTEX_ATTRIB_TANGENT)
				{
					*((ATOM_Vector3f*)dst) = mesh->tangentData[i];
					dst += sizeof(ATOM_Vector3f);
				}

				if (attrib & ATOM_VERTEX_ATTRIB_BINORMAL)
				{
					*((ATOM_Vector3f*)dst) = mesh->binormalData[i];
					dst += sizeof(ATOM_Vector3f);
				}

				if (attrib & ATOM_VERTEX_ATTRIB_TEX1_2)
				{
					*((ATOM_Vector2f*)dst) = mesh->texcoordData[i];
					dst += sizeof(ATOM_Vector2f);
				}
			}
		}
		else
		{
			if (mesh->vertices)
			{
				memcpy (ms.vertices, &mesh->vertexData[0], mesh->vertexData.size() * 3 * sizeof(float));
			}

			if (mesh->colors)
			{
				memcpy (ms.colors, &mesh->diffuseData[0], mesh->diffuseData.size() * sizeof(unsigned));
			}

			if (mesh->normals)
			{
				memcpy (ms.normals, &mesh->normalData[0], mesh->normalData.size() * 3 * sizeof(float));
			}

			if (mesh->tangents)
			{
				memcpy (ms.tangents, &mesh->tangentData[0], mesh->tangentData.size() * 3 * sizeof(float));
			}

			if (mesh->binormals)
			{
				memcpy (ms.binormals, &mesh->binormalData[0], mesh->binormalData.size() * 3 * sizeof(float));
			}

			if (mesh->texcoords)
			{
				memcpy (ms.texcoords, &mesh->texcoordData[0], mesh->texcoordData.size() * 2 * sizeof(float));
			}

			if (mesh->blendweights && mesh->blendindices)
			{
				float *blendweights = (float*)ms.blendweights;
				float *blendindices = (float*)ms.blendindices;
				for (unsigned i = 0; i < mesh->weights.size(); ++i)
				{
					blendweights[i] = mesh->weights[i].weight;
					blendindices[i] = mesh->weights[i].jointIndex;
				}
			}
		}

		if (mesh->indices)
		{
			memcpy (ms.indices, &mesh->indexData[0], mesh->indexData.size() * sizeof(short));
		}
	}

	return true;
}

void ATOM_Model::wait (void)
{
	ATOM_STACK_TRACE(ATOM_Model::wait);

	ATOM_ModelLoadInterface *loadInterface = (ATOM_ModelLoadInterface*)getLoadInterface ();
	if (loadInterface->getLoadingState() == ATOM_LoadInterface::LS_LOADING)
	{
		ATOM_ModelLoader *loader = (ATOM_ModelLoader*)loadInterface->getLoader ();

		if (!_lockStruct->wait)
		{
			ATOM_ContentStream::setHighPriorityInterface (loadInterface);
			while (!_lockStruct->wait)
			{
				ATOM_ContentStream::processRequests ();
			}
			ATOM_ContentStream::setHighPriorityInterface (0);
		}

		for (unsigned i = 0; i < _lockStruct->depends.size(); ++i)
		{
			_lockStruct->depends[i]->insureLoadDone ();
		}
		_lockStruct->depends.clear ();
		ATOM_ContentStream::waitForInterfaceDone (loadInterface);
	}
}

void ATOM_Model::unlock (void)
{
	ATOM_STACK_TRACE(ATOM_Model::unlock);

	if (_lockStruct->meshLockStructs.size() > 0)
	{
		for (unsigned i = 0; i < _meshes.size(); ++i)
		{
			ATOM_Mesh *mesh = _meshes[i].get();

			if (mesh->interleavedarray)
			{
				ATOM_HWInstancingGeometry *geom = ATOM_NEW(ATOM_HWInstancingGeometry);
				mesh->interleavedarray->unlock ();
				geom->setStream (mesh->interleavedarray.get());
				mesh->geometry = geom;
			}
			else
			{
				ATOM_MultiStreamGeometry *geom = ATOM_NEW(ATOM_MultiStreamGeometry);

				if (mesh->vertices)
				{
					mesh->vertices->unlock ();
					geom->addStream (mesh->vertices.get());
				}

				if (mesh->colors)
				{
					mesh->colors->unlock ();
					geom->addStream (mesh->colors.get());
				}

				if (mesh->normals)
				{
					mesh->normals->unlock ();
					geom->addStream (mesh->normals.get());
				}

				if (mesh->tangents)
				{
					mesh->tangents->unlock ();
					geom->addStream (mesh->tangents.get());
				}

				if (mesh->binormals)
				{
					mesh->binormals->unlock ();
					geom->addStream (mesh->binormals.get());
				}

				if (mesh->texcoords)
				{
					mesh->texcoords->unlock ();
					geom->addStream (mesh->texcoords.get());
				}

				if (mesh->blendweights)
				{
					mesh->blendweights->unlock ();
					geom->addStream (mesh->blendweights.get());
				}

				if (mesh->blendindices)
				{
					mesh->blendindices->unlock ();
					geom->addStream (mesh->blendindices.get());
				}

				mesh->geometry = geom;
			}

			if (mesh->indices)
			{
				mesh->indices->unlock ();
				mesh->geometry->setIndices (mesh->indices.get());
			}

			

			mesh->geometry->createVertexDecl ();





			//-- wangjian modified ---//
			// 异步加载
			// 这里是之前的版本，可以不要了
			//// 如果之前在IO线程中没有创建纹理，在这里创建纹理
			//if (!mesh->diffuseTextureName.empty ())
			//{
			//	mesh->diffuseTexture = ATOM_FindTextureResource (mesh->diffuseTextureName.c_str());
			//	if (!mesh->diffuseTexture)
			//	{
			//		mesh->diffuseTexture = ATOM_CreateEmptyTextureResource (mesh->diffuseTextureName.c_str());
			//	}

			//	const char *mipfilter = getMeshProperty ("MipmapMode", mesh, this);
			//	bool skipMipmap = propCmpValue (mipfilter, "None");

			//	_lockStruct->depends.push_back (mesh->diffuseTexture->getLoadInterface());

			//	mesh->diffuseTexture->getLoadInterface()->setFileName (mesh->diffuseTextureName.c_str());
			//	mesh->diffuseTexture->getLoadInterface()->setSkipMipMap (skipMipmap);
			//	mesh->diffuseTexture->getLoadInterface()->mtload (0, _lockStruct->group, &ATOM_Model::finishRequest, _lockStruct);
			//}
			//------------------------//






			//mesh->vertexData.clear ();
			//mesh->normalData.clear ();
			//mesh->tangentData.clear ();
			//mesh->binormalData.clear ();
			//mesh->texcoordData.clear ();
			//mesh->diffuseData.clear ();
			//mesh->indexData.clear ();
		}

		_lockStruct->meshLockStructs.clear ();
	}
}

void ATOM_Model::clear (void)
{
	ATOM_STACK_TRACE(ATOM_Model::clear);

	_meshes.clear ();
	_tracks.clear ();
	_skeleton = 0;
	ATOM_DELETE(_aabbtree);
	_aabbtree = 0;
	_properties = -1;
	_propertySetCache = 0;
}

void ATOM_Model::finishRequest (ATOM_LoadingRequest *request)
{
	ATOM_STACK_TRACE(ATOM_Model::finishRequest);

	ATOM_DELETE(request);
}

void ATOM_Mesh::initJointMask (void)
{
	ATOM_STACK_TRACE(ATOM_Mesh::initJointMask);

	if (!weights.empty ())
	{
		const unsigned numJointsReserve = 128;
		jointMask.reserve (numJointsReserve);
		for (unsigned i = 0; i < weights.size(); ++i)
		{
			if (weights[i].weight > 0.1f)
			{
				int joint = weights[i].jointIndex;
				if (joint >= jointMask.size())
				{
					jointMask.resize (joint + 1);
				}
				jointMask.setBitOn (joint);
			}
		}
	}
}

bool ATOM_Model::load (ATOM_RenderDevice *device, const char *filename)
{
	ATOM_STACK_TRACE(ATOM_Model::load);

	if (filename == 0)
	{
		filename = getObjectName ();
	}

	if (!filename) 
	{
		return false;
	}

	ATOM_AutoFile file(filename, ATOM_VFS::read|ATOM_VFS::binary);
	if (!file) return false;
	unsigned size = file->size ();
	if (size == 0)
	{
		return false;
	}

	ATOM_VECTOR<char> buffer(size);
	if (file->read (&buffer[0], size) != size)
	{
		return false;
	}

	//--- wangjian modified ---//
	// 异步加载
	// 该方法用在非多线程加载中，将纹理加载标记改为false,在加载时不加载纹理
	if (memload (device, &buffer[0], size, /*false*/ true, true))
	//-------------------------//
	{
		if (!device)
		{
			return true;
		}

		if (!initMaterials (true, false))
		{
			return false;
		}

		ATOM_ModelLockStruct lockStruct;

		if (lock ())
		{
			bool ret = realize ();

			unlock ();

			if (!ret)
			{
				return false;
			}

			return true;
		}
	}

	return false;
}

//--- wangjian added for async loading ---//
// 异步加载 ： 异步加载使用该方法
bool ATOM_Model::load (const char *filename)
{
	ATOM_STACK_TRACE(ATOM_Model::load);

	if (filename == 0)
	{
		filename = getObjectName ();
	}

	if (!filename) 
	{
		return false;
	}

	ATOM_AutoFile file(filename, ATOM_VFS::read|ATOM_VFS::binary);
	if (!file) return false;
	unsigned size = file->size ();
	if (size == 0)
	{
		return false;
	}

	ATOM_VECTOR<char> buffer(size);
	if (file->read (&buffer[0], size) != size)
	{
		return false;
	}

	// 异步加载中不加载创建纹理
	// 确保异步加载时为FALSE
	if ( memload (0, &buffer[0], size, false/*true*/, true) )
		return true;

	return false;
}
//----------------------//

void ATOM_Mesh::render(ATOM_RenderDevice* device)
{
	ATOM_STACK_TRACE(ATOM_Mesh::render);

	unsigned numPrim = indices ? indices->getNumIndices() / 3 : (vertices ? vertices->getNumVertices() / 3 : 0);

	if (numPrim)
	{
		geometry->draw (device, ATOM_PRIMITIVE_TRIANGLES, numPrim, 0);
	}
	//if (vertices)
	//{
	//	device->ClearStreamSource ();
	//	device->SetStreamSource (vertices.get(), vertices->GetAttributes());

	//	unsigned vertexAttrib = mat->getVertexAttrib ();
	//	if (vertexAttrib & VERTEX_ATTRIB_PRIMARY_COLOR)
	//	{
	//		if (!colors) return;
	//		device->SetStreamSource (colors.get(), colors->GetAttributes());
	//	}

	//	if (vertexAttrib & VERTEX_ATTRIB_NORMAL)
	//	{
	//		if (!normals) return;
	//		device->SetStreamSource (normals.get(), normals->GetAttributes());
	//	}

	//	if (vertexAttrib & VERTEX_ATTRIB_TEX1_2)
	//	{
	//		if (texcoords)
	//		{
	//			device->SetStreamSource (texcoords.get(), VERTEX_ATTRIB_TEX1_2);
	//		}
	//	}
	//}
	//else if (interleavedarray)
	//{
	//	device->SetInterleavedStreamSource (interleavedarray.get(), interleavedarray->GetAttributes());
	//}

	//if (indices)
	//{
	//	device->RenderStreamsIndexed (indices.get(), ATOM_PRIMITIVE_TRIANGLES, indices->GetNumIndices() / 3, 0);
	//}
	//else
	//{
	//	device->RenderStreams (ATOM_PRIMITIVE_TRIANGLES, vertices->GetNumVertices() / 3);
	//}
}

ATOM_AABBTree *ATOM_Model::getAABBTree (void) const 
{ 
	return _aabbtree; 
}

void ATOM_Model::addTrack (ATOM_ModelAnimationTrack *track)
{
	ATOM_STACK_TRACE(ATOM_Model::addTrack);

	if (track)
	{
		for (unsigned i = 0; i < _tracks.size(); ++i)
		{
			if (_tracks[i] == track)
			{
				return;
			}
		}
		_tracks.push_back (track);
	}
}

void ATOM_Model::addMesh (ATOM_Mesh *mesh)
{
	ATOM_STACK_TRACE(ATOM_Model::addMesh);

	if (mesh)
	{
		for (unsigned i = 0; i < _meshes.size(); ++i)
		{
			if (_meshes[i] == mesh)
			{
				return;
			}
		}
		_meshes.push_back (mesh);
	}
}

void ATOM_Model::setSkeleton (ATOM_Skeleton *skeleton)
{
	ATOM_STACK_TRACE(ATOM_Model::setSkeleton);

	_skeleton = skeleton;
}

unsigned ATOM_Model::getNumTracks (void) const
{
	ATOM_STACK_TRACE(ATOM_Model::getNumTracks);

	return _tracks.size();
}

unsigned ATOM_Model::getNumMeshes (void) const
{
	ATOM_STACK_TRACE(ATOM_Model::getNumMeshes);

	return _meshes.size();
}

ATOM_ModelAnimationTrack *ATOM_Model::getTrack (unsigned index) const
{
	ATOM_STACK_TRACE(ATOM_Model::getTrack);

	return _tracks[index].get();
}

ATOM_Mesh *ATOM_Model::getMesh (unsigned index) const
{
	ATOM_STACK_TRACE(ATOM_Model::getMesh);

	return _meshes[index].get();
}

ATOM_Skeleton *ATOM_Model::getSkeleton (void) const
{
	ATOM_STACK_TRACE(ATOM_Model::getSkeleton);

	return _skeleton.get();
}

void ATOM_Model::setPropertyCache (ATOM_PropertySetCache *props)
{
	ATOM_STACK_TRACE(ATOM_Model::setPropertyCache);

	_propertySetCache = props;
}

void ATOM_Model::setPropertyIndex (int index)
{
	ATOM_STACK_TRACE(ATOM_Model::setPropertyIndex);

	_properties = index;
}

const char *ATOM_Model::getProperty (const char *key) const
{
	ATOM_STACK_TRACE(ATOM_Model::getProperty);

	if (_properties == -1 || !_propertySetCache)
	{
		return 0;
	}

	const ATOM_PropertySet &propset = _propertySetCache->getPropertySet (_properties);

	return propset.getValue (key);	
}

ATOM_LoadInterface *ATOM_Model::getLoadInterface (void)
{
	ATOM_STACK_TRACE(ATOM_Model::getLoadInterface);

	if (!_loadInterface)
	{
		_loadInterface = ATOM_NEW(ATOM_ModelLoadInterface, this);
	}
	return _loadInterface.get();
}

ATOM_ModelLockStruct *ATOM_Model::getLockStruct (void)
{
	ATOM_STACK_TRACE(ATOM_Model::getLockStruct);

	return _lockStruct;
}

