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

template<class L> class PEGASUS_EXPORT DQueue : virtual public unlocked_dq<L>
{
   private: 

      Mutex *_mutex;

   public:
      typedef internal_dq Internal;
      typedef unlocked_dq<L> Base;
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

      inline virtual void insert_first(L *element) throw(IPCException) 
      {
	 _mutex->lock(pegasus_thread_self());
	 Base::insert_first(element);
	 _mutex->unlock();
      }

      inline virtual void insert_last(L *element) throw(IPCException)
      {
	 _mutex->lock(pegasus_thread_self());
	 Base::insert_last(element);
	 _mutex->unlock();
      }
      
      inline virtual void empty_list( void ) throw(IPCException) 
      {
	 if( Internal::count() > 0) {
	    _mutex->lock(pegasus_thread_self()); 
	    empty_list();
	    _mutex->unlock();
	 }
	 return;
      }

      virtual L *remove_first ( void ) throw(IPCException) 
      { 
	 _mutex->lock(pegasus_thread_self());
	 L *ret = Base::remove_first();
	 _mutex->unlock();
	 return ret;
      }

      virtual L *remove_last ( void ) throw(IPCException) 
      { 
	 _mutex->lock(pegasus_thread_self());
	 L *ret = Base::remove_last();
	 _mutex->unlock();
	 return ret;
      }
      
      virtual L *remove_no_lock(void *key) throw(IPCException)
      {
	 if( pegasus_thread_self() != _mutex->get_owner())
	    throw Permission(pegasus_thread_self());
	 return Base::remove_no_lock(key);
      }

      virtual L *remove(void *key) throw(IPCException)
      {
	 L *ret = 0;
	 if( Internal::count() > 0 ) {
	    _mutex->lock(pegasus_thread_self());
	    ret = DQueue< L >::remove_no_lock(key);
	    _mutex->unlock() ;
	 }
	 return(ret);
      }

      virtual L *reference(void *key) throw(IPCException)
      {
	 if( pegasus_thread_self() != _mutex->get_owner())
	    throw Permission(pegasus_thread_self());
	 return Base::reference(key);
      }

      inline virtual L *reference(L *key)
      {
	 if( pegasus_thread_self() != _mutex->get_owner())
	    throw Permission(pegasus_thread_self());
	 return Base::reference(key);
      }

      inline virtual L *remove(L *key)
      {
	 L *ret = 0;
	 if( Internal::count() > 0 ) {
	    _mutex->lock(pegasus_thread_self());
	    ret = Base::remove(key);
	    _mutex->unlock() ;
	 }
	 return ret;
      }

      virtual L *next( void * ref) throw(IPCException)
      {
	 if (_mutex->get_owner() != pegasus_thread_self())
	    throw Permission(pegasus_thread_self()) ;
	 return Base::next( ref );
      }
      
      virtual L *prev( void *ref) throw(IPCException)
      {
	 if (_mutex->get_owner() != pegasus_thread_self())
	    throw Permission(pegasus_thread_self());
	 return  Base::prev( ref );
	 
      }

      inline virtual void lock(void) throw(IPCException) { _mutex->lock(pegasus_thread_self()); }
      inline virtual void unlock(void) throw(IPCException) { _mutex->unlock() ; }
      inline virtual void try_lock(void) throw(IPCException) {  _mutex->try_lock(pegasus_thread_self()); }
      
      virtual Boolean exists(void *key) throw(IPCException) 
      {
	 Boolean ret = false;
	 if(Internal::count() > 0)
	 {
	    _mutex->lock(pegasus_thread_self());
	    ret = Base::exists(key);
	    _mutex->unlock();
	 }
	 return(ret);
      }
      inline virtual Uint32 count(void) { return Internal::count() ; }
} ;


template<class L> class PEGASUS_EXPORT AsyncDQueue: virtual public internal_dq
{
   private: // asyncdqueue

      Mutex *_cond;
      Condition *_slot;
      Condition *_node;
      AtomicInt *_actual_count;
      AtomicInt *_disallow;
      AtomicInt * _capacity;

      inline void _insert_prep(void) throw(IPCException)
      {
	 lock(pegasus_thread_self());
	 while(true == is_full())
	 {
	    if(_disallow->value() > 0)
	    {
	       unlock();
	       throw ListClosed();
	    }	    
	    _slot->unlocked_wait(pegasus_thread_self());
	 }
      }
      
      inline void _insert_recover(void) throw(IPCException)
      {
	 _node->unlocked_signal(pegasus_thread_self());
	 (*_actual_count)++;
	 unlock();
      }
      
      inline void _unlink_prep(void) throw(IPCException)
      {
	 lock(pegasus_thread_self());
	 while(true == is_empty())
	 {
	    if(_disallow->value() > 0)
	    {
	       unlock();
	       throw ListClosed();
	    }
	    _node->unlocked_wait(pegasus_thread_self());
	 }
      }
      
      inline void _unlink_recover(void) throw(IPCException)
      {
	 _slot->unlocked_signal(pegasus_thread_self());
	 (*_actual_count)--;
	 unlock();
      }
      
      inline L *_remove_no_lock(void *key) throw(IPCException)
      {
	 if(_disallow->value() > 0)
	 {
	    unlock();	    
	    throw ListClosed();
	 }
	 if( pegasus_thread_self() != _cond->get_owner())
	    throw Permission(pegasus_thread_self());
	 L *ret = static_cast<L *>(Base::next(0));
	 while(ret != 0)
	 {
	    if(ret->operator==((L *)key))
	       return static_cast<L *>(Base::remove(static_cast<void *>(ret)));
	    ret = static_cast<L *>(Base::next(static_cast<void *>(ret)));
	 }
	 return 0;
      }

   public:

      typedef internal_dq Base;
      
      AsyncDQueue(void) 
	 : Base(false)
      {
	 _cond = 0;
	 _slot = 0;
	 _node = 0;
	 _actual_count = 0;
	 _disallow = 0;
	 _capacity = 0;
      }
      
      AsyncDQueue(Boolean head, Uint32 capacity )
	 : Base(head)
      {
	 if(head == true)
	 {
	    _cond = new Mutex();
	    _slot = new Condition(*_cond);
	    _node = new Condition(*_cond);
	    _actual_count = new AtomicInt(0);
	    _disallow = new AtomicInt(0);
	    _capacity = new AtomicInt(capacity);
	    
	 }
	 else
	 {
	    _cond = 0;
	    _slot = 0;
	    _node = 0;
	    _actual_count = 0;
	    _disallow = 0;
	    _capacity = 0;
	 }
      }

      virtual ~AsyncDQueue(void)
      {
	 delete _cond;
	 delete _slot;
	 delete _node;
	 delete _actual_count;
	 delete _disallow;
	 delete _capacity;
      }
      
      inline void shutdown_queue(void)
      {
	 lock(pegasus_thread_self());
	 (*_disallow)++;
	 _node->unlocked_signal(pegasus_thread_self());
	 _node->disallow();
	 _slot->unlocked_signal(pegasus_thread_self());  
	 _slot->disallow();
	 unlock();
      }

      inline Boolean is_full(void)
      {
	 if(_actual_count->value() >= _capacity->value())
	    return true;
	 return false;
      }
      
      inline Boolean is_empty(void)
      {
	 if(_actual_count->value() == 0)
	    return true;
	 return false;
      }
      
      inline Boolean is_shutdown(void)
      {
	 if( _disallow->value() > 0)
	    return true;
	 return false;
      }
      
      inline void lock(PEGASUS_THREAD_TYPE myself) throw(IPCException)
      {
	 _cond->lock(myself);
      }
      
      inline void unlock(void)
      {
	 _cond->unlock();
      }

      inline void set_capacity(Uint32 capacity) throw(IPCException)
      {
	 lock(pegasus_thread_self());
	 _capacity = capacity;
	 unlock();
      }

      inline Uint32 get_capacity(void)
      {
	 return _capacity->value();
      }
      
      inline virtual void insert_first(L *element) throw(IPCException)
      {
	 lock(pegasus_thread_self());
	 if(true == is_full())
	 {
	    unlock();
	    throw ListFull(_capacity->value());
	 }
	 Base::insert_first(static_cast<void *>(element));
	 _insert_recover();
      }
      
      inline void insert_first_wait(L *element) throw(IPCException)
      {
	 _insert_prep();
	 Base::insert_first(static_cast<void *>(element));
	 _insert_recover();
      }

      inline virtual void insert_last(L *element) throw(IPCException)
      {
	 lock(pegasus_thread_self());
	 if(true == is_full())
	 {
	    unlock();
	    throw ListFull(_capacity->value());
	 }
	 Base::insert_last(static_cast<void *>(element));
	 _insert_recover();
      }
      
      inline void insert_last_wait(L *element) throw(IPCException)
      {
	 _insert_prep();
	 Base::insert_last(element);
	 _insert_recover();
      }
      
      inline virtual void empty_list(void) throw(IPCException)
      {
	 lock(pegasus_thread_self());
	 Base::empty_list();
	 (*_actual_count) = 0; 
	 _slot->unlocked_signal(pegasus_thread_self());
	 unlock();
      }
      
      inline virtual L *remove_first(void) throw(IPCException)
      {
	 lock(pegasus_thread_self());
	 L *ret = static_cast<L *>(Base::remove_first());
	 if(ret != 0)
	    (*_actual_count)--;
	 unlock();
	 return ret;
      }
      
      inline L *remove_first_wait(void) throw(IPCException)
      {
	 _unlink_prep();
	 L *ret = static_cast<L *>(Base::remove_first());
	 _unlink_recover();
	 return ret;
      }
      
      inline virtual L *remove_last(void) throw(IPCException)
      {
	 lock(pegasus_thread_self());
	 L *ret = static_cast<L *>(Base::remove_last());
	 if(ret != 0)
	    (*_actual_count)--;
	 unlock();
	 return ret;
      }

      inline L *remove_last_wait(void) throw(IPCException)
      {
	 _unlink_prep();
	 L *ret = static_cast<L *>(Base::remove_last());
	 _unlink_recover();
	 return ret;
      }

      inline virtual L *remove(void *key) throw(IPCException)
      {
	 lock(pegasus_thread_self());
	 L *ret = _remove_no_lock(key);
	 if(ret != 0)
	 {
	    (*_actual_count)--;
	    _slot->unlocked_signal(pegasus_thread_self());
	 }
	 unlock();
	 return ret;
      }

      inline L *remove_wait(void *key) throw(IPCException)
      {
	 lock(pegasus_thread_self());
	 
	 L *ret = _remove_no_lock(key);
	 while( ret == 0 )
	 {
	    _node->unlocked_wait(pegasus_thread_self());
	    ret = _remove_no_lock(key);
	 }
	 if(ret != 0)
	 {
	    (*_actual_count)--;
	    _slot->unlocked_signal(pegasus_thread_self());
	 }
	 unlock();
	 return ret;
      }
      

  inline virtual Uint32 count(void) { return _actual_count->value() ; }
};
      

PEGASUS_NAMESPACE_END

#endif /* Pegasus_DQueue_h */

