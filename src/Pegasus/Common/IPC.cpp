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

#ifndef PEGASUS_ATOMIC_INT_NATIVE

AtomicInt::AtomicInt() : _rep._value(0) {_rep._mutex = Mutex(); }

AtomicInt::AtomicInt(Uint32 initial) : _rep._value(initial) { _rep._mutex = Mutex() ; }

AtomicInt::~AtomicInt() { _rep._mutex.unlock(); }

AtomicInt::AtomicInt(const AtomicInt& original)
{
  _rep._mutex = Mutex();
  _rep._value = original.value();
} 

AtomicInt& AtomicInt::operator=(const AtomicInt& original )
{
  // to avoid deadlocks, always be certain to only hold one mutex at a time. 
  // therefore, get the original value (which will lock and unlock the original's mutex)
  // and _then_ lock this mutex. This pattern is repeated throughout the class
  
  Uint32 temp = original.value();
  _rep._mutex.lock();
  _rep._value = temp;
  _rep._mutex.unlock();
  return *this;
}

AtomicInt& AtomicInt::operator=(Uint32 val)
{
  _rep._mutex.lock();
  _rep._value = val;
  _rep._mutex.unlock();
  return *this;
}

Uint32 AtomicInt::value()
{
  _rep._mutex.lock();
  Uint32 retval = _rep._value;
  _rep._mutex.unlock();
  return retval;
}

void AtomicInt::operator++(void)
{
    _rep._mutex.lock();
    _rep._value++;
    _rep._mutex.unlock();
}


void AtomicInt::operator--(void)
{
    _rep._mutex.lock();
    _rep._value--;
    _rep._mutex.unlock();
}

Uint32 AtomicInt::operator+(const AtomicInt& val)
{
  // never acquire a mutex while holding a mutex 
  Uint32 retval = val.value(); 
  _rep._mutex.lock();
  retval += _rep._value ;
  _rep._mutex.unlock();
  return retval;
}

Uint32 AtomicInt::operator+(Uint32 val)
{
    _rep._mutex.lock();
    Uint32 retval = _rep._value + val;
    _rep._mutex.unlock();
    return retval;
}

Uint32 AtomicInt::operator-(const AtomicInt& val)
{
  // never acquire a mutex while holding a mutex
  Uint32 retval =  val.value();
  _mutex.lock();
  retval += _rep._value;
  _mutex.unlock();
  return retval;
}

Uint32 AtomicInt::operator-(Uint32 val)
{
    _rep._mutex.lock();
    Uint32 retval = _rep._value - val;
    _rep._mutex.unlock();
    return retval;
}


AtomicInt& AtomicInt::operator+=(const AtomicInt& val)
{
  // never acquire a mutex while holding a mutex
  Uint32 temp = val.value();
  _rep._mutex.lock();
  _rep._value += temp;
  _rep._mutex.unlock();
  return *this;
}

AtomicInt& operator+=(Uint32 val)
{
  _rep._mutex.lock();
  _rep._value += val;
  _rep._mutex.unlock();
  return *this;
}

AtomicInt& operator-=(const AtomicInt& val)
{
  Uint32 temp = val.value();
  _rep._mutex.lock();
  _rep._value -= temp;
  _rep._mutex.unlock();
  return *this;
}

AtomicInt& operator-=(Uint32 val)
{
  _rep._mutex.lock();
  _rep._value -= val;
  _rep._mutex.unlock();
  return *this;
}

//Mutex * AtomicInt::getMutex()
//{
//    return &_mutex;
//}

#endif // PEGASUS_ATOMIC_INT_NATIVE
PEGASUS_NAMESPACE_END
