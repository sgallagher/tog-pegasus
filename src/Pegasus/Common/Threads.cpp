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

#include <errno.h>
#include "Threads.h"
#include "IDFactory.h"
#include "TSDKey.h"
#include "Once.h"

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include <sys/timeb.h>
#endif
#if defined(PEGASUS_OS_ZOS)
# include <unistd.h>
#endif

#if defined(PEGASUS_OS_ZOS)
# include <unistd.h>
#endif

#if defined(PEGASUS_PLATFORM_SOLARIS_IX86_CC) || \
    defined(PEGASUS_PLATFORM_SOLARIS_X86_64_CC)
# include <unistd.h>
#endif

PEGASUS_NAMESPACE_BEGIN

void Threads::sleep(int msec)
{
#if defined(PEGASUS_HAVE_NANOSLEEP)

    struct timespec wait, remwait;
    wait.tv_sec = msec / 1000;
    wait.tv_nsec = (msec % 1000) * 1000000;

    while ((nanosleep(&wait, &remwait) == -1) && (errno == EINTR))
    {
        wait.tv_sec = remwait.tv_sec;
        wait.tv_nsec = remwait.tv_nsec;
    }

#elif defined(PEGASUS_OS_TYPE_WINDOWS)

    if (msec == 0)
    {
        Sleep(0);
        return;
    }

    struct _timeb end, now;
    _ftime( &end );
    end.time += (msec / 1000);
    msec -= (msec / 1000);
    end.millitm += msec;

    do
    {
        Sleep(0);
        _ftime(&now);
    }
    while (end.millitm > now.millitm && end.time >= now.time);

#else
    if (msec < 1000)
    {
        usleep(msec*1000);
    }
    else
    {
        // sleep for loop seconds
        ::sleep(msec / 1000);
        // Usleep the remaining micro seconds
        usleep( (msec*1000) % 1000000 );
    }

#endif
}

//==============================================================================
//
// _get_stack_multiplier()
//
//==============================================================================

static inline int _get_stack_multiplier()
{
#if defined(PEGASUS_OS_VMS)

    static int _multiplier = 0;
    static MutexType _multiplier_mutex = PEGASUS_MUTEX_INITIALIZER;

    //
    // This code uses a, 'hidden' (non-documented), VMS only, logical
    //  name (environment variable), PEGASUS_VMS_THREAD_STACK_MULTIPLIER,
    //  to allow in the field adjustment of the thread stack size.
    //
    // We only check for the logical name once to not have an
    //  impact on performance.
    //
    // Note:  This code may have problems in a multithreaded environment
    //  with the setting of doneOnce to true.
    //
    // Current code in Cimserver and the clients do some serial thread
    //  creations first so this is not a problem now.
    //

    if (_multiplier == 0)
    {
        mutex_lock(&_multiplier_mutex);

        if (_multiplier == 0)
        {
            const char *env = getenv("PEGASUS_VMS_THREAD_STACK_MULTIPLIER");

            if (env)
                _multiplier = atoi(env);

            if (_multiplier == 0)
                _multiplier = 2;
        }

        mutex_unlock(&_multiplier_mutex);
    }

    return _multiplier;
#elif defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
    return 2;
#else
    return 1;
#endif
}

//==============================================================================
//
// PEGASUS_HAVE_PTHREADS
//
//==============================================================================

#if defined(PEGASUS_HAVE_PTHREADS)

int Threads::create(
    ThreadType& thread,
    Type type,
    void* (*start)(void*),
    void* arg)
{
    // Initialize thread attributes:

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // Detached:

    if (type == DETACHED)
    {
#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
        int ds = 1;
        pthread_attr_setdetachstate(&attr, &ds);
#else
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
#endif
    }

    // Stack size:

    int multiplier = _get_stack_multiplier();

    if (multiplier != 1)
    {
        size_t stacksize;

        if (pthread_attr_getstacksize(&attr, &stacksize) == 0)
        {
            int rc = pthread_attr_setstacksize(&attr, stacksize * multiplier);
            PEGASUS_ASSERT(rc == 0);
        }
    }

    // Scheduling policy:

#if defined(PEGASUS_PLATFORM_SOLARIS_SPARC_GNU) || \
    defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC)
# if (defined SUNOS_5_7)
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
# else
    pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
# endif
#endif // PEGASUS_PLATFORM_SOLARIS_SPARC_GNU

    // Create thread:

    int rc = pthread_create(&thread.thread, &attr, start, arg);

    if (rc != 0)
    {
        thread = ThreadType();
        return rc;
    }

    // Destroy attributes now.

    pthread_attr_destroy(&attr);

    // Return:

    return 0;
}

ThreadType Threads::self()
{
    ThreadType tt;
    tt.thread = pthread_self();
    return tt;
}

#endif /* PEGASUS_HAVE_PTHREADS */

PEGASUS_NAMESPACE_END
