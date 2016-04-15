<?xml version="1.0"?>
<material>
	<code>
        <![CDATA[
		
		//////////////////////////////////////// NORMAL /////////////////////////////////////////////
		
		void vs_nodiffuse (in a2v_nodiffuse IN, out v2p OUT)
		{
		#if USE_INSTANCING
		
			float4 worldpos;
			float3x3 matWorld;
			OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position,worldpos,matWorld);
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
			
				OUT.position = mul(IN.position, MVP);
				OUT.viewpos = mul(IN.position, worldview);
				OUT.normal = mul(IN.normal, (float3x3)worldview);
				
			#endif
			
		#endif
		
			OUT.diffuse = diffuseColor;
			OUT.tex0 = IN.tex0;
			
		#if USE_BUMP
		
			#if USE_COMPRESSION_VERSION
				float3 tangent = IN.normal.zww;
				float s_tangentz = ( fmod(IN.position.w,2.0f) ) * 2.0f - 1.0f;
				tangent.z = sqrt(1.0f-dot(tangent.xy,tangent.xy)) * s_tangentz;
				OUT.tangent = mul(tangent, (float3x3)worldview);
				OUT.binormal = cross(OUT.normal,OUT.tangent);
			#else
				OUT.tangent = mul(IN.tangent, (float3x3)worldview);
				OUT.binormal = mul(IN.binormal, (float3x3)worldview);
			#endif
			
		#endif
		}
		
		void vs_diffuse (in a2v_diffuse IN, out v2p OUT)
		{
		#if USE_INSTANCING
			float4 worldpos;
			float3x3 matWorld;
			OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position,worldpos,matWorld);
			OUT.viewpos = mul(worldpos, viewMatrix);
			OUT.normal = mul(IN.normal, matWorld);
			OUT.normal = mul(OUT.normal, (float3x3)viewMatrix);
		#else
			OUT.position = mul(IN.position, MVP);
			OUT.viewpos = mul(IN.position, worldview);
			OUT.normal = mul(IN.normal, (float3x3)worldview);
		#endif
			
			OUT.diffuse = diffuseColor * IN.diffuse;
			OUT.tex0 = IN.tex0;
			
		#if USE_BUMP
			OUT.tangent = mul(IN.tangent, (float3x3)worldview);
			OUT.binormal = mul(IN.binormal, (float3x3)worldview);
		#endif
		}

		void vs_nodiffuse_notex (in a2v_nodiffuse_notex IN, out v2p_notex OUT)
		{
		#if USE_INSTANCING
			float4 worldpos;
			float3x3 matWorld;
			OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position,worldpos,matWorld);
			OUT.viewpos = mul(worldpos, viewMatrix);
			OUT.normal = mul(IN.normal, matWorld);
			OUT.normal = mul(OUT.normal, (float3x3)viewMatrix);
		#else
			OUT.position = mul(IN.position, MVP);
			OUT.viewpos = mul(IN.position, worldview);
			OUT.normal = mul(IN.normal, (float3x3)worldview);
		#endif
		
			OUT.diffuse = diffuseColor;
		}

		void vs_diffuse_notex (in a2v_diffuse_notex IN, out v2p_notex OUT)
		{
		#if USE_INSTANCING
			float4 worldpos;
			float3x3 matWorld;
			OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position,worldpos,matWorld);
			OUT.viewpos = mul(worldpos, viewMatrix);
			OUT.normal = mul(IN.normal, matWorld);
			OUT.normal = mul(OUT.normal, (float3x3)viewMatrix);
		#else
			OUT.position = mul(IN.position, MVP);
			OUT.viewpos = mul(IN.position, worldview);
			OUT.normal = mul(IN.normal, (float3x3)worldview);
		#endif
			
			OUT.diffuse = diffuseColor * IN.diffuse;
		}
		
		void vs_nodiffuse_sssss (in a2v_nodiffuse IN, out v2p_sssss OUT)
		{
		#if USE_INSTANCING
			float4 worldpos;
			float3x3 matWorld;
			OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position,worldpos,matWorld);
			OUT.pos_vs = mul(worldpos, viewMatrix);
			OUT.normal_vs = mul(IN.normal, matWorld);
			OUT.normal_vs = mul(OUT.normal_vs, (float3x3)viewMatrix);
		#else
			OUT.position = mul(IN.position, MVP);
			OUT.pos_vs = mul(IN.position, worldview);
			OUT.normal_vs = mul(IN.normal, (float3x3)worldview);
		#endif
		
			OUT.tex0 = IN.tex0;
			OUT.diffuse = diffuseColor;
			OUT.screenCoord = getTexCoordFromClipSpace (OUT.position);
		}
		
		void vs_nodiffuse_env (in a2v_nodiffuse IN, out v2p_env OUT)
		{
		#if USE_INSTANCING
		
			float4 worldpos;
			float3x3 matWorld;
			OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position,worldpos,matWorld);
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
			
				OUT.position = mul(IN.position, MVP);
				OUT.viewpos = mul(IN.position, worldview);
				OUT.normal = mul(IN.normal, (float3x3)worldview);
				
			#endif
			
		#endif
		
			OUT.diffuse = diffuseColor;
			OUT.tex0 = IN.tex0;
			
		#if USE_BUMP
		
			#if USE_COMPRESSION_VERSION
				float3 tangent = IN.normal.zww;
				float s_tangentz = ( fmod(IN.position.w,2.0f) ) * 2.0f - 1.0f;
				tangent.z = sqrt(1.0f-dot(tangent.xy,tangent.xy)) * s_tangentz;
				OUT.tangent = mul(tangent, (float3x3)worldview);
				OUT.binormal = cross(OUT.normal,OUT.tangent);
			#else
				OUT.tangent = mul(IN.tangent, (float3x3)worldview);
				OUT.binormal = mul(IN.binormal, (float3x3)worldview);
			#endif
			
		#endif
		
			OUT.tex0 = IN.tex0;
			OUT.diffuse = diffuseColor;
			OUT.screenCoord = getTexCoordFromClipSpace (OUT.position);
		}
		
		//////////////////////////////////////// ALPHA BLENDING /////////////////////////////////////////////
				
		void vs_blend_nodiffuse (in a2v_blend_nodiffuse IN, out v2p_blend OUT)
		{
		#if USE_INSTANCING
		
			#if USE_RIMLIGHT
			
				float4 worldpos;
				float3x3 matWorld;
				OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position,worldpos,matWorld);
				OUT.viewpos = mul(worldpos, viewMatrix);
				OUT.normal = mul(IN.normal, matWorld);
				OUT.normal = mul(OUT.normal, (float3x3)viewMatrix);
				
			#else
			
				OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position);
				
			#endif
			
		#else
		
			OUT.position = mul(IN.position, MVP);
			
			#if USE_RIMLIGHT
				OUT.viewpos = mul( IN.position, worldview);
				OUT.normal = mul(IN.normal, (float3x3)worldview);
			#endif
			
		#endif
		
			OUT.diffuse = diffuseColor;
			OUT.tex0 = IN.tex0;
		}
		
		void vs_blend_diffuse (in a2v_blend_diffuse IN, out v2p_blend OUT)
		{
		#if USE_INSTANCING
		
			#if USE_RIMLIGHT
			
				float4 worldpos;
				float3x3 matWorld;
				OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position,worldpos,matWorld);
				OUT.viewpos = mul(worldpos, viewMatrix);
				OUT.normal = mul(IN.normal, matWorld);
				OUT.normal = mul(OUT.normal, (float3x3)viewMatrix);
				
			#else
			
				OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position);
				
			#endif
			
		#else
		
			OUT.position = mul(IN.position, MVP);
			
			#if USE_RIMLIGHT
				OUT.viewpos = mul( IN.position, worldview);
				OUT.normal = mul(IN.normal, (float3x3)worldview);
			#endif
			
		#endif
		
			OUT.diffuse = diffuseColor * IN.diffuse;
			OUT.tex0 = IN.tex0;
		}
		
		void vs_blend_nodiffuse_notex (in a2v_blend_nodiffuse_notex IN, out v2p_blend_notex OUT)
		{
		#if USE_INSTANCING
		
			#if USE_RIMLIGHT
			
				float4 worldpos;
				float3x3 matWorld;
				OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position,worldpos,matWorld);
				OUT.viewpos = mul(worldpos, viewMatrix);
				OUT.normal = mul(IN.normal, matWorld);
				OUT.normal = mul(OUT.normal, (float3x3)viewMatrix);
				
			#else
			
				OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position);
				
			#endif
			
		#else
		
			OUT.position = mul(IN.position, MVP);
			
			#if USE_RIMLIGHT
				OUT.viewpos = mul( IN.position, worldview);
				OUT.normal = mul(IN.normal, (float3x3)worldview);
			#endif
			
		#endif
		
			OUT.diffuse = diffuseColor;
		}
		
		void vs_blend_diffuse_notex (in a2v_blend_diffuse_notex IN, out v2p_blend_notex OUT)
		{
		#if USE_INSTANCING
		
			#if USE_RIMLIGHT
			
				float4 worldpos;
				float3x3 matWorld;
				OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position,worldpos,matWorld);
				OUT.viewpos = mul(worldpos, viewMatrix);
				OUT.normal = mul(IN.normal, matWorld);
				OUT.normal = mul(OUT.normal, (float3x3)viewMatrix);
				
			#else
			
				OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position);
				
			#endif
			
		#else
		
			OUT.position = mul(IN.position, MVP);
			
			#if USE_RIMLIGHT
				OUT.viewpos = mul( IN.position, worldview);
				OUT.normal = mul(IN.normal, (float3x3)worldview);
			#endif
			
		#endif
		
			OUT.diffuse = diffuseColor * IN.diffuse;
		}
		
		//////////////////////////////////////// SKINNING /////////////////////////////////////////////
		
	#if USE_SKINNING
	
		void vs_nodiffuse_skinning (in a2v_nodiffuse_skinning IN, out v2p OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			
			float3 skinnedNormal = SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal = mul(skinnedNormal, (float3x3)worldview);
			
			OUT.viewpos = mul(float4(skinnedVertex, 1.0), worldview);
			OUT.diffuse = diffuseColor;
			OUT.tex0 = IN.tex0;
			
		#if USE_BUMP
			float3 skinnedTangent = SkinningVectorTransform (IN.tangent, IN.blendindices, IN.blendweights);
			float3 skinnedBinormal = SkinningVectorTransform (IN.binormal, IN.blendindices, IN.blendweights);
			OUT.tangent = mul(skinnedTangent, (float3x3)worldview);
			OUT.binormal = mul(skinnedBinormal, (float3x3)worldview);
		#endif
		}
		
		void vs_diffuse_skinning (in a2v_diffuse_skinning IN, out v2p OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			
			float3 skinnedNormal = SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal = mul(skinnedNormal, (float3x3)worldview);
			
			OUT.viewpos = mul(float4(skinnedVertex, 1.0), worldview);
			OUT.diffuse = diffuseColor * IN.diffuse;
			OUT.tex0 = IN.tex0;
			
		#if USE_BUMP
			float3 skinnedTangent = SkinningVectorTransform (IN.tangent, IN.blendindices, IN.blendweights);
			float3 skinnedBinormal = SkinningVectorTransform (IN.binormal, IN.blendindices, IN.blendweights);
			OUT.tangent = mul(skinnedTangent, (float3x3)worldview);
			OUT.binormal = mul(skinnedBinormal, (float3x3)worldview);
		#endif
		}
		
		void vs_nodiffuse_notex_skinning (in a2v_nodiffuse_notex_skinning IN, out v2p_notex OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			float3 skinnedNormal = SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal = mul(skinnedNormal, (float3x3)worldview);
			OUT.viewpos = mul(float4(skinnedVertex, 1.0), worldview);
			OUT.diffuse = diffuseColor;
		}
		
		void vs_diffuse_notex_skinning (in a2v_diffuse_notex_skinning IN, out v2p_notex OUT)
		{
      		float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			float3 skinnedNormal = SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal = mul(skinnedNormal, (float3x3)worldview);
			OUT.viewpos = mul(float4(skinnedVertex, 1.0), worldview);
			OUT.diffuse = diffuseColor * IN.diffuse;
		}
	
		void vs_blend_nodiffuse_skinning (in a2v_blend_nodiffuse_skinning IN, out v2p_blend OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			OUT.diffuse = diffuseColor;
			OUT.tex0 = IN.tex0;
			
		#if USE_RIMLIGHT
			OUT.viewpos = mul( float4(skinnedVertex, 1.0), worldview);
			float3 skinnedNormal = SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal = mul(skinnedNormal, (float3x3)worldview);
		#endif
		}
		
		void vs_blend_diffuse_skinning (in a2v_blend_diffuse_skinning IN, out v2p_blend OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			OUT.diffuse = diffuseColor * IN.diffuse;
			OUT.tex0 = IN.tex0;
			
		#if USE_RIMLIGHT
			OUT.viewpos = mul( float4(skinnedVertex, 1.0), worldview);
			float3 skinnedNormal = SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal = mul(skinnedNormal, (float3x3)worldview);
		#endif
		}
		
		void vs_blend_nodiffuse_notex_skinning (in a2v_blend_nodiffuse_notex_skinning IN, out v2p_blend_notex OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			OUT.diffuse = diffuseColor;
			
		#if USE_RIMLIGHT
			OUT.viewpos = mul( float4(skinnedVertex, 1.0), worldview);
			float3 skinnedNormal = SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal = mul(skinnedNormal, (float3x3)worldview);
		#endif
		}
		
		void vs_blend_diffuse_notex_skinning (in a2v_blend_diffuse_notex_skinning IN, out v2p_blend_notex OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			OUT.diffuse = diffuseColor * IN.diffuse;
			
		#if USE_RIMLIGHT
			OUT.viewpos = mul( float4(skinnedVertex, 1.0), worldview);
			float3 skinnedNormal = SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal = mul(skinnedNormal, (float3x3)worldview);
		#endif
		}
		
		void vs_nodiffuse_sssss_skinning (in a2v_nodiffuse_skinning IN, out v2p_sssss OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			OUT.diffuse = diffuseColor;
			OUT.tex0 = IN.tex0;
			
			OUT.screenCoord = getTexCoordFromClipSpace (OUT.position);
			
			float3 skinnedNormal = SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal_vs = mul(skinnedNormal, (float3x3)worldview);
			
			OUT.pos_vs = mul(float4(skinnedVertex, 1.0), worldview);
		}
		
		void vs_skinning_shadowmap (in a2v_nodiffuse_skinning IN, out v2p_shadowmap OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			OUT.tex0 = IN.tex0;
			OUT.depth = OUT.position.z/OUT.position.w;
		}
		
		void vs_notex_skinning_shadowmap (in a2v_nodiffuse_notex_skinning IN, out v2p_notex_shadowmap OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			OUT.depth = OUT.position.z/OUT.position.w;
		}
		
		void vs_nodiffuse_env_skinning (in a2v_nodiffuse_skinning IN, out v2p_env OUT)
		{
			float3 skinnedVertex = SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position = mul(float4(skinnedVertex, 1.0), MVP);
			
			float3 skinnedNormal = SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal = mul(skinnedNormal, (float3x3)worldview);
			
			OUT.viewpos = mul(float4(skinnedVertex, 1.0), worldview);
			OUT.diffuse = diffuseColor;
			OUT.tex0 = IN.tex0;
			
		#if USE_BUMP
			float3 skinnedTangent = SkinningVectorTransform (IN.tangent, IN.blendindices, IN.blendweights);
			float3 skinnedBinormal = SkinningVectorTransform (IN.binormal, IN.blendindices, IN.blendweights);
			OUT.tangent = mul(skinnedTangent, (float3x3)worldview);
			OUT.binormal = mul(skinnedBinormal, (float3x3)worldview);
		#endif
			
			OUT.screenCoord = getTexCoordFromClipSpace (OUT.position);
		}
		
	#endif
		
		//////////////////////////////////// shadow map ////////////////////////////////////////////////////
		
		void vs_notex_shadowmap (in a2v_nodiffuse_notex IN, out v2p_notex_shadowmap OUT)
		{
		#if USE_INSTANCING
			OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position);
		#else
			OUT.position = mul(IN.position, MVP);
		#endif
		
			OUT.depth = OUT.position.z/OUT.position.w;
		}
		
		void vs_shadowmap (in a2v_nodiffuse IN, out v2p_shadowmap OUT)
		{
		#if USE_INSTANCING
			OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position);
		#else
			OUT.position = mul( float4( IN.position.xyz,1.0 ), MVP);
		#endif
			
			OUT.tex0 = IN.tex0;
			OUT.depth = OUT.position.z/OUT.position.w;
		}
	
		//////////////////////////////////// post z ////////////////////////////////////////////////////
		
		void vs_postzpass (in a2v_postzpass IN, out v2p_postzpass OUT)
		{
		#if USE_INSTANCING
			OUT.position = GetInstanceHPosition((int)IN.instanceId,IN.position);
		#else
			OUT.position = mul(IN.position, MVP);
		#endif
		}
		
		///////////////////////////////////////////////////////////////////////////////////////
		
	]]></code>
</material>
