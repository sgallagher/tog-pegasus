//%//-*-c++-*-/////////////////////////////////////////////////////////////////
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#ifndef PEG_INTERNAL_DQ_include
#define PEG_INTERNAL_DQ_include

PEGASUS_NAMESPACE_BEGIN

#define PEG_DQUEUE_FIRST 0
#define PEG_DQUEUE_LAST 1


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

      inline void insert_first(void *element)
      {
	 internal_dq *ins = new internal_dq(false);
	 ins->_rep = element;
	 ins->insert_first(*this); 
      }

      inline void insert_last(void *element)
      {
	 internal_dq *ins = new internal_dq(false);
	 ins->_rep = element;
	 ins->insert_last(*this);
      }
      
      inline virtual void empty_list( void )
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

      inline void *remove_first ( void ) { return(remove(PEG_DQUEUE_FIRST) );}
      inline void *remove_last ( void ) { return(remove(PEG_DQUEUE_LAST) );}

      inline void *remove(void *key)
      {
	 void *ret = 0;
	 if(key == 0)
	    return ret;
	 
	 if(_count > 0)
	 {
	    internal_dq *temp = _next;
	    if(_cur->_rep == key)
	    {
	       temp  = _cur;
	       _cur = _cur->_next;
	    }
	    
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

      inline void *reference(void *key)
      {
	 if( _count > 0 ) {
	    internal_dq *temp = _next;
	    while(temp->_isHead == false ) {
	       if(key == temp->_rep)
	       {
		  _cur = temp;
		  return(temp->_rep);
	       }
	       
	       temp = temp->_next;
	    }
	 }
	 return(NULL);
      }

      inline void *next( void * ref)
      {
	 if( ref == NULL)
	    _cur = _next;
	 else {
	    _cur = _cur->_next;
	 }
	 return(_cur->_rep);
      }

      inline void *prev( void * ref)
      {
	 if( ref == NULL)
	    _cur = _prev;
	 else {
	    _cur = _cur->_prev;
	 }
	 return(_cur->_rep);
      }

      inline Boolean exists(void *key) 
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
      inline virtual Uint32 count(void) { return _count ; }
} ;


template<class L> class PEGASUS_EXPORT unlocked_dq : virtual public internal_dq
{
public:
      typedef internal_dq  Base;
      
      unlocked_dq(void) : Base(false) { }
      unlocked_dq(Boolean head) : Base(head) { }
      
      virtual ~unlocked_dq() {  }

      inline virtual void insert_first(L *element) 
      {
	 Base::insert_first(static_cast<void *>(element));
      }

      inline virtual void insert_last(L *element) 
      {
	 Base::insert_last(static_cast<void *>(element));
      }
      
      inline virtual void empty_list( void ) 
      {
	 Base::empty_list();
      }

      inline virtual L *remove_first ( void ) 
      { 
	 return static_cast<L *>(Base::remove_first());
      }

      inline virtual L *remove_last ( void ) 
      { 
	 return static_cast<L *>(Base::remove_last());
      }
      
      inline virtual L *remove_no_lock(void *key) 
      {
	 L *ret = static_cast<L *>(Base::next(0));
	 while(ret != 0)
	 {
	    if(ret->operator==(key))
	       return static_cast<L *>(Base::remove(static_cast<void *>(ret)));
	    ret = static_cast<L *>(Base::next(static_cast<void *>(ret)));
	 }
	 return 0;
      }

      inline virtual L *remove(void *key) 
      {
	 L *ret = 0;
	 if( count() > 0 ) 
	 {
	    ret = unlocked_dq<L>::remove_no_lock(key);
	 }
	 return(ret);
      }
      
      
      
      inline virtual L *reference(void *key) 
      {
	 if(Base::count() > 0 ) 
	 {
	    L *ret = static_cast<L *>(Base::next(0));
	    while(ret != 0)
	    {
	       if(ret->operator==(key))
		  return ret;
	       ret = static_cast<L *>(Base::next(static_cast<void *>(ret)));
	    }
	 }
	 return(0);
      }

      inline virtual L *reference(L *key)
      {
	 if(Base::count() > 0 ) 
	 {
	    L *ret = static_cast<L *>(Base::next(0));
	    while(ret != 0)
	    {
	       if(ret->operator==(key))
		  return ret;
	       ret = static_cast<L *>(Base::next(static_cast<void *>(ret)));
	    }
	 }
	 return(0);
      }

      inline virtual L *remove(L *key)
      {
	 L *ret = unlocked_dq::reference(key);
	 if(ret != 0)
	    ret =  static_cast<L *>(Base::remove(static_cast<void *>(ret)));
	 return ret;
      }

      inline virtual L *next( void * ref) 
      {
	 return static_cast<L *>(Base::next( ref));
      }
      
      inline virtual L *prev( void *ref) 
      {
	 return  static_cast<L *>(Base::prev(ref));
      }

      virtual Boolean exists(void *key) 
      {
	 Boolean ret = false;
	 if(Base::count() > 0)
	 {
	    L *found = static_cast<L *>(Base::next(0));
	    while(found != 0)
	    {
	       if(found->operator==(key) == true)
	       {
		  ret = true;
		  break;
	       }
	       found = static_cast<L *>(Base::next(static_cast<void *>(found)));
	    }
	 }
	 return(ret);
      }
      
      inline virtual Uint32 count(void) { return Base::count() ; }
};

PEGASUS_NAMESPACE_END

#endif // PEG_INTERNAL_DQ_include
