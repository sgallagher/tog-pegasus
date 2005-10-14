//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <assert.h>
#include <Pegasus/Common/AutoPtr.h>

#ifndef PEG_INTERNAL_DQ_include
#define PEG_INTERNAL_DQ_include

PEGASUS_NAMESPACE_BEGIN

#define PEG_DQUEUE_FIRST 0
#define PEG_DQUEUE_LAST 1


class PEGASUS_COMMON_LINKAGE internal_dq {
   private:
      void *_rep;
      internal_dq *_next;
      internal_dq  *_prev;
      internal_dq *_cur;
      Boolean _isHead ;
      int _count;

            
      // unlink this node from whichever list it is on
       void unlink( void  ) 
      { 
	 _prev->_next = _next; 
	 _next->_prev = _prev; 
	 _next = _prev = 0;
      }
    
       void insert_first(internal_dq & head)
      { 
	 _prev = &head; 
	 _next = head._next; 
	 head._next->_prev = this; 
	 head._next = this;   
	 head._count++; 
      }

       void insert_last(internal_dq & head) 
      {
	 _next = &head;
	 _prev = head._prev;
	 head._prev->_next = this;
	 head._prev = this;
	 head._count++;
      }

       void *remove( int code )
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
	 if (_isHead == true )
	    empty_list(); 
      }

       void insert_first(void *element)
      {
	 if(element == 0)
	    return;
	 
     AutoPtr<internal_dq> ins(new internal_dq(false));
	 ins->_rep = element;
	 ins->insert_first(*this); 
     ins.release();
      }

       void insert_last(void *element)
      {
	 if(element == 0)
	    return;

     AutoPtr<internal_dq> ins(new internal_dq(false));
	 ins->_rep = element;
	 ins->insert_last(*this);
     ins.release();
      }
      
       virtual void empty_list( void )
      {
	 if ( _isHead == true ) 
	 {
	    while( _count > 0 ) {
	       internal_dq *temp = _next;
	       temp->unlink();
	       if(temp->_rep != NULL)
		  ::operator delete(temp->_rep);
	       delete temp;
	       _count--;
	    }
	 }
	 
	 return;
      }

       void *remove_first ( void ) { return(remove(PEG_DQUEUE_FIRST) );}
       void *remove_last ( void ) { return(remove(PEG_DQUEUE_LAST) );}

       void *remove(const void *key)
      {
	 if(key == 0)
	    return 0;
	 void *ret = 0;

	 if(_count > 0)
	 {
	    internal_dq *temp = _next;
	    if(_cur->_rep == key)
	    {
	       temp  = _cur;
	       _cur = _cur->_prev;
	    }
	    
	    while ( temp->_isHead == false ) {
	       if( temp->_rep == key ) {
		  _cur = temp->_prev;
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

       void *reference(const void *key)
      {
	 if(key == 0)
	    return 0;
	 
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
	 return 0;
      }

       void *next( const void * ref)
      {
	 if( ref == 0)
	    _cur = _next;
	 else {
	    _cur = _cur->_next;
	 }
	 return(_cur->_rep);
      }

       void *prev( const void * ref)
      {
	 if( ref == 0 )
	    _cur = _prev;
	 else {
	    _cur = _cur->_prev;
	 }
	 return(_cur->_rep);
      }

       Boolean exists(const void *key) 
      {
	 if(key == 0 )
	    return false;
	 
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
       virtual Uint32 count(void) { return _count ; }
} ;




template<class L> class PEGASUS_COMMON_LINKAGE unlocked_dq
{
   private:
      L *_rep;
      unlocked_dq<L> *_next;
      unlocked_dq<L>  *_prev;
      unlocked_dq<L> *_cur;
      Boolean _isHead ;
      int _count;

      // unlink this node from whichever list it is on
       void unlink( void  ) 
      { 
	 _prev->_next = _next; 
	 _next->_prev = _prev; 
	 _next = _prev = 0;
      }
    
      void insert_first(unlocked_dq<L> & head)
      { 
	 _prev = &head; 
	 _next = head._next; 
	 head._next->_prev = this; 
	 head._next = this;   
	 (head._count)++; 
      }

      void insert_last(unlocked_dq<L> & head) 
      {
	 _next = &head;
	 _prev = head._prev;
	 head._prev->_next = this;
	 head._prev = this;
	 (head._count)++;
      }

       L *remove( int code )
      {
	 L *ret = NULL;
	
	 if( _count > 0 ) {
	    unlocked_dq<L> *temp = NULL;
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
      
      unlocked_dq() : _rep(0), _isHead(false), _count(0)
      {
	 _next = 0;
	 _prev = 0;
	 _cur = 0;
      }
      
      unlocked_dq(Boolean head ) : _rep(NULL), _isHead(head), _count(0)
      {
	 if ( _isHead == true )
	 {
	    _next = this;
	    _prev = this;
	    _cur = this;
	 }
	 
      }
            
      virtual ~unlocked_dq() 
      {  
	 if ( _isHead == true)
	    empty_list();
      }

      void insert_first(L *element) 
      {
	 if( element == 0 )
	    return;
	 unlocked_dq<L> *ins = new unlocked_dq<L>(false);
	 ins->_rep = element;
	 ins->insert_first(*this);
      }

      void insert_last(L *element) 
      {
	 if( element == 0 )
	    return;
	 unlocked_dq<L> *ins = new unlocked_dq<L>();
	 ins->_rep = element;
	 ins->insert_last(*this);
      }
      
       virtual void empty_list( void ) 
      {

	 if ( _isHead == true ) 
	 {
	    while( _count > 0 ) {
	       unlocked_dq<L> *temp = _next;
	       temp->unlink();
	       delete temp->_rep;
	       delete temp;
	       _count--;
	    }
	 }
	 
	 return;
      }

       L *remove_first ( void ) { return(remove(PEG_DQUEUE_FIRST) );}
       L *remove_last ( void ) { return(remove(PEG_DQUEUE_LAST) );}

            
       virtual L *remove(const L *key)
      {
	 if(key == 0)
	    return 0;
	 L *ret = 0;
	 
	 if(_count > 0)
	 {
	    unlocked_dq<L> *temp = _next;
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
      

       virtual L *remove(const void *key) 
      {

	 if(key == 0)
	    return 0;
	 return unlocked_dq<L>::remove(reinterpret_cast<const L *>(key));
      }
      
       virtual L *reference(const void *key) 
      {
	 if( key == 0 )
	    return 0;
	 
	 if(_count > 0 ) 
	 {
	    L *ret = next(0);
	    while(ret != 0)
	    {
	       if(ret->operator==(key))
		  return ret;
	       ret = next(ret);
	    }
	 }
	 return(0);
      }

       virtual L *reference(const L *key)
      {
	 if(key == 0)
	    return 0;
	 
	 if(_count > 0 ) 
	 {
	    L *ret = next(0);
	    while(ret != 0)
	    {
	       if(ret->operator==(key))
		  return ret;
	       ret = next(ret);
	    }
	 }
	 return(0);
      }

      
       virtual L *next(const void *ref) 
      {
	 assert(this->_isHead == true);
	 
	 if( ref == 0)
	    _cur = this->_next;
	 else {
	       _cur = _cur->_next;
	 }
	 return(_cur->_rep);
      }
      
       virtual L *prev(const void *ref) 
      {
	 assert(this->_isHead == true);
	 if( ref == 0 )
	    _cur = _prev;
	 else {
	    _cur = _cur->_prev;
	 }
	 return(_cur->_rep);
      }

      virtual Boolean exists(const void *key) 
      {
	 if(key == 0)
	    return false;
	 
	 Boolean ret = false;
	 if(_count > 0)
	 {
	    L *found = next(0);
	    while(found != 0)
	    {
	       if(found->operator==(key) == true)
	       {
		  ret = true;
		  break;
	       }
	       found = next(found);
	    }
	 }
	 return(ret);
      }
      
       virtual Uint32 count(void) { return _count ; }
};

PEGASUS_NAMESPACE_END

#endif // PEG_INTERNAL_DQ_include
