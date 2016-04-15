#include "StdAfx.h"
#include "paramcallback.h"

void stateParamCallback_AlphaBlendEnable (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_AlphaBlendAttributes *state = (ATOM_AlphaBlendAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->enableAlphaBlending (*param->i != 0);
		break;
	case ATOM_MaterialParam::ParamType_Float:
		state->enableAlphaBlending (*param->f != 0.f);
		break;
	default:
		break;
	}
}

void stateParamCallback_SrcBlend (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_AlphaBlendAttributes *state = (ATOM_AlphaBlendAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setSrcBlend ((ATOM_RenderAttributes::BlendFunc)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_DestBlend (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_AlphaBlendAttributes *state = (ATOM_AlphaBlendAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setDestBlend ((ATOM_RenderAttributes::BlendFunc)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_BlendColor (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_AlphaBlendAttributes *state = (ATOM_AlphaBlendAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Vector:
		{
			const ATOM_Vector4f &v = *param->v;
			state->setBlendConstant (ATOM_ColorARGB(v.x, v.y, v.z, v.w));
			break;
		}
	default:
		break;
	}
}

void stateParamCallback_ColorWriteRed (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_ColorWriteAttributes *state = (ATOM_ColorWriteAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		{
			unsigned mask = state->getColorWriteMask ();
			int value = *param->i;
			if (value)
				mask |= ATOM_RenderAttributes::ColorMask_Red;
			else
				mask &= ~ATOM_RenderAttributes::ColorMask_Red;
			state->setColorWriteMask (mask);
			break;
		}
	default:
		break;
	}
}

void stateParamCallback_ColorWriteGreen (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_ColorWriteAttributes *state = (ATOM_ColorWriteAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		{
			unsigned mask = state->getColorWriteMask ();
			int value = *param->i;
			if (value)
				mask |= ATOM_RenderAttributes::ColorMask_Green;
			else
				mask &= ~ATOM_RenderAttributes::ColorMask_Green;
			state->setColorWriteMask (mask);
			break;
		}
	default:
		break;
	}
}

void stateParamCallback_ColorWriteBlue (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_ColorWriteAttributes *state = (ATOM_ColorWriteAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		{
			unsigned mask = state->getColorWriteMask ();
			int value = *param->i;
			if (value)
				mask |= ATOM_RenderAttributes::ColorMask_Blue;
			else
				mask &= ~ATOM_RenderAttributes::ColorMask_Blue;
			state->setColorWriteMask (mask);
			break;
		}
	default:
		break;
	}
}

void stateParamCallback_ColorWriteAlpha (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_ColorWriteAttributes *state = (ATOM_ColorWriteAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		{
			unsigned mask = state->getColorWriteMask ();
			int value = *param->i;
			if (value)
				mask |= ATOM_RenderAttributes::ColorMask_Alpha;
			else
				mask &= ~ATOM_RenderAttributes::ColorMask_Alpha;
			state->setColorWriteMask (mask);
			break;
		}
	default:
		break;
	}
}

void stateParamCallback_DepthWriteEnable (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_DepthAttributes *state = (ATOM_DepthAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->enableDepthWrite (*param->i != 0);
		break;
	case ATOM_MaterialParam::ParamType_Float:
		state->enableDepthWrite (*param->f != 0.f);
		break;
	default:
		break;
	}
}

void stateParamCallback_DepthFunc (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_DepthAttributes *state = (ATOM_DepthAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setDepthFunc ((ATOM_RenderAttributes::CompareFunc)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilEnable (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->enableStencil (*param->i != 0);
		break;
	case ATOM_MaterialParam::ParamType_Float:
		state->enableStencil (*param->f != 0.f);
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilTwoSideEnable (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->enableTwoSidedStencil (*param->i != 0);
		break;
	case ATOM_MaterialParam::ParamType_Float:
		state->enableTwoSidedStencil (*param->f != 0.f);
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilReadMask (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setStencilReadMask (*param->i);
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilWriteMask (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setStencilWriteMask (*param->i);
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilRef (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setStencilRef (*param->i);
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilFailOpFront (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setStencilFailOpFront ((ATOM_RenderAttributes::StencilOp)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilZFailOpFront (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setStencilDepthFailOpFront ((ATOM_RenderAttributes::StencilOp)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilPassOpFront (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setStencilPassOpFront ((ATOM_RenderAttributes::StencilOp)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilFuncFront (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setStencilFuncFront ((ATOM_RenderAttributes::CompareFunc)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilFailOpBack (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setStencilFailOpBack ((ATOM_RenderAttributes::StencilOp)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilZFailOpBack (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setStencilDepthFailOpBack ((ATOM_RenderAttributes::StencilOp)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilPassOpBack (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setStencilPassOpBack ((ATOM_RenderAttributes::StencilOp)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_StencilFuncBack (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_StencilAttributes *state = (ATOM_StencilAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setStencilFuncBack ((ATOM_RenderAttributes::CompareFunc)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_FillMode (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_RasterizerAttributes *state = (ATOM_RasterizerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setFillMode ((ATOM_RenderAttributes::FillMode)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_CullMode (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_RasterizerAttributes *state = (ATOM_RasterizerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setCullMode ((ATOM_RenderAttributes::CullMode)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_FrontFace (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_RasterizerAttributes *state = (ATOM_RasterizerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setFrontFace ((ATOM_RenderAttributes::FrontFace)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_MultisampleEnable (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_MultisampleAttributes *state = (ATOM_MultisampleAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->enableMultisample (*param->i != 0);
		break;
	case ATOM_MaterialParam::ParamType_Float:
		state->enableMultisample (*param->f != 0.f);
		break;
	default:
		break;
	}
}

void stateParamCallback_AlphaToCoverageEnable (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_MultisampleAttributes *state = (ATOM_MultisampleAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->enableAlphaToCoverage (*param->i != 0);
		break;
	case ATOM_MaterialParam::ParamType_Float:
		state->enableAlphaToCoverage (*param->f != 0.f);
		break;
	default:
		break;
	}
}

void stateParamCallback_Filter (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_SamplerAttributes *state = (ATOM_SamplerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setFilter ((ATOM_RenderAttributes::SamplerFilter)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_AddressU (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_SamplerAttributes *state = (ATOM_SamplerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setAddressU ((ATOM_RenderAttributes::SamplerAddress)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_AddressV (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_SamplerAttributes *state = (ATOM_SamplerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setAddressV ((ATOM_RenderAttributes::SamplerAddress)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_AddressW (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_SamplerAttributes *state = (ATOM_SamplerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setAddressW ((ATOM_RenderAttributes::SamplerAddress)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_MipmapLODBias (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_SamplerAttributes *state = (ATOM_SamplerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setMipmapLodBias (*param->i);
		break;
	case ATOM_MaterialParam::ParamType_Float:
		state->setMipmapLodBias (*param->f);
		break;
	default:
		break;
	}
}

void stateParamCallback_MaxAnisotropic (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_SamplerAttributes *state = (ATOM_SamplerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setMaxAnisotropic (*param->i);
		break;
	case ATOM_MaterialParam::ParamType_Float:
		state->setMaxAnisotropic (*param->f);
		break;
	default:
		break;
	}
}

void stateParamCallback_CompareFunc (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_SamplerAttributes *state = (ATOM_SamplerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setCompareFunc ((ATOM_RenderAttributes::CompareFunc)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_BorderColor (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_SamplerAttributes *state = (ATOM_SamplerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Vector:
		{
			const ATOM_Vector4f &v = *param->v;
			state->setBorderColor (ATOM_ColorARGB(v.x, v.y, v.z, v.w));
			break;
		}
	default:
		break;
	}
}

void stateParamCallback_MaxLOD (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_SamplerAttributes *state = (ATOM_SamplerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setMaxLOD (*param->i);
		break;
	case ATOM_MaterialParam::ParamType_Float:
		state->setMaxLOD (*param->f);
		break;
	default:
		break;
	}
}

void stateParamCallback_SRGBTexture (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_SamplerAttributes *state = (ATOM_SamplerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Float:
		state->enableSRGBTexture (*param->f != 0.f);
		break;
	case ATOM_MaterialParam::ParamType_Int:
		state->enableSRGBTexture (*param->i != 0);
		break;
	default:
		break;
	}
}

void stateParamCallback_Texture (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_SamplerAttributes *state = (ATOM_SamplerAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Texture:
		state->setTexture (param->t);
		break;
	default:
		break;
	}
}

void stateParamCallback_AlphaTestEnable (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_AlphaTestAttributes *state = (ATOM_AlphaTestAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->enableAlphaTest (*param->i != 0);
		break;
	case ATOM_MaterialParam::ParamType_Float:
		state->enableAlphaTest (*param->f != 0.f);
		break;
	default:
		break;
	}
}

void stateParamCallback_AlphaFunc (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_AlphaTestAttributes *state = (ATOM_AlphaTestAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->enableAlphaTest ((ATOM_RenderAttributes::CompareFunc)(*param->i));
		break;
	default:
		break;
	}
}

void stateParamCallback_AlphaRef (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	ATOM_AlphaTestAttributes *state = (ATOM_AlphaTestAttributes*)context;
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Int:
		state->setAlphaRef ((*param->i)/255.f);
		break;
	case ATOM_MaterialParam::ParamType_Float:
		state->setAlphaRef (*param->f);
		break;
	default:
		break;
	}
}

void stateParamCallback_Shader (ATOM_EffectParameterValue *param, void *context, void *context2)
{
	if (context && context2)
	{
		ATOM_Shader *shader = (ATOM_Shader*)context;
		ATOM_Shader::ParamHandle handle = (ATOM_Shader::ParamHandle)context2;
	
		switch (param->handle->getParamType ())
		{
		case ATOM_MaterialParam::ParamType_Float:
			shader->setFloat (handle, *param->f);
			//shader->setFloat (handle, ((ATOM_MaterialFloatParam*)param)->getValue());
			break;
		case ATOM_MaterialParam::ParamType_FloatArray:
			shader->setFloatArray (handle, param->f, param->dimension);
			//shader->setFloatArray (handle, ((ATOM_MaterialFloatArrayParam*)param)->getValue(), ((ATOM_MaterialFloatArrayParam*)param)->getDimension());
			break;
		case ATOM_MaterialParam::ParamType_Int:
			shader->setInt (handle, *param->i);
			//shader->setInt (handle, ((ATOM_MaterialIntParam*)param)->getValue());
			break;
		case ATOM_MaterialParam::ParamType_IntArray:
			shader->setIntArray (handle, param->i, param->dimension);
			//shader->setIntArray (handle, ((ATOM_MaterialIntArrayParam*)param)->getValue(), ((ATOM_MaterialIntArrayParam*)param)->getDimension());
			break;
		case ATOM_MaterialParam::ParamType_Vector:
			shader->setVector (handle, *param->v);
			//shader->setVector (handle, ((ATOM_MaterialVectorParam*)param)->getValue());
			break;
		case ATOM_MaterialParam::ParamType_VectorArray:
			shader->setVectorArray (handle, param->v, param->dimension);
			//shader->setVectorArray (handle, ((ATOM_MaterialVectorArrayParam*)param)->getValue(), ((ATOM_MaterialVectorArrayParam*)param)->getDimension());
			break;
		case ATOM_MaterialParam::ParamType_Matrix44:
			shader->setMatrix44 (handle, *param->m44);
			//shader->setMatrix44 (handle, ((ATOM_MaterialMatrix44Param*)param)->getValue());
			break;
		case ATOM_MaterialParam::ParamType_Matrix44Array:
			shader->setMatrix44Array (handle, param->m44, param->dimension);
			//shader->setMatrix44Array (handle, ((ATOM_MaterialMatrix44ArrayParam*)param)->getValue(), ((ATOM_MaterialMatrix44ArrayParam*)param)->getDimension());
			break;
		case ATOM_MaterialParam::ParamType_Matrix43:
			shader->setMatrix43 (handle, *param->m43);
			//shader->setMatrix43 (handle, ((ATOM_MaterialMatrix43Param*)param)->getValue());
			break;
		case ATOM_MaterialParam::ParamType_Matrix43Array:
			shader->setMatrix43Array (handle, param->m43, param->dimension);
			//shader->setMatrix43Array (handle, ((ATOM_MaterialMatrix43ArrayParam*)param)->getValue(), ((ATOM_MaterialMatrix43ArrayParam*)param)->getDimension());
			break;
		default:
			break;
		}
	}
}

void mvpMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_MVP, *param->m44);

	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue (vsConstantPtr, param->m44, descVS);
		//((ATOM_Matrix4x4f*)vsConstantPtr)->transposeFrom (*param->m44);
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue (psConstantPtr, param->m44, descPS);
		//((ATOM_Matrix4x4f*)psConstantPtr)->transposeFrom (*param->m44);
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_MVP, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
}

void invMVPMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_INV_MVP, *param->m44);
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//((ATOM_Matrix4x4f*)vsConstantPtr)->transposeFrom (*param->m44);
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//((ATOM_Matrix4x4f*)psConstantPtr)->transposeFrom (*param->m44);
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_INV_MVP, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
}

void transposeMVPMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_MVP, *param->m44);
	param->m44->transpose ();

	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//*((ATOM_Matrix4x4f*)vsConstantPtr) = *param->m44;
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//*((ATOM_Matrix4x4f*)psConstantPtr) = *param->m44;
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_MVP, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
	//((ATOM_Matrix4x4f*)param->getValuePtr())->transpose ();
}

void worldMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_WORLD, *param->m44);
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue (vsConstantPtr, param->m44, descVS);
		//((ATOM_Matrix4x4f*)vsConstantPtr)->transposeFrom (*param->m44);
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue (psConstantPtr, param->m44, descPS);
		//((ATOM_Matrix4x4f*)psConstantPtr)->transposeFrom (*param->m44);
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_WORLD, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
}

void invWorldMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_INV_WORLD, *param->m44);
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue (vsConstantPtr, param->m44, descVS);
		//((ATOM_Matrix4x4f*)vsConstantPtr)->transposeFrom (*param->m44);
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue (psConstantPtr, param->m44, descPS);
		//((ATOM_Matrix4x4f*)psConstantPtr)->transposeFrom (*param->m44);
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_INV_WORLD, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
}

void transposeWorldMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_WORLD, *param->m44);
	param->m44->transpose ();
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue (vsConstantPtr, param->m44, descVS);
		//*((ATOM_Matrix4x4f*)vsConstantPtr) = *param->m44;
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue (psConstantPtr, param->m44, descPS);
		//*((ATOM_Matrix4x4f*)psConstantPtr) = *param->m44;
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_WORLD, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
	//((ATOM_Matrix4x4f*)(param->getValuePtr()))->transpose ();
}

void viewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_VIEW, *param->m44);
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//((ATOM_Matrix4x4f*)vsConstantPtr)->transposeFrom (*param->m44);
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//((ATOM_Matrix4x4f*)psConstantPtr)->transposeFrom (*param->m44);
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_VIEW, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
}

void invViewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_INV_VIEW, *param->m44);
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//((ATOM_Matrix4x4f*)vsConstantPtr)->transposeFrom (*param->m44);
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//((ATOM_Matrix4x4f*)psConstantPtr)->transposeFrom (*param->m44);
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_INV_VIEW, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
}

void transposeViewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_VIEW, *param->m44);
	param->m44->transpose ();
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//*((ATOM_Matrix4x4f*)vsConstantPtr) = *param->m44;
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//*((ATOM_Matrix4x4f*)psConstantPtr) = *param->m44;
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_VIEW, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
	//((ATOM_Matrix4x4f*)(param->getValuePtr()))->transpose ();
}

void projMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_PROJECTION, *param->m44);
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//((ATOM_Matrix4x4f*)vsConstantPtr)->transposeFrom (*param->m44);
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//((ATOM_Matrix4x4f*)psConstantPtr)->transposeFrom (*param->m44);
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_PROJECTION, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
}

void invProjMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_INV_PROJECTION, *param->m44);
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//((ATOM_Matrix4x4f*)vsConstantPtr)->transposeFrom (*param->m44);
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//((ATOM_Matrix4x4f*)psConstantPtr)->transposeFrom (*param->m44);
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_INV_PROJECTION, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
}

void transposeProjMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_PROJECTION, *param->m44);
	param->m44->transpose ();
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//*((ATOM_Matrix4x4f*)vsConstantPtr) = *param->m44;
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//*((ATOM_Matrix4x4f*)psConstantPtr) = *param->m44;
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_PROJECTION, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
	//((ATOM_Matrix4x4f*)(param->getValuePtr()))->transpose ();
}

void modelViewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_MODELVIEW, *param->m44);
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//((ATOM_Matrix4x4f*)vsConstantPtr)->transposeFrom (*param->m44);
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//((ATOM_Matrix4x4f*)psConstantPtr)->transposeFrom (*param->m44);
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_MODELVIEW, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
}

void invModelViewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_INV_MODELVIEW, *param->m44);
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//((ATOM_Matrix4x4f*)vsConstantPtr)->transposeFrom (*param->m44);
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//((ATOM_Matrix4x4f*)psConstantPtr)->transposeFrom (*param->m44);
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_INV_MODELVIEW, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
}

void transposeModelViewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_MODELVIEW, *param->m44);
	param->m44->transpose ();
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//*((ATOM_Matrix4x4f*)vsConstantPtr) = *param->m44;
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//*((ATOM_Matrix4x4f*)psConstantPtr) = *param->m44;
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_MODELVIEW, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
	//((ATOM_Matrix4x4f*)(param->getValuePtr()))->transpose ();
}

void viewProjMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Matrix44);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_VIEWPROJ, *param->m44);
	if (vsConstantPtr)
	{
		param->handle->writeRegisterValue(vsConstantPtr, param->m44, descVS);
		//((ATOM_Matrix4x4f*)vsConstantPtr)->transposeFrom (*param->m44);
	}
	if (psConstantPtr)
	{
		param->handle->writeRegisterValue(psConstantPtr, param->m44, descPS);
		//((ATOM_Matrix4x4f*)psConstantPtr)->transposeFrom (*param->m44);
	}
	//ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_VIEWPROJ, *((ATOM_Matrix4x4f*)(param->getValuePtr())));
}

void worldSpaceEyePositionCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Vector || (param->handle->getParamType() == ATOM_MaterialParam::ParamType_FloatArray && param->handle->getDimension() >= 3));

	ATOM_Matrix4x4f viewMatrix;
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_VIEW, viewMatrix);
	param->v->set (viewMatrix.m30, viewMatrix.m31, viewMatrix.m32, 1.f);

	if (vsConstantPtr)
	{
		*vsConstantPtr = *param->v;
	}

	if (psConstantPtr)
	{
		*psConstantPtr = *param->v;
	}
	//memcpy (param->getValuePtr(), &viewMatrix.m[3*4], sizeof(float)*3);
}

void objectSpaceEyePositionCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Vector || (param->handle->getParamType() == ATOM_MaterialParam::ParamType_FloatArray && param->handle->getDimension() >= 3));

	ATOM_Matrix4x4f viewMatrix, invWorldMatrix;
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_VIEW, viewMatrix);
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_INV_WORLD, invWorldMatrix);
	ATOM_Vector4f pos = viewMatrix.getRow (3);
	pos <<= invWorldMatrix;
	*param->v = pos;

	if (vsConstantPtr)
	{
		*vsConstantPtr = *param->v;
	}

	if (psConstantPtr)
	{
		*psConstantPtr = *param->v;
	}
	//memcpy (param->getValuePtr(), pos.xyzw, sizeof(float)*3);
}

void timeCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_ASSERT(param->handle->getParamType() == ATOM_MaterialParam::ParamType_Float);
	float val = ATOM_APP->getFrameStamp().currentTick * 0.001f;
	param->setFloat (val);

	if (vsConstantPtr)
	{
		vsConstantPtr->set (val, val, val, val);
	}
	
	if (psConstantPtr)
	{
		psConstantPtr->set (val, val, val, val);
	}
	//((ATOM_MaterialFloatParam*)param)->setValue (ATOM_APP->getFrameStamp().currentTick * 0.001f);
}

