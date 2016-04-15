#ifndef __ATOM3D_UTILS_RECT_H
#define __ATOM3D_UTILS_RECT_H

#if _MSC_VER > 1000
# pragma once
#endif

template <class T>
struct ATOM_Point2DT
{
	T x;
	T y;

	ATOM_Point2DT (void) 
	{
	}

	ATOM_Point2DT (T x_, T y_)
	:x(x_)
	,y(y_) 
	{
	}

	template <class U> 
	ATOM_Point2DT (const ATOM_Point2DT<U> &other)
	: x(static_cast<T>(other.x))
	, y(static_cast<T>(other.y)) 
	{
	}

	template <class U>
	ATOM_Point2DT<T> & operator = (const ATOM_Point2DT<U> &other)
	{
		x = static_cast<T>(other.x);
		y = static_cast<T>(other.y);
		return *this;
	}

	ATOM_Point2DT<T> operator - (void) const
	{
		return ATOM_Point2DT<T>(-x, -y);
	}

	template <class U>
	ATOM_Point2DT<T> & operator += (const ATOM_Point2DT<U> &other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	template <class U>
	ATOM_Point2DT<T> & operator -= (const ATOM_Point2DT<U> &other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	ATOM_Point2DT<T> & operator *= (int scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	template <class U>
	ATOM_Point2DT<T> & operator /= (int scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	ATOM_Point2DT<T> & operator *= (float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	template <class U>
	ATOM_Point2DT<T> & operator /= (float scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	ATOM_Point2DT<T> & operator *= (double scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	template <class U>
	ATOM_Point2DT<T> & operator /= (double scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}
};

template <class T>
inline bool operator == (const ATOM_Point2DT<T> &p1, const ATOM_Point2DT<T> &p2)
{
	return p1.x == p2.x && p1.y == p2.y;
}

template <class T>
inline bool operator != (const ATOM_Point2DT<T> &p1, const ATOM_Point2DT<T> &p2)
{
	return ! operator == (p1, p2);
}

template <class T>
inline ATOM_Point2DT<T> operator + (const ATOM_Point2DT<T> &p1, const ATOM_Point2DT<T> &p2)
{
	return ATOM_Point2DT<T> (p1.x + p2.x, p1.y + p2.y);
}

template <class T>
inline ATOM_Point2DT<T> operator - (const ATOM_Point2DT<T> &p1, const ATOM_Point2DT<T> &p2)
{
	return ATOM_Point2DT<T> (p1.x - p2.x, p1.y - p2.y);
}

template <class T>
inline ATOM_Point2DT<T> operator * (const ATOM_Point2DT<T> &p, int scalar)
{
	return ATOM_Point2DT<T> (p.x * scalar, p.y * scalar);
}


template <class T>
inline ATOM_Point2DT<T> operator * (int scalar, const ATOM_Point2DT<T> &p)
{
	return ATOM_Point2DT<T> (p.x * scalar, p.y * scalar);
}

template <class T>
inline ATOM_Point2DT<T> operator / (const ATOM_Point2DT<T> &p, int scalar)
{
	return ATOM_Point2DT<T> (p.x / scalar, p.y / scalar);
}

template <class T>
inline ATOM_Point2DT<T> operator * (const ATOM_Point2DT<T> &p, float scalar)
{
	return ATOM_Point2DT<T> (p.x * scalar, p.y * scalar);
}


template <class T>
inline ATOM_Point2DT<T> operator * (float scalar, const ATOM_Point2DT<T> &p)
{
	return ATOM_Point2DT<T> (p.x * scalar, p.y * scalar);
}

template <class T>
inline ATOM_Point2DT<T> operator / (const ATOM_Point2DT<T> &p, float scalar)
{
	return ATOM_Point2DT<T> (p.x / scalar, p.y / scalar);
}

template <class T>
inline ATOM_Point2DT<T> operator * (const ATOM_Point2DT<T> &p, double scalar)
{
	return ATOM_Point2DT<T> (p.x * scalar, p.y * scalar);
}


template <class T>
inline ATOM_Point2DT<T> operator * (double scalar, const ATOM_Point2DT<T> &p)
{
	return ATOM_Point2DT<T> (p.x * scalar, p.y * scalar);
}

template <class T>
inline ATOM_Point2DT<T> operator / (const ATOM_Point2DT<T> &p, double scalar)
{
	return ATOM_Point2DT<T> (p.x / scalar, p.y / scalar);
}

template <class T>
struct ATOM_Size2DT
{
	T w;
	T h;

	ATOM_Size2DT (void) 
	{
	}

	ATOM_Size2DT (T w_, T h_)
	:w(w_)
	,h(h_) 
	{
	}

	template <class U>
	ATOM_Size2DT (const ATOM_Size2DT<U> &other)
	:w(static_cast<T>(other.w))
	,h(static_cast<T>(other.h))
	{
	}

	template <class U>
	ATOM_Size2DT<T> & operator = (const ATOM_Size2DT<U> &other)
	{
		w = static_cast<T>(other.w);
		h = static_cast<T>(other.h);
		return *this;
	}

	template <class U>
	ATOM_Size2DT<T> & operator += (const ATOM_Size2DT<U> &other)
	{
		w += other.w;
		h += other.h;
		return *this;
	}

	template <class U>
	ATOM_Size2DT<T> & operator -= (const ATOM_Size2DT<U> &other)
	{
		w -= other.w;
		h -= other.h;
		return *this;
	}

	ATOM_Size2DT<T> & operator *= (int scalar)
	{
		w *= scalar;
		h *= scalar;
		return *this;
	}

	ATOM_Size2DT<T> & operator /= (int scalar)
	{
		w /= scalar;
		h /= scalar;
		return *this;
	}

	ATOM_Size2DT<T> & operator *= (float scalar)
	{
		w *= scalar;
		h *= scalar;
		return *this;
	}

	ATOM_Size2DT<T> & operator /= (float scalar)
	{
		w /= scalar;
		h /= scalar;
		return *this;
	}

	ATOM_Size2DT<T> & operator *= (double scalar)
	{
		w *= scalar;
		h *= scalar;
		return *this;
	}

	ATOM_Size2DT<T> & operator /= (double scalar)
	{
		w /= scalar;
		h /= scalar;
		return *this;
	}
};

template <class T>
inline bool operator == (const ATOM_Size2DT<T> &s1, const ATOM_Size2DT<T> &s2)
{
  return s1.w == s2.w && s1.h == s2.h;
}

template <class T>
inline bool operator != (const ATOM_Size2DT<T> &s1, const ATOM_Size2DT<T> &s2)
{
  return ! operator == (s1, s2);
}

template <class T>
inline ATOM_Size2DT<T> operator + (const ATOM_Size2DT<T> &sz1, const ATOM_Size2DT<T> &sz2)
{
  return ATOM_Size2DT<T> (sz1.w + sz2.w, sz1.h + sz2.h);
}

template <class T>
inline ATOM_Size2DT<T> operator - (const ATOM_Size2DT<T> &sz1, const ATOM_Size2DT<T> &sz2)
{
  return ATOM_Size2DT<T> (sz1.w - sz2.w, sz1.h - sz2.h);
}

template <class T>
inline ATOM_Size2DT<T> operator * (const ATOM_Size2DT<T> &sz1, int scalar)
{
  return ATOM_Size2DT<T> (sz1.w * scalar, sz1.h * scalar);
}

template <class T>
inline ATOM_Size2DT<T> operator * (int scalar, const ATOM_Size2DT<T> &sz1)
{
  return ATOM_Size2DT<T> (sz1.w * scalar, sz1.h * scalar);
}

template <class T>
inline ATOM_Size2DT<T> operator / (const ATOM_Size2DT<T> &sz1, int scalar)
{
  return ATOM_Size2DT<T> (sz1.w / scalar, sz1.h / scalar);
}

template <class T>
inline ATOM_Size2DT<T> operator * (const ATOM_Size2DT<T> &sz1, float scalar)
{
  return ATOM_Size2DT<T> (sz1.w * scalar, sz1.h * scalar);
}

template <class T>
inline ATOM_Size2DT<T> operator * (float scalar, const ATOM_Size2DT<T> &sz1)
{
  return ATOM_Size2DT<T> (sz1.w * scalar, sz1.h * scalar);
}

template <class T>
inline ATOM_Size2DT<T> operator / (const ATOM_Size2DT<T> &sz1, float scalar)
{
  return ATOM_Size2DT<T> (sz1.w / scalar, sz1.h / scalar);
}

template <class T>
inline ATOM_Size2DT<T> operator * (const ATOM_Size2DT<T> &sz1, double scalar)
{
  return ATOM_Size2DT<T> (sz1.w * scalar, sz1.h * scalar);
}

template <class T>
inline ATOM_Size2DT<T> operator * (double scalar, const ATOM_Size2DT<T> &sz1)
{
  return ATOM_Size2DT<T> (sz1.w * scalar, sz1.h * scalar);
}

template <class T>
inline ATOM_Size2DT<T> operator / (const ATOM_Size2DT<T> &sz1, double scalar)
{
  return ATOM_Size2DT<T> (sz1.w / scalar, sz1.h / scalar);
}

template <class T>
struct ATOM_Rect2DT
{
	ATOM_Point2DT<T> point;
	ATOM_Size2DT<T> size;

	ATOM_Rect2DT (void) 
	{
	}

	ATOM_Rect2DT (T x_, T y_, T w_, T h_)
	:point(x_, y_)
	,size(w_, h_) 
	{
	}

	ATOM_Rect2DT (const ATOM_Point2DT<T> &point_, const ATOM_Size2DT<T> &size_)
	:point(point_)
	,size(size_) 
	{
	}

	template <class U>
	ATOM_Rect2DT (const ATOM_Rect2DT<U> &other)
	:point(other.point)
	,size(other.size)
	{
	}

	template <class U>
	ATOM_Rect2DT<T> & operator = (const ATOM_Rect2DT<U> &other)
	{
		point = other.point;
		size = other.size;
		return *this;
	}

	bool isPointIn (T x_, T y_) const
	{
		return x_ >= point.x && x_ < point.x + size.w && y_ >= point.y && y_ < point.y + size.h;
	}

	template <class U>
	bool isRectIn (const ATOM_Rect2DT<U> &subRect) const
	{
		return (subRect.point.x >= point.x && subRect.point.x+subRect.size.w <= point.x+size.w && subRect.point.y >= point.y && subRect.point.y+subRect.size.h <= point.y+size.h );
	}

	template <class U>
	bool intersectedWith (const ATOM_Rect2DT<U> &other) const
	{
		return point.x < other.point.x + other.size.w && point.y < other.point.y + other.size.h && point.x + size.w > other.point.x && point.y + size.h > other.point.y;
	}

	static inline ATOM_Rect2DT<T> getIntersection (const ATOM_Rect2DT<T> &rc1, const ATOM_Rect2DT<T> &rc2)
	{
		T x1 = rc1.point.x + rc1.size.w;
		T y1 = rc1.point.y + rc1.size.h;
		T X1 = rc2.point.x + rc2.size.w;
		T Y1 = rc2.point.y + rc2.size.h;
		T iX = (rc1.point.x > rc2.point.x) ? rc1.point.x : rc2.point.x;
		T iY = (rc1.point.y > rc2.point.y) ? rc1.point.y : rc2.point.y;
		T iX1 = (x1 < X1) ? x1 : X1;
		T iY1 = (y1 < Y1) ? y1 : Y1;
		return ATOM_Rect2DT<T> (iX, iY, (iX < iX1) ? iX1-iX : 0, (iY < iY1) ? iY1-iY : 0);
	}
};

template <class T>
inline bool operator == (const ATOM_Rect2DT<T> &r1, const ATOM_Rect2DT<T> &r2)
{
  return r1.point == r2.point && r1.size == r2.size;
}

template <class T>
inline bool operator != (const ATOM_Rect2DT<T> &r1, const ATOM_Rect2DT<T> &r2)
{
  return ! operator == (r1, r2);
}

typedef ATOM_Point2DT<int> ATOM_Point2Di;
typedef ATOM_Size2DT<int> ATOM_Size2Di;
typedef ATOM_Rect2DT<int> ATOM_Rect2Di;

typedef ATOM_Point2DT<float> ATOM_Point2Df;
typedef ATOM_Size2DT<float> ATOM_Size2Df;
typedef ATOM_Rect2DT<float> ATOM_Rect2Df;

#endif // __ATOM3D_UTILS_RECT_H
