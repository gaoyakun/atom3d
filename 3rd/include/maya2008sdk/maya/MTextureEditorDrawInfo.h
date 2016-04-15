#ifndef __MTextureEditorDrawInfo_h 
#define __MTextureEditorDrawInfo_h 

//
//-
// ==========================================================================
// Copyright (C) 1995 - 2006 Autodesk, Inc., and/or its licensors.  All 
// rights reserved.
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
// CLASS:    MTextureEditorDrawInfo
//
//    This class is used by drawUV method of MPxSurfaceShapeUI to specify 
//    the current object drawing state for a user defined shape.  
//
// *****************************************************************************
//
// CLASS DESCRIPTION (MTextureEditorDrawInfo)
//
// *****************************************************************************
//
#if defined __cplusplus

// *****************************************************************************

// INCLUDED HEADER FILES


#include <maya/MStatus.h>
#include <maya/MTypes.h>
#include <maya/MObject.h>
#include <maya/M3dView.h>
#include <maya/MDrawRequest.h>

// *****************************************************************************

// DECLARATIONS

class MSelectionMask;
class MSelectionTypeSet;
class MPoint;
class MPointArray;
class MVector;
class MSelectionList;
class MMatrix;

// *****************************************************************************

// CLASS DECLARATION (MTextureEditorDrawInfo)

/// Drawing state for drawing to the UV texture window with custom 
/// shapes. See MPxSurfaceShapeUI (OpenMayaUI) for more information. 
/// 
/**
*/
#ifdef _WIN32
#pragma warning(disable: 4522)
#endif // _WIN32

class OPENMAYAUI_EXPORT MTextureEditorDrawInfo
{
public:
	///
	MTextureEditorDrawInfo();
	///
	MTextureEditorDrawInfo( const MTextureEditorDrawInfo& in );
	///
	virtual ~MTextureEditorDrawInfo();

	///
	enum DrawingFunction { 
		///
		kDrawFunctionFirst = 1, 
		/// Draw wireframe only (default)
		kDrawWireframe = kDrawFunctionFirst, 
		/// Draw vertices, uvs, faces, and edges
		kDrawEverything, 
		/// Draw vertices for selection
		kDrawVertexForSelect, 
		/// Draw edges for selection 
		kDrawEdgeForSelect, 
		/// Draw faces for selection 
		kDrawFacetForSelect, 
		/// Draw uvs for selection 
		kDrawUVForSelect, 
		///
		kDrawFunctionLast = kDrawUVForSelect,
	} ; 
	
	/// 
	DrawingFunction			drawingFunction() const; 
	/// 
	void					setDrawingFunction( DrawingFunction func ); 
	
protected: 

	// No protected members 
	
private: 
	const char*				className() const; 
	MTextureEditorDrawInfo( void * in ); 
	
	void *					fData

; 
};

#ifdef _WIN32
#pragma warning(default: 4522)
#endif // _WIN32

// *****************************************************************************
#endif /* __cplusplus */
#endif /* _MTextureEditorDrawInfo */
