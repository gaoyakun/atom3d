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

#ifndef _OT_OTMATRIX_H_
#define _OT_OTMATRIX_H_

#ifdef _DEBUG
  #include "stdio.h"
#endif

#include "otvector3.h"
#include "otutil.h"

namespace opentree
{
  /// \addtogroup Utilities
  /// @{

  /** 4x4 Transformation Matrix
   * 
   */
  class otMatrix
  {

  private:
    /** Linear float Array.
     *  Contains the 16 values required for the 4x4 Matrix.
     *
     *  TODO: Format this nicely
     *  Structure:\n
     *  0  4  8  12      x.x   y.x   z.x   t.x\n
     *  1  5  9  13      x.y   y.y   z.y   t.y\n
     *  2  6  10 14      x.z   y.z   z.z   t.z\n
     *  3  7  11 15       ?     ?     ?     1\n
     *
     */
    float M[16];

  public:
    otMatrix()
    {
      identity(); 
    }

    ~otMatrix()
    {
    }

    /** Checks if the Matrix is valid.
     * If the matrix contains DIV#0 or INF, then it's not valid.
     */
    bool checkValid()
    {
      float sum = M[ 0]+M[ 1]+M[ 2]+M[ 3]
                + M[ 4]+M[ 5]+M[ 6]+M[ 7]
                + M[ 8]+M[ 9]+M[10]+M[11]
                + M[12]+M[13]+M[14]+M[15];

      if (!(sum < 0.0) && !(sum >= 0.0))
      {
        #ifdef _DEBUG
          printf("\n\n\n\n !!! INVALID MATIX !!! \n");
          printf("%.2f\t%.2f\t%.2f\t%.2f\n", M[0], M[4], M[ 8], M[12]);
          printf("%.2f\t%.2f\t%.2f\t%.2f\n", M[1], M[5], M[ 9], M[13]);
          printf("%.2f\t%.2f\t%.2f\t%.2f\n", M[2], M[6], M[10], M[14]);
          printf("%.2f\t%.2f\t%.2f\t%.2f\n", M[3], M[7], M[11], M[15]);
        #endif
        return false;
      }
      return true;
    }

    /// Creates an identity Matrix.
    void identity()
    {
      M[ 0] = 1; M[ 1] = 0; M[ 2] = 0; M[ 3] = 0;
      M[ 4] = 0; M[ 5] = 1; M[ 6] = 0; M[ 7] = 0;
      M[ 8] = 0; M[ 9] = 0; M[10] = 1; M[11] = 0;
      M[12] = 0; M[13] = 0; M[14] = 0; M[15] = 1;
    }

    /// Creates an zero Matrix.
    void zero()
    {
      M[ 0] = 0; M[ 1] = 0; M[ 2] = 0; M[ 3] = 0;
      M[ 4] = 0; M[ 5] = 0; M[ 6] = 0; M[ 7] = 0;
      M[ 8] = 0; M[ 9] = 0; M[10] = 0; M[11] = 0;
      M[12] = 0; M[13] = 0; M[14] = 0; M[15] = 0;
    }

    /// Sets the translation to x, y, z
    void translate(float x, float y, float z)
    {
      M[12] += x;
      M[13] += y;
      M[14] += z;
    }

    /// Sets the translation to v
    inline void translate(const otVector3& v)
    {
      translate(v.X, v.Y, v.Z); 
    }

    /// Gets the translation of the matrix
    inline otVector3 getTranslation()
    {
      return otVector3(M[12],M[13],M[14]); 
    }

    /** Uses one matrix to rotate another
     * This is a simplified version of the matrix multiplication.
     * It does only translate, therefor we save flops
     */
    otMatrix& rot(const otMatrix& other)
    {
      const float *N = other.M;
      float R[16];

      R[0]  = M[0]*N[0]  + M[4]*N[1]  + M[8] *N[2];
      R[1]  = M[1]*N[0]  + M[5]*N[1]  + M[9] *N[2];
      R[2]  = M[2]*N[0]  + M[6]*N[1]  + M[10]*N[2];
      R[3]  = 0;
      R[4]  = M[0]*N[4]  + M[4]*N[5]  + M[8] *N[6];
      R[5]  = M[1]*N[4]  + M[5]*N[5]  + M[9] *N[6];
      R[6]  = M[2]*N[4]  + M[6]*N[5]  + M[10]*N[6];
      R[7]  = 0;
      R[8]  = M[0]*N[8]  + M[4]*N[9]  + M[8] *N[10];
      R[9]  = M[1]*N[8]  + M[5]*N[9]  + M[9] *N[10];
      R[10] = M[2]*N[8]  + M[6]*N[9]  + M[10]*N[10];

      for (unsigned i = 0; i < 11; i++)
        M[i] = R[i];

  #ifdef _DEBUG
      checkValid();
  #endif

      return *this;
    }

    ///Rotates the matrix around #axis by #angle degrees
    inline void lrotateDegree(float angle, otVector3 axis)
    { lrotateRad(angle*D2RAD, axis); }

    ///Rotates the matrix around #axis by #angle degree
    void lrotateRad(float angle, otVector3 axis)
    {
      axis.normalize();
      float x = axis.X;
      float y = axis.Y;
      float z = axis.Z;
      float s = sin(angle);
      float c = cos(angle);

      otMatrix m;

      m.M[0] =   c +x*x*(1-c);
      m.M[1] =  z*s+x*y*(1-c);
      m.M[2] = -y*s+x*z*(1-c);
                        
      m.M[4] = -z*s+x*y*(1-c);
      m.M[5] =   c +y*y*(1-c);
      m.M[6] =  x*s+y*z*(1-c);
                        
      m.M[8] =  y*s+x*z*(1-c);
      m.M[9] = -x*s+y*z*(1-c);
      m.M[10] =  c +z*z*(1-c);

      this->rot(m);
    }

    ///Rotates the matrix around #axis by #angle degrees
    inline void grotateDegree(float angle, otVector3 axis)
    { grotateRad(angle*D2RAD, axis); }

    ///Rotates the matrix around #axis by #angle degree
    void grotateRad(float angle, otVector3 axis)
    {
      axis.normalize();
      float x = axis.X;
      float y = axis.Y;
      float z = axis.Z;
      float s = sin(angle);
      float c = cos(angle);

      otMatrix m;

      m.M[0] =   c +x*x*(1-c);
      m.M[1] =  z*s+x*y*(1-c);
      m.M[2] = -y*s+x*z*(1-c);
                        
      m.M[4] = -z*s+x*y*(1-c);
      m.M[5] =   c +y*y*(1-c);
      m.M[6] =  x*s+y*z*(1-c);
                        
      m.M[8] =  y*s+x*z*(1-c);
      m.M[9] = -x*s+y*z*(1-c);
      m.M[10] =  c +z*z*(1-c);

      m.rot(*this);
      m.translate(M[12],M[13],M[14]);
      *this = m;
    }

    ///Rotates the matrix around the local x axis by #angle degrees
    inline void lrotXDegree(float x) { if (x) lrotXRad(x*D2RAD); }

    ///Rotates the matrix around the local x axis by #angle rads
    void lrotXRad(float x)
    {
      float sx=sin(x); float cx=cos(x);

      float tmp;

      // This might look a bit funny, it's basically just a rotation + multiplication...
      // Since we do only rotate a part of it, we don't need the full multiplication stuff.

      tmp   = M[4] * cx  + sx * M[8];
      M[8]  = -M[4] * sx  + cx * M[8];
      M[4]  = tmp;

      tmp   = M[5] * cx  + sx * M[9];
      M[9]  = -M[5] * sx  + cx * M[9];
      M[5]  = tmp;

      tmp   = M[6] * cx  + sx * M[10];
      M[10] = -M[6] * sx  + cx * M[10];
      M[6]  = tmp;
    }

    ///Rotates the matrix around the global x axis by #angle degrees
    inline void grotXDegree(float x) { if (x) grotXRad(x*D2RAD); }

    ///Rotates the matrix around the global x axis by #angle rads
    void grotXRad(float x)
    {
      float sx=sin(x); float cx=cos(x);

      float tmp;

      // This might look a bit funny, it's basically just a rotation + multiplication...
      // Since we do only rotate a part of it, we don't need the full multiplication stuff.

      tmp   =  cx*M[1]  - sx*M[2];
      M[2]  =  sx*M[1]  + cx*M[2];
      M[1]  =  tmp;

      tmp   =  cx*M[5]  - sx*M[6];
      M[6]  =  sx*M[5]  + cx*M[6];
      M[5]  =  tmp;

      tmp   =  cx*M[9]  - sx*M[10];
      M[10] =  sx*M[9]  + cx*M[10];
      M[9]  =  tmp;
    }

    ///Rotates the matrix around the local y axis by #angle degrees
    inline void lrotYDegree(float y) { if (y) lrotYRad(y*D2RAD); }

    ///Rotates the matrix around the local y axis by #angle rads
    void lrotYRad(float y)
    {
      float sy=sin(y); float cy=cos(y);

      float tmp;

      // This might look a bit funny, it's basically just a rotation + multiplication...
      // Since we do only rotate a part of it, we don't need the full multiplication stuff.

      tmp   =  cy*M[0] + sy*M[2];
      M[2]  = -sy*M[0] + cy*M[2];
      M[0]  = tmp;

      tmp   =  cy*M[4] + sy*M[6];
      M[6]  = -sy*M[4] + cy*M[6];
      M[4]  = tmp;
      	
      tmp   =  cy*M[8] + sy*M[10];
      M[10] = -sy*M[8] + cy*M[10];
      M[8]  = tmp;
    }

    ///Rotates the matrix around the global y axis by #angle degrees
    inline void grotYDegree(float y) { if (y) grotYRad(y*D2RAD); }

    ///Rotates the matrix around the global y axis by #angle rads
    void grotYRad(float y)
    {
      float sy=sin(y); float cy=cos(y);

      float tmp;

      // This might look a bit funny, it's basically just a rotation + multiplication...
      // Since we do only rotate a part of it, we don't need the full multiplication stuff.

      tmp  = M[0] * cy  + sy * M[8];
      M[8] = - M[0] * sy  + cy * M[8];
      M[0] = tmp;

      tmp  = M[1] * cy  + sy * M[9];
      M[9] = - M[1] * sy  + cy * M[9];
      M[1] = tmp;

      tmp   = M[2] * cy  + sy * M[10];
      M[10] = - M[2] * sy  + cy * M[10];
      M[2]  = tmp;
    }

    ///Rotates the matrix around the local z axis by #angle degrees
    inline void lrotZDegree(float z) { if (z) lrotZRad(z*D2RAD); }

    ///Rotates the matrix around the local z axis by #angle rads
    void lrotZRad(float z)
    {
      float sz=sin(z); float cz=cos(z);

      float tmp;

      // This might look a bit funny, it's basically just a rotation + multiplication...
      // Since we do only rotate a part of it, we don't need the full multiplication stuff.

      tmp  = M[0] * cz  - sz * M[4];
      M[4] = M[0] * sz  + cz * M[4];
      M[0] = tmp;

      tmp  = M[1] * cz  - sz * M[5];
      M[5] = M[1] * sz  + cz * M[5];
      M[1] = tmp;

      tmp  = M[2] * cz  - sz * M[6];
      M[6] = M[2] * sz  + cz * M[6];
      M[2] = tmp;
    }

    ///Rotates the matrix around the global z axis by #angle degrees
    inline void grotZDegree(float z) { if (z) grotZRad(z*D2RAD); }

    ///Rotates the matrix around the global z axis by #angle rads
    void grotZRad(float z)
    {
      float sz=sin(z); float cz=cos(z);

      float tmp;

      // This might look a bit funny, it's basically just a rotation + multiplication...
      // Since we do only rotate a part of it, we don't need the full multiplication stuff.

      tmp   =  sz*M[0]  + cz*M[1];
      M[0]  =  cz*M[0]  - sz*M[1];
      M[1]  =  tmp;

      tmp   =  sz*M[4]  + cz*M[5];
      M[4]  =  cz*M[4]  - sz*M[5];
      M[5]  =  tmp;

      tmp   =  sz*M[8]  + cz*M[9];
      M[8]  =  cz*M[8]  - sz*M[9];
      M[9]  =  tmp;
    }

    ///Sets the scale of the matrix to x, y, z
    void scale(float x, float y, float z)
    {
      M[0] *=x; M[1] *= x; M[2] *= x; M[3] *= x;
      M[4] *=y; M[5] *= y; M[6] *= y; M[7] *= y;
      M[8] *=z; M[9] *= z; M[10]*= z; M[11]*= z;
    }

    ///Sets the scale of the matrix to x, y, z
    void scale(otVector3& factor)
    {
      M[0] *= factor.X; M[1] *= factor.X; M[2] *= factor.X; M[3] *= factor.X;
      M[4] *= factor.Y; M[5] *= factor.Y; M[6] *= factor.Y; M[7] *= factor.Y;
      M[8] *= factor.Z; M[9] *= factor.Z; M[10]*= factor.Z; M[11]*= factor.Z;
    }

    ///Gets the X vector of the matrix' coordinat system.
    otVector3 getX()
    {
      return otVector3(M[0], M[1], M[2]);
    }

    ///Gets the Y vector of the matrix' coordinat system.
    otVector3 getY()
    {
      return otVector3(M[4], M[5], M[6]);
    }

    ///Gets the Z vector of the matrix' coordinat system.
    otVector3 getZ()
    {
      return otVector3(M[8], M[9], M[10]);
    }

    otMatrix operator*(const otMatrix& other)
    {
      otMatrix result;
      const float *N = other.M;
      float *R = result.M;

      R[0]  = M[0]*N[0]  + M[4]*N[1]  + M[8] *N[2]  + M[12]*N[3];
      R[1]  = M[1]*N[0]  + M[5]*N[1]  + M[9] *N[2]  + M[13]*N[3];
      R[2]  = M[2]*N[0]  + M[6]*N[1]  + M[10]*N[2]  + M[14]*N[3];
      R[3]  = M[3]*N[0]  + M[7]*N[1]  + M[11]*N[2]  + M[15]*N[3];

      R[4]  = M[0]*N[4]  + M[4]*N[5]  + M[8] *N[6]  + M[12]*N[7];
      R[5]  = M[1]*N[4]  + M[5]*N[5]  + M[9] *N[6]  + M[13]*N[7];
      R[6]  = M[2]*N[4]  + M[6]*N[5]  + M[10]*N[6]  + M[14]*N[7];
      R[7]  = M[3]*N[4]  + M[7]*N[5]  + M[11]*N[6]  + M[15]*N[7];
      	
      R[8]  = M[0]*N[8]  + M[4]*N[9]  + M[8] *N[10] + M[12]*N[11];
      R[9]  = M[1]*N[8]  + M[5]*N[9]  + M[9] *N[10] + M[13]*N[11];
      R[10] = M[2]*N[8]  + M[6]*N[9]  + M[10]*N[10] + M[14]*N[11];
      R[11] = M[3]*N[8]  + M[7]*N[9]  + M[11]*N[10] + M[15]*N[11];
      	
      R[12] = M[0]*N[12] + M[4]*N[13] + M[8] *N[14] + M[12]*N[15];
      R[13] = M[1]*N[12] + M[5]*N[13] + M[9] *N[14] + M[13]*N[15];
      R[14] = M[2]*N[12] + M[6]*N[13] + M[10]*N[14] + M[14]*N[15];
      R[15] = M[3]*N[12] + M[7]*N[13] + M[11]*N[14] + M[15]*N[15];

      #ifdef _DEBUG
        checkValid();
      #endif

      return result;
    }

    otMatrix& operator*=(const otMatrix& other)
    {
      const float *N = other.M;
      float R[16];

      R[0]  = M[0]*N[0]  + M[4]*N[1]  + M[8] *N[2]  + M[12]*N[3];
      R[1]  = M[1]*N[0]  + M[5]*N[1]  + M[9] *N[2]  + M[13]*N[3];
      R[2]  = M[2]*N[0]  + M[6]*N[1]  + M[10]*N[2]  + M[14]*N[3];
      R[3]  = M[3]*N[0]  + M[7]*N[1]  + M[11]*N[2]  + M[15]*N[3];

      R[4]  = M[0]*N[4]  + M[4]*N[5]  + M[8] *N[6]  + M[12]*N[7];
      R[5]  = M[1]*N[4]  + M[5]*N[5]  + M[9] *N[6]  + M[13]*N[7];
      R[6]  = M[2]*N[4]  + M[6]*N[5]  + M[10]*N[6]  + M[14]*N[7];
      R[7]  = M[3]*N[4]  + M[7]*N[5]  + M[11]*N[6]  + M[15]*N[7];
      	
      R[8]  = M[0]*N[8]  + M[4]*N[9]  + M[8] *N[10] + M[12]*N[11];
      R[9]  = M[1]*N[8]  + M[5]*N[9]  + M[9] *N[10] + M[13]*N[11];
      R[10] = M[2]*N[8]  + M[6]*N[9]  + M[10]*N[10] + M[14]*N[11];
      R[11] = M[3]*N[8]  + M[7]*N[9]  + M[11]*N[10] + M[15]*N[11];
      	
      R[12] = M[0]*N[12] + M[4]*N[13] + M[8] *N[14] + M[12]*N[15];
      R[13] = M[1]*N[12] + M[5]*N[13] + M[9] *N[14] + M[13]*N[15];
      R[14] = M[2]*N[12] + M[6]*N[13] + M[10]*N[14] + M[14]*N[15];
      R[15] = M[3]*N[12] + M[7]*N[13] + M[11]*N[14] + M[15]*N[15];

      #ifdef _DEBUG
        checkValid();
      #endif

      M[ 0] = R[ 0]; M[ 1] = R[ 1]; M[ 2] = R[ 2]; M[ 3] = R[ 3];
      M[ 4] = R[ 4]; M[ 5] = R[ 5]; M[ 6] = R[ 6]; M[ 7] = R[ 7];
      M[ 8] = R[ 8]; M[ 9] = R[ 9]; M[10] = R[10]; M[11] = R[11];
      M[12] = R[12]; M[13] = R[13]; M[14] = R[14]; M[15] = R[15];

      return *this;
    }

    /// Sets one matrix equal another.
    bool operator==(const otMatrix& other)
    {
      const float* N = other.M;
      float d = 0.00003f;
      bool r = (fabs(M[ 0]-N[ 0])<=d && fabs(M[ 1]-N[ 1])<=d && fabs(M[ 2]-N[ 2])<=d &&
                fabs(M[ 3]-N[ 3])<=d && fabs(M[ 4]-N[ 4])<=d && fabs(M[ 5]-N[ 5])<=d &&
                fabs(M[ 6]-N[ 6])<=d && fabs(M[ 7]-N[ 7])<=d && fabs(M[ 8]-N[ 8])<=d &&
                fabs(M[ 9]-N[ 9])<=d && fabs(M[10]-N[10])<=d && fabs(M[11]-N[11])<=d &&
                fabs(M[12]-N[12])<=d && fabs(M[13]-N[13])<=d && fabs(M[14]-N[14])<=d &&
                fabs(M[15]-N[15])<=d );
      if (r)
        return true;
      else
        return false;
    }

    otMatrix& operator=(const otMatrix& other)
    {
      const float* N = other.M;
      M[ 0] = N[ 0]; M[ 1] = N[ 1]; M[ 2] = N[ 2]; M[ 3] = N[ 3];
      M[ 4] = N[ 4]; M[ 5] = N[ 5]; M[ 6] = N[ 6]; M[ 7] = N[ 7];
      M[ 8] = N[ 8]; M[ 9] = N[ 9]; M[10] = N[10]; M[11] = N[11];
      M[12] = N[12]; M[13] = N[13]; M[14] = N[14]; M[15] = N[15];
      #ifdef _DEBUG
        checkValid();
      #endif
      return *this;
    }

    /// Fully transforms the vector
    otVector3 operator*(const otVector3& V) const
    {
      return otVector3( V.X*M[0] + V.Y*M[1] + V.Z*M[2]  + M[12],
                        V.X*M[4] + V.Y*M[5] + V.Z*M[6]  + M[13],
                        V.X*M[8] + V.Y*M[9] + V.Z*M[10] + M[14] );
    }

    /// Fully transforms the vector
    otVector3& operator*=(otVector3& V)
    {
      return V.set( V.X*M[0] + V.Y*M[1] + V.Z*M[2]  + M[12],
                    V.X*M[4] + V.Y*M[5] + V.Z*M[6]  + M[13],
                    V.X*M[8] + V.Y*M[9] + V.Z*M[10] + M[14] );
    }

    /** Vector Rotation
     * Doesn't fully transform the vector like a multiplication but only rotates it.
     * It returns a new vector and doesn't change the input
     */
    otVector3 apply(const otVector3& V) const
    {
      return otVector3( V.X*M[0] + V.Y*M[1] + V.Z*M[2],
                        V.X*M[4] + V.Y*M[5] + V.Z*M[6],
                        V.X*M[8] + V.Y*M[9] + V.Z*M[10]);
    }

    /** Vector Rotation
     * Doesn't fully transform the vector like a multiplication but only rotates it.
     * It applies the rotation to the input vector and overwrite the values.
     */
    otVector3& applyTo(otVector3& V) const
    {
      return V.set( V.X*M[0] + V.Y*M[1] + V.Z*M[2],
                    V.X*M[4] + V.Y*M[5] + V.Z*M[6],
                    V.X*M[8] + V.Y*M[9] + V.Z*M[10]);
    }

    /**
     * Applies rotated translation to the matrix.
     * Moves the translation along the inner axis of the matrix
     * (taking the rotation of the matrix into account)
     * If you can discribe it less confusing, please let us know :)
     */
    void thistranslate(otVector3& v)
    {
      thistranslate(v.X, v.Y, v.Z);
    }

    /**
     * Applies rotated translation to the matrix.
     * Moves the translation along the inner axis of the matrix
     * (taking the rotation of the matrix into account)
     * If you can discribe it less confusing, please let us know :)
     */
    void thistranslate(float x,float y,float z)
    {
      thisXtranslate(x);
      thisYtranslate(y);
      thisZtranslate(z);
    }

    /**
     * Applies rotated translation to the matrix.
     * Moves the translation along the inner x axis of the matrix
     * (taking the rotation of the matrix into account)
     * If you can discribe it less confusing, please let us know :)
     */
    void thisXtranslate(float z)
    {
      M[12] += z*M[0];
      M[13] += z*M[1];
      M[14] += z*M[2];
    }

    /**
     * Applies rotated translation to the matrix.
     * Moves the translation along the inner y axis of the matrix
     * (taking the rotation of the matrix into account)
     * If you can discribe it less confusing, please let us know :)
     */
    void thisYtranslate(float z)
    {
      M[12] += z*M[4];
      M[13] += z*M[5];
      M[14] += z*M[6];
    }

    /**
     * Applies rotated translation to the matrix.
     * Moves the translation along the inner z axis of the matrix
     * (taking the rotation of the matrix into account)
     * If you can discribe it less confusing, please let us know :)
     */
    void thisZtranslate(float z)
    {
      M[12] += z*M[8];
      M[13] += z*M[9];
      M[14] += z*M[10];
    }

    //inline float operator[](unsigned x) 
    //{ 
    //  if (x < 16) return M[x]; 
    //}

    inline float operator()(unsigned row, unsigned col)
    { 
      return M[col*4 + row]; 
    }

    ///Raw access to the matrix' float array.
    inline operator float* ()
    { 
      return M; 
    }

    ///Raw access to the matrix' float array.
    inline operator const float* () const
    { 
      return M; 
    }

    ///Create look-at View Matrix.
    void lookAt(otVector3& pos, otVector3& target, otVector3& up)
    {
      // Z vector
      otVector3 zaxis = target-pos;
      zaxis.normalize();

      // X vector = Y cross Z
      otVector3 xaxis = up & zaxis;

      // Recompute Y = Z cross X
      otVector3 yaxis = zaxis & xaxis;

      xaxis.normalize();
      yaxis.normalize();

      M[0] = xaxis.X; M[1] = xaxis.Y; M[2] = xaxis.Z; M[3] = 0;
      M[4] = yaxis.X; M[5] = yaxis.Y; M[6] = yaxis.Z; M[7] = 0;
      M[8] = zaxis.X; M[9] = zaxis.Y; M[10]= zaxis.Z; M[11]= 0;
      M[12]= - pos.X; M[13]= - pos.Y; M[14]= - pos.Z; M[15]= 1;
    }
  };
  /// @}
}

#endif //_OT_OTMATRIX_H_
