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
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
{
    Boolean bad = false;
    try
    {
        // ATTN-P2-KS 20 Mar 2002 - Needs expansion of tests.
        // get the output display flag.
        Boolean verbose = (getenv("PEGASUS_TEST_VERBOSE")) ? true : false;
        
        CIMDateTime dt;
        dt.set("19991224120000.000000+360");
    
        dt.clear();
        assert (dt.equal (CIMDateTime ("00000000000000.000000:000")));
    
        {
            bad = false;

            try
            {
                dt.set("too short");
            }
            catch (InvalidDateTimeFormatException&)
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
            catch (InvalidDateTimeFormatException&)
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
        Sint64              differenceInMicroseconds;
    
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
        differenceInMicroseconds = CIMDateTime::getDifference (startTime,
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            -PEGASUS_SINT64_LITERAL(10800000000));
    
        //
        //  Test date difference with microseconds
        //
        // Set the start and finish times
        startTime.clear ();
        finishTime.clear ();
        finishTime.set ("20020507170000.000003-480");
        startTime.set ("20020507170000.000000-300");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime, 
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(10800000003));
    
        // Set the start and finish times
        startTime.clear ();
        finishTime.clear ();
        finishTime.set ("20020507170000.000000-480");
        startTime.set ("20020507170000.000003-300");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime, 
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
        assert (differenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(10799999997));
    
        // Set the start and finish times
        startTime.clear();
        finishTime.clear();
        finishTime.set("20020507170000.000000-480");
        startTime.set("20020507170000.000000-300");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime,
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(10800000000));
    
        // Set the start and finish times
        startTime.clear();
        startTime.set("20020507170000.000000+330");
        finishTime.clear(); 
        finishTime.set("20020507170000.000000-480");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime,
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(48600000000));
    
        // Set the start and finish times
        startTime.clear();
        finishTime.clear(); 
        finishTime.set("20020507170000.000000+330");
        startTime.set("20020507170000.000000-480");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime,
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            -PEGASUS_SINT64_LITERAL(48600000000));
    
        //
        //  ATTN-CAKG-P2-20020819: the following test currently returns an 
        //  incorrect result on Windows
        //
        //
        // Set the start and finish times
        //
        startTime.set ("19011214000000.000000-000");
        finishTime.set("19011215000000.000000-000");
    
        //
        // Call getDifference
        //
        try 
        {
            differenceInMicroseconds = CIMDateTime::getDifference (startTime,
                finishTime);
        }
        catch(DateTimeOutOfRangeException& e)
        {
        }
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
            char buffer [32];
            sprintf (buffer, "%lli", differenceInMicroseconds);
            cout << "differenceInMicroseconds : " << buffer << endl;
        }
    
        //assert (differenceInMicroseconds == 
        //    PEGASUS_SINT64_LITERAL(86400000000));
    
        //
        // Test for Out of Range dates on HP-UX, since mktime on HP-UX only
        // allows dates within a certain range. mktime (3C) man page on HP-UX
        // does not document the allowed ranges. 
        //
        // Set the start and finish times
        //
        startTime.set ("19000101000000.000000-000");
        finishTime.set("19000102000000.000000-000");
    
        //
        // Call getDifference
        //
        bad = false;
        try
        {
            differenceInMicroseconds = CIMDateTime::getDifference
                (startTime,finishTime);
        }
        catch (DateTimeOutOfRangeException& e)
        {
            bad = true;
        }

#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
        assert(bad);
#endif

        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        //
        // Set the start and finish times
        //
        startTime.set ("20370101000000.000000-000");
        finishTime.set("20370102000000.000000-000");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime,
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(86400000000));
    
        //
        // Test for Out of Range dates on HP-UX, since mktime on HP-UX only
        // allows dates within a certain range. mktime (3C) man page on HP-UX
        // does not document the allowed ranges. 
        //
        // Set the start and finish times
        //
        startTime.set ("20380120000000.000000-000");
        finishTime.set("20380121000000.000000-000");
    
        //
        // Call getDifference
        //
        bad = false;
        try
        {
            differenceInMicroseconds = CIMDateTime::getDifference
                (startTime,finishTime);
        }
        catch (DateTimeOutOfRangeException& e)
        {
            bad = true;
        }
#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
        assert(bad);
#endif
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        //
        // Test for Out of Range dates on HP-UX, since mktime on HP-UX only
        // allows dates within a certain range. mktime (3C) man page on HP-UX
        // does not document the allowed ranges. 
        //
        // Test maximum date difference
        //
        // Set the start and finish times
        //
        startTime.set("00010101000000.000000-000");
        finishTime.set("99991231235959.999999-000");
    
        //
        // Call getDifference
        //
        bad = false;
        try
        {
            differenceInMicroseconds = CIMDateTime::getDifference
                (startTime,finishTime);
        }
        catch (DateTimeOutOfRangeException& e)
        {
            bad = true;
        }
#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
        assert(bad);
#endif
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        // Check for interval
        CIMDateTime 	 startInterval;
        CIMDateTime		 finishInterval;
        Sint64      	 intervalDifferenceInMicroseconds;
    
        startInterval.set  ("00000001010100.000000:000");
        finishInterval.set ("00000001010200.000000:000");
    
        if (verbose)
        {
            cout << "Format              : ddddddddhhmmss.mmmmmm:000" << endl;
            cout << "Start interval is   : " << startInterval << endl;
            cout << "Finish interval is  : " << finishInterval << endl;
        }
    
        intervalDifferenceInMicroseconds = CIMDateTime::getDifference
                                         (startInterval, finishInterval);
    
        assert ( startInterval.isInterval() == true );
        assert ( intervalDifferenceInMicroseconds == 60000000 );
    
        //
        //  Test maximum interval difference
        //
        startInterval.set  ("00000000000000.000000:000");
        finishInterval.set ("99999999235959.999999:000");
    
        if (verbose)
        {
            cout << "Format              : ddddddddhhmmss.mmmmmm:000" << endl;
            cout << "Start interval is   : " << startInterval << endl;
            cout << "Finish interval is  : " << finishInterval << endl;
        }
    
        intervalDifferenceInMicroseconds = CIMDateTime::getDifference
                                         (startInterval, finishInterval);
    
        assert ( startInterval.isInterval() == true );
        assert ( intervalDifferenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(8639999999999999999) );
    
        {
            Boolean bad = false;
    
            try
            {
                CIMDateTime::getDifference(startInterval, finishTime);
            }
            catch (InvalidDateTimeFormatException&)
            {
                bad = true;
            }
    
            assert(bad);
        }

    }
    catch (Exception & e)
    {
        cout << "Exception: " << e.getMessage () << endl;
        exit (1);
    }
    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
