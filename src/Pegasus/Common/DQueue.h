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

PEGASUS_NAMESPACE_BEGIN

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

  // insert this node into list owned by head 
  // this node becomes the first node 
  inline void insert(DQueue & head) {  _prev = head; 
                      _next = head._next; 
                     head._next->_prev = this; 
                     head._next = this;   
                     head._count++; }

public:
  DQueue(Boolean head = true) :  _rep(NULL), _isHead(head), _count(0) 
    { 
      _next = this; 
      _prev = this; 
      _cur = this;
    }

  ~DQueue() { empty_list() ; }
  void insert(L *);
  void empty_list( void ) ;
  L *remove( void ) ;
  L *remove(Sint8 *key) ;
  L *reference(Sint8 *key);
  L *next( L * ); // poor man's iterators 
  L *prev( L * );
  Boolean exists(Sint8 *key);
  int count(void);
} ;

PEGASUS_NAMESPACE_END

#endif /* Pegasus_DQueue_h */

