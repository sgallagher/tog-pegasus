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
// Modified By: Markus Mueller
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_Thread_h
#define Pegasus_Thread_h

#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/DQueue.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_EXPORT cleanup_handler
{

 public:
  cleanup_handler( void (*routine)(void *), void *arg  ) : _routine(routine), _arg(arg)  {}
  ~cleanup_handler() ;

 private:
  void execute(void) { _routine(_arg); } 
  cleanup_handler();
  void (*_routine)(void *);
  inline Boolean operator==(void *key) { if(key == (void *)_routine) return true; return false; }
  void *_arg; 
  PEGASUS_CLEANUP_HANDLE _cleanup_buffer;
  friend class Dqueue;
  friend class Thread;
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

  PEGASUS_THREAD_TYPE self(void) ;

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

static void default_delete(void * data) { delete [] (char *) data; }

class  PEGASUS_EXPORT thread_data
{

 public:
  thread_data( Sint8 *key ) : _delete_func(NULL) , _data(NULL), _size(0)
    {
      _key = strdup(key) ; 
    }
  
  thread_data(Sint8 *key, int size) 
    {
      _delete_func = default_delete;
      _data = new char [size];
    }

  thread_data(Sint8 *key, int size, void *data)
    {
      _delete_func = default_delete;
      _data = new char [size];
      memcpy(_data, data, size);
    }

  ~thread_data() { if( _data != NULL) _delete_func( _data ); }  

  void *get_data(void );
  Uint32 get_data_size(void);
  void *put_data(Sint8 *key, void (*delete_func) (void *), Uint32 size, void *data  )
    {
      void *old_data = data;
      _delete_func = delete_func;
      _data = data;
      return(old_data);
    }
 private:
  inline Boolean operator ==(void *key) ;
  void (*_delete_func) (void *data) ;
  thread_data();
  void *_data;
  Uint32 _size;
  Sint8 *_key;
  friend class Dqueue;
  friend class Thread;
};


///////////////////////////////////////////////////////////////////////////

class PEGASUS_EXPORT Thread
{

 public:
  Thread( PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *start )(void *),
          void *parameter, Boolean detached );

  ~Thread();

  void run(void);

  // get the user parameter 
  void *get_parm(void);

  // send the thread a signal -- may not be appropriate due to Windows 
  //  void kill(int signum); 

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
  // void suspend(void) ;

  // resume this thread
  // void resume(void) ;

  void sleep(Uint32 msec) ;

  // block the calling thread until this thread terminates
  void join(void );


  // stack of functions to be called when thread terminates
  // will be called last in first out (LIFO)
  void cleanup_push( void (*routine) (void *), void *parm ) throw(IPCException);
  void cleanup_pop(Boolean execute = true) throw(IPCException);

  // thread specific data (thread_data object methods)

  // create an empty tsd and index it according to <key>
  void create_tsd(void *key );

  // create an empty tsd with a pre-allocated buffer of <size>
  void create_tsd(void *key, int size) ;
  
  // create and initialize a tsd
  void create_tsd(void *key, int size, void *buffer);

  // get the buffer associated with the key
  void *get_tsd(void *key);

  // delete the tsd associated with the key
  void delete_tsd(void *key);

  // create or re-initialize tsd associated with the key
  // if the tsd already exists, return the existing buffer
  void * put_tsd(void *key, void (*delete_func)(void *), Uint32 size, void *value);

  inline PEGASUS_THREAD_RETURN get_exit(void) { return _exit_code; }
  inline PEGASUS_THREAD_TYPE self(void) {return _handle.thid; }

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

  PEGASUS_THREAD_RETURN _exit_code;

} ;


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

#endif // Pegasus_Thread_h
