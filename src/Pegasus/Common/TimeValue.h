//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author: Michael E. Brasher
//
// $Log: TimeValue.h,v $
// Revision 1.1  2001/04/10 23:01:52  mike
// Added new TimeValue class and regression tests for it.
// Modified Stopwatch class to use TimeValue class.
//
//
//END_HISTORY

#ifndef Pegasus_TimeValue_h
#define Pegasus_TimeValue_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

/** The TimeValue class represents time expressed in seconds plus microseconds.
*/
class PEGASUS_COMMON_LINKAGE TimeValue
{
public:

    TimeValue() : _seconds(0), _microseconds(0)
    {
    }

    TimeValue(Uint32 seconds, Uint32 microseconds) 
	: _seconds(seconds), _microseconds(microseconds)
    {
    }

    Uint32 getSeconds() const 
    {
	return _seconds;
    }

    void setSeconds(Uint32 seconds)
    {
	_seconds = seconds;
    }

    Uint32 getMicroseconds() const 
    {
	return _microseconds;
    }

    void setMicroseconds(Uint32 microseconds)
    {
	_microseconds = microseconds;
    }

    Uint32 toMilliseconds() const 
    {
	return _seconds * 1000 + _microseconds / 1000; 
    }

    static TimeValue getCurrentTime();

private:
    Uint32 _seconds;
    Uint32 _microseconds;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_TimeValue_h */
