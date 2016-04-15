#ifndef __ATOM_UTILS_CACHEALLOC_H
#define __ATOM_UTILS_CACHEALLOC_H

template <class T>
class ATOM_CacheAlloc
  {
    struct node
    {
      char data[sizeof(T)];
      node *next;
      void init (void) { new(reinterpret_cast<T*>(data)) T(); }
      void fini (void) { (reinterpret_cast<T*>(data))->~T(); }
      node (void): next(0) {}
    };
  protected:
    ATOM_CacheAlloc (void);
    ~ATOM_CacheAlloc (void);
  public:
    static ATOM_CacheAlloc &getInstance (void) {
      static ATOM_CacheAlloc<T> instance;
      return instance;
    }
  public:
    T *allocate (void);
    void deallocate (T *p);
    void purge (void);
  private:
    node *_M_reserved;
  };

template <class T>
inline ATOM_CacheAlloc<T>::ATOM_CacheAlloc (void): _M_reserved(0) {
}

template <class T>
inline ATOM_CacheAlloc<T>::~ATOM_CacheAlloc (void) {
  purge ();
}

template <class T>
inline void ATOM_CacheAlloc<T>::purge (void) {
  while (_M_reserved)
  {
    node *next = _M_reserved->next;
    _M_reserved->fini ();
    delete _M_reserved;
    _M_reserved = next;
  }
}

template <class T>
inline T *ATOM_CacheAlloc<T>::allocate (void) {
  node *result = _M_reserved;
  if (result)
  {
    _M_reserved = _M_reserved->next;
  }
  else
  {
    result = new node;
  }
  result->init ();
  return reinterpret_cast<T*>(result);
}

template <class T>
inline void ATOM_CacheAlloc<T>::deallocate (T *p) {
  if (p)
  {
    node *n = reinterpret_cast<node*>(p);
    n->fini ();
    n->next = _M_reserved;
    _M_reserved = n;
  }
}

template <class T>
T *cache_alloc (void) {
  return ATOM_CacheAlloc<T>::getInstance().allocate ();
}

template <class T>
void cache_destroy (T *p) {
  return ATOM_CacheAlloc<T>::getInstance().deallocate (p);
}

#endif // __ATOM_UTILS_CACHEALLOC_H
