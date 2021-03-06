<?xml version="1.0"?>
<material>
	<define name="NUM_JOINT_MATRICES" value="78" />
	<param name="boneMatrices" type="float4[NUM_JOINT_MATRICES*3]" default="$BoneMatrices"/>
	<code>
        <![CDATA[
		
		//////////////////////////////////// defines ////////////////////////////////////////////////////
		// skinning spec
		
		#ifdef BLENDWEIGHTS
		#undef BLENDWEIGHTS
		#endif
		#define 	BLENDWEIGHTS	float4 blendweights: TEXCOORD1;
		
		#ifdef BLENDINDICES
		#undef BLENDINDICES
		#endif
		#define 	BLENDINDICES	float4 blendindices: TEXCOORD2;
		
		// enable skinning
		#ifdef USE_SKINNING
		#undef USE_SKINNING
		#endif
		#define 	USE_SKINNING	1
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		float3 SkinningPositionTransform (in float4 posBindPos, in float4 jointIndices, in float4 jointWeights)
		{
			float3 p = 0;	
			float4 indices = jointIndices*3;
			for (int i = 0; i < 4; ++i)
			{
				p += float3(dot(posBindPos, boneMatrices[indices[i]+0]), 
							dot(posBindPos, boneMatrices[indices[i]+1]), 
							dot(posBindPos, boneMatrices[indices[i]+2])
							) * jointWeights[i];
			}
			return p;
		}
		float3 SkinningVectorTransform (in float3 vecBindPos, in float4 jointIndices, in float4 jointWeights)
		{
			float3 p = 0;
			float4 indices = jointIndices*3;
			for (int i = 0; i < 4; ++i)
			{
				p += float3(dot(vecBindPos, boneMatrices[indices[i]+0].xyz), 
							dot(vecBindPos, boneMatrices[indices[i]+1].xyz), 
							dot(vecBindPos, boneMatrices[indices[i]+2].xyz)
							) * jointWeights[i];
			}
			return p;
		}
	]]></code>
</material>
