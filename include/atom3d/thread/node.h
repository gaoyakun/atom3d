#ifndef __DEBUGHELP_NODE_H__
#define __DEBUGHELP_NODE_H__

#if _MSC_VER > 1000
#pragma once
#endif

template<class T> 
struct lf_Node {
  T _value;
  lf_Node<T> * volatile _next;

  lf_Node (void) : _value(), _next(0) {}
  lf_Node (const T &v) : _next(0), _value(v) {}
};


#endif // __DEBUGHELP_NODE_H__
