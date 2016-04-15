#ifndef __ATOM3D_UTILS_CUBE_H
#define __ATOM3D_UTILS_CUBE_H

#if _MSC_VER > 1000
# pragma once
#endif

template <class T>
struct ATOM_Point3DT
{
	T x;
	T y;
	T z;

	ATOM_Point3DT (void) 
	{
	}

	ATOM_Point3DT (T x_, T y_, T z_)
	:x(x_)
	,y(y_) 
	,z(z_)
	{
	}

	template <class U> 
	ATOM_Point3DT (const ATOM_Point3DT<U> &other)
	: x(static_cast<T>(other.x))
	, y(static_cast<T>(other.y)) 
	, z(static_cast<T>(other.z))
	{
	}

	template <class U>
	ATOM_Point3DT<T> & operator = (const ATOM_Point3DT<U> &other)
	{
		x = static_cast<T>(other.x);
		y = static_cast<T>(other.y);
		z = static_cast<T>(other.z);
		return *this;
	}

	template <class U>
	ATOM_Point3DT<T> & operator += (const ATOM_Point3DT<U> &other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	template <class U>
	ATOM_Point3DT<T> & operator -= (const ATOM_Point3DT<U> &other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	ATOM_Point3DT<T> & operator *= (int scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	template <class U>
	ATOM_Point3DT<T> & operator /= (int scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	ATOM_Point3DT<T> & operator *= (float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	template <class U>
	ATOM_Point3DT<T> & operator /= (float scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	ATOM_Point3DT<T> & operator *= (double scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	template <class U>
	ATOM_Point3DT<T> & operator /= (double scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}
};

template <class T>
inline bool operator == (const ATOM_Point3DT<T> &p1, const ATOM_Point3DT<T> &p2)
{
	return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
}

template <class T>
inline bool operator != (const ATOM_Point3DT<T> &p1, const ATOM_Point3DT<T> &p2)
{
	return ! operator == (p1, p2);
}

template <class T>
inline ATOM_Point3DT<T> operator + (const ATOM_Point3DT<T> &p1, const ATOM_Point3DT<T> &p2)
{
	return ATOM_Point3DT<T> (p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
}

template <class T>
inline ATOM_Point3DT<T> operator - (const ATOM_Point3DT<T> &p1, const ATOM_Point3DT<T> &p2)
{
	return ATOM_Point3DT<T> (p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
}

template <class T>
inline ATOM_Point3DT<T> operator * (const ATOM_Point3DT<T> &p, int scalar)
{
	return ATOM_Point3DT<T> (p.x * scalar, p.y * scalar, p.z * scalar);
}


template <class T>
inline ATOM_Point3DT<T> operator * (int scalar, const ATOM_Point3DT<T> &p)
{
	return ATOM_Point3DT<T> (p.x * scalar, p.y * scalar, p.z * scalar);
}

template <class T>
inline ATOM_Point3DT<T> operator / (const ATOM_Point3DT<T> &p, int scalar)
{
	return ATOM_Point3DT<T> (p.x / scalar, p.y / scalar, p.z / scalar);
}

template <class T>
inline ATOM_Point3DT<T> operator * (const ATOM_Point3DT<T> &p, float scalar)
{
	return ATOM_Point3DT<T> (p.x * scalar, p.y * scalar, p.z * scalar);
}


template <class T>
inline ATOM_Point3DT<T> operator * (float scalar, const ATOM_Point3DT<T> &p)
{
	return ATOM_Point3DT<T> (p.x * scalar, p.y * scalar, p.z * scalar);
}

template <class T>
inline ATOM_Point3DT<T> operator / (const ATOM_Point3DT<T> &p, float scalar)
{
	return ATOM_Point3DT<T> (p.x / scalar, p.y / scalar, p.z / scalar);
}

template <class T>
inline ATOM_Point3DT<T> operator * (const ATOM_Point3DT<T> &p, double scalar)
{
	return ATOM_Point3DT<T> (p.x * scalar, p.y * scalar, p.z * scalar);
}


template <class T>
inline ATOM_Point3DT<T> operator * (double scalar, const ATOM_Point3DT<T> &p)
{
	return ATOM_Point3DT<T> (p.x * scalar, p.y * scalar, p.z * scalar);
}

template <class T>
inline ATOM_Point3DT<T> operator / (const ATOM_Point3DT<T> &p, double scalar)
{
	return ATOM_Point3DT<T> (p.x / scalar, p.y / scalar, p.z / scalar);
}

template <class T>
struct ATOM_Size3DT
{
	T w;
	T h;
	T d;

	ATOM_Size3DT (void) 
	{
	}

	ATOM_Size3DT (T w_, T h_, T d_)
	:w(w_)
	,h(h_) 
	,d(d_)
	{
	}

	template <class U>
	ATOM_Size3DT (const ATOM_Size3DT<U> &other)
	:w(static_cast<T>(other.w))
	,h(static_cast<T>(other.h))
	,d(static_cast<T>(other.d))
	{
	}

	template <class U>
	ATOM_Size3DT<T> & operator = (const ATOM_Size3DT<U> &other)
	{
		w = static_cast<T>(other.w);
		h = static_cast<T>(other.h);
		d = static_cast<T>(other.d);
		return *this;
	}

	template <class U>
	ATOM_Size3DT<T> & operator += (const ATOM_Size3DT<U> &other)
	{
		w += other.w;
		h += other.h;
		d += other.d;
		return *this;
	}

	template <class U>
	ATOM_Size3DT<T> & operator -= (const ATOM_Size3DT<U> &other)
	{
		w -= other.w;
		h -= other.h;
		d -= other.d;
		return *this;
	}

	ATOM_Size3DT<T> & operator *= (int scalar)
	{
		w *= scalar;
		h *= scalar;
		d *= scalar;
		return *this;
	}

	ATOM_Size3DT<T> & operator /= (int scalar)
	{
		w /= scalar;
		h /= scalar;
		d /= scalar;
		return *this;
	}

	ATOM_Size3DT<T> & operator *= (float scalar)
	{
		w *= scalar;
		h *= scalar;
		d *= scalar;
		return *this;
	}

	ATOM_Size3DT<T> & operator /= (float scalar)
	{
		w /= scalar;
		h /= scalar;
		d /= scalar;
		return *this;
	}

	ATOM_Size3DT<T> & operator *= (double scalar)
	{
		w *= scalar;
		h *= scalar;
		d *= scalar;
		return *this;
	}

	ATOM_Size3DT<T> & operator /= (double scalar)
	{
		w /= scalar;
		h /= scalar;
		d /= scalar;
		return *this;
	}
};

template <class T>
inline bool operator == (const ATOM_Size3DT<T> &s1, const ATOM_Size3DT<T> &s2)
{
  return s1.w == s2.w && s1.h == s2.h && s1.d == s2.d;
}

template <class T>
inline bool operator != (const ATOM_Size3DT<T> &s1, const ATOM_Size3DT<T> &s2)
{
  return ! operator == (s1, s2);
}

template <class T>
inline ATOM_Size3DT<T> operator + (const ATOM_Size3DT<T> &sz1, const ATOM_Size3DT<T> &sz2)
{
  return ATOM_Size3DT<T> (sz1.w + sz2.w, sz1.h + sz2.h, sz1.d + sz2.d);
}

template <class T>
inline ATOM_Size3DT<T> operator - (const ATOM_Size3DT<T> &sz1, const ATOM_Size3DT<T> &sz2)
{
  return ATOM_Size3DT<T> (sz1.w - sz2.w, sz1.h - sz2.h, sz1.d - sz2.d);
}

template <class T>
inline ATOM_Size3DT<T> operator * (const ATOM_Size3DT<T> &sz1, int scalar)
{
  return ATOM_Size3DT<T> (sz1.w * scalar, sz1.h * scalar, sz1.d * scalar);
}

template <class T>
inline ATOM_Size3DT<T> operator * (int scalar, const ATOM_Size3DT<T> &sz1)
{
  return ATOM_Size3DT<T> (sz1.w * scalar, sz1.h * scalar, sz1.d * scalar);
}

template <class T>
inline ATOM_Size3DT<T> operator / (const ATOM_Size3DT<T> &sz1, int scalar)
{
  return ATOM_Size3DT<T> (sz1.w / scalar, sz1.h / scalar, sz1.d / scalar);
}

template <class T>
inline ATOM_Size3DT<T> operator * (const ATOM_Size3DT<T> &sz1, float scalar)
{
  return ATOM_Size3DT<T> (sz1.w * scalar, sz1.h * scalar, sz1.d * scalar);
}

template <class T>
inline ATOM_Size3DT<T> operator * (float scalar, const ATOM_Size3DT<T> &sz1)
{
  return ATOM_Size3DT<T> (sz1.w * scalar, sz1.h * scalar, sz1.d * scalar);
}

template <class T>
inline ATOM_Size3DT<T> operator / (const ATOM_Size3DT<T> &sz1, float scalar)
{
  return ATOM_Size3DT<T> (sz1.w / scalar, sz1.h / scalar, sz1.d / scalar);
}

template <class T>
inline ATOM_Size3DT<T> operator * (const ATOM_Size3DT<T> &sz1, double scalar)
{
  return ATOM_Size3DT<T> (sz1.w * scalar, sz1.h * scalar, sz1.d * scalar);
}

template <class T>
inline ATOM_Size3DT<T> operator * (double scalar, const ATOM_Size3DT<T> &sz1)
{
  return ATOM_Size3DT<T> (sz1.w * scalar, sz1.h * scalar, sz1.d * scalar);
}

template <class T>
inline ATOM_Size3DT<T> operator / (const ATOM_Size3DT<T> &sz1, double scalar)
{
  return ATOM_Size3DT<T> (sz1.w / scalar, sz1.h / scalar, sz1.d / scalar);
}

template <class T>
struct ATOM_Cube3DT
{
	ATOM_Point3DT<T> point;
	ATOM_Size3DT<T> size;

	ATOM_Cube3DT (void) 
	{
	}

	ATOM_Cube3DT (T x_, T y_, T z_, T w_, T h_, T d_)
	:point(x_, y_, z_)
	,size(w_, h_, d_) 
	{
	}

	ATOM_Cube3DT (const ATOM_Point3DT<T> &point_, const ATOM_Size3DT<T> &size_)
	:point(point_)
	,size(size_) 
	{
	}

	template <class U>
	ATOM_Cube3DT (const ATOM_Cube3DT<U> &other)
	:point(other.point)
	,size(other.size)
	{
	}

	template <class U>
	ATOM_Cube3DT<T> & operator = (const ATOM_Cube3DT<U> &other)
	{
		point = other.point;
		size = other.size;
		return *this;
	}

	bool isPointIn (T x_, T y_, T z_) const
	{
		return x_ >= point.x && x_ < point.x + size.w && y_ >= point.y && y_ < point.y + size.h && z_ >= point.z && z_ < point.z + size.d;
	}

	template <class U>
	bool isCubeIn (const ATOM_Cube3DT<U> &subCube) const
	{
		return subCube.point.x >= point.x 
			&& subCube.point.x+subCube.size.w <= point.x+size.w 
			&& subCube.point.y >= point.y 
			&& subCube.point.y+subCube.size.h <= point.y+size.h
			&& subCube.point.z >= point.z
			&& subCube.point.z+subCube.size.d <= point.z+size.d;
	}

	template <class U>
	bool intersectedWith (const ATOM_Cube3DT<U> &other) const
	{
		return point.x < other.point.x + other.size.w 
			&& point.y < other.point.y + other.size.h 
			&& point.z < other.point.z + other.size.d
			&& point.x + size.w > other.point.x 
			&& point.y + size.h > other.point.y
			&& point.z + size.d > other.point.z;
	}

	static inline ATOM_Cube3DT<T> getIntersection (const ATOM_Cube3DT<T> &rc1, const ATOM_Cube3DT<T> &rc2)
	{
		T x1 = rc1.point.x + rc1.size.w;
		T y1 = rc1.point.y + rc1.size.h;
		T z1 = rc1.point.z + rc1.size.d;
		T X1 = rc2.point.x + rc2.size.w;
		T Y1 = rc2.point.y + rc2.size.h;
		T Z1 = rc2.point.z + rc2.size.d;
		T iX = (rc1.point.x > rc2.point.x) ? rc1.point.x : rc2.point.x;
		T iY = (rc1.point.y > rc2.point.y) ? rc1.point.y : rc2.point.y;
		T iZ = (rc1.point.z > rc2.point.z) ? rc1.point.z : rc2.point.z;
		T iX1 = (x1 < X1) ? x1 : X1;
		T iY1 = (y1 < Y1) ? y1 : Y1;
		T iZ1 = (z1 < Z1) ? z1 : Z1;
		return ATOM_Cube3DT<T> (iX, iY, iZ, ((iX < iX1) ? iX1-iX : 0), ((iY < iY1) ? iY1-iY : 0), ((iZ < iZ1) ? iZ1-iZ : 0));
	}
};

template <class T>
inline bool operator == (const ATOM_Cube3DT<T> &r1, const ATOM_Cube3DT<T> &r2)
{
  return r1.point == r2.point && r1.size == r2.size;
}

template <class T>
inline bool operator != (const ATOM_Cube3DT<T> &r1, const ATOM_Cube3DT<T> &r2)
{
  return ! operator == (r1, r2);
}

typedef ATOM_Point3DT<int> ATOM_Point3Di;
typedef ATOM_Size3DT<int> ATOM_Size3Di;
typedef ATOM_Cube3DT<int> ATOM_Cube3Di;

typedef ATOM_Point3DT<float> ATOM_Point3Df;
typedef ATOM_Size3DT<float> ATOM_Size3Df;
typedef ATOM_Cube3DT<float> ATOM_Cube3Df;

#endif // __ATOM3D_UTILS_RECT_H
