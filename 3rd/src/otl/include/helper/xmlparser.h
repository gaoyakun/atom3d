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

#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include "opentree/opentree.h"
#include "tinyxml/tinyxml.h"

namespace opentree
{

  /// \addtogroup Helper
  /// @{

  /// XML Parser for the OpenTree Tree format
  class xmlParser
  {
  private:
    void GetParam (float &param, const char* name, TiXmlNode* node)
    {
      TiXmlElement* e = node->FirstChildElement(name);
      if (!e)
        param = 0;
      else
        param = (float)atof(e->FirstChild()->ToText()->Value());
    }

    void GetParam (int &param, const char* name, TiXmlNode* node)
    {
      TiXmlElement* e = node->FirstChildElement(name);
      if (!e)
        param = 0;
      else
        param = atoi(e->FirstChild()->ToText()->Value());
    }

    void GetParam (const char* &param, const char* name, TiXmlNode* node)
    {
      TiXmlElement* e = node->FirstChildElement(name);
      if (!e)
        param = "";
      else
        param = e->FirstChild()->ToText()->Value();
    }

  public:
    xmlParser() {}
    ~xmlParser() {}

    bool Load(TreeData& params, const char* filename)
    {
      TiXmlDocument doc(filename);
      if (!doc.LoadFile())
        return false;
        
      TiXmlHandle docHandle(&doc);
      TiXmlNode* species = docHandle.FirstChild("opentree").FirstChild("species").Node();
      if (species)
      {
        GetParam(params.shape, "Shape", species);
        GetParam(params.levels, "Levels", species);
        GetParam(params.scale, "Scale", species);
        GetParam(params.scaleV, "ScaleV", species);
        GetParam(params.baseSize, "BaseSize", species);
        GetParam(params.ratio, "Ratio", species);
        GetParam(params.ratioPower, "RatioPower", species);
        GetParam(params.flare, "Flare", species);
        GetParam(params.lobes, "Lobes", species);
        GetParam(params.lobeDepth, "LobeDepth", species);
        //GetParam(params.smooth, "Smooth", species);
        GetParam(params.attractionUp, "AttractionUp", species);
        GetParam(params.leaves, "Leaves", species);
        //GetParam(params.leafShape, "LeafShape", species);
        GetParam(params.leafScale, "LeafScale", species);
        GetParam(params.leafScaleX, "LeafScaleX", species);
        GetParam(params.leafQuality, "LeafQuality", species);
        //GetParam(params.leafStemLen, "LeafStemLen", species);
        GetParam(params.leafShapeRatio, "LeafDistrib", species);
        GetParam(params.leafBend, "LeafBend", species);
        GetParam(params.leafScale, "LeafScale", species);
        GetParam(params.pruneRatio, "PruneRatio", species);
        GetParam(params.prunePowerLow, "PrunePowerLow", species);
        GetParam(params.prunePowerHigh, "PrunePowerHigh", species);
        GetParam(params.pruneWidth, "PruneWidth", species);
        GetParam(params.pruneWidthPeak, "PruneWidthPeak", species);
        
        TiXmlElement* level = 0;
        while ((level = species->IterateChildren("level", level)->ToElement()) != 0)
        {
          int l;
          if (level->QueryIntAttribute("level", &l) != TIXML_SUCCESS)
            return false;
          params.level[l].levelNumber = l;
          Level* lp = &params.level[l];
          
          if (l == 0)
          {
            GetParam(params.trunk.scale, "Scale", level);
            GetParam(params.trunk.scaleV, "ScaleV", level);
            GetParam(params.trunk.baseSplits, "BaseSplits", level);
            GetParam(params.trunk.dist, "BranchDist", level);
          }
          
          GetParam(lp->downAngle, "DownAngle", level);
          GetParam(lp->downAngleV, "DownAngleV", level);
          GetParam(lp->rotate, "Rotate", level);
          GetParam(lp->rotateV, "RotateV", level);
          GetParam(lp->branches, "Branches", level);
          GetParam(lp->branchDist, "BranchDist", level); // Used by Arbaro
          GetParam(lp->length, "Length", level);
          GetParam(lp->lengthV, "LengthV", level);
          GetParam(lp->taper, "Taper", level);
          GetParam(lp->segSplits, "SegSplits", level);
          GetParam(lp->splitAngle, "SplitAngle", level);
          GetParam(lp->splitAngleV, "SplitAngleV", level);
          GetParam(lp->curveRes, "CurveRes", level);
          GetParam(lp->curve, "Curve", level);
          GetParam(lp->curveBack, "CurveBack", level);
          GetParam(lp->curveV, "CurveV", level);
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

#endif // _XMLPARSER_H_
