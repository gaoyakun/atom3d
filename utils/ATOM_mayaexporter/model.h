#ifndef __ATOM_maxexporter_model_h__
#define __ATOM_maxexporter_model_h__

#include <ATOM_geometry.h>
#include "mesh.h"
#include "skeleton.h"

class ATOM_MyAABBTree: public ATOM_AABBTree
{
protected:
	virtual unsigned writeFile (void *handle, const void *buffer, unsigned size) const
	{
		return fwrite (buffer, 1, size, (FILE*)handle);
	}

	virtual unsigned readFile (void *handle, void *buffer, unsigned size) const
	{
		return fread (buffer, 1, size, (FILE*)handle);
	}
};

struct skeletonKeyframe
{
	unsigned time;								//time of keyframe
	std::vector<ATOM_Matrix4x4f> jointMatrix;
};

struct skeletonAction
{
	std::string name;
	std::vector<skeletonKeyframe> keyframes;
};

class SkeletonViewer;
class PropertyEditor;

class MyModel
{
public:
	void addMesh (const MyMesh &mesh);
	bool save (const char *filename, bool saveSkeleton, bool saveAnimations, bool saveMeshes, SkeletonViewer *skeletonViewer, PropertyEditor *props, VertexAnimationInfo *animationInfo);
	std::vector<MyMesh> &meshes(void) { return _meshes; }
	const std::vector<MyMesh> &meshes(void) const { return _meshes; }
	void setNumActions (unsigned num) 
	{ 
		_actionProps.resize(num);
		_actionFrames.resize(num); 
		_actionBBoxes.resize (num);
		_frameTimes.resize(num);
		_actionNames.resize(num);
		_jointMatrices.resize(num);
		_jointMatrixIndices.resize(num);
		_actionHashes.resize(num);
	}
	unsigned getNumActions (void) const { return _actionFrames.size(); }
	void setActionProps (unsigned action, PropertyEditor *props) { _actionProps[action] = props; }
	PropertyEditor *getActionProps (unsigned action) const { return _actionProps[action]; }
	void setNumActionFrames (unsigned action, unsigned frameCount) 
	{ 
		_actionFrames[action] = frameCount; 
		_frameTimes[action].resize (frameCount);
		_jointMatrices[action].resize (frameCount);
		_jointMatrixIndices[action].resize (frameCount);
	}
	unsigned getNumActionFrames (unsigned action) const { return _actionFrames[action]; }
	void setFrameTime (unsigned action, unsigned frame, unsigned time) { _frameTimes[action][frame] = time; }
	unsigned getFrameTime (unsigned action, unsigned frame) const { return _frameTimes[action][frame]; }
	void setActionName (unsigned action, const char *name) { _actionNames[action] = name; }
	const char *getActionName (unsigned action) const { return _actionNames[action].c_str(); }
	void setFrameJoints (unsigned action, unsigned frame, const std::vector<ATOM_Matrix4x4f> &matrices) { _jointMatrices[action][frame] = matrices; }
	const std::vector<ATOM_Matrix4x4f> &getFrameJoints (unsigned action, unsigned frame) const { return _jointMatrices[action][frame]; }
	void setFrameJointIndex (unsigned action, unsigned frame, int index) { _jointMatrixIndices[action][frame] = index; }
	int getFrameJointIndex (unsigned action, unsigned frame) const { return _jointMatrixIndices[action][frame]; }
	void setActionHash (unsigned action, unsigned hash) { _actionHashes[action] = hash; }
	unsigned getActionHash (unsigned action) const { return _actionHashes[action]; }
	ATOM_BBox &getActionBBox (unsigned action) { return _actionBBoxes[action]; }
	const ATOM_BBox &getActionBBox (unsigned action) const { return _actionBBoxes[action]; }
	void setSkeleton (ATOM_Skeleton *skeleton) { _skeleton = skeleton; }
	ATOM_Skeleton *getSkeleton (void) const { return _skeleton; }

private:
	ATOM_Vector3f _bboxMin;
	ATOM_Vector3f _bboxMax;

private:
	void computeBoundingBox (float minPoint[3], float maxPoint[3]);
	void generateAABBTree (void);

private:
	std::vector<MyMesh> _meshes;
	ATOM_MyAABBTree _aabbtree;
	ATOM_Skeleton *_skeleton;
	std::vector<PropertyEditor*> _actionProps;
	std::vector<unsigned> _actionFrames;
	std::vector<std::vector<unsigned> > _frameTimes;
	std::vector<std::string> _actionNames;
	std::vector<ATOM_BBox> _actionBBoxes;
	std::vector<std::vector<std::vector<ATOM_Matrix4x4f> > > _jointMatrices;
	std::vector<std::vector<int> > _jointMatrixIndices;
	std::vector<unsigned> _actionHashes;
};

#endif // __ATOM_maxexporter_model_h__
