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
// Modified By: Karl Schopmeyer(k.schopmeyer@opengroup.org)
//              Sushma Fernandes, Hewlett Packard Company 
//                  (sushma_fernandes@hp.com)
//              Roger Kumpf, Hewlett Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_DateTime_h
#define Pegasus_DateTime_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <time.h>
#ifdef PEGASUS_INTERNALONLY
#include <iostream>
#endif

PEGASUS_NAMESPACE_BEGIN

/* ATTN: P3. KS Several functions should be added to this class for datetime manipulation
   including get and set each subcomponent (year, month, etc), test for equality,
   create intervals from absolutes, possibly gett current time, Note that
   the Java rep is probably tostring, not get string, 
*/

/**
    The CIMDateTime class represents the CIM datetime data type as a C++ class 
    CIMDateTime. A CIM datetime may contain a date or an interval. CIMDateTime 
    is an intrinsic CIM data type which represents the time as a formated 
    fixedplength string.

    <PRE>
    A date has the following form:
	yyyymmddhhmmss.mmmmmmsutc

    Where

	yyyy = year (0-1999)
	mm = month (1-12)
	dd = day (1-31)
	hh = hour (0-23)
	mm = minute (0-59)
	ss = second (0-59)
	mmmmmm = microseconds.
	s = '+' or '-' to represent the UTC sign.
	utc = UTC offset (same as GMT offset).

    An interval has the following form:

	ddddddddhhmmss.mmmmmm:000

    Where

	dddddddd = days
	hh = hours (0-23)
	mm = minutes (0-59)
	ss = seconds (0-59)
	mmmmmm = microseconds
    </PRE>

    Note that intervals always end in ":000" (this is how they
    are distinguished from dates).

    Intervals are really durations since they do not specify a start and
    end time (as one expects when speaking about an interval). It is
    better to think of an interval as specifying time elapsed since
    some event.

    CIMDateTime objects are constructed from C character strings or from
    other CIMDateTime objects. These character strings must be exactly
    twenty-five characters and conform to one of the forms idententified
    above.

    CIMDateTime objects which are not explicitly initialized will be
    implicitly initialized with the null time interval:

	00000000000000.000000:000

    Instances can be tested for nullness with the isNull() method.
*/
class PEGASUS_COMMON_LINKAGE CIMDateTime
{
public:

    /** Create a new CIMDateTime object with NULL DateTime definition.
    */
    CIMDateTime();

    /** CIMDateTime - Creat a CIM CIMDateTime instance from a string 
	constant representing the CIM DateTime formatted datetime
        See the class documentation for CIMDateTime for the defintion of the
        input string for absolute and interval datetime.
	@param str String object containing the CIM DateTime formatted string
    */
    CIMDateTime(const char* str);

    /** CIMDateTime Create a CIMDateTime instance from another 
	CIMDateTime object
	@param x CIMDateTime object to be copied.
    */
    CIMDateTime(const CIMDateTime& x);

    /** DateTime Destructor. */
    ~CIMDateTime();

    /**  Assign one DateTime object to another
    @param - The DateTime Object to assign
    <pre>
    CIMDateTime d1;
    CIMDateTime d2 = "00000000000000.000000:000";
    d1 = d1;
    </pre>
    */
    CIMDateTime& operator=(const CIMDateTime& x);

    /** CIMDateTime isNull method - Tests for an all zero date time. Note that
        today this function checks for absolute datetime == zero, not interval.
	<PRE>
	CIMDateTime dt;
	dt.clear();
    	assert(dt.isNull());
    	</PRE>
	@return This method returns true of the contents are
	"00000000000000.000000:000". Else it returns false
    */
    Boolean isNull() const;

    /** getString - Returns the string representing the DateTime value of the
        CIMDateTime Object.
        ATTN: P3 KS.  This simply returns a string whether the datetime is a
        real value or the NULL value.  It is up to the user to test.  Should
        this be modified so we do something like an interupt on NULL?
    */
    const char* getString() const;

    /** method set - Sets the date time. Creates the CIMDateTime instance from 
	the input string constant which must be
	in the datetime format.

	    <PRE>
	    CIMDateTime dt;
	    dt.set("19991224120000.000000+360");
	    </PRE>

	@return On format error, CIMDateTime set throws the exception 
	BadDateTimeFormat
	@exception Throws exception BadDateTimeFormat on format error.
    */
    void set(const char* str);

    /**  clear - Clears the datetime class instance. The date time is set to our
        defintion of NULL, absolute zero date time.
    */
    void clear();

    /**
    Get current time as CIMDateTime. The time returned is always in UTC format.

    @return CIMDateTime   Contains the current datetime as a CIMDateTime object.

    Note: Original code was taken from OperationSystem::getLocalDateTime()
    */
    static CIMDateTime getCurrentDateTime();

    /**
    Get the difference between two CIMDateTimes.

    @param startTime     Contains the start time.

    @param finishTime    Contains the finish time.

    @return difference   Difference between the two datetimes in seconds.
    
    @throws BadFormat

    */
    static Real64 getDifference(CIMDateTime startTime, CIMDateTime finishTime);
 
    /**
    Checks whether the datetime is an interval.

    @return isInterval  True if the datetime is an interval, else false
    */
    Boolean isInterval();

private:

    enum { FORMAT_LENGTH = 25 };

    char _rep[FORMAT_LENGTH + 1];

#ifdef PEGASUS_INTERNALONLY
    //
    // Length of the string required to store only the date and time without
    // the UTC sign and UTC offset.
    // Format is yyyymmddhhmmss.
    // Note : The size does not include the null byte.
    //
    enum { DATE_TIME_LENGTH = 14 };

    //
    // Length of the string required to store the  formatted date and time
    // Format is yyyy:mm:dd:hh:mm:ss.
    //
    enum { FORMATTED_DATE_TIME = 20 };

    Boolean _set(const char* str);

    /**
       This function extracts the different components of the date and time
       from the string passed and assigns it to the tm structure.

       @param dateTimeStr  Contains the string to be assigned.

       @param tm           Contains the tm structure to be updated.
    */
    static void formatDateTime(char* dateTime, tm* tm);
#endif

    /** CIMDateTime - ATTN: Friend operator Test for CIMDateTime 
	equality
    */
    PEGASUS_COMMON_LINKAGE friend Boolean operator==(
	const CIMDateTime& x,
	const CIMDateTime& y);
};

PEGASUS_COMMON_LINKAGE Boolean operator==(
    const CIMDateTime& x, 
    const CIMDateTime& y);

#ifdef PEGASUS_INTERNALONLY
PEGASUS_COMMON_LINKAGE PEGASUS_STD(ostream)& operator<<(
    PEGASUS_STD(ostream)& os, 
    const CIMDateTime& x);
#endif

#define PEGASUS_ARRAY_T CIMDateTime
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_DateTime_h */
