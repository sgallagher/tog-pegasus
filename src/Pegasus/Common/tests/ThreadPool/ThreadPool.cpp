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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Tracer.h>

#include <sys/types.h>
#if !defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include <unistd.h>
#endif
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <string.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

Boolean verbose = false;

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL funcSleepUntilCancelled(
    void* parm)
{
    AtomicInt* cancelled = static_cast<AtomicInt*>(parm);

    while (cancelled->value() == 0)
    {
        pegasus_sleep(1);
    }

    return 0;
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL funcSleepSpecifiedMilliseconds(
    void* parm)
{
#ifdef PEGASUS_POINTER_64BIT
    Uint32 sleepMilliseconds = (Uint64)parm;
#elif PEGASUS_PLATFORM_AIX_RS_IBMCXX
    unsigned long sleepMilliseconds = (unsigned long)parm;
#else
    Uint32 sleepMilliseconds = (Uint32)parm;
#endif

    pegasus_sleep(sleepMilliseconds);

    return 0;
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL funcIncrementCounter(
    void* parm)
{
    AtomicInt* counter = static_cast<AtomicInt*>(parm);

    (*counter)++;
    pegasus_sleep(50);

    return 0;
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL funcThrow(void* parm)
{
    throw Uint32(10);
    PEGASUS_UNREACHABLE(return 0);
}

void testDestructAsThreadCompletes()
{
    AtomicInt cancelled = 0;

    struct timeval deallocateWait = {0, 0};
    ThreadPool* threadPool = new ThreadPool(0, "Tester", 0, 1, deallocateWait);

    threadPool->allocate_and_awaken(&cancelled, funcSleepUntilCancelled);

    cancelled = 1;

    delete threadPool;
}

void testloopDestructAsThreadCompletes()
{
    if (verbose)
    {
        cout << "testloopDestructAsThreadCompletes" << endl;
    }

    try
    {
        int done = 0;
        const int limit = 10000;
        while (done < limit)
        {
            if (verbose || (done % 1000 == 0))
            {
                printf("testDestructAsThreadCompletes: iteration %d of %d\n",
                    done+1, limit);
            }
            testDestructAsThreadCompletes();
            done++;
        }
    }
    catch (const Exception& e)
    {
        cout << "Exception in testloopDestructAsThreadCompletes: " <<
            e.getMessage() << endl;
        assert(false);
    }
}

void testCleanupIdleThread()
{
    if (verbose)
    {
        cout << "testCleanupIdleThread" << endl;
    }

    try
    {
        struct timeval deallocateWait = { 0, 1 };
        ThreadPool threadPool(0, "test cleanup", 0, 6, deallocateWait);

        threadPool.allocate_and_awaken(
            (void*)1, funcSleepSpecifiedMilliseconds);
        pegasus_sleep(1000);

        assert(threadPool.idleCount() == 1);
        threadPool.cleanupIdleThreads();
        assert(threadPool.idleCount() == 0);
    }
    catch (const Exception& e)
    {
        cout << "Exception in testCleanupIdleThread: " <<
            e.getMessage() << endl;
        assert(false);
    }
}

void testDestructWithRunningThreads()
{
    if (verbose)
    {
        cout << "testDestructWithRunningThreads" << endl;
    }

    try
    {
        struct timeval deallocateWait = { 0, 1 };
        ThreadPool threadPool(0, "test destruct", 0, 0, deallocateWait);

        threadPool.allocate_and_awaken(
            (void*)100, funcSleepSpecifiedMilliseconds);
        threadPool.allocate_and_awaken(
            (void*)200, funcSleepSpecifiedMilliseconds);
        threadPool.allocate_and_awaken(
            (void*)300, funcSleepSpecifiedMilliseconds);
        threadPool.allocate_and_awaken(
            (void*)400, funcSleepSpecifiedMilliseconds);
        threadPool.allocate_and_awaken(
            (void*)500, funcSleepSpecifiedMilliseconds);
        threadPool.allocate_and_awaken(
            (void*)600, funcSleepSpecifiedMilliseconds);
        threadPool.allocate_and_awaken(
            (void*)700, funcSleepSpecifiedMilliseconds);
        threadPool.allocate_and_awaken(
            (void*)800, funcSleepSpecifiedMilliseconds);
        threadPool.allocate_and_awaken(
            (void*)900, funcSleepSpecifiedMilliseconds);

        assert(threadPool.runningCount() > 0);
    }
    catch (const Exception& e)
    {
        cout << "Exception in testDestructWithRunningThreads: " <<
            e.getMessage() << endl;
        assert(false);
    }
}

void testOverloadPool()
{
    if (verbose)
    {
        cout << "testOverloadPool" << endl;
    }

    try
    {
        struct timeval deallocateWait = { 0, 1 };
        ThreadPool threadPool(0, "test overload", 0, 4, deallocateWait);
        ThreadStatus threadStarted;

        threadStarted = threadPool.allocate_and_awaken(
            (void*)3000, funcSleepSpecifiedMilliseconds);
        assert(threadStarted == PEGASUS_THREAD_OK);

        threadStarted = threadPool.allocate_and_awaken(
            (void*)3000, funcSleepSpecifiedMilliseconds);
        assert(threadStarted == PEGASUS_THREAD_OK);

        threadStarted = threadPool.allocate_and_awaken(
            (void*)3000, funcSleepSpecifiedMilliseconds);
        assert(threadStarted == PEGASUS_THREAD_OK);

        threadStarted = threadPool.allocate_and_awaken(
            (void*)3000, funcSleepSpecifiedMilliseconds);
        assert(threadStarted == PEGASUS_THREAD_OK);

        threadStarted = threadPool.allocate_and_awaken(
            (void*)300, funcSleepSpecifiedMilliseconds);
        assert(threadStarted == PEGASUS_THREAD_INSUFFICIENT_RESOURCES);
        
        ThreadStatus rc = PEGASUS_THREAD_OK;
        while ( (rc =threadPool.allocate_and_awaken(
            (void*)100, funcSleepSpecifiedMilliseconds)) != PEGASUS_THREAD_OK)
        {
          if (rc ==PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
            pegasus_yield();
          else
           throw Exception("Could not allocate and awaken a thread."); 
        }
    }
    catch (const Exception& e)
    {
        cout << "Exception in testOverloadPool: " << e.getMessage() << endl;
        assert(false);
    }
}

void testHighWorkload()
{
    if (verbose)
    {
        cout << "testHighWorkload" << endl;
    }

    try
    {
        AtomicInt counter = 0;

        struct timeval deallocateWait = { 0, 1 };
        ThreadPool* threadPool =
            new ThreadPool(0, "test workload", 0, 10, deallocateWait);

        for (Uint32 i = 0; i < 50; i++)
        {
	    ThreadStatus rc = PEGASUS_THREAD_OK;
            while ( (rc =threadPool->allocate_and_awaken(
                &counter, funcIncrementCounter)) != PEGASUS_THREAD_OK)
            {
		if (rc == PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
                	pegasus_yield();
	 	else
			throw Exception("Coudl not allocate a thread for counter.");	
            }
        }

        delete threadPool;

        assert(counter.value() == 50);
    }
    catch (const Exception& e)
    {
        cout << "Exception in testHighWorkload: " << e.getMessage() << endl;
        assert(false);
    }
}

void testWorkException()
{
    if (verbose)
    {
        cout << "testWorkException" << endl;
    }

    try
    {
        struct timeval deallocateWait = { 0, 1 };
        ThreadPool threadPool(0, "test exception", 0, 6, deallocateWait);

        threadPool.allocate_and_awaken((void*)1, funcThrow);
        pegasus_sleep(100);
    }
    catch (const Exception& e)
    {
        cout << "Exception in testWorkException: " << e.getMessage() << endl;
        assert(false);
    }
}

void testBlockingThread()
{
    if (verbose)
    {
        cout << "testBlockingThread" << endl;
    }

    try
    {
        struct timeval deallocateWait = { 5, 0 };
        ThreadPool threadPool(0, "test blocking", 0, 6, deallocateWait);
        Semaphore blocking(0);
	ThreadStatus rt = PEGASUS_THREAD_OK;
        while ( (rt =threadPool.allocate_and_awaken(
            (void*)16, funcSleepSpecifiedMilliseconds, &blocking)) != PEGASUS_THREAD_OK)
        {
	  if (rt == PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
            pegasus_yield();
	  else
	   throw Exception("Could not allocate thread for funcSleepSpecifiedMilliseconds function.");
        }

        blocking.wait();
        threadPool.cleanupIdleThreads();
    }
    catch (const Exception& e)
    {
        cout << "Exception in testBlockingThread: " << e.getMessage() << endl;
        assert(false);
    }
}

int main(int argc, char **argv)
{
    verbose = (getenv("PEGASUS_TEST_VERBOSE")) ? true : false;

#if defined(PEGASUS_DEBUG)
    if (verbose)
    {
        Tracer::setTraceComponents("ALL");
        Tracer::setTraceLevel(Tracer::LEVEL4);
        Tracer::setTraceFile("thread_pool.trc");
    }
#endif

    testCleanupIdleThread();
    testDestructWithRunningThreads();
    testOverloadPool();
    testWorkException();
    testHighWorkload();
    testBlockingThread();

#if defined(PEGASUS_DEBUG)
    if (verbose)
    {
        Tracer::setTraceComponents("");
    }
#endif

    testloopDestructAsThreadCompletes();

    cout << argv[0] << " +++++ passed all tests" << endl;

    return(0);
}
