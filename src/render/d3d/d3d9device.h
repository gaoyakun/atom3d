#ifndef __ATOM_GLRENDER_ID3D9DEVICE_H
#define __ATOM_GLRENDER_ID3D9DEVICE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <ddraw.h>
#include "renderdevice.h"
#include "vertexattrib.h"

extern "C" const char*  WINAPI DXGetErrorStringA(HRESULT hr);
extern "C" const char*  WINAPI DXGetErrorDescriptionA(HRESULT hr);

#if !defined(ATOM_NO_D3D_ERROR_CHECK)
# define ATOM_CHECK_D3D_RESULT(hr) ::internal::verifyD3DResult(hr, __FUNCTION__)
#else
# define ATOM_CHECK_D3D_RESULT(hr) (hr)
#endif

namespace internal
{
	static inline const char *GetD3DResultString (HRESULT hr)
	{
#if ATOM3D_COMPILER_MSVC
		const char *str = DXGetErrorStringA (hr);
		if (!str)
		{
			str = "Unknown D3D error";
		}
		return str;
#else
		switch (hr)
		{
		case D3D_OK: return "D3D_OK";
		case D3DERR_WRONGTEXTUREFORMAT: return "D3DERR_WRONGTEXTUREFORMAT";
		case D3DERR_UNSUPPORTEDCOLOROPERATION: return "D3DERR_UNSUPPORTEDCOLOROPERATION";
		case D3DERR_UNSUPPORTEDCOLORARG: return "D3DERR_UNSUPPORTEDCOLORARG";
		case D3DERR_UNSUPPORTEDALPHAOPERATION: return "D3DERR_UNSUPPORTEDALPHAOPERATION";
		case D3DERR_UNSUPPORTEDALPHAARG: return "D3DERR_UNSUPPORTEDALPHAARG";
		case D3DERR_TOOMANYOPERATIONS: return "D3DERR_TOOMANYOPERATIONS";
		case D3DERR_CONFLICTINGTEXTUREFILTER: return "D3DERR_CONFLICTINGTEXTUREFILTER";
		case D3DERR_UNSUPPORTEDFACTORVALUE: return "D3DERR_UNSUPPORTEDFACTORVALUE";
		case D3DERR_CONFLICTINGRENDERSTATE: return "D3DERR_CONFLICTINGRENDERSTATE";
		case D3DERR_UNSUPPORTEDTEXTUREFILTER: return "D3DERR_UNSUPPORTEDTEXTUREFILTER";
		case D3DERR_CONFLICTINGTEXTUREPALETTE: return "D3DERR_CONFLICTINGTEXTUREPALETTE";
		case D3DERR_DRIVERINTERNALERROR: return "D3DERR_DRIVERINTERNALERROR";
		case D3DERR_NOTFOUND: return "D3DERR_NOTFOUND";
		case D3DERR_MOREDATA: return "D3DERR_MOREDATA";
		case D3DERR_DEVICELOST: return "D3DERR_DEVICELOST";
		case D3DERR_DEVICENOTRESET: return "D3DERR_DEVICENOTRESET";
		case D3DERR_NOTAVAILABLE: return "D3DERR_NOTAVAILABLE";
		case D3DERR_OUTOFVIDEOMEMORY: return "D3DERR_OUTOFVIDEOMEMORY";
		case D3DERR_INVALIDDEVICE: return "D3DERR_INVALIDDEVICE";
		case D3DERR_INVALIDCALL: return "D3DERR_INVALIDCALL";
		case D3DERR_DRIVERINVALIDCALL: return "D3DERR_DRIVERINVALIDCALL";
		case D3DERR_WASSTILLDRAWING: return "D3DERR_WASSTILLDRAWING";
		case D3DOK_NOAUTOGEN: return "D3DOK_NOAUTOGEN";
		default: return "Unknown D3D error";
		}
#endif
	}
	static inline HRESULT verifyD3DResult (HRESULT hr, const char *func)
	{
		static char errmsg[512];
		if (FAILED(hr))
		{
			_snprintf (errmsg, 256, "<%s> D3D ERROR: 0x%08X(%s)", func, hr, ::internal::GetD3DResultString(hr));
			ATOM_LOGGER::error (errmsg);
			ATOM_LOGGER::log ("Stacktrace:\n");
			ATOM_LOGGER::log ("%s", ATOM_GetStackTraceString());
		}
		return hr;
	}
}

#if defined(_DEBUG) && !defined(NDEBUG)

enum DBG_StateChangeType
{
	StateChange_Unknown = 0,
	StateChange_SRGB,
	StateChange_Texture,
	StateChange_TextureFilter,
	StateChange_TextureAddress,
	StateChange_SamplerBorderColor,
	StateChange_SamplerMaxLOD,
	StateChange_MaxAnisotropic,
	StateChange_MipmapLODBias,
	StateChange_AlphaBlending,
	StateChange_AlphaTest,
	StateChange_VertexShader,
	StateChange_PixelShader,
	StateChange_Depth,
	StateChange_Stencil,
	StateChange_Rasterizer,
	StateChange_Fog,
	StateChange_ColorWrite,
	StateChange_Multisample,

	StateChange_Count
};

struct DBG_StateChangeInfo
{
	int stateChangeCount[StateChange_Count];
};

#define DBG_MARK_STATE_CHANGE(info, type) info.stateChangeCount[type]++;
#define DBG_CLEAR_STATE_CHANGE(info) memset(&info, 0, sizeof(DBG_StateChangeInfo))

#else

#define DBG_MARK_STATE_CHANGE(info, type)
#define DBG_CLEAR_STATE_CHANGE(info)

#endif

class ATOM_D3DTexture;
class ATOM_StateCache;
class ATOM_D3D9Device : public ATOM_RenderDevice
{
	friend class ATOM_D3D9Window;
	friend class ATOM_D3DEffect;
	friend struct StreamSourceInfo;

private:
	struct SuperVertex
	{
		float x, y, z, w;
		float nx, ny, nz;
		unsigned color0;
		unsigned color1;
		float texcoords[ATOM_MAX_TEXTURE_LAYER_COUNT][3];
	};

public:
	ATOM_D3D9Device(void);
	virtual ~ATOM_D3D9Device(void);

public:
	// Initialize
	virtual bool setup (ATOM_DisplayMode *fullscreen_mode, const char *title, int w, int h, ATOM_RenderWindow::ShowMode showMode, bool naked, bool resizable, int multisample, void *windowid, void *parentid, void *iconid, bool contentBackup, void *instance);
	virtual bool initialize (ATOM_RenderWindow *window, int multisample);
	virtual bool getBackbufferSize (int *w, int *h) const;
	virtual void *getRenderContext (void) const;
	virtual ATOM_PixelFormat getBestHDRFormatRGBA (int *supportFiltering) const;
	virtual ATOM_PixelFormat getBestHDRFormatRG (int *supportFiltering) const;
	virtual ATOM_PixelFormat getBestHDRFormatR (int *supportFiltering) const;
	virtual bool isRenderTargetFormat (ATOM_PixelFormat format) const;
	virtual bool isDeviceReady (void) const;

public:
	//--- wangjian MODIFIED for Multithreaded ---//
	// 异步加载 ： 添加异步加载优先级(默认是异步加载）
	virtual ATOM_AUTOREF(ATOM_Texture) createTextureResource (	const char *filename, unsigned width, unsigned height, ATOM_PixelFormat format, 
																int loadPriority = ATOM_LoadPriority_ASYNCBASE	);
	//-------------------------------------------//
	virtual ATOM_AUTOREF(ATOM_Texture) findTextureResource (const char *filename);
	virtual ATOM_AUTOREF(ATOM_Texture) createTexture (const char *filename, unsigned width, unsigned height, ATOM_PixelFormat format);

public:
	virtual bool setLight (unsigned index, const ATOM_VertexLight *light);
	virtual bool enableLight (unsigned index, bool enable);
	virtual bool setAmbientColor (ATOM_ColorARGB color);
	virtual bool setMaterial (const ATOM_VertexLightMaterial *material);
	virtual bool enableVertexLighting (bool enable);

public:
	virtual bool supportRenderTarget (ATOM_PixelFormat format);
	virtual bool supportRenderTargetPostPixelShaderBlending (ATOM_PixelFormat format);
	virtual bool supportRenderTargetFiltering (ATOM_PixelFormat format);
	virtual void enableScissorTest (ATOM_RenderWindow *view, bool enable);
	virtual bool isScissorTestEnabled (ATOM_RenderWindow *view) const;
    virtual bool readColorBuffer(ATOM_RenderWindow *view, int x, int y, int w, int h, ATOM_PixelFormat format, void *pixels);
    virtual bool readDepthBuffer(ATOM_RenderWindow *view, int x, int y, int w, int h, void *pixels);
    virtual bool readStencilBuffer (ATOM_RenderWindow *view, int x, int y, int w, int h, void *pixels);
    virtual bool screenShot (ATOM_RenderWindow *view, ATOM_AUTOREF(ATOM_Image) pImage);
	virtual unsigned getAvailableTextureMem (void) const;
	virtual void setTextureQuality (ATOM_RenderDevice::TextureQuality quality);
	virtual ATOM_RenderDevice::TextureQuality getTextureQuality (void) const;
    virtual bool saveImageToFile (int w, int h, const void *rawdata, ATOM_PixelFormat format, const char *filename);

public:
	virtual bool setRenderTarget (int index, ATOM_Texture *texture, unsigned face = -1);
	//--- wangjian modified ---//
	// * 新增两个参数：用于通过 setRenderTargetSurface 函数设置RT之后，需要获取RT的表面大小时 *//
	virtual ATOM_Texture *getRenderTarget (int index, int * rt_width = 0, int * rt_height = 0) const;
	//-------------------------//
	virtual const char *getVendor (void) const;
	virtual const char *getRenderer (void) const;
	virtual void evictManagedResource (void);
	virtual bool beginFrame(void);
	virtual void endFrame(void);
	virtual bool setVertexDecl(ATOM_VertexDecl vd);
	virtual bool setTexture (int sampler, ATOM_Texture *texture);
	virtual bool setStreamSource(unsigned stream, ATOM_VertexArray* vertex_array, unsigned offset);
	virtual bool setStreamSourceFreq (unsigned stream, unsigned freq);
	virtual void renderStreams(int type, int prim_count);
	virtual void renderStreamsIndexed(ATOM_IndexArray* index_array, int type, int prim_count, int offset);
	virtual void renderInterleavedMemoryStreamsIndexed (int type, int vertex_count, int vertex_range, unsigned attrib, unsigned stride, const void *stream, const unsigned short *indices);
	virtual void setViewCursor (ATOM_RenderWindow *view);
	virtual void outputText (ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color, const char *text, ...);
	virtual void outputTextOutline (ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color, ATOM_ColorARGB colorOutline, const char *text, ...);
	virtual void outputTextShadow (ATOM_FontHandle font, int x, int y, int offsetx, int offsety, ATOM_ColorARGB color, ATOM_ColorARGB colorShadow, const char *text, ...);
	virtual bool setCustomCursor (ATOM_RenderWindow *view, ATOM_Image *image, int hotspot_x, int hotspot_y, const int *region);
	virtual bool setCustomCursor (ATOM_RenderWindow *view, ATOM_Texture *image, int hotspot_x, int hotspot_y, const int *region);
	virtual bool setCustomCursor (ATOM_RenderWindow *view, HCURSOR hCursor);
	virtual void setCursorPosition (int x, int y);
	virtual bool showCustomCursor (ATOM_RenderWindow *view, bool show);
	virtual unsigned allocOcclusionQuery (void);
	virtual void freeOcclusionQuery (unsigned query);
	virtual bool beginOcclusionQuery (unsigned query);
	virtual void endOcclusionQuery (void);
	virtual unsigned getOcclusionQueryResult (unsigned query);
	virtual bool pushRenderStates(void);
	virtual bool popRenderStates(void);
	virtual void loadDefaultRenderStates (void);
	virtual void *getInternalDevice (void) const;
	virtual bool setConstantsVS (unsigned startRegister, const void *data, unsigned regCount);
	virtual bool setConstantsPS (unsigned startRegister, const void *data, unsigned regCount);

public:
	virtual bool applyAlphaBlendState (ATOM_AlphaBlendAttributes *state, int pass, bool force);
	virtual bool applyColorWriteState (ATOM_ColorWriteAttributes *state, int pass, bool force);
	virtual bool applyDepthState (ATOM_DepthAttributes *state, int pass, bool force);
	virtual bool applyStencilState (ATOM_StencilAttributes *state, int pass, bool force);
	virtual bool applyRasterizerState (ATOM_RasterizerAttributes *state, int pass, bool force);
	virtual bool applyMultisampleState (ATOM_MultisampleAttributes *state, int pass, bool force);
	virtual bool applySamplerState (ATOM_SamplerAttributes *state, int index, int pass, bool force);
	virtual bool applyAlphaTestState (ATOM_AlphaTestAttributes *state, int pass, bool force);

public:
	virtual unsigned getNumDrawCalls (void) const;
	virtual unsigned getNumPrimitivesDrawn (void) const;
	virtual unsigned getNumTextures (void) const;
	virtual unsigned getFrameStamp (void) const;
    virtual void syncRenderStates(void);
	virtual bool resizeDevice (unsigned w, unsigned h);

public:
	virtual bool getImageFileInfoInMemory (const void *data, unsigned size, ATOM_ImageFileInfo *fileInfo) const;	    
	virtual void setConfigure(const char *entry, int value);

public:
	virtual ATOM_AUTOREF(ATOM_Shader) allocVertexShader (const char *name);
	virtual ATOM_AUTOREF(ATOM_Shader) allocPixelShader (const char *name);
	virtual ATOM_AUTOREF(ATOM_GPUProgram) allocGPUProgram (const char *name);
	virtual ATOM_AUTOREF(ATOM_Shader) lookupVertexShader (const char *name);
	virtual ATOM_AUTOREF(ATOM_Shader) lookupPixelShader (const char *name);
	virtual ATOM_AUTOREF(ATOM_GPUProgram) lookupGPUProgram (const char *name);
	virtual ATOM_VertexDecl createMultiStreamVertexDeclaration (unsigned attrib, unsigned attribFlags/* = 0*/);
	virtual ATOM_VertexDecl createMultiStreamVertexDeclarationEx (unsigned attrib0, ...);
	virtual ATOM_VertexDecl createInterleavedVertexDeclaration (unsigned attrib, unsigned attribInstancData, unsigned attribFlags/* = 0*/);
	virtual void destroyVertexDecl (ATOM_VertexDecl vd);
	virtual ATOM_AUTOREF(ATOM_DepthBuffer) allocDepthBuffer (unsigned width, unsigned height);
	virtual bool setDepthBuffer (ATOM_DepthBuffer *depthBuffer);
	virtual ATOM_DepthBuffer *getDepthBuffer (void) const;
	virtual ATOM_StateCache *getStateCache (void) const;

	//--- wangjian added ---//
	// * 新增API, 用以支持RESZ[暂时未用到]
	virtual void renderStreamsImmediateRESZ(ATOM_Texture * texture_resz);
	// * 新增API, 用以分配一个 Render Target Surface 资源
	// * 该API主要用于某些特殊使用情况，比如在shadow map渲染时，如果设备支持NRT，那么在使用NULL RENDER TARGET时需要直接生成
	// * RT surface,而非通过生成纹理得到RT surface.
	// * 在渲染到纹理时，直接绑定该RT的surface.
	virtual ATOM_AUTOREF(ATOM_RenderTargetSurface) allocRenderTargetSurface (unsigned width, unsigned height,ATOM_RenderTargetSurface::ATOM_RTSURFACE_FORMAT fmt);
	// * 新增API, 用以绑定一个 Render Target Surface
	// * 该API主要用于某些特殊使用情况，比如在shadow map渲染时，如果设备支持NRT，那么在使用NULL RENDER TARGET时需要直接生成
	// * RT surface,而非通过生成纹理得到RT surface.
	// * 在渲染到纹理时，直接绑定该RT的surface.
	// * 注意：在渲染到纹理之后通过 setRenderTargetSurface(index,NULL)重置RT.
	virtual bool setRenderTargetSurface (int index, ATOM_RenderTargetSurface *rts);

	virtual void applySlopeScaleBias(float fDepthBias = .0f, float fSlopeScaleBias = 0.0f );

	virtual void enableSRGBWrite(bool enable = false);
	//----------------------//

protected:
	virtual void _setClearColor_Impl (const float *rgba);
	virtual void _setClearDepth_Impl (float depth);
	virtual void _setClearStencil_Impl (int stencil);
	virtual void _clear_Impl (bool color, bool depth, bool stencil);
	virtual void _setModelViewMatrix_Impl (const ATOM_Matrix4x4f *world, const ATOM_Matrix4x4f *view);
	virtual void _setProjectionMatrix_Impl (const ATOM_Matrix4x4f *proj);
	virtual void _setTextureMatrix_Impl (int stage, const ATOM_Matrix4x4f *matrix);
	virtual void _setViewport_Impl (const int *xywh, float depthMin, float depthMax);
	virtual void _present_Impl (void);
	virtual void _setScissorRect_Impl (const int *scissor);
	virtual void _enableHardwareCursor_Impl (bool enable);

public:
    IDirect3DDevice9 *getD3DDevice(void) const;
    void setD3DDevice(IDirect3DDevice9 *device);
    const D3DPRESENT_PARAMETERS *getD3DPresentParams (void) const;
    IDirect3D9 *getD3D(void) const;
    void setD3D(IDirect3D9 *d3d);
    IDirect3DSurface9 *getBackBuffer (void);
    IDirect3DSurface9 *getDepthStencilBuffer (void);
    IDirect3DSurface9 *getDepthStencilSurface (void);
    struct ATOM_D3D9WindowInfo *getD3DWindowInfo (void) const;
    void setInitialStates (void);
    bool isTextureFormatOk (D3DFORMAT format) const;
	bool supportSRGBWrite (D3DFORMAT format) const;
    bool isRenderTargetFormatOk (D3DFORMAT format) const;
    bool isDepthStencilTextureFormatOk (D3DFORMAT format) const;
    bool isDepthStencilFormatOk (D3DFORMAT format) const;
	bool doesTextureFormatSupportSRGB (D3DFORMAT format) const;
    bool canAutoGenMipmap (D3DFORMAT format) const;
    void shutdown (void);
    bool testResetDevice(void);
	D3DFORMAT getDepthTextureFormat(void) const;

private:
	void invalidateAllResources (void);
	void restoreAllResources (void);
	void invalidateResources (void);
	void restoreResources (void);
	void adjustLights(void);
	void setClientStates(unsigned streams);
	unsigned requireIntermediateVertexBuffer (unsigned size);
	unsigned requireIntermediateIndexBuffer (unsigned size);
	IDirect3DVertexDeclaration9 *createImmediatelyVertexDeclaration (unsigned attrib, unsigned nlayers, const unsigned *cmps) const;
	ATOM_AUTOREF(ATOM_Image) cursorSurfaceToImage (IDirect3DSurface9 *surface);
	IDirect3DSurface9 *imageToCursorSurface (ATOM_Image *image, const int *region);
	IDirect3DSurface9 *textureToCursorSurface (ATOM_Texture *texture, const int *region, ATOM_AUTOREF(ATOM_Image) &image);
	void setupCaps (void);
	D3DFORMAT findBestHDRFormat (int *supportFiltering, D3DFORMAT format1, D3DFORMAT format2) const;
	bool getBackbufferDimension (int *w, int *h) const;
	D3DFORMAT determinDepthTextureFormat (void) const;

private:
	struct StateSetColorBuffer
	{
		DWORD alphaTestEnable;
		DWORD alphaFunc;
		DWORD alphaRef;
		DWORD alphaBlendEnable;
		DWORD ditherEnable;
		DWORD srcBlend;
		DWORD destBlend;
		DWORD blendColor;
		DWORD colorWriteEnable;
	};

	struct StateSetDepthBuffer
	{
		DWORD depthEnable;
		DWORD depthFunc;
		DWORD depthWrite;
	};

	struct StateSetStencilBuffer
	{
		DWORD stencilEnable;
		DWORD stencilFunc;
		DWORD stencilPass;
		DWORD stencilFail;
		DWORD stencilZFail;
		DWORD stencilMask;
		DWORD stencilWriteMask;
		DWORD stencilRef;
	};

	unsigned _M_current_clientstate_mask;
	unsigned _M_current_pointer_mask;
	bool _M_need_adjustlights;
	D3DPRESENT_PARAMETERS _M_d3dpp;
	IDirect3D9 *_M_d3d;
	IDirect3DDevice9 *_M_d3ddevice;
	IDirect3DVertexBuffer9 *_M_intermediate_vertexbuffer;
	IDirect3DIndexBuffer9 *_M_intermediate_indexbuffer;
	IDirect3DVertexDeclaration9 *_M_current_decl;
	IDirect3DSurface9 *_M_default_backbuffer;
	IDirect3DSurface9 *_M_default_depthbuffer;
	ATOM_AUTOREF(ATOM_Texture) _M_cursor_tmp_texture;
	ATOM_AUTOREF(ATOM_Texture) _M_current_rendertargets[4];
	//--- wangjian added ---//
	// * 新增成员变量
	// * 该成员变量主要用于某些特殊使用情况，在使用 setRenderTargetSurface 时用以保存当前的 RenderTargetSurface
	ATOM_AUTOREF(ATOM_RenderTargetSurface) _M_current_rtss[4];
	//----------------------//
	ATOM_AUTOREF(ATOM_DepthBuffer) _M_currentDepthBuffer;
	unsigned _M_intermediate_vertexbuffer_size;
	unsigned _M_intermediate_vertexbuffer_pos;
	unsigned _M_intermediate_indexbuffer_size;
	unsigned _M_intermediate_indexbuffer_pos;
	unsigned _M_num_vertices;
	bool _M_has_zbuffer;
	bool _M_has_stencilbuffer;
	bool _M_interleaved_stream;
	bool _M_device_ready;
	TextureQuality _M_texture_quality;
	ATOM_HASHMAP<unsigned, IDirect3DVertexDeclaration9*> _M_immediately_vertex_decl;
	ATOM_SET<ATOM_VertexDecl> _M_decls;
	ATOM_VECTOR<IDirect3DQuery9*> _M_occ_queries;
	unsigned _M_current_query;
	ATOM_StateCache *_stateCache;

	ATOM_PixelFormat _M_best_HDR_format;
	int _M_HDR_format_support_filtering;
	ATOM_PixelFormat _M_best_HDR_format_rg;
	int _M_HDR_format_rg_support_filtering;
	ATOM_PixelFormat _M_best_HDR_format_r;
	int _M_HDR_format_r_support_filtering;
	DWORD _M_taa_fourcc;
	unsigned _M_num_drawcalls;
	unsigned _M_num_primitives;
	unsigned _M_frame_stamp;
	int _M_beginframe_counter;
	ATOM_VECTOR<IDirect3DStateBlock9*> _M_statestack;

	int _backbufferW;
	int _backbufferH;
	D3DFORMAT _depthTextureFormat;

	int _statesetUpdateStamp;

	ATOM_VECTOR<StateSetColorBuffer> _M_colorbuffer_state_stack;
	ATOM_VECTOR<StateSetDepthBuffer> _M_depthbuffer_state_stack;
	ATOM_VECTOR<StateSetStencilBuffer> _M_stencilbuffer_state_stack;

#if defined(_DEBUG) && !defined(NDEBUG)
	DBG_StateChangeInfo _stateChangeInfo;
#endif
};

#endif // __ATOM_GLRENDER_ID3D9DEVICE_H
