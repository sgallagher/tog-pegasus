//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/Thread.h>

#if defined(PEGASUS_OS_TYPE_UNIX)
# include <unistd.h>
# if defined(PEGASUS_HAS_SIGNALS)
#  include <sys/wait.h>
# endif
#endif

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean verbose = false;

static Mutex forkSafeMutex;
PEGASUS_FORK_SAFE_MUTEX(forkSafeMutex)

ThreadReturnType PEGASUS_THREAD_CDECL mutexLockThread(void* parm)
{
    if (verbose)
    {
        cout << "mutexLockThread locking" << endl;
    }

    forkSafeMutex.lock();

    if (verbose)
    {
        cout << "mutexLockThread got lock" << endl;
    }

    Threads::sleep(1000);

    if (verbose)
    {
        cout << "mutexLockThread unlocking" << endl;
    }

    forkSafeMutex.unlock();

    return 0;
}

#if defined(PEGASUS_OS_TYPE_UNIX)
void testForkSafeMutex()
{
    Thread th1(mutexLockThread, 0, false);
    th1.run();
    Threads::sleep(100);

    pid_t forkStatus = fork();
    if (forkStatus < 0)
    {
        cerr << "fork() error " << errno << endl;
        PEGASUS_ASSERT(0);
    }
    else if (forkStatus == 0)
    {
        // Child

        if (verbose)
        {
            cout << "Child process locking" << endl;
        }

        forkSafeMutex.lock();

        if (verbose)
        {
            cout << "Child process got lock" << endl;
        }

        forkSafeMutex.unlock();

        if (verbose)
        {
            cout << "Child process unlocking" << endl;
        }

        exit(0);
    }
    else
    {
        // Parent
#if defined(PEGASUS_HAS_SIGNALS)
        // Make sure the child process exits
        pid_t status = 0;
        do
        {
            status = waitpid(forkStatus, 0, 0);
        } while ((status == -1) && (errno == EINTR));

        PEGASUS_TEST_ASSERT(status != -1);
#endif
    }

    th1.join();
}
#endif

void test1(int argc, char** argv)
{
    verbose = (getenv("PEGASUS_TEST_VERBOSE")) ? true : false;

    try
    {
#if defined(PEGASUS_OS_TYPE_UNIX) && \
    !(defined(PEGASUS_OS_ZOS) && (__TARGET_LIB__ < 0x41090000))
        testForkSafeMutex();
#endif
    }
    catch (Exception& e)
    {
        cerr << argv[0] << " Exception " << e.getMessage() << endl;
        exit(1);
    }
}

static Mutex _mutex;
static int _count = 0;
static const int _ITERATIONS = 1000;

ThreadReturnType PEGASUS_THREAD_CDECL _thread(void*)
{
    for (int i = 0; i < _ITERATIONS; i++)
    {
        _mutex.lock();
        int count = _count;
        _count = rand() % _ITERATIONS;
        _count = rand() % _ITERATIONS;
        Threads::yield();
        count++;
        _count = count;
        _mutex.unlock();
        Threads::yield();
    }

    return 0;
}

void test2()
{
    _count = 0;
    const size_t N = 5;
    Thread* threads[N];

    for (size_t i = 0; i < N; i++)
        threads[i] = new Thread(_thread, 0, false);

    for (size_t i = 0; i < N; i++)
        threads[i]->run();

    for (size_t i = 0; i < N; i++)
        threads[i]->join();

    for (size_t i = 0; i < N; i++)
        delete threads[i];

    PEGASUS_TEST_ASSERT(_count == 5 * _ITERATIONS);
}

void test3()
{
    _count = 0;
    Thread th1(_thread, 0, false);
    Thread th2(_thread, 0, false);
    Thread th3(_thread, 0, false);
    Thread th4(_thread, 0, false);
    Thread th5(_thread, 0, false);

    th1.run();
    th2.run();
    th3.run();
    th4.run();
    th5.run();

    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();

    PEGASUS_TEST_ASSERT(_count == 5 * _ITERATIONS);
}

int main(int argc, char** argv)
{
#if 0
    test1(argc, argv);
    test2();
#endif
    test3();

    _mutex.lock();
    _mutex.unlock();

    cout << argv[0] << " +++++ passed all tests" << endl;
    return 0;
}
