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

#include "Thread.h"
#include <Pegasus/Common/IPC.h>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "ThreadWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "ThreadUnix.cpp"
#else
# error "Unsupported platform"
#endif

PEGASUS_NAMESPACE_BEGIN

void thread_data::default_delete(void * data) 
{ 
   if( data != NULL)
      ::operator delete(data); 
}

Boolean Thread::_signals_blocked = false;

// for non-native implementations
#ifndef PEGASUS_THREAD_CLEANUP_NATIVE 
void Thread::cleanup_push( void (*routine)(void *), void *parm) throw(IPCException)
{
    cleanup_handler *cu = new cleanup_handler(routine, parm);
    try 
    {
	_cleanup.insert_first(cu); 
    } 
    catch(IPCException& e) 
    { 
	delete cu;
	throw; 
    }
    return;
}
	  
void Thread::cleanup_pop(Boolean execute) throw(IPCException)
{
    cleanup_handler *cu ;
    try 
    { 
	cu = _cleanup.remove_first() ;
    }
    catch(IPCException& e) 
    {
	PEGASUS_ASSERT(0); 
    }
    if(execute == true)
	cu->execute();
    delete cu;
}
		    
#endif


//thread_data *Thread::put_tsd(Sint8 *key, void (*delete_func)(void *), Uint32 size, void *value) throw(IPCException)


#ifndef PEGASUS_THREAD_EXIT_NATIVE 
void Thread::exit_self(PEGASUS_THREAD_RETURN exit_code) 
{ 
    // execute the cleanup stack and then return 
   while( _cleanup.count() )
   {
       try 
       { 
	   cleanup_pop(true); 
       }
       catch(IPCException& e) 
       { 
	  PEGASUS_ASSERT(0); 
	  break; 
       } 
   }
   _exit_code = exit_code;
   exit_thread(exit_code);
}


#endif


ThreadPool::ThreadPool(Sint16 initial_size, 
		       Sint16 max, 
		       Sint16 min, 
		       Sint8 *key)
   : _max_threads(max), _min_threads(min),
     _current_threads(0), _waiters(initial_size), 
     _pool_sem(0), _pool(true), _running(true), 
     _dying(0)
{
   _allocate_wait.tv_sec = 1;
   _allocate_wait.tv_usec = 0;
   _deallocate_wait.tv_sec = 30;
   _deallocate_wait.tv_usec = 0;
   _deadlock_detect.tv_sec = 60;
   _deadlock_detect.tv_usec = 0;
   memset(_key, 0x00, 17);
   if(key != 0)
      strncpy(_key, key, 16);
   if(_max_threads < initial_size)
      _max_threads = initial_size;
   if(_min_threads > initial_size)
      _min_threads = initial_size;
   
   int i;
   for(i = 0; i < initial_size; i++)
   {
      _link_pool(_init_thread());
   }
}

ThreadPool::~ThreadPool(void)
{
   _dying++;
   Thread *th = _pool.remove_first();
   while(th != 0)
   {
      // signal the thread's sleep semaphore
      th->cancel();
      th->join();
      th->empty_tsd();
      delete th;
      th = _pool.remove_first();
   }
}

// make this static to the class 
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ThreadPool::_loop(void *parm)
{
   Thread *myself = (Thread *)parm;
   if(myself == 0)
      throw NullPointer();
   ThreadPool *pool = (ThreadPool *)myself->get_parm();
   if(pool == 0 )
      throw NullPointer();
   Semaphore *sleep_sem;
   struct timeval *deadlock_timer;
   
   try 
   {
      sleep_sem = (Semaphore *)myself->reference_tsd("sleep sem");
      myself->dereference_tsd();
      deadlock_timer = (struct timeval *)myself->reference_tsd("deadlock timer");
      myself->dereference_tsd();
   }
   catch(IPCException & e)
   {
      myself->exit_self(0);
   }
   if(sleep_sem == 0 || deadlock_timer == 0)
      throw NullPointer();

   while(pool->_dying < 1)
   {
      myself->test_cancel();
      sleep_sem->wait();
      // when we awaken we reside on the running queue, not the pool queue
      myself->test_cancel();
      gettimeofday(deadlock_timer, NULL);
      
      PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *_work)(void *);
      void *parm;

      try 
      {
	 _work = (PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *)(void *)) \
	    myself->reference_tsd("work func");
	 myself->dereference_tsd();
	 parm = myself->reference_tsd("work parm");
	 myself->dereference_tsd();
      }
      catch(IPCException & e)
      {
	 myself->exit_self(0);
      }
      
      if(_work == 0)
	 throw NullPointer();
      _work(parm);
	    
      // put myself back onto the available list 
      try 
      {
	 pool->_running.remove((void *)myself);
	 pool->_link_pool(myself);
      }
      catch(IPCException & e)
      {
	 myself->exit_self(0);
      }
   }
   myself->exit_self(0);
   return((PEGASUS_THREAD_RETURN)0);
}


void ThreadPool::allocate_and_awaken(void *parm,
				     PEGASUS_THREAD_RETURN \
				     (PEGASUS_THREAD_CDECL *work)(void *))
   throw(IPCException)
{
   struct timeval start;
   gettimeofday(&start, NULL);
   
   Thread *th = _pool.remove_first();

   while (th == 0 && _dying < 1)
   {
      try  // we couldn't get a free thread from the pool
      {
	 // wait for the right interval and try again
	 while(th == 0 && _dying < 1)
	 {
	    _check_deadlock(&start);
	    Uint32 interval = _allocate_wait.tv_sec * 1000;
	    if(_allocate_wait.tv_usec > 0)
	       interval += (_deallocate_wait.tv_usec / 1000);
	    // will throw a timeout if no thread comes free
	    _pool_sem.time_wait(interval);
	    th = _pool.remove_first();
	 }
      }
      catch(TimeOut & to)
      {
	 if(_current_threads < _max_threads)
	 {
	    th = _init_thread();
	    break;
	 } 
      } 
      // will throw a Deadlock Exception before falling out of the loop
      _check_deadlock(&start);
   } // while th == null
   
   if(_dying < 1)
   {
      // initialize the thread data with the work function and parameters
      th->remove_tsd("work func");
      th->put_tsd("work func", NULL, 
		  sizeof( PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *)(void *)),
		  (void *)work);
      th->remove_tsd("work parm");
      th->put_tsd("work parm", NULL, sizeof(void *), parm);
      
      // put the thread on the running list 
      _running.insert_first(th);

      // signal the thread's sleep semaphore to awaken it
      Semaphore *sleep_sem = (Semaphore *)th->reference_tsd("sleep sem");
      if(sleep_sem == 0)
	 throw NullPointer();
      sleep_sem->signal();
   }
   else
      _pool.insert_first(th);
}

// caller is responsible for only calling this routine during slack periods
// but should call it at least once per _deadlock_detect with the running q
// and at least once per _deallocate_wait for the pool q

void ThreadPool::_kill_dead_threads(DQueue<Thread> *q, Boolean (*check)(struct timeval *)) 
   throw(IPCException)
{
   struct timeval now;
   gettimeofday(&now, NULL);
   
   DQueue<Thread> dead(true) ;
   
   if(q->count() > 0 )
   {
      try 
      {
	 q->try_lock();
      }
      catch(AlreadyLocked & a)
      {
	 return;
      }

      Thread *context = 0;
      struct timeval dt = { 0, 0 };
      struct timeval *dtp;
      Thread *th = q->next(context);
      while (th != 0 )
      {
	 try 
	 {
	    dtp = (struct timeval *)th->try_reference_tsd("deadlock timer");
	 }
	 catch(AlreadyLocked & a)
	 {
	    context = th;
	    th = q->next(context);
	    continue;
	 }
	 
	 if(dtp != 0)
	 {
	    memcpy(&dt, dtp, sizeof(struct timeval));
	    
	 }
	 th->dereference_tsd();
	 if( true == check(&dt))
	 {
	    th = q->remove_no_lock((void *)th);
	    
	    if(th != 0)
	    {
	       dead.insert_first(th);
	       th = 0;
	    }
	 }
	 context = th;
	 th = q->next(context);
      }
      q->unlock();
   }
   
   if(dead.count())
   {
      Thread *th = dead.remove_first();
      while(th != 0)
      {
	 th->cancel();
	 th->join();
	 delete th;
	 th = dead.remove_first();
      }
   }
   return;
}

Boolean ThreadPool::_check_time(struct timeval *start, struct timeval *interval)
{
   struct timeval now;
   gettimeofday(&now, NULL);
   if( (now.tv_sec - start->tv_sec) > interval->tv_sec || 
       (((now.tv_sec - start->tv_sec) == interval->tv_sec) &&
	((now.tv_usec - start->tv_usec) >= interval->tv_usec ) ) )
      return true;
   else
      return false;
}


PEGASUS_NAMESPACE_END

