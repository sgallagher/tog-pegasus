//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Modified By: Sushma Fernandes, Hewlett-Packard Company
//              (sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
{
    // ATTN-P2-KS 20 Mar 2002 - Needs expansion of tests.
    // get the output display flag.
    Boolean verbose = (getenv("PEGASUS_TEST_VERBOSE")) ? true : false;
    
    CIMDateTime dt;
    dt.set("19991224120000.000000+360");

    dt.clear();
    assert(dt.isNull());

    {
	Boolean bad = false;

	try
	{
	    dt.set("too short");
	}
	catch (BadDateTimeFormat&)
	{
	    bad = true;
	}

	assert(bad);
    }

    {
	Boolean bad = false;

	try
	{
	    dt.set("too short");
	    dt.set("19990132120000.000000+360");
	}
	catch (BadDateTimeFormat&)
	{
	    bad = true;
	}

	assert(bad);
    }
    if (verbose)
	cout << dt << endl;

    CIMDateTime dt1;
    dt1 = dt;
 
    //
    // Tests for getCurrentDateTime and getDifference.
    //
    CIMDateTime         startTime, finishTime;
    Sint64              differenceInSeconds;

    //
    // Call getCurrentDateTime
    //
    //startTime = CIMDateTime::getCurrentDateTime();
    //finishTime = CIMDateTime::getCurrentDateTime();

    // Set the start and finish times
    startTime.set("20020507170000.000000-480");
    finishTime.set("20020507170000.000000-300");

    //
    // Call getDifference
    //
    try 
    {
        differenceInSeconds = CIMDateTime::getDifference(startTime,finishTime);
    }
    catch(BadFormat &e)
    {
    }

    if (verbose)
    {
        cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
        cout << "Start date time is  : " << startTime << endl;
        cout << "Finish date time is : " << finishTime << endl;
    }

    assert ( differenceInSeconds == -10800 );

    // Set the start and finish times
    startTime.clear();
    finishTime.clear();
    finishTime.set("20020507170000.000000-480");
    startTime.set("20020507170000.000000-300");

    //
    // Call getDifference
    //
    try 
    {
        differenceInSeconds = CIMDateTime::getDifference(startTime,finishTime);
    }
    catch(BadFormat &e)
    {
    }

    if (verbose)
    {
        cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
        cout << "Start date time is  : " << startTime << endl;
        cout << "Finish date time is : " << finishTime << endl;
    }

    assert ( differenceInSeconds == 10800 );

    // Set the start and finish times
    startTime.clear();
    startTime.set("20020507170000.000000+330");
    finishTime.clear(); 
    finishTime.set("20020507170000.000000-480");

    //
    // Call getDifference
    //
    try 
    {
        differenceInSeconds = CIMDateTime::getDifference(startTime,finishTime);
    }
    catch(BadFormat &e)
    {
    }

    if (verbose)
    {
        cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
        cout << "Start date time is  : " << startTime << endl;
        cout << "Finish date time is : " << finishTime << endl;
    }

    assert ( differenceInSeconds == 48600 );

    // Set the start and finish times
    startTime.clear();
    finishTime.clear(); 
    finishTime.set("20020507170000.000000+330");
    startTime.set("20020507170000.000000-480");

    //
    // Call getDifference
    //
    try 
    {
        differenceInSeconds = CIMDateTime::getDifference(startTime,finishTime);
    }
    catch(BadFormat &e)
    {
    }

    if (verbose)
    {
        cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
        cout << "Start date time is  : " << startTime << endl;
        cout << "Finish date time is : " << finishTime << endl;
    }

    assert ( differenceInSeconds == -48600 );

    // Check for interval
    CIMDateTime 	 startInterval;
    CIMDateTime		 finishInterval;
    Sint64      	 intervalDifferenceInSeconds;

    startInterval.set  ("00000001010100.000000:000");
    finishInterval.set ("00000001010200.000000:000");

    if (verbose)
    {
        cout << "Format              : ddddddddhhmmss.mmmmmm:000" << endl;
        cout << "Start interval is   : " << startInterval << endl;
        cout << "Finish interval is  : " << finishInterval << endl;
    }

    intervalDifferenceInSeconds = CIMDateTime::getDifference
                                     (startInterval, finishInterval);

    assert ( startInterval.isInterval() == true );
    assert ( intervalDifferenceInSeconds == 60 );

    try 
    {
        CIMDateTime::getDifference(startInterval, finishTime);
    }
    catch (BadFormat& bfe)
    {
    }
    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
