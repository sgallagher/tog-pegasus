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
// Modified By: Markus Mueller
//              Ramnath Ravindran (Ramnath.Ravindran@compaq.com)
//              David Eger (dteger@us.ibm.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Aruran, IBM (ashanmug@in.ibm.com) for BUG# 3518
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_IPC_h
#define Pegasus_IPC_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/AutoPtr.h>

#if !defined(PEGASUS_OS_SOLARIS) && !defined(PEGASUS_OS_LSB) && !defined(PEGASUS_OS_VMS)
#define PEGASUS_NEED_CRITICAL_TYPE
#endif

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include "IPCWindows.h"
#elif defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
# include "IPCUnix.h"
#elif defined(PEGASUS_PLATFORM_HPUX_ACC)
# include "IPCHpux.h"
#elif defined(PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)
# include "IPCUnix.h"
#elif defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC)
# include "IPCSun.h"
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX)
# include "IPCAix.h"
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# include "IPCzOS.h"
#elif defined(PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)
# include "IPCTru64.h"
#elif defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
# include "IPCOs400.h"
#elif defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU)
# include "IPCUnix.h"
#elif defined(PEGASUS_OS_VMS)
# include "IPCVms.h"
#else
# error "Unsupported platform"
#endif

#define PEG_SEM_READ 1
#define PEG_SEM_WRITE 2

#include <Pegasus/Common/internal_dq.h>

PEGASUS_NAMESPACE_BEGIN

int timeval_subtract (struct timeval *result,
                      struct timeval *x,
                      struct timeval *y);

//%///////////////// ----- IPC related functions ------- //////////////////////
// ----- NOTE - these functions are PRIMITIVES that MUST be implemented
//               by the platform. e.g., see IPCUnix.cpp

void PEGASUS_COMMON_LINKAGE disable_cancel();
void PEGASUS_COMMON_LINKAGE enable_cancel();
//void PEGASUS_COMMON_LINKAGE native_cleanup_push( void (*)(void *), void * );
void PEGASUS_COMMON_LINKAGE native_cleanup_pop(Boolean execute);

#ifdef PEGASUS_NEED_CRITICAL_TYPE
void PEGASUS_COMMON_LINKAGE init_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_COMMON_LINKAGE enter_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_COMMON_LINKAGE try_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_COMMON_LINKAGE destroy_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_COMMON_LINKAGE exit_crit(PEGASUS_CRIT_TYPE *crit);
#endif

PEGASUS_THREAD_TYPE PEGASUS_COMMON_LINKAGE pegasus_thread_self();
void PEGASUS_COMMON_LINKAGE exit_thread(PEGASUS_THREAD_RETURN rc);
void PEGASUS_COMMON_LINKAGE pegasus_sleep(int ms);
void PEGASUS_COMMON_LINKAGE destroy_thread(PEGASUS_THREAD_TYPE th, PEGASUS_THREAD_RETURN rc);



//%//////// -------- IPC Exception Classes -------- ///////////////////////////////

class PEGASUS_COMMON_LINKAGE IPCException
{
public:
    IPCException(PEGASUS_THREAD_TYPE owner): _owner(owner) { }
    inline PEGASUS_THREAD_TYPE get_owner() { return(_owner); }
private:
    PEGASUS_THREAD_TYPE _owner;
};

class PEGASUS_COMMON_LINKAGE Deadlock: public IPCException
{
public:
    Deadlock(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
private:
    Deadlock();
};

class PEGASUS_COMMON_LINKAGE AlreadyLocked: public IPCException
{
public:
    AlreadyLocked(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
private:
    AlreadyLocked();
};

class PEGASUS_COMMON_LINKAGE TimeOut: public IPCException
{
public:
    TimeOut(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
private:
    TimeOut();
};

class PEGASUS_COMMON_LINKAGE Permission: public IPCException
{
public:
    Permission(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
private:
    Permission();
};

class PEGASUS_COMMON_LINKAGE WaitFailed: public IPCException
{
public:
    WaitFailed(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
private:
    WaitFailed();
};

class PEGASUS_COMMON_LINKAGE WaitInterrupted: public IPCException
{
public:
    WaitInterrupted(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
private:
    WaitInterrupted();
};

class PEGASUS_COMMON_LINKAGE TooManyReaders: public IPCException
{
public:
    TooManyReaders(PEGASUS_THREAD_TYPE owner) : IPCException(owner) { }
private:
    TooManyReaders();
};


class PEGASUS_COMMON_LINKAGE ListFull: public IPCException
{
public:
    ListFull(Uint32 count) : IPCException(pegasus_thread_self())
    {
        _capacity = count;
    }

    Uint32 get_capacity() const throw()
    {
        return _capacity;
    }

private:
    ListFull();
    Uint32 _capacity;
};

class PEGASUS_COMMON_LINKAGE ListClosed: public IPCException
{
public:
    ListClosed() : IPCException(pegasus_thread_self())
    {
    }
};

class PEGASUS_COMMON_LINKAGE ModuleClosed: public IPCException
{
public:
    ModuleClosed() : IPCException(pegasus_thread_self())
    {
    }
};


//%////////////////////////////////////////////////////////////////////////////
class PEGASUS_COMMON_LINKAGE Mutex
{
public:

    Mutex();
    Mutex(int type);

    ~Mutex();

    // block until gaining the lock - throw a deadlock
    // exception if process already holds the lock
    // @exception Deadlock
    // @exception WaitFailed
    void lock(PEGASUS_THREAD_TYPE caller);

    // try to gain the lock - lock succeeds immediately if the
    // mutex is not already locked. throws an exception and returns
    // immediately if the mutex is currently locked.
    // @exception Deadlock
    // @exception AlreadyLocked
    // @exception WaitFailed
    void try_lock(PEGASUS_THREAD_TYPE caller);

    // wait for milliseconds and throw an exception then return if the wait
    // expires without gaining the lock. Otherwise return without throwing an
    // exception.
    // @exception Deadlock
    // @exception TimeOut
    // @exception WaitFailed
    void timed_lock( Uint32 milliseconds, PEGASUS_THREAD_TYPE caller);

    // unlock the semaphore
    // @exception Permission
    void unlock();

    inline PEGASUS_THREAD_TYPE get_owner() { return(_mutex.owner); }

private:
    inline void _set_owner(PEGASUS_THREAD_TYPE owner) { _mutex.owner = owner; }
    PEGASUS_MUTEX_HANDLE _mutex;
    PEGASUS_MUTEX_HANDLE & _get_handle()
    {
        return _mutex;
    }

    // Hide the assignment operator to avoid implicit use of the default
    // assignment operator.  Do not use this method.
    Mutex& operator=(const Mutex& original) {return *this;}

    // Hide the copy constructor to avoid implicit use of the default
    // copy constructor.  Do not use this method.
    Mutex(const Mutex& _mutex);

    friend class Condition;
};


//%//////////////////////////////////////////////////////////////
//  AutoMutex - use when you could lose scope due to an exception
/////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE AutoMutex
{
public:
    AutoMutex(Mutex& mutex, Boolean autoLock = true)
        : _mutex(mutex),
          _locked(autoLock)
    {
        if (autoLock)
        {
            _mutex.lock(pegasus_thread_self());
        }
    }

    ~AutoMutex()
    {
        try
        {
            unlock();
        }
        catch (...)
        {
            // Do not propagate exception from destructor
        }
    }

    void lock()
    {
        if (_locked)
        {
            throw AlreadyLocked(pegasus_thread_self());
        }

        _mutex.lock(pegasus_thread_self());
        _locked = true;
    }

    void unlock()
    {
        if (!_locked)
        {
            throw Permission(pegasus_thread_self());
        }

        _mutex.unlock();
        _locked = false;
    }

    Boolean isLocked() const
    {
        return _locked;
    }

private:
    AutoMutex();    // Unimplemented
    AutoMutex(const AutoMutex& x);    // Unimplemented
    AutoMutex& operator=(const AutoMutex& x);    // Unimplemented

    Mutex& _mutex;
    Boolean _locked;
};


//%////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE Semaphore
{
public:

    /**
        Creates a semaphore and sets its initial value as specified.
        @param initial The initial value for the Semaphore (defaults to 1).
    */
    Semaphore(Uint32 initial = 1);

    ~Semaphore();

    /**
        Blocks until this Semaphore is in a signalled state.
        @param ignoreInterrupt Indicates whether the wait operation should
        continue (true) or an exception should be thrown (false) when an
        interrupt is received.
        @exception WaitFailed If unable to block on the semaphore.
        @exception WaitInterrupted If the operation is interrupted.
    */
    void wait(Boolean ignoreInterrupt = true);

    /**
        Checks whether the Semaphore is signalled without waiting.  This method
        returns normally if the Semaphore has a non-zero count.
        @exception WaitFailed If the wait operation does not immediately
        succeed.
    */
    void try_wait();

    /**
        Waits for the Semaphore to be signalled for a specified time interval.
        This method returns normally if the Semaphore has a non-zero count or
        it is signalled during the specified time interval.
        @param milliseconds The time interval to wait (in milliseconds).
        @exception TimeOut If the wait operation does not succeed within
        the specified time interval.
    */
    void time_wait(Uint32 milliseconds);

    /**
        Increments the count of the semaphore.
    */
    void signal();

    /**
        Return the count of the semaphore.
    */
    int count() const;

private:

    Semaphore(const Semaphore& x);    // Unimplemented
    Semaphore& operator=(const Semaphore& x);    // Unimplemented

    mutable PEGASUS_SEM_HANDLE _semaphore;

    // may not need to use the _count member on
    // platforms that allow you to ask the semaphore for
    // its count
    mutable int _count;

    friend class Condition;
};


#if !defined(PEGASUS_ATOMIC_INT_NATIVE)
//-----------------------------------------------------------------
/// Generic definition of Atomic integer
//-----------------------------------------------------------------

#undef PEGASUS_ATOMIC_TYPE
typedef struct {
    Uint32 _value;
    PEGASUS_MUTABLE Mutex  _mutex;
} PEGASUS_ATOMIC_TYPE;

#endif


//-----------------------------------------------------------------
/// Atomic Integer class definition
//-----------------------------------------------------------------

class PEGASUS_COMMON_LINKAGE AtomicInt
{
public:
    AtomicInt();
    AtomicInt(Uint32 initial);
   ~AtomicInt();

    AtomicInt(const AtomicInt& original); // copy

    AtomicInt& operator=(const AtomicInt& original); // assignment

    //const AtomicInt& operator=(Uint32 val);
    AtomicInt& operator=(Uint32 val);

    Uint32 value() const;

    void operator++(); // prefix
    void operator++(int); // postfix

    void operator--(); // prefix
    void operator--(int) ; // postfix

    Uint32 operator+(const AtomicInt& val);
    Uint32 operator+(Uint32 val);

    Uint32 operator-(const AtomicInt& val);
    Uint32 operator-(Uint32 val);

    AtomicInt& operator+=(const AtomicInt& val);
    AtomicInt& operator+=(Uint32 val);
    AtomicInt& operator-=(const AtomicInt& val);
    AtomicInt& operator-=(Uint32 val);

    inline Boolean operator>(Uint32 cmp) const {return (this->value() > cmp);}
    inline Boolean operator>=(Uint32 cmp) const {return (this->value() >= cmp);}
    inline Boolean operator<(Uint32 cmp)const {return (this->value() < cmp);}
    inline Boolean operator<=(Uint32 cmp) const {return (this->value() <= cmp);}
    inline Boolean operator==(Uint32 cmp) const {return (this->value() == cmp);}

    // This method should ease reference counting
    Boolean DecAndTestIfZero();

    // Mutex * getMutex(); keep this hidden - it will only exist on platforms
    // without native atomic types

private:
    PEGASUS_ATOMIC_TYPE _rep;
#ifdef PEGASUS_NEED_CRITICAL_TYPE
    mutable PEGASUS_CRIT_TYPE _crit;
#endif /* PEGASUS_NEED_CRITICAL_TYPE */
};


//-----------------------------------------------------------------
/// Generic definition of read/write semaphore
//-----------------------------------------------------------------

#ifndef PEGASUS_READWRITE_NATIVE

typedef struct pegasus_rwlock {
    Semaphore _rlock;
    Mutex _wlock;
    Mutex _internal_lock;
    PEGASUS_THREAD_TYPE _owner;
    pegasus_rwlock() : _rlock(10), _wlock(), _internal_lock(), _owner(pegasus_thread_self())
    {
    }
} PEGASUS_RWLOCK_HANDLE;

#endif

class PEGASUS_COMMON_LINKAGE ReadWriteSem
{
public:
    ReadWriteSem();
    ~ReadWriteSem();

    // @exception Deadlock
    // @exception Permission
    // @exception WaitFailed
    inline void wait_read(PEGASUS_THREAD_TYPE caller)
    {
        wait(PEG_SEM_READ, caller );
    }

    // @exception Deadlock
    // @exception Permission
    // @exception WaitFailed
    inline void wait_write(PEGASUS_THREAD_TYPE caller)
    {
        wait(PEG_SEM_WRITE, caller);
    }

    // @exception Deadlock
    // @exception Permission
    // @exception AlreadyLocked
    // @exception WaitFailed
    inline void try_wait_read(PEGASUS_THREAD_TYPE caller)
    {
        try_wait(PEG_SEM_READ, caller);
    }

    // @exception Deadlock
    // @exception Permission
    // @exception AlreadyLocked
    // @exception WaitFailed
    inline void try_wait_write(PEGASUS_THREAD_TYPE caller)
    {
        try_wait(PEG_SEM_WRITE, caller);
    }

    // @exception Deadlock
    // @exception Permission
    // @exception TimeOut
    // @exception WaitFailed
    inline void timed_wait_read(PEGASUS_THREAD_TYPE caller, int milliseconds)
    {
        timed_wait(PEG_SEM_READ, caller, milliseconds);
    }

    // @exception Deadlock
    // @exception Permission
    // @exception TimeOut
    // @exception WaitFailed
    inline void timed_wait_write(PEGASUS_THREAD_TYPE caller, int milliseconds)
    {
        timed_wait(PEG_SEM_WRITE, caller, milliseconds);
    }

    // @exception Permission
    inline void unlock_read(PEGASUS_THREAD_TYPE caller)
    {
        unlock(PEG_SEM_READ, caller);
    }

    // @exception Permission
    inline void unlock_write(PEGASUS_THREAD_TYPE caller)
    {
        unlock(PEG_SEM_WRITE, caller);
    }

    int read_count() const;
    int write_count() const;

    // @exception Deadlock
    // @exception Permission
    // @exception WaitFailed
    // @exception TooManyReaders
    void wait(Uint32 mode, PEGASUS_THREAD_TYPE caller);

    // @exception Deadlock
    // @exception Permission
    // @exception WaitFailed
    // @exception TooManyReaders
    void try_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller);

    // @exception Timeout
    // @exception Deadlock
    // @exception Permission
    // @exception WaitFailed
    // @exception TooManyReaders
    void timed_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller, int milliseconds);

    // @exception Permission
    void unlock(Uint32 mode, PEGASUS_THREAD_TYPE caller);

private:
    AtomicInt _readers;
    AtomicInt _writers;
    PEGASUS_RWLOCK_HANDLE _rwlock;
    friend void extricate_read_write(void *);
};


// Classes used for safe locking of ReadWriteSem

class ReadLock
{
public:
    ReadLock(ReadWriteSem & rwsem) : _rwsem(rwsem)
    {
        _rwsem.wait_read(pegasus_thread_self());
    }

    ~ReadLock()
    {
        _rwsem.unlock_read(pegasus_thread_self());
    }

private:
    ReadWriteSem & _rwsem;
};

class WriteLock
{
public:
    WriteLock(ReadWriteSem & rwsem) : _rwsem(rwsem)
    {
        _rwsem.wait_write(pegasus_thread_self());
    }

    ~WriteLock()
    {
        _rwsem.unlock_write(pegasus_thread_self());
    }

private:
    ReadWriteSem & _rwsem;
};


//-----------------------------------------------------------------
/// Generic definition of conditional semaphore
//-----------------------------------------------------------------


#ifndef PEGASUS_CONDITIONAL_NATIVE

// typedef PEGASUS_SEMAPHORE_TYPE PEGASUS_COND_TYPE;

class PEGASUS_COMMON_LINKAGE cond_waiter {
public:
    cond_waiter( PEGASUS_THREAD_TYPE caller, Sint32 time = -1)
        : waiter(caller), signalled(0) { }

    ~cond_waiter()
    {
       signalled.signal();
    }

    inline Boolean operator==(const void *key) const
    {
        if((PEGASUS_THREAD_TYPE)key == waiter)
            return true;
        return false;
      }
      
    inline Boolean operator ==(const cond_waiter & b ) const
    {
        return (operator ==(b.waiter)) ;
    }

private:
      cond_waiter();
      PEGASUS_THREAD_TYPE waiter;
      Semaphore signalled;
      friend class Condition;
};

typedef struct peg_condition{
    internal_dq  _waiters;
    Mutex _spin;
    peg_condition() : _waiters(true), _spin()  { }
} PEGASUS_COND_TYPE;

#endif

class PEGASUS_COMMON_LINKAGE Condition
{
public:
    // create the condition variable
    Condition();
    ~Condition();
    Condition(Mutex& mutex);

    // signal the condition variable
    // @exception IPCException
    void signal(PEGASUS_THREAD_TYPE caller);

    // @exception IPCException
    void lock_object(PEGASUS_THREAD_TYPE caller);

    // @exception IPCException
    void try_lock_object(PEGASUS_THREAD_TYPE caller);

    // @exception IPCException
    void wait_lock_object(PEGASUS_THREAD_TYPE caller, int milliseconds);

    void unlock_object();

    // without pthread_mutex_lock/unlock
    // @exception IPCException
    void unlocked_wait(PEGASUS_THREAD_TYPE caller);

    // @exception IPCException
    void unlocked_timed_wait(int milliseconds, PEGASUS_THREAD_TYPE caller);

    // @exception IPCException
    void unlocked_signal(PEGASUS_THREAD_TYPE caller);

    void set_owner(PEGASUS_THREAD_TYPE caller)
    {
        _cond_mutex->_set_owner(caller);
    }

    void disallow()
    {
        _disallow++;
    }

    void reallow()
    {
        if(_disallow.value() > 0)
            _disallow--;
    }

    Boolean is_shutdown() const
    {
        if(_disallow.value() > 0)
            return true;
        return false;
    }

private:
    AtomicInt _disallow; // don't allow any further waiters
    Boolean _destroy_mut;
    PEGASUS_COND_TYPE _condition; // special type to control execution flow
    AutoPtr<Mutex> _cond_mutex; // the conditional mutex //PEP101
    friend void extricate_condition(void *);

    // Hide the assignment operator to avoid implicit use of the default
    // assignment operator.  Do not use this method.
    Condition& operator=(const Condition& original) {return *this;}

    // Hide the copy constructor to avoid implicit use of the default
    // copy constructor.  Do not use this method.
    Condition(const Condition& original) {}
};


#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include "IPCWindows_inline.h"
#elif defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_HPUX_ACC)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_OS_VMS)
# include "IPCVms_inline.h"
#endif

PEGASUS_NAMESPACE_END

#endif /* Pegasus_IPC_h */
