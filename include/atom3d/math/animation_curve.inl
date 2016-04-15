template <class ValueType, class ValueOwner, class LinearPolicy>
inline unsigned ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPointList::getCount(void) const 
{
  return _points.size();
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline const typename ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPoint & ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPointList::operator [] (unsigned index) const 
{
  return _points[index];
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline typename ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPoint & ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPointList::operator [] (unsigned index) 
{
  return _points[index];
}

template <class ValueType, class ValueOwner, class LinearPolicy>
unsigned ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPointList::getPeriod (void) const
{
	return _points.size() > 1 ? _points.back().x - _points.front().x : 0;
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline unsigned ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPointList::insert(long x, const ValueOwner &y) 
{
	unsigned n = 0;
	for (typename point_list::iterator it = _points.begin(); it != _points.end(); ++it, ++n)
	{
		if (x == it->x)
		{
			it->val = y;
			return n;
		}
		else if (x < it->x)
		{
			_points.insert (it, typename ATOM_AnimationCurve::KeyPoint(x, y));
			return n;
		}
	}
	_points.push_back (typename ATOM_AnimationCurve::KeyPoint(x, y));
	return _points.size() - 1;
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline void ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPointList::remove(unsigned index) 
{
	_points.erase(_points.begin() + index);
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline void ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPointList::clear() 
{
	_points.resize(0);
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::ATOM_AnimationCurve (void)
: _keyChanged(false) 
, _waveType(ATOM_WAVE_TYPE_SPLINE)
, _address(ATOM_CURVE_ADDRESS_CONTINUE)
{
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline unsigned ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::insertKey (long x, const ValueOwner &y) 
{
	_keyChanged = true;
	return _keyPoints.insert(x, y);
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline unsigned ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::findKey (long x) const
{
	for (unsigned i = 0; i < _keyPoints.getCount(); ++i)
	{
		if (_keyPoints[i].x == x)
		{
			return i;
		}
	}
	return unsigned(-1);
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline void ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::removeKey (unsigned index) 
{
	_keyChanged = true;
	_keyPoints.remove(index);
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline void ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::removeAllKeys (void) 
{
	_keyPoints.clear();
	_keyChanged = true;
}

template <class ValueType, class ValueOwner, class LinearPolicy>
void ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::setWaveType (ATOM_WaveType waveType)
{
	_waveType = waveType;
}

template <class ValueType, class ValueOwner, class LinearPolicy>
ATOM_WaveType ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::getWaveType (void) const
{
	return _waveType;
}

template <class ValueType, class ValueOwner, class LinearPolicy>
void ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::setAddress (ATOM_CurveAddress address)
{
	_address = address;
}

template <class ValueType, class ValueOwner, class LinearPolicy>
ATOM_CurveAddress ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::getAddress (void) const
{
	return _address;
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline ValueType ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::evalSpline(long x) const 
{
	if ( _keyPoints.getCount() == 2)
	{
		return evalLinear(x);
	}

	if ( _keyPoints.getCount() == 1)
	{
		return _keyPoints[0].value();
	}

	if ( _keyPoints.getCount() == 0)
	{
		return ValueType();
	}

	if ( _keyChanged)
	{
		_keyChanged = false;
		computeSpline();
	}

	int seg = getSegment(x);
	if ( seg < 0)
	{
		seg = 0;
	}
	else if ( seg >= int(_keyPoints.getCount() - 1))
	{
		seg = _keyPoints.getCount() - 2;
	}

	++seg;

	long t1 = x - _keyPoints[seg - 1].x;
	long t2 = _h[seg] - t1;

	return ((-_a[seg - 1] / 6.0 * (t2 + _h[seg]) * t1 + _keyPoints[seg - 1].value()) * t2	+ (-_a[seg] / 6.0 * (t1 + _h[seg]) * t2 + _keyPoints[seg].value()) * t1) / _h[seg];
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline ValueType ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::evalLinear(long x) const 
{
	int seg = getSegment(x);
	if ( seg < 0)
	{
		seg = 0;
	}
	else if ( seg >= int(_keyPoints.getCount() - 1))
	{
		seg = _keyPoints.getCount() - 2;
	}

	long t = x - _keyPoints[seg].x;
	//return _keyPoints[seg].value() + (_keyPoints[seg + 1].value() - _keyPoints[seg].value()) * t / (_keyPoints[seg + 1].x - _keyPoints[seg].x);

	return LinearPolicy().ipol(_keyPoints[seg].value(), _keyPoints[seg+1].value(), float(t)/(_keyPoints[seg+1].x - _keyPoints[seg].x));
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline ValueType ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::evalStep(long x) const 
{
	int count = _keyPoints.getCount();
	if (count == 1)
	{
		return _keyPoints[0].value();
	}
	else if (count == 0)
	{
		return ValueType();
	}

	int seg = getSegment(x);
	if ( seg < 0)
	{
		seg = 0;
	}
	else if ( seg >= count - 1)
	{
		seg = count - 1;
	}
	return _keyPoints[seg].value();
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline void ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::computeSpline(void) const 
{
	unsigned nk = _keyPoints.getCount();
	assert(nk > 2);

	std::vector<float> sub( nk - 1), diag(nk - 1), sup(nk - 1);
	_a.resize(nk);
	_h.resize(nk);
	_a[0] = 0.f;
	_a[nk-1] = 0.f;

	for ( unsigned i = 1; i < nk; ++i)
	{
		_h[i] = _keyPoints[i].x - _keyPoints[i - 1].x;
	}

	for ( unsigned i = 1; i < nk - 1; ++i)
	{
		diag[i] = (_h[i] + _h[i + 1]) / 3.0f;
		sup[i] = _h[i + 1] / 6.0f;
		sub[i] = _h[i] / 6.0f;
		_a[i] = (_keyPoints[i + 1].value() - _keyPoints[i].value()) / _h[i + 1] - (_keyPoints[i].value() - _keyPoints[i - 1].value()) / _h[i];
	}

	solveTridiag(sub, diag, sup);
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline void ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::solveTridiag(std::vector<float>& sub, std::vector<float>& diag, std::vector<float>& sup) const 
{
	unsigned n = _keyPoints.getCount() - 2;
	for (unsigned i = 2; i <= n; ++i)
	{
		sub[i] /= diag[i - 1];
		diag[i] -= sub[i] * sup[i - 1];
		_a[i] -= _a[i - 1] * sub[i] ;
	}
	_a[n] /= diag[n];
	for (unsigned i = n - 1; i >= 1; --i)
	{
		_a[i] = (_a[i] - _a[i + 1] * sup[i]) / diag[i];
	}
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline int ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::getSegment(long x) const 
{
	if ( x < _keyPoints[0].x)
	{
		return -1;
	}

	if ( x >= _keyPoints[_keyPoints.getCount() - 1].x)
	{
		return _keyPoints.getCount();
	}

	unsigned result;
	for ( result = 0; result < _keyPoints.getCount() - 1; ++result)
	{
		if ( x < _keyPoints[result + 1].x)
		{
			break;
		}
	}

	if ( result == _keyPoints.getCount())
	{
		result = _keyPoints.getCount() - 1;
	}

	return result;
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline ValueType ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::eval(long x) const 
{
	unsigned numKeys = getNumKeys ();
	if (numKeys > 1)
	{
		long period = _keyPoints.getPeriod ();

		switch (_address)
		{
		case ATOM_CURVE_ADDRESS_WRAP:
			{
				while (x < _keyPoints[0].x)
				{
					x += period;
				}
				while (x > _keyPoints[0].x + period)
				{
					x -= period;
				}
				break;
			}
		case ATOM_CURVE_ADDRESS_CLAMP:
			{
				if (x < _keyPoints[0].x)
				{
					x = _keyPoints[0].x;
				}
				else if (x > _keyPoints[0].x + period)
				{
					x = _keyPoints[0].x + period;
				}
				break;
			}
		default:
			break;
		}
	}
	else if (numKeys == 1)
	{
		return evalStep (x);
	}
	else
	{
		return ValueType();
	}

	switch (_waveType)
	{
	case ATOM_WAVE_TYPE_STEP	:
		return evalStep(x); 
	case ATOM_WAVE_TYPE_LINEAR:
		return evalLinear(x); 
	case ATOM_WAVE_TYPE_SPLINE:
	default:
		return evalSpline(x); 
	}
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline unsigned ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::getNumKeys (void) const 
{
	return _keyPoints.getCount();
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline const typename ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPoint & ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::operator [](unsigned index) const 
{
	return _keyPoints[index];
}

template <class ValueType, class ValueOwner, class LinearPolicy>
inline typename ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::KeyPoint & ATOM_AnimationCurve<ValueType, ValueOwner, LinearPolicy>::operator [](unsigned index) 
{
	return _keyPoints[index];
}
