#ifndef __ATOM3D_THREAD_LOCKFREE_STACK_H
#define __ATOM3D_THREAD_LOCKFREE_STACK_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "node.h"
#include "cas.h"

template<class T> 
class ATOM_LockFreeStack 
{
public:
  ATOM_LockFreeStack() : _head(0), _pops(0) {}

public:
  void push(lf_Node<T> * node);
  lf_Node<T> * pop();

private:
  // NOTE: the order of these members is assumed by ATOM_CAS2.
  lf_Node<T> * volatile _head;
  volatile unsigned _pops;
};

template<class T> 
void ATOM_LockFreeStack<T>::push(lf_Node<T> * node)
{
  for(;;)
  {
    node->_next = _head;
    if(ATOM_CAS((long volatile*)&_head, (long)node->_next, (long)node))
    {
      break;
    }
  }
}

template<class T> 
lf_Node<T> * ATOM_LockFreeStack<T>::pop(void)
{
  for(;;)
  {
    lf_Node<T> * head = _head;
    unsigned pops = _pops;
    if(!head)
    {
      return 0;
    }

    // NOTE: Memory reclaimation is difficult in this context.  If another thread breaks in here
    // and pops the head, and then frees it, then pHead->pNext is an invalid operation.  One solution
    // would be to use hazard pointers (http://researchweb.watson.ibm.com/people/m/michael/ieeetpds-2004.pdf).

    lf_Node<T> * next = head->_next;
    if(ATOM_CAS2((long volatile *)&_head, (long)head, (long)pops, (long)next, (long)(pops + 1)))
    {
      return head;
    }
  }
}
#endif // __ATOM3D_THREAD_LOCKFREE_STACK_H
