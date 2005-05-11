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
// Modified By: 
//         Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//         Aruran, IBM (ashanmug@in.ibm.com) for BUG# 3518
//
//%/////////////////////////////////////////////////////////////////////////////




PEGASUS_NAMESPACE_BEGIN
   
Mutex::Mutex()
{
   _mutex.mut = CreateMutex(NULL, false, NULL);
   _mutex.owner = (PEGASUS_THREAD_TYPE)0;
}

Mutex::Mutex(int mutex_type)
{
   _mutex.mut = CreateMutex(NULL, false, NULL);
   _mutex.owner = (PEGASUS_THREAD_TYPE)0;
}

Mutex::Mutex(const Mutex & mutex)
{
   DuplicateHandle(GetCurrentProcess(), mutex._mutex.mut, 
		   GetCurrentProcess(), &(_mutex.mut), 
		   0, TRUE, 
		   DUPLICATE_SAME_ACCESS | MUTEX_ALL_ACCESS | SYNCHRONIZE);


   _mutex.owner = (PEGASUS_THREAD_TYPE)0;
}


Mutex::~Mutex()
{
   WaitForSingleObject(_mutex.mut, INFINITE);
   CloseHandle(_mutex.mut);
}



static const int SEM_VALUE_MAX = 0x0000ffff;
Semaphore::Semaphore(Uint32 initial) 
{
   if(initial > SEM_VALUE_MAX)
      initial = SEM_VALUE_MAX - 1;
   _count = initial;
   _semaphore.owner = (PEGASUS_THREAD_TYPE)GetCurrentThreadId();
   _semaphore.sem = CreateSemaphore(NULL, initial, SEM_VALUE_MAX, NULL);
}

Semaphore::~Semaphore()
{
   CloseHandle(_semaphore.sem);
}


//-----------------------------------------------------------------
/// Native Windows  implementation of AtomicInt class
//-----------------------------------------------------------------
#if defined(PEGASUS_ATOMIC_INT_NATIVE)

AtomicInt::AtomicInt(): _rep(0) { init_crit(&_crit); }

AtomicInt::AtomicInt( Uint32 initial): _rep(initial) { init_crit(&_crit); }

AtomicInt::~AtomicInt() { destroy_crit(&_crit); }

AtomicInt::AtomicInt(const AtomicInt& original) 
{
   _rep = original._rep;
   init_crit(&_crit);
}

#endif // Native Atomic Type 


//-----------------------------------------------------------------
// Native implementation of Conditional semaphore object
//-----------------------------------------------------------------

#ifdef PEGASUS_CONDITIONAL_NATIVE

Condition::Condition(void) : _disallow(0)
{ 
   _cond_mutex.reset(new Mutex());
   _destroy_mut = true;

    // Change from PulseEvent to SetEvent, this is part of 
    //    fix to avoid deadlock in CIMClient Constructor.

    // Change 2nd parm to FALSE (create an auto-reset event)

    _condition = CreateEvent( NULL, FALSE, FALSE, NULL);


} 

Condition::Condition(Mutex& mutex) : _disallow(0), _condition()
{
   _cond_mutex.reset(&mutex);
   _destroy_mut = false;

    // Change from PulseEvent to SetEvent, this is part of 
    //    fix to avoid deadlock in CIMClient Constructor.
   
    // Change 2nd parm to FALSE (create an auto-reset event)
    _condition = CreateEvent( NULL, FALSE, FALSE, NULL);
}


Condition::~Condition(void)
{
   // don't allow any new waiters
   _disallow++;
   
    // Change from PulseEvent to SetEvent, this is part of 
    //    fix to avoid deadlock in CIMClient Constructor.
    // Change from PulseEvent to SetEvent
    //PulseEvent(_condition);
    SetEvent(_condition);


   if(_destroy_mut == true)
      _cond_mutex.reset();
   else
      _cond_mutex.release();

    // Change from PulseEvent to SetEvent, this is part of 
    //    fix to avoid deadlock in CIMClient Constructor.
    // Change Added next line to cleanup handle
    CloseHandle(_condition);

}

#endif // native conditional semaphore
//----------------------------------------------------------------- 
// END of native conditional semaphore implementation
//-----------------------------------------------------------------


PEGASUS_NAMESPACE_END
