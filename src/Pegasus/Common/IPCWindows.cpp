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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
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

PEGASUS_NAMESPACE_END
