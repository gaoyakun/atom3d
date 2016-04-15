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

#ifndef _TESTXMLPARSER_H_
#define _TESTXMLPARSER_H_

#include "tinyxml/tinyxml.h"
#include "testerdata.h"

/*
 *XML Parser for the OpenTree Treegen and Webertest examples
 */

/// @{

class TestXmlParser
{
private:
  TiXmlDocument doc; 
  TiXmlNode* trees;
  TiXmlNode* tree;

  void GetParam (const char* &param, const char* name, TiXmlNode* node)
  {
    TiXmlElement* e = node->FirstChildElement(name);
    if (!e)
      param = "";
    else
      param = e->FirstChild()->ToText()->Value();
  }

public:
  TestXmlParser() : trees(0), tree(0) {}; 
  ~TestXmlParser() {};

  bool Load(const char* filename)
  {
    if (!doc.LoadFile(filename))
      return false;

    TiXmlHandle docHandle(&doc);
    trees = docHandle.FirstChild("trees").Node();

    return true;
  };

  bool getNext(TesterData& data)
  {
    while ((tree = trees->IterateChildren(tree))) // Extra parentheses to silence gcc
    {
      if (tree->Type() == TiXmlNode::ELEMENT)
      {
        GetParam (data.name, "name", tree);
        GetParam (data.algo, "algorithm", tree);
        GetParam (data.format, "format", tree);
        GetParam (data.inputfile, "inputfile", tree);
        GetParam (data.outputfile, "outputfile", tree);         
        return true;
      }
    }
    return false;
  }
};

/// }@

#endif // _TESTXMLPARSER_H_
