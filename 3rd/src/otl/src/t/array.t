#include "opentree/utils/otarray.h"

using namespace opentree;

class otArrayTest : public CppUnit::TestFixture
{
  otArray<int> intArray;

public:
  void setUp()
  {
  }

  void tearDown()
  {
  }

  void testInitial()
  {
    CPPUNIT_ASSERT(intArray.getCount() == 0);
  }

  void testAdd()
  {
    intArray.add(1);
    CPPUNIT_ASSERT(intArray.getCount() == 1);
    intArray.add(2);
    CPPUNIT_ASSERT(intArray.getCount() == 2);
    CPPUNIT_ASSERT(intArray[0] == 1 && intArray[1] == 2);
  }

  void testRemove()
  {
    intArray.add(1);
    intArray.add(2);
    intArray.remove(1);
    CPPUNIT_ASSERT(intArray.getCount() == 1);
    CPPUNIT_ASSERT(intArray[0] == 1);
  }

  CPPUNIT_TEST_SUITE(otArrayTest);
    CPPUNIT_TEST(testInitial);
    CPPUNIT_TEST(testAdd);
    CPPUNIT_TEST(testRemove);
  CPPUNIT_TEST_SUITE_END();
};
