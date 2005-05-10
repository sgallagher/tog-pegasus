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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: David Dillard, VERITAS Software Corp.
//              (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////


//%// ---- inline implmentations of Windows IPC routines ----

#ifndef IPCWindows_inline_h
#define IPCWindows_inline_h

//-----------------------------------------------------------------
/// Native Windows inline implementation of Mutex class
//-----------------------------------------------------------------

// block until gaining the lock - throw a deadlock
// exception if process already holds the lock
inline void Mutex::lock(PEGASUS_THREAD_TYPE caller)
{
    if(_mutex.owner == caller)
        throw( Deadlock( _mutex.owner ) );

    DWORD errorcode = WaitForSingleObject(_mutex.mut, INFINITE);
    if(errorcode == WAIT_FAILED)
        throw( WaitFailed( _mutex.owner) );
    _mutex.owner = caller;
}

// try to gain the lock - lock succeeds immediately if the
// mutex is not already locked. throws an exception and returns
// immediately if the mutex is currently locked.
inline void Mutex::try_lock(PEGASUS_THREAD_TYPE caller)
{
    if(_mutex.owner == caller)
        throw( Deadlock( _mutex.owner ) );

    DWORD errorcode = WaitForSingleObject(_mutex.mut, 0);
    if (errorcode == WAIT_TIMEOUT)
        throw(AlreadyLocked(_mutex.owner));
    if(errorcode == WAIT_FAILED)
        throw(WaitFailed(_mutex.owner));
    _mutex.owner = caller;
}

// wait for milliseconds and throw an exception then return if the wait
// expires without gaining the lock. Otherwise return without throwing an
// exception.

inline void Mutex::timed_lock( Uint32 milliseconds , PEGASUS_THREAD_TYPE caller)
{
    if(_mutex.owner == caller)
        throw( Deadlock( _mutex.owner ) );

    DWORD errorcode = WaitForSingleObject(_mutex.mut, milliseconds);
    if (errorcode == WAIT_TIMEOUT)
        throw(TimeOut(_mutex.owner));
    if(errorcode == WAIT_FAILED)
        throw(WaitFailed(_mutex.owner));
    _mutex.owner = caller;
}

// unlock the mutex
inline void Mutex::unlock()
{
    PEGASUS_THREAD_TYPE m_owner = _mutex.owner;
    _mutex.owner = 0;
    ReleaseMutex(_mutex.mut);
}


//-----------------------------------------------------------------
/// Native Windows inline implementation of Semaphore class
//-----------------------------------------------------------------

// block until this semaphore is in a signalled state
// note that windows does not support interrupt
inline void Semaphore::wait(Boolean ignoreInterrupt)
{
    DWORD errorcode = WaitForSingleObject(_semaphore.sem, INFINITE);
    if(errorcode != WAIT_FAILED)
        _count--;
    else
        throw(WaitFailed((PEGASUS_THREAD_TYPE)GetCurrentThreadId()));
}

// wait succeeds immediately if semaphore has a non-zero count,
// return immediately and throw and exception if the
// count is zero.
inline void Semaphore::try_wait()
{
    DWORD errorcode = WaitForSingleObject(_semaphore.sem, 0);
    if(errorcode == WAIT_TIMEOUT || errorcode == WAIT_FAILED)
        throw(WaitFailed((PEGASUS_THREAD_TYPE)GetCurrentThreadId()));
    _count--;
}


// wait for milliseconds and throw an exception
// if wait times out without gaining the semaphore
inline void Semaphore::time_wait(Uint32 milliseconds)
{
    DWORD errorcode = WaitForSingleObject(_semaphore.sem, milliseconds);
    if (errorcode == WAIT_TIMEOUT || errorcode == WAIT_FAILED)
        throw(TimeOut((PEGASUS_THREAD_TYPE)GetCurrentThreadId()));
    _count--;
}

// increment the count of the semaphore
inline void Semaphore::signal()
{
    _count++;
    ReleaseSemaphore(_semaphore.sem, 1, NULL);
}

// return the count of the semaphore
inline int Semaphore::count() const
{
    return(_count);
}


//-----------------------------------------------------------------
/// Native Windows inline implementation of AtomicInt class
//-----------------------------------------------------------------
#if defined(PEGASUS_ATOMIC_INT_NATIVE)

inline AtomicInt& AtomicInt::operator=(const AtomicInt& original)
{
    InterlockedExchange(&_rep, original._rep);
    return *this;
}

inline AtomicInt& AtomicInt::operator=(Uint32 val)
{
    InterlockedExchange(&_rep, val);
    return *this;
}

inline Uint32 AtomicInt::value(void) const
{
    return ((Uint32)_rep);
}

inline void AtomicInt::operator++(void) { InterlockedIncrement(&_rep); }
inline void AtomicInt::operator--(void) { InterlockedDecrement(&_rep); }
inline void AtomicInt::operator++(int) { InterlockedIncrement(&_rep); }
inline void AtomicInt::operator--(int) { InterlockedDecrement(&_rep); }

inline Uint32 AtomicInt::operator+(const AtomicInt& val)
{
    //InterlockedExchangeAdd(&_rep, val._rep);
    return (_rep + val._rep);
}

inline Uint32 AtomicInt::operator+(Uint32 val)
{
    //InterlockedExchangeAdd(&_rep, val);
    return (_rep + val);
}

inline Uint32 AtomicInt::operator-(const AtomicInt& val)
{
    //LONG temp_operand, temp_result;
    //temp_operand = InterlockedExchangeAdd((long *)&(val._rep), 0);
    //temp_result = InterlockedExchangeAdd(&_rep, 0);
    //return(temp_result - temp_operand);
    return (_rep - val._rep);
}

inline Uint32 AtomicInt::operator-(Uint32 val)
{
    //LONG temp_operand, temp_result;
    //temp_operand = InterlockedExchangeAdd( (long *)&val, 0);
    //temp_result = InterlockedExchangeAdd(&_rep, 0);
    //return(temp_result - temp_operand);
    return (_rep - val);
}

inline AtomicInt& AtomicInt::operator+=(const AtomicInt& val)
{
    InterlockedExchangeAdd(&_rep, val._rep);
    return *this;
}

inline AtomicInt& AtomicInt::operator+=(Uint32 val)
{
    InterlockedExchangeAdd(&_rep, val);
    return *this;
}

inline AtomicInt& AtomicInt::operator-=(const AtomicInt& val)
{
    LONG temp ;
    InterlockedExchange(&temp, val._rep);
    enter_crit(&_crit);
    _rep -= temp;
    exit_crit(&_crit);

    return *this;
}

inline AtomicInt& AtomicInt::operator-=(Uint32 val)
{
    LONG temp ;
    InterlockedExchange(&temp, val);
    enter_crit(&_crit);
    _rep -= temp;
    exit_crit(&_crit);

    return *this;
}

inline Boolean AtomicInt::DecAndTestIfZero()
{
    return(InterlockedDecrement(&_rep) == 0);
}


#endif // inline atomic int

//_________________________________________________________________
//
/// Native Windows implementation of the conditional semaphore
//_________________________________________________________________

#ifdef PEGASUS_CONDITIONAL_NATIVE

inline void Condition::signal(PEGASUS_THREAD_TYPE caller)
{
    _cond_mutex->lock(caller);
    try
    {
        unlocked_signal(caller);
    }
    catch(...)
    {
        unlock_object();
        throw;
    }
    _cond_mutex->unlock();
}

inline void Condition::unlocked_signal(PEGASUS_THREAD_TYPE caller)
{
    if(_cond_mutex->_mutex.owner != caller)
        throw Permission((PEGASUS_THREAD_TYPE)caller);

    // Change from PulseEvent to SetEvent, this is part of
    //    fix to avoid deadlock in CIMClient Constructor.
    //PulseEvent(_condition);
    SetEvent(_condition);
}

inline void Condition::lock_object(PEGASUS_THREAD_TYPE caller)
{
    if(_disallow.value() > 0 )
        throw ListClosed();
    _cond_mutex->lock(caller);
}

inline void Condition::try_lock_object(PEGASUS_THREAD_TYPE caller)
{
    if(_disallow.value() > 0 )
        throw ListClosed();
    _cond_mutex->try_lock(caller);
}

inline void Condition::wait_lock_object(PEGASUS_THREAD_TYPE caller, int milliseconds)
{
    if(_disallow.value() > 0)
        throw ListClosed();
    _cond_mutex->timed_lock(milliseconds, caller);
    if( _disallow.value() > 0 )
    {
        _cond_mutex->unlock();
        throw ListClosed();
    }
}

inline void Condition::unlock_object(void)
{
    _cond_mutex->unlock();
}

inline void Condition::unlocked_wait(PEGASUS_THREAD_TYPE caller)
{
    unlocked_timed_wait(-1, caller);
}

inline void Condition::unlocked_timed_wait(int milliseconds, PEGASUS_THREAD_TYPE caller)
{
    if(_disallow.value() > 0)
    {
        _cond_mutex->unlock();
        throw ListClosed();
    }

    if(_cond_mutex->_mutex.owner != caller)
        throw Permission((PEGASUS_THREAD_TYPE)caller);
    if(milliseconds == -1)
        milliseconds = INFINITE;

    // Change from PulseEvent to SetEvent, this is part of
    //    fix to avoid deadlock in CIMClient Constructor
    // Change added next line since SignalObjectAndWait
    //       releases the mutex
    _cond_mutex->_mutex.owner = 0;

    DWORD retcode = SignalObjectAndWait(_cond_mutex->_mutex.mut, _condition,
                                            milliseconds, false);
    if(retcode == WAIT_TIMEOUT)
        throw TimeOut(pegasus_thread_self());
    _cond_mutex->lock(caller);
}


#endif // inline native conditional

#endif // IPCWindows_inline_h
