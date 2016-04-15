<?xml version="1.0"?>
<material>
	<param name="WorldViewProj" type="float4x4" />
	<param name="v3LightDir" type="float4" />
	<param name="v3CameraPos" type="float4" />
	<param name="v3InvWaveLength" type="float4" />
	<param name="fCameraHeight" type="float" />
	<param name="fSkydomeRadius" type="float" />
	<param name="KrESun" type="float" />
	<param name="KmESun" type="float" />
	<param name="g" type="float4" />
	<param name="fExposure" type="float" />
	<code>
    <![CDATA[
		struct a2v
		{
			float4 Position : POSITION0;
			float3 Diffuse : TEXCOORD0;
		};
		struct v2p
		{
			float4 Position : POSITION0;
			float3 Direction : TEXCOORD0;
			float4 RayleighColor : TEXCOORD1;
			float4 MieColor : TEXCOORD2;
		};
		struct p2c
		{
			float4 color : COLOR0;
			float depth : DEPTH0;
		};
	]]></code>
</material>
