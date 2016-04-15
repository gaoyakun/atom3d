#include "opentree/utils/otvector3.h"

using namespace opentree;

class otVector3Test : public CppUnit::TestFixture
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
    otVector3 vec;
    CPPUNIT_ASSERT(vec.X == 0 && vec.Y == 0 && vec.Z == 0);
    CPPUNIT_ASSERT(vec.length() == 0);
  }

  void testNormalizeZeroVector()
  {
    otVector3 vec;
    // Should return another zero vector
    CPPUNIT_ASSERT(vec.normalize() == otVector3());
  }

  void testCopyConstructor()
  {
    otVector3 vec = otVector3(1, 2, 3);
    otVector3 vec2 (vec);
    CPPUNIT_ASSERT(vec2.X == vec.X && vec2.Y == vec.Y && vec2.Z == vec.Z);
  }

  void testLength()
  {
    otVector3 vec (1, 0, 0);
    otVector3 vec2 (0, 1, 0);
    otVector3 vec3 (0, 0, 1);
    CPPUNIT_ASSERT(vec.length() == 1 && vec2.length() == 1
      && vec3.length() == 1);

    vec = otVector3(cos(PI/4), sin(PI/4), 0);
    CPPUNIT_ASSERT(nearEqual(vec.length(), 1));
  }

  void testNormalize()
  {
    CPPUNIT_ASSERT(otVector3(0, 0, 0).normalize().length() == 0);
    CPPUNIT_ASSERT(otVector3(2, 0, 0).normalize().length() == 1);
    CPPUNIT_ASSERT(nearEqual(otVector3(2, 5, 6).normalize().length(), 1));
    CPPUNIT_ASSERT(nearEqual(otVector3(1, 2, 3).normalize().length(), 1));
  }


  // Arithmetic

  void testAddSubtract()
  {
    CPPUNIT_ASSERT(otVector3() + otVector3(1, 2, 3) == otVector3(1, 2, 3));
    CPPUNIT_ASSERT(otVector3(1, 2, 3) - otVector3(1, 2, 3) == otVector3());
  }

  void testDotProduct()
  {
    CPPUNIT_ASSERT((otVector3() | otVector3()) == 0);
    CPPUNIT_ASSERT((otVector3(1, 1, 1) | otVector3()) == 0);
    CPPUNIT_ASSERT((otVector3(1, 1, 1) | otVector3(1, 1, 1)) == 1+1+1);
    CPPUNIT_ASSERT((otVector3(1, 2, 1) | otVector3(1, 1, 1)) == 1+2+1);
    CPPUNIT_ASSERT((otVector3(1, 2, 1) | otVector3(1, 5, 1)) == 1+10+1);
    CPPUNIT_ASSERT((otVector3(1, 2, 3) | otVector3(4, 5, 6)) == 4+10+18);
  }

  void testCrossProduct()
  {
    CPPUNIT_ASSERT((otVector3() & otVector3()) == otVector3());

    otVector3 i (1, 0, 0);
    otVector3 j (0, 1, 0);
    otVector3 k (0, 0, 1);

    CPPUNIT_ASSERT((i & j) == k);
    CPPUNIT_ASSERT((j & i) == -k);
    CPPUNIT_ASSERT((i & k) == -j);
    CPPUNIT_ASSERT((k & i) == j);
    CPPUNIT_ASSERT((j & k) == i);
    CPPUNIT_ASSERT((k & j) == -i);

    CPPUNIT_ASSERT((i & i) == otVector3());
    CPPUNIT_ASSERT((j & j) == otVector3());
    CPPUNIT_ASSERT((k & k) == otVector3());
  }


  // Rotation

  // TODO: Write rotation function test cases

  CPPUNIT_TEST_SUITE(otVector3Test);
    CPPUNIT_TEST(testInitial);
    CPPUNIT_TEST(testNormalizeZeroVector);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testLength);
    CPPUNIT_TEST(testNormalize);

    CPPUNIT_TEST(testAddSubtract);
    CPPUNIT_TEST(testDotProduct);
    CPPUNIT_TEST(testCrossProduct);
  CPPUNIT_TEST_SUITE_END();
};
