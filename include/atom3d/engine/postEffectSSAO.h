#ifndef _POST_EFFECT_SSAO_H_
#define _POST_EFFECT_SSAO_H_


#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

#define _GAUSSIAN_TAP_COUNT_FINAL 5/*9*//*15*/

class ATOM_ENGINE_API ATOM_SSAOEffect : public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_SSAOEffect, ATOM_SSAOEffect)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_SSAOEffect)

public:
	//! 构造函数
	ATOM_SSAOEffect (void);

	//! 析构函数
	virtual ~ATOM_SSAOEffect (void);

public:
	virtual bool render (ATOM_RenderDevice *device);

	virtual bool init(ATOM_RenderDevice *device);
	virtual bool destroy();

	ATOM_Texture * getAOBuffer() const;

	virtual void debugDraw(int b);

	// 属性设置
public:

	void setAoAmount(const float amount);
	const float getAoAmount(void) const;

	void setAoRadius(const float radius);
	const float getAoRadius(void) const;

	void setDistanceThreshold(const float bias);
	const float getDistanceThreshold(void) const;

	void setAoBias(const float bias);
	const float getAoBias(void) const;

	void setAoBlur(const int blur);
	const int getAoBlur(void) const;

	void setProjScale(const float scale);
	const float getProjScale(void) const;

	void setBilateralDepthThresh(const float threshold);
	const float getBilateralDepthThresh(void) const;

	void setMipLogMaxOffset(const float blur);
	const float getMipLogMaxOffset(void) const;

	void setUseDownScaledBlur(const int downscale);
	const int getUseDownScaledBlur(void) const;

	void setRadiusParam(const ATOM_Vector4f & aoRadiusParam);
	const ATOM_Vector4f & getRadiusParam(void) const;

private:

	bool createRT(ATOM_RenderDevice *device);
	bool createMaterial(ATOM_RenderDevice *device);

	void computeCSZ(ATOM_RenderDevice *device);
	void downSampleDepth(ATOM_RenderDevice *device);
	void genRawAO(ATOM_RenderDevice *device);
	void blur(ATOM_RenderDevice *device);
	void applyAO(ATOM_RenderDevice *device);

	bool getSampleOffsets_Blur (unsigned texSize, float texCoordOffset[15], ATOM_Vector4f *colorWeight, float deviation, float multiplier);

private:

	int								_useDownScaledBlur;			// 使用DOWNSCALED的模糊BUFFER
	bool							_recreateRT;				// 重新创建RT

	float							_aoAmount;
	float							_aoRadius;
	
	float							_distanceThreshold;
	float							_aoBias;			
	int								_aoblur;
	float							_mipLogMaxOffset;
	float							_projScale;
	float							_bilateralDepthThresh;
	ATOM_Vector4f					_aoRadiusParam;

	
	ATOM_AUTOPTR(ATOM_Material)		_material;
	bool _materialError;
	ATOM_AUTOREF(ATOM_Texture)		_randomRotateTex;
	ATOM_AUTOREF(ATOM_Texture)		_xor256Tex;

	/** This must be greater than or equal to the MAX_MIP_LEVEL and  defined in SAO_AO.pix. */
#define MAX_MIP_LEVEL (5)

	ATOM_AUTOREF(ATOM_Texture)		_depthBufferMips[MAX_MIP_LEVEL];	//	
	ATOM_AUTOREF(ATOM_Texture)		_cszBuffer;							// 		
	ATOM_AUTOREF(ATOM_Texture)		_downScaledDepth;					// down scaled depth texture 
	ATOM_AUTOREF(ATOM_Texture)		_AORawBuffer;						// AO buffer
	ATOM_AUTOREF(ATOM_Texture)		_AOBuffer[2];						// AO buffer
	

	unsigned						_rtWidth;
	unsigned						_rtHeight;
	ATOM_Vector2f avSampleOffsets_horizontal[_GAUSSIAN_TAP_COUNT_FINAL];
	ATOM_Vector4f avSampleWeights_horizontal[_GAUSSIAN_TAP_COUNT_FINAL];
	ATOM_Vector2f avSampleOffsets_vertical[_GAUSSIAN_TAP_COUNT_FINAL];
	ATOM_Vector4f avSampleWeights_vertical[_GAUSSIAN_TAP_COUNT_FINAL];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
