//%///////////////////////////////////////////////////////////////////////////////
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


#ifndef Pegasus_DQueue_h
#define Pegasus_DQueue_h


#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

const int PEG_DQUEUE_FIRST = 0;
const int PEG_DQUEUE_LAST = 1;

template<class L> class PEGASUS_EXPORT DQueue {
 private: 
  L *_rep;
  DQueue *_next;
  DQueue  *_prev;
  DQueue *_cur;
  Boolean _isHead ;
  int _count;
  Mutex _mutex;

  // unlink this node from whichever list it is on
  inline void unlink( void  ) 
    { 
      _prev->_next = _next; 
      _next->_prev = _prev; 
    }

  inline void insert_first(DQueue & head) throw (IPCException)
    { 
      head._mutex.lock(pegasus_thread_self());
      _prev = &head; 
      _next = head._next; 
      head._next->_prev = this; 
      head._next = this;   
      head._count++; 
      head._mutex.unlock( );
    }

  inline void insert_last(DQueue & head) throw(IPCException)
    {
      head._mutex.lock(pegasus_thread_self()) ;
      _next = &head;
      _prev = head._prev;
      head._prev->next = this;
      head._prev = this;
      head._count++;
      head._mutex.unlock( );
    }
  inline L *remove( int code ) throw(IPCException)
    {
      L *ret = NULL;
      
      if( _count > 0 ) {
	_mutex.lock(pegasus_thread_self());
	DQueue *temp = NULL;
	if(code == PEG_DQUEUE_FIRST )
	  temp = _next;
	else
	  temp = _prev;
	
	temp->unlink();
	ret = temp->_rep;
	// unhinge ret from temp so it doesn't get destroyed 
	temp->_rep = NULL ;
	delete temp;
	_count--;
	_mutex.unlock( );
      }
      return(ret);
    }

public:

    
  DQueue(Boolean head = true) :  _rep(NULL), _isHead(head), _count(0) 
    { 
      _next = this; 
      _prev = this; 
      _cur = this;
    }


  ~DQueue() {  this->empty_list(); _mutex.~Mutex(); }
  void insert_first(L *element) throw(IPCException) 
{
  DQueue *ins = new DQueue(false);
  ins->_rep = element;
  try {  ins->insert_first(*this); }
  catch(...) { delete ins; throw; }
}

  void insert_last(L *) throw(IPCException)
{
  DQueue *ins = new DQueue(false);
  ins->_rep = element;
  try {  ins->insert_last(*this); }
  catch(...) { delete ins; throw; }
}

  void empty_list( void ) throw(IPCException) 
{
  if( _count > 0) {
    _mutex.lock(pegasus_thread_self()); 
    while( _count > 0 ) {
      DQueue *temp = _next;
      temp->unlink();
      if(temp->_rep != NULL)
	delete temp->_rep;
      delete temp;
      _count--;
    }
    PEGASUS_ASSERT(_count == 0);
    _mutex.unlock();
  }
  return;
}



  inline L *remove_first ( void ) throw(IPCException) { return(remove(PEG_DQUEUE_FIRST) );}
  inline L *remove_last ( void ) throw(IPCException) { return(remove(PEG_DQUEUE_LAST) );}

  L *remove(void *key) throw(IPCException)
{
  L *ret = NULL;

  if( _count > 0 ) {
    _mutex.lock(pegasus_thread_self());
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

  L *reference(void *key) throw(IPCException)
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


  inline L *next( L * ref) throw(IPCException)
    {
      if (_mutex.owner() != pegasus_thread_self())
	throw(Permission(pegasus_thread_self()));
      if( ref == NULL)
	_cur = _next;
      else {
	PEGASUS_ASSERT(ref == _cur->_rep);
	_cur = _cur->next;
      }
      return(_cur->rep);
    }

  inline L *prev( L * ref) throw(IPCException)
    {
      if (_mutex.owner() != pegasus_thread_self())
	throw(Permission(pegasus_thread_self()));
      if( ref == NULL)
	_cur = _prev;
      else {
	PEGASUS_ASSERT(ref == _cur->_rep);
	_cur = _cur->prev;
      }
      return(_cur->rep);
    }

  inline void lock(void) throw(IPCException) { _mutex.lock(pegasus_thread_self()); }
  inline void unlock(void) throw(IPCException) { _mutex.unlock() ; }
  Boolean exists(void *key) throw(IPCException) 
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
  inline int count(void) { return _count ; }
} ;

PEGASUS_NAMESPACE_END

#endif /* Pegasus_DQueue_h */

