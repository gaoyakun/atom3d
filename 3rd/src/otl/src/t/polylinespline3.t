#include "opentree/utils/otpolylinespline3.h"
#include "stdio.h"

using namespace opentree;

class otPolyLineSpline3Test : public CppUnit::TestFixture
{
  static const float EPSILON = 0.0001f;

public:
  void setUp()
  {
  }

  void tearDown()
  {
  }

  bool nearEqual(float a, float b)
  {
    return (fabs(a - b) <= EPSILON);
  }

  void testInitial()
  {
    otPolyLineSpline3 s;
    CPPUNIT_ASSERT(s.getLength() == 0);
    s.setPoint(0, otVector3(0, 0, 0));
    CPPUNIT_ASSERT(s.getLength() == 0);
  }

  void testLength()
  {
    otPolyLineSpline3 s(2);
    s.setPoint(0, otVector3(0, 0, 0));
    s.setPoint(1, otVector3(1, 0, 0));
    CPPUNIT_ASSERT(nearEqual(s.getLength(), 1));

    s.setPoint(2, otVector3(1, 1, 0));
    CPPUNIT_ASSERT(nearEqual(s.getLength(), 2));
  }

  void testFindNearest()
  {
    /*
             1
           |
           |
       ____| 
             0
      0    1
    */
    otPolyLineSpline3 s(2);
    s.setPoint(0, otVector3(0, 0, 0));
    CPPUNIT_ASSERT(s.findNearest(otVector3(0, 99, 0)) == otVector3(0, 0, 0));
    s.setPoint(1, otVector3(1, 0, 0));
    CPPUNIT_ASSERT(s.findNearest(otVector3(2, 1, 0)) == otVector3(2, 0, 0));
    CPPUNIT_ASSERT(s.findNearest(otVector3(.4, .5, 0)) == otVector3(.4, 0, 0));
    CPPUNIT_ASSERT(s.findNearest(otVector3(.6, .75, 0)) == otVector3(.6, 0, 0));
    s.setPoint(2, otVector3(1, 1, 0));
    CPPUNIT_ASSERT(s.findNearest(otVector3(1, 2, 1)) == otVector3(1, 2, 0));
    CPPUNIT_ASSERT(s.findNearest(otVector3(.6, .5, 0)) == otVector3(1, .5, 0));
  }

  CPPUNIT_TEST_SUITE(otPolyLineSpline3Test);
    CPPUNIT_TEST(testInitial);
    CPPUNIT_TEST(testLength);
    CPPUNIT_TEST(testFindNearest);
  CPPUNIT_TEST_SUITE_END();
};
