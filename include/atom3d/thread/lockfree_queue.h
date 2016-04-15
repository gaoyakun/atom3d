#ifndef __ATOM3D_THREAD_LOCKFREE_QUEUE_H
#define __ATOM3D_THREAD_LOCKFREE_QUEUE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "cas.h"
#include "node.h"

template<class T> 
class ATOM_LockFreeQueue 
{
public:
  ATOM_LockFreeQueue(lf_Node<T> * dummy) : _pops(0), _pushes(0)
  {
    _head = _tail = dummy;
  }

public:
  void add(lf_Node<T> * node);
  lf_Node<T> * remove();

private:
  // NOTE: the order of these members is assumed by dhCAS2.
  lf_Node<T> * volatile _head;
  volatile unsigned  _pops;
  lf_Node<T> * volatile _tail;
  volatile unsigned  _pushes;

};

template<class T> 
void ATOM_LockFreeQueue<T>::add(lf_Node<T> * node) 
{
  node->_next = 0;

  unsigned pushes;
  lf_Node<T> * tail;

  for (;;)
  {
    pushes = _pushes;
    tail = _tail;

    // NOTE: The Queue has the same consideration as the Stack.  If _pTail is
    // freed on a different thread, then this code can cause an access violation.

    // If the node that the tail points to is the last node
    // then update the last node to point at the new node.
    if(dhCAS(&(_tail->_next), 0L, (long)node))
    {
      break;
    }
    else
    {
      // Since the tail does not point at the last node,
      // need to keep updating the tail until it does.
      dhCAS2(&_tail, tail, pushes, _tail->_next, pushes + 1);
    }
  }

  // If the tail points to what we thought was the last node
  // then update the tail to point to the new node.
  dhCAS2(&_tail, tail, pushes, node, pushes + 1);
}

template<class T> 
lf_Node<T> * ATOM_LockFreeQueue<T>::remove() 
{
  T value;
  lf_Node<T> * head;

  for(;;)
  {
    unsigned pops = _pops;
    unsigned pushes = _pushes;
    head = _head;
    lf_Node<T> * next = head->_next;

    // Verify that we did not get the pointers in the middle
    // of another update.
    if(pops != _pops)
    {
      continue;
    }

    // Check if the queue is empty.
    if(head == _tail)
    {
      if(!next)
      {
          head = 0; // queue is empty
          break;
      }
      // Special case if the queue has nodes but the tail
      // is just behind. Move the tail off of the head.
      dhCAS2(&_tail, head, pushes, next, pushes + 1);
    }
    else if (next)
    {
      value = next->value;

      // Move the head pointer, effectively removing the node
      if(dhCAS2(&_head, head, pops, next, pops + 1))
      {
        break;
      }
    }
  }

  if(head)
  {
    head->value = value;
  }

  return head;
}

#endif // __ATOM3D_THREAD_LOCKFREE_QUEUE_H
