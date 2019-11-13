#ifndef __ATOM3D_ENGINE_MODEL2_H
#define __ATOM3D_ENGINE_MODEL2_H

#include "../ATOM_render.h"
#include "basedefs.h"
#include "skeleton.h"
#include "animation_data_cache.h"
#include "joint_animation_track.h"

//--- wangjian added ---//
// 异步加载： 异步加载相关类
#include "ayncLoader.h"
class ATOM_SharedModel;
class ATOM_AsyncableLoadModel : public ATOM_AsyncableLoadObject
{
public:
	ATOM_AsyncableLoadModel();
	virtual ~ATOM_AsyncableLoadModel();

	//-----------------------//
	virtual bool Load();
	virtual bool Lock(bool & needRecurse);
	virtual bool Fill();
	virtual bool Unlock();
	//-----------------------//
	virtual bool OnloadEnd();
	virtual void SetLoadAllFinished();
	virtual bool IsLoadAllFinished();
	virtual void GarbageCollect();
	//-----------------------//
	virtual void processAllDependents();
	//-----------------------//

	void SetModel( ATOM_SharedModel * model, int version = 2 );
	void SetModelFlag(unsigned flag);
	unsigned GetModelFlag(void);
	void AddMeshIndexBuffer( void * stream_buffer, int offset, int index_count );
	void AddMeshVertexBufferStream( int meshid, 
									unsigned int attriid, 
									void * stream_buffer, 
									int offset, 
									int vertexSize, 
									int vertexcount,
									unsigned attribFlag = 0 );
	void AddMeshMaterialParam( void * material_buffer, ATOM_STRING corematfile );
	void AddDependent(ATOM_STRING dep, ATOM_ParameterTable::ValueHandle texHandle = 0);

protected:
	
	union 
	{
		ATOM_ChunkIO * _chunkIO;
		ATOM_Model * _model;
	}m_modelData;
	int m_version;

	unsigned m_modelFlag;
	//int stage;
	struct sDeviceIndexBufferFillParam
	{
		void *	_stream_buffer;
		int		_offset;
		int		_size;
		void *	_ib_buffer_data;
		sDeviceIndexBufferFillParam( void * stream_buffer, int offset, int size ):
		_stream_buffer(stream_buffer),_offset(offset),_size(size),_ib_buffer_data(0){}
	};
	struct sDeviceVertexBufferFillParam
	{
		unsigned int	_attribute_id;
		void *			_stream_buffer;
		int				_offset;
		int				_vertexSize;
		int				_vertexCount;
		void *			_vb_buffer_data;
		unsigned		_attribute_flag;

		sDeviceVertexBufferFillParam( int attribute_id, void * stream_buffer, int offset, int vertexsize, int vertexCount, unsigned attribFlag = 0 ):
		_attribute_id(attribute_id),_stream_buffer(stream_buffer),_offset(offset),_vertexSize(vertexsize),_vertexCount(vertexCount),_vb_buffer_data(0),_attribute_flag(attribFlag){}
	};
	ATOM_VECTOR<sDeviceIndexBufferFillParam>	m_indexbufferFillParams;			// 子网格索引BUFFER
	typedef ATOM_VECTOR<sDeviceVertexBufferFillParam> DeviceVBFpVec;
	ATOM_VECTOR<DeviceVBFpVec>					m_vertexbufferFillParams;			// 子网格顶点BUFFER

	struct sMaterialFillParam
	{
		void *		_material_buffer;
		ATOM_AUTOPTR(ATOM_Material)	_mat;
		ATOM_STRING _coreMatFile;
	};
	ATOM_VECTOR<sMaterialFillParam>				m_materialFillParams;				// 子网格材质

private:
	void _garbageCollect();

	void _fillMaterialParams();

	unsigned int m_meshID;

	ATOM_VECTOR<ATOM_STRING> m_vecDependents;
	ATOM_VECTOR<ATOM_ParameterTable::ValueHandle>	m_vecTexParamHandles;

	bool m_bAllFinished;
};
//----------------------//

class ATOM_UserAttributes;
class ATOM_ENGINE_API ATOM_SharedModel;

class ATOM_ENGINE_API ATOM_SharedMesh
{
public:
	ATOM_SharedMesh (ATOM_SharedModel *model);
	virtual ~ATOM_SharedMesh (void);

public:
	void drawWithoutMaterial (ATOM_RenderDevice *device);
	bool draw (ATOM_RenderDevice *device, ATOM_Material *material);
	
public:
	ATOM_SharedModel *getModel (void) const;
	const char *getName (void) const;
	void setName (const char *name);
	const ATOM_BBox &getBoundingbox (void) const;
	void setBoundingbox (const ATOM_BBox &bbox);
	ATOM_Geometry *getGeometry (void) const;
	void setGeometry (ATOM_Geometry *geometry);
	ATOM_Material *getMaterial (void) const;
	void setMaterial (ATOM_Material *material);
	void setJointMap (const ATOM_VECTOR<int> &jointMap);
	const ATOM_VECTOR<int> &getJointMap (void) const;
	bool isAlphaBlendEnabled (void) const;
	void enableAlphaBlend (bool enable);

	// wangjian added
	// 生成切线空间
	void genTangentSpace();
	//--------------------------//
private:
	ATOM_SharedModel *_owner;
	ATOM_STRING _name;
	ATOM_BBox _boundingBox;
	ATOM_Geometry *_geometry;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_VECTOR<int> _jointMap;
	ATOM_ParameterTable::ValueHandle _paramBlendEnable;
};

class ATOM_ENGINE_API ATOM_SharedModel: public ATOM_Object
{
	ATOM_CLASS(engine, ATOM_SharedModel, ATOM_SharedModel)

private:
	struct MainChunk
	{
		int version;
		int numMeshes;
	};

	struct MeshChunk
	{
		ATOM_VECTOR<unsigned short> indices;
		ATOM_VECTOR<ATOM_Vector3f> vertices;
		ATOM_VECTOR<ATOM_ColorARGB> colors;
		ATOM_VECTOR<float> psizes;
		ATOM_VECTOR<ATOM_Vector3f> normals;
		ATOM_VECTOR<ATOM_Vector3f> tangents;
		ATOM_VECTOR<ATOM_Vector3f> binormals;
	};

	struct MaterialChunk
	{
		ATOM_STRING materialFileName;
		ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)> materialParameters;
	};

public:
	ATOM_SharedModel (void);
	virtual ~ATOM_SharedModel (void);

public:
	bool load_nm (ATOM_RenderDevice *device, const char *filename, bool bMt = false);
	bool load_obj (ATOM_RenderDevice* device, const char* filename, bool bMt = false);

	//--- wangjian modified ---//
	// 异步加载：增加了一个异步加载标记bMt（默认为非异步）
	bool load (ATOM_RenderDevice *device, const char *filename, int loadPriority = ATOM_LoadPriority_IMMEDIATE);
#if 1
	int		load_half (ATOM_RenderDevice *device, const char *filename, int loadPriority = ATOM_LoadPriority_IMMEDIATE);
	bool	save_half (const char *filename);
	bool	convertToCompressed (ATOM_RenderDevice *device, const char *filename);
	bool	saveIsolation (const char *filename);
#endif
	//-------------------------//

	void unload (void);
	bool save (const char *filename);

	unsigned getNumMeshes (void) const;
	ATOM_SharedMesh *getMesh (unsigned index) const;
	unsigned getNumTracks (void) const;
	ATOM_JointAnimationTrack *getTrack (unsigned index) const;
	const ATOM_BBox &getBoundingbox (void) const;
	ATOM_Skeleton *getSkeleton (void) const;
	ATOM_UserAttributes *getAttributes (void) const;
	void deleteMesh (int index);

	//--- wangjian added ---//
	// 异步加载：相关函数
	ATOM_AsyncableLoadModel * getAsyncLoader();
	bool fillModelData(ATOM_RenderDevice *device,int version = 2 );
	bool postprocessLoading(ATOM_Model * pModel);
	//void setAnimationTexture(ATOM_Texture* animTex);
	//ATOM_Texture * getAnimationTexture(void);
	//----------------------//

private:
	bool readUserAttributes (ATOM_UserAttributes *attributes, ATOM_ChunkStream *stream);
	bool saveUserAttributes (const ATOM_UserAttributes *attributes, ATOM_ChunkStream *stream);

private:
	ATOM_VECTOR<ATOM_SharedMesh*> _meshes;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_JointAnimationTrack)> _tracks;
	ATOM_AUTOPTR(ATOM_JointAnimationDataCache) _animationDataCache;
	ATOM_AUTOPTR(ATOM_Skeleton) _skeleton;
	ATOM_BBox _boundingbox;
	ATOM_UserAttributes *_userAttributes;

	//--- wangjian added ---//
	// 异步加载：相关成员
	ATOM_AUTOPTR(ATOM_AsyncableLoadModel)	_asyncLoader;
	//ATOM_AUTOREF(ATOM_Texture)				_animationTexture;
	//----------------------//
};

#endif // __ATOM3D_ENGINE_MODEL2_H
