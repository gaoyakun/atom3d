<?xml version="1.0"?>
<material>
	<code>
        <![CDATA[
		
		#if USE_BUMP
		
			// calculate the normal from normal map
			float3 calc_normal (float2 tex, float3 tangent, float3 binormal, float3 normal)
			{
		
			#if 0
				float3 n = normalize( tex2D(normalSampler, tex).xyz * 2.0 - 1.0 );		// RGB
			#else
				float3 n;
				n.xy 	= tex2D(normalSampler, tex).ag * 2.0 - 1.0;						// dxt5 ( r swizzle to a )
				n.z 	= sqrt(saturate(1 - dot(n.xy, n.xy)));								// derived z
				n 		= normalize( n );														// renormalize
			#endif
				float3x3 mat = float3x3(tangent, binormal, normal);
				return normalize(mul(n, mat));
			}
			
			// get the glossness from gloss map
			float get_gloss (float2 tex)
			{
				return tex2D(glossSampler, tex).g;		// green channel of gloss map
			}
			
			// get the glossness from gloss map
			float get_flux_mask (float2 tex)
			{
				return tex2D(glossSampler, tex).r;		// red channel of gloss map
			}
			
			void get_normal_gloss( 	float2 tex, float3 tangent, float3 binormal, float3 normal,
									out float3 n, out float gloss )
			{
				float4 norm_gloss = tex2D(normalSampler, tex);
				
				n.xy = norm_gloss.ag * 2.0 - 1.0;					// dxt5 ( r swizzle to a )
				n.z = sqrt(saturate(1 - dot(n.xy, n.xy)));			// derived z
				n = normalize( n );									// renormalize
				float3x3 mat = float3x3(tangent, binormal, normal);
				n = normalize( mul(n, mat) );
				
				//gloss = norm_gloss.r;								// gloss is in r channel
				
				// dxt5 : use r & b channels to store glossness.
				gloss = ( norm_gloss.r * 16.0f + norm_gloss.b ) / 16.0f;
			}
			
		#endif
		
		void ps_notex (	in v2p_notex IN, 
						#if SUPPORT_SM30 && USE_Albedo_Encode
						in float4 screenCoord : VPOS,	// wangjian added
						#endif
						out Gbuffer_PSOutput OUT)
		{
			resetGbufferLayout (OUT);
			
			float3 color = IN.diffuse.rgb * colorMultiplier;
			
		#if USE_RIMLIGHT
			color.rgb += calcRim( IN.viewpos.xyz, IN.normal.xyz, rimColor.rgb );
		#endif
			
			storeRawAlbedoAndGlossness (	color.rgb, 
											glossness, 
											OUT
											#if SUPPORT_SM30 && USE_Albedo_Encode
											, screenCoord
											#endif
										);
										
			storeRawNormalAndSpecularPower (	normalize(IN.normal), 
												shininess,
												OUT	);
			
			//--- wangjian modified ---//
		#if 0
			float halflambert = halfLambertScale;
			storeAux0 (halflambert, Ks, 1.f, OUT);
		#else
			float thick = thickness;					// get thickness
			thick *= sign( halfLambertScale - 0.99 );	// halfLambertScale is < 1, use translucent effect, and the thick is < 0.
			storeAux0 ( thick, Ks, 1.f, OUT );			// the thick will be convert to [0,1] range for unsigned buffer storage.
		#endif
			//-------------------------//
			
		}
		
		void ps (	in v2p IN,
				#if SUPPORT_SM30 && USE_Albedo_Encode
					in float4 screenCoord : VPOS,	// wangjian added
				#endif
					out Gbuffer_PSOutput OUT)
		{
			resetGbufferLayout (OUT);
			
			float4 color = tex2D(diffuseSampler, IN.tex0) * IN.diffuse;
			
			float3 normal = IN.normal;
			float gloss = glossness;
			
		#if USE_BUMP
		
			#if 1
				normal = calc_normal(IN.tex0, IN.tangent, IN.binormal, IN.normal);
			#else
				get_normal_gloss( IN.tex0, IN.tangent, IN.binormal, IN.normal, normal, gloss );
				gloss *= glossness;
			#endif
			
			#if 1
				gloss *= get_gloss(IN.tex0);
			#endif
			
		#endif
		
		#if ( USE_BUMP && USE_FLUX )
			float fluxmask = get_flux_mask(IN.tex0);
			float2 time_factor;
			sincos( time * fluxSpeed, time_factor.x, time_factor.y );
			float3 v = normalize( float3( time_factor.y, 0, time_factor.x ) );
			float spec = pow ( saturate( dot( v, normal ) ) , shininess );
			color.rgb *= 1 + spec * gloss * 256.0 * fluxmask;
		#endif
		
			color.rgb *= colorMultiplier;
		
		#if USE_RIMLIGHT
			color.rgb += calcRim( IN.viewpos.xyz, IN.normal.xyz, rimColor.rgb );
		#endif
		
			//color = float4(IN.tex0.x, IN.tex0.y, 1.0, 1.0) * IN.diffuse;
			storeRawAlbedoAndGlossness ( 	color.rgb, 
											gloss, 
											OUT
										 #if SUPPORT_SM30 && USE_Albedo_Encode
											, screenCoord
										 #endif
										);
										
			storeRawNormalAndSpecularPower (	normalize(normal), 
												shininess,
												OUT	);
			
			//--- wangjian modified ---//
		#if 0
			float halflambert = halfLambertScale;
			storeAux0 (halflambert, Ks, 1.f, OUT);
		#else
			float thick = saturate( tex2D(TranlucentSampler, IN.tex0).r * thickness );		// get thickness
			float hl = halfLambertScale - 0.99;
			thick 		+= ceil( saturate( hl ) );
			thick 		*= sign( hl );						// halfLambertScale is < 1, use translucent effect, and the thick is < 0.
			storeAux0 ( thick, Ks, 1.f, OUT );				// the thick will be convert to [0,1] range for unsigned buffer storage.
		#endif
			//-------------------------//
		}
		
		void ps_notex_alphatest (	in v2p_notex IN, 
									#if SUPPORT_SM30 && USE_Albedo_Encode
									in float4 screenCoord : VPOS,	// wangjian added
									#endif
									out Gbuffer_PSOutput OUT)
		{
			clip (IN.diffuse.a - alpharef);
			
			resetGbufferLayout (OUT);
			
			float3 color = IN.diffuse.rgb * colorMultiplier;
			
		#if USE_RIMLIGHT
			color.rgb += calcRim( IN.viewpos.xyz, IN.normal.xyz, rimColor.rgb );
		#endif
			
			storeRawAlbedoAndGlossness (	color.rgb,
											glossness,
											OUT
										#if SUPPORT_SM30 && USE_Albedo_Encode
										, 	screenCoord
										#endif
										);
			storeRawNormalAndSpecularPower (	normalize(IN.normal), 
												shininess,
												OUT	);
			
			//--- wangjian modified ---//
		#if 0
			float halflambert = halfLambertScale;
			storeAux0 (halflambert, Ks, 1.f, OUT);
		#else
			float thick = thickness;					// get thickness
			thick *= sign( halfLambertScale - 0.99 );	// halfLambertScale is < 1, use translucent effect, and the thick is < 0.
			storeAux0 ( thick, Ks, 1.f, OUT );			// the thick will be convert to [0,1] range for unsigned buffer storage.
		#endif
			//-------------------------//
		}

		void ps_alphatest (	in v2p IN,
							#if SUPPORT_SM30 && USE_Albedo_Encode
							in float4 screenCoord : VPOS,	// wangjian added
							#endif
							out Gbuffer_PSOutput OUT)
		{
			float4 color = tex2D(diffuseSampler, IN.tex0) * IN.diffuse;
			clip (color.a - alpharef);
			
			resetGbufferLayout (OUT);
			
			float3 normal = IN.normal;
			float gloss = glossness;
			
		#if USE_BUMP
		
			#if 1
				normal = calc_normal(IN.tex0, IN.tangent, IN.binormal, IN.normal);
			#else
				get_normal_gloss( IN.tex0, IN.tangent, IN.binormal, IN.normal, normal, gloss );
				gloss *= glossness;
			#endif
			
			#if 1
				gloss *= get_gloss(IN.tex0);
			#endif
			
		#endif
		
		
		#if ( USE_BUMP && USE_FLUX )
		
			float fluxmask = get_flux_mask(IN.tex0);
			float2 time_factor;
			sincos( time * fluxSpeed, time_factor.x, time_factor.y );
			float3 v = normalize( float3( time_factor.y, 0, time_factor.x ) );
			float spec = pow ( saturate( dot( v, normal ) ) , shininess );
			color.rgb *= 1 + spec * gloss * 256.0 * fluxmask;
			
		#endif
		
			color.rgb *= colorMultiplier;
		
		#if USE_RIMLIGHT
			color.rgb += calcRim( IN.viewpos.xyz, IN.normal.xyz, rimColor.rgb );
		#endif
			
			storeRawAlbedoAndGlossness (	color.rgb, 
											gloss,
											OUT
											#if SUPPORT_SM30 && USE_Albedo_Encode
											, screenCoord
											#endif
										);
										
			storeRawNormalAndSpecularPower (	normalize(normal), 
												shininess,
												OUT	);
			
			//--- wangjian modified ---//
		#if 0
			float halflambert = halfLambertScale;
			storeAux0 (halflambert, Ks, 1.f, OUT);
		#else
			float thick = saturate( tex2D(TranlucentSampler, IN.tex0).r * thickness );		// get thickness
			float hl 	= halfLambertScale - 0.99;
			thick 		+= ceil( saturate( hl ) );
			thick 		*= sign( hl );	// halfLambertScale is < 1, use translucent effect, and the thick is < 0.
			storeAux0 ( thick, Ks, 1.f, OUT );				// the thick will be convert to [0,1] range for unsigned buffer storage.
		#endif
			//-------------------------//
		}				
    
		
		
		float4 ps_blend (in v2p_blend IN) : COLOR
		{
			float4 color = 	tex2D(diffuseSampler, IN.tex0) * 
							IN.diffuse * 
							transparency * 
							float4( colorMultiplier, colorMultiplier, colorMultiplier, 1 );
							
		#if USE_RIMLIGHT
			color.rgb += calcRim( IN.viewpos.xyz, IN.normal.xyz, rimColor.rgb );
		#endif
		
			return float4( color.rgb , color.a );
		}				
    
		float4 ps_blend_notex (in v2p_blend_notex IN) : COLOR
		{
			float4 color = IN.diffuse * transparency * float4( colorMultiplier, colorMultiplier, colorMultiplier, 1 );
			
		#if USE_RIMLIGHT
			color.rgb += calcRim( IN.viewpos.xyz, IN.normal.xyz, rimColor.rgb );
		#endif
		
			return float4( color.rgb, color.a );
		}				
    
		float4 ps_blend_alphatest (in v2p_blend IN) : COLOR
		{
			float4 color = tex2D(diffuseSampler, IN.tex0) * IN.diffuse;
			clip (color.a - alpharef);
			
			color *= transparency * float4( colorMultiplier, colorMultiplier, colorMultiplier, 1 );
			
		#if USE_RIMLIGHT
			color.rgb += calcRim( IN.viewpos.xyz, IN.normal.xyz, rimColor.rgb );
		#endif
		
			return float4( color.rgb, color.a );
		}				
    
		float4 ps_blend_notex_alphatest (in v2p_blend_notex IN) : COLOR
		{
			clip (IN.diffuse.a - alpharef);
			
			float4 color = IN.diffuse * transparency * float4( colorMultiplier, colorMultiplier, colorMultiplier, 1 );
			
		#if USE_RIMLIGHT
			color.rgb += calcRim( IN.viewpos.xyz, IN.normal.xyz, rimColor.rgb );
		#endif
		
			return float4( color.rgb, color.a );
		}				

		float4 ps_postzpass (in v2p_postzpass IN): COLOR0
		{
			return 1.0;
		}
	
		float4 ps_shadowmap (in v2p_shadowmap IN): COLOR0
		{
			return IN.depth;
		}

		float4 ps_alphatest_shadowmap (in v2p_shadowmap IN): COLOR0
		{
			float4 color = tex2D(diffuseSampler, IN.tex0);
			clip (color.a - alpharef);
			return IN.depth;
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		float4 ps_sssss (in v2p_sssss IN): COLOR0
		{
			float skinMask = tex2D(skinMaskSampler, IN.tex0).r;
			clip (skinMask - 0.1);
			
			// sssss
			float4 diffuseSpec = calcSSSSS( IN.screenCoord );
			
			// rim
		#if USE_RIMLIGHT
			float3 cRim = calcRim( IN.pos_vs.xyz, IN.normal_vs.xyz, rimColor.rgb );
		#else
			float3 cRim = calcRim( IN.pos_vs.xyz, IN.normal_vs.xyz, diffuseSpec.rgb );
		#endif
		
			// get diffuse and specular reflectance
			float4 diffuseGlossiness = getAlbedoAndGlossness(IN.screenCoord.xy / IN.screenCoord.w);
			
			// mul rim with gloss
			//cRim *= diffuseSpec.a;// * cGlossMap.xyz ;
			
			// calc diffuse and specular
			diffuseSpec *= diffuseGlossiness;
			
			return float4( diffuseSpec.rgb + diffuseSpec.aaa + cRim, 1 );
		}
		
		float4 ps_alphatest_sssss (in v2p_sssss IN): COLOR0
		{
			float4 color = tex2D(diffuseSampler, IN.tex0) * IN.diffuse;
			clip (color.a - alpharef);
			
			float skinMask = tex2D(skinMaskSampler, IN.tex0).r;
			clip (skinMask - 0.1);
			
			// sssss
			float4 diffuseSpec = calcSSSSS( IN.screenCoord );
			
			// rim
		#if USE_RIMLIGHT
			float3 cRim = calcRim( IN.pos_vs.xyz, IN.normal_vs.xyz, rimColor.rgb );
		#else
			float3 cRim = calcRim( IN.pos_vs.xyz, IN.normal_vs.xyz, diffuseSpec.rgb );
		#endif
			
			// get diffuse and specular reflectance
			float4 diffuseGlossiness = getAlbedoAndGlossness(IN.screenCoord.xy / IN.screenCoord.w);
			
			// mul rim with gloss
			//cRim *= diffuseSpec.a;// * cGlossMap.xyz ;
			
			// calc diffuse and specular
			diffuseSpec *= diffuseGlossiness;
			
			return float4( diffuseSpec.rgb + diffuseSpec.aaa + cRim, 1 );
		}		
		/////////////////////////////////////////////////////////////////////////////////////////////
		
		float4 ps_environment (in v2p_env IN) : COLOR0
		{
			float3 normal = IN.normal;
		
			float gloss = glossness;
			
		#if USE_BUMP
		
			#if 1
				normal = calc_normal(IN.tex0, IN.tangent, IN.binormal, IN.normal);
			#else
				get_normal_gloss( IN.tex0, IN.tangent, IN.binormal, IN.normal, normal, gloss );
				gloss *= glossness;
			#endif
			
			#if 1
				gloss *= get_gloss(IN.tex0);
			#endif
			
		#endif
		
		#if 1
			normal 						= normalize( normal );
			float3 envLighting 			= tex2D( sphereSampler, normal.xy * float2( 0.5, -0.5 ) + float2( 0.5,0.5 ) );
		#else
			// calc the reflection vector
			float3 eyedir 				= normalize( IN.viewpos.xyz );
			float3 refl 				= reflect( eyedir, normalize( normal.xyz ) );
			
			refl 						= mul( refl.xyz, (float3x3)View );
			refl 						= normalize( refl );
			float3 envLighting 			= texCUBE( cubeSampler, refl ).rgb;
		#endif
			
			float4 lightinfo = getLightAccum(IN.screenCoord.xy/IN.screenCoord.w);
			
			float4 color = float4( envLighting * envLightingMultiplier, 1 );
			color.xyz *= lightinfo.xyz * 0.5 + 0.5;
			
			color.xyz += envLighting * envLightingMultiplier * lightinfo.w * 10;
			
			return color;
		}
		
		float4 ps_alphatest_environment (in v2p_env IN) : COLOR0
		{
			float4 color = tex2D(diffuseSampler, IN.tex0) * IN.diffuse;
			clip (color.a - alpharef);
			
			float3 normal = IN.normal;
		
			float gloss = glossness;
			
		#if USE_BUMP
		
			#if 1
				normal = calc_normal(IN.tex0, IN.tangent, IN.binormal, IN.normal);
			#else
				get_normal_gloss( IN.tex0, IN.tangent, IN.binormal, IN.normal, normal, gloss );
				gloss *= glossness;
			#endif
			
			#if 1
				gloss *= get_gloss(IN.tex0);
			#endif
			
		#endif
			
			normal 						= normalize( normal );
			float3 envLighting 			= tex2D( sphereSampler, normal.xy * float2( 0.5, -0.5 ) + float2( 0.5,0.5 ) );
			
			float4 lightinfo = getLightAccum(IN.screenCoord.xy/IN.screenCoord.w);
			
			float4 result = float4( envLighting * envLightingMultiplier, 1 );
			result.xyz *= lightinfo.xyz * 0.2 + 0.8;
			
			//result.xyz += envLighting * envLightingMultiplier * lightinfo.w;
			
			return result;
		}
		
	]]></code>
</material>
