//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Mike Day (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "IPC.h"

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "IPCWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "IPCUnix.cpp"
#else
# error "Unsupported platform"
#endif


PEGASUS_NAMESPACE_BEGIN

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


ReadWriteSem::ReadWriteSem(void) : _readers(0), _writers(0), _rwlock() {}

ReadWriteSem::~ReadWriteSem(void)
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
   catch(IPCException& e) 
   {
      e = e; // oops - this is really bad
      PEGASUS_ASSERT(0); 
   }
   while(_readers > 0 || _writers > 0) 
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
   throw(TimeOut, Deadlock, Permission, WaitFailed, TooManyReaders)
{

//-----------------------------------------------------------------
// Lock this object to maintain integrity while we decide 
// exactly what to do next.
//-----------------------------------------------------------------
   IPCException * caught = NULL;
   
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
      catch(IPCException& e)
      {
	 caught = &e;
	 goto throw_from_here;
      }
      
      if(mode == PEG_SEM_WRITE)
      {
//-----------------------------------------------------------------
// Write Lock Step 1: lock the object and allow all the readers to exit
//-----------------------------------------------------------------


	 if(milliseconds == 0) // fast wait
	 {
	    if(_readers > 0)
	    {
	       _rwlock._internal_lock.unlock();
	       caught = new WaitFailed(pegasus_thread_self());
	       goto throw_from_here;
	    }
	 }
	 else if(milliseconds == -1) // infinite wait
	 {
	    while(_readers > 0 )
	       pegasus_yield();
	 }
	 else // timed wait 
	 {
	    struct timeval start, now;
	    gettimeofday(&start, NULL);
	    start.tv_usec += (1000 * milliseconds);
	    while(_readers > 0)
	    {
	       gettimeofday(&now, NULL);
	       if((now.tv_usec > start.tv_usec) || now.tv_sec > start.tv_sec )
	       {
		  _rwlock._internal_lock.unlock();
		  caught = new TimeOut(pegasus_thread_self());
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
	    catch(IPCException e) 
	    {
	       _rwlock._internal_lock.unlock();
	       caught = &e;
	       goto throw_from_here;
	    }
	 }
	 else if(milliseconds == -1) // infinite wait
	 {
	    try 
	    {
	       _rwlock._wlock.lock(pegasus_thread_self());
	    }
	    catch (IPCException& e) 
	    {
	       _rwlock._internal_lock.unlock();
	       caught = &e;
	       goto throw_from_here;
	    }
	 }
	 else // timed wait
	 {
	    try
	    {
	       _rwlock._wlock.timed_lock(milliseconds, pegasus_thread_self());
	    }
	    catch(IPCException& e)
	    {
	       _rwlock._internal_lock.unlock();
	       caught = &e;
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
	    if(_writers > 0)
	    {
	       _rwlock._internal_lock.unlock();
	       caught = new WaitFailed(pegasus_thread_self());
	       goto throw_from_here;
	    }
	 }
	 else if(milliseconds == -1) // infinite wait
	 {
	    while(_writers > 0)
	       pegasus_yield(); 
	 }
 	 else // timed wait
	 {
	    struct timeval start, now;
	    gettimeofday(&start, NULL);
	    start.tv_usec += (milliseconds * 1000);
	    
	    while(_writers > 0)
	    {
	       gettimeofday(&now, NULL);
	       if((now.tv_usec > start.tv_usec) || (now.tv_sec > start.tv_sec))
	       {
		  _rwlock._internal_lock.unlock();
		  caught = new TimeOut(pegasus_thread_self());
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
	    catch(IPCException& e) 
	    {
	       // the wait failed, there must be too many readers already. 
	       // unlock the object
	      e = e;
	       _rwlock._internal_lock.unlock();
	       caught = new TooManyReaders(pegasus_thread_self());
	    }
	 }
	 else if(milliseconds == -1) // infinite wait
	 {
	    try 
	    {
	       _rwlock._rlock.wait(); 
	    }
	    catch(IPCException& e)
	    {
	       _rwlock._internal_lock.unlock();
	       caught = &e;
	       goto throw_from_here;
	    }
	 }      
	 else // timed wait
	 {
	    try 
	    {
	       _rwlock._rlock.time_wait(milliseconds);
	    }
	    catch(IPCException& e)
	    {
	       _rwlock._internal_lock.unlock();
	       caught = &e;
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
   if(caught != NULL)
      throw(*caught);
   return;
}

void ReadWriteSem::wait(Uint32 mode, PEGASUS_THREAD_TYPE caller) 
   throw(Deadlock, Permission, WaitFailed, TooManyReaders)
{
   timed_wait(mode, caller, -1);
}

void ReadWriteSem::try_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller) 
   throw(Deadlock, Permission, WaitFailed, TooManyReaders)
{
   timed_wait(mode, caller, 0);
}


void ReadWriteSem::unlock(Uint32 mode, PEGASUS_THREAD_TYPE caller) 
   throw(Permission)
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

int ReadWriteSem::read_count()

{
   return( _readers.value() );
}

int ReadWriteSem::write_count()
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

AtomicInt::AtomicInt() {_rep._value = 0; _rep._mutex = Mutex(); }

AtomicInt::AtomicInt(Uint32 initial) {_rep._value = initial;  _rep._mutex = Mutex() ; }

AtomicInt::~AtomicInt()
{

}

AtomicInt::AtomicInt(const AtomicInt& original)
{
    _rep._mutex = Mutex();
    _rep._value = original._rep._value;
} 

AtomicInt& AtomicInt::operator=(const AtomicInt& original )
{
    // to avoid deadlocks, always be certain to only hold one mutex at a time. 
    // therefore, get the original value (which will lock and unlock the original's mutex)
    // and _then_ lock this mutex. This pattern is repeated throughout the class
  
    Uint32 temp = original._rep._value;
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value = temp;
    _rep._mutex.unlock();
    return *this;
}

AtomicInt& AtomicInt::operator=(Uint32 val)
{
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value = val;
    _rep._mutex.unlock();
    return *this;
}

Uint32 AtomicInt::value()
{
    _rep._mutex.lock(pegasus_thread_self());
    Uint32 retval = _rep._value;
    _rep._mutex.unlock();
    return retval;
}

void AtomicInt::operator++(void)
{
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value++;
    _rep._mutex.unlock();
}

void AtomicInt::operator++(int)
{
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value++;
    _rep._mutex.unlock();
}

void AtomicInt::operator--(void)
{
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value--;
    _rep._mutex.unlock();
}

void AtomicInt::operator--(int)
{
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value--;
    _rep._mutex.unlock();
}

Uint32 AtomicInt::operator+(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex 
    Uint32 retval = val._rep._value; 
    _rep._mutex.lock(pegasus_thread_self());
    retval += _rep._value ;
    _rep._mutex.unlock();
    return retval;
}

Uint32 AtomicInt::operator+(Uint32 val)
{
    _rep._mutex.lock(pegasus_thread_self());
    Uint32 retval = _rep._value + val;
    _rep._mutex.unlock();
    return retval;
}

Uint32 AtomicInt::operator-(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex
    Uint32 retval =  val._rep._value;
    _rep._mutex.lock(pegasus_thread_self());
    retval += _rep._value;
    _rep._mutex.unlock();
    return retval;
}

Uint32 AtomicInt::operator-(Uint32 val)
{
    _rep._mutex.lock(pegasus_thread_self());
    Uint32 retval = _rep._value - val;
    _rep._mutex.unlock();
    return retval;
}


AtomicInt& AtomicInt::operator+=(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex
    Uint32 temp = val._rep._value;
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value += temp;
    _rep._mutex.unlock();
    return *this;
}
AtomicInt& AtomicInt::operator+=(Uint32 val)
{
    // never acquire a mutex while holding a mutex
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value += val;
    _rep._mutex.unlock();
    return *this;
}

AtomicInt& AtomicInt::operator-=(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex
    Uint32 temp = val._rep._value;
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value -= temp;
    _rep._mutex.unlock();
    return *this;
}

AtomicInt& AtomicInt::operator-=(Uint32 val)
{
    // never acquire a mutex while holding a mutex
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value -= val;
    _rep._mutex.unlock();
    return *this;
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

Condition::Condition(void) : _disallow(0), _condition()
{ 
   _cond_mutex = new Mutex();
   _destroy_mut = true;
} 

Condition::Condition(const Mutex & mutex) : _disallow(0), _condition()
{
   _cond_mutex = const_cast<Mutex *>(&mutex);
   _destroy_mut = false;
}


Condition::~Condition(void)
{
   cond_waiter *lingerers;
   // don't allow any new waiters
   _disallow++;
   _condition._spin.lock(pegasus_thread_self());

   while(NULL != (lingerers = static_cast<cond_waiter *>(_condition._waiters.remove_last())))
   {
      lingerers->signalled.signal();
   }
   _condition._spin.unlock();
   while( _condition._waiters.count())   {
      pegasus_yield();
   }
   if(_destroy_mut == true)
      delete _cond_mutex;
}

void Condition::signal(PEGASUS_THREAD_TYPE caller)
   throw(IPCException)
{
   cond_mutex->lock(caller);
   
   try
   {
      unlocked_signal(caller);
   }
   catch(...)
   {
      cond_mutex->unlock();
      throw;
   }
      cond_mutex->unlock();
}

void Condition::unlocked_signal(PEGASUS_THREAD_TYPE caller)
   throw(IPCException)
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
   throw(IPCException)
{
   if(_disallow.value() > 0) 
      throw ListClosed();
   _cond_mutex->lock(caller);
}

void Condition::try_lock_object(PEGASUS_THREAD_TYPE caller)
   throw(IPCException)
{
   if(_disallow.value() > 0 ) 
      throw ListClosed();
   _cond_mutex->try_lock(caller);
}

void Condition::wait_lock_object(PEGASUS_THREAD_TYPE caller, int milliseconds)
   throw(IPCException)
{
   if(_disallow.value() > 0) 
      throw ListClosed();
   _cond_mutex->timed_lock(milliseconds, caller);
}

void Condition::unlock_object(void)
{
   _cond_mutex->unlock();
}

void Condition::unlocked_wait(PEGASUS_THREAD_TYPE caller) 
   throw(IPCException)
{
   unlocked_timed_wait(-1, caller);
}

void Condition::unlocked_timed_wait(int milliseconds, PEGASUS_THREAD_TYPE caller) 
   throw(IPCException)
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
	 catch(TimeOut &)
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
