//%/////-*-c++-*-/////////////////////////////////////////////////////////
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



#ifndef Pegasus_internal_dq_h
#define Pegasus_internal_dq_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

#define PEG_DQUEUE_FIRST 0
#define PEG_DQUEUE_LAST 1

#if 0
template<class L> class PEGASUS_EXPORT internal_dq {
   private:
      L *_rep;
      internal_dq *_next;
      internal_dq  *_prev;
      internal_dq *_cur;
      Boolean _isHead ;
      int _count;

      // unlink this node from whichever list it is on
      inline void unlink( void  ) 
      { 
	 _prev->_next = _next; 
	 _next->_prev = _prev; 
      }
    
      inline void insert_first(internal_dq & head)
      { 
	 _prev = &head; 
	 _next = head._next; 
	 head._next->_prev = this; 
	 head._next = this;   
	 head._count++; 
      }

      inline void insert_last(internal_dq& head) 
      {
	 _next = &head;
	 _prev = head._prev;
	 head._prev->_next = this;
	 head._prev = this;
	 head._count++;
      }

      inline L *remove( int code )
      {
	 L *ret = NULL;
	
	 if( _count > 0 ) {
	    internal_dq *temp = NULL;
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
	 }
	 return(ret);
      }

   public:
    
      internal_dq(Boolean head = true) :  _rep(NULL), _isHead(head), _count(0)
      { 
	 _next = this; 
	 _prev = this; 
	 _cur = this;
      }

      virtual ~internal_dq() 
      {  
	 empty_list(); 
      }

      virtual void insert_first(L *element)
      {
	 internal_dq *ins = new internal_dq(false);
	 ins->_rep = element;
	 ins->insert_first(*this); 
      }

      virtual void insert_last(L *element)
      {
	 internal_dq *ins = new internal_dq(false);
	 ins->_rep = element;
	 ins->insert_last(*this);
      }
      
      virtual void empty_list( void )
      {
	 while( _count > 0 ) {
	    internal_dq *temp = _next;
	    temp->unlink();
	    if(temp->_rep != NULL)
	       delete temp->_rep;
	    delete temp;
	    _count--;
	 }
	 return;
      }

      inline virtual L *remove_first ( void ) { return(remove(PEG_DQUEUE_FIRST) );}
      inline virtual L *remove_last ( void ) { return(remove(PEG_DQUEUE_LAST) );}

      virtual L *remove(void *key)
      {
	 L *ret = NULL;
	 if(_count > 0)
	 {
	    internal_dq *temp = _next;
	    while ( temp->_isHead == false ) {
	       if( temp->_rep == key ) {
		  temp->unlink();
		  ret = temp->_rep;
		  temp->_rep = NULL;
		  delete temp;
		  _count--;
		  break;
	       }
	       temp = temp->_next;
	    }
	 }
	 return(ret); 
      }

      virtual L *reference(void *key)
      {
	 if( _count > 0 ) {
	    internal_dq *temp = _next;
	    while(temp->_isHead == false ) {
	       if( temp->_rep->operator==( key)) 
		  return(temp->_rep);
	       temp = temp->_next;
	    }
	 }
	 return(NULL);
      }


      virtual inline L *next( L * ref)
      {
	 if( ref == NULL)
	    _cur = _next;
	 else {
	    _cur = _cur->_next;
	 }
	 return(_cur->_rep);
      }

      virtual inline L *prev( L * ref)
      {
	 if( ref == NULL)
	    _cur = _prev;
	 else {
	    _cur = _cur->_prev;
	 }
	 return(_cur->_rep);
      }

      virtual Boolean exists(void *key) 
      {

	 Boolean ret = false;
	 if( _count > 0) {
	    internal_dq *temp = _next;
	    while(temp->_isHead == false ) 
	    {
	       if( temp->_rep->operator==( key )) 
	       {
		  ret = true;
		  break;
	       }
	       temp = temp->_next;
	    }
	 }
	 return(ret);
      }

      inline virtual int count(void) { return _count ; }
} ;

#endif

class PEGASUS_EXPORT internal_dq {
   private:
      void *_rep;
      internal_dq *_next;
      internal_dq  *_prev;
      internal_dq *_cur;
      Boolean _isHead ;
      int _count;

      // unlink this node from whichever list it is on
      inline void unlink( void  ) 
      { 
	 _prev->_next = _next; 
	 _next->_prev = _prev; 
      }
    
      inline void insert_first(internal_dq & head)
      { 
	 _prev = &head; 
	 _next = head._next; 
	 head._next->_prev = this; 
	 head._next = this;   
	 head._count++; 
      }

      inline void insert_last(internal_dq & head) 
      {
	 _next = &head;
	 _prev = head._prev;
	 head._prev->_next = this;
	 head._prev = this;
	 head._count++;
      }

      inline void *remove( int code )
      {
	 void *ret = NULL;
	
	 if( _count > 0 ) {
	    internal_dq *temp = NULL;
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
	 }
	 return(ret);
      }

   public:
    
      internal_dq(Boolean head = true) :  _rep(NULL), _isHead(head), _count(0)
      { 
	 _next = this; 
	 _prev = this; 
	 _cur = this;
      }

      virtual ~internal_dq() 
      {  
	 empty_list(); 
      }

      virtual void insert_first(void *element)
      {
	 internal_dq *ins = new internal_dq(false);
	 ins->_rep = element;
	 ins->insert_first(*this); 
      }

      virtual void insert_last(void *element)
      {
	 internal_dq *ins = new internal_dq(false);
	 ins->_rep = element;
	 ins->insert_last(*this);
      }
      
      virtual void empty_list( void )
      {
	 while( _count > 0 ) {
	    internal_dq *temp = _next;
	    temp->unlink();
	    if(temp->_rep != NULL)
	       ::operator delete(temp->_rep);
	    delete temp;
	    _count--;
	 }
	 return;
      }

      inline virtual void *remove_first ( void ) { return(remove(PEG_DQUEUE_FIRST) );}
      inline virtual void *remove_last ( void ) { return(remove(PEG_DQUEUE_LAST) );}

      virtual void *remove(void *key)
      {
	 void *ret = NULL;
	 if(_count > 0)
	 {
	    internal_dq *temp = _next;
	    while ( temp->_isHead == false ) {
	       if( temp->_rep == key ) {
		  temp->unlink();
		  ret = temp->_rep;
		  temp->_rep = NULL;
		  delete temp;
		  _count--;
		  break;
	       }
	       temp = temp->_next;
	    }
	 }
	 return(ret); 
      }

      virtual void *reference(void *key)
      {
	 if( _count > 0 ) {
	    internal_dq *temp = _next;
	    while(temp->_isHead == false ) {
	       if(key == temp->_rep)
		  return(temp->_rep);
	       temp = temp->_next;
	    }
	 }
	 return(NULL);
      }

      virtual inline void *next( void * ref)
      {
	 if( ref == NULL)
	    _cur = _next;
	 else {
	    _cur = _cur->_next;
	 }
	 return(_cur->_rep);
      }

      virtual inline void *prev( void * ref)
      {
	 if( ref == NULL)
	    _cur = _prev;
	 else {
	    _cur = _cur->_prev;
	 }
	 return(_cur->_rep);
      }

      virtual Boolean exists(void *key) 
      {
	 Boolean ret = false;
	 if( _count > 0) {
	    internal_dq *temp = _next;
	    while(temp->_isHead == false ) 
	    {
	       if( temp->_rep == key ) 
	       {
		  ret = true;
		  break;
	       }
	       temp = temp->_next;
	    }
	 }
	 return(ret);
      }
      inline virtual int count(void) { return _count ; }
} ;

PEGASUS_NAMESPACE_END

#endif
