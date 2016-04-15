#ifndef ATOM_GEOMETRY_AABB_H_
#define ATOM_GEOMETRY_AABB_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include"ray.h"

template<typename T>
class ATOM_AABB
{
  typedef ATOM_Vector3f	Vector3;
  typedef ATOM_Vector4f	Vector4;
  typedef ATOM_Matrix4x4f	Matrix44;
private:
  Vector3		_Center, _HalfWidth;

public:
	inline ATOM_AABB()
	{
		reset();
	}
	inline ATOM_AABB(const Vector3 &center, const Vector3 &extends)
	{
		setCenter (center);
		setHalfWidth (extends);
	}
	inline void	reset()
	{
		_Center.set(0.f, 0.f, 0.f);
		_HalfWidth.set(0.f, 0.f, 0.f);
	}

public:
	inline bool				isZero() const					{return _Center.IsZero()&&_HalfWidth.IsZero();}
	inline void				setCenter(const Vector3 &V)		{_Center= V;		}
	inline void				setHalfWidth(const Vector3 &V)	{_HalfWidth	= V;	}
	inline const Vector3&	getCenter() const				{return _Center;	}
	inline const Vector3&	getHalfWidth() const			{return _HalfWidth;	}
	inline T          getCenterX() const { return _Center.x(); }
	inline T          getCenterY() const { return _Center.y(); }
	inline T          getCenterZ() const { return _Center.z(); }
	inline T          getHalfWidthX() const { return _HalfWidth.x(); }
	inline T          getHalfWidthY() const { return _HalfWidth.y(); }
	inline T          getHalfWidthZ() const { return _HalfWidth.z(); }

#define _AABB_MIN_SELF (getCenter() - getHalfWidth())
#define _AABB_MAX_SELF (getCenter() + getHalfWidth())
#define _AABB_MIN_OTHER (Other.getCenter() - Other.getHalfWidth())
#define _AABB_MAX_OTHER (Other.getCenter() + Other.getHalfWidth())

  inline bool contains(const ATOM_AABB<T> &Other) const
  {
    return _AABB_MIN_SELF <= _AABB_MIN_OTHER && _AABB_MAX_SELF >= _AABB_MAX_OTHER;
  }

  inline bool contains2(const ATOM_AABB<T> &Other) const
  {
    return _AABB_MIN_SELF < _AABB_MIN_OTHER && _AABB_MAX_SELF > _AABB_MAX_OTHER;
  }

  inline bool outside(const ATOM_AABB<T> &Other) const
  {
    return !(_AABB_MIN_SELF < _AABB_MAX_OTHER) || !(_AABB_MIN_OTHER < _AABB_MAX_SELF);
  }

  inline bool outside2(const ATOM_AABB<T> &Other) const
  {
    return !(_AABB_MIN_SELF <= _AABB_MAX_OTHER) || !(_AABB_MIN_OTHER <= _AABB_MAX_SELF);
  }

#undef _AABB_MIN_SELF
#undef _AABB_MAX_SELF
#undef _AABB_MIN_OTHER
#undef _AABB_MAX_OTHER

  inline void unite(const Vector3 &v)
  {
    Vector3 vmin = _Center - _HalfWidth;
    Vector3 vmax = _Center + _HalfWidth;

    if (v.x() < vmin.x())
      vmin.x() = v.x();
    if (v.x() > vmax.x())
      vmax.x() = v.x();
    if (v.y() < vmin.y())
      vmin.y() = v.y();
    if (v.y() > vmax.y())
      vmax.y() = v.y();
    if (v.z() < vmin.z())
      vmin.z() = v.z();
    if (v.z() > vmax.z())
      vmax.z() = v.z();

    _Center = (vmin + vmax) * 0.5f;
    _HalfWidth = (vmax - vmin) * 0.5f;
  }

	inline void				unite(const ATOM_AABB<T>	&Other)		//根据别的AABB，来合并成自己的AABB；
	{
		const Vector3 &ocenter		= Other.getCenter();
		const Vector3 &ohalfwidth	= Other.getHalfWidth();
		if( !ohalfwidth[0] && !ohalfwidth[1]  && !ohalfwidth[2] )
			return;
		Vector3 v1, v2;
		for(int i=0;i<3;i++)
		{
			//ATOM_ASSERT( _HalfWidth[i]>=0 && ohalfwidth[i] );
			if( _Center[i]+_HalfWidth[i] < ocenter[i]+ohalfwidth[i] )
				v2[i]				= ocenter[i]+ohalfwidth[i];
			else
				v2[i]				= _Center[i]+_HalfWidth[i];
			if( _Center[i]-_HalfWidth[i] > ocenter[i]-ohalfwidth[i] )
				v1[i]				= ocenter[i]-ohalfwidth[i];
			else
				v1[i]				= _Center[i]-_HalfWidth[i];
		}
		_Center						= (v1+v2)/2;
		_HalfWidth					= v1-_Center;
		_HalfWidth.Abs();
	}

	//----------------------------------------------------------------------
	void transform(const Matrix44 &Matrix)
	{
		Vector3 v[8];
		v[0][0]= _Center[0] + _HalfWidth[0];
		v[0][1]= _Center[1] + _HalfWidth[1];
		v[0][2]= _Center[2] + _HalfWidth[2];

		v[1][0]= _Center[0] + _HalfWidth[0];
		v[1][1]= _Center[1] + _HalfWidth[1];
		v[1][2]= _Center[2] - _HalfWidth[2];

		v[2][0]= _Center[0] - _HalfWidth[0];
		v[2][1]= _Center[1] + _HalfWidth[1];
		v[2][2]= _Center[2] - _HalfWidth[2];

		v[3][0]= _Center[0] - _HalfWidth[0];
		v[3][1]= _Center[1] + _HalfWidth[1];
		v[3][2]= _Center[2] + _HalfWidth[2];

		v[4][0]= _Center[0] + _HalfWidth[0];
		v[4][1]= _Center[1] - _HalfWidth[1];
		v[4][2]= _Center[2] + _HalfWidth[2];

		v[5][0]= _Center[0] + _HalfWidth[0];
		v[5][1]= _Center[1] - _HalfWidth[1];
		v[5][2]= _Center[2] - _HalfWidth[2];

		v[6][0]= _Center[0] - _HalfWidth[0];
		v[6][1]= _Center[1] - _HalfWidth[1];
		v[6][2]= _Center[2] - _HalfWidth[2];

		v[7][0]= _Center[0] - _HalfWidth[0];
		v[7][1]= _Center[1] - _HalfWidth[1];
		v[7][2]= _Center[2] + _HalfWidth[2];
		//------------------------------------------------------------------
		Vector3 center, halfwidth, min, max;
		for(int i=0;i<8;i++)
		{
			v[i]= Matrix.TransformVertex(v[i]);
			if( i == 0 )
			{
				min= v[i];
				max= v[i];
				continue;
			}
			for(int j=0;j<3;j++)
			{
				if( min[j] > v[i][j] )
					min[j]= v[i][j];
				if( max[j] < v[i][j] )
					max[j]= v[i][j];
			}
		}
		setCenter((min+max)/2);
		setHalfWidth((max-min)/2);
	}

	//----------------------------------------------------------------------
	inline bool		intersectPoint(const Vector3 &Point, int nSkip= -1) const
	{
		Vector3 tp= Point - _Center;
		for(int i=0;i<3;i++)
		{
			if( nSkip == i )
				continue;
			if( fabs(tp[i]) > _HalfWidth[i] )
				return false;
		}
		return true;
	}
	//fDistance[out]---- if return true, fDistance is the distance from ray origin to the intersect point
	inline bool		intersectRay(const ATOM_Ray<T> &TheRay, T &fDistance) const
	{
#if 1
    T d[2];

    if (TheRay.getLength() == 0.f)
    {
      if (intersection_find_AABB_Ray (getCenter(), getHalfWidth(), TheRay.getOrigin(), TheRay.getDirection(), d))
      {
        fDistance = TheRay.getLength() * d[0];
        return true;
      }
    }
    else
    {
      BaseMatrix<T, 1, 3> dir = TheRay.getDirection();
      dir.SetLength (TheRay.getLength());
      if (intersection_find_AABB_Segment (getCenter(), getHalfWidth(), TheRay.getOrigin(), dir, d))
      {
        fDistance = TheRay.getLength() * d[0];
        return true;
      }
    }
    return false;
#else
		const Vector3 &dir		= TheRay.getDirection();
		const Vector3 &origin	= TheRay.getOrigin();
		//------------------------------------------------------------------
		bool b_intersect		= false;
		Vector3 t_pos;
		fDistance				= 0;
		Vector4 t_plane;						//平面方程的ax+by+cz=d
		for(int i=0;i<6;i++)					//6 planes
		{
			t_plane.Zero();
			int n_faceindex		= i%3;
			t_plane[n_faceindex]= 1;
			t_plane[3]			= _Center[n_faceindex] + ((i<3)?_HalfWidth[n_faceindex]:(-_HalfWidth[n_faceindex]));
			Vector3 normal(Vector3(t_plane[0], t_plane[1], t_plane[2]));
			if( i >= 3 )
				normal			= -normal;
			if( 0 > dir.Dot(normal) )		//法线方向不合适，跳过
				continue;
			T k					= t_plane[0]*dir[0] + t_plane[1]*dir[1] + t_plane[2]*dir[2];
			if( !k )
				continue;
			for(int j=0;j<3;j++)
			{
				int oj0			= (j+1)%3;
				int oj1			= (j+2)%3;
				t_pos[j]		= dir[j] * (t_plane[3] - t_plane[oj0]*origin[oj0] - t_plane[oj1]*origin[oj1]) +
									origin[j] * (t_plane[oj0]*dir[oj0] + t_plane[oj1]*dir[oj1]);
			}
			t_pos				/= k;
			if( intersectPoint(t_pos, n_faceindex) )
			{
				b_intersect		= true;
				T l				= (t_pos-origin).Length();
				if( !fDistance )
					fDistance	= l;
				else
				{
					if( fDistance > l )
						fDistance	= l;
				}
			}
		}
		if( TheRay.getLength() )
		{
			if( TheRay.getLength() < fDistance )
				return false;
		}
		return b_intersect;
#endif
	}
};
typedef ATOM_AABB<float>	Aabbf;

#endif//ATOM_GEOMETRY_AABB_H_
