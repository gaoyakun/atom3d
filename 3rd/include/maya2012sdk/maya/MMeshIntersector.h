#ifndef _MMeshIntersector
#define _MMeshIntersector
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
// CLASS:    MMeshIntersector
//
// ****************************************************************************

#if defined __cplusplus

// ****************************************************************************
// INCLUDED HEADER FILES


#include <float.h>
#include <maya/MStatus.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatVector.h>
#include <maya/MMatrix.h>

// ****************************************************************************
// DECLARATIONS

class MStatus;
class MPointOnMesh;

// ****************************************************************************
// CLASS DECLARATION (MMeshIntersector)

//! \ingroup OpenMaya
//! \brief Mesh intersector. 
/*!
	The MMeshIntersector class contains methods for efficiently finding
	the closest point to a mesh.  An octree algorithm is used to
	find the closest point.

	The create() method builds the internal data required for the algorithm.
	As a result, calls to it should be minimized as it is a heavy operation.

	This class allows multiple threads to evaluate closest points
	simultaneously as the method getClosestPoint is threadsafe.

*/
class OPENMAYA_EXPORT MMeshIntersector
{
public:
	MMeshIntersector(void);
	virtual ~MMeshIntersector(void);

	MStatus create( MObject &meshObject, const MMatrix& matrix = MMatrix::identity );

	bool isCreated(void);

	MStatus getClosestPoint( MPoint& point, MPointOnMesh& meshPoint, double maxDistance = DBL_MAX );

	static const char*	className();

protected:
// No protected members
private:
	void *instance;
};

// Mesh point information. (OpenMaya) (OpenMaya.py)
class OPENMAYA_EXPORT MPointOnMesh
{
public:

	MPointOnMesh();
	MPointOnMesh( const MPointOnMesh& other );
	MPointOnMesh& operator=( const MPointOnMesh& other );
	
	MFloatPoint& getPoint();
	MFloatVector& getNormal();
	
	int faceIndex();	
	int triangleIndex();

protected:
// No protected members

private:
	friend class MMeshIntersector;
	MFloatPoint point;
	MFloatVector normal;
	int faceId;
	int triangleId;
};

#endif /* __cplusplus */
#endif /* _MMeshIntersector */
