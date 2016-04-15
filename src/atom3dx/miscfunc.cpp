#include "StdAfx.h"
#include "basedefs.h"
#include "axisnode.h"
#include "trackballnode.h"
#include "skeleton_visualizer.h"
#include "ATOM3DX.h"

ATOMX_API void ATOMX_CALL ATOMX_RegisterObjects (void)
{
	ATOM_RegisterType (ATOMX_AxisNode::_classname(), &ATOMX_AxisNode::_create, &ATOMX_AxisNode::_destroy, &ATOMX_AxisNode::_purge, ATOMX_AxisNode::_get_script_interface(), false);
	ATOM_RegisterType (ATOMX_TrackBallNode::_classname(), &ATOMX_TrackBallNode::_create, &ATOMX_TrackBallNode::_destroy, &ATOMX_TrackBallNode::_purge, ATOMX_TrackBallNode::_get_script_interface(), false);
	ATOM_RegisterType (ATOMX_SkeletonVisualizerNode::_classname(), &ATOMX_SkeletonVisualizerNode::_create, &ATOMX_SkeletonVisualizerNode::_destroy, &ATOMX_SkeletonVisualizerNode::_purge, ATOMX_SkeletonVisualizerNode::_get_script_interface(), false);
}

class FindNodeByNameVisitor: public ATOM_Visitor
{
private:
	ATOM_Node *_node;
	ATOM_STRING _name;
	bool _useClassName;

public:
	FindNodeByNameVisitor (const char *name, bool useClassName): _node(0), _name(name), _useClassName(useClassName) {}

public:
	virtual void visit (ATOM_Node &node)
	{
		const char *str = _useClassName ? node.getClassName() : node.getObjectName();
		if (!strcmp (str, _name.c_str()))
		{
			_node = &node;
			cancelTraverse ();
		}
	}

	ATOM_Node *getResult (void) const
	{
		return _node;
	}
};

ATOMX_API ATOM_Node * ATOMX_CALL ATOMX_FindFirstNodeByClassName (ATOM_Node *rootNode, const char *classname)
{
	if (!rootNode || !classname)
	{
		return 0;
	}

	FindNodeByNameVisitor v(classname, true);

	v.traverse (*rootNode);

	return v.getResult ();
}

ATOMX_API ATOM_Node * ATOMX_CALL ATOMX_FindFirstNodeByObjectName (ATOM_Node *rootNode, const char *objectname)
{
	if (!rootNode || !objectname)
	{
		return 0;
	}

	FindNodeByNameVisitor v(objectname, false);

	v.traverse (*rootNode);

	return v.getResult ();
}

ATOMX_API bool ATOMX_CALL ATOMX_ReadFile (const char *filename, bool text, ATOM_VECTOR<char> &buffer)
{
	if (!filename)
	{
		return false;
	}

	int mode = ATOM_VFS::read;
	if (text)
	{
		mode |= ATOM_VFS::text;
	}

	ATOM_AutoFile file(filename, mode);
	if (!file)
	{
		return 0;
	}

	unsigned size = file->size ();
	if (text)
	{
		size++;
	}

	buffer.resize (text ? size+1 : size);

	unsigned sizeRead = file->read (&buffer[0], size);

	if (!text && sizeRead != size)
	{
		return false;
	}

	if (text)
	{
		buffer[sizeRead] = '\0';
	}

	return true;
}

ATOMX_API void ATOMX_CALL ATOMX_CreateCylinder (const ATOM_Vector3f &center, bool withCaps, int smoothLevel, const ATOM_Vector3f &axis, float radius, float height, ATOM_Vector3f *vertexbuffer, unsigned short *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned short indexOffset)
{
	if (smoothLevel < 3)
	{
		smoothLevel = 3;
	}
	unsigned numVerts = smoothLevel * 2;
	unsigned numTris = smoothLevel * 2;
	if (withCaps)
	{
		numTris += 2 * (smoothLevel - 2);
	}

	if (numVertices)
	{
		*numVertices = numVerts;
	}

	if (numIndices)
	{
		*numIndices = numTris * 3;
	}

	if (!vertexbuffer && !indexbuffer)
	{
		return;
	}

	float theta = ATOM_TwoPi / float(smoothLevel);

	ATOM_Vector3f axisY = axis;
	axisY.normalize ();

	ATOM_Vector3f temp(0.f, 0.f, 0.f);
	int idx = 0;
	if (ATOM_abs(axisY.xyz[1]) < ATOM_abs(axisY.xyz[idx]))
	{
		idx = 1;
	}
	if (ATOM_abs(axisY.xyz[2]) < ATOM_abs(axisY.xyz[idx]))
	{
		idx = 2;
	}
	temp.xyz[idx] = 1.f;

	ATOM_Vector3f axisX = crossProduct (axisY, temp);

	if (vertexbuffer)
	{
		for (unsigned i = 0; i < smoothLevel; ++i)
		{
			ATOM_Matrix4x4f rotMatrix = ATOM_Matrix4x4f::getRotateMatrixAngleNormalizedAxis (-(i * theta), axisY);
			ATOM_Vector3f point = rotMatrix >> axisX;
			point *= radius;
			*vertexbuffer++ = center + point;
			*vertexbuffer++ = center + point + axisY * height;
		}
	}

	if (indexbuffer)
	{
		for (unsigned i = 0; i < smoothLevel * 2; ++i)
		{
			*indexbuffer++ = (((i + 1) / 2) * 2) % (smoothLevel * 2) + indexOffset;
			*indexbuffer++ = ((i / 2) * 2) + 1 + indexOffset;
			*indexbuffer++ = (i + 2) % (smoothLevel * 2) + indexOffset;
		}

		if (withCaps)
		{
			for (unsigned i = 0; i < smoothLevel - 2; ++i)
			{
				*indexbuffer++ = 0 + indexOffset;
				*indexbuffer++ = (i + 1) * 2 + indexOffset;
				*indexbuffer++ = (i + 2) * 2 + indexOffset;
			}
			for (unsigned i = 0; i < smoothLevel - 2; ++i)
			{
				*indexbuffer++ = (i + 1) * 2 + 1 + indexOffset;
				*indexbuffer++ = 1 + indexOffset;
				*indexbuffer++ = (i + 2) * 2 + 1 + indexOffset;
			}
		}
	}
}

ATOMX_API void ATOMX_CALL ATOMX_CreateCone (const ATOM_Vector3f &center, bool withCap, int smoothLevel, const ATOM_Vector3f &axis, float radius, float height, ATOM_Vector3f *vertexbuffer, unsigned short *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned short indexOffset)
{
	if (smoothLevel < 3)
	{
		smoothLevel = 3;
	}

	unsigned numVerts = smoothLevel + 1;
	unsigned numTris = smoothLevel;
	if (withCap)
	{
		numTris += smoothLevel - 2;
	}

	if (numVertices)
	{
		*numVertices = numVerts;
	}
	if (numIndices)
	{
		*numIndices = 3 * numTris;
	}

	if (!vertexbuffer && !indexbuffer)
	{
		return;
	}

	float theta = ATOM_TwoPi / float(smoothLevel);

	ATOM_Vector3f axisY = axis;
	axisY.normalize ();

	ATOM_Vector3f temp(0.f, 0.f, 0.f);
	int idx = 0;
	if (ATOM_abs(axisY.xyz[1]) < ATOM_abs(axisY.xyz[idx]))
	{
		idx = 1;
	}
	if (ATOM_abs(axisY.xyz[2]) < ATOM_abs(axisY.xyz[idx]))
	{
		idx = 2;
	}
	temp.xyz[idx] = 1.f;

	ATOM_Vector3f axisX = crossProduct (axisY, temp);

	if (vertexbuffer)
	{
		for (unsigned i = 0; i < smoothLevel; ++i)
		{
			ATOM_Matrix4x4f rotMatrix = ATOM_Matrix4x4f::getRotateMatrixAngleNormalizedAxis ((i * theta), axisY);
			ATOM_Vector3f point = rotMatrix >> axisX;
			point *= radius;
			*vertexbuffer++ = center + point;
		}
		*vertexbuffer++ = center + axisY * height;
	}

	if (indexbuffer)
	{
		for (unsigned i = 0; i < smoothLevel; ++i)
		{
			*indexbuffer++ = smoothLevel + indexOffset;
			*indexbuffer++ = i + indexOffset;
			*indexbuffer++ = ((i + 1) % smoothLevel) + indexOffset;
		}

		if (withCap)
		{
			for (unsigned i = 0; i < smoothLevel - 2; ++i)
			{
				*indexbuffer++ = i + 1 + indexOffset;
				*indexbuffer++ = 0 + indexOffset;
				*indexbuffer++ = ((i + 2) % smoothLevel) + indexOffset;
			}
		}
	}
}

ATOMX_API void ATOMX_CALL ATOMX_CreateSphere (const ATOM_Vector3f &center, int numRings, int numSegments, float radius, ATOM_Vector3f *vertexbuffer, unsigned short *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned short indexOffset)
{
	if (numRings < 2)
	{
		numRings = 2;
	}

	if (numSegments < 3)
	{
		numSegments = 3;
	}

	if (numVertices)
	{
		*numVertices = (numRings + 1) * (numSegments + 1);
	}

	if (numIndices)
	{
		*numIndices = 2 * numRings * (numSegments + 1);
	}

	if (!vertexbuffer && !indexbuffer)
	{
		return;
	}

	float fDeltaRingAngle = ( ATOM_Pi / numRings );
	float fDeltaSegAngle = ( ATOM_TwoPi / numSegments );

	unsigned short wVerticeIndex = 0 ; 

	// Generate the group of rings for the sphere
	for( int ring = 0; ring < numRings + 1 ; ring++ )
	{
		float r0, y0;
		ATOM_sincos (ring * fDeltaRingAngle, &r0, &y0);

		// Generate the group of segments for the current ring
		for( int seg = 0; seg < numSegments + 1 ; seg++ )
		{
			float x0, z0;
			ATOM_sincos (seg * fDeltaSegAngle, &x0, &z0);
			x0 *= r0;
			z0 *= r0;

			// Add one vertices to the strip which makes up the sphere
			if (vertexbuffer)
			{
				vertexbuffer->x = center.x + x0 * radius;
				vertexbuffer->y = center.y + y0 * radius;
				vertexbuffer->z = center.z + z0 * radius;
				vertexbuffer++;
			}

			// add two indices except for last ring 
			if (indexbuffer && ring != numRings ) 
			{
				*indexbuffer++ = wVerticeIndex + indexOffset; 
				*indexbuffer++ = wVerticeIndex + (unsigned short)(numSegments + 1) + indexOffset; 
				wVerticeIndex++ ; 
			}
		} // end for seg 
	} // end for ring 
}

ATOMX_API void ATOMX_CALL ATOMX_CreateBox (const ATOM_Vector3f &center, const ATOM_Vector3f &size, ATOM_Vector3f *vertexbuffer, unsigned short *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned short indexOffset)
{
}

ATOMX_API void ATOMX_CALL ATOMX_CreateCircle (const ATOM_Vector3f &center, int smoothLevel, float radius, const ATOM_Vector3f &axis, ATOM_Vector3f *vertexbuffer, unsigned short *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned short indexOffset)
{
	if (smoothLevel < 3)
	{
		smoothLevel = 3;
	}

	if (numVertices)
	{
		*numVertices = smoothLevel;
	}

	if (numIndices)
	{
		*numIndices = smoothLevel * 2;
	}

	if (!vertexbuffer && !indexbuffer)
	{
		return;
	}

	float theta = ATOM_TwoPi / float(smoothLevel);

	ATOM_Vector3f axisY = axis;
	axisY.normalize ();

	ATOM_Vector3f temp(0.f, 0.f, 0.f);
	int idx = 0;
	if (ATOM_abs(axisY.xyz[1]) < ATOM_abs(axisY.xyz[idx]))
	{
		idx = 1;
	}
	if (ATOM_abs(axisY.xyz[2]) < ATOM_abs(axisY.xyz[idx]))
	{
		idx = 2;
	}
	temp.xyz[idx] = 1.f;
	ATOM_Vector3f axisX = crossProduct (axisY, temp);

	for (unsigned i = 0; i < smoothLevel; ++i)
	{
		ATOM_Matrix4x4f rotMatrix = ATOM_Matrix4x4f::getRotateMatrixAngleNormalizedAxis ((i * theta), axisY);
		ATOM_Vector3f point = rotMatrix >> axisX;
		point *= radius;

		if (vertexbuffer)
		{
			*vertexbuffer++ = center + point;
		}

		if (indexbuffer)
		{
			*indexbuffer++ = indexOffset + i;
			*indexbuffer++ = indexOffset + ((i + 1) % smoothLevel);
		}
	}
}

ATOMX_API float ATOMX_CALL ATOMX_MeasureScreenDistance (ATOM_Camera *camera, const ATOM_Vector3f &locationWorld, float distanceWorld)
{
	ATOM_Vector3f cameraPos = camera->getPosition ();
	float length = (cameraPos - locationWorld).getLength ();

	// adjust width for easy pick
	float screenH = camera->getViewport().size.h * 0.5f;
	return distanceWorld * screenH *camera->getInvTanHalfFovy() / length;
}

static ATOM_STRING identifyFileName (const char *filename)
{
	char buffer[ATOM_VFS::max_filename_length] = { 0 };
	ATOM_CompletePath (filename, buffer);
	strlwr (buffer);
	return buffer;
}

static void insertFileToMap (ATOM_SET<ATOM_STRING> &filenameMap, const char *filename)
{
	if (filename && filename[0])
	{
		filenameMap.insert (identifyFileName (filename));
	}
}

void ATOMX_ReferenceCookie::insert (const char *filename)
{
	insertFileToMap (_filenameSet, filename);
}

bool ATOMX_ReferenceCookie::test (const char *filename)
{
	ATOM_STRING s = identifyFileName (filename);
	return _filenameSet.find (s) != _filenameSet.end ();
}

static void _GetReferencedFileListOfUI (ATOM_TiXmlElement *e, ATOM_SET<ATOM_STRING> &filenameMap)
{
	for (ATOM_TiXmlElement *eImageList = e->FirstChildElement ("imagelist"); eImageList; eImageList = eImageList->NextSiblingElement("imagelist"))
	{
		for (ATOM_TiXmlElement *eImage = eImageList->FirstChildElement ("image"); eImage; eImage = eImage->NextSiblingElement("image"))
		{
			int id = 0;
			if (eImage->QueryIntAttribute ("id", &id) != ATOM_TIXML_SUCCESS)
			{
				continue;
			}
			int type = 0;
			if (eImage->QueryIntAttribute ("type", &type) != ATOM_TIXML_SUCCESS)
			{
				continue;
			}

			int color = 0;
			const char *texNormal = 0;
			const char *texHover = 0;
			const char *texHold = 0;
			const char *texDisabled = 0;
			const char *regionStr = 0;
			const char *matNormal = 0;
			const char *matHover = 0;
			const char *matHold = 0;
			const char *matDisabled = 0;
			ATOM_Rect2Di region (0,0,0,0);

			switch (type)
			{
			case ATOM_GUIImage::IT_MATERIAL:
				insertFileToMap (filenameMap, eImage->Attribute("normal"));
				insertFileToMap (filenameMap, eImage->Attribute("hover"));
				insertFileToMap (filenameMap, eImage->Attribute("hold"));
				insertFileToMap (filenameMap, eImage->Attribute("disabled"));
				insertFileToMap (filenameMap, eImage->Attribute("mat_normal"));
				insertFileToMap (filenameMap, eImage->Attribute("mat_hover"));
				insertFileToMap (filenameMap, eImage->Attribute("mat_hold"));
				insertFileToMap (filenameMap, eImage->Attribute("mat_disabled"));
				break;
			case ATOM_GUIImage::IT_IMAGE:
			case ATOM_GUIImage::IT_IMAGE9:
				insertFileToMap (filenameMap, eImage->Attribute("normal"));
				insertFileToMap (filenameMap, eImage->Attribute("hover"));
				insertFileToMap (filenameMap, eImage->Attribute("hold"));
				insertFileToMap (filenameMap, eImage->Attribute("disabled"));
				break;
			default:
				break;
			}
		}
	}
	for (ATOM_TiXmlElement *child = e->FirstChildElement("component"); child; child = child->NextSiblingElement("component"))
	{
		_GetReferencedFileListOfUI (child, filenameMap);
	}
}

static int _parseArrayDim (const char *type, const char *str)
{
	size_t len = strlen (str);
	size_t type_len = strlen (type);

	if (len > type_len+2)
	{
		if (!memcmp (type, str, type_len) && str[type_len] == '[')
		{
			const char *start = str + type_len + 1;
			const char *end = strchr (start, ']');
			if (end)
			{
				ATOM_STRING s(start, end - start);
				return atoi (s.c_str());
			}
		}
	}
	return 0;
}

static ATOM_MaterialParam::Type _parseTypeString (const char *str, int &dim)
{
	dim = 1;
	if (!strcmp (str, "float"))
	{
		return ATOM_MaterialParam::ParamType_Float;
	}
	else if (!strcmp (str, "float4") || !strcmp(str, "float3") || !strcmp(str, "float2"))
	{
		return ATOM_MaterialParam::ParamType_Vector;
	}
	else if (!strcmp (str, "float4x4"))
	{
		return ATOM_MaterialParam::ParamType_Matrix44;
	}
	else if (!strcmp (str, "float4x3"))
	{
		return ATOM_MaterialParam::ParamType_Matrix43;
	}
	else if (!strcmp (str, "texture"))
	{
		return ATOM_MaterialParam::ParamType_Texture;
	}
	else if (!strcmp (str, "sampler") || !strcmp (str, "sampler2D") || !strcmp (str, "sampler3D") || !strcmp (str, "samplerCUBE"))
	{
		return ATOM_MaterialParam::ParamType_Sampler;
	}
	else if (!strcmp (str, "int"))
	{
		return ATOM_MaterialParam::ParamType_Int;
	}
	else 
	{
		dim = _parseArrayDim ("float", str);
		if (dim)
		{
			return ATOM_MaterialParam::ParamType_FloatArray;
		}
		dim = _parseArrayDim ("float4", str);
		if (dim)
		{
			return ATOM_MaterialParam::ParamType_VectorArray;
		}
		dim = _parseArrayDim ("float3", str);
		if (dim)
		{
			return ATOM_MaterialParam::ParamType_VectorArray;
		}
		dim = _parseArrayDim ("float2", str);
		if (dim)
		{
			return ATOM_MaterialParam::ParamType_VectorArray;
		}
		dim = _parseArrayDim ("float4x4", str);
		if (dim)
		{
			return ATOM_MaterialParam::ParamType_Matrix44Array;
		}
		dim = _parseArrayDim ("float4x3", str);
		if (dim)
		{
			return ATOM_MaterialParam::ParamType_Matrix43Array;
		}
		dim = _parseArrayDim ("int", str);
		if (dim)
		{
			return ATOM_MaterialParam::ParamType_IntArray;
		}
		dim = 0;
		return ATOM_MaterialParam::ParamType_Unknown;
	}
}

struct ParamInfo
{
	ATOM_MaterialParam::Type type;
	int dimension;
};

static ATOM_MAP<ATOM_STRING, ATOM_MAP<ATOM_STRING, ParamInfo> > _matNameMap;

static const ATOM_MAP<ATOM_STRING, ParamInfo> &_getTextureParameterList (const char *coreMaterialFileName)
{
	ATOM_STRING s = identifyFileName (coreMaterialFileName);
	auto it = _matNameMap.find (s);
	if (it == _matNameMap.end ())
	{
		ATOM_MAP<ATOM_STRING, ParamInfo> &nameMap = _matNameMap[s];

		ATOM_AutoFile f(coreMaterialFileName, ATOM_VFS::read|ATOM_VFS::text);
		if (f)
		{
			ATOM_VECTOR<char> file_content(f->size()+1);
			char *str = &file_content[0];
			unsigned n = f->read (str, f->size());
			str[n] = '\0';

			ATOM_TiXmlDocument doc;
			doc.Parse (str);
			if (!doc.Error ())
			{
				ATOM_TiXmlElement *root = doc.RootElement ();
				if (root && !stricmp(root->Value(), "material"))
				{
					for (ATOM_TiXmlElement *param = root->FirstChildElement("param"); param; param = param->NextSiblingElement("param"))
					{
						const char *type = param->Attribute ("type");
						const char *name = param->Attribute ("name");
						if (type && name)
						{
							ParamInfo info;
							info.type = _parseTypeString (type, info.dimension);
							if (type != ATOM_MaterialParam::ParamType_Unknown)
							{
								nameMap[name] = info;
							}
						}
					}
					for (ATOM_TiXmlElement *includeElement = root->FirstChildElement("include"); includeElement; includeElement = includeElement->NextSiblingElement("include"))
					{
						const char *filename = includeElement->Attribute ("filename");
						if (filename)
						{
							const ATOM_MAP<ATOM_STRING, ParamInfo> &includeNameMap = _getTextureParameterList (filename);
							for (ATOM_MAP<ATOM_STRING, ParamInfo>::const_iterator it = includeNameMap.begin(); it != includeNameMap.end(); ++it)
							{
								nameMap[it->first] = it->second;
							}
						}
					}
				}
			}
		}
		return nameMap;
	}
	else
	{
		return it->second;
	}
}

ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfNM (const char *modelFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	if (cookie)
	{
		if (cookie->test (modelFilename))
		{
			return;
		}
		else
		{
			cookie->insert (modelFilename);
		}
	}

	ATOM_HARDREF(ATOM_Model) model;
	if (model->load (0, modelFilename))
	{
		for (unsigned i = 0; i < model->getNumMeshes(); ++i)
		{
			ATOM_STRING diffuseTexture = model->getMesh(i)->diffuseTextureName;
			if( !diffuseTexture.empty() )
			{
				insertFileToMap (fileList, diffuseTexture.c_str());
			}
		}
	}
}

ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfMat (const char *matFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	if (cookie)
	{
		if (cookie->test (matFilename))
		{
			return;
		}
		else
		{
			cookie->insert (matFilename);
		}
	}

	ATOM_AutoFile fp(matFilename, ATOM_VFS::read|ATOM_VFS::text);
	if (!fp)
	{
		return;
	}

	unsigned size = fp->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = fp->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument xmldoc;
	xmldoc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (xmldoc.Error ())
	{
		return;
	}

	ATOM_TiXmlElement *xmlroot = xmldoc.RootElement ();
	if (!xmlroot)
	{
		return;
	}

	if (strcmp (xmlroot->Value(), "material"))
	{
		return;
	}

	for (ATOM_TiXmlElement *includeElement = xmlroot->FirstChildElement("include"); includeElement; includeElement = includeElement->NextSiblingElement("include"))
	{
		const char *filename = includeElement->Attribute ("filename");
		if (filename)
		{
			insertFileToMap (fileList, filename);
			ATOMX_GetReferencedFileListOfMat (filename, fileList, cookie);
		}
	}
}

ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfNM2 (const char *modelFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	if (cookie)
	{
		if (cookie->test (modelFilename))
		{
			return;
		}
		else
		{
			cookie->insert (modelFilename);
		}
	}


	ATOM_ChunkIO chunkIO;
	if (!chunkIO.load (modelFilename))
	{
		return;
	}
	ATOM_ChunkIO::Chunk *topChunk = chunkIO.getTopChunk();
	if (!topChunk)
	{
		return;
	}
	ATOM_ChunkIO::Chunk *mainChunk = topChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','m','e'));
	if (!mainChunk)
	{
		return;
	}
	ATOM_ChunkIO::Chunk *meshChunk = mainChunk->getFirstChild (ATOM_MAKE_FOURCC('a','t','s','m'));
	while (meshChunk)
	{
		ATOM_ChunkIO::Chunk *materialChunk = meshChunk->getFirstChild (ATOM_MAKE_FOURCC('m','a','t','l'));
		if (materialChunk)
		{
			int size = materialChunk->getChunkStream()->readCString (0);
			materialChunk->getChunkStream()->seek (-size);
			char buffer[ATOM_VFS::max_filename_length];
			materialChunk->getChunkStream()->readCString (buffer);
			insertFileToMap (fileList, buffer);

			ATOMX_GetReferencedFileListOfMat (buffer, fileList, cookie);
			const ATOM_MAP<ATOM_STRING, ParamInfo> &paramList = _getTextureParameterList (buffer);

			for (;;)
			{
				int size = materialChunk->getChunkStream()->readCString (0);
				materialChunk->getChunkStream()->seek (-size);
				materialChunk->getChunkStream()->readCString (buffer);
				if (buffer[0] == '\0')
				{
					break;
				}
				ATOM_MAP<ATOM_STRING, ParamInfo>::const_iterator it = paramList.find (buffer);
				if (it == paramList.end ())
				{
					continue;
				}
				switch (it->second.type)
				{
				case ATOM_MaterialParam::ParamType_Float:
					{
						materialChunk->getChunkStream()->readFloat ();
						break;
					}
				case ATOM_MaterialParam::ParamType_FloatArray:
					{
						for (int i = 0; i < it->second.dimension; ++i)
						{
							materialChunk->getChunkStream()->readFloat();
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Int:
					{
						materialChunk->getChunkStream()->readInt ();
						break;
					}
				case ATOM_MaterialParam::ParamType_IntArray:
					{
						for (int i = 0; i < it->second.dimension; ++i)
						{
							materialChunk->getChunkStream()->readInt ();
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Vector:
					{
						ATOM_Vector4f v;
						materialChunk->getChunkStream()->read (&v, sizeof(ATOM_Vector4f));
						break;
					}
				case ATOM_MaterialParam::ParamType_VectorArray:
					{
						ATOM_Vector4f v;
						for (int i = 0; i < it->second.dimension; ++i)
						{
							materialChunk->getChunkStream()->read (&v, sizeof(ATOM_Vector4f));
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Matrix44:
					{
						ATOM_Matrix4x4f m;
						materialChunk->getChunkStream()->read (&m, sizeof(ATOM_Matrix4x4f));
						break;
					}
				case ATOM_MaterialParam::ParamType_Matrix44Array:
					{
						ATOM_Matrix4x4f m;
						for (int i = 0; i < it->second.dimension; ++i)
						{
							materialChunk->getChunkStream()->read (&m, sizeof(ATOM_Matrix4x4f));
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Matrix43:
					{
						ATOM_Matrix3x4f m;
						materialChunk->getChunkStream()->read (&m, sizeof(ATOM_Matrix3x4f));
						break;
					}
				case ATOM_MaterialParam::ParamType_Matrix43Array:
					{
						ATOM_Matrix3x4f m;
						for (int i = 0; i < it->second.dimension; ++i)
						{
							materialChunk->getChunkStream()->read (&m, sizeof(ATOM_Matrix3x4f));
						}
						break;
					}
				case ATOM_MaterialParam::ParamType_Texture:
					{
						int size = materialChunk->getChunkStream()->readCString(0);
						materialChunk->getChunkStream()->seek (-size);
						materialChunk->getChunkStream()->readCString (buffer);
						insertFileToMap (fileList, buffer);
						break;
					}
				}
			}
		}
		meshChunk = meshChunk->getNextSibling (ATOM_MAKE_FOURCC('a','t','s','m'));
	}
}

ATOMX_API void ATOMX_CALL ATOMX_StoreFileName (const char *filename, ATOM_SET<ATOM_STRING> &fileList)
{
	if (filename)
	{
		insertFileToMap (fileList, filename);
	}
}

ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileList (const char *filename, bool nonAsyncOnly, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	if (filename)
	{
		const char *ext = strrchr (filename, '.');
		if (!ext)
		{
			ext = "";
		}

		if (!stricmp (ext, ".ui"))
		{
			ATOMX_GetReferencedFileListOfUI (filename, fileList, cookie);
		}
		else if (!stricmp (ext, ".nm2"))
		{
			ATOMX_GetReferencedFileListOfNM2 (filename, fileList,cookie);
		}
		else if (!stricmp (ext, ".nm"))
		{
			ATOMX_GetReferencedFileListOfNM (filename, fileList, cookie);
		}
		else if (!stricmp (ext, ".cps"))
		{
			ATOMX_GetReferencedFileListOfCPS (filename, fileList, cookie);
		}
		else if (!stricmp (ext, ".csp"))
		{
			ATOMX_GetReferencedFileListOfCSP (filename, fileList, cookie);
		}
		else if (!stricmp (ext, ".cp"))
		{
			ATOMX_GetReferencedFileListOfCP (filename, fileList, cookie);
		}
		else if (!stricmp (ext, ".ccp"))
		{
			ATOMX_GetReferencedFileListOfCCP (filename, fileList, cookie);
		}
		else if (!stricmp (ext, ".3sg"))
		{
			ATOMX_GetReferencedFileListOf3SG (filename, nonAsyncOnly, fileList, cookie);
		}
		else if (!stricmp (ext, ".mat"))
		{
			ATOMX_GetReferencedFileListOfMat (filename, fileList, cookie);
		}
	}
}

ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfUI (const char *uiFilename, ATOM_SET<ATOM_STRING> &filenameMap, ATOMX_ReferenceCookie *cookie)
{
	if (cookie)
	{
		if (cookie->test (uiFilename))
		{
			return;
		}
		else
		{
			cookie->insert (uiFilename);
		}
	}


	ATOM_AutoFile f(uiFilename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return;
	}

	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error ())
	{
		return;
	}

	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
	{
		return;
	}

	if (strcmp (root->Value(), "UI"))
	{
		return;
	}

	_GetReferencedFileListOfUI (root, filenameMap);
}

static void _processCPSActor (ATOM_TiXmlElement *rootActorElement, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	for (ATOM_TiXmlElement *actorElement = rootActorElement->FirstChildElement("Actor"); actorElement; actorElement = actorElement->NextSiblingElement("Actor"))
	{
		const char *type = actorElement->Attribute ("ClassType");
		int ref = 0;
		actorElement->Attribute ("Ref", &ref);
		if (ref)
		{
			continue;
		}
		if (!stricmp (type, ATOM_Geode::_classname()))
		{
			const char *modelFileName = actorElement->Attribute ("ModelFileName");
			if (modelFileName)
			{
				insertFileToMap (fileList, modelFileName);
				const char *ext = strrchr (modelFileName, '.');
				if (ext)
				{
					if (!stricmp (ext, ".nm"))
					{
						ATOMX_GetReferencedFileListOfNM (modelFileName, fileList, cookie);
					}
					else if (!stricmp (ext, ".nm2"))
					{
						ATOMX_GetReferencedFileListOfNM2 (modelFileName, fileList, cookie);
					}
				}
			}
		}
		else if (!stricmp (type, ATOM_ShapeNode::_classname()))
		{
			const char *modelFileName = actorElement->Attribute ("ModelFile");
			if (modelFileName)
			{
				insertFileToMap (fileList, modelFileName);
				const char *ext = strrchr (modelFileName, '.');
				if (ext)
				{
					if (!stricmp (ext, ".nm"))
					{
						ATOMX_GetReferencedFileListOfNM (modelFileName, fileList, cookie);
					}
					else if (!stricmp (ext, ".nm2"))
					{
						ATOMX_GetReferencedFileListOfNM2 (modelFileName, fileList, cookie);
					}
				}
			}
			const char *materialFile = actorElement->Attribute ("Material");
			if (materialFile)
			{
				insertFileToMap (fileList, materialFile);
				ATOMX_GetReferencedFileListOfMat (materialFile, fileList, cookie);
			}
			else
			{
				insertFileToMap (fileList, "/materials/builtin/shape.mat");
				ATOMX_GetReferencedFileListOfMat ("/materials/builtin/shape.mat", fileList, cookie);
			}
			for (ATOM_TiXmlElement *paramElement = actorElement->FirstChildElement("Param"); paramElement; paramElement = paramElement->NextSiblingElement("Param"))
			{
				const char *type = paramElement->Attribute ("type");
				if (type && !stricmp (type, "texture"))
				{
					const char *textureFileName = paramElement->Attribute ("value");
					if (textureFileName)
					{
						insertFileToMap (fileList, textureFileName);
					}
				}
			}
		}
		else if (!stricmp (type, ATOM_ParticleSystem::_classname()))
		{
			const char *particletexture = actorElement->Attribute ("ParticleTexture");
			if (particletexture)
			{
				insertFileToMap (fileList, particletexture);
			}
			const char *colortexture = actorElement->Attribute ("ColorTexture");
			if (colortexture)
			{
				insertFileToMap (fileList, colortexture);
			}
			insertFileToMap (fileList, "/materials/builtin/particlesys.mat");
		}
		else if (!stricmp (type, ATOM_WeaponTrail::_classname()))
		{
			const char *trailtexture = actorElement->Attribute ("TrailTexture");
			if (trailtexture)
			{
				insertFileToMap (fileList, trailtexture);
			}
			insertFileToMap (fileList, "/materials/builtin/weapontrail.mat");
		}
		else if (!stricmp (type, ATOM_Decal::_classname()))
		{
			const char *texturefile = actorElement->Attribute ("TextureFileName");
			if (texturefile)
			{
				insertFileToMap (fileList, texturefile);
			}
			insertFileToMap (fileList, "/materials/builtin/decal.mat");
		}

		_processCPSActor (actorElement, fileList, cookie);
	}
}

ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCPS (const char *cpsFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	if (cookie)
	{
		if (cookie->test (cpsFilename))
		{
			return;
		}
		else
		{
			cookie->insert (cpsFilename);
		}
	}


	ATOM_AutoFile f(cpsFilename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return;
	}

	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error ())
	{
		return;
	}

	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
	{
		return;
	}

	if (strcmp (root->Value(), "Composition"))
	{
		return;
	}

	_processCPSActor (root, fileList, cookie);
}

ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCSP (const char *cspFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	if (cookie)
	{
		if (cookie->test (cspFilename))
		{
			return;
		}
		else
		{
			cookie->insert (cspFilename);
		}
	}

	ATOM_AutoFile fp(cspFilename, ATOM_VFS::read|ATOM_VFS::text);
	if (!fp)
	{
		return;
	}

	unsigned size = fp->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = fp->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument xmldoc;
	xmldoc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (xmldoc.Error ())
	{
		return;
	}

	ATOM_TiXmlElement *xmlroot = xmldoc.RootElement ();
	if (!xmlroot)
	{
		return;
	}

	if (strcmp (xmlroot->Value(), "node"))
	{
		return;
	}

	const char *modelFileName = xmlroot->Attribute ("ModelFileName");
	if (modelFileName)
	{
		insertFileToMap (fileList, modelFileName);
		const char *ext = strrchr (modelFileName, '.');
		if (ext)
		{
			if (!stricmp (ext, ".nm"))
			{
				ATOMX_GetReferencedFileListOfNM (modelFileName, fileList, cookie);
			}
			else if (!stricmp (ext, ".nm2"))
			{
				ATOMX_GetReferencedFileListOfNM2 (modelFileName, fileList, cookie);
			}
		}
	}

	for (ATOM_TiXmlElement *matElement = xmlroot->FirstChildElement("mat"); matElement; matElement = matElement->NextSiblingElement ("mat"))
	{
		const char *matFileName = matElement->Attribute ("material");
		if (matFileName)
		{
			insertFileToMap (fileList, matFileName);

			if (cookie)
			{
				if (cookie->test (matFileName))
				{
					continue;
				}
				else
				{
					cookie->insert (matFileName);
				}
			}

			ATOM_AutoFile matFile(matFileName, ATOM_VFS::read|ATOM_VFS::text);
			if (matFile)
			{
				unsigned size = matFile->size ();
				char *p = ATOM_NEW_ARRAY(char, size);
				size = matFile->read (p, size);
				p[size] = '\0';
				ATOM_TiXmlDocument docmat;
				docmat.Parse (p);
				ATOM_DELETE_ARRAY(p);
				if (docmat.Error ())
				{
					continue;
				}

				ATOM_TiXmlElement *matroot = docmat.RootElement ();
				if (!matroot)
				{
					continue;
				}

				if (strcmp (matroot->Value(), "material"))
				{
					continue;
				}

				const char *coremat = matroot->Attribute ("core");
				if (coremat)
				{
					insertFileToMap (fileList, coremat);
				}

				for (ATOM_TiXmlElement *paramElement = matroot->FirstChildElement ("Param"); paramElement; paramElement = paramElement->NextSiblingElement("Param"))
				{
					const char *type = paramElement->Attribute ("type");
					const char *value = paramElement->Attribute ("value");
					if (type && value)
					{
						if (!strcmp (type, "texture"))
						{
							insertFileToMap (fileList, value);
						}
					}
				}
			}
		}
	}
}

ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCCP (const char *ccpFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	if (cookie)
	{
		if (cookie->test (ccpFilename))
		{
			return;
		}
		else
		{
			cookie->insert (ccpFilename);
		}
	}

	ATOM_AutoFile fp(ccpFilename, ATOM_VFS::read|ATOM_VFS::text);
	if (!fp)
	{
		return;
	}

	unsigned size = fp->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = fp->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument xmldoc;
	xmldoc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (xmldoc.Error ())
	{
		return;
	}

	ATOM_TiXmlElement *xmlroot = xmldoc.RootElement ();
	if (!xmlroot)
	{
		return;
	}

	if (strcmp (xmlroot->Value(), "root"))
	{
		return;
	}

	const char *skeletonFileName = xmlroot->Attribute ("skeleton");
	if (skeletonFileName)
	{
		insertFileToMap (fileList, skeletonFileName);
		const char *ext = strrchr (skeletonFileName, '.');
		if (!stricmp (ext, ".nm"))
		{
			ATOMX_GetReferencedFileListOfNM (skeletonFileName, fileList, cookie);
		}
		else if (!stricmp (ext, ".nm2"))
		{
			ATOMX_GetReferencedFileListOfNM2 (skeletonFileName, fileList, cookie);
		}
	}

	for (ATOM_TiXmlElement *actionElement = xmlroot->FirstChildElement("action"); actionElement; actionElement=actionElement->NextSiblingElement("action"))
	{
		const char *filename = actionElement->Attribute ("file");
		if (filename)
		{
			insertFileToMap (fileList, filename);
			const char *ext = strrchr (filename, '.');
			if (!stricmp (ext, ".nm"))
			{
				ATOMX_GetReferencedFileListOfNM (filename, fileList, cookie);
			}
			else if (!stricmp (ext, ".nm2"))
			{
				ATOMX_GetReferencedFileListOfNM2 (filename, fileList, cookie);
			}
		}
	}

	for (ATOM_TiXmlElement *componentElement = xmlroot->FirstChildElement ("component"); componentElement; componentElement = componentElement->NextSiblingElement("component"))
	{
		for (ATOM_TiXmlElement *meshElement = componentElement->FirstChildElement("mesh"); meshElement; meshElement=meshElement->NextSiblingElement ("mesh"))
		{
			const char *filename = meshElement->Attribute ("file");
			if (filename)
			{
				insertFileToMap (fileList, filename);
				const char *ext = strrchr (filename, '.');
				if (!stricmp (ext, ".nm"))
				{
					ATOMX_GetReferencedFileListOfNM (filename, fileList, cookie);
				}
				else if (!stricmp (ext, ".nm2"))
				{
					ATOMX_GetReferencedFileListOfNM2 (filename, fileList, cookie);
				}
			}
		}
	}
}

ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCP (const char *cpFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	if (cookie)
	{
		if (cookie->test (cpFilename))
		{
			return;
		}
		else
		{
			cookie->insert (cpFilename);
		}
	}

	ATOM_AutoFile fp(cpFilename, ATOM_VFS::read|ATOM_VFS::text);
	if (!fp)
	{
		return;
	}

	unsigned size = fp->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = fp->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument xmldoc;
	xmldoc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (xmldoc.Error ())
	{
		return;
	}

	ATOM_TiXmlElement *xmlroot = xmldoc.RootElement ();
	if (!xmlroot)
	{
		return;
	}

	if (strcmp (xmlroot->Value(), "node"))
	{
		return;
	}

	const char *coreFileName = xmlroot->Attribute ("CoreFile");
	if (coreFileName)
	{
		insertFileToMap (fileList, coreFileName);
		ATOMX_GetReferencedFileListOfCCP (coreFileName, fileList, cookie);
	}
}

ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfTerrain (const char *terrainFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	if (cookie)
	{
		if (cookie->test (terrainFilename))
		{
			return;
		}
		else
		{
			cookie->insert (terrainFilename);
		}
	}

	ATOM_AutoFile fp(terrainFilename, ATOM_VFS::read|ATOM_VFS::text);
	if (!fp)
	{
		return;
	}

	unsigned size = fp->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = fp->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument xmldoc;
	xmldoc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (xmldoc.Error ())
	{
		return;
	}

	ATOM_TiXmlElement *xmlroot = xmldoc.RootElement ();
	if (!xmlroot)
	{
		return;
	}

	if (strcmp (xmlroot->Value(), "Terrain"))
	{
		return;
	}

	const char *heightFileName = xmlroot->Attribute ("HeightMap");
	if (heightFileName)
	{
		insertFileToMap (fileList, heightFileName);
	}

	ATOM_TiXmlElement *detailsElement = xmlroot->FirstChildElement ("Details");
	if (detailsElement)
	{
		for (ATOM_TiXmlElement *detailElement = detailsElement->FirstChildElement("Detail"); detailElement; detailElement = detailElement->NextSiblingElement("Detail"))
		{
			const char *detailFileName = detailElement->Attribute ("FileName");
			if (detailFileName)
			{
				insertFileToMap (fileList, detailFileName);
			}
		}
	}

	ATOM_TiXmlElement *alphasElement = xmlroot->FirstChildElement ("Alphas");
	if (alphasElement)
	{
		for (ATOM_TiXmlElement *alphaElement = alphasElement->FirstChildElement("Alpha"); alphaElement; alphaElement = alphaElement->NextSiblingElement("Alpha"))
		{
			const char *alphaFileName = alphaElement->Attribute ("FileName");
			if (alphaFileName)
			{
				insertFileToMap (fileList, alphaFileName);
			}
		}
	}

	ATOM_TiXmlElement *vegetationsElement = xmlroot->FirstChildElement ("Vegetations");
	if (vegetationsElement)
	{
		for (ATOM_TiXmlElement *vegetationElement = vegetationsElement->FirstChildElement("Vegetation"); vegetationElement; vegetationElement = vegetationElement->NextSiblingElement("Vegetation"))
		{
			const char *densityFileName = vegetationElement->Attribute ("DensityFileName");
			if (densityFileName)
			{
				insertFileToMap (fileList, densityFileName);
			}
			const char *textureFileName = vegetationElement->Attribute ("TextureFileName");
			if (textureFileName)
			{
				insertFileToMap (fileList, textureFileName);
			}
		}
	}
	insertFileToMap (fileList, "/materials/builtin/terrain.mat");
	ATOMX_GetReferencedFileListOfMat ("/materials/builtin/terrain.mat", fileList, cookie);
}

struct NodeLoadCallbackData
{
	ATOM_SET<ATOM_STRING> *fileList;
	ATOMX_ReferenceCookie *cookie;
	bool nonAsyncOnly;
};

ATOM_AUTOREF(ATOM_Node) DummyLoadNodeCallback (ATOM_RenderDevice *device, const ATOM_TiXmlElement *xml, bool bLoad, void *userData)
{
	NodeLoadCallbackData *data = (NodeLoadCallbackData*)userData;
	ATOM_SET<ATOM_STRING> &fileList = *data->fileList;
	ATOMX_ReferenceCookie *cookie = data->cookie;

	int asyncFlag = 1;
	xml->QueryIntAttribute ("AsyncLoad", &asyncFlag);
	bool async = (asyncFlag!=-1) && data->nonAsyncOnly;

	const char *classname = xml->Attribute ("class");
	if (classname)
	{
		if (!strcmp (classname, ATOM_Terrain::_classname()))
		{
			const char *terrainFileName = xml->Attribute ("FileName");
			if (terrainFileName)
			{
				insertFileToMap (fileList, terrainFileName);
				ATOMX_GetReferencedFileListOfTerrain (terrainFileName, fileList, cookie);
			}
		}
		else if (!strcmp (classname, ATOM_Sky::_classname()))
		{
			if (!async)
			{
				const char *cloudFileName = xml->Attribute("CloudTextureFileName");
				if (cloudFileName)
				{
					insertFileToMap (fileList, cloudFileName);
				}

				const char *skyFileName = xml->Attribute("SkyTextureFileName");
				if (skyFileName)
				{
					insertFileToMap (fileList, skyFileName);
				}
			}

			insertFileToMap (fileList, "/materials/builtin/sky.mat");
			ATOMX_GetReferencedFileListOfMat ("/materials/builtin/sky.mat", fileList, cookie);
		}
		else if (!strcmp (classname, ATOM_Geode::_classname()))
		{
			if (!async)
			{
				const char *modelFileName = xml->Attribute ("ModelFileName");
				if (modelFileName)
				{
					insertFileToMap (fileList, modelFileName);
					const char *ext = strrchr (modelFileName, '.');
					if (ext)
					{
						if (!stricmp (ext, ".nm"))
						{
							ATOMX_GetReferencedFileListOfNM (modelFileName, fileList, cookie);
						}
						else if (!stricmp (ext, ".nm2"))
						{
							ATOMX_GetReferencedFileListOfNM2 (modelFileName, fileList, cookie);
						}
					}
				}
			}
		}
		else if (!strcmp (classname, ATOM_CompositionNode::_classname()))
		{
			const char *cpsFileName = xml->Attribute ("CompositionFileName");
			if (cpsFileName)
			{
				if (!async)
				{
					insertFileToMap (fileList, cpsFileName);
					ATOMX_GetReferencedFileListOfCPS (cpsFileName, fileList, cookie);
				}
			}
		}
		else if (!strcmp (classname, ATOM_ShapeNode::_classname()))
		{
			const char *modelFileName = xml->Attribute ("ModelFile");
			if (!async && modelFileName)
			{
				insertFileToMap (fileList, modelFileName);
				const char *ext = strrchr (modelFileName, '.');
				if (ext)
				{
					if (!stricmp (ext, ".nm"))
					{
						ATOMX_GetReferencedFileListOfNM (modelFileName, fileList, cookie);
					}
					else if (!stricmp (ext, ".nm2"))
					{
						ATOMX_GetReferencedFileListOfNM2 (modelFileName, fileList, cookie);
					}
				}
			}
			const char *materialFile = xml->Attribute ("Material");
			if (materialFile)
			{
				insertFileToMap (fileList, materialFile);
				ATOMX_GetReferencedFileListOfMat (materialFile, fileList, cookie);
			}

			if (!async)
			{
				for (const ATOM_TiXmlElement *paramElement = xml->FirstChildElement("Param"); paramElement; paramElement=paramElement->NextSiblingElement("Param"))
				{
					const char *type = paramElement->Attribute ("type");
					if (type && !stricmp (type, "texture"))
					{
						const char *textureFileName = paramElement->Attribute ("value");
						if (textureFileName)
						{
							insertFileToMap (fileList, textureFileName);
						}
					}
				}
			}
		}
		else if (!strcmp (classname, ATOM_Decal::_classname()))
		{
			if (!async)
			{
				const char *textureFileName = xml->Attribute ("TextureFileName");
				if (textureFileName)
				{
					insertFileToMap (fileList, textureFileName);
				}
			}
			insertFileToMap (fileList, "/materials/builtin/decal.mat");
			ATOMX_GetReferencedFileListOfMat ("/materials/builtin/decal.mat", fileList, cookie);
		}
		else if (!strcmp (classname, ClientSimpleCharacter::_classname()))
		{
			if (!async)
			{
				const char *modelFileName = xml->Attribute ("ModelFileName");
				if (modelFileName)
				{
					insertFileToMap (fileList, modelFileName);
					const char *ext = strrchr (modelFileName, '.');
					if (ext)
					{
						if (!stricmp (ext, ".nm"))
						{
							ATOMX_GetReferencedFileListOfNM (modelFileName, fileList, cookie);
						}
						else if (!stricmp (ext, ".nm2"))
						{
							ATOMX_GetReferencedFileListOfNM2 (modelFileName, fileList, cookie);
						}
					}
				}
			}
		}
		else if (!strcmp (classname, ATOM_BkImage::_classname()))
		{
			if (!async)
			{
				const char *imageFileName = xml->Attribute ("ImageFile");
				if (imageFileName)
				{
					insertFileToMap (fileList, imageFileName);
				}
			}
		}
		else if (!strcmp (classname, ATOM_ParticleSystem::_classname()))
		{
			if (!async)
			{
				const char *particleTexture = xml->Attribute ("ParticleTexture");
				if (particleTexture)
				{
					insertFileToMap (fileList, particleTexture);
				}
				const char *colorTexture = xml->Attribute ("ColorTexture");
				if (colorTexture)
				{
					insertFileToMap (fileList, colorTexture);
				}
			}
			insertFileToMap (fileList, "/materials/builtin/particlesys.mat");
			ATOMX_GetReferencedFileListOfMat ("/materials/builtin/particlesys.mat", fileList, cookie);
		}
		else if (!strcmp (classname, ATOM_WeaponTrail::_classname()))
		{
			if (!async)
			{
				const char *trailTexture = xml->Attribute ("TrailTexture");
				if (trailTexture)
				{
					insertFileToMap (fileList, trailTexture);
				}
			}
			insertFileToMap (fileList, "/materials/builtin/weapontrail.mat");
			ATOMX_GetReferencedFileListOfMat ("/materials/builtin/weapontrail.mat", fileList, cookie);
		}
	}

	return nullptr;
}

ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOf3SG (const char *scnFilename, bool NonAsyncOnly, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	if (cookie)
	{
		if (cookie->test (scnFilename))
		{
			return;
		}
		else
		{
			cookie->insert (scnFilename);
		}
	}

	ATOMX_ReferenceCookie *c = cookie ? cookie : ATOM_NEW(ATOMX_ReferenceCookie);

	NodeLoadCallbackData data;
	data.cookie = c;
	data.fileList = &fileList;
	data.nonAsyncOnly = NonAsyncOnly;
	ATOM_SDLScene scene;
	scene.setLoadNodeCallback (&DummyLoadNodeCallback, &data);
	scene.load (scnFilename);
}

static void insertMaterialFile (ATOM_SET<ATOM_STRING> &fileList, const char *filename, ATOMX_ReferenceCookie *cookie)
{
	insertFileToMap (fileList, filename);
	ATOMX_GetReferencedFileListOfMat (filename, fileList, cookie);
}

ATOMX_API void ATOMX_CALL ATOMX_GetFileListInDirectory (const char *directory, bool recursive, bool addDepends, bool nonAsyncOnly, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	ATOM_VFS::finddata_t *finddata = ATOM_FindFirst (directory);
	if (!finddata)
	{
		return;
	}

	do
	{
		if (finddata->type == ATOM_VFS::directory)
		{
			if (recursive)
			{
				ATOMX_GetFileListInDirectory (finddata->filename, recursive, addDepends, nonAsyncOnly, fileList, cookie);
			}
		}
		else
		{
			insertFileToMap (fileList, finddata->filename);

			if (addDepends)
			{
				const char *ext = strrchr (finddata->filename, '.');
				if (!ext)
				{
					ext = "";
				}
				if (!stricmp (ext, ".ui"))
				{
					ATOMX_GetReferencedFileListOfUI (finddata->filename, fileList, cookie);
				}
				else if (!stricmp (ext, ".nm"))
				{
					ATOMX_GetReferencedFileListOfNM (finddata->filename, fileList, cookie);
				}
				else if (!stricmp (ext, ".nm2"))
				{
					ATOMX_GetReferencedFileListOfNM2 (finddata->filename, fileList, cookie);
				}
				else if (!stricmp (ext, ".cps"))
				{
					ATOMX_GetReferencedFileListOfCPS (finddata->filename, fileList, cookie);
				}
				else if (!stricmp (ext, ".csp"))
				{
					ATOMX_GetReferencedFileListOfCSP (finddata->filename, fileList, cookie);
				}
				else if (!stricmp (ext, ".cp"))
				{
					ATOMX_GetReferencedFileListOfCP (finddata->filename, fileList, cookie);
				}
				else if (!stricmp (ext, ".ccp"))
				{
					ATOMX_GetReferencedFileListOfCCP (finddata->filename, fileList, cookie);
				}
				else if (!stricmp (ext, ".3sg"))
				{
					ATOMX_GetReferencedFileListOf3SG (finddata->filename, nonAsyncOnly, fileList, cookie);
				}
				else if (!stricmp (ext, ".mat"))
				{
					ATOMX_GetReferencedFileListOfMat (finddata->filename, fileList, cookie);
				}
			}
		}
	}
	while (ATOM_FindNext (finddata));
	ATOM_CloseFind (finddata);
}

ATOMX_API void ATOMX_CALL ATOMX_GetPrerequiredFileList (ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie)
{
	ATOMX_GetFileListInDirectory ("/materials", true, true, true, fileList, cookie);
}

ATOMX_API bool ATOMX_CALL ATOMX_CopyFile (const char *sourceFile, const char *destDir, bool createDir)
{
	if (!sourceFile || sourceFile[0] != '/')
	{
		return false;
	}

	ATOM_PhysicVFS v;

	ATOM_STRING destFileName = destDir;
	if (destFileName.back() != '\\' && destFileName.back() != '/')
	{
		destFileName += '/';
	}

	if (createDir)
	{
		destFileName += sourceFile+1;
	}
	else
	{
		const char *p = strrchr (sourceFile, '/');
		destFileName += p+1;
	}

	char buffer[ATOM_VFS::max_filename_length];
	if (!v.identifyPath (destFileName.c_str(), buffer))
	{
		return false;
	}

	char *lastSep = strrchr (buffer, '\\');
	*lastSep = '\0';
	if (!v.isDir(buffer) && !v.mkDir (buffer))
	{
		return false;
	}
	*lastSep = '\\';

	char srcFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (sourceFile, srcFileName);

	if (!::CopyFileA (srcFileName, buffer, FALSE))
	{
		return false;
	}

	return true;
}

