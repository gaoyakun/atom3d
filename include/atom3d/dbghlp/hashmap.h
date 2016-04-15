#ifndef __DEBUGHELP_HASHMAP_H
#define __DEBUGHELP_HASHMAP_H

#include <memory>
#include <functional>
#include "hasher.h"
#include "hashtable.h"

// Forward declaration of equality operator; needed for friend declaration.

template <class _Key,class _Tp,class _Alloc = std::allocator<_Key>,class _HashFcn = ATOM_hasher<_Key>,class _EqualKey = atom_internal::equal_to<_Key> >
class ATOM_HashMap;

template <class _Key,class _Tp,class _HashFn,class _EqKey>
inline bool operator==(const ATOM_HashMap<_Key,_Tp,_HashFn,_EqKey>&, const ATOM_HashMap<_Key,_Tp,_HashFn,_EqKey>&);

template <class _Key,class _Tp,class _Alloc,class _HashFcn,class _EqualKey>
class ATOM_HashMap
  {
    typedef std::allocator<typename _Alloc::value_type> std_allocator_type;

  private:
    typedef ATOM_HashTable<std::pair<const _Key,_Tp>,_Key,_Alloc,_HashFcn,atom_internal::select1st<std::pair<const _Key,_Tp> >,_EqualKey> _Ht;
    _Ht _M_ht;

  public:
    typedef typename _Ht::key_type key_type;
    typedef _Tp data_type;
    typedef _Tp mapped_type;
    typedef typename _Ht::value_type value_type;
    typedef typename _Ht::hasher hasher;
    typedef typename _Ht::key_equal key_equal;
    typedef typename _Ht::size_type size_type;
    typedef typename _Ht::difference_type difference_type;
    typedef typename _Ht::pointer pointer;
    typedef typename _Ht::const_pointer const_pointer;
    typedef typename _Ht::reference reference;
    typedef typename _Ht::const_reference const_reference;
    typedef typename _Ht::iterator iterator;
    typedef typename _Ht::const_iterator const_iterator;
    typedef typename _Ht::allocator_type allocator_type;

    allocator_type _M_a;

    hasher hash_funct() const {
      return _M_ht.hash_funct();
    }
    key_equal key_eq() const {
      return _M_ht.key_eq();
    }

  public:
    ATOM_HashMap()
      : _M_ht(100, hasher(), key_equal()) {
    }
    explicit ATOM_HashMap(size_type __n)
      : _M_ht(__n, hasher(), key_equal()) {
    }
    ATOM_HashMap(size_type __n, const hasher& __hf)
      : _M_ht(__n, __hf, key_equal()) {
    }
    ATOM_HashMap(size_type __n, const hasher& __hf, const key_equal& __eql)
      : _M_ht(__n, __hf, __eql) {
    }

    template <class _InputIterator>
    ATOM_HashMap(_InputIterator __f, _InputIterator __l)
      : _M_ht(100, hasher(), key_equal()) {
      _M_ht.insert_unique(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashMap(_InputIterator __f, _InputIterator __l, size_type __n)
      : _M_ht(__n, hasher(), key_equal()) {
      _M_ht.insert_unique(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashMap(_InputIterator __f, _InputIterator __l, size_type __n, const hasher& __hf)
      : _M_ht(__n, __hf, key_equal()) {
      _M_ht.insert_unique(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashMap(_InputIterator __f, _InputIterator __l, size_type __n, const hasher& __hf, const key_equal& __eql)
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
    void swap(ATOM_HashMap& __hs) {
      _M_ht.swap(__hs._M_ht);
    }

    template <class _K1,class _T1,class _HF,class _EqK>
    friend bool operator==(const ATOM_HashMap<_K1,_T1,_HF,_EqK>&, const ATOM_HashMap<_K1,_T1,_HF,_EqK>&);

    iterator begin() {
      return _M_ht.begin();
    }
    iterator end() {
      return _M_ht.end();
    }
    const_iterator begin() const {
      return _M_ht.begin();
    }
    const_iterator end() const {
      return _M_ht.end();
    }

  public:
    std::pair<iterator,bool> insert(const value_type& __obj) {
      return _M_ht.insert_unique(__obj);
    }

    template <class _InputIterator>
    void insert(_InputIterator __f, _InputIterator __l) {
      _M_ht.insert_unique(__f, __l);
    }

    std::pair<iterator,bool> insert_noresize(const value_type& __obj) {
      return _M_ht.insert_unique_noresize(__obj);
    }    

    template <class T>
    iterator find(const T& __key) {
      return _M_ht.find(__key);
    }

    template <class T>
    const_iterator find(const T& __key) const {
      return _M_ht.find(__key);
    }

    _Tp& operator[](const key_type& __key) {
      return _M_ht.find_or_insert(value_type(__key, _Tp())).second;
    }

    template <class T>
    size_type count(const T& __key) const {
      return _M_ht.count(__key);
    }

    template <class T>
    std::pair<iterator, iterator> equal_range(const T& __key) {
      return _M_ht.equal_range(__key);
    }

    template <class T>
    std::pair<const_iterator, const_iterator> equal_range(const T& __key) const {
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

template <class _Key,class _Tp,class _Alloc,class _HashFcn,class _EqlKey>
inline bool operator==(const ATOM_HashMap<_Key,_Tp,_Alloc,_HashFcn,_EqlKey>& __hm1,
                       const ATOM_HashMap<_Key,_Tp,_Alloc,_HashFcn,_EqlKey>& __hm2) {
  return __hm1._M_ht == __hm2._M_ht;
}

template <class _Key,class _Tp,class _Alloc,class _HashFcn,class _EqlKey>
inline bool operator!=(const ATOM_HashMap<_Key,_Tp,_Alloc,_HashFcn,_EqlKey>& __hm1,
                       const ATOM_HashMap<_Key,_Tp,_Alloc,_HashFcn,_EqlKey>& __hm2) {
  return !(__hm1 == __hm2);
}

template <class _Key,class _Tp,class _Alloc,class _HashFcn,class _EqlKey>
inline void swap(ATOM_HashMap<_Key,_Tp,_Alloc,_HashFcn,_EqlKey>& __hm1, ATOM_HashMap<_Key,_Tp,_Alloc,_HashFcn,_EqlKey>& __hm2) {
  __hm1.swap(__hm2);
}

// Forward declaration of equality operator; needed for friend declaration.

template <class _Key,class _Tp,class _Alloc = std::allocator<_Key>,class _HashFcn = ATOM_hasher<_Key>,class _EqualKey = atom_internal::equal_to<_Key> >
class ATOM_HashMultiMap;

template <class _Key,class _Tp,class _Alloc,class _HF,class _EqKey>
inline bool operator==(const ATOM_HashMultiMap<_Key,_Tp,_Alloc,_HF,_EqKey>& __hm1,
                       const ATOM_HashMultiMap<_Key,_Tp,_Alloc,_HF,_EqKey>& __hm2);

template <class _Key,class _Tp,class _Alloc,class _HashFcn,class _EqualKey>
class ATOM_HashMultiMap
  {
  private:
    typedef ATOM_HashTable<std::pair<const _Key,_Tp>,_Key,_Alloc,_HashFcn,atom_internal::select1st<std::pair<const _Key,_Tp> >,_EqualKey> _Ht;
    _Ht _M_ht;

  public:
    typedef typename _Ht::key_type key_type;
    typedef _Tp data_type;
    typedef _Tp mapped_type;
    typedef typename _Ht::value_type value_type;
    typedef typename _Ht::hasher hasher;
    typedef typename _Ht::key_equal key_equal;
    typedef typename _Ht::size_type size_type;
    typedef typename _Ht::difference_type difference_type;
    typedef typename _Ht::pointer pointer;
    typedef typename _Ht::const_pointer const_pointer;
    typedef typename _Ht::reference reference;
    typedef typename _Ht::const_reference const_reference;
    typedef typename _Ht::iterator iterator;
    typedef typename _Ht::const_iterator const_iterator;
    typedef typename _Ht::allocator_type allocator_type;

    allocator_type _M_a;

    hasher hash_funct() const {
      return _M_ht.hash_funct();
    }
    key_equal key_eq() const {
      return _M_ht.key_eq();
    }

  public:
    ATOM_HashMultiMap()
      : _M_ht(100, hasher(), key_equal()) {
    }
    explicit ATOM_HashMultiMap(size_type __n)
      : _M_ht(__n, hasher(), key_equal()) {
    }
    ATOM_HashMultiMap(size_type __n, const hasher& __hf)
      : _M_ht(__n, __hf, key_equal()) {
    }
    ATOM_HashMultiMap(size_type __n, const hasher& __hf, const key_equal& __eql)
      : _M_ht(__n, __hf, __eql) {
    }

    template <class _InputIterator>
    ATOM_HashMultiMap(_InputIterator __f, _InputIterator __l)
      : _M_ht(100, hasher(), key_equal()) {
      _M_ht.insert_equal(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashMultiMap(_InputIterator __f, _InputIterator __l, size_type __n)
      : _M_ht(__n, hasher(), key_equal()) {
      _M_ht.insert_equal(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashMultiMap(_InputIterator __f, _InputIterator __l, size_type __n, const hasher& __hf)
      : _M_ht(__n, __hf, key_equal()) {
      _M_ht.insert_equal(__f, __l);
    }

    template <class _InputIterator>
    ATOM_HashMultiMap(_InputIterator __f, _InputIterator __l, size_type __n, const hasher& __hf, const key_equal& __eql)
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
    void swap(ATOM_HashMultiMap& __hs) {
      _M_ht.swap(__hs._M_ht);
    }

    template <class _K1,class _T1,class _A1,class _HF,class _EqK>
    friend bool operator==(const ATOM_HashMultiMap<_K1,_T1,_A1,_HF,_EqK>&, const ATOM_HashMultiMap<_K1,_T1,_A1,_HF,_EqK>&);

    iterator begin() {
      return _M_ht.begin();
    }
    iterator end() {
      return _M_ht.end();
    }
    const_iterator begin() const {
      return _M_ht.begin();
    }
    const_iterator end() const {
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
    iterator find(const T& __key) {
      return _M_ht.find(__key);
    }

    template <class T>
    const_iterator find(const T& __key) const {
      return _M_ht.find(__key);
    }

    template <class T>
    size_type count(const T& __key) const {
      return _M_ht.count(__key);
    }

    template <class T>
    std::pair<iterator, iterator> equal_range(const T& __key) {
      return _M_ht.equal_range(__key);
    }

    template <class T>
    std::pair<const_iterator, const_iterator> equal_range(const T& __key) const {
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

template <class _Key,class _Tp,class _Alloc,class _HF,class _EqKey>
inline bool operator==(const ATOM_HashMultiMap<_Key,_Tp,_Alloc,_HF,_EqKey>& __hm1,
                       const ATOM_HashMultiMap<_Key,_Tp,_Alloc,_HF,_EqKey>& __hm2) {
  return __hm1._M_ht == __hm2._M_ht;
}

template <class _Key,class _Tp,class _Alloc,class _HF,class _EqKey>
inline bool operator!=(const ATOM_HashMultiMap<_Key,_Tp,_Alloc,_HF,_EqKey>& __hm1,
                       const ATOM_HashMultiMap<_Key,_Tp,_Alloc,_HF,_EqKey>& __hm2) {
  return !(__hm1 == __hm2);
}

template <class _Key,class _Tp,class _Alloc,class _HashFcn,class _EqlKey>
inline void swap(ATOM_HashMultiMap<_Key,_Tp,_Alloc,_HashFcn,_EqlKey>& __hm1, ATOM_HashMultiMap<_Key,_Tp,_Alloc,_HashFcn,_EqlKey>& __hm2) {
  __hm1.swap(__hm2);
}
#endif // __DEBUGHELP_HASHMAP_H

