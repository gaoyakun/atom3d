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

#ifndef _XMLSAVERARBARO_H_
#define _XMLSAVERARBARO_H_

#include "opentree/opentree.h"
#include "tinyxml/tinyxml.h"

namespace opentree
{

  /// \addtogroup Helper
  /// @{

  /** XML Saver for the Arbaro Tree format.
  * For completeness we allow the export to the arbaro format now too.
  */
  class xmlSaverArbaro
  {
  private:
    void SetParam (float param, const char* name, TiXmlNode* node)
    {
      char strbuf[256];
      sprintf(strbuf, "%.8f", param);

      TiXmlNode* paramnode = node->InsertEndChild(TiXmlElement(name));
      paramnode->InsertEndChild(TiXmlText(strbuf));
    }

    void SetParam (int param, const char* name, TiXmlNode* node)
    {
      char strbuf[256];
      sprintf(strbuf, "%d", param);

      TiXmlNode* paramnode = node->InsertEndChild(TiXmlElement(name));
      paramnode->InsertEndChild(TiXmlText(strbuf));
    }

    void SetParam (const char* param, const char* name, TiXmlNode* node)
    {
      TiXmlNode* paramnode = node->InsertEndChild(TiXmlElement(name));
      paramnode->InsertEndChild(TiXmlText(param));
    }

  public:
    xmlSaverArbaro() {}
    ~xmlSaverArbaro() {}

    bool Save(TreeData& params, const char* filename)
    {
      TiXmlDocument doc;

      doc.InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", ""));
      TiXmlNode* root = doc.InsertEndChild(TiXmlElement("arbaro"));
      TiXmlNode* species = root->InsertEndChild(TiXmlElement("species"));

      SetParam(params.shape, "Shape", species);
      SetParam(params.levels, "Levels", species);
      SetParam(params.scale, "Scale", species);
      SetParam(params.scaleV, "ScaleV", species);
      SetParam(params.baseSize, "BaseSize", species);
      SetParam(params.levels, "Levels", species);
      SetParam(params.ratio, "Ratio", species);
      SetParam(params.ratioPower, "RatioPower", species);
      SetParam(params.flare, "Flare", species);
      SetParam(params.lobes, "Lobes", species);
      SetParam(params.lobeDepth, "LobeDepth", species);
      SetParam(params.attractionUp, "AttractionUp", species);
      SetParam(params.leaves, "Leaves", species);
      SetParam(params.leafScale, "LeafScale", species);
      SetParam(params.leafScaleX, "LeafScaleX", species);
      SetParam(params.leafQuality, "LeafQuality", species);
      SetParam(params.leafShapeRatio, "LeafDistrib", species);
      SetParam(params.leafBend, "LeafBend", species);
      SetParam(params.leafScale, "LeafScale", species);
      SetParam(params.pruneRatio, "PruneRatio", species);
      SetParam(params.prunePowerLow, "PrunePowerLow", species);
      SetParam(params.prunePowerHigh, "PrunePowerHigh", species);
      SetParam(params.pruneWidth, "PruneWidth", species);
      SetParam(params.pruneWidthPeak, "PruneWidthPeak", species);

      SetParam(params.trunk.scale, "0Scale", species);
      SetParam(params.trunk.scaleV, "0ScaleV", species);
      SetParam(params.trunk.baseSplits, "0BaseSplits", species);
      SetParam(params.trunk.dist,  "0BranchDist",  species);

      if (params.levels >= 0)
      {
        params.level[0].levelNumber = 0;
        SetParam(params.level[0].downAngle,   "0DownAngle",   species);
        SetParam(params.level[0].downAngleV,  "0DownAngleV",  species);
        SetParam(params.level[0].rotate,      "0Rotate",      species);
        SetParam(params.level[0].rotateV,     "0RotateV",     species);
        SetParam(params.level[0].branches,    "0Branches",    species);
        SetParam(params.level[0].branchDist,  "0BranchDist",  species);
        SetParam(params.level[0].length,      "0Length",      species);
        SetParam(params.level[0].lengthV,     "0LengthV",     species);
        SetParam(params.level[0].taper,       "0Taper",       species);
        SetParam(params.level[0].segSplits,   "0SegSplits",   species);
        SetParam(params.level[0].splitAngle,  "0SplitAngle",  species);
        SetParam(params.level[0].splitAngleV, "0SplitAngleV", species);
        SetParam(params.level[0].curveRes,    "0CurveRes",    species);
        SetParam(params.level[0].curve,       "0Curve",       species);
        SetParam(params.level[0].curveBack,   "0CurveBack",   species);
        SetParam(params.level[0].curveV,      "0CurveV",      species);
      }
      if (params.levels >= 1)
      {
        params.level[1].levelNumber = 1;
        SetParam(params.level[1].downAngle,   "1DownAngle",   species);
        SetParam(params.level[1].downAngleV,  "1DownAngleV",  species);
        SetParam(params.level[1].rotate,      "1Rotate",      species);
        SetParam(params.level[1].rotateV,     "1RotateV",     species);
        SetParam(params.level[1].branches,    "1Branches",    species);
        SetParam(params.level[1].branchDist,  "1BranchDist",  species);
        SetParam(params.level[1].length,      "1Length",      species);
        SetParam(params.level[1].lengthV,     "1LengthV",     species);
        SetParam(params.level[1].taper,       "1Taper",       species);
        SetParam(params.level[1].segSplits,   "1SegSplits",   species);
        SetParam(params.level[1].splitAngle,  "1SplitAngle",  species);
        SetParam(params.level[1].splitAngleV, "1SplitAngleV", species);
        SetParam(params.level[1].curveRes,    "1CurveRes",    species);
        SetParam(params.level[1].curve,       "1Curve",       species);
        SetParam(params.level[1].curveBack,   "1CurveBack",   species);
        SetParam(params.level[1].curveV,      "1CurveV",      species);
      }
      if (params.levels >= 2)
      {
        params.level[2].levelNumber = 2;
        SetParam(params.level[2].downAngle,   "2DownAngle",   species);
        SetParam(params.level[2].downAngleV,  "2DownAngleV",  species);
        SetParam(params.level[2].rotate,      "2Rotate",      species);
        SetParam(params.level[2].rotateV,     "2RotateV",     species);
        SetParam(params.level[2].branches,    "2Branches",    species);
        SetParam(params.level[2].branchDist,  "2BranchDist",  species);
        SetParam(params.level[2].length,      "2Length",      species);
        SetParam(params.level[2].lengthV,     "2LengthV",     species);
        SetParam(params.level[2].taper,       "2Taper",       species);
        SetParam(params.level[2].segSplits,   "2SegSplits",   species);
        SetParam(params.level[2].splitAngle,  "2SplitAngle",  species);
        SetParam(params.level[2].splitAngleV, "2SplitAngleV", species);
        SetParam(params.level[2].curveRes,    "2CurveRes",    species);
        SetParam(params.level[2].curve,       "2Curve",       species);
        SetParam(params.level[2].curveBack,   "2CurveBack",   species);
        SetParam(params.level[2].curveV,      "2CurveV",      species);
      }
      if (params.levels >= 3)
      {
        params.level[3].levelNumber = 3;
        SetParam(params.level[3].downAngle,   "3DownAngle",   species);
        SetParam(params.level[3].downAngleV,  "3DownAngleV",  species);
        SetParam(params.level[3].rotate,      "3Rotate",      species);
        SetParam(params.level[3].rotateV,     "3RotateV",     species);
        SetParam(params.level[3].branches,    "3Branches",    species);
        SetParam(params.level[3].branchDist,  "3BranchDist",  species);
        SetParam(params.level[3].length,      "3Length",      species);
        SetParam(params.level[3].lengthV,     "3LengthV",     species);
        SetParam(params.level[3].taper,       "3Taper",       species);
        SetParam(params.level[3].segSplits,   "3SegSplits",   species);
        SetParam(params.level[3].splitAngle,  "3SplitAngle",  species);
        SetParam(params.level[3].splitAngleV, "3SplitAngleV", species);
        SetParam(params.level[3].curveRes,    "3CurveRes",    species);
        SetParam(params.level[3].curve,       "3Curve",       species);
        SetParam(params.level[3].curveBack,   "3CurveBack",   species);
        SetParam(params.level[3].curveV,      "3CurveV",      species);
      }
      if (params.levels >= 4)
      {
        params.level[4].levelNumber = 4;
        SetParam(params.level[4].downAngle,   "4DownAngle",   species);
        SetParam(params.level[4].downAngleV,  "4DownAngleV",  species);
        SetParam(params.level[4].rotate,      "4Rotate",      species);
        SetParam(params.level[4].rotateV,     "4RotateV",     species);
        SetParam(params.level[4].branches,    "4Branches",    species);
        SetParam(params.level[4].branchDist,  "4BranchDist",  species);
        SetParam(params.level[4].length,      "4Length",      species);
        SetParam(params.level[4].lengthV,     "4LengthV",     species);
        SetParam(params.level[4].taper,       "4Taper",       species);
        SetParam(params.level[4].segSplits,   "4SegSplits",   species);
        SetParam(params.level[4].splitAngle,  "4SplitAngle",  species);
        SetParam(params.level[4].splitAngleV, "4SplitAngleV", species);
        SetParam(params.level[4].curveRes,    "4CurveRes",    species);
        SetParam(params.level[4].curve,       "4Curve",       species);
        SetParam(params.level[4].curveBack,   "4CurveBack",   species);
        SetParam(params.level[4].curveV,      "4CurveV",      species);
      }

      if (!doc.SaveFile(filename))
        return false;
      
      return true;
    }
  };

  /// @}

}

#endif // _XMLSAVERARBARO_H_
