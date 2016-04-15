<?xml version="1.0"?>
<material>
	<param name="sssss" type="int" default="0" editortype="bool" />
	<param name="depthThreshold" type="float" default="1" editortype="none" />
	<param name="scatterRadius" type="float" default="2" editortype="none" />
	<param name="rimStrength" type="float" default="0.0" editortype="none" />
	<param name="rimPower" type="float" default="3" editortype="none" />
	<param name="rimColor" type="float4" default="1.0,1.0,1.0,1.0" editortype="color" />
	<param name="sssLayerColor0" type="float4" default="0.233,0.455,0.649,1.0" editortype="color" />
	<param name="sssLayerColor1" type="float4" default="0.218,0.545,0.351,1.0" editortype="color" />
	<param name="sssLayerColor2" type="float4" default="0.358,0.000,0.0,1.0" editortype="color" />
	<param name="sssLayerColor3" type="float4" default="0.191,0.0,0.0,1.0" editortype="color" />
	<param name="skinMaskTexture" type="texture" default="/textures/default/white.dds" editortype="none" />
	<param name="skinMaskSampler" type="sampler2D">
        <texture value="$skinMaskTexture" />
        <filter value="ppn" />
		<lodbias value="0" />
        <addressu value="wrap" />
        <addressv value="wrap" />
        <srgb value="false" />
    </param>
  <code>
    <![CDATA[
		
		half3 SampleAccumulatedIrradiance( half3 cDiffuse, half2 vTapTC, float fCenterTapDepth, half fThreshold )
		{
			half4 cIrradianceTap = getLightAccum(vTapTC);//tex2D( LightAccumulationSampler, vTapTC ).xyzw;						// 1 tex
			
		#if 0
			// decode exp2(-lightinfo)
			cIrradianceTap = -log2(cIrradianceTap);
		#endif
		
			float fTapDepth = linearizeDepth (getDepth (vTapTC));
			float fTapMask 	= saturate( abs(fTapDepth - fCenterTapDepth) * fThreshold );					// 4 alu
			return fTapMask * (cDiffuse - cIrradianceTap) + cIrradianceTap;								// 2 alu
		}
		
		half GetFresnel(half NdotI, half bias, half power)
		{
			half facing = (1.0 - NdotI);
			return saturate( bias + (1-bias) * pow(facing, power) );
		}
		
		// assumes 0 is min
		half smoothstep_opt(in half maxi, in half x)
		{
			x = saturate( x / maxi );
			return  x * x  * (3.0 - 2.0 * x);
		}
		
		// calc sssss
		float4 calcSSSSS( in float4 screenCoord )
		{
			// Sample taps
			const half2 vSampleTaps[11] =
			{
				half2(0.0, -1.0), 
				half2(-0.866, 0.5), 
				half2(0.866, 0.5), 
				half2(-1.414, -1.414), 
				half2(-1.414, 1.414),
				half2(1.414, 1.414), 
				half2(1.414, -1.414), 
				half2(-4.0, 0.0), 
				half2(4.0, 0.0), 
				half2(0.0, 4.0), 
				half2(0.0, -4.0)
			};
			
			// Sample weights
			const half3 cSkinWeights[4] =
			{
				half3(0.233, 0.455, 0.649),
				half3(0.218, 0.545, 0.351),
				half3(0.358, 0.000, 0.000),
				half3(0.191, 0.000, 0.000),
			};

			// Constants
			const float2 tcProj = screenCoord.xy / screenCoord.w; // 2 alu
			
			half4 lightinfo = getLightAccum(tcProj);//tex2D( LightAccumulationSampler, tcProj );
			
		#if 0
			// decode exp2(-lightinfo)
			lightinfo = -log2(lightinfo);
		#endif
		
			half3 cDiffuse = lightinfo.xyz;
			
			const float fCenterTapDepth = screenCoord.w;//linearizeDepth (getDepth (tcProj));
			//const half fThreshold = 1.0f;
			float fThreshold = depthThreshold;

			// Don't scale by pixel size! Scale is in world-coords and needs to be consistent across all resolutions/FOVs!
			const float2 vKernelScale = 0.00035h * ( scatterRadius /* g_PS_ProjRatio.z*/) / fCenterTapDepth; // 4 alu

			// Sample the first layer.
			half3 cDiffuseLayer0 = cDiffuse;
			cDiffuseLayer0 += SampleAccumulatedIrradiance( cDiffuse, tcProj + vSampleTaps[0] * vKernelScale, fCenterTapDepth, fThreshold );
			cDiffuseLayer0 += SampleAccumulatedIrradiance( cDiffuse, tcProj + vSampleTaps[1] * vKernelScale, fCenterTapDepth, fThreshold );
			cDiffuseLayer0 += SampleAccumulatedIrradiance( cDiffuse, tcProj + vSampleTaps[2] * vKernelScale, fCenterTapDepth, fThreshold );
			cDiffuseLayer0 *= 0.25h; // 1 alu

			// Sample the second layer.
			half3 cDiffuseLayer1 = cDiffuseLayer0;
			cDiffuseLayer1 += SampleAccumulatedIrradiance( cDiffuse, tcProj + vSampleTaps[3] * vKernelScale, fCenterTapDepth, fThreshold );
			cDiffuseLayer1 += SampleAccumulatedIrradiance( cDiffuse, tcProj + vSampleTaps[4] * vKernelScale, fCenterTapDepth, fThreshold );
			cDiffuseLayer1 += SampleAccumulatedIrradiance( cDiffuse, tcProj + vSampleTaps[5] * vKernelScale, fCenterTapDepth, fThreshold );
			cDiffuseLayer1 += SampleAccumulatedIrradiance( cDiffuse, tcProj + vSampleTaps[6] * vKernelScale, fCenterTapDepth, fThreshold );		
			cDiffuseLayer1 *= 0.2h; // 1 alu

			// Sample the third layer.
			half3 cDiffuseLayer2 = cDiffuseLayer1;
			cDiffuseLayer2 += SampleAccumulatedIrradiance( cDiffuse, tcProj + vSampleTaps[7] * vKernelScale, fCenterTapDepth, fThreshold );
			cDiffuseLayer2 += SampleAccumulatedIrradiance( cDiffuse, tcProj + vSampleTaps[8] * vKernelScale, fCenterTapDepth, fThreshold );
			cDiffuseLayer2 += SampleAccumulatedIrradiance( cDiffuse, tcProj + vSampleTaps[9] * vKernelScale, fCenterTapDepth, fThreshold );
			cDiffuseLayer2 += SampleAccumulatedIrradiance( cDiffuse, tcProj + vSampleTaps[10] * vKernelScale, fCenterTapDepth, fThreshold );
			cDiffuseLayer2 *= 0.2h; // 1 alu

		#if 0
			// Combine all layers with respective weights.
			cDiffuse = 	cDiffuse 		* cSkinWeights[0] + 
						cDiffuseLayer0 	* cSkinWeights[1] + 
						cDiffuseLayer1 	* cSkinWeights[2] + 
						cDiffuseLayer2 	* cSkinWeights[3]; // 4 alu
		#else
		
			// Combine all layers with respective weights.
			cDiffuse = 	cDiffuse 		* sssLayerColor0.xyz + 
						cDiffuseLayer0 	* sssLayerColor1.xyz + 
						cDiffuseLayer1 	* sssLayerColor2.xyz + 
						cDiffuseLayer2 	* sssLayerColor3.xyz; // 4 alu
		#endif
						
			//cDiffuse = cDiffuse 		* cSkinWeights[0];
			
			return float4( cDiffuse, lightinfo.w );
		}
		
		// calc rim
		float3 calcRim(in float3 eyedir_vs, in float3 normal_vs, in float3 rimClr )
		{
			// Pre-computed rim term
			float3 normalVS = normal_vs;
			normalVS *= 0.5 - sign(normalVS.z);
			half fNdotE 	= 	saturate( dot( normalize( -eyedir_vs.xyz ), normalize( normalVS.xyz ) ) );
			half fresnel 	=  	GetFresnel( fNdotE, 0, rimPower );
			half rim 		= 	smoothstep_opt( 1.0, fresnel ) * rimStrength;
			return rim * rimClr;
		}
	
    ]]>
  </code>
</material>

