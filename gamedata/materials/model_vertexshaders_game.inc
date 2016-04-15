<?xml version="1.0"?>
<material>
	<code>
        <![CDATA[
		
		//////////////////////////////////// custom ////////////////////////////////////////////////////
				
		void vs_bodyEffect (in a2v_bodyEffect IN, out v2p_bodyEffect OUT)
		{
		#if USE_INSTANCING
		
			float4 worldpos;
			float3x3 matWorld;
			OUT.position = GetInstanceHPosition( (int)IN.instanceId, IN.position, worldpos, matWorld );
			OUT.viewpos = mul(worldpos, viewMatrix);
			OUT.normal = mul(IN.normal, matWorld);
			OUT.normal = mul(OUT.normal, (float3x3)viewMatrix);
			
		#else

			#if USE_COMPRESSION_VERSION
			
				OUT.position = mul( float4(IN.position.xyz,1.0), MVP );
				OUT.viewpos = mul( float4(IN.position.xyz,1.0), worldview );
				
				#if 1
				float3 normal = 0;// = IN.normal.xyy;
				normal.xy = IN.normal.xy;
				//float s_normalz = ( IN.position.w - fmod(IN.position.w,2.0f) ) * 2.0f - 1.0f;
				normal.z = sqrt( saturate(1-dot(normal.xy,normal.xy)) );// * s_normalz;
				#else
				float3 normal = float3( IN.normal.xy,IN.position.w);
				#endif
				
				OUT.normal = mul( normal.xyz, (float3x3)worldview );
				
			#else
				
				float4 position = IN.position;
				
			#if 1
				float3 normal = normalize(IN.normal);
				position.xyz += normal * bodyEffectParams.w;
			#endif
			
				OUT.position = mul( position, MVP);
				OUT.viewpos = mul( position, worldview);
				OUT.normal = mul( IN.normal, (float3x3)worldview );
				
			#endif
			
		#endif
		
			OUT.texcoord.xy = IN.tex0.xy;
			
			// bodyEffectParams.x is the angle
			float2 moveDir;
			sincos( bodyEffectParams.x, moveDir.x, moveDir.y );
			
			// bodyEffectParams.y is the speed / bodyEffectParams.z is the tiling factor
			float2 tc = ( IN.position.zy );
			OUT.texcoord.zw = ( tc + bodyEffectTime * moveDir * bodyEffectParams.y ) * bodyEffectParams.z ;//IN.tex0;
		}
		
		void vs_bodyEffect_skinning (in a2v_skinning_bodyEffect IN, out v2p_bodyEffect OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			float3 skinnedNormal = SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			
		#if 1
			skinnedNormal = normalize(skinnedNormal);
			skinnedVertex += skinnedNormal * bodyEffectParams.w;
		#endif
		
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			OUT.normal = mul(skinnedNormal, (float3x3)worldview);
			
			OUT.viewpos = mul(float4(skinnedVertex, 1.0), worldview);
			
			OUT.texcoord.xy = IN.tex0;
			
			// bodyEffectParams.x is the angle
			float2 moveDir;
			sincos( bodyEffectParams.x, moveDir.x, moveDir.y );
			
			// bodyEffectParams.y is the speed / bodyEffectParams.z is the tiling factor
			float2 tc = ( IN.position.zy );
			OUT.texcoord.zw = ( tc + bodyEffectTime * moveDir * bodyEffectParams.y ) * bodyEffectParams.z ;//IN.tex0;
		}
		
		///////////////////////////////////////////////////////////////////////////////////////
		
	]]></code>
</material>
