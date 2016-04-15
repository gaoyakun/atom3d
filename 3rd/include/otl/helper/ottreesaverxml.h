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

#ifndef _OT_OTTREESAVERXML_H_
#define _OT_OTTREESAVERXML_H_

#include "opentree/ottree.h"
#include "opentree/otstem.h"
#include "opentree/otleaf.h"
#include "tinyxml/tinyxml.h"

namespace opentree
{
  /// \addtogroup Helper
  /// @{

  /** Serializes Trees
   * This class allows the serialization of a otTree classes.
   * It returns a XML string of all stems and leaves.
   */
  class otTreeSaverXML
  {
  private:

    TiXmlDocument doc;

    TiXmlNode* root;

    void saveTree(otTree* tree)
    {
      TiXmlNode* xmltree = root->InsertEndChild(TiXmlElement("tree"));

      xmltree->ToElement()->SetAttribute("Children", tree->getTrunkCount());

      for (unsigned int i = 0; i < tree->getTrunkCount(); i++)
      {
        saveStem(tree->getTrunk(i),xmltree);
      }
    }

    void saveStem(otStem* stem, TiXmlNode* parent)
    {
      TiXmlNode* xmlstem = parent->InsertEndChild(TiXmlElement("stem"));

      xmlstem->ToElement()->SetAttribute("level", stem->levelNo);
      xmlstem->ToElement()->SetAttribute("Leaves", stem->getLeafCount());
      xmlstem->ToElement()->SetAttribute("Children", stem->getChildCount());

      savePosSpline(stem->getPosSpline(),xmlstem);
      saveRadSpline(stem->getRadSpline(),xmlstem);

      for (unsigned int i = 0; i < stem->getLeafCount(); i++)
      {
        saveLeaf(stem->getLeaf(i),xmlstem);
      }

      for (unsigned int i = 0; i < stem->getChildCount(); i++)
      {
        saveStem(stem->getChild(i),xmlstem);
      }
    }

    void saveLeaf(otLeaf& leaf, TiXmlNode* parent)
    {
      TiXmlNode* xmlleaf = parent->InsertEndChild(TiXmlElement("leaf"));
      xmlleaf->ToElement()->SetDoubleAttribute("length", leaf.length);
      xmlleaf->ToElement()->SetDoubleAttribute("width", leaf.width);

      float* M = leaf.transform;
      TiXmlNode* xmltransf = xmlleaf->InsertEndChild(TiXmlElement("transformation"));
      TiXmlNode* row1 = xmltransf->InsertEndChild(TiXmlElement("row1"));
      row1->ToElement()->SetDoubleAttribute("col1", M[0]);
      row1->ToElement()->SetDoubleAttribute("col2", M[1]);
      row1->ToElement()->SetDoubleAttribute("col3", M[2]);
      row1->ToElement()->SetDoubleAttribute("col4", M[3]);
      TiXmlNode* row2 = xmltransf->InsertEndChild(TiXmlElement("row2"));
      row2->ToElement()->SetDoubleAttribute("col1", M[4]);
      row2->ToElement()->SetDoubleAttribute("col2", M[5]);
      row2->ToElement()->SetDoubleAttribute("col3", M[6]);
      row2->ToElement()->SetDoubleAttribute("col4", M[7]);
      TiXmlNode* row3 = xmltransf->InsertEndChild(TiXmlElement("row3"));
      row3->ToElement()->SetDoubleAttribute("col1", M[8]);
      row3->ToElement()->SetDoubleAttribute("col2", M[9]);
      row3->ToElement()->SetDoubleAttribute("col3", M[10]);
      row3->ToElement()->SetDoubleAttribute("col4", M[11]);
      TiXmlNode* row4 = xmltransf->InsertEndChild(TiXmlElement("row4"));
      row4->ToElement()->SetDoubleAttribute("col1", M[12]);
      row4->ToElement()->SetDoubleAttribute("col2", M[13]);
      row4->ToElement()->SetDoubleAttribute("col3", M[14]);
      row4->ToElement()->SetDoubleAttribute("col4", M[15]);
    }

    void savePosSpline(otPolyLineSpline3* spline, TiXmlNode* parent)
    {
      TiXmlNode* xmlspline = parent->InsertEndChild(TiXmlElement("posspline"));
      xmlspline->ToElement()->SetAttribute("Points", spline->getPointList().getCount());

      for (unsigned int i=0; i<spline->getPointList().getCount(); i++)
      {
        TiXmlNode* xmlpoint = xmlspline->InsertEndChild(TiXmlElement("point3d"));
        otPolyLineSpline3::Point p = spline->getPointList().get(i);
        xmlpoint->ToElement()->SetDoubleAttribute("X", p.x);
        xmlpoint->ToElement()->SetDoubleAttribute("Y.x", p.y.X);
        xmlpoint->ToElement()->SetDoubleAttribute("Y.y", p.y.Y);
        xmlpoint->ToElement()->SetDoubleAttribute("Y.z", p.y.Z);
      }
    }

    void saveRadSpline(otPolyLineSpline* spline, TiXmlNode* parent)
    {
      TiXmlNode* xmlspline = parent->InsertEndChild(TiXmlElement("radspline"));
      xmlspline->ToElement()->SetAttribute("Points", spline->getPointList().getCount());

      for (unsigned int i=0; i<spline->getPointList().getCount(); i++)
      {
        TiXmlNode* xmlpoint = xmlspline->InsertEndChild(TiXmlElement("point"));
        otPolyLineSpline::Point p = spline->getPointList().get(i);
        xmlpoint->ToElement()->SetDoubleAttribute("X", p.x);
        xmlpoint->ToElement()->SetDoubleAttribute("Y", p.y);
      }
    }

  public:
    otTreeSaverXML() : root(0) {}
    ~otTreeSaverXML() {}

    /// Writes output to #data and return length of the string.
    void save(otTree* tree, const char* filename)
    {
      doc.InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", ""));
      root = doc.InsertEndChild(TiXmlElement("ottree"));
      saveTree(tree);
      doc.SaveFile(filename);
    }
  };

  /// @}
}

#endif //_OT_OTTREESAVERXML_H_
