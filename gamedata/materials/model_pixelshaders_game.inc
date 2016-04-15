<?xml version="1.0"?>
<material>
	<code>
        <![CDATA[
		
		// wangjian added for test game body effect
		float4 ps_bodyEffect (in v2p_bodyEffect IN) : COLOR
		{
			float4 customColor = tex2D(bodyEffectSampler, IN.texcoord.zw);
			customColor.xyz *= bodyEffectColor.xyz * bodyEffectColor.w * 255.0;
			
			// edge hilight
		#if 1
			float3 normalVS = 	IN.normal;
			normalVS 		*= 	0.5 - sign(normalVS.z);
			half fNdotE 	= 	saturate( dot( normalize( -IN.viewpos.xyz ), normalize( normalVS.xyz ) ) );
			half fresnel 	=  	GetFresnel( fNdotE, 0, edgeHilightParams.x );
			half rim 		= 	smoothstep( edgeHilightParams.y, edgeHilightParams.z, fresnel );
		#endif
			
			customColor.xyz = lerp( customColor.xyz, edgeHilightColor.xyz * edgeHilightParams.w, rim );
			return float4( customColor.xyz, rim );
		}	
		
		// wangjian added for test game body effect
		float4 ps_alphatest_bodyEffect (in v2p_bodyEffect IN) : COLOR
		{
			float4 color = tex2D(diffuseSampler, IN.texcoord.xy);
			clip (color.a - alpharef);
			
			float4 customColor = tex2D(bodyEffectSampler, IN.texcoord.zw);
			customColor.xyz *= bodyEffectColor.xyz * bodyEffectColor.w * 255.0;
			
			// rim lighting
		#if 1
			float3 normalVS = 	IN.normal;
			normalVS 		*= 	0.5 - sign(normalVS.z);
			half fNdotE 	= 	saturate( dot( normalize( -IN.viewpos.xyz ), normalize( normalVS.xyz ) ) );
			half fresnel 	=  	GetFresnel( fNdotE, 0, edgeHilightParams.x );
			half rim 		= 	smoothstep( edgeHilightParams.y, edgeHilightParams.z, fresnel );
		#endif
			
			customColor.xyz = lerp( customColor.xyz, edgeHilightColor.xyz * edgeHilightParams.w, rim );
			//float blendFactor = ( rim == 1 ) ? 1.0 : 0.5;
			return float4( customColor.xyz, rim );
		}
		
	]]></code>
</material>
