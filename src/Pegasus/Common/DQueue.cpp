//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to 
// deal in the Software without restriction, including without limitation the 
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN 
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "DQueue.h"

PEGASUS_NAMESPACE_BEGIN



 // insert new node at the back of the list 
template<class L> void DQueue<L>::insert_first(L *element) throw(IPCException)
{
  DQueue *ins = new DQueue(false);
  ins->_rep = element;
  try {  ins->insert_first(*this); }
  catch(...) { delete ins; throw; }
}

template<class L> void DQueue<L>::insert_last(L *element) throw(IPCException)
{
  DQueue *ins = new DQueue(false);
  ins->_rep = element;
  try {  ins->insert_last(*this); }
  catch(...) { delete ins; throw; }
}

template<class L> void DQueue<L>::empty_list(void) throw(IPCException)
{
  if( _count > 0) {
    _mutex.lock(); 
    while( _count > 0 ) {
      DQueue *temp = _next;
      temp->unlink();
      if(temp->_rep != NULL)
	delete temp->_rep;
      delete temp;
      _count--;
    }
    assert(_count == 0);
    _mutex.unlock();
  }
  return;
}

template<class L> L *DQueue<L>::remove(void *key) throw(IPCException)
{
  L *ret = NULL;

  if( _count > 0 ) {
    _mutex.lock();
    DQueue *temp = _next;
    while ( temp->_isHead == false ) {
      if( temp->_rep->operator==( key ) ) {
	temp->unlink();
	ret = temp->_rep;
	temp->_rep = NULL;
	delete temp;
	_count--;
	break;
      }
      temp = temp->_next;
    }
    _mutex.unlock() ;
  }
  return(ret); 
}

template<class L> L *DQueue<L>::reference(void *key) throw(IPCException)
{
  // force thread to be owner of the list before it can reference
  // a node on the list 
  if( pegasus_thread_self() != _mutex.get_owner())
    throw(Permission(pegasus_thread_self()));
  if( _count > 0 ) {
    DQueue *temp = _next;
    while(temp->_isHead == false ) {
      if( temp->_rep->operator==( key)) 
	return(temp->_rep);
      temp = temp->_next;
    }
  }
  return(NULL);
}

template<class L> Boolean DQueue<L>::exists(void *key) throw(IPCException)
{
  Boolean ret = false;
  _mutex.lock();
  if( _count > 0) {
    DQueue *temp = _next;
    while(temp->_isHead == false ) {
      if( temp->_rep->operator==( key )) {
	ret = true;
	break;
      }
      temp = temp->_next;
    }
  }
  _mutex.unlock();
  return(ret);
}



PEGASUS_NAMESPACE_END
