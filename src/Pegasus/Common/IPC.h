//%/////////////////////////////////////////////////////////////////////////////
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_IPC_h
#define Pegasus_IPC_h

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include "IPCWindows.h"
#elif defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
# include "IPCUnix.h"
#elif defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
# include "IPCUnix.h"
#else
# error "Unsupported platform"
#endif

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
class PEGASUS_EXPORT Mutex
{

 public:

  Mutex(void) ;
  ~Mutex(void);

  // block until gaining the lock - throw a deadlock 
  // exception if process already holds the lock 
  void lock(void);
  
  // try to gain the lock - lock succeeds immediately if the 
  // mutex is not already locked. throws an exception and returns
  // immediately if the mutex is currently locked. 
  Boolean try_lock(void);

  // wait for milliseconds and throw an exception then return if the wait
  // expires without gaining the lock. Otherwise return without throwing an
  // exception. 
  void timed_lock( Uint32 milliseconds );

  // unlock the semaphore
  void unlock(void);

  PEGASUS_MUTEX_TYPE * getMutex();

 private:
  
  PEGASUS_MUTEX_HANDLE _mutex;

} ;

///////////////////////////////////////////////////////////////////////////////

#define SEM_READ 1
#define SEM_WRITE 2

class PEGASUS_EXPORT ReadWriteSem
{
 
 public:
  ReadWriteSem(Uint32 mode = SEM_WRITE);
  ~ReadWriteSem();

  void wait(Uint32 mode);

  Boolean try_wait(Uint32 mode);

  void timed_wait(Uint32 mode, int milliseconds);

  void unlock(Uint32 mode);
  int read_count(void);
  int write_count(void);

 private: 

  PEGASUS_RWLOCK_HANDLE _rwlock;

} ;

///////////////////////////////////////////////////////////////////////////////

class PEGASUS_EXPORT Condition
{
  
 public:
    
  // create the condition variable
  Condition() ;
  ~Condition();

  // block until this condition is in a signalled state 
  void wait(void);

  // wait for milliseconds and throw an exception
  // if wait times out without being signaled
  void time_wait( Uint32 milliseconds );

  // signal the condition variable
  void signal(void);

  // lock the accompanying Mutex
  // this is useful when synchronizing thread startus
  Mutex * getMutex(void);

  // without pthread_mutex_lock/unlock
  void unlocked_wait(void);
  void unlocked_timed_wait(int milliseconds);
  void unlocked_signal(void);

 private:

  PEGASUS_COND_HANDLE  _condition;

  Mutex _cond_mutex;
};

///////////////////////////////////////////////////////////////////////////////

class PEGASUS_EXPORT Semaphore
{
  
 public:
    
  // create the semaphore and set its initial value to the <initial>
  Semaphore(Uint32 initial = 1 ) ;
  ~Semaphore( );

  // block until this semaphore is in a signalled state 
  void wait(void);

  // wait succeeds immediately if semaphore has a non-zero count, 
  // return immediately and throw and exception if the 
  // count is zero. 
  Boolean try_wait(void) ;

  // wait for milliseconds and throw an exception
  // if wait times out without gaining the semaphore
  void time_wait( Uint32 milliseconds );


  // increment the count of the semaphore 
  void signal(void);

  // return the count of the semaphore
  int count(void); 

 private:

  PEGASUS_SEM_HANDLE  _semaphore;

  // may not need to use the _count member on
  // platorms that allow you to ask the semaphore for 
  // its count 
  int _count; 
};

///////////////////////////////////////////////////////////////////////////////

class AtomicInt
{
 public:

    AtomicInt();

    AtomicInt(Uint32 initial);

    ~AtomicInt();

    Uint32  value();

    void operator++(void);

    void operator--(void);

    Uint32 operator+(AtomicInt val);

    Uint32 operator+(Uint32 val);

    Uint32 operator-(AtomicInt val);

    Uint32 operator-(Uint32 val);

    Mutex * getMutex();

 private:

    Uint32 _value;
    Mutex  _mutex;   
};
    
///////////////////////////////////////////////////////////////////////////////


class PEGASUS_EXPORT cleanup_handler
{

 public:
  cleanup_handler( void (*routine)(void *), void *arg  ) ;
  ~cleanup_handler() ;
  void execute(void) { _routine(_arg); } 
 private:
  cleanup_handler();
  PEGASUS_CLEANUP_HANDLE _cleanup_buffer;
  void (*_routine)(void *);
  void *_arg; 
};

///////////////////////////////////////////////////////////////////////////////

class PEGASUS_EXPORT SimpleThread
{

 public:
  SimpleThread( PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *start )(void *),
          void *parameter, Boolean detached );

  ~SimpleThread();

  void run(void);

  Uint32 threadId(void);

  // get the user parameter 
  void *get_parm(void);

  // cancellation must be deferred (not asynchronous)
  // for user-level threads the thread itself can decide
  // when it should die. 
  void cancel(void);

  void kill(int signum);

  // cancel if there is a pending cancellation request
  void test_cancel(void);

  // for user-level threads  - put the calling thread
  // to sleep and jump to the thread scheduler. 
  // platforms with preemptive scheduling and native threads 
  // can define this to be a no-op. 
  // platforms without preemptive scheduling like NetWare 
  // or gnu portable threads will have an existing 
  // routine that can be mapped to this method 

  void thread_switch(void);

  // suspend this thread 
  void suspend(void) ;

  // resume this thread
  void resume(void) ;

  void sleep(Uint32 msec) ;

  // block the calling thread until this thread terminates
  void join( PEGASUS_THREAD_RETURN *ret_val);


  // stack of functions to be called when thread terminates
  // will be called last in first out (LIFO)
  void cleanup_push( void (*routine) (void *), void *parm );
  void cleanup_pop(Boolean execute) ;

 private:
  SimpleThread();

  PEGASUS_THREAD_HANDLE _handle;
  Boolean _is_detached;
  Boolean _cancel_enabled;
  Boolean _cancelled; 
  
  //PEGASUS_SEM_HANDLE _suspend_count;
  Semaphore _suspend;

  // always pass this * as the void * parameter to the thread
  // store the user parameter in _thread_parm 

  PEGASUS_THREAD_RETURN  ( PEGASUS_THREAD_CDECL *_start)(void *) ;

  void *_thread_parm;
} ;

///////////////////////////////////////////////////////////////////////////////

#if 0
class  PEGASUS_EXPORT thread_data
{

 public:
  thread_data( Sint8 *key  ) : _data(NULL),  _delete(NULL) 
    {
      _key = strdup(key) ; 
    }

  thread_data( Sint8 *key, int size) :  _delete( default_delete ) 
    { 
      _ data = new char [ size ]
        _key = strdup(key) ; 
    }

  ~thread_data() { if( _data != NULL) _delete( _data ); }  
  Boolean operator ==(const Sint8 *key) const; 
  Boolean operator ==( const thread_data & b) const { 
           return ( operator ==( b.key() ) ) ; }
  //void *get_data(Sint8 *key);
  //void *put_data(Sint8 *key, void (*delete) (void *), void *data  );
 private:
  void default_delete(char *data) { delete [] data ; }
  void (*_delete) (void *) ;
  thread_data( ) ;
  void *_data;
  Sint8 *_key;
};
#endif

#if 0
class PEGASUS_EXPORT Thread
{

 public:
  Thread( PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *start )(void *),
          void *parameter, Boolean detached );

  ~Thread();

  void run(void);

  // get the user parameter 
  void *get_parm(void);

  // cancellation must be deferred (not asynchronous)
  // for user-level threads the thread itself can decide
  // when it should die. 
  void cancel(void);

  // cancel if there is a pending cancellation request
  void test_cancel(void);

  // for user-level threads  - put the calling thread
  // to sleep and jump to the thread scheduler. 
  // platforms with preemptive scheduling and native threads 
  // can define this to be a no-op. 
  // platforms without preemptive scheduling like NetWare 
  // or gnu portable threads will have an existing 
  // routine that can be mapped to this method 

  void thread_switch(void);

  // suspend this thread 
  void suspend(void) ;

  // resume this thread
  void resume(void) ;

  void sleep(Uint32 msec) ;

  // block the calling thread until this thread terminates
  void join( PEGASUS_THREAD_RETURN *ret_val);


  // stack of functions to be called when thread terminates
  // will be called last in first out (LIFO)
  void cleanup_push( void (*routine) (void *), void *parm );
  void cleanup_pop(Boolean execute) ;

  // thread specific data (thread_data object methods)

  void create_tsd(Sint8 *key ) ;
  void create_tsd(Sint8 *key, int size) ;
  void *get_tsd(Sint8 *key);
  void delete_tsd(Sint8 *key);
  void *put_tsd(Sint8 *key, void (*delete)(void *), void *value);

  // cleanup handlers

  void push_cleanup_handler(void (*handler)(void *), void *parm) ;
  void pop_cleanup_handler(Boolean execute = true) ;

 private:
  Thread();

  PEGASUS_THREAD_HANDLE _handle;
  Boolean _is_detached;
  Boolean _cancel_enabled;
  Boolean _cancelled; 
  
  PEGASUS_SEM_HANDLE _suspend_count;

  // always pass this * as the void * parameter to the thread
  // store the user parameter in _thread_parm 

  PEGASUS_THREAD_RETURN  ( PEGASUS_THREAD_CDECL *_start)(void *) ;

  DQueue<cleanup_handler> _cleanup;
  DQueue<thread_data> _tsd;
  void *_thread_parm;

} ;
#endif

#if 0
class PEGASUS_EXPORT Aggregator {

 public:

  Aggregator();
  ~Aggregator();

  void started(void);
  void completed(void);
  void remaining(int operations);
  void put_result(CIMReference *ref);

 private: 
  int _reference_count;

  // keep track of the thread running this operation so we can kill
  // it if necessary 
  Thread _owner;

  // this is a phased aggregate. when it is complete is will
  // be streamed to the client regardless of the state of 
  // siblings 
  Boolean _is_phased;

  int _total_values;
  int _completed_values;
  int _total_child_values;
  int _completed_child_values;
  int _completion_state;
  struct timeval _last_update; 
  time_t lifetime;
  Aggregator *_parent;
  // children may be phased or not phased
  DQueue _children;
  // empty results that are filled by provider
  DQueue _results;
  // array of predicates for events and 
  // stored queries (cursors)
  Array _filter;
} ;
#endif
PEGASUS_NAMESPACE_END

#endif /* Pegasus_IPC_h */
