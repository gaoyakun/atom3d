#ifndef __ATOM3D_UTILS_CACHEDQUEUE_H
#define __ATOM3D_UTILS_CACHEDQUEUE_H

#if 0
#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>

template <class T, class Al = std::allocator<T> >
class ATOM_CachedQueue
{
public:
	ATOM_CachedQueue (void);
	ATOM_CachedQueue (unsigned size);

public:
	unsigned getSize (void) const;
	void resize (unsigned newSize);
	bool isEmpty (void) const;
	void pushBack (const T &val);
	void popBack (void);
	void pushFront (const T &val);
	void popFront (void);
	void insert (unsigned pos, const T &val);
	void remove (unsigned pos);
	T &at (unsigned pos);
	const T &at (unsigned pos) const;
	T & operator [] (unsigned pos);
	const T & operator [] (unsigned pos) const;

private:
	unsigned _first;
	unsigned _last;
	std::vector<T, Al> _vec;
};

template <class T, class Al>
inline ATOM_CachedQueue<T, Al>::ATOM_CachedQueue (void)
: _first(0)
, _last(0)
{
}

template <class T, class Al>
inline ATOM_CachedQueue<T, Al>::ATOM_CachedQueue (unsigned size)
: _first(0)
, _last(size)
, _vec(size)
{
}

template <class T, class Al>
inline unsigned ATOM_CachedQueue<T, Al>::getSize (void) const
{
	return _last - _first;
}

template <class T, class Al>
inline void ATOM_CachedQueue<T, Al>::resize (unsigned newSize)
{
	_vec.resize (newSize);
	_first = 0;
	_last = newSize;
}

template <class T, class Al>
inline bool ATOM_CachedQueue<T, Al>::isEmpty (void) const
{
	return _first == _last;
}

template <class T, class Al>
inline void ATOM_CachedQueue<T, Al>::pushBack (const T &val)
{
	if (_last == _vec.size())
	{
		_vec.push_back (val);
	}
	else
	{
		_vec[_last] = val;
	}
	++_last;
}

template <class T, class Al>
inline void ATOM_CachedQueue<T, Al>::popBack (void)
{
	if (_last > _first)
	{
		--_last;
	}
}

template <class T, class Al>
inline void ATOM_CachedQueue<T, Al>::pushFront (const T &val)
{
	if (_first > 0)
	{
		_vec[--_first] = val;
	}
	else
	{
		_vec.insert (_vec.begin(), val);
		++_last;
	}
}

template <class T, class Al>
inline void ATOM_CachedQueue<T, Al>::popFront (void)
{
}

template <class T, class Al>
inline void ATOM_CachedQueue<T, Al>::insert (unsigned pos, const T &val)
{
}

template <class T, class Al>
inline void ATOM_CachedQueue<T, Al>::remove (unsigned pos)
{
}

template <class T, class Al>
inline T &ATOM_CachedQueue<T, Al>::at (unsigned pos)
{
}

template <class T, class Al>
inline const T &ATOM_CachedQueue<T, Al>::at (unsigned pos) const
{
}

template <class T, class Al>
inline T & ATOM_CachedQueue<T, Al>::operator [] (unsigned pos)
{
}

template <class T, class Al>
inline const T & ATOM_CachedQueue<T, Al>::operator [] (unsigned pos) const
{
}

#endif

#endif // __ATOM3D_UTILS_CACHEDQUEUE_H
