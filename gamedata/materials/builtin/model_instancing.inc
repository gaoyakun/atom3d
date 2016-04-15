<?xml version="1.0"?>
<material>
	<define name="NUM_INSTANCE_PER_BATCH" value="50" />
	<param name="instanceDatas" type="float4[NUM_INSTANCE_PER_BATCH*3]" />
	<param name="viewMatrix" type="float4x4" default="$Inv_View_Matrix" />
	<param name="VP" type="float4x4" default="$ViewProjection_Matrix" />
	<code>
        <![CDATA[
		
		//////////////////////////////////// defines ////////////////////////////////////////////////////
		// geometry instancing spec
		#ifdef INSTANCE_ID
		#undef INSTANCE_ID
		#endif
		#define INSTANCE_ID    float instanceId: TEXCOORD3;		// instancing ID
		
		// enable bump mapping
		#ifdef USE_INSTANCING
		#undef USE_INSTANCING
		#endif
		#define  	USE_INSTANCING		1
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		// output the world position
		void parseInstanceData(	in int id,
								in float4 position,
								out float4 worldpos )
		{
			float4 world0 = instanceDatas[id * 3];
			float4 world1 = instanceDatas[id * 3 + 1];
			float4 world2 = instanceDatas[id * 3 + 2];
			
			float3x3 worldmatrix = float3x3( world0.xyz,world1.xyz,world2.xyz);
			
			#if 0
				float3 translate = float3( world0.w, world1.w, world2.w );
				worldpos.xyz = mul(position.xyz, worldmatrix);
				worldpos.xyz += translate;
			#else
				float4x4 worldMat = float4x4(	world0.x,world0.y,world0.z,0,
												world1.x,world1.y,world1.z,0,
												world2.x,world2.y,world2.z,0,
												world0.w,world1.w,world2.w,1);
												
				worldpos = mul( float4(position.xyz,1), worldMat );
			#endif
			
			worldpos.w = 1;
		}
		
		// output the world position & world matrix
		void parseInstanceData(	in int id, 
								in float4 position,
								out float4 worldpos, 
								out float3x3 worldmatrix )
		{
			float4 world0 = instanceDatas[id * 3];
			float4 world1 = instanceDatas[id * 3 + 1];
			float4 world2 = instanceDatas[id * 3 + 2];
			
			worldmatrix = float3x3( world0.xyz,world1.xyz,world2.xyz);
			
			#if 0
				float3 translate = float3( world0.w, world1.w, world2.w );
				worldpos.xyz = mul(position.xyz, worldmatrix);
				worldpos.xyz += translate;
			#else
			
				float4x4 worldMat = float4x4(	world0.x,world0.y,world0.z,0,
												world1.x,world1.y,world1.z,0,
												world2.x,world2.y,world2.z,0,
												world0.w,world1.w,world2.w,1);
												
				worldpos = mul( float4(position.xyz,1), worldMat );
			
			#endif
			
			
			worldpos.w = 1;
		}
		
		float4 GetInstanceHPosition(in int id,in float4 position)
		{
			float4 worldpos;
			parseInstanceData(id, position, worldpos);
			return mul(worldpos, VP);
		}
		
		float4 GetInstanceHPosition(in int id,in float4 position,
									out float4 worldpos,out float3x3 worldmatrix)
		{
			parseInstanceData(id, position, worldpos, worldmatrix);
			return mul(worldpos, VP);
		}
		
	]]></code>
</material>
