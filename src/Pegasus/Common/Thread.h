//%///////////-*-c++-*-//////////////////////////////////////////////////////
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
// Modified By: Markus Mueller
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_Thread_h
#define Pegasus_Thread_h
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/DQueue.h>

// REVIEW: Spend time getting to know this.

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE cleanup_handler
{

   public:
      cleanup_handler( void (*routine)(void *), void *arg  ) : _routine(routine), _arg(arg)  {}
      ~cleanup_handler()  {; }
      inline Boolean operator==(const void *key) const
      { 
	 if(key == (void *)_routine) 
	    return true; 
	 return false; 
      }
      inline Boolean operator ==(const cleanup_handler & b) const 
      {
	 return(operator==((const void *)b._routine));
      }
   private:
      void execute(void) { _routine(_arg); } 
      cleanup_handler();
      void (*_routine)(void *);

      void *_arg; 
      PEGASUS_CLEANUP_HANDLE _cleanup_buffer;
      friend class DQueue<class cleanup_handler>;
      friend class Thread;
};

///////////////////////////////////////////////////////////////////////////////


class  PEGASUS_COMMON_LINKAGE thread_data
{

   public:
      static void default_delete(void *data);
      
      thread_data( const Sint8 *key ) : _delete_func(NULL) , _data(NULL), _size(0)
      {
	 PEGASUS_ASSERT(key != NULL);
	 size_t keysize = strlen(key);
	 _key = new Sint8 [keysize + 1];
	 memcpy(_key, key, keysize);
	 _key[keysize] = 0x00;
	 
      }
  
      thread_data(const Sint8 *key, size_t size) : _delete_func(default_delete), _size(size)
      {
	 PEGASUS_ASSERT(key != NULL);
	 size_t keysize = strlen(key);
	 _key = new Sint8 [keysize + 1];
	 memcpy(_key, key, keysize);
	 _key[keysize] = 0x00;
	 _data = ::operator new(_size) ;

      }

      thread_data(const Sint8 *key, size_t size, void *data) : _delete_func(default_delete), _size(size)
      {
	 PEGASUS_ASSERT(key != NULL);
	 PEGASUS_ASSERT(data != NULL);
	 size_t keysize = strlen(key);

	 _key = new Sint8[keysize + 1];
	 memcpy(_key, key, keysize);
	 _key[keysize] = 0x00;
	 _data = ::operator new(_size);
	 memcpy(_data, data, size);
      }

      ~thread_data() 
      { 
	 if( _data != NULL) 
	    if(_delete_func != NULL)
            {
	       _delete_func( _data ); 
            }
	 if( _key != NULL )
	    delete [] _key;
      }  

      void put_data(void (*del)(void *), size_t size, void *data ) throw(NullPointer)
      {
	 if(_data != NULL)
	    if(_delete_func != NULL)
	       _delete_func(_data);

	 _delete_func = del;
	 _data = data;
	 _size = size;
	 return ;
      }

      size_t get_size(void) { return _size; }

      void get_data(void **data, size_t *size) 
      {  
	 if(data == NULL || size == NULL)
	    throw NullPointer();
	 
	 *data = _data;
	 *size = _size;
	 return;
	 
      }

      void copy_data(void **buf, size_t *size) throw(BufferTooSmall, NullPointer)
      {
	 if((buf == NULL) || (size == NULL)) 
	    throw NullPointer() ; 
	 *buf = ::operator new(_size);
	 *size = _size;
	 memcpy(*buf, _data, _size);
	 return;
      }
      
      inline Boolean operator==(const void *key) const 
      { 
	 if ( ! strcmp(_key, (Sint8 *)key)) 
	    return(true); 
	 return(false);
      } 

      inline Boolean operator==(const thread_data& b) const
      {
	 return(operator==((const void *)b._key));
      }

   private:
      void (*_delete_func) (void *data) ;
      thread_data();
      void *_data;
      size_t _size;
      Sint8 *_key;

      friend class DQueue<thread_data>;
      friend class Thread;
};


///////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE ThreadPool;

class PEGASUS_COMMON_LINKAGE Thread
{

   public:
      Thread( PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *start )(void *),
	      void *parameter, Boolean detached );

      ~Thread();

      void run(void);

      // get the user parameter 
      inline void *get_parm(void) { return _thread_parm; }

      // send the thread a signal -- may not be appropriate due to Windows 
      //  void kill(int signum); 

      // cancellation must be deferred (not asynchronous)
      // for user-level threads the thread itself can decide
      // when it should die. 
      void cancel(void);

      // cancel if there is a pending cancellation request
      void test_cancel(void);

      Boolean is_cancelled(void);
      
      // for user-level threads  - put the calling thread
      // to sleep and jump to the thread scheduler. 
      // platforms with preemptive scheduling and native threads 
      // can define this to be a no-op. 
      // platforms without preemptive scheduling like NetWare 
      // or gnu portable threads will have an existing 
      // routine that can be mapped to this method 

      void thread_switch(void);

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
      // suspend this thread 
      void suspend(void) ;

      // resume this thread
      void resume(void) ;
#endif

      static void sleep(Uint32 msec) ;

      // block the calling thread until this thread terminates
      void join( void );
      void thread_init(void);

      // thread routine needs to call this function when
      // it is ready to exit
      void exit_self(PEGASUS_THREAD_RETURN return_code) ;

      // stack of functions to be called when thread terminates
      // will be called last in first out (LIFO)
      void cleanup_push( void (*routine) (void *), void *parm ) throw(IPCException);
      void cleanup_pop(Boolean execute = true) throw(IPCException);

      // create and initialize a tsd
      inline void create_tsd(const Sint8 *key, int size, void *buffer) throw(IPCException)
      {
	 thread_data *tsd = new thread_data(key, size, buffer);
	 try { _tsd.insert_first(tsd); }
	 catch(IPCException& e) { e = e; delete tsd; throw; }
      }

      // get the buffer associated with the key
      // NOTE: this call leaves the tsd LOCKED !!!! 
      inline void *reference_tsd(const Sint8 *key) throw(IPCException)
      {
	 _tsd.lock(); 
	 thread_data *tsd = _tsd.reference((const void *)key);
	 if(tsd != NULL)
	    return( (void *)(tsd->_data) );
	 else
	    return(NULL);
      }

      inline void *try_reference_tsd(const Sint8 *key) throw(IPCException)
      {
	 _tsd.try_lock();
	 thread_data *tsd = _tsd.reference((const void *)key);
	 if(tsd != NULL)
	    return((void *)(tsd->_data) );
	 else
	    return(NULL);
      }
      

      // release the lock held on the tsd
      // NOTE: assumes a corresponding and prior call to reference_tsd() !!!
      inline void dereference_tsd(void) throw(IPCException)
      {
	 _tsd.unlock();
      }

      // delete the tsd associated with the key
      inline void delete_tsd(const Sint8 *key) throw(IPCException)
      {
	 thread_data *tsd = _tsd.remove((const void *)key);
	 if(tsd != NULL)
	    delete tsd;
      }

      inline void *remove_tsd(const Sint8 *key) throw(IPCException)
      {
	 return(_tsd.remove((const void *)key));
      }
      
      inline void empty_tsd(void) throw(IPCException)
      {
	 _tsd.empty_list();
      }
      
      // create or re-initialize tsd associated with the key
      // if the tsd already exists, return the existing buffer
      thread_data *put_tsd(const Sint8 *key, void (*delete_func)(void *), Uint32 size, void *value) 
	 throw(IPCException)

      {
	 PEGASUS_ASSERT(key != NULL);
	 thread_data *tsd ;
	 tsd = _tsd.remove((const void *)key);  // may throw an IPC exception 
	 thread_data *ntsd = new thread_data(key);
	 ntsd->put_data(delete_func, size, value);
	 try { _tsd.insert_first(ntsd); }
	 catch(IPCException& e) { e = e; delete ntsd; throw; }
	 return(tsd);
      }
      inline PEGASUS_THREAD_RETURN get_exit(void) { return _exit_code; }
      inline PEGASUS_THREAD_TYPE self(void) {return pegasus_thread_self(); }

      PEGASUS_THREAD_HANDLE getThreadHandle() {return _handle;}

      inline Boolean operator==(const void *key) const 
      { 
	 if ( (void *)this == key) 
	    return(true); 
	 return(false);
      } 
      inline Boolean operator==(const Thread & b) const
      {
	 return(operator==((const void *)&b ));
      }

      void detach(void);
  
   private:
      Thread();
      inline void create_tsd(const Sint8 *key ) throw(IPCException)
      {
	 thread_data *tsd = new thread_data(key);
	 try { _tsd.insert_first(tsd); }
	 catch(IPCException& e) { e = e; delete tsd; throw; }
      }
      PEGASUS_THREAD_HANDLE _handle;
      Boolean _is_detached;
      Boolean _cancel_enabled;
      Boolean _cancelled; 
  
      PEGASUS_SEM_HANDLE _suspend_count;

      // always pass this * as the void * parameter to the thread
      // store the user parameter in _thread_parm 

      PEGASUS_THREAD_RETURN  ( PEGASUS_THREAD_CDECL *_start)(void *) ;
      DQueue<class cleanup_handler> _cleanup;
      DQueue<class thread_data> _tsd;

      void *_thread_parm;
      PEGASUS_THREAD_RETURN _exit_code;
      static Boolean _signals_blocked;
      friend class ThreadPool;
} ;


class PEGASUS_COMMON_LINKAGE ThreadPool
{
   public:

      ThreadPool(Sint16 initial_size,
		 const Sint8 *key,
		 Sint16 min,
		 Sint16 max,
		 struct timeval & alloc_wait,
		 struct timeval & dealloc_wait, 
		 struct timeval & deadlock_detect);
      
      ~ThreadPool(void);

      void allocate_and_awaken(void *parm, 
			       PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *work)(void *), 
			       Semaphore *blocking = 0)
	 throw(IPCException);
      

      Uint32 kill_dead_threads( void ) 
	 throw(IPCException);
      
      void get_key(Sint8 *buf, int bufsize);

      inline Boolean operator==(const void *key) const 
      { 
	 if ( ! strncmp( reinterpret_cast<Sint8 *>(const_cast<void *>(key)), _key, 16  )) 
	    return(true); 
	 return(false);
      } 
      inline Boolean operator==(const ThreadPool & b) const
      {
	 return(operator==((const void *) b._key ));
      }

      inline void set_min_threads(Sint16 min)
      {
	 _min_threads = min;
      }
      
      inline Sint16 get_min_threads(void) const
      {
	 return _min_threads;
      }

      inline void set_max_threads(Sint16 max)
      {
	 _max_threads = max;
      }
      
      inline Sint16 get_max_threads(void) const
      {
	 return _max_threads;
      }
      
      inline void set_allocate_wait(const struct timeval & alloc_wait)
      {
	 _allocate_wait.tv_sec = alloc_wait.tv_sec;
	 _allocate_wait.tv_usec = alloc_wait.tv_usec;
      }
      
      inline struct timeval *get_allocate_wait(struct timeval *buffer) const
      {
	 if(buffer == 0)
	    throw NullPointer();
	 buffer->tv_sec = _allocate_wait.tv_sec;
	 buffer->tv_usec = _allocate_wait.tv_usec;
	 return buffer;
      }

      inline void set_deallocate_wait(const struct timeval & dealloc_wait)
      {
	 _deallocate_wait.tv_sec = dealloc_wait.tv_sec;
	 _deallocate_wait.tv_usec = dealloc_wait.tv_usec;
      }
      
      inline struct timeval *get_deallocate_wait(struct timeval *buffer) const
      {
	 if(buffer == 0)
	    throw NullPointer();
	 buffer->tv_sec = _deallocate_wait.tv_sec;
	 buffer->tv_usec = _deallocate_wait.tv_usec;
	 return buffer;
      }

      inline void set_deadlock_detect(const struct timeval & deadlock)
      {
	 _deadlock_detect.tv_sec = deadlock.tv_sec;
	 _deadlock_detect.tv_usec = deadlock.tv_usec;
      }
      
      inline struct timeval * get_deadlock_detect(struct timeval *buffer) const
      {
	 if(buffer == 0)
	    throw NullPointer();
	 buffer->tv_sec = _deadlock_detect.tv_sec;
	 buffer->tv_usec = _deadlock_detect.tv_usec;
	 return buffer;
      }

      inline Uint32 running_count(void)
      {
	 return _running.count();
      }
      
      static Boolean check_time(struct timeval *start, struct timeval *interval);

   private:
      ThreadPool(void);
      Sint16 _max_threads;
      Sint16 _min_threads;
      AtomicInt _current_threads;
      struct timeval _allocate_wait;
      struct timeval _deallocate_wait;
      struct timeval _deadlock_detect;
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _loop(void *);
      Sint8 _key[17];
      DQueue<Thread> _pool;
      DQueue<Thread> _running;
      DQueue<Thread> _dead;
      AtomicInt _dying;
      

      static void _sleep_sem_del(void *p);
      
      void _check_deadlock(struct timeval *start) throw(Deadlock);
      Boolean _check_deadlock_no_throw(struct timeval *start);
      Boolean _check_dealloc(struct timeval *start);
      Thread *_init_thread(void) throw(IPCException);
      void _link_pool(Thread *th) throw(IPCException);
      static PEGASUS_THREAD_RETURN  _undertaker(void *);
      
 };


inline void ThreadPool::_sleep_sem_del(void *p)
{
   if(p != 0)
   {
      delete (Semaphore *)p;
   }
}

inline void ThreadPool::_check_deadlock(struct timeval *start) throw(Deadlock)
{
   if (true == check_time(start, &_deadlock_detect))
      throw Deadlock(pegasus_thread_self());
   return;
}


inline Boolean ThreadPool::_check_deadlock_no_throw(struct timeval *start)
{
   return(check_time(start, &_deadlock_detect));
}

inline Boolean ThreadPool::_check_dealloc(struct timeval *start)
{
   return(check_time(start, &_deallocate_wait));
}

inline Thread *ThreadPool::_init_thread(void) throw(IPCException)
{
   Thread *th = (Thread *) new Thread(_loop, this, false);
   // allocate a sleep semaphore and pass it in the thread context
   // initial count is zero, loop function will sleep until
   // we signal the semaphore
   Semaphore *sleep_sem = (Semaphore *) new Semaphore(0);
   th->put_tsd("sleep sem", &_sleep_sem_del, sizeof(Semaphore), (void *)sleep_sem);
   
   struct timeval *dldt = (struct timeval *) ::operator new(sizeof(struct timeval));
   th->put_tsd("deadlock timer", thread_data::default_delete, sizeof(struct timeval), (void *)dldt);
   // thread will enter _loop(void *) and sleep on sleep_sem until we signal it
   th->run();
   _current_threads++;
   pegasus_yield();
   
   return th;
}

inline void ThreadPool::_link_pool(Thread *th) throw(IPCException)
{
   if(th == 0)
      throw NullPointer();
   _pool.insert_first(th);
}


#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "ThreadWindows_inline.h"
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# include "ThreadzOS_inline.h"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "ThreadUnix_inline.h"
#endif

PEGASUS_NAMESPACE_END

#endif // Pegasus_Thread_h
