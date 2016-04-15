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

#ifndef _XMLPARSERARBARO_H_
#define _XMLPARSERARBARO_H_

#include "opentree/opentree.h"
#include "tinyxml/tinyxml.h"

namespace opentree
{

  /// \addtogroup Helper
  /// @{

  /** XML Parser for the Arbaro Tree format.
  * Since Arbaro provides a great tool for modeling weber trees, it is helpful
  * to have a parser which imports the arbaro xml format.
  */
  class xmlParserArbaro
  {
  private:
    void GetParam (float &param, const char* name, TiXmlNode* node)
    {
      for( TiXmlElement* child = node->FirstChildElement(); child; child = child->NextSiblingElement() )
      {
        const char* e_name  = child->Attribute("name");
        if (!strcmp(name,e_name))
        {
          double f = 0;
          child->Attribute("value", &f);
          param = (float)f;
          return;
        }
      }
      param = 0;
    }

    void GetParam (int &param, const char* name, TiXmlNode* node)
    {
      for( TiXmlElement* child = node->FirstChildElement(); child; child = child->NextSiblingElement() )
      {
        const char* e_name  = child->Attribute("name");
        if (!strcmp(name,e_name))
        {
          int i;
          child->Attribute("value", &i);
          param = i;
          return;
        }
      }
      param = 0;
    }

    void GetParam (const char* &param, const char* name, TiXmlNode* node)
    {
      for( TiXmlElement* child = node->FirstChildElement(); child; child = child->NextSiblingElement() )
      {
        const char* e_name  = child->Attribute("name");
        if (!strcmp(name,e_name))
        {
          const char* s = child->Attribute("value");
          if (s) param = s;
        }
      }
    }

  public:
    xmlParserArbaro() {}
    ~xmlParserArbaro() {}

    bool Load(TreeData& params, const char* filename)
    {
      TiXmlDocument doc(filename);
      if (!doc.LoadFile())
        return false;
        
      TiXmlHandle docHandle(&doc);
      TiXmlNode* species = docHandle.FirstChild("arbaro").FirstChild("species").Node();
      if (species)
      {
        GetParam(params.shape, "Shape", species);
        GetParam(params.levels, "Levels", species);
        GetParam(params.scale, "Scale", species);
        GetParam(params.scaleV, "ScaleV", species);
        GetParam(params.baseSize, "BaseSize", species);
        GetParam(params.levels, "Levels", species);
        GetParam(params.ratio, "Ratio", species);
        GetParam(params.ratioPower, "RatioPower", species);
        GetParam(params.flare, "Flare", species);
        GetParam(params.lobes, "Lobes", species);
        GetParam(params.lobeDepth, "LobeDepth", species);
        GetParam(params.attractionUp, "AttractionUp", species);
        GetParam(params.leaves, "Leaves", species);
        GetParam(params.leafScale, "LeafScale", species);
        GetParam(params.leafScaleX, "LeafScaleX", species);
        GetParam(params.leafQuality, "LeafQuality", species);
        GetParam(params.leafShapeRatio, "LeafDistrib", species);
        GetParam(params.leafBend, "LeafBend", species);
        GetParam(params.leafScale, "LeafScale", species);
        GetParam(params.pruneRatio, "PruneRatio", species);
        GetParam(params.prunePowerLow, "PrunePowerLow", species);
        GetParam(params.prunePowerHigh, "PrunePowerHigh", species);
        GetParam(params.pruneWidth, "PruneWidth", species);
        GetParam(params.pruneWidthPeak, "PruneWidthPeak", species);

        GetParam(params.trunk.scale, "0Scale", species);
        GetParam(params.trunk.scaleV, "0ScaleV", species);
        GetParam(params.trunk.baseSplits, "0BaseSplits", species);
        GetParam(params.trunk.dist,  "0BranchDist",  species);

        if (params.levels >= 0)
        {
          params.level[0].levelNumber = 0;
          GetParam(params.level[0].downAngle,   "0DownAngle",   species);
          GetParam(params.level[0].downAngleV,  "0DownAngleV",  species);
          GetParam(params.level[0].rotate,      "0Rotate",      species);
          GetParam(params.level[0].rotateV,     "0RotateV",     species);

          GetParam(params.level[0].branches,    "0Branches",    species);
          // HELP, I don't understand Arbaro here! Why is 0Branches = 0? :)
          if (params.level[0].branches == 0) params.level[0].branches = 1;

          GetParam(params.level[0].branchDist,  "0BranchDist",  species);
          GetParam(params.level[0].length,      "0Length",      species);
          GetParam(params.level[0].lengthV,     "0LengthV",     species);
          GetParam(params.level[0].taper,       "0Taper",       species);
          GetParam(params.level[0].segSplits,   "0SegSplits",   species);
          GetParam(params.level[0].splitAngle,  "0SplitAngle",  species);
          GetParam(params.level[0].splitAngleV, "0SplitAngleV", species);
          GetParam(params.level[0].curveRes,    "0CurveRes",    species);
          GetParam(params.level[0].curve,       "0Curve",       species);
          GetParam(params.level[0].curveBack,   "0CurveBack",   species);
          GetParam(params.level[0].curveV,      "0CurveV",      species);
        }
        if (params.levels >= 1)
        {
          params.level[1].levelNumber = 1;
          GetParam(params.level[1].downAngle,   "1DownAngle",   species);
          GetParam(params.level[1].downAngleV,  "1DownAngleV",  species);
          GetParam(params.level[1].rotate,      "1Rotate",      species);
          GetParam(params.level[1].rotateV,     "1RotateV",     species);
          GetParam(params.level[1].branches,    "1Branches",    species);
          GetParam(params.level[1].branchDist,  "1BranchDist",  species);
          GetParam(params.level[1].length,      "1Length",      species);
          GetParam(params.level[1].lengthV,     "1LengthV",     species);
          GetParam(params.level[1].taper,       "1Taper",       species);
          GetParam(params.level[1].segSplits,   "1SegSplits",   species);
          GetParam(params.level[1].splitAngle,  "1SplitAngle",  species);
          GetParam(params.level[1].splitAngleV, "1SplitAngleV", species);
          GetParam(params.level[1].curveRes,    "1CurveRes",    species);
          GetParam(params.level[1].curve,       "1Curve",       species);
          GetParam(params.level[1].curveBack,   "1CurveBack",   species);
          GetParam(params.level[1].curveV,      "1CurveV",      species);
        }
        if (params.levels >= 2)
        {
          params.level[2].levelNumber = 2;
          GetParam(params.level[2].downAngle,   "2DownAngle",   species);
          GetParam(params.level[2].downAngleV,  "2DownAngleV",  species);
          GetParam(params.level[2].rotate,      "2Rotate",      species);
          GetParam(params.level[2].rotateV,     "2RotateV",     species);
          GetParam(params.level[2].branches,    "2Branches",    species);
          GetParam(params.level[2].branchDist,  "2BranchDist",  species);
          GetParam(params.level[2].length,      "2Length",      species);
          GetParam(params.level[2].lengthV,     "2LengthV",     species);
          GetParam(params.level[2].taper,       "2Taper",       species);
          GetParam(params.level[2].segSplits,   "2SegSplits",   species);
          GetParam(params.level[2].splitAngle,  "2SplitAngle",  species);
          GetParam(params.level[2].splitAngleV, "2SplitAngleV", species);
          GetParam(params.level[2].curveRes,    "2CurveRes",    species);
          GetParam(params.level[2].curve,       "2Curve",       species);
          GetParam(params.level[2].curveBack,   "2CurveBack",   species);
          GetParam(params.level[2].curveV,      "2CurveV",      species);
        }
        if (params.levels >= 3)
        {
          params.level[3].levelNumber = 3;
          GetParam(params.level[3].downAngle,   "3DownAngle",   species);
          GetParam(params.level[3].downAngleV,  "3DownAngleV",  species);
          GetParam(params.level[3].rotate,      "3Rotate",      species);
          GetParam(params.level[3].rotateV,     "3RotateV",     species);
          GetParam(params.level[3].branches,    "3Branches",    species);
          GetParam(params.level[3].branchDist,  "3BranchDist",  species);
          GetParam(params.level[3].length,      "3Length",      species);
          GetParam(params.level[3].lengthV,     "3LengthV",     species);
          GetParam(params.level[3].taper,       "3Taper",       species);
          GetParam(params.level[3].segSplits,   "3SegSplits",   species);
          GetParam(params.level[3].splitAngle,  "3SplitAngle",  species);
          GetParam(params.level[3].splitAngleV, "3SplitAngleV", species);
          GetParam(params.level[3].curveRes,    "3CurveRes",    species);
          GetParam(params.level[3].curve,       "3Curve",       species);
          GetParam(params.level[3].curveBack,   "3CurveBack",   species);
          GetParam(params.level[3].curveV,      "3CurveV",      species);
        }
        if (params.levels >= 4)
        {
          params.level[4].levelNumber = 4;
          GetParam(params.level[4].downAngle,   "4DownAngle",   species);
          GetParam(params.level[4].downAngleV,  "4DownAngleV",  species);
          GetParam(params.level[4].rotate,      "4Rotate",      species);
          GetParam(params.level[4].rotateV,     "4RotateV",     species);
          GetParam(params.level[4].branches,    "4Branches",    species);
          GetParam(params.level[4].branchDist,  "4BranchDist",  species);
          GetParam(params.level[4].length,      "4Length",      species);
          GetParam(params.level[4].lengthV,     "4LengthV",     species);
          GetParam(params.level[4].taper,       "4Taper",       species);
          GetParam(params.level[4].segSplits,   "4SegSplits",   species);
          GetParam(params.level[4].splitAngle,  "4SplitAngle",  species);
          GetParam(params.level[4].splitAngleV, "4SplitAngleV", species);
          GetParam(params.level[4].curveRes,    "4CurveRes",    species);
          GetParam(params.level[4].curve,       "4Curve",       species);
          GetParam(params.level[4].curveBack,   "4CurveBack",   species);
          GetParam(params.level[4].curveV,      "4CurveV",      species);
        }

      }
      else
      {
        return false;
      }
      
      return true;
    }
  };

  /// @}

}

#endif // _XMLPARSERARBARO_H_
