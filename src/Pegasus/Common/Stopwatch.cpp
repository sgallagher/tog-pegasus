//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////
#include "Stopwatch.h"

#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/System.h>

#include <iostream>

PEGASUS_NAMESPACE_BEGIN

Stopwatch::Stopwatch() : _start(0), _stop(0), _total(0)
{
}

void Stopwatch::start()
{
    _start = TimeValue::getCurrentTime().toMicroseconds();
}

void Stopwatch::stop()
{
    _stop = TimeValue::getCurrentTime().toMicroseconds();
    _total += _stop - _start;
}

void Stopwatch::reset()
{
    _start = 0;
    _stop = 0;
    _total = 0;
}

double Stopwatch::getElapsed() const
{
    Sint64 tmp = (Sint64)_total;
    return (double)tmp / (double)1000000.0;
}

Uint64 Stopwatch::getElapsedUsec() const
{
    return _total;
}

void Stopwatch::printElapsed()
{
    PEGASUS_STD(cout) << getElapsed() << " seconds" << PEGASUS_STD(endl);
}

PEGASUS_NAMESPACE_END
