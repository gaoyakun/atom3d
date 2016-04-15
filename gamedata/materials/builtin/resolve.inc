<?xml version="1.0"?>
<material auto="0">
	<param name="resolveScale" type="float" default="0"/>
	<param name="resolveMaskTex" type="texture" default="/textures/default/perlinnoise_small.dds" editortype="none"/>
	<param name="resolveMaskSampler" type="sampler2D">
		<filter value="lln"/>
		<addressu value="clamp"/>
		<addressv value="clamp"/>
		<texture value="$resolveMaskTex"/>
		<srgb value="false"/>
	</param>
	
	<code><![CDATA[
		
		void resolve( in float2 uv, inout float4 color )
		{
			float resolvemask = 1.0 - tex2D (resolveMaskSampler, uv).r;
			color.a -= resolvemask * resolveScale;
			color.a = saturate( color.a );
		}
				
	]]></code>
</material>