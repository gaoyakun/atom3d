#ifndef __ATOM_MAYAEXPORT_EXPORTOPTIONS_H
#define __ATOM_MAYAEXPORT_EXPORTOPTIONS_H

#if _MSC_VER > 1000
# pragma once
#endif

class ExportOptions
{
public:
	ExportOptions (void)
		: _exportActions(true)
		, _exportMeshes(true)
		, _exportSkeletons(false)
		, _saveAnimationSettings(true)
		, _doubleside(false) 
		, _exportVertexColor(false)
		, _exportVertexTransparency(false)
		, _uvAnimationNoIpol(false)
		, _bindPoseSource(0)
		, _selectBoneByName(false)
	{
	}

public:
	bool exportActions (void) const { return _exportActions; }
	void setExportActions (bool b) { _exportActions = b; }
	bool exportMeshes (void) const { return _exportMeshes; }
	void setExportMeshes (bool b) { _exportMeshes = b; }
	bool exportSkeletons (void) const { return _exportSkeletons; }
	void setExportSkeletons (bool b) { _exportSkeletons = b; }
	bool doubleSide (void) const { return _doubleside; }
	void setDoubleSide (bool b) { _doubleside = b; }
	bool exportVertexColor (void) const { return _exportVertexColor; }
	void setExportVertexColor (bool b) { _exportVertexColor = b; }
	bool exportVertexTransparency (void) const { return _exportVertexTransparency; }
	void setExportVertexTransparency (bool b) { _exportVertexTransparency = b; }
	bool saveAnimationSettings (void) const { return _saveAnimationSettings; }
	void setSaveAnimationSettings (bool b) { _saveAnimationSettings = b; }
	bool uvAnimationNoIpol (void) const { return _uvAnimationNoIpol; }
	void setUvAnimationNoIpol (bool b) { _uvAnimationNoIpol; }
	int getBindPoseSource (void) const { return _bindPoseSource; }
	void setBindPoseSource (int val) { _bindPoseSource = val; }
	bool getSelectBoneByName (void) const { return _selectBoneByName; }
	void setSelectBoneByName (bool b) { _selectBoneByName = b; }

private:
	bool _exportActions;
	bool _exportMeshes;
	bool _exportSkeletons;
	bool _saveAnimationSettings;
	bool _doubleside;
	bool _exportVertexColor;
	bool _exportVertexTransparency;
	bool _uvAnimationNoIpol;
	bool _selectBoneByName;
	int _bindPoseSource;
};

#endif // __ATOM_MAYAEXPORT_EXPORTOPTIONS_H
