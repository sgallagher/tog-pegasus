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

#include "Threads.h"
#include "IDFactory.h"
#include "TSDKey.h"

PEGASUS_NAMESPACE_BEGIN

void Threads::sleep(int msec)
{
#if defined(PEGASUS_HAVE_NANOSLEEP)

    struct timespec wait;
    wait.tv_sec = msec / 1000;
    wait.tv_nsec = (msec % 1000) * 1000000;
    nanosleep(&wait, NULL);

#elif defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)

   int loop;
   int microsecs = msec * 1000; /* convert from milliseconds to microseconds */

   if (microsecs < 1000000)
       usleep(microsecs);
   else
   {
       loop = microsecs / 1000000;
       for(int i = 0; i < loop; i++)
           usleep(1000000);
       if ((loop*1000000) < microsecs)
           usleep(microsecs - (loop*1000000));
   }

#elif defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)

    if (ms == 0)
    {         
        Sleep(0);
        return;
    }

    struct _timeb end, now;
    _ftime( &end );
    end.time += (ms / 1000);
    ms -= (ms / 1000);
    end.millitm += ms;

    do
    {
        Sleep(0);
        _ftime(&now);
    } 
    while( end.millitm > now.millitm && end.time >= now.time);

#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
    int seconds;
    int microsecs = msec * 1000;

    if (microsecs < 1000000)
    {
        usleep(microsecs/2);
        pthread_testintr();
        usleep(microsecs/2);
    }
    else
    {
        // sleep for loop seconds
        seconds = microsecs / 1000000;
        sleep(seconds);

        // Usleep the remaining time
        if ((seconds*1000000) < microsecs)
            usleep(microsecs - (seconds*1000000));
    }
#elif defined(PEGASUS_OS_VMS)

    sleep(msec / 1000);

#endif
}

//==============================================================================
//
// Thread id TSD:
//
//==============================================================================

static MutexType _mutex = PEGASUS_MUTEX_INITIALIZER;
static int _initialized;
static TSDKeyType _key;

static void _init_id_tsd()
{
    mutex_lock(&_mutex);

    if (_initialized == 0)
    {
        TSDKey::create(&_key);
        _initialized = 1;
    }

    mutex_unlock(&_mutex);
}

static inline void _set_id_tsd(Uint32 id)
{
    if (_initialized == 0)
        _init_id_tsd();

    TSDKey::set_thread_specific(_key, (void*)(long)id);
}

static inline Uint32 _get_id_tsd()
{
    if (_initialized == 0)
        _init_id_tsd();

    void* ptr = TSDKey::get_thread_specific(_key);

    if (!ptr)
    {
        // Main thread's id is 1!
        return 1;
    }

    return (Uint32)(long)ptr;
}

//==============================================================================
//
// PEGASUS_HAVE_PTHREADS
//
//==============================================================================

static IDFactory _thread_ids(2); /* 1 reserved for main thread */

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

    if (type == DETACHED)
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

#if defined(PEGASUS_PLATFORM_SOLARIS_SPARC_GNU) || \
    defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC)
# if defined SUNOS_5_7
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
# else
    pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
# endif
#endif // PEGASUS_PLATFORM_SOLARIS_SPARC_GNU

    // Create thread:

    pthread_t thr;
    int rc = pthread_create(&thr, &attr, start, arg);

    if (rc != 0)
    {
        thread = ThreadType();
        return rc;
    }

    // Assign thread id (and put into thread specific storage).

    Uint32 id = _thread_ids.getID();
    _set_id_tsd(id);

    // Return:

    thread = ThreadType(thr, id);
    return 0;
}

ThreadType Threads::self() 
{
    return ThreadType(pthread_self(), _get_id_tsd());
}

#endif /* PEGASUS_HAVE_PTHREADS */

PEGASUS_NAMESPACE_END
