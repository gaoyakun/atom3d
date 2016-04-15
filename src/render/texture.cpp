#include "stdafx.h"
#include "loadinterface.h"
#include "texturemanager.h"
#include "d3d9device.h"

typedef struct _DDS_HEADER
{
	DWORD dwSize;                 // size of the DDSURFACEDESC structure
	DWORD dwFlags;                // determines what fields are valid
	DWORD dwHeight;               // height of surface to be created
	DWORD dwWidth;                // width of input surface
	union
	{
		LONG lPitch;                 // distance to start of next line (return value only)
		DWORD dwLinearSize;           // Formless late-allocated optimized surface size
	}DUMMYUNIONNAMEN( 1 );
	union
	{
		DWORD dwBackBufferCount;      // number of back buffers requested
		DWORD dwDepth;                // the depth if this is a volume texture 
	}DUMMYUNIONNAMEN( 5 );
	union
	{
		DWORD dwMipMapCount;          // number of mip-map levels requestde
		// dwZBufferBitDepth removed, use ddpfPixelFormat one instead
		DWORD dwRefreshRate;          // refresh rate (used when display mode is described)
		DWORD dwSrcVBHandle;          // The source used in VB::Optimize
	}DUMMYUNIONNAMEN( 2 );
	DWORD       dwAlphaBitDepth;        // depth of alpha buffer requested
	DWORD       dwReserved;             // reserved
	DWORD       lpSurface32;            // this ptr isn't 64bit safe in the ddraw.h header
	union
	{
		DDCOLORKEY ddckCKDestOverlay;      // color key for destination overlay use
		DWORD dwEmptyFaceColor;       // Physical color for empty cubemap faces
	}DUMMYUNIONNAMEN( 3 );
	DDCOLORKEY  ddckCKDestBlt;          // color key for destination blt use
	DDCOLORKEY  ddckCKSrcOverlay;       // color key for source overlay use
	DDCOLORKEY  ddckCKSrcBlt;           // color key for source blt use
	union
	{
		DDPIXELFORMAT ddpfPixelFormat;        // pixel format description of the surface
		DWORD dwFVF;                  // vertex format description of vertex buffers
	}DUMMYUNIONNAMEN( 4 );
	DDSCAPS2    ddsCaps;                // direct draw surface capabilities
	DWORD       dwTextureStage;         // stage in multitexture cascade
} DDS_HEADER;

extern ATOM_RenderDevice *GetGlobalDevice (void);
extern void GetSurfaceInfo( UINT width, UINT height, int fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows );

inline int getWidth( const int width, const int mipMapLevel)
{
	int a = width >> mipMapLevel;
	return (a == 0)? 1 : a;
}
inline int getHeight( const int height, const int mipMapLevel) 
{
	int a = height >> mipMapLevel;
	return (a == 0)? 1 : a;
}
inline int getDepth( const int depth, const int mipMapLevel) 
{
	int a = depth >> mipMapLevel;
	return (a == 0)? 1 : a;
}
inline bool isCompressedFormat(const ATOM_PixelFormat format)
{
	return (format >= ATOM_PIXEL_FORMAT_DXT1) && (format <= ATOM_PIXEL_FORMAT_DXT5);
}
// Accepts only compressed formats
inline int getBytesPerBlock(const ATOM_PixelFormat format)
{
	return (format == ATOM_PIXEL_FORMAT_DXT1)? 8 : 16;
}

/*
ATOM_PIXEL_FORMAT_UNKNOWN = -1,

ATOM_PIXEL_FORMAT_START = 0,
ATOM_PIXEL_FORMAT_RGBA8888 = 0,
ATOM_PIXEL_FORMAT_RGBX8888,
ATOM_PIXEL_FORMAT_ARGB8888,
ATOM_PIXEL_FORMAT_XRGB8888,
ATOM_PIXEL_FORMAT_BGRA8888,
ATOM_PIXEL_FORMAT_BGRX8888,
ATOM_PIXEL_FORMAT_ABGR8888,
ATOM_PIXEL_FORMAT_XBGR8888,

ATOM_PIXEL_FORMAT_RGB888,
ATOM_PIXEL_FORMAT_BGR888,

ATOM_PIXEL_FORMAT_RGBA4444,
ATOM_PIXEL_FORMAT_RGBX4444,
ATOM_PIXEL_FORMAT_ARGB4444,
ATOM_PIXEL_FORMAT_XRGB4444,
ATOM_PIXEL_FORMAT_BGRA4444,
ATOM_PIXEL_FORMAT_BGRX4444,
ATOM_PIXEL_FORMAT_ABGR4444,
ATOM_PIXEL_FORMAT_XBGR4444,

ATOM_PIXEL_FORMAT_ARGB1555,
ATOM_PIXEL_FORMAT_XRGB1555,
ATOM_PIXEL_FORMAT_RGBA5551,
ATOM_PIXEL_FORMAT_RGBX5551,
ATOM_PIXEL_FORMAT_BGR565,

ATOM_PIXEL_FORMAT_A8,
ATOM_PIXEL_FORMAT_GREY8,
ATOM_PIXEL_FORMAT_GREY8A8,

ATOM_PIXEL_FORMAT_DEPTHSTENCIL,

ATOM_PIXEL_FORMAT_RGBA16,
ATOM_PIXEL_FORMAT_RGB16,
ATOM_PIXEL_FORMAT_A16,
ATOM_PIXEL_FORMAT_GREY16,
ATOM_PIXEL_FORMAT_GREY16A16,

ATOM_PIXEL_FORMAT_RGBA32F,
ATOM_PIXEL_FORMAT_R32F,
ATOM_PIXEL_FORMAT_RG32F,

ATOM_PIXEL_FORMAT_RGBA16F,
ATOM_PIXEL_FORMAT_R16F,
ATOM_PIXEL_FORMAT_RG16F,

ATOM_PIXEL_FORMAT_DXT1,
ATOM_PIXEL_FORMAT_DXT3,
ATOM_PIXEL_FORMAT_DXT5,
*/
// Does not accept compressed formats
// 不含压缩格式
inline int getBytesPerPixel(const ATOM_PixelFormat format)
{
	static const int bytesPP[] = 
	{
		0,									//  [ATOM_PIXEL_FORMAT_UNKNOWN]
		4, 4, 4, 4, 4, 4, 4, 4, 4,			//  [ATOM_PIXEL_FORMAT_START - ATOM_PIXEL_FORMAT_XBGR8888]
		3, 3,								//  [ATOM_PIXEL_FORMAT_RGB888 - ATOM_PIXEL_FORMAT_BGR888]
		2, 2, 2, 2, 2, 2, 2, 2,				//  [ATOM_PIXEL_FORMAT_RGBA4444 - ATOM_PIXEL_FORMAT_XBGR4444]
		2, 2, 2, 2, 2,						//  [ATOM_PIXEL_FORMAT_ARGB1555 - ATOM_PIXEL_FORMAT_BGR565]
		1, 1, 2,							//  [ATOM_PIXEL_FORMAT_A8 - ATOM_PIXEL_FORMAT_GREY8A8]
		0,									//	[ATOM_PIXEL_FORMAT_DEPTHSTENCIL : 暂时用0]
		8, 6, 2, 2,	4,						//  [ATOM_PIXEL_FORMAT_RGBA16 - ATOM_PIXEL_FORMAT_GREY16A16]
		16, 4, 8,							//  [ATOM_PIXEL_FORMAT_RGBA32F - ATOM_PIXEL_FORMAT_RG32F]
		8, 2, 4								//  [ATOM_PIXEL_FORMAT_RGBA16F - ATOM_PIXEL_FORMAT_RG16F]
	};
	return bytesPP[format];
}
int getMipMappedSize(	const int width, const int height, const int depth, 
						const int firstMipMapLevel, int nMipMapLevels, 
						ATOM_PixelFormat srcFormat	)
{
	int w = getWidth ( width, firstMipMapLevel );
	int h = getHeight( height, firstMipMapLevel );
	int d = getDepth ( depth, firstMipMapLevel );

	if (srcFormat == ATOM_PIXEL_FORMAT_UNKNOWN)
	{
		ATOM_LOGGER::error("");
		return -1;
	}

	int size = 0;
	while (nMipMapLevels)
	{
		if (isCompressedFormat(srcFormat))
		{
			size += ((w + 3) >> 2) * ((h + 3) >> 2) * d;
		} 
		else 
		{
			size += w * h * d;
		}

		w >>= 1;
		h >>= 1;
		d >>= 1;
		if (w + h + d == 0) break;
		if (w == 0) w = 1;
		if (h == 0) h = 1;
		if (d == 0) d = 1;

		nMipMapLevels--;
	}

	if (isCompressedFormat(srcFormat))
	{
		size *= getBytesPerBlock(srcFormat);
	} 
	else 
	{
		size *= getBytesPerPixel(srcFormat);
	}

	return (depth == 0)? 6 * size : size;
}
unsigned char * getPixels(	unsigned char *pixels, const int width, const int height, const int depth,
							const int mipMapLevel, const int mipMapCount,
							ATOM_PixelFormat srcFormat	)
{
	return (mipMapLevel < mipMapCount)? pixels + getMipMappedSize( width, height, depth, 0, mipMapLevel, srcFormat ) : 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--- wangjian added ---//
// 异步加载
ATOM_AsyncableLoadTexture::ATOM_AsyncableLoadTexture():
	m_width(0),
	m_height(0),
	m_depth(1),
	m_numMips(1),
	m_mipStart(0),
	m_dataPtrStart(0),
	m_format(ATOM_PIXEL_FORMAT_UNKNOWN),
	m_needResize(false),
	m_needRePow2(false)
{
	m_bExistInDisk = false;
}
ATOM_AsyncableLoadTexture::~ATOM_AsyncableLoadTexture()
{
	m_lockedRect.clear();
}
void ATOM_AsyncableLoadTexture::SetAttribute( ATOM_Texture * texture, int width,int height, ATOM_PixelFormat format, unsigned int flags, bool skipMipMap )
{
	if( texture && !host )
		host = texture;

	m_width = width;
	m_height = height;
	m_format = format;
	m_flags = flags;
	m_skipMipMap = skipMipMap || (texture->getFlags() & ATOM_Texture::NOMIPMAP) != 0;
}
void ATOM_AsyncableLoadTexture::GetAttribute( int & width, int & height, ATOM_PixelFormat & fmt, unsigned int & flags, bool & skipMipMap )
{
	width = m_width;
	height = m_height;
	fmt = m_format;
	flags = m_flags;
	skipMipMap = m_skipMipMap;
}
bool ATOM_AsyncableLoadTexture::Load()
{
	//return false;
	if( !host )
		return false;

	if( __super::Load() == false )
		return false;

	///////////////////////////////////////////////////
	if( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_INLOADING )
	{
		ATOM_LOGGER::error ("%s(%s) Async loading stage flag is not match.\n", __FUNCTION__, filename.c_str());
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNC_FAIL_ERR_STAGEFLAGMISMATCH;		// 失败原因：加载阶段标记不匹配
		return false;
	}

	//////////////////////////////////////////////////////

	// 打开文件读取
	ATOM_AutoFile fp(filename.c_str(), ATOM_VFS::read);
	if (!fp)
	{
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_FILEOPENFAILED;		// 失败原因：文件打开出错
		ATOM_LOGGER::error ("%s(%s)  Open file failed.\n", __FUNCTION__, filename.c_str());

#if 0
		// 使用一张棋盘黑白纹理取代之
		ATOM_AutoFile fp_checkerboard("/editor/textures/checkerboard.dds",ATOM_VFS::read);
		if( fp_checkerboard )
		{
			ATOM_LOGGER::error ("%s(%s) Use Checkerboard texture Instead.\n", __FUNCTION__, filename.c_str());
			fp = fp_checkerboard;
		}
		else
		{
			ATOM_LOGGER::error ("%s(%s) Checkerboard texture not found.\n", __FUNCTION__, filename.c_str());
			return false;
		}
#endif
		return false;
	}

	// 文件大小检查
	unsigned size = fp->size ();
	if (!size)
	{
		ATOM_LOGGER::error ("%s(%s) file size is too small.\n", __FUNCTION__, filename.c_str());

		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;				// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_FILESIZETOOSMALL;	// 失败原因：文件大小出错

		return false;
	}

	// 获得数据缓存指针 没有 则分配数据内存
	void* buffer = GetBuffer(size);
	ATOM_ASSERT(buffer!=0);
	if( buffer )
	{
		// 读取文件内容
		if( size != fp->read (buffer, size) )
		{
			ATOM_LOGGER::error ("%s(%s) file size is not correct.\n", __FUNCTION__, filename.c_str());

			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;			// 加载失败
			errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;	// 失败原因：读取内部错误

			GarbageCollect();

			return false;
		}
	}
	else
	{
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;			// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;	// 失败原因：读取内部错误

		ATOM_LOGGER::error ("%s(%s) memory allocation failed.\n", __FUNCTION__, filename.c_str());
		return false;
	}

	//======================================= new added ========================================================//

	/*if( strstr( filename.c_str(),"100/4.jpg") )
	{
		int i = 0;
	}*/

	ATOM_ImageFileInfo info;
	if (!GetGlobalDevice()->getImageFileInfoInMemory (buffer, size, &info))
	{
		result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;			// 加载失败
		errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;	// 失败原因：读取内部错误

		ATOM_LOGGER::error ("%s(%s) get image info failed.\n", __FUNCTION__, filename.c_str());
		return false;
	}

	if (GetGlobalDevice()->getCapabilities().texture_npo2_c && (!ATOM_ispo2(info.width) || !ATOM_ispo2(info.height)))
	{
		m_skipMipMap = true;
	}

	m_width		= info.width;
	m_height	= info.height;
	m_depth		= info.depth;
	m_numMips	= m_skipMipMap ? 1 : info.numMips;
	if (m_format == ATOM_PIXEL_FORMAT_UNKNOWN)
	{
		m_format = info.format;
	}
	m_imagetype = info.type;
	m_texturetype = info.textureType;

	switch (m_format)
	{
	case ATOM_PIXEL_FORMAT_RGBA8888:
		m_format = ATOM_PIXEL_FORMAT_BGRA8888;
		break;
	case ATOM_PIXEL_FORMAT_RGBX8888:
		m_format = ATOM_PIXEL_FORMAT_BGRX8888;
		break;
	case ATOM_PIXEL_FORMAT_RGB888:
		m_format = ATOM_PIXEL_FORMAT_BGR888;
		break;
	}

	//-----------------------------------------------------------------------------//
#if 1
	
	// 判断纹理大小是否大于设备支持的最大纹理
	int max_tex_size = GetGlobalDevice()->getCapabilities().max_texture_size;
	//max_tex_size = 2048;
	if( max_tex_size < m_width || max_tex_size < m_height )
	{
		ATOM_LOGGER::warning ("%s(%s) texture size [ width: %d | height: %d ] is larger than max texture size supported.\n", __FUNCTION__, filename.c_str(), m_width, m_height );
		
		m_needResize = true;

		int width_new	= ATOM_min2(m_width,max_tex_size);
		int height_new	= ATOM_min2(m_height,max_tex_size);
		
		if(m_width>max_tex_size)
			height_new = width_new * ( (float)m_height / m_width );
		else if(m_height>max_tex_size)
			width_new = height_new * ( (float)m_width / m_height );

		bool bSkipMip	= !ATOM_ispo2(width_new) || !ATOM_ispo2(height_new);

		if( !bSkipMip )
		{
			int size_org		= ATOM_min2(m_width,m_height);
			int size_new		= ATOM_min2(width_new,height_new);
			m_mipStart			= 0;
			int mipMapLevel		= 0;
			while( size_new != size_org )
			{
				size_org >>= 1;
				
				m_dataPtrStart += getMipMappedSize( m_width, m_height, m_depth, mipMapLevel, 1, m_format );
				m_mipStart++;
			}
		}

		m_width			= width_new;
		m_height		= height_new;
	}

#if 0
	// 判断纹理大小是否满足设备对于NONPOW2的条件.
	// 设备对于NONPOW2的支持可能是有条件的，这里不对条件做过多的判断，只要设备不满足无条件支持NONPOW2的条件，则视为不支持NONPOW2
	// 此时对纹理大小必须转换到满足条件的POW2大小
	bool bUnConditionalNonPower2 = GetGlobalDevice()->getCapabilities().texture_npo2;
	//bUnConditionalNonPower2 = false;
	if ( !bUnConditionalNonPower2 && ( !ATOM_ispo2(m_width) || !ATOM_ispo2(m_height) )	)
	{
		ATOM_LOGGER::warning (	"%s(%s) texture size [ width: %d | height: %d ] is NOT POWER OF 2\n", 
								__FUNCTION__, 
								filename.c_str(), 
								m_width, m_height );

		if( !ATOM_ispo2(m_width) )
		{
			m_width = (int)ATOM_nextpo2( (unsigned)m_width);
			/*if( m_width > 4 )
				m_width >>= 1;*/
			if( m_width > max_tex_size )
				m_width = max_tex_size;
		}
		if( !ATOM_ispo2(m_height) )
		{
			m_height = (int)ATOM_nextpo2( (unsigned)m_height);
			/*if( m_height > 4 )
			m_height >>= 1;*/
			if( m_height > max_tex_size )
				m_height = max_tex_size;
		}

		ATOM_LOGGER::warning (	"%s(%s) Resize to : [ width: %d | height: %d ]\n", 
								__FUNCTION__, 
								filename.c_str(), 
								m_width, m_height );

		ATOM_PixelFormat format = m_format;
		// 如果是DXT压缩格式 转换为ATOM_PIXEL_FORMAT_RGBA8888格式
		if( m_format >= ATOM_PIXEL_FORMAT_DXT1 && m_format <= ATOM_PIXEL_FORMAT_DXT5 )
			format = ATOM_PIXEL_FORMAT_RGBA8888;

		ATOM_RWops *input = ATOM_RWops::createMemRWops (buffer, size);
		ATOM_BaseImage image;
		bool succ = image.init (input, m_imagetype, format);
		ATOM_RWops::destroyRWops (input);
		if (!succ)
		{
			ATOM_LOGGER::error ("%s(%s) image initialize failed.\n", __FUNCTION__, filename.c_str());
			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// 创建失败
			errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// 失败原因：内部错误
			return false;
		}

		// 重新设置大小
		image.resize(m_width,m_height);
		// 如果是DXT压缩格式 转换为原格式
		if( m_format >= ATOM_PIXEL_FORMAT_DXT1 && m_format <= ATOM_PIXEL_FORMAT_DXT5 )
			image.convertFormat(m_format);

		// 重新设置数据块
		ATOM_FREE(buffer);
		data = 0;
		data_size = 0;
		size = image.getBufferSize();
		buffer = GetBuffer(size);
		memcpy(buffer,(void*)image.getData(),size);
		
		// 重设MIP级别
		m_numMips = image.getNumMipmaps();

		m_dataPtrStart = 0;
		m_needResize = false;
		m_needRePow2 = true;
	}
#endif

#endif
	//-----------------------------------------------------------------------------//

	/*if( m_imagetype==ATOM_ImageCodec::JPEG )
	{
		m_format = ATOM_PIXEL_FORMAT_BGR888;
	}*/

	// 当前CUBEMAP暂时只支持DDS格式
	if( m_imagetype == ATOM_ImageCodec::DDS && m_texturetype == ATOM_Texture::CUBEMAP )
	{
		//----------------------------------------------------------------------------------//
		// 检查CUBEMAP的大小是否超过设备支持的最大CUBEMAP的大小 如果是 返回FALSE
		if( GetGlobalDevice()->getCapabilities().max_texture_size_cubemap < m_width || 
			GetGlobalDevice()->getCapabilities().max_texture_size_cubemap < m_height )
		{
			result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_LOAD_FAILED;			// 加载失败
			errorno = ATOM_AsyncLoader::ATOM_ASYNCLOAD_FAIL_ERR_INTERNALERROR;	// 失败原因：读取内部错误

			ATOM_LOGGER::error ("%s(%s) cube texture size [ width: %d | height: %d ] is larger than max cube texture size supported.\n", __FUNCTION__, filename.c_str(), m_width, m_height);
			return false;
		}
		//----------------------------------------------------------------------------------//

		m_depth	= 0;

		unsigned char* tmpData = (unsigned char*)ATOM_MALLOC( size );
		memcpy(tmpData,buffer,size);

		DDS_HEADER * header = (DDS_HEADER *)( (unsigned char*)buffer + sizeof(unsigned) );
		int width  = header->dwWidth;
		int height = header->dwHeight;
		int depth  = (header->ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP)? 0 : (header->dwDepth == 0)? 1 : header->dwDepth;
		int nMipMaps = header->dwMipMapCount;

		unsigned char *texData_ptr = (unsigned char*)buffer + sizeof(unsigned) + sizeof(DDS_HEADER);
		unsigned char *tmpData_ptr = (unsigned char*)tmpData + sizeof(unsigned) + sizeof(DDS_HEADER);

		for (int face = 0; face < 6; face++)
		{
			for (int mipMapLevel = 0; mipMapLevel < m_numMips; mipMapLevel++)
			{
				int faceSize		= getMipMappedSize( m_width, m_height, m_depth, mipMapLevel, 1, m_format ) / 6;
				unsigned char *src	= getPixels( texData_ptr, m_width, m_height, m_depth, mipMapLevel, m_numMips, m_format ) + face * faceSize;

				memcpy( src, tmpData_ptr, faceSize);
				tmpData_ptr += faceSize;
			}
			/*if ((flags & DONT_LOAD_MIPMAPS) && header.dwMipMapCount > 1){
				fseek(file, getMipMappedSize(1, header.dwMipMapCount - 1) / 6, SEEK_CUR);
			}*/
		}

		ATOM_FREE(tmpData);
	}
	

	// 文件加载成功
	Done();

	return true;
}
bool ATOM_AsyncableLoadTexture::OnloadEnd()
{
	// 递减引用计数 如果为0 调用析构
	//ATOM_Texture * texture = m_texture.get();
	//m_texture = 0;
	//if( texture )
	if( host )
	{
		//ATOM_Texture * tex = (ATOM_Texture*)(host.get());
		ATOM_Texture * tex = (ATOM_Texture*)(host);
		ATOM_ASSERT(tex);
		if(tex)
		{
			// 重试总数5次
			int retry_count = 5;
			while( retry_count > 0 )
			{
				if( !tex->loadTexImageFromFile(0,0,0,ATOM_PIXEL_FORMAT_UNKNOWN) )
				{
					result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// 创建失败
					errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// 失败原因：内部错误

					ATOM_LOGGER::error ("%s(%s) texture create content failed.\n", __FUNCTION__, filename.c_str());
				}
				else
				{
					retry_count = 0;
				}

				--retry_count;
			}
			if( retry_count == 0 )
			{
				result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// 创建失败
				errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// 失败原因：内部错误

				ATOM_LOGGER::error ("%s(%s) retry count is exceeded 5, texture create content failed completely.\n", __FUNCTION__, filename.c_str());
			}
		}
	}

	return __super::OnloadEnd();
}
//=================================//
bool ATOM_AsyncableLoadTexture::Lock(bool & needRecurse)
{
	//return false;

	if( host )
	{
		//ATOM_Texture * tex = (ATOM_Texture*)(host.get());
		ATOM_Texture * tex = (ATOM_Texture*)(host);
		ATOM_ASSERT(tex);
		if(tex)
		{
#if 0
			if (m_texturetype != ATOM_Texture::TEXTURE2D)
			{
				if (!tex->loadTexImageFromFile (filename.c_str(), 0, 0, ATOM_PIXEL_FORMAT_UNKNOWN))
				{
					ATOM_LOGGER::warning("<%s>: the texture %s type is not 2D. \n",__FUNCTION__,filename.c_str());
					return false;
				}
			}
#else
			// 当前CUBEMAP只支持DDS格式
			if( m_imagetype == ATOM_ImageCodec::DDS && m_texturetype == ATOM_Texture::CUBEMAP )
			{
				// 如果是JPG格式或者PNG格式纹理
				// 自动生成MIPMAPS

				unsigned flag = ATOM_Texture::CUBEMAP;
				if( !m_skipMipMap && 
					( m_imagetype == ATOM_ImageCodec::JPEG || m_imagetype == ATOM_ImageCodec::PNG ) )
				{
					//flag |= ATOM_Texture::AUTOGENMIPMAP;
				}

				// 创建一张空纹理
				tex->setFlags(flag);
				if ( !tex->loadTexImageFromMemory ( 0, m_width, m_height, m_depth, m_format, m_numMips ) )
				{
					ATOM_LOGGER::error ("%s(%s) texture lock failed.\n", __FUNCTION__, filename.c_str());
					result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// 创建失败
					errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// 失败原因：内部错误
					return false;
				}

				unsigned numMips = tex->getNumLevels();
				if (m_numMips > numMips)
				{
					m_numMips = numMips;
				}
				numMips = m_skipMipMap ? 1 : m_numMips;
				m_lockedRect.resize ( numMips * 6 );		// 每个MIP级别都有6个FACE需要处理

				// 锁定每个MIP级别的每个FACE
				for (unsigned level = 0; level < numMips; ++level)
				{
					for( unsigned face = 0; face < 6; ++face )
					{
						if ( !tex->lock ( level, face, &m_lockedRect[ level * 6 + face ] ) )
						{
							ATOM_LOGGER::error ("<%s>: (%s type : %s) texture lock failed.\n", 
												__FUNCTION__, 
												filename.c_str(),
												( m_texturetype == ATOM_Texture::CUBEMAP ) ? "cubeMap" : "2D");
							result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// 创建失败
							errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// 失败原因：内部错误
							return false;
						}
					}
				}
				
			}
#endif
			// 2D纹理
			else if( m_texturetype == ATOM_Texture::TEXTURE2D )
			{
				// 如果是JPG格式或者PNG格式纹理
				// 自动生成MIPMAPS
				unsigned flag = ATOM_Texture::TEXTURE2D;
				if( !m_skipMipMap && 
					( m_imagetype == ATOM_ImageCodec::JPEG || m_imagetype == ATOM_ImageCodec::PNG ) )
				{
					//flag |= ATOM_Texture::AUTOGENMIPMAP;
				}
				tex->setFlags(flag);
				if (!tex->loadTexImageFromMemory (0, m_width, m_height, 1, m_format, m_numMips))
				{
					ATOM_LOGGER::error ("%s(%s) texture lock failed.\n", __FUNCTION__, filename.c_str());
					result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// 创建失败
					errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// 失败原因：内部错误
					return false;
				}

				unsigned numMips = tex->getNumLevels();
				if (m_numMips > numMips)
				{
					m_numMips = numMips;
				}
				m_lockedRect.resize (m_skipMipMap ? 1 : m_numMips);

				for (unsigned i = 0; i < m_lockedRect.size(); ++i)
				{
					if (!tex->lock (i, &m_lockedRect[i]))
					{
						ATOM_LOGGER::error ("%s(%s) texture lock failed.\n", __FUNCTION__, filename.c_str());
						result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// 创建失败
						errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// 失败原因：内部错误
						return false;
					}
				}
			}
		}

		return __super::Lock(needRecurse);
	}
	
	return false;
}

bool ATOM_AsyncableLoadTexture::Fill()
{
	//return false;

	if( host )
	{
		//ATOM_Texture * tex = (ATOM_Texture*)(host.get());
		ATOM_Texture * tex = (ATOM_Texture*)(host);
		ATOM_ASSERT(tex);
		if(tex)
		{
#if 0
			if (m_texturetype != ATOM_Texture::TEXTURE2D)
			{
				return __super::Fill();
			}
#endif

			/*if( strstr( filename.c_str(),"100/9.jpg") )
			{
				int i = 0;
			}
			if( strstr( filename.c_str(),"100/1.jpg") )
			{
				int i = 0;
			}*/

			unsigned _width = m_width,_height = m_height;
			unsigned numBytes, rowBytes, numRows;

			unsigned size = 0;
			void * data = GetBuffer(size);

			switch (m_imagetype)
			{
			case ATOM_ImageCodec::DDS:
				{
					unsigned char *texData = (unsigned char*)data + sizeof(unsigned) + sizeof(DDS_HEADER);

#if 1
					texData += m_needResize ? m_dataPtrStart : 0;
#endif

#if 0
					for (unsigned i = 0; i < m_lockedRect.size(); ++i)
					{
						GetSurfaceInfo (_width, _height, m_format, &numBytes, &rowBytes, &numRows);

						unsigned char *destBits = (unsigned char*)m_lockedRect[i].bits;

						for (unsigned h = 0; h < numRows; ++h)
						{
							memcpy (destBits, texData, rowBytes);
							destBits += m_lockedRect[i].pitch;
							texData += rowBytes;
						}

						_width >>= 1;
						_height >>= 1;

						if (_width == 0)
						{
							_width = 1;
						}

						if (_height == 0)
						{
							_height = 1;
						}
					}
#else

					unsigned char *src = 0;

					int mipMapCount = m_numMips;
					int mipMapLevel = 0;

					// 循环所有的MIP级别
					// 如果该级别存在数据
					while ( ( src = getPixels( texData, m_width, m_height, m_depth, mipMapLevel, mipMapCount, m_format ) ) != 0 )
					{
						int size = getMipMappedSize( m_width, m_height, m_depth, mipMapLevel, 1, m_format );

						/*if (img.is3D())
						{
							D3DLOCKED_BOX box;
							if (((LPDIRECT3DVOLUMETEXTURE9) tex.texture)->LockBox(mipMapLevel, &box, NULL, 0) == D3D_OK){
								memcpy(box.pBits, src, size);
								((LPDIRECT3DVOLUMETEXTURE9) tex.texture)->UnlockBox(mipMapLevel);
							}
						} 
						else*/

						// 如果是CUBEMAP 当前CUBEMAP暂时只支持DDS格式
						if( m_texturetype == ATOM_Texture::CUBEMAP )
						{
							size /= 6;
							for (int face = 0; face < 6; face++)
							{
								unsigned char *destBits = (unsigned char*)m_lockedRect[ mipMapLevel * 6 + face ].bits;
								memcpy( destBits, src, size);
								src += size;
							}
						} 
						else
						{
							unsigned char *destBits = (unsigned char*)m_lockedRect[ mipMapLevel ].bits;
							memcpy( destBits, src, size);
						}

						mipMapLevel++;
					}

#endif
					break;
				}
			case ATOM_ImageCodec::PNG:
			case ATOM_ImageCodec::JPEG:
				{


					
					ATOM_BaseImage image;
					if( m_needRePow2 )
					{
						 image.init (_width,_height,m_format,data,1);
					}
					else
					{
						ATOM_RWops *input = ATOM_RWops::createMemRWops (data, size);
						bool succ = image.init (input, m_imagetype, m_format);
						ATOM_RWops::destroyRWops (input);
						if (!succ)
						{
							ATOM_LOGGER::error ("%s(%s) texture fill failed.\n", __FUNCTION__, filename.c_str());
							result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// 创建失败
							errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// 失败原因：内部错误
							return false;
						}
					}

					// 如果需要重新设置大小
					if( m_needResize )
						image.resize(_width,_height);

					GetSurfaceInfo (_width, _height, m_format, &numBytes, &rowBytes, &numRows);
					unsigned char *src = image.getData();
					unsigned char *dest = (unsigned char*)m_lockedRect[0].bits;

					memcpy(dest, src, numBytes);
					/*for (unsigned h = 0; h < numRows; ++h)
					{
						memcpy (dest, src, rowBytes);
						dest += m_lockedRect[0].pitch;
						src += rowBytes;
					}*/

					break;
				}
			default:
				return false;
			}

			// 垃圾收集
			GarbageCollect();

			return __super::Fill();
		}
	}

	return false;
}
bool ATOM_AsyncableLoadTexture::Unlock()
{
	//return false;

	if( host )
	{
		//ATOM_Texture * tex = (ATOM_Texture*)(host.get());
		ATOM_Texture * tex = (ATOM_Texture*)(host);
		ATOM_ASSERT(tex);
		if(tex)
		{
#if 0
			if (m_texturetype != ATOM_Texture::TEXTURE2D)
			{
				return __super::Unlock();
			}
#endif

			/*if( strstr( filename.c_str(),"100/9.jpg") )
			{
				int i = 0;
			}*/
			
			// 当前CUBEMAP暂时只支持DDS格式
			if( m_imagetype == ATOM_ImageCodec::DDS && m_texturetype == ATOM_Texture::CUBEMAP )
			{
				for (unsigned level = 0; level < m_numMips; ++level)
				{
					for( unsigned face = 0; face < 6; ++face )
					{
						if( !tex->unlock ( level, face ) )
						{
							ATOM_LOGGER::error ("%s(%s) texture unlock failed.\n", __FUNCTION__, filename.c_str());
							result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// 创建失败
							errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// 失败原因：内部错误
							return false;
						}
					}
				}
			}
			else if( m_texturetype == ATOM_Texture::TEXTURE2D )
			{
				for (unsigned i = 0; i < m_lockedRect.size(); ++i)
				{
					if( !tex->unlock (i) )
					{
						ATOM_LOGGER::error ("%s(%s) texture unlock failed.\n", __FUNCTION__, filename.c_str());
						result = ATOM_AsyncLoader::ATOM_ASYNCLOAD_RES_CREATE_FAILED;			// 创建失败
						errorno = ATOM_AsyncLoader::ATOM_ASYNCCREATE_FAIL_ERR_INTERNALERROR;	// 失败原因：内部错误
						return false;
					}
				}
			}
			

			return __super::Unlock();
		}
	}

	return false;
}

void ATOM_AsyncableLoadTexture::setExistInDisk(bool bExist)
{
	m_bExistInDisk = bExist;
}
bool ATOM_AsyncableLoadTexture::isExistInDisk() const
{
	return m_bExistInDisk;
}

//---------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------//

ATOM_Texture::ATOM_Texture(void) {
  _M_flags = 0;
  _M_node = 0;
  _M_pixel_width = 0;
  _M_pixel_height = 0;
  _contentLost = false;
  _locked = 0;
  _asyncLoader = 0;
  _rt_face = -1;
}

ATOM_Texture::~ATOM_Texture (void)
{
	if( ATOM_AsyncLoader::isEnableLog() )
		ATOM_LOGGER::log("%s : %s [ Address : %x ] is Destroyed \n",	__FUNCTION__, 
																		getObjectName() ? getObjectName() : "",
																		_asyncLoader.get() ? (unsigned)(_asyncLoader.get()) : 0	);

	// 将host设置为0
	if( _asyncLoader.get() )
		_asyncLoader->host = 0;
	ATOM_AsyncLoader::OnAsyncLoadObjectFinished(_asyncLoader.get());
}

unsigned ATOM_Texture::getFlags (void) const 
{
  return _M_flags;
}

int ATOM_Texture::getWidth (void) const 
{
  return _M_pixel_width;
}

int ATOM_Texture::getHeight (void) const 
{
  return _M_pixel_height;
}

void ATOM_Texture::setFileName(int index, const char *filename)
{
  ATOM_ASSERT (index >= 0 && index < 6);
  ATOM_ASSERT (filename);
  _M_filenames[index] = filename;
}

const char *ATOM_Texture::getFileName(int index) const
{
  ATOM_ASSERT (index >= 0 && index < 6);
  return _M_filenames[index].empty() ? 0 : _M_filenames[index].c_str();
}

void ATOM_Texture::setRenderDevice (ATOM_RenderDevice *device)
{
	ATOM_GfxResource::setRenderDevice (device);
	if (_M_node && !device)
	{
		_M_node->manager->detachNode (_M_node);
		_M_node->manager->freeTextureNode (_M_node);
		_M_node = 0;
	}
}

ATOM_TextureLoadInterface *ATOM_Texture::getLoadInterface (void)
{
	if (!_loadInterface)
	{
		_loadInterface = ATOM_NEW(ATOM_TextureLoadInterface, this);
	}
	return _loadInterface.get();
}

//--- wangjian added ---//
ATOM_AsyncableLoadTexture * ATOM_Texture::getAsyncLoader()
{
	if (!_asyncLoader)
	{
		_asyncLoader = ATOM_NEW(ATOM_AsyncableLoadTexture);
		_asyncLoader->host = this;
	}
	return _asyncLoader.get();
}
void ATOM_Texture::setRenderTargetFace(int face/*=-1*/)
{
	_rt_face = face;
}
int	ATOM_Texture::getRenderTargetFace()
{
	return _rt_face;
}
//----------------------//

void ATOM_Texture::setFlags (unsigned flags) 
{
  _M_flags = flags;
}

void ATOM_Texture::setNode (ATOM_TextureNode *node) 
{
	_M_node = node;
}

ATOM_TextureNode *ATOM_Texture::getNode (void) const 
{
	return _M_node;
}

bool ATOM_Texture::isContentLost (void) const
{
	return _contentLost;
}

void ATOM_Texture::setContentLost (bool b)
{
	_contentLost = b;
}

bool ATOM_Texture::isLocked (void) const
{
	return _locked > 0;
}
