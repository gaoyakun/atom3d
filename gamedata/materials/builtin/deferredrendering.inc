<?xml version="1.0"?>
<material>
  <define prereq_type="DeviceCaps" prereq_name="SM30" name="SUPPORT_SM30" value="0"/>
  <define prereq_type="UserOption" prereq_name="AlbedoEncode" name="USE_Albedo_Encode" value="0"/>
  <param name="InvScreenSize" type="float4" default="$InvScreenSize"/>
  <param name="Gbuffer_DepthTexture" type="texture" default="$Gbuffer_DepthTexture"/>
  <param name="Gbuffer_AlbedoTexture" type="texture" default="$Gbuffer_AlbedoTexture"/>
  <param name="Gbuffer_NormalTexture" type="texture" default="$Gbuffer_NormalTexture"/>
  <param name="DeferredShadedTexture" type="texture" default="$Deferred_ShadedTexture"/>
  <param name="LightAccumulationTexture" type="texture" default="$Deferred_LightBuffer"/>
  <param name="FinalResultTexture" type="texture" default="$Deferred_CompositionTexture"/>
  <param name="SunShadowTexture" type="texture" default="$Deferred_SunShadowTexture"/>
  <param name="sunShadowSampler" type="sampler2D" >
    <filter value="lln"/>
    <addressu value="border"/>
    <addressv value="border"/>
    <bordercolor value="1,1,1,1"/>
    <texture value="$SunShadowTexture"/>
  </param>
  <param name="Gbuffer_DepthSampler" type="sampler2D" >
    <filter value="ppp"/>
    <addressu value="clamp"/>
    <addressv value="clamp"/>
    <texture value="$Gbuffer_DepthTexture"/>
  </param>
  <param name="Gbuffer_AlbedoSampler" type="sampler2D" >
    <filter value="ppp"/>
    <addressu value="clamp"/>
    <addressv value="clamp"/>
    <texture value="$Gbuffer_AlbedoTexture"/>
  </param>
  <param name="Gbuffer_NormalSampler" type="sampler2D" >
    <filter value="ppp"/>
    <addressu value="clamp"/>
    <addressv value="clamp"/>
    <texture value="$Gbuffer_NormalTexture"/>
  </param>
  <param name="LightAccumulationSampler" type="sampler2D" >
    <filter value="ppp"/>
    <addressu value="clamp"/>
    <addressv value="clamp"/>
    <texture value="$LightAccumulationTexture"/>
  </param>
  <param name="LightAccSRGBSampler" type="sampler2D" >
    <filter value="ppp"/>
    <addressu value="clamp"/>
    <addressv value="clamp"/>
	<srgb value="true" />
    <texture value="$LightAccumulationTexture"/>
  </param>
  <param name="DeferredShadedSampler" type="sampler2D" >
    <filter value="lln"/>
    <addressu value="clamp"/>
    <addressv value="clamp"/>
    <texture value="$DeferredShadedTexture"/>
  </param>
  <param name="FinalResultSampler" type="sampler2D" >
    <filter value="ppp"/>
    <addressu value="clamp"/>
    <addressv value="clamp"/>
    <texture value="$FinalResultTexture"/>
  </param>
  <param name="sizeParam" type="float4" default="$Deferred_SizeParam"/>
  <param name="linearDepthParam" type="float4" default="$Deferred_LinearizeDepthParam"/>
  <param name="viewAspect" type="float" default="$Camera_ViewAspect"/>
  <param name="invTanHalfFov" type="float" default="$Camera_InvTanHalfFov"/>
  <param name="useHDR" type="float" default="$Use_HDR"/>
  <param name="screenCoordScaleBias" type="float4" default="$Deferred_ScreenCoordScaleBias"/>
	<param name="SSSBRDFTexture" type="texture" default="/textures/sss/brdf_sss.png" editortype="none"/>
	<param name="SSSBRDFSampler" type="sampler2D" >
		<filter value="lln"/>
		<addressu value="border"/>
		<addressv value="border"/>
		<bordercolor value="1,1,1,1"/>
		<texture value="$SSSBRDFTexture"/>
		<srgb value="false" />
	</param>
  <code>
    <![CDATA[
	struct Gbuffer_PSOutput
	{
		float4 color0 : COLOR0;
		float4 color1 : COLOR1;
		//--- wangjian modified ---//
		// only 2 rt : RGBA8888
		//float4 color2 : COLOR2;
		//-------------------------//
	};

    // reset Gbuffer mrt layout
    void resetGbufferLayout (out Gbuffer_PSOutput output)
    {
      output.color0 = 0;
      output.color1 = 0;
	  //--- wangjian modified ---//
      //output.color2 = float4(1.f, 1.f, 1.f, 1.f);
	  output.color0.a = 1.0;
	  //-------------------------//
    }
	
	//--- wangjian added ---//
	
	half2 encodeNormal (half3 n)
	{
		n.z *= -1;
#if 0
		// stereo
		half scale = 1.7777;
		half2 enc = n.xy / (n.z+1);
		enc /= scale;
		enc = enc*0.5+0.5;
		return half4(enc,0,0);
		
#elif 1
		/* ce3
		half2 enc = normalize(n.xy) * (sqrt(-n.z*0.5+0.5));
		enc = enc*0.5+0.5;
		return enc;
		*/
		
		// lambert
		half p = sqrt(n.z*8+8);
		return half4(n.xy/p + 0.5,0,0);
#else
		// x & y
		return half4(n.xy*0.5+0.5,0,0);
#endif
	}
	half3 decodeNormal (half3 enc)
	{

#if 0
		// stereo
		half scale = 1.7777;
		half3 nn =
			enc.xyz*half3(2*scale,2*scale,0) +
			half3(-scale,-scale,1);
		half g = 2.0 / dot(nn.xyz,nn.xyz);
		half3 n;
		n.xy = g*nn.xy;
		n.z = -(g-1);
		return n;
		
#elif 1		
		/* //ce3
		half4 nn =
	    half4( enc,0)*half4(2,2,0,0) +
	    half4(-1,-1,1,-1);
		half l = dot(nn.xyz,-nn.xyw);
		nn.z = l;
		nn.xy *= sqrt(l);
		half3 res = nn.xyz * 2 + half3(0,0,-1);
		res.z *= -1;
		return res;
		*/
	
		// lambert
		half2 fenc 	= enc * 4-2;
		half f 		= dot( fenc,fenc );
		half g 		= sqrt( 1 - f / 4 );
		half3 n;
		n.xy 		= fenc * g;
		n.z 		= -( 1-f /2 );
		return n;
#else
		// x & y
		half3 n;
		n.xy = enc.xy*2-1;
		n.z = -sqrt(1-dot(n.xy, n.xy));
		return n;
#endif
	}
	//---------------------//
    
		// normalAndSpecularPower float4(normal.x, normal.y, normal.z, specularPower)
    void storeNormalAndSpecularPower (in float4 normalAndSpecularPower, inout Gbuffer_PSOutput output)
    {
		//--- wangjian modified ---//
		//output.color0 = normalAndSpecularPower;
		output.color0.rg = encodeNormal(normalAndSpecularPower.xyz);
		output.color0.b = normalAndSpecularPower / 256.0;
		//-------------------------//
    }

	// normalAndSpecularPower float3(normal.x, normal.y, normal.z) float specularPower
    void storeRawNormalAndSpecularPower(in float3 normal, in float specularpower, inout Gbuffer_PSOutput output)
    {
		//--- wangjian modified ---//
		output.color0.rg = encodeNormal(normal);
		output.color0.b = specularpower / 256.0;
		//-------------------------//
    }

    // albedoAndGlossness = float4(albedo.r, albedo.g, albedo.b, glossness)
    void storeAlbedoAndGlossness (in float4 albedoAndGlossness, inout Gbuffer_PSOutput output)
    {
		output.color1 	= albedoAndGlossness;
    }

    // albedoAndGlossness = float4(albedo.r, albedo.g, albedo.b, glossness)
    void storeRawAlbedoAndGlossness (in float3 albedo, in float glossness, inout Gbuffer_PSOutput output, in float4 screenCoord = float4(0,0,0,0) )
    {
	//#if SUPPORT_SM30 && USE_Albedo_Encode
	
	//	float Y = 0.299 * albedo.r + 0.587 * albedo.g + 0.114 * albedo.b;
	//	float Cb = 0.5 + ( -0.168 * albedo.r - 0.331 * albedo.g + 0.5 * albedo.b );
	//	float Cr = 0.5 + ( 0.5 * albedo.r - 0.418 * albedo.g - 0.081 * albedo.b );
		
	//	output.color1.r = Y;
		
		//output.color1.g = fmod( screenCoord.x,2.0 ) * Cb + ( 1 - fmod( screenCoord.x,2.0 ) ) * Cr;
		
	//	output.color1.g = (( fmod(screenCoord.x,2.0) == fmod(screenCoord.y,2.0) )) ? Cb : Cr;
		
	//	output.color1.a = glossness;
	//#else
	
		//--- wangjian modified ---//
		output.color1.rgb 	= albedo;
		output.color1.a 	= glossness;
		//-------------------------//
		
	//#endif
    }

    // aux0 = float4(Kd, Ks, SunShadow, N/A)
    void storeAux0 (float Kd, float Ks, float sunShadow, inout Gbuffer_PSOutput output)
    {
		//--- wangjian modified ---//
		//output.color2 = float4(Kd, Ks, sunShadow, 1.f);
		// if KD is < 0 , then use sss.
		output.color0.a = Kd * 0.5 + 0.5; 					//Kd * 1.0 / 16.0 + Ks * 1.0 / 256.0;		// Kd : 0 - 15 / Ks : 0 - 15
		//-------------------------//
    }
    
    // get depth value
    float getDepth (in float2 uv)
    {
		uv = uv * screenCoordScaleBias.xy + screenCoordScaleBias.zw;
		
		return tex2D(Gbuffer_DepthSampler, uv).r;
    }
    
    // get normal and specular power, return float3(normal.x, normal.y, normal.z, specularPower)
	//--- wangjian modified ---//
    float3 getNormalAndSpecularPower (	in float2 uv, 
										out float specPower,
										out float halfLambertScale	)
    {
		uv 						= uv * screenCoordScaleBias.xy + screenCoordScaleBias.zw;
		
		float4 normal_Sp 		= tex2D( Gbuffer_NormalSampler, uv );
		float3 normal 			= decodeNormal( float3( normal_Sp.xy, 0 ) );
		
		specPower 				= normal_Sp.z * 256.0;
		halfLambertScale 		= normal_Sp.w * 2 - 1;
				
		return normal;
    }
	//------------------------//

    // get albedo and glossness, return float4(albedo.r, albedo.g, albedo.b, glossness)
	//--- wangjian modified ---//
	float edge_filter(float2 center, float2 a0, float2 a1, float2 a2, float2 a3)
	{ 
	  const float THRESH = 50.0f/255.0f;
	  const float4 one_constant = float4( 1,1,1,1 );
	  float4 lum = float4( a0.x, a1.x, a2.x, a3.x );
	  float4 chrominance = float4( a0.y, a1.y, a2.y, a3.y );
	  
	  float4 weights = 1.0 - step( THRESH, abs( lum - center.xxxx ) ); 
	  float weight_sum = dot( weights, one_constant );

	  //Handle the special case where all the weights are zero.
	  //In HDR scenes it's better to set the chrominance to zero. 
	  //Here we just use the chrominance of the first neighbor.

	  weights.x = ( weight_sum == 0.0 ) ? 1.0 : weights.x; 
	  weight_sum = ( weight_sum == 0.0 ) ? 1.0 : weight_sum;

	  return dot( weights, chrominance ) / weight_sum;
	}
    float4 getAlbedoAndGlossness (in float2 uv, in float4 sc = float4(0,0,0,0) )
    {
	/*
	#if SUPPORT_SM30 && USE_Albedo_Encode
		
		float4 color = tex2D(Gbuffer_AlbedoSampler, uv);
		
		//edge-directed reconstruction:
		float2 a0 = tex2D(Gbuffer_AlbedoSampler, uv + float2(InvScreenSize.x,0.0)).rg;
		float2 a1 = tex2D(Gbuffer_AlbedoSampler, uv - float2(InvScreenSize.x,0.0)).rg;
		float2 a2 = tex2D(Gbuffer_AlbedoSampler, uv + float2(0.0,InvScreenSize.y)).rg;
		float2 a3 = tex2D(Gbuffer_AlbedoSampler, uv - float2(0.0,InvScreenSize.y)).rg;	
		float chroma = edge_filter(color.rg, a0, a1, a2, a3);
		bool pattern = ( fmod(sc.x,2.0) == fmod(sc.y,2.0) );
		float CB = (pattern) ? chroma : color.g;
		float CR = (pattern) ? color.g : chroma;
		
		color.rgb = float3( color.r + 1.402 * ( CB - 0.5 ), 
							color.r - 0.344 * ( CR - 0.5 ) - 0.714 * ( CB - 0.5 ), 
							color.r + 1.772 * ( CR - 0.5 ) );
							
		return color;
	#else
	*/
	
		uv = uv * screenCoordScaleBias.xy + screenCoordScaleBias.zw;
		
		return tex2D(Gbuffer_AlbedoSampler, uv);
	
	//#endif
    }
	//-------------------------//

    // get aux0
    float4 getAux0 (in float2 uv)
    {
		//--- wangjian modified ---//
		//return tex2D(Gbuffer_Aux0Sampler, uv);
		float KdKs = tex2D(Gbuffer_NormalSampler, uv).a * 16.0;
		float Ks = frac( KdKs );
		return float4( KdKs - Ks, Ks * 16.0, 1.0, 0 );
		
		//-------------------------//
    }
	
	float4 getLightAccum(in float2 uv)
	{
		uv = uv * screenCoordScaleBias.xy + screenCoordScaleBias.zw;
		float4 lightBuffer = tex2D(LightAccumulationSampler, uv);
		lightBuffer = -log2(lightBuffer);
		return lightBuffer;
	}

    // shading
	//--- wagnjian modified ---//
    float4 shade ( in float2 uv, in float4 sc = float4(0,0,0,0) )
    {
		//--- wangjian modified ---//
		float4 albedoAndGlossness = getAlbedoAndGlossness (uv,sc);
		float4 lightBuffer = getLightAccum(uv);
		
		// decode exp2(-lightBuffer)
		//if( useHDR == 0 )
		//	lightBuffer = -log2(lightBuffer);
			
		float4 combined = albedoAndGlossness * lightBuffer;
		float3 specular = combined.a /* 255.0*/;
		
		combined = float4( combined.rgb + specular.rgb, 1.0 );
		//---------------------------//
		
		return combined;
    }
	//--- wagnjian added ---//
    float4 shade_LDR( in float2 uv, in float4 sc = float4(0,0,0,0) )
    {
		float4 albedoAndGlossness = getAlbedoAndGlossness (uv,sc);
		float4 lightBuffer = tex2D(LightAccumulationSampler, uv);
		
		// decode exp2(-lightBuffer)
		//if( useHDR == 0 )
		//	lightBuffer = -log2(lightBuffer);
		
		float4 combined = albedoAndGlossness * lightBuffer;
		
		float3 vSample = combined.rgb + combined.a;
					
		return float4( vSample, 1.0 );
    }
	//-------------------------//
    
    // get shaded color
    float4 getShadedColor (in float2 uv)
    {
		uv = uv * screenCoordScaleBias.xy + screenCoordScaleBias.zw;
		
		return tex2D(DeferredShadedSampler, uv);
    }
    
    // get uv from clip space position
    // uniform sizeParam = float4(texture.width, texture.height, 0.5/texture.width, 0.5/texture.height)
    float4 getTexCoordFromClipSpace (in float4 clipSpacePosition)
    {
      float4 uv = clipSpacePosition;
      uv.x = ((clipSpacePosition.x + clipSpacePosition.w) * sizeParam.x + clipSpacePosition.w) * sizeParam.z;
      uv.y = ((clipSpacePosition.w - clipSpacePosition.y) * sizeParam.y + clipSpacePosition.w) * sizeParam.w;
	  	  
      return uv;
    }
    
    // linearize depth
    float linearizeDepth (float depth)
    {
      return linearDepthParam.x/(linearDepthParam.y-depth);
    }
    
    // get eye direction from UV
    float3 getEyeDirFromTexCoord (in float2 uv)
    {
      float x = uv.x * 2.f - 1.f;
      float y = (1.f - uv.y) * 2.f - 1.f;
      return float3 (x * viewAspect, y, invTanHalfFov);
    }
    
    // get eye space position from UV
    float3 getEyePosFromTexCoord (in float2 uv, in float3 eyedir)
    {
      float depth = linearizeDepth (getDepth (uv));
      return eyedir * depth / eyedir.z;
    }
    
    // get eye direction from aspect ratio and FOV
    float3 getEyeDirFromClipSpace (in float4 clipSpacePosition)
    {
      return float3 (clipSpacePosition.x * viewAspect, clipSpacePosition.y, invTanHalfFov);
    }
    
    ]]>
  </code>
</material>

