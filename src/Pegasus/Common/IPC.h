//%//-*-c++-*-//////////////////////////////////////////////////////////////////
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: Markus Mueller
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_IPC_h
#define Pegasus_IPC_h

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include "IPCWindows.h"
#elif defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
# include "IPCUnix.h"
#elif defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
# include "IPCUnix.h"
#else
# error "Unsupported platform"
#endif

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN


//%//////// -------- IPC Exception Classes -------- ///////////////////////////////

class PEGASUS_EXPORT IPCException
{
 public:
    IPCException(PEGASUS_THREAD_TYPE owner): _owner(owner) { }
    //  ~IPCException(void);
    inline PEGASUS_THREAD_TYPE get_owner(void) { return(_owner); }
 private:
    IPCException(void);
    PEGASUS_THREAD_TYPE _owner;  

};

class PEGASUS_EXPORT Deadlock: public IPCException
{
 public:
    Deadlock(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
 private:
    Deadlock(void);
};

class PEGASUS_EXPORT AlreadyLocked: public IPCException
{
 public: 
    AlreadyLocked(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
 private:
    AlreadyLocked(void);
};

class PEGASUS_EXPORT TimeOut: public IPCException
{
 public: 
    TimeOut(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
 private:
    TimeOut(void);
};

class PEGASUS_EXPORT Permission: public IPCException
{
 public: 
    Permission(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
 private:
    Permission(void);
} ;

class PEGASUS_EXPORT WaitFailed: public IPCException
{
 public: 
    WaitFailed(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
 private:
    WaitFailed(void);
} ;


//%///////////////// ----- IPC related functions ------- //////////////////////

PEGASUS_THREAD_TYPE pegasus_thread_self(void);


//%////////////////////////////////////////////////////////////////////////////
class PEGASUS_EXPORT Mutex
{

 public:

    Mutex(void) ;
    Mutex(int type);
    ~Mutex(void);

    // block until gaining the lock - throw a deadlock 
    // exception if process already holds the lock 
    void lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed);
  
    // try to gain the lock - lock succeeds immediately if the 
    // mutex is not already locked. throws an exception and returns
    // immediately if the mutex is currently locked. 
    void try_lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, AlreadyLocked, WaitFailed);

    // wait for milliseconds and throw an exception then return if the wait
    // expires without gaining the lock. Otherwise return without throwing an
    // exception. 
    void timed_lock( Uint32 milliseconds, PEGASUS_THREAD_TYPE caller) throw(Deadlock, TimeOut, WaitFailed);

    // unlock the semaphore
    void unlock(void) throw(Permission);

    inline PEGASUS_MUTEX_TYPE* getMutex() { return(&_mutex.mut); }
    inline PEGASUS_THREAD_TYPE get_owner() { return(_mutex.owner); }

 private:
  
    PEGASUS_MUTEX_HANDLE _mutex;

} ;


//%////////////////////////////////////////////////////////////////////////////

class PEGASUS_EXPORT Semaphore
{
  
 public:
    
    // create the semaphore and set its initial value to the <initial>
    Semaphore(Uint32 initial = 1 ) ;
    ~Semaphore( );

    // block until this semaphore is in a signalled state 
    void wait(void) ;

    // wait succeeds immediately if semaphore has a non-zero count, 
    // return immediately and throw and exception if the 
    // count is zero. 
    void try_wait(void) throw(WaitFailed);

    // wait for milliseconds and throw an exception
    // if wait times out without gaining the semaphore
    void time_wait( Uint32 milliseconds ) throw(TimeOut);


    // increment the count of the semaphore 
    void signal(void);

    // return the count of the semaphore
    int count(void); 

 private:

    PEGASUS_SEM_HANDLE  _semaphore;

    // may not need to use the _count member on
    // platorms that allow you to ask the semaphore for 
    // its count 
    int _count; 
};


#if defined(PEGASUS_ATOMIC_INT_NATIVE)

#else
//-----------------------------------------------------------------
/// Generic definition of Atomic integer
//-----------------------------------------------------------------

#undef PEGASUS_ATOMIC_TYPE
typedef struct {
    Uint32 _value;
    Mutex  _mutex;
} PEGASUS_ATOMIC_TYPE;

#endif

class AtomicInt
{
 public:

    AtomicInt();

    AtomicInt(Uint32 initial);

    ~AtomicInt() ;

    AtomicInt(const AtomicInt& original ) ; // copy 

    AtomicInt& operator=(const AtomicInt& original ); // assignment
    AtomicInt& operator=(Uint32 val);

    Uint32  value(void);

    void operator++(void); // prefix
    inline void operator++(int) { this->operator++(); }  // postfix

    void operator--(void); // prefix
    inline void operator--(int) { this->operator--(); } // postfix

    Uint32 operator+(const AtomicInt& val);
    Uint32 operator+(Uint32 val);

    Uint32 operator-(const AtomicInt& val);
    Uint32 operator-(Uint32 val);

    AtomicInt& operator+=(const AtomicInt& val);
    AtomicInt& operator+=(Uint32 val);
    AtomicInt& operator-=(const AtomicInt& val);
    AtomicInt& operator-=(Uint32 val);

    // Mutex * getMutex(); keep this hidden - it will only exist on platforms
    // without native atomic types 

 private:
    PEGASUS_ATOMIC_TYPE _rep; //    sig_atomic_t on POSIX systems with glibc
};
    

//-----------------------------------------------------------------
/// Generic definition of read/write semaphore
//-----------------------------------------------------------------

#if defined(PEGASUS_READWRITE_NATIVE)

#else 
typedef struct {
    Mutex _wlock;
    Semaphore _rlock;
    PEGASUS_THREAD_T _owner;
} PEGASUS_RWLOCK_HANDLE;
#endif

const Uint32 PEG_SEM_READ = 1 ;
const Uint32 PEG_SEM_WRITE = 2 ;

class PEGASUS_EXPORT ReadWriteSem
{
 
 public:
    ReadWriteSem(void);
    ~ReadWriteSem();

    inline void wait_read(PEGASUS_THREAD_TYPE caller) throw(Deadlock, Permission, WaitFailed) 
	{
	    try { wait(PEG_SEM_READ, caller ); }
	    catch (...) { throw; }
	}
    inline void wait_write(PEGASUS_THREAD_TYPE caller) throw(Deadlock, Permission, WaitFailed)
	{
	    try { wait(PEG_SEM_WRITE, caller); }
	    catch(...) { throw; }
	}

    inline void try_wait_read(PEGASUS_THREAD_TYPE caller) throw(Deadlock, Permission, AlreadyLocked, WaitFailed)
	{
	    try { try_wait(PEG_SEM_READ, caller); }
	    catch(...) { throw; }
	}

    inline void try_wait_write(PEGASUS_THREAD_TYPE caller) throw(Deadlock, Permission, AlreadyLocked, WaitFailed)
	{
	    try { try_wait(PEG_SEM_WRITE, caller); }
	    catch(...) { throw; }
	}

    inline void timed_wait_read(PEGASUS_THREAD_TYPE caller, int milliseconds) throw(Deadlock, Permission, TimeOut, WaitFailed)
	{
	    try { timed_wait(PEG_SEM_READ, caller, milliseconds); }
	    catch(...) { throw; }
	}

    inline void timed_wait_write(PEGASUS_THREAD_TYPE caller, int milliseconds) throw(Deadlock, Permission, TimeOut, WaitFailed)
	{
	    try {timed_wait(PEG_SEM_WRITE, caller, milliseconds); }
	    catch(...) { throw; }
	}

    inline void unlock_read(PEGASUS_THREAD_TYPE caller) throw(Permission)
	{
	    try { unlock(PEG_SEM_READ, caller); }
	    catch(...) { throw; }
	}

    inline void unlock_write(PEGASUS_THREAD_TYPE caller) throw(Permission)
	{
	    try { unlock(PEG_SEM_WRITE, caller); }
	    catch(...) { throw; }
	}
    int read_count(void);
    int write_count(void);

 private: 

    void wait(Uint32 mode, PEGASUS_THREAD_TYPE caller) throw(Deadlock, Permission, WaitFailed);
    void try_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller) throw(AlreadyLocked, Deadlock, Permission, WaitFailed);
    void timed_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller, int milliseconds) throw(TimeOut, Deadlock, Permission, WaitFailed);
    void unlock(Uint32 mode, PEGASUS_THREAD_TYPE caller) throw(Permission);
    AtomicInt _readers;
    AtomicInt _writers;
    PEGASUS_RWLOCK_HANDLE _rwlock;
} ;

//-----------------------------------------------------------------
/// Generic definition of conditional semaphore
//-----------------------------------------------------------------


#if defined(PEGASUS_CONDITIONAL_NATIVE)

#else  
typedef PEGASUS_MUTEX_TYPE PEGASUS_COND_TYPE;

typedef struct {
    PEGASUS_COND_TYPE _cond;
    PEGASUS_THREAD_TYPE _owner;
    PEGASUS_MUTEX_TYPE _internal_mut;
} PEGASUS_COND_HANDLE;

#endif 

class PEGASUS_EXPORT Condition
{
  
 public:
    
    // create the condition variable
    Condition()  ;
    ~Condition();

    // block until this condition is in a signalled state 
    void wait(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed);

    // wait for milliseconds and throw an exception
    // if wait times out without being signaled
    void time_wait( Uint32 milliseconds, PEGASUS_THREAD_TYPE caller ) throw(TimeOut, Deadlock, WaitFailed);

    // signal the condition variable
    void signal(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed);

    // lock the accompanying Mutex
    // this is useful when synchronizing thread startus

    inline Mutex* getMutex(void) {return(&_cond_mutex) ;}
  
    // without pthread_mutex_lock/unlock
    void unlocked_wait(PEGASUS_THREAD_TYPE caller)  ;
    void unlocked_timed_wait(int milliseconds, PEGASUS_THREAD_TYPE caller) throw(TimeOut);
    void unlocked_signal(PEGASUS_THREAD_TYPE caller) ;

 private:
    PEGASUS_THREAD_TYPE _owner;
    PEGASUS_COND_TYPE _condition;
    Mutex _cond_mutex;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_IPC_h */
