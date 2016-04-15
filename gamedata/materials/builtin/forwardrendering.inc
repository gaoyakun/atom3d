<?xml version="1.0"?>
<material>
  <define name="NUM_POINT_LIGHT" value="4" />
  <param name="AmbientColorIntensity" type="float4" default="$Custom_SceneAmbient"/>
  <param name="SunLightDir" type="float4" default="$Custom_SunLightDir"/>
  <param name="SunLightColorIntensity" type="float4" default="$Custom_SunLightColorIntensity"/>
  
  <!--
  <param name="PointLightPosition" type="float4[NUM_POINT_LIGHT]" default="$Custom_LightPosition"/>
  <param name="PointLightColorIntensity" type="float4[NUM_POINT_LIGHT]" default="$Custom_LightColorIntensity"/>
  <param name="PointLightAttenuation" type="float4[NUM_POINT_LIGHT]" default="$Custom_LightAttenuate"/>
  -->
  
  <param name="cameraNearFarFactor" type="float4" default="$Custom_LightAttenuate"/>
  <code>
    <![CDATA[
	
		float fresnel_term (float3 light_vec, float3 half_vec, float spec)
		{
		  float t = saturate(dot(light_vec, half_vec));
		  return spec + (1-spec)*pow(1-t, 5);
		}
		float specular_normalize_factor (float roughness)
		{
		  return (roughness+2.0)/8.0;
		}
		float roughness_term (float3 half_vec, float3 normal, float roughness)
		{
		  return pow (saturate(dot(half_vec, normal)), roughness);
		}
		float roughness_term (float spec, float3 light_vec, float3 half_vec, float3 normal, float roughness)
		{
		  return pow (saturate(dot(half_vec, normal)), roughness) * fresnel_term (light_vec, half_vec, spec);
		}
		float specular_term (float3 half_vec, float3 normal, float roughness)
		{
		  return specular_normalize_factor(roughness)*roughness_term(half_vec, normal, roughness);
		}
		float specular_term (float spec, float3 light_vec, float3 half_vec, float3 normal, float roughness)
		{
		  return specular_normalize_factor(roughness)*roughness_term(spec, light_vec, half_vec, normal, roughness);
		}
		float3 calc_brdf_diffuse_only (float3 diff, float3 l, float3 n)
		{
		  return saturate(diff * dot(n, l));
		}
		float3 calc_brdf (float3 diff, float3 spec, float roughness, float3 l, float3 h, float3 n)
		{
		  return max((diff + specular_term (spec, l, h, n, roughness)) * dot(n, l), 0);
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		float4 AmbientLighting()
		{
			return float4( AmbientColorIntensity.xyz * AmbientColorIntensity.w, 0 ); 
		}
		float4 DirectionalLighting( in float3 normal, in float3 eyespacepos, in float halfLambertScale, in float shininess )
		{
			float NdotL = saturate( saturate( dot( SunLightDir.xyz, normal ) ) * halfLambertScale + 1.0 - halfLambertScale );
		  
			float3 viewDir = normalize(eyespacepos);
			float3 half_vec = normalize( SunLightDir.xyz - eyespacepos );
			float spec = roughness_term( half_vec, normal, shininess );
		  
			//float shadowTerm = saturate( tex2D(sunShadowSampler, IN.uv).r );
		  
		    return float4( SunLightColorIntensity.rgb, spec ) * SunLightColorIntensity.a * NdotL;
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
			float spec = roughness_term( half_vec, normal, shininess );
			
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
	
    ]]>
  </code>
</material>

