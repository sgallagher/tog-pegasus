//%//-*-c++-*-/////////////////////////////////////////////////////////////////
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
//         Ramnath Ravindran (Ramnath.Ravindran@compaq.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#define PEGASUS_NEED_CRITICAL_TYPE

#ifndef Pegasus_IPC_h
#define Pegasus_IPC_h

#include <Pegasus/Common/Config.h>
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include "IPCWindows.h"
#elif defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
# include "IPCUnix.h"
#elif defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
# include "IPCHpux.h"
#elif defined(PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)
# include "IPCUnix.h"
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX)
# include "IPCAix.h"
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# include "IPCzOS.h"
#elif defined(PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)
# include "IPCTru64.h"
#else
# error "Unsupported platform"
#endif

#include <Pegasus/Common/Exception.h>

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

void PEGASUS_COMMON_LINKAGE disable_cancel(void);
void PEGASUS_COMMON_LINKAGE enable_cancel(void);
//void PEGASUS_COMMON_LINKAGE native_cleanup_push( void (*)(void *), void * );
void PEGASUS_COMMON_LINKAGE native_cleanup_pop(Boolean execute);

#ifdef PEGASUS_NEED_CRITICAL_TYPE
void PEGASUS_COMMON_LINKAGE init_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_COMMON_LINKAGE enter_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_COMMON_LINKAGE try_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_COMMON_LINKAGE destroy_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_COMMON_LINKAGE exit_crit(PEGASUS_CRIT_TYPE *crit);
#endif

PEGASUS_THREAD_TYPE PEGASUS_COMMON_LINKAGE pegasus_thread_self(void);
void PEGASUS_COMMON_LINKAGE exit_thread(PEGASUS_THREAD_RETURN rc); 
void PEGASUS_COMMON_LINKAGE pegasus_sleep(int ms);
void PEGASUS_COMMON_LINKAGE destroy_thread(PEGASUS_THREAD_TYPE th, PEGASUS_THREAD_RETURN rc);



//%//////// -------- IPC Exception Classes -------- ///////////////////////////////

class PEGASUS_COMMON_LINKAGE IPCException
{
   public:
      IPCException(PEGASUS_THREAD_TYPE owner): _owner(owner) { }
      inline PEGASUS_THREAD_TYPE get_owner(void) { return(_owner); }
   private:
      IPCException(void);
      PEGASUS_THREAD_TYPE _owner;  

};

class PEGASUS_COMMON_LINKAGE Deadlock: public IPCException
{
   public:
      Deadlock(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
   private:
      Deadlock(void);
};

class PEGASUS_COMMON_LINKAGE AlreadyLocked: public IPCException
{
   public: 
      AlreadyLocked(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
   private:
      AlreadyLocked(void);
};

class PEGASUS_COMMON_LINKAGE TimeOut: public IPCException
{
   public: 
      TimeOut(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
   private:
      TimeOut(void);
};

class PEGASUS_COMMON_LINKAGE Permission: public IPCException
{
   public: 
      Permission(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
   private:
      Permission(void);
} ;

class PEGASUS_COMMON_LINKAGE WaitFailed: public IPCException
{
   public: 
      WaitFailed(PEGASUS_THREAD_TYPE owner) : IPCException(owner) {}
   private:
      WaitFailed(void);
} ;

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
      Uint32 get_capacity(void) 
      {
	 return _capacity;
      }
   private:
      ListFull(void );
      Uint32 _capacity;
};

class PEGASUS_COMMON_LINKAGE ListClosed: public IPCException
{
   public:
      ListClosed(void) : IPCException(pegasus_thread_self()) 
      {
      }
};
      


//%////////////////////////////////////////////////////////////////////////////
class PEGASUS_COMMON_LINKAGE Mutex
{

   public:

      Mutex(void) ;
      Mutex(int type);

      // to be able to share the mutex handle between different condition variables
      Mutex(const Mutex& _mutex);
      
      ~Mutex(void);

      // block until gaining the lock - throw a deadlock 
      // exception if process already holds the lock 
      void lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed);
  
      // try to gain the lock - lock succeeds immediately if the 
      // mutex is not already locked. throws an exception and returns
      // immediately if the mutex is currently locked. 
      void try_lock(PEGASUS_THREAD_TYPE caller) 
	 throw(Deadlock, AlreadyLocked, WaitFailed);

      // wait for milliseconds and throw an exception then return if the wait
      // expires without gaining the lock. Otherwise return without throwing an
      // exception. 
      void timed_lock( Uint32 milliseconds, PEGASUS_THREAD_TYPE caller) 
	 throw(Deadlock, TimeOut, WaitFailed);

      // unlock the semaphore
      void unlock(void) throw(Permission);

      inline PEGASUS_THREAD_TYPE get_owner() { return(_mutex.owner); }

   private:
      inline void _set_owner(PEGASUS_THREAD_TYPE owner) { _mutex.owner = owner; }
      PEGASUS_MUTEX_HANDLE _mutex;
      PEGASUS_MUTEX_HANDLE & _get_handle(void) 
      {
	 return _mutex;
      }
      friend class Condition;
} ;


//%////////////////////////////////////////////////////////////////////////////
 
class PEGASUS_COMMON_LINKAGE Semaphore
{
  
   public:
    
      // create the semaphore and set its initial value to the <initial>
      Semaphore(Uint32 initial = 1 ) ;
      Semaphore(const Semaphore & sem);
      
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
//      void _extricate(void);
      friend class Condition;
};


#if defined(PEGASUS_ATOMIC_INT_NATIVE)

#else
//-----------------------------------------------------------------
/// Generic definition of Atomic integer
//-----------------------------------------------------------------

#undef PEGASUS_ATOMIC_INT_NATIVE
#undef PEGASUS_ATOMIC_TYPE
typedef struct {
      Uint32 _value;
      Mutex  _mutex;
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

      ~AtomicInt() ;

      AtomicInt(const AtomicInt& original ) ; // copy 

      AtomicInt& operator=(const AtomicInt& original ); // assignment
      AtomicInt& operator=(Uint32 val);

      Uint32  value(void);

      void operator++(void); // prefix
      void operator++(int); // postfix

      void operator--(void); // prefix
      void operator--(int) ; // postfix

      Uint32 operator+(const AtomicInt& val);
      Uint32 operator+(Uint32 val);

      Uint32 operator-(const AtomicInt& val);
      Uint32 operator-(Uint32 val);

      AtomicInt& operator+=(const AtomicInt& val);
      AtomicInt& operator+=(Uint32 val);
      AtomicInt& operator-=(const AtomicInt& val);
      AtomicInt& operator-=(Uint32 val);

      inline Boolean operator>(Uint32 cmp) {return (this->value() > cmp);}
      inline Boolean operator>=(Uint32 cmp) {return (this->value() >= cmp);}
      inline Boolean operator<(Uint32 cmp) {return (this->value() < cmp);}
      inline Boolean operator<=(Uint32 cmp) {return (this->value() <= cmp);}
      inline Boolean operator==(Uint32 cmp) {return (this->value() == cmp);}

      // Mutex * getMutex(); keep this hidden - it will only exist on platforms
      // without native atomic types 

   private:
      PEGASUS_ATOMIC_TYPE _rep; //    atomic_t on POSIX systems with glibc
#ifdef PEGASUS_NEED_CRITICAL_TYPE
      PEGASUS_CRIT_TYPE _crit;
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
      void wait(Uint32 mode, PEGASUS_THREAD_TYPE caller) 
	 throw(Deadlock, Permission, WaitFailed, TooManyReaders);
      void try_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller) 
	 throw(Deadlock, Permission, WaitFailed, TooManyReaders);
      void timed_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller, int milliseconds) 
	 throw(TimeOut, Deadlock, Permission, WaitFailed, TooManyReaders);
      void unlock(Uint32 mode, PEGASUS_THREAD_TYPE caller)
      	 throw(Permission);

   private: 
      void _extricate(void);
      AtomicInt _readers; 
      AtomicInt _writers;
      PEGASUS_RWLOCK_HANDLE _rwlock;
      // friend template class DQueue;
      friend void extricate_read_write(void *);
   } ;

//-----------------------------------------------------------------
/// Generic definition of conditional semaphore
//-----------------------------------------------------------------


#ifndef PEGASUS_CONDITIONAL_NATIVE

// typedef PEGASUS_SEMAPHORE_TYPE PEGASUS_COND_TYPE;

class PEGASUS_COMMON_LINKAGE cond_waiter {
   public:
      cond_waiter( PEGASUS_THREAD_TYPE caller, Sint32 time = -1) : 
	 waiter(caller), signalled(0) { }
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
      Condition(void)  ;
      ~Condition(void);
      Condition(const Mutex& mutex);

      // signal the condition variable
      void signal(PEGASUS_THREAD_TYPE caller) 
	 throw(IPCException);
      void lock_object(PEGASUS_THREAD_TYPE caller) 
	 throw(IPCException);
      void try_lock_object(PEGASUS_THREAD_TYPE caller)
	 throw(IPCException);
      void wait_lock_object(PEGASUS_THREAD_TYPE caller, int milliseconds)
	 throw(IPCException);
      void unlock_object(void);

      // without pthread_mutex_lock/unlock
      void unlocked_wait(PEGASUS_THREAD_TYPE caller) 
	 throw(IPCException);
      void unlocked_timed_wait(int milliseconds, PEGASUS_THREAD_TYPE caller) 
	 throw(IPCException);
      void unlocked_signal(PEGASUS_THREAD_TYPE caller)
	 throw(IPCException);

      void set_owner(PEGASUS_THREAD_TYPE caller) 
      {
	 _cond_mutex->_set_owner(caller);
      }
      
      void disallow(void)
      {
	 _disallow++;
      }
      
      void reallow(void)
      {
	 if(_disallow.value() > 0)
	    _disallow--;
      }

      Boolean is_shutdown(void)
      {
	 if(_disallow.value() > 0)
	    return true;
	 return false;
      }
      
      
   private:
      AtomicInt _disallow; // don't allow any further waiters
      Boolean _destroy_mut;
      PEGASUS_COND_TYPE _condition; // special type to control execution flow
      Mutex *_cond_mutex; // the conditional mutex
      friend void extricate_condition(void *);
      
};


#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include "IPCWindows_inline.h"
#elif defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# include "IPCUnix_inline.h"
#elif defined(PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)
# include "IPCUnix_inline.h"
#endif

PEGASUS_NAMESPACE_END

#endif /* Pegasus_IPC_h */
