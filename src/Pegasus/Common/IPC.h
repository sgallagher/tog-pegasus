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
#define PEG_SEM_READ 1
#define PEG_SEM_WRITE 2

//%///////////////// ----- IPC related functions ------- //////////////////////
// ----- NOTE - these functions are PRIMITIVES that MUST be implemented
//               by the platform. e.g., see IPCUnix.cpp

void PEGASUS_EXPORT disable_cancel(void);
void PEGASUS_EXPORT enable_cancel(void);
void PEGASUS_EXPORT native_cleanup_push( void (*)(void *), void * );
void PEGASUS_EXPORT native_cleanup_pop(Boolean execute);
void PEGASUS_EXPORT init_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_EXPORT enter_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_EXPORT try_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_EXPORT destroy_crit(PEGASUS_CRIT_TYPE *crit);
void PEGASUS_EXPORT exit_crit(PEGASUS_CRIT_TYPE *crit);
PEGASUS_THREAD_TYPE PEGASUS_EXPORT pegasus_thread_self(void);
void PEGASUS_EXPORT exit_thread(PEGASUS_THREAD_RETURN rc);
void PEGASUS_EXPORT sleep(int ms);

//%//////// -------- IPC Exception Classes -------- ///////////////////////////////

class PEGASUS_EXPORT IPCException
{
   public:
      IPCException(PEGASUS_THREAD_TYPE owner): _owner(owner) { }
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

class PEGASUS_EXPORT TooManyReaders: public IPCException
{
   public:
      TooManyReaders(PEGASUS_THREAD_TYPE owner) : IPCException(owner) { }
   private:
      TooManyReaders();
};


//-----------------------------------------------------------------
// internal (unprotected) doubly linked list template
//-----------------------------------------------------------------
template<class L> class PEGASUS_EXPORT internal_dq {
   private: 
      L *_rep;
      internal_dq *_next;
      internal_dq  *_prev;
      Boolean _isHead ;
      int _count;

      // unlink this node from whichever list it is on
      inline void unlink( void  ) 
      { 
	 _prev->_next = _next; 
	 _next->_prev = _prev; 
      }
    
      inline void insert_first(internal_dq & head) 
      { 
	 _prev = head; 
	 _next = head._next; 
	 head._next->_prev = this; 
	 head._next = this;   
	 head._count++; 
      }

      inline void insert_last(internal_dq & head)
      {
	 _next = head;
	 _prev = head._prev;
	 head._prev->next = this;
	 head._prev = this;
	 head._count++;
      }

      inline L *remove( void )
      {
	 L *ret = NULL;
	
	 if( _count > 0 ) {
	    internal_dq *temp = _next;
	    temp->unlink();
	    ret = temp->_rep;
	    // unhinge ret from temp so it doesn't get destroyed 
	    temp->_rep = NULL ;
	    delete temp;
	    _count--;
	 }
	 return(ret);
      }

      friend class Condition;

   public:
    
      internal_dq(Boolean head = true) :  _rep(NULL), _isHead(head), _count(0) 
      { 
	 _next = this; 
	 _prev = this; 
      }
      ~internal_dq() 
      {  
	 this->empty_list(); 
      }
      inline void insert_first(L *element) 
      {
	 internal_dq *ins = new internal_dq(false);
	 ins->_rep = element;
	 ins->_prev = this;
	 ins->_next = this->_next;
	 this->_next->_prev = ins;
	 this->_next = ins;
	 _count++;
      }
      inline void insert_last(L *element) 
      {
	 internal_dq *ins = new internal_dq(false);
	 ins->_rep = element;
	 ins->_next = this;
	 ins->_prev = this->_prev;
	 this->_prev->_next = ins;
	 this->_prev = ins;
	 _count++;
      }
      inline void empty_list( void )
      {
	 if( _count > 0) {
	    while( _count > 0 ) {
	       internal_dq<L> *temp = _next;
	       temp->unlink();
	       if(temp->_rep != NULL)
		  delete temp->_rep;
	       delete temp;
	       _count--;
	    }
	    PEGASUS_ASSERT(_count == 0);
	 }
	 return;
      }

      inline L *remove(void *key)
      {
	 L *ret = NULL;
	 
	 if( _count > 0 ) {
	    internal_dq *temp = _next;
	    while ( temp->_isHead == false ) {
	       if( temp->_rep == key ) {
		  temp->unlink();
		  ret = temp->_rep;
		  temp->_rep = NULL;
		  delete temp;
		  _count--;
		  break;
	       }
	       temp = temp->_next;
	    }
	 }
	 return(ret); 
      }
      inline int count(void) { return _count ; }
} ;




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

      inline PEGASUS_THREAD_TYPE get_owner() { return(_mutex.owner); }

   private:
      PEGASUS_MUTEX_HANDLE _mutex;
      friend class Condition;
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
//      void _extricate(void);
      friend class Condition;
};


#ifndef PEGASUS_ATOMIC_INT_NATIVE
//-----------------------------------------------------------------
/// Generic definition of Atomic integer
//-----------------------------------------------------------------

#undef PEGASUS_ATOMIC_TYPE
typedef struct {
      Uint32 _value;
      Mutex  _mutex;
} PEGASUS_ATOMIC_TYPE;

#endif


//-----------------------------------------------------------------
/// Atomic Integer class definition
//-----------------------------------------------------------------

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

      inline void operator++(void); // prefix
      inline void operator++(int) { this->operator++(); }  // postfix

      inline void operator--(void); // prefix
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
      PEGASUS_CRIT_TYPE _crit;
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
      int _readers; 
      int _writers;
      PEGASUS_RWLOCK_HANDLE _rwlock;
      // friend template class DQueue;
      friend void extricate_read_write(void *);
   } ;

//-----------------------------------------------------------------
/// Generic definition of conditional semaphore
//-----------------------------------------------------------------


#ifndef PEGASUS_CONDITIONAL_NATIVE

// typedef PEGASUS_SEMAPHORE_TYPE PEGASUS_COND_TYPE;

class PEGASUS_EXPORT cond_waiter {
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
      internal_dq<cond_waiter> _waiters;
      Mutex _spin;
      peg_condition() : _waiters(true), _spin()  { }
} PEGASUS_COND_TYPE;

#endif 

class PEGASUS_EXPORT Condition
{ 
   public:
    
      // create the condition variable
      Condition(void)  ;
      ~Condition(void);

      // signal the condition variable
      void signal(PEGASUS_THREAD_TYPE caller) 
	 throw(Deadlock, WaitFailed, Permission);
      void lock_object(PEGASUS_THREAD_TYPE caller) 
	 throw(Deadlock, WaitFailed);
      void try_lock_object(PEGASUS_THREAD_TYPE caller)
	 throw(Deadlock, AlreadyLocked, WaitFailed);
      void wait_lock_object(PEGASUS_THREAD_TYPE caller, int milliseconds)
	 throw(Deadlock, TimeOut, WaitFailed);
      void unlock_object(void);

      // without pthread_mutex_lock/unlock
      void unlocked_wait(PEGASUS_THREAD_TYPE caller) 
	 throw(Permission);
      void unlocked_timed_wait(int milliseconds, PEGASUS_THREAD_TYPE caller) 
	 throw(TimeOut, Permission);
      void unlocked_signal(PEGASUS_THREAD_TYPE caller)
	 throw(Permission);


   private:
      int _disallow; // don't allow any further waiters
      PEGASUS_COND_TYPE _condition; // special type to control execution flow
      Mutex _cond_mutex; // the conditional mutex
      friend void extricate_condition(void *);
      
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_IPC_h */
