//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Thread.h"
#include <exception>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Tracer.h>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "ThreadWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "ThreadUnix.cpp"
#elif defined(PEGASUS_OS_TYPE_NSK)
# include "ThreadNsk.cpp"
#elif defined(PEGASUS_OS_VMS)
# include "ThreadVms.cpp"
#else
# error "Unsupported platform"
#endif

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN


void thread_data::default_delete(void * data)
{
   if( data != NULL)
      ::operator delete(data);
}

// l10n start
void language_delete(void * data)
{
   if( data != NULL)
   {
      AutoPtr<AcceptLanguages> al(static_cast<AcceptLanguages *>(data));
   }
}
// l10n end

Boolean Thread::_signals_blocked = false;
// l10n
#ifndef PEGASUS_OS_ZOS
PEGASUS_THREAD_KEY_TYPE Thread::_platform_thread_key = PEGASUS_THREAD_KEY_TYPE(-1);
#else
PEGASUS_THREAD_KEY_TYPE Thread::_platform_thread_key;
#endif
Boolean Thread::_key_initialized = false;
Boolean Thread::_key_error = false;


// for non-native implementations
#ifndef PEGASUS_THREAD_CLEANUP_NATIVE
void Thread::cleanup_push( void (*routine)(void *), void *parm) throw(IPCException)
{
    AutoPtr<cleanup_handler> cu(new cleanup_handler(routine, parm));
    _cleanup.insert_first(cu.get());
    cu.release();
    return;
}
	
void Thread::cleanup_pop(Boolean execute) throw(IPCException)
{
    AutoPtr<cleanup_handler> cu ;
    try
    {
	cu.reset(_cleanup.remove_first());
    }
    catch(IPCException&)
    {
	PEGASUS_ASSERT(0);
     }
    if(execute == true)
	cu->execute();
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
			language_delete, 
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


class auto_int
{
   public:
      auto_int(AtomicInt* num)
	 : _int(num)
      {
	 _int->operator++();
      }
      ~auto_int(void)
      {
	 _int->operator--();
      }
      AtomicInt *_int;
};


AtomicInt _idle_control;

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
		       const char *key,
		       Sint16 min,
		       Sint16 max,
		       struct timeval & alloc_wait,
		       struct timeval & dealloc_wait)
   : _max_threads(max), _min_threads(min),
     _current_threads(0),
     _pool(true), _running(true),
     _dying(0)
{
   _allocate_wait.tv_sec = alloc_wait.tv_sec;
   _allocate_wait.tv_usec = alloc_wait.tv_usec;
   _deallocate_wait.tv_sec = dealloc_wait.tv_sec;
   _deallocate_wait.tv_usec = dealloc_wait.tv_usec;
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


// Note:   <<< Fri Oct 17 09:19:03 2003 mdd >>>
// the pegasus_yield() calls that preceed each th->join() are to 
// give a thread on the running list a chance to reach a cancellation
// point before the join 

ThreadPool::~ThreadPool(void)
{
   PEG_METHOD_ENTER(TRC_THREAD, "Thread::~ThreadPool");
   try 
   {      
      // Set the dying flag so all thread know the destructor has been entered
      _dying++;
      
      // remove from the global pools list 
      _pools.remove(this);

      // start with idle threads. 
      Thread *th = 0;
      th = _pool.remove_first();
      Semaphore* sleep_sem;
      
      while(th != 0)
      {
	 sleep_sem = (Semaphore *)th->reference_tsd("sleep sem");
         PEGASUS_ASSERT(sleep_sem != 0);

	 if(sleep_sem == 0)
	 {
	    th->dereference_tsd();
	 }
         else
         {
	    // Signal to get the thread out of the work loop.
	    sleep_sem->signal();

	    // Signal to get the thread past the end. See the comment
	    // "wait to be awakend by the thread pool destructor"
	    // Note: the current implementation of Thread for Windows
	    // does not implement "pthread" cancelation points so this
	    // is needed.
	    sleep_sem->signal();
	    th->dereference_tsd();
	    th->join();
	    delete th;
         }
	 th = _pool.remove_first();
      }

      while(_idle_control.value())
	 pegasus_yield();
      
      {
	 th = _running.remove_first();
	 while(th != 0)
	 {	 
	    // signal the thread's sleep semaphore

	    sleep_sem = (Semaphore *)th->reference_tsd("sleep sem");
            PEGASUS_ASSERT(sleep_sem != 0);

	    if(sleep_sem == 0 )
	    {
	       th->dereference_tsd();
	    }
            else
            {
	       sleep_sem->signal();
	       sleep_sem->signal();
	       th->dereference_tsd();
	       //th->cancel();
	       pegasus_yield();
	    
	       th->join();
	       delete th;
	    }
	    th = _running.remove_first();
         }
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
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
          "ThreadPool::_loop: Thread pointer is null");
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
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
          "ThreadPool::_loop: ThreadPool pointer is null");
      PEG_METHOD_EXIT();
      throw NullPointer();
   }
   if(pool->_dying.value())
   {
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
          "ThreadPool::_loop: ThreadPool is dying(1)");
      PEG_METHOD_EXIT();
      return((PEGASUS_THREAD_RETURN)0);
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

   catch(...)
   {
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
		    "ThreadPool::_loop: Failure getting sleep_sem or deadlock_timer.");
      _graveyard(myself);
      PEG_METHOD_EXIT();
      return((PEGASUS_THREAD_RETURN)0);
   }
   
   if(sleep_sem == 0 || deadlock_timer == 0)
   {
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
          "ThreadPool::_loop: sleep_sem or deadlock_timer are null.");
      _graveyard(myself);
      PEG_METHOD_EXIT();
      return((PEGASUS_THREAD_RETURN)0);
   }

   while(1)
   {
      if(pool->_dying.value())
	 break;
      
      try 
      {
				Boolean ignoreInterrupt = false;
				sleep_sem->wait(ignoreInterrupt);
      }
      catch (WaitInterrupted &e)
      {
	/* From the sem_wait manpage:
 The sem_trywait() and sem_wait() functions may fail if:

       EINTR  A signal interrupted this function.
	*/
            PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
		"Sleep semaphore wait failed. Doing a continue");
	    continue;
      }
      catch(IPCException& )
      {
         Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
	   "ThreadPool::_loop: failure on sleep_sem->wait().");
	 _graveyard(myself);	 
	 PEG_METHOD_EXIT();
	 return((PEGASUS_THREAD_RETURN)0);
      }
      
      // when we awaken we reside on the running queue, not the pool queue
      
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
         Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
           "ThreadPool::_loop: Failure accessing work func, work parm, or blocking sem.");
	/*
	 * We cannot move ourselves to the dead queue b/c the TSD might be still
	 * locked and _graveyard is not equipped to de-lock (dereference_tsd) the TSD.
	 * Only the kill_dead_threads has enough logic to handle such situations.
	 _graveyard( myself);
	*/
	 PEG_METHOD_EXIT();
	 return((PEGASUS_THREAD_RETURN)0);
      }
      
      if(_work == 0)
      {
         Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
           "ThreadPool::_loop: work func is null.");
         PEG_METHOD_EXIT();
	 return((PEGASUS_THREAD_RETURN)0);
      }

      if(_work ==
         (PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *)(void *)) &_undertaker)
      {
	/*
	* The undertaker is set by  ThreadPool::kill_dead_threads which awakens this thread,
	*  joins it and then removes it from the queue. Hence no reason to go to the 
	_graveyard( myself);
	*/
         PEG_METHOD_EXIT();
	 _work(parm);
      }

      gettimeofday(deadlock_timer, NULL);

      if (pool->_dying.value() == 0)
      {
         try 
         {
            PEG_TRACE_STRING(TRC_THREAD, Tracer::LEVEL4,
		"Worker started");
	    _work(parm);
            PEG_TRACE_STRING(TRC_THREAD, Tracer::LEVEL4,
		"Worker finished");
         }
         catch(Exception & e)
         {
            PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
               String("Exception from _work in ThreadPool::_loop: ") +
                  e.getMessage());
            PEG_METHOD_EXIT();
            return((PEGASUS_THREAD_RETURN)0);
         }
#if !defined(PEGASUS_OS_LSB)
         catch (exception& e)
         {
            PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
               String("Exception from _work in ThreadPool::_loop: ") +
                  e.what());
            PEG_METHOD_EXIT();
            return((PEGASUS_THREAD_RETURN)0);
         }
#endif
         catch(...)
         {
            Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
              "ThreadPool::_loop: execution of _work failed.");
            PEG_METHOD_EXIT();
	    return((PEGASUS_THREAD_RETURN)0);
         }
       }
      
      // put myself back onto the available list
      try
      {
	 if(pool->_dying.value() == 0)
	 {
	    gettimeofday(deadlock_timer, NULL);
	    if( blocking_sem != 0 )
	       blocking_sem->signal();
      
	    // If we are not on _running then ~ThreadPool has removed
	    // us and now "owns" our pointer.
	    if ( pool->_running.remove((void *)myself) != 0 )
            {
	       pool->_pool.insert_first(myself);
            }
            else
            {
               Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                  "ThreadPool::_loop: Failed to remove thread from running queue.");
               PEG_METHOD_EXIT();
	       return((PEGASUS_THREAD_RETURN)0);
            }
	 }
	 else
	 {
	    PEG_METHOD_EXIT();
	    return((PEGASUS_THREAD_RETURN)0);
	 }
      }
      catch(...)
      {
        Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
             "ThreadPool::_loop: Adding thread to idle pool failed.");
	 PEG_METHOD_EXIT();
	 return((PEGASUS_THREAD_RETURN)0);
      }
      
   }

   // TODO: Why is this needed? Why not just continue?
   // wait to be awakend by the thread pool destructor
   //sleep_sem->wait();

   myself->test_cancel();

   PEG_METHOD_EXIT();
   return((PEGASUS_THREAD_RETURN)0);
}

Boolean ThreadPool::allocate_and_awaken(void *parm,
				        PEGASUS_THREAD_RETURN \
				        (PEGASUS_THREAD_CDECL *work)(void *), 
				        Semaphore *blocking)
   throw(IPCException)
{
   PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::allocate_and_awaken");

   // Allocate_and_awaken will not run if the _dying flag is set.
   // Once the lock is acquired, ~ThreadPool will not change
   // the value of _dying until the lock is released.
   
   try
   {
      if (_dying.value())
      {
         Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
          "ThreadPool::allocate_and_awaken: ThreadPool is dying(1).");
         // ATTN: Error result has not yet been defined
         return true;
      }
      struct timeval start;
      gettimeofday(&start, NULL);
      Thread *th = 0;
   
      th = _pool.remove_first();
   
      if (th == 0)
      {
         if(_max_threads == 0 || _current_threads < _max_threads)
         {
	    th = _init_thread();
         }
      }

      if (th == 0)
      {
        // ATTN-DME-P3-20031103: This trace message should not be
        // be labeled TRC_DISCARDED_DATA, because it does not
        // necessarily imply that a failure has occurred.  However,
        // this label is being used temporarily to help isolate
        // the cause of client timeout problems.

        Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
           "ThreadPool::allocate_and_awaken: Insufficient resources: "
           " pool = %s, running threads = %d, idle threads = %d",
           _key, _running.count(), _pool.count());
         return false;
      }

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
         Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
           "ThreadPool::allocate_and_awaken: thread data is corrupted.");
         PEG_METHOD_EXIT();
         throw NullPointer();
      }
      Tracer::trace(TRC_THREAD, Tracer::LEVEL4, "Signal thread to awaken");
      sleep_sem->signal();
      th->dereference_tsd();
   }
   catch (...)
   {
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
          "ThreadPool::allocate_and_awaken: Operation Failed.");
      PEG_METHOD_EXIT();
      // ATTN: Error result has not yet been defined
      return true;
   }
   PEG_METHOD_EXIT();
   return true;
}

// caller is responsible for only calling this routine during slack periods
// but should call it at least once per _deallocate_wait for the pool q

Uint32 ThreadPool::kill_dead_threads(void)
	 throw(IPCException)
{
   // Since the kill_dead_threads, ThreadPool or allocate_and_awaken 
   // manipulate the threads on the ThreadPool queues, they should never 
   // be allowed to run at the same time. 

   PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::kill_dead_threads");
   // << Thu Oct 23 14:41:02 2003 mdd >> 
   // not true, the queues are thread safe. they are syncrhonized. 

   auto_int do_not_destruct(&_idle_control);
   
   try
   {
      struct timeval now;
      gettimeofday(&now, NULL);
      Uint32 bodies = 0;
      AtomicInt needed(0);
   
      if (_dying.value())
      {
         return 0;
      }
   
      Thread *th = 0;
      internal_dq idq;
      
      if(_pool.count() > 0 )
      {
	 try
	 {
	    _pool.try_lock();
	 }
	 catch(...)
	 {
	    return bodies;
	 }

	 struct timeval dt = { 0, 0 };
	 struct timeval *dtp;

	 th = _pool.next(th);
	 while (th != 0 )
	 {
	    try
	    {
	       dtp = (struct timeval *)th->try_reference_tsd("deadlock timer");
	    }
	    catch(...)
	    {
	       _pool.unlock();
	       return bodies;
	    }

	    if(dtp != 0)
	    {
	       memcpy(&dt, dtp, sizeof(struct timeval));
	    }
	    th->dereference_tsd();
	    struct timeval deadlock_timeout;
	    Boolean too_long;
	    too_long = check_time(&dt, get_deallocate_wait(&deadlock_timeout));

	    if( true == too_long)
	    {
	       // escape if we are down to the minimum thread count
	       _current_threads--;
	       if( _current_threads.value() < (Uint32)_min_threads )
	       {
		  _current_threads++;
		  th = _pool.next(th);
		  continue;
	       }

	       th = _pool.remove_no_lock((void *)th);
               idq.insert_first((void*)th);
	    }
	    th = _pool.next(th);
	 }
	 _pool.unlock();
      }

      th = (Thread*)idq.remove_last();
      while(th != 0)
      {
         th->delete_tsd("work func");
         th->put_tsd("work func", NULL,
            sizeof( PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *)(void *)),
            (void *)&_undertaker);
         th->delete_tsd("work parm");
         th->put_tsd("work parm", NULL, sizeof(void *), th);
	       
         // signal the thread's sleep semaphore to awaken it
         Semaphore *sleep_sem = (Semaphore *)th->reference_tsd("sleep sem");
         PEGASUS_ASSERT(sleep_sem != 0);
	       
         bodies++;
         th->dereference_tsd();
         sleep_sem->signal();
         th->join();  // Clean up the thread
         delete th;
         th = (Thread*)idq.remove_last();
      }

     Tracer::trace(TRC_THREAD, Tracer::LEVEL2,
		"We need %u new threads", needed.value());
      while (needed.value() > 0)   {
         _link_pool(_init_thread());
         needed--;
         pegasus_sleep(0);
      }
       return bodies; 
    }
    catch (...)
    {
    }
   PEG_METHOD_EXIT();
    return 0;
}


Boolean ThreadPool::check_time(struct timeval *start, struct timeval *interval)
{
   // never time out if the interval is zero
   if(interval && interval->tv_sec == 0 && interval->tv_usec == 0)
      return false;
   
   struct timeval now , finish , remaining ;
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
   
   PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::_undertaker");
   exit_thread((PEGASUS_THREAD_RETURN)1);
   PEG_METHOD_EXIT();
   return (PEGASUS_THREAD_RETURN)1;
}

PEGASUS_THREAD_RETURN ThreadPool::_graveyard(Thread *t)
{
  PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::_graveyard");
  ThreadPool *pool = (ThreadPool *)t->get_parm();
  if(pool == 0 ) {
    Tracer::trace(TRC_THREAD, Tracer::LEVEL2,
		  "Could not obtain the pool information from the Thread.", t);

      return (PEGASUS_THREAD_RETURN)1;
  }
  if (pool->_pool.exists(t))
    {
      if (pool->_pool.remove( (void *) t) != 0)
	{
    	Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
		"Moving thread %p", t);
	/* We are moving the thread to the _running queue b/c
	_only_ kill_dead_threads has enough logic to take care 
	of cleaning up the threads.*/

	  pool->_running.insert_first( t );	  
	}
      else
	{
	  Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
			"Could not move Thread %p from _pool to _runing queue.", t);
	  return (PEGASUS_THREAD_RETURN)1;
	}
    }

  else if (pool->_running.exists(t))
    {
	 Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
			"Thread %p is on _running queue. Letting kill_dead_threads take care of the problem.", t);
	  return (PEGASUS_THREAD_RETURN)1;
    }
  else
    {
      Tracer::trace(TRC_THREAD, Tracer::LEVEL2,
		    "Thread is not on any queue! Moving it to the running queue.");
      pool->_running.insert_first( t );	
    }
  PEG_METHOD_EXIT();
  return (PEGASUS_THREAD_RETURN)0;
}

 void ThreadPool::_sleep_sem_del(void *p)
{
   if(p != 0)
   {
      delete (Semaphore *)p;
   }
}

 Boolean ThreadPool::_check_dealloc(struct timeval *start)
{
   return(check_time(start, &_deallocate_wait));
}

 Thread *ThreadPool::_init_thread(void) throw(IPCException)
{
  PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::_init_thread");
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
  
   if (!th->run())
   {
      delete th;
      return 0;
   }
   _current_threads++;
   pegasus_yield();
  PEG_METHOD_EXIT();
   
   return th;
}

 void ThreadPool::_link_pool(Thread *th) throw(IPCException)
{
   if(th == 0)
   {
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
          "ThreadPool::_link_pool: Thread pointer is null.");
      throw NullPointer();
   }
   try 
   {
      _pool.insert_first(th);
   }
   catch(...)
   {
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
          "ThreadPool::_link_pool: _pool.insert_first failed.");
   }
}


PEGASUS_NAMESPACE_END

