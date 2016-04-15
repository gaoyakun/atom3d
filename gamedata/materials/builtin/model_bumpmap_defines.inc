<?xml version="1.0"?>
<material>
	<param name="autoTangentFrame" type="int" editortype="bool" default="false"/>
	<param name="time" type="float" default="$TimeInSecond" />
	<param name="fluxSpeed" type="float" default="0" editortype="none" />
	
	<param name="normalTexture" type="texture" editortype="none"/>
	<param name="normalSampler" type="sampler2D">
		<texture value="$normalTexture"/>
		<filter value="lln"/>
		<srgb value="false"/>
	</param>
  
	<param name="glossTexture" type="texture" editortype="none"/>
	<param name="glossSampler" type="sampler2D">
		<texture value="$glossTexture"/>
		<filter value="lln"/>
		<srgb value="true"/>
	</param>
	<code>
        <![CDATA[
		
		//////////////////////////////////// defines ////////////////////////////////////////////////////
		// tangent frame
		
		#ifdef TANGENT_a2v
		#undef TANGENT_a2v
		#endif
		#define 	TANGENT_a2v		float3 tangent: TANGENT;
		
		#ifdef BINORMAL_a2v
		#undef BINORMAL_a2v
		#endif
		#define 	BINORMAL_a2v	float3 binormal: BINORMAL;
		
		#ifdef TANGENT_v2p
		#undef TANGENT_v2p
		#endif
		#define 	TANGENT_v2p		float3 tangent: TEXCOORD3;
		
		#ifdef BINORMAL_v2p
		#undef BINORMAL_v2p
		#endif
		#define 	BINORMAL_v2p	float3 binormal: TEXCOORD4;
		
		// enable bump mapping
		#ifdef USE_BUMP
		#undef USE_BUMP
		#endif
		#define  	USE_BUMP		1

		////////////////////////////////////////////////////////////////////////////////////////////////////////
	]]></code>
</material>
