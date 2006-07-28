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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef ThreadUnix_inline_h
#define ThreadUnix_inline_h

#include <signal.h>

#ifdef PEGASUS_PLATFORM_SOLARIS_SPARC_CC
// _start wrapper to overcome "C" "C++" binding warnings
// (code "borrowed" from ThreadzOS_inline.h)
// Actually the Solaris compiler doesn't need this as "C" "C++"
// bindings are the same, but it moans like hell about it !!
// (Its correct to moan, but its a pain all the same).

/*
************************************************************************
*************************************************************************
revision 1.12
date: 2003/08/06 13:58:15;  author: keith.petley;  state: Exp;  lines: +30 -0
 - The Solaris compiler moans that pthread_create is expecting a function with
   "C" linkage as the third parameter and we are passing one with "C++" linkage.
   Not only is this true it appears the compiler is correct to complain, even
   though "C" and "C++" linkage are the same on Solaris.

   Added a structure zosParmDef to hold the desired function and its parameter.
   The pass a wrapper function with "C" linkage to pthread_create that simply
   calls the function in zosParmDef with the included parameter.

   Note the structure is malloc'ed in Thread::run, but _must_ be free'd by the
   newly created thread. This is may not be pretty, but it does avoid some nasty
   race conditions.

   zosParmDef shamelessly "borrowed" from the IBM zOS port.

 - On Solaris 5.8 the IPC test (and some others) would deadlock with the
   process consuming large amounts of CPU time. It turns out the "dqe" thread
   in the IPC test has a "hard" loop checking for messages on the queue. If
   the main thread and one of the dqe threads ended up on the same LWP process
   then the dqe would spin and never get pre-empted. This locked out the main
   thread which never got a chance to stop the dqe thread.

   Changed the scheduling policy from SCHED_OTHER to SCHED_RR for Solaris. This
   seems to fix the IPC deadlock by pre-empting dqe after a set time.

 - All the above wrapped in an #ifdef SOLARIS so it doesn't affect other
   platforms.

*************************************************************************
*************************************************************************
*/


typedef struct {                                   
    void * (PEGASUS_THREAD_CDECL * _start)(void *);
    void * realParm;                               
} zosParmDef;                                      

extern "C" { void * _linkage(void * zosParm); }
                                                   
inline ThreadStatus Thread::run()
{
    zosParmDef * zosParm = (zosParmDef *)malloc(sizeof(zosParmDef));
    zosParm->_start = _start;
    zosParm->realParm = (void *) this;

    Threads::Type type = _is_detached ? Threads::DETACHED : Threads::JOINABLE;
    int rc = Threads::create(_handle.thid, type, &_linkage, zosParm);

   /* On Sun Solaris, the manpage states that 'ENOMEM' is the error
      code returned when there is no insufficient memory, but the 
      POSIX standard mentions EAGAIN as the proper return code, so
      we checking for both. */ 
    if ((rc == EAGAIN) || (rc==ENOMEM))
    {
        Threads::clear(_handle.thid);
        return PEGASUS_THREAD_INSUFFICIENT_RESOURCES;
    }
    else if (rc != 0)
    {
	// The error code can be retrieved from  'errno'.
        Threads::clear(_handle.thid);
	return PEGASUS_THREAD_SETUP_FAILURE;
    }
    return PEGASUS_THREAD_OK;
}
#else // PEGASUS_PLATFORM_SOLARIS_SPARC_CC
inline ThreadStatus Thread::run()
{
    Threads::Type type = _is_detached ? Threads::DETACHED : Threads::JOINABLE;
    int rc = Threads::create(_handle.thid, type, _start, this);

   /* On Linux distributions released prior 2005, the 
      implementation of Native POSIX Thread Library 
      returns ENOMEM instead of EAGAIN when there are no 
      insufficient memory.  Hence we are checking for both. 

      More details can be found : http://sources.redhat.com/bugzilla/show_bug.cgi?id=386
    */
    if ((rc == EAGAIN) || (rc == ENOMEM))
    {
        Threads::clear(_handle.thid);
        return PEGASUS_THREAD_INSUFFICIENT_RESOURCES;
    }
    else if (rc != 0)
    {
        Threads::clear(_handle.thid);
	return PEGASUS_THREAD_SETUP_FAILURE;
    }
    return PEGASUS_THREAD_OK;
}
#endif // PEGASUS_PLATFORM_SOLARIS_SPARC_CC


inline void Thread::cancel()
{
   _cancelled = true;
   pthread_cancel(_handle.thid.handle());
}

inline void Thread::test_cancel()
{
  pthread_testcancel();
}

inline Boolean Thread::is_cancelled(void)
{
   return _cancelled;
}

inline void Thread::thread_switch()
{
  sched_yield();
}

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
inline void Thread::suspend()
{
    pthread_kill(_handle.thid.handle(),SIGSTOP);
}

inline void Thread::resume()
{
    pthread_kill(_handle.thid.handle(),SIGCONT);
}
#endif


inline void Thread::sleep(Uint32 msec)
{
   Threads::sleep(msec);
}

inline void Thread::join(void) 
{ 
   if((! _is_detached) && (Threads::id(_handle.thid) != 0))
      pthread_join(_handle.thid.handle(), &_exit_code) ; 
   Threads::clear(_handle.thid);
}

inline void Thread::thread_init(void)
{
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
  _cancel_enabled = true;
}

// *****----- native thread exit routine -----***** //

#if defined(PEGASUS_PLATFORM_HPUX_ACC) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
#define PEGASUS_THREAD_EXIT_NATIVE 
inline void Thread::exit_self(void *return_code)
{
    // NOTE: pthread_exit exhibits unusual behavior on RHEL 3 U2, as
    // documented in Bugzilla 3836.  Where feasible, it may be advantageous
    // to avoid using this function.
    pthread_exit(return_code);
}
#endif

inline void Thread::detach(void)
{
   _is_detached = true;
   pthread_detach(_handle.thid.handle());
}

#endif // ThreadUnix_inline_h
