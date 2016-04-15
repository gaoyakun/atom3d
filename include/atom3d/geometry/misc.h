#ifndef __ATOM_GEOMETRY_MISC_H
#define __ATOM_GEOMETRY_MISC_H

#include <vector>

#include "../ATOM_utils.h"
#include "../ATOM_math.h"

bool calc_tangents (std::vector<ATOM_Vector3f> &positions, 
                    std::vector<ATOM_Vector2f> &texcoord,
                    std::vector<unsigned short> &index,
                    std::vector<ATOM_Vector3f> &normals,
                    std::vector<ATOM_Vector3f> &tangents,
                    std::vector<ATOM_Vector3f> &binormals);

bool tri_indexer (std::vector<ATOM_Vector3f> &positions, 
                  std::vector<ATOM_Vector2f> &texcoord, 
                  std::vector<unsigned short> &indices);

class ATOM_CubemapGenerator
{
public:
	ATOM_CubemapGenerator (void);
	virtual ~ATOM_CubemapGenerator (void);
	virtual void generate (unsigned size);
public:
	ATOM_ColorARGB *getFace (unsigned i) { return _faces[i]; }
protected:
	virtual ATOM_ColorARGB generateSample (int face, float s, float t) = 0;
private:
	ATOM_ColorARGB *_faces[6];
};

class ATOM_NormalizationCubemapGenerator: public ATOM_CubemapGenerator
{
protected:
	virtual ATOM_ColorARGB generateSample (int face, float s, float t);
};

class ATOM_SimpleDiffuseCubemapGenerator: public ATOM_CubemapGenerator
{
public:
	void setLightDir (const ATOM_Vector3f &lightdir) { _lightdir = lightdir; }
	void setLightColor (const ATOM_Vector3f &lightcolor) { _lightcolor = lightcolor; }
	void setAmbientColor (const ATOM_Vector3f &ambientcolor) { _ambientcolor = ambientcolor; }

protected:
	virtual ATOM_ColorARGB generateSample (int face, float s, float t);

protected:
	ATOM_Vector3f _lightdir;
	ATOM_Vector3f _lightcolor;
	ATOM_Vector3f _ambientcolor;
};

class ATOM_DiffuseCubemapGenerator: public ATOM_CubemapGenerator
{
public:
	ATOM_DiffuseCubemapGenerator (void);
	virtual ~ATOM_DiffuseCubemapGenerator (void);
public:
	void setEnvmaps (unsigned size, ATOM_ColorARGB *envmap[6], ATOM_ColorARGB *normalmap[6], ATOM_ColorARGB *dirmap[6]);
	void setDirectionLight (const ATOM_Vector3f &lightDir, ATOM_ColorARGB lightColor);
protected:
	virtual ATOM_ColorARGB generateSample (int face, float s, float t);
private:
	unsigned _size;
	ATOM_Vector3f *_envmaps[6];
	ATOM_Vector3f *_normalmaps[6];
	ATOM_Vector3f *_dirmaps[6];
	ATOM_Vector3f _lightDir;
	ATOM_Vector3f _lightColor;
};

#endif // __ATOM_GEOMETRY_MISC_H
