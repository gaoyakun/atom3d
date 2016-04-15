#ifndef __ATOM_DEBUGHELP_STL_H
#define __ATOM_DEBUGHELP_STL_H

#if _MSC_VER > 1000
# pragma once
#endif

#if defined(_MSC_VER)
# pragma warning(disable:4786)
#endif

#include <string>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <list>
#include <vector>

#include "basedefs.h"
#include "mem.h"

//#if defined(__GNUC__)||(_MSC_VER >= 1310)
# include "meta/typetraits.h"
# include "hashset.h"
# include "hashmap.h"
//#endif

#define ATOM_STL_ALLOCATOR

#if defined(ATOM_NO_DEBUGHELP_MEM_MNGR)||(ATOM3D_COMPILER_MSVC&&_MSC_VER < 1310)
# undef ATOM_STL_ALLOCATOR
#endif

#if defined ATOM_STL_ALLOCATOR

#if defined(_MSC_VER)
# pragma warning(push)
# pragma warning(disable:4503)
# pragma warning(disable:4100)
#endif

template<class T>
struct ATOM_allocator_base
{
	typedef T value_type;
};

template<class T>
struct ATOM_allocator_base<const T>
{
	typedef T value_type;
};

template <class T>
class ATOM_allocator: public ATOM_allocator_base<T>
{
public:
  typedef ATOM_allocator_base<T> basetype;
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
    typedef ATOM_allocator<U> other;
  };

  pointer address(reference val) const
  {
    return &val;
  }

  const_pointer address(const_reference val) const
  {
    return &val;
  }

  ATOM_allocator(void)
  {
  }

  ATOM_allocator(const ATOM_allocator<T> &)
  {
  }

  template <class U>
  ATOM_allocator(const ATOM_allocator<U>&)
  {
  }

  template <class U>
  ATOM_allocator<T> &operator=(const ATOM_allocator<U>&)
  {
    return *this;
  }

  void deallocate(pointer p, size_type)
  {
    ATOM_ALIGNED_FREE(p);
  }

  pointer allocate(size_type count)
  {
    return reinterpret_cast<pointer>(ATOM_ALIGNED_MALLOC(count * sizeof(T), 16));
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
bool operator==(const ATOM_allocator<T>&, const ATOM_allocator<U>&)
{
	return true;
}

template<class T, class U> 
inline
bool operator!=(const ATOM_allocator<T>&, const ATOM_allocator<U>&)
{
	return false;
}

template<> 
class ATOM_allocator<void>
{
public:
	typedef void T;
	typedef T *pointer;
	typedef const T *const_pointer;
	typedef T value_type;

	template<class U>
	struct rebind
	{
		typedef ATOM_allocator<U> other;
	};

  ATOM_allocator()
	{
	}

	ATOM_allocator(const ATOM_allocator<T>&)
	{
	}

	template<class U>
	ATOM_allocator(const ATOM_allocator<U>&)
	{
	}

	template<class U>
	ATOM_allocator<T>& operator=(const ATOM_allocator<U>&)
	{
		return (*this);
	}
};

template <class T>
class ATOM_Vector: public std::vector<T, ATOM_allocator<T> >
{
public:
  typedef std::vector<T, ATOM_allocator<T> > basetype;
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
  ATOM_Vector (): basetype () {}
  explicit ATOM_Vector (const allocator_type &_Al): basetype (_Al) {}
  explicit ATOM_Vector (size_type _Count): basetype (_Count) {}
  ATOM_Vector (size_type _Count, const T &_Val): basetype (_Count, _Val) {}
  ATOM_Vector (size_type _Count, const T &_Val, const allocator_type &_Al): basetype(_Count, _Val, _Al) {}
  ATOM_Vector (const ATOM_Vector &rhs): basetype (rhs) {}
  ATOM_Vector (const basetype &rhs): basetype (rhs.begin(), rhs.end()) {}
  template <class _Iter> ATOM_Vector (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_Vector (_Iter _First, _Iter _Last, const allocator_type &_Al): basetype (_First, _Last, _Al) {}
};

template <class T>
class ATOM_Deque: public std::deque<T, ATOM_allocator<T> >
{
public:
	typedef std::deque<T, ATOM_allocator<T> > basetype;
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
	ATOM_Deque (): basetype () {}
	explicit ATOM_Deque (const allocator_type &_Al): basetype (_Al) {}
	explicit ATOM_Deque (size_type _Count): basetype (_Count) {}
	ATOM_Deque (size_type _Count, const T &_Val): basetype (_Count, _Val) {}
	ATOM_Deque (size_type _Count, const T &_Val, const allocator_type &_Al): basetype (_Count, _Val, _Al) {}
	ATOM_Deque (const ATOM_Deque &rhs): basetype (rhs) {}
	template <class _Iter> ATOM_Deque (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
	template <class _Iter> ATOM_Deque (_Iter _First, _Iter _Last, const allocator_type &_Al): basetype (_First, _Last, _Al) {}
};

template <class T, class C = ATOM_Deque<T> >
class ATOM_Queue: public std::queue<T, C>
{
  typedef std::queue<T, C> basetype;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::size_type size_type;

public:
  ATOM_Queue (): basetype () {}
  ATOM_Queue (const C &c): basetype (c) {}
};

template <class T, class C = ATOM_Deque<T> >
class ATOM_Stack: public std::stack<T, C>
{
public:
  typedef std::stack<T, C> basetype;
  typedef typename basetype::value_type value_type;
  typedef typename basetype::size_type size_type;

public:
  ATOM_Stack (): basetype () {}
  ATOM_Stack (const C &c): basetype (c) {}
};

template <class T>
class ATOM_List: public std::list<T, ATOM_allocator<T> >
{
public:
  typedef std::list<T, ATOM_allocator<T> > basetype;
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
  ATOM_List (): basetype () {}
  explicit ATOM_List (const allocator_type &_Al): basetype (_Al) {}
  explicit ATOM_List (size_type _Count): basetype (_Count) {}
  ATOM_List (size_type _Count, const T &_Val): basetype (_Count, _Val) {}
  ATOM_List (size_type _Count, const T &_Val, const allocator_type &_Al): basetype(_Count, _Val, _Al) {}
  ATOM_List (const ATOM_List &rhs): basetype (rhs) {}
  ATOM_List (const basetype &rhs): basetype (rhs.begin(), rhs.end()) {}
  template <class _Iter> ATOM_List (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_List (_Iter _First, _Iter _Last, const allocator_type &_Al): basetype (_First, _Last, _Al) {}
};

template <class Key, class Comp = std::less<Key> >
class ATOM_Set: public std::set<Key, Comp, ATOM_allocator<Key> >
{
public:
  typedef std::set<Key, Comp, ATOM_allocator<Key> > basetype;
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
  ATOM_Set (): basetype () {}
  explicit ATOM_Set (const key_compare &_Pred): basetype (_Pred) {}
  ATOM_Set (const key_compare &_Pred, const allocator_type &_Al): basetype (_Pred, _Al) {}
  template <class _Iter> ATOM_Set (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_Set (_Iter _First, _Iter _Last, const key_compare &_Pred): basetype (_First, _Last, _Pred) {}
  template <class _Iter> ATOM_Set (_Iter _First, _Iter _Last, const key_compare &_Pred, const allocator_type &_Al): basetype (_First, _Last, _Pred, _Al) {}
};

template <class Key, class Comp = std::less<Key> >
class ATOM_MultiSet: public std::multiset<Key, Comp, ATOM_allocator<Key> >
{
public:
  typedef std::multiset<Key, Comp, ATOM_allocator<Key> > basetype;
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
  ATOM_MultiSet (): basetype () {}
  explicit ATOM_MultiSet (const key_compare &_Pred): basetype (_Pred) {}
  ATOM_MultiSet (const key_compare &_Pred, const allocator_type &_Al): basetype (_Pred, _Al) {}
  template <class _Iter> ATOM_MultiSet (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_MultiSet (_Iter _First, _Iter _Last, const key_compare &_Pred): basetype (_First, _Last, _Pred) {}
  template <class _Iter> ATOM_MultiSet (_Iter _First, _Iter _Last, const key_compare &_Pred, const allocator_type &_Al): basetype (_First, _Last, _Pred, _Al) {}
};

template <class Key, class Val, class Comp = std::less<Key> >
class ATOM_Map: public std::map<Key, Val, Comp, ATOM_allocator<std::pair<const Key, Val> > >
{
public:
  typedef std::map<Key, Val, Comp, ATOM_allocator<std::pair<const Key, Val> > > basetype;
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
  ATOM_Map (): basetype () {}
  explicit ATOM_Map (const key_compare &_Pred): basetype (_Pred) {}
  ATOM_Map (const key_compare &_Pred, const allocator_type &_Al): basetype (_Pred, _Al) {}
  template <class _Iter> ATOM_Map (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_Map (_Iter _First, _Iter _Last, const key_compare &_Pred): basetype (_First, _Last, _Pred) {}
  template <class _Iter> ATOM_Map (_Iter _First, _Iter _Last, const key_compare &_Pred, const allocator_type &_Al): basetype (_First, _Last, _Pred, _Al) {}
};

template <class Key, class Val, class Comp = std::less<Key> >
class ATOM_MultiMap: public std::multimap<Key, Val, Comp, ATOM_allocator<std::pair<const Key, Val> > >
{
public:
  typedef std::multimap<Key, Val, Comp, ATOM_allocator<std::pair<const Key, Val> > > basetype;
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
  ATOM_MultiMap (): basetype () {}
  explicit ATOM_MultiMap (const key_compare &_Pred): basetype (_Pred) {}
  ATOM_MultiMap (const key_compare &_Pred, const allocator_type &_Al): basetype (_Pred, _Al) {}
  template <class _Iter> ATOM_MultiMap (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
  template <class _Iter> ATOM_MultiMap (_Iter _First, _Iter _Last, const key_compare &_Pred): basetype (_First, _Last, _Pred) {}
  template <class _Iter> ATOM_MultiMap (_Iter _First, _Iter _Last, const key_compare &_Pred, const allocator_type &_Al): basetype (_First, _Last, _Pred, _Al) {}
};

template <class Key>
class ATOM_HashSetEx: public ATOM_HashSet<Key, ATOM_allocator<Key> >
{
public:
  typedef ATOM_HashSet<Key, ATOM_allocator<Key> > basetype;
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
  ATOM_HashSetEx (): basetype () {}
  template <class _Iter> ATOM_HashSetEx (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
};

template <class Key>
class ATOM_HashMultiSetEx: public ATOM_HashMultiSet<Key, ATOM_allocator<Key> >
{
public:
  typedef ATOM_HashMultiSet<Key, ATOM_allocator<Key> > basetype;
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
  ATOM_HashMultiSetEx (): basetype () {}
  template <class _Iter> ATOM_HashMultiSetEx (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
};

template <class Key, class Val>
class ATOM_HashMapEx: public ATOM_HashMap<Key, Val, ATOM_allocator<std::pair<const Key, Val> > >
{
public:
  typedef ATOM_HashMap<Key, Val, ATOM_allocator<std::pair<const Key, Val> > > basetype;
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
  ATOM_HashMapEx (): basetype () {}
  template <class _Iter> ATOM_HashMapEx (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
};

template <class Key, class Val>
class ATOM_HashMultiMapEx: public ATOM_HashMultiMap<Key, Val, ATOM_allocator<std::pair<const Key, Val> > >
{
public:
  typedef ATOM_HashMultiMap<Key, Val, ATOM_allocator<std::pair<const Key, Val> > > basetype;
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
  ATOM_HashMultiMapEx (): basetype () {}
  template <class _Iter> ATOM_HashMultiMapEx (_Iter _First, _Iter _Last): basetype (_First, _Last) {}
};

# define ATOM_STLC1(container, type) container<type, ATOM_allocator< type >::rebind<container< type >::allocator_type::value_type>::other>
# define ATOM_STLC2(container, type1, type2) container<type1, type2, ATOM_allocator< type1 >::rebind<container< type1, type2 >::allocator_type::value_type>::other>
# define ATOM_STLC3(container, type1, type2, type3) container<type1, type2, type3, ATOM_allocator< type1 >::rebind<container< type1, type2, type3 >::allocator_type::value_type>::other>
# define ATOM_STLC1_T(container, type) container<type, typename ATOM_allocator< type >::template rebind<typename container< type >::allocator_type::value_type>::other>
# define ATOM_STLC2_T(container, type1, type2) container<type1, type2, typename ATOM_allocator< type1 >::template rebind<typename container< type1, type2 >::allocator_type::value_type>::other>
# define ATOM_STLC3_T(container, type1, type2, type3) container<type1, type2, type3, typename ATOM_allocator< type1 >::template rebind<typename container< type1, type2, type3 >::allocator_type::value_type>::other>

typedef std::basic_string<char, std::char_traits<char>, ATOM_allocator<char> > ATOM_string;
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, ATOM_allocator<wchar_t> > ATOM_wstring;

# define ATOM_VECTOR ATOM_Vector
# define ATOM_LIST ATOM_List
# define ATOM_STACK ATOM_Stack
# define ATOM_QUEUE ATOM_Queue
# define ATOM_DEQUE ATOM_Deque
# define ATOM_SET ATOM_Set
# define ATOM_MULTISET ATOM_MultiSet
# define ATOM_MAP ATOM_Map
# define ATOM_MULTIMAP ATOM_MultiMap
# define ATOM_HASHSET ATOM_HashSetEx
# define ATOM_HASHMULTISET ATOM_HashMultiSetEx
# define ATOM_HASHMAP ATOM_HashMapEx
# define ATOM_HASHMULTIMAP ATOM_HashMultiMapEx
# define ATOM_STRING ATOM_string
# define ATOM_WSTRING ATOM_wstring

#if defined(_MSC_VER)
# pragma warning(pop)
#endif

#else

# define ATOM_STLC1(container, type) container<type>
# define ATOM_STLC2(container, type1, type2) container<type1, type2>
# define ATOM_STLC3(container, type1, type2, type3) container<type1, type2, type3>
# define ATOM_STLC1_T(container, type) container<type>
# define ATOM_STLC2_T(container, type1, type2) container<type1, type2>
# define ATOM_STLC3_T(container, type1, type2, type3) container<type1, type2, type3>

# define ATOM_VECTOR std::vector
# define ATOM_LIST std::list
# define ATOM_STACK std::stack
# define ATOM_QUEUE std::queue
# define ATOM_DEQUE std::deque
# define ATOM_SET std::set
# define ATOM_MULTISET std::multiset
# define ATOM_MAP std::map
# define ATOM_MULTIMAP std::multimap

#if _MSC_VER <= 1200
# define ATOM_HASHSET ATOM_SET
# define ATOM_HASHMULTISET ATOM_MULTISET
# define ATOM_HASHMAP ATOM_MAP
# define ATOM_HASHMULTIMAP ATOM_MULTIMAP
#else
# define ATOM_HASHSET ATOM_HashSet
# define ATOM_HASHMULTISET ATOM_HashMultiSet
# define ATOM_HASHMAP ATOM_HashMap
# define ATOM_HASHMULTIMAP ATOM_HashMultiMap
#endif

# define ATOM_STRING std::string
# define ATOM_WSTRING std::wstring

#endif

#endif // __ATOM_DEBUGHELP_STL_H
