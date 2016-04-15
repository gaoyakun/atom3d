#include <opentree/weber/webertypes.h>
#include <opentree/ottree.h>

#include "testxmlparser.h"
#include "testerdata.h"

/** \addtogroup TreeGen
 * Generates Weber Trees and writes them down into a file.
 * Great to create pregenerated tree before refactoring the weber algorithm.
 */

/// @{

class TreeGen
{
private:
  opentree::TreeData params;
  opentree::otTree* tree;
  int parse(const char* format, const char* file);
  int generate(const char* algo);
  int save(const char* file);

public:
  TreeGen() : tree(0) {}
  ~TreeGen() {}

  int Run(const char* file);
};

/// }@
