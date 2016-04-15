#ifndef __ATOM_RENDER_RESOURCE_MNGR_H
#define __ATOM_RENDER_RESOURCE_MNGR_H

#if _MSC_VER > 1000
#pragma once
#endif

#include <d3d9.h>
#include <ddraw.h>
#include "basedefs.h"
#include "growablearray.h"

//--------------------------------------------------------------------------------------
// structures
//--------------------------------------------------------------------------------------
typedef struct _DDSURFACEDESC2_32BIT
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
} DDSURFACEDESC2_32BIT;

struct LOADER_DEVICE
{
	LPDIRECT3DDEVICE9 pDev9;
	LOADER_DEVICE() {}
	LOADER_DEVICE( IDirect3DDevice9* pDevice ) { pDev9 = pDevice; }
};

struct DEVICE_TEXTURE
{
	UINT Width;
	UINT Height;
	UINT MipLevels;
	UINT Format;
	IDirect3DTexture9 *pTexture9;
	UINT64 EstimatedSize;
	BOOL bInUse;
	UINT RecentUseCounter;
};

struct DEVICE_VERTEX_BUFFER
{
	UINT iSizeBytes;
	IDirect3DVertexBuffer9*  pVB9;
	BOOL bInUse;
	UINT RecentUseCounter;
};

struct DEVICE_INDEX_BUFFER
{
	UINT iSizeBytes;
	UINT ibFormat;
	IDirect3DIndexBuffer9*   pIB9;
	BOOL bInUse;
	UINT RecentUseCounter;
};

//--------------------------------------------------------------------------------------
// CResourceReuseCache class
//--------------------------------------------------------------------------------------
class CResourceReuseCache
{
private:
LOADER_DEVICE							m_Device;
CGrowableArray<DEVICE_TEXTURE*>			m_TextureList;
CGrowableArray<DEVICE_VERTEX_BUFFER*>	m_VBList;
CGrowableArray<DEVICE_INDEX_BUFFER*>	m_IBList;
UINT64									m_MaxManagedMemory;
UINT64									m_UsedManagedMemory;
BOOL									m_bSilent;
BOOL									m_bDontCreateResources;

int FindTexture( IDirect3DTexture9* pTex9 );
int EnsureFreeTexture( UINT Width, UINT Height, UINT MipLevels, UINT Format );
UINT64 GetEstimatedSize( UINT Width, UINT Height, UINT MipLevels, UINT Format );

int FindVB( IDirect3DVertexBuffer9* pVB );
int EnsureFreeVB( UINT iSizeBytes );

int FindIB( IDirect3DIndexBuffer9* pIB );
int EnsureFreeIB( UINT iSizeBytes, UINT ibFormat );

void DestroyTexture9( DEVICE_TEXTURE* pTex );
void DestroyVB9( DEVICE_VERTEX_BUFFER* pVB );
void DestroyIB9( DEVICE_INDEX_BUFFER* pVB );

public:
CResourceReuseCache( LPDIRECT3DDEVICE9 pDev );
~CResourceReuseCache();

// memory handling
void SetMaxManagedMemory( UINT64 MaxMemory );
UINT64 GetMaxManagedMemory();
UINT64 GetUsedManagedMemory();
void SetDontCreateResources( BOOL bDontCreateResources );
UINT64 DestroyLRUTexture();
UINT64 DestroyLRUVB();
UINT64 DestroyLRUIB();
void DestroyLRUResources( UINT64 SizeGainNeeded );

// texture functions
IDirect3DTexture9* GetFreeTexture9( UINT Width, UINT Height, UINT MipLevels, UINT Format );
void UnuseDeviceTexture9( IDirect3DTexture9* pTexture );
int GetNumTextures();
DEVICE_TEXTURE* GetTexture( int i );

// vertex buffer functions
IDirect3DVertexBuffer9* GetFreeVB9( UINT sizeBytes );
void UnuseDeviceVB9( IDirect3DVertexBuffer9* pVB );
int GetNumVBs();
DEVICE_VERTEX_BUFFER* GetVB( int i );

// index buffer functions
IDirect3DIndexBuffer9* GetFreeIB9( UINT sizeBytes, UINT ibFormat );
void UnuseDeviceIB9( IDirect3DIndexBuffer9* pVB );
int GetNumIBs();
DEVICE_INDEX_BUFFER* GetIB( int i );

void OnDestroy();

};

D3DFORMAT GetD3D9Format( DDPIXELFORMAT ddpf );
void GetSurfaceInfo( UINT width, UINT height, D3DFORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows );

#endif // __ATOM_RENDER_RESOURCE_MNGR_H
