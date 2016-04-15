<?xml version="1.0"?>
<material>
  <define name="NUM_POINT_LIGHT" value="4" />
  
  <param name="fogFactor_fs" type="float4" default="$Custom_FogFactor"/>
  <param name="fogColor_fs"  type="float4" default="$Custom_FogColor" />
  
  <param name="AmbientColorIntensity_fs" type="float4" default="$Custom_SceneAmbient"/>
  <param name="SunLightDir_fs" type="float4" default="$Custom_SunLightDir"/>
  <param name="SunLightColorIntensity_fs" type="float4" default="$Custom_SunLightColorIntensity"/>
  
  <!--
  <param name="PointLightPosition" type="float4[NUM_POINT_LIGHT]" default="$Custom_LightPosition"/>
  <param name="PointLightColorIntensity" type="float4[NUM_POINT_LIGHT]" default="$Custom_LightColorIntensity"/>
  <param name="PointLightAttenuation" type="float4[NUM_POINT_LIGHT]" default="$Custom_LightAttenuate"/>
  -->
  
  <code>
    <![CDATA[
	
		float4 getTexCoordFromClipSpace_fs (in float4 clipSpacePosition)
		{
		  float4 uv = clipSpacePosition;
		  //uv.x = ((clipSpacePosition.x + clipSpacePosition.w) * SizeParam_fs.x + clipSpacePosition.w) * SizeParam_fs.z;
		  //uv.y = ((clipSpacePosition.w - clipSpacePosition.y) * SizeParam_fs.y + clipSpacePosition.w) * SizeParam_fs.w;
			  
		  return uv;
		}
	
		float fresnel_term_fs (float3 light_vec, float3 half_vec, float spec)
		{
		  float t = saturate(dot(light_vec, half_vec));
		  return spec + (1-spec)*pow(1-t, 5);
		}
		float specular_normalize_factor_fs (float roughness)
		{
		  return (roughness+2.0)/8.0;
		}
		float roughness_term_fs (float3 half_vec, float3 normal, float roughness)
		{
		  return pow (saturate(dot(half_vec, normal)), roughness);
		}
		float roughness_term_fs (float spec, float3 light_vec, float3 half_vec, float3 normal, float roughness)
		{
		  return pow (saturate(dot(half_vec, normal)), roughness) * fresnel_term_fs (light_vec, half_vec, spec);
		}
		float specular_term_fs (float3 half_vec, float3 normal, float roughness)
		{
		  return specular_normalize_factor_fs(roughness) * roughness_term_fs(half_vec, normal, roughness);
		}
		float specular_term_fs (float spec, float3 light_vec, float3 half_vec, float3 normal, float roughness)
		{
		  return specular_normalize_factor_fs(roughness) * roughness_term_fs(spec, light_vec, half_vec, normal, roughness);
		}
		float3 calc_brdf_diffuse_only_fs (float3 diff, float3 l, float3 n)
		{
		  return saturate(diff * dot(n, l));
		}
		float3 calc_brdf_fs (float3 diff, float3 spec, float roughness, float3 l, float3 h, float3 n)
		{
		  return max((diff + specular_term_fs (spec, l, h, n, roughness)) * dot(n, l), 0);
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		float4 AmbientLighting()
		{
			return float4( AmbientColorIntensity_fs.xyz * AmbientColorIntensity_fs.w, 0 ); 
		}
		float DiffuseTerm( in float3 normal, in float halfLambertScale )
		{
			return saturate( saturate( dot( normalize(SunLightDir_fs.xyz), normal ) ) /* halfLambertScale + 1.0 - halfLambertScale*/ );
		}
		float SpecularTerm( in float3 normal, in float3 eyespacepos,  in float shininess )
		{
			float3 viewDir 	= normalize(eyespacepos);
			float3 half_vec = normalize( normalize(SunLightDir_fs.xyz) - viewDir );
			return roughness_term_fs( half_vec, normal, shininess );
		}
		float4 DiffuseLighting( in float3 normal, in float halfLambertScale )
		{
			float NdotL = saturate( saturate( dot( normalize(SunLightDir_fs.xyz), normal ) ) /* halfLambertScale + 1.0 - halfLambertScale*/ );
			return float4( SunLightColorIntensity_fs.rgb * SunLightColorIntensity_fs.a * NdotL, 1 );
		}
		float4 DirectionalLighting( in float3 normal, in float3 eyespacepos, in float halfLambertScale, in float shininess )
		{
			float3 lightDir_norm = normalize(SunLightDir_fs.xyz);
			float3 viewDir = normalize(eyespacepos);
			float3 half_vec = normalize( lightDir_norm - viewDir );
			float NdotL = saturate( saturate( dot( lightDir_norm, normal ) ) /* halfLambertScale + 1.0 - halfLambertScale*/ );
			float spec = roughness_term_fs( half_vec, normal, shininess );
				  
		    return float4( SunLightColorIntensity_fs.rgb, spec ) * SunLightColorIntensity_fs.a * NdotL;
		}
		
		/*
		float4 PointLighting( in int lightindex, in float3 normal, in float3 eyespacepos, in float halfLambertScale, in float shininess )
		{
			if( PointLightAttenuation[lightindex].w == 0 )
				return 0;
				
			float3 lightpos = PointLightPosition[lightindex].xyz;
			float4 lightColorIntensity = PointLightColorIntensity[lightindex];
			float3 lightAttenuation = PointLightAttenuation[lightindex].xyz;

			float3 lightvec = lightpos - eyespacepos;
			float lightDist = length (lightvec);
			lightvec /= lightDist;
			
			float NdotL = saturate( saturate( dot( lightvec.xyz, normal ) ) * halfLambertScale + 1.0 - halfLambertScale );
		  
			float3 viewDir = normalize(eyespacepos);
			float3 half_vec = normalize(lightvec - viewDir);
			float spec = roughness_term_fs( half_vec, normal, shininess );
			
			float atten = 1.0 / dot( lightAttenuation, float3(1, lightDist,lightDist * lightDist) );
		  
			//float shadowTerm = saturate( tex2D(sunShadowSampler, IN.uv).r );
		  
		    return float4( lightColorIntensity.rgb, spec ) * lightColorIntensity.a * NdotL * atten;
		}
		*/
		
		float4 ForwardLighting( in float3 normal, in float3 eyespacepos, in float halfLambertScale, in float shininess )
		{
			float3 normal_nr = normalize(normal);
			float4 lighting = AmbientLighting();
			lighting += DirectionalLighting(normal_nr,eyespacepos,halfLambertScale,shininess);
			
			//lighting += PointLighting(0,normal_nr,eyespacepos,halfLambertScale,shininess);
			//lighting += PointLighting(1,normal_nr,eyespacepos,halfLambertScale,shininess);
			//lighting += PointLighting(2,normal_nr,eyespacepos,halfLambertScale,shininess);
			//lighting += PointLighting(3,normal_nr,eyespacepos,halfLambertScale,shininess);
			
			return lighting;
		}
		
		float4 CalcFog( in float depth )
		{
			float fog = saturate( ( 1.0f - saturate( 1.0f - ( depth - fogFactor_fs.x ) * fogFactor_fs.y ) ) * fogFactor_fs.w );
			return float4( fogColor_fs.rgb * fog, 1.0 - fog );
		}
		void ApplyFog( inout float4 color, in float4 fog )
        {			
			color.rgb *= fog.a;
			color.rgb += fog.rgb;
        }
	
    ]]>
  </code>
</material>

