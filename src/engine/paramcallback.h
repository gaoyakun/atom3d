#ifndef __ATOM_RENDER_PARAMCALLBACK_H
#define __ATOM_RENDER_PARAMCALLBACK_H

class ATOM_EffectParameterValue;

// apply callbacks
void stateParamCallback_AlphaBlendEnable (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_SrcBlend (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_DestBlend (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_BlendColor (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_ColorWriteRed (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_ColorWriteGreen (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_ColorWriteBlue (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_ColorWriteAlpha (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_DepthWriteEnable (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_DepthFunc (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilEnable (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilTwoSideEnable (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilReadMask (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilWriteMask (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilRef (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilFailOpFront (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilZFailOpFront (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilPassOpFront (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilFuncFront (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilFailOpBack (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilZFailOpBack (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilPassOpBack (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_StencilFuncBack (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_FillMode (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_CullMode (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_FrontFace (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_MultisampleEnable (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_AlphaToCoverageEnable (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_Filter (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_AddressU (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_AddressV (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_AddressW (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_MipmapLODBias (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_MaxAnisotropic (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_CompareFunc (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_BorderColor (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_MaxLOD (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_SRGBTexture (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_Texture (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_AlphaTestEnable (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_AlphaFunc (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_AlphaRef (ATOM_EffectParameterValue *param, void *context, void *context2);
void stateParamCallback_Shader (ATOM_EffectParameterValue *param, void *context, void *context2);

// value callbacks
void mvpMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void invMVPMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void transposeMVPMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void worldMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void invWorldMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void transposeWorldMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void viewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void invViewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void transposeViewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void projMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void invProjMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void transposeProjMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void modelViewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void invModelViewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void transposeModelViewMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void viewProjMatrixCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void worldSpaceEyePositionCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void objectSpaceEyePositionCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
void timeCallback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);

#endif // __ATOM_RENDER_PARAMCALLBACK_H
