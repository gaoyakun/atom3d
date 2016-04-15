#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

#if _MSC_VER > 1000
# pragma once
#endif

#include "nm_translator.h"
#include "mtl_translator.h"

#ifndef ATOM_MAYAEXPORTER_EXP
#	ifdef _WIN32
#		define ATOM_MAYAEXPORTER_EXP __declspec(dllexport)
#	else
#		define ATOM_MAYAEXPORTER_EXP
#	endif
#endif

ATOM_MAYAEXPORTER_EXP MStatus initializePlugin (MObject mObject)
{
	MFnPlugin exporterPlugin (mObject, "UUGame", "0.1", "Any");
	
	MStatus stat = exporterPlugin.registerFileTranslator ("atom mesh v2 exporter", "none", &NM_Translator::creator); 
	if (stat != MS::kSuccess)
	{
		return stat;
	}

	stat = exporterPlugin.registerFileTranslator ("atom material v2 exporter", "none", &MTL_Translator::creator);
	if (stat != MS::kSuccess)
	{
		return stat;
	}

	return MS::kSuccess;
}

ATOM_MAYAEXPORTER_EXP MStatus uninitializePlugin (MObject mObject)
{
	MFnPlugin exporterPlugin (mObject);

	exporterPlugin.deregisterFileTranslator ("atom mesh exporter");
	exporterPlugin.deregisterFileTranslator ("atom material exporter");

	return MS::kSuccess;
}
