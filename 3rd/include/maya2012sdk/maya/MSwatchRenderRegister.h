#ifndef _MSwatchRenderRegister
#define _MSwatchRenderRegister
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
// CLASS:    MSwatchRenderRegister
//
// ****************************************************************************

#if defined __cplusplus

// ****************************************************************************
// INCLUDED HEADER FILES


#include <maya/MStatus.h>
#include <maya/MTypes.h>

// ****************************************************************************
// DECLARATIONS

class MObject;
class MString ;
class MSwatchRenderBase ;

//! \brief Pointer to a function which returns a swatch generator object.
/*
 \param[in] dependNode Node for which the swatch is being generated.
 \param[in] renderNode Shader to be used in rendering the node's swatch.
 \param[in] imageResolution Desired resolution, in pixels, of the swatch.
*/
typedef MSwatchRenderBase* (*MSwatchRenderCreatorFnPtr) (MObject dependNode, MObject renderNode, int imageResolution);

//! \ingroup OpenMayaRender
//! \brief Manages swatch generators
/*!
  Provides an interface for plugins to register/unregister swatch
  generator classes (derived from MSwatchRenderBase) with Maya.
  Whenever a swatch needs to be generated for a node, it checks the
  classfication string of the node for the preferred swatch generator.
  If a match is found, it creates and manages the swatch generator
  object (that is it deletes the swatch generator object once the
  image has been generated). The doIteration function is called for
  generating the swatch.  The doIteration function is called
  repeatedly (during idle events) till it returns true. This allows
  for generation of the swatch in stages.
*/
class OPENMAYARENDER_EXPORT MSwatchRenderRegister {
public:
	//! registers a new swatch generator creation function by name.
	static MStatus	registerSwatchRender(MString swatchGenName, MSwatchRenderCreatorFnPtr fnPtr);

	//! removes the previously registered swatch generator
	static MStatus	unregisterSwatchRender(MString swatchGenName);

protected:
// No protected members

private:
// No private members
};

#endif /* __cplusplus */
#endif /* _MSwatchRenderRegister */

