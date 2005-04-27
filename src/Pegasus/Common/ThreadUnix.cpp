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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Mike Day (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////


PEGASUS_NAMESPACE_BEGIN

static sigset_t *block_signal_mask(sigset_t *sig)
{
    sigemptyset(sig);
    // should not be used for main()
    sigaddset(sig, SIGHUP);
    sigaddset(sig, SIGINT);
    // maybe useless, since KILL can't be blocked according to POSIX
    sigaddset(sig, SIGKILL);

    sigaddset(sig, SIGABRT);
    sigaddset(sig, SIGALRM);
    sigaddset(sig, SIGPIPE);


// Note: older versions of the linux pthreads library use SIGUSR1 and SIGUSR2
// internally to stop and start threads that are blocking, the newer ones
// implement this through the kernel's real time signals

// since SIGSTOP/CONT can handle suspend()/resume() on Linux
// block them
// #if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
//     sigaddset(sig, SIGUSR1);
//     sigaddset(sig, SIGUSR2);
// #endif
#ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
    pthread_sigmask(SIG_BLOCK, sig, NULL);
#else
    sigprocmask(SIG_BLOCK, sig, NULL);
#endif
    return sig;
}

#if defined PEGASUS_PLATFORM_ZOS_ZSERIES_IBM || defined PEGASUS_PLATFORM_SOLARIS_SPARC_CC
extern "C" {
    void * _linkage(void * zosParm)
    {
        zosParmDef * zos;
        void * retval;

        zos = (zosParmDef *)zosParm;
        retval = (*(zos->_start))(zos->realParm);
        free(zosParm);
        return retval;
    }
}
#endif


////////////////////////////////////////////////////////////////////////////

Thread::Thread(
    PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *start)(void *),
    void *parameter,
    Boolean detached)
    : _is_detached(detached),
      _cancel_enabled(true),
      _cancelled(false),
      _suspend_count(),
      _start(start),
      _cleanup(true),
      _tsd(true),
      _thread_parm(parameter),
      _exit_code(0)
{
    pthread_attr_init(&_handle.thatt);

#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
    size_t stacksize;
    if (pthread_attr_getstacksize(&_handle.thatt, &stacksize) == 0)
    {
       int rc = pthread_attr_setstacksize(&_handle.thatt, stacksize*2);
       PEGASUS_ASSERT(rc == 0);
    }
#endif

    _handle.thid = 0;
}

Thread::~Thread()
{
    try
    {
        join();
        pthread_attr_destroy(&_handle.thatt);

        empty_tsd();
    }
    catch (...)
    {
        // Do not allow the destructor to throw an exception
    }
}

PEGASUS_NAMESPACE_END
