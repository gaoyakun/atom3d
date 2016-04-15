<?xml version="1.0"?>
<material>
	<code>
        <![CDATA[

		/////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////// defines ///////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////
		
		// enable rim lighting
		#ifdef 		USE_RIMLIGHT
		#undef 		USE_RIMLIGHT
		#endif
		#define 	USE_RIMLIGHT	1
		
		// 	normal for alpha blend material
		#ifdef 		NORM_ALPHABLEND
		#undef 		NORM_ALPHABLEND
		#endif
		#define 	NORM_ALPHABLEND	float3 normal: NORMAL;
		
		//	view space position for alpha blend material
		#ifdef 		VIEWPOS_ALPHABLEND_v2p
		#undef 		VIEWPOS_ALPHABLEND_v2p
		#endif
		#define     VIEWPOS_ALPHABLEND_v2p 	float3 viewpos: TEXCOORD1;
		
		//	view space normal for alpha blend material
		#ifdef 		NORM_ALPHABLEND_v2p
		#undef 		NORM_ALPHABLEND_v2p
		#endif
		#define     NORM_ALPHABLEND_v2p 	float3 normal: TEXCOORD2;
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		
	]]></code>
</material>
