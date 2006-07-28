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
// Modified By: Sean Keenan (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef ThreadVms_inline_h
#define ThreadVms_inline_h

inline ThreadStatus Thread::run()
{
    Threads::Type type = _is_detached ? Threads::DETACHED : Threads::JOINABLE;
    int rc = Threads::create(_handle.thid, type, _start, this);

    /* On VMS the return code when there is inssuficient resources to create
    a thread is ENOMEM. The POSIX standard defines that it should be EAGAIN,
    hence we checking both values.

    For more details:
     http://aether.lbl.gov/htbin/helpgate/HELP/DECTHREADS/PTHREAD_ROUTINES/PTHREAD_CREATE/RETURN_VALUES
    */
    if ((rc == EAGAIN) || (rc==ENOMEM))
    {
        _handle.thid.clear();
        return PEGASUS_THREAD_INSUFFICIENT_RESOURCES;
    }
    else if (rc != 0)
    {
        _handle.thid.clear();
	return PEGASUS_THREAD_SETUP_FAILURE;
    }
    return PEGASUS_THREAD_OK;
}


inline void Thread::cancel()
{
   _cancelled = true;
   pthread_cancel(_handle.thid.thread());
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

inline void Thread::sleep(Uint32 msec)
{
   Threads::sleep(msec);
}

inline void Thread::join(void) 
{ 
   if((! _is_detached) && (_handle.thid.id() != 0))
      pthread_join(_handle.thid.thread(), &_exit_code) ; 
   _handle.thid.clear();
}

inline void Thread::thread_init(void)
{
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
  _cancel_enabled = true;
}

inline void Thread::detach(void)
{
   _is_detached = true;
   pthread_detach(_handle.thid.thread());
}

#endif // ThreadVms_inline_h
