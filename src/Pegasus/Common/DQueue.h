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

template<class L> class PEGASUS_COMMON_LINKAGE DQueue : public internal_dq
{
   public:
      static void * operator new(size_t size);
      static void operator delete(void *, size_t);
            
   private: 
      Mutex *_mutex;
      AtomicInt *_actual_count;
      DQueue *_dq_next;
      static DQueue<L> *_headOfFreeList;
      static const int BLOCK_SIZE;
      static Mutex _alloc_mut;
      
   public:
      typedef internal_dq Base;
      DQueue(void);
      DQueue(Boolean head) ;

      virtual ~DQueue() ;

      void lock(void) throw(IPCException);
      void unlock(void) throw(IPCException);
      void try_lock() throw(IPCException);

      void insert_first_no_lock(L *element) throw(IPCException);
      void insert_first(L *element) throw(IPCException) ;
      void insert_last_no_lock(L *element) throw(IPCException);
      void insert_last(L *element) throw(IPCException);
      void empty_list( void ) throw(IPCException) ;
      L *remove_first ( void ) throw(IPCException) ;
      L *remove_last ( void ) throw(IPCException) ;
      L *remove_no_lock(const void *key) throw(IPCException);
      L *remove_no_lock(const L *key) throw(IPCException);
      L *remove(const void *key) throw(IPCException);
      L *remove(const L *key) throw(IPCException);
      L *reference(const void *key) throw(IPCException);
      L *reference(const L *key);
      L *next( const void * ref) throw(IPCException);
      L *prev( const void *ref) throw(IPCException);
      Boolean exists(const void *key) throw(IPCException) ;
      Boolean exists(const L *key) throw(IPCException);
      
      Uint32 count(void) { return _actual_count->value() ; }
} ;

 

template<class L> class PEGASUS_COMMON_LINKAGE AsyncDQueue: public internal_dq
{

   public:
      static void * operator new(size_t size);
      static void operator delete(void *, size_t);
      
   private: // asyncdqueue

      Mutex *_cond;
      Condition *_slot;
      Condition *_node;
      AtomicInt *_actual_count;
      AtomicInt *_disallow;
      AtomicInt * _capacity;
      AsyncDQueue *_dq_next;

      static AsyncDQueue *_headOfFreeList;
      static const int BLOCK_SIZE;
      static Mutex _alloc_mut;
      
      void _insert_prep(void) throw(IPCException);
      void _insert_recover(void) throw(IPCException);
      void _unlink_prep(void) throw(IPCException);
      void _unlink_recover(void) throw(IPCException);
      L *_remove_no_lock(const void *key) throw(IPCException);
      L *_remove_no_lock(const L *key) throw(IPCException);

   public:

      typedef internal_dq Base;
      
      AsyncDQueue(void) ;
      AsyncDQueue(Boolean head, Uint32 capacity );
      virtual ~AsyncDQueue(void);
      void shutdown_queue(void);
      Boolean is_full(void);
      Boolean is_empty(void);
      Boolean is_shutdown(void);
      void try_lock(PEGASUS_THREAD_TYPE myself) throw(IPCException);
      void lock(PEGASUS_THREAD_TYPE myself) throw(IPCException);
      void unlock(void);
      void wait_for_node(void) throw(IPCException);
      void set_capacity(Uint32 capacity) throw(IPCException);
      Uint32 get_capacity(void);
      void insert_first(L *element) throw(IPCException);
      void insert_first_wait(L *element) throw(IPCException);
      void insert_last(L *element) throw(IPCException);
      void insert_last_wait(L *element) throw(IPCException);
      void empty_list(void) throw(IPCException);
      L *remove_first(void) throw(IPCException);
      L *remove_first_wait(void) throw(IPCException);
      L *remove_last(void) throw(IPCException);
      L *remove_last_wait(void) throw(IPCException);
      L *remove(const void *key) throw(IPCException);
      L *remove(const L *key) throw(IPCException);
      L *remove_no_lock(const void *key) throw(IPCException);
      L *remove_no_lock(const L *key) throw(IPCException);
      L *remove_wait(const void *key) throw(IPCException);
      L *next(const L *ref) throw(IPCException);
      L *prev(const L *ref) throw(IPCException);
      L *reference(const void *key) throw(IPCException);
      L *reference(const L *key) throw(IPCException);
      Uint32 count(void) ;
};
      



PEGASUS_NAMESPACE_END

#endif /* Pegasus_DQueue_h */

