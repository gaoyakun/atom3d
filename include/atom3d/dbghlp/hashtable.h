#ifndef __DEBUGHELP_HASHTABLE_H
#define __DEBUGHELP_HASHTABLE_H

#include "hasher.h"
#include <vector>
#include <algorithm>
#include <functional>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4267)
# pragma warning(disable:4512)

// these predicts are missed in msvc
namespace atom_internal
{
  template <class _Tp>
  struct identity : public std::unary_function<_Tp,_Tp>
  {
    _Tp& operator ()(_Tp& x) const {
      return x;
    }
    const _Tp& operator ()(const _Tp& x) const {
      return x;
    }
  };

  template <class _Pair>
  struct select1st : public std::unary_function<_Pair,typename _Pair::first_type>
  {
    typename _Pair::first_type& operator ()(_Pair& x) const {
      return x.first;
    }
    const typename _Pair::first_type& operator ()(const _Pair& x) const {
      return x.first;
    }
  };

  template <class _Pair>
  struct select2nd : public std::unary_function<_Pair,typename _Pair::second_type>
  {
    typename _Pair::second_type& operator()(_Pair& x) const {
      return x.second;
    }
    const typename _Pair::second_type& operator()(const _Pair& x) const {
      return x.second;
    }
  };
} // namespace atom_internal
#endif // _MSC_VER

namespace atom_internal
{
  template <class T>
  struct equal_to
  {
    template <class U>
    bool operator () (const T &left, const U &right) const
    {
      return (left == right);
    }

    bool operator () (const char *left, const char *right) const
    {
      return (left==right) || (left && right && !strcmp(left, right));
    }

    bool operator () (const wchar_t *left, const wchar_t *right) const
    {
      return (left==right) || (left && right && !wcscmp(left, right));
    }
  };
}

template <class _Val>
struct _ATOM_HashTable_node
{
  _ATOM_HashTable_node* _M_next;
  _Val _M_val;
};  

template <class _Val,class _Key,class _Alloc,class _HashFcn,class _ExtractKey,class _EqualKey>
class ATOM_HashTable;

template <class _Val,class _Key,class _Alloc,class _HashFcn,class _ExtractKey,class _EqualKey>
struct _ATOM_HashTable_iterator;

template <class _Val,class _Key,class _Alloc,class _HashFcn,class _ExtractKey,class _EqualKey>
struct _ATOM_HashTable_const_iterator;

template <class _Val,class _Key,class _Alloc,class _HashFcn,class _ExtractKey,class _EqualKey>
struct _ATOM_HashTable_iterator
{
  typedef ATOM_HashTable<_Val,_Key,_Alloc,_HashFcn,_ExtractKey,_EqualKey> _ATOM_HashTable;
  typedef _ATOM_HashTable_iterator<_Val,_Key,_Alloc,_HashFcn,_ExtractKey,_EqualKey> iterator;
  typedef _ATOM_HashTable_const_iterator<_Val,_Key,_Alloc,_HashFcn,_ExtractKey,_EqualKey> const_iterator;
  typedef _ATOM_HashTable_node<_Val> _Node;

  typedef std::forward_iterator_tag iterator_category;
#if defined(__GNUC__)||(_MSC_VER >= 1310)
  typedef std::ptrdiff_t difference_type;
#else
  typedef ptrdiff_t difference_type;
#endif
  typedef unsigned size_type;
  typedef _Val value_type;
  typedef _Val& reference;
  typedef _Val* pointer;

  _Node* _M_cur;
  _ATOM_HashTable* _M_ht;

  _ATOM_HashTable_iterator(_Node* __n, _ATOM_HashTable* __tab)
    : _M_cur(__n), _M_ht(__tab) {
  }
  _ATOM_HashTable_iterator() {
  }
  reference operator*() const {
    return _M_cur->_M_val;
  }
  pointer operator->() const {
    return &(operator * ());
  }
  iterator& operator++();
  iterator operator++(int);
  bool operator==(const iterator& __it) const {
    return _M_cur == __it._M_cur;
  }
  bool operator!=(const iterator& __it) const {
    return _M_cur != __it._M_cur;
  }
};


template <class _Val,class _Key,class _Alloc,class _HashFcn,class _ExtractKey,class _EqualKey>
struct _ATOM_HashTable_const_iterator
  {
    typedef ATOM_HashTable<_Val,_Key,_Alloc,_HashFcn,_ExtractKey,_EqualKey> _ATOM_HashTable;
    typedef _ATOM_HashTable_iterator<_Val,_Key,_Alloc,_HashFcn,_ExtractKey,_EqualKey> iterator;
    typedef _ATOM_HashTable_const_iterator<_Val,_Key,_Alloc,_HashFcn,_ExtractKey,_EqualKey> const_iterator;
    typedef _ATOM_HashTable_node<_Val> _Node;

    typedef std::forward_iterator_tag iterator_category;
#if defined(__GNUC__)||(_MSC_VER >= 1310)
    typedef std::ptrdiff_t difference_type;
#else
    typedef ptrdiff_t difference_type;
#endif
    typedef unsigned size_type;
    typedef _Val value_type;
    typedef const _Val& reference;
    typedef const _Val* pointer;

    const _Node* _M_cur;
    const _ATOM_HashTable* _M_ht;

    _ATOM_HashTable_const_iterator(const _Node* __n, const _ATOM_HashTable* __tab)
      : _M_cur(__n), _M_ht(__tab) {
    }
    _ATOM_HashTable_const_iterator() {
    }
    _ATOM_HashTable_const_iterator(const iterator& __it)
      : _M_cur(__it._M_cur), _M_ht(__it._M_ht) {
    }
    reference operator*() const {
      return _M_cur->_M_val;
    }
    pointer operator->() const {
      return &(operator * ());
    }
    const_iterator& operator++();
    const_iterator operator++(int);
    bool operator==(const const_iterator& __it) const {
      return _M_cur == __it._M_cur;
    }
    bool operator!=(const const_iterator& __it) const {
      return _M_cur != __it._M_cur;
    }
  };

namespace
{
  // Note: assumes long is at least 32 bits.
  enum {
    __num_primes = 28
  };

  static const unsigned long __prime_list[__num_primes] = {
    53ul,
    97ul,
    193ul,
    389ul,
    769ul,
    1543ul,
    3079ul,
    6151ul,
    12289ul,
    24593ul,
    49157ul,
    98317ul,
    196613ul,
    393241ul,
    786433ul,
    1572869ul,
    3145739ul,
    6291469ul,
    12582917ul,
    25165843ul,
    50331653ul,
    100663319ul,
    201326611ul,
    402653189ul,
    805306457ul,
    1610612741ul,
    3221225473ul,
    4294967291ul
  };

  inline unsigned long __next_prime(unsigned long __n) {
    const unsigned long* __first = __prime_list;
    const unsigned long* __last = __prime_list + (int) __num_primes;
    const unsigned long* pos = std::lower_bound(__first, __last, __n);
    return (pos == __last) ? *(__last - 1) : *pos;
  }
}

// Forward declaration of operator==.

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
class ATOM_HashTable;

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
bool operator==(const ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>& __ht1, const ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>& __ht2);

template <class _Val,class _Key,class _Alloc,class _HashFcn,class _ExtractKey,class _EqualKey>
class ATOM_HashTable
  {
  public:
    typedef _Key key_type;
    typedef _Val value_type;
    typedef _HashFcn hasher;
    typedef _EqualKey key_equal;

    typedef unsigned size_type;
#if defined(__GNUC__)||(_MSC_VER >= 1310)
    typedef std::ptrdiff_t difference_type;
#else
    typedef ptrdiff_t difference_type;
#endif
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    hasher hash_funct() const {
      return _M_hash;
    }
    key_equal key_eq() const {
      return _M_equals;
    }

  private:
    typedef _ATOM_HashTable_node<_Val> _Node;

  public:
    typedef _Alloc allocator_type;

  private:

#if defined(__GNUC__)||(_MSC_VER >= 1310)
    typedef typename allocator_type::template rebind<_Node>::other node_allocator_type;
    typedef typename allocator_type::template rebind<_Node*>::other bucket_allocator_type;
#else
    typedef std::allocator<_Node> node_allocator_type;
    typedef std::allocator<_Node*> bucket_allocator_type;
#endif

    node_allocator_type _M_node_allocator;
    _Node* _M_get_node() {
      return _M_node_allocator.allocate(1);
    }
    void _M_put_node(_Node* __p) {
      _M_node_allocator.deallocate(__p, 1);
    }

  private:
    hasher _M_hash;
    key_equal _M_equals;
    _ExtractKey _M_get_key;
    std::vector<_Node*, bucket_allocator_type> _M_buckets;
    size_type _M_num_elements;

  public:
    typedef _ATOM_HashTable_iterator<_Val,_Key,_Alloc,_HashFcn,_ExtractKey,_EqualKey> iterator;
    typedef _ATOM_HashTable_const_iterator<_Val,_Key,_Alloc,_HashFcn,_ExtractKey,_EqualKey> const_iterator;

    friend struct _ATOM_HashTable_iterator<_Val,_Key,_Alloc,_HashFcn,_ExtractKey,_EqualKey>;
    friend struct _ATOM_HashTable_const_iterator<_Val,_Key,_Alloc,_HashFcn,_ExtractKey,_EqualKey>;

  public:
    ATOM_HashTable(size_type __n, const _HashFcn& __hf, const _EqualKey& __eql, const _ExtractKey& __ext)
      : _M_hash(__hf), _M_equals(__eql), _M_get_key(__ext), _M_num_elements(0) {
      _M_initialize_buckets(__n);
    }

    ATOM_HashTable(size_type __n, const _HashFcn& __hf, const _EqualKey& __eql)
      : _M_hash(__hf), _M_equals(__eql), _M_get_key(_ExtractKey()), _M_num_elements(0) {
      _M_initialize_buckets(__n);
    }

    ATOM_HashTable(const ATOM_HashTable& __ht)
      : _M_hash(__ht._M_hash), _M_equals(__ht._M_equals), _M_get_key(__ht._M_get_key), _M_num_elements(0) {
      _M_copy_from(__ht);
    }

    ATOM_HashTable& operator=(const ATOM_HashTable& __ht) {
      if ( &__ht != this)
      {
        clear();
        _M_hash = __ht._M_hash;
        _M_equals = __ht._M_equals;
        _M_get_key = __ht._M_get_key;
        _M_copy_from(__ht);
      }
      return *this;
    }

    ~ATOM_HashTable() {
      clear();
    }

    size_type size() const {
      return _M_num_elements;
    }
    size_type max_size() const {
      return size_type(-1);
    }
    bool empty() const {
      return size() == 0;
    }

    void swap(ATOM_HashTable& __ht) {
      std::swap(_M_hash, __ht._M_hash);
      std::swap(_M_equals, __ht._M_equals);
      std::swap(_M_get_key, __ht._M_get_key);
      _M_buckets.swap(__ht._M_buckets);
      std::swap(_M_num_elements, __ht._M_num_elements);
    }

    iterator begin() {
      for ( size_type __n = 0; __n < _M_buckets.size(); ++__n)
        if ( _M_buckets[__n])
        {
          return iterator(_M_buckets[__n], this);
        }
      return end();
    }

    iterator end() {
      return iterator(0, this);
    }

    const_iterator begin() const {
      for ( size_type __n = 0; __n < _M_buckets.size(); ++__n)
        if ( _M_buckets[__n])
        {
          return const_iterator(_M_buckets[__n], this);
        }
      return end();
    }

    const_iterator end() const {
      return const_iterator(0, this);
    }

    template <class _Vl,class _Ky,class _Al,class _HF,class _Ex,class _Eq>
    friend bool operator==(const ATOM_HashTable<_Vl,_Ky,_Al,_HF,_Ex,_Eq>&, const ATOM_HashTable<_Vl,_Ky,_Al,_HF,_Ex,_Eq>&);

  public:
    size_type bucket_count() const {
      return _M_buckets.size();
    }
    size_type max_bucket_count() const {
      return __prime_list[(int) __num_primes - 1];
    } 

    size_type elems_in_bucket(size_type __bucket) const {
      size_type __result = 0;
      for ( _Node*__cur = _M_buckets[__bucket]; __cur; __cur = __cur->_M_next)
        __result += 1;
      return __result;
    }

    std::pair<iterator, bool> insert_unique(const value_type& __obj) {
      resize(_M_num_elements + 1);
      return insert_unique_noresize(__obj);
    }

    iterator insert_equal(const value_type& __obj) {
      resize(_M_num_elements + 1);
      return insert_equal_noresize(__obj);
    }

    std::pair<iterator, bool> insert_unique_noresize(const value_type& __obj);
    iterator insert_equal_noresize(const value_type& __obj);

    template <class _InputIterator>
    void insert_unique(_InputIterator __f, _InputIterator __l) {
      insert_unique(__f, __l, __iterator_category(__f));
    }

    template <class _InputIterator>
    void insert_equal(_InputIterator __f, _InputIterator __l) {
      insert_equal(__f, __l, __iterator_category(__f));
    }

    template <class _InputIterator>
    void insert_unique(_InputIterator __f, _InputIterator __l, std::input_iterator_tag) {
      for ( ; __f != __l; ++__f)
        insert_unique(*__f);
    }

    template <class _InputIterator>
    void insert_equal(_InputIterator __f, _InputIterator __l, std::input_iterator_tag) {
      for ( ; __f != __l; ++__f)
        insert_equal(*__f);
    }

    template <class _ForwardIterator>
    void insert_unique(_ForwardIterator __f, _ForwardIterator __l, std::forward_iterator_tag) {
      size_type __n = std::distance(__f, __l);
      resize(_M_num_elements + __n);
      for ( ; __n > 0; --__n, ++__f)
        insert_unique_noresize(*__f);
    }

    template <class _ForwardIterator>
    void insert_equal(_ForwardIterator __f, _ForwardIterator __l, std::forward_iterator_tag) {
      size_type __n = std::distance(__f, __l);
      resize(_M_num_elements + __n);
      for ( ; __n > 0; --__n, ++__f)
        insert_equal_noresize(*__f);
    }

    reference find_or_insert(const value_type& __obj);

    template <class T>
    iterator find(const T& __key) {
      size_type __n = _M_bkt_num_key(__key);
      _Node* __first;
      for ( __first = _M_buckets[__n];
            __first && !_M_equals(_M_get_key(__first->_M_val), __key);
            __first = __first->_M_next)
      {
      }
      return iterator(__first, this);
    } 

    template <class T>
    const_iterator find(const T& __key) const {
      size_type __n = _M_bkt_num_key(__key);
      const _Node* __first;
      for ( __first = _M_buckets[__n];
            __first && !_M_equals(_M_get_key(__first->_M_val), __key);
            __first = __first->_M_next)
        ;

      return const_iterator(__first, this);
    } 

    template <class T>
    size_type count(const T& __key) const {
      const size_type __n = _M_bkt_num_key(__key);
      size_type __result = 0;

      for ( const _Node*__cur = _M_buckets[__n]; __cur; __cur = __cur->_M_next)
        if ( _M_equals(_M_get_key(__cur->_M_val), __key))
        {
          ++__result;
        }
      return __result;
    }

    template <class T>
    std::pair<iterator, iterator> equal_range(const T& __key) {
      typedef std::pair<iterator,iterator> _Pii;
      const size_type __n = _M_bkt_num_key(__key);

      for ( _Node*__first = _M_buckets[__n]; __first; __first = __first->_M_next)
        if ( _M_equals(_M_get_key(__first->_M_val), __key))
        {
          for ( _Node*__cur = __first->_M_next; __cur; __cur = __cur->_M_next)
            if ( !_M_equals(_M_get_key(__cur->_M_val), __key))
              return _Pii(iterator(__first, this), iterator(__cur, this));
          for ( size_type __m = __n + 1; __m < _M_buckets.size(); ++__m)
            if ( _M_buckets[__m])
              return _Pii(iterator(__first, this), iterator(_M_buckets[__m], this));
          return _Pii(iterator(__first, this), end());
        }
      return _Pii(end(), end());
    }

    template <class T>
    std::pair<const_iterator, const_iterator> equal_range(const T& __key) const {
      typedef std::pair<const_iterator,const_iterator> _Pii;
      const size_type __n = _M_bkt_num_key(__key);

      for ( const _Node*__first = _M_buckets[__n]; __first; __first = __first->_M_next)
        if ( _M_equals(_M_get_key(__first->_M_val), __key))
        {
          for ( const _Node*__cur = __first->_M_next; __cur; __cur = __cur->_M_next)
            if ( !_M_equals(_M_get_key(__cur->_M_val), __key))
              return _Pii(const_iterator(__first, this), const_iterator(__cur, this));

          for ( size_type __m = __n + 1; __m < _M_buckets.size(); ++__m)
            if ( _M_buckets[__m])
              return _Pii(const_iterator(__first, this), const_iterator(_M_buckets[__m], this));

          return _Pii(const_iterator(__first, this), end());
        }
      return _Pii(end(), end());
    }

    template <class T>
    size_type erase(const T& __key) {
      const size_type __n = _M_bkt_num_key(__key);
      _Node* __first = _M_buckets[__n];
      size_type __erased = 0;

      if ( __first)
      {
        _Node* __cur = __first;
        _Node* __next = __cur->_M_next;

        while ( __next)
        {
          if ( _M_equals(_M_get_key(__next->_M_val), __key))
          {
            __cur->_M_next = __next->_M_next;
            _M_delete_node(__next);
            __next = __cur->_M_next;
            ++__erased;
            --_M_num_elements;
          }
          else
          {
            __cur = __next;
            __next = __cur->_M_next;
          }
        }

        if ( _M_equals(_M_get_key(__first->_M_val), __key))
        {
          _M_buckets[__n] = __first->_M_next;
          _M_delete_node(__first);
          ++__erased;
          --_M_num_elements;
        }
      }
      return __erased;
    }

    void erase(const iterator& __it);
    void erase(iterator __first, iterator __last);
    void erase(const const_iterator& __it);
    void erase(const_iterator __first, const_iterator __last);
    void resize(size_type __num_elements_hint);
    void clear();

  private:
    size_type _M_next_size(size_type __n) const {
      return __next_prime(__n);
    }

    void _M_initialize_buckets(size_type __n) {
      const size_type __n_buckets = _M_next_size(__n);
      _M_buckets.reserve(__n_buckets);
      _M_buckets.insert(_M_buckets.end(), __n_buckets, (_Node *) 0);
      _M_num_elements = 0;
    }

    template <class T>
    size_type _M_bkt_num_key(const T& __key) const {
      return _M_bkt_num_key(__key, _M_buckets.size());
    }

    size_type _M_bkt_num(const value_type& __obj) const {
      return _M_bkt_num_key(_M_get_key(__obj));
    }

    template <class T>
    size_type _M_bkt_num_key(const T& __key, size_t __n) const {
      return _M_hash(__key) % __n;
    }

    size_type _M_bkt_num(const value_type& __obj, size_t __n) const {
      return _M_bkt_num_key(_M_get_key(__obj), __n);
    }

    _Node* _M_new_node(const value_type& __obj) {
      _Node* __n = _M_get_node();
      __n->_M_next = 0;

      try
      {
        new (&__n->_M_val) value_type(__obj);
        //std::_Construct(&__n->_M_val, __obj);
        return __n;
      }
      catch ( ...)
      {
        _M_put_node(__n);
        throw;
      }
    }

    void _M_delete_node(_Node* __n) {
      (&__n->_M_val)->~value_type();
      _M_put_node(__n);
    }

    void _M_erase_bucket(const size_type __n, _Node* __first, _Node* __last);
    void _M_erase_bucket(const size_type __n, _Node* __last);
    void _M_copy_from(const ATOM_HashTable& __ht);
  };

template <class _Val,class _Key,class _Alloc,class _HF,class _ExK,class _EqK>
inline _ATOM_HashTable_iterator<_Val,_Key,_Alloc,_HF,_ExK,_EqK>&
_ATOM_HashTable_iterator<_Val,_Key,_Alloc,_HF,_ExK,_EqK> ::operator++() {
  const _Node* __old = _M_cur;
  _M_cur = _M_cur->_M_next;
  if ( !_M_cur)
  {
    size_type __bucket = _M_ht->_M_bkt_num(__old->_M_val);
    while ( !_M_cur && ++__bucket < _M_ht->_M_buckets.size())
      _M_cur = _M_ht->_M_buckets[__bucket];
  }
  return *this;
}

template <class _Val,class _Key,class _Alloc,class _HF,class _ExK,class _EqK>
inline _ATOM_HashTable_iterator<_Val,_Key,_Alloc,_HF,_ExK,_EqK>
_ATOM_HashTable_iterator<_Val,_Key,_Alloc,_HF,_ExK,_EqK> ::operator++(int) {
  iterator __tmp = *this;
  ++ * this;
  return __tmp;
}

template <class _Val,class _Key,class _Alloc,class _HF,class _ExK,class _EqK>
inline _ATOM_HashTable_const_iterator<_Val,_Key,_Alloc,_HF,_ExK,_EqK>&
_ATOM_HashTable_const_iterator<_Val,_Key,_Alloc,_HF,_ExK,_EqK> ::operator++() {
  const _Node* __old = _M_cur;
  _M_cur = _M_cur->_M_next;
  if ( !_M_cur)
  {
    size_type __bucket = _M_ht->_M_bkt_num(__old->_M_val);
    while ( !_M_cur && ++__bucket < _M_ht->_M_buckets.size())
      _M_cur = _M_ht->_M_buckets[__bucket];
  }
  return *this;
}

template <class _Val,class _Key,class _Alloc,class _HF,class _ExK,class _EqK>
inline _ATOM_HashTable_const_iterator<_Val,_Key,_Alloc,_HF,_ExK,_EqK>
_ATOM_HashTable_const_iterator<_Val,_Key,_Alloc,_HF,_ExK,_EqK> ::operator++(int) {
  const_iterator __tmp = *this;
  ++ * this;
  return __tmp;
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
inline bool operator==(const ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>& __ht1, const ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>& __ht2) {
  typedef typename ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::_Node _Node;

  if ( __ht1._M_buckets.size() != __ht2._M_buckets.size())
  {
    return false;
  }

  for ( size_t __n = 0; __n < __ht1._M_buckets.size(); ++__n)
  {
    _Node* __cur1 = __ht1._M_buckets[__n];
    _Node* __cur2 = __ht2._M_buckets[__n];

    // Check same length of lists
    for ( ; __cur1 && __cur2; __cur1 = __cur1->_M_next, __cur2 = __cur2->_M_next)
    {
    }

    if ( __cur1 || __cur2)
      return false;

    // Now check one's elements are in the other
    for ( __cur1 = __ht1._M_buckets[__n] ; __cur1; __cur1 = __cur1->_M_next)
    {
      bool _found__cur1 = false;
      for ( _Node*__cur2 = __ht2._M_buckets[__n]; __cur2; __cur2 = __cur2->_M_next)
      {
        if ( __cur1->_M_val == __cur2->_M_val)
        {
          _found__cur1 = true;
          break;
        }
      }
      if ( !_found__cur1)
        return false;
    }
  }
  return true;
}  

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
inline bool operator!=(const ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>& __ht1, const ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>& __ht2) {
  return !(__ht1 == __ht2);
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Extract,class _EqKey>
inline void swap(ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Extract,_EqKey>& __ht1, ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Extract,_EqKey>& __ht2) {
  __ht1.swap(__ht2);
}


template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
std::pair<typename ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::iterator, bool> ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::insert_unique_noresize(const value_type& __obj) {
  const size_type __n = _M_bkt_num(__obj);
  _Node* __first = _M_buckets[__n];

  for ( _Node*__cur = __first; __cur; __cur = __cur->_M_next)
    if ( _M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj)))
    {
      return std::pair<iterator,bool>(iterator(__cur, this), false);
    }

  _Node* __tmp = _M_new_node(__obj);
  __tmp->_M_next = __first;
  _M_buckets[__n] = __tmp;
  ++_M_num_elements;
  return std::pair<iterator,bool>(iterator(__tmp, this), true);
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
typename ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::iterator ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::insert_equal_noresize(const value_type& __obj) {
  const size_type __n = _M_bkt_num(__obj);
  _Node* __first = _M_buckets[__n];

  for ( _Node*__cur = __first; __cur; __cur = __cur->_M_next)
    if ( _M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj)))
    {
      _Node* __tmp = _M_new_node(__obj);
      __tmp->_M_next = __cur->_M_next;
      __cur->_M_next = __tmp;
      ++_M_num_elements;
      return iterator(__tmp, this);
    }

  _Node* __tmp = _M_new_node(__obj);
  __tmp->_M_next = __first;
  _M_buckets[__n] = __tmp;
  ++_M_num_elements;
  return iterator(__tmp, this);
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
typename ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::reference ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::find_or_insert(const value_type& __obj) {
  resize(_M_num_elements + 1);

  size_type __n = _M_bkt_num(__obj);
  _Node* __first = _M_buckets[__n];

  for ( _Node*__cur = __first; __cur; __cur = __cur->_M_next)
    if ( _M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj)))
    {
      return __cur->_M_val;
    }

  _Node* __tmp = _M_new_node(__obj);
  __tmp->_M_next = __first;
  _M_buckets[__n] = __tmp;
  ++_M_num_elements;
  return __tmp->_M_val;
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
void ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::erase(const iterator& __it) {
  _Node* __p = __it._M_cur;

  if ( __p)
  {
    const size_type __n = _M_bkt_num(__p->_M_val);
    _Node* __cur = _M_buckets[__n];

    if ( __cur == __p)
    {
      _M_buckets[__n] = __cur->_M_next;
      _M_delete_node(__cur);
      --_M_num_elements;
    }
    else
    {
      _Node* __next = __cur->_M_next;
      while ( __next)
      {
        if ( __next == __p)
        {
          __cur->_M_next = __next->_M_next;
          _M_delete_node(__next);
          --_M_num_elements;
          break;
        }
        else
        {
          __cur = __next;
          __next = __cur->_M_next;
        }
      }
    }
  }
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
void ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::erase(iterator __first, iterator __last) {
  size_type __f_bucket = __first._M_cur ? _M_bkt_num(__first._M_cur->_M_val) : _M_buckets.size();
  size_type __l_bucket = __last._M_cur ? _M_bkt_num(__last._M_cur->_M_val) : _M_buckets.size();

  if ( __first._M_cur == __last._M_cur)
  {
    return;
  }
  else if ( __f_bucket == __l_bucket)
  {
    _M_erase_bucket(__f_bucket, __first._M_cur, __last._M_cur);
  }
  else
  {
    _M_erase_bucket(__f_bucket, __first._M_cur, 0);

    for ( size_type __n = __f_bucket + 1; __n < __l_bucket; ++__n)
      _M_erase_bucket(__n, 0);

    if ( __l_bucket != _M_buckets.size())
      _M_erase_bucket(__l_bucket, __last._M_cur);
  }
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
inline void ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::erase(const_iterator __first, const_iterator __last) {
  erase(iterator(const_cast<_Node*>(__first._M_cur), const_cast<ATOM_HashTable*>(__first._M_ht)),
    iterator(const_cast<_Node*>(__last._M_cur), const_cast<ATOM_HashTable*>(__last._M_ht)));
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
inline void ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::erase(const const_iterator& __it) {
  erase(iterator(const_cast<_Node*>(__it._M_cur), const_cast<ATOM_HashTable*>(__it._M_ht)));
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
void ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::resize(size_type __num_elements_hint) {
  const size_type __old_n = _M_buckets.size();
  if ( __num_elements_hint > __old_n)
  {
    const size_type __n = _M_next_size(__num_elements_hint);
    if ( __n > __old_n)
    {
      std::vector<_Node*, bucket_allocator_type> __tmp(__n, (_Node*) (0));
      try
      {
        for ( size_type __bucket = 0; __bucket < __old_n; ++__bucket)
        {
          _Node* __first = _M_buckets[__bucket];
          while ( __first)
          {
            size_type __new_bucket = _M_bkt_num(__first->_M_val, __n);
            _M_buckets[__bucket] = __first->_M_next;
            __first->_M_next = __tmp[__new_bucket];
            __tmp[__new_bucket] = __first;
            __first = _M_buckets[__bucket];
          }
        }
        _M_buckets.swap(__tmp);
      }
      catch ( ...)
      {
        for ( size_type __bucket = 0; __bucket < __tmp.size(); ++__bucket)
        {
          while ( __tmp[__bucket])
          {
            _Node* __next = __tmp[__bucket]->_M_next;
            _M_delete_node(__tmp[__bucket]);
            __tmp[__bucket] = __next;
          }
        }
        throw;
      }
    }
  }
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
void ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::_M_erase_bucket(const size_type __n, _Node* __first, _Node* __last) {
  _Node* __cur = _M_buckets[__n];
  if ( __cur == __first)
  {
    _M_erase_bucket(__n, __last);
  }
  else
  {
    _Node* __next;
    for ( __next = __cur->_M_next; __next != __first; __cur = __next, __next = __cur->_M_next)
      ;
    while ( __next != __last)
    {
      __cur->_M_next = __next->_M_next;
      _M_delete_node(__next);
      __next = __cur->_M_next;
      --_M_num_elements;
    }
  }
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
void ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::_M_erase_bucket(const size_type __n, _Node* __last) {
  _Node* __cur = _M_buckets[__n];
  while ( __cur != __last)
  {
    _Node* __next = __cur->_M_next;
    _M_delete_node(__cur);
    __cur = __next;
    _M_buckets[__n] = __cur;
    --_M_num_elements;
  }
}

template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
void ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::clear() {
  for ( size_type __i = 0; __i < _M_buckets.size(); ++__i)
  {
    _Node* __cur = _M_buckets[__i];
    while ( __cur != 0)
    {
      _Node* __next = __cur->_M_next;
      _M_delete_node(__cur);
      __cur = __next;
    }
    _M_buckets[__i] = 0;
  }
  _M_num_elements = 0;
}


template <class _Val,class _Key,class _Alloc,class _HF,class _Ex,class _Eq>
void ATOM_HashTable<_Val,_Key,_Alloc,_HF,_Ex,_Eq>::_M_copy_from(const ATOM_HashTable& __ht) {
  _M_buckets.clear();
  _M_buckets.reserve(__ht._M_buckets.size());
  _M_buckets.insert(_M_buckets.end(), __ht._M_buckets.size(), (_Node *) 0);
  try
  {
    for ( size_type __i = 0; __i < __ht._M_buckets.size(); ++__i)
    {
      const _Node* __cur = __ht._M_buckets[__i];
      if ( __cur)
      {
        _Node* __local_copy = _M_new_node(__cur->_M_val);
        _M_buckets[__i] = __local_copy;

        for ( _Node*__next = __cur->_M_next; __next; __cur = __next, __next = __cur->_M_next)
        {
          __local_copy->_M_next = _M_new_node(__next->_M_val);
          __local_copy = __local_copy->_M_next;
        }
      }
    }
    _M_num_elements = __ht._M_num_elements;
  }
  catch ( ...)
  {
    clear();
    throw;
  }
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif // __DEBUGHELP_HASHTABLE_H

