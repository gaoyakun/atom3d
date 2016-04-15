<?xml version="1.0" ?>
<material auto="1">
    <include filename="/materials/builtin/forward/forwardrendering.inc" />
	
    <code>
        <![CDATA[
		
		struct a2v_nodiffuse_fs
		{
			float4 position: POSITION;
			float3 normal: NORMAL;
			float2 tex0: TEXCOORD0;
		};
		
		struct a2v_diffuse_fs
		{
			float4 position: POSITION;
			float3 normal: NORMAL;
      		float4 diffuse: COLOR0;
			float2 tex0: TEXCOORD0;
		};
		
		struct a2v_nodiffuse_notex_fs
		{
			float4 position: POSITION;
			float3 normal: NORMAL;
		};
		
		struct a2v_diffuse_notex_fs
		{
			float4 position: POSITION;
			float3 normal: NORMAL;
			float4 diffuse: COLOR0;
		};
		
		struct a2v_nodiffuse_skinning_fs
		{
			float4 position: POSITION;
			float3 normal: NORMAL;
			float2 tex0: TEXCOORD0;
			float4 blendweights: TEXCOORD1;
			float4 blendindices: TEXCOORD2;
		};
		
		struct a2v_diffuse_skinning_fs
		{
			float4 position: POSITION;
			float3 normal: NORMAL;
			float4 diffuse: COLOR0;
			float2 tex0: TEXCOORD0;
			float4 blendweights: TEXCOORD1;
			float4 blendindices: TEXCOORD2;
		};
		
		struct a2v_nodiffuse_notex_skinning_fs
		{
			float4 position: POSITION;
			float3 normal: NORMAL;
			float4 blendweights: TEXCOORD1;
			float4 blendindices: TEXCOORD2;
		};
		
		struct a2v_diffuse_notex_skinning_fs
		{
			float4 position: POSITION;
			float3 normal: NORMAL;		
			float4 diffuse: COLOR0;
			float4 blendweights: TEXCOORD1;
			float4 blendindices: TEXCOORD2;
		};
		
		struct a2v_blend_nodiffuse_fs
		{
		  float4 position: POSITION;
		  float2 tex0: TEXCOORD0;
		};
		
		struct a2v_blend_diffuse_fs
		{
		  float4 position: POSITION;
		  float4 diffuse: COLOR0;
		  float2 tex0: TEXCOORD0;
		};
		
		struct a2v_blend_nodiffuse_notex_fs
		{
		  float4 position: POSITION;
		};
		
		struct a2v_blend_diffuse_notex_fs
		{
		  float4 position: POSITION;
		  float4 diffuse: COLOR0;
		};
		
		struct a2v_blend_nodiffuse_skinning_fs
		{
		  float4 position: POSITION;
		  float2 tex0: TEXCOORD0;
		  float4 blendweights: TEXCOORD1;
		  float4 blendindices: TEXCOORD2;
		};
		
		struct a2v_blend_diffuse_skinning_fs
		{
		  float4 position: POSITION;
		  float4 diffuse: COLOR0;
		  float2 tex0: TEXCOORD0;
		  float4 blendweights: TEXCOORD1;
		  float4 blendindices: TEXCOORD2;
		};
		
		struct a2v_blend_nodiffuse_notex_skinning_fs
		{
		  float4 position: POSITION;
		  float4 blendweights: TEXCOORD1;
		  float4 blendindices: TEXCOORD2;
		};
		
		struct a2v_blend_diffuse_notex_skinning_fs
		{
		  float4 position: POSITION;
		  float4 diffuse: COLOR0;
		  float4 blendweights: TEXCOORD1;
		  float4 blendindices: TEXCOORD2;
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		struct v2p_fs
		{
			float4 position	: POSITION;
			float4 diffuse	: COLOR0;
			float2 tex0		: TEXCOORD0;
			float3 viewpos	: TEXCOORD1;
			float3 normal	: TEXCOORD2;
			float4 fog 		: TEXCOORD3;
		};
		
		struct v2p_notex_fs
		{
			float4 position	: POSITION;
			float4 diffuse	: COLOR0;
			float3 viewpos	: TEXCOORD0;
			float3 normal	: TEXCOORD1;
			float4 fog 		: TEXCOORD2;
		};
		
		struct v2p_blend_fs
		{
		  float4 position	: POSITION;
		  float4 diffuse	: COLOR0;
		  float2 tex0		: TEXCOORD0;
		  float4 fog 		: TEXCOORD1;
		};
    
		struct v2p_blend_notex_fs
		{
		  float4 position	: POSITION;
		  float4 diffuse	: COLOR0;
		  float4 fog 		: TEXCOORD0;
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
		void vs_nodiffuse_fs (in a2v_nodiffuse_fs IN, out v2p_fs OUT)
		{
			OUT.position 			= mul(IN.position, MVP);
			OUT.viewpos 			= mul(IN.position, worldview);
			OUT.diffuse 			= diffuseColor;
			OUT.tex0 				= IN.tex0;
			OUT.normal 				= mul(IN.normal, (float3x3)worldview);
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_diffuse_fs (in a2v_diffuse_fs IN, out v2p_fs OUT)
		{
			OUT.position 			= mul(IN.position, MVP);
			OUT.viewpos 			= mul(IN.position, worldview);
			OUT.diffuse 			= diffuseColor * IN.diffuse;
			OUT.tex0 				= IN.tex0;
			OUT.normal 				= mul(IN.normal, (float3x3)worldview);
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_nodiffuse_notex_fs (in a2v_nodiffuse_notex_fs IN, out v2p_notex_fs OUT)
		{
			OUT.position 			= mul(IN.position, MVP);
			OUT.viewpos 			= mul(IN.position, worldview);
			OUT.diffuse 			= diffuseColor;
			OUT.normal 				= mul(IN.normal, (float3x3)worldview);
			OUT.fog					= CalcFog(OUT.position.w);
		}

		void vs_diffuse_notex_fs (in a2v_diffuse_notex_fs IN, out v2p_notex_fs OUT)
		{
			OUT.position 			= mul(IN.position, MVP);
			OUT.viewpos 			= mul(IN.position, worldview);
			OUT.diffuse 			= diffuseColor * IN.diffuse;
			OUT.normal 				= mul(IN.normal, (float3x3)worldview);
			OUT.fog					= CalcFog(OUT.position.w);
		}
		

		void vs_nodiffuse_skinning_fs (in a2v_nodiffuse_skinning_fs IN, out v2p_fs OUT)
		{
			float3 skinnedVertex 	= SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position 			= mul(float4(skinnedVertex, 1.0), MVP);
			float3 skinnedNormal 	= SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal 				= mul(skinnedNormal, (float3x3)worldview);
			OUT.viewpos 			= mul(IN.position, worldview);
			OUT.diffuse 			= diffuseColor;
			OUT.tex0 				= IN.tex0;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_diffuse_skinning_fs (in a2v_diffuse_skinning_fs IN, out v2p_fs OUT)
		{
			float3 skinnedVertex 	= SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position 			= mul(float4(skinnedVertex, 1.0), MVP);
			float3 skinnedNormal 	= SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal 				= mul(skinnedNormal, (float3x3)worldview);
			OUT.viewpos 			= mul(IN.position, worldview);
			OUT.diffuse 			= diffuseColor * IN.diffuse;
			OUT.tex0 				= IN.tex0;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		
		void vs_nodiffuse_notex_skinning_fs (in a2v_nodiffuse_notex_skinning_fs IN, out v2p_notex_fs OUT)
		{
			float3 skinnedVertex 	= SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position 			= mul(float4(skinnedVertex, 1.0), MVP);
			float3 skinnedNormal 	= SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal 				= mul(skinnedNormal, (float3x3)worldview);
			OUT.viewpos 			= mul(IN.position, worldview);
			OUT.diffuse 			= diffuseColor;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_diffuse_notex_skinning_fs (in a2v_diffuse_notex_skinning_fs IN, out v2p_notex_fs OUT)
		{
      		float3 skinnedVertex 	= SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position 			= mul(float4(skinnedVertex, 1.0), MVP);
			float3 skinnedNormal 	= SkinningVectorTransform (IN.normal, IN.blendindices, IN.blendweights);
			OUT.normal 				= mul(skinnedNormal, (float3x3)worldview);
			OUT.viewpos 			= mul(IN.position, worldview);
			OUT.diffuse 			= diffuseColor * IN.diffuse;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_blend_nodiffuse_fs (in a2v_blend_nodiffuse_fs IN, out v2p_blend_fs OUT)
		{
			OUT.position 			= mul(IN.position, MVP);
			OUT.diffuse 			= diffuseColor;
			OUT.tex0 				= IN.tex0;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_blend_diffuse_fs (in a2v_blend_diffuse_fs IN, out v2p_blend_fs OUT)
		{
			OUT.position 			= mul(IN.position, MVP);
			OUT.diffuse 			= diffuseColor * IN.diffuse;
			OUT.tex0 				= IN.tex0;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_blend_nodiffuse_notex_fs (in a2v_blend_nodiffuse_notex_fs IN, out v2p_blend_notex_fs OUT)
		{
			OUT.position 			= mul(IN.position, MVP);
			OUT.diffuse 			= diffuseColor;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_blend_diffuse_notex_fs (in a2v_blend_diffuse_notex_fs IN, out v2p_blend_notex_fs OUT)
		{
			OUT.position 			= mul(IN.position, MVP);
			OUT.diffuse 			= diffuseColor * IN.diffuse;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_blend_nodiffuse_skinning_fs (in a2v_blend_nodiffuse_skinning_fs IN, out v2p_blend_fs OUT)
		{
			float3 skinnedVertex 	= SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position 			= mul(float4(skinnedVertex, 1.0), MVP);
			OUT.diffuse 			= diffuseColor;
			OUT.tex0 				= IN.tex0;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_blend_diffuse_skinning_fs (in a2v_blend_diffuse_skinning_fs IN, out v2p_blend_fs OUT)
		{
			float3 skinnedVertex 	= SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position 			= mul(float4(skinnedVertex, 1.0), MVP);
			OUT.diffuse 			= diffuseColor * IN.diffuse;
			OUT.tex0 				= IN.tex0;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_blend_nodiffuse_notex_skinning_fs (in a2v_blend_nodiffuse_notex_skinning_fs IN, out v2p_blend_notex_fs OUT)
		{
			float3 skinnedVertex 	= SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position 			= mul(float4(skinnedVertex, 1.0), MVP);
			OUT.diffuse 			= diffuseColor;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		void vs_blend_diffuse_notex_skinning_fs (in a2v_blend_diffuse_notex_skinning_fs IN, out v2p_blend_notex_fs OUT)
		{
			float3 skinnedVertex 	= SkinningPositionTransform (IN.position, IN.blendindices, IN.blendweights);
			OUT.position 			= mul(float4(skinnedVertex, 1.0), MVP);
			OUT.diffuse 			= diffuseColor * IN.diffuse;
			OUT.fog					= CalcFog(OUT.position.w);
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		float4 ps_fs (	in v2p_fs IN ) : COLOR
		{
			float4 diffuseColor = tex2D(diffuseSampler, IN.tex0) * IN.diffuse;

			float4 lightvalue = ForwardLighting( IN.normal,IN.viewpos,halfLambertScale,shininess );
			
			float4 color = float4( diffuseColor.rgb * lightvalue.rgb + glossness * lightvalue.a, 1.0f );
			
			ApplyFog( color, IN.fog );
			
			return color;
		}				

		float4 ps_notex_fs (	in v2p_notex_fs IN ) : COLOR
		{
			float4 diffuseColor = IN.diffuse;

			float4 lightvalue = ForwardLighting( IN.normal,IN.viewpos,halfLambertScale,shininess );
						
			float4 color = float4( diffuseColor.rgb * lightvalue.rgb + glossness * lightvalue.a, 1.0f );
			
			ApplyFog( color, IN.fog );
			
			return color;
		}

		float4 ps_alphatest_fs ( in v2p_fs IN ) : COLOR
		{
			float4 diffuseColor = tex2D(diffuseSampler, IN.tex0) * IN.diffuse;
			
			clip (diffuseColor.a - alpharef);
			
			float4 lightvalue = ForwardLighting( IN.normal,IN.viewpos, halfLambertScale, shininess );
		  
			float4 color = float4( diffuseColor.rgb * lightvalue.rgb + glossness * lightvalue.a, 1.0f );
			
			ApplyFog( color, IN.fog );
			
			return color;
		}				
    
		float4 ps_notex_alphatest_fs ( in v2p_notex_fs IN ) : COLOR
		{
			float4 diffuseColor = IN.diffuse;
			
			clip (IN.diffuse.a - alpharef);
			
			float4 lightvalue = ForwardLighting( IN.normal,IN.viewpos,halfLambertScale,shininess );
		  
			float4 color = float4( diffuseColor.rgb * lightvalue.rgb + glossness * lightvalue.a, 1.0f );
			
			ApplyFog( color, IN.fog );
			
			return color;
		}
		
		float4 ps_blend_fs (in v2p_blend_fs IN) : COLOR
		{
			float4 color = tex2D(diffuseSampler, IN.tex0) * IN.diffuse * transparency * float4( colorMultiplier, colorMultiplier, colorMultiplier, 1 );
			
			ApplyFog( color, IN.fog );
			
			return color;
		}				
    
		float4 ps_blend_notex_fs (in v2p_blend_notex_fs IN) : COLOR
		{
			float4 color = IN.diffuse * transparency * float4( colorMultiplier, colorMultiplier, colorMultiplier, 1 );
			
			ApplyFog( color, IN.fog );
			
			return color;
		}				
    
		float4 ps_blend_alphatest_fs (in v2p_blend_fs IN) : COLOR
		{
			float4 color = tex2D(diffuseSampler, IN.tex0) * IN.diffuse;
			clip (color.a - alpharef);
			color = color * transparency * float4( colorMultiplier, colorMultiplier, colorMultiplier, 1 );
			
			ApplyFog( color, IN.fog );
			
			return color;
		}				
    
		float4 ps_blend_notex_alphatest_fs (in v2p_blend_notex_fs IN) : COLOR
		{
			clip (IN.diffuse.a - alpharef);
			float4 color = IN.diffuse * transparency * float4( colorMultiplier, colorMultiplier, colorMultiplier, 1 );
			
			ApplyFog( color, IN.fog );
			
			return color;
		}
    
    ]]>
	</code>
    
</material>
