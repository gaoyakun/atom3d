#ifndef __ATOM_maxexporter_mesh_h__
#define __ATOM_maxexporter_mesh_h__

#include <string>
#include <vector>
#include <ATOM_math.h>

class ATOM_Skeleton;
struct VertexAnimationInfo;

enum TransparencyMode
{
	TM_NORMAL,		// normal blending (srcAlpha, 1-srcAlpha)
	TM_ADDITIVE,	// additive blending (1, 1)
	TM_PARTICLE,	// particle blending (srcAlpha, 1)
	TM_MODULATE,	// modulate (0, srcColor)
	TM_MODULATE_B,	// modulate brighter (srcAlpha, srcColor)
	TM_IMODULATE,	// inverse modulate (0, 1-srcColor)
	TM_IMODULATE_B,	// inverse modulate brigher (srcAlpha, 1-srcColor)
	TM_INORMAL,		// inverse normal blending (1-srcAlpha, srcAlpha)
};

enum MaterialStyle
{
	MS_DEFAULT,
	MS_METAL,
	MS_PLASTIC,
	MS_SKIN
};

struct material
{
	std::string name;
	std::string diffuseMap1;
	std::string diffuseMap2;
	std::string normalMap;
	ATOM_Vector3f emissiveColor;
	ATOM_Vector3f ambientColor;
	ATOM_Vector3f diffuseColor;
	bool alphaTest;
	bool doubleSide;
	float alpharef;
	float shininess;
	float transparency;
	int diffuseChannel1;
	int diffuseChannel2;
	int normalmapChannel;
};

struct StripV;

struct i4
{
	union
	{
		struct
		{
			int x, y, z, w;
		};
		int xyzw[4];
	};

	friend inline bool operator == (const i4 &i1, const i4 &i2)
	{
		return i1.x == i2.x && i1.y == i2.y && i1.z == i2.z && i1.w == i2.w;
	}

	friend inline bool operator != (const i4 &i1, const i4 &i2)
	{
		return ! operator == (i1, i2);
	}
};

class MyMesh
{
public:
	struct VertexAnimationFrame
	{
		std::vector<ATOM_Vector3f> vertices;
		std::vector<ATOM_Vector2f> uvs;
		ATOM_BBox boundingbox;
		ATOM_Matrix4x4f transform;

		mutable int vertexCacheIndex;
		mutable int uvCacheIndex;
		mutable int bboxCacheIndex;
		mutable int transformCacheIndex;
	};

	struct MaterialFrame
	{
		float transparency;
	};

	struct SkeletonAnimationFrame
	{
		std::vector<ATOM_Matrix4x4f> jointMatrices;
		ATOM_BBox boundingbox;

		mutable int jointCacheIndex;
		mutable int bboxCacheIndex;
	};

	struct Action
	{
		std::vector<int> frames;
		bool vertexAnimation;
		bool uvAnimation;
		bool jointAnimation;
		bool materialAnimation;
	};

public:
	MyMesh (const char *name = 0);
	~MyMesh (void);

public:
	static const int HasNormals = (1<<0);
	static const int HasTangents = (1<<1);
	static const int HasColors = (1<<2);
	static const int Transparent = (1<<3);
	static const int ATOM_Billboard = (1<<4);
	static const int UVAnimationNoIpol = (1<<5);
	static const int FixBlending = (1<<6);

	struct Vertex
	{
		ATOM_Vector3f position;
		unsigned color;
		ATOM_Vector3f normal;
		ATOM_Vector3f tangent;
		ATOM_Vector3f binormal;
		ATOM_Vector2f texcoords;
		ATOM_Vector4f weights;
		i4 weightjoints;
	};

public:
	// read methods
	void alarm (const char *format, ...);
	int getNumVertices (void) const;
	int getNumIndices (void) const;
	const ATOM_Vector3f &getVertex (int index) const;
	const ATOM_Vector4f &getWeights (int index) const;
	const i4 &getJointIndices (int index) const;
	const unsigned &getColor (int index) const;
	const ATOM_Vector3f &getNormal (int index) const;
	const ATOM_Vector3f &getTangent (int index) const;
	const ATOM_Vector3f &getBinormal (int index) const;
	const unsigned short &getIndex (int index) const;
	const ATOM_Vector2f &getTexCoord (int index) const;
	const material &getMaterial (void) const;
	int getVertexFlags (void) const;
	const Vertex &getSuperVertex (int index) const;
	void setWorldMatrix (const ATOM_Matrix4x4f &matrix);
	const ATOM_Matrix4x4f &getWorldMatrix (void) const;

	// animations
	void addAnimationFrame (unsigned action, const ATOM_Matrix4x4f &worldMatrix, const std::vector<ATOM_Vector3f> &verts, const std::vector<ATOM_Vector2f> &uvs);
	void addSkeletonFrame (unsigned action, const std::vector<ATOM_Matrix4x4f> &matrices);
	void addMaterialFrame (unsigned action, float transparency);
	void setNumActions (unsigned num) { _actions.resize (num); }
	unsigned getNumActions (void) const { return _actions.size(); }
	const Action &getAction (unsigned index) const { return _actions[index]; }
	const std::vector<MyMesh::VertexAnimationFrame> &getVertexAnimationFrames (void) const { return _frames; }
	ATOM_Skeleton *getSkeleton (void) const { return _skeleton; }
	void setSkeleton (ATOM_Skeleton *skeleton) { _skeleton = skeleton; }
	unsigned getNumVertexAnimationFrames (void) const { return _frames.size(); }
	const VertexAnimationFrame &getVertexAnimationFrame (unsigned index) const { return _frames[index]; }
	unsigned getNumSkeletonAnimationFrames (void) const { return _skeletonFrames.size(); }
	const SkeletonAnimationFrame &getSkeletonAnimationFrame (unsigned index) const { return _skeletonFrames[index]; }
	unsigned getNumMaterialFrames (void) const { return _materialFrames.size(); }
	const MaterialFrame &getMaterialFrame (unsigned index) const { return _materialFrames[index]; }
	void setTransparencyMode (TransparencyMode mode) { _transparencyMode = mode; }
	TransparencyMode getTransparencyMode (void) const { return _transparencyMode; }
	void setMaterialStyle (MaterialStyle style) { _materialStyle = style; }
	MaterialStyle getMaterialStyle (void) const { return _materialStyle; }


	// write methods
	void setNumVertices (int n);
	void setVertex (int index, const ATOM_Vector3f &v);
	void setWeights (int index, const ATOM_Vector4f &v);
	void setJointIndices (int index, const i4 &jointIndices);
	void setVertex (int index, float x, float y, float z);
	void setColor (int index, const ATOM_Vector4f &c);
	void setTexCoord (int index, const ATOM_Vector2f &v);
	void setTexCoord (int index, float u, float v);
	material &getMaterial (void);
	void setVertexFlags (int flags);
	void setSuperVertex (int index, const Vertex &v);
	void pushSuperVertex (const Vertex &v);

	// property
	void addProperty (const char *key, const char *value);
	const std::map<std::string, std::string> &getProperties (void) const;

	// serialize
	bool process (VertexAnimationInfo *animationInfo, ATOM_Skeleton *skeleton);

	//
	void useSkeleton (bool b);
	bool usingSkeleton (void) const;

private:
	bool genIndices (void);
	bool computeNormals (int smoothAngle);
	void optimizeFaces (void);
	void optimizeJoints (ATOM_Skeleton *skeleton);
	bool vertexEqual (const StripV &v1, const StripV &v2) const;
	static bool compare_vertex (const Vertex &v1, const Vertex &v2);

private:
	int _vertexFlags;
	material _material;
	TransparencyMode _transparencyMode;
	MaterialStyle _materialStyle;
	std::string _name;

	std::vector<unsigned short> _indices;
	std::vector<Vertex> _vertices;
	std::vector<Action> _actions;
	std::vector<VertexAnimationFrame> _frames;
	std::vector<SkeletonAnimationFrame> _skeletonFrames;
	std::vector<MaterialFrame> _materialFrames;
	std::map<std::string, std::string> _properties;

	ATOM_Matrix4x4f _worldMatrix;

	ATOM_Skeleton *_skeleton;
	bool _useSkeleton;
};

#endif // __ATOM_maxexporter_mesh_h__
