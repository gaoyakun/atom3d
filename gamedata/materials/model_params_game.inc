<?xml version="1.0"?>
<material>
	<param name="enablebodyEffect" type="int" default="0" editortype="none" />
	<param name="bodyEffectTime" type="float" default="$TimeInSecond" />
    <param name="bodyEffectColor" type="float4" default="1.0,1.0,1.0,0.0" editortype="color" />
	<param name="bodyEffectParams" type="float4" default="0.0,1.0,1.0,0.0" editortype="none" />
	<param name="edgeHilightColor" type="float4" default="0.0,0.0,0.0,1.0" editortype="color" />
	<param name="edgeHilightParams" type="float4" default="1.0,0.0,1.0,0.0" editortype="none" />
	<param name="bodyEffectTexture" type="texture" default="/textures/default/white.dds" editortype="none" />
    <param name="bodyEffectSampler" type="sampler2D">
        <texture value="$bodyEffectTexture" />
        <filter value="lln" />
        <addressu value="wrap" />
        <addressv value="wrap" />
        <srgb value="true" />
    </param>
</material>
