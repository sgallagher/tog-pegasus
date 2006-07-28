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
// Author: Mike Brasher (m.brasher@inovadevelopment.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Threads_h
#define Pegasus_Threads_h

#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>

// ATTN: can we consolidate these someplace?

#if defined(PEGASUS_HAVE_PTHREADS)
# if defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
# define _MULTI_THREADED // Is this really necessary?
# endif
# include <pthread.h>
# include <errno.h>
# include <sys/time.h>
#elif defined(PEGASUS_HAVE_WINDOWS_THREADS)
# include <windows.h>
# include <process.h>
#else
# error "<Pegasus/Common/Threads.h>: not implemented"
#endif

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// PEGASUS_THREAD_CDECL
//
//==============================================================================

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# define PEGASUS_THREAD_CDECL __stdcall
#else
# define PEGASUS_THREAD_CDECL /* empty */
#endif

//==============================================================================
//
// Thread-related type definitions
//
//==============================================================================

#if defined(PEGASUS_HAVE_PTHREADS)

struct ThreadType
{
public:

    ThreadType()
    {
        memset(this, 0, sizeof(*this));
    }

    ThreadType(const ThreadType& x) : _thread(x._thread), _id(x._id) 
    {
    }

    ThreadType(pthread_t thread, Uint32 id) : _thread(thread), _id(id) 
    {
    }

    ThreadType& operator=(const ThreadType& x)
    {
        if (&x != this)
        {
            _id = x._id;
            _thread = x._thread;
        }
        return *this;
    }

    pthread_t handle() const 
    {
        return _thread; 
    }

    void print() const
    {
        printf("ThreadType(%lu, %lu)\n", _thread, (unsigned long)_id);
    }

private:
    pthread_t _thread;

    // And id of zero indicates a null object. 1 indicates the main thread.
    Uint32 _id;
    friend class Threads;
};

typedef void* ThreadReturnType;
#endif

#if defined(PEGASUS_HAVE_WINDOWS_THREADS)
struct ThreadType
{
    HANDLE handle;
};
typedef unsigned ThreadReturnType;
#endif

//==============================================================================
//
// ThreadHandle
//
//==============================================================================

#if defined(PEGASUS_HAVE_PTHREADS)
struct ThreadHandle
{
    ThreadType thid;
};
#elif defined(PEGASUS_HAVE_WINDOWS_THREADS)
struct ThreadHandle
{
    ThreadType thid;
};
#endif

//==============================================================================
//
// Threads
//
//==============================================================================

class PEGASUS_COMMON_LINKAGE Threads
{
public:

    enum Type { DETACHED, JOINABLE };

    static int create(
        ThreadType& thread, 
        Type type,
        void* (*start)(void*), 
        void* arg);

    static ThreadType self();

    static bool equal(ThreadType x, ThreadType y);

    static void exit(ThreadReturnType rc);

    static void cancel(ThreadType th, ThreadReturnType rc);

    static void yield();

    static void sleep(int msec);

    static void cleanup_push(void (*start)(void*), void* arg);

    static void cleanup_pop(int execute);

    static Uint32 id(const ThreadType& x = Threads::self());

    static void clear(ThreadType& x);
};

//==============================================================================
//
// POSIX Threads Implementation
//
//==============================================================================

#if defined(PEGASUS_HAVE_PTHREADS)

inline bool Threads::equal(ThreadType x, ThreadType y) 
{ 
    return pthread_equal(x.thread(), y.thread());
}

inline void Threads::exit(ThreadReturnType rc)
{
    pthread_exit(rc);
}

inline void Threads::cancel(ThreadType th, ThreadReturnType rc)
{
    pthread_cancel(th.thread());
}

inline void Threads::yield()
{
#if defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) || \
    defined(PEGASUS_PLATFORM_HPUX_ACC) || \
    defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM) || \
    defined(PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX) || \
    defined(PEGASUS_OS_VMS)
    sched_yield();
#else
    pthread_yield();
#endif
}

inline void Threads::cleanup_push(void (*func)(void*), void* arg)
{
    // ATTN: it is doubtful whether cleanup handlers ever really worked.
    //       They are only used in two places and not used in many other
    //       places where mutexes are obtained. Further, they are only
    //       implemented correctly on one or two platforms. For now, we
    //       will defer their implementation until we can find a way to
    //       implement them on all platforms (using thread local storage).
}

inline void Threads::cleanup_pop(int execute)
{
    // ATTN: not implemented.
}

inline Uint32 Threads::id(const ThreadType& x)
{
    return x._id;
}

inline void Threads::clear(ThreadType& x)
{
    memset(&x, 0, sizeof(x));
}

#endif /* defined(PEGASUS_HAVE_PTHREADS) */

//==============================================================================
//
// Windows Threads Implementation
//
//==============================================================================

#if defined(PEGASUS_HAVE_WINDOWS_THREADS)

inline ThreadType Threads::self() 
{
    ThreadType tt;
    tt.handle = HANDLE(GetCurrentThreadId());
    return tt;
}

inline bool Threads::equal(ThreadType x, ThreadType y) 
{
    return x.handle == y.handle;
}

inline void Threads::exit(ThreadReturnType rc)
{
    _endthreadex(rc);
}

inline void Threads::cancel(ThreadType th, ThreadReturnType rc)
{
    TerminateThread(th.handle, rc);
}

inline void Threads::yield()
{
    Sleep(0);
}

inline void Threads::cleanup_push(void (*func)(void*), void* arg)
{
    // ATTN: Not implemented on Windows.
}

inline void Threads::cleanup_pop(int execute)
{
    // ATTN: Not implemented on Windows.
}

inline Uint32 Threads::id(const ThreadType& x)
{
    return (Uint32)(long)x.handle;
}

inline void Threads::clear(ThreadType& x)
{
    x.handle = NULL;
}

#endif /* defined(PEGASUS_HAVE_WINDOWS_THREADS) */

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Threads_h */
