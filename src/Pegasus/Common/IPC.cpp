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
// Modified By:
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

AtomicInt::AtomicInt() : _mutex()
{
    _value = 0;
}

AtomicInt::AtomicInt(Uint32 initial) : _mutex()
{
    _value = initial;
}

AtomicInt::~AtomicInt()
{
    _mutex.unlock();
}

Uint32 AtomicInt::value()
{
    _mutex.lock();
    Uint32 retval = _value;
    _mutex.unlock();
    return retval;
}

void AtomicInt::operator++(void)
{
    _mutex.lock();
    _value++;
    _mutex.unlock();
}

void AtomicInt::operator--(void)
{
    _mutex.lock();
    _value--;
    _mutex.unlock();
}

Uint32 AtomicInt::operator+(AtomicInt val)
{
    _mutex.lock();
    Uint32 retval = _value + val.value();
    _mutex.unlock();
    return retval;
}

Uint32 AtomicInt::operator+(Uint32 val)
{
    _mutex.lock();
    Uint32 retval = _value + val;
    _mutex.unlock();
    return retval;
}

Uint32 AtomicInt::operator-(AtomicInt val)
{
    _mutex.lock();
    Uint32 retval = _value - val.value();
    _mutex.unlock();
    return retval;
}

Uint32 AtomicInt::operator-(Uint32 val)
{
    _mutex.lock();
    Uint32 retval = _value - val;
    _mutex.unlock();
    return retval;
}

Mutex * AtomicInt::getMutex()
{
    return &_mutex;
}

PEGASUS_NAMESPACE_END
