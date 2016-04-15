/*
  Copyright (c) 2004 Pascal Kirchdorfer, Seth Yastrov

  This software is provided 'as-is', without any express or implied warranty. In
  no event will the authors be held liable for any damages arising from the use
  of this software.

  Permission is granted to anyone to use this software for any purpose, including
  commercial applications, and to alter it and redistribute it freely, subject to
  the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim
       that you wrote the original software. If you use this software in a product,
       an acknowledgment in the product documentation would be appreciated but is
       not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _OT_WEBERTYPES_H_
#define _OT_WEBERTYPES_H_

#include <opentree/utils/otutil.h>

namespace opentree
{
  /// \addtogroup Weber
  /// @{

  enum ShapeType
  {
    CONICAL = 0,
    SPHERICAL = 1,
    HEMISPHERICAL = 2,
    CYLINDRICAL = 3,
    TAPERED_CYLINDRICAL = 4,
    FLAME = 5,
    INVERSE_CONICAL = 6,
    TEND_FLAME = 7,
    ENVELOPE = 8
  };

  class Trunk
  {
  public:
    float scale;
    float scaleV;
    float baseSplits;
    float dist;
  };

  class Level
  {
  public:
    int levelNumber;
    float downAngle;
    float downAngleV;
    float rotate;
    float rotateV;
    int branches;
    float branchDist;  //Used by Arbaro
    float length;
    float lengthV;
    float taper;
    float segSplits;
    float splitAngle;
    float splitAngleV;
    int curveRes;
    float curve;
    float curveBack;
    float curveV;
  };

  class TreeData
  {
  public:
    Trunk trunk;
    Level level[5];
    
    int shape;
    float baseSize;
    float scale;
    float scaleV;
    //float zScale;  //I have no clue where this is used -.-
    //float zScaleV; //I have no clue where this is used -.-
    int levels;
    float ratio;
    float ratioPower;
    int lobes;
    float lobeDepth;
    float flare;
    int leaves;
    float leafScale;
    float leafScaleX;
    float leafQuality; // mentioned in chapter 4.5 but not in the parameter list...
    int leafShapeRatio; // in the paper this is hardcoded to 4...
    float leafBend;
    float attractionUp;
    float pruneRatio;
    float prunePowerLow;
    float prunePowerHigh;
    float pruneWidth;
    float pruneWidthPeak;
    
    // For internal use
    float scale_tree;
    TreeData() : leafQuality(0.1f), scale_tree(10) {}
  };

  /// @}

}

#endif // _OT_WEBERTYPES_H_
