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

	//! ����任����ģʽ
	enum TransformMode
	{
		//! �ڲ�ʹ��
		InvMatrixIndex = 1,
		//! �ڲ�ʹ��
		TransposeMatrixIndex = 2,
		//! �ڲ�ʹ��
		InvTransposeMatrixIndex = 3,

		//! ����任����ģ�Ϳռ�任������ռ䣩
		World = 0,
		//! ����任���棨����ռ�任��ģ�Ϳռ䣩
		InvWorld = World+InvMatrixIndex,
		//! ����任��ת�þ���
		TransposeWorld = World+TransposeMatrixIndex,
		//! ����任����ת�þ���
		InvTransposeWorld = World+InvTransposeMatrixIndex,
		//! ������任��������ռ�任��������ռ䣩
		View,
		//! ������任�������������ռ�任������ռ䣩
		InvView = View+InvMatrixIndex,
		//! ������任��ת�þ���
		TransposeView = View+TransposeMatrixIndex,
		//! ������任����ת�þ���
		InvTransposeView = View+InvTransposeMatrixIndex,
		//! ͶӰ�任����������ռ�任���ӿڿռ䣩
		Projection,
		//! ͶӰ�任��������ӿڿռ�任��������ռ䣩
		InvProjection = Projection+InvMatrixIndex,
		//! ͶӰ�任��ת�þ���
		TransposeProjection = Projection+TransposeMatrixIndex,
		//! ͶӰ�任����ת�þ���
		InvTransposeProjection = Projection+InvTransposeMatrixIndex,
		//! ����-������任����ģ�Ϳռ�任��������ռ䣬�˾�����ɶ�ȡ����ֱ�����ã�
		WorldView,
		//! ����-������任�������������ռ�任��ģ�Ϳռ䣬�˾�����ɶ�ȡ����ֱ�����ã�
		InvWorldView = WorldView+InvMatrixIndex,
		//! ����-������任��ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		TransposeWorldView = WorldView+TransposeMatrixIndex,
		//! ����-������任����ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTransposeWorldView = WorldView+InvTransposeMatrixIndex,
		//! �����-ͶӰ�任��������ռ�任���ӿڿռ䣬�˾�����ɶ�ȡ����ֱ�����ã�
		ViewProjection,
		//! �����-ͶӰ�任��������ӿڿռ�任������ռ䣬�˾�����ɶ�ȡ����ֱ�����ã�
		InvViewProjection = ViewProjection+InvMatrixIndex,
		//! �����-ͶӰ�任��ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		TransposeViewProjection = ViewProjection+TransposeMatrixIndex,
		//! �����-ͶӰ�任����ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTransposeViewProjection = ViewProjection+InvTransposeMatrixIndex,
		//! ����-�����-ͶӰ�任����ģ�Ϳռ�任���ӿڿռ䣬�˾�����ɶ�ȡ����ֱ�����ã�
		WorldViewProjection,
		//! ����-�����-ͶӰ�任��������ӿڿռ�任��ģ�Ϳռ䣬�˾�����ɶ�ȡ����ֱ�����ã�
		InvWorldViewProjection = WorldViewProjection+InvMatrixIndex,
		//! ����-�����-ͶӰ�任��ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		TransposeWorldViewProjection = WorldViewProjection+TransposeMatrixIndex,
		//! ����-�����-ͶӰ�任����ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTransposeWorldViewProjection = WorldViewProjection+InvTransposeMatrixIndex,
		//! ����0�任����
		Texture0,
		//! ����0�任���������󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTexture0 = Texture0+InvMatrixIndex,
		//! ����0�任�����ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		TransposeTexture0 = Texture0+TransposeMatrixIndex,
		//! ����0�任�������ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTransposeTexture0 = Texture0+InvTransposeMatrixIndex,
		//! ����1�任����
		Texture1,
		//! ����1�任���������󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTexture1 = Texture1+InvMatrixIndex,
		//! ����1�任�����ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		TransposeTexture1 = Texture1+TransposeMatrixIndex,
		//! ����1�任�������ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTransposeTexture1 = Texture1+InvTransposeMatrixIndex,
		//! ����2�任����
		Texture2,
		//! ����2�任���������󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTexture2 = Texture2+InvMatrixIndex,
		//! ����2�任�����ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		TransposeTexture2 = Texture2+TransposeMatrixIndex,
		//! ����2�任�������ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTransposeTexture2 = Texture2+InvTransposeMatrixIndex,
		//! ����3�任����
		Texture3,
		//! ����3�任���������󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTexture3 = Texture3+InvMatrixIndex,
		//! ����3�任�����ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		TransposeTexture3 = Texture3+TransposeMatrixIndex,
		//! ����3�任�������ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTransposeTexture3 = Texture3+InvTransposeMatrixIndex,
		//! ����4�任����
		Texture4,
		//! ����4�任���������󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTexture4 = Texture4+InvMatrixIndex,
		//! ����4�任�����ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		TransposeTexture4 = Texture4+TransposeMatrixIndex,
		//! ����4�任�������ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTransposeTexture4 = Texture4+InvTransposeMatrixIndex,
		//! ����5�任����
		Texture5,
		//! ����5�任���������󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTexture5 = Texture5+InvMatrixIndex,
		//! ����5�任�����ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		TransposeTexture5 = Texture5+TransposeMatrixIndex,
		//! ����5�任�������ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTransposeTexture5 = Texture5+InvTransposeMatrixIndex,
		//! ����6�任����
		Texture6,
		//! ����6�任���������󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTexture6 = Texture6+InvMatrixIndex,
		//! ����6�任�����ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		TransposeTexture6 = Texture6+TransposeMatrixIndex,
		//! ����6�任�������ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTransposeTexture6 = Texture6+InvTransposeMatrixIndex,
		//! ����7�任����
		Texture7,
		//! ����7�任���������󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTexture7 = Texture7+InvMatrixIndex,
		//! ����7�任�����ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		TransposeTexture7 = Texture7+TransposeMatrixIndex,
		//! ����7�任�������ת�þ��󣨴˾�����ɶ�ȡ����ֱ�����ã�
		InvTransposeTexture7 = Texture7+InvTransposeMatrixIndex
	};

public:
	//! ��λ��Ⱦ�豸.
	//! \return true �ɹ� false ʧ��
	virtual bool reset (void) = 0;

	//! ��ȡ��Ⱦ״̬������
	//! \return ��Ⱦ״̬������
	virtual ATOM_StateCache *getStateCache (void) const = 0;

	//! ���ö����Դ
	//! \param index �ƹ�����
	//! \param light �ƹ�ṹ
	//! \return true �ɹ� false ʧ��
	virtual bool setLight (unsigned index, const VertexLight *light) = 0;

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
	virtual bool setMaterial (const VertexLightMaterial *material) = 0;

	//! ��/�رն������
	//! \param enable true�� false�ر�
	//! \return true �ɹ� false ʧ��
	virtual bool enableVertexLighting (bool enable) = 0;

public:
	//! ���ö���ܵ��ı任����
	//! \param mode �任ģʽ
	//! \param matrix ����
	//! \return true�ɹ� falseʧ��
	virtual bool setTransform(TransformMode mode, const ATOM_Matrix4x4f &matrix) = 0;

	//! ��ȡ����ܵ��ı任����
	//! \param mode �任ģʽ
	//! \param matrix ����任����
	virtual const ATOM_Matrix4x4f &getTransform(TransformMode mode) const = 0;

	//! ��ȡ�豸��Ⱦ��������
	//! \return �豸��Ⱦ���������ṹ
	virtual const ATOM_GfxDriverCaps *getCapabilities (void) const = 0;

	//! ��ʼһ֡�µ���Ⱦ
	//! \return true�ɹ� falseʧ��
	//! \sa endFrame
	virtual bool begin(void) = 0;

	//! ������ǰ֡��Ⱦ
	//! \sa beginFrame
	//! \note ���beginFrame����false���������
	virtual void end(void) = 0;

	//! ���õ�ǰ��Ⱦ�������Ķ���
	//! \param vd ����������
	//! \return true�ɹ� false���ɹ�
	virtual bool setVertexDecl(VertexDecl vd) = 0;

	//! ���ö�����
	//! \param stream �����
	//! \param vertex_array ��������
	//! \param offset ��������Զ���������ʼλ�õ��ֽ�ƫ����
	//! \return true�ɹ� false���ɹ�
	virtual bool setStreamSource(unsigned stream, ATOM_VertexArray* vertex_array, unsigned offset = 0) = 0;

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

	//! ʹ��������Ⱦ�ڴ��еĶ�����
	//! \param type ͼԪ����
	//! \param index_count ��������
	//! \param vertex_range ��������
	//! \param attrib ����������
	//! \param stride ���������С
	//! \param stream ����������
	virtual void renderInterleavedMemoryStreamsIndexed (int type, int index_count, int vertex_range, unsigned attrib, unsigned stride, const void *stream, const unsigned short *indices) = 0;

	//! ��Ⱦ����
	//! \param font ����
	//! \param x ���ֻ��ߵ�x����
	//! \param y ���ֻ��ߵ�y����
	//! \param color ���ֵ���ɫ
	//! \param text ��ʽ���ı�
	virtual void outputText (ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color, const char *text, ...) = 0;

	//! ��ǰ�任������ջ
	//! \param mode �任ģʽ
	//! \return true�ɹ� falseʧ��
	virtual bool pushMatrix(TransformMode mode) = 0;

	//! �任�����ջ
	//! \param mode �任ģʽ
	//! \return true�ɹ� falseʧ��
	virtual bool popMatrix(TransformMode mode) = 0;

	//! ������Ⱦ״̬��Ĭ��
	virtual void loadDefaultRenderStates (void) = 0;

	//! ����һ���µ�δ��ʼ������ͼ����
	//! \param name ��������
	//! \param flags ��ͼ����
	//! \return �´�����δ��ʼ����ͼ����
	virtual ATOM_AUTOREF(ATOM_Texture) allocEmptyTexture(const char *name, unsigned flags) = 0;

	//! ����һ����ͼ���󲢳�ʼ��.
	//! �ɴ�����ά��ͼ�������ͼ����������ͼ������������ͼ������d��ʾ��ͼ��ȣ��������������ͼ��w��ʾ��ͼ�߳���h��d��ʹ�ã����ػ�����˳��Ϊǰ�����ϣ��£�����
	//! \param name ��������
	//! \param image ���ػ���������NULL
	//! \param w ��ͼ���
	//! \param h ��ͼ�߶�
	//! \param d ��ͼ��ȣ������ͼʹ�ô˲�����
	//! \param format ��ͼ�����ظ�ʽ
	//! \param flags ��ͼ����
	//! \return �´�������ͼ
	virtual ATOM_AUTOREF(ATOM_Texture) allocTexture(const char* name,
									  const void* image,
									  int w,
									  int h,
									  int d,
									  ATOM_PixelFormat format,
									  unsigned flags) = 0;

	//! ����һ�������ͼ���󲢳�ʼ��.
	//! \param name ��������
	//! \param width ��ͼ���
	//! \param height ��ͼ�߶�
	//! \return �´����������ͼ
	virtual ATOM_AUTOREF(ATOM_Texture) allocDepthTexture (const char *name, unsigned width, unsigned height, bool bUseDX9DSFormat = false ) = 0;

	//! ͨ���ļ�����һ����ͼ
	//! \param name ��������
	//! \param filename �ļ���
	//! \param format ��ͼ�����ظ�ʽ�����ΪATOM_PIXEL_FORMAT_UNKNOWN���Զ�����
	virtual ATOM_AUTOREF(ATOM_Texture) allocTextureFromFile (const char *name, const char *filename, ATOM_PixelFormat format = ATOM_PIXEL_FORMAT_UNKNOWN) = 0;

	//! ����һ����������
	//! \param name ��������
	//! \param attrib ����������
	//! \param usage ʹ��ģʽ
	//! \param size �������
	//! \return �����Ķ�������
	virtual ATOM_AUTOREF(ATOM_VertexArray) allocVertexArray(const char *name, unsigned attrib, int usage, unsigned size) = 0;

	//! ����һ����������
	//! \param name ��������
	//! \param usage ʹ��ģʽ
	//! \param size ��������
	//! \param use32bit trueʹ��32λ���� falseʹ��16λ����
	//! \return ��������������
	virtual ATOM_AUTOREF(ATOM_IndexArray) allocIndexArray(const char *name, int usage, unsigned size, bool use32bit) = 0;

	//! ����һ������������
	//! \param name ��������
	//! \return �����Ķ���
	virtual ATOM_AUTOREF(ATOM_Shader) allocVertexShader (const char *name) = 0;

	//! ����һ��Ƭ�γ������
	//! \param name ��������
	//! \return �����Ķ���
	virtual ATOM_AUTOREF(ATOM_Shader) allocPixelShader (const char *name) = 0;

	//! ����һ��GPU�������
	//! \param name ��������
	//! \return �����Ķ���
	virtual ATOM_AUTOREF(ATOM_GPUProgram) allocGPUProgram (const char *name) = 0;

	//! ����һ����Ⱦ״̬���϶���
	//! \param name ��������
	//! \return �����Ķ���
	virtual ATOM_AUTOREF(ATOM_RenderStateSet) allocRenderStateSet (const char *name) = 0;

	//! ����һ����Ⱦ״̬����
	//! \param stateType Ҫ��������Ⱦ״̬����
	//! \return ��������Ⱦ״̬����
	virtual ATOM_AUTOPTR(ATOM_RenderAttributes) allocRenderAttributes (ATOM_RenderAttributes::RenderState stateType) = 0;

	//! ����һ���ඥ������ʽ��������
	//! \param attrib ���ж��������Լ���
	//! \return ��ʽ��������
	virtual VertexDecl createMultiStreamVertexDeclaration (unsigned attrib) = 0;

	//! ����һ���ඥ������ʽ��������
	//! \param attrib0 ����������
	//! \return ��ʽ��������
	virtual VertexDecl createMultiStreamVertexDeclarationEx (unsigned attrib0, ...) = 0;

	//! ����һ������������ʽ��������
	//! \param attrib ����������
	//! \return ��ʽ��������
	virtual VertexDecl createInterleavedVertexDeclaration (unsigned attrib) = 0;

	//! ɾ��һ���Ѵ����Ķ�������ʽ��������
	//! \param vd ��������ʽ��������
	virtual void destroyVertexDecl (VertexDecl vd) = 0;

	//! ����һ����Ȼ�����
	//! \param name ��������
	//! \param width ��Ȼ��������
	//! \param height ��Ȼ������߶�
	//! \return ��Ȼ���������
	virtual ATOM_AUTOREF(ATOM_DepthBuffer) allocDepthBuffer (const char *name, unsigned width, unsigned height) = 0;

	//! ���õ�ǰ����Ȼ�����
	//! \param depthBuffer Ҫ���õ���Ȼ�����
	//! \return true�ɹ� falseʧ��
	virtual bool setDepthBuffer (ATOM_DepthBuffer *depthBuffer) = 0;

	//! ��ȡ��ǰ����Ȼ�����
	//! \return ��ǰ����Ȼ�������NULL
	virtual ATOM_DepthBuffer *getDepthBuffer (void) const = 0;

public:
    //! Ӧ��Alpha�ں���Ⱦ״̬
	//! \param state ״̬
	//! \param force true�����Cache��ֱ������
	//! \return true�ɹ� falseʧ��
	virtual bool applyAlphaBlendState (ATOM_AlphaBlendAttributes *state, int pass, bool force) = 0;

    //! Ӧ����ɫ������д��������Ⱦ״̬
	//! \param state ״̬
	//! \param force true�����Cache��ֱ������
	//! \return true�ɹ� falseʧ��
	virtual bool applyColorWriteState (ATOM_ColorWriteAttributes *state, int pass, bool force) = 0;

    //! Ӧ����Ȼ�������Ⱦ״̬
	//! \param state ״̬
	//! \param force true�����Cache��ֱ������
	//! \return true�ɹ� falseʧ��
	virtual bool applyDepthState (ATOM_DepthAttributes *state, int pass, bool force) = 0;

    //! Ӧ��ģ�建������Ⱦ״̬
	//! \param state ״̬
	//! \param force true�����Cache��ֱ������
	//! \return true�ɹ� falseʧ��
	virtual bool applyStencilState (ATOM_StencilAttributes *state, int pass, bool force) = 0;

    //! Ӧ�ù�դ����Ⱦ״̬
	//! \param state ״̬
	//! \param force true�����Cache��ֱ������
	//! \return true�ɹ� falseʧ��
	virtual bool applyRasterizerState (ATOM_RasterizerAttributes *state, int pass, bool force) = 0;

    //! Ӧ��ȫ���������Ⱦ״̬
	//! \param state ״̬
	//! \param force true�����Cache��ֱ������
	//! \return true�ɹ� falseʧ��
	virtual bool applyMultisampleState (ATOM_MultisampleAttributes *state, int pass, bool force) = 0;

    //! Ӧ�ò�������Ⱦ״̬
	//! \param state ״̬
	//! \param force true�����Cache��ֱ������
	//! \return true�ɹ� falseʧ��
	virtual bool applySamplerState (ATOM_SamplerAttributes *state, int index, int pass, bool force) = 0;

    //! Ӧ����ͼ��Ⱦ״̬
	//! \param state ״̬
	//! \param force true�����Cache��ֱ������
	//! \return true�ɹ� falseʧ��
	virtual bool applyTextureStageState (ATOM_TextureStageAttributes *state, int index, int pass, bool force) = 0;

    //! Ӧ����Ч��Ⱦ״̬
	//! \param state ״̬
	//! \param force true�����Cache��ֱ������
	//! \return true�ɹ� falseʧ��
	virtual bool applyFogState (ATOM_FogAttributes *state, int pass, bool force) = 0;

    //! Ӧ��alpha������Ⱦ״̬
	//! \param state ״̬
	//! \param force true�����Cache��ֱ������
	//! \return true�ɹ� falseʧ��
	virtual bool applyAlphaTestState (ATOM_AlphaTestAttributes *state, int pass, bool force) = 0;

	//! ͬ��cache�е���Ⱦ״̬
    virtual void syncRenderStates(void) = 0;

public:
	//! �ͷ�һ���豸��Դ���ڲ�ʹ��
	//! \param resource ��Դ
    virtual void freeResource (ATOM_GfxResource *resource) = 0;

	//! �ͷ������豸��Դ���ڲ�ʹ��
    virtual void freeAllResources (void) = 0;

	//! ����Դ�б���ժ��һ���豸��Դ���ڲ�ʹ��
    virtual void removeResource (ATOM_GfxResource *resource) = 0;

	//! ���һ���豸��Դ���б��ڲ�ʹ��
    virtual void insertResource (ATOM_GfxResource *resource) = 0;

	//! ���б���ɾ��������Դ���ڲ�ʹ��
    virtual void removeAllResources (void) = 0;
};

#endif // __ATOM3D_GFXDRIVER_GFXDRIVER_H
