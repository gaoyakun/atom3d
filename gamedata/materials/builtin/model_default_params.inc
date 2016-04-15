<?xml version="1.0"?>
<material>
	<param name="hasVertexColor" type="int" default="0" editortype="bool" />
    <param name="hasDiffuseTexture" type="int" default="0" editortype="bool" />
    <param name="enableAlphaBlend" type="int" default="0" editortype="bool" />
    <param name="enableAlphaTest" type="int" defualt="0" editortype="bool" />
    <param name="enablePostZPass" type="int" default="0" editortype="bool" />
    <param name="projectable" type="int" default="0" editortype="bool" />
    <param name="enableSkinning" type="int" />
    <param name="alpharef" type="float" editortype="none" />
    <param name="transparency" type="float4" default="1,1,1,1" editortype="none" />
    <param name="Kd" type="float" default="1" editortype="none" />
    <param name="Ks" type="float" default="1" editortype="none" />
    <param name="halfLambertScale" type="float" default="1.0" editortype="none" />
    <param name="cullmode" type="int" default="1" editortype="e_cullmode" />
    <param name="srcBlend" type="int" default="5" editortype="e_blendfunc" />
    <param name="destBlend" type="int" default="6" editortype="e_blendfunc" />
    <param name="filter" type="int" default="13" editortype="e_samplerfilter" />
    <param name="miplod" type="float" default="0" editortype="none" />
    <param name="diffuseTexture" type="texture" editortype="none" />
    <param name="diffuseSampler" type="sampler2D">
        <texture value="$diffuseTexture" />
        <filter value="$filter" />
		<lodbias value="$miplod" />
        <addressu value="wrap" />
        <addressv value="wrap" />
        <srgb value="true" />
    </param>
    <param name="diffuseColor" type="float4" default="1.0,1.0,1.0,1.0" editortype="color" />
    <param name="MVP" type="float4x4" default="$MVP_Matrix" />
    <param name="worldview" type="float4x4" default="$WorldView_Matrix" />
	<param name="View" type="float4x4" default="$View_Matrix" />
    <param name="shininess" type="float" default="64" editortype="none" />
    <param name="glossness" type="float" default="0" editortype="none" />
	<param name="colorMultiplier" type="float" default="1" editortype="none" />
	<param name="thickness" type="float" default="0.0" editortype="none" />
	<param name="TranlucentTexture" type="texture" default="/textures/default/black.dds" editortype="none" />
    <param name="TranlucentSampler" type="sampler2D">
        <texture value="$TranlucentTexture" />
        <filter value="$filter" />
		<lodbias value="$miplod" />
        <addressu value="wrap" />
        <addressv value="wrap" />
        <srgb value="false" />
    </param>
	<param name="envLighting" type="int" default="0" editortype="none" />
	<param name="envLightingMultiplier" type="float" default="1" editortype="none" />
	<param name="envTexture" type="texture" editortype="none"/>
	<param name="cubeSampler" type="samplerCUBE" >
		<texture value="$envTexture"/>
		<filter value="lll"/>
		<srgb value="true" />
	</param>
	<param name="sphereSampler" type="sampler2D" >
		<texture value="$envTexture"/>
		<addressu value="clamp"/>
		<addressv value="clamp"/>
		<filter value="lll"/>
		<srgb value="true" />
	</param>
</material>
