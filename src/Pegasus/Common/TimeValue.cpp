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
// $Log: TimeValue.cpp,v $
// Revision 1.1  2001/04/10 23:01:52  mike
// Added new TimeValue class and regression tests for it.
// Modified Stopwatch class to use TimeValue class.
//
//
//END_HISTORY

#include <Pegasus/Common/Config.h>
#include "TimeValue.h"

PEGASUS_NAMESPACE_BEGIN

#include <cstdio>
#include <cstdlib>

#ifdef PEGASUS_OS_TYPE_WINDOWS
# include <windows.h>
# include <sys/types.h>
# include <sys/timeb.h>
#elif PEGASUS_OS_TYPE_UNIX
# include <time.h>
#elif
# error not implemented on this platform
#endif

void _GetCurrentTime(Uint32& seconds, Uint32& milliseconds)
{
#ifdef PEGASUS_OS_TYPE_UNIX
    timeval tv;
    gettimeofday(&tv, 0);
    seconds  = (int)tv.tv_sec;
    milliseconds = (int)tv.tvusec;
#else
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER largeInt = { ft.dwLowDateTime, ft.dwHighDateTime };
    largeInt.QuadPart -= 0x19db1ded53e8000;
    seconds = long(largeInt.QuadPart / (10000 * 1000));
    milliseconds = long((largeInt.QuadPart % (10000 * 1000)) / 10);
#endif
}

TimeValue TimeValue::getCurrentTime()
{
    Uint32 seconds;
    Uint32 milliseconds;
    _GetCurrentTime(seconds, milliseconds);
    return TimeValue(seconds, milliseconds);
}

PEGASUS_NAMESPACE_END
