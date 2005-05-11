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
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              Alagaraja Ramasubramanian (alags_raj@in.ibm.com) for Bug#1090
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//              Robert Kieninger, IBM (kieningr@de.ibm.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Aruran, IBM (ashanmug@in.ibm.com) for BUG# 3518
//
//%/////////////////////////////////////////////////////////////////////////////

#include "IPC.h"

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "IPCWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "IPCUnix.cpp"
#elif defined(PEGASUS_OS_VMS)
# include "IPCVms.cpp"
#else
# error "Unsupported platform"
#endif

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#include "IPCzOS.cpp"
#endif

#include "InternalException.h"
//#include "NativeCleanup.h"

PEGASUS_NAMESPACE_BEGIN

int timeval_subtract (struct timeval *result,
		      struct timeval *x,
		      struct timeval *y)
{
   /* Perform the carry for the later subtraction by updating Y. */
   if (x->tv_usec < y->tv_usec) {
      int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
      y->tv_usec -= 1000000 * nsec;
      y->tv_sec += nsec;
   }
   if (x->tv_usec - y->tv_usec > 1000000) {
      int nsec = (x->tv_usec - y->tv_usec) / 1000000;
      y->tv_usec += 1000000 * nsec;
      y->tv_sec -= nsec;
   }

   /* Compute the time remaining to wait.
      `tv_usec' is certainly positive. */
   result->tv_sec = x->tv_sec - y->tv_sec;
   result->tv_usec = x->tv_usec - y->tv_usec;

   /* Return 1 if result is negative. */
   return x->tv_sec < y->tv_sec;
}



//-----------------------------------------------------------------
/// Generic Implementation of read/write semaphore class
//-----------------------------------------------------------------
#ifndef PEGASUS_READWRITE_NATIVE


// // If i get cancelled, I MUST ensure:
// 1) I do not hold the internal mutex
// 2) I do not hold the write lock
// 3) I am not using a reader slot

void extricate_read_write(void *parm)
{
   ReadWriteSem *rws = (ReadWriteSem *)parm;
   PEGASUS_THREAD_TYPE myself = pegasus_thread_self();

   if(rws->_rwlock._wlock.get_owner() == myself)
      rws->_rwlock._wlock.unlock();
   else
      if(rws->_readers > 0)
         rws->_rwlock._rlock.signal();
   if (rws->_rwlock._internal_lock.get_owner() == myself)
      rws->_rwlock._internal_lock.unlock();
}


ReadWriteSem::ReadWriteSem() : _readers(0), _writers(0), _rwlock() { }

ReadWriteSem::~ReadWriteSem()
{
   // lock everyone out of this object
   try
   {
      _rwlock._internal_lock.lock(pegasus_thread_self());
   }
   catch(Deadlock& d)
   {
      d = d; // no problem - we own the lock, which is what we want
   }
   catch(IPCException& )
   {
      PEGASUS_ASSERT(0);
   }
   while(_readers.value() > 0 || _writers.value() > 0)
   {
      pegasus_yield();
   }
   _rwlock._internal_lock.unlock();
}






//-----------------------------------------------------------------
// if milliseconds == -1, wait indefinately
// if milliseconds == 0, fast wait
//-----------------------------------------------------------------
void ReadWriteSem::timed_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller, int milliseconds)
{

//-----------------------------------------------------------------
// Lock this object to maintain integrity while we decide
// exactly what to do next.
//-----------------------------------------------------------------
    //    AutoPtr<IPCException> caught;
    //IPCException  caught((PEGASUS_THREAD_TYPE)0);
    //WaitFailed caughtWaitFailed((PEGASUS_THREAD_TYPE)0);
    //TimeOut caughtTimeOut((PEGASUS_THREAD_TYPE)0);
    //TooManyReaders caughtTooManyReaders((PEGASUS_THREAD_TYPE)0);

    PEGASUS_THREAD_TYPE zero;
    zero = 0;
    IPCException caught(zero);
    WaitFailed caughtWaitFailed(zero);
    TimeOut caughtTimeOut(zero);
    TooManyReaders caughtTooManyReaders(zero);
    
   { // cleanup stack frame
      native_cleanup_push(extricate_read_write, this);
      try
      {
	 if(milliseconds == 0)
	    _rwlock._internal_lock.try_lock(pegasus_thread_self());
	 else if(milliseconds == -1)
	    _rwlock._internal_lock.lock(pegasus_thread_self());
	 else
	    _rwlock._internal_lock.timed_lock(milliseconds, pegasus_thread_self());
      }
      catch(const IPCException& e)
      {
         caught = e;
         goto throw_from_here;
      }

      if(mode == PEG_SEM_WRITE)
      {
//-----------------------------------------------------------------
// Write Lock Step 1: lock the object and allow all the readers to exit
//-----------------------------------------------------------------


	 if(milliseconds == 0) // fast wait
	 {
	    if(_readers.value() > 0)
	    {
	       _rwlock._internal_lock.unlock();
	       //caught.reset(new WaitFailed(pegasus_thread_self()));
	       caughtWaitFailed = WaitFailed(pegasus_thread_self());
	       goto throw_from_here;
	    }
	 }
	   else if(milliseconds == -1) // infinite wait
	 {
	    while(_readers.value() > 0 )
	       pegasus_yield();
	 }
	 else // timed wait
	 {
	    struct timeval start, now;
	    gettimeofday(&start, NULL);
	    start.tv_usec += (1000 * milliseconds);
	    while(_readers.value() > 0)
	    {
	       gettimeofday(&now, NULL);
	       if((now.tv_usec > start.tv_usec) || now.tv_sec > start.tv_sec )
	       {
		  _rwlock._internal_lock.unlock();
		  //caught.reset(new TimeOut(pegasus_thread_self()));
		  caughtTimeOut = TimeOut(pegasus_thread_self());
		  goto throw_from_here;
	       }
	       pegasus_yield();
	    }
	 }
//-----------------------------------------------------------------
// Write Lock Step 2: Obtain the Write Mutex
//  Although there are no readers, there may be a writer
//-----------------------------------------------------------------
	 if(milliseconds == 0) // fast wait
	 {
	    try
	    {
	       _rwlock._wlock.try_lock(pegasus_thread_self());
	    }
	    catch(IPCException& e)
	    {
	       _rwlock._internal_lock.unlock();
	       caught = e;
	       goto throw_from_here;
	    }
	 }
	 else if(milliseconds == -1) // infinite wait
	 {
	    try
	    {
	       _rwlock._wlock.lock(pegasus_thread_self());
	    }
	    catch (const IPCException& e)
	    {
	       _rwlock._internal_lock.unlock();
	       caught = e;
	       goto throw_from_here;
	    }
	 }
	 else // timed wait
	 {
	    try
	    {
	       _rwlock._wlock.timed_lock(milliseconds, pegasus_thread_self());
	    }
	    catch(const IPCException& e)
	    {
	       _rwlock._internal_lock.unlock();
	       caught = e;
	       goto throw_from_here;
	    }
	 }

//-----------------------------------------------------------------
// Write Lock Step 3: set the writer count to one, unlock the object
//   There are no readers and we are the only writer !
//-----------------------------------------------------------------
	 _writers = 1;
	 // set the owner
	 _rwlock._owner = pegasus_thread_self();
	 // unlock the object
	 _rwlock._internal_lock.unlock();
      } // PEG_SEM_WRITE
      else
      {
//-----------------------------------------------------------------
// Read Lock Step 1: Wait for the existing writer (if any) to clear
//-----------------------------------------------------------------
	 if(milliseconds == 0) // fast wait
	 {
	    if(_writers.value() > 0)
	    {
	       _rwlock._internal_lock.unlock();
	       //caught.reset(new WaitFailed(pegasus_thread_self()));
	       caughtWaitFailed = WaitFailed(pegasus_thread_self());
	       goto throw_from_here;
	    }
	 }
	 else if(milliseconds == -1) // infinite wait
	 {
	    while(_writers.value() >  0)
	       pegasus_yield();
	 }
	 else // timed wait
	 {
	    struct timeval start, now;
	    gettimeofday(&start, NULL);
	    start.tv_usec += (milliseconds * 1000);

	    while(_writers.value() > 0)
	    {
	       gettimeofday(&now, NULL);
	       if((now.tv_usec > start.tv_usec) || (now.tv_sec > start.tv_sec))
	       {
		  _rwlock._internal_lock.unlock();
		  //caught.reset(new TimeOut(pegasus_thread_self()));
		  caughtTimeOut = TimeOut(pegasus_thread_self());
		  goto throw_from_here;
	       }
	       pegasus_yield();
	       pegasus_gettimeofday(&now);
	    }
	 }

//-----------------------------------------------------------------
// Read Lock Step 2: wait for a reader slot to open up, then return
//  At this point there are no writers, but there may be too many
//  readers.
//-----------------------------------------------------------------
	 if(milliseconds == 0) // fast wait
	 {
	    try
	    {
	       _rwlock._rlock.try_wait();
	    }
	    catch(const IPCException&)
	    {
	       // the wait failed, there must be too many readers already.
	       // unlock the object
	       caughtTooManyReaders = TooManyReaders(pegasus_thread_self());
	       _rwlock._internal_lock.unlock();
	       //caught.reset(new TooManyReaders(pegasus_thread_self()));
	    }
	 }
	 else if(milliseconds == -1) // infinite wait
	 {
	    try
	    {
	       _rwlock._rlock.wait();
	    }
	    catch(const IPCException& e)
	    {
	       _rwlock._internal_lock.unlock();
	       caught = e;
	       goto throw_from_here;
	    }
	 }
	 else // timed wait
	 {
	    try
	    {
	       _rwlock._rlock.time_wait(milliseconds);
	    }
	    catch(const IPCException& e)
	    {
	       _rwlock._internal_lock.unlock();
	       caught = e;
	       goto throw_from_here;
	    }
	 }

//-----------------------------------------------------------------
// Read Lock Step 3: increment the number of readers, unlock the object,
// return
//-----------------------------------------------------------------
	 _readers++;
	 _rwlock._internal_lock.unlock();
      }
      throw_from_here:
      native_cleanup_pop(0);
   } // cleanup stack frame

   if (caught.get_owner() != 0)
      throw caught;
   if (caughtWaitFailed.get_owner() != 0)
      throw caughtWaitFailed;
   if (caughtTimeOut.get_owner() != 0)
      throw caughtTimeOut;
   if (caughtTooManyReaders.get_owner() != 0)
      throw caughtTooManyReaders;
   return;
}





//---------------------------------------------------------------------
void ReadWriteSem::wait(Uint32 mode, PEGASUS_THREAD_TYPE caller)
{
   timed_wait(mode, caller, -1);
}

void ReadWriteSem::try_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller)
{
   timed_wait(mode, caller, 0);
}


void ReadWriteSem::unlock(Uint32 mode, PEGASUS_THREAD_TYPE caller)
{
   if(mode == PEG_SEM_WRITE && _writers.value() != 0 )
   {
      _writers = 0;
      _rwlock._wlock.unlock();
   }
   else if (_readers.value() != 0 )
   {
      _readers--;
      _rwlock._rlock.signal();
   }
}

int ReadWriteSem::read_count() const

{
   return( _readers.value() );
}

int ReadWriteSem::write_count() const
{
   return( _writers.value() );
}

#endif // ! PEGASUS_READWRITE_NATIVE
//-----------------------------------------------------------------
// END of generic read/write semaphore implementaion
//-----------------------------------------------------------------



//-----------------------------------------------------------------
/// Generic Implementation of Atomic Integer class
//-----------------------------------------------------------------

#ifndef PEGASUS_ATOMIC_INT_NATIVE

AtomicInt::AtomicInt() {_rep._value = 0;  }

AtomicInt::AtomicInt(Uint32 initial) {_rep._value = initial; }

AtomicInt::~AtomicInt()
{

}

AtomicInt::AtomicInt(const AtomicInt& original)
{
    _rep._value = original._rep._value;
}

AtomicInt& AtomicInt::operator=(const AtomicInt& original )
{
    // to avoid deadlocks, always be certain to only hold one mutex at a time.
    // therefore, get the original value (which will lock and unlock the original's mutex)
    // and _then_ lock this mutex. This pattern is repeated throughout the class

    Uint32 temp = original._rep._value;
    AutoMutex autoMut(_rep._mutex);
    _rep._value = temp;

    return *this;
}

AtomicInt& AtomicInt::operator=(Uint32 val)
{
    AutoMutex autoMut(_rep._mutex);
    _rep._value = val;

    return *this;
}

Uint32 AtomicInt::value() const
{
    AutoMutex autoMut(_rep._mutex);
    Uint32 retval = _rep._value;

    return retval;
}

void AtomicInt::operator++()
{
    AutoMutex autoMut(_rep._mutex);
    _rep._value++;

}

void AtomicInt::operator++(int)
{
    AutoMutex autoMut(_rep._mutex);
    _rep._value++;

}

void AtomicInt::operator--()
{
    AutoMutex autoMut(_rep._mutex);
    _rep._value--;

}

void AtomicInt::operator--(int)
{
    AutoMutex autoMut(_rep._mutex);
    _rep._value--;

}

Uint32 AtomicInt::operator+(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex
    Uint32 retval = val._rep._value;
    AutoMutex autoMut(_rep._mutex);
    retval += _rep._value ;

    return retval;
}

Uint32 AtomicInt::operator+(Uint32 val)
{
    AutoMutex autoMut(_rep._mutex);
    Uint32 retval = _rep._value + val;

    return retval;
}

Uint32 AtomicInt::operator-(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex
    Uint32 retval =  val._rep._value;
    AutoMutex autoMut(_rep._mutex);
    retval += _rep._value;

    return retval;
}

Uint32 AtomicInt::operator-(Uint32 val)
{
    AutoMutex autoMut(_rep._mutex);
    Uint32 retval = _rep._value - val;

    return retval;
}


AtomicInt& AtomicInt::operator+=(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex
    Uint32 temp = val._rep._value;
    AutoMutex autoMut(_rep._mutex);
    _rep._value += temp;

    return *this;
}
AtomicInt& AtomicInt::operator+=(Uint32 val)
{
    // never acquire a mutex while holding a mutex
    AutoMutex autoMut(_rep._mutex);
    _rep._value += val;

    return *this;
}

AtomicInt& AtomicInt::operator-=(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex
    Uint32 temp = val._rep._value;
    AutoMutex autoMut(_rep._mutex);
    _rep._value -= temp;

    return *this;
}

AtomicInt& AtomicInt::operator-=(Uint32 val)
{
    // never acquire a mutex while holding a mutex
    AutoMutex autoMut(_rep._mutex);
    _rep._value -= val;

    return *this;
}

Boolean AtomicInt::DecAndTestIfZero()
{
    AutoMutex autoMut(_rep._mutex);
    _rep._value--;
    Boolean b = (_rep._value == 0);

    return b;
}

#endif // ! PEGASUS_ATOMIC_INT_NATIVE
//-----------------------------------------------------------------
// END of generic atomic integer implementation
//-----------------------------------------------------------------



//-----------------------------------------------------------------
// Generic implementation of conditional semaphore object
//-----------------------------------------------------------------
#ifndef PEGASUS_CONDITIONAL_NATIVE


// may be entered by many different threads concurrently
// ensure that, upon exit, I OWN the mutex
// and I DO NOT own the critical section

void extricate_condition(void *parm)
{
   Condition *c = (Condition *)parm;

   // if I own the critical section, release it
   if(pegasus_thread_self() == c->_condition._spin.get_owner())
      c->_condition._spin.unlock();
   // if I DO NOT own the mutex, obtain it
   if(pegasus_thread_self() != c->_cond_mutex->get_owner())
      c->_cond_mutex->lock(pegasus_thread_self());
}

Condition::Condition() : _disallow(0), _condition()
{
   _cond_mutex.reset(new Mutex());
   _destroy_mut = true;
}

Condition::Condition(Mutex & mutex) : _disallow(0), _condition()
{
   _cond_mutex.reset(&mutex);
   _destroy_mut = false;
}


Condition::~Condition()
{
   cond_waiter *lingerers;
   // don't allow any new waiters
   _disallow++;

   {
   AutoMutex autoMut(_condition._spin);

   while(NULL != (lingerers = static_cast<cond_waiter *>(_condition._waiters.remove_last())))
   {
      lingerers->signalled.signal();
   }
   } // mutex unlocks here

   while( _condition._waiters.count())   {
      pegasus_yield();
   }
   if(_destroy_mut == true)
      _cond_mutex.reset();
   else
      _cond_mutex.release();
}

void Condition::signal(PEGASUS_THREAD_TYPE caller)
{
   _cond_mutex->lock(caller);

   try
   {
      unlocked_signal(caller);
   }
   catch(...)
   {
      _cond_mutex->unlock();
      throw;
   }
    _cond_mutex->unlock();
}

void Condition::unlocked_signal(PEGASUS_THREAD_TYPE caller)
{
      // enforce that the caller owns the conditional lock
   if(_cond_mutex->_mutex.owner != caller)
      throw(Permission((PEGASUS_THREAD_TYPE)caller));

   // lock the internal list
   _condition._spin.lock(caller);
   if (_condition._waiters.count() > 0)
   {
      cond_waiter *waiters = static_cast<cond_waiter *>(_condition._waiters.next(0));
      while( waiters != 0)
      {
         waiters->signalled.signal();
         waiters = static_cast<cond_waiter *>(_condition._waiters.next(waiters));
      }
   }
   _condition._spin.unlock();
}

void Condition::lock_object(PEGASUS_THREAD_TYPE caller)
{
   if(_disallow.value() > 0)
      throw ListClosed();
   _cond_mutex->lock(caller);
}

void Condition::try_lock_object(PEGASUS_THREAD_TYPE caller)
{
   if(_disallow.value() > 0 )
      throw ListClosed();
   _cond_mutex->try_lock(caller);
}

void Condition::wait_lock_object(PEGASUS_THREAD_TYPE caller, int milliseconds)
{
   if(_disallow.value() > 0)
      throw ListClosed();
   _cond_mutex->timed_lock(milliseconds, caller);
}

void Condition::unlock_object()
{
   _cond_mutex->unlock();
}

void Condition::unlocked_wait(PEGASUS_THREAD_TYPE caller)
{
   unlocked_timed_wait(-1, caller);
}

void Condition::unlocked_timed_wait(int milliseconds, PEGASUS_THREAD_TYPE caller)
{
   if(_disallow.value() > 0)
   {
      _cond_mutex->unlock();
      throw ListClosed();
   }
   // enforce that the caller owns the conditional lock
   if(_cond_mutex->_mutex.owner != caller)
      throw Permission((PEGASUS_THREAD_TYPE)caller);
   cond_waiter *waiter = new cond_waiter(caller, milliseconds);
   {
      native_cleanup_push(extricate_condition, this);
      // lock the internal list
      _condition._spin.lock(caller);
      _condition._waiters.insert_first(waiter);
      // unlock the condition mutex
      _cond_mutex->unlock();
      _condition._spin.unlock();
      if(milliseconds == -1)
         waiter->signalled.wait();
      else
	 try
	 {
	    waiter->signalled.time_wait(milliseconds);
	 }
	 catch(const TimeOut &)
	 {
	    _cond_mutex->lock(caller);
	    throw;
	 }

      _condition._spin.lock(caller);
      _condition._waiters.remove(waiter);
      _condition._spin.unlock();
      delete waiter;
      _cond_mutex->lock(caller);
      native_cleanup_pop(0);
   }
   return;
}




#endif // PEGASUS_CONDITIONAL_NATIVE
//-----------------------------------------------------------------
// END of generic implementation of conditional semaphore object
//-----------------------------------------------------------------

PEGASUS_NAMESPACE_END
