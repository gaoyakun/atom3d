inline ATOM_BBox::ATOM_BBox (void):
  _minPoint(-0.5f, -0.5f, -0.5f),
  _maxPoint(0.5f, 0.5f, 0.5f)
{
}

inline ATOM_BBox::ATOM_BBox (const ATOM_Vector3f &vMin, const ATOM_Vector3f &vMax):
  _minPoint(vMin),
  _maxPoint(vMax)
{
}

inline const ATOM_Vector3f &ATOM_BBox::getMin (void) const
{
  return _minPoint;
}

inline const ATOM_Vector3f &ATOM_BBox::getMax (void) const
{
  return _maxPoint;
}

inline void ATOM_BBox::setMin (const ATOM_Vector3f &vMin)
{
  _minPoint = vMin;
}

inline void ATOM_BBox::setMax (const ATOM_Vector3f &vMax)
{
  _maxPoint = vMax;
}

inline ATOM_Vector3f ATOM_BBox::getCenter (void) const
{
  return 0.5f * (_minPoint + _maxPoint);
}

inline ATOM_Vector3f ATOM_BBox::getExtents (void) const
{
  return 0.5f * (_maxPoint - _minPoint);
}

inline ATOM_Vector3f ATOM_BBox::getSize (void) const
{
  return _maxPoint - _minPoint;
}

inline float ATOM_BBox::getDiagonalSize (void) const
{
  return (_maxPoint - _minPoint).getLength ();
}

inline void ATOM_BBox::transform (const ATOM_Matrix4x4f &matrix)
{
#if 0

  ATOM_Vector3f tmp, minp(FLT_MAX, FLT_MAX, FLT_MAX), maxp(-FLT_MAX, -FLT_MAX, -FLT_MAX), corners[8];

  corners[0] = _minPoint;
  corners[1].x = _minPoint.x; corners[1].y = _maxPoint.y; corners[1].z = _minPoint.z;
  corners[2].x = _maxPoint.x; corners[2].y = _maxPoint.y; corners[2].z = _minPoint.z;
  corners[3].x = _maxPoint.x; corners[3].y = _minPoint.y; corners[3].z = _minPoint.z;
  corners[4] = _maxPoint;
  corners[5].x = _minPoint.x; corners[5].y = _maxPoint.y; corners[5].z = _maxPoint.z;
  corners[6].x = _minPoint.x; corners[6].y = _minPoint.y; corners[6].z = _maxPoint.z;
  corners[7].x = _maxPoint.x; corners[7].y = _minPoint.y; corners[7].z = _maxPoint.z;

  for (int i = 0; i < 8; ++i)
  {
    ATOM_Vector3f tmp = matrix >> corners[i];
    if (tmp.x > maxp.x) maxp.x = tmp.x;
    if (tmp.y > maxp.y) maxp.y = tmp.y;
    if (tmp.z > maxp.z) maxp.z = tmp.z;
    if (tmp.x < minp.x) minp.x = tmp.x;
    if (tmp.y < minp.y) minp.y = tmp.y;
    if (tmp.z < minp.z) minp.z = tmp.z;
  }

  _minPoint = minp;
  _maxPoint = maxp;

#else

	ATOM_Vector3f min_org = _minPoint;
	ATOM_Vector3f max_org = _maxPoint;
	ATOM_Vector3f t = matrix.getRow3(3);

	int r;
	for ( int col = 0; col <3; ++col )
	{
		r = col;

		_minPoint[col] = _maxPoint[col] = t[col];
		
		for( int row = 0; row <3; ++row )
		{
			float e = matrix.m[r] * min_org[row];
			float f = matrix.m[r] * max_org[row];
			if( e < f )
			{
				_minPoint[col] += e;
				_maxPoint[col] += f;
			}
			else
			{
				_minPoint[col] += f;
				_maxPoint[col] += e;
			}

			r += 4;
		}
	}
#endif
}

inline bool ATOM_BBox::intersects (const ATOM_BBox &other) const
{
  if ((_maxPoint.x < other._minPoint.x) ||
      (_minPoint.x > other._maxPoint.x) ||
      (_maxPoint.y < other._minPoint.y) ||
      (_minPoint.y > other._maxPoint.y) ||
      (_maxPoint.z < other._minPoint.z) ||
      (_minPoint.z > other._maxPoint.z))
  {
    return false;
  }
  return true;
}

inline bool ATOM_BBox::contains (const ATOM_BBox &other) const
{
  if ((_minPoint.x <= other._minPoint.x) && (_maxPoint.x >= other._maxPoint.x) &&
      (_minPoint.y <= other._minPoint.y) && (_maxPoint.y >= other._maxPoint.y) &&
      (_minPoint.z <= other._minPoint.z) && (_maxPoint.z >= other._maxPoint.z))
  {
    return true;
  }
  return false;
}

inline bool ATOM_BBox::contains (const ATOM_Vector3f &point) const
{
  if ((_minPoint.x <= point.x) && (_maxPoint.x >= point.x) &&
      (_minPoint.y <= point.y) && (_maxPoint.y >= point.y) &&
      (_minPoint.z <= point.z) && (_maxPoint.z >= point.z))
  {
    return true;
  }
  return false;
}

inline ATOM_BBox::ClipState ATOM_BBox::getClipState (const ATOM_BBox &other) const
{
  if (contains (other))
  {
    return ClipInside;
  }
  else if (intersects (other))
  {
    return Clipped;
  }
  else
  {
    return ClipOutside;
  }
}

inline ATOM_BBox::ClipState ATOM_BBox::getClipStateMask (const ATOM_Matrix4x4f &viewProjection, unsigned mask) const
{
	int andFlags = 0xFFFF;
	int orFlags = 0;
	ATOM_Vector4f v0(0.f, 0.f, 0.f, 1.f);

	bool clipLeft = (mask & ClipLeft) != 0;
	bool clipRight = (mask & ClipRight) != 0;
	bool clipTop = (mask & ClipTop) != 0;
	bool clipBottom = (mask & ClipBottom) != 0;
	bool clipNear = (mask & ClipNear) != 0;
	bool clipFar = (mask & ClipFar) != 0;

	for (int i = 0; i < 8; ++i)
	{
		int clip = 0;
		v0.x = (i & 1) ? _minPoint.x : _maxPoint.x;
		v0.y = (i & 2) ? _minPoint.y : _maxPoint.y;
		v0.z = (i & 4) ? _minPoint.z : _maxPoint.z;

		ATOM_Vector4f v1 = viewProjection >> v0;

		if (clipLeft && v1.x < -v1.w)
		{
			clip |= ClipLeft;
		}
		else if (clipRight && v1.x > v1.w)
		{
			clip |= ClipRight;
		}

		if (clipBottom && v1.y < -v1.w)
		{
			clip |= ClipBottom;
		}
		else if (clipTop && v1.y > v1.w)
		{
			clip |= ClipTop;
		}

		if (clipFar && v1.z < -v1.w)
		{
			clip |= ClipFar;
		}
		else if (clipNear && v1.z > v1.w)
		{
			clip |= ClipNear;
		}

		andFlags &= clip;
		orFlags |= clip;
	}

	if (0 == orFlags)
	{
		return ATOM_BBox::ClipInside;
	}
	else if (0 != andFlags)
	{
		return ATOM_BBox::ClipOutside;
	}
	else
	{
		return ATOM_BBox::Clipped;
	}
}

inline ATOM_BBox::ClipState ATOM_BBox::getClipState (const ATOM_Matrix4x4f &viewProjection) const
{
  int andFlags = 0xFFFF;
  int orFlags = 0;
  ATOM_Vector4f v0(0.f, 0.f, 0.f, 1.f);

  for (int i = 0; i < 8; ++i)
  {
    int clip = 0;
    v0.x = (i & 1) ? _minPoint.x : _maxPoint.x;
    v0.y = (i & 2) ? _minPoint.y : _maxPoint.y;
    v0.z = (i & 4) ? _minPoint.z : _maxPoint.z;

    ATOM_Vector4f v1 = viewProjection >> v0;

    if (v1.x < -v1.w)
    {
      clip |= ClipLeft;
    }
    else if (v1.x > v1.w)
    {
      clip |= ClipRight;
    }

    if (v1.y < -v1.w)
    {
      clip |= ClipBottom;
    }
    else if (v1.y > v1.w)
    {
      clip |= ClipTop;
    }

    if (v1.z < -v1.w)
    {
      clip |= ClipFar;
    }
    else if (v1.z > v1.w)
    {
      clip |= ClipNear;
    }

    andFlags &= clip;
    orFlags |= clip;
  }

  if (0 == orFlags)
  {
    return ATOM_BBox::ClipInside;
  }
  else if (0 != andFlags)
  {
    return ATOM_BBox::ClipOutside;
  }
  else
  {
    return ATOM_BBox::Clipped;
  }
}

inline ATOM_BBox::ClipState ATOM_BBox::getClipState (const ATOM_Frustum3d &frustum) const
{
	bool bad_intesect = false;
	ATOM_Vector3f center = getCenter();
	ATOM_Vector3f extends = getExtents();

	for (int i = 0; i < 6; ++i) 
	{
		const ATOM_Plane &p = frustum[i];	

		if (p.getA() * (center.x + p.getPX() * extends.x) +
			p.getB() * (center.y + p.getPY() * extends.y) +
			p.getC() * (center.z + p.getPZ() * extends.z) +
			p.getD() < 0)
			return ATOM_BBox::ClipOutside;

		if (p.getA() * (center.x + p.getNX() * extends.x) +
			p.getB() * (center.y + p.getNY() * extends.y) +
			p.getC() * (center.z + p.getNZ() * extends.z) +
			p.getD() < 0)
			bad_intesect  = true;
	}

	return bad_intesect ? ATOM_BBox::Clipped : ATOM_BBox::ClipInside;
}

inline ATOM_BBox::ClipState ATOM_BBox::getClipStateMask (const ATOM_Frustum3d &frustum, unsigned mask) const
{
	bool bad_intesect = false;
	ATOM_Vector3f center = getCenter();
	ATOM_Vector3f extends = getExtents();

	for (int i = 0; i < 6; ++i) 
	{
		if ((mask & (1<<i)) == 0)
		{
			continue;
		}

		const ATOM_Plane &p = frustum[i];	

		if (p.getA() * (center.x + p.getPX() * extends.x) +
			p.getB() * (center.y + p.getPY() * extends.y) +
			p.getC() * (center.z + p.getPZ() * extends.z) +
			p.getD() < 0)
			return ATOM_BBox::ClipOutside;

		if (p.getA() * (center.x + p.getNX() * extends.x) +
			p.getB() * (center.y + p.getNY() * extends.y) +
			p.getC() * (center.z + p.getNZ() * extends.z) +
			p.getD() < 0)
			bad_intesect  = true;
	}

	return bad_intesect ? ATOM_BBox::Clipped : ATOM_BBox::ClipInside;
}

inline ATOM_BBox::ClipState ATOM_BBox::getClipState (const ATOM_Matrix4x4f &viewProjection, float minScreenContribute) const
{
  int andFlags = 0xFFFF;
  int orFlags = 0;
  ATOM_Vector4f v0(0.f, 0.f, 0.f, 1.f);
  ATOM_Vector4f v[8];

  for (int i = 0; i < 8; ++i)
  {
	ATOM_Vector4f &v1 = v[i];

	int clip = 0;
    v0.x = (i & 1) ? _minPoint.x : _maxPoint.x;
    v0.y = (i & 2) ? _minPoint.y : _maxPoint.y;
    v0.z = (i & 4) ? _minPoint.z : _maxPoint.z;

    v1 = viewProjection >> v0;

    if (v1.x < -v1.w)
    {
      clip |= ClipLeft;
    }
    else if (v1.x > v1.w)
    {
      clip |= ClipRight;
    }

    if (v1.y < -v1.w)
    {
      clip |= ClipBottom;
    }
    else if (v1.y > v1.w)
    {
      clip |= ClipTop;
    }

    if (v1.z < -v1.w)
    {
      clip |= ClipFar;
    }
    else if (v1.z > v1.w)
    {
      clip |= ClipNear;
    }

    andFlags &= clip;
    orFlags |= clip;
  }

  ATOM_BBox::ClipState state;

  if (0 == orFlags)
  {
    state = ATOM_BBox::ClipInside;
  }
  else if (0 != andFlags)
  {
    return ATOM_BBox::ClipOutside;
  }
  else
  {
    state = ATOM_BBox::Clipped;
  }

  if (minScreenContribute > 0.f)
  {
	  float minX = 2.f;
	  float minY = 2.f;
	  float maxX = -2.f;
	  float maxY = -2.f;

	  for (int i = 0; i < 8; ++i)
	  {
		  float z = v[i].z / v[i].w;
		  if (z < 0.f || z > 1.f)
		  {
			  continue;
		  }
		  float x = v[i].x / v[i].w;
		  if (x < minX) minX = x;
		  if (x > maxX) maxX = x;
		  float y = v[i].y / v[i].w;
		  if (y < minY) minY = y;
		  if (y > maxY) maxY = y;
	  }

	  if (minX < -1.f) minX = -1.f; else if (minX > 1.f) minX = 1.f;
	  if (minY < -1.f) minY = -1.f; else if (minY > 1.f) minY = 1.f;
	  if (maxX < -1.f) maxX = -1.f; else if (maxX > 1.f) maxX = 1.f;
	  if (maxY < -1.f) maxY = -1.f; else if (maxY > 1.f) maxY = 1.f;
	  float contrib = (maxX - minX) * (maxY - minY);

	  if (contrib < minScreenContribute)
	  {
		  return ATOM_BBox::ClipOutside;
	  }
  }

  return state;
}

inline ATOM_Vector3f ATOM_BBox::point(int i) const 
{ 
	return ATOM_Vector3f( (i&1)?_minPoint.x:_maxPoint.x, (i&2)?_minPoint.y:_maxPoint.y, (i&4)?_minPoint.z:_maxPoint.z );  
}

inline bool ATOM_BBox::intersect (const ATOM_Vector3f &lineBegin, const ATOM_Vector3f &lineVector, ATOM_Vector3f &intersectionPoint) const
{
  if (contains (lineBegin))
  {
    intersectionPoint = lineBegin;
    return true;
  }

  int plane[3];
  plane[0] = lineBegin.x > 0 ? 0 : 1;
  plane[1] = lineBegin.y > 0 ? 2 : 3;
  plane[2] = lineBegin.z > 0 ? 4 : 5;

  for (int i = 0; i < 3; ++i)
  {
    switch (plane[i])
    {
    case 0:
      if (_intersectConstX (_minPoint.x, lineBegin, lineVector, intersectionPoint) && _pipConstX (intersectionPoint))
      {
        return true;
      }
      break;
    case 1:
      if (_intersectConstX (_maxPoint.x, lineBegin, lineVector, intersectionPoint) && _pipConstX (intersectionPoint))
      {
        return true;
      }
      break;
    case 2:
      if (_intersectConstY (_minPoint.y, lineBegin, lineVector, intersectionPoint) && _pipConstY (intersectionPoint))
      {
        return true;
      }
      break;
    case 3:
      if (_intersectConstY (_maxPoint.y, lineBegin, lineVector, intersectionPoint) && _pipConstY (intersectionPoint))
      {
        return true;
      }
      break;
    case 4:
      if (_intersectConstZ (_minPoint.z, lineBegin, lineVector, intersectionPoint) && _pipConstZ (intersectionPoint))
      {
        return true;
      }
      break;
    case 5:
      if (_intersectConstZ (_maxPoint.z, lineBegin, lineVector, intersectionPoint) && _pipConstZ (intersectionPoint))
      {
        return true;
      }
      break;
    }
  }
  return false;
}

inline bool ATOM_BBox::_intersectConstX (const float x, const ATOM_Vector3f &lineBegin, const ATOM_Vector3f &lineVector, ATOM_Vector3f &out) const
{
  if (lineVector.x != 0.f)
  {
    float t = (x - lineBegin.x) / lineVector.x;
    if (t >= 0.f && t < 1.f)
    {
      out = lineBegin + lineVector * t;
      return true;
    }
  }
  return false;
}

inline bool ATOM_BBox::_intersectConstY (const float y, const ATOM_Vector3f &lineBegin, const ATOM_Vector3f &lineVector, ATOM_Vector3f &out) const
{
  if (lineVector.y != 0.f)
  {
    float t = (y - lineBegin.y) / lineVector.y;
    if (t >= 0.f && t < 1.f)
    {
      out = lineBegin + lineVector * t;
      return true;
    }
  }
  return false;
}

inline bool ATOM_BBox::_intersectConstZ (const float z, const ATOM_Vector3f &lineBegin, const ATOM_Vector3f &lineVector, ATOM_Vector3f &out) const
{
  if (lineVector.z != 0.f)
  {
    float t = (z - lineBegin.z) / lineVector.z;
    if (t >= 0.f && t < 1.f)
    {
      out = lineBegin + lineVector * t;
      return true;
    }
  }
  return false;
}

inline bool ATOM_BBox::_pipConstX (const ATOM_Vector3f &v) const
{
  if (v.y >= _minPoint.y && v.y <= _maxPoint.y && v.z >= _minPoint.z && v.z <= _maxPoint.z)
  {
    return true;
  }
  return false;
}

inline bool ATOM_BBox::_pipConstY (const ATOM_Vector3f &v) const
{
  if (v.x >= _minPoint.x && v.x <= _maxPoint.x && v.z >= _minPoint.z && v.z <= _maxPoint.z)
  {
    return true;
  }
  return false;
}

inline bool ATOM_BBox::_pipConstZ (const ATOM_Vector3f &v) const
{
  if (v.x >= _minPoint.x && v.x <= _maxPoint.x && v.y >= _minPoint.y && v.y <= _maxPoint.y)
  {
    return true;
  }
  return false;
}

inline void ATOM_BBox::extend (const ATOM_Vector3f &v)
{
	if (v.x < _minPoint.x)
	{
		_minPoint.x = v.x;
	}

	if (v.x > _maxPoint.x)
	{
		_maxPoint.x = v.x;
	}

	if (v.y < _minPoint.y)
	{
		_minPoint.y = v.y;
	}

	if (v.y > _maxPoint.y)
	{
		_maxPoint.y = v.y;
	}

	if (v.z < _minPoint.z)
	{
		_minPoint.z = v.z;
	}

	if (v.z > _maxPoint.z)
	{
		_maxPoint.z = v.z;
	}
}

inline void ATOM_BBox::extend (const ATOM_Vector3f *verts, unsigned numVerts)
{
	for (unsigned i = 0; i < numVerts; ++i)
	{
		extend (verts[i]);
	}
}

inline void ATOM_BBox::beginExtend (void)
{
	_minPoint.set (FLT_MAX, FLT_MAX, FLT_MAX);
	_maxPoint.set (-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

inline bool ATOM_BBox::isValid (void) const
{
	if (_minPoint.x > _maxPoint.x)
	{
		return false;
	}

	if (_minPoint.y > _maxPoint.y)
	{
		return false;
	}

	if (_minPoint.z > _maxPoint.z)
	{
		return false;
	}

	return true;
}

inline bool operator == (const ATOM_BBox &bbox1, const ATOM_BBox &bbox2)
{
	return bbox1._minPoint == bbox2._minPoint && bbox1._maxPoint == bbox2._maxPoint;
}

inline bool operator != (const ATOM_BBox &bbox1, const ATOM_BBox &bbox2)
{
	return ! operator == (bbox1, bbox2);
}

