//%/////-*-c++-*-////   /////////////////////////////////////////////////////
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

#include <Pegasus/Common/internal_dq.h>
#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

template<class L> class PEGASUS_EXPORT DQueue : private internal_dq  {
   private: 
      Mutex _mutex;

   public:
      typedef internal_dq  Base;
      
      
      DQueue(Boolean head = true ) 
	 :  Base(head), _mutex()
      { 
      }

      virtual ~DQueue() {  }

      virtual void insert_first(L *element) throw(IPCException) 
      {
	 _mutex.lock(pegasus_thread_self());
	 Base::insert_first(static_cast<void *>(element));
	 _mutex.unlock();
      }

      virtual void insert_last(L *element) throw(IPCException)
      {
	 _mutex.lock(pegasus_thread_self());
	 Base::insert_last(static_cast<void *>(element));
	 _mutex.unlock();
      }
      
      virtual void empty_list( void ) throw(IPCException) 
      {
	 if( Base::count() > 0) {
	    _mutex.lock(pegasus_thread_self()); 
	    Base::empty_list();
	    _mutex.unlock();
	 }
	 return;
      }

      virtual void *remove_first ( void ) throw(IPCException) 
      { 
	 _mutex.lock(pegasus_thread_self());
	 void *ret = Base::remove_first();
	 _mutex.unlock();
	 return ret;
      }

      virtual void *remove_last ( void ) throw(IPCException) 
      { 
	 _mutex.lock(pegasus_thread_self());
	 void *ret = Base::remove_last();
	 _mutex.unlock();
	 return ret;
      }
      
      virtual void *remove_no_lock(void *key) throw(IPCException)
      {
	 if( pegasus_thread_self() != _mutex.get_owner())
	    throw(Permission(pegasus_thread_self()));
	 L *ret = static_cast<L *>(Base::next(0));
	 while(ret != 0)
	 {
	    if(ret->operator==(key))
	       return(Base::remove(static_cast<void *>(ret)));
	    ret = static_cast<L *>(Base::next(static_cast<void *>(ret)));
	 }
	 return 0;
      }

      void *remove(void *key) throw(IPCException)
      {
	 void *ret = 0;
	 if( Base::count() > 0 ) {
	    _mutex.lock(pegasus_thread_self());
	    ret = remove_no_lock(key);
	    _mutex.unlock() ;
	 }
	 return(ret);
      }

      void *reference(void *key) throw(IPCException)
      {
	 if( pegasus_thread_self() != _mutex.get_owner())
	    throw(Permission(pegasus_thread_self()));
	 if(Base::count() > 0 ) {
	    L *ret = static_cast<L *>(Base::next(0));
	    while(ret != 0)
	    {
	       if(ret->operator==(key))
		  return static_cast<void *>(ret);
	       ret = static_cast<L *>(Base::next(static_cast<void *>(ret)));
	    }
	 }
	 return(0);
      }

      void *next( void * ref) throw(IPCException)
      {
	 if (_mutex.get_owner() != pegasus_thread_self())
	    throw(Permission(pegasus_thread_self()));
	 return  Base::next( ref);
      }
      
      void *prev( void *ref) throw(IPCException)
      {
	 if (_mutex.get_owner() != pegasus_thread_self())
	    throw(Permission(pegasus_thread_self()));
	 return  Base::prev(ref) ;
	 
      }

      inline virtual void lock(void) throw(IPCException) { _mutex.lock(pegasus_thread_self()); }
      inline virtual void unlock(void) throw(IPCException) { _mutex.unlock() ; }
      inline virtual void try_lock(void) throw(IPCException) {  _mutex.try_lock(pegasus_thread_self()); }
      
      Boolean exists(void *key) throw(IPCException) 
      {
	 Boolean ret = false;
	 if(Base::count() > 0)
	 {
	    _mutex.lock(pegasus_thread_self());
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
	    _mutex.unlock();
	 }
	 return(ret);
      }
      inline virtual int count(void) { return Base::count() ; }
} ;

PEGASUS_NAMESPACE_END

#endif /* Pegasus_DQueue_h */

