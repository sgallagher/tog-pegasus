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
// Modified By: Karl Schopmeyer(k.schopmeyer@opengroup.org)
//              Sushma Fernandes, Hewlett Packard Company 
//                  (sushma_fernandes@hp.com)
//              Roger Kumpf, Hewlett Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_DateTime_h
#define Pegasus_DateTime_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/* ATTN: P3. KS Several functions should be added to this class for datetime manipulation
   including get and set each subcomponent (year, month, etc), test for equality,
   create intervals from absolutes, possibly gett current time, Note that
   the Java rep is probably tostring, not get string, 
*/

class CIMDateTimeRep;

/**
    The CIMDateTime class represents the CIM datetime data type as a C++ class 
    CIMDateTime. A CIM datetime may contain a date or an interval. CIMDateTime 
    is an intrinsic CIM data type which represents the time as a formatted 
    fixed length string.

    <PRE>
    A date has the following form:
	yyyymmddhhmmss.mmmmmmsutc

    Where

	yyyy = year (1-9999)
	mm = month (1-12)
	dd = day (1-31)
	hh = hour (0-23)
	mm = minute (0-59)
	ss = second (0-59)
	mmmmmm = microseconds
	s = '+' or '-' to represent the UTC sign
	utc = UTC offset (same as GMT offset)

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

    CIMDateTime objects are constructed from String objects or from
    other CIMDateTime objects.  These character strings must be exactly
    twenty-five characters and conform to one of the forms identified
    above.

    CIMDateTime objects that are not explicitly initialized will be
    implicitly initialized with a zero time interval:

	00000000000000.000000:000

*/
class PEGASUS_COMMON_LINKAGE CIMDateTime
{
public:

    /** Creates a new CIMDateTime object with a zero interval value
    */
    CIMDateTime();

    /** CIMDateTime - Creates a CIM CIMDateTime instance from a string 
	constant representing the CIM DateTime-formatted datetime
        See the class documentation for CIMDateTime for the definition of the
        input string for absolute and interval datetime.
	@param str String object containing the CIM DateTime-formatted string
    */
    CIMDateTime(const String & str);

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

    /** toString - Returns the string representing the DateTime value of the
        CIMDateTime Object.
    */
    String toString () const;

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
    void set(const String & str);

    /**  clear - Clears the datetime class instance.  The date time is set to 
         a zero interval value.
    */
    void clear();

    /**
    Get current time as CIMDateTime. The time returned is the local time.

    @return CIMDateTime   Contains the current datetime as a CIMDateTime object.
    */
    static CIMDateTime getCurrentDateTime();

    /**
    Computes the difference in microseconds between two CIMDateTime dates or 
    two CIMDateTime intervals 

    @param startTime     Contains the start datetime

    @param finishTime    Contains the finish datetime

    @return difference   Difference between the two datetimes in microseconds
    
    @throws BadDateTimeFormat If one argument is a datetime and one is an
                              interval
    */
    static Sint64 getDifference(CIMDateTime startTime, CIMDateTime finishTime);
 
    /**
    Checks whether the datetime is an interval.

    @return isInterval  True if the datetime is an interval, else false
    */
    Boolean isInterval();

    /**
    Compares the CIMDateTime object to another CIMDateTime object for equality

    @return True if the two CIMDateTime objects are equal,
            False otherwise
    */
    Boolean equal (const CIMDateTime & x) const;

private:

    CIMDateTimeRep* _rep;

    Boolean _set(const String & dateTimeStr);
};

PEGASUS_COMMON_LINKAGE Boolean operator==(
    const CIMDateTime& x, 
    const CIMDateTime& y);

#define PEGASUS_ARRAY_T CIMDateTime
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_DateTime_h */
