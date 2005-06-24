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
// Author: Markus Mueller (markus_mueller@de.ibm.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

// this extra include file is needed, because zOS uses a char[8] for its
// thread-id and refuses to accept the long int _handle.thid directly

#ifndef ThreadzOS_inline_h
#define ThreadzOS_inline_h

typedef struct {                                   
    void * (PEGASUS_THREAD_CDECL * _start)(void *);
    void * realParm;                               
} zosParmDef;                                      

extern "C" { void * _linkage(void * zosParm); };
                                                   
inline ThreadStatus Thread::run()
{
    zosParmDef * zosParm = (zosParmDef *)malloc(sizeof(zosParmDef));
    zosParm->_start = _start;
    zosParm->realParm = (void *) this;
    if (_is_detached)
    {
        int ds = 1;
        pthread_attr_setdetachstate(&_handle.thatt, &ds);
    }

    int rc;
    rc = pthread_create((pthread_t *)&_handle.thid,
                        &_handle.thatt, &_linkage, zosParm);
    if (rc == EAGAIN)
    {
        _handle.thid = 0;
        return PEGASUS_THREAD_INSUFFICIENT_RESOURCES;
    }
    else if (rc != 0)
    {
        _handle.thid = 0;
	return PEGASUS_THREAD_SETUP_FAILURE;
    }
    return PEGASUS_THREAD_OK;
}

inline void Thread::cancel()
{
   _cancelled = true;
   pthread_cancel(*(pthread_t *)&_handle.thid);
}

inline void Thread::test_cancel()
{
  pthread_testintr();
}

inline Boolean Thread::is_cancelled(void)
{
   return _cancelled;
}

inline void Thread::thread_switch()
{
  pthread_yield(NULL);
}

inline void Thread::sleep(Uint32 msec)
{
   pegasus_sleep(msec);
}

inline void Thread::join(void) 
{ 
   if((! _is_detached) && (_handle.thid != 0))
      pthread_join(*(pthread_t *)&_handle.thid, &_exit_code) ; 
   _handle.thid = 0;
}

inline void Thread::thread_init(void)
{
  pthread_setintr(PTHREAD_INTR_ENABLE);
  pthread_setintrtype(PTHREAD_INTR_ASYNCHRONOUS);
  _cancel_enabled = true;
}

// *****----- native thread exit routine -----***** //


// *****----- native cleanup routines -----***** //

inline void Thread::detach(void)
{
   _is_detached = true;
   pthread_detach((pthread_t *)&_handle.thid);
}

#endif // ThreadzOS_inline_h
