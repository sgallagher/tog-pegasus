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
    Uint64              differenceInSeconds;

    //
    // Call getCurrentDateTime
    //
    startTime = CIMDateTime::getCurrentDateTime();
    finishTime = CIMDateTime::getCurrentDateTime();

    //
    // Call getDifference
    //
    try 
    {
        differenceInSeconds = CIMDateTime::getDifference( startTime, finishTime );
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

    if (startTime == finishTime)
    {
        assert ( differenceInSeconds == 0 );
    }

    // Check for interval
    CIMDateTime testInterval;
    testInterval.set("20020412175729.000000:000");

    assert ( testInterval.isInterval() == true );

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
