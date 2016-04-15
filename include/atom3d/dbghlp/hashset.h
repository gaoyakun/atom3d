#ifndef __DEBUGHELP_HASHSET_H
#define __DEBUGHELP_HASHSET_H

#include "hashtable.h"
#include <functional>

// Forward declaration of equality operator; needed for friend declaration.

template <class _Value,class _Alloc = std::allocator<_Value>,class _HashFcn = ATOM_hasher<_Value>,class _EqualKey = atom_internal::equal_to<_Value> >
class ATOM_HashSet;

template <class _Value,class _Alloc,class _HashFcn,class _EqualKey>
inline bool operator==(const ATOM_HashSet<_Value,_Alloc,_HashFcn,_EqualKey>& __hs1,
                       const ATOM_HashSet<_Value,_Alloc,_HashFcn,_EqualKey>& __hs2);

template <class _Value,class _Alloc,class _HashFcn,class _EqualKey>
class ATOM_HashSet
  {
  private:
    typedef ATOM_HashTable<_Value,_Value,_Alloc,_HashFcn,atom_internal::identity<_Value>,_EqualKey> _Ht;
    _Ht _M_ht;

  public:
    typedef typename _Ht::key_type key_type;
    typedef typename _Ht::value_type value_type;
    typedef typename _Ht::hasher hasher;
    typedef typename _Ht::key_equal key_equal;
    typedef typename _Ht::size_type size_type;
    typedef typename _Ht::difference_type difference_type;
    typedef typename _Ht::const_pointer pointer;
    typedef typename _Ht::const_pointer const_pointer;
    typedef typename _Ht::const_reference reference;
    typedef typename _Ht::const_reference const_reference;
    typedef typename _Ht::const_iterator iterator;
    typedef typename _Ht::const_iterator const_iterator;
    typedef typename _Ht::allocator_type allocator_type;

    hasher hash_funct() const {
      return _M_ht.hash_funct();
    }
    key_equal key_eq() const {
      return _M_ht.key_eq();
    }

  public:
    ATOM_HashSet()
      : _M_ht(100, hasher(), key_equal()) {
    }
    explicit ATOM_HashSet(size_type __n)
      : _M_ht(__n, hasher(), key_equal()) {
    }
    ATOM_HashSet(size_type __n, const hasher& __hf)
      : _M_ht(__n, __hf, key_equal()) {
    }
    ATOM_HashSet(size_type __n, const hasher& __hf, const key_equal& __eql)
      : _M_ht(__n, __hf, __eql) {
    }

    template <class _InputIterator>
    ATOM_HashSet(_InputIterator __f, _InputIterator __l)
      : _M_ht(100, hasher(), key_equal()) {
      _M_ht.insert_unique(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashSet(_InputIterator __f, _InputIterator __l, size_type __n)
      : _M_ht(__n, hasher(), key_equal()) {
      _M_ht.insert_unique(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashSet(_InputIterator __f, _InputIterator __l, size_type __n, const hasher& __hf)
      : _M_ht(__n, __hf, key_equal()) {
      _M_ht.insert_unique(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashSet(_InputIterator __f, _InputIterator __l, size_type __n, const hasher& __hf, const key_equal& __eql)
      : _M_ht(__n, __hf, __eql) {
      _M_ht.insert_unique(__f, __l);
    }

  public:
    size_type size() const {
      return _M_ht.size();
    }
    size_type max_size() const {
      return _M_ht.max_size();
    }
    bool empty() const {
      return _M_ht.empty();
    }
    void swap(ATOM_HashSet& __hs) {
      _M_ht.swap(__hs._M_ht);
    }

    template <class _Val,class _Al,class _HF,class _EqK>
    friend bool operator==(const ATOM_HashSet<_Val,_Al,_HF,_EqK>&, const ATOM_HashSet<_Val,_Al,_HF,_EqK>&);

    iterator begin() const {
      return _M_ht.begin();
    }
    iterator end() const {
      return _M_ht.end();
    }

  public:
    std::pair<iterator, bool> insert(const value_type& __obj) {
      std::pair<typename _Ht::iterator,bool> __p = _M_ht.insert_unique(__obj);
      return std::pair<iterator,bool>(__p.first, __p.second);
    }

    template <class _InputIterator>
    void insert(_InputIterator __f, _InputIterator __l) {
      _M_ht.insert_unique(__f, __l);
    }

    std::pair<iterator, bool> insert_noresize(const value_type& __obj) {
      std::pair<typename _Ht::iterator,bool> __p = _M_ht.insert_unique_noresize(__obj);
      return std::pair<iterator,bool>(__p.first, __p.second);
    }

    template <class T>
    iterator find(const T& __key) const {
      return _M_ht.find(__key);
    }

    template <class T>
    size_type count(const T& __key) const {
      return _M_ht.count(__key);
    }

    template <class T>
    std::pair<iterator, iterator> equal_range(const T& __key) const {
      return _M_ht.equal_range(__key);
    }

    template <class T>
    size_type erase(const T& __key) {
      return _M_ht.erase(__key);
    }
    void erase(iterator __it) {
      _M_ht.erase(__it);
    }
    void erase(iterator __f, iterator __l) {
      _M_ht.erase(__f, __l);
    }
    void clear() {
      _M_ht.clear();
    }

  public:
    void resize(size_type __hint) {
      _M_ht.resize(__hint);
    }
    size_type bucket_count() const {
      return _M_ht.bucket_count();
    }
    size_type max_bucket_count() const {
      return _M_ht.max_bucket_count();
    }
    size_type elems_in_bucket(size_type __n) const {
      return _M_ht.elems_in_bucket(__n);
    }
  };

template <class _Value,class _Alloc,class _HashFcn,class _EqualKey>
inline bool operator==(const ATOM_HashSet<_Value,_Alloc,_HashFcn,_EqualKey>& __hs1,
                       const ATOM_HashSet<_Value,_Alloc,_HashFcn,_EqualKey>& __hs2) {
  return __hs1._M_ht == __hs2._M_ht;
}

template <class _Value,class _Alloc,class _HashFcn,class _EqualKey>
inline bool operator!=(const ATOM_HashSet<_Value,_Alloc,_HashFcn,_EqualKey>& __hs1,
                       const ATOM_HashSet<_Value,_Alloc,_HashFcn,_EqualKey>& __hs2) {
  return !(__hs1 == __hs2);
}

template <class _Val,class _Alloc,class _HashFcn,class _EqualKey>
inline void swap(ATOM_HashSet<_Val,_Alloc,_HashFcn,_EqualKey>& __hs1, ATOM_HashSet<_Val,_Alloc,_HashFcn,_EqualKey>& __hs2) {
  __hs1.swap(__hs2);
}


template <class _Value,class _Alloc = std::allocator<_Value>,class _HashFcn = ATOM_hasher<_Value>,class _EqualKey = atom_internal::equal_to<_Value> >
class ATOM_HashMultiSet;

template <class _Val,class _Alloc,class _HashFcn,class _EqualKey>
inline bool operator==(const ATOM_HashMultiSet<_Val,_Alloc,_HashFcn,_EqualKey>& __hs1,
                       const ATOM_HashMultiSet<_Val,_Alloc,_HashFcn,_EqualKey>& __hs2);


template <class _Value,class _Alloc,class _HashFcn,class _EqualKey>
class ATOM_HashMultiSet
  {
  private:
    typedef ATOM_HashTable<_Value,_Value,_Alloc,_HashFcn,atom_internal::identity<_Value>,_EqualKey> _Ht;
    _Ht _M_ht;

  public:
    typedef typename _Ht::key_type key_type;
    typedef typename _Ht::value_type value_type;
    typedef typename _Ht::hasher hasher;
    typedef typename _Ht::key_equal key_equal;
    typedef typename _Ht::size_type size_type;
    typedef typename _Ht::difference_type difference_type;
    typedef typename _Ht::const_pointer pointer;
    typedef typename _Ht::const_pointer const_pointer;
    typedef typename _Ht::const_reference reference;
    typedef typename _Ht::const_reference const_reference;
    typedef typename _Ht::const_iterator iterator;
    typedef typename _Ht::const_iterator const_iterator;
    typedef typename _Ht::allocator_type allocator_type;

    hasher hash_funct() const {
      return _M_ht.hash_funct();
    }
    key_equal key_eq() const {
      return _M_ht.key_eq();
    }

  public:
    ATOM_HashMultiSet()
      : _M_ht(100, hasher(), key_equal()) {
    }
    explicit ATOM_HashMultiSet(size_type __n)
      : _M_ht(__n, hasher(), key_equal()) {
    }
    ATOM_HashMultiSet(size_type __n, const hasher& __hf)
      : _M_ht(__n, __hf, key_equal()) {
    }
    ATOM_HashMultiSet(size_type __n, const hasher& __hf, const key_equal& __eql)
      : _M_ht(__n, __hf, __eql) {
    }

    template <class _InputIterator>
    ATOM_HashMultiSet(_InputIterator __f, _InputIterator __l)
      : _M_ht(100, hasher(), key_equal()) {
      _M_ht.insert_equal(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashMultiSet(_InputIterator __f, _InputIterator __l, size_type __n)
      : _M_ht(__n, hasher(), key_equal()) {
      _M_ht.insert_equal(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashMultiSet(_InputIterator __f, _InputIterator __l, size_type __n, const hasher& __hf)
      : _M_ht(__n, __hf, key_equal()) {
      _M_ht.insert_equal(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashMultiSet(_InputIterator __f, _InputIterator __l, size_type __n, const hasher& __hf, const key_equal& __eql)
      : _M_ht(__n, __hf, __eql) {
      _M_ht.insert_equal(__f, __l);
    }

  public:
    size_type size() const {
      return _M_ht.size();
    }
    size_type max_size() const {
      return _M_ht.max_size();
    }
    bool empty() const {
      return _M_ht.empty();
    }
    void swap(ATOM_HashMultiSet& hs) {
      _M_ht.swap(hs._M_ht);
    }

    template <class _Val,class _Al,class _HF,class _EqK>
    friend bool operator==(const ATOM_HashMultiSet<_Val,_Al,_HF,_EqK>&, const ATOM_HashMultiSet<_Val,_Al,_HF,_EqK>&);

    iterator begin() const {
      return _M_ht.begin();
    }
    iterator end() const {
      return _M_ht.end();
    }

  public:
    iterator insert(const value_type& __obj) {
      return _M_ht.insert_equal(__obj);
    }

    template <class _InputIterator>
    void insert(_InputIterator __f, _InputIterator __l) {
      _M_ht.insert_equal(__f, __l);
    }

    iterator insert_noresize(const value_type& __obj) {
      return _M_ht.insert_equal_noresize(__obj);
    }    

    template <class T>
    iterator find(const T& __key) const {
      return _M_ht.find(__key);
    }

    template <class T>
    size_type count(const T& __key) const {
      return _M_ht.count(__key);
    }

    template <class T>
    std::pair<iterator, iterator> equal_range(const T& __key) const {
      return _M_ht.equal_range(__key);
    }

    template <class T>
    size_type erase(const T& __key) {
      return _M_ht.erase(__key);
    }
    void erase(iterator __it) {
      _M_ht.erase(__it);
    }
    void erase(iterator __f, iterator __l) {
      _M_ht.erase(__f, __l);
    }
    void clear() {
      _M_ht.clear();
    }

  public:
    void resize(size_type __hint) {
      _M_ht.resize(__hint);
    }
    size_type bucket_count() const {
      return _M_ht.bucket_count();
    }
    size_type max_bucket_count() const {
      return _M_ht.max_bucket_count();
    }
    size_type elems_in_bucket(size_type __n) const {
      return _M_ht.elems_in_bucket(__n);
    }
  };

template <class _Val,class _Alloc,class _HashFcn,class _EqualKey>
inline bool operator==(const ATOM_HashMultiSet<_Val,_Alloc,_HashFcn,_EqualKey>& __hs1,
                       const ATOM_HashMultiSet<_Val,_Alloc,_HashFcn,_EqualKey>& __hs2) {
  return __hs1._M_ht == __hs2._M_ht;
}

template <class _Val,class _Alloc,class _HashFcn,class _EqualKey>
inline bool operator!=(const ATOM_HashMultiSet<_Val,_Alloc,_HashFcn,_EqualKey>& __hs1,
                       const ATOM_HashMultiSet<_Val,_Alloc,_HashFcn,_EqualKey>& __hs2) {
  return !(__hs1 == __hs2);
}

template <class _Val,class _Alloc,class _HashFcn,class _EqualKey>
inline void swap(ATOM_HashMultiSet<_Val,_Alloc,_HashFcn,_EqualKey>& __hs1, ATOM_HashMultiSet<_Val,_Alloc,_HashFcn,_EqualKey>& __hs2) {
  __hs1.swap(__hs2);
}

#endif // __DEBUGHELP_HASHSET_H


