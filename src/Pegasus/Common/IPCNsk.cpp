//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//   FILE        IPCNsk.cpp
//
//   PURPOSE     IPC NSK dummy class to satisfy the PEGASUS Code.
//
//   REVISION HISTORY
//
//   Date      Release   Revisor           Revision
//   04NOV01   1         Rudy Schuet       Initial creation
//
// ***************************************************************************
//                                                                             

int gettimeofday (struct timeval *, struct timezone *) { return 0; }

PEGASUS_NAMESPACE_BEGIN

inline void disable_cancel(void) {}
inline void enable_cancel(void) {}
void native_cleanup_push( void (*)(void *), void * ) {}
void native_cleanup_pop(Boolean execute) {}
inline void init_crit(PEGASUS_CRIT_TYPE *crit) {}
inline void enter_crit(PEGASUS_CRIT_TYPE *crit) {}
inline void try_crit(PEGASUS_CRIT_TYPE *crit) {}
inline void destroy_crit(PEGASUS_CRIT_TYPE *crit) {}
inline void exit_crit(PEGASUS_CRIT_TYPE *crit) {}
PEGASUS_THREAD_TYPE pegasus_thread_self(void) {}
void exit_thread(PEGASUS_THREAD_RETURN rc) {}
void sleep(int ms) {}
inline void pegasus_yield(void) {}
static int pegasus_gettimeofday(struct timeval *tv) { return 0; }

Mutex::Mutex() {}

Mutex::Mutex(int type) {}
Mutex::~Mutex() {}
void Mutex::lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed) {}
void Mutex::try_lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, AlreadyLocked,
   {}
void Mutex::timed_lock( Uint32 milliseconds , PEGASUS_THREAD_TYPE caller)
  throw(Deadlock, TimeOut, WaitFailed) {}
void Mutex::unlock() throw(Permission) {}

Semaphore::Semaphore(Uint32 initial) {}
Semaphore::~Semaphore() {}
void Semaphore::wait(void) {}
void Semaphore::try_wait(void) throw(WaitFailed) {}
void Semaphore::time_wait( Uint32 milliseconds ) throw(TimeOut) {}
void Semaphore::signal() {}

 PEGASUS_NAMESPACE_END
