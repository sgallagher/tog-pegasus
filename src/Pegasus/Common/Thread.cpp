//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM,
// Compaq Computer Corporation
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
// Modified By: Rudy Schuet (rudy.schuet@compaq.com) 11/12/01
//              added nsk platform support  
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Thread.h"
#include <Pegasus/Common/IPC.h>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "ThreadWindows.cpp" 
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "ThreadUnix.cpp"
#elif defined(PEGASUS_OS_TYPE_NSK)
# include "ThreadNsk.cpp"
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
    catch(IPCException&) 
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
    catch(IPCException&) 
    {
	PEGASUS_ASSERT(0); 
     }
    if(execute == true)
	cu->execute();
    delete cu;
}
		    
#endif


//thread_data *Thread::put_tsd(const Sint8 *key, void (*delete_func)(void *), Uint32 size, void *value) throw(IPCException)


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
       catch(IPCException&) 
       { 
	  PEGASUS_ASSERT(0); 
	  break; 
       } 
   }
   _exit_code = exit_code;
   exit_thread(exit_code);
   _handle.thid = 0;
}


#endif

ThreadPool::ThreadPool(Sint16 initial_size,
		       const Sint8 *key,
		       Sint16 min,
		       Sint16 max,
		       struct timeval & alloc_wait,
		       struct timeval & dealloc_wait, 
		       struct timeval & deadlock_detect)
   : _max_threads(max), _min_threads(min),
     _current_threads(0), _waiters(initial_size), 
     _pool_sem(0), _pool(true), _running(true), 
     _dead(true), _dying(0)
{
   _allocate_wait.tv_sec = alloc_wait.tv_sec;
   _allocate_wait.tv_usec = alloc_wait.tv_usec;
   _deallocate_wait.tv_sec = dealloc_wait.tv_sec; 
   _deallocate_wait.tv_usec = dealloc_wait.tv_usec;
   _deadlock_detect.tv_sec = deadlock_detect.tv_sec;
   _deadlock_detect.tv_usec = deadlock_detect.tv_usec;
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
      Semaphore *sleep_sem = (Semaphore *)th->reference_tsd("sleep sem");

      if(sleep_sem == 0)
      {
	 th->dereference_tsd();
	 throw NullPointer();
      }
      
      sleep_sem->signal();
      sleep_sem->signal();
      th->dereference_tsd();
      // signal the thread's sleep semaphore
      th->cancel();
      th->join();
      th->empty_tsd();
      delete th;
      th = _pool.remove_first();
   }

   th = _running.remove_first();
   while(th != 0)
   {
      // signal the thread's sleep semaphore
      th->cancel();
      th->join();
      th->empty_tsd();
      delete th;
      th = _running.remove_first();
   }

   th = _dead.remove_first();
   while(th != 0)
   {
      // signal the thread's sleep semaphore
      th->cancel();
      th->join();
      th->empty_tsd();
      delete th;
      th = _dead.remove_first();
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
   Semaphore *sleep_sem = 0;
   struct timeval *deadlock_timer = 0;
   
   try 
   {
      sleep_sem = (Semaphore *)myself->reference_tsd("sleep sem");
      myself->dereference_tsd();
      deadlock_timer = (struct timeval *)myself->reference_tsd("deadlock timer");
      myself->dereference_tsd();
   }
   catch(IPCException &)
   {
      myself->exit_self(0);
   }
   if(sleep_sem == 0 || deadlock_timer == 0)
      throw NullPointer();

   while(pool->_dying < 1)
   {
      sleep_sem->wait();
      // when we awaken we reside on the running queue, not the pool queue
      if(pool->_dying > 0)
	 break;
     
      
      PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *_work)(void *) = 0;
      void *parm = 0;

      try 
      {
	 _work = (PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *)(void *)) \
	    myself->reference_tsd("work func");
	 myself->dereference_tsd();
	 parm = myself->reference_tsd("work parm");
	 myself->dereference_tsd();
      }
      catch(IPCException &)
      {
	 myself->exit_self(0);
      }
      
      if(_work == 0)
	 throw NullPointer();
      gettimeofday(deadlock_timer, NULL);
      _work(parm);
	    
      // put myself back onto the available list 
      try 
      {
	 pool->_running.remove((void *)myself);
	 pool->_link_pool(myself);
      }
      catch(IPCException &)
      {
	 myself->exit_self(0);
      }
   }
   // wait to be awakend by the thread pool destructor
   sleep_sem->wait();
   myself->test_cancel();
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


   // wait for the right interval and try again
   while(th == 0 && _dying < 1)
   {
      _check_deadlock(&start);
      Uint32 interval = (_allocate_wait.tv_sec * 1000) + _allocate_wait.tv_usec;
      // will throw a timeout if no thread comes free
      try 
      {
	 _pool_sem.time_wait(interval);
      }
      catch(TimeOut & timeout)
      {
	 if (timeout.get_owner())
	    ;
	 
	 if(_current_threads < _max_threads)
	 {
	    th = _init_thread();
	    continue;
	 } 
      } 
      th = _pool.remove_first();
   }
      
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
      {
	 th->dereference_tsd();
	 throw NullPointer();
      }
      
      sleep_sem->signal();
      th->dereference_tsd();
   }
   else
      _pool.insert_first(th);
}

// caller is responsible for only calling this routine during slack periods
// but should call it at least once per _deadlock_detect with the running q
// and at least once per _deallocate_wait for the pool q

void ThreadPool::kill_dead_threads(void) 
	 throw(IPCException)
{
   struct timeval now;
   gettimeofday(&now, NULL);
   

   // first go thread the dead q and clean it up as much as possible
   while(_dead.count() > 0)
   {
      Thread *dead = _dead.remove_first();
      if(dead == 0)
	 throw NullPointer();
      if(dead->_handle.thid != 0)
      {
	 dead->detach();
	 destroy_thread(dead->_handle.thid, 0);
	 dead->_handle.thid = 0;
	 while(dead->_cleanup.count() )
	 {
	    // this may throw a permission exception, 
	    // which I will remove from the code prior to stabilizing
	    dead->cleanup_pop(true);
	 }
      }
      delete dead;
   }
   
   DQueue<Thread> * map[2] = 
      {
	 &_pool, &_running
      };
   
   
   DQueue<Thread> *q = 0;
   int i = 0;
   AtomicInt needed(0);
   
   for( q = map[i] ; i < 2; i++, q = map[i])
   {
      if(q->count() > 0 )
      {
	 try 
	 {
	    q->try_lock();
	 }
	 catch(AlreadyLocked &)
	 {
	    q++;
	    continue;
	 }

	 struct timeval dt = { 0, 0 };
	 struct timeval *dtp;
	 Thread *th = 0;
	 th = q->next(th);
	 while (th != 0 )
	 {
	    try 
	    {
	       dtp = (struct timeval *)th->try_reference_tsd("deadlock timer");
	    }
	    catch(AlreadyLocked &)
	    {
	       th = q->next(th);
	       continue;
	    }
	 
	    if(dtp != 0)
	    {
	       memcpy(&dt, dtp, sizeof(struct timeval));
	    
	    }
	    th->dereference_tsd();
	    struct timeval deadlock_timeout;
	    if( true == check_time(&dt, get_deadlock_detect(&deadlock_timeout) ))
	    {
	       // if we are deallocating from the pool, escape if we are
	       // down to the minimum thread count 
	       if( _current_threads.value() <= (Uint32)_min_threads )
	       {
		  if( i == 1)
		  {
		     th = q->next(th);
		     continue;
		  }
		  else 
		  {
		     // we are killing a hung thread and we will drop below the 
		     // minimum. create another thread to make up for the one
		     // we are about to kill
		     needed++;
		  }
	       }
	       
	       th = q->remove_no_lock((void *)th);
	    
	       if(th != 0)
	       {
		  th->remove_tsd("work func");
		  th->put_tsd("work func", NULL, 
			      sizeof( PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *)(void *)),
			      (void *)&_undertaker);
		  th->remove_tsd("work parm");
		  th->put_tsd("work parm", NULL, sizeof(void *), th);
	        
		  // signal the thread's sleep semaphore to awaken it
		  Semaphore *sleep_sem = (Semaphore *)th->reference_tsd("sleep sem");
	       
		  if(sleep_sem == 0)
		  {
		     th->dereference_tsd();
		     throw NullPointer();
		  }
		  // put the thread on the dead  list 
		  _dead.insert_first(th);
		  sleep_sem->signal(); 
		  th->dereference_tsd();
		  th = 0;
	       }
	    }
	    th = q->next(th);
	 }
	 q->unlock();
	 while (needed.value() > 0)
	 {
	    _link_pool(_init_thread());
	    needed--;
	 }
      }
   }
   
   
   return;
}

Boolean ThreadPool::check_time(struct timeval *start, struct timeval *interval)
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


PEGASUS_THREAD_RETURN ThreadPool::_undertaker( void *parm )
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   if(myself != 0)
   {
      myself->detach();
      myself->_handle.thid = 0;
      myself->cancel();
      myself->test_cancel();
      myself->exit_self(0);
   }
   return((PEGASUS_THREAD_RETURN)0);
}


PEGASUS_NAMESPACE_END

