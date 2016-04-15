#include "StdAfx.h"
#include "modelanimationtrack.h"
#include "instancemesh.h"
#include "instanceskeleton.h"

unsigned trackCount = 0;

ATOM_ModelAnimationTrack::ATOM_ModelAnimationTrack (void)
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::ATOM_ModelAnimationTrack);

	trackCount++;

	_propIndex = -1;
	_jointMatricesIndex = -1;
	_lastFrame = -1.f;
	_hashcode = 0;
	_bboxComputed = false;
	_bboxPrecomputed = false;
}

ATOM_ModelAnimationTrack::~ATOM_ModelAnimationTrack (void)
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::~ATOM_ModelAnimationTrack);

	trackCount--;
}

unsigned ATOM_ModelAnimationTrack::getTotalAnimationTime (void) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::getTotalAnimationTime);

	return _keyframes.empty () ? 0 : _keyframes.back().frameTime - _keyframes.front().frameTime;
}

unsigned ATOM_ModelAnimationTrack::getAnimationParts (ATOM_InstanceMesh *mesh, float frame, bool animationChanged) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::getAnimationParts);

	int meshIndex = mesh->getIndex();
	bool contentLost = mesh->isContentLost();
	unsigned frame1 = ATOM_floor(frame);
	unsigned frame2 = ((frame - frame1) == 0.f) ? frame1 : frame1 + 1;

	unsigned ret = 0;

	int jointIndex = -1;

	if (meshIndex < _keyframes[frame1].meshKeyFrames.size())
	{
		const MeshKeyFrame &mkf1 = _keyframes[frame1].meshKeyFrames[meshIndex];
		const MeshKeyFrame &mkf2 = _keyframes[frame2].meshKeyFrames[meshIndex];

		int vertexIndex1 = mkf1.vertexIndex;
		if (vertexIndex1 >= 0)
		{
			ret |= AP_VERTEX;
		}

		int uvIndex1 = mkf1.uvIndex;
		if (uvIndex1 >= 0)
		{
			ret |= AP_UV;
		}

		jointIndex = mkf1.jointIndex;
	}

	if (jointIndex < 0)
		jointIndex = _keyframes[frame1].jointIndex;

	if (jointIndex >= 0 && !mesh->getMesh()->weights.empty() && !mesh->getMesh()->vertices_sa.empty())
	{
		ret |= AP_VERTEX;
		ret |= AP_NORMAL;
	}

	return ret;
}

bool ATOM_ModelAnimationTrack::needProcessVertices (ATOM_InstanceMesh *mesh, float frame, bool animationChanged) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::needProcessVertices);

	int meshIndex = mesh->getIndex();
	unsigned frame1 = ATOM_floor(frame);
	unsigned frame2 = ((frame - frame1) == 0.f) ? frame1 : frame1 + 1;

	if (meshIndex < _keyframes[frame1].meshKeyFrames.size())
	{
		int vertexIndex1 = _keyframes[frame1].meshKeyFrames[meshIndex].vertexIndex >= 0;
		if (vertexIndex1 >= 0)
		{
			return true;
		}

		int jointIndex = -1;
		if (meshIndex < _keyframes[frame1].meshKeyFrames.size())
			jointIndex = _keyframes[frame1].meshKeyFrames[meshIndex].jointIndex;
		if (jointIndex < 0)
			jointIndex = _keyframes[frame1].jointIndex;

		return jointIndex >= 0 && !mesh->getMesh()->weights.empty() && !mesh->getMesh()->vertices_sa.empty();
	}

	return false;
}

bool ATOM_ModelAnimationTrack::needProcessUVs (ATOM_InstanceMesh *mesh, float frame, bool animationChanged) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::needProcessUVs);

	int meshIndex = mesh->getIndex ();
	unsigned frame1 = ATOM_floor(frame);
	unsigned frame2 = ((frame - frame1) == 0.f) ? frame1 : frame1 + 1;

	if (meshIndex < _keyframes[frame1].meshKeyFrames.size())
	{
		int uvIndex1 = _keyframes[frame1].meshKeyFrames[meshIndex].uvIndex >= 0;
		if (uvIndex1 >= 0)
		{
			return true;
		}
	}
	return false;
}

bool ATOM_ModelAnimationTrack::needProcessNormals (ATOM_InstanceMesh *mesh, float frame, bool animationChanged) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::needProcessNormals);

	int meshIndex = mesh->getIndex ();
	unsigned frame1 = ATOM_floor(frame);

	int jointIndex = -1;
	if (meshIndex < _keyframes[frame1].meshKeyFrames.size())
		jointIndex = _keyframes[frame1].meshKeyFrames[meshIndex].jointIndex;
	if (jointIndex < 0)
		jointIndex = _keyframes[frame1].jointIndex;

	if (jointIndex >= 0 && !mesh->getMesh()->weights.empty() && !mesh->getMesh()->vertices_sa.empty())
	{
		return true;
	}

	return false;
}

bool ATOM_ModelAnimationTrack::updateMeshMT (ATOM_MeshAnimationContext *context) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::updateMeshMT);

	static int reportError = 0;

	ATOM_Mutex::ScopeMutex updateLock (_updateLock);

	if (_keyframes.empty ()) 
	{
		return false;
	}

	// 检验和mesh的匹配
	if (_hashcode != 0)
	{
		//需要匹配验证 
		if (_hashcode != context->imesh->getMesh()->hashcode)
		{
			//未匹配
			//return false;
		}
	}

	unsigned cycleTick = context->cycleTick;
	bool billboard = context->imesh->getMesh()->billboard;

	// get the index of the core mesh within the model
	int index = context->meshIndex;
	ATOM_ASSERT(index >= 0);

	// calculate the current frame
	unsigned totalTime = getTotalAnimationTime();
	ATOM_ASSERT(cycleTick <= totalTime);

	float theFrame = getAnimationFrame (cycleTick);
	if (theFrame < 0.f)
	{
		return false;
	}

	int frame1 = ATOM_ftol(theFrame);
	float factor = theFrame - frame1;
	int frame2 = ((factor == 0.f) ? frame1 : frame1 + 1);

	context->lastAnimationFrame = factor + frame1;

	bool processVerts = false;
	bool processUVs = false;

	if (index < _keyframes[frame1].meshKeyFrames.size())
	{
		// get the animation keyframe information of the mesh
		const MeshKeyFrame &keyframeMesh1 = _keyframes[frame1].meshKeyFrames[index];
		const MeshKeyFrame &keyframeMesh2 = _keyframes[frame2].meshKeyFrames[index];

		float transparency = keyframeMesh1.transparency + (keyframeMesh2.transparency - keyframeMesh1.transparency) * factor;
		ATOM_Vector4f color = context->diffuseColorIn;
		color.w = transparency;
		context->diffuseColorOut = color;

		// process vertex animation if possible
		processVerts = keyframeMesh1.vertexIndex >= 0 ;
		processUVs = keyframeMesh1.uvIndex >= 0;

		bool processVertices = processVerts || processUVs;
		if (processVertices)
		{
			const ATOM_Vector3f *vertices1 = 0, *vertices2 = 0;
			const ATOM_Vector2f *uvs1 = 0, *uvs2 = 0;
			unsigned numVerts = 0;

			// update vertex animation
			if (processVerts && !context->vertices)
			{
				return false;
			}
			if (processUVs && !context->uvs)
			{
				return false;
			}

			if (processVerts)
			{
				numVerts = _keyframeDatas->getVertexCount(keyframeMesh1.vertexIndex);
				vertices1 = _keyframeDatas->getVertices(keyframeMesh1.vertexIndex);
				vertices2 = _keyframeDatas->getVertices(keyframeMesh2.vertexIndex);
			}

			if (processUVs)
			{
				uvs1 = _keyframeDatas->getUVs(keyframeMesh1.uvIndex);
				uvs2 = _keyframeDatas->getUVs(keyframeMesh2.uvIndex);
				if (numVerts == 0)
				{
					numVerts = _keyframeDatas->getUVCount (keyframeMesh1.uvIndex);
				}
			}

			ATOM_Vector3f *verts = (ATOM_Vector3f*)context->vertices;
			ATOM_Vector2f *uvs = (ATOM_Vector2f*)context->uvs;

			updateVertexAnimation (vertices1, uvs1, vertices2, uvs2, factor, verts, uvs, numVerts, context->imesh->getMesh()->uvAnimationNoIpol);

			if (processVerts)
			{
				if (context->imesh->getMesh()->billboard)
				{
					const ATOM_Vector3f &n1 = keyframeMesh1.billboardNormal;
					const ATOM_Vector3f &n2 = keyframeMesh2.billboardNormal;
					ATOM_Vector3f n = n1 + (n2 - n1) * factor;
					n.normalize ();
					context->billboardNormal = n;

					const ATOM_Vector3f &x1 = keyframeMesh1.billboardXAxis;
					const ATOM_Vector3f &x2 = keyframeMesh2.billboardXAxis;
					ATOM_Vector3f x = x1 + (x2-x1) * factor;
					x.normalize ();
					context->billboardXAxis = x;

					const ATOM_Vector3f &c1 = keyframeMesh1.billboardCenter;
					const ATOM_Vector3f &c2 = keyframeMesh2.billboardCenter;
					ATOM_Vector3f c = c1 + (c2-c1) * factor;
					context->billboardCenter = c;

					const ATOM_Matrix4x4f *m = _keyframeDatas->getTransforms();
					const ATOM_Matrix4x4f &matrix1 = m[keyframeMesh1.transformIndex];
					const ATOM_Matrix4x4f &matrix2 = m[keyframeMesh2.transformIndex];
					context->worldMatrix = matrix1 + (matrix2 - matrix1) * factor;
				}
			}
			else
			{
				context->billboardNormal = context->imesh->getMesh()->billboardNormal;
				context->billboardXAxis = context->imesh->getMesh()->billboardXAxis;
			}
		}
	}

	// process skeletal animation if possible
	int jointIndex1 = -1;
	if (index < _keyframes[frame1].meshKeyFrames.size())
		jointIndex1 = _keyframes[frame1].meshKeyFrames[index].jointIndex;
	if (jointIndex1 < 0)
		jointIndex1 = _keyframes[frame1].jointIndex;

	int jointIndex2 = -1;
	if (index < _keyframes[frame2].meshKeyFrames.size())
		jointIndex2 = _keyframes[frame2].meshKeyFrames[index].jointIndex;
	if (jointIndex2 < 0)
		jointIndex2 = _keyframes[frame2].jointIndex;

	// must update the skeleton
	if (jointIndex1 >= 0 && jointIndex2 >= 0)
	{
		unsigned numJointTransformInfos = _keyframeDatas->getJointCount(jointIndex1);
		const ATOM_JointTransformInfo *jointTransforms1 = _keyframeDatas->getJoints(jointIndex1);
		const ATOM_JointTransformInfo *jointTransforms2 = _keyframeDatas->getJoints(jointIndex2);

		_blendedJointTransforms.resize (numJointTransformInfos);
		slerpJointQuats (&_blendedJointTransforms[0], jointTransforms1, jointTransforms2, factor, _blendedJointTransforms.size());

#if 0
		for (unsigned i = 0; i < _blendedJointTransforms.size(); ++i)
		{
			const ATOM_Vector4f &v = _blendedJointTransforms[i].scale;
			ATOM_LOGGER::log ("%f, %f, %f\n", v.x, v.y, v.z);
		}
#endif

		_jointMatrices.resize (numJointTransformInfos);
		convertJointQuatToJointMat (&_jointMatrices[0], &_blendedJointTransforms[0], _jointMatrices.size());
		if (context->skeleton && context->skeleton->getSkeleton()->getJointParents().size() > 0)
		{
			// transform joint to global space
			transformSkeleton (&_jointMatrices[0], &context->skeleton->getSkeleton()->getJointParents()[0], _jointMatrices.size());
		}

		if (processVerts)
		{
			// no need to update the vertices if vertex animation was used.
			return true;
		}
	}

	bool processSkeletons = (jointIndex1 >= 0 && jointIndex2 >= 0 && !context->imesh->getMesh()->weights.empty() && !context->imesh->getMesh()->vertices_sa.empty());

	if (processSkeletons)
	{
		bool needRemapJoint = context->imesh->getJointMap().size() > 0 && _jointHashes.size() > 0;

		unsigned numVerts = context->imesh->getMesh()->vertices_sa.size();

		unsigned vertexAttrib = context->vertexAttrib;

		bool needNorm = (vertexAttrib & ATOM_VERTEX_ATTRIB_NORMAL) != 0;

		ATOM_Vector3f *verts = (ATOM_Vector3f*)context->vertices;
		if (!verts)
		{
			return false;
		}

		ATOM_Vector3f *norms = (ATOM_Vector3f*)context->normals;
		if (needNorm && !norms)
		{
			return false;
		}

		const ATOM_Mesh::SkeletonVertex *base = &context->imesh->getMesh()->vertices_sa[0];
		if (needRemapJoint)
		{
			if (!norms)
			{
				transformVerts (verts, numVerts, &_jointMatrices[0], base, &context->imesh->getMesh()->weights[0], &context->imesh->getJointMap()[0]);
			}
			else
			{
				transformVertsAndNormals (verts, norms, numVerts, &_jointMatrices[0], base, &context->imesh->getMesh()->weights[0], &context->imesh->getJointMap()[0]);
			}
		}
		else
		{
			if (!norms)
			{
				transformVerts (verts, numVerts, &_jointMatrices[0], base, &context->imesh->getMesh()->weights[0]);
			}
			else
			{
				transformVertsAndNormals (verts, norms, numVerts, &_jointMatrices[0], base, &context->imesh->getMesh()->weights[0]);
			}
		}

		return true;
	}

	return false;
}

bool ATOM_ModelAnimationTrack::updateVertexAnimation (ATOM_InstanceMesh *mesh, unsigned cycleTick)
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::updateVertexAnimation);

	if (_keyframes.empty ()) 
	{
		return false;
	}

	bool billboard = mesh->getMesh()->billboard;

	// get the index of the core mesh within the model
	int index = mesh->getIndex ();
	ATOM_ASSERT(index >= 0);

	// calculate the current frame
	float theFrame = getAnimationFrame (cycleTick);
	if (theFrame < 0.f)
	{
		return false;
	}

	int frame1 = ATOM_ftol(theFrame);
	float factor = theFrame - frame1;
	int frame2 = ((factor == 0.f) ? frame1 : frame1 + 1);

	bool processVerts = false;
	bool processUVs = false;
	bool hasVertexAnimation = false;
	bool hasUVAnimation = false;

	if (index < _keyframes[frame1].meshKeyFrames.size())
	{
		// get the animation keyframe information of the mesh
		const MeshKeyFrame &keyframeMesh1 = _keyframes[frame1].meshKeyFrames[index];
		const MeshKeyFrame &keyframeMesh2 = _keyframes[frame2].meshKeyFrames[index];

		float transparency = keyframeMesh1.transparency + (keyframeMesh2.transparency - keyframeMesh1.transparency) * factor;
		ATOM_Vector4f color = mesh->getDiffuseColor ();
		color.w = ATOM_saturate (transparency);
		mesh->setDiffuseColor (color);

		// process vertex animation if possible
		hasVertexAnimation = processVerts = keyframeMesh1.vertexIndex >= 0;
		hasUVAnimation = processUVs = keyframeMesh1.uvIndex >= 0;

		bool processVertices = processVerts || processUVs;
		if (processVertices)
		{
			const ATOM_Vector3f *vertices1 = 0, *vertices2 = 0;
			const ATOM_Vector2f *uvs1 = 0, *uvs2 = 0;
			unsigned numVerts = 0;

			// update vertex animation
			if (processVerts)
			{
				numVerts = _keyframeDatas->getVertexCount(keyframeMesh1.vertexIndex);
				vertices1 = _keyframeDatas->getVertices(keyframeMesh1.vertexIndex);
				vertices2 = _keyframeDatas->getVertices(keyframeMesh2.vertexIndex);
			}

			if (processUVs)
			{
				uvs1 = _keyframeDatas->getUVs(keyframeMesh1.uvIndex);
				uvs2 = _keyframeDatas->getUVs(keyframeMesh2.uvIndex);
				if (numVerts == 0)
				{
					numVerts = _keyframeDatas->getUVCount (keyframeMesh1.uvIndex);
				}
			}

			ATOM_AUTOREF(ATOM_VertexArray) va = mesh->getVertexArray ();
			if (processVerts && !va)
			{
				// vertex deformation exists, create the intermediate vertex array if needed
				va = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_DYNAMIC, numVerts);
				mesh->setVertexArray (va.get());
			}
			else if (!processVerts && va)
			{
				// no vertex deformation, delete the intermediate vertex array
				// mesh->setVertexArray (0);
				va = 0;
			}

			ATOM_AUTOREF(ATOM_VertexArray) vuv = mesh->getUVArray ();
			if (processUVs && !vuv)
			{
				// uv deformation exists, create the intermediate vertex array if needed
				vuv = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_DYNAMIC, numVerts);
				mesh->setUVArray (vuv.get());
			}
			else if (!processUVs && vuv)
			{
				// no uv deformation, delete the intermediate vertex array
				mesh->setUVArray (0);
				vuv = 0;
			}

			ATOM_Vector3f *verts = va ? (ATOM_Vector3f*)va->lock (ATOM_LOCK_WRITEONLY, 0, 0, true) : 0;
			ATOM_Vector2f *uvs = vuv ? (ATOM_Vector2f*)vuv->lock (ATOM_LOCK_WRITEONLY, 0, 0, true) : 0;

			updateVertexAnimation (vertices1, uvs1, vertices2, uvs2, factor, verts, uvs, numVerts, mesh->getMesh()->uvAnimationNoIpol);

			if (va)
			{
				va->unlock ();
			}

			if (vuv)
			{
				vuv->unlock ();
			}

			if (processVerts)
			{
				if (mesh->getMesh()->billboard)
				{
					const ATOM_Vector3f &n1 = keyframeMesh1.billboardNormal;
					const ATOM_Vector3f &n2 = keyframeMesh2.billboardNormal;
					ATOM_Vector3f n = n1 + (n2 - n1) * factor;
					n.normalize ();
					mesh->setBillboardNormal (n);

					const ATOM_Vector3f &x1 = keyframeMesh1.billboardXAxis;
					const ATOM_Vector3f &x2 = keyframeMesh2.billboardXAxis;
					ATOM_Vector3f x = x1 + (x2-x1) * factor;
					x.normalize ();
					mesh->setBillboardXAxis (x);

					const ATOM_Vector3f &c1 = keyframeMesh1.billboardCenter;
					const ATOM_Vector3f &c2 = keyframeMesh2.billboardCenter;
					ATOM_Vector3f c = c1 + (c2-c1) * factor;
					mesh->setBillboardCenter (c);

					const ATOM_Matrix4x4f *m = _keyframeDatas->getTransforms ();
					const ATOM_Matrix4x4f &matrix1 = m[keyframeMesh1.transformIndex];
					const ATOM_Matrix4x4f &matrix2 = m[keyframeMesh2.transformIndex];
					mesh->setWorldMatrix (matrix1 + (matrix2 - matrix1) * factor);
				}
			}
			else
			{
				mesh->setBillboardNormal (mesh->getMesh()->billboardNormal);
				mesh->setBillboardXAxis (mesh->getMesh()->billboardXAxis);
			}

			return processVerts;
		}
	}

	return false;
}

bool ATOM_ModelAnimationTrack::updateMesh (unsigned cycleTick, ATOM_InstanceMesh *mesh, ATOM_InstanceSkeleton *skeleton) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::updateMesh);

	static int reportError = 0;

	ATOM_Mutex::ScopeMutex updateLock (_updateLock);

	if (_keyframes.empty ()) 
	{
		return false;
	}

	// 检验和mesh的匹配
	if (_hashcode != 0)
	{
		//需要匹配验证 
		if (_hashcode != mesh->getMesh()->hashcode)
		{
			//未匹配
			//return false;
		}
	}

	bool billboard = mesh->getMesh()->billboard;

	// get the index of the core mesh within the model
	int index = mesh->getIndex ();
	ATOM_ASSERT(index >= 0);

	// calculate the current frame
	unsigned totalTime = getTotalAnimationTime();
	ATOM_ASSERT(cycleTick <= totalTime);

	float theFrame = getAnimationFrame (cycleTick);
	if (theFrame < 0.f)
	{
		return false;
	}

	int frame1 = ATOM_ftol(theFrame);
	float factor = theFrame - frame1;
	int frame2 = ((factor == 0.f) ? frame1 : frame1 + 1);

	bool updateJoints = ATOM_floor(mesh->getLastAnimationFrame()) != frame1;
	mesh->setLastAnimationFrame(factor + frame1);

	bool processVerts = false;
	bool processUVs = false;
	bool hasVertexAnimation = false;
	bool hasUVAnimation = false;

	if (index < _keyframes[frame1].meshKeyFrames.size())
	{
		// get the animation keyframe information of the mesh
		const MeshKeyFrame &keyframeMesh1 = _keyframes[frame1].meshKeyFrames[index];
		const MeshKeyFrame &keyframeMesh2 = _keyframes[frame2].meshKeyFrames[index];

		float transparency = keyframeMesh1.transparency + (keyframeMesh2.transparency - keyframeMesh1.transparency) * factor;
		ATOM_Vector4f color = mesh->getDiffuseColor ();
		color.w = ATOM_saturate (transparency);
		mesh->setDiffuseColor (color);

		// process vertex animation if possible
		hasVertexAnimation = processVerts = keyframeMesh1.vertexIndex >= 0;
		hasUVAnimation = processUVs = keyframeMesh1.uvIndex >= 0;

		bool processVertices = processVerts || processUVs;
		if (processVertices)
		{
			const ATOM_Vector3f *vertices1 = 0, *vertices2 = 0;
			const ATOM_Vector2f *uvs1 = 0, *uvs2 = 0;
			unsigned numVerts = 0;

			// update vertex animation
			if (processVerts)
			{
				numVerts = _keyframeDatas->getVertexCount(keyframeMesh1.vertexIndex);
				vertices1 = _keyframeDatas->getVertices(keyframeMesh1.vertexIndex);
				vertices2 = _keyframeDatas->getVertices(keyframeMesh2.vertexIndex);
			}

			if (processUVs)
			{
				uvs1 = _keyframeDatas->getUVs(keyframeMesh1.uvIndex);
				uvs2 = _keyframeDatas->getUVs(keyframeMesh2.uvIndex);
				if (numVerts == 0)
				{
					numVerts = _keyframeDatas->getUVCount (keyframeMesh1.uvIndex);
				}
			}

			ATOM_AUTOREF(ATOM_VertexArray) va = mesh->getVertexArray ();
			if (processVerts && !va)
			{
				// vertex deformation exists, create the intermediate vertex array if needed
				va = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_DYNAMIC, numVerts);
				mesh->setVertexArray (va.get());
			}
			else if (!processVerts && va)
			{
				// no vertex deformation, delete the intermediate vertex array
				// mesh->setVertexArray (0);
				va = 0;
			}

			ATOM_AUTOREF(ATOM_VertexArray) vuv = mesh->getUVArray ();
			if (processUVs && !vuv)
			{
				// uv deformation exists, create the intermediate vertex array if needed
				vuv = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_DYNAMIC, numVerts);
				mesh->setUVArray (vuv.get());
			}
			else if (!processUVs && vuv)
			{
				// no uv deformation, delete the intermediate vertex array
				mesh->setUVArray (0);
				vuv = 0;
			}

			ATOM_Vector3f *verts = va ? (ATOM_Vector3f*)va->lock (ATOM_LOCK_WRITEONLY, 0, 0, true) : 0;
			ATOM_Vector2f *uvs = vuv ? (ATOM_Vector2f*)vuv->lock (ATOM_LOCK_WRITEONLY, 0, 0, true) : 0;

			updateVertexAnimation (vertices1, uvs1, vertices2, uvs2, factor, verts, uvs, numVerts, mesh->getMesh()->uvAnimationNoIpol);

			if (va)
			{
				va->unlock ();
			}

			if (vuv)
			{
				vuv->unlock ();
			}

			if (processVerts)
			{
				if (mesh->getMesh()->billboard)
				{
					const ATOM_Vector3f &n1 = keyframeMesh1.billboardNormal;
					const ATOM_Vector3f &n2 = keyframeMesh2.billboardNormal;
					ATOM_Vector3f n = n1 + (n2 - n1) * factor;
					n.normalize ();
					mesh->setBillboardNormal (n);
#if 1
					const ATOM_Vector3f &x1 = keyframeMesh1.billboardXAxis;
					const ATOM_Vector3f &x2 = keyframeMesh2.billboardXAxis;
					ATOM_Vector3f x = x1 + (x2-x1) * factor;
					x.normalize ();
					mesh->setBillboardXAxis (x);
#endif
					const ATOM_Vector3f &c1 = keyframeMesh1.billboardCenter;
					const ATOM_Vector3f &c2 = keyframeMesh2.billboardCenter;
					ATOM_Vector3f c = c1 + (c2-c1) * factor;
					mesh->setBillboardCenter (c);

					const ATOM_Matrix4x4f *m = _keyframeDatas->getTransforms ();
					const ATOM_Matrix4x4f &matrix1 = m[keyframeMesh1.transformIndex];
					const ATOM_Matrix4x4f &matrix2 = m[keyframeMesh2.transformIndex];
					mesh->setWorldMatrix (matrix1 + (matrix2 - matrix1) * factor);
				}
			}
			else
			{
				mesh->setBillboardNormal (mesh->getMesh()->billboardNormal);
				mesh->setBillboardXAxis (mesh->getMesh()->billboardXAxis);
			}
		}
	}

	// process skeletal animation if possible
	int jointIndex = -1;
	if (index < _keyframes[frame1].meshKeyFrames.size())
		jointIndex = _keyframes[frame1].meshKeyFrames[index].jointIndex;
	if (jointIndex < 0)
		jointIndex = _keyframes[frame1].jointIndex;

	// must update the skeleton
	if (jointIndex >= 0)
	{
		if (_jointMatricesIndex != jointIndex)
		{
			const ATOM_JointTransformInfo *jointTransforms = _keyframeDatas->getJoints(jointIndex);
			unsigned numJointTransformInfos = _keyframeDatas->getJointCount (jointIndex);
			_jointMatrices.resize (numJointTransformInfos);
			convertJointQuatToJointMat (&_jointMatrices[0], jointTransforms, _jointMatrices.size());
			if (skeleton && skeleton->getSkeleton()->getJointParents().size() > 0)
			{
				// transform joint to global space
				transformSkeleton (&_jointMatrices[0], &skeleton->getSkeleton()->getJointParents()[0], _jointMatrices.size());
			}
			_jointMatricesIndex = jointIndex;
		}

		if (hasVertexAnimation)
		{
			// no need to update the vertices if vertex animation was used.
			return true;
		}
	}

	bool processSkeletons = (jointIndex >= 0 && !mesh->getMesh()->weights.empty() && !mesh->getMesh()->vertices_sa.empty());

	if (processSkeletons)
	{
		if (!updateJoints && mesh->getVertexArray() && !mesh->getVertexArray()->isContentLost())
		{
			return true;
		}

		bool needRemapJoint = mesh->getJointMap().size() > 0 && _jointHashes.size() > 0;

		unsigned numVerts = mesh->getMesh()->vertices_sa.size();

		ATOM_AUTOREF(ATOM_VertexArray) va = mesh->getVertexArray ();
		if (!va)
		{
			// vertex deformation exists, create the intermediate vertex array if needed
			va = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_DYNAMIC, numVerts);
			mesh->setVertexArray (va.get());
			if (!va)
			{
				if (0 == reportError++)
				{
					ATOM_LOGGER::error ("Create vertex array failed\n");
				}
				return false;
			}
		}

		ATOM_AUTOREF(ATOM_VertexArray) vn = mesh->getNormalArray ();
		if (!vn)
		{
			// create the normal vertex array if needed
			vn = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_NORMAL, ATOM_USAGE_DYNAMIC, numVerts);
			mesh->setNormalArray (vn.get());
			if (!vn)
			{
				if (0 == reportError++)
				{
					ATOM_LOGGER::error ("Create normal vertex array failed\n");
				}
				return false;
			}
		}

		ATOM_Vector3f *verts = va ? (ATOM_Vector3f*)va->lock (ATOM_LOCK_WRITEONLY, 0, 0, true) : 0;
		if (va && !verts)
		{
			if (0 == reportError++)
			{
				ATOM_LOGGER::error ("lock vertex array failed\n");
			}
			return false;
		}

		ATOM_Vector3f *norms = vn ? (ATOM_Vector3f*)vn->lock (ATOM_LOCK_WRITEONLY, 0, 0, true) : 0;
		if (vn && !norms)
		{
			if (va)
			{
				va->unlock ();
			}
			if (0 == reportError++)
			{
				ATOM_LOGGER::error ("lock normal vertex array failed\n");
			}
			return false;
		}

#if 0
		for (unsigned v = 0; v < numVerts; ++v)
		{
			ATOM_Matrix4x4f matrixAccum(0.f);
			for (;;)
			{
				unsigned jointIndex = needRemapJoint ? mesh->getJointMap()[weightP->jointIndex] : weightP->jointIndex;
				matrixAccum += _jointMatrices[jointIndex] * weightP->weight;
				if (weightP->offsetToNextWeight == sizeof(ATOM_Mesh::VertexWeight))
				{
					++weightP;
					break;
				}
				++weightP;
			}
			accumMatrices[v] = matrixAccum;
		}

		const ATOM_Mesh::SkeletonVertex *srcVerts = &mesh->getMesh()->vertices_sa[0];

		for (unsigned v = 0; v < numVerts; ++v)
		{
			ATOM_Vector3f tmp(srcVerts->vertex.x, srcVerts->vertex.y, srcVerts->vertex.z);
			ATOM_Vector3f r = accumMatrices[v] >> tmp;
			verts->x = r.z;
			verts->y = r.y;
			verts->z = r.z;
			++srcVerts;
			++verts;
		}

		//if (vn)
		//{
		//	ATOM_Vector3f *norms = (ATOM_Vector3f*)vn->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		//	const ATOM_Vector3f *srcNorms = &mesh->getMesh()->normals_sa[0];
		//	for (unsigned v = 0; v < numVerts; ++v)
		//	{
		//		*norms++ = accumMatrices[v].transformVector(*srcNorms++);
		//	}
		//	vn->unlock ();
		//}

		//if (vt)
		//{
		//	ATOM_Vector3f *tangents = (ATOM_Vector3f*)vt->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		//	const ATOM_Vector3f *srcTangents = &mesh->getMesh()->tangents_sa[0];
		//	for (unsigned v = 0; v < numVerts; ++v)
		//	{
		//		*tangents++ = accumMatrices[v].transformVector(*srcTangents++);
		//	}
		//	vt->unlock ();
		//}
#else
		const ATOM_Mesh::SkeletonVertex *base = &mesh->getMesh()->vertices_sa[0];
		if (needRemapJoint)
		{
			if (!norms)
			{
				transformVerts (verts, numVerts, &_jointMatrices[0], base, &mesh->getMesh()->weights[0], &mesh->getJointMap()[0]);
			}
			else
			{
				transformVertsAndNormals (verts, norms, numVerts, &_jointMatrices[0], base, &mesh->getMesh()->weights[0], &mesh->getJointMap()[0]);
			}
		}
		else
		{
			if (!norms)
			{
				transformVerts (verts, numVerts, &_jointMatrices[0], base, &mesh->getMesh()->weights[0]);
			}
			else
			{
				transformVertsAndNormals (verts, norms, numVerts, &_jointMatrices[0], base, &mesh->getMesh()->weights[0]);
				//for (unsigned i = 0; i < numVerts; ++i)
				//{
				//	ATOM_LOGGER::Log ("--- %f, %f, %f\n", verts[i].x, verts[i].y, verts[i].z);
				//}
			}
		}
#endif

		if (verts) va->unlock ();
		if (norms) vn->unlock ();

		return true;
	}
	else if (!hasVertexAnimation)
	{
		mesh->setVertexArray (0);
	}

	return false;
}

void ATOM_ModelAnimationTrack::updateVertexAnimation (
	const ATOM_Vector3f *vertices1, 
	const ATOM_Vector2f *uvs1,
	const ATOM_Vector3f *vertices2,
	const ATOM_Vector2f *uvs2,
	float factor,
	ATOM_Vector3f *vertices, 
	ATOM_Vector2f *uvs, 
	unsigned numVerts, 
	bool uvAnimationNoIpol) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::updateVertexAnimation);

	if (vertices1 && vertices1 == vertices2)
	{
		ATOM_ASSERT(vertices);
		memcpy (vertices, vertices1, numVerts * sizeof(ATOM_Vector3f));
	}
	else if (vertices && vertices1 && vertices2)
	{
		const ATOM_Vector3f *src1 = vertices1;
		const ATOM_Vector3f *src2 = vertices2;
		unsigned count = numVerts;
		while (count--)
		{
			*vertices++ = *src1 + factor * (*src2 - *src1);
			++src1;
			++src2;
		}
	}

	if (uvs1 && (uvAnimationNoIpol || uvs1 == uvs2))
	{
		ATOM_ASSERT(uvs);
		memcpy (uvs, uvs1, numVerts * sizeof(ATOM_Vector2f));
	}
	else if (uvs && uvs1 && uvs2)
	{
		const ATOM_Vector2f *src1 = uvs1;
		const ATOM_Vector2f *src2 = uvs2;
		unsigned count = numVerts;
		while (count--)
		{
			*uvs++ = *src1 + factor * (*src2 - *src1);
			++src1;
			++src2;
		}
	}
}

void ATOM_ModelAnimationTrack::setName (const char *name)
{
	_actionName = name ? name : "";
}

const char *ATOM_ModelAnimationTrack::getName (void) const
{
	return _actionName.c_str();
}

void ATOM_ModelAnimationTrack::setNumFrames (unsigned num)
{
	_keyframes.resize (num);
}

unsigned ATOM_ModelAnimationTrack::getNumFrames (void) const
{
	return _keyframes.size();
}

ATOM_ModelAnimationTrack::KeyFrame &ATOM_ModelAnimationTrack::getFrame (unsigned frame)
{
	return _keyframes[frame];
}

const ATOM_ModelAnimationTrack::KeyFrame &ATOM_ModelAnimationTrack::getFrame (unsigned frame) const
{
	return _keyframes[frame];
}

void ATOM_ModelAnimationTrack::setAnimationDatas (ATOM_ModelAnimationDataCache *data)
{
	_keyframeDatas = data;
}

ATOM_ModelAnimationDataCache *ATOM_ModelAnimationTrack::getAnimationDatas (void) const
{
	return _keyframeDatas.get();
}

void ATOM_ModelAnimationTrack::setHashCode (unsigned code)
{
	_hashcode = code;
}

unsigned ATOM_ModelAnimationTrack::getHashCode (void) const
{
	return _hashcode;
}

static void writeContent (ATOM_File *f, const char *format, ...)
{
	char buffer[2048] = { '\0' };

	va_list args;
	va_start (args, format);
    vsnprintf(buffer, 2048, format, args);
	va_end (args);

	f->write (buffer, strlen (buffer));
}

void ATOM_ModelAnimationTrack::dumpToFile (const char *filename) const
{
	ATOM_AutoFile f(filename, ATOM_VFS::write|ATOM_VFS::text);

	if (f)
	{
		writeContent (f, "[action]\n");
		writeContent (f, "frameCount = %d\n", _keyframes.size());

		for (unsigned i = 0; i < _keyframes.size(); ++i)
		{
			writeContent (f, "[frame]\n");
			writeContent (f, "meshCount = %d\n", _keyframes[i].meshKeyFrames.size());
			writeContent (f, "frameTime = %d\n", _keyframes[i].frameTime);
			writeContent (f, "jointIndex = %d\n", _keyframes[i].jointIndex);

			if (_keyframes[i].jointIndex >= 0)
			{
				const ATOM_JointTransformInfo *transforms = _keyframeDatas->getJoints(_keyframes[i].jointIndex);
				unsigned numJoints = _keyframeDatas->getJointCount (_keyframes[i].jointIndex);
				float checksum = 0.f;
				for (unsigned j = 0; j < numJoints; ++j)
				{
					checksum += transforms[j].rotate.x;
					checksum += transforms[j].rotate.y;
					checksum += transforms[j].rotate.z;
					checksum += transforms[j].rotate.w;
					checksum += transforms[j].scale.x;
					checksum += transforms[j].scale.y;
					checksum += transforms[j].scale.z;
					checksum += transforms[j].translate.x;
					checksum += transforms[j].translate.y;
					checksum += transforms[j].translate.z;
				}
				writeContent (f, "jointCount = %d\n", numJoints);
				writeContent (f, "checksum = %f\n", checksum);
			}
		}
	}
}

#define BBOX_INTERP_INTERVAL 500

void ATOM_ModelAnimationTrack::setPrecomputedBoundingbox (const ATOM_BBox &bbox)
{
	_precomputedBBox = bbox;
	_bboxPrecomputed = true;
}

const ATOM_BBox &ATOM_ModelAnimationTrack::getPrecomputedBoundingbox (void)
{
	return _precomputedBBox;
}

bool ATOM_ModelAnimationTrack::hasPrecomputedBoundingbox (void) const
{
	return _bboxPrecomputed;
}

bool ATOM_ModelAnimationTrack::getBoundingbox (unsigned cycleTick, ATOM_BBox &bbox) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::getBoundingbox);

	if (_bboxPrecomputed)
	{
		bbox = _precomputedBBox;
		return true;
	}

	unsigned sz = _bboxInterpolateArray.size();
	if (sz == 0)
	{
		return false;
	}

	unsigned total = getTotalAnimationTime ();
	if (cycleTick > total)
	{
		cycleTick = total;
	}
	unsigned index = cycleTick / BBOX_INTERP_INTERVAL;
	unsigned k = cycleTick - index * BBOX_INTERP_INTERVAL;
	float factor = float(k) / float(BBOX_INTERP_INTERVAL);

	const ATOM_BBox &bbox1 = _bboxInterpolateArray[index % sz];
	const ATOM_BBox &bbox2 = _bboxInterpolateArray[(index+1) % sz];
	ATOM_Vector3f ptMin = bbox1.getMin() + (bbox2.getMin() - bbox1.getMin()) * factor;
	ATOM_Vector3f ptMax = bbox1.getMax() + (bbox2.getMax() - bbox1.getMax()) * factor;

	bbox.setMin (ptMin);
	bbox.setMax (ptMax);

	return true;
}

void ATOM_ModelAnimationTrack::computeBBoxInterpolateArray (ATOM_Skeleton *skeleton, const ATOM_BitArray *jointMask)
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::computeBBoxInterpolateArray);

	if (_bboxComputed)
	{
		return;
	}
	_bboxComputed = true;

	_bboxInterpolateArray.resize (0);

	if (skeleton)
	{
		unsigned numInterpolateNumber = (getTotalAnimationTime() + BBOX_INTERP_INTERVAL - 1) / BBOX_INTERP_INTERVAL + 1;

		for (unsigned i = 0; i < numInterpolateNumber; i++)
		{
			float frame = getAnimationFrame (i * BBOX_INTERP_INTERVAL);
			unsigned frame1 = ATOM_floor(frame);
			int jointIndex = _keyframes[frame1].jointIndex;
			if (jointIndex < 0)
			{
				const ATOM_VECTOR<MeshKeyFrame> &mkf = _keyframes[frame1].meshKeyFrames;
				for (unsigned i = 0; i < mkf.size(); ++i)
				{
					jointIndex = mkf[i].jointIndex;
					if (jointIndex >= 0)
					{
						break;
					}
				}
			}
			if (jointIndex < 0)
			{
				continue;
			}

			const ATOM_JointTransformInfo *jointTransforms = _keyframeDatas->getJoints(jointIndex);
			unsigned numJointTransforms = _keyframeDatas->getJointCount (jointIndex);

			ATOM_Matrix4x4f matrix;
			ATOM_Vector3f ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
			ATOM_Vector3f ptMax = -ptMin;

			for (unsigned n = 0; n < numJointTransforms; ++n)
			{
				if (n >= jointMask->size() || !jointMask->testBit (n))
				{
					continue;
				}

				getBoneMatrix (frame, n, skeleton, matrix);

				if (matrix.m30 > ptMax.x) ptMax.x = matrix.m30;
				if (matrix.m30 < ptMin.x) ptMin.x = matrix.m30;
				if (matrix.m31 > ptMax.y) ptMax.y = matrix.m31;
				if (matrix.m31 < ptMin.y) ptMin.y = matrix.m31;
				if (matrix.m32 > ptMax.z) ptMax.z = matrix.m32;
				if (matrix.m32 < ptMin.z) ptMin.z = matrix.m32;
			}

			_bboxInterpolateArray.push_back (ATOM_BBox (ptMin, ptMax));
		}
	}
}

bool ATOM_ModelAnimationTrack::getBoneMatrix (float frame, unsigned index, ATOM_Skeleton *skeleton, ATOM_Matrix4x4f &matrix) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::getBoneMatrix);

	ATOM_JointTransformInfo transform;
	if (getJointTransform(frame, index, &transform))
	{
		transform.rotate.toMatrix (matrix);
		matrix.m00 *= transform.scale.x;
		matrix.m01 *= transform.scale.x;
		matrix.m02 *= transform.scale.x;
		matrix.m10 *= transform.scale.y;
		matrix.m11 *= transform.scale.y;
		matrix.m12 *= transform.scale.y;
		matrix.m20 *= transform.scale.z;
		matrix.m21 *= transform.scale.z;
		matrix.m22 *= transform.scale.z;
		matrix.m30 = transform.translate.x;
		matrix.m31 = transform.translate.y;
		matrix.m32 = transform.translate.z;

#if 1
		ATOM_Matrix4x4f bindMatrix;
		skeleton->getJointBindMatrix (index).toMatrix44(bindMatrix);
#else
		ATOM_Matrix4x4f bindMatrix = skeleton->getJointBindMatrix (index);
#endif
		matrix >>= bindMatrix;

		std::swap (matrix.m00, matrix.m02);
		std::swap (matrix.m10, matrix.m12);
		std::swap (matrix.m20, matrix.m22);
		std::swap (matrix.m30, matrix.m32);

		return true;
	}

	return false;
}

void ATOM_ModelAnimationTrack::computeBillboardNormal (unsigned mesh, const unsigned short *indices, const ATOM_Matrix4x4f &worldMatrix)
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::computeBillboardNormal);

	ATOM_ASSERT(_keyframeDatas);

	for (unsigned i = 0; i < _keyframes.size(); ++i)
	{
		MeshKeyFrame &mkf = _keyframes[i].meshKeyFrames[mesh];

		if (mkf.vertexIndex >= 0 && _keyframeDatas)
		{
#if 0
			ATOM_Matrix4x4f invWorldMatrix = (mkf.transformIndex >= 0) ? _keyframeDatas->transformCache[mkf.transformIndex] : worldMatrix;
			invWorldMatrix.invertAffine ();

			const ATOM_VECTOR<ATOM_Vector3f> &verts = _keyframeDatas->vertexCache[mkf.vertexIndex];
			ATOM_Vector3f v0 = invWorldMatrix >> verts[indices[0]];
			ATOM_Vector3f v1 = invWorldMatrix >> verts[indices[1]];
			ATOM_Vector3f v2 = invWorldMatrix >> verts[indices[2]];

			mkf.billboardNormal = crossProduct(v1-v0, v2-v0);
			mkf.billboardNormal.normalize ();
			mkf.billboardXAxis = v1-v0;
			mkf.billboardXAxis.normalize ();
#else
			const ATOM_Vector3f *verts = _keyframeDatas->getVertices(mkf.vertexIndex);
			unsigned num = _keyframeDatas->getVertexCount (mkf.vertexIndex);
			const ATOM_Vector3f &v0 = verts[indices[0]];
			const ATOM_Vector3f &v1 = verts[indices[1]];
			const ATOM_Vector3f &v2 = verts[indices[2]];

			mkf.billboardNormal = crossProduct(v1-v0, v2-v0);
			mkf.billboardNormal.normalize ();
			mkf.billboardXAxis = v1-v0;
			mkf.billboardXAxis.normalize ();

			mkf.billboardCenter.set (0.f, 0.f, 0.f);
			for (unsigned i = 0; i < num; ++i)
			{
				mkf.billboardCenter += verts[i];
			}
			mkf.billboardCenter /= float(num);
#endif
		}
	}
}

const char *ATOM_ModelAnimationTrack::getProperty (const char *key) const
{
	if (_propIndex == -1 || !_propIndex)
	{
		return 0;
	}

	const ATOM_PropertySet &propset = _propertySetCache->getPropertySet (_propIndex);

	return propset.getValue (key);	
}

void ATOM_ModelAnimationTrack::setPropertyCache (ATOM_PropertySetCache *propCache)
{
	_propertySetCache = propCache;
}

void ATOM_ModelAnimationTrack::setPropertyIndex (int index)
{
	_propIndex = index;
}

float ATOM_ModelAnimationTrack::getAnimationFrame (unsigned cycleTick) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::getAnimationFrame);

	int frame1 = -1;

	if (_keyframes.size () > 0)
	{
		for (int i = 0; i < _keyframes.size(); ++i)
		{
			if (_keyframes[i].frameTime <= cycleTick)
			{
				frame1 = i;
			}
			else
			{
				break;
			}
		}
		if (frame1 < 0)
		{
			return 0.f;
		}
		else if (frame1 >= _keyframes.size() - 1)
		{
			return _keyframes.size() - 1;
		}

		int frame2 = frame1 + 1;
		float factor = float(cycleTick - _keyframes[frame1].frameTime)/float(_keyframes[frame2].frameTime - _keyframes[frame1].frameTime);
		return frame1 + factor;
	}

	return frame1;
}

int ATOM_ModelAnimationTrack::getJointIndex (unsigned frame) const
{
	int jointIndex = -1;
	if (_keyframes.size() > frame)
	{
		if (_keyframes[frame].meshKeyFrames.size() > 0)
		{
			jointIndex = _keyframes[frame].meshKeyFrames[0].jointIndex;
		}
		if (jointIndex < 0)
		{
			jointIndex = _keyframes[frame].jointIndex;
		}
	}
	return jointIndex;
}

bool ATOM_ModelAnimationTrack::getJointTransform (float frame, int joint, ATOM_JointTransformInfo *p) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::getJointTransform);

	int frame1 = ATOM_ftol(ATOM_floor(frame));
	int jointIndex = _keyframes[frame1].jointIndex;
	if (jointIndex < 0)
	{
		const ATOM_VECTOR<MeshKeyFrame> &mkf = _keyframes[frame1].meshKeyFrames;
		for (unsigned i = 0; i < mkf.size(); ++i)
		{
			jointIndex = mkf[i].jointIndex;
			if (jointIndex >= 0)
			{
				break;
			}
		}
	}
	if (jointIndex >= 0)
	{
		const ATOM_JointTransformInfo *jointTransforms = _keyframeDatas->getJoints(jointIndex);
		*p = jointTransforms[joint];
		return true;
	}
	return false;
}

void ATOM_ModelAnimationTrack::invalidBoundingbox (void)
{
	_bboxComputed = false;
}

const ATOM_JointTransformInfo *ATOM_ModelAnimationTrack::getFrameJoints (unsigned frame, unsigned *numJoints) const
{
	ATOM_STACK_TRACE(ATOM_ModelAnimationTrack::getFrameJoints);

	if (_keyframes.size() <= frame)
	{
		return 0;
	}

	int jointIndex = getJointIndex (frame);

	if (jointIndex < 0)
	{
		return 0;
	}

	if (numJoints)
	{
		*numJoints = _keyframeDatas->getJointCount(jointIndex);
	}

	return  _keyframeDatas->getJoints(jointIndex);
}

