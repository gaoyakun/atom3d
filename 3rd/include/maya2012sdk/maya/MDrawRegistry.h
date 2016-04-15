#ifndef _MDrawRegistry
#define _MDrawRegistry
//
//-
// ==========================================================================
// Copyright (C) 2010 Autodesk, Inc., and/or its licensors.  All rights
// reserved.
//
// The coded instructions, statements, computer programs, and/or related
// material (collectively the "Data") in these files contain unpublished
// information proprietary to Autodesk, Inc. ("Autodesk") and/or its
// licensors,  which is protected by U.S. and Canadian federal copyright law
// and by international treaties.
//
// The Data may not be disclosed or distributed to third parties or be
// copied or duplicated, in whole or in part, without the prior written
// consent of Autodesk.
//
// The copyright notices in the Software and this entire statement,
// including the above license grant, this restriction and the following
// disclaimer, must be included in all copies of the Software, in whole
// or in part, and all derivative works of the Software, unless such copies
// or derivative works are solely in the form of machine-executable object
// code generated by a source language processor.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// AUTODESK DOES NOT MAKE AND HEREBY DISCLAIMS ANY EXPRESS OR IMPLIED
// WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE WARRANTIES OF
// NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
// OR ARISING FROM A COURSE OF DEALING, USAGE, OR TRADE PRACTICE. IN NO
// EVENT WILL AUTODESK AND/OR ITS LICENSORS BE LIABLE FOR ANY LOST
// REVENUES, DATA, OR PROFITS, OR SPECIAL, DIRECT, INDIRECT, OR
// CONSEQUENTIAL DAMAGES, EVEN IF AUTODESK AND/OR ITS LICENSORS HAS
// BEEN ADVISED OF THE POSSIBILITY OR PROBABILITY OF SUCH DAMAGES.
// ==========================================================================
//+
//
// CLASS:    MDrawRegistry
//
// ****************************************************************************

#if defined __cplusplus


#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MObject.h>

// ****************************************************************************
// NAMESPACE

namespace MHWRender
{

class MPxShaderOverride;
class MPxGeometryOverride;
class MPxDrawOverride;

// ****************************************************************************
// CLASS DECLARATION (MDrawRegistry)

//! \ingroup OpenMayaRender
//! \brief Access the registry associating node types with custom draw classes
/*!
This class provides a means to register custom draw overrides to be used by
Viewport 2.0 to draw dependency nodes.

The registry is based on classification strings. Each draw override must be
registered with a classification string and any node type with a matching
classification string will be drawn using the registered draw override.
*/
class OPENMAYARENDER_EXPORT MDrawRegistry
{
public:
	typedef MPxShaderOverride* (*ShaderOverrideCreator)(const MObject&);
	static MStatus registerShaderOverrideCreator(
					const MString& drawClassification,
					const MString& registrantId,
					ShaderOverrideCreator creator);
	static MStatus deregisterShaderOverrideCreator(
					const MString& drawClassification,
					const MString& registrantId);

	typedef MPxGeometryOverride* (*GeometryOverrideCreator)(const MObject&);
	static MStatus registerGeometryOverrideCreator(
					const MString& drawClassification,
					const MString& registrantId,
					GeometryOverrideCreator creator);
	static MStatus deregisterGeometryOverrideCreator(
					const MString& drawClassification,
					const MString& registrantId);

	typedef MPxDrawOverride* (*DrawOverrideCreator)(const MObject&);
	static MStatus registerDrawOverrideCreator(
					const MString& drawClassification,
					const MString& registrantId,
					DrawOverrideCreator creator);
	static MStatus deregisterDrawOverrideCreator(
					const MString& drawClassification,
					const MString& registrantId);

	static const char* className();

private:
	MDrawRegistry();
	~MDrawRegistry();
};

} // namespace MHWRender

// ****************************************************************************

#endif /* __cplusplus */
#endif /* _MDrawRegistry */

