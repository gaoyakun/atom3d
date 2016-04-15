#ifndef __ATOM_DEBUGHELP_POOL_H__
#define __ATOM_DEBUGHELP_POOL_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_thread.h"
#include "new_wrapper.h"
#include "stl.h"

#if defined(ATOM_NO_POOL)

#define ATOM_POOL_NEW ATOM_NEW
#define ATOM_POOL_DELETE ATOM_DELETE
#define ATOM_POOL_PURGE ATOM_DELETE

#else

#define ATOM_POOL_NEW(classname, ...) PoolAlloc< classname >::alloc(__VA_ARGS__)
#define ATOM_POOL_DELETE(p) ATOM_PoolGarbage(p)
#define ATOM_POOL_PURGE(p) ATOM_PoolDelete(p)

#endif

typedef void (ATOM_CALL *destructFunc) (void*);

class ATOM_DBGHLP_API p_allocator_base
{
public:
	p_allocator_base (unsigned size);
	virtual ~p_allocator_base (void);
public:
	virtual void *allocate (destructFunc d);
	virtual void garbage (void *p);
	virtual void deallocate (void *p);
	virtual void purge (void);
private:
	struct _al_instance *_freelist;
	unsigned _size;
	ATOM_FastMutex _lock;
};

ATOM_DBGHLP_API void ATOM_CALL ATOM_PoolGarbage (void *p);
ATOM_DBGHLP_API void ATOM_CALL ATOM_PoolDelete (void *p);
ATOM_DBGHLP_API void ATOM_CALL ATOM_PoolPurge (void);
ATOM_DBGHLP_API p_allocator_base *getBaseAllocator (unsigned size);

template <class T, bool pod>
struct destructorHelper
{
	void operator () (void *p)
	{
		((T*)p)->~T();
	}
};

template <class T>
struct destructorHelper<T, true>
{
	void operator () (void *p)
	{
	}
};

template <class T>
inline void ATOM_CALL destructor (void *p)
{
	destructorHelper<T, ATOM_IsPOD<T>::result>()(p);
}

template <class T>
inline destructFunc getDestructor (void)
{
	return &destructor<T>;
}

template <class T>
class PoolAlloc
{
	static p_allocator_base *getAllocator (void)
	{
		static p_allocator_base *alloc = getBaseAllocator(sizeof(T));
		return alloc;
	}
public:
	static T *_alloc (void)
	{
		p_allocator_base *al = getAllocator ();
		return (T*)al->allocate (getDestructor<T>());
	}

	static T * alloc (void)
	{
		T *p = _alloc ();
		if (p)
		{
			new (p) T;
		}
		return p;
	}

	template <class A1>
	static T * alloc (A1 a1)
	{
		T *p = _alloc ();
		if (p)
		{
			new (p) T(a1);
		}
		return p;
	}

	template <class A1, class A2>
	static T * alloc (A1 a1, A2 a2)
	{
		T *p = _alloc ();
		if (p)
		{
			new (p) T(a1,a2);
		}
		return p;
	}

	template <class A1, class A2, class A3>
	static T * alloc (A1 a1, A2 a2, A3 a3)
	{
		T *p = _alloc ();
		if (p)
		{
			new (p) T(a1,a2,a3);
		}
		return p;
	}

	template <class A1, class A2, class A3, class A4>
	static T * alloc (A1 a1, A2 a2, A3 a3, A4 a4)
	{
		T *p = _alloc ();
		if (p)
		{
			new (p) T(a1,a2,a3,a4);
		}
		return p;
	}

	template <class A1, class A2, class A3, class A4, class A5>
	static T * alloc (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
	{
		T *p = _alloc ();
		if (p)
		{
			new (p) T(a1,a2,a3,a4,a5);
		}
		return p;
	}

	template <class A1, class A2, class A3, class A4, class A5, class A6>
	static T * alloc (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
	{
		T *p = _alloc ();
		if (p)
		{
			new (p) T(a1,a2,a3,a4,a5,a6);
		}
		return p;
	}

	template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	static T * alloc (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
	{
		T *p = _alloc ();
		if (p)
		{
			new (p) T(a1,a2,a3,a4,a5,a6,a7);
		}
		return p;
	}

	template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	static T * alloc (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
	{
		T *p = _alloc ();
		if (p)
		{
			new (p) T(a1,a2,a3,a4,a5,a6,a7,a8);
		}
		return p;
	}

	template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	static T * alloc (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
	{
		T *p = _alloc ();
		if (p)
		{
			new (p) T(a1,a2,a3,a4,a5,a6,a7,a8,a9);
		}
		return p;
	}

	template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
	static T * alloc (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
	{
		T *p = _alloc ();
		if (p)
		{
			new (p) T(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
		}
		return p;
	}
};

// STL pool allocator
template<class T>
struct ATOM_pool_allocator_base
{
	typedef T value_type;
};

template<class T>
struct ATOM_pool_allocator_base<const T>
{
	typedef T value_type;
};

template <class T>
class ATOM_pool_allocator: public ATOM_pool_allocator_base<T>
{
public:
  typedef ATOM_pool_allocator_base<T> basetype;
  typedef typename basetype::value_type value_type;
  typedef value_type *pointer;
  typedef value_type &reference;
  typedef const value_type *const_pointer;
  typedef const value_type &const_reference;
  typedef unsigned size_type;
  typedef std::ptrdiff_t difference_type;

  template <class U>
  struct rebind
  {
    typedef ATOM_pool_allocator<U> other;
  };

  pointer address(reference val) const
  {
    return &val;
  }

  const_pointer address(const_reference val) const
  {
    return &val;
  }

  ATOM_pool_allocator (void)
  {
  }

  ATOM_pool_allocator (const ATOM_pool_allocator<T> &)
  {
  }

  template <class U>
  ATOM_pool_allocator (const ATOM_pool_allocator<U>&)
  {
  }

  template <class U>
  ATOM_pool_allocator<T> &operator=(const ATOM_pool_allocator<U>&)
  {
    return *this;
  }

  void deallocate(pointer p, size_type)
  {
	ATOM_PoolGarbage (p);
  }

  pointer allocate(size_type count)
  {
	  return PoolAlloc<T>::alloc ();
  }

  pointer allocate(size_type count, const void *)
  {
    return allocate(count);
  }

  void construct(pointer p, const T &val)
  {
    new (p) T (val);
  }

  void destroy(pointer p)
  {
    if (!ATOM_IsPOD<T>::result)
    {
      p->~T();
    }
  }

  size_type max_size(void) const
  {
    size_type count = (size_type)(-1) / sizeof(T);
    return ((0 < count) ? count : 1);
  }
};

template<class T, class U> 
inline
bool operator==(const ATOM_pool_allocator<T>&, const ATOM_pool_allocator<U>&)
{
	return true;
}

template<class T, class U> 
inline
bool operator!=(const ATOM_pool_allocator<T>&, const ATOM_pool_allocator<U>&)
{
	return false;
}

template<> 
class ATOM_pool_allocator<void>
{
public:
	typedef void T;
	typedef T *pointer;
	typedef const T *const_pointer;
	typedef T value_type;

	template<class U>
	struct rebind
	{
		typedef ATOM_pool_allocator<U> other;
	};

	ATOM_pool_allocator()
	{
	}

	ATOM_pool_allocator(const ATOM_pool_allocator<void>&)
	{
	}

	template<class U>
	ATOM_pool_allocator(const ATOM_pool_allocator<U>&)
	{
	}

	template<class U>
	ATOM_pool_allocator<void>& operator=(const ATOM_pool_allocator<U>&)
	{
		return (*this);
	}
};

template <class T>
class ATOM_PoolVector: public std::vector<T, ATOM_pool_allocator<T> >
{
public:
  typedef std::vector<T, ATOM_pool_allocator<T> > basetype;
  typedef typename basetype::allocator_type allocator_type;
  typedef typename basetype::size_type size_type;
  typedef typename basetype::difference_type difference_type;
  typedef typename basetype::pointer pointer;
  typedef typename basetype::const_pointer const_pointer;
  typedef typename basetype::reference reference;
  typedef typename basetype::const_reference const_reference;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::iterator iterator;
  typedef typename basetype::const_iterator const_iterator;

public:
  ATOM_PoolVector (): basetype () {}
  explicit ATOM_PoolVector (const allocator_type &_Al): basetype (_Al) {}
  explicit ATOM_PoolVector (size_type _Count): basetype (_Count) {}
  ATOM_PoolVector (size_type _Count, const T &_Val): basetype (_Count, _Val) {}
  ATOM_PoolVector (size_type _Count, const T &_Val, const allocator_type &_Al): basetype(_Count, _Val, _Al) {}
  ATOM_PoolVector (const ATOM_PoolVector &rhs): basetype (rhs) {}
  ATOM_PoolVector (const basetype &rhs): basetype (rhs.begin(), rhs.end()) {}
  template <class _Iter> ATOM_PoolVector (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_PoolVector (_Iter _First, _Iter _Last, const allocator_type &_Al): basetype (_First, _Last, _Al) {}
};

template <class T>
class ATOM_PoolDeque: public std::deque<T, ATOM_pool_allocator<T> >
{
public:
  typedef std::deque<T, ATOM_pool_allocator<T> > basetype;
  typedef typename basetype::allocator_type allocator_type;
  typedef typename basetype::size_type size_type;
  typedef typename basetype::difference_type difference_type;
  typedef typename basetype::pointer pointer;
  typedef typename basetype::const_pointer const_pointer;
  typedef typename basetype::reference reference;
  typedef typename basetype::const_reference const_reference;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::iterator iterator;
  typedef typename basetype::const_iterator const_iterator;

public:
  ATOM_PoolDeque (): basetype () {}
  explicit ATOM_PoolDeque (const allocator_type &_Al): basetype (_Al) {}
  explicit ATOM_PoolDeque (size_type _Count): basetype (_Count) {}
  ATOM_PoolDeque (size_type _Count, const T &_Val): basetype (_Count, _Val) {}
  ATOM_PoolDeque (size_type _Count, const T &_Val, const allocator_type &_Al): basetype (_Count, _Val, _Al) {}
  ATOM_PoolDeque (const ATOM_PoolDeque &rhs): basetype (rhs) {}
  template <class _Iter> ATOM_PoolDeque (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_PoolDeque (_Iter _First, _Iter _Last, const allocator_type &_Al): basetype (_First, _Last, _Al) {}
};

template <class T, class C = ATOM_PoolDeque<T> >
class ATOM_PoolQueue: public std::queue<T, C>
{
  typedef std::queue<T, C> basetype;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::size_type size_type;

public:
  ATOM_PoolQueue (): basetype () {}
  ATOM_PoolQueue (const C &c): basetype (c) {}
};

template <class T, class C = ATOM_PoolDeque<T> >
class ATOM_PoolStack: public std::stack<T, C>
{
public:
  typedef std::stack<T, C> basetype;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::size_type size_type;

public:
  ATOM_PoolStack (): basetype () {}
  ATOM_PoolStack (const C &c): basetype (c) {}
};

template <class T>
class ATOM_PoolList: public std::list<T, ATOM_pool_allocator<T> >
{
public:
  typedef std::list<T, ATOM_pool_allocator<T> > basetype;
  typedef typename basetype::allocator_type allocator_type;
  typedef typename basetype::size_type size_type;
  typedef typename basetype::difference_type difference_type;
  typedef typename basetype::pointer pointer;
  typedef typename basetype::const_pointer const_pointer;
  typedef typename basetype::reference reference;
  typedef typename basetype::const_reference const_reference;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::iterator iterator;
  typedef typename basetype::const_iterator const_iterator;
  typedef typename basetype::reverse_iterator reverse_iterator;
  typedef typename basetype::const_reverse_iterator const_reverse_iterator;

public:
  ATOM_PoolList (): basetype () {}
  explicit ATOM_PoolList (const allocator_type &_Al): basetype (_Al) {}
  explicit ATOM_PoolList (size_type _Count): basetype (_Count) {}
  ATOM_PoolList (size_type _Count, const T &_Val): basetype (_Count, _Val) {}
  ATOM_PoolList (size_type _Count, const T &_Val, const allocator_type &_Al): basetype(_Count, _Val, _Al) {}
  ATOM_PoolList (const ATOM_PoolList &rhs): basetype (rhs) {}
  ATOM_PoolList (const basetype &rhs): basetype (rhs.begin(), rhs.end()) {}
  template <class _Iter> ATOM_PoolList (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_PoolList (_Iter _First, _Iter _Last, const allocator_type &_Al): basetype (_First, _Last, _Al) {}
};

template <class Key, class Comp = std::less<Key> >
class ATOM_PoolSet: public std::set<Key, Comp, ATOM_pool_allocator<Key> >
{
public:
  typedef std::set<Key, Comp, ATOM_pool_allocator<Key> > basetype;
  typedef typename basetype::allocator_type allocator_type;
  typedef typename basetype::size_type size_type;
  typedef typename basetype::difference_type difference_type;
  typedef typename basetype::pointer pointer;
  typedef typename basetype::const_pointer const_pointer;
  typedef typename basetype::reference reference;
  typedef typename basetype::const_reference const_reference;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::iterator iterator;
  typedef typename basetype::const_iterator const_iterator;
  typedef typename basetype::reverse_iterator reverse_iterator;
  typedef typename basetype::const_reverse_iterator const_reverse_iterator;
  typedef typename basetype::key_type key_type;
  typedef typename basetype::key_compare key_compare;

public:
  ATOM_PoolSet (): basetype () {}
  explicit ATOM_PoolSet (const key_compare &_Pred): basetype (_Pred) {}
  ATOM_PoolSet (const key_compare &_Pred, const allocator_type &_Al): basetype (_Pred, _Al) {}
  template <class _Iter> ATOM_PoolSet (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_PoolSet (_Iter _First, _Iter _Last, const key_compare &_Pred): basetype (_First, _Last, _Pred) {}
  template <class _Iter> ATOM_PoolSet (_Iter _First, _Iter _Last, const key_compare &_Pred, const allocator_type &_Al): basetype (_First, _Last, _Pred, _Al) {}
};

template <class Key, class Comp = std::less<Key> >
class ATOM_PoolMultiSet: public std::multiset<Key, Comp, ATOM_pool_allocator<Key> >
{
public:
  typedef std::multiset<Key, Comp, ATOM_pool_allocator<Key> > basetype;
  typedef typename basetype::allocator_type allocator_type;
  typedef typename basetype::size_type size_type;
  typedef typename basetype::difference_type difference_type;
  typedef typename basetype::pointer pointer;
  typedef typename basetype::const_pointer const_pointer;
  typedef typename basetype::reference reference;
  typedef typename basetype::const_reference const_reference;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::iterator iterator;
  typedef typename basetype::const_iterator const_iterator;
  typedef typename basetype::reverse_iterator reverse_iterator;
  typedef typename basetype::const_reverse_iterator const_reverse_iterator;
  typedef typename basetype::key_type key_type;
  typedef typename basetype::key_compare key_compare;

public:
  ATOM_PoolMultiSet (): basetype () {}
  explicit ATOM_PoolMultiSet (const key_compare &_Pred): basetype (_Pred) {}
  ATOM_PoolMultiSet (const key_compare &_Pred, const allocator_type &_Al): basetype (_Pred, _Al) {}
  template <class _Iter> ATOM_PoolMultiSet (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_PoolMultiSet (_Iter _First, _Iter _Last, const key_compare &_Pred): basetype (_First, _Last, _Pred) {}
  template <class _Iter> ATOM_PoolMultiSet (_Iter _First, _Iter _Last, const key_compare &_Pred, const allocator_type &_Al): basetype (_First, _Last, _Pred, _Al) {}
};

template <class Key, class Val, class Comp = std::less<Key> >
class ATOM_PoolMap: public std::map<Key, Val, Comp, ATOM_pool_allocator<std::pair<const Key, Val> > >
{
public:
  typedef std::map<Key, Val, Comp, ATOM_pool_allocator<std::pair<const Key, Val> > > basetype;
  typedef typename basetype::allocator_type allocator_type;
  typedef typename basetype::size_type size_type;
  typedef typename basetype::difference_type difference_type;
  typedef typename basetype::pointer pointer;
  typedef typename basetype::const_pointer const_pointer;
  typedef typename basetype::reference reference;
  typedef typename basetype::const_reference const_reference;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::iterator iterator;
  typedef typename basetype::const_iterator const_iterator;
  typedef typename basetype::reverse_iterator reverse_iterator;
  typedef typename basetype::const_reverse_iterator const_reverse_iterator;
  typedef typename basetype::key_type key_type;
  typedef typename basetype::mapped_type mapped_type;
  typedef typename basetype::key_compare key_compare;

public:
  ATOM_PoolMap (): basetype () {}
  explicit ATOM_PoolMap (const key_compare &_Pred): basetype (_Pred) {}
  ATOM_PoolMap (const key_compare &_Pred, const allocator_type &_Al): basetype (_Pred, _Al) {}
  template <class _Iter> ATOM_PoolMap (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_PoolMap (_Iter _First, _Iter _Last, const key_compare &_Pred): basetype (_First, _Last, _Pred) {}
  template <class _Iter> ATOM_PoolMap (_Iter _First, _Iter _Last, const key_compare &_Pred, const allocator_type &_Al): basetype (_First, _Last, _Pred, _Al) {}
};

template <class Key, class Val, class Comp = std::less<Key> >
class ATOM_PoolMultiMap: public std::multimap<Key, Val, Comp, ATOM_pool_allocator<std::pair<const Key, Val> > >
{
public:
  typedef std::multimap<Key, Val, Comp, ATOM_pool_allocator<std::pair<const Key, Val> > > basetype;
  typedef typename basetype::allocator_type allocator_type;
  typedef typename basetype::size_type size_type;
  typedef typename basetype::difference_type difference_type;
  typedef typename basetype::pointer pointer;
  typedef typename basetype::const_pointer const_pointer;
  typedef typename basetype::reference reference;
  typedef typename basetype::const_reference const_reference;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::iterator iterator;
  typedef typename basetype::const_iterator const_iterator;
  typedef typename basetype::reverse_iterator reverse_iterator;
  typedef typename basetype::const_reverse_iterator const_reverse_iterator;
  typedef typename basetype::key_type key_type;
  typedef typename basetype::mapped_type mapped_type;
  typedef typename basetype::key_compare key_compare;

public:
  ATOM_PoolMultiMap (): basetype () {}
  explicit ATOM_PoolMultiMap (const key_compare &_Pred): basetype (_Pred) {}
  ATOM_PoolMultiMap (const key_compare &_Pred, const allocator_type &_Al): basetype (_Pred, _Al) {}
  template <class _Iter> ATOM_PoolMultiMap (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_PoolMultiMap (_Iter _First, _Iter _Last, const key_compare &_Pred): basetype (_First, _Last, _Pred) {}
  template <class _Iter> ATOM_PoolMultiMap (_Iter _First, _Iter _Last, const key_compare &_Pred, const allocator_type &_Al): basetype (_First, _Last, _Pred, _Al) {}
};

template <class Key>
class ATOM_PoolHashSet: public ATOM_HashSet<Key, ATOM_pool_allocator<Key> >
{
public:
  typedef ATOM_HashSet<Key, ATOM_pool_allocator<Key> > basetype;
  typedef typename basetype::allocator_type allocator_type;
  typedef typename basetype::size_type size_type;
  typedef typename basetype::difference_type difference_type;
  typedef typename basetype::pointer pointer;
  typedef typename basetype::const_pointer const_pointer;
  typedef typename basetype::reference reference;
  typedef typename basetype::const_reference const_reference;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::iterator iterator;
  typedef typename basetype::const_iterator const_iterator;
  typedef typename basetype::key_type key_type;

public:
  ATOM_PoolHashSet (): basetype () {}
  template <class _Iter> ATOM_PoolHashSet (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
};

template <class Key>
class ATOM_PoolHashMultiSet: public ATOM_HashMultiSet<Key, ATOM_pool_allocator<Key> >
{
public:
  typedef ATOM_HashMultiSet<Key, ATOM_pool_allocator<Key> > basetype;
  typedef typename basetype::allocator_type allocator_type;
  typedef typename basetype::size_type size_type;
  typedef typename basetype::difference_type difference_type;
  typedef typename basetype::pointer pointer;
  typedef typename basetype::const_pointer const_pointer;
  typedef typename basetype::reference reference;
  typedef typename basetype::const_reference const_reference;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::iterator iterator;
  typedef typename basetype::const_iterator const_iterator;
  typedef typename basetype::key_type key_type;

public:
  ATOM_PoolHashMultiSet (): basetype () {}
  template <class _Iter> ATOM_PoolHashMultiSet (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
};

template <class Key, class Val>
class ATOM_PoolHashMap: public ATOM_HashMap<Key, Val, ATOM_pool_allocator<std::pair<const Key, Val> > >
{
public:
  typedef ATOM_HashMap<Key, Val, ATOM_pool_allocator<std::pair<const Key, Val> > > basetype;
  typedef typename basetype::allocator_type allocator_type;
  typedef typename basetype::size_type size_type;
  typedef typename basetype::difference_type difference_type;
  typedef typename basetype::pointer pointer;
  typedef typename basetype::const_pointer const_pointer;
  typedef typename basetype::reference reference;
  typedef typename basetype::const_reference const_reference;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::iterator iterator;
  typedef typename basetype::const_iterator const_iterator;
  typedef typename basetype::key_type key_type;
  typedef typename basetype::mapped_type mapped_type;

public:
  ATOM_PoolHashMap (): basetype () {}
  template <class _Iter> ATOM_PoolHashMap (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
};

template <class Key, class Val>
class ATOM_PoolHashMultiMap: public ATOM_HashMultiMap<Key, Val, ATOM_pool_allocator<std::pair<const Key, Val> > >
{
public:
  typedef ATOM_HashMultiMap<Key, Val, ATOM_pool_allocator<std::pair<const Key, Val> > > basetype;
  typedef typename basetype::allocator_type allocator_type;
  typedef typename basetype::size_type size_type;
  typedef typename basetype::difference_type difference_type;
  typedef typename basetype::pointer pointer;
  typedef typename basetype::const_pointer const_pointer;
  typedef typename basetype::reference reference;
  typedef typename basetype::const_reference const_reference;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::iterator iterator;
  typedef typename basetype::const_iterator const_iterator;
  typedef typename basetype::key_type key_type;
  typedef typename basetype::mapped_type mapped_type;

public:
  ATOM_PoolHashMultiMap (): basetype () {}
  template <class _Iter> ATOM_PoolHashMultiMap (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
};

#if defined(ATOM_NO_POOL)

# define ATOM_POOL_STLC1 ATOM_STLC1
# define ATOM_POOL_STLC2 ATOM_STLC2
# define ATOM_POOL_STLC3 ATOM_STLC3
# define ATOM_POOL_STLC1_T ATOM_STLC1_T
# define ATOM_POOL_STLC2_T ATOM_STLC2_T
# define ATOM_POOL_STLC3_T ATOM_STLC3_T

# define ATOM_POOL_VECTOR ATOM_VECTOR
# define ATOM_POOL_LIST ATOM_LIST
# define ATOM_POOL_STACK ATOM_STACK
# define ATOM_POOL_QUEUE ATOM_QUEUE
# define ATOM_POOL_DEQUE ATOM_DEQUE
# define ATOM_POOL_SET ATOM_SET
# define ATOM_POOL_MULTISET ATOM_MULTISET
# define ATOM_POOL_MAP ATOM_MAP
# define ATOM_POOL_MULTIMAP ATOM_MULTIMAP
# define ATOM_POOL_HASHSET ATOM_HASHSET
# define ATOM_POOL_HASHMULTISET ATOM_HASHMULTISET
# define ATOM_POOL_HASHMAP ATOM_HASHMAP
# define ATOM_POOL_HASHMULTIMAP ATOM_HASHMULTIMAP

#else

# define ATOM_POOL_STLC1(container, type) container<type, ATOM_pool_allocator< type >::rebind<container< type >::allocator_type::value_type>::other>
# define ATOM_POOL_STLC2(container, type1, type2) container<type1, type2, ATOM_pool_allocator< type1 >::rebind<container< type1, type2 >::allocator_type::value_type>::other>
# define ATOM_POOL_STLC3(container, type1, type2, type3) container<type1, type2, type3, ATOM_pool_allocator< type1 >::rebind<container< type1, type2, type3 >::allocator_type::value_type>::other>
# define ATOM_POOL_STLC1_T(container, type) container<type, typename ATOM_pool_allocator< type >::template rebind<typename container< type >::allocator_type::value_type>::other>
# define ATOM_POOL_STLC2_T(container, type1, type2) container<type1, type2, typename ATOM_pool_allocator< type1 >::template rebind<typename container< type1, type2 >::allocator_type::value_type>::other>
# define ATOM_POOL_STLC3_T(container, type1, type2, type3) container<type1, type2, type3, typename ATOM_pool_allocator< type1 >::template rebind<typename container< type1, type2, type3 >::allocator_type::value_type>::other>

# define ATOM_POOL_VECTOR ATOM_PoolVector
# define ATOM_POOL_LIST ATOM_PoolList
# define ATOM_POOL_STACK ATOM_PoolStack
# define ATOM_POOL_QUEUE ATOM_PoolQueue
# define ATOM_POOL_DEQUE ATOM_PoolDeque
# define ATOM_POOL_SET ATOM_PoolSet
# define ATOM_POOL_MULTISET ATOM_PoolMultiSet
# define ATOM_POOL_MAP ATOM_PoolMap
# define ATOM_POOL_MULTIMAP ATOM_PoolMultiMap
# define ATOM_POOL_HASHSET ATOM_PoolHashSet
# define ATOM_POOL_HASHMULTISET ATOM_PoolHashMultiSet
# define ATOM_POOL_HASHMAP ATOM_PoolHashMap
# define ATOM_POOL_HASHMULTIMAP ATOM_PoolHashMultiMap

#endif

#endif // __ATOM_DEBUGHELP_POOL_H__
