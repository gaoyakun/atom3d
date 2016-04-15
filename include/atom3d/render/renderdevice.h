/**	\file animatedtexturechannel.h
 *	��Ⱦ�豸�������.
 *
 *	\author ������
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_RENDER_IRENDERDEVICE_H
#define __ATOM_RENDER_IRENDERDEVICE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <stack>
#include "../ATOM_utils.h"
#include "../ATOM_kernel.h"
#include "../ATOM_image.h"
#include "../ATOM_ttfont.h"
#include "basedefs.h"
#include "gfxtypes.h"
#include "renderwindow.h"
#include "texture.h"
#include "depthbuffer.h"
#include "vertexarray.h"
#include "indexarray.h"
#include "shader.h"
#include "stateset.h"
#include "gpuprogram.h"
//--- wangjian added ---//
// ����һ��render target surface����Դ����
#include "rendertargetsurface.h"
//----------------------//

#define ATOM_STREAMSOURCE_INDEXEDDATA (1<<30)
#define ATOM_STREAMSOURCE_INSTANCEDATA (2<<30)

class ATOM_VBManager;
class ATOM_IBManager;
class ATOM_TextureManager;
class ATOM_CallbackChannel;
class ATOM_StateCache;

struct ATOM_ImageFileInfo
{
	ATOM_PixelFormat format;
	int type;
	int textureType;
	unsigned width;
	unsigned height;
	unsigned depth;
	unsigned numMips;
};

struct ATOM_RenderDeviceCaps
{
	bool  hardware_accel;
	bool  index_32bit;
	bool  fsaa;
	bool  blend_color;
	bool  mipmap_generation;
	bool  alpha_to_coverage;
	bool  anisotropic_filter;
	bool  hardware_instancing;
	bool  texture_stream_write;
	bool  texture_cubemap;
	bool  texture_sRGB_read;
	bool  framebuffer_sRGB_write;
	bool  texture_s3tc_compress;
	bool  texture_3d;
	bool  texture_depth;
	bool  texture_packed_depth;
	bool  texture_float32;
	bool  texture_float16;
	bool  texture_npo2;
	bool  texture_npo2_c;
	bool  rendertarget_float32;
	bool  rendertarget_float16;
	bool  shadowmap;
	bool  occlusion_query;
	bool  per_vertex_point_size;
	bool  vs_dynamic_branching;
	bool  ps_dynamic_branching;
	bool  support_sRGB_write;
	bool  mrt_independent_bit_depth;
	bool  mrt_independent_write_masks;
	bool  mrt_post_blending;
	//--- wangjian added for some special caps ---//
	bool  null_rtf;										// render target format : NULL
	bool  hardware_pcf;									// uss hardware pcf for shadow map( NV )
	bool  ati_fetch4;									// use fetch4 to enable hardware pcf for Shadow map( ATI )
	bool  declTypes_float16;							// support float 16 vertex decl
	//--------------------------------------------//
	int   max_clip_planes;
	int   max_texture_size;
	int   max_texture_size_3d;
	int   max_texture_size_cubemap;
	int   max_texture_unit_count;
	int	  max_sampler_count;
	int   max_rendertarget_count;
	float max_anisotropy;
	float max_point_size;
	int   max_vertex_program_uniforms;
	int   max_fragment_program_uniforms;
	int   major_version;
	int   minor_version;
	int   micro_version;
	float version;
	int	  shader_model;
	int	  shader_model_ext;

	// misc
	bool INTEL_DRIVER_BUG;
	bool ATI_SHADER_BUG;
	bool ATI_MIPMAP_BUG;

	//caps from graphic test
	struct TestCaps_t 
	{
		bool nv_shadowmap;
		bool ati_pixelmapping;
	};
	TestCaps_t testcaps;
};

struct ATOM_CursorInfo
{
	ATOM_AUTOREF(ATOM_Image) image;
	bool show;
	int hotspot[2];
	int region[4];
	void *userdata;
	void *hcursor;
	ATOM_CursorInfo(void);
};

struct ATOM_RenderView
{
	ATOM_AUTOREF(ATOM_RenderWindow) window;
	float clearcolor[4];
	float cleardepth;
	int clearstencil;
	bool clearcolor_dirty;
	bool cleardepth_dirty;
	bool clearstencil_dirty;
	int viewport[4];
	float depthMin;
	float depthMax;
	int scissor[4];
	int scissor2[4];
	bool enable_scissor_test;
	ATOM_CursorInfo cursor_info;

	ATOM_RenderView (void)
	{
		clearcolor[0] = 0.f;
		clearcolor[1] = 0.f;
		clearcolor[2] = 0.f;
		clearcolor[3] = 0.f;
		clearcolor_dirty = true;
		cleardepth = 1.f;
		cleardepth_dirty = true;
		clearstencil = 0;
		clearstencil_dirty = true;
		viewport[0] = 0;
		viewport[1] = 0;
		viewport[2] = 0;
		viewport[3] = 0;
		depthMin = 0.f;
		depthMax = 1.f;
		scissor[0] = 0;
		scissor[1] = 0;
		scissor[2] = 0;
		scissor[3] = 0;
		enable_scissor_test = false;
	}
};

struct ATOM_VertexLight
{
	enum Type
	{
		VLT_DIRECTIONAL,
		VLT_POINT,
		VLT_SPOT
	};

	Type type;
	ATOM_Vector4f diffuse;
	ATOM_Vector4f specular;
	ATOM_Vector4f ambient;
	ATOM_Vector3f position;
	ATOM_Vector3f direction;
	float range;
	float falloff;
	float attenuation0;
	float attenuation1;
	float attenuation2;
	float theta;
	float phi;
};

struct ATOM_VertexLightMaterial
{
	ATOM_Vector4f diffuse;
	ATOM_Vector4f ambient;
	ATOM_Vector4f specular;
	ATOM_Vector4f emissive;
	float power;
};

struct ATOM_DisplayMode
{
	int width;
	int height;
	int depth;
	int freq;
};

typedef void * ATOM_VertexDecl;

//! \class ATOM_RenderDevice
//! ��Ⱦ�豸��
//! \todo ����
//! \todo ���ƶ���Ⱦ���ڵ�֧��
//! \author ������
class ATOM_RenderDevice
{
public:
	//! �����������
	//! \sa ATOM_RenderDevice::setTextureQuality
	enum TextureQuality
	{
		//! �رո������Թ���
		TQ_LOW,
		//! 4���������Թ���
		TQ_MEDIUM,
		//! 8���������Թ���
		TQ_HIGH,
		//! 16���������Թ���(���֧��)
		TQ_ULTRA_HIGH
	};

	enum 
	{
		STATESET_COLOR_BUFFER = (1<<0),
		STATESET_DEPTH_BUFFER = (1<<1),
		STATESET_STENCIL_BUFFER = (1<<2),
	};

	//! ���캯��
	ATOM_RenderDevice(void);

	//! ��������
	virtual ~ATOM_RenderDevice(void);

public:
	//! ��ȡ��Ⱦ����
	//! \return ��Ⱦ����
    virtual ATOM_AUTOREF(ATOM_RenderWindow) getWindow (void) const;

	//! ��ȡ��ǰ��Ⱦ���� 
	//! \return ��ǰ��Ⱦ����
    virtual ATOM_RenderWindow *getCurrentView (void);

	//! ��ȡ��ǰ���ָ����Ϣ
	//! \return ��ǰ���ָ����Ϣ
    virtual ATOM_CursorInfo *getCurrentCursorInfo (void);

public:
	//! ��ʼ����Ⱦ�豸�ʹ���
	//! \todo �ӿ��������
	//! \param fullscreen_mode ȫ��ģʽ����������Ǵ���ģʽ�������Ӧ��ΪNULL
	//! \param title ���ڱ���
	//! \param w ���ڿ�ȣ����fullscreen_mode��windowid��ΪNULL�������û��
	//! \param h ���ڿ�ȣ����fullscreen_mode��windowid��ΪNULL�������û��
	//! \param show ���Ϊtrue�򴴽����ں�������ʾ������Ϊ���ش���
	//! \param naked ���Ϊtrue�򴰿ڰ����߿�ͱ����������fullscreen_mode��windowid��ΪNULL�������û��
	//! \param resizable ���Ϊtrue���ڿ��Ըı��С�����fullscreen_mode��windowid��ΪNULL�������û��
	//! \param multisample ȫ��Ļ���ܳݵı���(FSAA)
	//! \param windowid ָ��һ�����д��ھ������windows����ϵͳ�����������ָ��HWND��ָ�롣������������ΪNULL�������������д�����Ⱦ�豸
	//! \param iconid ����ͼ����
	//! \return true �ɹ� false ʧ��
	virtual bool setup (ATOM_DisplayMode *fullscreen_mode, const char *title, int w, int h, ATOM_RenderWindow::ShowMode showMode, bool naked, bool resizable, int multisample, void *windowid, void *parentid, void *iconid, bool contentBackup, void *instance);

	//! ��һ�������ϴ�����Ⱦ�豸.
	//! �������Ϊ�ڲ�ʹ��
	//! \todo �������˽ӿ�
	//! \param window ����
	//! \param multisample ȫ��Ļ���ܳݵı���(FSAA)
	//! \return true �ɹ� false ʧ��
	virtual bool initialize (ATOM_RenderWindow *window, int multisample) = 0;

	//! \return true ֧�� false ��֧��
	virtual bool isMultisampleSupported (void);

	//! \return ֧�� false ֧��
	virtual bool isSampleSupported (unsigned sample);

	//! ���һ����Ⱦ����.
	//! ����֧�ֶ����Ⱦ���ڣ�ͨ��������������һ����Ⱦ���ڵ��豸
	//! \param window ��Ⱦ����
	//! \return true �ɹ� false ʧ��
	//! \sa removeView setCurrentView
	virtual bool addView (ATOM_RenderWindow *window);

	//! ɾ��һ����Ⱦ����
	//! \param window Ҫɾ���Ĵ���
	//! \sa addView setCurrentView
	virtual void removeView (ATOM_RenderWindow *window);

	//! ���õ�ǰ��Ⱦ����.
	//! \param window ��Ⱦ���ڣ����ô˺������������Ⱦ���ý��Դ˴���Ϊ��ȾĿ��
	virtual void setCurrentView (ATOM_RenderWindow *window);

	//! ��ȡ�󱸻������Ĵ�С
	//! \param w ������
	//! \param h ����߶�
	//! \return true �ɹ� false ʧ��
	virtual bool getBackbufferSize (int *w, int *h) const = 0;

	//! ��ȡ�豸ָ��.
	//! ��Windows����ϵͳ����Direct3D�豸ָ��
	//! \todo �������Ʋ�ȷ��
	//! \return �豸ָ��
	virtual void *getRenderContext (void) const = 0;

	//! ��ȡ���豸֧�ֵ���ѵľ���RGBA�ĸ�ͨ����HDR���ظ�ʽ
	//! \param supportFiltering ������óɹ�����������������ֵ��ʾ�����ظ�ʽ֧��Ӳ�������Թ��ˣ�����֧��Ӳ�������Թ���
	//! \return ���ظ�ʽ��������PIXEL_FORMAT_RGBA32F����PIXEL_FORMAT_RGBA16F������豸��֧�ָ�����ͼ����ATOM_PIXEL_FORMAT_UNKNOWN
	virtual ATOM_PixelFormat getBestHDRFormatRGBA (int *supportFiltering) const = 0;

	//! ��ȡ���豸֧�ֵ���ѵľ���RG����ͨ����HDR���ظ�ʽ
	//! \param supportFiltering ������óɹ�����������������ֵ��ʾ�����ظ�ʽ֧��Ӳ�������Թ��ˣ�����֧��Ӳ�������Թ���
	//! \return ���ظ�ʽ��������PIXEL_FORMAT_RG32F����PIXEL_FORMAT_RG16F������豸��֧�ָ�����ͼ����ATOM_PIXEL_FORMAT_UNKNOWN
	virtual ATOM_PixelFormat getBestHDRFormatRG (int *supportFiltering) const = 0;

	//! ��ȡ���豸֧�ֵ���ѵľ���Rͨ����HDR���ظ�ʽ
	//! \param supportFiltering ������óɹ�����������������ֵ��ʾ�����ظ�ʽ֧��Ӳ�������Թ��ˣ�����֧��Ӳ�������Թ���
	//! \return ���ظ�ʽ��������PIXEL_FORMAT_R32F����PIXEL_FORMAT_R16F������豸��֧�ָ�����ͼ����ATOM_PIXEL_FORMAT_UNKNOWN
	virtual ATOM_PixelFormat getBestHDRFormatR (int *supportFiltering) const = 0;

	//! ���һ�����ظ�ʽ�Ƿ������ΪRenderTarget
	//! \param format Ҫ���ĸ�ʽ
	//! \return true���� false������
	virtual bool isRenderTargetFormat (ATOM_PixelFormat format) const = 0;

	//! ��ѯ�豸�Ƿ����
	//! ������DeviceLost״̬��Ϊ�Ǿ���״̬
	virtual bool isDeviceReady (void) const = 0;

	virtual ATOM_StateCache *getStateCache (void) const = 0;

public:
	//! ����ĳ����Ⱦ���ڵĺ󱸻��������ɫ
	//! \param view ��Ⱦ����
	//! \param r ���ɫ��ɫ��������Χ0��1
	//! \param g ���ɫ��ɫ��������Χ0��1
	//! \param b ���ɫ��ɫ��������Χ0��1
	//! \param a ���ɫAlpha��������Χ0��1
	virtual void setClearColor(ATOM_RenderWindow *view, float r, float g, float b, float a);

	//! ��ȡĳ����Ⱦ���ڵĺ󱸻��������ɫ
	//! \param view ��Ⱦ����
	//! \param r ���ɫ��ɫ��������Χ0��1
	//! \param g ���ɫ��ɫ��������Χ0��1
	//! \param b ���ɫ��ɫ��������Χ0��1
	//! \param a ���ɫAlpha��������Χ0��1
	virtual void getClearColor(ATOM_RenderWindow *view, float &r, float &g, float &b, float &a) const;

	//! ��ȡĳ����Ⱦ���ڵĺ󱸻��������ɫ
	//! \param view ��Ⱦ����
	//! \return ���ɫ
	virtual ATOM_ColorARGB getClearColor(ATOM_RenderWindow *view) const;

	//! ����ĳ����Ⱦ���ڵĺ󱸻��������ɫ
	//! \param view ��Ⱦ����
	//! \param color ARGB��ɫֵ
	virtual void setClearColor(ATOM_RenderWindow *view, ATOM_ColorARGB color);

	//! ����ĳ����Ⱦ���ڵ���Ȼ��������ֵ
	//! \param view ��Ⱦ����
	//! \param d ���ֵ����Χ0��1
	virtual void setClearDepth(ATOM_RenderWindow *view, float d);

	//! ����ĳ����Ⱦ���ڵ�ģ�建�������ֵ
	//! \param view ��Ⱦ����
	//! \param s ���ֵ����Χ0��255
	virtual void setClearStencil(ATOM_RenderWindow *view, int s);

	//! ����ĳ����Ⱦ���ڵ��ӿڷ�Χ
	//! \param view ��Ⱦ����
	//! \param x �ӿ����Ͻ�X����
	//! \param y �ӿ����Ͻ�Y����
	//! \param w �ӿڿ��
	//! \param h �ӿڸ߶�
	//! \param depthMin ��С�����ֵ
	//! \param depthMax �������ֵ
	//! \sa getViewport
	virtual void setViewport(ATOM_RenderWindow *view, int x, int y, int w, int h, float depthMin = 0.f, float depthMax = 1.0f);

	//! ����ĳ����Ⱦ���ڵ��ӿڷ�Χ
	//! \param view ��Ⱦ����
	//! \param viewport �ӿڷ�Χ
	//! \param depthMin ��С�����ֵ
	//! \param depthMax �������ֵ
	//! \sa getViewport
	virtual void setViewport(ATOM_RenderWindow *view, const ATOM_Rect2Di &viewport, float depthMin = 0.f, float depthMax = 1.0f);

	//! ��ȡĳ����Ⱦ���ڵ��ӿڷ�Χ
	//! \param view ��Ⱦ����
	//! \param x �ӿ����Ͻ�X����
	//! \param y �ӿ����Ͻ�Y����
	//! \param w �ӿڿ��
	//! \param h �ӿڸ߶�
	//! \param depthMin ��С�����ֵ
	//! \param depthMax �������ֵ
	//! \sa setViewport
	virtual void getViewport(ATOM_RenderWindow *view, int* x, int* y, int* w, int* h, float *depthMin = 0, float *depthMax = 0) const;

	//! ��ȡĳ����Ⱦ���ڵ��ӿڷ�Χ
	//! \param view ��Ⱦ����
	//! \param depthMin ��С�����ֵ
	//! \param depthMax �������ֵ
	//! \return �ӿڷ�Χ
	//! \sa setViewport
	virtual ATOM_Rect2Di getViewport(ATOM_RenderWindow *view, float *depthMin = 0, float *depthMax = 0) const;

	//! �����ӿڼ�������
	//! \param view ��Ⱦ����
	//! \param x �������Ͻ�X����
	//! \param y �������Ͻ�Y����
	//! \param w ������
	//! \param h ����߶�
	virtual void setScissorRect (ATOM_RenderWindow *view, int x, int y, int w, int h);

	//! �����ӿڼ�������
	//! \param view ��Ⱦ����
	//! \param rc ����Χ
	virtual void setScissorRect (ATOM_RenderWindow *view, const ATOM_Rect2Di &rc);

	//! ��ȡ��ǰ�ӿڼ��÷�Χ
	//! \param view ��Ⱦ����
	//! \param x �������Ͻ�X����
	//! \param y �������Ͻ�Y����
	//! \param w ������
	//! \param h ����߶�
	virtual void getScissorRect (ATOM_RenderWindow *view, int *x, int *y, int *w, int *h) const;

	//! ��ȡ��ǰ�ӿڼ��÷�Χ
	//! \param view ��Ⱦ����
	//! \return ��������
	virtual ATOM_Rect2Di getScissorRect (ATOM_RenderWindow *view) const;

	//! �����Ƿ������ӿڼ���
	//! \param view ��Ⱦ����
	//! \param enable true��ʾ����false��ʾ������
	virtual void enableScissorTest (ATOM_RenderWindow *view, bool enable) = 0;

	//! ��ѯ�ӿڼ����Ƿ��
	//! \return true �� false δ��
	//! \sa enableScissorTest setScissorRect getScissorRect
	virtual bool isScissorTestEnabled (ATOM_RenderWindow *view) const = 0;

	//! ��ȡĳ����Ⱦ���ڵ���ɫ����������
	//! \param view ��Ⱦ����
	//! \param x ��ȡ��Χ���Ͻ�X����
	//! \param y ��ȡ��Χ���Ͻ�Y����
	//! \param w ��ȡ��Χ�Ŀ��
	//! \param h ��ȡ��Χ�ĸ߶�
	//! \param format ��ȡ�����ظ�ʽ
	//! \param pixels ��ȡ����������������㹻�Ĵ�С
	//! \return true �ɹ� false ʧ��
	virtual bool readColorBuffer(ATOM_RenderWindow *view, int x, int y, int w, int h, ATOM_PixelFormat format, void *pixels) = 0;

	//! ��ȡĳ����Ⱦ���ڵ���Ȼ���������
	//! \todo ��D3D�иú����޷�����
	//! \param view ��Ⱦ����
	//! \param x ��ȡ��Χ���Ͻ�X����
	//! \param y ��ȡ��Χ���Ͻ�Y����
	//! \param w ��ȡ��Χ�Ŀ��
	//! \param h ��ȡ��Χ�ĸ߶�
	//! \param pixels ��ȡ������
	//! \return true �ɹ� false ʧ��
	virtual bool readDepthBuffer(ATOM_RenderWindow *view, int x, int y, int w, int h, void *pixels) = 0;

	//! ��ȡĳ����Ⱦ���ڵ�ģ�建��������
	//! \todo ��D3D�иú����޷�����
	//! \param view ��Ⱦ����
	//! \param x ��ȡ��Χ���Ͻ�X����
	//! \param y ��ȡ��Χ���Ͻ�Y����
	//! \param w ��ȡ��Χ�Ŀ��
	//! \param h ��ȡ��Χ�ĸ߶�
	//! \param pixels ��ȡ������
	//! \return true �ɹ� false ʧ��
	virtual bool readStencilBuffer (ATOM_RenderWindow *view, int x, int y, int w, int h, void *pixels) = 0;

	//! ��ȡĳ����Ⱦ���ڵĺ���ɫ���������ݲ����浽ͼ����
	//! \param view ��Ⱦ����
	//! \param path �����ļ�·��
	//! \return true �ɹ� false ʧ��
	virtual bool screenShot (ATOM_RenderWindow *view, const char *path = NULL);

	//! ��ȡĳ����Ⱦ���ڵĺ���ɫ���������ݲ����浽ATOM_Image��
	//! \param view ��Ⱦ����
	//! \param image ATOM_Image��ָ��
	//! \return true �ɹ� false ʧ��
	virtual bool screenShot (ATOM_RenderWindow *view, ATOM_AUTOREF(ATOM_Image) image) = 0;

	//! ��ȡ���õ������ڴ�����
	//! \return ���������ڴ��ֽ���
	//! \note �ú������ص�ֵ������ȷ
	virtual unsigned getAvailableTextureMem (void) const = 0;

	//! ��������������Թ�������
	//! \param quality ���˲���
	//! \sa ATOM_RenderDevice::TextureQuality ATOM_RenderDevice::getTextureQuality
	virtual void setTextureQuality (ATOM_RenderDevice::TextureQuality quality) = 0;

	//! ��ȡ��ǰ����������Թ�������
	//! \return ��������
	//! \sa ATOM_RenderDevice::TextureQuality ATOM_RenderDevice::setTextureQuality
	virtual ATOM_RenderDevice::TextureQuality getTextureQuality (void) const = 0;

	//! ���ö����Դ
	//! \param index �ƹ�����
	//! \param light �ƹ�ṹ
	//! \return true �ɹ� false ʧ��
	virtual bool setLight (unsigned index, const ATOM_VertexLight *light) = 0;

	//! ��/�ر�һ�������Դ
	//! \param index �ƹ�����
	//! \param enable true�� false�ر�
	//! \return true �ɹ� false ʧ��
	virtual bool enableLight (unsigned index, bool enable) = 0;

	//! ����ȫ�ֻ�����
	//! \param color ȫ�ֻ�������ɫ
	//! \return true �ɹ� false ʧ��
	virtual bool setAmbientColor (ATOM_ColorARGB color) = 0;

	//! ���õ�ǰ��������
	//! \param material ���ʽṹ
	//! \return true �ɹ� false ʧ��
	virtual bool setMaterial (const ATOM_VertexLightMaterial *material) = 0;

	//! ��/�رն������
	//! \param enable true�� false�ر�
	//! \return true �ɹ� false ʧ��
	virtual bool enableVertexLighting (bool enable) = 0;

public:
	//! ���ö���ܵ��ı任����
	//! \param mode �任ģʽ
	//! \param matrix ����
	virtual void setTransform(int mode, const ATOM_Matrix4x4f &matrix);

	//! ��ȡ����ܵ��ı任����
	//! \param mode �任ģʽ
	//! \param matrix ����任����
	virtual void getTransform(int mode, ATOM_Matrix4x4f &matrix);

	//! ��������任����
	//! \param stage ��ͼ�����
	//! \param matrix ����
	virtual void setTextureTransform(int stage, const ATOM_Matrix4x4f &matrix);

	//! ��ȡ����任����
	//! \param stage ��ͼ�����
	//! \param matrix ����
	virtual void getTextureTransform(int stage, ATOM_Matrix4x4f &matrix);

	//! ��ȡ�����̱�ʾ
	//! \return ��ʾ
	//! \todo ûʲô�ã�ȥ��
	virtual const char *getVendor (void) const = 0;

	//! ��ȡRenderer
	//! \todo ûʲô�ã�ȥ��
	virtual const char *getRenderer (void) const = 0;

	//! ��ȡ�豸��Ⱦ��������
	//! \return �豸��Ⱦ���������ṹ
	virtual const ATOM_RenderDeviceCaps & getCapabilities (void) const;

	//! �������豸�������Դʹ�õ���ʾ�ڴ�ȫ���ͷ�
	virtual void evictManagedResource (void);

	//! ��ʼһ֡�µ���Ⱦ
	//! \return true�ɹ� falseʧ��
	//! \sa endFrame
	virtual bool beginFrame(void);

	//! ������ǰ֡��Ⱦ
	//! \sa beginFrame
	//! \note ���beginFrame����false���������
	virtual void endFrame(void);

	//! ����󱸻�����
	//! \param color true�����ɫ������ false�������ɫ������
	//! \param depth true�����Ȼ����� false�������Ȼ�����
	//! \param stencil true���ģ�建���� false�����ģ�建����
	virtual void clear(bool color, bool depth, bool stencil);

	//! ���õ�ǰ��Ⱦ�������Ķ���
	//! \param vd ����������
	//! \return true�ɹ� false���ɹ�
	virtual bool setVertexDecl(ATOM_VertexDecl vd) = 0;

	//! ������ͼ
	//! \param sampler ����������
	//! \param texture ��ͼָ��
	//! \return true�ɹ� false���ɹ�
	virtual bool setTexture (int sampler, ATOM_Texture *texture) = 0;

	//! ���ö�����
	//! \param stream �����
	//! \param vertex_array ��������
	//! \param offset ��������Զ���������ʼλ�õ��ֽ�ƫ����
	//! \return true�ɹ� false���ɹ�
	virtual bool setStreamSource(unsigned stream, ATOM_VertexArray* vertex_array, unsigned offset = 0) = 0;

	//! ���ö��������ظ�ʹ�÷�ʽ.
	//! ��Ҫ����ʵ������Ⱦ
	//! \param stream �����
	//! \param freq �ظ���ʽ
	//! \return true�ɹ� false���ɹ�
	virtual bool setStreamSourceFreq (unsigned stream, unsigned freq) = 0;

	//! ��Ⱦ��ǰ������
	//! \param type ͼԪ����
	//! \param prim_count ͼԪ����
	virtual void renderStreams(int type, int prim_count) = 0;

	//! ʹ��������Ⱦ��ǰ������
	//! \param index_array ����������
	//! \param type ͼԪ����
	//! \param prim_count ͼԪ����
	//! \param offset �������������еڼ���������ʼ��Ⱦ
	virtual void renderStreamsIndexed(ATOM_IndexArray* index_array, int type, int prim_count, int offset = 0) = 0;

	//! ��ȡ�ڲ��豸ָ��
	virtual void *getInternalDevice (void) const = 0;

	//! �ύVertexShader����
	virtual bool setConstantsVS (unsigned startRegister, const void *data, unsigned regCount) = 0;

	//! �ύPixelShader����
	virtual bool setConstantsPS (unsigned startRegister, const void *data, unsigned regCount) = 0;

	virtual bool supportRenderTarget (ATOM_PixelFormat format) = 0;
	virtual bool supportRenderTargetPostPixelShaderBlending (ATOM_PixelFormat format) = 0;
	virtual bool supportRenderTargetFiltering (ATOM_PixelFormat format) = 0;
	virtual void renderInterleavedMemoryStreamsIndexed (int type, int vertex_count, int vertex_range, unsigned attrib, unsigned stride, const void *stream, const unsigned short *indices) = 0;
	virtual void outputText (ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color, const char *text, ...) = 0;
	virtual void outputTextOutline (ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color, ATOM_ColorARGB colorOutline, const char *text, ...) = 0;
	virtual void outputTextShadow (ATOM_FontHandle font, int x, int y, int offsetx, int offsety, ATOM_ColorARGB color, ATOM_ColorARGB colorShadow, const char *text, ...) = 0;
	virtual void getViewPoint (float *v) const;
	virtual void present(void);

	virtual bool setRenderTarget (int index, ATOM_Texture *texture, unsigned face = -1) = 0;
	//--- wangjian modified ---//
	// * ������������������ͨ�� setRenderTargetSurface ��������RT֮����Ҫ��ȡRT�ı����Сʱ *//
	virtual ATOM_Texture *getRenderTarget (int index, int * rt_width = 0, int * rt_height = 0 ) const = 0;
	//-------------------------//
	virtual bool setCustomCursor (ATOM_RenderWindow *view, ATOM_Image *image, int hotspot_x = 0, int hotspot_y = 0, const int *region = 0);
	virtual bool setCustomCursor (ATOM_RenderWindow *view, ATOM_Texture *image, int hotspot_x = 0, int hotspot_y = 0, const int *region = 0);
	virtual bool setCustomCursor (ATOM_RenderWindow *view, HCURSOR hCursor);
	virtual bool showCustomCursor (ATOM_RenderWindow *view, bool show);
	virtual bool isCustomCursorShown (ATOM_RenderWindow *view) const;
	virtual void setCursorPosition (int x, int y);
	virtual void setDefaultCursor (ATOM_RenderWindow *view);
	virtual unsigned allocOcclusionQuery (void) = 0;
	virtual void freeOcclusionQuery (unsigned query) = 0;
	virtual bool beginOcclusionQuery (unsigned query) = 0;
	virtual void endOcclusionQuery () = 0;
	virtual unsigned getOcclusionQueryResult (unsigned query) = 0;
	// Transform matrix manipulate functions
	virtual void pushMatrix(int mode);
	virtual void popMatrix(int mode);
	virtual void loadIdentity(int mode);
	// states
	virtual void loadDefaultRenderStates (void) = 0;
	// Texture manipulate functions
	virtual ATOM_AUTOREF(ATOM_Texture) allocTexture(const char *name, unsigned flags);
	virtual ATOM_AUTOREF(ATOM_Texture) allocTexture(const char* name,
									  const void* image,
									  int w,
									  int h,
									  ATOM_PixelFormat format,
									  unsigned flags);
	virtual ATOM_AUTOREF(ATOM_Texture) allocVolumeTexture(const char* name,
									  const void* image,
									  int w,
									  int h,
									  int d,
									  ATOM_PixelFormat format,
									  unsigned flags);
	virtual ATOM_AUTOREF(ATOM_Texture) allocCubeTexture(const char* name,
									  const void* image[6],
									  int size,
									  ATOM_PixelFormat format,
									  unsigned flags);
	virtual ATOM_AUTOREF(ATOM_Texture) allocDepthTexture (unsigned width, unsigned height, bool bUseDX9DSFormat = false );
	//--- wangjian MODIFIED for Multithreaded ---//
	// �첽���� �� ����첽�������ȼ�
	virtual ATOM_AUTOREF(ATOM_Texture) createTextureResource (	const char *filename, unsigned width = 0, unsigned height = 0, ATOM_PixelFormat format = ATOM_PIXEL_FORMAT_UNKNOWN, 
																int loadPriority = ATOM_LoadPriority_ASYNCBASE	) = 0;
	//-------------------------------------------//
	virtual ATOM_AUTOREF(ATOM_Texture) findTextureResource (const char *filename) = 0;
	virtual ATOM_AUTOREF(ATOM_Texture) createTexture (const char *filename, unsigned width = 0, unsigned height = 0, ATOM_PixelFormat format = ATOM_PIXEL_FORMAT_UNKNOWN) = 0;
	virtual ATOM_AUTOREF(ATOM_Texture) lookupTexture(const char* name);
	// Vertex/Index array manipulate functions
	virtual ATOM_AUTOREF(ATOM_VertexArray) allocVertexArray(unsigned attrib, int usage, unsigned size, bool realize = true, unsigned attribFlags = 0 );
	virtual ATOM_AUTOREF(ATOM_IndexArray) allocIndexArray(int usage, unsigned size, bool use32bit, bool realize);
	// Effect manipulate functions
	virtual ATOM_AUTOREF(ATOM_Shader) allocVertexShader (const char *name) = 0;
	virtual ATOM_AUTOREF(ATOM_Shader) allocPixelShader (const char *name) = 0;
	virtual ATOM_AUTOREF(ATOM_GPUProgram) allocGPUProgram (const char *name) = 0;
	virtual ATOM_AUTOREF(ATOM_RenderStateSet) allocRenderStateSet (const char *name);
	virtual ATOM_AUTOREF(ATOM_Shader) lookupVertexShader (const char *name) = 0;
	virtual ATOM_AUTOREF(ATOM_Shader) lookupPixelShader (const char *name) = 0;
	virtual ATOM_AUTOREF(ATOM_GPUProgram) lookupGPUProgram (const char *name) = 0;
	virtual ATOM_AUTOREF(ATOM_RenderStateSet) lookupRenderStateSet (const char *name);
	virtual ATOM_AUTOPTR(ATOM_RenderAttributes) allocRenderAttributes (ATOM_RenderAttributes::RenderState stateType);
	// Vertex Declaration
	virtual ATOM_VertexDecl createMultiStreamVertexDeclaration (unsigned attrib, unsigned attribFlags = 0) = 0;	// wangjian modified
	virtual ATOM_VertexDecl createMultiStreamVertexDeclarationEx (unsigned attrib0, ...) = 0;
	virtual ATOM_VertexDecl createInterleavedVertexDeclaration (unsigned attrib, unsigned attribInstancData = 0, unsigned attribFlags = 0) = 0;	// wangjian modified
	virtual void destroyVertexDecl (ATOM_VertexDecl vd) = 0;
	// Depth buffer
	virtual ATOM_AUTOREF(ATOM_DepthBuffer) allocDepthBuffer (unsigned width, unsigned height) = 0;
	virtual bool setDepthBuffer (ATOM_DepthBuffer *depthBuffer) = 0;
	virtual ATOM_DepthBuffer *getDepthBuffer (void) const = 0;
	// Statistics functions
	virtual unsigned getNumDrawCalls (void) const;
	virtual unsigned getNumPrimitivesDrawn (void) const;
	virtual unsigned getNumTextMaps (void) const;
	virtual unsigned getNumTextures (void) const = 0;

	//-- wangjian added for test ---//
	// * ����API, ����֧��RESZ[��ʱδ�õ�]
	virtual void renderStreamsImmediateRESZ(ATOM_Texture * texture_resz) = 0;
	// * ����API, ���Է���һ�� Render Target Surface ��Դ
	// * ��API��Ҫ����ĳЩ����ʹ�������������shadow map��Ⱦʱ������豸֧��NRT����ô��ʹ��NULL RENDER TARGETʱ��Ҫֱ������
	// * RT surface,����ͨ����������õ�RT surface.
	// * ����Ⱦ������ʱ��ֱ�Ӱ󶨸�RT��surface.
	//! \param width ����Ŀ�
	//! \param height ����ĸ�
	//! \param fmt ����ĸ�ʽ[��ʱֻʹ�õ�NULLRT��ʽ]
	virtual ATOM_AUTOREF(ATOM_RenderTargetSurface) allocRenderTargetSurface (	unsigned width, unsigned height,
																				ATOM_RenderTargetSurface::ATOM_RTSURFACE_FORMAT fmt	) = 0;
	// * ����API, ���԰�һ�� Render Target Surface
	// * ��API��Ҫ����ĳЩ����ʹ�������������shadow map��Ⱦʱ������豸֧��NRT����ô��ʹ��NULL RENDER TARGETʱ��Ҫֱ������
	// * RT surface,����ͨ����������õ�RT surface.
	// * ����Ⱦ������ʱ��ֱ�Ӱ󶨸�RT��surface.
	// * ע�⣺����Ⱦ������֮��ͨ�� setRenderTargetSurface(index,NULL)����RT.
	virtual bool setRenderTargetSurface (int index, ATOM_RenderTargetSurface *rts) = 0;

	virtual void applySlopeScaleBias(float fDepthBias = .0f, float fSlopeScaleBias = 0.0f ) = 0;

	virtual void enableSRGBWrite(bool enable) = 0;
	//------------------------------//

	virtual bool isContentBackupEnabled (void) const;


public:
	virtual bool getImageFileInfoInMemory (const void *data, unsigned size, ATOM_ImageFileInfo *fileInfo) const = 0;	    

public:
    // render states
	virtual bool applyAlphaBlendState (ATOM_AlphaBlendAttributes *state, int pass, bool force) = 0;
	virtual bool applyColorWriteState (ATOM_ColorWriteAttributes *state, int pass, bool force) = 0;
	virtual bool applyDepthState (ATOM_DepthAttributes *state, int pass, bool force) = 0;
	virtual bool applyStencilState (ATOM_StencilAttributes *state, int pass, bool force) = 0;
	virtual bool applyRasterizerState (ATOM_RasterizerAttributes *state, int pass, bool force) = 0;
	virtual bool applyMultisampleState (ATOM_MultisampleAttributes *state, int pass, bool force) = 0;
	virtual bool applySamplerState (ATOM_SamplerAttributes *state, int index, int pass, bool force) = 0;
	virtual bool applyAlphaTestState (ATOM_AlphaTestAttributes *state, int pass, bool force) = 0;

public:
    virtual bool saveImageToFile (int w, int h, const void *rawdata, ATOM_PixelFormat format, const char *filename) = 0;

public:
    virtual void syncRenderStates(void);
	virtual bool pushRenderStates(void) = 0;
	virtual bool popRenderStates(void) = 0;
    virtual void setViewCursor (ATOM_RenderWindow *view);
    virtual void freeResource (ATOM_GfxResource *resource);
    virtual void freeAllResources (void);
    virtual void removeResource (ATOM_GfxResource *resource);
    virtual void insertResource (ATOM_GfxResource *resource);
    virtual void removeAllResources (void);
    virtual void setConfigure(const char *entry, int value) = 0;
	virtual bool dumpTextureStats (const char *filename);
	virtual bool resizeDevice (unsigned w, unsigned h) = 0;
	virtual ATOM_GfxResource *getResourceList (void) const;
	virtual unsigned getFrameStamp (void) const = 0;
	virtual void activate (bool active);
	virtual bool isActive (void) const;

protected:
    virtual void _setClearColor_Impl (const float *rgba) = 0;
    virtual void _setClearDepth_Impl (float depth) = 0;
    virtual void _setClearStencil_Impl (int stencil) = 0;
    virtual void _clear_Impl (bool color, bool depth, bool stencil) = 0;
    virtual void _setModelViewMatrix_Impl (const ATOM_Matrix4x4f *world, const ATOM_Matrix4x4f *view) = 0;
    virtual void _setProjectionMatrix_Impl (const ATOM_Matrix4x4f *proj) = 0;
    virtual void _setTextureMatrix_Impl (int stage, const ATOM_Matrix4x4f *matrix) = 0;
    virtual void _setViewport_Impl (const int *xywh, float depthMin, float depthMax) = 0;
    virtual void _present_Impl (void);
    virtual void _setScissorRect_Impl (const int *scissor) = 0;


protected:
    ATOM_Matrix4x4f _M_view_matrix;
    ATOM_Matrix4x4f _M_world_matrix;
    ATOM_Matrix4x4f _M_projection_matrix;
    ATOM_Matrix4x4f _M_model_view_matrix;
	ATOM_Matrix4x4f _M_view_projection_matrix;
    ATOM_Matrix4x4f _M_mvp_matrix;
    ATOM_Matrix4x4f _M_inv_model_view_matrix;
    ATOM_Matrix4x4f _M_inv_view_matrix;
    ATOM_Matrix4x4f _M_inv_world_matrix;
    ATOM_Matrix4x4f _M_inv_projection_matrix;
    ATOM_Matrix4x4f _M_inv_mvp_matrix;
    ATOM_Matrix4x4f _M_texture_matrix[ATOM_MAX_TEXTURE_LAYER_COUNT];
	bool _M_model_view_dirty;
    bool _M_mvp_dirty;
    bool _M_inv_world_dirty;
    bool _M_inv_model_view_dirty;
    bool _M_inv_projection_dirty;
    bool _M_inv_mvp_dirty;
    ATOM_VECTOR<ATOM_Matrix4x4f> _M_view_matrix_stack;
    ATOM_VECTOR<ATOM_Matrix4x4f> _M_world_matrix_stack;
    ATOM_VECTOR<ATOM_Matrix4x4f> _M_proj_matrix_stack;

	float _viewportScaleX;
	float _viewportScaleY;
	float _viewportTranslateX;
	float _viewportTranslateY;
	bool _needAdjustProjectionMatrix;

	bool _M_auto_apply_a2c;

    ATOM_TextureManager* _M_texture_manager;
    ATOM_VBManager* _M_vb_manager;
    ATOM_IBManager* _M_ib_manager;

    ATOM_AUTOREF(ATOM_Texture) _M_rendertarget;

    bool _M_enable_hardware_cursor;
	bool _M_enable_content_backup;

    ATOM_AUTOREF(ATOM_RenderWindow) _M_render_window;
    ATOM_AUTOREF(ATOM_RenderWindow) _M_cursor_window;
    ATOM_RenderView *_M_current_view;
    ATOM_VECTOR<ATOM_RenderView*> _M_views;
    bool _M_reset_display;

	bool _M_device_active;

    unsigned _M_resource_dead_tick;

    ATOM_Mutex _M_lock;
    ATOM_GfxResource *_M_resource_list;
  };

ATOM_RenderDevice * ATOM_CreateRenderDevice (ATOM_DisplayMode *mode, int w, int h, ATOM_RenderWindow::ShowMode show, bool naked, bool resizable, int multisample, const char *title, void *windowid, void *parentid, void *iconid, bool contentBackup, void *instance);
void ATOM_DestroyRenderDevice (ATOM_RenderDevice *device);

#endif // __ATOM_RENDER_IRENDERDEVICE_H
/*! @} */
