#include <algorithm>

#include "misc.h"
#include "NVMeshMender.h"

struct StripV
{
  unsigned origin_index;
  unsigned new_index;
  ATOM_Vector3f pos;
  ATOM_Vector2f texcoord;
};

static inline bool compare_data (const StripV &v1, const StripV &v2)
{
  if (v1.pos.x < v2.pos.x)
  {
    return true;      
  }
  else if (v1.pos.x > v2.pos.x)
  {
    return false;      
  }

  if (v1.pos.y < v2.pos.y)
  {
    return true;      
  }
  else if (v1.pos.y > v2.pos.y)
  {
    return false;      
  }

  if (v1.pos.z < v2.pos.z)
  {
    return true;      
  }
  else if (v1.pos.z > v2.pos.z)
  {
    return false;      
  }

  if (v1.texcoord.x < v2.texcoord.x)
  {
    return true;
  }
  else if (v1.texcoord.x > v2.texcoord.x)
  {
    return false;      
  }

  if (v1.texcoord.y < v2.texcoord.y)
  {
    return true;
  }
  else if (v1.texcoord.y > v2.texcoord.y)
  {
    return false;      
  }

  return false;
}

static inline bool compare_index (const StripV &v1, const StripV &v2)
{
  return v1.origin_index < v2.origin_index;
}

bool calc_tangents (std::vector<ATOM_Vector3f> &positions, 
                    std::vector<ATOM_Vector2f> &texcoord,
                    std::vector<unsigned short> &index,
                    std::vector<ATOM_Vector3f> &normals,
                    std::vector<ATOM_Vector3f> &tangents,
                    std::vector<ATOM_Vector3f> &binormals)
{
  std::vector<unsigned> indices;
  std::vector<unsigned> remap;
  std::vector<ATOM_MeshMender::Vertex> verts;

  if (positions.size() != texcoord.size())
  {
    return false;
  }

  for (unsigned i = 0; i < index.size(); ++i)
  {
    indices.push_back (index[i]);
  }

  ATOM_MeshMender::Vertex inv;
  for (unsigned i = 0; i < positions.size(); ++i)
  {
    inv.pos = positions[i];
    inv.s = texcoord[i].x;
    inv.t = texcoord[i].y;
    verts.push_back (inv);
  }

  const float minNormalCreaseCos = 0.5f;
  const float minTangentCreaseCos = 0.4f;
  const float minBinormalCreaseCos = 0.4f;
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

  if (index.size() != indices.size())
  {
    return false;
  }

  for (unsigned i = 0; i < index.size(); ++i)
  {
    index[i] = indices[i];
  }

  positions.resize(0);
  texcoord.resize(0);
  normals.resize(0);
  tangents.resize(0);
  binormals.resize(0);

  for (unsigned i = 0; i < verts.size(); ++i)
  {
    positions.push_back (verts[i].pos);
    normals.push_back (verts[i].normal);
    texcoord.push_back (ATOM_Vector2f(verts[i].s, verts[i].t));
    tangents.push_back (verts[i].tangent);
    binormals.push_back (verts[i].binormal);
    if (tangents.back().x == 0 && tangents.back().y == 0 && tangents.back().z == 0)
    {
      tangents.back().set(1, 0, 0);
    }
    if (binormals.back().x == 0 && binormals.back().y == 0 && binormals.back().z == 0)
    {
      tangents.back().set(0, 0, 1);
    }
  }

  return true;
}

bool tri_indexer (std::vector<ATOM_Vector3f> &positions, 
                  std::vector<ATOM_Vector2f> &texcoord, 
                  std::vector<unsigned short> &indices)
{
  unsigned count = positions.size();

  if (count == 0 || count != texcoord.size())
  {
    return false;
  }

  std::vector<StripV> v(count);
  for (unsigned i = 0; i < v.size(); ++i)
  {
    v[i].pos = positions[i];
    v[i].texcoord = texcoord[i];
    v[i].origin_index = i;
  }

  std::sort (v.begin(), v.end(), &compare_data);

  unsigned max_index = 0;
  v[0].new_index = max_index;
  positions[max_index] = v[max_index].pos;
  texcoord[max_index] = v[max_index].texcoord;

  for (unsigned i = 1; i < v.size(); ++i)
  {
    if (v[i].pos != v[i-1].pos || v[i].texcoord != v[i-1].texcoord)
    {
      ++max_index;
      positions[max_index] = v[i].pos;
      texcoord[max_index] = v[i].texcoord;
    }
    v[i].new_index = max_index;
  }

  std::sort (v.begin(), v.end(), &compare_index);

  indices.resize(v.size());

  for (unsigned i = 0; i < v.size(); ++i)
  {
    indices[i] = v[i].new_index;
  }

  positions.resize(max_index+1);
  texcoord.resize(max_index+1);
  return true;
}

ATOM_CubemapGenerator::ATOM_CubemapGenerator (void)
{
	for (unsigned face = 0; face < 6; ++face) _faces[face] = 0;
}

ATOM_CubemapGenerator::~ATOM_CubemapGenerator (void)
{
	for (unsigned face = 0; face < 6; ++face) 
	{
		delete [] _faces[face];
	}
}

void ATOM_CubemapGenerator::generate (unsigned size)
{
	for (unsigned face = 0; face < 6; ++face) 
	{
		delete [] _faces[face];
		_faces[face] = new ATOM_ColorARGB[size * size];
	}

	float scalar = 2.f / float(size);
	for (unsigned face = 0; face < 6; ++face)
	{
		ATOM_ColorARGB *data = _faces[face];
		for (unsigned row = 0; row < size; ++row)
		{
			for (unsigned col = 0; col < size; ++col)
			{
				float s = col * scalar - 1.f;
				float t = row * scalar - 1.f;
				*data++ = generateSample (face, s, t);
			}
		}
	}
}

ATOM_ColorARGB ATOM_NormalizationCubemapGenerator::generateSample (int face, float s, float t)
{
	float nx, ny, nz;

	switch (face)
	{
	case 0:
		nx = 1.f;
		ny = -t;
		nz = -s;
		break;
	case 1:
		nx = -1.f;
		ny = -t;
		nz = s;
		break;
	case 2:
		nx = s;
		ny = 1.f;
		nz = t;
		break;
	case 3:
		nx = s;
		ny = -1.f;
		nz = -t;
		break;
	case 4:
		nx = s;
		ny = -t;
		nz = 1.f;
		break;
	case 5:
		nx = -s;
		ny = -t;
		nz = -1.f;
		break;
	default:
		return 0;
	}

	float l = 1.f / ATOM_sqrt(nx*nx + ny*ny + nz*nz);
	nx *= l;
	ny *= l;
	nz *= l;

	return ATOM_ColorARGB (nx*0.5f+0.5f, ny*0.5f+0.5f, nz*0.5f+0.5f, 1.f);
}

ATOM_ColorARGB ATOM_SimpleDiffuseCubemapGenerator::generateSample (int face, float s, float t)
{
	float nx, ny, nz;

	switch (face)
	{
	case 0:
		nx = 1.f;
		ny = -t;
		nz = -s;
		break;
	case 1:
		nx = -1.f;
		ny = -t;
		nz = s;
		break;
	case 2:
		nx = s;
		ny = 1.f;
		nz = t;
		break;
	case 3:
		nx = s;
		ny = -1.f;
		nz = -t;
		break;
	case 4:
		nx = s;
		ny = -t;
		nz = 1.f;
		break;
	case 5:
		nx = -s;
		ny = -t;
		nz = -1.f;
		break;
	default:
		return 0;
	}

	ATOM_Vector3f normal(nx, ny, nz);
	normal.normalize ();

	float lum = dotProduct (normal, _lightdir);
	if (lum < 0.f) lum = 0.f;

	ATOM_Vector3f c = _lightcolor * (_ambientcolor + ATOM_Vector3f(lum, lum, lum));
	if (c.x > 1.f) c.x = 1.f;
	if (c.y > 1.f) c.y = 1.f;
	if (c.z > 1.f) c.z = 1.f;

	return ATOM_ColorARGB (c.x, c.y, c.z, 1.f);
}

ATOM_DiffuseCubemapGenerator::ATOM_DiffuseCubemapGenerator (void)
{
	for (int i = 0; i < 6; ++i)
	{
		_envmaps[i] = 0;
		_normalmaps[i] = 0;
	}

	_size = 0;
	_lightColor.set(0.f, 0.f, 0.f);
	_lightDir.set(0.f, 1.f, 0.f);
}

ATOM_DiffuseCubemapGenerator::~ATOM_DiffuseCubemapGenerator (void)
{
	for (int i = 0; i < 6; ++i)
	{
		delete [] _envmaps[i];
		delete [] _normalmaps[i];
		delete [] _dirmaps[i];
	}
}

void ATOM_DiffuseCubemapGenerator::setEnvmaps (unsigned size, ATOM_ColorARGB *envmap[6], ATOM_ColorARGB *normalmap[6], ATOM_ColorARGB *dirmap[6])
{
	_size = size;
	float x, y, z, w;

	for (unsigned i = 0; i < 6; ++i)
	{
		_envmaps[i] = new ATOM_Vector3f[size * size];
		_normalmaps[i] = new ATOM_Vector3f[size * size];
		_dirmaps[i] = new ATOM_Vector3f[size * size];

		for (unsigned n = 0; n < size * size; ++n)
		{
			envmap[i][n].getFloats (&x, &y, &z, &w);
			_envmaps[i][n].set (x, y, z);

			normalmap[i][n].getFloats (&x, &y, &z, &w);
			_normalmaps[i][n].set (x * 2.f - 1.f, y * 2.f - 1.f, z * 2.f - 1.f);

			dirmap[i][n].getFloats (&x, &y, &z, &w);
			_dirmaps[i][n].set (x * 2.f - 1.f, y * 2.f - 1.f, z * 2.f - 1.f);
		}
	}
}

void ATOM_DiffuseCubemapGenerator::setDirectionLight (const ATOM_Vector3f &lightDir, ATOM_ColorARGB lightColor)
{
	float t;

	_lightDir = -lightDir;
	_lightDir.normalize ();
	lightColor.getFloats (&_lightColor.x, &_lightColor.y, &_lightColor.z, &t);
}

ATOM_ColorARGB ATOM_DiffuseCubemapGenerator::generateSample (int face, float s, float t)
{
	float nx, ny, nz;

	switch (face)
	{
	case 0:
		nx = 1.f;
		ny = -t;
		nz = -s;
		break;
	case 1:
		nx = -1.f;
		ny = -t;
		nz = s;
		break;
	case 2:
		nx = s;
		ny = 1.f;
		nz = t;
		break;
	case 3:
		nx = s;
		ny = -1.f;
		nz = -t;
		break;
	case 4:
		nx = s;
		ny = -t;
		nz = 1.f;
		break;
	case 5:
		nx = -s;
		ny = -t;
		nz = -1.f;
		break;
	default:
		return 0;
	}

	ATOM_Vector3f normal(nx, ny, nz);
	normal.normalize ();

	ATOM_Vector3f color(0.f, 0.f, 0.f);
	unsigned sampleCount = 0;
	ATOM_Vector3f sampleNormal;
	ATOM_Vector3f sampleDir;

	for (unsigned i = 0; i < 6; ++i)
	{
		const ATOM_Vector3f *colors = _envmaps[i];
		const ATOM_Vector3f *normals = _normalmaps[i];
		const ATOM_Vector3f *dirs = _dirmaps[i];

		for (unsigned n = 0; n < _size * _size; ++n)
		{
			if (dotProduct (dirs[n], normal) <= 0.f)
			{
				continue;
			}

			++sampleCount;
			float factor = -dotProduct (normals[n], dirs[n]);
			if (factor > 0.f)
			{
				color += colors[i] * factor;
			}
		}
	}

	color /= float(sampleCount);

	float dfactor = dotProduct (normal, _lightDir);
	if (dfactor > 0.f)
	{
		color += dfactor * _lightColor;
	}

	if (color.x > 1.f) color.x = 1.f;
	if (color.y > 1.f) color.y = 1.f;
	if (color.z > 1.f) color.z = 1.f;

	return ATOM_ColorARGB (color.x, color.y, color.z, 1.f);
}

