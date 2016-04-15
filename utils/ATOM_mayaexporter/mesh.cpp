#include <algorithm>
#include <stdarg.h>
#include <iostream>
#include <windows.h>

#include <ATOM_geometry.h>
#include <ATOM_utils.h>
#include "mesh.h"
#include "NVMeshMender.h"
#include "skeleton.h"

MyMesh::MyMesh (const char *name)
{
	_name = name ? name : "<noname>";
	_vertexFlags = 0;
	_transparencyMode = TM_NORMAL;
	_materialStyle = MS_DEFAULT;
	_useSkeleton = false;
}

MyMesh::~MyMesh (void)
{
}

void MyMesh::setWorldMatrix (const ATOM_Matrix4x4f &matrix)
{
	_worldMatrix = matrix;
}

const ATOM_Matrix4x4f &MyMesh::getWorldMatrix (void) const
{
	return _worldMatrix;
}

void MyMesh::addMaterialFrame (unsigned action, float transparency)
{
	_materialFrames.resize (_materialFrames.size() + 1);
	_materialFrames.back ().transparency = transparency;
}

void MyMesh::addSkeletonFrame (unsigned action, const std::vector<ATOM_Matrix4x4f> &matrices)
{
	_skeletonFrames.resize (_skeletonFrames.size() + 1);
	_skeletonFrames.back().jointMatrices = matrices;

	assert(matrices.size() > 0);
	_skeletonFrames.back().boundingbox.beginExtend ();
	for (unsigned i = 0; i < matrices.size(); ++i)
	{
		_skeletonFrames.back().boundingbox.extend (ATOM_Vector3f(matrices[i].m32, matrices[i].m31, matrices[i].m30));
	}
}

void MyMesh::addAnimationFrame (unsigned actionindex, const ATOM_Matrix4x4f &worldMatrix, const std::vector<ATOM_Vector3f> &verts, const std::vector<ATOM_Vector2f> &uvs)
{
	_frames.resize (_frames.size() + 1);
	_frames.back().vertices.resize (verts.size());
	bool isBillboard = (_vertexFlags & MyMesh::ATOM_Billboard) != 0;

	if (verts.size())
	{
		if (_actions[actionindex].frames.size() > 0)
		{
			_frames.back().boundingbox = _frames[_actions[actionindex].frames.back()].boundingbox;
		}
		else
		{
			_frames.back().boundingbox.beginExtend ();
		}

		for (unsigned i = 0; i < verts.size(); ++i)
		{
			ATOM_Vector3f v = verts[i];
			float t = v.x;
			v.x = v.z;
			v.z = t;

			_frames.back().vertices[i] = v;
			_frames.back().boundingbox.extend (v);
		}
	}
	else
	{
		_frames.back().boundingbox.setMin (ATOM_Vector3f(0,0,0));
		_frames.back().boundingbox.setMax (ATOM_Vector3f(0,0,0));
	}

	_frames.back().uvs = uvs;
	_frames.back().transform = worldMatrix;

	_actions[actionindex].frames.push_back (_frames.size()-1);
}

int MyMesh::getNumVertices (void) const
{
	return _vertices.size ();
}

int MyMesh::getNumIndices (void) const
{
	return _indices.size();
}

const ATOM_Vector3f &MyMesh::getVertex (int index) const
{
	return _vertices[index].position;
}

const ATOM_Vector4f &MyMesh::getWeights (int index) const
{
	return _vertices[index].weights;
}

const i4 &MyMesh::getJointIndices (int index) const
{
	return _vertices[index].weightjoints;
}

const unsigned &MyMesh::getColor (int index) const
{
	return _vertices[index].color;
}

const ATOM_Vector3f &MyMesh::getNormal (int index) const
{
	return _vertices[index].normal;
}

const ATOM_Vector3f &MyMesh::getTangent (int index) const
{
	return _vertices[index].tangent;
}

const ATOM_Vector3f &MyMesh::getBinormal (int index) const
{
	return _vertices[index].binormal;
}

const unsigned short & MyMesh::getIndex (int index) const
{
	return _indices[index];
}

const ATOM_Vector2f &MyMesh::getTexCoord (int index) const
{
	return _vertices[index].texcoords;
}

int MyMesh::getVertexFlags (void) const
{
	return _vertexFlags;
}

const material &MyMesh::getMaterial (void) const
{
	return _material;
}

void MyMesh::setNumVertices (int n)
{
	_vertices.resize (n);
}

void MyMesh::setVertex (int index, const ATOM_Vector3f &v)
{
	_vertices[index].position = v;
}

void MyMesh::setWeights (int index, const ATOM_Vector4f &v)
{
	_vertices[index].weights = v;
}

void MyMesh::setJointIndices (int index, const i4 &indices)
{
	_vertices[index].weightjoints = indices;
}

void MyMesh::setVertex (int index, float x, float y, float z)
{
	_vertices[index].position.set(x, y, z);
}

void MyMesh::setColor (int index, const ATOM_Vector4f &c)
{
	ATOM_ColorARGB color(c.x, c.y, c.z, c.w);
	_vertices[index].color = color;
}

void MyMesh::setTexCoord (int index, const ATOM_Vector2f &v)
{
	_vertices[index].texcoords = v;
}

void MyMesh::setTexCoord (int index, float u, float v)
{
	_vertices[index].texcoords.set(u, v);
}

material &MyMesh::getMaterial (void)
{
	return _material;
}

void MyMesh::setVertexFlags (int flags)
{
	_vertexFlags = flags;
}

struct StripV
{
	unsigned origin_index;
	unsigned new_index;
	MyMesh::Vertex vertex;

	std::vector<ATOM_Vector3f> frameVertices;
	std::vector<ATOM_Vector2f> frameUVs;
};

bool MyMesh::vertexEqual (const StripV &v1, const StripV &v2) const
{
	if (v1.vertex.position != v2.vertex.position)
	{
		return false;
	}

	if (v1.vertex.normal != v2.vertex.normal)
	{
		return false;
	}

	if (v1.vertex.texcoords != v2.vertex.texcoords)
	{
		return false;
	}

	if ((_vertexFlags & MyMesh::HasColors) != 0)
	{
		if (v1.vertex.color != v2.vertex.color)
		{
			return false;
		}
	}

	if (getSkeleton ())
	{
		if (v1.vertex.weights != v2.vertex.weights)
		{
			return false;
		}

		if (v1.vertex.weightjoints != v2.vertex.weightjoints)
		{
			return false;
		}
	}

	return true;
}

struct IndexCompare
{
	bool operator () (const StripV &v1, const StripV &v2) const
	{
		return v1.origin_index < v2.origin_index;
	}
};

struct VertexCompare
{
	bool _hasColor;
	bool _hasWeights;

	VertexCompare (bool hasColor, bool hasWeights): _hasColor(hasColor), _hasWeights(hasWeights) {}

	bool operator () (const StripV &v1, const StripV &v2) const
	{
		assert(v1.frameVertices.size() == v2.frameVertices.size());

		if (v1.vertex.position.x < v2.vertex.position.x)
		{
			return true;      
		}
		else if (v1.vertex.position.x > v2.vertex.position.x)
		{
			return false;      
		}

		if (v1.vertex.position.y < v2.vertex.position.y)
		{
			return true;      
		}
		else if (v1.vertex.position.y > v2.vertex.position.y)
		{
			return false;      
		}

		if (v1.vertex.position.z < v2.vertex.position.z)
		{
			return true;      
		}
		else if (v1.vertex.position.z > v2.vertex.position.z)
		{
			return false;      
		}

		if (v1.vertex.normal.x < v2.vertex.normal.x)
		{
			return true;      
		}
		else if (v1.vertex.normal.x > v2.vertex.normal.x)
		{
			return false;      
		}

		if (v1.vertex.normal.y < v2.vertex.normal.y)
		{
			return true;      
		}
		else if (v1.vertex.normal.y > v2.vertex.normal.y)
		{
			return false;      
		}

		if (v1.vertex.normal.z < v2.vertex.normal.z)
		{
			return true;      
		}
		else if (v1.vertex.normal.z > v2.vertex.normal.z)
		{
			return false;      
		}

		if (v1.vertex.tangent.x < v2.vertex.tangent.x)
		{
			return true;      
		}
		else if (v1.vertex.tangent.x > v2.vertex.tangent.x)
		{
			return false;      
		}

		if (v1.vertex.tangent.y < v2.vertex.tangent.y)
		{
			return true;      
		}
		else if (v1.vertex.tangent.y > v2.vertex.tangent.y)
		{
			return false;      
		}

		if (v1.vertex.tangent.z < v2.vertex.tangent.z)
		{
			return true;      
		}
		else if (v1.vertex.tangent.z > v2.vertex.tangent.z)
		{
			return false;      
		}

		if (_hasColor)
		{
			if (v1.vertex.color < v2.vertex.color)
			{
				return true;      
			}
			else if (v1.vertex.color > v2.vertex.color)
			{
				return false;      
			}
		}

		if (_hasWeights)
		{
			if (v1.vertex.weights.x < v2.vertex.weights.x)
			{
				return true;
			}
			else if (v1.vertex.weights.x > v2.vertex.weights.x)
			{
				return false;
			}
			if (v1.vertex.weights.y < v2.vertex.weights.y)
			{
				return true;
			}
			else if (v1.vertex.weights.y > v2.vertex.weights.y)
			{
				return false;
			}
			if (v1.vertex.weights.z < v2.vertex.weights.z)
			{
				return true;
			}
			else if (v1.vertex.weights.z > v2.vertex.weights.z)
			{
				return false;
			}
			if (v1.vertex.weights.w < v2.vertex.weights.w)
			{
				return true;
			}
			else if (v1.vertex.weights.w > v2.vertex.weights.w)
			{
				return false;
			}

			if (v1.vertex.weightjoints.x < v2.vertex.weightjoints.x)
			{
				return true;
			}
			else if (v1.vertex.weightjoints.x > v2.vertex.weightjoints.x)
			{
				return false;
			}
			if (v1.vertex.weightjoints.y < v2.vertex.weightjoints.y)
			{
				return true;
			}
			else if (v1.vertex.weightjoints.y > v2.vertex.weightjoints.y)
			{
				return false;
			}
			if (v1.vertex.weightjoints.z < v2.vertex.weightjoints.z)
			{
				return true;
			}
			else if (v1.vertex.weightjoints.z > v2.vertex.weightjoints.z)
			{
				return false;
			}
			if (v1.vertex.weightjoints.w < v2.vertex.weightjoints.w)
			{
				return true;
			}
			else if (v1.vertex.weightjoints.w > v2.vertex.weightjoints.w)
			{
				return false;
			}
		}

		if (v1.vertex.texcoords.x < v2.vertex.texcoords.x)
		{
			return true;
		}
		else if (v1.vertex.texcoords.x > v2.vertex.texcoords.x)
		{
			return false;      
		}

		if (v1.vertex.texcoords.y < v2.vertex.texcoords.y)
		{
			return true;
		}
		else if (v1.vertex.texcoords.y > v2.vertex.texcoords.y)
		{
			return false;      
		}

		return false;
	}
};

void MyMesh::alarm (const char *format, ...)
{
	char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 1024, format, args);
    va_end(args);
	std::cout << buffer << std::endl;
}

bool MyMesh::genIndices (void)
{
#if 0
	_indices.resize (_vertices.size());
	for (unsigned i = 0; i < _indices.size(); ++i)
	{
		_indices[i] = i;
	}
	return true;
#else
	unsigned count = _vertices.size ();

	if (!count)
	{
		alarm ("警告：模型%s的面片数为0，已忽略！", _name.c_str());
		return false;
	}

	std::vector<StripV> v(count);
	for (unsigned i = 0; i < v.size(); ++i)
	{
		v[i].vertex = _vertices[i];
		v[i].origin_index = i;
		for (unsigned n = 0; n < _frames.size(); ++n)
		{
			if (_frames[n].vertices.size() > 0)
			{
				assert(_frames[n].vertices.size() == count);
				v[i].frameVertices.push_back (_frames[n].vertices[i]);
			}
			if (_frames[n].uvs.size() > 0)
			{
				assert(_frames[n].uvs.size() == count);
				v[i].frameUVs.push_back (_frames[n].uvs[i]);
			}
		}
	}

	std::sort (v.begin(), v.end(), VertexCompare((_vertexFlags & MyMesh::HasColors) != 0, getSkeleton() != 0));

	unsigned max_index = 0;
	v[0].new_index = max_index;
	_vertices[max_index] = v[max_index].vertex;
	for (unsigned f = 0; f < _frames.size(); ++f)
	{
		if (!_frames[f].vertices.empty ())
		{
			_frames[f].vertices[max_index] = v[max_index].frameVertices[f];
		}

		if (!_frames[f].uvs.empty())
		{
			_frames[f].uvs[max_index] = v[max_index].frameUVs[f];
		}
	}

	for (unsigned i = 1; i < v.size(); ++i)
	{
		if (!vertexEqual (v[i], v[i-1]))
		{
			++max_index;
			if (max_index > 0xFFFF)
			{
				alarm ("警告：模型%s的索引数大于65536，已忽略！", _name.c_str());
				return false;
			}
			_vertices[max_index] = v[i].vertex;
			for (unsigned f = 0; f < _frames.size(); ++f)
			{
				if (!_frames[f].vertices.empty ())
				{
					_frames[f].vertices[max_index] = v[i].frameVertices[f];
				}

				if (!_frames[f].uvs.empty ())
				{
					_frames[f].uvs[max_index] = v[i].frameUVs[f];
				}
			}
		}
		v[i].new_index = max_index;
	}

	std::sort (v.begin(), v.end (), IndexCompare());

	_indices.resize (v.size ());

	for (unsigned i = 0; i < v.size(); ++i)
	{
		_indices[i] = v[i].new_index;
	}

	_vertices.resize (max_index + 1);
	for (unsigned f = 0; f < _frames.size(); ++f)
	{
		if (!_frames[f].vertices.empty ())
		{
			_frames[f].vertices.resize (max_index + 1);
		}
		_frames[f].uvs.resize (max_index + 1);
	}

	return true;
#endif
}

#if 0
bool MyMesh::computeNormals (int smoothAngle)
{
	std::vector<unsigned> indices(_indices.size());
	std::vector<unsigned> remap;
	std::vector<ATOM_MeshMender::Vertex> verts;

	for (unsigned i = 0; i < _indices.size(); ++i)
	{
		indices[i] = _indices[i];
	}

	ATOM_MeshMender::Vertex inv;
	bool nochannel = _material.normalmapChannel == -1;
	bool hascolor = (_vertexFlags & MyMesh::HasColors) != 0;
	bool hasweights = getSkeleton() != 0;

    for (unsigned i = 0; i < _vertices.size(); ++i)
    {
		inv.pos = _vertices[i].position;
		inv.s = nochannel ? 0.f : _vertices[i].texcoords.x;
		inv.t = nochannel ? 0.f : _vertices[i].texcoords.y;
        verts.push_back (inv);
    }

	const float minNormalCreaseCos = cosf (smoothAngle * 3.1416f / 180.f);
	const float minTangentCreaseCos = minNormalCreaseCos;
	const float minBinormalCreaseCos = minNormalCreaseCos;
	const float weightNormalsByArea = 0.5f;

	ATOM_MeshMender mender;
	mender.Mend (verts, 
			  indices, 
			  remap, 
			  minNormalCreaseCos, 
			  minTangentCreaseCos, 
			  minBinormalCreaseCos, 
			  weightNormalsByArea, 
			  ATOM_MeshMender::CALCULATE_NORMALS,
			  ATOM_MeshMender::DONT_RESPECT_SPLITS,
			  ATOM_MeshMender::DONT_FIX_CYLINDRICAL);

	if (_indices.size() != indices.size())
	{
		alarm ("警告：模型%s计算法线得到不同的索引值，这是个BUG，请报告开发者！此模型已忽略", _name.c_str());
		return false;
	}

	for (unsigned i = 0; i < _indices.size(); ++i)
	{
		_indices[i] = indices[i];
	}

	std::vector<MyMesh::Vertex> newVertices; 

	for (unsigned i = 0; i < verts.size(); ++i)
	{
		MyMesh::Vertex v;
		v.position = verts[i].pos;
		v.normal = verts[i].normal;

		if (!nochannel)
		{
			v.tangent = verts[i].tangent;
			v.binormal = verts[i].binormal;
			if (v.tangent == ATOM_Vector3f(0, 0, 0)) v.tangent.set (1, 0, 0);
			if (v.binormal == ATOM_Vector3f(0, 0, 0)) v.binormal.set (0, 0, 1);

			v.texcoords.x = verts[i].s;
			v.texcoords.y = verts[i].t;
		}
		else
		{
			v.texcoords = _vertices[remap[i]].texcoords;
		}

		if (hascolor)
		{
			v.color = _vertices[remap[i]].color;
		}

		if (hasweights)
		{
			v.weights = _vertices[remap[i]].weights;
			v.weightjoints = _vertices[remap[i]].weightjoints;
		}

		newVertices.push_back (v);
	}

	for (unsigned frame = 0; frame < this->_frames.size(); ++frame)
	{
		std::vector<ATOM_Vector3f> newFrameVerts(newVertices.size());
		std::vector<ATOM_Vector2f> newFrameUVs(newVertices.size());

		if (!_frames[frame].vertices.empty ())
		{
			for (unsigned v = 0; v < newFrameVerts.size(); ++v)
			{
				newFrameVerts[v] = _frames[frame].vertices[remap[v]];
			}
			_frames[frame].vertices = newFrameVerts;
		}

		if (!_frames[frame].uvs.empty ())
		{
			for (unsigned v = 0; v < newFrameVerts.size(); ++v)
			{
				newFrameUVs[v] = _frames[frame].uvs[remap[v]];
			}
			_frames[frame].uvs = newFrameUVs;
		}
	}

	if (nochannel)
	{
		_vertexFlags &= ~MyMesh::HasTangents;
	}
	else
	{
		_vertexFlags |= MyMesh::HasTangents;
	}

	_vertexFlags |= MyMesh::HasNormals;
	_vertices = newVertices;

	return true;
}

#endif

void MyMesh::optimizeFaces (void)
{
	std::vector<unsigned short> indicesOpt(_indices.size());
	optimizeFacesLRU (_indices.size() / 3, &_indices[0], &indicesOpt[0], 24);
	_indices = indicesOpt;

	unsigned oldVertexCount = _vertices.size();
	std::vector<unsigned> vertexRemap(oldVertexCount);
	unsigned numVertices = optimizeVertices (_indices.size() / 3, &_indices[0], &vertexRemap[0]);
	remapIndexArray (&_indices[0], &vertexRemap[0], _indices.size());

	remapVertexArray (&_vertices[0], sizeof(Vertex), _vertices.size(), &vertexRemap[0]);
	_vertices.resize (numVertices);

	for (unsigned i = 0; i < _frames.size(); ++i)
	{
		if (!_frames[i].vertices.empty ())
		{
			remapVertexArray (&(_frames[i].vertices[0]), sizeof(ATOM_Vector3f), oldVertexCount, &vertexRemap[0]);
			_frames[i].vertices.resize (numVertices);
		}

		if (!_frames[i].uvs.empty ())
		{
			remapVertexArray (&(_frames[i].uvs[0]), sizeof(ATOM_Vector2f), oldVertexCount, &vertexRemap[0]);
			_frames[i].uvs.resize (numVertices);
		}
	}

}

void MyMesh::optimizeJoints (ATOM_Skeleton *skeleton)
{
	const std::vector<int> &remap = skeleton->getRemapOldToNew ();

	for (unsigned i = 0; i < _vertices.size(); ++i)
	{
		Vertex &v = _vertices[i];

		v.weightjoints.x = remap[v.weightjoints.x];
		v.weightjoints.y = remap[v.weightjoints.y];
		v.weightjoints.z = remap[v.weightjoints.z];
		v.weightjoints.w = remap[v.weightjoints.w];
	}
}

bool MyMesh::process (VertexAnimationInfo *animationInfo, ATOM_Skeleton *skeleton)
{
	bool vertexAnimation = false;
	bool uvAnimation = false;
	bool jointAnimation = false;
	bool materialAnimation = false;

	int a = 0;

	for (int a = 0; a < _actions.size(); ++a)
	{
		Action &action = _actions[a];
		const std::vector<int> &frames = _actions[a].frames;
 
		if (frames.size () > 1)
		{
			for (int i = 0; i < frames.size()-1; ++i)
			{
				_frames[frames[i]].boundingbox = _frames[frames.back()].boundingbox;
			}
		}

		if (frames.size() > 0)
		{
			const unsigned cmpsize = _vertices.size() * sizeof(ATOM_Vector3f);
			action.vertexAnimation = false;
			if (_frames.size() > 0 && _frames[0].vertices.size() > 0)
			{
				for (unsigned n = 1; n < frames.size(); ++n)
				{
					if (memcmp (&(_frames[frames[0]].vertices[0]), &(_frames[frames[n]].vertices[0]), cmpsize))
					{
						action.vertexAnimation = true;
						vertexAnimation = true;
						break;
					}
				}
				if (!action.vertexAnimation)
				{
					for (unsigned n = 1; n < frames.size(); ++n)
					{
						if (_frames[frames[0]].transform != _frames[frames[n]].transform)
						{
							action.vertexAnimation = true;
							vertexAnimation = true;
							break;
						}
					}
				}
			}

			const unsigned cmpsizeuv = _vertices.size() * sizeof(ATOM_Vector2f);
			action.uvAnimation = false;
			if (_frames.size() > 0 && _frames[0].uvs.size() > 0)
			{
				for (unsigned n = 1; n < frames.size(); ++n)
				{
					if (memcmp (&(_frames[frames[0]].uvs[0]), &(_frames[frames[n]].uvs[0]), cmpsizeuv))
					{
						action.uvAnimation = true;
						uvAnimation = true;
						break;
					}
				}
			}

			action.materialAnimation = false;
			if (_materialFrames.size() > 0)
			{
				for (unsigned n = 1; n < frames.size(); ++n)
				{
					if (_materialFrames[frames[0]].transparency != _materialFrames[frames[n]].transparency)
					{
						action.materialAnimation = true;
						_material.transparency = 0.f; // force to smooth alpha blending
						break;
					}
				}
			}

			action.jointAnimation = _skeletonFrames.size() > 0 && _skeletonFrames[0].jointMatrices.size() > 0;
			if (action.jointAnimation)
			{
				jointAnimation = true;
			}
		}
		else
		{
			action.vertexAnimation = false;
			action.uvAnimation = false;
			action.jointAnimation = false;
			action.materialAnimation = false;
		}
	}

	if (materialAnimation)
	{
		_materialFrames.clear ();
	}

	//if (!vertexAnimation && !uvAnimation)
	//{
	//	_frames.clear ();
	//}
	//else if (!vertexAnimation)
	//{
	//	for (unsigned n = 0; n < _frames.size(); ++n)
	//	{
	//		_frames[n].vertices.clear ();
	//	}
	//}
	//else if (!uvAnimation)
	//{
	//	for (unsigned n = 0; n < _frames.size(); ++n)
	//	{
	//		_frames[n].uvs.clear ();
	//	}
	//}

	if (!jointAnimation)
	{
		_skeletonFrames.clear ();
	}

	if (!genIndices ())
	{
		std::cout << "Generate mesh vertex indices failed." << std::endl;
		return false;
	}

#if 0
	if (!computeNormals (smoothAngle))
	{
		std::cout << "Compute mesh normals failed." << std::endl;
		return false;
	}
#else
	_vertexFlags |= MyMesh::HasNormals;
	_vertexFlags |= MyMesh::HasTangents;
#endif

	optimizeFaces ();

	if (_useSkeleton && skeleton)
	{
		optimizeJoints (skeleton);
	}

	return true;
}

const MyMesh::Vertex &MyMesh::getSuperVertex (int index) const
{
	return _vertices[index];
}

void MyMesh::setSuperVertex (int index, const MyMesh::Vertex &v)
{
	_vertices[index] = v;
}

void MyMesh::pushSuperVertex (const MyMesh::Vertex &v)
{
	_vertices.push_back (v);
}

void MyMesh::addProperty (const char *key, const char *value)
{
	_properties[key] = value;
}

const std::map<std::string, std::string> &MyMesh::getProperties (void) const
{
	return _properties;
}

void MyMesh::useSkeleton (bool b)
{
	_useSkeleton = b;
}

bool MyMesh::usingSkeleton (void) const
{
	return _useSkeleton;
}


