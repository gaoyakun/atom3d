#ifndef __ATOM3D_GFXDRIVER_GFXDRIVER_H
#define __ATOM3D_GFXDRIVER_GFXDRIVER_H

#include "../ATOM_ttfont.h"
#include "basedefs.h"
#include "texture.h"
#include "shader.h"
#include "gpuprogram.h"
#include "vertexarray.h"
#include "indexarray.h"
#include "stateset.h"

class ATOM_GfxWindow;
class ATOM_StateCache;
class ATOM_GfxDriverCaps;

class ATOM_GfxDriver
{
public:
	typedef void * VertexDecl;

	struct VertexLightMaterial
	{
		ATOM_Vector4f diffuse;
		ATOM_Vector4f ambient;
		ATOM_Vector4f specular;
		ATOM_Vector4f emissive;
		float power;
	};

	struct VertexLight
	{
		enum Type
		{
			Directional,
			Point,
			Spot
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

	//! 顶点变换矩阵模式
	enum TransformMode
	{
		//! 内部使用
		InvMatrixIndex = 1,
		//! 内部使用
		TransposeMatrixIndex = 2,
		//! 内部使用
		InvTransposeMatrixIndex = 3,

		//! 世界变换矩阵（模型空间变换到世界空间）
		World = 0,
		//! 世界变换的逆（世界空间变换到模型空间）
		InvWorld = World+InvMatrixIndex,
		//! 世界变换的转置矩阵
		TransposeWorld = World+TransposeMatrixIndex,
		//! 世界变换的逆转置矩阵
		InvTransposeWorld = World+InvTransposeMatrixIndex,
		//! 摄像机变换矩阵（世界空间变换到摄像机空间）
		View,
		//! 摄像机变换的逆矩阵（摄像机空间变换到世界空间）
		InvView = View+InvMatrixIndex,
		//! 摄像机变换的转置矩阵
		TransposeView = View+TransposeMatrixIndex,
		//! 摄像机变换的逆转置矩阵
		InvTransposeView = View+InvTransposeMatrixIndex,
		//! 投影变换矩阵（摄像机空间变换到视口空间）
		Projection,
		//! 投影变换的逆矩阵（视口空间变换到摄像机空间）
		InvProjection = Projection+InvMatrixIndex,
		//! 投影变换的转置矩阵
		TransposeProjection = Projection+TransposeMatrixIndex,
		//! 投影变换的逆转置矩阵
		InvTransposeProjection = Projection+InvTransposeMatrixIndex,
		//! 世界-摄像机变换矩阵（模型空间变换到摄像机空间，此矩阵仅可读取不可直接设置）
		WorldView,
		//! 世界-摄像机变换的逆矩阵（摄像机空间变换到模型空间，此矩阵仅可读取不可直接设置）
		InvWorldView = WorldView+InvMatrixIndex,
		//! 世界-摄像机变换的转置矩阵（此矩阵仅可读取不可直接设置）
		TransposeWorldView = WorldView+TransposeMatrixIndex,
		//! 世界-摄像机变换的逆转置矩阵（此矩阵仅可读取不可直接设置）
		InvTransposeWorldView = WorldView+InvTransposeMatrixIndex,
		//! 摄像机-投影变换矩阵（世界空间变换到视口空间，此矩阵仅可读取不可直接设置）
		ViewProjection,
		//! 摄像机-投影变换的逆矩阵（视口空间变换到世界空间，此矩阵仅可读取不可直接设置）
		InvViewProjection = ViewProjection+InvMatrixIndex,
		//! 摄像机-投影变换的转置矩阵（此矩阵仅可读取不可直接设置）
		TransposeViewProjection = ViewProjection+TransposeMatrixIndex,
		//! 摄像机-投影变换的逆转置矩阵（此矩阵仅可读取不可直接设置）
		InvTransposeViewProjection = ViewProjection+InvTransposeMatrixIndex,
		//! 世界-摄像机-投影变换矩阵（模型空间变换到视口空间，此矩阵仅可读取不可直接设置）
		WorldViewProjection,
		//! 世界-摄像机-投影变换的逆矩阵（视口空间变换到模型空间，此矩阵仅可读取不可直接设置）
		InvWorldViewProjection = WorldViewProjection+InvMatrixIndex,
		//! 世界-摄像机-投影变换的转置矩阵（此矩阵仅可读取不可直接设置）
		TransposeWorldViewProjection = WorldViewProjection+TransposeMatrixIndex,
		//! 世界-摄像机-投影变换的逆转置矩阵（此矩阵仅可读取不可直接设置）
		InvTransposeWorldViewProjection = WorldViewProjection+InvTransposeMatrixIndex,
		//! 纹理0变换矩阵
		Texture0,
		//! 纹理0变换矩阵的逆矩阵（此矩阵仅可读取不可直接设置）
		InvTexture0 = Texture0+InvMatrixIndex,
		//! 纹理0变换矩阵的转置矩阵（此矩阵仅可读取不可直接设置）
		TransposeTexture0 = Texture0+TransposeMatrixIndex,
		//! 纹理0变换矩阵的逆转置矩阵（此矩阵仅可读取不可直接设置）
		InvTransposeTexture0 = Texture0+InvTransposeMatrixIndex,
		//! 纹理1变换矩阵
		Texture1,
		//! 纹理1变换矩阵的逆矩阵（此矩阵仅可读取不可直接设置）
		InvTexture1 = Texture1+InvMatrixIndex,
		//! 纹理1变换矩阵的转置矩阵（此矩阵仅可读取不可直接设置）
		TransposeTexture1 = Texture1+TransposeMatrixIndex,
		//! 纹理1变换矩阵的逆转置矩阵（此矩阵仅可读取不可直接设置）
		InvTransposeTexture1 = Texture1+InvTransposeMatrixIndex,
		//! 纹理2变换矩阵
		Texture2,
		//! 纹理2变换矩阵的逆矩阵（此矩阵仅可读取不可直接设置）
		InvTexture2 = Texture2+InvMatrixIndex,
		//! 纹理2变换矩阵的转置矩阵（此矩阵仅可读取不可直接设置）
		TransposeTexture2 = Texture2+TransposeMatrixIndex,
		//! 纹理2变换矩阵的逆转置矩阵（此矩阵仅可读取不可直接设置）
		InvTransposeTexture2 = Texture2+InvTransposeMatrixIndex,
		//! 纹理3变换矩阵
		Texture3,
		//! 纹理3变换矩阵的逆矩阵（此矩阵仅可读取不可直接设置）
		InvTexture3 = Texture3+InvMatrixIndex,
		//! 纹理3变换矩阵的转置矩阵（此矩阵仅可读取不可直接设置）
		TransposeTexture3 = Texture3+TransposeMatrixIndex,
		//! 纹理3变换矩阵的逆转置矩阵（此矩阵仅可读取不可直接设置）
		InvTransposeTexture3 = Texture3+InvTransposeMatrixIndex,
		//! 纹理4变换矩阵
		Texture4,
		//! 纹理4变换矩阵的逆矩阵（此矩阵仅可读取不可直接设置）
		InvTexture4 = Texture4+InvMatrixIndex,
		//! 纹理4变换矩阵的转置矩阵（此矩阵仅可读取不可直接设置）
		TransposeTexture4 = Texture4+TransposeMatrixIndex,
		//! 纹理4变换矩阵的逆转置矩阵（此矩阵仅可读取不可直接设置）
		InvTransposeTexture4 = Texture4+InvTransposeMatrixIndex,
		//! 纹理5变换矩阵
		Texture5,
		//! 纹理5变换矩阵的逆矩阵（此矩阵仅可读取不可直接设置）
		InvTexture5 = Texture5+InvMatrixIndex,
		//! 纹理5变换矩阵的转置矩阵（此矩阵仅可读取不可直接设置）
		TransposeTexture5 = Texture5+TransposeMatrixIndex,
		//! 纹理5变换矩阵的逆转置矩阵（此矩阵仅可读取不可直接设置）
		InvTransposeTexture5 = Texture5+InvTransposeMatrixIndex,
		//! 纹理6变换矩阵
		Texture6,
		//! 纹理6变换矩阵的逆矩阵（此矩阵仅可读取不可直接设置）
		InvTexture6 = Texture6+InvMatrixIndex,
		//! 纹理6变换矩阵的转置矩阵（此矩阵仅可读取不可直接设置）
		TransposeTexture6 = Texture6+TransposeMatrixIndex,
		//! 纹理6变换矩阵的逆转置矩阵（此矩阵仅可读取不可直接设置）
		InvTransposeTexture6 = Texture6+InvTransposeMatrixIndex,
		//! 纹理7变换矩阵
		Texture7,
		//! 纹理7变换矩阵的逆矩阵（此矩阵仅可读取不可直接设置）
		InvTexture7 = Texture7+InvMatrixIndex,
		//! 纹理7变换矩阵的转置矩阵（此矩阵仅可读取不可直接设置）
		TransposeTexture7 = Texture7+TransposeMatrixIndex,
		//! 纹理7变换矩阵的逆转置矩阵（此矩阵仅可读取不可直接设置）
		InvTransposeTexture7 = Texture7+InvTransposeMatrixIndex
	};

public:
	//! 复位渲染设备.
	//! \return true 成功 false 失败
	virtual bool reset (void) = 0;

	//! 获取渲染状态缓冲区
	//! \return 渲染状态缓冲区
	virtual ATOM_StateCache *getStateCache (void) const = 0;

	//! 设置顶点光源
	//! \param index 灯光索引
	//! \param light 灯光结构
	//! \return true 成功 false 失败
	virtual bool setLight (unsigned index, const VertexLight *light) = 0;

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
	virtual bool setMaterial (const VertexLightMaterial *material) = 0;

	//! 打开/关闭顶点光照
	//! \param enable true打开 false关闭
	//! \return true 成功 false 失败
	virtual bool enableVertexLighting (bool enable) = 0;

public:
	//! 设置顶点管道的变换矩阵
	//! \param mode 变换模式
	//! \param matrix 矩阵
	//! \return true成功 false失败
	virtual bool setTransform(TransformMode mode, const ATOM_Matrix4x4f &matrix) = 0;

	//! 获取顶点管道的变换矩阵
	//! \param mode 变换模式
	//! \param matrix 保存变换矩阵
	virtual const ATOM_Matrix4x4f &getTransform(TransformMode mode) const = 0;

	//! 获取设备渲染能力参数
	//! \return 设备渲染能力参数结构
	virtual const ATOM_GfxDriverCaps *getCapabilities (void) const = 0;

	//! 开始一帧新的渲染
	//! \return true成功 false失败
	//! \sa endFrame
	virtual bool begin(void) = 0;

	//! 结束当前帧渲染
	//! \sa beginFrame
	//! \note 如果beginFrame返回false则无需调用
	virtual void end(void) = 0;

	//! 设置当前渲染顶点流的定义
	//! \param vd 顶点流定义
	//! \return true成功 false不成功
	virtual bool setVertexDecl(VertexDecl vd) = 0;

	//! 设置顶点流
	//! \param stream 流序号
	//! \param vertex_array 顶点数组
	//! \param offset 顶点流相对顶点数组起始位置的字节偏移量
	//! \return true成功 false不成功
	virtual bool setStreamSource(unsigned stream, ATOM_VertexArray* vertex_array, unsigned offset = 0) = 0;

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

	//! 使用索引渲染内存中的顶点流
	//! \param type 图元类型
	//! \param index_count 索引数量
	//! \param vertex_range 顶点数量
	//! \param attrib 顶点流类型
	//! \param stride 单个顶点大小
	//! \param stream 索引缓冲区
	virtual void renderInterleavedMemoryStreamsIndexed (int type, int index_count, int vertex_range, unsigned attrib, unsigned stride, const void *stream, const unsigned short *indices) = 0;

	//! 渲染文字
	//! \param font 字体
	//! \param x 文字基线的x坐标
	//! \param y 文字基线的y坐标
	//! \param color 文字的颜色
	//! \param text 格式化文本
	virtual void outputText (ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color, const char *text, ...) = 0;

	//! 当前变换矩阵入栈
	//! \param mode 变换模式
	//! \return true成功 false失败
	virtual bool pushMatrix(TransformMode mode) = 0;

	//! 变换矩阵出栈
	//! \param mode 变换模式
	//! \return true成功 false失败
	virtual bool popMatrix(TransformMode mode) = 0;

	//! 设置渲染状态到默认
	virtual void loadDefaultRenderStates (void) = 0;

	//! 创建一个新的未初始化的贴图对象
	//! \param name 对象名称
	//! \param flags 贴图特性
	//! \return 新创建的未初始化贴图对象
	virtual ATOM_AUTOREF(ATOM_Texture) allocEmptyTexture(const char *name, unsigned flags) = 0;

	//! 创建一个贴图对象并初始化.
	//! 可创建二维贴图，体积贴图或立方体贴图，如果是体积贴图，参数d表示贴图厚度，如果是立方体贴图，w表示贴图边长，h和d不使用，像素缓冲区顺序为前，后，上，下，左，右
	//! \param name 对象名称
	//! \param image 像素缓冲区或者NULL
	//! \param w 贴图宽度
	//! \param h 贴图高度
	//! \param d 贴图厚度（体积贴图使用此参数）
	//! \param format 贴图的像素格式
	//! \param flags 贴图特性
	//! \return 新创建的贴图
	virtual ATOM_AUTOREF(ATOM_Texture) allocTexture(const char* name,
									  const void* image,
									  int w,
									  int h,
									  int d,
									  ATOM_PixelFormat format,
									  unsigned flags) = 0;

	//! 创建一个深度贴图对象并初始化.
	//! \param name 对象名称
	//! \param width 贴图宽度
	//! \param height 贴图高度
	//! \return 新创建的深度贴图
	virtual ATOM_AUTOREF(ATOM_Texture) allocDepthTexture (const char *name, unsigned width, unsigned height, bool bUseDX9DSFormat = false ) = 0;

	//! 通过文件创建一个贴图
	//! \param name 对象名称
	//! \param filename 文件名
	//! \param format 贴图的像素格式，如果为ATOM_PIXEL_FORMAT_UNKNOWN则自动设置
	virtual ATOM_AUTOREF(ATOM_Texture) allocTextureFromFile (const char *name, const char *filename, ATOM_PixelFormat format = ATOM_PIXEL_FORMAT_UNKNOWN) = 0;

	//! 创建一个顶点数组
	//! \param name 对象名称
	//! \param attrib 顶点流属性
	//! \param usage 使用模式
	//! \param size 顶点个数
	//! \return 创建的顶点数组
	virtual ATOM_AUTOREF(ATOM_VertexArray) allocVertexArray(const char *name, unsigned attrib, int usage, unsigned size) = 0;

	//! 创建一个索引数组
	//! \param name 对象名称
	//! \param usage 使用模式
	//! \param size 索引个数
	//! \param use32bit true使用32位索引 false使用16位索引
	//! \return 创建的索引数组
	virtual ATOM_AUTOREF(ATOM_IndexArray) allocIndexArray(const char *name, int usage, unsigned size, bool use32bit) = 0;

	//! 创建一个顶点程序对象
	//! \param name 对象名称
	//! \return 创建的对象
	virtual ATOM_AUTOREF(ATOM_Shader) allocVertexShader (const char *name) = 0;

	//! 创建一个片段程序对象
	//! \param name 对象名称
	//! \return 创建的对象
	virtual ATOM_AUTOREF(ATOM_Shader) allocPixelShader (const char *name) = 0;

	//! 创建一个GPU程序对象
	//! \param name 对象名称
	//! \return 创建的对象
	virtual ATOM_AUTOREF(ATOM_GPUProgram) allocGPUProgram (const char *name) = 0;

	//! 创建一个渲染状态集合对象
	//! \param name 对象名称
	//! \return 创建的对象
	virtual ATOM_AUTOREF(ATOM_RenderStateSet) allocRenderStateSet (const char *name) = 0;

	//! 创建一个渲染状态对象
	//! \param stateType 要创建的渲染状态类型
	//! \return 创建的渲染状态对象
	virtual ATOM_AUTOPTR(ATOM_RenderAttributes) allocRenderAttributes (ATOM_RenderAttributes::RenderState stateType) = 0;

	//! 创建一个多顶点流格式声明对象
	//! \param attrib 所有顶点流属性集合
	//! \return 格式声明对象
	virtual VertexDecl createMultiStreamVertexDeclaration (unsigned attrib) = 0;

	//! 创建一个多顶点流格式声明对象
	//! \param attrib0 顶点流属性
	//! \return 格式声明对象
	virtual VertexDecl createMultiStreamVertexDeclarationEx (unsigned attrib0, ...) = 0;

	//! 创建一个交错顶点流格式声明对象
	//! \param attrib 顶点流属性
	//! \return 格式声明对象
	virtual VertexDecl createInterleavedVertexDeclaration (unsigned attrib) = 0;

	//! 删除一个已创建的顶点流格式声明对象
	//! \param vd 顶点流格式声明对象
	virtual void destroyVertexDecl (VertexDecl vd) = 0;

	//! 创建一个深度缓冲区
	//! \param name 对象名称
	//! \param width 深度缓冲区宽度
	//! \param height 深度缓冲区高度
	//! \return 深度缓冲区对象
	virtual ATOM_AUTOREF(ATOM_DepthBuffer) allocDepthBuffer (const char *name, unsigned width, unsigned height) = 0;

	//! 设置当前的深度缓冲区
	//! \param depthBuffer 要设置的深度缓冲区
	//! \return true成功 false失败
	virtual bool setDepthBuffer (ATOM_DepthBuffer *depthBuffer) = 0;

	//! 获取当前的深度缓冲区
	//! \return 当前的深度缓冲区或NULL
	virtual ATOM_DepthBuffer *getDepthBuffer (void) const = 0;

public:
    //! 应用Alpha融合渲染状态
	//! \param state 状态
	//! \param force true不检查Cache，直接设置
	//! \return true成功 false失败
	virtual bool applyAlphaBlendState (ATOM_AlphaBlendAttributes *state, int pass, bool force) = 0;

    //! 应用颜色缓冲区写入掩码渲染状态
	//! \param state 状态
	//! \param force true不检查Cache，直接设置
	//! \return true成功 false失败
	virtual bool applyColorWriteState (ATOM_ColorWriteAttributes *state, int pass, bool force) = 0;

    //! 应用深度缓冲区渲染状态
	//! \param state 状态
	//! \param force true不检查Cache，直接设置
	//! \return true成功 false失败
	virtual bool applyDepthState (ATOM_DepthAttributes *state, int pass, bool force) = 0;

    //! 应用模板缓冲区渲染状态
	//! \param state 状态
	//! \param force true不检查Cache，直接设置
	//! \return true成功 false失败
	virtual bool applyStencilState (ATOM_StencilAttributes *state, int pass, bool force) = 0;

    //! 应用光栅化渲染状态
	//! \param state 状态
	//! \param force true不检查Cache，直接设置
	//! \return true成功 false失败
	virtual bool applyRasterizerState (ATOM_RasterizerAttributes *state, int pass, bool force) = 0;

    //! 应用全屏抗锯齿渲染状态
	//! \param state 状态
	//! \param force true不检查Cache，直接设置
	//! \return true成功 false失败
	virtual bool applyMultisampleState (ATOM_MultisampleAttributes *state, int pass, bool force) = 0;

    //! 应用采样器渲染状态
	//! \param state 状态
	//! \param force true不检查Cache，直接设置
	//! \return true成功 false失败
	virtual bool applySamplerState (ATOM_SamplerAttributes *state, int index, int pass, bool force) = 0;

    //! 应用贴图渲染状态
	//! \param state 状态
	//! \param force true不检查Cache，直接设置
	//! \return true成功 false失败
	virtual bool applyTextureStageState (ATOM_TextureStageAttributes *state, int index, int pass, bool force) = 0;

    //! 应用雾效渲染状态
	//! \param state 状态
	//! \param force true不检查Cache，直接设置
	//! \return true成功 false失败
	virtual bool applyFogState (ATOM_FogAttributes *state, int pass, bool force) = 0;

    //! 应用alpha测试渲染状态
	//! \param state 状态
	//! \param force true不检查Cache，直接设置
	//! \return true成功 false失败
	virtual bool applyAlphaTestState (ATOM_AlphaTestAttributes *state, int pass, bool force) = 0;

	//! 同步cache中的渲染状态
    virtual void syncRenderStates(void) = 0;

public:
	//! 释放一个设备资源，内部使用
	//! \param resource 资源
    virtual void freeResource (ATOM_GfxResource *resource) = 0;

	//! 释放所有设备资源，内部使用
    virtual void freeAllResources (void) = 0;

	//! 从资源列表里摘除一个设备资源，内部使用
    virtual void removeResource (ATOM_GfxResource *resource) = 0;

	//! 添加一个设备资源到列表，内部使用
    virtual void insertResource (ATOM_GfxResource *resource) = 0;

	//! 从列表中删除所有资源，内部使用
    virtual void removeAllResources (void) = 0;
};

#endif // __ATOM3D_GFXDRIVER_GFXDRIVER_H
