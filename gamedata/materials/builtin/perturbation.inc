<?xml version="1.0"?>
<material auto="0">
	<param name="perturbation" type="int" default="0" editortype="none"/>
	
	<param name="softenFactor" type="float" default="0.1" editortype="none"/>
	<param name="perturbParams" type="float4" default="1.0,1.0,1.0,1.0" editortype="none"/>
	
	<param name="Time" type="float" default="$TimeInSecond" />
	
	<param name="ShadedSceneMap"  type="texture" default="$Deferred_ShadedSceneTexture" />
	<param name="shadedSceneSampler" type="sampler2D">
		<filter value="ppn"/>
		<addressu value="clamp"/>
		<addressv value="clamp"/>
		<texture value="$ShadedSceneMap"/>
		<srgb value="true"/>
	</param>
	<param name="perturbTexture" type="texture" editortype="none"/>
	<param name="perturbSampler" type="sampler2D">
		<filter value="lln"/>
		<addressu value="wrap"/>
		<addressv value="wrap"/>
		<texture value="$perturbTexture"/>
		<srgb value="false"/>
	</param>
	<param name="perturbMaskTex" type="texture" editortype="none"/>
	<param name="perturbMaskSampler" type="sampler2D">
		<filter value="lln"/>
		<addressu value="clamp"/>
		<addressv value="clamp"/>
		<texture value="$perturbMaskTex"/>
		<srgb value="false"/>
	</param>
	<code><![CDATA[
		
		float4 SiGrowablePoissonDisc13FilterRGB(sampler2D tSource, float2 texCoord, float2 pixelSize, float discRadius)
		{
			float2	poisson[12] = 	{	float2(-0.326212f, -0.40581f),
										float2(-0.840144f, -0.07358f),
										float2(-0.695914f, 0.457137f),
										float2(-0.203345f, 0.620716f),
										float2(0.96234f, -0.194983f),
										float2(0.473434f, -0.480026f),
										float2(0.519456f, 0.767022f),
										float2(0.185461f, -0.893124f),
										float2(0.507431f, 0.064425f),
										float2(0.89642f, 0.412458f),
										float2(-0.32194f, -0.932615f),
										float2(-0.791559f, -0.59771f)
									};
									
			// Center tap
			float4 center = tex2D (tSource, texCoord);
			float2 coord = texCoord.xy;
			
			float4 cOut = center;
			
			for(int tap = 0; tap < 12; tap++)
			{
				coord = texCoord.xy + ( pixelSize * poisson[tap] * discRadius);
				
				// Sample pixel
				cOut += tex2D(tSource, coord);
			}
		
			return float4( cOut.rgb / 13.0f, center.a );
		}
				
	]]></code>
</material>