//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
#include <Pegasus/Common/Tracer.h>

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

// l10n
PEGASUS_THREAD_KEY_TYPE Thread::_platform_thread_key;
Boolean Thread::_key_initialized = false;
Boolean Thread::_key_error = false;

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

// l10n start
Sint8 Thread::initializeKey()
{
   PEG_METHOD_ENTER(TRC_THREAD, "Thread::initializeKey");
   if (!Thread::_key_initialized)
   {
	if (Thread::_key_error)
	{
       		Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
	        	  "Thread: ERROR - thread key error"); 
		return -1;
	}

	if (pegasus_key_create(&Thread::_platform_thread_key) == 0)
	{
        	Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
	        	  "Thread: able to create a thread key");   
	   	Thread::_key_initialized = true;	
	}
	else
	{
       		Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
	        	  "Thread: ERROR - unable to create a thread key"); 
	   	Thread::_key_error = true;
		return -1;
	}
   }

   PEG_METHOD_EXIT();
   return 0;  
}

Thread * Thread::getCurrent()
{
    PEG_METHOD_ENTER(TRC_THREAD, "Thread::getCurrent");	
    if (Thread::initializeKey() != 0)
    {
	return NULL;  
    }
    PEG_METHOD_EXIT();  
    return (Thread *)pegasus_get_thread_specific(_platform_thread_key); 
}

void Thread::setCurrent(Thread * thrd)
{
   PEG_METHOD_ENTER(TRC_THREAD, "Thread::setCurrent");
   if (Thread::initializeKey() == 0)
   {
   	if (pegasus_set_thread_specific(Thread::_platform_thread_key,
								 (void *) thrd) == 0)
        {
        	Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
	        	  "Successful set Thread * into thread specific storage");   
        }
        else
        {
        	Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
	        	  "ERROR: got error setting Thread * into thread specific storage");   
        }
   }
   PEG_METHOD_EXIT();  
}

AcceptLanguages * Thread::getLanguages()
{
    PEG_METHOD_ENTER(TRC_THREAD, "Thread::getLanguages");		
    
	Thread * curThrd = Thread::getCurrent();
	if (curThrd == NULL)
		return NULL;
   	AcceptLanguages * acceptLangs =
   		 (AcceptLanguages *)curThrd->reference_tsd("acceptLanguages");
	curThrd->dereference_tsd();
    PEG_METHOD_EXIT(); 	
	return acceptLangs;
}

void Thread::setLanguages(AcceptLanguages *langs) //l10n
{
   PEG_METHOD_ENTER(TRC_THREAD, "Thread::setLanguages");
   		
   Thread * currentThrd = Thread::getCurrent();
   if (currentThrd != NULL)
   {
   		// deletes the old tsd and creates a new one
		currentThrd->put_tsd("acceptLanguages",
			thread_data::default_delete, 
			sizeof(AcceptLanguages *),
			langs);   		
   }
   
   PEG_METHOD_EXIT();    		
}

void Thread::clearLanguages() //l10n
{
   PEG_METHOD_ENTER(TRC_THREAD, "Thread::clearLanguages");
   	
   Thread * currentThrd = Thread::getCurrent();
   if (currentThrd != NULL)
   {
   		// deletes the old tsd
		currentThrd->delete_tsd("acceptLanguages");   		
   }
   
   PEG_METHOD_EXIT();   		
}
// l10n end      

DQueue<ThreadPool> ThreadPool::_pools(true);


void ThreadPool::kill_idle_threads(void)
{
   static struct timeval now, last = {0, 0};
   
   pegasus_gettimeofday(&now);
   if(now.tv_sec - last.tv_sec > 5)
   {
      _pools.lock();
      ThreadPool *p = _pools.next(0);
      while(p != 0)
      {
	 try 
	 {
	    p->kill_dead_threads();
	 }
	 catch(...)
	 {
	 }
	 p = _pools.next(p);
      }
      _pools.unlock();
      pegasus_gettimeofday(&last);
   }
}


ThreadPool::ThreadPool(Sint16 initial_size,
		       const Sint8 *key,
		       Sint16 min,
		       Sint16 max,
		       struct timeval & alloc_wait,
		       struct timeval & dealloc_wait,
		       struct timeval & deadlock_detect)
   : _max_threads(max), _min_threads(min),
     _current_threads(0),
     _pool(true), _running(true),
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
   if(_max_threads > 0 && _max_threads < initial_size)
      _max_threads = initial_size;
   if(_min_threads > initial_size)
      _min_threads = initial_size;

   int i;
   for(i = 0; i < initial_size; i++)
   {
      _link_pool(_init_thread());
   }
   _pools.insert_last(this);

}



ThreadPool::~ThreadPool(void)
{
   try 
   {
      _pools.remove(this);
      _dying++;
      Thread *th = 0;
      th = _pool.remove_first();
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
   catch(...)
   {
   }
}

// make this static to the class
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ThreadPool::_loop(void *parm)
{
   PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::_loop");

   Thread *myself = (Thread *)parm;
   if(myself == 0)
   {
      PEG_METHOD_EXIT();
      throw NullPointer();
   }
   
// l10n
   // Set myself into thread specific storage
   // This will allow code to get its own Thread
   Thread::setCurrent(myself);	

   ThreadPool *pool = (ThreadPool *)myself->get_parm();
   if(pool == 0 ) 
   {
      PEG_METHOD_EXIT();
      throw NullPointer();
   }
   Semaphore *sleep_sem = 0;
   Semaphore *blocking_sem = 0;
   
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
      PEG_METHOD_EXIT();
      myself->exit_self(0);
   }
   catch(...)
   {
      PEG_METHOD_EXIT();
      myself->exit_self(0);
   }
   
   if(sleep_sem == 0 || deadlock_timer == 0)
   {
      PEG_METHOD_EXIT();
      throw NullPointer();
   }

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
	 blocking_sem = (Semaphore *)myself->reference_tsd("blocking sem");
	 myself->dereference_tsd();

      }
      catch(IPCException &)
      {
	 PEG_METHOD_EXIT();
	 myself->exit_self(0);
      }

      if(_work == 0)
      {
         PEG_METHOD_EXIT();
	 throw NullPointer();
      }

      if(_work ==
         (PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *)(void *)) &_undertaker)
      {
	 _work(parm);
      }

      gettimeofday(deadlock_timer, NULL);
      try 
      {
	 _work(parm);
      }
      catch(...)
      {
	 gettimeofday(deadlock_timer, NULL);
      }
      
      gettimeofday(deadlock_timer, NULL);
      if( blocking_sem != 0 )
	 blocking_sem->signal();
      
      // put myself back onto the available list
      try
      {
	 pool->_running.remove((void *)myself);
	 pool->_link_pool(myself);
      }
      catch(IPCException &)
      {
	 PEG_METHOD_EXIT();
	 myself->exit_self(0);
      }
   }
   // wait to be awakend by the thread pool destructor
   sleep_sem->wait();
   myself->test_cancel();

   PEG_METHOD_EXIT();
   myself->exit_self(0);
   return((PEGASUS_THREAD_RETURN)0);
}

void ThreadPool::allocate_and_awaken(void *parm,
				     PEGASUS_THREAD_RETURN \
				     (PEGASUS_THREAD_CDECL *work)(void *), 
				     Semaphore *blocking)

   throw(IPCException)
{
   PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::allocate_and_awaken");
   struct timeval start;
   gettimeofday(&start, NULL);

   Thread *th = _pool.remove_first();
   
   // wait for the right interval and try again
   while (th == 0 && _dying < 1)
   {
      _check_deadlock(&start) ;
      
      if(_max_threads == 0 || _current_threads < _max_threads)
      {
	 th = _init_thread();
	 continue;
      }
      pegasus_yield();
      th = _pool.remove_first();
   }


   if(_dying < 1)
   {
      // initialize the thread data with the work function and parameters
      Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
          "Initializing thread with work function and parameters: parm = %p",
          parm);

      th->delete_tsd("work func");
      th->put_tsd("work func", NULL,
		  sizeof( PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *)(void *)),
		  (void *)work);
      th->delete_tsd("work parm");
      th->put_tsd("work parm", NULL, sizeof(void *), parm);
      th->delete_tsd("blocking sem");
      if(blocking != 0 )
	 th->put_tsd("blocking sem", NULL, sizeof(Semaphore *), blocking);
      
      // put the thread on the running list
      _running.insert_first(th);

      // signal the thread's sleep semaphore to awaken it
      Semaphore *sleep_sem = (Semaphore *)th->reference_tsd("sleep sem");

      if(sleep_sem == 0)
      {
	 th->dereference_tsd();
         PEG_METHOD_EXIT();
	 throw NullPointer();
      }
      Tracer::trace(TRC_THREAD, Tracer::LEVEL4, "Signal thread to awaken");
      sleep_sem->signal();
      th->dereference_tsd();
   }
   else
      _pool.insert_first(th);

   PEG_METHOD_EXIT();
}

// caller is responsible for only calling this routine during slack periods
// but should call it at least once per _deadlock_detect with the running q
// and at least once per _deallocate_wait for the pool q

Uint32 ThreadPool::kill_dead_threads(void)
	 throw(IPCException)
{
   struct timeval now;
   gettimeofday(&now, NULL);
   Uint32 bodies = 0;
   
   // first go thread the dead q and clean it up as much as possible
   while(_dead.count() > 0)
   {
      Tracer::trace(TRC_THREAD, Tracer::LEVEL4, "ThreadPool:: removing and joining dead thread");
      Thread *dead = _dead.remove_first();
      if(dead == 0)
	 throw NullPointer();
      dead->join();
      delete dead;
   }

   DQueue<Thread> * map[2] =
      {
	 &_pool, &_running
      };


   DQueue<Thread> *q = 0;
   int i = 0;
   AtomicInt needed(0);

#ifdef PEGASUS_DISABLE_KILLING_HUNG_THREADS
   // This change prevents the thread pool from killing "hung" threads.
   // The definition of a "hung" thread is one that has been on the run queue
   // for longer than the time interval set when the thread pool was created.
   // Cancelling "hung" threads has proven to be problematic.

   // With this change the thread pool will not cancel "hung" threads.  This
   // may prevent a crash depending upon the state of the "hung" thread.  In
   // the case that the thread is actually hung, this change causes the
   // thread resources not to be reclaimed.

   // Idle threads, those that have not executed a routine for a time
   // interval, continue to be destroyed.  This is normal and should not
   // cause any problems.
   for( ; i < 1; i++)
#else
   for( ; i < 2; i++)
#endif
   { 
      q = map[i];
      if(q->count() > 0 )
      {
	 try
	 {
	    q->try_lock();
	 }
	 catch(...)
	 {
	    return bodies;
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
	    catch(...)
	    {
	       q->unlock();
	       return bodies;
	    }
	
	    if(dtp != 0)
	    {
	       memcpy(&dt, dtp, sizeof(struct timeval));
	    }
	    th->dereference_tsd();
	    struct timeval deadlock_timeout;
	    Boolean too_long;
	    if( i == 0)
	    {
	       too_long = check_time(&dt, get_deallocate_wait(&deadlock_timeout));
	    }
	    else 
	    {
	       too_long = check_time(&dt, get_deadlock_detect(&deadlock_timeout));
	    }
	    
	    if( true == too_long)
	    {
	       // if we are deallocating from the pool, escape if we are
	       // down to the minimum thread count
	       _current_threads--;
	       if( _current_threads.value() < (Uint32)_min_threads )
	       {
		  if( i == 0)
		  {
		     _current_threads++;
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
		  if( i == 0 )
		  {
		     th->delete_tsd("work func");
		     th->put_tsd("work func", NULL,
				 sizeof( PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *)(void *)),
				 (void *)&_undertaker);
		     th->delete_tsd("work parm");
		     th->put_tsd("work parm", NULL, sizeof(void *), th);
		     
		     // signal the thread's sleep semaphore to awaken it
		     Semaphore *sleep_sem = (Semaphore *)th->reference_tsd("sleep sem");
		     
		     if(sleep_sem == 0)
		     {
			q->unlock();
			th->dereference_tsd();
			throw NullPointer();
		     }
		     
		     bodies++;
		     th->dereference_tsd();
		     _dead.insert_first(th);
		     sleep_sem->signal();
		     th = 0;
		  }
		  else 
		  {
		     // deadlocked threads
		     Tracer::trace(TRC_THREAD, Tracer::LEVEL4, "Killing a deadlocked thread");
		     th->cancel();
		     delete th;
		  }
	       }
	    }
	    th = q->next(th);
	    pegasus_sleep(1);
	 }
	 q->unlock();
	 while (needed.value() > 0)
	 {
	    _link_pool(_init_thread());
	    needed--;
	    pegasus_sleep(0);
	 }
      }
   }
    return bodies; 
}


Boolean ThreadPool::check_time(struct timeval *start, struct timeval *interval)
{
   // never time out if the interval is zero
   if(interval && interval->tv_sec == 0 && interval->tv_usec == 0)
      return false;
   
   struct timeval now, finish, remaining ;
   Uint32 usec;
   pegasus_gettimeofday(&now);
   /* remove valgrind error */
   pegasus_gettimeofday(&remaining);
   

   finish.tv_sec = start->tv_sec + interval->tv_sec;
   usec = start->tv_usec + interval->tv_usec;
   finish.tv_sec += (usec / 1000000);
   usec %= 1000000;
   finish.tv_usec = usec;
    
   if ( timeval_subtract(&remaining, &finish, &now) )
      return true;
   else
      return false;
}

PEGASUS_THREAD_RETURN ThreadPool::_undertaker( void *parm )
{
   exit_thread((PEGASUS_THREAD_RETURN)1);
   return (PEGASUS_THREAD_RETURN)1;
}


 void ThreadPool::_sleep_sem_del(void *p)
{
   if(p != 0)
   {
      delete (Semaphore *)p;
   }
}

 void ThreadPool::_check_deadlock(struct timeval *start) throw(Deadlock)
{
   if (true == check_time(start, &_deadlock_detect))
      throw Deadlock(pegasus_thread_self());
   return;
}


 Boolean ThreadPool::_check_deadlock_no_throw(struct timeval *start)
{
   return(check_time(start, &_deadlock_detect));
}

 Boolean ThreadPool::_check_dealloc(struct timeval *start)
{
   return(check_time(start, &_deallocate_wait));
}

 Thread *ThreadPool::_init_thread(void) throw(IPCException)
{
   Thread *th = (Thread *) new Thread(_loop, this, false);
   // allocate a sleep semaphore and pass it in the thread context
   // initial count is zero, loop function will sleep until
   // we signal the semaphore
   Semaphore *sleep_sem = (Semaphore *) new Semaphore(0);
   th->put_tsd("sleep sem", &_sleep_sem_del, sizeof(Semaphore), (void *)sleep_sem);
   
   struct timeval *dldt = (struct timeval *) ::operator new(sizeof(struct timeval));
   pegasus_gettimeofday(dldt);
   
   th->put_tsd("deadlock timer", thread_data::default_delete, sizeof(struct timeval), (void *)dldt);
   // thread will enter _loop(void *) and sleep on sleep_sem until we signal it
  
   th->run();
   _current_threads++;
   pegasus_yield();
   
   return th;
}

 void ThreadPool::_link_pool(Thread *th) throw(IPCException)
{
   if(th == 0)
      throw NullPointer();
   _pool.insert_first(th);
}


PEGASUS_NAMESPACE_END

