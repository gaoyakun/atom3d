<?xml version="1.0"?>
<material>
	<param name="texture" type="texture" />
	<param name="zfunc" type="int" />
	<param name="filter" type="int" />
	<param name="diffuseSampler" type="sampler2D">
		<filter value="$filter" />
		<addressu value="clamp" />
		<addressv value="clamp" />
		<texture value="$texture" />
	</param>
	<code><![CDATA[
		struct v2p
		{
			float4 position : POSITION0;
      float4 diffuse : COLOR0;
			float2 tex : TEXCOORD0;
      float2 rc : TEXCOORD1;
		};
	]]></code>						
</material>
