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

#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

template<class L> class PEGASUS_EXPORT DQueue : private internal_dq  
{
   private: 

      Mutex *_mutex;

   public:
      typedef internal_dq  Base;
      DQueue(void) : Base(false)
      {
	 _mutex = 0;
      }
      
      DQueue(Boolean head) 
	 :  Base(head)
      {
	 if(head == true)
	    _mutex = new Mutex();
	 else
	    _mutex = 0;
      }

      virtual ~DQueue() { if(_mutex != 0) delete _mutex; }

      virtual void insert_first(L *element) throw(IPCException) 
      {
	 _mutex->lock(pegasus_thread_self());
	 Base::insert_first(static_cast<void *>(element));
	 _mutex->unlock();
      }

      virtual void insert_last(L *element) throw(IPCException)
      {
	 _mutex->lock(pegasus_thread_self());
	 Base::insert_last(static_cast<void *>(element));
	 _mutex->unlock();
      }
      
      virtual void empty_list( void ) throw(IPCException) 
      {
	 if( Base::count() > 0) {
	    _mutex->lock(pegasus_thread_self()); 
	    Base::empty_list();
	    _mutex->unlock();
	 }
	 return;
      }

      virtual L *remove_first ( void ) throw(IPCException) 
      { 
	 _mutex->lock(pegasus_thread_self());
	 void *ret = Base::remove_first();
	 _mutex->unlock();
	 return static_cast<L *>(ret);
      }

      virtual L *remove_last ( void ) throw(IPCException) 
      { 
	 _mutex->lock(pegasus_thread_self());
	 void *ret = Base::remove_last();
	 _mutex->unlock();
	 return static_cast<L *>(ret);
      }
      
      virtual L *remove_no_lock(void *key) throw(IPCException)
      {
	 if( pegasus_thread_self() != _mutex->get_owner())
	    throw(Permission(pegasus_thread_self()));
	 L *ret = static_cast<L *>(Base::next(0));
	 while(ret != 0)
	 {
	    if(ret->operator==(key))
	       return static_cast<L *>(Base::remove(static_cast<void *>(ret)));
	    ret = static_cast<L *>(Base::next(static_cast<void *>(ret)));
	 }
	 return 0;
      }

      virtual L *remove(void *key) throw(IPCException)
      {
	 L *ret = 0;
	 if( Base::count() > 0 ) {
	    _mutex->lock(pegasus_thread_self());
	    ret = remove_no_lock(key);
	    _mutex->unlock() ;
	 }
	 return(ret);
      }

      virtual L *reference(void *key) throw(IPCException)
      {
	 if( pegasus_thread_self() != _mutex->get_owner())
	    throw(Permission(pegasus_thread_self()));
	 if(Base::count() > 0 ) {
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

      virtual L *next( void * ref) throw(IPCException)
      {
	 if (_mutex->get_owner() != pegasus_thread_self())
	    throw(Permission(pegasus_thread_self()));
	 return static_cast<L *>(Base::next( ref));
      }
      
      virtual L *prev( void *ref) throw(IPCException)
      {
	 if (_mutex->get_owner() != pegasus_thread_self())
	    throw(Permission(pegasus_thread_self()));
	 return  static_cast<L *>(Base::prev(ref));
	 
      }

      inline virtual void lock(void) throw(IPCException) { _mutex->lock(pegasus_thread_self()); }
      inline virtual void unlock(void) throw(IPCException) { _mutex->unlock() ; }
      inline virtual void try_lock(void) throw(IPCException) {  _mutex->try_lock(pegasus_thread_self()); }
      
      virtual Boolean exists(void *key) throw(IPCException) 
      {
	 Boolean ret = false;
	 if(Base::count() > 0)
	 {
	    _mutex->lock(pegasus_thread_self());
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
	    _mutex->unlock();
	 }
	 return(ret);
      }
      inline virtual int count(void) { return Base::count() ; }
} ;


template<class L> class PEGASUS_EXPORT AsyncDQueue: private DQueue< L > 
{
   private:

      Semaphore *_enq_sem;
      Semaphore *_dq_sem;
      AtomicInt *_actual_count;
      Uint32 _capacity;


   public:

      typedef DQueue< L > Base;
      AsyncDQueue(void) 
	 : Base(false),  _capacity(0)
      {
	 _enq_sem = 0;
	 _dq_sem = 0;
	 _actual_count = 0;
      }
      
      AsyncDQueue(Boolean head, Uint32 capacity )
	 : Base(head), _capacity(capacity)
      {
	 if(head == true)
	 {
	    _enq_sem = new Semaphore(0);
	    _dq_sem = new Semaphore(0);
	    _actual_count = new AtomicInt(0);
	 }
	 else
	 {
	    _enq_sem = 0;
	    _dq_sem= 0;
	    _actual_count = 0;
	 }
      }

      virtual ~AsyncDQueue(void)
      {
	 delete _enq_sem;
	 delete _dq_sem;
	 delete _actual_count;
      }
      
      inline virtual Boolean is_full(void)
      {
	 if(_actual_count->value() >= _capacity)
	    return true;
	 return false;
      }
      
      inline virtual Boolean is_empty(void)
      {
	 if(_actual_count->value() > 0)
	    return true;
	 return false;
      }
      
      inline virtual void wait_slot(void) throw(IPCException)
      {
	 _dq_sem->wait();
      }
      
      inline virtual void try_wait_slot(void) throw(IPCException)
      {
	 _dq_sem->try_wait();
      }

      inline virtual void time_wait_slot(Uint32 ms) throw(IPCException)
      {
	 _dq_sem->time_wait(ms);
      }
      
      inline virtual void wait_node(void) throw(IPCException)
      {
	 _enq_sem->wait();
      }
      
      inline virtual void try_wait_node(void) throw(IPCException)
      {
	 _enq_sem->try_wait();
      }
      
      inline virtual void time_wait_node(Uint32 ms) throw(IPCException)
      {
	 _enq_sem->try_wait();
      }
      
      virtual void insert_first(L *element) throw(IPCException, ListFull)
      {
	 
	 if(true == is_full())
	    throw ListFull(_capacity);
	 Base::insert_first(element);
	 (*_actual_count)++;
	 // signal threads that may be waiting for a node insertion
	 _enq_sem->signal();
	 return;
      }

      virtual void insert_last(L *element) throw(IPCException, ListFull)
      {
	 if(true == is_full())
	    throw ListFull(_capacity);
	 Base::insert_last(element);
	 (*_actual_count)++;
	 // signal threads that may be waiting for a node insertion
	 _enq_sem->signal();
	 return;
      }
      
      virtual void empty_list(void) throw(IPCException)
      {
	 Uint32 removed = _actual_count->value();
	 Base::empty_list();
	 (*_actual_count) = 0; 
	 while(removed)
	 {
	    _dq_sem->signal();
	    removed--;
	 }
      }
      
      virtual L *remove_first(void) throw(IPCException)
      {
	 L *ret = Base::remove_first();
	 if(ret != 0)
	 {
	    (*_actual_count)--;
	    _dq_sem->signal();
	 }
	 
	 return ret;
      }
      
      virtual L *remove_last(void) throw(IPCException)
      {
	 L *ret = Base::remove_last();
	 if(ret != 0)
	 {
	    (*_actual_count)--;
	    _dq_sem->signal();
	 }
	 return ret;
      }
      
      virtual L *remove_no_lock(void *key) throw(IPCException)
      {
	 L *ret = Base::remove_no_lock(key);
	 if(ret != 0)
	 {
	    (*_actual_count)--;
	    _dq_sem->signal();
	 }
	 return ret;
      }
      
      virtual L *remove(void *key) throw(IPCException)
      {
	 L *ret = Base::remove(key);
	 if(ret != 0)
	 {
	    (*_actual_count)--;
	    _dq_sem->signal();
	 }
	 return ret;
      }
};
      

PEGASUS_NAMESPACE_END

#endif /* Pegasus_DQueue_h */

