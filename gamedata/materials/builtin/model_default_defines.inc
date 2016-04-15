<?xml version="1.0"?>
<material>
	<code>
        <![CDATA[

		/////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////// defines ///////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////
		
		//----------------------------------------//
		// default 
		#define 	POS 		float4 position: POSITION;
		#define 	TEX0 		float2 tex0: TEXCOORD0;
		#define 	NORM 		float3 normal: NORMAL;
		#define 	DIFFUSE 	float4 diffuse: COLOR0;
		//----------------------------------------//
		
		//----------------------------------------//
		// tangent frame( default is empty, set when use bump mapping)
		#define 	TANGENT_a2v
		#define 	BINORMAL_a2v
		#define 	TANGENT_v2p
		#define 	BINORMAL_v2p
		//----------------------------------------//
		
		//----------------------------------------//
		// skinning spec( default is empty, set when use skinning)
		#define 	BLENDWEIGHTS
		#define 	BLENDINDICES
		//----------------------------------------//
		
		//----------------------------------------//
		// geometry instancing spec( default is empty, set when use instancing)
		#define 	INSTANCE_ID
		//----------------------------------------//
		
		//----------------------------------------//
		// alpha blend material specific( default is empty, set when use rim lighting for alpha blend material )
		#define 	NORM_ALPHABLEND						// 	normal for alpha blend material
		#define     VIEWPOS_ALPHABLEND_v2p 				//	view space position for alpha blend material( float3 viewpos: TEXCOORD1; )
		#define     NORM_ALPHABLEND_v2p 				//	view space normal for alpha blend material ( float3 normal: TEXCOORD2; )
		//----------------------------------------//
		
		//----------------------------------------//
		// settings/toggles
		#define  	USE_SKINNING	0			// skinning animation ( default is disabled )
		#define  	USE_BUMP		0			// bump mapping ( default is disabled )
		#define  	USE_FLUX		0			// flux ( default is disabled )
		#define  	USE_INSTANCING	0			// geometry instancing ( default is disabled )
		#define  	USE_RIMLIGHT	0			// rim lighting ( default is disabled )
		
		#define 	USE_COMPRESSION_VERSION 0	// use compression format ( default is disabled )
		//----------------------------------------//
		
		
		
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		
	]]></code>
</material>
