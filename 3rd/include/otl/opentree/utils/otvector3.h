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

#ifndef _OT_OTVECTOR_H_
#define _OT_OTVECTOR_H_

#include "otutil.h"

namespace opentree
{
  /// \addtogroup Utilities
  /// @{

  /**
   * Class to handle vectors.
   * If you need to rotate a vector around the same angle 
   * a lot, please look at otRotVector3 as well.
   */
  class otVector3
  {
  public:
    /// Coordinates
    float X, Y, Z;

    otVector3() : X(0), Y(0), Z(0)
    {
    }

    otVector3(float x,float y, float z) : X(x), Y(y), Z(z)
    {
    }

    /// Copy constructor.
    otVector3(const otVector3& other) : X(other.X), Y(other.Y), Z(other.Z)
    {
    }

    ~otVector3()
    {
    }

    /// Normalizes the vector.
    otVector3& normalize()
    {
      // check for null vector.
      if (X == 0 && Y == 0 && Z == 0) return *this;

      float square_len = (X*X + Y*Y + Z*Z);

      float inv = 1.0f / sqrtf(square_len);
      X *= inv; Y *= inv; Z *= inv;

      return *this;
    }

    /// Returns the length of the vector.
    float length()
    {
      return sqrtf(X*X + Y*Y + Z*Z);
    }

    /// Rotates the Vector around the X axis.
    otVector3& rotXRad(const float angle)
    {
      if(angle==0.0f) return (*this);

      float dSin=sin(angle);
      float dCos=cos(angle);

      float y = Y*dCos - Z*dSin;
      float z = Y*dSin + Z*dCos;

      Y = y; 
      Z = z;

      return *this;
    }

    /// Rotates the Vector around the Y axis.
    otVector3& rotYRad(const float angle)
    {
      if(angle==0.0f) return (*this);

      float dSin=sin(angle);
      float dCos=cos(angle);

      float x = X*dCos - Z*dSin;
      float z = X*dSin + Z*dCos;

      X = x; 
      Z = z;

      return *this;
    }

    /// Rotates the Vector around the Z axis.
    otVector3& rotZRad(const float angle)
    {
      if(angle==0.0f) return (*this);

      float dSin=sin(angle);
      float dCos=cos(angle);

      float x = X*dCos - Y*dSin;
      float y = X*dSin + Y*dCos;

      X = x; 
      Y = y;

      return *this;
    }

    /// Rotates the Vector around the #axis by #angle rads.
    otVector3& rotateRad(float angle, const otVector3& axis)
    {
      float rotX = axis.X;
      float rotY = axis.Y;
      float rotZ = axis.Z;
      float rotS = sin(angle);
      float rotC = cos(angle);
      float one_minus_rotC = 1-rotC;
      float SX = rotS*rotX;
      float SY = rotS*rotY;
      float SZ = rotS*rotZ;

      float XX = rotX*rotX*one_minus_rotC + rotC;
      float XY = rotX*rotY*one_minus_rotC - SZ;
      float XZ = rotX*rotZ*one_minus_rotC + SY;
      float YX = rotY*rotX*one_minus_rotC + SZ;
      float YY = rotY*rotY*one_minus_rotC + rotC;
      float YZ = rotY*rotZ*one_minus_rotC - SX;
      float ZX = rotZ*rotX*one_minus_rotC - SY;
      float ZY = rotZ*rotY*one_minus_rotC + SX;
      float ZZ = rotZ*rotZ*one_minus_rotC + rotC;

      X=XX*X+XY*Y+XZ*Z; Y=YX*X+YY*Y+YZ*Z; Z=ZX*X+ZY*Y+ZZ*Z;
      return *this;
    }

    /// Sets the value of the Vector.
    otVector3& set(float x ,float y, float z)
    {
      X = x; Y = y; Z = z;
      return *this;
    }

    otVector3& operator=(const otVector3& other)
    {
      X = other.X; Y = other.Y; Z = other.Z;
      return *this;
    }
    bool operator==(const otVector3& other)
    {
      float d = 0.0001f;
      if (fabs(X-other.X)<=d && fabs(Y-other.Y)<=d && fabs(Z-other.Z)<=d)
        return true;
      else
        return false;
    }

    otVector3 operator-() const
    {
      return otVector3(-X,-Y,-Z);
    }

    otVector3 operator*(const otVector3& other) const
    {
      return otVector3(X * other.X, Y * other.Y, Z * other.Z);
    }
    otVector3& operator*=(const otVector3& other)
    {
      X *= other.X; Y *= other.Y; Z *= other.Z;
      return *this;
    }

    otVector3 operator+(const otVector3& other) const
    {
      return otVector3(X + other.X, Y + other.Y, Z + other.Z);
    }
    otVector3& operator+=(const otVector3& other)
    {
      X+=other.X; Y+=other.Y; Z+=other.Z;
      return *this;
    }

    otVector3 operator-(const otVector3& other) const
    {
      return otVector3(X - other.X, Y - other.Y, Z - other.Z);
    }
    otVector3& operator-=(const otVector3& other)
    {
      X-=other.X; Y-=other.Y; Z-=other.Z;
      return *this;
    }

    /// Dot product
    float operator|(const otVector3& other) const
    {
      return X*other.X + Y*other.Y + Z*other.Z;
    }
    /// Cross product
    otVector3 operator&(const otVector3& other) const
    {
      return otVector3(Y * other.Z - Z * other.Y,
                       Z * other.X - X * other.Z,
                       X * other.Y - Y * other.X);
    }

    otVector3 operator*(const float& other) const
    {
      return otVector3(X * other, Y * other, Z * other);
    }
    otVector3& operator*=(const float& other)
    {
      X *= other; Y *= other; Z *= other;
      return *this;
    }

    otVector3 operator/(const float& other) const
    {
      return otVector3(X / other, Y / other, Z / other);
    }
    otVector3& operator/=(const float& other)
    {
      X /= other; Y /= other; Z /= other;
      return *this;
    }
  };

  /// @}
}

#endif //_OT_OTVECTOR_H_
