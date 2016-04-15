<?xml version="1.0"?>
<material>
	<code>
        <![CDATA[
		
		/////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////// APP ---->>>> VS ///////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////
		
		//------------------- no diffuse ---------------------------//
		
		/* No BLEND | NO diffuse | NO TEXTURE */
		struct a2v_nodiffuse_notex
		{
			POS
			NORM
			
			INSTANCE_ID
			
		};
		
		/* No BLEND | NO diffuse */
		struct a2v_nodiffuse
		{
			POS
			NORM
			TEX0
			
			TANGENT_a2v
			BINORMAL_a2v
			
			INSTANCE_ID
			
		};
		
		/* No BLEND | NO diffuse | NO TEXTURE | SKINNING */
		struct a2v_nodiffuse_notex_skinning
		{
			POS
			NORM
			
			TANGENT_a2v
			BINORMAL_a2v
			
			BLENDWEIGHTS 		//	float4 blendweights: TEXCOORD1;
			BLENDINDICES 		// 	float4 blendindices: TEXCOORD2;
			
		};
		
		/* No BLEND | NO diffuse | SKINNING */
		struct a2v_nodiffuse_skinning
		{
			POS
			NORM
			TEX0
			
			TANGENT_a2v
			BINORMAL_a2v
			
			BLENDWEIGHTS 		//	float4 blendweights: TEXCOORD1;
			BLENDINDICES 		// 	float4 blendindices: TEXCOORD2;
			
		};
		
		//--------------------- diffuse ------------------------//
		
		/* No BLEND | diffuse | NO TEXTURE */
		struct a2v_diffuse_notex
		{
			POS
			NORM
			DIFFUSE
			
			INSTANCE_ID
			
		};
		
		/* No BLEND | diffuse */
		struct a2v_diffuse
		{
			POS
			NORM
      		DIFFUSE
			TEX0
			
			TANGENT_a2v
			BINORMAL_a2v
			
			INSTANCE_ID
			
		};
		
		/* No BLEND | diffuse | NO TEXTURE | SKINNING */
		struct a2v_diffuse_notex_skinning
		{
			POS
			NORM
      		DIFFUSE
			
			BLENDWEIGHTS 	//	float4 blendweights: TEXCOORD1;
			BLENDINDICES 	// 	float4 blendindices: TEXCOORD2;
			
		};
		
		/* No BLEND | diffuse | SKINNING */
		struct a2v_diffuse_skinning
		{
			POS
			NORM
      		DIFFUSE
			TEX0
			
			TANGENT_a2v
			BINORMAL_a2v
			
			BLENDWEIGHTS 	//	float4 blendweights: TEXCOORD1;
			BLENDINDICES 	// 	float4 blendindices: TEXCOORD2;
			
		};
		
		////////////////////////////// alpha blend //////////////////////////////////
		
		//---------------------  no diffuse ------------------------//
		
		/* BLEND | NO diffuse | NO TEXTURE */
		struct a2v_blend_nodiffuse_notex
		{
			POS
			
			INSTANCE_ID
			
			NORM_ALPHABLEND
				
		};
		
		/* BLEND | NO diffuse */
		struct a2v_blend_nodiffuse
		{
			POS
			TEX0
			
			INSTANCE_ID
			
			NORM_ALPHABLEND
					
		};
		
		/* BLEND | NO diffuse | NO TEXTURE | SKINNING */
		struct a2v_blend_nodiffuse_notex_skinning
		{
			POS
		  
			BLENDWEIGHTS 	//	float4 blendweights: TEXCOORD1;
			BLENDINDICES 	// 	float4 blendindices: TEXCOORD2;
			
			NORM_ALPHABLEND
			
		};
		
		/* BLEND | NO diffuse | SKINNING */
		struct a2v_blend_nodiffuse_skinning
		{
			POS
			TEX0
		  
			BLENDWEIGHTS 	//	float4 blendweights: TEXCOORD1;
			BLENDINDICES 	// 	float4 blendindices: TEXCOORD2;
			
			NORM_ALPHABLEND
			
		};
		
		//---------------------  diffuse ------------------------//
		
		/* BLEND | diffuse | NO TEXTURE */
		struct a2v_blend_diffuse_notex
		{
			POS
			DIFFUSE
			
			INSTANCE_ID
			
			NORM_ALPHABLEND
			
		};
		
		/* BLEND | diffuse */
		struct a2v_blend_diffuse
		{
			POS
			DIFFUSE
			TEX0
			
			INSTANCE_ID
			
			NORM_ALPHABLEND
			
		};
		
		/* BLEND | diffuse | NO TEXTURE | SKINNING */
		struct a2v_blend_diffuse_notex_skinning
		{
			POS
			DIFFUSE
		  
			BLENDWEIGHTS 	//	float4 blendweights: TEXCOORD1;
			BLENDINDICES 	// 	float4 blendindices: TEXCOORD2;
			
			NORM_ALPHABLEND
			
		};
		
		/* BLEND | diffuse | SKINNING */
		struct a2v_blend_diffuse_skinning
		{
			POS
			DIFFUSE
			TEX0
		  
			BLENDWEIGHTS 	//	float4 blendweights: TEXCOORD1;
			BLENDINDICES 	// 	float4 blendindices: TEXCOORD2;
			
			NORM_ALPHABLEND
			
		};
		
		////////////////////////////// POST Z //////////////////////////////////
		
		/* POST Z */
		struct a2v_postzpass
		{
			POS
			
			INSTANCE_ID
			
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////// VS ----->>>>> PS ///////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////
		
		/* NO TEXTURE */
		struct v2p_notex
		{
			POS
			DIFFUSE
			
			float3 viewpos: TEXCOORD0;
			float3 normal: TEXCOORD1;
			
		};

		/* DEFAULT */
		struct v2p
		{
			POS
			DIFFUSE
			TEX0
			
			float3 viewpos: TEXCOORD1;
			float3 normal: TEXCOORD2;
			
			TANGENT_v2p
			BINORMAL_v2p
			
		};
		
		/* BLEND | NO TEXTURE */
		struct v2p_blend_notex
		{
			POS
			DIFFUSE
			
			VIEWPOS_ALPHABLEND_v2p
			NORM_ALPHABLEND_v2p
			
		};
		
		/* BLEND DEFAULT */
		struct v2p_blend
		{
			POS
			DIFFUSE
			TEX0
			
			VIEWPOS_ALPHABLEND_v2p
			NORM_ALPHABLEND_v2p
			
		};
		
		/* NO TEXTURE | SHADOW MAP */
		struct v2p_notex_shadowmap
		{
			POS
			
			float depth: TEXCOORD1;
			
		};
		
		/* SHADOW MAP */
		struct v2p_shadowmap
		{
			POS
			TEX0
			
			float depth: TEXCOORD1;
			
		};
		
		/* POST Z */
		struct v2p_postzpass
		{
			POS
			
		};
		
		/* SSSSS */
		struct v2p_sssss
		{
			POS
			DIFFUSE
			TEX0
			
			float4 screenCoord: TEXCOORD1;
			float3 pos_vs: TEXCOORD2;
			float3 normal_vs: TEXCOORD3;
			
		};
		
		/* Environment */
		struct v2p_env
		{
			POS
			DIFFUSE
			TEX0
			
			float3 viewpos: TEXCOORD1;
			float3 normal: TEXCOORD2;
			
			TANGENT_v2p
			BINORMAL_v2p
			
			float4 screenCoord: TEXCOORD5;
		};
		
	]]></code>
</material>
