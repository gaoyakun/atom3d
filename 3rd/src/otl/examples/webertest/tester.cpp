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

#include "tester.h"
#include "testerdata.h"
#include "helper/xmlparser.h"
#include "helper/xmlparserarbaro.h"

#include "opentree/ottree.h"

#include "opentree/weber/weber.h"
#include "helper/ottreeloader.h"

#include "opentree/utils/otcompare.h"

#include <time.h>

opentree::otTree* Tester::createTree(const char* file, const char* format, const char* algo)
{
  opentree::TreeData params;

  if(!strncmp(format, "otl-xml", 7))
  {
    opentree::xmlParser parser;
    if (!parser.Load(params, file))
      return 0;
  }
  else if(!strncmp(format, "arbaro-xml", 7))
  {
    opentree::xmlParserArbaro parser;
    if (!parser.Load(params, file))
      return 0;
  }

  opentree::iWeber* gen = 0;
  if(!strncmp(algo, "otl-weber", 7))
  {
    gen = opentree::newWeber();
  }
  else if(!strncmp(algo, "arbaro-weber", 7))
  {
    gen = opentree::newWeberArbaro();
  }

  gen->setParams(params);
  opentree::otTree* tree = gen->generate();
  delete gen;
  return tree;
}

opentree::otTree* Tester::loadPregenTree(const char* filename)
{
  FILE* file = fopen(filename, "rb");
  if (file)
  {
    fseek(file, 0,SEEK_END);
    unsigned int size = ftell(file);
    fseek(file, 0,SEEK_SET);

    char* params = new char[size];
    fread(params, 1, size, file);
    fclose(file);
  
    opentree::otTreeLoader loader;
    opentree::otTree* tree = loader.load(params);
    delete [] params;
    return tree;
  }
  else 
    return 0;
}

bool Tester::test(TesterData& data)
{
  opentree::setRandomSeed(4);
  opentree::otTree* tree1 = 0;

  unsigned int gen_start = clock();
  tree1 = createTree(data.inputfile, data.format, data.algo);
  if (!tree1) 
    return false;
  unsigned int gen_time = clock() - gen_start;

  unsigned int load_start = clock();
  opentree::otTree* tree2 = loadPregenTree(data.outputfile);
  if (!tree2)
  {
    delete tree1;
    return false;
  }
  unsigned int load_time = clock() - load_start;

  printf(" {%d,%d} ",gen_time,load_time);

  opentree::otCompare comp;
  bool retval = comp.compare(tree1, tree2);

  delete tree1;
  delete tree2;

  return retval;
}

void Tester::Run(const char* file)
{
  TesterData data;
  TestXmlParser parser;
  
  if (!parser.Load(file))
  {
    printf ("Error parsing the xml configuration file\n");
    return; 
  }
  
  while(parser.getNext(data))
  {
    printf ("Checking %s... ", data.name);

    bool resval = test(data);

    if (resval)
      printf("[ succeeded ]\n");
    else
      printf("[ failed ]\n");
  }
}
