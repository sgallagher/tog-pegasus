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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_TimeValue_h
#define Pegasus_TimeValue_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Linkage.h>

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

    void fromMilliseconds(Uint32 milliseconds)
    {
	_seconds = milliseconds / 1000;
	_microseconds = (milliseconds % 1000) * 1000;
    }

    Uint32 toMilliseconds() const 
    {
	return _seconds * 1000 + _microseconds / 1000; 
    }

    static TimeValue getCurrentTime()
    {
	Uint32 seconds;
	Uint32 milliseconds;
	System::getCurrentTime(seconds, milliseconds);
	return TimeValue(seconds, milliseconds * 1000);
    }

private:
    Uint32 _seconds;
    Uint32 _microseconds;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_TimeValue_h */
