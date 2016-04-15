#include "StdAfx.h"
#include <float.h>
#include <iostream>
#include "model.h"
#include "cache.h"
#include "skeletonviewer.h"
#include "propedit.h"
#include "skinanim_sse.h"
#include "exporter.h"

MyModel::MyModel (Logger *logger)
{
	_logger = logger;
}

void MyModel::addMesh (const MyMesh &mesh)
{
	_meshes.push_back (mesh);
}

void MyModel::computeBoundingBox (float minPoint[3], float maxPoint[3])
{
	minPoint[0] = minPoint[1] = minPoint[2] = FLT_MAX;
	maxPoint[0] = maxPoint[1] = maxPoint[2] = -FLT_MAX;

	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		const MyMesh &mesh = _meshes[i];
		unsigned nVerts = mesh.getNumVertices ();
		for (unsigned n = 0; n < nVerts; ++n)
		{
			const ATOM_Vector3f &v = mesh.getVertex (n);
			if (v.x < minPoint[0]) minPoint[0] = v.x;
			if (v.x > maxPoint[0]) maxPoint[0] = v.x;
			if (v.y < minPoint[1]) minPoint[1] = v.y;
			if (v.y > maxPoint[1]) maxPoint[1] = v.y;
			if (v.z < minPoint[2]) minPoint[2] = v.z;
			if (v.z > maxPoint[2]) maxPoint[2] = v.z;
		}
	}
}

struct PropertyData
{
	unsigned writeSize;
	std::vector<std::string> names;
	std::vector<std::string> values;

	PropertyData (void): writeSize (0)
	{
	}

	friend bool operator == (const PropertyData &pd1, const PropertyData &pd2)
	{
		if (pd1.names.size() != pd2.names.size())
		{
			return false;
		}

		for (unsigned n = 0; n < pd1.names.size(); ++n)
		{
			if (pd1.names[n] != pd2.names[n])
			{
				return false;
			}
			if (pd1.values [n] != pd2.values[n])
			{
				return false;
			}
		}

		return true;
	}

	void addProperty (const char *name, const char *value)
	{
		if (writeSize == 0)
		{
			writeSize = sizeof(unsigned);
		}

		names.push_back (name);
		values.push_back (value);

		writeSize += 2 * sizeof(unsigned);
		writeSize += names.back().length() + 1;
		writeSize += values.back().length() + 1;
	}

	bool write (FILE *fp)
	{
		unsigned actualWritten = 0;
		assert (names.size() > 0);
		unsigned numProps = names.size();
		actualWritten += fwrite (&numProps, 1, sizeof(unsigned), fp);

		for (unsigned prop = 0; prop < numProps; ++prop)
		{
			unsigned len = names[prop].length() + 1;
			actualWritten += fwrite (&len, 1, sizeof(unsigned), fp);
			actualWritten += fwrite (names[prop].c_str(), 1, len, fp);
			len = values[prop].length() + 1;
			actualWritten += fwrite (&len, 1, sizeof(unsigned), fp);
			actualWritten += fwrite (values[prop].c_str(), 1, len, fp);
		}

		return actualWritten == writeSize;
	}
};

struct PropertyDataCache
{
	unsigned writeSize;
	std::vector<PropertyData> propDatas;

	PropertyDataCache (void): writeSize (0)
	{
	}

	unsigned addPropertyData (const PropertyData &pd)
	{
		for (unsigned i = 0; i < propDatas.size(); ++i)
		{
			if (propDatas[i] == pd)
			{
				return i;
			}
		}

		if (writeSize == 0)
		{
			writeSize += sizeof(unsigned);
		}

		propDatas.push_back (pd);
		writeSize += sizeof(unsigned);
		writeSize += pd.writeSize;

		return propDatas.size() - 1;
	}

	bool write (FILE *fp)
	{
		assert (propDatas.size() > 0);
		unsigned actualWritten = 0;

		unsigned numPropDatas = propDatas.size();
		actualWritten += fwrite (&numPropDatas, 1, sizeof(unsigned), fp);

		for (unsigned i = 0; i < numPropDatas; ++i)
		{
			unsigned size = propDatas[i].writeSize;
			actualWritten += fwrite (&size, 1, sizeof(unsigned), fp);
			if (!propDatas[i].write (fp))
			{
				return false;
			}
			actualWritten += propDatas[i].writeSize;
		}

		return actualWritten == writeSize;
	}
};

struct SkeletonData
{
	unsigned writeSize;
	ATOM_Skeleton *sklt;

	SkeletonData (ATOM_Skeleton *skeleton): sklt(skeleton), writeSize(0)
	{
		if (sklt)
		{
			writeSize += sizeof(unsigned); // chunk version
			writeSize += sizeof(unsigned); // number of skeletons

			for (unsigned i = 0; i < 1; ++i)
			{
				writeSize += sizeof(unsigned); // skeleton hash
				writeSize += sizeof(unsigned); // count of joints
				writeSize += sklt->getAttachPoints().size() * sizeof(float) * 16;
				writeSize += sklt->getAttachPoints().size() * sizeof(int);
				writeSize += sklt->getAttachPoints().size() * 64;
			}
		}
	}

	bool write (FILE *fp)
	{
		if (sklt)
		{
			unsigned actualWritten = 0;

			unsigned chunkversion = 1;
			actualWritten += fwrite (&chunkversion, 1, sizeof(unsigned), fp);

			unsigned numSkeletons = 1;
			actualWritten += fwrite (&numSkeletons, 1, sizeof(unsigned), fp);

			for (unsigned s = 0; s < numSkeletons; ++s)
			{
				unsigned hash = sklt->getHash ();
				actualWritten += fwrite (&hash, 1, sizeof(unsigned), fp);

				unsigned numJoints = sklt->getAttachPoints().size();
				actualWritten += fwrite (&numJoints, 1, sizeof(unsigned), fp);
				const std::vector<int> &newToOld = sklt->getRemapNewToOld ();
				for (unsigned m = 0; m < numJoints; ++m)
				{
					const ATOM_Matrix4x4f &matrix = sklt->getJoints()[newToOld[m]].localMatrix;
					actualWritten += fwrite (matrix.m, 1, sizeof(float)*16, fp);
				}

				for (unsigned m = 0; m < numJoints; ++m)
				{
					int parent = sklt->getJoints()[newToOld[m]].parentIndex;
					if (parent >= 0)
					{
						parent = sklt->getRemapOldToNew ()[parent];
					}
					actualWritten += fwrite (&parent, 1, sizeof(int), fp);
				}

				for (unsigned m = 0; m < numJoints; ++m)
				{
					char buffer[64];
					strcpy (buffer, sklt->getJoints()[newToOld[m]].name.c_str());
					actualWritten += fwrite (buffer, 1, 64, fp);
				}
			}
			return actualWritten == writeSize;
		}
		return false;
	}
};

struct AnimationData
{
	unsigned writeSize;
	bool saveMesh;
	std::vector<int> propIndices;

	AnimationData (const MyModel &model, PropertyDataCache *propCache, bool bSaveMesh, bool saveAnimation)
	{
		writeSize = 0;
		saveMesh = bSaveMesh;

		if (!saveAnimation)
		{
			return;
		}

		if (model.getNumActions ())
		{
			writeSize += sizeof(unsigned); // num joints
			if (model.getSkeleton() && model.getSkeleton()->getAttachPoints().size())
			{
				writeSize += sizeof(unsigned) * model.getSkeleton()->getAttachPoints().size();
			}

			writeSize += sizeof(unsigned); // num actions
			writeSize += sizeof(unsigned); // num meshes

			for (unsigned a = 0; a < model.getNumActions(); ++a)
			{
				writeSize += 64; // action name
				writeSize += sizeof(unsigned); // num frames of this action
				writeSize += sizeof(unsigned); // action hash
				writeSize += sizeof(int); // action property index

				int index = -1;
				PropertyEditor *props = model.getActionProps (a);
				if (props->getNumProperties() > 0)
				{
					PropertyData pd;
					for (unsigned n = 0; n < props->getNumProperties(); ++n)
					{
						pd.addProperty (props->getPropertyName(n), props->getPropertyValue(n));
					}
					index = propCache->addPropertyData (pd);
				}
				propIndices.push_back (index);

				// 是否有包围盒
				int hasBoundingbox = model.getActionBBox(a).isValid () ? 1 : 0;
				writeSize += sizeof(int);
				if (hasBoundingbox)
				{
					writeSize += sizeof(ATOM_BBox);
				}

				// 预留action版本/类型鉴定域，目前未实现
				writeSize += sizeof(unsigned);

				for (unsigned i = 0; i < model.getNumActionFrames(a); ++i)
				{
					writeSize += sizeof(unsigned); // frameTime of the action
					writeSize += sizeof(int); // action joint cache index

					if (saveMesh)
					{
						for (unsigned m = 0; m < model.meshes().size(); ++m)
						{
							writeSize += sizeof(int); // vertex cache index
							writeSize += sizeof(int); // uv cache index
							writeSize += sizeof(int); // joint cache index
							writeSize += sizeof(int); // bbox cache index
							writeSize += sizeof(int); // transform cache index
							writeSize += sizeof(float); // mesh transparency
						}
					}
				}
			}
		}
	}

	bool write (const MyModel &model, PropertyDataCache *propCache, FILE *fp)
	{
		if (writeSize == 0)
		{
			return true;
		}

		unsigned actualWriteSize = 0;

		unsigned numJoints = model.getSkeleton() ? model.getSkeleton()->getAttachPoints().size() : 0;
		actualWriteSize += fwrite (&numJoints, 1, sizeof(unsigned), fp);
		if (numJoints)
		{
			const std::vector<int> &remapNewToOld = model.getSkeleton()->getRemapNewToOld();
			for (unsigned joint = 0; joint < numJoints; ++joint)
			{
				actualWriteSize += fwrite (&model.getSkeleton()->getJoints()[remapNewToOld[joint]].hashCode, 1, sizeof(unsigned), fp);
			}
		}

		unsigned numActions = model.getNumActions ();
		actualWriteSize += fwrite (&numActions, 1, sizeof(unsigned), fp);

		unsigned numMeshes = saveMesh ? model.meshes().size() : 0;
		actualWriteSize += fwrite (&numMeshes, 1, sizeof(unsigned), fp);

		for (unsigned a = 0; a < numActions; ++a)
		{
			char name[64];
			strcpy (name, model.getActionName(a));
			actualWriteSize += fwrite (name, 1, 64, fp);

			unsigned numFrames = model.getNumActionFrames (a);
			actualWriteSize += fwrite (&numFrames, 1, sizeof(unsigned), fp);

			unsigned hash = model.getActionHash(a);
			actualWriteSize += fwrite (&hash, 1, sizeof(unsigned), fp);

			actualWriteSize += fwrite (&propIndices[a], 1, sizeof(int), fp);

			// 是否有包围盒
			int hasBoundingbox = model.getActionBBox(a).isValid () ? 1 : 0;
			actualWriteSize += fwrite (&hasBoundingbox, 1, sizeof(int), fp);
			if (hasBoundingbox)
			{
				actualWriteSize += fwrite (&model.getActionBBox(a), 1, sizeof(ATOM_BBox), fp);
			}

			// 预留action版本/类型鉴定域，目前未实现
			unsigned reserved = 0;
			actualWriteSize += fwrite (&reserved, 1, sizeof(unsigned), fp);

			// 保存每一帧数据
			for (unsigned f = 0; f < numFrames; ++f)
			{
				unsigned frameTime = model.getFrameTime (a, f);
				actualWriteSize += fwrite (&frameTime, 1, sizeof(unsigned), fp);

				assert(model.meshes().size() > 0);
				int jointCacheIndex = saveMesh ? -1 : model.getFrameJointIndex(a, f);
				actualWriteSize += fwrite (&jointCacheIndex, 1, sizeof(int), fp);

				for (unsigned m = 0; m < numMeshes; ++m)
				{
					unsigned meshFrame = model.meshes()[m].getAction(a).frames[f];

					const MyMesh::VertexAnimationFrame &vaf = model.meshes()[m].getVertexAnimationFrame(meshFrame);
					int invalid = -1;
					actualWriteSize += fwrite (&invalid, 1, sizeof(int), fp);
					actualWriteSize += fwrite (&invalid, 1, sizeof(int), fp);

					if (model.meshes()[m].getAction(a).jointAnimation)
					{
						const MyMesh::SkeletonAnimationFrame &saf = model.meshes()[m].getSkeletonAnimationFrame(meshFrame);
						actualWriteSize += fwrite (&saf.jointCacheIndex, 1, sizeof(int), fp);
					}
					else
					{
						int n = -1;
						actualWriteSize += fwrite (&n, 1, sizeof(int), fp);
					}

					if (model.meshes()[m].getAction(a).jointAnimation && model.meshes()[m].usingSkeleton ())
					{
						const MyMesh::SkeletonAnimationFrame &saf = model.meshes()[m].getSkeletonAnimationFrame(meshFrame);
						actualWriteSize += fwrite (&saf.bboxCacheIndex, 1, sizeof(int), fp);
					}
					else
					{
						actualWriteSize += fwrite (&vaf.bboxCacheIndex, 1, sizeof(int), fp);
					}

					actualWriteSize += fwrite (&vaf.transformCacheIndex, 1, sizeof(int), fp);

					float transparency = model.meshes()[m].getMaterial().transparency;
					if (1 || model.meshes()[m].getAction(a).materialAnimation)
					{
						transparency = model.meshes()[m].getMaterialFrame (meshFrame).transparency;
					}
					else if (transparency == 0.f)
					{
						transparency = 1.f;
					}

					actualWriteSize += fwrite (&transparency, 1, sizeof(float), fp);
				}
			}
		}

		return actualWriteSize == writeSize;
	}
};

static void getBoneMatrix (int joint, ATOM_Skeleton *skeleton, ATOM_Matrix3x4f *mats, ATOM_Matrix4x4f &matrix)
{
	ATOM_Matrix4x4f bindMatrix = skeleton->getJoints()[joint].bindMatrix;
	const ATOM_Matrix3x4f &jointMat = mats[joint];
	jointMat.toMatrix44 (matrix);
	matrix >>= bindMatrix;
}

static void computeActionFrameBoundingbox (ATOM_Skeleton *skeleton, ATOM_Matrix3x4f *mats, unsigned numJoints, ATOM_BBox &bbox)
{
	ATOM_Matrix4x4f boneMatrix;

	for (unsigned i = 0; i < numJoints; ++i)
	{
		getBoneMatrix (i, skeleton, mats, boneMatrix);
		bbox.extend (ATOM_Vector3f(boneMatrix.m30, boneMatrix.m31, boneMatrix.m32));
	}
}

static void convertJointTransformsToLocalSpace (std::vector<JointTransform> &transforms, ATOM_Skeleton *skeleton, ATOM_BBox &bbox)
{
	unsigned numTransforms = transforms.size();
	JointTransformInfo *info = (JointTransformInfo*)_aligned_malloc (sizeof(JointTransformInfo) * numTransforms, 16);
	ATOM_Matrix3x4f *mat = (ATOM_Matrix3x4f*)_aligned_malloc (sizeof(ATOM_Matrix3x4f) * numTransforms, 16);
	int *parents = (int*)_aligned_malloc (sizeof(int) * numTransforms, 16);

	for (unsigned i = 0; i < numTransforms; ++i)
	{
		info[i].rotate = transforms[i].rotate;
		info[i].scale.set (transforms[i].scale.x, transforms[i].scale.y, transforms[i].scale.z, 1.f);
		info[i].translate.set (transforms[i].translation.x, transforms[i].translation.y, transforms[i].translation.z, 1.f);
		parents[i] = skeleton->getJoints()[i].parentIndex;
	}

	convertJointQuatToJointMat (mat, info, numTransforms);
	computeActionFrameBoundingbox (skeleton, mat, numTransforms, bbox);

	int parent = 0;
	while (parent < numTransforms)
	{
		assert(parents[parent] < 0);
		int first = ++parent;
		for (; parent < numTransforms && parents[parent] >= 0 ; parent++)
			;
		int last = parent - 1;
		if (first <= last)
		{
			untransformSkeleton (mat, parents, first, last);
		}
	}

	for (unsigned joint = 0; joint < numTransforms; ++joint)
	{
		ATOM_Matrix4x4f m;
		mat[joint].toMatrix44 (m);

		JointTransform &transform = transforms[joint];
		ATOM_Matrix4x4f r;
		ATOM_Vector3f t, s;
		m.decompose (t, r, s);

		transform.rotate.fromMatrix (r);
		transform.scale = s;
		transform.translation = t;
	}	

	_aligned_free (info);
	_aligned_free (mat);
	_aligned_free (parents);
}

static void convertJointTransformsToLocalSpace (std::vector<ATOM_Matrix3x4f> &transforms, ATOM_Skeleton *skeleton, ATOM_BBox &bbox)
{
	unsigned numTransforms = transforms.size();
	int *parents = (int*)_aligned_malloc (sizeof(int) * numTransforms, 16);
	for (unsigned i = 0; i < numTransforms; ++i)
	{
		parents[i] = skeleton->getJoints()[i].parentIndex;
	}

	computeActionFrameBoundingbox (skeleton, &transforms[0], numTransforms, bbox);

	int parent = 0;
	while (parent < numTransforms)
	{
		assert(parents[parent] < 0);
		int first = ++parent;
		for (; parent < numTransforms && parents[parent] >= 0 ; parent++)
			;
		int last = parent - 1;
		if (first <= last)
		{
			untransformSkeleton (&transforms[0], parents, first, last);
		}
	}
}


struct AnimationCache
{
	Vector3Cache vertexCache;
	Vector2Cache uvCache;
	JointTransformCache jointCache;
	ATOM_BBoxCache bboxCache;
	Matrix44Cache transformCache;
	unsigned writeSize;
	bool saveMesh;

	AnimationCache (MyModel &model, bool bSaveMesh, bool saveAnimation)
	{
		writeSize = 0;
		saveMesh = bSaveMesh;

		if (!saveAnimation)
		{
			return;
		}


		if (model.getNumActions ())
		{
			for (unsigned i = 0; i < model.getNumActions(); ++i)
			{
				for (unsigned j = 0; j < model.getNumActionFrames(i); ++j)
				{
					if (model.getFrameJoints(i, j).size() > 0)
					{
#if 1
						std::vector<JointTransform> transforms(model.getFrameJoints(i, j).size());
						for (unsigned jt = 0; jt < transforms.size(); ++jt)
						{
							ATOM_Matrix4x4f mat = model.getFrameJoints(i, j)[jt];
							transforms[jt].translation.set(mat.m30, mat.m31, mat.m32);
							float sx = sqrtf(mat.m00 * mat.m00 + mat.m01 * mat.m01 + mat.m02 * mat.m02);
							float sy = sqrtf(mat.m10 * mat.m10 + mat.m11 * mat.m11 + mat.m12 * mat.m12);
							float sz = sqrtf(mat.m20 * mat.m20 + mat.m21 * mat.m21 + mat.m22 * mat.m22);
							transforms[jt].scale.set (sx, sy, sz);
							ATOM_Matrix4x4f matRotate(
								mat.m00/sx, mat.m01/sx, mat.m02/sx, 0.f,
								mat.m10/sy, mat.m11/sy, mat.m12/sy, 0.f,
								mat.m20/sz, mat.m21/sz, mat.m22/sz, 0.f,
								0.f,        0.f,        0.f,        1.f
								);
							transforms[jt].rotate.fromMatrix (matRotate);
						}
#else
						std::vector<JointTransform> transforms(model.getFrameJoints(i, j).size());
						for (unsigned jt = 0; jt < transforms.size(); ++jt)
						{
							const ATOM_Matrix4x4f &mat = model.getFrameJoints(i, j)[jt];
							transforms[jt].translation.set(mat.m30, mat.m31, mat.m32);
							float sx = sqrtf(mat.m00 * mat.m00 + mat.m01 * mat.m01 + mat.m02 * mat.m02);
							float sy = sqrtf(mat.m10 * mat.m10 + mat.m11 * mat.m11 + mat.m12 * mat.m12);
							float sz = sqrtf(mat.m20 * mat.m20 + mat.m21 * mat.m21 + mat.m22 * mat.m22);
							transforms[jt].scale.set (sx, sy, sz);
							ATOM_Matrix4x4f matRotate(
								mat.m00/sx, mat.m01/sx, mat.m02/sx, 0.f,
								mat.m10/sy, mat.m11/sy, mat.m12/sy, 0.f,
								mat.m20/sz, mat.m21/sz, mat.m22/sz, 0.f,
								0.f,        0.f,        0.f,        1.f
								);
							transforms[jt].rotate.fromMatrix (matRotate);
						}

						convertJointTransformsToLocalSpace (transforms, model.getSkeleton (), actionBBox);
#endif
						const std::vector<int> newToOld = model.getSkeleton()->getRemapNewToOld ();
						std::vector<JointTransform> transformsRemap(newToOld.size());
						for (unsigned n = 0; n < transformsRemap.size(); ++n)
						{
							transformsRemap[n] = transforms[newToOld[n]];
						}
						model.setFrameJointIndex(i, j, jointCache.add (transformsRemap));
					}
					else
					{
						model.setFrameJointIndex(i, j, -1);
					}
				}
			}

			for (unsigned i = 0; i < model.meshes().size(); ++i)
			{
				const MyMesh &mesh = model.meshes()[i];
				bool isBillboard = (mesh.getVertexFlags() & MyMesh::ATOM_Billboard) != 0;

				for (unsigned a = 0; a < mesh.getNumActions(); ++a)
				{
					const MyMesh::Action &action = mesh.getAction(a);
					for (unsigned frame = 0; frame < action.frames.size(); ++frame)
					{
						unsigned theFrame = action.frames[frame];
						assert(theFrame < mesh.getNumVertexAnimationFrames());
						const MyMesh::VertexAnimationFrame &vaf = mesh.getVertexAnimationFrame (theFrame);

						if (action.vertexAnimation || action.uvAnimation || frame == 0)
						{
							vaf.vertexCacheIndex = vaf.vertices.size() > 0 ? vertexCache.add (vaf.vertices) : -1;
							vaf.bboxCacheIndex = bboxCache.add (vaf.boundingbox);
							vaf.transformCacheIndex = isBillboard ? transformCache.add (vaf.transform) : -1;
							vaf.uvCacheIndex = vaf.uvs.size() > 0 ? uvCache.add (vaf.uvs) : -1;
						}
						else
						{
							const MyMesh::VertexAnimationFrame &vaf0 = mesh.getVertexAnimationFrame (action.frames[0]);
							vaf.vertexCacheIndex = vaf0.vertexCacheIndex;
							vaf.bboxCacheIndex = vaf0.bboxCacheIndex;
							vaf.transformCacheIndex = vaf0.transformCacheIndex;
							vaf.uvCacheIndex = vaf0.uvCacheIndex;
						}

						if (action.jointAnimation)
						{
							assert(theFrame < mesh.getNumSkeletonAnimationFrames());
							const MyMesh::SkeletonAnimationFrame &saf = mesh.getSkeletonAnimationFrame (theFrame);
							if (action.jointAnimation)
							{
								assert(saf.jointMatrices.size() > 0);
#if 1
								std::vector<JointTransform> transforms(saf.jointMatrices.size());
								for (unsigned jt = 0; jt < transforms.size(); ++jt)
								{
									ATOM_Matrix4x4f mat = saf.jointMatrices[jt];
									transforms[jt].translation.set(mat.m30, mat.m31, mat.m32);
									float sx = sqrtf(mat.m00 * mat.m00 + mat.m01 * mat.m01 + mat.m02 * mat.m02);
									float sy = sqrtf(mat.m10 * mat.m10 + mat.m11 * mat.m11 + mat.m12 * mat.m12);
									float sz = sqrtf(mat.m20 * mat.m20 + mat.m21 * mat.m21 + mat.m22 * mat.m22);
									transforms[jt].scale.set (sx, sy, sz);
									ATOM_Matrix4x4f matRotate(
										mat.m00/sx, mat.m01/sx, mat.m02/sx, 0.f,
										mat.m10/sy, mat.m11/sy, mat.m12/sy, 0.f,
										mat.m20/sz, mat.m21/sz, mat.m22/sz, 0.f,
										0.f,        0.f,        0.f,        1.f
										);
									transforms[jt].rotate.fromMatrix (matRotate);
								}
#else
								for (unsigned jt = 0; jt < transforms.size(); ++jt)
								{
									const ATOM_Matrix4x4f &mat = saf.jointMatrices[jt];
									transforms[jt].translation.set(mat.m30, mat.m31, mat.m32);
									float sx = sqrtf(mat.m00 * mat.m00 + mat.m01 * mat.m01 + mat.m02 * mat.m02);
									float sy = sqrtf(mat.m10 * mat.m10 + mat.m11 * mat.m11 + mat.m12 * mat.m12);
									float sz = sqrtf(mat.m20 * mat.m20 + mat.m21 * mat.m21 + mat.m22 * mat.m22);
									transforms[jt].scale.set (sx, sy, sz);
									ATOM_Matrix4x4f matRotate(
										mat.m00/sx, mat.m01/sx, mat.m02/sx, 0.f,
										mat.m10/sy, mat.m11/sy, mat.m12/sy, 0.f,
										mat.m20/sz, mat.m21/sz, mat.m22/sz, 0.f,
										0.f,        0.f,        0.f,        1.f
										);
									transforms[jt].rotate.fromMatrix (matRotate);
									//ATOM_Vector3f axis;
									//float angle;
									//transforms[jt].rotate.toAxisAngle (angle, axis);
									//transforms[jt].rotate.fromAxisAngle (angle, ATOM_Vector3f(axis.z, axis.y, axis.x));
								}
								convertJointTransformsToLocalSpace (transforms, model.getSkeleton (), bbox);
#endif
								const std::vector<int> newToOld = model.getSkeleton()->getRemapNewToOld ();
								std::vector<JointTransform> transformsRemap(newToOld.size());
								for (unsigned n = 0; n < transformsRemap.size(); ++n)
								{
									transformsRemap[n] = transforms[newToOld[n]];
								}
								saf.jointCacheIndex = jointCache.add (transformsRemap);
								saf.bboxCacheIndex = bboxCache.add (model.getActionBBox (a));
							}
							else
							{
								saf.jointCacheIndex = -1;
								saf.bboxCacheIndex = -1;
							}
						}
					}
				}
			}

			writeSize += sizeof(unsigned); // count
			for (unsigned i = 0; i < vertexCache.getNumElements (); ++i)
			{
				writeSize += sizeof(unsigned); // count
				writeSize += vertexCache.getElement(i).size() * sizeof(float) * 3;
			}
			writeSize += sizeof(unsigned); // count
			for (unsigned i = 0; i < uvCache.getNumElements (); ++i)
			{
				writeSize += sizeof(unsigned); // count
				writeSize += uvCache.getElement(i).size() * sizeof(float) * 2;
			}
			writeSize += sizeof(unsigned); // count
			for (unsigned i = 0; i < jointCache.getNumElements(); ++i)
			{
				writeSize += sizeof(unsigned); // count
				writeSize += jointCache.getElement(i).size() * sizeof(JointTransform);
			}
			writeSize += sizeof(unsigned); // count
			for (unsigned i = 0; i < bboxCache.getNumElements(); ++i)
			{
				writeSize += sizeof(ATOM_BBox);
			}
			writeSize += sizeof(unsigned); // count
			for (unsigned i = 0; i < transformCache.getNumElements(); ++i)
			{
				writeSize += sizeof(ATOM_Matrix4x4f);
			}
		}
	}

	bool write (FILE *fp)
	{
		if (writeSize == 0)
		{
			return true;
		}

		unsigned actualWriteSize = 0;

		unsigned vertexElementNum = vertexCache.getNumElements ();
		actualWriteSize += fwrite (&vertexElementNum, 1, sizeof(unsigned), fp);
		for (unsigned i = 0; i < vertexElementNum; ++i)
		{
			unsigned count = vertexCache.getElement(i).size();
			actualWriteSize += fwrite (&count, 1, sizeof(unsigned), fp);
			if (count)
			{
				actualWriteSize += fwrite(&vertexCache.getElement(i)[0], 1, count * sizeof(float) * 3, fp);
			}
		}
		unsigned uvElementNum = uvCache.getNumElements ();
		actualWriteSize += fwrite (&uvElementNum, 1, sizeof(unsigned), fp);
		for (unsigned i = 0; i < uvElementNum; ++i)
		{
			unsigned count = uvCache.getElement(i).size();
			actualWriteSize += fwrite (&count, 1, sizeof(unsigned), fp);
			if (count)
			{
				actualWriteSize += fwrite(&uvCache.getElement(i)[0], 1, count * sizeof(float) * 2, fp);
			}
		}
		unsigned jointElementNum = jointCache.getNumElements ();
		actualWriteSize += fwrite (&jointElementNum, 1, sizeof(unsigned), fp);
		for (unsigned i = 0; i < jointElementNum; ++i)
		{
			unsigned count = jointCache.getElement(i).size();
			actualWriteSize += fwrite (&count, 1, sizeof(unsigned), fp);
			if (count)
			{
				actualWriteSize += fwrite(&jointCache.getElement(i)[0], 1, count * sizeof(JointTransform), fp);
			}
		}
		unsigned bboxElementNum = bboxCache.getNumElements ();
		actualWriteSize += fwrite (&bboxElementNum, 1, sizeof(unsigned), fp);
		for (unsigned i = 0; i < bboxElementNum; ++i)
		{
			actualWriteSize += fwrite (&bboxCache.getElement(i), 1, sizeof(ATOM_BBox), fp);
		}
		unsigned transformElementNum = transformCache.getNumElements ();
		actualWriteSize += fwrite (&transformElementNum, 1, sizeof(unsigned), fp);
		for (unsigned i = 0; i < transformElementNum; ++i)
		{
			actualWriteSize += fwrite (&transformCache.getElement(i), 1, sizeof(ATOM_Matrix4x4f), fp);
		}
		return actualWriteSize == writeSize;
	}
};

struct AutoClose
{
	FILE *_fp;

	AutoClose (FILE *fp): _fp(fp) {}
	~AutoClose (void) { if (_fp) fclose(_fp); }
};

struct MaterialChunk
{
	char diffuseMapName1[256];
	char diffuseMapName2[256];
	char normalMapName[256];
	float emissiveColor[3];
	float ambientColor[3];
	float diffuseColor[3];
	int alphaTest;
	int doubleSide;
	float alpharef;
	float shininess;
	float transparency;
	int diffuseChannel1;
	int diffuseChannel2;
	int normalmapChannel;
};

struct MaterialChunkEx: public MaterialChunk
{
	unsigned materialType;
};

struct MeshChunk
{
	int numVertices;
	int numIndices;
	int hasColor;
	int hasNormal;
	int hasTangents;
	int numChannels;
	int vertexOffset;
	int colorOffset;
	int normalOffset;
	int tangentOffset;
	int binormalOffset;
	int texcoordOffset;
	int indexOffset;
	int isBillboard;
	float worldMatrix[16];
	int transparencyMode;
	int uvAnimationNoIpol;
	int fixBlending;
	int prophandle;

	unsigned materialFourcc;
	unsigned materialChunkSize;
	MaterialChunkEx materialChunk;
};

struct MainChunk
{
	int version;
};

struct ATOM_BBoxChunk
{
	float minPoint[3];
	float maxPoint[3];
};

bool MyModel::save (const char *filename, bool saveSkeleton, bool saveAnimations, bool saveMeshes, SkeletonViewer *viewer, PropertyEditor *props, VertexAnimationInfo *animationInfo)
{
	_logger->output ("--- 开始输出到%s ---\n", filename);

	if (meshes().size() > 0)
	{
		unsigned i = 0;
		while (i < meshes().size())
		{
			_logger->output ("处理网格%d ...", i);
			if (!meshes()[i].process (animationInfo, viewer->getSkeleton().skeleton))
			{
				meshes().erase (meshes().begin() + i);
				_logger->output ("失败\n");
			}
			else
			{
				_logger->output ("完成\n");
				++i;
			}
		}

		if (meshes().empty ())
		{
			_logger->output ("** 导出失败!\n");
			return false;
		}
	}

	_logger->output ("生成包围体结构\n");
	generateAABBTree ();

	FILE *fp = fopen (filename, "wb");
	if (!fp)
	{
		_logger->output ("创建文件失败!\n");
		return false;
	}

	AutoClose ac(fp);

	const unsigned MeshChunkFourCC = ATOM_MAKE_FOURCC('M','E','S','H');
	const unsigned MeshChunkFourCC2 = ATOM_MAKE_FOURCC('M','E','S','2');
	const unsigned MeshChunkFourCC3 = ATOM_MAKE_FOURCC('M','E','S','3');
	const unsigned MeshChunkFourCC4 = ATOM_MAKE_FOURCC('M','E','S','4');
	const unsigned MeshChunkFourCC5 = ATOM_MAKE_FOURCC('M','E','S','5');
	const unsigned MeshChunkFourCC6 = ATOM_MAKE_FOURCC('M','E','S','6');
	const unsigned MeshChunkFourCC7 = ATOM_MAKE_FOURCC('M','E','S','7');
	const unsigned MaterialChunkFourCC = ATOM_MAKE_FOURCC('M','T','R','L');
	const unsigned MaterialChunkFourCC2 = ATOM_MAKE_FOURCC('M','T','R','2');
	const unsigned MaterialChunkFourCC3 = ATOM_MAKE_FOURCC('M','T','R','3');
	const unsigned GeometryDataChunkFourCC = ATOM_MAKE_FOURCC('G','E','O','M');
	const unsigned MainChunkFourCC = ATOM_MAKE_FOURCC('N','3','M','F');
	const unsigned ATOM_BBoxChunkFourCC = ATOM_MAKE_FOURCC('B','B','O','X');
	const unsigned AABBTreeChunkFourCC = ATOM_MAKE_FOURCC('A', 'B', 'T', '0');
	const unsigned VertexAnimationFourCC = ATOM_MAKE_FOURCC('V', 'E', 'R', 'A');
	const unsigned SkeletonFourCC = ATOM_MAKE_FOURCC('S', 'K', 'L', 'T');
	const unsigned SkeletonFourCC2 = ATOM_MAKE_FOURCC('S', 'K', 'L', '2');
	const unsigned SkeletonFourCC3 = ATOM_MAKE_FOURCC('S', 'K', 'L', '3');
	const unsigned VertexWeightFourCC = ATOM_MAKE_FOURCC('W', 'G', 'H', 'T');
	const unsigned VertexWeightFourCC2 = ATOM_MAKE_FOURCC('W', 'G', 'H', '2');
	const unsigned SkeletonAnimationFourCC = ATOM_MAKE_FOURCC('S', 'K', 'L', 'A');
	const unsigned AnimationCacheFourCC = ATOM_MAKE_FOURCC('A', 'C', 'A', 'C');
	const unsigned AnimationCacheFourCC2 = ATOM_MAKE_FOURCC('A', 'C', 'A', '2');
	const unsigned MeshAnimationDataFourCC = ATOM_MAKE_FOURCC('M', 'A', 'D', 'T');
	const unsigned MeshAnimationDataFourCC2 = ATOM_MAKE_FOURCC('M', 'A', 'D', '2');
	const unsigned MeshAnimationDataFourCC3 = ATOM_MAKE_FOURCC('M', 'A', 'D', '3');
	const unsigned MeshAnimationDataFourCC4 = ATOM_MAKE_FOURCC('M', 'A', 'D', '4');
	const unsigned PropertyDataCacheFourCC = ATOM_MAKE_FOURCC('P', 'R', 'P', 'C');
	const unsigned ModelPropertiesFourCC = ATOM_MAKE_FOURCC('M', 'P', 'R', 'P');

	const unsigned meshChunkSize = sizeof(MeshChunk);

	const unsigned chunkHeaderSize = 2 * sizeof(unsigned);
	const unsigned mainChunkSize = sizeof(MainChunk);
	const unsigned materialChunkSize = sizeof(MaterialChunkEx);
	const unsigned bboxChunkSize = sizeof(ATOM_BBoxChunk);
	const unsigned AABBTreeChunkSize = _aabbtree.getStoreSize ();
	
	unsigned geometryChunkSize = 0;

	std::vector<MeshChunk> meshChunks(_meshes.size());

	PropertyDataCache propertyDataCache;

	int modelPropsIndex = -1;
	if (props && props->getNumProperties() > 0)
	{
		PropertyData pd;
		for (unsigned i = 0; i < props->getNumProperties(); ++i)
		{
			pd.addProperty (props->getPropertyName (i), props->getPropertyValue (i));
		}
		modelPropsIndex = propertyDataCache.addPropertyData (pd);
	}

	if (saveMeshes)
	{
		for (unsigned i = 0; i < _meshes.size(); ++i)
		{
			const MyMesh &mesh = _meshes[i];

			PropertyData pd;
			for (std::map<std::string, std::string>::const_iterator it = _meshes[i].getProperties().begin(); it != _meshes[i].getProperties().end(); ++it)
			{
				pd.addProperty (it->first.c_str(), it->second.c_str());
			}

			MeshChunk &meshChunk = meshChunks[i];

			int numVertices = mesh.getNumVertices();
			int numIndices = mesh.getNumIndices();
			int vertexOffset = geometryChunkSize;
			int colorOffset = 0;
			int normalOffset = 0;
			int tangentOffset = 0;
			int binormalOffset = 0;
			int texcoordOffset = 0;
			int indexOffset = 0;
			int numChannels = 1;
			bool hasColor = (mesh.getVertexFlags() & MyMesh::HasColors) != 0;
			bool hasNormal = (mesh.getVertexFlags() & MyMesh::HasNormals) != 0;
			bool hasTangents = (mesh.getVertexFlags() & MyMesh::HasTangents) != 0;
			bool isBillboard = (mesh.getVertexFlags() & MyMesh::ATOM_Billboard) != 0;
			bool uvAnimationNoIpol = (mesh.getVertexFlags() & MyMesh::UVAnimationNoIpol) != 0;
			bool fixBlending = (mesh.getVertexFlags() & MyMesh::FixBlending) != 0;
			
			meshChunk.numVertices = numVertices;
			meshChunk.numIndices = numIndices;
			meshChunk.numChannels = numChannels;
			meshChunk.hasColor = hasColor;
			meshChunk.hasNormal = hasNormal;
			meshChunk.hasTangents = hasTangents;
			meshChunk.isBillboard = isBillboard ? 1 : 0;
			meshChunk.numChannels = numChannels;
			meshChunk.vertexOffset = geometryChunkSize;
			meshChunk.colorOffset = 0;
			meshChunk.normalOffset = 0;
			meshChunk.tangentOffset = 0;
			meshChunk.binormalOffset = 0;
			meshChunk.texcoordOffset = 0;
			meshChunk.indexOffset = 0;
			meshChunk.transparencyMode = mesh.getTransparencyMode ();
			meshChunk.uvAnimationNoIpol = uvAnimationNoIpol ? 1 : 0;
			meshChunk.fixBlending = fixBlending ? 1 : 0;
			meshChunk.materialFourcc = MaterialChunkFourCC3;
			meshChunk.materialChunkSize = sizeof(MaterialChunkEx);
			meshChunk.prophandle = pd.writeSize > 0 ? propertyDataCache.addPropertyData (pd) : -1;

			memcpy(meshChunk.worldMatrix, mesh.getWorldMatrix().m, 16*sizeof(float));

			geometryChunkSize += numVertices * 3 * sizeof(float);

			if (hasColor)
			{
				meshChunk.colorOffset = geometryChunkSize;
				geometryChunkSize += numVertices * sizeof(unsigned);
			}

			if (hasNormal) 
			{
				meshChunk.normalOffset = geometryChunkSize;
				geometryChunkSize += numVertices * 3 * sizeof(float);
			}

			if (hasTangents)
			{
				meshChunk.tangentOffset = geometryChunkSize;
				geometryChunkSize += numVertices * 3 * sizeof(float);

				//meshChunk.binormalOffset = geometryChunkSize;
				//geometryChunkSize += numVertices * 3 * sizeof(float);
			}

			meshChunk.texcoordOffset = geometryChunkSize;
			geometryChunkSize += numVertices * 2 * sizeof(float) * numChannels;

			meshChunk.indexOffset = geometryChunkSize;
			geometryChunkSize += numIndices * sizeof(unsigned short);

			const material &m = mesh.getMaterial ();
			strcpy (meshChunk.materialChunk.diffuseMapName1, m.diffuseMap1.c_str());
			strcpy (meshChunk.materialChunk.diffuseMapName2, m.diffuseMap2.c_str());
			strcpy (meshChunk.materialChunk.normalMapName, m.normalMap.c_str());
			meshChunk.materialChunk.alphaTest = m.alphaTest ? 1 : 0;
			meshChunk.materialChunk.doubleSide = m.doubleSide ? 1 : 0;
			meshChunk.materialChunk.alpharef = m.alpharef;
			meshChunk.materialChunk.shininess = m.shininess;
			meshChunk.materialChunk.transparency = m.transparency;
			meshChunk.materialChunk.diffuseChannel1 = m.diffuseChannel1;
			meshChunk.materialChunk.diffuseChannel2 = m.diffuseChannel2;
			meshChunk.materialChunk.normalmapChannel = m.normalmapChannel;
			meshChunk.materialChunk.emissiveColor[0] = m.emissiveColor.x;
			meshChunk.materialChunk.emissiveColor[1] = m.emissiveColor.y;
			meshChunk.materialChunk.emissiveColor[2] = m.emissiveColor.z;
			meshChunk.materialChunk.ambientColor[0] = 1.f;
			meshChunk.materialChunk.ambientColor[1] = 1.f;
			meshChunk.materialChunk.ambientColor[2] = 1.f;
			meshChunk.materialChunk.diffuseColor[0] = m.diffuseColor.x;
			meshChunk.materialChunk.diffuseColor[1] = m.diffuseColor.y;
			meshChunk.materialChunk.diffuseColor[2] = m.diffuseColor.z;
			meshChunk.materialChunk.materialType = mesh.getMaterialStyle ();
		}
	}

	unsigned animationChunkSize = 0;
	AnimationCache animationCache (*this, saveMeshes, saveAnimations);
	AnimationData animationData (*this, &propertyDataCache, saveMeshes, saveAnimations);
	SkeletonData skeletonData(viewer->getSkeleton().skeleton);

	unsigned weightSize = 0;
	ATOM_Skeleton *skeleton = 0;
	if (saveMeshes)
	{
		for (unsigned i = 0; i < _meshes.size(); ++i)
		{
			weightSize += sizeof(unsigned); // weight flag
			weightSize += sizeof(unsigned); // skeleton hash

			if (_meshes[i].getSkeleton())
			{
				assert (skeleton == 0 || skeleton == _meshes[i].getSkeleton());
				skeleton = _meshes[i].getSkeleton ();
				weightSize += _meshes[i].getNumVertices() * (4 * sizeof(float) + 4 * sizeof(int) + 4 * sizeof(int));
			}
		}

		weightSize += sizeof(unsigned); // num joints
		if (skeleton)
		{
			weightSize += sizeof(unsigned) * skeleton->getAttachPoints().size(); // joint hashes
		}
	}

	unsigned animationDataSize = saveAnimations ? animationData.writeSize : 0;
	unsigned animationCacheSize = saveAnimations ? animationCache.writeSize : 0;
	unsigned modelPropsSize = modelPropsIndex >= 0 ? sizeof(unsigned) : 0;

	unsigned totalSize = 
		// main chunk
		mainChunkSize + 
		// bbox chunk
		(saveMeshes ? chunkHeaderSize + bboxChunkSize : 0) + 
		// aabbtree chunk
		(saveMeshes ? chunkHeaderSize + AABBTreeChunkSize : 0) +
		// mesh chunks
		(saveMeshes ? _meshes.size() * (chunkHeaderSize + meshChunkSize) : 0) + 
		// geometry chunk
		(saveMeshes ? (chunkHeaderSize + geometryChunkSize) : 0) +
		// weight chunk
		(weightSize > 0 ? (chunkHeaderSize + weightSize) : 0) +
		// animations
		(animationDataSize > 0 ? (chunkHeaderSize + animationDataSize) : 0) +
		(animationCacheSize > 0 ? (chunkHeaderSize + animationCacheSize) : 0) +
		// skeletons
		((saveSkeleton && skeletonData.writeSize > 0) ? (chunkHeaderSize + skeletonData.writeSize) : 0) +
		// properties
		((propertyDataCache.writeSize > 0) ? (chunkHeaderSize + propertyDataCache.writeSize) : 0) +
		// model properties
		((modelPropsSize > 0) ? (chunkHeaderSize + modelPropsSize) : 0);
		
	/*
		std::cout << "ATOM_Model details:" << std::endl;
		std::cout << "mainChunkSize = " << mainChunkSize << std::endl;
		if (saveMeshes)
		{
			std::cout << "bboxChunkSize = " << bboxChunkSize << std::endl;
			std::cout << "AABBTreeChunkSize = " << AABBTreeChunkSize << std::endl;
			std::cout << "MeshChunkSize = " << meshChunkSize << " * " << _meshes.size() << std::endl;
			std::cout << "GeometryChunkSize = " << geometryChunkSize << std::endl;
		}
		if (weightSize > 0)
		{
			std::cout << "WeightSize = " << weightSize << std::endl;
		}
		std::cout << "AnimationDataSize = " << animationDataSize << std::endl;
		std::cout << "AnimationCacheSize = " << animationCacheSize << std::endl;
		if (saveSkeleton)
		{
			std::cout << "SkeletonDataSize = " << skeletonData.writeSize << std::endl;
		}
		std::cout << "PropertyDataSize = " << propertyDataCache.writeSize << std::endl;
		std::cout << "ModelPropertySize = " << modelPropsSize << std::endl;
	*/

	MainChunk mainChunk;

	_logger->output ("写入文件头!\n");
	if (fwrite (&MainChunkFourCC, 1, sizeof(unsigned), fp) != sizeof(unsigned))
	{
		_logger->output ("**写入文件失败!\n");
		return false;
	}

	if (fwrite (&totalSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
	{
		_logger->output ("**写入文件失败!\n");
		return false;
	}

	mainChunk.version = 2;
	if (fwrite (&mainChunk, 1, sizeof(mainChunk), fp) != sizeof(mainChunk))
	{
		_logger->output ("**写入文件失败!\n");
		return false;
	}

	if (saveMeshes)
	{
		_logger->output ("保存包围体数据!\n");

		if (fwrite (&AABBTreeChunkFourCC, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (fwrite (&AABBTreeChunkSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		_aabbtree.save (fp);

		ATOM_BBoxChunk bboxChunk;
		computeBoundingBox (bboxChunk.minPoint, bboxChunk.maxPoint);
		if (fwrite (&ATOM_BBoxChunkFourCC, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}
		if (fwrite (&bboxChunkSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}
		if (fwrite (&bboxChunk, 1, bboxChunkSize, fp) != bboxChunkSize)
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		_logger->output ("保存几何数据\n");

		for (unsigned i = 0; i < meshChunks.size(); ++i)
		{
			if (fwrite (&MeshChunkFourCC7, 1, sizeof(unsigned), fp) != sizeof(unsigned))
			{
				_logger->output ("**写入文件失败!\n");
				return false;
			}

			if (fwrite (&meshChunkSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
			{
				_logger->output ("**写入文件失败!\n");
				return false;
			}

			if (fwrite (&meshChunks[i], 1, sizeof(MeshChunk), fp) != sizeof(MeshChunk))
			{
				_logger->output ("**写入文件失败!\n");
				return false;
			}
		}

		if (fwrite (&GeometryDataChunkFourCC, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (fwrite (&geometryChunkSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		for (unsigned i = 0; i < _meshes.size(); ++i)
		{
			const MyMesh &mesh = _meshes[i];
			unsigned nVerts = mesh.getNumVertices ();
			bool hasColor = (mesh.getVertexFlags() & MyMesh::HasColors) != 0;
			bool hasNormal = (mesh.getVertexFlags() & MyMesh::HasNormals) != 0;
			bool hasTangent = (mesh.getVertexFlags() & MyMesh::HasTangents) != 0;
			unsigned nChannels = 1;

			for (unsigned v = 0; v < nVerts; ++v)
			{
				if (fwrite (&mesh.getVertex(v), 1, sizeof(ATOM_Vector3f), fp) != sizeof(ATOM_Vector3f))
				{
					_logger->output ("**写入文件失败!\n");
					return false;
				}
			}

			if (hasColor)
			{
				for (unsigned v = 0; v < nVerts; ++v)
				{
					if (fwrite (&mesh.getColor(v), 1, sizeof(unsigned), fp) != sizeof(unsigned))
					{
						_logger->output ("**写入文件失败!\n");
						return false;
					}
				}
			}

			if (hasNormal)
			{
				for (unsigned v = 0; v < nVerts; ++v)
				{
					if (fwrite (&mesh.getNormal(v), 1, sizeof(ATOM_Vector3f), fp) != sizeof(ATOM_Vector3f))
					{
						_logger->output ("**写入文件失败!\n");
						return false;
					}
				}
			}

			if (hasTangent)
			{
				for (unsigned v = 0; v < nVerts; ++v)
				{
					if (fwrite (&mesh.getTangent(v), 1, sizeof(ATOM_Vector3f), fp) != sizeof(ATOM_Vector3f))
					{
						_logger->output ("**写入文件失败!\n");
						return false;
					}
				}
				//for (unsigned v = 0; v < nVerts; ++v)
				//{
				//	if (fwrite (&mesh.getBinormal(v), 1, sizeof(ATOM_Vector3f), fp) != sizeof(ATOM_Vector3f))
				//		return false;
				//}
			}

			for (unsigned channel = 0; channel < nChannels; ++channel)
			{
				for (unsigned v = 0; v < nVerts; ++v)
				{
					if (fwrite (&mesh.getTexCoord(v), 1, sizeof(ATOM_Vector2f), fp) != sizeof(ATOM_Vector2f))
					{
						_logger->output ("**写入文件失败!\n");
						return false;
					}
				}
			}

			for (int v = 0; v < mesh.getNumIndices(); ++v)
			{
				if (fwrite (&mesh.getIndex (v), 1, sizeof(unsigned short), fp) != sizeof(unsigned short))
				{
					_logger->output ("**写入文件失败!\n");
					return false;
				}
			}
		}
	}

	if (weightSize > 0)
	{
		_logger->output ("保存蒙皮数据\n");
		if (fwrite (&VertexWeightFourCC2, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (fwrite (&weightSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		ATOM_Skeleton *skeleton = 0;

		for (unsigned i = 0; i < _meshes.size(); ++i)
		{
			unsigned hasWeight = _meshes[i].getSkeleton() ? 1 : 0;

			if (fwrite (&hasWeight, 1, sizeof(unsigned), fp) != sizeof(unsigned))
			{
				_logger->output ("**写入文件失败!\n");
				return false;
			}

			unsigned hash = hasWeight ? _meshes[i].getSkeleton()->getHash() : 0;

			if (fwrite (&hash, 1, sizeof(unsigned), fp) != sizeof(unsigned))
			{
				_logger->output ("**写入文件失败!\n");
				return false;
			}

			if (hasWeight)
			{
				skeleton = _meshes[i].getSkeleton ();

				for (int v = 0; v < _meshes[i].getNumVertices(); ++v)
				{
					for (unsigned n = 0; n < 4; ++n)
					{
						if (fwrite (&_meshes[i].getWeights(v).xyzw[n], 1, sizeof(float), fp) != sizeof(float))
						{
							_logger->output ("**写入文件失败!\n");
							return false;
						}
						if (fwrite (&_meshes[i].getJointIndices(v).xyzw[n], 1, sizeof(int), fp) != sizeof(int))
						{
							_logger->output ("**写入文件失败!\n");
							return false;
						}
						unsigned offset = (4-n) * (sizeof(float) + sizeof(int) + sizeof(unsigned));
						if (fwrite (&offset, 1, sizeof(unsigned), fp) != sizeof(unsigned))
						{
							_logger->output ("**写入文件失败!\n");
							return false;
						}
					}
				}
			}
		}

		unsigned numJoints = skeleton ? skeleton->getAttachPoints().size() : 0;
		if (fwrite (&numJoints, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}
		if (numJoints)
		{
			const std::vector<int> &newToOld = skeleton->getRemapNewToOld ();
			for (unsigned joint = 0; joint < numJoints; ++joint)
			{
				if (fwrite (&skeleton->getJoints()[newToOld[joint]].hashCode, 1, sizeof(unsigned), fp) != sizeof(unsigned))
				{
					_logger->output ("**写入文件失败!\n");
					return false;
				}
			}
		}
	}

	if (animationCacheSize > 0)
	{
		_logger->output ("保存动画数据\n");

		if (fwrite (&AnimationCacheFourCC2, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (fwrite (&animationCacheSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (!animationCache.write (fp))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}
	}

	if (animationDataSize > 0)
	{
		_logger->output ("保存关键帧数据\n");

		if (fwrite (&MeshAnimationDataFourCC4, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (fwrite (&animationDataSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (!animationData.write (*this, &propertyDataCache, fp))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}
	}

	if (saveSkeleton && skeletonData.writeSize > 0)
	{
		_logger->output ("保存骨骼信息!\n");

		if (fwrite (&SkeletonFourCC3, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (fwrite (&skeletonData.writeSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (!skeletonData.write (fp))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}
	}

	if (propertyDataCache.writeSize > 0)
	{
		_logger->output ("保存自定义属性\n");

		if (fwrite (&PropertyDataCacheFourCC, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (fwrite (&propertyDataCache.writeSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (!propertyDataCache.write (fp))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}
	}

	if (modelPropsIndex >= 0)
	{
		if (fwrite (&ModelPropertiesFourCC, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		unsigned n = sizeof(unsigned);
		if (fwrite (&n, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}

		if (fwrite (&modelPropsIndex, 1, sizeof(int), fp) != sizeof(int))
		{
			_logger->output ("**写入文件失败!\n");
			return false;
		}
	}

	return true;
}

void MyModel::generateAABBTree (void)
{
	std::vector<ATOM_Vector3f> totalVertices;
	std::vector<unsigned short> totalIndices;

	unsigned v_counter = 0;
	unsigned i_counter = 0;

	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		const MyMesh &mesh = _meshes[i];
		unsigned nVerts = mesh.getNumVertices ();
		unsigned nIndices = mesh.getNumIndices ();

		totalVertices.resize (v_counter + nVerts);
		totalIndices.resize (i_counter + nIndices);

		for (unsigned vert = 0; vert < nVerts; ++vert)
		{
			totalVertices[v_counter + vert] = mesh.getVertex (vert);
		}

		for (unsigned idx = 0; idx < nIndices; ++idx)
		{
			totalIndices[i_counter + idx] = v_counter + mesh.getIndex (idx);
		}

		v_counter += nVerts;
		i_counter += nIndices;
	}

	ATOM_AABBTreeBuilder builder;
	builder.buildFromPrimitives (&_aabbtree, ATOM_AABBTree::PT_TRIANGLES, &totalVertices[0], totalVertices.size(), &totalIndices[0], totalIndices.size() / 3, 4);
}
