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
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
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


void Thread::cleanup_push( void (*routine)(void *), void *parm)
{
    AutoPtr<cleanup_handler> cu(new cleanup_handler(routine, parm));
    _cleanup.insert_first(cu.get());
    cu.release();
    return;
}

void Thread::cleanup_pop(Boolean execute)
{
    AutoPtr<cleanup_handler> cu;
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


//thread_data *Thread::put_tsd(const Sint8 *key, void (*delete_func)(void *), Uint32 size, void *value)


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
        if (pegasus_set_thread_specific(
               Thread::_platform_thread_key, (void *) thrd) == 0)
        {
            Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
                "Successful set Thread * into thread specific storage");
        }
        else
        {
            Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
                "ERROR: error setting Thread * into thread specific storage");
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

    Thread* currentThrd = Thread::getCurrent();
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


///////////////////////////////////////////////////////////////////////////////
//
// ThreadPool
//
///////////////////////////////////////////////////////////////////////////////

ThreadPool::ThreadPool(
    Sint16 initialSize,
    const char* key,
    Sint16 minThreads,
    Sint16 maxThreads,
    struct timeval& deallocateWait)
    : _maxThreads(maxThreads),
      _minThreads(minThreads),
      _currentThreads(0),
      _idleThreads(true),
      _runningThreads(true),
      _dying(0)
{
    _deallocateWait.tv_sec = deallocateWait.tv_sec;
    _deallocateWait.tv_usec = deallocateWait.tv_usec;

    memset(_key, 0x00, 17);
    if (key != 0)
    {
        strncpy(_key, key, 16);
    }

    if ((_maxThreads > 0) && (_maxThreads < initialSize))
    {
        _maxThreads = initialSize;
    }

    if (_minThreads > initialSize)
    {
        _minThreads = initialSize;
    }

    for (int i = 0; i < initialSize; i++)
    {
        _addToIdleThreadsQueue(_initializeThread());
    }
}

ThreadPool::~ThreadPool()
{
    PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::~ThreadPool");

    try
    {
        // Set the dying flag so all thread know the destructor has been entered
        _dying++;
       Tracer::trace(TRC_THREAD, Tracer::LEVEL2,
		"Cleaning up %d idle threads. ", _currentThreads.value());
        while (_currentThreads.value() > 0)
        {
            Thread* thread = _idleThreads.remove_first();
            if (thread != 0)
            {
                _cleanupThread(thread);
                _currentThreads--;
            }
            else
            {
                pegasus_yield();
            }
        }
    }
    catch (...)
    {
    }
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ThreadPool::_loop(void* parm)
{
    PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::_loop");

    try
    {
        Thread* myself = (Thread *)parm;
        PEGASUS_ASSERT(myself != 0);

        // Set myself into thread specific storage
        // This will allow code to get its own Thread
        Thread::setCurrent(myself);

        ThreadPool* pool = (ThreadPool *)myself->get_parm();
        PEGASUS_ASSERT(pool != 0);

        Semaphore* sleep_sem = 0;
        struct timeval* lastActivityTime = 0;

        try
        {
            sleep_sem = (Semaphore *)myself->reference_tsd("sleep sem");
            myself->dereference_tsd();
            PEGASUS_ASSERT(sleep_sem != 0);

            lastActivityTime =
                (struct timeval *)myself->reference_tsd("last activity time");
            myself->dereference_tsd();
            PEGASUS_ASSERT(lastActivityTime != 0);
        }
        catch (...)
        {
            Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "ThreadPool::_loop: Failure getting sleep_sem or "
                    "lastActivityTime.");
            PEGASUS_ASSERT(false);
            pool->_idleThreads.remove(myself);
            pool->_currentThreads--;
            PEG_METHOD_EXIT();
            return((PEGASUS_THREAD_RETURN)1);
        }

        while (1)
        {
            try
            {
                sleep_sem->wait();
            }
            catch (...)
            {
                Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    "ThreadPool::_loop: failure on sleep_sem->wait().");
                PEGASUS_ASSERT(false);
                pool->_idleThreads.remove(myself);
                pool->_currentThreads--;
                PEG_METHOD_EXIT();
                return((PEGASUS_THREAD_RETURN)1);
            }

            // When we awaken we reside on the _runningThreads queue, not the
            // _idleThreads queue.

            PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL* work)(void *) = 0;
            void* parm = 0;
            Semaphore* blocking_sem = 0;

            try
            {
                work = (PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *)(void *))
                    myself->reference_tsd("work func");
                myself->dereference_tsd();
                parm = myself->reference_tsd("work parm");
                myself->dereference_tsd();
                blocking_sem = (Semaphore *)myself->reference_tsd("blocking sem");
                myself->dereference_tsd();
            }
            catch (...)
            {
                Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    "ThreadPool::_loop: Failure accessing work func, work parm, "
                        "or blocking sem.");
                PEGASUS_ASSERT(false);
                pool->_idleThreads.remove(myself);
                pool->_currentThreads--;
                PEG_METHOD_EXIT();
                return((PEGASUS_THREAD_RETURN)1);
            }

            if (work == 0)
            {
                Tracer::trace(TRC_THREAD, Tracer::LEVEL4,
                    "ThreadPool::_loop: work func is 0, meaning we should exit.");
                break;
            }

            gettimeofday(lastActivityTime, NULL);

            try
            {
                PEG_TRACE_STRING(TRC_THREAD, Tracer::LEVEL4, "Work starting.");
                work(parm);
                PEG_TRACE_STRING(TRC_THREAD, Tracer::LEVEL4, "Work finished.");
            }
            catch (Exception & e)
            {
                PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    String("Exception from work in ThreadPool::_loop: ") +
                        e.getMessage());
            }
#if !defined(PEGASUS_OS_LSB)
            catch (const exception& e)
            {
                PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    String("Exception from work in ThreadPool::_loop: ") +
                        e.what());
            }
#endif
            catch (...)
            {
                PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    "Unknown exception from work in ThreadPool::_loop.");
            }

            // put myself back onto the available list
            try
            {
                gettimeofday(lastActivityTime, NULL);
                if (blocking_sem != 0)
                {
                    blocking_sem->signal();
                }

                Boolean removed = pool->_runningThreads.remove((void *)myself);
                PEGASUS_ASSERT(removed);

                pool->_idleThreads.insert_first(myself);
            }
            catch (...)
            {
                Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    "ThreadPool::_loop: Adding thread to idle pool failed.");
                PEGASUS_ASSERT(false);
                pool->_currentThreads--;
                PEG_METHOD_EXIT();
                return((PEGASUS_THREAD_RETURN)1);
            }
        }
    }
    catch (const Exception& e)
    {
        PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Caught exception: \"" + e.getMessage() + "\".  Exiting _loop.");
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Caught unrecognized exception.  Exiting _loop.");
    }

    PEG_METHOD_EXIT();
    return((PEGASUS_THREAD_RETURN)0);
}

ThreadStatus ThreadPool::allocate_and_awaken(
    void* parm,
    PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL* work)(void *),
    Semaphore* blocking)
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
            return PEGASUS_THREAD_UNAVAILABLE;
        }
        struct timeval start;
        gettimeofday(&start, NULL);
        Thread* th = 0;

        th = _idleThreads.remove_first();

        if (th == 0)
        {
            if ((_maxThreads == 0) || (_currentThreads < _maxThreads))
            {
                th = _initializeThread();
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
                _key, _runningThreads.count(), _idleThreads.count());
            return PEGASUS_THREAD_INSUFFICIENT_RESOURCES;
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
        if (blocking != 0)
            th->put_tsd("blocking sem", NULL, sizeof(Semaphore *), blocking);

        // put the thread on the running list
        _runningThreads.insert_first(th);

        // signal the thread's sleep semaphore to awaken it
        Semaphore* sleep_sem = (Semaphore *)th->reference_tsd("sleep sem");
        PEGASUS_ASSERT(sleep_sem != 0);

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
        return PEGASUS_THREAD_SETUP_FAILURE;
    }
    PEG_METHOD_EXIT();
    return PEGASUS_THREAD_OK;
}

// caller is responsible for only calling this routine during slack periods
// but should call it at least once per _deallocateWait interval.

Uint32 ThreadPool::cleanupIdleThreads()
{
    PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::cleanupIdleThreads");

    Uint32 numThreadsCleanedUp = 0;

    Uint32 numIdleThreads = _idleThreads.count();
    for (Uint32 i = 0; i < numIdleThreads; i++)
    {
        // Do not dip below the minimum thread count
        if (_currentThreads.value() <= (Uint32)_minThreads)
        {
            break;
        }

        Thread* thread = _idleThreads.remove_last();

        // If there are no more threads in the _idleThreads queue, we're done.
        if (thread == 0)
        {
            break;
        }

        struct timeval* lastActivityTime;
        try
        {
            lastActivityTime = (struct timeval *)thread->try_reference_tsd(
                "last activity time");
            PEGASUS_ASSERT(lastActivityTime != 0);
        }
        catch (...)
        {
            PEGASUS_ASSERT(false);
            _idleThreads.insert_last(thread);
            break;
        }

        Boolean cleanupThisThread =
            _timeIntervalExpired(lastActivityTime, &_deallocateWait);
        thread->dereference_tsd();

        if (cleanupThisThread)
        {
            _cleanupThread(thread);
            _currentThreads--;
            numThreadsCleanedUp++;
        }
        else
        {
            _idleThreads.insert_first(thread);
        }
    }

    PEG_METHOD_EXIT();
    return numThreadsCleanedUp;
}

void ThreadPool::_cleanupThread(Thread* thread)
{
    PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::cleanupThread");

    // Set the "work func" and "work parm" to 0 so _loop() knows to exit.
    thread->delete_tsd("work func");
    thread->put_tsd(
        "work func", 0,
        sizeof(PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *)(void *)),
        (void *) 0);
    thread->delete_tsd("work parm");
    thread->put_tsd("work parm", 0, sizeof(void *), 0);

    // signal the thread's sleep semaphore to awaken it
    Semaphore* sleep_sem = (Semaphore *)thread->reference_tsd("sleep sem");
    PEGASUS_ASSERT(sleep_sem != 0);
    sleep_sem->signal();
    thread->dereference_tsd();

    thread->join();
    delete thread;

    PEG_METHOD_EXIT();
}

Boolean ThreadPool::_timeIntervalExpired(
    struct timeval* start,
    struct timeval* interval)
{
    // never time out if the interval is zero
    if (interval && (interval->tv_sec == 0) && (interval->tv_usec == 0))
    {
        return false;
    }

    struct timeval now, finish, remaining;
    Uint32 usec;
    pegasus_gettimeofday(&now);
    pegasus_gettimeofday(&remaining);    // Avoid valgrind error

    finish.tv_sec = start->tv_sec + interval->tv_sec;
    usec = start->tv_usec + interval->tv_usec;
    finish.tv_sec += (usec / 1000000);
    usec %= 1000000;
    finish.tv_usec = usec;

    return (timeval_subtract(&remaining, &finish, &now) != 0);
}

void ThreadPool::_deleteSemaphore(void *p)
{
    delete (Semaphore *)p;
}

Thread* ThreadPool::_initializeThread()
{
    PEG_METHOD_ENTER(TRC_THREAD, "ThreadPool::_initializeThread");

    Thread* th = (Thread *) new Thread(_loop, this, false);

    // allocate a sleep semaphore and pass it in the thread context
    // initial count is zero, loop function will sleep until
    // we signal the semaphore
    Semaphore* sleep_sem = (Semaphore *) new Semaphore(0);
    th->put_tsd(
        "sleep sem", &_deleteSemaphore, sizeof(Semaphore), (void *)sleep_sem);

    struct timeval* lastActivityTime =
        (struct timeval *) ::operator new(sizeof(struct timeval));
    pegasus_gettimeofday(lastActivityTime);

    th->put_tsd("last activity time", thread_data::default_delete,
        sizeof(struct timeval), (void *)lastActivityTime);
    // thread will enter _loop() and sleep on sleep_sem until we signal it

    if (th->run() != PEGASUS_THREAD_OK)
    {
		Tracer::trace(TRC_THREAD, Tracer::LEVEL2,
			"Could not create thread. Error code is %d.", errno);
        delete th;
        return 0;
    }
    _currentThreads++;
    pegasus_yield();

    PEG_METHOD_EXIT();
    return th;
}

void ThreadPool::_addToIdleThreadsQueue(Thread* th)
{
    if (th == 0)
    {
        Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "ThreadPool::_addToIdleThreadsQueue: Thread pointer is null.");
        throw NullPointer();
    }

    try
    {
        _idleThreads.insert_first(th);
    }
    catch (...)
    {
        Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "ThreadPool::_addToIdleThreadsQueue: _idleThreads.insert_first "
                "failed.");
    }
}

PEGASUS_NAMESPACE_END
