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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Stopwatch_h
#define Pegasus_Stopwatch_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/System.h>

PEGASUS_NAMESPACE_BEGIN

/** Stopwatch - A class for measuring elapsed time
    Stopwatch is a class for measuring time intervals within the environment. 
    It is intended to be a developers tool primarily.
*/
class PEGASUS_COMMON_LINKAGE Stopwatch
{
private:
    TimeValue _start;

public:

    /**	stopwatch constructor. The constructor creates the object and
    starts the timer
    @example Stopwatch time;
    */
    Stopwatch();

    /**	Reset Stopwatch	resets an existing Stopwatch object to the 
    current time value
    */
    void reset();

    /**	getElapsed - Get the elapsed time for the defined stopwatch.
    @return Returns the elapsed time value as a double
    */
    double getElapsed() const;

    /**	printElapsed method sends the current value of the timer and
    sends it to standardout as a string with the word seconds attached
    */
    void printElapsed();
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Stopwatch_h */
