//%//////////-*-c++-*-//////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

// this extra include file is needed, because zOS uses a char[8] for its
// thread-id and refuses to accept the long int _handle.thid directly

#ifndef ThreadzOS_inline_h
#define ThreadzOS_inline_h

inline void Thread::run()
{
    if (_is_detached)
    {
        int ds = 1;
        pthread_attr_setdetachstate(&_handle.thatt, &ds);
    }
    pthread_create((pthread_t *)&_handle.thid, &_handle.thatt, _start, this);
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
  //sched_yield();
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
  pthread_setintrtype(PTHREAD_INTR_CONTROLLED);
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
