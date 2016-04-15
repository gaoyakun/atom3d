#include <ATOM.h>
#include "modelio.h"
#include "resource.h"

static const unsigned MeshChunkFourCC = ATOM_MAKE_FOURCC('M','E','S','H');
static const unsigned MeshChunkFourCC2 = ATOM_MAKE_FOURCC('M','E','S','2');
static const unsigned MeshChunkFourCC3 = ATOM_MAKE_FOURCC('M','E','S','3');
static const unsigned MeshChunkFourCC4 = ATOM_MAKE_FOURCC('M','E','S','4');
static const unsigned MeshChunkFourCC5 = ATOM_MAKE_FOURCC('M','E','S','5');
static const unsigned MeshChunkFourCC6 = ATOM_MAKE_FOURCC('M','E','S','6');
static const unsigned MeshChunkFourCC7 = ATOM_MAKE_FOURCC('M','E','S','7');
static const unsigned MaterialChunkFourCC = ATOM_MAKE_FOURCC('M','T','R','L');
static const unsigned MaterialChunkFourCC2 = ATOM_MAKE_FOURCC('M','T','R','2');
static const unsigned MaterialChunkFourCC3 = ATOM_MAKE_FOURCC('M','T','R','3');
static const unsigned MaterialChunkFourCC4 = ATOM_MAKE_FOURCC('M','T','R','4');
static const unsigned GeometryDataChunkFourCC = ATOM_MAKE_FOURCC('G','E','O','M');
static const unsigned MainChunkFourCC = ATOM_MAKE_FOURCC('N','3','M','F');
static const unsigned ATOM_BBoxChunkFourCC = ATOM_MAKE_FOURCC('B','B','O','X');
static const unsigned AABBTreeChunkFourCC = ATOM_MAKE_FOURCC('A', 'B', 'T', '0');
static const unsigned VertexWeightFourCC = ATOM_MAKE_FOURCC('W', 'G', 'H', 'T');
static const unsigned VertexWeightFourCC2 = ATOM_MAKE_FOURCC('W', 'G', 'H', '2');
static const unsigned AnimationCacheFourCC = ATOM_MAKE_FOURCC('A', 'C', 'A', 'C');
static const unsigned AnimationCacheFourCC2 = ATOM_MAKE_FOURCC('A', 'C', 'A', '2');
static const unsigned MeshAnimationDataFourCC = ATOM_MAKE_FOURCC('M', 'A', 'D', 'T');
static const unsigned MeshAnimationDataFourCC2 = ATOM_MAKE_FOURCC('M', 'A', 'D', '2');
static const unsigned MeshAnimationDataFourCC3 = ATOM_MAKE_FOURCC('M', 'A', 'D', '3');
static const unsigned MeshAnimationDataFourCC4 = ATOM_MAKE_FOURCC('M', 'A', 'D', '4');
static const unsigned SkeletonFourCC = ATOM_MAKE_FOURCC('S', 'K', 'L', 'T');
static const unsigned PropertyDataCacheFourCC = ATOM_MAKE_FOURCC('P', 'R', 'P', 'C');
static const unsigned ModelPropertiesFourCC = ATOM_MAKE_FOURCC('M', 'P', 'R', 'P');

struct MemReadPred
{
	char *_mem;
	unsigned long _size;
	mutable unsigned long _pos;

	MemReadPred (char *mem, unsigned size): _mem(mem), _size(size), _pos(0) {}

	bool operator () (void *buffer, unsigned long size) const
	{
		if (_size >= _pos + size)
		{
			memcpy (buffer, _mem + _pos, size);
			_pos += size;
			return true;
		}
		return false;
	}

	bool skip (int offset)
	{
		if (offset == 0)
		{
			return true;
		}

		if (offset > 0 && _size >= _pos + offset)
		{
			_pos += offset;
			return true;
		}
		else if (offset < 0 && _pos >= -offset)
		{
			_pos -= (-offset);
			return true;
		}

		return false;
	}

	unsigned long size (void) const
	{
		return _size;
	}
};

class ModelChunkReader: public ATOM_ChunkReader<MemReadPred>
{
	static const unsigned MeshChunkFourCC = ATOM_MAKE_FOURCC('M','E','S','H');
	static const unsigned MeshChunkFourCC2 = ATOM_MAKE_FOURCC('M','E','S','2');
	static const unsigned MeshChunkFourCC3 = ATOM_MAKE_FOURCC('M','E','S','3');
	static const unsigned MeshChunkFourCC4 = ATOM_MAKE_FOURCC('M','E','S','4');
	static const unsigned MeshChunkFourCC5 = ATOM_MAKE_FOURCC('M','E','S','5');
	static const unsigned MeshChunkFourCC6 = ATOM_MAKE_FOURCC('M','E','S','6');
	static const unsigned MeshChunkFourCC7 = ATOM_MAKE_FOURCC('M','E','S','7');
	static const unsigned MaterialChunkFourCC = ATOM_MAKE_FOURCC('M','T','R','L');
	static const unsigned MaterialChunkFourCC2 = ATOM_MAKE_FOURCC('M','T','R','2');
	static const unsigned MaterialChunkFourCC3 = ATOM_MAKE_FOURCC('M','T','R','3');
	static const unsigned GeometryDataChunkFourCC = ATOM_MAKE_FOURCC('G','E','O','M');
	static const unsigned MainChunkFourCC = ATOM_MAKE_FOURCC('N','3','M','F');
	static const unsigned ATOM_BBoxChunkFourCC = ATOM_MAKE_FOURCC('B','B','O','X');
	static const unsigned AABBTreeChunkFourCC = ATOM_MAKE_FOURCC('A', 'B', 'T', '0');
	static const unsigned VertexWeightFourCC = ATOM_MAKE_FOURCC('W', 'G', 'H', 'T');
	static const unsigned VertexWeightFourCC2 = ATOM_MAKE_FOURCC('W', 'G', 'H', '2');
	static const unsigned AnimationCacheFourCC = ATOM_MAKE_FOURCC('A', 'C', 'A', 'C');
	static const unsigned AnimationCacheFourCC2 = ATOM_MAKE_FOURCC('A', 'C', 'A', '2');
	static const unsigned MeshAnimationDataFourCC = ATOM_MAKE_FOURCC('M', 'A', 'D', 'T');
	static const unsigned MeshAnimationDataFourCC2 = ATOM_MAKE_FOURCC('M', 'A', 'D', '2');
	static const unsigned MeshAnimationDataFourCC3 = ATOM_MAKE_FOURCC('M', 'A', 'D', '3');
	static const unsigned MeshAnimationDataFourCC4 = ATOM_MAKE_FOURCC('M', 'A', 'D', '4');
	static const unsigned SkeletonFourCC = ATOM_MAKE_FOURCC('S', 'K', 'L', 'T');
	static const unsigned PropertyDataCacheFourCC = ATOM_MAKE_FOURCC('P', 'R', 'P', 'C');
	static const unsigned ModelPropertiesFourCC = ATOM_MAKE_FOURCC('M', 'P', 'R', 'P');

	struct MeshInfo
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
	};

	struct MeshInfo2: public MeshInfo
	{
		int isBillboard;
	};

	struct MeshInfo3: public MeshInfo2
	{
		ATOM_Matrix4x4f worldMatrix;
	};

	struct MeshInfo4: public MeshInfo3
	{
		ATOM_Mesh::TransparencyMode transparencyMode;
	};

	struct MeshInfo5: public MeshInfo4
	{
		int uvAnimationNoIpol;
	};

	struct MeshInfo6: public MeshInfo5
	{
		int fixBlending;
	};

	struct MeshInfo7: public MeshInfo6
	{
		int propHandle;
	};

	struct MaterialInfo
	{
		char diffuseMapName1[256];
		char diffuseMapName2[256];
		char normalMapName[256];
		float specularColor[3];
		float Kd;
		float Ks;
		float reserved;
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

	struct MaterialInfo2: public MaterialInfo
	{
		int materialStyle;
	};

	ATOM_VECTOR<unsigned> &meshOffsets;
	ATOM_VECTOR<unsigned> &materialOffsets;

public:
	bool supportKdKs;

public:
	ModelChunkReader (ModelIO *modelIO)
		: meshOffsets (modelIO->getMeshChunkOffsets())
		, materialOffsets (modelIO->getMaterialChunkOffsets())
	{
	}

	~ModelChunkReader (void)
	{
	}

	virtual ReadOp onChunkFound (unsigned fourcc, unsigned size, MemReadPred &pred)
	{
		switch (fourcc)
		{
		case MainChunkFourCC:
			{
				int version;
				if (!pred (&version, sizeof(int)))
					return RO_CANCELED;
				if (version > 2)
				{
					ATOM_LOGGER::error ("Unsupported model version: %d\n", version);
					return RO_CANCELED;
				}
				return read (pred, size - sizeof(int)) ? RO_PROCESSED : RO_CANCELED;
			}
		case MeshChunkFourCC:
		case MeshChunkFourCC2:
		case MeshChunkFourCC3:
		case MeshChunkFourCC4:
		case MeshChunkFourCC5:
		case MeshChunkFourCC6:
		case MeshChunkFourCC7:
			{
				meshOffsets.push_back (pred._pos);
				unsigned chunkSize = 0;
				MeshInfo7 meshInfo;

				if (fourcc == MeshChunkFourCC)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo)))
						return RO_CANCELED;
					chunkSize = sizeof(MeshInfo);
				}
				else if (fourcc == MeshChunkFourCC2)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo2)))
						return RO_CANCELED;
					chunkSize = sizeof(MeshInfo2);
				}
				else if (fourcc == MeshChunkFourCC3)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo3)))
						return RO_CANCELED;
					chunkSize = sizeof(MeshInfo3);
				}
				else if (fourcc == MeshChunkFourCC4)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo4)))
						return RO_CANCELED;
					chunkSize = sizeof(MeshInfo4);
				}
				else if (fourcc == MeshChunkFourCC5)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo5)))
						return RO_CANCELED;
					chunkSize = sizeof(MeshInfo5);
				}
				else if (fourcc == MeshChunkFourCC6)
				{
					if (!pred (&meshInfo, sizeof(MeshInfo6)))
						return RO_CANCELED;
					chunkSize = sizeof(MeshInfo6);
				}
				else
				{
					if (!pred (&meshInfo, sizeof(MeshInfo7)))
						return RO_CANCELED;
					chunkSize = sizeof(MeshInfo7);
				}

				return read (pred, size - chunkSize) ? RO_PROCESSED : RO_CANCELED;
			}
		case MaterialChunkFourCC:
		case MaterialChunkFourCC2:
		case MaterialChunkFourCC3:
		case MaterialChunkFourCC4:
			{
				materialOffsets.push_back (pred._pos);
				MaterialInfo2 materialInfo;

				if (!pred (&materialInfo, size))
					return RO_CANCELED;

				if (size != sizeof(MaterialInfo2))
				{
					if (fourcc == MaterialChunkFourCC)
					{
						materialInfo.alpharef = 0.5f;
					}
				}

				supportKdKs = (fourcc == MaterialChunkFourCC4);
				return RO_PROCESSED;
			}
		default:
			{
				return RO_SKIPPED;
			}
		}
	}
};

static const int shininessRange[2] = { 1, 255 };
static const int transparencyRange[2] = { 0, 100 };
static const int alpharefRange[2] = { 0, 100 };
static const int diffuseGainRange[2] = { 0, 500 };
static const int specularGainRange[2] = { 0, 500 };
static const float shininessRangeScale = 1.f;
static const float transparencyRangeScale = 100.f;
static const float alpharefRangeScale = 100.f;
static const float diffuseGainRangeScale = 100.f;
static const float specularGainRangeScale = 100.f;

static int calcSliderPos (const int *range, float scale, float value)
{
	int pos = scale * value;
	if (pos < range[0])
		pos = range[0];
	if (pos > range[1])
		pos = range[1];

	return pos;
}

ModelIO::ModelIO (void)
{
	_fileContent = 0;
	_modifyCopy = 0;
	_size = 0;
}

bool ModelIO::load (const char *filename)
{
	clear ();

	ATOM_AutoFile f(filename, ATOM_VFS::read);

	if (f)
	{
		_size = f->size ();

		if (_fileContent)
		{
			ATOM_DELETE_ARRAY(_fileContent);
			_fileContent = 0;
		}

		if (_modifyCopy)
		{
			ATOM_DELETE_ARRAY(_modifyCopy);
			_modifyCopy = 0;
		}

		if (_size > 0)
		{
			_fileContent = ATOM_NEW_ARRAY(char, _size);
			f->read (_fileContent, _size);
			if (prepareModel (_fileContent, _size))
			{
				_modifyCopy = ATOM_NEW_ARRAY(char, _size);
				memcpy (_modifyCopy, _fileContent, _size);
				_filename = filename;
				fixKdKs ();
				return true;
			}
			else
			{
				ATOM_DELETE_ARRAY(_fileContent);
				_fileContent = 0;
			}
		}
	}

	return false;
}

ModelIO::~ModelIO (void)
{
	clear ();
}

void ModelIO::clear (void)
{
	ATOM_DELETE_ARRAY(_fileContent);
	_fileContent = 0;

	ATOM_DELETE_ARRAY(_modifyCopy);
	_modifyCopy = 0;

	_filename = "";
	_size = 0;
	_supportKdKs = false;
	_meshChunkOffsets.clear ();
	_materialChunkOffsets.clear ();
}

bool ModelIO::prepareModel (char *content, unsigned size)
{
	MemReadPred pred(content, size);
	ModelChunkReader reader (this);
	if (!reader.readFile (pred))
	{
		return false;
	}

	_supportKdKs = reader.supportKdKs;
	return true;
}

unsigned ModelIO::getNumMeshes (void) const
{
	return _meshChunkOffsets.size();
}

const char *ModelIO::getFileName (void) const
{
	return _filename.c_str();
}

ATOM_Vector4f ModelIO::getSpecularColor (unsigned mesh, bool modified) const
{
	unsigned offset = _materialChunkOffsets[mesh];
	ATOM_Vector3f *pSpecular = (ATOM_Vector3f*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256);
	return ATOM_Vector4f(pSpecular->x, pSpecular->y, pSpecular->z, 1.f);
}

void ModelIO::setSpecularColor (unsigned mesh, const ATOM_Vector4f &color, bool modified)
{
	unsigned offset = _materialChunkOffsets[mesh];
	ATOM_Vector3f *pSpecular = (ATOM_Vector3f*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256);
	pSpecular->set (color.x, color.y, color.z);
}

ATOM_Vector4f ModelIO::getDiffuseColor (unsigned mesh, bool modified) const
{
	unsigned offset = _materialChunkOffsets[mesh];
	ATOM_Vector3f *pDiffuse = (ATOM_Vector3f*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float));
	return ATOM_Vector4f(pDiffuse->x, pDiffuse->y, pDiffuse->z, 1.f);
}

void ModelIO::setDiffuseColor (unsigned mesh, const ATOM_Vector4f &color, bool modified)
{
	unsigned offset = _materialChunkOffsets[mesh];
	ATOM_Vector3f *pDiffuse = (ATOM_Vector3f*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float));
	pDiffuse->set (color.x, color.y, color.z);
}

float ModelIO::getShininess (unsigned mesh, bool modified) const
{
	unsigned offset = _materialChunkOffsets[mesh];
	float *pShininess = (float*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float) + sizeof(int) + sizeof(int) + sizeof(float));
	return *pShininess;
}

void ModelIO::setShininess (unsigned mesh, float shininess, bool modified)
{
	unsigned offset = _materialChunkOffsets[mesh];
	float *pShininess = (float*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float) + sizeof(int) + sizeof(int) + sizeof(float));
	*pShininess = shininess;
}

bool ModelIO::isAlphaTestEnabled (unsigned mesh, bool modified) const
{
	unsigned offset = _materialChunkOffsets[mesh];
	int *pAlphaTestEnable = (int*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float));
	return (*pAlphaTestEnable) != 0;
}

void ModelIO::enableAlphaTest (unsigned mesh, bool enable, bool modified)
{
	unsigned offset = _materialChunkOffsets[mesh];
	int *pAlphaTestEnable = (int*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float));
	*pAlphaTestEnable = enable ? 1 : 0;
}

float ModelIO::getAlphaRef (unsigned mesh, bool modified) const
{
	unsigned offset = _materialChunkOffsets[mesh];
	float *pAlphaRef = (float*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float) + sizeof(int) + sizeof(int));
	return *pAlphaRef;
}

void ModelIO::setAlphaRef (unsigned mesh, float alpharef, bool modified) const
{
	unsigned offset = _materialChunkOffsets[mesh];
	float *pAlphaRef = (float*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float) + sizeof(int) + sizeof(int));
	*pAlphaRef = alpharef;
}

float ModelIO::getTransparency (unsigned mesh, bool modified) const
{
	unsigned offset = _materialChunkOffsets[mesh];
	float *pTrans = (float*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float) + sizeof(int) + sizeof(int) + sizeof(float) + sizeof(float));
	return *pTrans;
}

void ModelIO::setTransparency (unsigned mesh, float transparency, bool modified)
{
	unsigned offset = _materialChunkOffsets[mesh];
	float *pTrans = (float*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float) + sizeof(int) + sizeof(int) + sizeof(float) + sizeof(float));
	*pTrans = transparency;
}

bool ModelIO::isDoubleSided (unsigned mesh, bool modified) const
{
	unsigned offset = _materialChunkOffsets[mesh];
	int *pDoubleSided = (int*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float) + sizeof(int));
	return (*pDoubleSided) != 0;
}

void ModelIO::setDoubleSided (unsigned mesh, bool doubleSided, bool modified)
{
	unsigned offset = _materialChunkOffsets[mesh];
	int *pDoubleSided = (int*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float) + sizeof(int));
	(*pDoubleSided) = doubleSided ? 1 : 0;
}

float ModelIO::getDiffuseGain (unsigned mesh, bool modified) const
{
	if (_supportKdKs)
	{
		unsigned offset = _materialChunkOffsets[mesh];
		float *pDiffuseGain = (float*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float));
		return *pDiffuseGain;
	}
	else
	{
		return 1.f;
	}
}

void ModelIO::setDiffuseGain (unsigned mesh, float diffuseGain, bool modified)
{
	unsigned offset = _materialChunkOffsets[mesh];
	float *pDiffuseGain = (float*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float));
	*pDiffuseGain = diffuseGain;
}

float ModelIO::getSpecularGain (unsigned mesh, bool modified)
{
	if (_supportKdKs)
	{
		unsigned offset = _materialChunkOffsets[mesh];
		float *pSpecularGain = (float*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + sizeof(float));
		return *pSpecularGain;
	}
	else
	{
		return 1.f;
	}
}

void ModelIO::setSpecularGain (unsigned mesh, float specularGain, bool modified)
{
	unsigned offset = _materialChunkOffsets[mesh];
	float *pSpecularGain = (float*)((modified ? _modifyCopy : _fileContent) + offset + 3 * 256 + 3 * sizeof(float) + sizeof(float));
	*pSpecularGain = specularGain;
}

const char *ModelIO::getDiffuseMapFileName (unsigned mesh, bool modified) const
{
	unsigned offset = _materialChunkOffsets[mesh];
	const char *pDiffuseMapFileName = (modified ? _modifyCopy : _fileContent) + offset;
	return pDiffuseMapFileName;
}

void ModelIO::setDiffuseMapFileName (unsigned mesh, const char *filename, bool modified)
{
	unsigned offset = _materialChunkOffsets[mesh];
	char *pDiffuseMapFileName = (modified ? _modifyCopy : _fileContent) + offset;
	if (filename && strlen (filename) < 256)
	{
		strcpy (pDiffuseMapFileName, filename);
	}
}

const char *ModelIO::getTransparencyMapFileName (unsigned mesh, bool modified) const
{
	unsigned offset = _materialChunkOffsets[mesh];
	const char *pTransparencyMapFileName = (modified ? _modifyCopy : _fileContent) + offset + 256;
	return pTransparencyMapFileName;
}

void ModelIO::setTransparencyMapFileName (unsigned mesh, const char *filename, bool modified)
{
	unsigned offset = _materialChunkOffsets[mesh];
	char *pTransparencyMapFileName = (modified ? _modifyCopy : _fileContent) + offset + 256;
	if (filename && strlen (filename) < 256)
	{
		strcpy (pTransparencyMapFileName, filename);
	}
}

const char *ModelIO::getNormalMapFileName (unsigned mesh, bool modified) const
{
	unsigned offset = _materialChunkOffsets[mesh];
	const char *pNormalMapFileName = (modified ? _modifyCopy : _fileContent) + offset + 256 * 2;
	return pNormalMapFileName;
}

void ModelIO::setNormalMapFileName (unsigned mesh, const char *filename, bool modified)
{
	unsigned offset = _materialChunkOffsets[mesh];
	char *pNormalMapFileName = (modified ? _modifyCopy : _fileContent) + offset + 256 * 2;
	if (filename && strlen (filename) < 256)
	{
		strcpy (pNormalMapFileName, filename);
	}
}

ATOM_VECTOR<unsigned> &ModelIO::getMeshChunkOffsets (void)
{
	return _meshChunkOffsets;
}

ATOM_VECTOR<unsigned> &ModelIO::getMaterialChunkOffsets (void)
{
	return _materialChunkOffsets;
}

ATOM_STRING ModelIO::chooseVFSFile (HWND dlg, const char *filter)
{
	ATOM_FileDlg filedlg(0, "/", NULL, NULL, filter, dlg);
	if (filedlg.doModal () == IDOK) 
	{
		return filedlg.getSelectedFileName(0);
	}
	return "";
}

bool ModelIO::chooseColor (HWND dlg, ATOM_ColorARGB &color)
{
	static COLORREF custColors[16] = { 0 };
	CHOOSECOLOR cc;
	memset (&cc, 0, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = GetActiveWindow ();
	cc.Flags = CC_RGBINIT|CC_FULLOPEN|CC_ANYCOLOR;
	cc.rgbResult = RGB(color.getByteR(), color.getByteG(), color.getByteB());
	cc.lpCustColors = (LPDWORD)custColors;
	if (::ChooseColor (&cc))
	{
		color.setBytes (GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult), 255);
		return true;
	}
	return false;
}

INT_PTR CALLBACK EditModelDialogProc (HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static ModelIO *modelio = NULL;
	static int currentMesh = 0;

	switch(message) {
		case WM_INITDIALOG:
			{
				modelio = (ModelIO*)lParam;
				HWND meshList = ::GetDlgItem (hWnd, IDC_MESHLIST);
				for (unsigned i = 0; i < modelio->getNumMeshes(); ++i)
				{
					char buffer[64];
					sprintf (buffer, "ATOM_Mesh %d", i);
					ListBox_AddString (meshList, buffer);
				}
				ListBox_SetCurSel (meshList, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_SHININESS), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(shininessRange[0], shininessRange[1]));
				::SendMessage (::GetDlgItem (hWnd, IDC_TRANSPARENCY), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(transparencyRange[0], transparencyRange[1]));
				::SendMessage (::GetDlgItem (hWnd, IDC_ALPHAREF), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(alpharefRange[0], alpharefRange[1]));
				::SendMessage (::GetDlgItem (hWnd, IDC_DIFFUSEGAIN), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(diffuseGainRange[0], diffuseGainRange[1]));
				::SendMessage (::GetDlgItem (hWnd, IDC_SPECULARGAIN), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(specularGainRange[0], specularGainRange[1]));
				modelio->syncModelProperties (hWnd, 0);
				modelio->applyModelProperties (hWnd, 0);
				currentMesh = 0;
				char title[512];
				sprintf (title, "编辑模型 - %s", modelio->getFileName());
				::SetWindowTextA (hWnd, title);
				return TRUE;
			}

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_SETDIFFUSEMAP:
				{
					ATOM_STRING filename = modelio->chooseVFSFile (hWnd, "PNG文件(*.png)|*.png|DDS文件(*.dds)|*.dds|JPG文件(*.jpg)|*.jpg|所有文件(*.*)|*.*|");
					if (!filename.empty ())
					{
						::SetWindowTextA(::GetDlgItem(hWnd, IDC_DIFFUSEMAPFILENAME), filename.c_str());
					}
					break;
				}
			case IDC_SETOPACITYMAP:
				{
					ATOM_STRING filename = modelio->chooseVFSFile (hWnd, "PNG文件(*.png)|*.png|DDS文件(*.dds)|*.dds|JPG文件(*.jpg)|*.jpg|所有文件(*.*)|*.*|");
					if (!filename.empty ())
					{
						::SetWindowTextA(::GetDlgItem(hWnd, IDC_OPACITYMAPFILENAME), filename.c_str());
					}
					break;
				}
			case IDC_SETNORMALMAP:
				{
					ATOM_STRING filename = modelio->chooseVFSFile (hWnd, "PNG文件(*.png)|*.png|DDS文件(*.dds)|*.dds|JPG文件(*.jpg)|*.jpg|所有文件(*.*)|*.*|");
					if (!filename.empty ())
					{
						::SetWindowTextA(::GetDlgItem(hWnd, IDC_NORMALMAPFILENAME), filename.c_str());
					}
					break;
				}
			case IDC_DIFFUSECOLOR:
				{
					int mesh = ListBox_GetCurSel(::GetDlgItem(hWnd, IDC_MESHLIST));
					if (mesh != LB_ERR)
					{
						ATOM_Vector4f c = modelio->getDiffuseColor (mesh, true);
						ATOM_ColorARGB color(c.x, c.y, c.z, 1.f);
						if (modelio->chooseColor (hWnd, color))
						{
							color.getFloats (&c.x, &c.y, &c.z, &c.w);
							c.w = 1.f;
							modelio->setDiffuseColor (mesh, c, true);
						}
					}
					break;
				}
			case IDC_SPECULARCOLOR:
				{
					int mesh = ListBox_GetCurSel(::GetDlgItem(hWnd, IDC_MESHLIST));
					if (mesh != LB_ERR)
					{
						ATOM_Vector4f c = modelio->getSpecularColor (mesh, true);
						ATOM_ColorARGB color(c.x, c.y, c.z, 1.f);
						if (modelio->chooseColor (hWnd, color))
						{
							color.getFloats (&c.x, &c.y, &c.z, &c.w);
							c.w = 1.f;
							modelio->setSpecularColor (mesh, c, true);
						}
					}
					break;
				}
			case IDC_RESTOREVALUES:
				{
					modelio->restoreInitialValues ();
					if (currentMesh >= 0)
					{
						modelio->syncModelProperties (hWnd, currentMesh);
					}
					break;
				}
			case IDOK:
				{
					if (currentMesh >= 0)
					{
						modelio->applyModelProperties (hWnd, currentMesh);
					}
					::EndDialog (hWnd, IDOK);
					break;
				}
			case IDCANCEL:
				{
					modelio->restoreInitialValues ();
					::EndDialog (hWnd, IDCANCEL);
					break;
				}
			case IDC_MESHLIST:
				{
					if (HIWORD(wParam) == LBN_SELCHANGE)
					{
						if (currentMesh >= 0)
						{
							modelio->applyModelProperties (hWnd, currentMesh);
						}

						int index = ListBox_GetCurSel(::GetDlgItem(hWnd, IDC_MESHLIST));
						if (index != LB_ERR)
						{
							currentMesh = index;
						}
						else
						{
							currentMesh = -1;
						}

						modelio->syncModelProperties (hWnd, currentMesh);
					}
					break;
				}
			default:
				return FALSE;
			}
			break;
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}

bool ModelIO::showEditDialog (HWND parentWnd)
{
	return ::DialogBoxParam(NULL, MAKEINTRESOURCE(IDD_EDITMODEL), parentWnd, EditModelDialogProc, (LPARAM)this) == IDOK;
}

void ModelIO::applyModelProperties (HWND dlg, unsigned mesh)
{
	char buffer[512];
	HWND editDiffuseMap = ::GetDlgItem (dlg, IDC_DIFFUSEMAPFILENAME);
	::GetWindowTextA (editDiffuseMap, buffer, 512);
	setDiffuseMapFileName(mesh, buffer, true);

	HWND editOpacityMap = ::GetDlgItem (dlg, IDC_OPACITYMAPFILENAME);
	::GetWindowTextA (editOpacityMap, buffer, 512);
	setTransparencyMapFileName(mesh, buffer, true);

	HWND editNormalMap = ::GetDlgItem (dlg, IDC_NORMALMAPFILENAME);
	::GetWindowTextA (editNormalMap, buffer, 512);
	setNormalMapFileName(mesh, buffer, true);

	HWND sliderShininess = ::GetDlgItem (dlg, IDC_SHININESS);
	float shininess = (::SendMessage (sliderShininess, TBM_GETPOS, 0, 0)) / shininessRangeScale;
	setShininess (mesh, shininess, true);

	HWND sliderTrans = ::GetDlgItem (dlg, IDC_TRANSPARENCY);
	float trans = (::SendMessage (sliderTrans, TBM_GETPOS, 0, 0)) / transparencyRangeScale;
	setTransparency(mesh, trans, true);

	HWND sliderAlphaRef = ::GetDlgItem (dlg, IDC_ALPHAREF);
	float alpharef = (::SendMessage (sliderAlphaRef, TBM_GETPOS, 0, 0)) / alpharefRangeScale;
	setAlphaRef(mesh, alpharef, true);

	HWND sliderDiffuseGain = ::GetDlgItem (dlg, IDC_DIFFUSEGAIN);
	float diffusegain = (::SendMessage (sliderDiffuseGain, TBM_GETPOS, 0, 0)) / diffuseGainRangeScale;
	setDiffuseGain(mesh, diffusegain, true);

	HWND sliderSpecularGain = ::GetDlgItem (dlg, IDC_SPECULARGAIN);
	float speculargain = (::SendMessage (sliderSpecularGain, TBM_GETPOS, 0, 0)) / specularGainRangeScale;
	setSpecularGain (mesh, speculargain, true);

	HWND chkDoubleSide = ::GetDlgItem (dlg, IDC_DOUBLESIDE);
	bool doubleside = BST_CHECKED == ::SendMessage (chkDoubleSide, BM_GETCHECK, 0, 0);
	setDoubleSided (mesh, doubleside, true);

	HWND chkAlphaTest = ::GetDlgItem (dlg, IDC_ALPHATEST);
	bool alphatest = BST_CHECKED == ::SendMessage (chkAlphaTest, BM_GETCHECK, 0, 0);
	enableAlphaTest (mesh, alphatest, true);
}

void ModelIO::syncModelProperties (HWND dlg, unsigned mesh)
{
	HWND editDiffuseMap = ::GetDlgItem (dlg, IDC_DIFFUSEMAPFILENAME);
	::SetWindowTextA (editDiffuseMap, getDiffuseMapFileName(mesh, true));

	HWND editOpacityMap = ::GetDlgItem (dlg, IDC_OPACITYMAPFILENAME);
	::SetWindowTextA (editOpacityMap, getTransparencyMapFileName(mesh, true));

	HWND editNormalMap = ::GetDlgItem (dlg, IDC_NORMALMAPFILENAME);
	::SetWindowTextA (editNormalMap, getNormalMapFileName(mesh, true));

	HWND sliderShininess = ::GetDlgItem (dlg, IDC_SHININESS);
	::SendMessage (sliderShininess, TBM_SETPOS, TRUE, calcSliderPos (shininessRange, shininessRangeScale, getShininess(mesh, true)));

	HWND sliderTrans = ::GetDlgItem (dlg, IDC_TRANSPARENCY);
	::SendMessage (sliderTrans, TBM_SETPOS, TRUE, calcSliderPos (transparencyRange, transparencyRangeScale, getTransparency(mesh, true)));

	HWND sliderAlphaRef = ::GetDlgItem (dlg, IDC_ALPHAREF);
	::SendMessage (sliderAlphaRef, TBM_SETPOS, TRUE, calcSliderPos (alpharefRange, alpharefRangeScale, getAlphaRef(mesh, true)));

	HWND sliderDiffuseGain = ::GetDlgItem (dlg, IDC_DIFFUSEGAIN);
	::SendMessage (sliderDiffuseGain, TBM_SETPOS, TRUE, calcSliderPos (diffuseGainRange, diffuseGainRangeScale, getDiffuseGain(mesh, true)));

	HWND sliderSpecularGain = ::GetDlgItem (dlg, IDC_SPECULARGAIN);
	::SendMessage (sliderSpecularGain, TBM_SETPOS, TRUE, calcSliderPos (specularGainRange, specularGainRangeScale, getSpecularGain(mesh, true)));

	HWND chkDoubleSide = ::GetDlgItem (dlg, IDC_DOUBLESIDE);
	::SendMessage (chkDoubleSide, BM_SETCHECK, isDoubleSided(mesh, true) ? BST_CHECKED : BST_UNCHECKED, 0);

	HWND chkAlphaTest = ::GetDlgItem (dlg, IDC_ALPHATEST);
	::SendMessage (chkAlphaTest, BM_SETCHECK, isAlphaTestEnabled(mesh, true) ? BST_CHECKED : BST_UNCHECKED, 0);

	::EnableWindow (::GetDlgItem (dlg, IDC_SETNORMALMAP), getNormalMapFileName(mesh, true)[0] == '\0' ? FALSE : TRUE);
}

bool ModelIO::modified (void)
{
	for (unsigned i = 0; i < getNumMeshes(); ++i)
	{
		ATOM_Vector4f specularColorNew = getSpecularColor (i, true);
		ATOM_Vector4f specularColorOld = getSpecularColor (i, false);
		if (ATOM_ColorARGB(specularColorNew.x, specularColorNew.y, specularColorNew.z, 1.f) 
			!= ATOM_ColorARGB(specularColorOld.x, specularColorOld.y, specularColorOld.z, 1.f))
		{
			return true;
		}

		ATOM_Vector4f diffuseColorNew = getDiffuseColor (i, true);
		ATOM_Vector4f diffuseColorOld = getDiffuseColor (i, false);
		if (ATOM_ColorARGB(diffuseColorNew.x, diffuseColorNew.y, diffuseColorNew.z, 1.f) 
			!= ATOM_ColorARGB(diffuseColorOld.x, diffuseColorOld.y, diffuseColorOld.z, 1.f))
		{
			return true;
		}

		if (calcSliderPos (shininessRange, shininessRangeScale, getShininess(i, true))
			!= calcSliderPos (shininessRange, shininessRangeScale, getShininess(i, false)))
		{
			return true;
		}

		if (isAlphaTestEnabled(i, true) != isAlphaTestEnabled(i, false))
		{
			return true;
		}

		if (isDoubleSided (i, true) != isDoubleSided (i, false))
		{
			return true;
		}

		if (calcSliderPos (alpharefRange, alpharefRangeScale, getAlphaRef(i, true))
			!= calcSliderPos (alpharefRange, alpharefRangeScale, getAlphaRef(i, false)))
		{
			return true;
		}

		if (calcSliderPos (transparencyRange, transparencyRangeScale, getTransparency(i, true))
			!= calcSliderPos (transparencyRange, transparencyRangeScale, getTransparency(i, false)))
		{
			return true;
		}

		if (calcSliderPos (diffuseGainRange, diffuseGainRangeScale, getDiffuseGain(i, true))
			!= calcSliderPos (diffuseGainRange, diffuseGainRangeScale, getDiffuseGain(i, false)))
		{
			return true;
		}

		if (calcSliderPos (specularGainRange, specularGainRangeScale, getSpecularGain(i, true))
			!= calcSliderPos (specularGainRange, specularGainRangeScale, getSpecularGain(i, false)))
		{
			return true;
		}

		if (strcmp (getDiffuseMapFileName(i, true), getDiffuseMapFileName(i, false)))
		{
			return true;
		}

		if (strcmp (getTransparencyMapFileName(i, true), getTransparencyMapFileName(i, false)))
		{
			return true;
		}

		if (strcmp (getNormalMapFileName(i, true), getNormalMapFileName(i, false)))
		{
			return true;
		}
	}

	return false;
}

void ModelIO::restoreInitialValues (void)
{
	if (_fileContent)
	{
		memcpy (_modifyCopy, _fileContent, _size);
		fixKdKs ();
	}
}

bool ModelIO::save (const char *filename)
{
	if (filename == 0)
	{
		filename = _filename.c_str();
	}

	if (_fileContent)
	{
		ATOM_AutoFile f (filename, ATOM_VFS::write);
		if (f)
		{
			if (f->write (_modifyCopy, _size) == _size)
			{
				return true;
			}
		}
	}
	return false;
}

void ModelIO::fixKdKs (void)
{
	if (!_supportKdKs)
	{
		for (unsigned i = 0; i < _materialChunkOffsets.size(); ++i)
		{
			unsigned *pFourCC = (unsigned*)(_modifyCopy + _materialChunkOffsets[i] - 2 * sizeof(unsigned));
			*pFourCC = MaterialChunkFourCC4;
			setDiffuseGain (i, 1.f, true);
			setSpecularGain (i, 1.f, true);
		}

		_supportKdKs = true;
	}
}
