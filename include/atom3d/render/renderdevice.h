/**	\file animatedtexturechannel.h
 *	渲染设备类的声明.
 *
 *	\author 高雅昆
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
// 新增一个render target surface的资源类型
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
//! 渲染设备类
//! \todo 减肥
//! \todo 改善多渲染窗口的支持
//! \author 高雅昆
class ATOM_RenderDevice
{
public:
	//! 纹理过滤质量
	//! \sa ATOM_RenderDevice::setTextureQuality
	enum TextureQuality
	{
		//! 关闭各向异性过滤
		TQ_LOW,
		//! 4倍各向异性过滤
		TQ_MEDIUM,
		//! 8倍各向异性过滤
		TQ_HIGH,
		//! 16倍各向异性过滤(如果支持)
		TQ_ULTRA_HIGH
	};

	enum 
	{
		STATESET_COLOR_BUFFER = (1<<0),
		STATESET_DEPTH_BUFFER = (1<<1),
		STATESET_STENCIL_BUFFER = (1<<2),
	};

	//! 构造函数
	ATOM_RenderDevice(void);

	//! 析构函数
	virtual ~ATOM_RenderDevice(void);

public:
	//! 获取渲染窗口
	//! \return 渲染窗口
    virtual ATOM_AUTOREF(ATOM_RenderWindow) getWindow (void) const;

	//! 获取当前渲染窗口 
	//! \return 当前渲染窗口
    virtual ATOM_RenderWindow *getCurrentView (void);

	//! 获取当前鼠标指针信息
	//! \return 当前鼠标指针信息
    virtual ATOM_CursorInfo *getCurrentCursorInfo (void);

public:
	//! 初始化渲染设备和窗口
	//! \todo 接口重新设计
	//! \param fullscreen_mode 全屏模式参数，如果是窗口模式这个参数应该为NULL
	//! \param title 窗口标题
	//! \param w 窗口宽度，如果fullscreen_mode和windowid不为NULL这个参数没用
	//! \param h 窗口宽度，如果fullscreen_mode和windowid不为NULL这个参数没用
	//! \param show 如果为true则创建窗口后立即显示，否则为隐藏窗口
	//! \param naked 如果为true则窗口包括边框和标题栏，如果fullscreen_mode和windowid不为NULL这个参数没用
	//! \param resizable 如果为true窗口可以改变大小，如果fullscreen_mode和windowid不为NULL这个参数没用
	//! \param multisample 全屏幕抗拒齿的倍数(FSAA)
	//! \param windowid 指向一个现有窗口句柄，在windows操作系统中这个参数是指向HWND的指针。如果这个参数不为NULL则会在这个窗口中创建渲染设备
	//! \param iconid 窗口图标句柄
	//! \return true 成功 false 失败
	virtual bool setup (ATOM_DisplayMode *fullscreen_mode, const char *title, int w, int h, ATOM_RenderWindow::ShowMode showMode, bool naked, bool resizable, int multisample, void *windowid, void *parentid, void *iconid, bool contentBackup, void *instance);

	//! 在一个窗口上创建渲染设备.
	//! 这个函数为内部使用
	//! \todo 不公开此接口
	//! \param window 窗口
	//! \param multisample 全屏幕抗拒齿的倍数(FSAA)
	//! \return true 成功 false 失败
	virtual bool initialize (ATOM_RenderWindow *window, int multisample) = 0;

	//! \return true 支持 false 不支持
	virtual bool isMultisampleSupported (void);

	//! \return 支持 false 支持
	virtual bool isSampleSupported (unsigned sample);

	//! 添加一个渲染窗口.
	//! 我们支持多个渲染窗口，通过这个参数来添加一个渲染窗口到设备
	//! \param window 渲染窗口
	//! \return true 成功 false 失败
	//! \sa removeView setCurrentView
	virtual bool addView (ATOM_RenderWindow *window);

	//! 删除一个渲染窗口
	//! \param window 要删除的窗口
	//! \sa addView setCurrentView
	virtual void removeView (ATOM_RenderWindow *window);

	//! 设置当前渲染窗口.
	//! \param window 渲染窗口，调用此函数后后续的渲染调用将以此窗口为渲染目标
	virtual void setCurrentView (ATOM_RenderWindow *window);

	//! 获取后备缓冲区的大小
	//! \param w 保存宽度
	//! \param h 保存高度
	//! \return true 成功 false 失败
	virtual bool getBackbufferSize (int *w, int *h) const = 0;

	//! 获取设备指针.
	//! 在Windows操作系统返回Direct3D设备指针
	//! \todo 函数名称不确切
	//! \return 设备指针
	virtual void *getRenderContext (void) const = 0;

	//! 获取本设备支持的最佳的具有RGBA四个通道的HDR像素格式
	//! \param supportFiltering 如果调用成功，这个参数保存非零值表示该像素格式支持硬件二线性过滤，否则不支持硬件二线性过滤
	//! \return 像素格式，可以是PIXEL_FORMAT_RGBA32F或者PIXEL_FORMAT_RGBA16F，如果设备不支持浮点贴图返回ATOM_PIXEL_FORMAT_UNKNOWN
	virtual ATOM_PixelFormat getBestHDRFormatRGBA (int *supportFiltering) const = 0;

	//! 获取本设备支持的最佳的具有RG两个通道的HDR像素格式
	//! \param supportFiltering 如果调用成功，这个参数保存非零值表示该像素格式支持硬件二线性过滤，否则不支持硬件二线性过滤
	//! \return 像素格式，可以是PIXEL_FORMAT_RG32F或者PIXEL_FORMAT_RG16F，如果设备不支持浮点贴图返回ATOM_PIXEL_FORMAT_UNKNOWN
	virtual ATOM_PixelFormat getBestHDRFormatRG (int *supportFiltering) const = 0;

	//! 获取本设备支持的最佳的具有R通道的HDR像素格式
	//! \param supportFiltering 如果调用成功，这个参数保存非零值表示该像素格式支持硬件二线性过滤，否则不支持硬件二线性过滤
	//! \return 像素格式，可以是PIXEL_FORMAT_R32F或者PIXEL_FORMAT_R16F，如果设备不支持浮点贴图返回ATOM_PIXEL_FORMAT_UNKNOWN
	virtual ATOM_PixelFormat getBestHDRFormatR (int *supportFiltering) const = 0;

	//! 检查一个像素格式是否可以作为RenderTarget
	//! \param format 要检查的格式
	//! \return true可以 false不可以
	virtual bool isRenderTargetFormat (ATOM_PixelFormat format) const = 0;

	//! 查询设备是否就绪
	//! 当处于DeviceLost状态则为非就绪状态
	virtual bool isDeviceReady (void) const = 0;

	virtual ATOM_StateCache *getStateCache (void) const = 0;

public:
	//! 设置某个渲染窗口的后备缓冲区清除色
	//! \param view 渲染窗口
	//! \param r 清除色红色分量，范围0到1
	//! \param g 清除色绿色分量，范围0到1
	//! \param b 清除色蓝色分量，范围0到1
	//! \param a 清除色Alpha分量，范围0到1
	virtual void setClearColor(ATOM_RenderWindow *view, float r, float g, float b, float a);

	//! 获取某个渲染窗口的后备缓冲区清除色
	//! \param view 渲染窗口
	//! \param r 清除色红色分量，范围0到1
	//! \param g 清除色绿色分量，范围0到1
	//! \param b 清除色蓝色分量，范围0到1
	//! \param a 清除色Alpha分量，范围0到1
	virtual void getClearColor(ATOM_RenderWindow *view, float &r, float &g, float &b, float &a) const;

	//! 获取某个渲染窗口的后备缓冲区清除色
	//! \param view 渲染窗口
	//! \return 清除色
	virtual ATOM_ColorARGB getClearColor(ATOM_RenderWindow *view) const;

	//! 设置某个渲染窗口的后备缓冲区清除色
	//! \param view 渲染窗口
	//! \param color ARGB颜色值
	virtual void setClearColor(ATOM_RenderWindow *view, ATOM_ColorARGB color);

	//! 设置某个渲染窗口的深度缓冲区清除值
	//! \param view 渲染窗口
	//! \param d 清除值，范围0到1
	virtual void setClearDepth(ATOM_RenderWindow *view, float d);

	//! 设置某个渲染窗口的模板缓冲区清除值
	//! \param view 渲染窗口
	//! \param s 清除值，范围0到255
	virtual void setClearStencil(ATOM_RenderWindow *view, int s);

	//! 设置某个渲染窗口的视口范围
	//! \param view 渲染窗口
	//! \param x 视口左上角X坐标
	//! \param y 视口左上角Y坐标
	//! \param w 视口宽度
	//! \param h 视口高度
	//! \param depthMin 最小的深度值
	//! \param depthMax 最大的深度值
	//! \sa getViewport
	virtual void setViewport(ATOM_RenderWindow *view, int x, int y, int w, int h, float depthMin = 0.f, float depthMax = 1.0f);

	//! 设置某个渲染窗口的视口范围
	//! \param view 渲染窗口
	//! \param viewport 视口范围
	//! \param depthMin 最小的深度值
	//! \param depthMax 最大的深度值
	//! \sa getViewport
	virtual void setViewport(ATOM_RenderWindow *view, const ATOM_Rect2Di &viewport, float depthMin = 0.f, float depthMax = 1.0f);

	//! 获取某个渲染窗口的视口范围
	//! \param view 渲染窗口
	//! \param x 视口左上角X坐标
	//! \param y 视口左上角Y坐标
	//! \param w 视口宽度
	//! \param h 视口高度
	//! \param depthMin 最小的深度值
	//! \param depthMax 最大的深度值
	//! \sa setViewport
	virtual void getViewport(ATOM_RenderWindow *view, int* x, int* y, int* w, int* h, float *depthMin = 0, float *depthMax = 0) const;

	//! 获取某个渲染窗口的视口范围
	//! \param view 渲染窗口
	//! \param depthMin 最小的深度值
	//! \param depthMax 最大的深度值
	//! \return 视口范围
	//! \sa setViewport
	virtual ATOM_Rect2Di getViewport(ATOM_RenderWindow *view, float *depthMin = 0, float *depthMax = 0) const;

	//! 设置视口剪裁区域
	//! \param view 渲染窗口
	//! \param x 区域左上角X坐标
	//! \param y 区域左上角Y坐标
	//! \param w 区域宽度
	//! \param h 区域高度
	virtual void setScissorRect (ATOM_RenderWindow *view, int x, int y, int w, int h);

	//! 设置视口剪裁区域
	//! \param view 渲染窗口
	//! \param rc 区域范围
	virtual void setScissorRect (ATOM_RenderWindow *view, const ATOM_Rect2Di &rc);

	//! 获取当前视口剪裁范围
	//! \param view 渲染窗口
	//! \param x 区域左上角X坐标
	//! \param y 区域左上角Y坐标
	//! \param w 区域宽度
	//! \param h 区域高度
	virtual void getScissorRect (ATOM_RenderWindow *view, int *x, int *y, int *w, int *h) const;

	//! 获取当前视口剪裁范围
	//! \param view 渲染窗口
	//! \return 剪裁区域
	virtual ATOM_Rect2Di getScissorRect (ATOM_RenderWindow *view) const;

	//! 设置是否允许视口剪裁
	//! \param view 渲染窗口
	//! \param enable true表示允许，false表示不允许
	virtual void enableScissorTest (ATOM_RenderWindow *view, bool enable) = 0;

	//! 查询视口剪裁是否打开
	//! \return true 打开 false 未打开
	//! \sa enableScissorTest setScissorRect getScissorRect
	virtual bool isScissorTestEnabled (ATOM_RenderWindow *view) const = 0;

	//! 读取某个渲染窗口的颜色缓冲区内容
	//! \param view 渲染窗口
	//! \param x 读取范围左上角X坐标
	//! \param y 读取范围左上角Y坐标
	//! \param w 读取范围的宽度
	//! \param h 读取范围的高度
	//! \param format 读取的像素格式
	//! \param pixels 读取缓冲区，必须具有足够的大小
	//! \return true 成功 false 失败
	virtual bool readColorBuffer(ATOM_RenderWindow *view, int x, int y, int w, int h, ATOM_PixelFormat format, void *pixels) = 0;

	//! 读取某个渲染窗口的深度缓冲区内容
	//! \todo 在D3D中该函数无法工作
	//! \param view 渲染窗口
	//! \param x 读取范围左上角X坐标
	//! \param y 读取范围左上角Y坐标
	//! \param w 读取范围的宽度
	//! \param h 读取范围的高度
	//! \param pixels 读取缓冲区
	//! \return true 成功 false 失败
	virtual bool readDepthBuffer(ATOM_RenderWindow *view, int x, int y, int w, int h, void *pixels) = 0;

	//! 读取某个渲染窗口的模板缓冲区内容
	//! \todo 在D3D中该函数无法工作
	//! \param view 渲染窗口
	//! \param x 读取范围左上角X坐标
	//! \param y 读取范围左上角Y坐标
	//! \param w 读取范围的宽度
	//! \param h 读取范围的高度
	//! \param pixels 读取缓冲区
	//! \return true 成功 false 失败
	virtual bool readStencilBuffer (ATOM_RenderWindow *view, int x, int y, int w, int h, void *pixels) = 0;

	//! 读取某个渲染窗口的后备颜色缓冲区内容并保存到图像中
	//! \param view 渲染窗口
	//! \param path 虚拟文件路径
	//! \return true 成功 false 失败
	virtual bool screenShot (ATOM_RenderWindow *view, const char *path = NULL);

	//! 读取某个渲染窗口的后备颜色缓冲区内容并保存到ATOM_Image类
	//! \param view 渲染窗口
	//! \param image ATOM_Image类指针
	//! \return true 成功 false 失败
	virtual bool screenShot (ATOM_RenderWindow *view, ATOM_AUTOREF(ATOM_Image) image) = 0;

	//! 获取可用的纹理内存数量
	//! \return 可用纹理内存字节数
	//! \note 该函数返回的值并不精确
	virtual unsigned getAvailableTextureMem (void) const = 0;

	//! 设置纹理各向异性过滤质量
	//! \param quality 过滤参数
	//! \sa ATOM_RenderDevice::TextureQuality ATOM_RenderDevice::getTextureQuality
	virtual void setTextureQuality (ATOM_RenderDevice::TextureQuality quality) = 0;

	//! 获取当前纹理各向异性过滤质量
	//! \return 过滤质量
	//! \sa ATOM_RenderDevice::TextureQuality ATOM_RenderDevice::setTextureQuality
	virtual ATOM_RenderDevice::TextureQuality getTextureQuality (void) const = 0;

	//! 设置顶点光源
	//! \param index 灯光索引
	//! \param light 灯光结构
	//! \return true 成功 false 失败
	virtual bool setLight (unsigned index, const ATOM_VertexLight *light) = 0;

	//! 打开/关闭一个顶点光源
	//! \param index 灯光索引
	//! \param enable true打开 false关闭
	//! \return true 成功 false 失败
	virtual bool enableLight (unsigned index, bool enable) = 0;

	//! 设置全局环境光
	//! \param color 全局环境光颜色
	//! \return true 成功 false 失败
	virtual bool setAmbientColor (ATOM_ColorARGB color) = 0;

	//! 设置当前顶点光材质
	//! \param material 材质结构
	//! \return true 成功 false 失败
	virtual bool setMaterial (const ATOM_VertexLightMaterial *material) = 0;

	//! 打开/关闭顶点光照
	//! \param enable true打开 false关闭
	//! \return true 成功 false 失败
	virtual bool enableVertexLighting (bool enable) = 0;

public:
	//! 设置顶点管道的变换矩阵
	//! \param mode 变换模式
	//! \param matrix 矩阵
	virtual void setTransform(int mode, const ATOM_Matrix4x4f &matrix);

	//! 获取顶点管道的变换矩阵
	//! \param mode 变换模式
	//! \param matrix 保存变换矩阵
	virtual void getTransform(int mode, ATOM_Matrix4x4f &matrix);

	//! 设置纹理变换矩阵
	//! \param stage 贴图层序号
	//! \param matrix 矩阵
	virtual void setTextureTransform(int stage, const ATOM_Matrix4x4f &matrix);

	//! 获取纹理变换矩阵
	//! \param stage 贴图层序号
	//! \param matrix 矩阵
	virtual void getTextureTransform(int stage, ATOM_Matrix4x4f &matrix);

	//! 获取供货商标示
	//! \return 标示
	//! \todo 没什么用，去掉
	virtual const char *getVendor (void) const = 0;

	//! 获取Renderer
	//! \todo 没什么用，去掉
	virtual const char *getRenderer (void) const = 0;

	//! 获取设备渲染能力参数
	//! \return 设备渲染能力参数结构
	virtual const ATOM_RenderDeviceCaps & getCapabilities (void) const;

	//! 将所有设备管理的资源使用的显示内存全部释放
	virtual void evictManagedResource (void);

	//! 开始一帧新的渲染
	//! \return true成功 false失败
	//! \sa endFrame
	virtual bool beginFrame(void);

	//! 结束当前帧渲染
	//! \sa beginFrame
	//! \note 如果beginFrame返回false则无需调用
	virtual void endFrame(void);

	//! 清除后备缓冲区
	//! \param color true清除颜色缓冲区 false不清除颜色缓冲区
	//! \param depth true清除深度缓冲区 false不清除深度缓冲区
	//! \param stencil true清除模板缓冲区 false不清除模板缓冲区
	virtual void clear(bool color, bool depth, bool stencil);

	//! 设置当前渲染顶点流的定义
	//! \param vd 顶点流定义
	//! \return true成功 false不成功
	virtual bool setVertexDecl(ATOM_VertexDecl vd) = 0;

	//! 设置贴图
	//! \param sampler 采样器索引
	//! \param texture 贴图指针
	//! \return true成功 false不成功
	virtual bool setTexture (int sampler, ATOM_Texture *texture) = 0;

	//! 设置顶点流
	//! \param stream 流序号
	//! \param vertex_array 顶点数组
	//! \param offset 顶点流相对顶点数组起始位置的字节偏移量
	//! \return true成功 false不成功
	virtual bool setStreamSource(unsigned stream, ATOM_VertexArray* vertex_array, unsigned offset = 0) = 0;

	//! 设置顶点流的重复使用方式.
	//! 主要用于实例化渲染
	//! \param stream 流序号
	//! \param freq 重复方式
	//! \return true成功 false不成功
	virtual bool setStreamSourceFreq (unsigned stream, unsigned freq) = 0;

	//! 渲染当前顶点流
	//! \param type 图元类型
	//! \param prim_count 图元数量
	virtual void renderStreams(int type, int prim_count) = 0;

	//! 使用索引渲染当前顶点流
	//! \param index_array 索引缓冲区
	//! \param type 图元类型
	//! \param prim_count 图元数量
	//! \param offset 从索引缓冲区中第几个索引开始渲染
	virtual void renderStreamsIndexed(ATOM_IndexArray* index_array, int type, int prim_count, int offset = 0) = 0;

	//! 获取内部设备指针
	virtual void *getInternalDevice (void) const = 0;

	//! 提交VertexShader参数
	virtual bool setConstantsVS (unsigned startRegister, const void *data, unsigned regCount) = 0;

	//! 提交PixelShader参数
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
	// * 新增两个参数：用于通过 setRenderTargetSurface 函数设置RT之后，需要获取RT的表面大小时 *//
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
	// 异步加载 ： 添加异步加载优先级
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
	// * 新增API, 用以支持RESZ[暂时未用到]
	virtual void renderStreamsImmediateRESZ(ATOM_Texture * texture_resz) = 0;
	// * 新增API, 用以分配一个 Render Target Surface 资源
	// * 该API主要用于某些特殊使用情况，比如在shadow map渲染时，如果设备支持NRT，那么在使用NULL RENDER TARGET时需要直接生成
	// * RT surface,而非通过生成纹理得到RT surface.
	// * 在渲染到纹理时，直接绑定该RT的surface.
	//! \param width 表面的宽
	//! \param height 表面的高
	//! \param fmt 表面的格式[暂时只使用到NULLRT格式]
	virtual ATOM_AUTOREF(ATOM_RenderTargetSurface) allocRenderTargetSurface (	unsigned width, unsigned height,
																				ATOM_RenderTargetSurface::ATOM_RTSURFACE_FORMAT fmt	) = 0;
	// * 新增API, 用以绑定一个 Render Target Surface
	// * 该API主要用于某些特殊使用情况，比如在shadow map渲染时，如果设备支持NRT，那么在使用NULL RENDER TARGET时需要直接生成
	// * RT surface,而非通过生成纹理得到RT surface.
	// * 在渲染到纹理时，直接绑定该RT的surface.
	// * 注意：在渲染到纹理之后通过 setRenderTargetSurface(index,NULL)重置RT.
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
