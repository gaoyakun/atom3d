#include "StdAfx.h"
#include <fstream>
#include "model2.h"
#include "model.h"
#include "modelanimationtrack.h"
#include "instanceskeleton.h"

struct ObjVertex
{
	ATOM_Vector3f pos;
	ATOM_Vector3f normal;
	ATOM_Vector2f uv;
	bool operator == (const ObjVertex& other) const
	{
		return pos == other.pos && normal == other.normal && uv == other.uv;
	}
};

struct ObjMesh
{
	ATOM_WSTRING mtlName;
	ATOM_VECTOR<ATOM_Vector3f> positions;
	ATOM_VECTOR<ATOM_Vector2f> texcoords;
	ATOM_VECTOR<ATOM_Vector3f> normals;
	ATOM_VECTOR<ObjVertex> vertices;
	ATOM_VECTOR<unsigned short> indices;
	ATOM_HASHMAP<int, ATOM_VECTOR<int> > vertexMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// wangjian added : ʹ��ѹ���������ݰ汾
enum eMODELFLAG
{
	MF_NONE = 0,
	MF_SKINNING = 1 << 0,
	MF_COMRESSION = 1 << 1,
	MF_TOTAL,
};

namespace FloatConversion
{
	typedef float			float32_t;
	typedef unsigned short	uint16_t;
	typedef unsigned		uint32_t;

	static const size_t FLOAT16_EXPONENT_BIAS = 15;
	static const size_t FLOAT16_EXPONENT_MAX = ( static_cast< size_t >( 1 ) << 5 ) - 1;
	static const size_t FLOAT16_MANTISSA_MAX = ( static_cast< size_t >( 1 ) << 10 ) - 1;

	static const size_t FLOAT32_EXPONENT_BIAS = 127;
	static const size_t FLOAT32_EXPONENT_MAX = ( static_cast< size_t >( 1 ) << 8 ) - 1;
	static const size_t FLOAT32_MANTISSA_MAX = ( static_cast< size_t >( 1 ) << 23 ) - 1;

	/// IEEE 754-2008 half-precision (16-bit) floating-point storage.
	union Float16
	{
		/// Packed data.
		uint16_t packed;

		/// Bitmask components.
		struct
		{
#if 1/*HELIUM_ENDIAN_LITTLE*/
			/// Mantissa.
			uint16_t mantissa : 10;
			/// Exponent.
			uint16_t exponent : 5;
			/// Sign.
			uint16_t sign : 1;
#else
			/// Sign.
			uint16_t sign : 1;
			/// Exponent.
			uint16_t exponent : 5;
			/// Mantissa.
			uint16_t mantissa : 10;
#endif
		} components;
	};

	/// IEEE 754 single-precision (32-bit) floating-point storage.
	union Float32
	{
		/// Floating-point value.
		float32_t value;
		/// Packed data.
		uint32_t packed;

		/// Bitmask components.
		struct
		{
#if 1/*HELIUM_ENDIAN_LITTLE*/
			/// Mantissa.
			uint32_t mantissa : 23;
			/// Exponent.
			uint32_t exponent : 8;
			/// Sign.
			uint32_t sign : 1;
#else
			/// Sign.
			uint32_t sign : 1;
			/// Exponent.
			uint32_t exponent : 8;
			/// Mantissa.
			uint32_t mantissa : 23;
#endif
		} components;
	};

	/// Compute the base-2 logarithm of an unsigned 32-bit integer.
	///
	/// @param[in] value  Unsigned 32-bit integer.
	///
	/// @return  Base-2 logarithm.
	///
	/// @see Log2( uint64_t )
	size_t Log2( uint32_t value )
	{
		/*HELIUM_ASSERT( value != 0 );

#if HELIUM_CC_CL
		unsigned long bitIndex = 0;
		HELIUM_VERIFY( _BitScanReverse( &bitIndex, value ) );

		return bitIndex;
#elif HELIUM_CC_GCC || HELIUM_CC_CLANG
		return ( 31 - __builtin_clz( value ) );
#else
#warning Compiling unoptimized Log2() implementation.  Please evaluate the availability of more optimal implementations for the current platform/compiler.*/
		size_t bitIndex = sizeof( value ) * 8 - 1;
		uint32_t mask = ( 1 << bitIndex );
		while( !( value & mask ) )
		{
			if( bitIndex == 0 )
			{
				break;
			}

			--bitIndex;
			mask >>= 1;
		}

		return bitIndex;
//#endif
	}

	/// Convert a Float32 value to a Float16.
	///
	/// Values outside the minimum and maximum range supported by a half-precision float will be clamped.  Infinity
	/// values will be preserved.
	///
	/// @param[in] value  Float32 value to convert.
	///
	/// @return  Float16 value.
	///
	/// @see Float16To32()
	Float16 Float32To16( Float32 value )
	{
		Float16 result;
		result.components.sign = value.components.sign;

		uint32_t exponent = value.components.exponent;
		if( exponent == FLOAT32_EXPONENT_MAX )
		{
			result.components.exponent = FLOAT16_EXPONENT_MAX;
			result.components.mantissa = 0;
		}
		else if( exponent < FLOAT32_EXPONENT_BIAS - FLOAT16_EXPONENT_BIAS + 1 )
		{
			// Bits will shift off entirely for very small numbers, so we don't need to check for denormalized values.
			uint32_t mantissa = ( 1 << 23 ) | value.components.mantissa;
			size_t mantissaShift =
				( 23 - 10 ) +
				( FLOAT32_EXPONENT_BIAS - FLOAT16_EXPONENT_BIAS + 1 ) -
				exponent;

			result.components.exponent = 0;
			result.components.mantissa = static_cast< uint16_t >( mantissa >> mantissaShift );
		}
		else if( exponent > FLOAT32_EXPONENT_BIAS + ( FLOAT16_EXPONENT_MAX - FLOAT16_EXPONENT_BIAS - 1 ) )
		{
			// Clamp values that are too large to represent in a half-precision float.
			result.components.exponent = FLOAT16_EXPONENT_MAX - 1;
			result.components.mantissa = FLOAT16_MANTISSA_MAX;
		}
		else
		{
			result.components.exponent = static_cast< uint16_t >(
				FLOAT32_EXPONENT_BIAS - FLOAT16_EXPONENT_BIAS + exponent );
			result.components.mantissa = static_cast< uint16_t >(
				value.components.mantissa >> ( 23 - 10 ) );
		}

		return result;
	}

	/// Convert a Float16 value to a Float32.
	///
	/// Infinity values will be preserved.
	///
	/// @param[in] value  Float16 value to convert.
	///
	/// @return  Float32 value.
	///
	/// @see Float32To16()
	Float32 Float16To32( Float16 value )
	{
		Float32 result;
		result.components.sign = value.components.sign;

		uint32_t exponent = value.components.exponent;
		if( exponent == FLOAT16_EXPONENT_MAX )
		{
			result.components.exponent = FLOAT32_EXPONENT_MAX;
			result.components.mantissa = 0;
		}
		else if( exponent == 0 )
		{
			uint32_t mantissa = value.components.mantissa;
			if( mantissa == 0 )
			{
				result.components.exponent = 0;
				result.components.mantissa = 0;
			}
			else
			{
				uint32_t mantissaShift = 10 - static_cast< uint32_t >( Log2( mantissa ) );
				result.components.exponent = FLOAT32_EXPONENT_BIAS - ( FLOAT16_EXPONENT_BIAS - 1 ) - mantissaShift;
				result.components.mantissa =
					mantissa << ( mantissaShift + 23 - 10 );
			}
		}
		else
		{
			result.components.exponent = FLOAT32_EXPONENT_BIAS - FLOAT16_EXPONENT_BIAS + exponent;
			result.components.mantissa = static_cast< uint32_t >( value.components.mantissa ) <<
				( 23 - 10 );
		}

		return result;
	}

	inline uint16_t FloatToHalf( float32_t f )
	{
		Float32 temp;
		temp.value = f;
		return Float32To16( temp ).packed;
	}

	inline float32_t HalfToFloat( uint16_t h )
	{
		Float16 temp;
		temp.packed = h;
		return Float16To32( temp ).value;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void calculateTangentBasis (long numVertices, const ATOM_Vector3f *vertex, /*const*/ ATOM_Vector3f *normal, const ATOM_Vector2f *texcoord, long numTris, const unsigned short *indices, ATOM_Vector3f *tangent, ATOM_Vector3f *binormal)
{
	ATOM_Vector3f *tan1 = ATOM_NEW_ARRAY(ATOM_Vector3f, numVertices * 2);
	ATOM_Vector3f *tan2 = tan1 + numVertices;
	memset (tan1, 0, sizeof(ATOM_Vector3f)*2*numVertices);

	for (long a = 0; a < numTris; ++a)
	{
		long i1 = *indices++;
		long i2 = *indices++;
		long i3 = *indices++;

		const ATOM_Vector3f &v1 = vertex[i1];
		const ATOM_Vector3f &v2 = vertex[i2];
		const ATOM_Vector3f &v3 = vertex[i3];

		const ATOM_Vector2f &w1 = texcoord[i1];
		const ATOM_Vector2f &w2 = texcoord[i2];
		const ATOM_Vector2f &w3 = texcoord[i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 0.0f;
		if( (s1*t2-s2*t1) != 0 )
			r = 1.f/(s1*t2-s2*t1);

		ATOM_Vector3f sdir((t2*x1-t1*x2)*r, (t2*y1-t1*y2)*r, (t2*z1-t1*z2)*r);
		ATOM_Vector3f tdir((s1*x2-s2*x1)*r, (s1*y2-s2*y1)*r, (s1*z2-s2*z1)*r);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (long a = 0; a < numVertices; ++a)
	{
		// Ϊ�˱������߿ռ��������ȶԷ��߽��е�λ��,ͬʱ����Ҳ���±�����
		ATOM_Vector3f & n = normal[a];
		n.normalize();

		const ATOM_Vector3f &t = tan1[a];

		tangent[a] = (t - n * dotProduct(n, t));
		tangent[a].normalize ();

		float s = (dotProduct (crossProduct (n, t), tan2[a]) < 0.f) ? -1.f : 1.f;

		binormal[a] = crossProduct (n, tangent[a]) * s;
		binormal[a].normalize ();
	}

	ATOM_DELETE_ARRAY(tan1);
}

//--- wangjian added ---//
void chooseProperCoreMatName( ATOM_STRING & corematname, bool bHwInstancingMesh )
{
	// ����������Ʋ�Ϊ�� ��
	// �������Ʊ���Ϊmodel_default.mat����model_bumpmap.mat
	if( ( !corematname.empty() ) && 
		( corematname != "/materials/builtin/model_default.mat"/* && corematname != "/materials/builtin/model_bumpmap.mat"*/ ) )
	{
		return;
	}

	// ���Ϊ�գ�Ĭ����model_default.mat
	if( corematname.empty() )
	{
		corematname = "/materials/builtin/model_default.mat";
	}

	// ���������INSTANCING ��Ƥ������ʱ��֧��INSTANCING
#if 0
	if( bHwInstancingMesh && ATOM_RenderSettings::isUseHWInstancing() )
#else
	if( bHwInstancingMesh && ATOM_RenderSettings::getRenderCaps().supportInstancing )
#endif
	{
		if( ATOM_RenderSettings::isUseInstancingStaticBuffer() )
		{
			// ���������model_default ��ʹ��model_default��instancing����
			if( corematname == "/materials/builtin/model_default.mat" )
			{
				corematname = "/materials/builtin/model_default_instancing_staticbuffer.mat";
			}
			// ���������model_bumpmap ��ʹ��model_bumpmap��instancing����
			/*else if( corematname == "/materials/builtin/model_bumpmap.mat" )
			{
				corematname = "/materials/builtin/model_bumpmap_instancing_staticbuffer.mat";
			}*/
		}
		else
		{
			// ���������model_default ��ʹ��model_default��instancing����
			if( corematname == "/materials/builtin/model_default.mat" )
			{
				corematname = "/materials/builtin/model_default_instancing.mat";
			}
			// ���������model_bumpmap ��ʹ��model_bumpmap��instancing����
			/*else if( corematname == "/materials/builtin/model_bumpmap.mat" )
			{
			corematname = "/materials/builtin/model_bumpmap_instancing.mat";
			}*/
		}
	}
}

bool useHWInstancingMesh( bool bSkinnedMesh, ATOM_STRING corematname )
{
#if 0
	if( !ATOM_RenderSettings::isUseHWInstancing() ||											// û�п���ʵ����
		bSkinnedMesh ||																			// ��Ƥ����				
		( !corematname.empty() && corematname != "/materials/builtin/model_default.mat" ) )		// �������ƴ��� �� ���ǡ�model_default��(todo:֧��model_bumpmap)
		return false;
#else
	if( !ATOM_RenderSettings::getRenderCaps().supportInstancing ||								// �豸��֧��ʵ����
		bSkinnedMesh ||																			// ��Ƥ����				
		( !corematname.empty() && corematname != "/materials/builtin/model_default.mat" ) )		// �������ƴ��� �� ���ǡ�model_default��(todo:֧��model_bumpmap)
		return false;
#endif

	return true;
}

bool isBumpMapMaterial( const char*  corematname )
{
	if( strstr( corematname,"model_bumpmap" ) || 
		strstr( corematname,"model_flux" )	)
		return true;
	return false;
}
//----------------------//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_SharedMesh::ATOM_SharedMesh (ATOM_SharedModel *model)
{
	_owner = model;
	_geometry = 0;
	_paramBlendEnable = 0;
}

ATOM_SharedMesh::~ATOM_SharedMesh (void)
{
	ATOM_DELETE(_geometry);
}

ATOM_SharedModel *ATOM_SharedMesh::getModel (void) const
{
	return _owner;
}

const char *ATOM_SharedMesh::getName (void) const
{
	return _name.c_str();
}

void ATOM_SharedMesh::setName (const char *name)
{
	_name = name ? name : "";
}

const ATOM_BBox &ATOM_SharedMesh::getBoundingbox (void) const
{
	return _boundingBox;
}

void ATOM_SharedMesh::setBoundingbox (const ATOM_BBox &bbox)
{
	_boundingBox = bbox;
}

ATOM_Geometry *ATOM_SharedMesh::getGeometry (void) const
{
	return _geometry;
}

void ATOM_SharedMesh::setGeometry (ATOM_Geometry *geometry)
{
	_geometry = geometry;
}

ATOM_Material *ATOM_SharedMesh::getMaterial (void) const
{
	return _material.get();
}

void ATOM_SharedMesh::setMaterial (ATOM_Material *material)
{
	_material = material;
	_paramBlendEnable = material ? material->getParameterTable()->getValueHandle ("enableAlphaBlend") : 0;
}

void ATOM_SharedMesh::drawWithoutMaterial (ATOM_RenderDevice *device)
{
	ATOM_IndexArray *indices = _geometry->getIndices ();
	if( !indices )
		return;

	unsigned numPrim = indices->getNumIndices() / 3;
	if (numPrim)
	{
		_geometry->draw (device, ATOM_PRIMITIVE_TRIANGLES, numPrim, 0);
	}
}

bool ATOM_SharedMesh::draw(ATOM_RenderDevice *device, ATOM_Material *material)
{
	if (!material)
	{
		material = _material.get();
	}

	if (material)
	{
		ATOM_IndexArray *indices = _geometry->getIndices ();
		if( !indices )
			return false;

		unsigned numPrim = indices->getNumIndices() / 3;
		if (numPrim)
		{
			unsigned numPasses = material->begin (device);
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (material->beginPass (device, pass))
				{
					_geometry->draw (device, ATOM_PRIMITIVE_TRIANGLES, numPrim, 0);
					material->endPass (device, pass);
				}
			}
			material->end (device);
		}
	}

	return true;
}

void ATOM_SharedMesh::setJointMap (const ATOM_VECTOR<int> &jointMap)
{
	_jointMap = jointMap; 
}

const ATOM_VECTOR<int> &ATOM_SharedMesh::getJointMap (void) const
{
	return _jointMap;
}

bool ATOM_SharedMesh::isAlphaBlendEnabled (void) const
{
	return _paramBlendEnable ? (*_paramBlendEnable->i) != 0 : false;
}

void ATOM_SharedMesh::enableAlphaBlend (bool enable)
{
	if (_paramBlendEnable)
	{
		*_paramBlendEnable->i = enable ? 1 : 0;
	}
}

// wangjian added
// �������߿ռ�
void ATOM_SharedMesh::genTangentSpace()
{
	// calculate tangents
	ATOM_MultiStreamGeometry * geo = (ATOM_MultiStreamGeometry*)(_geometry);
	if( !geo )
		return;

	ATOM_VertexArray *vertices	= geo->getStream (ATOM_VERTEX_ATTRIB_COORD);
	ATOM_VertexArray *normals	= geo->getStream (ATOM_VERTEX_ATTRIB_NORMAL);
	ATOM_VertexArray *texcoords = geo->getStream (ATOM_VERTEX_ATTRIB_TEX1_2);
	ATOM_VertexArray *tangents	= geo->getStream (ATOM_VERTEX_ATTRIB_TANGENT);
	ATOM_VertexArray *binormals = geo->getStream (ATOM_VERTEX_ATTRIB_BINORMAL);
	ATOM_IndexArray *indices	= geo->getIndices ();

	if(tangents)
	{
		geo->removeStream(tangents);
	}
	if(binormals)
	{
		geo->removeStream(binormals);
	}

	if ( vertices && normals && texcoords && indices /*&& (!tangents||!binormals)*/ )
	{
		unsigned numVertices = vertices->getNumVertices ();
		unsigned numTris = indices->getNumIndices() / 3;

		ATOM_AUTOREF(ATOM_VertexArray) tangents = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_TANGENT, ATOM_USAGE_STATIC, numVertices, true);
		ATOM_AUTOREF(ATOM_VertexArray) binormals = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_BINORMAL, ATOM_USAGE_STATIC, numVertices, true);

		ATOM_Vector3f *vertex = (ATOM_Vector3f *)vertices->lock (ATOM_LOCK_READWRITE, 0, 0, true);
		ATOM_Vector3f *normal = (ATOM_Vector3f *)normals->lock (ATOM_LOCK_READWRITE, 0, 0, true);
		ATOM_Vector2f *texcoord = (ATOM_Vector2f *)texcoords->lock (ATOM_LOCK_READWRITE, 0, 0, true);
		ATOM_Vector3f *tangent = (ATOM_Vector3f*)tangents->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		ATOM_Vector3f *binormal = (ATOM_Vector3f*)binormals->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		unsigned short *index = (unsigned short*)indices->lock (ATOM_LOCK_READWRITE, 0, 0, true);

		calculateTangentBasis (numVertices, vertex, normal, texcoord, numTris, index, tangent, binormal);

		vertices->unlock ();
		normals->unlock ();
		texcoords->unlock ();
		tangents->unlock ();
		binormals->unlock ();
		indices->unlock ();

		geo->addStream (tangents.get());
		geo->addStream (binormals.get());
	}
}
//--------------------------//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--- wangjian added ---//
// �첽���� �� ���ģ����
ATOM_AsyncableLoadModel::ATOM_AsyncableLoadModel():m_version(2),m_modelFlag(0)
{
	m_modelData._chunkIO = 0;
	m_modelData._model = 0;
	m_meshID = 0;
	m_bAllFinished = false;
}
ATOM_AsyncableLoadModel::~ATOM_AsyncableLoadModel()
{
	GarbageCollect();

	m_indexbufferFillParams.clear();
	m_materialFillParams.clear();
	/*ATOM_VECTOR<DeviceVBFpVec>::iterator iter = m_vertexbufferFillParams.begin();
	for(;iter!=m_vertexbufferFillParams.end();++iter)
	{
		DeviceVBFpVec & v = (*iter);
		v.clear();
	}*/
	m_vertexbufferFillParams.clear();
}
void ATOM_AsyncableLoadModel::SetModel( ATOM_SharedModel * model, int version )
{
	if( model && !host )
		host = model;

	m_version = version;
	m_bAllFinished = false;
}
void ATOM_AsyncableLoadModel::SetModelFlag(unsigned flag)
{
	m_modelFlag = flag;
}
unsigned ATOM_AsyncableLoadModel::GetModelFlag(void)
{
	return m_modelFlag;
}
void ATOM_AsyncableLoadModel::SetLoadAllFinished()
{
	__super::SetLoadAllFinished();
	m_bAllFinished = true;
}
bool ATOM_AsyncableLoadModel::Load()
{
	if( !host )
		return false;

	if( __super::Load() == false )
	{
		return false;
	}

	if( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_INLOADING )
	{
		ATOM_LOGGER::error ("%s(%s) Async loading stage flag is not match.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// ����ʧ��
		errorno = ATOM_AsyncLoader::ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH;		// ʧ��ԭ�򣺼��ؽ׶α�ǲ�ƥ��
		return false;
	}

	if( filename.empty() )
	{
		ATOM_LOGGER::error ("%s(%s) The file name is empty.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// ����ʧ��
		errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_FILEOPENFAILED;		// ʧ��ԭ��
		return false;
	}

	
#if 0

	// �ö�ȡ�ļ�֮��õ���sig���а汾���ж�
	unsigned sig;

	{
		ATOM_AutoFile f (filename.c_str(), ATOM_VFS::read);
		if (!f)
		{
			ATOM_LOGGER::error ("%s(%s)  Open file failed.\n", __FUNCTION__, filename.c_str());

			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// ����ʧ��
			errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_FILEOPENFAILED;		// ʧ��ԭ���ļ��򿪳���

			return false;
		}
		if (f->size() < sizeof(unsigned))
		{
			ATOM_LOGGER::error ("%s(%s, %u, %u) file size is too small.\n", __FUNCTION__, filename.c_str());

			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// ����ʧ��
			errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_FILESIZETOOSMALL;	// ʧ��ԭ���ļ���С����
			return false;
		}
	
		if( !f->read (&sig, sizeof(unsigned)) )
		{
			ATOM_LOGGER::error ("%s(%s) file read signiture failed.\n", __FUNCTION__, filename.c_str());

			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;			// ����ʧ��
			errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;	// ʧ��ԭ�򣺶�ȡ�ڲ�����
			return false;
		}

		if (sig == 0x4e334d46)
		{
			m_version = 1;
		}
	}
#else

	// ֱ��ͨ���ļ���׺���жϰ汾
	if( !strstr( filename.c_str(), ".nm2" ) )
		m_version = 1;

#endif

	if( m_version == 2 )
	{
		m_modelData._chunkIO = ATOM_NEW(ATOM_ChunkIO);
		data = m_modelData._chunkIO;
	}
	else
	{
		m_modelData._model = ATOM_NEW(ATOM_Model);
		data = m_modelData._model;
	}

	// �°汾��ģ��
	if( m_version == 2 )
	{
		ATOM_ChunkIO * _chunkIO = (ATOM_ChunkIO*)data;
		// ����ģ������
		if ( !_chunkIO || !_chunkIO->load (filename.c_str()))
		{
			ATOM_LOGGER::error ("%s(%s, %u, %u) chunk io ( new version ) load error.\n", __FUNCTION__, filename.c_str());

			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;			// ����ʧ��
			errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;	// ʧ��ԭ���ڲ�����

			GarbageCollect();

			return false;
		}

		// ���ģ������
		/*ATOM_SharedModel * model_shared = (ATOM_SharedModel*)(host.get());*/
		ATOM_SharedModel * model_shared = (ATOM_SharedModel*)(host);
		ATOM_ASSERT(model_shared);
		if( !model_shared->fillModelData(ATOM_GetRenderDevice(),m_version) )
		{
			ATOM_LOGGER::error ("%s(%s) fill Model Data failed.\n", __FUNCTION__, filename.c_str());

			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// ����ʧ��
			errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// ʧ��ԭ���ڲ�����

			GarbageCollect();

			return false;
		}

		//ATOM_LOGGER::log("ATOM_AsyncableLoadModel::Load() --- version 2 , %s \n", filename.c_str() );
	}
	//----------------------------------
	// �ϰ汾��ģ��
	//----------------------------------
	else
	{
		//--- ����ģ������ ---//
		// ע�⣺ ����ģ��ʱ�����첽���ز��Ҵ�������
		// �첽���ز��Ҵ�������Ĳ�������LOCK�н��е�
		ATOM_Model * _model = (ATOM_Model*)data;
		if( !_model || !_model->load(filename.c_str() ) )
		{
			ATOM_LOGGER::error ("%s(%s) old model version load error.\n", __FUNCTION__, filename.c_str());

			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;			// ����ʧ��
			errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;	// ʧ��ԭ���ڲ�����

			GarbageCollect();

			return false;
		}

		//--- ����֮����Ҫ��MESH��������Ϊģ������������� ---//
		for(  int i = 0; i < _model->getNumMeshes(); ++i )
		{
			ATOM_Mesh * mesh = _model->getMesh(i);
			if( mesh && !mesh->diffuseTextureName.empty() )
			{
				AddDependent(mesh->diffuseTextureName);
			}
		}
		//------------------------------------------------------//

		//--- ��ʼ��ģ�Ͳ��� ---//
		if (!_model->initMaterials (true, false))
		{
			ATOM_LOGGER::error ("%s(%s) Init material failed.\n", __FUNCTION__, filename.c_str());
			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;				// ����ʧ��
			errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_MATERIAL_INITFAILED;	// ʧ��ԭ�򣺲��ʳ�ʼ��ʧ��

			GarbageCollect();

			return false;
		}
	}

	// �ļ����ݼ��سɹ�
	Done();

	return true;
}

bool ATOM_AsyncableLoadModel::OnloadEnd()
{
	if( !host )
		return false;

	bool result = true;
	//if( host )
	//{
	//	ATOM_SharedModel * model = dynamic_cast<ATOM_SharedModel*>(host);
	//	ATOM_ASSERT(model);
	//	if(model)
	//	{
	//		// ��������5��
	//		int retry_count = 5;

	//		while( retry_count > 0 )
	//		{
	//			// ���ģ������
	//			if( !model->fillModelData(ATOM_GetRenderDevice(),m_version) )
	//			{
	//				result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// ����ʧ��
	//				errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// ʧ��ԭ���ڲ�����

	//				ATOM_LOGGER::error ("%s(%s) model create content failed.\n", __FUNCTION__, filename.c_str());
	//			}
	//			else
	//			{
	//				retry_count = 0;
	//			}

	//			--retry_count;
	//		}

	//		// ����5������ʧ��
	//		if( retry_count == 0 )
	//		{
	//			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// ����ʧ��
	//			errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// ʧ��ԭ���ڲ�����

	//			ATOM_LOGGER::error ("%s(%s) retry count is exceeded 5, model create content failed completely.\n", __FUNCTION__, filename.c_str());

	//			result = false;

	//		}

	//		// ����Դ
	//		return Lock();
	//	}
	//}

	//// ��������
	//_garbageCollect();

	return result;
}
//-----------------------//
bool ATOM_AsyncableLoadModel::Lock(bool & needRecurse)
{
	if( !host )
		return false;

	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::log ("%s(%s) start Lock.\n", __FUNCTION__, filename.c_str());

	if( host )
	{
		//ATOM_SharedModel * model_shared = (ATOM_SharedModel*)(host.get());
		ATOM_SharedModel * model_shared = (ATOM_SharedModel*)(host);
		ATOM_ASSERT(model_shared);

		if( model_shared )
		{
			// �°汾��MODEL
			if( m_version == 2 )
			{
				// ѭ�����е�����
				for( int i = 0; i < model_shared->getNumMeshes(); ++i )
				{
					m_meshID = i;

					// ���������
					ATOM_SharedMesh * mesh_shared = model_shared->getMesh(m_meshID);
					if( mesh_shared )
					{
						/////////////////////////////////////////// ������������� ///////////////////////////////////////////////////////////
						ATOM_ASSERT( m_meshID>=0 && m_meshID < m_materialFillParams.size() );
						ATOM_ChunkIO::Chunk * materialChunk = (ATOM_ChunkIO::Chunk *)m_materialFillParams[m_meshID]._material_buffer;
						
						ATOM_AUTOPTR(ATOM_Material) material = 0;

						//--------------------------- wangjian modified --------------------------//
						// ѡ����ʵĺ��Ĳ����ļ�
						ATOM_STRING coremat_name = "";
						chooseProperCoreMatName(coremat_name,mesh_shared->getGeometry()->getType()==GT_HWINSTANCING);
						//------------------------------------------------------------------------//

						if (!materialChunk)
						{
							material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), coremat_name.c_str() );
							mesh_shared->setMaterial (material.get());
						}
						else
						{
							
							//--------------------------- wangjian modified --------------------------//
							// ѡ����ʵĺ��Ĳ����ļ�
							coremat_name = m_materialFillParams[m_meshID]._coreMatFile;
							chooseProperCoreMatName(coremat_name,mesh_shared->getGeometry()->getType()==GT_HWINSTANCING);
							//------------------------------------------------------------------------//

							// ��������
							material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), coremat_name.c_str());
							
							// ���ò���
							//mesh_shared->setMaterial ( material.get() );

							
							
							if (!material)
							{
								material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/model_default.mat");
								mesh_shared->setMaterial (material.get());
							}
							else
							{
#if 0
								ATOM_ParameterTable *paramTable = material->getParameterTable();

								for (;;)
								{
									int size = materialChunk->getChunkStream()->readCString (0);
									materialChunk->getChunkStream()->seek (-size);
									char *buffer = ATOM_NEW_ARRAY(char, size);
									materialChunk->getChunkStream()->readCString (buffer);
									if (*buffer == '\0')
									{
										ATOM_DELETE_ARRAY(buffer);
										break;
									}

									ATOM_ParameterTable::ValueHandle paramValue = paramTable->getValueHandle (buffer);
									ATOM_DELETE_ARRAY(buffer);
									if (!paramValue)
									{
										continue;
									}
									switch (paramValue->handle->getParamType())
									{
									case ATOM_MaterialParam::ParamType_Float:
										{
											paramValue->setFloat (materialChunk->getChunkStream()->readFloat ());
											break;
										}
									case ATOM_MaterialParam::ParamType_FloatArray:
										{
											materialChunk->getChunkStream()->read (paramValue->f, sizeof(float)*paramValue->handle->getDimension());
											break;
										}
									case ATOM_MaterialParam::ParamType_Int:
										{
											paramValue->setInt (materialChunk->getChunkStream()->readInt ());
											break;
										}
									case ATOM_MaterialParam::ParamType_IntArray:
										{
											materialChunk->getChunkStream()->read (paramValue->i, sizeof(int)*paramValue->handle->getDimension());
											break;
										}
									case ATOM_MaterialParam::ParamType_Vector:
										{
											materialChunk->getChunkStream()->read (paramValue->v, sizeof(ATOM_Vector4f));
											break;
										}
									case ATOM_MaterialParam::ParamType_VectorArray:
										{
											materialChunk->getChunkStream()->read (paramValue->v, sizeof(ATOM_Vector4f)*paramValue->handle->getDimension());
											break;
										}
									case ATOM_MaterialParam::ParamType_Matrix44:
										{
											materialChunk->getChunkStream()->read (paramValue->m44, sizeof(ATOM_Matrix4x4f));
											break;
										}
									case ATOM_MaterialParam::ParamType_Matrix44Array:
										{
											materialChunk->getChunkStream()->read (paramValue->m44, sizeof(ATOM_Matrix4x4f)*paramValue->handle->getDimension());
											break;
										}
									case ATOM_MaterialParam::ParamType_Matrix43:
										{
											materialChunk->getChunkStream()->read (paramValue->m43, sizeof(ATOM_Matrix3x4f));
											break;
										}
									case ATOM_MaterialParam::ParamType_Matrix43Array:
										{
											materialChunk->getChunkStream()->read (paramValue->m43, sizeof(ATOM_Matrix3x4f)*paramValue->handle->getDimension());
											break;
										}
									case ATOM_MaterialParam::ParamType_Texture:
										{
											int size = materialChunk->getChunkStream()->readCString(0);
											materialChunk->getChunkStream()->seek (-size);

											char *s = (char*)ATOM_MALLOC(size);

											materialChunk->getChunkStream()->readCString (s);
											// ����������첽���أ������и����⣬���ʹ�ø÷��������������õ�
											// ATOM_ID3DTextureManager::doAllocateTexture�������ڸ÷����У�����õ��ײ�Ķ��󹤳��Ĵ���������󷽷�
											// ���������󷽷���û�����ģ�����˵����IO�߳��д���������󣬶��ô�����û�м����ģ��������ܻᵼ�����⡣
											// ��������ŵ������̵߳�LOCK������
											paramValue->setTexture( s, priority );

											//--- ��Ϊģ����Ҫ�ȴ���������������ɲ�����ʾ ---//
											AddDependent(s);
											
											ATOM_FREE(s);

											break;
										}
									}
								}
#endif
							}
						}

						//----- ���ò��ʵ�ȫ��ID ----//
						if( material )
						{
							char strMatName[256] = {0};
							sprintf(strMatName,"%s_%u",filename.c_str(),i);
							material->setMaterialId(strMatName);
						}
						//--------------------------//


#if 0

						// ���ò���
						mesh_shared->setMaterial ( material.get() );

#else
						// ��ʱ�������
						m_materialFillParams[m_meshID]._mat = material;
#endif

						/////////////////////////////////////// ���������� ///////////////////////////////////////////////

						ATOM_ASSERT( m_meshID>=0 && m_meshID < m_indexbufferFillParams.size() );
						ATOM_Geometry * geometry = mesh_shared->getGeometry();
						if( geometry )
						{
							//=================== ��IB ======================//

							// ����IB
							ATOM_AUTOREF(ATOM_IndexArray) indices = 
								ATOM_GetRenderDevice()->allocIndexArray (	ATOM_USAGE_STATIC,
																			m_indexbufferFillParams[m_meshID]._size, 
																			false, 
																			true	);
							if (!indices)
							{
								ATOM_LOGGER::error ("%s(%s) index buffer allocation failed.\n", __FUNCTION__, filename.c_str());
								result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;					// ����ʧ��
								errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;				// ʧ��ԭ��

								// �ͷ�
								GarbageCollect();

								return false;
							}

							// ����IB
							geometry->setIndices (indices.get());

							// ����IB
							ATOM_IndexArray * indexbuffer = geometry->getIndices();
							if( indexbuffer )
							{
								m_indexbufferFillParams[m_meshID]._ib_buffer_data = indexbuffer->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
								if ( !m_indexbufferFillParams[m_meshID]._ib_buffer_data )
								{
									ATOM_LOGGER::error ("%s(%s) index buffer lock failed.\n", __FUNCTION__, filename.c_str());
									result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;					// ����ʧ��
									errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INDEXBUFFER_LOCKFAILED;	// ʧ��ԭ��IB��ʧ��

									// �ͷ�
									GarbageCollect();

									return false;
								}
							}
						
							//=================== ��VB ======================//
							ATOM_ASSERT( m_meshID>=0 && m_meshID < m_vertexbufferFillParams.size() );

							//------------------------------//
							// ʹ�ü�����ʵ������������
							//------------------------------//
							if( geometry->getType() == GT_HWINSTANCING )
							{
								ATOM_HWInstancingGeometry * hwinst_geo = static_cast<ATOM_HWInstancingGeometry *>(geometry);
								if( !hwinst_geo )
									return false;

								// ����STREAM�Ķ�������Ӧ����һ����
								unsigned int numVertices = m_vertexbufferFillParams[m_meshID][0]._vertexCount;

								unsigned attrib_total = 0;

								unsigned attrib_flags = 0;

								// �õ�attrib_total
								for( int j = 0; j < m_vertexbufferFillParams[m_meshID].size(); ++j )
								{
									attrib_total |= m_vertexbufferFillParams[m_meshID][j]._attribute_id;

									attrib_flags |= m_vertexbufferFillParams[m_meshID][j]._attribute_flag;
								}

								// �������㻺��
								ATOM_AUTOREF(ATOM_VertexArray) vertexArray = 
									ATOM_GetRenderDevice()->allocVertexArray ( attrib_total, ATOM_USAGE_STATIC, numVertices, true, attrib_flags );
								if (!vertexArray)
								{
									ATOM_LOGGER::error ("%s(%s) vertex buffer allocation failed.\n", __FUNCTION__, filename.c_str());
									result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;					// ����ʧ��
									errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;				// ʧ��ԭ��

									// �ͷ�
									GarbageCollect();

									return false;
								}

								// ���ö��㻺��
								hwinst_geo->setStream (vertexArray.get());

								// �������㻺�� �� ʹ�õ�һ��STREAM��BUFERָ��
								if( hwinst_geo->getStream() )
								{
									m_vertexbufferFillParams[m_meshID][0]._vb_buffer_data = 
										hwinst_geo->getStream()->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);

									if( !m_vertexbufferFillParams[m_meshID][0]._vb_buffer_data )
									{
										ATOM_LOGGER::error ("%s(%s) index buffer lock failed.\n", __FUNCTION__, filename.c_str());
										result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;					// ����ʧ��
										errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_VERTEXBUFFER_LOCKFAILED;	// ʧ��ԭ��VB��ʧ��

										// �ͷ�
										GarbageCollect();

										return false;
									}
								}
								
							}
							//------------------------------//
							// �Ǽ�����ʵ������������
							//------------------------------//
							else
							{
								ATOM_MultiStreamGeometry * multi_geo = static_cast<ATOM_MultiStreamGeometry *>(geometry);
								if( !multi_geo )
									return false;

								// ѭ�����е�STREAM
								for( int j = 0; j < m_vertexbufferFillParams[m_meshID].size(); ++j )
								{
									unsigned int attr_id = m_vertexbufferFillParams[m_meshID][j]._attribute_id;

									unsigned int attr_flag = m_vertexbufferFillParams[m_meshID][j]._attribute_flag;

									// ����VB
									ATOM_AUTOREF(ATOM_VertexArray) vertexArray = 
										ATOM_GetRenderDevice()->allocVertexArray (	attr_id, 
																					ATOM_USAGE_STATIC, 
																					m_vertexbufferFillParams[m_meshID][j]._vertexCount, 
																					true,
																					attr_flag	);
									if (!vertexArray)
									{
										ATOM_LOGGER::error ("%s(%s) vertex buffer allocation failed.\n", __FUNCTION__, filename.c_str());
										result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;					// ����ʧ��
										errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;				// ʧ��ԭ��

										// �ͷ�
										GarbageCollect();

										return false;
									}

									// ����VB
									multi_geo->addStream (vertexArray.get());

									// ����VB
									ATOM_VertexArray * vertexbuffer = multi_geo->getStream(attr_id);
									if( vertexbuffer )
									{
										m_vertexbufferFillParams[m_meshID][j]._vb_buffer_data = vertexbuffer->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
										if( !m_vertexbufferFillParams[m_meshID][j]._vb_buffer_data )
										{
											ATOM_LOGGER::error ("%s(%s) index buffer lock failed.\n", __FUNCTION__, filename.c_str());
											result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;					// ����ʧ��
											errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_VERTEXBUFFER_LOCKFAILED;	// ʧ��ԭ��VB��ʧ��

											// �ͷ�
											GarbageCollect();

											return false;
										}
									}
								}
							}
						}
					}

					//--- ��һ�δ�����һ����MESH ---//
					/*m_meshID++;
					if( m_meshID < model_shared->getNumMeshes() )
						needRecurse = true;
					else 
						needRecurse = false;*/
					//------------------------------//
				}
			}
			// �ϰ汾��MODEL
			else
			{
				ATOM_Model * model_old = (ATOM_Model*)data;
				if( model_old )
				{
					/*ATOM_ModelLockStruct lockStruct;*/

					// ����
					// ע�⣺ ����ģ��ʱ�����첽���ز��Ҵ�������
					// �첽���ز��Ҵ�������Ĳ������ڴ˽���
					// ������IO�߳��д����������
					if ( !model_old->lock () )
					{
						ATOM_LOGGER::error ("%s(%s) index buffer lock failed.\n", __FUNCTION__, filename.c_str());
						result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;					// ����ʧ��
						errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_DEVICEBUFFER_LOCKFAILED;	// ʧ��ԭ���豸BUFFER��ʧ��

						// �ͷ�
						GarbageCollect();

						return false;
					}
				}

				needRecurse = false;
			}
		}

		return __super::Lock(needRecurse);
	}

	return false;
}

// �����ʲ���
void ATOM_AsyncableLoadModel::_fillMaterialParams()
{

	ATOM_SharedModel * model_shared = (ATOM_SharedModel*)(host);
	ATOM_ASSERT(model_shared);

	// ѭ�����е�����
	for( int i = 0; i < model_shared->getNumMeshes(); ++i )
	{
		ATOM_ASSERT( i>=0 && i < m_materialFillParams.size() );
		ATOM_ChunkIO::Chunk * materialChunk = (ATOM_ChunkIO::Chunk *)m_materialFillParams[i]._material_buffer;

		ATOM_ASSERT(m_materialFillParams[i]._mat);

		ATOM_ParameterTable *paramTable = m_materialFillParams[i]._mat->getParameterTable();

		for (;;)
		{
			int size = materialChunk->getChunkStream()->readCString (0);
			materialChunk->getChunkStream()->seek (-size);
			char *buffer = ATOM_NEW_ARRAY(char, size);
			materialChunk->getChunkStream()->readCString (buffer);
			if (*buffer == '\0')
			{
				ATOM_DELETE_ARRAY(buffer);
				break;
			}

			ATOM_ParameterTable::ValueHandle paramValue = paramTable->getValueHandle (buffer);
			ATOM_DELETE_ARRAY(buffer);
			if (!paramValue)
			{
				continue;
			}
			switch (paramValue->handle->getParamType())
			{
			case ATOM_MaterialParam::ParamType_Float:
				{
					paramValue->setFloat (materialChunk->getChunkStream()->readFloat ());
					break;
				}
			case ATOM_MaterialParam::ParamType_FloatArray:
				{
					materialChunk->getChunkStream()->read (paramValue->f, sizeof(float)*paramValue->handle->getDimension());
					break;
				}
			case ATOM_MaterialParam::ParamType_Int:
				{
					paramValue->setInt (materialChunk->getChunkStream()->readInt ());
					break;
				}
			case ATOM_MaterialParam::ParamType_IntArray:
				{
					materialChunk->getChunkStream()->read (paramValue->i, sizeof(int)*paramValue->handle->getDimension());
					break;
				}
			case ATOM_MaterialParam::ParamType_Vector:
				{
					materialChunk->getChunkStream()->read (paramValue->v, sizeof(ATOM_Vector4f));
					break;
				}
			case ATOM_MaterialParam::ParamType_VectorArray:
				{
					materialChunk->getChunkStream()->read (paramValue->v, sizeof(ATOM_Vector4f)*paramValue->handle->getDimension());
					break;
				}
			case ATOM_MaterialParam::ParamType_Matrix44:
				{
					materialChunk->getChunkStream()->read (paramValue->m44, sizeof(ATOM_Matrix4x4f));
					break;
				}
			case ATOM_MaterialParam::ParamType_Matrix44Array:
				{
					materialChunk->getChunkStream()->read (paramValue->m44, sizeof(ATOM_Matrix4x4f)*paramValue->handle->getDimension());
					break;
				}
			case ATOM_MaterialParam::ParamType_Matrix43:
				{
					materialChunk->getChunkStream()->read (paramValue->m43, sizeof(ATOM_Matrix3x4f));
					break;
				}
			case ATOM_MaterialParam::ParamType_Matrix43Array:
				{
					materialChunk->getChunkStream()->read (paramValue->m43, sizeof(ATOM_Matrix3x4f)*paramValue->handle->getDimension());
					break;
				}
			case ATOM_MaterialParam::ParamType_Texture:
				{
					int size = materialChunk->getChunkStream()->readCString(0);
					materialChunk->getChunkStream()->seek (-size);

					char *s = (char*)ATOM_MALLOC(size);

					materialChunk->getChunkStream()->readCString (s);
					// ����������첽���أ������и����⣬���ʹ�ø÷��������������õ�
					// ATOM_ID3DTextureManager::doAllocateTexture�������ڸ÷����У�����õ��ײ�Ķ��󹤳��Ĵ���������󷽷�
					// ���������󷽷���û�����ģ�����˵����IO�߳��д���������󣬶��ô�����û�м����ģ��������ܻᵼ�����⡣
					// ��������ŵ������̵߳�LOCK������
					//paramValue->setTexture( s, priority );

					//--- ��Ϊģ����Ҫ�ȴ���������������ɲ�����ʾ ---//
					AddDependent(s,paramValue);

					ATOM_FREE(s);

					break;
				}
			}
		}
	}
}

bool ATOM_AsyncableLoadModel::Fill()
{
	if( !host )
		return false;

	//return __super::Fill();

	if( host )
	{
		/*ATOM_SharedModel * model_shared = (ATOM_SharedModel*)(host.get());*/
		ATOM_SharedModel * model_shared = (ATOM_SharedModel*)(host);
		ATOM_ASSERT(model_shared);

		/*if( strstr( filename.c_str(),"dxzb201") )
		{
			int i = 0;
		}*/

		if( model_shared )
		{
			// �°汾��MODEL
			if( m_version == 2 )
			{
				//ATOM_ChunkIO * chunkIO = (ATOM_ChunkIO *)data;
				
				//ATOM_ChunkIO::Chunk *topChunk = chunkIO->getTopChunk ();
				//ATOM_ChunkIO::Chunk *mainChunk = topChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','m','e'));
				//// read sub meshes
				//ATOM_ChunkIO::Chunk *meshChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','s','m'));
								
				if( model_shared->getNumMeshes() > 0 )
				{
					// ѭ�����е�����
					for( int i = 0; i < model_shared->getNumMeshes(); ++i )
					{
						//ATOM_ChunkStream *meshChunkStream = meshChunk->getChunkStream();

						ATOM_SharedMesh * mesh_shared = model_shared->getMesh(i);
						if( mesh_shared && mesh_shared->getGeometry() )
						{
							//=================== ���IB ======================//
							ATOM_IndexArray * indexbuffer = mesh_shared->getGeometry()->getIndices();
							if( indexbuffer )
							{
								if ( m_indexbufferFillParams[i]._ib_buffer_data )
								{
									ATOM_ChunkStream * meshChunkStream = (ATOM_ChunkStream *)(m_indexbufferFillParams[i]._stream_buffer);
									if( meshChunkStream )
									{
										meshChunkStream->seek(-meshChunkStream->position());
										meshChunkStream->seek(m_indexbufferFillParams[i]._offset);
										meshChunkStream->read (m_indexbufferFillParams[i]._ib_buffer_data, m_indexbufferFillParams[i]._size * sizeof(short));
									}
								}
							}

							//=================== ���VB ======================//
							
							//-------------------------------
							// ʹ�ü�����ʵ������������
							//-------------------------------
							if( mesh_shared->getGeometry()->getType() == GT_HWINSTANCING )
							{
								ATOM_HWInstancingGeometry * hwinst_geo = static_cast<ATOM_HWInstancingGeometry *>(mesh_shared->getGeometry());
								if( hwinst_geo && hwinst_geo->getStream() )
								{
									unsigned numStreams = m_vertexbufferFillParams[i].size();
									ATOM_VECTOR<char *> data_array;
									data_array.resize(numStreams,0);

									// ѭ�����е�STREAM
									for (int j = 0; j < numStreams; ++j)
									{
										// ��ȡÿ��STREAM�Ķ�������
										ATOM_ChunkStream * meshChunkStream = (ATOM_ChunkStream *)(m_vertexbufferFillParams[i][j]._stream_buffer);
										if( meshChunkStream )
										{
											unsigned dataSize = m_vertexbufferFillParams[i][j]._vertexSize * 
																m_vertexbufferFillParams[i][j]._vertexCount;
											data_array[j] = (char*)ATOM_MALLOC(dataSize);

											// �Ƶ�����λ��
											meshChunkStream->seek(-meshChunkStream->position());
											meshChunkStream->seek(m_vertexbufferFillParams[i][j]._offset);

											// ��ȡ
											meshChunkStream->read( data_array[j], dataSize );
										}
									}

									unsigned vertexsize_org  = 0;
									unsigned vertexsize_new  = 0;

									// ÿ��STREAM�Ķ�������Ӧ����һ����
									unsigned numVertices	= m_vertexbufferFillParams[i][0]._vertexCount;

									// ����ʱʹ�õĵ�һ��STREAM��ָ��
									char* dest_data			= (char*)m_vertexbufferFillParams[i][0]._vb_buffer_data;

									// ��ʱָ������
									ATOM_VECTOR<char *> data_array_tmp = data_array;
									
									// ѭ�����ж���
									for( int iVert = 0; iVert < numVertices; ++iVert )
									{
										// ѭ�����е�STREAM
										for (int iStream = 0; iStream < numStreams; ++iStream)
										{
											// �����С
											vertexsize_org	= m_vertexbufferFillParams[i][iStream]._vertexSize;
											vertexsize_new	= vertexsize_org;

											// ����ǿ���ѹ����ʽ��Ⱦ �� �Ƿ�ѹ���汾 �� �������ǿ�ѹ������
											// ����FLOAT��HALF��ת��
											unsigned attr_id = m_vertexbufferFillParams[i][iStream]._attribute_id;
											if( ATOM_RenderSettings::isModelCompressedFormatEnabled() && 
												!(m_modelFlag & MF_COMRESSION) && 
												ATOM_canUseCompress(attr_id) )
											{
												// FLOATת����HALF
												FloatConversion::uint16_t * pVertexData = (FloatConversion::uint16_t*)dest_data;
												if (!pVertexData)
												{
													return false;
												}
												FloatConversion::uint16_t * fp16_ptr = pVertexData;

												float * float_ptr = (float*)data_array_tmp[iStream];

												*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
												*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));

												unsigned component_count_uncompressd;
												unsigned component_count_compressd = ATOM_getCompressAttribComponent(attr_id,component_count_uncompressd);
												if( component_count_uncompressd == 3 )
												{
													*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
													*(fp16_ptr++) = FloatConversion::FloatToHalf(1.0f);				// ʹ��1 Ҳ������ڲ�ʹ��BUMPMAPPINGʱ����normal��Z������������
													// ��ʹ��BUMPMAPPINGʱ����normal��Z�����������ź�tangent��Z������������
													// ʹ��fmod�ķ�����ȡ������
												}
												else if( component_count_uncompressd == 4 )
												{
													*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
													*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
												}

												vertexsize_new = sizeof(unsigned short)*component_count_compressd;
											}
											else
											{
												memcpy( dest_data, data_array_tmp[iStream], vertexsize_org );	// ʹ��MEMCPY���ܻ��ֱ�Ӹ�ֵ������
											}
											
											// ��һ������
											dest_data				+=	vertexsize_new;
											data_array_tmp[iStream] +=	vertexsize_org;
										}
									}

									// ������ʱ����
									for( int iStream = 0; iStream < data_array.size(); ++iStream )
									{
										if( data_array[iStream] )
											ATOM_FREE(data_array[iStream]);
									}
								}
							}
							//-------------------------------
							// ʹ�÷Ǽ�����ʵ������������
							//-------------------------------
							else
							{
								ATOM_MultiStreamGeometry * multi_geo = static_cast<ATOM_MultiStreamGeometry *>(mesh_shared->getGeometry());
								if( multi_geo )
								{
									for( int j = 0; j < m_vertexbufferFillParams[i].size(); ++j )
									{
										unsigned int attr_id = m_vertexbufferFillParams[i][j]._attribute_id;
										ATOM_VertexArray * vertexbuffer = multi_geo->getStream(attr_id);
										if( vertexbuffer )
										{
											if( m_vertexbufferFillParams[i][j]._vb_buffer_data )
											{
												ATOM_ChunkStream * meshChunkStream = (ATOM_ChunkStream *)(m_vertexbufferFillParams[i][j]._stream_buffer);
												if( meshChunkStream )
												{
													// �Ƶ�����λ��
													meshChunkStream->seek(-meshChunkStream->position());
													meshChunkStream->seek(m_vertexbufferFillParams[i][j]._offset);

													// �������ѹ����ʽ��Ⱦ �� �Ƿ�ѹ���汾 �� �������ǿ�ѹ������
													// FLOATת����HALF
													if( ATOM_RenderSettings::isModelCompressedFormatEnabled() && 
														!(m_modelFlag & MF_COMRESSION) && 
														ATOM_canUseCompress(attr_id) )
													{
														unsigned component_count_uncompressd;
														unsigned component_count_compressd = ATOM_getCompressAttribComponent(attr_id,component_count_uncompressd);

														// ��ȡԭ��������
														unsigned vertexcount	= m_vertexbufferFillParams[i][j]._vertexCount;
														unsigned vertexsize		= m_vertexbufferFillParams[i][j]._vertexSize;
														unsigned size			= vertexsize * vertexcount;
														float * float_buffer = ATOM_NEW_ARRAY(float,vertexcount * vertexsize / sizeof(float));
														meshChunkStream->read ((void*)float_buffer, size);
														float * float_ptr = float_buffer;

														// ת����FLOAT16
														FloatConversion::uint16_t * pVertexData = (FloatConversion::uint16_t*)m_vertexbufferFillParams[i][j]._vb_buffer_data;
														if (!pVertexData)
														{
															return false;
														}
														FloatConversion::uint16_t * fp16_ptr = pVertexData;
														for ( int i = 0; i < vertexcount; ++i )
														{
															*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
															*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));

															if( component_count_uncompressd == 3 )
															{
																*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
																*(fp16_ptr++) = FloatConversion::FloatToHalf(1.0f);				// ʹ��1 Ҳ������ڲ�ʹ��BUMPMAPPINGʱ����normal��Z������������
																// ��ʹ��BUMPMAPPINGʱ����normal��Z�����������ź�tangent��Z������������
																// ʹ��fmod�ķ�����ȡ������
															}
															else if( component_count_uncompressd == 4 )
															{
																*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
																*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
															}
														}
													}
													// ֱ�Ӷ�ȡ���ݵ�VB
													else
													{
														meshChunkStream->read(	m_vertexbufferFillParams[i][j]._vb_buffer_data,
																				m_vertexbufferFillParams[i][j]._vertexSize * m_vertexbufferFillParams[i][j]._vertexCount);
													}
													
												}
											}
										}
									}
								}
							}
						}
					}
				}

				// �����ʲ���
				_fillMaterialParams();
			}
			// �ϰ汾��MODEL
			else
			{
				ATOM_Model * model = (ATOM_Model*)data;
				if( model )
				{
					// ���BUFFER
					if( !model->realize () )
					{
						ATOM_LOGGER::error ("%s(%s) Fill device buffer data failed.\n", __FUNCTION__, filename.c_str());
						result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;						// ����ʧ��
						errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_DEVICEBUFFER_FILLDATAFAILED;	// ʧ��ԭ�����DEVICE����ʧ��

						// �ͷ�
						GarbageCollect();

						return false;
					}
				}
			}

			// �����ռ�
			if( m_version == 2)
				GarbageCollect();
		}

		return __super::Fill();
	}

	return false;
}
bool ATOM_AsyncableLoadModel::Unlock()
{
	//return __super::Unlock();
	if( !host )
		return false;

	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::log ("%s(%s) start Unlock.\n", __FUNCTION__, filename.c_str());

	bool result = false;
	if( host )
	{
		/*ATOM_SharedModel * model_shared = (ATOM_SharedModel*)(host.get());*/
		ATOM_SharedModel * model_shared = (ATOM_SharedModel*)(host);
		ATOM_ASSERT(model_shared);

		if( model_shared )
		{
			// �°汾��MODEL
			if( m_version == 2 )
			{
				if( model_shared->getNumMeshes() > 0 )
				{
					// ѭ�����е�����
					for( int i = 0; i < model_shared->getNumMeshes(); ++i )
					{
						ATOM_SharedMesh * mesh = model_shared->getMesh(i);
						if( mesh && mesh->getGeometry() )
						{
							//================== ����IB ===============//
							ATOM_IndexArray * indexbuffer = mesh->getGeometry()->getIndices();
							if( indexbuffer )
								indexbuffer->unlock();

							//==================== ����VB ===============//
							
							//------------------------------//
							// ʹ�ü�����ʵ������������
							//------------------------------//
							if( mesh->getGeometry()->getType()==GT_HWINSTANCING )
							{
								ATOM_HWInstancingGeometry * hwinst_geo = static_cast<ATOM_HWInstancingGeometry *>(mesh->getGeometry());
								if( hwinst_geo && hwinst_geo->getStream() && m_vertexbufferFillParams[i][0]._vb_buffer_data )
								{
									hwinst_geo->getStream()->unlock();
								}
							}
							//------------------------------//
							// ʹ�÷Ǽ�����ʵ������������
							//------------------------------//
							else
							{
								ATOM_MultiStreamGeometry * multi_geo = static_cast<ATOM_MultiStreamGeometry *>(mesh->getGeometry());
								if( multi_geo )
								{
									for( int j = 0; j < m_vertexbufferFillParams[i].size(); ++j )
									{
										unsigned int attr_id = m_vertexbufferFillParams[i][j]._attribute_id;
										ATOM_VertexArray * vertexbuffer = multi_geo->getStream(attr_id);
										if( vertexbuffer && m_vertexbufferFillParams[i][j]._vb_buffer_data )
											vertexbuffer->unlock ();
									}
								}
							}

							//////////////////////////////////////////////////////////

							// �����������
							mesh->setMaterial(m_materialFillParams[i]._mat.get());
							m_materialFillParams[i]._mat = 0;
						}
					}

					// ������������
					for( int t = 0; t < m_vecDependents.size(); ++t )
					{
						if(m_vecTexParamHandles[t])
							m_vecTexParamHandles[t]->setTexture( m_vecDependents[t].c_str(), priority );
					}
				}

			}
			// �ϰ汾��MODEL
			else
			{
				ATOM_Model * model = (ATOM_Model*)data;
				if( model )
				{
					// ����
					model->unlock ();

					// ����֮�����
					model_shared->postprocessLoading( model );
				}

				//_garbageCollect();
			}
		}

		result = true;
	}

	// ��������

	GarbageCollect();

	// ���ø���
	__super::Unlock();

	return result;
}
void ATOM_AsyncableLoadModel::AddMeshIndexBuffer( void * stream_buffer, int offset, int index_count )
{
	sDeviceIndexBufferFillParam dibfp(stream_buffer,offset,index_count/**sizeof(short)*/);
	m_indexbufferFillParams.push_back( dibfp );
}
void ATOM_AsyncableLoadModel::AddMeshVertexBufferStream(	int meshid, 
															unsigned int attriid, 
															void * stream_buffer, 
															int offset, 
															int vertexSize, 
															int vertexcount,
															unsigned attribFlag/* = 0*/ )
{
	sDeviceVertexBufferFillParam dvbfp( attriid, stream_buffer, offset, vertexSize, vertexcount, attribFlag );
	if( meshid < m_vertexbufferFillParams.size() )
	{
		m_vertexbufferFillParams[meshid].push_back( dvbfp );
	}
	else
	{
		DeviceVBFpVec vec;
		vec.push_back( dvbfp );
		m_vertexbufferFillParams.push_back( vec );
	}
}
void ATOM_AsyncableLoadModel::AddMeshMaterialParam( void * material_buffer, ATOM_STRING corematfile )
{
	sMaterialFillParam param;
	param._material_buffer = material_buffer;
	param._coreMatFile = corematfile;
	m_materialFillParams.push_back(param);
}
void ATOM_AsyncableLoadModel::GarbageCollect()
{
	_garbageCollect();
}

void ATOM_AsyncableLoadModel::_garbageCollect()
{
	if( !data )
		return;

	if( m_version == 2 )
	{
		ATOM_DELETE(m_modelData._chunkIO);
		m_modelData._chunkIO = 0;
	}
	else
	{
		ATOM_DELETE(m_modelData._model);
		m_modelData._model = 0;
	}
	data = 0;

	//ATOM_LOGGER::error ("%s(%s) GARBAGE COLLCETION.\n", __FUNCTION__, filename.c_str());
}
bool ATOM_AsyncableLoadModel::IsLoadAllFinished()
{
	// ����Ѿ�ȫ������� ֱ�ӷ���TRUE
	if( m_bAllFinished )
		return true;

	// ��������ɱ��
	if( false == __super::IsLoadAllFinished() )
		return false;

	// ������е��������Ƿ���ɣ�����������
	ATOM_VECTOR<ATOM_STRING>::iterator iter = m_vecDependents.begin();
	for( ; iter != m_vecDependents.end(); ++iter )
	{
		// �������첽���ص���Դ����ȥ�Ҹ���������ҵ����Ҽ�����ɣ������
		if( !ATOM_AsyncLoader::IsResourceFinshed(*iter) )
		{
			// ���û���ҵ������ܸ�������ͬ�����صģ�����ģ��ʹ�ø�������ô���Ҹ������������ļ��ر��
			ATOM_AUTOREF(ATOM_Texture) texture = ATOM_LookupObject ( "ATOM_D3DTexture", (*iter).c_str() );
			// �����Ȼû�м������
			if( !texture.get() || !texture->getAsyncLoader()->IsLoadAllFinished() )
				return false;
		}
	}

	// ȫ������� ������ɱ��
	m_bAllFinished = true;

	return true;
}

void ATOM_AsyncableLoadModel::AddDependent(ATOM_STRING dep, ATOM_ParameterTable::ValueHandle texHandle)
{
	if(dep.empty())
		return;
	std::transform( dep.begin(), dep.end(), dep.begin(), ::tolower );
	m_vecDependents.push_back(dep);
	m_vecTexParamHandles.push_back(texHandle);
}

void ATOM_AsyncableLoadModel::processAllDependents()
{
	// ������е���������
	ATOM_VECTOR<ATOM_STRING>::iterator iter = m_vecDependents.begin();	
	for( ; iter != m_vecDependents.end(); ++iter )
	{
		if( ATOM_AsyncLoader::isEnableLog() )
		{
			ATOM_AUTOREF(ATOM_Texture) texture = ATOM_LookupObject ("ATOM_D3DTexture", (*iter).c_str());
			if( !texture )
			{
				ATOM_LOGGER::log("%s is Destroyed! Recreate the texture! \n",(*iter).c_str());
			}
			else
			{
				if( texture->getAsyncLoader() )
				{
					if( !texture->getAsyncLoader()->IsLoadAllFinished() )
						ATOM_LOGGER::log( "%s is Exist & it is in stage %u !\n",(*iter).c_str(), flag );
					else
						ATOM_LOGGER::log( "%s is Exist & it is all loaded !\n",(*iter).c_str());
				}
			}
		}

		// ���¼�������
		ATOM_CreateTextureResource((*iter).c_str(),ATOM_PIXEL_FORMAT_UNKNOWN,priority);	
	}
}


//---------------------------------------------------------------------//
//--- wangjian added ---//
// �첽����
ATOM_AsyncableLoadModel * ATOM_SharedModel::getAsyncLoader()
{
	if (!_asyncLoader)
	{
		_asyncLoader = ATOM_NEW(ATOM_AsyncableLoadModel);
		_asyncLoader->host = this;
	}
	return _asyncLoader.get();
}
bool ATOM_SharedModel::fillModelData(ATOM_RenderDevice *device, int version)
{
	if( !device )
		return false;

	unsigned int data_size = 0;
	void* data = getAsyncLoader()->GetBuffer(data_size);
	if( !data )
		return false;

	if(version==2)
	{
		ATOM_ChunkIO * chunkIO = (ATOM_ChunkIO *)data;
		if( chunkIO )
		{
			ATOM_ChunkIO::Chunk *topChunk = chunkIO->getTopChunk ();
			ATOM_ChunkIO::Chunk *mainChunk = topChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','m','e'));
			if (!mainChunk)
			{
				return false;
			}
			ATOM_ChunkStream *mainChunkStream = mainChunk->getChunkStream();
			int version = mainChunkStream->readInt ();
			int numMeshes = mainChunkStream->readInt ();

#if 1
			int flags = mainChunkStream->readInt ();
			getAsyncLoader()->SetModelFlag(flags);
#else
			int numSkeletons = mainChunkStream->readInt ();
#endif
			int numTracks = mainChunkStream->readInt ();
			mainChunkStream->read (&_boundingbox, sizeof(ATOM_BBox));

			// read skeleton
			ATOM_ChunkIO::Chunk *skeletonChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','s','k'));
			if (skeletonChunk)
			{
				ATOM_ChunkStream *skeletonChunkStream = skeletonChunk->getChunkStream();
				ATOM_AUTOPTR(ATOM_Skeleton) skeleton = ATOM_NEW(ATOM_Skeleton);
				skeleton->setHashCode (skeletonChunkStream->readInt ());
				skeleton->setNumJoints (skeletonChunkStream->readInt ());
				skeletonChunkStream->read (&skeleton->getJointMatrices()[0], sizeof(skeleton->getJointMatrices()[0])*skeleton->getJointMatrices().size());
				skeletonChunkStream->read (&skeleton->getJointParents()[0], sizeof(skeleton->getJointParents()[0])*skeleton->getJointParents().size());
				for (unsigned i = 0; i < skeleton->getNumJoints(); ++i)
				{
					char buffer[256];
					skeletonChunkStream->readCString (buffer);
					skeleton->addAttachPoint (buffer, i);
				}
				_skeleton = skeleton;
			}

			// read animation datas.
			ATOM_ChunkIO::Chunk *jointAnimationDataChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('j','a','d','t'));
			if (jointAnimationDataChunk)
			{
				ATOM_ChunkStream *jointAnimationDataChunkStream = jointAnimationDataChunk->getChunkStream ();
				_animationDataCache = ATOM_NEW(ATOM_JointAnimationDataCache);
				unsigned numFrames = jointAnimationDataChunkStream->readInt ();
				_animationDataCache->setNumFrames (numFrames);
				for (unsigned i = 0; i < numFrames; ++i)
				{
					unsigned numJoints = jointAnimationDataChunkStream->readInt ();
					_animationDataCache->setNumJoints (i, numJoints);
					jointAnimationDataChunkStream->read (_animationDataCache->getJoints (i), sizeof(ATOM_JointTransformInfo)*numJoints);
				}

				ATOM_ChunkIO::Chunk *jointAnimationTrackChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('j','a','t','k'));
				while (jointAnimationTrackChunk)
				{
					ATOM_ChunkStream *jointAnimationTrackChunkStream = jointAnimationTrackChunk->getChunkStream ();
					ATOM_AUTOPTR(ATOM_JointAnimationTrack) track = ATOM_NEW(ATOM_JointAnimationTrack, this);
					track->setAnimationDataCache (_animationDataCache.get());
					int num = jointAnimationTrackChunkStream->readCString (0);
					jointAnimationTrackChunkStream->seek (-num);
					char *s = (char*)ATOM_MALLOC(num);
					jointAnimationTrackChunkStream->readCString (s);
					track->setName (s);
					ATOM_FREE(s);
					int numFrames = jointAnimationTrackChunkStream->readInt ();
					track->setNumFrames (numFrames);
					jointAnimationTrackChunkStream->read (track->getFrameJointIndices(), numFrames*sizeof(unsigned));
					jointAnimationTrackChunkStream->read (track->getFrameTimes(), numFrames*sizeof(unsigned));
					_tracks.push_back (track);

					jointAnimationTrackChunk = jointAnimationTrackChunk->getNextSibling (ATOM_MAKE_FOURCC('j','a','t','k'));
				}
			}

			///////////////////////////////////////////////////////////////////////////////////////////////////////

			// read sub meshes
			ATOM_ChunkIO::Chunk *meshChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','s','m'));
			int iMesh_index = 0;
			while (meshChunk)
			{
				//-------- wangjian modified ----------//
				// ���ȶ�ȡ�����ļ�����
				ATOM_ChunkIO::Chunk *materialChunk = meshChunk->getFirstChild (ATOM_MAKE_FOURCC('m','a','t','l'));

				char *buffer = 0;
				if( materialChunk )
				{
					int size = materialChunk->getChunkStream()->readCString (0);
					materialChunk->getChunkStream()->seek (-size);
					buffer = ATOM_NEW_ARRAY(char, size);
					materialChunk->getChunkStream()->readCString (buffer);

					// read sub mesh material
					// ��ȡ������Ĳ��ʲ��Ҵ������ʶ��󣬿�����������
					// �����п��ܻᴴ�����Ĳ��ʶ�����ô���ܴ���SHADER����Ȼ���Ĳ��ʶ����ڳ���ʼִ��ʱ��Ӧ���Ѿ�ȫ�����ش��������Բ��ܱ�֤һ������ػ��߼�����ȫ��
					// ���������п��ܻ��첽����������ô���ܴ����������
					// �����߶����ܵ������⡣
					// ���Խ����ʵĶ�ȡ�Ƴ�ȥ��������IO�߳��н��С�
					// ��Ӳ���������
					getAsyncLoader()->AddMeshMaterialParam( materialChunk, buffer );
				}
				//====================================//

				ATOM_ChunkStream *meshChunkStream = meshChunk->getChunkStream();

				ATOM_SharedMesh *mesh = ATOM_NEW(ATOM_SharedMesh, this);

				// read mesh name
				int numchars = meshChunkStream->readCString (0);
				meshChunkStream->seek (-numchars);
				char *name = (char*)ATOM_MALLOC(numchars);
				meshChunkStream->readCString (name);
				mesh->setName (name);
				ATOM_FREE(name);

				// read mesh joint mapping data
				int numMappings = meshChunkStream->readInt ();
				if (numMappings > 0)
				{
					ATOM_VECTOR<int> jointMap(numMappings);
					meshChunkStream->read (&jointMap[0], numMappings*sizeof(int));
					mesh->setJointMap (jointMap);
				}

				// read vertex streams and index data
				//--- wangjian added ---//
				// ��ǰ���� SKINNING ��ģ�ͷ�Ϊ3�֣�
				// 1. ֻ��MESH��Ϣ û�й�����Ϣ Ҳ û�ж�����Ϣ numTracks = 0
				// 2. û�й�����Ϣ Ҳ û�ж�����Ϣ numTracks > 0
				// 3. ���й�����Ϣ ���ж�����Ϣ 
				bool bNoneSkin			= ( numTracks == 0 ) && (skeletonChunk) && (!jointAnimationDataChunk);
				if( bNoneSkin )
				{
					ATOM_VECTOR<int> jointMap;
					mesh->setJointMap(jointMap);
				}
				bool bSkinniedMesh = !mesh->getJointMap().empty();
				bool bHWInstancingMesh = useHWInstancingMesh(bSkinniedMesh,buffer?buffer:"");
				if( bHWInstancingMesh )
					mesh->setGeometry (ATOM_NEW(ATOM_HWInstancingGeometry));	// �������û�й�����Ϣ��ʹ��HWInstancing Geometry
				else
					mesh->setGeometry (ATOM_NEW(ATOM_MultiStreamGeometry));		// �������Ƥ���� ʹ��MultiStream Geometry
				if( buffer )
					ATOM_DELETE_ARRAY(buffer);
				//----------------------//

				int numStreams = meshChunkStream->readInt ();
				int numVertices = meshChunkStream->readInt ();
				int numIndices = meshChunkStream->readInt ();
				ATOM_BBox bbox;
				meshChunkStream->read (&bbox, sizeof(ATOM_BBox));
				mesh->setBoundingbox (bbox);

				//--------------- IB -------------//
				// ���IB��������
				getAsyncLoader()->AddMeshIndexBuffer((void*)meshChunkStream,meshChunkStream->position(),numIndices);

				// ��ǰ
				meshChunkStream->seek(sizeof(short)*numIndices);

				//--------------- VB -------------//
				for (int i = 0; i < numStreams; ++i)
				{
					unsigned attrib = meshChunkStream->readInt();

					unsigned attribFlag = ATTRIBUTE_FLAG_NONE;
#if 0
					if( flags & MF_COMRESSION )
					{
						if( ATOM_canUseCompress(attrib) )
							attribFlag |= ATTRIBUTE_FLAG_COMPRESSED;
					}
#else
					// �������ģ��ѹ����ʽ��Ⱦ ���� ����ģ��Ϊѹ����ʽ
					if( ATOM_RenderSettings::isModelCompressedFormatEnabled() || flags & MF_COMRESSION )
					{
						// ���������Ϊ��ѹ������
						if( ATOM_canUseCompress(attrib) )
						{
							attribFlag |= ATTRIBUTE_FLAG_COMPRESSED;
						}
					}
					
#endif

					unsigned vertexSize = ATOM_GetVertexSize ( attrib, flags & MF_COMRESSION ? attribFlag : 0 );
					if (!vertexSize)
					{
						ATOM_DELETE(mesh);
						return false;
					}
					
					// ���VB��������
					getAsyncLoader()->AddMeshVertexBufferStream(	iMesh_index, 
																	attrib, 
																	(void*)meshChunkStream, 
																	meshChunkStream->position(), 
																	vertexSize,
																	numVertices,
																	attribFlag	);

					// ��ǰ
					unsigned dataSize = numVertices * vertexSize;
					meshChunkStream->seek(dataSize);
				}
				//-------------------------------------//

				// ����������������
				_meshes.push_back (mesh);

				// ��һ������
				meshChunk = meshChunk->getNextSibling (ATOM_MAKE_FOURCC('a','t','s','m'));

				// ����MESH��ID����
				iMesh_index++;
			}

			// wangjian commented
#if 0
			if (_meshes.size() != numMeshes)
			{
				return false;
			}
#endif
			///////////////////////////////////////////////////////////////////////////////////////////////////////

			ATOM_ChunkIO::Chunk *userAttribChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','t','r'));
			if (userAttribChunk)
			{
				while (userAttribChunk)
				{
					readUserAttributes (_userAttributes, userAttribChunk->getChunkStream());
					userAttribChunk = userAttribChunk->getNextSibling (ATOM_MAKE_FOURCC('a', 't', 't', 'r'));
				}
			}
		}
	}
	else
	{
		
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////
// ����֮�����
bool ATOM_SharedModel::postprocessLoading( ATOM_Model * pModel )
{
	if( !pModel )
		return false;

	// ���ù���
	_skeleton = pModel->getSkeleton();

	// ����
	if ( pModel->getNumTracks() > 0 )
	{
		ATOM_ModelAnimationDataCache *cache = pModel->getTrack (0)->getAnimationDatas ();
		if (cache)
		{
			if (cache->getNumJointFrames () > 0)
			{
				_animationDataCache = ATOM_NEW(ATOM_JointAnimationDataCache);
				_animationDataCache->setNumFrames (cache->getNumJointFrames());
				for (unsigned i = 0; i < cache->getNumJointFrames(); ++i)
				{
					_animationDataCache->setJoints (i, cache->getJoints (i), cache->getJointCount (i));
				}

				for (unsigned i = 0; i < pModel->getNumTracks(); ++i)
				{
					ATOM_ModelAnimationTrack *track = pModel->getTrack (i);
					ATOM_AUTOPTR(ATOM_JointAnimationTrack) newTrack = ATOM_NEW(ATOM_JointAnimationTrack, this);
					newTrack->setAnimationDataCache (_animationDataCache.get());
					newTrack->setName (track->getName ());
					newTrack->setNumFrames (track->getNumFrames ());
					for (unsigned frame = 0; frame < track->getNumFrames(); ++frame)
					{
						newTrack->setFrameTime (frame, track->getFrame(frame).frameTime);
						newTrack->setFrameJointIndex (frame, track->getJointIndex (frame));
					}
					_tracks.push_back (newTrack);
				}
			}
		}
	}

	_boundingbox.beginExtend ();
	_meshes.resize (pModel->getNumMeshes());
	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		_meshes[i] = ATOM_NEW(ATOM_SharedMesh, this);
		_meshes[i]->setGeometry (pModel->getMesh(i)->geometry->clone ());
		_meshes[i]->setJointMap (pModel->getMesh(i)->jointMap);
		_meshes[i]->setBoundingbox (pModel->getMesh(i)->boundingBox);

#if 0
		// calculate tangents
		ATOM_MultiStreamGeometry *geo = (ATOM_MultiStreamGeometry*)(_meshes[i]->getGeometry());
		ATOM_VertexArray *vertices = geo->getStream (ATOM_VERTEX_ATTRIB_COORD);
		ATOM_VertexArray *normals = geo->getStream (ATOM_VERTEX_ATTRIB_NORMAL);
		ATOM_VertexArray *texcoords = geo->getStream (ATOM_VERTEX_ATTRIB_TEX1_2);
		ATOM_VertexArray *tangents = geo->getStream (ATOM_VERTEX_ATTRIB_TANGENT);
		ATOM_VertexArray *binormals = geo->getStream (ATOM_VERTEX_ATTRIB_BINORMAL);
		ATOM_IndexArray *indices = geo->getIndices ();
		if (0 && vertices && normals && texcoords && indices /*&& (!tangents||!binormals)*/)
		{
			unsigned numVertices = vertices->getNumVertices ();
			unsigned numTris = indices->getNumIndices() / 3;

			ATOM_AUTOREF(ATOM_VertexArray) tangents = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_TANGENT, ATOM_USAGE_STATIC, numVertices, true);
			ATOM_AUTOREF(ATOM_VertexArray) binormals = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_BINORMAL, ATOM_USAGE_STATIC, numVertices, true);

			ATOM_Vector3f *vertex = (ATOM_Vector3f *)vertices->lock (ATOM_LOCK_READONLY, 0, 0, false);
			ATOM_Vector3f *normal = (ATOM_Vector3f *)normals->lock (ATOM_LOCK_READONLY, 0, 0, false);
			ATOM_Vector2f *texcoord = (ATOM_Vector2f *)texcoords->lock (ATOM_LOCK_READONLY, 0, 0, false);
			ATOM_Vector3f *tangent = (ATOM_Vector3f*)tangents->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			ATOM_Vector3f *binormal = (ATOM_Vector3f*)binormals->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			unsigned short *index = (unsigned short*)indices->lock (ATOM_LOCK_READONLY, 0, 0, false);

			calculateTangentBasis (numVertices, vertex, normal, texcoord, numTris, index, tangent, binormal);

			vertices->unlock ();
			normals->unlock ();
			texcoords->unlock ();
			tangents->unlock ();
			binormals->unlock ();
			indices->unlock ();

			geo->addStream (tangents.get());
			geo->addStream (binormals.get());
		}
#endif

		//--- �������� ---//

		//--------------------------- wangjian modified --------------------------//
		// ѡ����ʵĺ��Ĳ����ļ�
		ATOM_STRING coremat_name( "" );
		chooseProperCoreMatName(coremat_name,_meshes[i]->getGeometry()->getType()==GT_HWINSTANCING);
		ATOM_AUTOPTR(ATOM_Material) material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), coremat_name.c_str());
		//------------------------------------------------------------------------//

		_meshes[i]->setMaterial (material.get());

		if (material)
		{
			ATOM_Mesh *mesh = pModel->getMesh(i);
			material->getParameterTable()->setTexture ("diffuseTexture", mesh->diffuseTextureName.c_str());
			material->getParameterTable()->setInt ("hasDiffuseTexture", mesh->diffuseTextureName.empty()?0:1);
			material->getParameterTable()->setInt ("hasVertexColor", mesh->colors?1:0);
			material->getParameterTable()->setInt ("enableAlphaBlend", mesh->transparency!=1.f?1:0);
			material->getParameterTable()->setInt ("enableAlphaTest", mesh->alphaTest?1:0);
			material->getParameterTable()->setFloat ("alpharef", mesh->alphaTest?0.5f:0);
			material->getParameterTable()->setInt ("cullmode", mesh->doubleSide?ATOM_RenderAttributes::CullMode_None:ATOM_RenderAttributes::CullMode_Back);
			material->getParameterTable()->setVector ("transparency", ( mesh->transparency == 0.f ? ATOM_Vector4f(1.0f) : ATOM_Vector4f(1,1,1,mesh->transparency) ) );
			//material->getParameterTable()->setFloat ("transparency", (mesh->transparency == 0.f ? 1.f : mesh->transparency));
			/*material->getParameterTable()->setFloat ("Kd", 1.f);
			material->getParameterTable()->setFloat ("Ks", 1.f);*/
			material->getParameterTable()->setVector ("diffuseColor", ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
			material->getParameterTable()->setFloat ("shininess", 64.f);
			material->getParameterTable()->setFloat ("glossness", 0.f);

			//----- ���ò��ʵ�ȫ��ID ----//
			char strMatName[256] = {0};
			sprintf(strMatName,"%s_%u",_asyncLoader->filename.c_str(),i);
			material->setMaterialId(strMatName);
			//--------------------------//
		}
		
		_boundingbox.extend (pModel->getMesh(i)->boundingBox.getMin());
		_boundingbox.extend (pModel->getMesh(i)->boundingBox.getMax());
	}
	return true;
}

//void ATOM_SharedModel::setAnimationTexture(ATOM_Texture* animTex)
//{
//	_animationTexture = animTex;
//}
//ATOM_Texture* ATOM_SharedModel::getAnimationTexture(void)
//{
//	return _animationTexture.get();
//}
//----------------------//

ATOM_SharedModel::ATOM_SharedModel (void)
{
	_userAttributes = ATOM_NEW(ATOM_UserAttributes);
	//--- wangjian added ---//
	_asyncLoader = 0;
	//_animationTexture = 0;
	//----------------------//
}

ATOM_SharedModel::~ATOM_SharedModel (void)
{
	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::log("%s : %s [ Address : %x ] is Destroyed \n",	__FUNCTION__, 
																		getObjectName() ? getObjectName() : "",
																		_asyncLoader.get() ? (unsigned)_asyncLoader.get() : 0	);
	// ��host����Ϊ0
	if( _asyncLoader.get() )
		_asyncLoader->host = 0;
	ATOM_AsyncLoader::OnAsyncLoadObjectFinished(_asyncLoader.get());

	unload ();

	ATOM_DELETE(_userAttributes);
}

bool ATOM_SharedModel::load_obj(ATOM_RenderDevice* device, const char* filename, bool bMt) {
	unload();
	wchar_t mtlFileName[256] = { 0 };
	wchar_t strCommand[256] = { 0 };
	ATOM_MAP<ATOM_WSTRING, ObjMesh*> objMeshes;
	ObjMesh* curMesh = 0;

	std::wifstream fileIn(filename);
	if (!fileIn) 
	{
		return false;
	}
	for (;;)
	{
		fileIn >> strCommand;
		if (!fileIn) 
		{
			break;
		}
		if (0 == wcscmp(strCommand, L"#"))
		{
			// comment
		}
		else if (0 == wcscmp(strCommand, L"mtllib"))
		{
			fileIn >> mtlFileName;
		}
		else if (0 == wcscmp(strCommand, L"usemtl"))
		{
			wchar_t name[256];
			fileIn >> name;
			ATOM_MAP<ATOM_WSTRING, ObjMesh*>::iterator it = objMeshes.find(name);
			if (it == objMeshes.end())
			{
				curMesh = new ObjMesh;
				objMeshes[name] = curMesh;
			}
			else
			{
				curMesh = it->second;
			}
		}
		else if (0 == wcscmp(strCommand, L"v"))
		{
			if (!curMesh)
			{
				curMesh = new ObjMesh;
				objMeshes[L"_"] = curMesh;
			}
			float x, y, z;
			fileIn >> x >> y >> z;
			curMesh->positions.push_back(ATOM_Vector3f(x, y, z));
		}
		else if (0 == wcscmp(strCommand, L"vt"))
		{
			if (!curMesh)
			{
				curMesh = new ObjMesh;
				objMeshes[L"_"] = curMesh;
			}
			float u, v;
			fileIn >> u >> v;
			curMesh->texcoords.push_back(ATOM_Vector2f(u, v));
		}
		else if (0 == wcscmp(strCommand, L"vn"))
		{
			if (!curMesh)
			{
				curMesh = new ObjMesh;
				objMeshes[L"_"] = curMesh;
			}
			float x, y, z;
			fileIn >> x >> y >> z;
			curMesh->normals.push_back(ATOM_Vector3f(x, y, z));
		}
		else if (0 == wcscmp(strCommand, L"f"))
		{
			if (!curMesh)
			{
				curMesh = new ObjMesh;
				objMeshes[L"_"] = curMesh;
			}
			int pos, tex, normal;
			ObjVertex vertex;
			for (int i = 0; i < 3; i++)
			{
				vertex.pos.set(0.f, 0.f, 0.f);
				vertex.normal.set(0.f, 0.f, 0.f);
				vertex.uv.set(0.f, 0.f);
				fileIn >> pos;
				vertex.pos = curMesh->positions[pos - 1];
				if ('/' == fileIn.peek())
				{
					fileIn.ignore();
					if ('/' != fileIn.peek())
					{
						fileIn >> tex;
						vertex.uv = curMesh->texcoords[tex - 1];
					}
					if ('/' != fileIn.peek())
					{
						fileIn.ignore();
						fileIn >> normal;
						vertex.normal = curMesh->normals[normal - 1];
					}
				}
				unsigned short index;
				bool found = false;
				ATOM_HASHMAP<int, ATOM_VECTOR<int> >::iterator it = curMesh->vertexMap.find(pos);
				if (it == curMesh->vertexMap.end()) 
				{
					curMesh->vertexMap[pos] = ATOM_VECTOR<int>();
				}
				else
				{
					for (unsigned k = 0; k < it->second.size(); k++)
					{
						if (curMesh->vertices[it->second[k]] == vertex)
						{
							index = it->second[k];
							found = true;
							break;
						}
					}
				}
				if (!found)
				{
					index = curMesh->vertices.size();
					curMesh->vertexMap[pos].push_back(index);
					curMesh->vertices.push_back(vertex);
				}
				curMesh->indices.push_back(index);
			}
		}
	}
	return false;
}

//--------------------------- wangjian modified -------------------------------//
// �첽���ز�����õ��ˣ�ֻ��ͬ���Ż�
bool ATOM_SharedModel::load_nm (ATOM_RenderDevice *device, const char *filename, bool bMt)
{
	unload ();

	{
		ATOM_HARDREF(ATOM_Model) model;
		if (!model->load (device, filename))
		{
			return false;
		}

		_skeleton = model->getSkeleton();

		if (model->getNumTracks() > 0)
		{
			ATOM_ModelAnimationDataCache *cache = model->getTrack (0)->getAnimationDatas ();
			if (cache)
			{
				if (cache->getNumJointFrames () > 0)
				{
					_animationDataCache = ATOM_NEW(ATOM_JointAnimationDataCache);
					_animationDataCache->setNumFrames (cache->getNumJointFrames());
					for (unsigned i = 0; i < cache->getNumJointFrames(); ++i)
					{
						_animationDataCache->setJoints (i, cache->getJoints (i), cache->getJointCount (i));
					}

					for (unsigned i = 0; i < model->getNumTracks(); ++i)
					{
						ATOM_ModelAnimationTrack *track = model->getTrack (i);
						ATOM_AUTOPTR(ATOM_JointAnimationTrack) newTrack = ATOM_NEW(ATOM_JointAnimationTrack, this);
						newTrack->setAnimationDataCache (_animationDataCache.get());
						newTrack->setName (track->getName ());
						newTrack->setNumFrames (track->getNumFrames ());
						for (unsigned frame = 0; frame < track->getNumFrames(); ++frame)
						{
							newTrack->setFrameTime (frame, track->getFrame(frame).frameTime);
							newTrack->setFrameJointIndex (frame, track->getJointIndex (frame));
						}
						_tracks.push_back (newTrack);
					}
				}
			}
		}

		_boundingbox.beginExtend ();
		_meshes.resize (model->getNumMeshes());
		for (unsigned i = 0; i < _meshes.size(); ++i)
		{
			_meshes[i] = ATOM_NEW(ATOM_SharedMesh, this);
			_meshes[i]->setGeometry (model->getMesh(i)->geometry->clone ());
			_meshes[i]->setJointMap (model->getMesh(i)->jointMap);
			_meshes[i]->setBoundingbox (model->getMesh(i)->boundingBox);

			// wangjian added: Ĭ�ϵ����NMģ�Ͳ�Ӧ�ð���������Ϣ
			/*ATOM_MultiStreamGeometry *geo = (ATOM_MultiStreamGeometry*)(_meshes[i]->getGeometry());
			ATOM_VertexArray *tangents = geo->getStream (ATOM_VERTEX_ATTRIB_TANGENT);
			if( tangents )
				geo->removeStream(tangents);
			ATOM_VertexArray *binormals = geo->getStream (ATOM_VERTEX_ATTRIB_BINORMAL);
			if( binormals )
				geo->removeStream(binormals);*/

#if 0
			// calculate tangents
			ATOM_MultiStreamGeometry *geo = (ATOM_MultiStreamGeometry*)(_meshes[i]->getGeometry());
			ATOM_VertexArray *vertices = geo->getStream (ATOM_VERTEX_ATTRIB_COORD);
			ATOM_VertexArray *normals = geo->getStream (ATOM_VERTEX_ATTRIB_NORMAL);
			ATOM_VertexArray *texcoords = geo->getStream (ATOM_VERTEX_ATTRIB_TEX1_2);
			ATOM_VertexArray *tangents = geo->getStream (ATOM_VERTEX_ATTRIB_TANGENT);
			ATOM_VertexArray *binormals = geo->getStream (ATOM_VERTEX_ATTRIB_BINORMAL);
			ATOM_IndexArray *indices = geo->getIndices ();
			if (0 && vertices && normals && texcoords && indices /*&& (!tangents||!binormals)*/)
			{
				unsigned numVertices = vertices->getNumVertices ();
				unsigned numTris = indices->getNumIndices() / 3;

				ATOM_AUTOREF(ATOM_VertexArray) tangents = device->allocVertexArray (ATOM_VERTEX_ATTRIB_TANGENT, ATOM_USAGE_STATIC, numVertices, true);
				ATOM_AUTOREF(ATOM_VertexArray) binormals = device->allocVertexArray (ATOM_VERTEX_ATTRIB_BINORMAL, ATOM_USAGE_STATIC, numVertices, true);

				ATOM_Vector3f *vertex = (ATOM_Vector3f *)vertices->lock (ATOM_LOCK_READONLY, 0, 0, false);
				ATOM_Vector3f *normal = (ATOM_Vector3f *)normals->lock (ATOM_LOCK_READONLY, 0, 0, false);
				ATOM_Vector2f *texcoord = (ATOM_Vector2f *)texcoords->lock (ATOM_LOCK_READONLY, 0, 0, false);
				ATOM_Vector3f *tangent = (ATOM_Vector3f*)tangents->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				ATOM_Vector3f *binormal = (ATOM_Vector3f*)binormals->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				unsigned short *index = (unsigned short*)indices->lock (ATOM_LOCK_READONLY, 0, 0, false);

				calculateTangentBasis (numVertices, vertex, normal, texcoord, numTris, index, tangent, binormal);

				vertices->unlock ();
				normals->unlock ();
				texcoords->unlock ();
				tangents->unlock ();
				binormals->unlock ();
				indices->unlock ();

				geo->addStream (tangents.get());
				geo->addStream (binormals.get());
			}
#endif

			//--- �������� ---//

			//--------------------------- wangjian modified --------------------------//
			// ѡ����ʵĺ��Ĳ����ļ�
			ATOM_STRING coremat_name( "" );
			chooseProperCoreMatName(coremat_name,_meshes[i]->getGeometry()->getType()==GT_HWINSTANCING);
			//------------------------------------------------------------------------//

			ATOM_AUTOPTR(ATOM_Material) material = ATOM_MaterialManager::createMaterialFromCore (device, coremat_name.c_str());
			_meshes[i]->setMaterial (material.get());
			if (material)
			{
				ATOM_Mesh *mesh = model->getMesh(i);

				//--- wangjian added ---//
				// �첽����
				// �����ģ����Դֱ�Ӽ��أ���ͨ���첽���أ�������·�����浽immediate table
				if( !mesh->diffuseTextureName.empty() )
					ATOM_AsyncLoader::AddToImmediateTable(mesh->diffuseTextureName.c_str());
				//----------------------//

				material->getParameterTable()->setTexture ("diffuseTexture", mesh->diffuseTextureName.c_str());
				material->getParameterTable()->setInt ("hasDiffuseTexture", mesh->diffuseTextureName.empty()?0:1);
				material->getParameterTable()->setInt ("hasVertexColor", mesh->colors?1:0);
				material->getParameterTable()->setInt ("enableAlphaBlend", mesh->transparency!=1.f?1:0);
				material->getParameterTable()->setInt ("enableAlphaTest", mesh->alphaTest?1:0);
				material->getParameterTable()->setFloat ("alpharef", mesh->alphaTest?0.5f:0);
				material->getParameterTable()->setInt ("cullmode", mesh->doubleSide?ATOM_RenderAttributes::CullMode_None:ATOM_RenderAttributes::CullMode_Back);
				material->getParameterTable()->setVector ("transparency", ( mesh->transparency == 0.f ? ATOM_Vector4f(1.0f) : ATOM_Vector4f(1,1,1,mesh->transparency) ) );
				//material->getParameterTable()->setFloat ("transparency", (mesh->transparency == 0.f ? 1.f : mesh->transparency));
				/*material->getParameterTable()->setFloat ("Kd", 1.f);
				material->getParameterTable()->setFloat ("Ks", 1.f);*/
				material->getParameterTable()->setVector ("diffuseColor", ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
				material->getParameterTable()->setFloat ("shininess", 64.f);
				material->getParameterTable()->setFloat ("glossness", 0.f);

				//----- ���ò��ʵ�ȫ��ID ----//
				char strMatName[256] = {0};
				sprintf(strMatName,"%s_%u",filename,i);
				material->setMaterialId(strMatName);
				//--------------------------//
			}
			_boundingbox.extend (model->getMesh(i)->boundingBox.getMin());
			_boundingbox.extend (model->getMesh(i)->boundingBox.getMax());
		}
	}

	// ���سɹ� ������ɱ��
	getAsyncLoader()->SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED);

	return true;
}

void ATOM_SharedModel::unload (void)
{
	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		ATOM_DELETE(_meshes[i]);
	}
	_meshes.clear ();
	_tracks.clear ();
	_animationDataCache = 0;
	_skeleton = 0;
	_userAttributes->clearAttributes ();
}

#include "kernel/profiler.h"

bool ATOM_SharedModel::load (ATOM_RenderDevice *device, const char *filename, int loadPriority)
{
	unload ();

//	//--- wangjian added for profile ---//
//#if 0 
//	ATOM_Profiler sharemodel_profiler(filename);
//#endif
//	//----------------------------------//

	//--- wangjian modified ---//
	// �첽����
	bool bMt = ( ATOM_AsyncLoader::IsRun() ) && ( loadPriority != ATOM_LoadPriority_IMMEDIATE );

	// ������첽����
	if( ATOM_AsyncLoader::IsRun() && bMt )
	{
		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::log("ATOM_SharedModel::load --- async load model : %s | priority : %d\n", filename, loadPriority );

		getAsyncLoader()->SetModel(this);
		if( !getAsyncLoader()->Start(filename,loadPriority) )
			return false;
	}
	// ֱ�Ӽ���
	else
	{
		if( ATOM_AsyncLoader::isEnableLog() )
		{
			ATOM_LOGGER::log("ATOM_SharedModel::load --- sync load model : %s \n", filename );
		}

		const char* ext = strrchr(filename, '.');
		if (ext && !stricmp(ext, ".obj")) {
			return load_obj(device, filename, bMt);
		}

		unsigned sig;

		{
			ATOM_AutoFile f (filename, ATOM_VFS::read);
			if (!f)
			{
				return false;
			}
			if (f->size() < sizeof(unsigned))
			{
				return false;
			}
			f->read (&sig, sizeof(unsigned));
		}

		// �ϰ汾��ģ�Ͷ�ȡ
		if (sig == 0x4e334d46)
		{
			//--- wangjian modified ---//
			return load_nm(device, filename, bMt);
		}

		// �°汾��ģ�Ͷ�ȡ

		ATOM_ChunkIO chunkIO;
		if (!chunkIO.load (filename))
		{
			return false;
		}

		ATOM_ChunkIO::Chunk *topChunk = chunkIO.getTopChunk ();
		ATOM_ChunkIO::Chunk *mainChunk = topChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','m','e'));
		if (!mainChunk)
		{
			return false;
		}
		ATOM_ChunkStream *mainChunkStream = mainChunk->getChunkStream();
		int version = mainChunkStream->readInt ();
		int numMeshes = mainChunkStream->readInt ();

		//-----------------------------------------------------------//
		// wangjian modified 
		// numSkeletons��δ�õ� �����Խ������Ϊѹ����ʽ�����ֱ��
#if 0
		int numSkeletons = mainChunkStream->readInt ();
#else 
		int flags = mainChunkStream->readInt ();
#endif
		//-----------------------------------------------------------//

		int numTracks = mainChunkStream->readInt ();
		mainChunkStream->read (&_boundingbox, sizeof(ATOM_BBox));

		// read skeleton
		ATOM_ChunkIO::Chunk *skeletonChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','s','k'));
		if (skeletonChunk)
		{
			ATOM_ChunkStream *skeletonChunkStream = skeletonChunk->getChunkStream();
			ATOM_AUTOPTR(ATOM_Skeleton) skeleton = ATOM_NEW(ATOM_Skeleton);
			skeleton->setHashCode (skeletonChunkStream->readInt ());
			skeleton->setNumJoints (skeletonChunkStream->readInt ());
			skeletonChunkStream->read (&skeleton->getJointMatrices()[0], sizeof(skeleton->getJointMatrices()[0])*skeleton->getJointMatrices().size());
			skeletonChunkStream->read (&skeleton->getJointParents()[0], sizeof(skeleton->getJointParents()[0])*skeleton->getJointParents().size());
			for (unsigned i = 0; i < skeleton->getNumJoints(); ++i)
			{
				char buffer[256];
				skeletonChunkStream->readCString (buffer);
				skeleton->addAttachPoint (buffer, i);
			}
			_skeleton = skeleton;
		}

		// read animation datas.
		ATOM_ChunkIO::Chunk *jointAnimationDataChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('j','a','d','t'));
		if (jointAnimationDataChunk)
		{
			ATOM_ChunkStream *jointAnimationDataChunkStream = jointAnimationDataChunk->getChunkStream ();
			_animationDataCache = ATOM_NEW(ATOM_JointAnimationDataCache);
			unsigned numFrames = jointAnimationDataChunkStream->readInt ();
			_animationDataCache->setNumFrames (numFrames);
			for (unsigned i = 0; i < numFrames; ++i)
			{
				unsigned numJoints = jointAnimationDataChunkStream->readInt ();
				_animationDataCache->setNumJoints (i, numJoints);
				jointAnimationDataChunkStream->read (_animationDataCache->getJoints (i), sizeof(ATOM_JointTransformInfo)*numJoints);
			}

			ATOM_ChunkIO::Chunk *jointAnimationTrackChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('j','a','t','k'));
			while (jointAnimationTrackChunk)
			{
				ATOM_ChunkStream *jointAnimationTrackChunkStream = jointAnimationTrackChunk->getChunkStream ();
				ATOM_AUTOPTR(ATOM_JointAnimationTrack) track = ATOM_NEW(ATOM_JointAnimationTrack, this);
				track->setAnimationDataCache (_animationDataCache.get());
				int num = jointAnimationTrackChunkStream->readCString (0);
				jointAnimationTrackChunkStream->seek (-num);
				char *s = (char*)ATOM_MALLOC(num);
				jointAnimationTrackChunkStream->readCString (s);
				track->setName (s);
				ATOM_FREE(s);
				int numFrames = jointAnimationTrackChunkStream->readInt ();
				track->setNumFrames (numFrames);
				jointAnimationTrackChunkStream->read (track->getFrameJointIndices(), numFrames*sizeof(unsigned));
				jointAnimationTrackChunkStream->read (track->getFrameTimes(), numFrames*sizeof(unsigned));
				_tracks.push_back (track);

				jointAnimationTrackChunk = jointAnimationTrackChunk->getNextSibling (ATOM_MAKE_FOURCC('j','a','t','k'));
			}
		}

		///////////////////////////////////////////// read sub meshes ///////////////////////////////////////////////////

		int mesh_index = 0;
		ATOM_ChunkIO::Chunk *meshChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','s','m'));
		while (meshChunk)
		{
			//-------- wangjian modified ----------//
			// ���ȶ�ȡ�����ļ�����
			ATOM_ChunkIO::Chunk *materialChunk = meshChunk->getFirstChild (ATOM_MAKE_FOURCC('m','a','t','l'));
			char *buffer = 0;
			if( materialChunk )
			{
				int size = materialChunk->getChunkStream()->readCString (0);
				materialChunk->getChunkStream()->seek (-size);
				buffer = ATOM_NEW_ARRAY(char, size);
				materialChunk->getChunkStream()->readCString (buffer);
			}
			//====================================//


			ATOM_ChunkStream *meshChunkStream = meshChunk->getChunkStream();

			ATOM_SharedMesh *mesh = ATOM_NEW(ATOM_SharedMesh, this);

			// read mesh name
			int numchars = meshChunkStream->readCString (0);
			meshChunkStream->seek (-numchars);
			char *name = (char*)ATOM_MALLOC(numchars);
			meshChunkStream->readCString (name);
			mesh->setName (name);
			ATOM_FREE(name);

			// read mesh joint mapping data
			int numMappings = meshChunkStream->readInt ();
			if (numMappings > 0)
			{
				ATOM_VECTOR<int> jointMap(numMappings);
				meshChunkStream->read (&jointMap[0], numMappings*sizeof(int));
				mesh->setJointMap (jointMap);
			}

			// read vertex streams and index data

			//--- wangjian added ---//
			// ��ǰ���� SKINNING ��ģ�ͷ�Ϊ3�֣�
			// 1. ֻ��MESH��Ϣ û�й�����Ϣ Ҳ û�ж�����Ϣ numTracks = 0
			// 2. û�й�����Ϣ Ҳ û�ж�����Ϣ numTracks > 0
			// 3. ���й�����Ϣ ���ж�����Ϣ 
			bool bNoneSkin			= ( numTracks == 0 ) && (skeletonChunk) && (!jointAnimationDataChunk);
			if( bNoneSkin )
			{
				ATOM_VECTOR<int> jointMap;
				mesh->setJointMap(jointMap);
			}
			bool bSkinniedMesh = !mesh->getJointMap().empty();
			bool bHWInstancingMesh = useHWInstancingMesh(bSkinniedMesh,buffer?buffer:"");
			if( bHWInstancingMesh )
				mesh->setGeometry (ATOM_NEW(ATOM_HWInstancingGeometry));	// �������û�й�����Ϣ��ʹ��HWInstancing Geometry
			else
				mesh->setGeometry (ATOM_NEW(ATOM_MultiStreamGeometry));		// �������Ƥ���� ʹ��MultiStream Geometry
			//----------------------//

			int numStreams = meshChunkStream->readInt ();
			int numVertices = meshChunkStream->readInt ();
			int numIndices = meshChunkStream->readInt ();
			ATOM_BBox bbox;
			meshChunkStream->read (&bbox, sizeof(ATOM_BBox));
			mesh->setBoundingbox (bbox);

			// ������䶥����������
			ATOM_AUTOREF(ATOM_IndexArray) indices = device->allocIndexArray (ATOM_USAGE_STATIC, numIndices, false, true);
			if (!indices)
			{
				ATOM_DELETE(mesh);
				return false;
			}
			void *pIndexData = indices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			if (!pIndexData)
			{
				ATOM_DELETE(mesh);
				return false;
			}
			meshChunkStream->read (pIndexData, sizeof(short)*numIndices);
			indices->unlock ();

			// ���ö�������
			mesh->getGeometry()->setIndices (indices.get());

			// ������䶥�����ݻ���

			// �������Ƥ���� ʹ��MultiStream Geometry
			if( !bHWInstancingMesh )
			{
				for (int i = 0; i < numStreams; ++i)
				{
					unsigned attrib = meshChunkStream->readInt();

					//-----------------------------------//
					// wangjian modified 
					// �жϸ�attrib�Ƿ��ѹ��
					unsigned int flag = ATTRIBUTE_FLAG_NONE;
#if 0
					if( flags & MF_COMRESSION )
					{
						if( ATOM_canUseCompress(attrib) )
							flag |= ATTRIBUTE_FLAG_COMPRESSED;
					}
#else
					// ���ģ���ļ���ѹ����ʽ ���� ����ѹ����ʽ��Ⱦ
					if( ATOM_RenderSettings::isModelCompressedFormatEnabled() || ( flags & MF_COMRESSION ) )
					{
						// �����attrib�ǿ�ѹ�� ����ѹ�����
						if( ATOM_canUseCompress(attrib) )
							flag |= ATTRIBUTE_FLAG_COMPRESSED;	
					}
#endif
					
						
					// �õ���attrib�Ĵ�С
					unsigned vertexSize = ATOM_GetVertexSize (attrib,flag);
					if (!vertexSize)
					{
						ATOM_DELETE(mesh);
						return false;
					}
					//-----------------------------------//

					// ����VB
					unsigned dataSize = numVertices * vertexSize;
					ATOM_AUTOREF(ATOM_VertexArray) vertexArray = device->allocVertexArray (attrib, ATOM_USAGE_STATIC, numVertices, true, flag);
					if (!vertexArray)
					{
						ATOM_DELETE(mesh);
						return false;
					}

					// �������ѹ����ʽ��Ⱦ �Ҹ������ǿ�ѹ�������� �� ģ�Ͱ汾Ϊ��ѹ���汾
					// FLOATת����HALF
					if( ATOM_RenderSettings::isModelCompressedFormatEnabled() && 
						!(flags & MF_COMRESSION) && 
						ATOM_canUseCompress(attrib) )
					{
						unsigned component_count_uncompressd;
						unsigned component_count_compressd = ATOM_getCompressAttribComponent(attrib,component_count_uncompressd);

						// ��ȡԭ��������
						unsigned size = numVertices * component_count_uncompressd * sizeof(float);
						float * float_buffer = ATOM_NEW_ARRAY(float,numVertices * component_count_uncompressd);
						meshChunkStream->read ((void*)float_buffer, size);
						float * float_ptr = float_buffer;

						// 
						FloatConversion::uint16_t * pVertexData = (FloatConversion::uint16_t*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
						if (!pVertexData)
						{
							ATOM_DELETE(mesh);
							return 0;
						}
						FloatConversion::uint16_t * fp16_ptr = pVertexData;
						for ( int i = 0; i < numVertices; ++i )
						{
							*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
							*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));

							if( component_count_uncompressd == 3 )
							{
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
								*(fp16_ptr++) = FloatConversion::FloatToHalf(1.0f);				// ʹ��1 Ҳ������ڲ�ʹ��BUMPMAPPINGʱ����normal��Z������������
								// ��ʹ��BUMPMAPPINGʱ����normal��Z�����������ź�tangent��Z������������
								// ʹ��fmod�ķ�����ȡ������
							}
							else if( component_count_uncompressd == 4 )
							{
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
							}
						}

						vertexArray->unlock ();

						ATOM_DELETE_ARRAY(float_buffer);
					}
					// ֱ�Ӷ�ȡģ�����񶥵����ݵ�VB
					else
					{
						void *pVertexData = vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
						if (!pVertexData)
						{
							ATOM_DELETE(mesh);
							return false;
						}
						meshChunkStream->read (pVertexData, dataSize);
						vertexArray->unlock ();
					}
					
					// ���STREAM
					((ATOM_MultiStreamGeometry*)(mesh->getGeometry()))->addStream (vertexArray.get());
				}
			}
			else
			{
				// ����
				unsigned attrib_total = 0;
				// ���Ա��
				unsigned attrib_total_flag = 0;

				ATOM_VECTOR<unsigned> data_array_vertexsize;
				data_array_vertexsize.resize(numStreams,0);
				ATOM_VECTOR<char *> data_array;
				data_array.resize(numStreams,0);

				for (int i = 0; i < numStreams; ++i)
				{
					// �������
					unsigned attrib = meshChunkStream->readInt();

					unsigned int attrib_flag = ATTRIBUTE_FLAG_NONE;

#if 0
					if( flags & MF_COMRESSION )
					{
						if( ATOM_canUseCompress(attrib) )
							attrib_flag |= ATTRIBUTE_FLAG_COMPRESSED;
					}
#else
					// ���ģ���ļ���ѹ����ʽ ���� ����ѹ����ʽ��Ⱦ
					if( ATOM_RenderSettings::isModelCompressedFormatEnabled() || ( flags & MF_COMRESSION ) )
					{
						// �����attrib�ǿ�ѹ�� ����ѹ�����
						if( ATOM_canUseCompress(attrib) )
							attrib_flag |= ATTRIBUTE_FLAG_COMPRESSED;
					}
#endif

					// ��ö����С
					unsigned vertexSize = ATOM_GetVertexSize (attrib,attrib_flag);
					data_array_vertexsize[i] = vertexSize;

					// �ܵĴ�С
					unsigned dataSize = numVertices * vertexSize;
					data_array[i] = (char*)ATOM_MALLOC(dataSize);

					// �������ѹ����ʽ��Ⱦ �Ҹ������ǿ�ѹ�������� �� ģ�Ͱ汾Ϊ��ѹ���汾
					// FLOATת����HALF
					if( ATOM_RenderSettings::isModelCompressedFormatEnabled() && 
						!(flags & MF_COMRESSION) && 
						ATOM_canUseCompress(attrib) )
					{
						unsigned component_count_uncompressd;
						unsigned component_count_compressd = ATOM_getCompressAttribComponent(attrib,component_count_uncompressd);

						// ��ȡԭ��������
						unsigned size = numVertices * component_count_uncompressd * sizeof(float);
						float * float_buffer = ATOM_NEW_ARRAY(float,numVertices * component_count_uncompressd);
						meshChunkStream->read ((void*)float_buffer, size);
						float * float_ptr = float_buffer;

						// ת��ΪFLOAT16
						FloatConversion::uint16_t * pVertexData = (FloatConversion::uint16_t*)(data_array[i]);
						if (!pVertexData)
						{
							ATOM_DELETE(mesh);
							return 0;
						}
						FloatConversion::uint16_t * fp16_ptr = pVertexData;
						for ( int i = 0; i < numVertices; ++i )
						{
							*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
							*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));

							if( component_count_uncompressd == 3 )
							{
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
								*(fp16_ptr++) = FloatConversion::FloatToHalf(1.0f);				// ʹ��1 Ҳ������ڲ�ʹ��BUMPMAPPINGʱ����normal��Z������������
								// ��ʹ��BUMPMAPPINGʱ����normal��Z�����������ź�tangent��Z������������
								// ʹ��fmod�ķ�����ȡ������
							}
							else if( component_count_uncompressd == 4 )
							{
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
							}
						}

						// ������ʱBUFFER
						ATOM_DELETE_ARRAY(float_buffer);
					}
					// ����ֱ�Ӷ�ȡ��������
					else
					{
						// ��ȡ����ʱ����
						meshChunkStream->read (data_array[i], dataSize);
					}
					
					// ����
					attrib_total |= attrib;

					// ���Ա��
					attrib_total_flag |= attrib_flag;
				}

				// �������㻺��
				ATOM_AUTOREF(ATOM_VertexArray) vertexArray = device->allocVertexArray ( attrib_total, 
																						ATOM_USAGE_STATIC, 
																						numVertices, 
																						true, 
																						attrib_total_flag );
				if (!vertexArray)
				{
					for( int i = 0; i < data_array.size(); ++i )
					{
						if( data_array[i] )
							ATOM_FREE(data_array[i]);
					}
					ATOM_DELETE(mesh);
					return false;
				}

				// �������㻺��
				char *pVertexData = (char*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				if (!pVertexData)
				{
					for( int i = 0; i < data_array.size(); ++i )
					{
						if( data_array[i] )
							ATOM_FREE(data_array[i]);
					}
					ATOM_DELETE(mesh);
					return false;
				}

				// ѭ�����ж���
				int vertexsize  = 0;
				char* dest_data = pVertexData;
				ATOM_VECTOR<char *> data_array_tmp = data_array;
				for( int iVert = 0; iVert < numVertices; ++iVert )
				{
					// ѭ�����е�STREAM
					for (int iStream = 0; iStream < numStreams; ++iStream)
					{
						vertexsize	= data_array_vertexsize[iStream];

						memcpy( dest_data, data_array_tmp[iStream],vertexsize );

						dest_data				+=	vertexsize;
						data_array_tmp[iStream] +=	vertexsize;
					}
				}
				vertexArray->unlock ();

				// ���ö��㻺��
				((ATOM_HWInstancingGeometry*)mesh->getGeometry())->setStream (vertexArray.get());

				for( int i = 0; i < data_array.size(); ++i )
				{
					if( data_array[i] )
						ATOM_FREE(data_array[i]);
				}
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////

			// read sub mesh material
			//ATOM_ChunkIO::Chunk *materialChunk = meshChunk->getFirstChild (ATOM_MAKE_FOURCC('m','a','t','l'));

			ATOM_AUTOPTR(ATOM_Material) material = 0;

			//--------------------------- wangjian modified --------------------------//
			// ѡ����ʵĺ��Ĳ����ļ�
			ATOM_STRING coremat_name = "";
			chooseProperCoreMatName(coremat_name,mesh->getGeometry()->getType()==GT_HWINSTANCING);
			//------------------------------------------------------------------------//

			if (!materialChunk)
			{
				material = ATOM_MaterialManager::createMaterialFromCore (device, coremat_name.c_str());
				mesh->setMaterial (material.get());
			}
			else
			{
				/*int size = materialChunk->getChunkStream()->readCString (0);
				materialChunk->getChunkStream()->seek (-size);
				char *buffer = ATOM_NEW_ARRAY(char, size);
				materialChunk->getChunkStream()->readCString (buffer);*/

				//--------------------------- wangjian modified --------------------------//
				// ѡ����ʵĺ��Ĳ����ļ�
				ATOM_STRING coremat_name = buffer;
				chooseProperCoreMatName(coremat_name,mesh->getGeometry()->getType()==GT_HWINSTANCING);
				//------------------------------------------------------------------------//

				material = ATOM_MaterialManager::createMaterialFromCore (device, coremat_name.c_str());
				mesh->setMaterial (material.get());
				ATOM_DELETE_ARRAY(buffer);

				if (!mesh->getMaterial())
				{
					material = ATOM_MaterialManager::createMaterialFromCore (	device, 
																				"/materials/builtin/model_default.mat"	);
					mesh->setMaterial (material.get());
				}
				else
				{
					ATOM_ParameterTable *paramTable = mesh->getMaterial()->getParameterTable();

					for (;;)
					{
						int size = materialChunk->getChunkStream()->readCString (0);
						materialChunk->getChunkStream()->seek (-size);
						char *buffer = ATOM_NEW_ARRAY(char, size);
						materialChunk->getChunkStream()->readCString (buffer);
						if (*buffer == '\0')
						{
							ATOM_DELETE_ARRAY(buffer);
							break;
						}

						ATOM_ParameterTable::ValueHandle paramValue = paramTable->getValueHandle (buffer);

						ATOM_DELETE_ARRAY(buffer);
						if (!paramValue)
						{
							continue;
						}

						switch (paramValue->handle->getParamType())
						{
						case ATOM_MaterialParam::ParamType_Float:
							{
								paramValue->setFloat (materialChunk->getChunkStream()->readFloat ());
								break;
							}
						case ATOM_MaterialParam::ParamType_FloatArray:
							{
								materialChunk->getChunkStream()->read (paramValue->f, sizeof(float)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Int:
							{
								paramValue->setInt (materialChunk->getChunkStream()->readInt ());
								break;
							}
						case ATOM_MaterialParam::ParamType_IntArray:
							{
								materialChunk->getChunkStream()->read (paramValue->i, sizeof(int)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Vector:
							{
								materialChunk->getChunkStream()->read (paramValue->v, sizeof(ATOM_Vector4f));
								break;
							}
						case ATOM_MaterialParam::ParamType_VectorArray:
							{
								materialChunk->getChunkStream()->read (paramValue->v, sizeof(ATOM_Vector4f)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix44:
							{
								materialChunk->getChunkStream()->read (paramValue->m44, sizeof(ATOM_Matrix4x4f));
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix44Array:
							{
								materialChunk->getChunkStream()->read (paramValue->m44, sizeof(ATOM_Matrix4x4f)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix43:
							{
								materialChunk->getChunkStream()->read (paramValue->m43, sizeof(ATOM_Matrix3x4f));
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix43Array:
							{
								materialChunk->getChunkStream()->read (paramValue->m43, sizeof(ATOM_Matrix3x4f)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Texture:
							{
								int size = materialChunk->getChunkStream()->readCString(0);
								materialChunk->getChunkStream()->seek (-size);
								char *s = (char*)ATOM_MALLOC(size);
								materialChunk->getChunkStream()->readCString (s);

								//--- wangjian added ---//
								// �첽����
								// �����ģ����Դֱ�Ӽ��أ���ͨ���첽���أ�������·�����浽immediate table
								ATOM_AsyncLoader::AddToImmediateTable(s);
								//----------------------//

								paramValue->setTexture(s,ATOM_LoadPriority_IMMEDIATE);

								//--- wangjian added ---//
								// �ͷ�
								ATOM_FREE(s);
								//----------------------//

								/*
								paramValue->textureFileName = s;
								ATOM_AUTOREF(ATOM_Texture) texture = ATOM_CreateTextureResource (paramValue->textureFileName.c_str());
								if (texture)
								{
									paramValue->setTexture (texture.get());
								}
								*/
								break;
							}
						}
					}
				}
			}

			//----- ���ò��ʵ�ȫ��ID ----//
			if( material )
			{
				char strMatName[256] = {0};
				sprintf(strMatName,"%s_%u",filename,mesh_index);
				material->setMaterialId(strMatName);
			}
			//--------------------------//

			_meshes.push_back (mesh);

			meshChunk = meshChunk->getNextSibling (ATOM_MAKE_FOURCC('a','t','s','m'));

			mesh_index++;
		}

		// wangjian commmented it!
#if 0
		if (_meshes.size() != numMeshes)
		{
			return false;
		}
#endif

		ATOM_ChunkIO::Chunk *userAttribChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','t','r'));
		if (userAttribChunk)
		{
			while (userAttribChunk)
			{
				readUserAttributes (_userAttributes, userAttribChunk->getChunkStream());
				userAttribChunk = userAttribChunk->getNextSibling (ATOM_MAKE_FOURCC('a', 't', 't', 'r'));
			}
		}

		//--- wangjian added ---//
		// �첽����
		// ���ֱ�Ӽ��ظ�ģ�ͣ��ڼ���֮��������ɱ��
		getAsyncLoader()->priority = ATOM_LoadPriority_IMMEDIATE;
		getAsyncLoader()->SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED);
		//----------------------//
	}

	return true;
}

unsigned ATOM_SharedModel::getNumMeshes (void) const
{
	return _meshes.size();
}

ATOM_SharedMesh *ATOM_SharedModel::getMesh (unsigned index) const
{
	return _meshes[index];
}

unsigned ATOM_SharedModel::getNumTracks (void) const
{
	return _tracks.size();
}

ATOM_JointAnimationTrack *ATOM_SharedModel::getTrack (unsigned index) const
{
	return _tracks[index].get();
}

const ATOM_BBox &ATOM_SharedModel::getBoundingbox (void) const
{
	return _boundingbox;
}

struct StreamInfo
{
    int attrib;
    ATOM_VertexArray *vertexArray;
};

// wangjian modified 
// ��������ģ��������Ϣ/������Ϣ/������Ϣ��NM2ģ�ͷ���ת��Ϊģ��������ϢNM2/������ϢNM2/����������ϢNM2��
// �ļ�������
// 1. ģ��������ϢNM2�� ��Ȼʹ��ԭ�ļ���
// 2. ������ϢNM2: ʹ��ԭ�ļ�·�� + /bone
// 3. ������ϢNM2: ʹ��ԭ�ļ�·�� + /��������

bool ATOM_SharedModel::saveIsolation (const char *filename)
{
	//=============================================================================================//
	// �������������Ϣ �������
	if (_skeleton)
	{
		ATOM_ChunkIO chunkIO;

		ATOM_ChunkIO::Chunk *mainChunk = chunkIO.getTopChunk()->appendChild ();
		mainChunk->setSig (ATOM_MAKE_FOURCC('a','t','m','e'));							// signiture
		mainChunk->getChunkStream()->writeInt (1);										// version
		mainChunk->getChunkStream()->writeInt (_meshes.size());							// ���������Ϣ ���豣����������

		//---------------------------------------------------//
		// wangjian modified
		// ��Ϊ_skeleton���û��ʹ�õ���������Ϊѹ����ʽ���
#if 0
		mainChunk->getChunkStream()->writeInt (_skeleton?1:0);
#else
		int flags = 0x0;
		int offset_flag = mainChunk->getChunkStream()->position();						// ���¸�λ��
		mainChunk->getChunkStream()->writeInt (flags);
#endif
		//---------------------------------------------------//

		mainChunk->getChunkStream()->writeInt (_tracks.size());							// ���������Ϣ ���豣�涯������
		
		mainChunk->getChunkStream()->write (&_boundingbox, sizeof(ATOM_BBox));			// ���������Ϣ ���豣���Χ��


		ATOM_ChunkIO::Chunk *skeletonChunk = mainChunk->appendChild ();
		skeletonChunk->setSig (ATOM_MAKE_FOURCC('a','t','s','k'));
		ATOM_ChunkStream *skeletonChunkStream = skeletonChunk->getChunkStream();
		skeletonChunkStream->writeInt (_skeleton->getHashCode());
		skeletonChunkStream->writeInt (_skeleton->getNumJoints());
		skeletonChunkStream->write (&_skeleton->getJointMatrices()[0], sizeof(_skeleton->getJointMatrices()[0])*_skeleton->getJointMatrices().size());
		skeletonChunkStream->write (&_skeleton->getJointParents()[0], sizeof(_skeleton->getJointParents()[0])*_skeleton->getJointParents().size());
		for (unsigned i = 0; i < _skeleton->getNumJoints(); ++i)
		{
			skeletonChunkStream->writeCString (_skeleton->getAttachPointName(i));
		}

		//-----------------------------------------------------------------------------//
		// ��������ļ�
		ATOM_STRING bone_filename = filename;
		unsigned pos = bone_filename.find_last_of('/')+1;
		bone_filename = bone_filename.substr(0,pos) + "bone.nm2";
		if( !chunkIO.save ( bone_filename.c_str() ) )
			return false;
		//-----------------------------------------------------------------------------//

	}
	
	//=============================================================================================//
	// ����ж�����Ϣ ���涯��
	if( _animationDataCache && !_tracks.empty() )
	{
		// ѭ��ÿ������
		//unsigned frame_offset = 0;
		for (unsigned i = 0; i < _tracks.size(); ++i)
		{
			ATOM_ChunkIO chunkIO;

			ATOM_ChunkIO::Chunk *mainChunk = chunkIO.getTopChunk()->appendChild ();
			mainChunk->setSig (ATOM_MAKE_FOURCC('a','t','m','e'));					// signiture
			mainChunk->getChunkStream()->writeInt (1);								// version
			mainChunk->getChunkStream()->writeInt (_meshes.size());					// ���涯����Ϣ ���豣����������

			//---------------------------------------------------//
			// wangjian modified
			// ��Ϊ_skeleton���û��ʹ�õ���������Ϊѹ����ʽ���
#if 0
			mainChunk->getChunkStream()->writeInt (_skeleton?1:0);
#else
			int flags = 0x0;
			int offset_flag = mainChunk->getChunkStream()->position();				// ���¸�λ��
			mainChunk->getChunkStream()->writeInt (flags);							// flag
#endif
			//---------------------------------------------------//

			mainChunk->getChunkStream()->writeInt (_tracks.size());					// �����ܵĶ�������

			mainChunk->getChunkStream()->write (&_boundingbox, sizeof(ATOM_BBox));	// ���涯����Ϣ ���豣���Χ����Ϣ

			// ���涯������
			
			// ����֡����
			ATOM_ChunkIO::Chunk *jointAnimationDataCacheChunk = mainChunk->appendChild ();
			jointAnimationDataCacheChunk->setSig (ATOM_MAKE_FOURCC('j','a','d','t'));
			ATOM_ChunkStream *cacheChunkStream = jointAnimationDataCacheChunk->getChunkStream();
			cacheChunkStream->writeInt ( _tracks[i]->getNumFrames() );

			// �������Ǵ�0��ʼ��
			unsigned * indices = _tracks[i]->getFrameJointIndices();
			for (unsigned f = 0; f < _tracks[i]->getNumFrames(); ++f)
			{
				unsigned numJoints = _animationDataCache->getNumJoints (indices[f]/*f + frame_offset*/);
				cacheChunkStream->writeInt (numJoints);
				cacheChunkStream->write (_animationDataCache->getJoints (indices[f]/*f + frame_offset*/), numJoints * sizeof(ATOM_JointTransformInfo) );
			}

			// �ö����Ķ���֡
			ATOM_ChunkIO::Chunk *trackChunk = mainChunk->appendChild ();
			trackChunk->setSig (ATOM_MAKE_FOURCC('j','a','t','k'));
			ATOM_ChunkStream *trackChunkStream = trackChunk->getChunkStream();
			trackChunkStream->writeCString (_tracks[i]->getName());
			trackChunkStream->writeInt (_tracks[i]->getNumFrames());

			
			for( int f = 0; f < _tracks[i]->getNumFrames(); f++ )
				indices[f] = f;
			trackChunkStream->write ( indices, _tracks[i]->getNumFrames() * sizeof(unsigned) );
			trackChunkStream->write ( _tracks[i]->getFrameTimes(), _tracks[i]->getNumFrames() * sizeof(unsigned) );

			//-----------------------------------------------------------------------------//
			// ���浥���Ķ����ļ�
			ATOM_STRING anim_filename = filename;
			unsigned pos = anim_filename.find_last_of('/')+1;
			anim_filename = anim_filename.substr(0,pos) + _tracks[i]->getName() + ".nm2";
			if( !chunkIO.save ( anim_filename.c_str() ) )
				return false;
			//-----------------------------------------------------------------------------//

			// ������һ������
			//frame_offset += _tracks[i]->getNumFrames();
		}
	}

	// ���񶥵���Ϣ
	if( !_meshes.empty() )
	{
		ATOM_ChunkIO chunkIO;

		ATOM_ChunkIO::Chunk *mainChunk = chunkIO.getTopChunk()->appendChild ();
		mainChunk->setSig (ATOM_MAKE_FOURCC('a','t','m','e'));						// signiture
		mainChunk->getChunkStream()->writeInt (1);									// version
		mainChunk->getChunkStream()->writeInt (_meshes.size());						// mesh count

		//---------------------------------------------------//
		// wangjian modified
		// ��Ϊ_skeleton���û��ʹ�õ���������Ϊѹ����ʽ���
#if 0
		mainChunk->getChunkStream()->writeInt (_skeleton?1:0);
#else
		int flags = 0x0;
		int offset_flag = mainChunk->getChunkStream()->position();					// ���¸�λ��
		mainChunk->getChunkStream()->writeInt (flags);
#endif
		//---------------------------------------------------//

		mainChunk->getChunkStream()->writeInt (_tracks.size());						// �����ܵĶ�������
		mainChunk->getChunkStream()->write (&_boundingbox, sizeof(ATOM_BBox));		// �����Χ����Ϣ

		// ѭ��ÿ������
		for ( unsigned i = 0; i < _meshes.size(); ++i )
		{
			// wangjian added 
			// ��������Ĳ��������Ƿ���model_bumpmap.mat������ǣ�����Ҫ����������Ϣ
			// ���򲻱���
			bool bNeedSaveTangents = false;
			ATOM_Material *material = _meshes[i]->getMaterial();
			ATOM_CoreMaterial *coreMaterial = material ? material->getCoreMaterial() : 0;
			if (coreMaterial)
			{
				if( isBumpMapMaterial( coreMaterial->getObjectName() ) )
					bNeedSaveTangents = true;
			}
			////////////////////////////////////////////////////////////////////////////////////////


			ATOM_VECTOR<StreamInfo> streamInfos;
			unsigned numVertices, numIndices;
			ATOM_MultiStreamGeometry *geo = (ATOM_MultiStreamGeometry*)_meshes[i]->getGeometry();

			int attribs[] = {
				ATOM_VERTEX_ATTRIB_NORMAL,
				ATOM_VERTEX_ATTRIB_PSIZE,
				ATOM_VERTEX_ATTRIB_PRIMARY_COLOR,
				ATOM_VERTEX_ATTRIB_TANGENT,
				ATOM_VERTEX_ATTRIB_BINORMAL,
				ATOM_VERTEX_ATTRIB_TEX_MASK(0),
				ATOM_VERTEX_ATTRIB_TEX_MASK(1),
				ATOM_VERTEX_ATTRIB_TEX_MASK(2),
				ATOM_VERTEX_ATTRIB_TEX_MASK(3),
				ATOM_VERTEX_ATTRIB_TEX_MASK(4),
				ATOM_VERTEX_ATTRIB_TEX_MASK(5),
				ATOM_VERTEX_ATTRIB_TEX_MASK(6),
				ATOM_VERTEX_ATTRIB_TEX_MASK(7)
			};

			ATOM_IndexArray *indices = geo->getIndices ();
			ATOM_ASSERT(indices);
			numIndices = indices->getNumIndices ();

			ATOM_VertexArray *vertices = geo->getStream (ATOM_VERTEX_ATTRIB_COORD);
			ATOM_ASSERT(vertices);
			numVertices = vertices->getNumVertices ();
			streamInfos.resize (streamInfos.size()+1);
			streamInfos.back().attrib = ATOM_VERTEX_ATTRIB_COORD;
			streamInfos.back().vertexArray = vertices;

			for (unsigned n = 0; n < sizeof(attribs)/sizeof(attribs[0]); ++n)
			{
				ATOM_VertexArray *stream = geo->getStream (attribs[n]);

				if (stream)
				{
					// ����Ƿ���Ҫ����������Ϣ 
					// ����MESH��������Ϣ ���� �����Ƿ�BUMP�� ������
					if( ( stream->getAttributes() & ( ATOM_VERTEX_ATTRIB_TANGENT | ATOM_VERTEX_ATTRIB_BINORMAL ) ) && 
						!bNeedSaveTangents )
						continue;

					streamInfos.resize (streamInfos.size()+1);
					streamInfos.back().attrib = stream->getAttributes();
					streamInfos.back().vertexArray = stream;
				}
			}

			ATOM_ChunkIO::Chunk *meshChunk = mainChunk->appendChild ();
			meshChunk->setSig (ATOM_MAKE_FOURCC('a','t','s','m'));
			ATOM_ChunkStream *meshChunkStream = meshChunk->getChunkStream();

			meshChunkStream->writeCString (_meshes[i]->getName());

			meshChunkStream->writeInt (_meshes[i]->getJointMap().size());
			if (!_meshes[i]->getJointMap().empty())
			{
				meshChunkStream->write (&_meshes[i]->getJointMap()[0], _meshes[i]->getJointMap().size()*sizeof(int));
			}

			meshChunkStream->writeInt (streamInfos.size());
			meshChunkStream->writeInt (numVertices);
			meshChunkStream->writeInt (numIndices);
			meshChunkStream->write (&_meshes[i]->getBoundingbox(), sizeof(ATOM_BBox));

			void *pIndexData = indices->lock (ATOM_LOCK_READWRITE, 0, 0, false);
			if (!pIndexData)
			{
				return false;
			}
			meshChunkStream->write (pIndexData, sizeof(short)*numIndices);
			indices->unlock ();

			for (unsigned n = 0; n < streamInfos.size(); ++n)
			{

				ATOM_VertexArray *vertexArray = streamInfos[n].vertexArray;
				unsigned attrib = vertexArray->getAttributes();
				meshChunkStream->writeInt (attrib);

				//------------------------------------------------------------------------------------//
#if 1
				// wangjian added : ���ʹ��ѹ���汾
				// ����ѹ�����
				bool bCompressed = false;
				if( vertexArray->getAttribFlags() & ATTRIBUTE_FLAG_COMPRESSED )
				{
					memset( (char*)mainChunk->getChunkStream()->getBuffer() + offset_flag, MF_COMRESSION, sizeof(int) );
					bCompressed = true;
				}
				unsigned dataSize = numVertices * ATOM_GetVertexSize(attrib,bCompressed ? ATTRIBUTE_FLAG_COMPRESSED : 0 );
#else
				unsigned dataSize = numVertices * ATOM_GetVertexSize(attrib);
#endif
				//------------------------------------------------------------------------------------//

				void *pVertexData = vertexArray->lock (ATOM_LOCK_READONLY, 0, 0, false);
				if (!pVertexData)
				{
					return false;
				}
				meshChunkStream->write (pVertexData, dataSize);
				vertexArray->unlock ();
			}

			material = _meshes[i]->getMaterial();
			coreMaterial = material ? material->getCoreMaterial() : 0;
			if (coreMaterial)
			{
				ATOM_ChunkIO::Chunk *materialChunk = meshChunk->appendChild ();
				materialChunk->setSig (ATOM_MAKE_FOURCC('m','a','t','l'));
				materialChunk->getChunkStream()->writeCString (coreMaterial->getObjectName());
				ATOM_ParameterTable *paramTable = material->getParameterTable ();
				for (int i = 0; i < paramTable->getNumParameters(); ++i)
				{
					ATOM_ParameterTable::ValueHandle value = paramTable->getParameter (i);
					ATOM_MaterialParam::EditorType editorType = value->handle->getEditorType();
					if (editorType != ATOM_MaterialParam::ParamEditorType_Disable)
					{
						materialChunk->getChunkStream()->writeCString (value->getParameterName());

						switch (value->handle->getParamType ())
						{
						case ATOM_MaterialParam::ParamType_Float:
							materialChunk->getChunkStream()->writeFloat (*value->f);
							break;
						case ATOM_MaterialParam::ParamType_FloatArray:
							materialChunk->getChunkStream()->write (value->f, sizeof(float)*value->handle->getDimension());
							break;
						case ATOM_MaterialParam::ParamType_Int:
							materialChunk->getChunkStream()->writeInt (*value->i);
							break;
						case ATOM_MaterialParam::ParamType_IntArray:
							materialChunk->getChunkStream()->write (value->i, sizeof(int)*value->handle->getDimension());
							break;
						case ATOM_MaterialParam::ParamType_Vector:
							materialChunk->getChunkStream()->write (value->v, sizeof(ATOM_Vector4f));
							break;
						case ATOM_MaterialParam::ParamType_VectorArray:
							materialChunk->getChunkStream()->write (value->v, sizeof(ATOM_Vector4f)*value->handle->getDimension());
							break;
						case ATOM_MaterialParam::ParamType_Matrix44:
							materialChunk->getChunkStream()->write (value->m44, sizeof(ATOM_Matrix4x4f));
							break;
						case ATOM_MaterialParam::ParamType_Matrix44Array:
							materialChunk->getChunkStream()->write (value->m44, sizeof(ATOM_Matrix4x4f)*value->handle->getDimension());
							break;
						case ATOM_MaterialParam::ParamType_Matrix43:
							materialChunk->getChunkStream()->write (value->m43, sizeof(ATOM_Matrix3x4f));
							break;
						case ATOM_MaterialParam::ParamType_Matrix43Array:
							materialChunk->getChunkStream()->write (value->m43, sizeof(ATOM_Matrix3x4f)*value->handle->getDimension());
							break;
						case ATOM_MaterialParam::ParamType_Texture:
							materialChunk->getChunkStream()->writeCString (value->textureFileName.c_str());
							break;
						default:
							break;
						}
					}
				}

				materialChunk->getChunkStream()->writeCString (""); // end of parameters
			}
		}

		if (_userAttributes->getNumAttributes() > 0)
		{
			ATOM_ChunkIO::Chunk *userAttribChunk = mainChunk->appendChild ();
			userAttribChunk->setSig (ATOM_MAKE_FOURCC('a', 't', 't', 'r'));
			saveUserAttributes (_userAttributes, userAttribChunk->getChunkStream ());
		}

		// ����������Ϣ
		if( !chunkIO.save (filename) )
			return false;
	}
	
	return true;
}

bool ATOM_SharedModel::save (const char *filename)
{
	ATOM_ChunkIO chunkIO;
	ATOM_ChunkIO::Chunk *mainChunk = chunkIO.getTopChunk()->appendChild ();
	mainChunk->setSig (ATOM_MAKE_FOURCC('a','t','m','e'));
	mainChunk->getChunkStream()->writeInt (1);						// version
	mainChunk->getChunkStream()->writeInt (_meshes.size());			// mesh count

	//---------------------------------------------------//
	// wangjian modified
	// ��Ϊ_skeleton���û��ʹ�õ���������Ϊѹ����ʽ���
#if 0
	mainChunk->getChunkStream()->writeInt (_skeleton?1:0);
#else
	int flags = 0x0;
	int offset_flag = mainChunk->getChunkStream()->position();			// ���¸�λ��
	mainChunk->getChunkStream()->writeInt (flags);
#endif
	//---------------------------------------------------//

	mainChunk->getChunkStream()->writeInt (_tracks.size());
	mainChunk->getChunkStream()->write (&_boundingbox, sizeof(ATOM_BBox));

	if (_skeleton)
	{
		ATOM_ChunkIO::Chunk *skeletonChunk = mainChunk->appendChild ();
		skeletonChunk->setSig (ATOM_MAKE_FOURCC('a','t','s','k'));
		ATOM_ChunkStream *skeletonChunkStream = skeletonChunk->getChunkStream();
		skeletonChunkStream->writeInt (_skeleton->getHashCode());
		skeletonChunkStream->writeInt (_skeleton->getNumJoints());
		skeletonChunkStream->write (&_skeleton->getJointMatrices()[0], sizeof(_skeleton->getJointMatrices()[0])*_skeleton->getJointMatrices().size());
		skeletonChunkStream->write (&_skeleton->getJointParents()[0], sizeof(_skeleton->getJointParents()[0])*_skeleton->getJointParents().size());
		for (unsigned i = 0; i < _skeleton->getNumJoints(); ++i)
		{
			skeletonChunkStream->writeCString (_skeleton->getAttachPointName(i));
		}
	}

	if (_animationDataCache)
	{
		ATOM_ChunkIO::Chunk *jointAnimationDataCacheChunk = mainChunk->appendChild ();
		jointAnimationDataCacheChunk->setSig (ATOM_MAKE_FOURCC('j','a','d','t'));
		ATOM_ChunkStream *cacheChunkStream = jointAnimationDataCacheChunk->getChunkStream();
		cacheChunkStream->writeInt (_animationDataCache->getNumFrames ());
		for (unsigned i = 0; i < _animationDataCache->getNumFrames(); ++i)
		{
			unsigned numJoints = _animationDataCache->getNumJoints (i);
			cacheChunkStream->writeInt (numJoints);
			cacheChunkStream->write (_animationDataCache->getJoints (i), numJoints*sizeof(ATOM_JointTransformInfo));
		}
	}

	for (unsigned i = 0; i < _tracks.size(); ++i)
	{
		ATOM_ChunkIO::Chunk *trackChunk = mainChunk->appendChild ();
		trackChunk->setSig (ATOM_MAKE_FOURCC('j','a','t','k'));
		ATOM_ChunkStream *trackChunkStream = trackChunk->getChunkStream();
		trackChunkStream->writeCString (_tracks[i]->getName());
		trackChunkStream->writeInt (_tracks[i]->getNumFrames());
		trackChunkStream->write (_tracks[i]->getFrameJointIndices(), _tracks[i]->getNumFrames()*sizeof(unsigned));
		trackChunkStream->write (_tracks[i]->getFrameTimes(), _tracks[i]->getNumFrames()*sizeof(unsigned));
	}

	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		// wangjian added 
		// ��������Ĳ��������Ƿ���model_bumpmap.mat������ǣ�����Ҫ����������Ϣ
		// ���򲻱���
		bool bNeedSaveTangents = false;
		ATOM_Material *material = _meshes[i]->getMaterial();
		ATOM_CoreMaterial *coreMaterial = material ? material->getCoreMaterial() : 0;
		if (coreMaterial)
		{
			if( isBumpMapMaterial( coreMaterial->getObjectName() ) )
				bNeedSaveTangents = true;
		}
		////////////////////////////////////////////////////////////////////////////////////////


		ATOM_VECTOR<StreamInfo> streamInfos;
		unsigned numVertices, numIndices;
		ATOM_MultiStreamGeometry *geo = (ATOM_MultiStreamGeometry*)_meshes[i]->getGeometry();

		int attribs[] = {
			ATOM_VERTEX_ATTRIB_NORMAL,
			ATOM_VERTEX_ATTRIB_PSIZE,
			ATOM_VERTEX_ATTRIB_PRIMARY_COLOR,
			ATOM_VERTEX_ATTRIB_TANGENT,
			ATOM_VERTEX_ATTRIB_BINORMAL,
			ATOM_VERTEX_ATTRIB_TEX_MASK(0),
			ATOM_VERTEX_ATTRIB_TEX_MASK(1),
			ATOM_VERTEX_ATTRIB_TEX_MASK(2),
			ATOM_VERTEX_ATTRIB_TEX_MASK(3),
			ATOM_VERTEX_ATTRIB_TEX_MASK(4),
			ATOM_VERTEX_ATTRIB_TEX_MASK(5),
			ATOM_VERTEX_ATTRIB_TEX_MASK(6),
			ATOM_VERTEX_ATTRIB_TEX_MASK(7)
		};

		ATOM_IndexArray *indices = geo->getIndices ();
		ATOM_ASSERT(indices);
		numIndices = indices->getNumIndices ();

		ATOM_VertexArray *vertices = geo->getStream (ATOM_VERTEX_ATTRIB_COORD);
		ATOM_ASSERT(vertices);
		numVertices = vertices->getNumVertices ();
		streamInfos.resize (streamInfos.size()+1);
		streamInfos.back().attrib = ATOM_VERTEX_ATTRIB_COORD;
		streamInfos.back().vertexArray = vertices;

		for (unsigned n = 0; n < sizeof(attribs)/sizeof(attribs[0]); ++n)
		{
			ATOM_VertexArray *stream = geo->getStream (attribs[n]);

			if (stream)
			{
				// ����Ƿ���Ҫ����������Ϣ 
				// ����MESH��������Ϣ ���� �����Ƿ�BUMP�� ������
				if( ( stream->getAttributes() & ( ATOM_VERTEX_ATTRIB_TANGENT | ATOM_VERTEX_ATTRIB_BINORMAL ) ) && 
					!bNeedSaveTangents )
					continue;

				streamInfos.resize (streamInfos.size()+1);
				streamInfos.back().attrib = stream->getAttributes();
				streamInfos.back().vertexArray = stream;
			}
		}

		ATOM_ChunkIO::Chunk *meshChunk = mainChunk->appendChild ();
		meshChunk->setSig (ATOM_MAKE_FOURCC('a','t','s','m'));
		ATOM_ChunkStream *meshChunkStream = meshChunk->getChunkStream();

		meshChunkStream->writeCString (_meshes[i]->getName());

		meshChunkStream->writeInt (_meshes[i]->getJointMap().size());
		if (!_meshes[i]->getJointMap().empty())
		{
			meshChunkStream->write (&_meshes[i]->getJointMap()[0], _meshes[i]->getJointMap().size()*sizeof(int));
		}

		meshChunkStream->writeInt (streamInfos.size());
		meshChunkStream->writeInt (numVertices);
		meshChunkStream->writeInt (numIndices);
		meshChunkStream->write (&_meshes[i]->getBoundingbox(), sizeof(ATOM_BBox));

		void *pIndexData = indices->lock (ATOM_LOCK_READWRITE, 0, 0, false);
		if (!pIndexData)
		{
			return false;
		}
		meshChunkStream->write (pIndexData, sizeof(short)*numIndices);
		indices->unlock ();

		for (unsigned n = 0; n < streamInfos.size(); ++n)
		{

			ATOM_VertexArray *vertexArray = streamInfos[n].vertexArray;
			unsigned attrib = vertexArray->getAttributes();
			meshChunkStream->writeInt (attrib);

			//------------------------------------------------------------------------------------//
#if 1
			// wangjian added : ���ʹ��ѹ���汾
			// ����ѹ�����
			bool bCompressed = false;
			if( vertexArray->getAttribFlags() & ATTRIBUTE_FLAG_COMPRESSED )
			{
				memset( (char*)mainChunk->getChunkStream()->getBuffer() + offset_flag, MF_COMRESSION, sizeof(int) );
				bCompressed = true;
			}
			unsigned dataSize = numVertices * ATOM_GetVertexSize(attrib,bCompressed ? ATTRIBUTE_FLAG_COMPRESSED : 0 );
#else
			unsigned dataSize = numVertices * ATOM_GetVertexSize(attrib);
#endif
			//------------------------------------------------------------------------------------//

			void *pVertexData = vertexArray->lock (ATOM_LOCK_READONLY, 0, 0, false);
			if (!pVertexData)
			{
				return false;
			}
			meshChunkStream->write (pVertexData, dataSize);
			vertexArray->unlock ();
		}

		material = _meshes[i]->getMaterial();
		coreMaterial = material ? material->getCoreMaterial() : 0;
		if (coreMaterial)
		{
			ATOM_ChunkIO::Chunk *materialChunk = meshChunk->appendChild ();
			materialChunk->setSig (ATOM_MAKE_FOURCC('m','a','t','l'));
			materialChunk->getChunkStream()->writeCString (coreMaterial->getObjectName());
			ATOM_ParameterTable *paramTable = material->getParameterTable ();
			for (int i = 0; i < paramTable->getNumParameters(); ++i)
			{
				ATOM_ParameterTable::ValueHandle value = paramTable->getParameter (i);
				ATOM_MaterialParam::EditorType editorType = value->handle->getEditorType();
				if (editorType != ATOM_MaterialParam::ParamEditorType_Disable)
				{
					materialChunk->getChunkStream()->writeCString (value->getParameterName());

					switch (value->handle->getParamType ())
					{
					case ATOM_MaterialParam::ParamType_Float:
						materialChunk->getChunkStream()->writeFloat (*value->f);
						break;
					case ATOM_MaterialParam::ParamType_FloatArray:
						materialChunk->getChunkStream()->write (value->f, sizeof(float)*value->handle->getDimension());
						break;
					case ATOM_MaterialParam::ParamType_Int:
						materialChunk->getChunkStream()->writeInt (*value->i);
						break;
					case ATOM_MaterialParam::ParamType_IntArray:
						materialChunk->getChunkStream()->write (value->i, sizeof(int)*value->handle->getDimension());
						break;
					case ATOM_MaterialParam::ParamType_Vector:
						materialChunk->getChunkStream()->write (value->v, sizeof(ATOM_Vector4f));
						break;
					case ATOM_MaterialParam::ParamType_VectorArray:
						materialChunk->getChunkStream()->write (value->v, sizeof(ATOM_Vector4f)*value->handle->getDimension());
						break;
					case ATOM_MaterialParam::ParamType_Matrix44:
						materialChunk->getChunkStream()->write (value->m44, sizeof(ATOM_Matrix4x4f));
						break;
					case ATOM_MaterialParam::ParamType_Matrix44Array:
						materialChunk->getChunkStream()->write (value->m44, sizeof(ATOM_Matrix4x4f)*value->handle->getDimension());
						break;
					case ATOM_MaterialParam::ParamType_Matrix43:
						materialChunk->getChunkStream()->write (value->m43, sizeof(ATOM_Matrix3x4f));
						break;
					case ATOM_MaterialParam::ParamType_Matrix43Array:
						materialChunk->getChunkStream()->write (value->m43, sizeof(ATOM_Matrix3x4f)*value->handle->getDimension());
						break;
					case ATOM_MaterialParam::ParamType_Texture:
						materialChunk->getChunkStream()->writeCString (value->textureFileName.c_str());
						break;
					default:
						break;
					}
				}
			}

			materialChunk->getChunkStream()->writeCString (""); // end of parameters
		}
	}

	if (_userAttributes->getNumAttributes() > 0)
	{
		ATOM_ChunkIO::Chunk *userAttribChunk = mainChunk->appendChild ();
		userAttribChunk->setSig (ATOM_MAKE_FOURCC('a', 't', 't', 'r'));
		saveUserAttributes (_userAttributes, userAttribChunk->getChunkStream ());
	}

	return chunkIO.save (filename);
}

///////////////////////////////////////////////////////////////////////////////
// wangjian added for test float16
// �ڽ�ԭ��ʽģ��ת����ѹ����ʽģ��ʱ����
// ѹ����ʽ�ݶ���
// position :	ʹ��D3DDECLTYPE_FLOAT16_4���棬���һ��W���ڱ����ڲ�ʹ��BUMPMAPPINGʱ����normal��Z������������
//				��ʹ��BUMPMAPPINGʱ����normal��Z�����������ź�tangent��Z�����������š�
//				ʹ��fmod�ķ�����ȡ�����š�
//				����Z����Ϊ-����Ϊ0������Z����Ϊ-����Ϊ0�����������Ϊ������Ϊ��0+0=0��.
//				����Z����Ϊ+����Ϊ1������Z����Ϊ+����Ϊ2�����������Ϊ������Ϊ��1+2=3��.
// texcoord :	ʹ��D3DDECLTYPE_FLOAT16_2���棨2���������ĸ���ʹ��D3DDECLTYPE_FLOAT16_4����
// normal :		ʹ��D3DDECLTYPE_SHORT2N���棨����D3DDECLTYPE_FLOAT16_2����2��SHORT�ֱ𱣴淨�ߵ�X/Y������Z������shader��ʹ��sqrt(1-x*x-y*y) * sign�õ���
// tangent :	����BUMPMAPPINGʱ��ʹ��D3DDECLTYPE_SHORT2N���棬2��SHORT�ֱ𱣴淨�ߵ�X/Y������Z������shader��ʹ��sqrt(1-x*x-y*y) * sign�õ���
// binormal :	��Ҫ�ˣ�ֱ����shader��ͨ�� N X T �õ���
// boneweights : ʹ��D3DDECLTYPE_FLOAT16_4���档
// boneindices : ʹ��D3DDECLTYPE_FLOAT16_4���档
///////////////////////////////////////////////////////////////////////////////
#if 1

// ----- ���԰汾 -------//
#if 0

bool ATOM_SharedModel::load_half (ATOM_RenderDevice *device, const char *filename, int loadPriority)
{
	unload ();

	//--- wangjian modified ---//
	// �첽����
	bool bMt = ( ATOM_AsyncLoader::IsRun() ) && ( loadPriority != ATOM_LoadPriority_IMMEDIATE );

	// ������첽����
	if( ATOM_AsyncLoader::IsRun() && bMt )
	{
		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::log("ATOM_SharedModel::load --- async load model : %s | priority : %d\n", filename, loadPriority );

		getAsyncLoader()->SetModel(this);
		if( !getAsyncLoader()->Start(filename,loadPriority) )
			return false;
	}
	// ֱ�Ӽ���
	else
	{
		if( ATOM_AsyncLoader::isEnableLog() )
		{
			ATOM_LOGGER::log("ATOM_SharedModel::load --- sync load model : %s \n", filename );
		}

		const char* ext = strrchr(filename, '.');
		if (ext && !stricmp(ext, ".obj")) {
			return load_obj(device, filename, bMt);
		}

		unsigned sig;

		{
			ATOM_AutoFile f (filename, ATOM_VFS::read);
			if (!f)
			{
				return false;
			}
			if (f->size() < sizeof(unsigned))
			{
				return false;
			}
			f->read (&sig, sizeof(unsigned));
		}

		// �ϰ汾��ģ�Ͷ�ȡ
		if (sig == 0x4e334d46)
		{
			//--- wangjian modified ---//
			if( load_nm (device, filename, bMt) )
			{
				//// ���سɹ� ������ɱ��
				//getAsyncLoader()->SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED);

				return true;
			}
			return false;
			//----------------------//
		}

		// �°汾��ģ�Ͷ�ȡ

		ATOM_ChunkIO chunkIO;
		if (!chunkIO.load (filename))
		{
			return false;
		}

		ATOM_ChunkIO::Chunk *topChunk = chunkIO.getTopChunk ();
		ATOM_ChunkIO::Chunk *mainChunk = topChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','m','e'));
		if (!mainChunk)
		{
			return false;
		}
		ATOM_ChunkStream *mainChunkStream = mainChunk->getChunkStream();
		int version = mainChunkStream->readInt ();
		int numMeshes = mainChunkStream->readInt ();
		int numSkeletons = mainChunkStream->readInt ();
		int numTracks = mainChunkStream->readInt ();
		mainChunkStream->read (&_boundingbox, sizeof(ATOM_BBox));

		// read skeleton
		ATOM_ChunkIO::Chunk *skeletonChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','s','k'));
		if (skeletonChunk)
		{
			ATOM_ChunkStream *skeletonChunkStream = skeletonChunk->getChunkStream();
			ATOM_AUTOPTR(ATOM_Skeleton) skeleton = ATOM_NEW(ATOM_Skeleton);
			skeleton->setHashCode (skeletonChunkStream->readInt ());
			skeleton->setNumJoints (skeletonChunkStream->readInt ());
			skeletonChunkStream->read (&skeleton->getJointMatrices()[0], sizeof(skeleton->getJointMatrices()[0])*skeleton->getJointMatrices().size());
			skeletonChunkStream->read (&skeleton->getJointParents()[0], sizeof(skeleton->getJointParents()[0])*skeleton->getJointParents().size());
			for (unsigned i = 0; i < skeleton->getNumJoints(); ++i)
			{
				char buffer[256];
				skeletonChunkStream->readCString (buffer);
				skeleton->addAttachPoint (buffer, i);
			}
			_skeleton = skeleton;
		}

		// read animation datas.
		ATOM_ChunkIO::Chunk *jointAnimationDataChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('j','a','d','t'));
		if (jointAnimationDataChunk)
		{
			ATOM_ChunkStream *jointAnimationDataChunkStream = jointAnimationDataChunk->getChunkStream ();
			_animationDataCache = ATOM_NEW(ATOM_JointAnimationDataCache);
			unsigned numFrames = jointAnimationDataChunkStream->readInt ();
			_animationDataCache->setNumFrames (numFrames);
			for (unsigned i = 0; i < numFrames; ++i)
			{
				unsigned numJoints = jointAnimationDataChunkStream->readInt ();
				_animationDataCache->setNumJoints (i, numJoints);
				jointAnimationDataChunkStream->read (_animationDataCache->getJoints (i), sizeof(ATOM_JointTransformInfo)*numJoints);
			}

			ATOM_ChunkIO::Chunk *jointAnimationTrackChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('j','a','t','k'));
			while (jointAnimationTrackChunk)
			{
				ATOM_ChunkStream *jointAnimationTrackChunkStream = jointAnimationTrackChunk->getChunkStream ();
				ATOM_AUTOPTR(ATOM_JointAnimationTrack) track = ATOM_NEW(ATOM_JointAnimationTrack, this);
				track->setAnimationDataCache (_animationDataCache.get());
				int num = jointAnimationTrackChunkStream->readCString (0);
				jointAnimationTrackChunkStream->seek (-num);
				char *s = (char*)ATOM_MALLOC(num);
				jointAnimationTrackChunkStream->readCString (s);
				track->setName (s);
				ATOM_FREE(s);
				int numFrames = jointAnimationTrackChunkStream->readInt ();
				track->setNumFrames (numFrames);
				jointAnimationTrackChunkStream->read (track->getFrameJointIndices(), numFrames*sizeof(unsigned));
				jointAnimationTrackChunkStream->read (track->getFrameTimes(), numFrames*sizeof(unsigned));
				_tracks.push_back (track);

				jointAnimationTrackChunk = jointAnimationTrackChunk->getNextSibling (ATOM_MAKE_FOURCC('j','a','t','k'));
			}
		}

		///////////////////////////////////////////// read sub meshes ///////////////////////////////////////////////////

		int mesh_index = 0;
		ATOM_ChunkIO::Chunk *meshChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','s','m'));
		while (meshChunk)
		{
			//-------- wangjian modified ----------//
			// ���ȶ�ȡ�����ļ�����
			ATOM_ChunkIO::Chunk *materialChunk = meshChunk->getFirstChild (ATOM_MAKE_FOURCC('m','a','t','l'));
			char *buffer = 0;
			if( materialChunk )
			{
				int size = materialChunk->getChunkStream()->readCString (0);
				materialChunk->getChunkStream()->seek (-size);
				buffer = ATOM_NEW_ARRAY(char, size);
				materialChunk->getChunkStream()->readCString (buffer);
			}
			//====================================//


			ATOM_ChunkStream *meshChunkStream = meshChunk->getChunkStream();

			ATOM_SharedMesh *mesh = ATOM_NEW(ATOM_SharedMesh, this);

			// read mesh name
			int numchars = meshChunkStream->readCString (0);
			meshChunkStream->seek (-numchars);
			char *name = (char*)ATOM_MALLOC(numchars);
			meshChunkStream->readCString (name);
			mesh->setName (name);
			ATOM_FREE(name);

			// read mesh joint mapping data
			int numMappings = meshChunkStream->readInt ();
			if (numMappings > 0)
			{
				ATOM_VECTOR<int> jointMap(numMappings);
				meshChunkStream->read (&jointMap[0], numMappings*sizeof(int));
				mesh->setJointMap (jointMap);
			}

			// read vertex streams and index data

			//--- wangjian added ---//
			// ��ǰ���� SKINNING ��ģ�ͷ�Ϊ3�֣�
			// 1. ֻ��MESH��Ϣ û�й�����Ϣ Ҳ û�ж�����Ϣ numTracks = 0
			// 2. û�й�����Ϣ Ҳ û�ж�����Ϣ numTracks > 0
			// 3. ���й�����Ϣ ���ж�����Ϣ 
			bool bNoneSkin			= ( numTracks == 0 ) && (skeletonChunk) && (!jointAnimationDataChunk);
			if( bNoneSkin )
			{
				ATOM_VECTOR<int> jointMap;
				mesh->setJointMap(jointMap);
			}
			bool bSkinniedMesh = !mesh->getJointMap().empty();
			bool bHWInstancingMesh = useHWInstancingMesh(bSkinniedMesh,buffer?buffer:"");
			if( bHWInstancingMesh )
				mesh->setGeometry (ATOM_NEW(ATOM_HWInstancingGeometry));	// �������û�й�����Ϣ��ʹ��HWInstancing Geometry
			else
				mesh->setGeometry (ATOM_NEW(ATOM_MultiStreamGeometry));		// �������Ƥ���� ʹ��MultiStream Geometry

			// ��mesh�Ƿ�����Ҫbumpmapping�Ĳ���
			bool bBumpMapping = isBumpMapMaterial(buffer);
			//----------------------//

			int numStreams = meshChunkStream->readInt ();
			int numVertices = meshChunkStream->readInt ();
			int numIndices = meshChunkStream->readInt ();
			ATOM_BBox bbox;
			meshChunkStream->read (&bbox, sizeof(ATOM_BBox));
			mesh->setBoundingbox (bbox);

			// ������䶥����������
			ATOM_AUTOREF(ATOM_IndexArray) indices = device->allocIndexArray (ATOM_USAGE_STATIC, numIndices, false, true);
			if (!indices)
			{
				ATOM_DELETE(mesh);
				return false;
			}
			void *pIndexData = indices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			if (!pIndexData)
			{
				ATOM_DELETE(mesh);
				return false;
			}
			meshChunkStream->read (pIndexData, sizeof(short)*numIndices);
			indices->unlock ();

			// ���ö�������
			mesh->getGeometry()->setIndices (indices.get());

			// ������䶥�����ݻ���

			// �������Ƥ���� ʹ��MultiStream Geometry
			if( !bHWInstancingMesh )
			{
				bool bHasNormal = false;
				bool bHasTangent = false;
				ATOM_AUTOREF(ATOM_VertexArray) vertexArray_Position = 0;
				ATOM_AUTOREF(ATOM_VertexArray) vertexArray_Normal = 0;

				for (int i = 0; i < numStreams; ++i)
				{
					ATOM_AUTOREF(ATOM_VertexArray) vertexArray = 0;
					unsigned dataSize = 0;
					//--------------------------------------------//
					// �Ƿ���ѹ��
					unsigned int flag = ATTRIBUTE_FLAG_NONE;
					unsigned attrib = meshChunkStream->readInt();

					// �������ӳ�䣬�Ҹ������Ǹ��������� ����(��Ϊ���踱������Ϣ��
					if( bBumpMapping && attrib == ATOM_VERTEX_ATTRIB_BINORMAL )
						continue;

					if( ATOM_canUseCompress(attrib) )
						flag |= ATTRIBUTE_FLAG_COMPRESSED;
					if( bBumpMapping )
						flag |= ATTRIBUTE_FLAG_BUMPMAPPING;
					//--------------------------------------------//

					// ������������������� ��������VB
					if( attrib != ATOM_VERTEX_ATTRIB_TANGENT )
					{
						unsigned vertexSize = ATOM_GetVertexSize (attrib,flag);
						if (!vertexSize)
						{
							ATOM_DELETE(mesh);
							return false;
						}

						dataSize = numVertices * vertexSize;
						vertexArray = device->allocVertexArray (attrib, ATOM_USAGE_STATIC, numVertices, true, flag);
						if (!vertexArray)
						{
							ATOM_DELETE(mesh);
							return false;
						}
					}

					//----------------------------------------//
					// ������������VB
					if( attrib == ATOM_VERTEX_ATTRIB_COORD )
						vertexArray_Position = vertexArray;
					// ���·�������VB
					if( attrib == ATOM_VERTEX_ATTRIB_NORMAL )
						vertexArray_Normal = vertexArray;
					//----------------------------------------//

					////////////////////////////////// wangjian added for test float16 ////////////////////////////////////////////
					if( flag & ATTRIBUTE_FLAG_COMPRESSED )
					{
						unsigned component_count_uncompressd;
						unsigned component_count_compressd = ATOM_getCompressAttribComponent(attrib,component_count_uncompressd);
						
						// ��ȡԭ��������
						unsigned size = numVertices * component_count_uncompressd * sizeof(float);
						float * float_buffer = ATOM_NEW_ARRAY(float,numVertices * component_count_uncompressd);
						meshChunkStream->read ((void*)float_buffer, size);
						float * float_ptr = float_buffer;

						// ������������������� ��������VB
						FloatConversion::uint16_t * pVertexData = 0;
						FloatConversion::uint16_t * fp16_ptr = 0;
						if( attrib != ATOM_VERTEX_ATTRIB_TANGENT )
						{
							pVertexData = (FloatConversion::uint16_t*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
							if (!pVertexData)
							{
								ATOM_DELETE(mesh);
								return false;
							}
							fp16_ptr= pVertexData;
						} 

						// ����Ƿ������� ������ �������� ������������VB
						FloatConversion::uint16_t * pVertexPositionData = 0;
						FloatConversion::uint16_t * pVertexNormalData = 0;
						if( attrib == ATOM_VERTEX_ATTRIB_NORMAL || attrib == ATOM_VERTEX_ATTRIB_TANGENT )
						{
							pVertexPositionData = (FloatConversion::uint16_t*)vertexArray_Position->lock (ATOM_LOCK_READWRITE, 0, 0, true);
							if (!pVertexPositionData)
							{
								ATOM_DELETE(mesh);
								return false;
							}

							if( attrib == ATOM_VERTEX_ATTRIB_TANGENT )
							{
								// 
								pVertexNormalData = (FloatConversion::uint16_t*)vertexArray_Normal->lock (ATOM_LOCK_READWRITE, 0, 0, true);
								if (!pVertexNormalData)
								{
									ATOM_DELETE(mesh);
									return false;
								}
							}
						}

						// ѭ�����еĶ���
						for ( int i = 0; i < numVertices; ++i )
						{
							if( attrib != ATOM_VERTEX_ATTRIB_TANGENT )
							{
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
							}

							if( component_count_uncompressd > 2 )
							{
								// ����Ƿ�������
								if( attrib == ATOM_VERTEX_ATTRIB_NORMAL )
								{
									float normal_z = *(float_ptr++);

									*(fp16_ptr++) = FloatConversion::FloatToHalf(normal_z);
									*(fp16_ptr++) = FloatConversion::FloatToHalf(1.0f);

									float sign_normal_z = 0.0f;
									if( normal_z >= 0 )
										sign_normal_z = 1.0f;

									// λ�õĵ�4���������淨�ߵ�������
									pVertexPositionData[i*4+3] = FloatConversion::FloatToHalf(normal_z);
								}
								// �������������
								else if( attrib == ATOM_VERTEX_ATTRIB_TANGENT )
								{
									// ���ߵ�X/Y���浽���ߵ�Z/W������
									pVertexNormalData[i*4+2] = FloatConversion::FloatToHalf(*(float_ptr++));
									pVertexNormalData[i*4+3] = FloatConversion::FloatToHalf(*(float_ptr++));

									// ����Z�����������ű��浽λ�õĵ�4������(�ͷ��ߵı��뵽һ��
									float tangent_z = *(float_ptr++);
									float sign = 0.0f;
									if( tangent_z >= 0 )
										sign = 1.0f;

									// ���Ƚ��뷨�ߵ�������
									float sign_normal_z = FloatConversion::HalfToFloat(pVertexPositionData[i*4+3]);
									sign_normal_z += tangent_z * 2.0;

									// λ�õĵ�4���������淨�ߺ����ߵ�������
									pVertexPositionData[i*4+3] = FloatConversion::FloatToHalf(sign_normal_z);
								}
								// ���� ֱ�Ӹ���
								else
								{
									*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
									*(fp16_ptr++) = FloatConversion::FloatToHalf(1.0f);				// ʹ��1 Ҳ������ڲ�ʹ��BUMPMAPPINGʱ����normal��Z������������
									// ��ʹ��BUMPMAPPINGʱ����normal��Z�����������ź�tangent��Z������������
									// ʹ��fmod�ķ�����ȡ������
								}
							}
							else if( component_count_uncompressd == 4 )
							{
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
							}
						}

						// ������������� ��Ϊû������VB ��������ν����
						if( attrib != ATOM_VERTEX_ATTRIB_TANGENT )
							vertexArray->unlock ();

						if( pVertexPositionData )
							vertexArray_Position->unlock();
						if( pVertexNormalData )
							vertexArray_Normal->unlock();

						ATOM_DELETE_ARRAY(float_buffer);
					}
					else
					{
						void *pVertexData = vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
						if (!pVertexData)
						{
							ATOM_DELETE(mesh);
							return false;
						}
						meshChunkStream->read (pVertexData, dataSize);
						vertexArray->unlock ();
					}
					
					if( vertexArray )
						((ATOM_MultiStreamGeometry*)(mesh->getGeometry()))->addStream (vertexArray.get());
				}
			}
			else
			{
				unsigned attrib_total = 0;
				ATOM_VECTOR<unsigned> data_array_vertexsize;
				data_array_vertexsize.resize(numStreams,0);
				ATOM_VECTOR<char *> data_array;
				data_array.resize(numStreams,0);

				for (int i = 0; i < numStreams; ++i)
				{
					// �������
					unsigned attrib = meshChunkStream->readInt();
					
					// ��ö����С
					unsigned vertexSize = ATOM_GetVertexSize (attrib);
					data_array_vertexsize[i] = vertexSize;

					// �ܵĴ�С
					unsigned dataSize = numVertices * vertexSize;
					data_array[i] = (char*)ATOM_MALLOC(dataSize);

					// ��ȡ����ʱ����
					meshChunkStream->read (data_array[i], dataSize);

					// ���Ա��
					attrib_total |= attrib;
				}

				// �������㻺��
				ATOM_AUTOREF(ATOM_VertexArray) vertexArray = device->allocVertexArray (attrib_total, ATOM_USAGE_STATIC, numVertices, true);
				if (!vertexArray)
				{
					for( int i = 0; i < data_array.size(); ++i )
					{
						if( data_array[i] )
							ATOM_FREE(data_array[i]);
					}
					ATOM_DELETE(mesh);
					return false;
				}

				// �������㻺��
				char *pVertexData = (char*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				if (!pVertexData)
				{
					for( int i = 0; i < data_array.size(); ++i )
					{
						if( data_array[i] )
							ATOM_FREE(data_array[i]);
					}
					ATOM_DELETE(mesh);
					return false;
				}

				// ѭ�����ж���
				int vertexsize  = 0;
				char* dest_data = pVertexData;
				ATOM_VECTOR<char *> data_array_tmp = data_array;
				for( int iVert = 0; iVert < numVertices; ++iVert )
				{
					// ѭ�����е�STREAM
					for (int iStream = 0; iStream < numStreams; ++iStream)
					{
						vertexsize	= data_array_vertexsize[iStream];

						// ʹ��MEMCPY���ܻ��ֱ�Ӹ�ֵ������
						memcpy( dest_data, data_array_tmp[iStream],vertexsize );

						dest_data				+=	vertexsize;
						data_array_tmp[iStream] +=	vertexsize;
					}
				}

				vertexArray->unlock ();

				// ���ö��㻺��
				((ATOM_HWInstancingGeometry*)mesh->getGeometry())->setStream (vertexArray.get());

				for( int i = 0; i < data_array.size(); ++i )
				{
					if( data_array[i] )
						ATOM_FREE(data_array[i]);
				}
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////

			// read sub mesh material
			//ATOM_ChunkIO::Chunk *materialChunk = meshChunk->getFirstChild (ATOM_MAKE_FOURCC('m','a','t','l'));

			ATOM_AUTOPTR(ATOM_Material) material = 0;

			//--------------------------- wangjian modified --------------------------//
			// ѡ����ʵĺ��Ĳ����ļ�
			ATOM_STRING coremat_name = "";
			chooseProperCoreMatName( coremat_name, mesh->getGeometry()->getType()==GT_HWINSTANCING );
			//------------------------------------------------------------------------//

			if (!materialChunk)
			{
				material = ATOM_MaterialManager::createMaterialFromCore (device, coremat_name.c_str());
				mesh->setMaterial (material.get());
			}
			else
			{
				/*int size = materialChunk->getChunkStream()->readCString (0);
				materialChunk->getChunkStream()->seek (-size);
				char *buffer = ATOM_NEW_ARRAY(char, size);
				materialChunk->getChunkStream()->readCString (buffer);*/

				//--------------------------- wangjian modified --------------------------//
				// ѡ����ʵĺ��Ĳ����ļ�
				ATOM_STRING coremat_name = buffer;
				chooseProperCoreMatName(coremat_name,mesh->getGeometry()->getType()==GT_HWINSTANCING);
				//------------------------------------------------------------------------//

				material = ATOM_MaterialManager::createMaterialFromCore (device, coremat_name.c_str());
				mesh->setMaterial (material.get());
				ATOM_DELETE_ARRAY(buffer);

				if (!mesh->getMaterial())
				{
					material = ATOM_MaterialManager::createMaterialFromCore (	device, 
																				"/materials/builtin/model_default.mat"	);
					mesh->setMaterial (material.get());
				}
				else
				{
					ATOM_ParameterTable *paramTable = mesh->getMaterial()->getParameterTable();

					for (;;)
					{
						int size = materialChunk->getChunkStream()->readCString (0);
						materialChunk->getChunkStream()->seek (-size);
						char *buffer = ATOM_NEW_ARRAY(char, size);
						materialChunk->getChunkStream()->readCString (buffer);
						if (*buffer == '\0')
						{
							ATOM_DELETE_ARRAY(buffer);
							break;
						}

						ATOM_ParameterTable::ValueHandle paramValue = paramTable->getValueHandle (buffer);
						//if( strstr( buffer,"halfLambertScale"))
						//{
						//	ATOM_LOGGER::log("version 2 --- model : %s  / has property halfLambertScale : %f \n", filename, (*(float*)(paramValue->handle->getValuePtr())) );
						//}

						////--- wangjian added for test ---//
						//ATOM_ParameterTable::ValueHandle paramValue1 = paramTable->getValueHandle ("halfLambertScale");
						//if( paramValue1 && paramValue1->handle )
						//	ATOM_LOGGER::log("version 2 --- model : %s  /  halfLambertScale : %f \n", filename, (*(float*)(paramValue1->handle->getValuePtr())) );
						//-------------------------------//

						ATOM_DELETE_ARRAY(buffer);
						if (!paramValue)
						{
							/*char c = materialChunk->getChunkStream()->readChar();
							if( c == '/' )
							{
								int size = materialChunk->getChunkStream()->readCString (0);
								continue;
							}
							while( !isAlphabetic(c) )
							{
								c = materialChunk->getChunkStream()->readChar();
							}
							materialChunk->getChunkStream()->seek (-1);*/
							continue;
						}

						switch (paramValue->handle->getParamType())
						{
						case ATOM_MaterialParam::ParamType_Float:
							{
								//--- wangjian added for test ---//
								/*if( strstr( buffer,"halfLambertScale") )
									break;*/
								//-------------------------------//

								paramValue->setFloat (materialChunk->getChunkStream()->readFloat ());
								break;
							}
						case ATOM_MaterialParam::ParamType_FloatArray:
							{
								materialChunk->getChunkStream()->read (paramValue->f, sizeof(float)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Int:
							{
								paramValue->setInt (materialChunk->getChunkStream()->readInt ());
								break;
							}
						case ATOM_MaterialParam::ParamType_IntArray:
							{
								materialChunk->getChunkStream()->read (paramValue->i, sizeof(int)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Vector:
							{
								materialChunk->getChunkStream()->read (paramValue->v, sizeof(ATOM_Vector4f));
								break;
							}
						case ATOM_MaterialParam::ParamType_VectorArray:
							{
								materialChunk->getChunkStream()->read (paramValue->v, sizeof(ATOM_Vector4f)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix44:
							{
								materialChunk->getChunkStream()->read (paramValue->m44, sizeof(ATOM_Matrix4x4f));
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix44Array:
							{
								materialChunk->getChunkStream()->read (paramValue->m44, sizeof(ATOM_Matrix4x4f)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix43:
							{
								materialChunk->getChunkStream()->read (paramValue->m43, sizeof(ATOM_Matrix3x4f));
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix43Array:
							{
								materialChunk->getChunkStream()->read (paramValue->m43, sizeof(ATOM_Matrix3x4f)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Texture:
							{
								int size = materialChunk->getChunkStream()->readCString(0);
								materialChunk->getChunkStream()->seek (-size);
								char *s = (char*)ATOM_MALLOC(size);
								materialChunk->getChunkStream()->readCString (s);

								//--- wangjian added ---//
								// �첽����
								// �����ģ����Դֱ�Ӽ��أ���ͨ���첽���أ�������·�����浽immediate table
								ATOM_AsyncLoader::AddToImmediateTable(s);
								//----------------------//

								paramValue->setTexture(s,ATOM_LoadPriority_IMMEDIATE);

								//--- wangjian added ---//
								// �ͷ�
								ATOM_FREE(s);
								//----------------------//

								/*
								paramValue->textureFileName = s;
								ATOM_AUTOREF(ATOM_Texture) texture = ATOM_CreateTextureResource (paramValue->textureFileName.c_str());
								if (texture)
								{
									paramValue->setTexture (texture.get());
								}
								*/
								break;
							}
						}
					}
				}
			}

			//----- ���ò��ʵ�ȫ��ID ----//
			if( material )
			{
				char strMatName[256] = {0};
				sprintf(strMatName,"%s_%u",filename,mesh_index);
				material->setMaterialId(strMatName);
			}
			//--------------------------//

			_meshes.push_back (mesh);

			meshChunk = meshChunk->getNextSibling (ATOM_MAKE_FOURCC('a','t','s','m'));

			mesh_index++;
		}

		// wangjian commented
#if 0
		if (_meshes.size() != numMeshes)
		{
			return false;
		}
#endif

		ATOM_ChunkIO::Chunk *userAttribChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','t','r'));
		if (userAttribChunk)
		{
			while (userAttribChunk)
			{
				readUserAttributes (_userAttributes, userAttribChunk->getChunkStream());
				userAttribChunk = userAttribChunk->getNextSibling (ATOM_MAKE_FOURCC('a', 't', 't', 'r'));
			}
		}

		//--- wangjian added ---//
		// �첽����
		// ���ֱ�Ӽ��ظ�ģ�ͣ��ڼ���֮��������ɱ��
		getAsyncLoader()->SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED);
		//----------------------//
	}

	return true;
}

#else

int ATOM_SharedModel::load_half ( ATOM_RenderDevice *device, const char *filename, int loadPriority )
{
	unload ();

	//--- wangjian modified ---//
	// �첽����
	bool bMt = ( ATOM_AsyncLoader::IsRun() ) && ( loadPriority != ATOM_LoadPriority_IMMEDIATE );

	// ������첽����
	if( ATOM_AsyncLoader::IsRun() && bMt )
	{
		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::log("ATOM_SharedModel::load --- async load model : %s | priority : %d\n", filename, loadPriority );

		getAsyncLoader()->SetModel(this);
		if( !getAsyncLoader()->Start(filename,loadPriority) )
			return 0;
	}
	// ֱ�Ӽ���
	else
	{
		if( ATOM_AsyncLoader::isEnableLog() )
		{
			ATOM_LOGGER::log("ATOM_SharedModel::load --- sync load model : %s \n", filename );
		}

		const char* ext = strrchr(filename, '.');
		if (ext && !stricmp(ext, ".obj")) {
			return load_obj(device, filename, bMt);
		}

		unsigned sig;

		{
			ATOM_AutoFile f (filename, ATOM_VFS::read);
			if (!f)
			{
				return 0;
			}
			if (f->size() < sizeof(unsigned))
			{
				return 0;
			}
			f->read (&sig, sizeof(unsigned));
		}

		// �ϰ汾��ģ�Ͷ�ȡ
		if (sig == 0x4e334d46)
		{
			//--- wangjian modified ---//
			if( load_nm (device, filename, bMt) )
			{
				return 1;
			}
			return 0;
			//----------------------//
		}

		// �°汾��ģ�Ͷ�ȡ

		ATOM_ChunkIO chunkIO;
		if (!chunkIO.load (filename))
		{
			return 0;
		}

		ATOM_ChunkIO::Chunk *topChunk = chunkIO.getTopChunk ();
		ATOM_ChunkIO::Chunk *mainChunk = topChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','m','e'));
		if (!mainChunk)
		{
			return 0;
		}
		ATOM_ChunkStream *mainChunkStream = mainChunk->getChunkStream();
		int version = mainChunkStream->readInt ();
		int numMeshes = mainChunkStream->readInt ();

		// ����Ѿ���ѹ���汾��ʽ�����˳�
		int flags = mainChunkStream->readInt ();
		if( flags & MF_COMRESSION )
		{
			return 2;
		}
		int numTracks = mainChunkStream->readInt ();
		mainChunkStream->read (&_boundingbox, sizeof(ATOM_BBox));

		// read skeleton
		ATOM_ChunkIO::Chunk *skeletonChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','s','k'));
		if (skeletonChunk)
		{
			ATOM_ChunkStream *skeletonChunkStream = skeletonChunk->getChunkStream();
			ATOM_AUTOPTR(ATOM_Skeleton) skeleton = ATOM_NEW(ATOM_Skeleton);
			skeleton->setHashCode (skeletonChunkStream->readInt ());
			skeleton->setNumJoints (skeletonChunkStream->readInt ());
			skeletonChunkStream->read (&skeleton->getJointMatrices()[0], sizeof(skeleton->getJointMatrices()[0])*skeleton->getJointMatrices().size());
			skeletonChunkStream->read (&skeleton->getJointParents()[0], sizeof(skeleton->getJointParents()[0])*skeleton->getJointParents().size());
			for (unsigned i = 0; i < skeleton->getNumJoints(); ++i)
			{
				char buffer[256];
				skeletonChunkStream->readCString (buffer);
				skeleton->addAttachPoint (buffer, i);
			}
			_skeleton = skeleton;
		}

		// read animation datas.
		ATOM_ChunkIO::Chunk *jointAnimationDataChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('j','a','d','t'));
		if (jointAnimationDataChunk)
		{
			ATOM_ChunkStream *jointAnimationDataChunkStream = jointAnimationDataChunk->getChunkStream ();
			_animationDataCache = ATOM_NEW(ATOM_JointAnimationDataCache);
			unsigned numFrames = jointAnimationDataChunkStream->readInt ();
			_animationDataCache->setNumFrames (numFrames);
			for (unsigned i = 0; i < numFrames; ++i)
			{
				unsigned numJoints = jointAnimationDataChunkStream->readInt ();
				_animationDataCache->setNumJoints (i, numJoints);
				jointAnimationDataChunkStream->read (_animationDataCache->getJoints (i), sizeof(ATOM_JointTransformInfo)*numJoints);
			}

			ATOM_ChunkIO::Chunk *jointAnimationTrackChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('j','a','t','k'));
			while (jointAnimationTrackChunk)
			{
				ATOM_ChunkStream *jointAnimationTrackChunkStream = jointAnimationTrackChunk->getChunkStream ();
				ATOM_AUTOPTR(ATOM_JointAnimationTrack) track = ATOM_NEW(ATOM_JointAnimationTrack, this);
				track->setAnimationDataCache (_animationDataCache.get());
				int num = jointAnimationTrackChunkStream->readCString (0);
				jointAnimationTrackChunkStream->seek (-num);
				char *s = (char*)ATOM_MALLOC(num);
				jointAnimationTrackChunkStream->readCString (s);
				track->setName (s);
				ATOM_FREE(s);
				int numFrames = jointAnimationTrackChunkStream->readInt ();
				track->setNumFrames (numFrames);
				jointAnimationTrackChunkStream->read (track->getFrameJointIndices(), numFrames*sizeof(unsigned));
				jointAnimationTrackChunkStream->read (track->getFrameTimes(), numFrames*sizeof(unsigned));
				_tracks.push_back (track);

				jointAnimationTrackChunk = jointAnimationTrackChunk->getNextSibling (ATOM_MAKE_FOURCC('j','a','t','k'));
			}
		}

		///////////////////////////////////////////// read sub meshes ///////////////////////////////////////////////////

		int mesh_index = 0;
		ATOM_ChunkIO::Chunk *meshChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','s','m'));
		while (meshChunk)
		{
			//-------- wangjian modified ----------//
			// ���ȶ�ȡ�����ļ�����
			ATOM_ChunkIO::Chunk *materialChunk = meshChunk->getFirstChild (ATOM_MAKE_FOURCC('m','a','t','l'));
			char *buffer = 0;
			if( materialChunk )
			{
				int size = materialChunk->getChunkStream()->readCString (0);
				materialChunk->getChunkStream()->seek (-size);
				buffer = ATOM_NEW_ARRAY(char, size);
				materialChunk->getChunkStream()->readCString (buffer);
			}
			//====================================//


			ATOM_ChunkStream *meshChunkStream = meshChunk->getChunkStream();

			ATOM_SharedMesh *mesh = ATOM_NEW(ATOM_SharedMesh, this);

			// read mesh name
			int numchars = meshChunkStream->readCString (0);
			meshChunkStream->seek (-numchars);
			char *name = (char*)ATOM_MALLOC(numchars);
			meshChunkStream->readCString (name);
			mesh->setName (name);
			ATOM_FREE(name);

			// read mesh joint mapping data
			int numMappings = meshChunkStream->readInt ();
			if (numMappings > 0)
			{
				ATOM_VECTOR<int> jointMap(numMappings);
				meshChunkStream->read (&jointMap[0], numMappings*sizeof(int));
				mesh->setJointMap (jointMap);
			}

			// read vertex streams and index data

			//--- wangjian added ---//
			// ��ǰ���� SKINNING ��ģ�ͷ�Ϊ3�֣�
			// 1. ֻ��MESH��Ϣ û�й�����Ϣ Ҳ û�ж�����Ϣ numTracks = 0
			// 2. û�й�����Ϣ Ҳ û�ж�����Ϣ numTracks > 0
			// 3. ���й�����Ϣ ���ж�����Ϣ 
			bool bNoneSkin			= ( numTracks == 0 ) && (skeletonChunk) && (!jointAnimationDataChunk);
			if( bNoneSkin )
			{
				ATOM_VECTOR<int> jointMap;
				mesh->setJointMap(jointMap);
			}
			bool bSkinniedMesh = !mesh->getJointMap().empty();
			bool bHWInstancingMesh = useHWInstancingMesh(bSkinniedMesh,buffer?buffer:"");
			if( bHWInstancingMesh )
				mesh->setGeometry (ATOM_NEW(ATOM_HWInstancingGeometry));	// �������û�й�����Ϣ��ʹ��HWInstancing Geometry
			else
				mesh->setGeometry (ATOM_NEW(ATOM_MultiStreamGeometry));		// �������Ƥ���� ʹ��MultiStream Geometry
			//----------------------//

			int numStreams = meshChunkStream->readInt ();
			int numVertices = meshChunkStream->readInt ();
			int numIndices = meshChunkStream->readInt ();
			ATOM_BBox bbox;
			meshChunkStream->read (&bbox, sizeof(ATOM_BBox));
			mesh->setBoundingbox (bbox);

			// ������䶥����������
			ATOM_AUTOREF(ATOM_IndexArray) indices = device->allocIndexArray (ATOM_USAGE_STATIC, numIndices, false, true);
			if (!indices)
			{
				ATOM_DELETE(mesh);
				return 0;
			}
			void *pIndexData = indices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			if (!pIndexData)
			{
				ATOM_DELETE(mesh);
				return 0;
			}
			meshChunkStream->read (pIndexData, sizeof(short)*numIndices);
			indices->unlock ();

			// ���ö�������
			mesh->getGeometry()->setIndices (indices.get());

			// ������䶥�����ݻ���

			// �������Ƥ���� ʹ��MultiStream Geometry
			if( !bHWInstancingMesh )
			{
				for (int i = 0; i < numStreams; ++i)
				{
					unsigned attrib = meshChunkStream->readInt();

					// wangjian added 
					// ����ǰ汾2��ѹ���汾��
					// �жϸ�attrib�Ƿ��ѹ��
					unsigned int flag = ATTRIBUTE_FLAG_NONE;
					if( ATOM_canUseCompress(attrib) )
						flag |= ATTRIBUTE_FLAG_COMPRESSED;
						
					// �õ���attrib�Ĵ�С
					unsigned vertexSize = ATOM_GetVertexSize (attrib,flag);
					if (!vertexSize)
					{
						ATOM_DELETE(mesh);
						return 0;
					}

					// ����VB
					unsigned dataSize = numVertices * vertexSize;
					ATOM_AUTOREF(ATOM_VertexArray) vertexArray = device->allocVertexArray (attrib, ATOM_USAGE_STATIC, numVertices, true, flag);
					if (!vertexArray)
					{
						ATOM_DELETE(mesh);
						return 0;
					}


					////////////////////////////////// wangjian added for test float16 ////////////////////////////////////////////

					// ��attrib�ǿ�ѹ����
					// ����ת����ѹ����ʽ
					if( flag & ATTRIBUTE_FLAG_COMPRESSED )
					{
						unsigned component_count_uncompressd;
						unsigned component_count_compressd = ATOM_getCompressAttribComponent(attrib,component_count_uncompressd);

						// ��ȡԭ��������
						unsigned size = numVertices * component_count_uncompressd * sizeof(float);
						float * float_buffer = ATOM_NEW_ARRAY(float,numVertices * component_count_uncompressd);
						meshChunkStream->read ((void*)float_buffer, size);
						float * float_ptr = float_buffer;

						// 
						FloatConversion::uint16_t * pVertexData = (FloatConversion::uint16_t*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
						if (!pVertexData)
						{
							ATOM_DELETE(mesh);
							return 0;
						}
						FloatConversion::uint16_t * fp16_ptr = pVertexData;
						for ( int i = 0; i < numVertices; ++i )
						{
							*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
							*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));

							if( component_count_uncompressd == 3 )
							{
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
								*(fp16_ptr++) = FloatConversion::FloatToHalf(1.0f);				// ʹ��1 Ҳ������ڲ�ʹ��BUMPMAPPINGʱ����normal��Z������������
								// ��ʹ��BUMPMAPPINGʱ����normal��Z�����������ź�tangent��Z������������
								// ʹ��fmod�ķ�����ȡ������

							}
							else if( component_count_uncompressd == 4 )
							{
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
								*(fp16_ptr++) = FloatConversion::FloatToHalf(*(float_ptr++));
							}
						}

						vertexArray->unlock ();

						ATOM_DELETE_ARRAY(float_buffer);
					}
					// ��attrib�ǲ���ѹ����
					else
					{
						void *pVertexData = vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
						if (!pVertexData)
						{
							ATOM_DELETE(mesh);
							return 0;
						}
						meshChunkStream->read (pVertexData, dataSize);
						vertexArray->unlock ();
					}

					((ATOM_MultiStreamGeometry*)(mesh->getGeometry()))->addStream (vertexArray.get());
				}

				// wangjian added for 
#if 0
				bool bNeedSaveTangents = false;
				if( isBumpMapMaterial( buffer ) )
					bNeedSaveTangents = true;

				if( bNeedSaveTangents )
				{
					ATOM_VertexArray *tangents	= ( (ATOM_MultiStreamGeometry*)mesh->getGeometry())->getStream (ATOM_VERTEX_ATTRIB_TANGENT);
					ATOM_VertexArray *binormals = ( (ATOM_MultiStreamGeometry*)mesh->getGeometry())->getStream (ATOM_VERTEX_ATTRIB_BINORMAL);
					if( !tangents || !binormals )
					{
						mesh->genTangentSpace();
					}
				}
#endif
			}
			else
			{
				unsigned attrib_total = 0;
				ATOM_VECTOR<unsigned> data_array_vertexsize;
				data_array_vertexsize.resize(numStreams,0);
				ATOM_VECTOR<char *> data_array;
				data_array.resize(numStreams,0);

				for (int i = 0; i < numStreams; ++i)
				{
					// �������
					unsigned attrib = meshChunkStream->readInt();
					
					// ��ö����С
					unsigned vertexSize = ATOM_GetVertexSize (attrib);
					data_array_vertexsize[i] = vertexSize;

					// �ܵĴ�С
					unsigned dataSize = numVertices * vertexSize;
					data_array[i] = (char*)ATOM_MALLOC(dataSize);

					// ��ȡ����ʱ����
					meshChunkStream->read (data_array[i], dataSize);

					// ���Ա��
					attrib_total |= attrib;
				}

				// �������㻺��
				ATOM_AUTOREF(ATOM_VertexArray) vertexArray = device->allocVertexArray (attrib_total, ATOM_USAGE_STATIC, numVertices, true);
				if (!vertexArray)
				{
					for( int i = 0; i < data_array.size(); ++i )
					{
						if( data_array[i] )
							ATOM_FREE(data_array[i]);
					}
					ATOM_DELETE(mesh);
					return 0;
				}

				// �������㻺��
				char *pVertexData = (char*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				if (!pVertexData)
				{
					for( int i = 0; i < data_array.size(); ++i )
					{
						if( data_array[i] )
							ATOM_FREE(data_array[i]);
					}
					ATOM_DELETE(mesh);
					return 0;
				}

				// ѭ�����ж���
				int vertexsize  = 0;
				char* dest_data = pVertexData;
				ATOM_VECTOR<char *> data_array_tmp = data_array;
				for( int iVert = 0; iVert < numVertices; ++iVert )
				{
					// ѭ�����е�STREAM
					for (int iStream = 0; iStream < numStreams; ++iStream)
					{
						vertexsize	= data_array_vertexsize[iStream];

						// ʹ��MEMCPY���ܻ��ֱ�Ӹ�ֵ������
						memcpy( dest_data, data_array_tmp[iStream],vertexsize );

						dest_data				+=	vertexsize;
						data_array_tmp[iStream] +=	vertexsize;
					}
				}

				vertexArray->unlock ();

				// ���ö��㻺��
				((ATOM_HWInstancingGeometry*)mesh->getGeometry())->setStream (vertexArray.get());

				for( int i = 0; i < data_array.size(); ++i )
				{
					if( data_array[i] )
						ATOM_FREE(data_array[i]);
				}
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////

			ATOM_AUTOPTR(ATOM_Material) material = 0;

			//--------------------------- wangjian modified --------------------------//
			// ѡ����ʵĺ��Ĳ����ļ�
			ATOM_STRING coremat_name = "";
			chooseProperCoreMatName(coremat_name,mesh->getGeometry()->getType()==GT_HWINSTANCING);
			//------------------------------------------------------------------------//

			if (!materialChunk)
			{
				material = ATOM_MaterialManager::createMaterialFromCore (device, coremat_name.c_str());
				mesh->setMaterial (material.get());
			}
			else
			{
				//--------------------------- wangjian modified --------------------------//
				// ѡ����ʵĺ��Ĳ����ļ�
				ATOM_STRING coremat_name = buffer;
				chooseProperCoreMatName(coremat_name,mesh->getGeometry()->getType()==GT_HWINSTANCING);
				//------------------------------------------------------------------------//

				material = ATOM_MaterialManager::createMaterialFromCore (device, coremat_name.c_str());
				mesh->setMaterial (material.get());
				ATOM_DELETE_ARRAY(buffer);

				if (!mesh->getMaterial())
				{
					material = ATOM_MaterialManager::createMaterialFromCore (	device, 
																				"/materials/builtin/model_default.mat"	);
					mesh->setMaterial (material.get());
				}
				else
				{
					ATOM_ParameterTable *paramTable = mesh->getMaterial()->getParameterTable();

					for (;;)
					{
						int size = materialChunk->getChunkStream()->readCString (0);
						materialChunk->getChunkStream()->seek (-size);
						char *buffer = ATOM_NEW_ARRAY(char, size);
						materialChunk->getChunkStream()->readCString (buffer);
						if (*buffer == '\0')
						{
							ATOM_DELETE_ARRAY(buffer);
							break;
						}

						ATOM_ParameterTable::ValueHandle paramValue = paramTable->getValueHandle (buffer);

						ATOM_DELETE_ARRAY(buffer);
						if (!paramValue)
						{
							continue;
						}

						switch (paramValue->handle->getParamType())
						{
						case ATOM_MaterialParam::ParamType_Float:
							{
								paramValue->setFloat (materialChunk->getChunkStream()->readFloat ());
								break;
							}
						case ATOM_MaterialParam::ParamType_FloatArray:
							{
								materialChunk->getChunkStream()->read (paramValue->f, sizeof(float)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Int:
							{
								paramValue->setInt (materialChunk->getChunkStream()->readInt ());
								break;
							}
						case ATOM_MaterialParam::ParamType_IntArray:
							{
								materialChunk->getChunkStream()->read (paramValue->i, sizeof(int)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Vector:
							{
								materialChunk->getChunkStream()->read (paramValue->v, sizeof(ATOM_Vector4f));
								break;
							}
						case ATOM_MaterialParam::ParamType_VectorArray:
							{
								materialChunk->getChunkStream()->read (paramValue->v, sizeof(ATOM_Vector4f)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix44:
							{
								materialChunk->getChunkStream()->read (paramValue->m44, sizeof(ATOM_Matrix4x4f));
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix44Array:
							{
								materialChunk->getChunkStream()->read (paramValue->m44, sizeof(ATOM_Matrix4x4f)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix43:
							{
								materialChunk->getChunkStream()->read (paramValue->m43, sizeof(ATOM_Matrix3x4f));
								break;
							}
						case ATOM_MaterialParam::ParamType_Matrix43Array:
							{
								materialChunk->getChunkStream()->read (paramValue->m43, sizeof(ATOM_Matrix3x4f)*paramValue->handle->getDimension());
								break;
							}
						case ATOM_MaterialParam::ParamType_Texture:
							{
								int size = materialChunk->getChunkStream()->readCString(0);
								materialChunk->getChunkStream()->seek (-size);
								char *s = (char*)ATOM_MALLOC(size);
								materialChunk->getChunkStream()->readCString (s);

								//--- wangjian added ---//
								// �첽����
								// �����ģ����Դֱ�Ӽ��أ���ͨ���첽���أ�������·�����浽immediate table
								ATOM_AsyncLoader::AddToImmediateTable(s);
								//----------------------//

								paramValue->setTexture(s,ATOM_LoadPriority_IMMEDIATE);

								//--- wangjian added ---//
								// �ͷ�
								ATOM_FREE(s);
								//----------------------//

								break;
							}
						}
					}
				}
			}

			//----- ���ò��ʵ�ȫ��ID ----//
			if( material )
			{
				char strMatName[256] = {0};
				sprintf(strMatName,"%s_%u",filename,mesh_index);
				material->setMaterialId(strMatName);
			}
			//--------------------------//

			_meshes.push_back (mesh);

			meshChunk = meshChunk->getNextSibling (ATOM_MAKE_FOURCC('a','t','s','m'));

			mesh_index++;
		}

		if (_meshes.size() != numMeshes)
		{
			return 0;
		}

		ATOM_ChunkIO::Chunk *userAttribChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','t','r'));
		if (userAttribChunk)
		{
			while (userAttribChunk)
			{
				readUserAttributes (_userAttributes, userAttribChunk->getChunkStream());
				userAttribChunk = userAttribChunk->getNextSibling (ATOM_MAKE_FOURCC('a', 't', 't', 'r'));
			}
		}

		//--- wangjian added ---//
		// �첽����
		// ���ֱ�Ӽ��ظ�ģ�ͣ��ڼ���֮��������ɱ��
		getAsyncLoader()->SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED);
		//----------------------//
	}

	return 1;
}

#endif

bool ATOM_SharedModel::save_half (const char *filename)
{
	ATOM_ChunkIO chunkIO;
	ATOM_ChunkIO::Chunk *mainChunk = chunkIO.getTopChunk()->appendChild ();
	mainChunk->setSig (ATOM_MAKE_FOURCC('a','t','m','e'));
	mainChunk->getChunkStream()->writeInt (1); // version
	mainChunk->getChunkStream()->writeInt (_meshes.size()); // mesh count
	mainChunk->getChunkStream()->writeInt (MF_COMRESSION);
	mainChunk->getChunkStream()->writeInt (_tracks.size());
	mainChunk->getChunkStream()->write (&_boundingbox, sizeof(ATOM_BBox));

	if (_skeleton)
	{
		ATOM_ChunkIO::Chunk *skeletonChunk = mainChunk->appendChild ();
		skeletonChunk->setSig (ATOM_MAKE_FOURCC('a','t','s','k'));
		ATOM_ChunkStream *skeletonChunkStream = skeletonChunk->getChunkStream();
		skeletonChunkStream->writeInt (_skeleton->getHashCode());
		skeletonChunkStream->writeInt (_skeleton->getNumJoints());
		skeletonChunkStream->write (&_skeleton->getJointMatrices()[0], sizeof(_skeleton->getJointMatrices()[0])*_skeleton->getJointMatrices().size());
		skeletonChunkStream->write (&_skeleton->getJointParents()[0], sizeof(_skeleton->getJointParents()[0])*_skeleton->getJointParents().size());
		for (unsigned i = 0; i < _skeleton->getNumJoints(); ++i)
		{
			skeletonChunkStream->writeCString (_skeleton->getAttachPointName(i));
		}
	}

	if (_animationDataCache)
	{
		ATOM_ChunkIO::Chunk *jointAnimationDataCacheChunk = mainChunk->appendChild ();
		jointAnimationDataCacheChunk->setSig (ATOM_MAKE_FOURCC('j','a','d','t'));
		ATOM_ChunkStream *cacheChunkStream = jointAnimationDataCacheChunk->getChunkStream();
		cacheChunkStream->writeInt (_animationDataCache->getNumFrames ());
		for (unsigned i = 0; i < _animationDataCache->getNumFrames(); ++i)
		{
			unsigned numJoints = _animationDataCache->getNumJoints (i);
			cacheChunkStream->writeInt (numJoints);
			cacheChunkStream->write (_animationDataCache->getJoints (i), numJoints*sizeof(ATOM_JointTransformInfo));
		}
	}

	for (unsigned i = 0; i < _tracks.size(); ++i)
	{
		ATOM_ChunkIO::Chunk *trackChunk = mainChunk->appendChild ();
		trackChunk->setSig (ATOM_MAKE_FOURCC('j','a','t','k'));
		ATOM_ChunkStream *trackChunkStream = trackChunk->getChunkStream();
		trackChunkStream->writeCString (_tracks[i]->getName());
		trackChunkStream->writeInt (_tracks[i]->getNumFrames());
		trackChunkStream->write (_tracks[i]->getFrameJointIndices(), _tracks[i]->getNumFrames()*sizeof(unsigned));
		trackChunkStream->write (_tracks[i]->getFrameTimes(), _tracks[i]->getNumFrames()*sizeof(unsigned));
	}

	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		// wangjian added 
		// ��������Ĳ��������Ƿ���model_bumpmap.mat������ǣ�����Ҫ����������Ϣ
		// ���򲻱���
		bool bNeedSaveTangents = false;
		ATOM_Material *material = _meshes[i]->getMaterial();
		ATOM_CoreMaterial *coreMaterial = material ? material->getCoreMaterial() : 0;
		if (coreMaterial)
		{
			if( isBumpMapMaterial( coreMaterial->getObjectName() ) )
				bNeedSaveTangents = true;
		}
		////////////////////////////////////////////////////////////////////////////////////////


		ATOM_VECTOR<StreamInfo> streamInfos;
		unsigned numVertices, numIndices;
		ATOM_MultiStreamGeometry *geo = (ATOM_MultiStreamGeometry*)_meshes[i]->getGeometry();

		int attribs[] = {
			ATOM_VERTEX_ATTRIB_NORMAL,
			ATOM_VERTEX_ATTRIB_PSIZE,
			ATOM_VERTEX_ATTRIB_PRIMARY_COLOR,
			ATOM_VERTEX_ATTRIB_TANGENT,
			ATOM_VERTEX_ATTRIB_BINORMAL,
			ATOM_VERTEX_ATTRIB_TEX_MASK(0),
			ATOM_VERTEX_ATTRIB_TEX_MASK(1),
			ATOM_VERTEX_ATTRIB_TEX_MASK(2),
			ATOM_VERTEX_ATTRIB_TEX_MASK(3),
			ATOM_VERTEX_ATTRIB_TEX_MASK(4),
			ATOM_VERTEX_ATTRIB_TEX_MASK(5),
			ATOM_VERTEX_ATTRIB_TEX_MASK(6),
			ATOM_VERTEX_ATTRIB_TEX_MASK(7)
		};

		ATOM_IndexArray *indices = geo->getIndices ();
		ATOM_ASSERT(indices);
		numIndices = indices->getNumIndices ();

		ATOM_VertexArray *vertices = geo->getStream (ATOM_VERTEX_ATTRIB_COORD);
		ATOM_ASSERT(vertices);
		numVertices = vertices->getNumVertices ();
		streamInfos.resize (streamInfos.size()+1);
		streamInfos.back().attrib = ATOM_VERTEX_ATTRIB_COORD;
		streamInfos.back().vertexArray = vertices;

		for (unsigned n = 0; n < sizeof(attribs)/sizeof(attribs[0]); ++n)
		{
			ATOM_VertexArray *stream = geo->getStream (attribs[n]);

			if (stream)
			{
				// ����Ƿ���Ҫ����������Ϣ 
				// ����MESH��������Ϣ ���� �����Ƿ�BUMP�� ������
				if( ( stream->getAttributes() & ( ATOM_VERTEX_ATTRIB_TANGENT | ATOM_VERTEX_ATTRIB_BINORMAL ) ) && 
					!bNeedSaveTangents )
					continue;

				streamInfos.resize (streamInfos.size()+1);
				streamInfos.back().attrib = stream->getAttributes();
				streamInfos.back().vertexArray = stream;
			}
		}

		ATOM_ChunkIO::Chunk *meshChunk = mainChunk->appendChild ();
		meshChunk->setSig (ATOM_MAKE_FOURCC('a','t','s','m'));
		ATOM_ChunkStream *meshChunkStream = meshChunk->getChunkStream();

		meshChunkStream->writeCString (_meshes[i]->getName());

		meshChunkStream->writeInt (_meshes[i]->getJointMap().size());
		if (!_meshes[i]->getJointMap().empty())
		{
			meshChunkStream->write (&_meshes[i]->getJointMap()[0], _meshes[i]->getJointMap().size()*sizeof(int));
		}

		meshChunkStream->writeInt (streamInfos.size());
		meshChunkStream->writeInt (numVertices);
		meshChunkStream->writeInt (numIndices);
		meshChunkStream->write (&_meshes[i]->getBoundingbox(), sizeof(ATOM_BBox));

		void *pIndexData = indices->lock (ATOM_LOCK_READWRITE, 0, 0, false);
		if (!pIndexData)
		{
			return 0;
		}
		meshChunkStream->write (pIndexData, sizeof(short)*numIndices);
		indices->unlock ();

		for (unsigned n = 0; n < streamInfos.size(); ++n)
		{
			ATOM_VertexArray *vertexArray = streamInfos[n].vertexArray;
			unsigned attrib = vertexArray->getAttributes();
			meshChunkStream->writeInt (attrib);

			bool bCompressed = ATOM_canUseCompress(attrib);
			unsigned dataSize = numVertices * ATOM_GetVertexSize(attrib,bCompressed ? ATTRIBUTE_FLAG_COMPRESSED : 0 );
			void *pVertexData = vertexArray->lock (ATOM_LOCK_READONLY, 0, 0, false);
			if (!pVertexData)
			{
				return 0;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////
			/*float * fp32_ptr = (float*)pVertexData;
			unsigned fp32_count = dataSize/sizeof(float);
			FloatConversion::uint16_t * half_buffer = ATOM_NEW_ARRAY(FloatConversion::uint16_t,fp32_count);
			FloatConversion::uint16_t * hf16_ptr = half_buffer;
			for ( int i = 0; i < fp32_count; ++i )
			{
				hf16_ptr[i] = FloatConversion::FloatToHalf(*fp32_ptr++);
			}
			meshChunkStream->write ( (void*)half_buffer, fp32_count*sizeof(FloatConversion::uint16_t) );
			ATOM_DELETE_ARRAY(half_buffer);*/
			meshChunkStream->write ( pVertexData, dataSize );
			////////////////////////////////////////////////////////////////////////////////////////////////

			vertexArray->unlock ();
		}

		material = _meshes[i]->getMaterial();
		coreMaterial = material ? material->getCoreMaterial() : 0;
		if (coreMaterial)
		{
			ATOM_ChunkIO::Chunk *materialChunk = meshChunk->appendChild ();
			materialChunk->setSig (ATOM_MAKE_FOURCC('m','a','t','l'));
			materialChunk->getChunkStream()->writeCString (coreMaterial->getObjectName());
			ATOM_ParameterTable *paramTable = material->getParameterTable ();
			for (int i = 0; i < paramTable->getNumParameters(); ++i)
			{
				ATOM_ParameterTable::ValueHandle value = paramTable->getParameter (i);
				ATOM_MaterialParam::EditorType editorType = value->handle->getEditorType();
				if (editorType != ATOM_MaterialParam::ParamEditorType_Disable)
				{
					materialChunk->getChunkStream()->writeCString (value->getParameterName());

					switch (value->handle->getParamType ())
					{
					case ATOM_MaterialParam::ParamType_Float:
						materialChunk->getChunkStream()->writeFloat (*value->f);
						break;
					case ATOM_MaterialParam::ParamType_FloatArray:
						materialChunk->getChunkStream()->write (value->f, sizeof(float)*value->handle->getDimension());
						break;
					case ATOM_MaterialParam::ParamType_Int:
						materialChunk->getChunkStream()->writeInt (*value->i);
						break;
					case ATOM_MaterialParam::ParamType_IntArray:
						materialChunk->getChunkStream()->write (value->i, sizeof(int)*value->handle->getDimension());
						break;
					case ATOM_MaterialParam::ParamType_Vector:
						materialChunk->getChunkStream()->write (value->v, sizeof(ATOM_Vector4f));
						break;
					case ATOM_MaterialParam::ParamType_VectorArray:
						materialChunk->getChunkStream()->write (value->v, sizeof(ATOM_Vector4f)*value->handle->getDimension());
						break;
					case ATOM_MaterialParam::ParamType_Matrix44:
						materialChunk->getChunkStream()->write (value->m44, sizeof(ATOM_Matrix4x4f));
						break;
					case ATOM_MaterialParam::ParamType_Matrix44Array:
						materialChunk->getChunkStream()->write (value->m44, sizeof(ATOM_Matrix4x4f)*value->handle->getDimension());
						break;
					case ATOM_MaterialParam::ParamType_Matrix43:
						materialChunk->getChunkStream()->write (value->m43, sizeof(ATOM_Matrix3x4f));
						break;
					case ATOM_MaterialParam::ParamType_Matrix43Array:
						materialChunk->getChunkStream()->write (value->m43, sizeof(ATOM_Matrix3x4f)*value->handle->getDimension());
						break;
					case ATOM_MaterialParam::ParamType_Texture:
						materialChunk->getChunkStream()->writeCString (value->textureFileName.c_str());
						break;
					default:
						break;
					}
				}
			}

			materialChunk->getChunkStream()->writeCString (""); // end of parameters
		}
	}

	if (_userAttributes->getNumAttributes() > 0)
	{
		ATOM_ChunkIO::Chunk *userAttribChunk = mainChunk->appendChild ();
		userAttribChunk->setSig (ATOM_MAKE_FOURCC('a', 't', 't', 'r'));
		saveUserAttributes (_userAttributes, userAttribChunk->getChunkStream ());
	}

	return chunkIO.save (filename);
}

bool ATOM_SharedModel::convertToCompressed (ATOM_RenderDevice *device, const char *filename)
{
	int resflag = load_half(device,filename);
	if( resflag == 2 )
	{
		return true;
	}
	else if( resflag == 1 )
	{
		return save_half(filename);
	}

	return false;
}

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_Skeleton *ATOM_SharedModel::getSkeleton (void) const
{
	return _skeleton.get();
}

bool ATOM_SharedModel::readUserAttributes (ATOM_UserAttributes *attributes, ATOM_ChunkStream *stream)
{
	char s[2048];

	int numAttributes = stream->readInt ();
	if (numAttributes > 0)
	{
		for (int i = 0; i < numAttributes; ++i)
		{
			stream->readCString (s);

			int type = stream->readInt ();
			switch (type)
			{
			case ATOM_Variant::INT:
				{
					int value = stream->readInt ();
					attributes->setAttribute (s, value);
					break;
				}
			case ATOM_Variant::INT_ARRAY:
				{
					int dim = stream->readInt ();
					ATOM_VECTOR<int> v(dim);
					for (int j = 0; j < dim; ++j)
					{
						v[j] = stream->readInt ();
					}
					ATOM_Variant value(&v[0], v.size());
					attributes->setAttribute (s, value);
					break;
				}
			case ATOM_Variant::FLOAT:
				{
					float value = stream->readFloat ();
					attributes->setAttribute (s, value);
					break;
				}
			case ATOM_Variant::FLOAT_ARRAY:
				{
					int dim = stream->readInt ();
					ATOM_VECTOR<float> v(dim);
					for (int j = 0; j < dim; ++j)
					{
						v[j] = stream->readFloat ();
					}
					ATOM_Variant value(&v[0], v.size());
					attributes->setAttribute (s, value);
					break;
				}
			case ATOM_Variant::STRING:
				{
					int len = stream->readCString(0);
					char *str = ATOM_NEW_ARRAY(char, len+1);
					stream->readCString (str);
					ATOM_Variant value;
					value.setS (str);
					attributes->setAttribute (s, value);
					ATOM_DELETE_ARRAY(str);
					break;
				}
			case ATOM_Variant::VECTOR4:
				{
					ATOM_Vector4f v;
					stream->read (&v, sizeof(ATOM_Vector4f));
					attributes->setAttribute (s, v);
					break;
				}
			case ATOM_Variant::MATRIX44:
				{
					ATOM_Matrix4x4f m;
					stream->read (&m, sizeof(ATOM_Matrix4x4f));
					attributes->setAttribute (s, m);
					break;
				}
			default:
				{
					break;
				}
			}
		}
	}
	return true;
}

bool ATOM_SharedModel::saveUserAttributes (const ATOM_UserAttributes *attributes, ATOM_ChunkStream *stream)
{
	unsigned num = attributes->getNumAttributes ();
	stream->writeInt (num);
	for (int i = 0; i < num; ++i)
	{
		stream->writeCString (attributes->getAttributeName (i));

		const ATOM_Variant &var = attributes->getAttributeValue (i);
		stream->writeInt (var.getType ());

		switch (var.getType())
		{
		case ATOM_Variant::INT:
			{
				stream->writeInt (var.getI());
				break;
			}
		case ATOM_Variant::INT_ARRAY:
			{
				stream->writeInt (var.getArraySize ());
				for (int j = 0; j < var.getArraySize (); ++j)
				{
					stream->writeInt (var.getIntArray ()[j]);
				}
				break;
			}
		case ATOM_Variant::FLOAT:
			{
				stream->writeFloat (var.getF());
				break;
			}
		case ATOM_Variant::FLOAT_ARRAY:
			{
				stream->writeInt (var.getArraySize ());
				for (int j = 0; j < var.getArraySize (); ++j)
				{
					stream->writeFloat (var.getFloatArray ()[j]);
				}
				break;
			}
		case ATOM_Variant::STRING:
			{
				stream->writeCString (var.getS ());
				break;
			}
		case ATOM_Variant::VECTOR4:
			{
				stream->write (var.getV(), sizeof(ATOM_Vector4f));
				break;
			}
		case ATOM_Variant::MATRIX44:
			{
				stream->write (var.getM(), sizeof(ATOM_Matrix4x4f));
				break;
			}
		default:
			{
				break;
			}
		}
	}
	return true;
}

ATOM_UserAttributes *ATOM_SharedModel::getAttributes (void) const
{
	return _userAttributes;
}

void ATOM_SharedModel::deleteMesh (int index)
{
	if (index < _meshes.size())
	{
		ATOM_DELETE(_meshes[index]);
		_meshes.erase(_meshes.begin() + index);
	}
}
