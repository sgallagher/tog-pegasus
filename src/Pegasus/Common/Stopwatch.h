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
//      Chip Vincent (cvincent@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Stopwatch_h
#define Pegasus_Stopwatch_h

#include <Pegasus/Common/Config.h>

#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** Stopwatch - A class for measuring elapsed time
    Stopwatch is a class for measuring time intervals within the environment.
    It is intended to be a developers tool primarily.
*/
class PEGASUS_COMMON_LINKAGE Stopwatch
{
public:

    /** constructor. The constructor creates the object. Start
    must be called to start the timer.
    @example Stopwatch time;
    */
    Stopwatch(void);

    /** start - Starts accumulating time and continues until stop is called.
    The object can be started and stopped multiple times to measure the
    sum of several intervals, but each start must have a corresponding start.
    */
    void start(void);

    /** stop - Stops the accumlation of time for an interval. The object
    should only stopped if it has been started.
    */
    void stop(void);

    /** reset - Effectively clears the time values stored by a Stopwatch.
    */
    void reset(void);

    /** getElapsed - Get the elapsed time for the defined stopwatch. This
    method should only be called if it is currently stopped.
    @return Returns the elapsed time value as a double
    */
    double getElapsed(void) const;

    /** printElapsed method gets the current value of the timer and
    sends it to standard out as a string with the word seconds attached
    */
    void printElapsed(void);

private:
    Uint32 _start;
    Uint32 _stop;

    double _total;

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Stopwatch_h */
