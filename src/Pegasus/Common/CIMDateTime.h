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
// Author:
//
// $Log: CIMDateTime.h,v $
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.2  2001/02/18 03:02:58  karl
// comment cleanup
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.3  2001/02/06 17:04:03  karl
// add documentation
//
// Revision 1.2  2001/01/24 13:54:09  karl
// Add Doc++ Document Comments
//
// Revision 1.1.1.1  2001/01/14 19:50:41  mike
// Pegasus import
//
//
//END_HISTORY

/*
CIMDateTime Header File.  This file contains the public definition of the CIM
CIMDateTime CIMType. It defines the C++ Class CIMDateTime
*/
////////////////////////////////////////////////////////////////////////////////
//
// CIMDateTime.h
//
//	This CIMDateTime class represents the CIM datetime data type.
//	Recall that a CIM datetime may contain a date or an interval.
//
//	A date has the following form:
//
//	    yyyymmddhhmmss.mmmmmmsutc
//
//	Where
//
//	    yyyy = year (0-1999)
//	    mm = month (1-12)
//	    dd = day (1-31)
//	    hh = hour (0-23)
//	    mm = minute (0-59)
//	    ss = second (0-59)
//	    mmmmmm = microseconds.
//	    s = '+' or '-' to represent the UTC sign.
//	    utc = UTC offset (same as GMT offset).
//
//	An interval has the following form:
//
//	    ddddddddhhmmss.mmmmmm:000
//
//	Where
//
//	    dddddddd = days
//	    hh = hours
//	    mm = minutes
//	    ss = seconds
//	    mmmmmm = microseconds
//
//	Note that intervals always end in ":000" (this is how they
//	are distinguished from dates).
//
//	Intervals are really durations since they do not specify a start and
//	end time (as one expects when speaking about an interval). It is
//	better to think of an interval as specifying time elapsed since
//	some event.
//
//	CIMDateTime objects are constructed from C character strings or from
//	other CIMDateTime objects. These character strings must be exactly
//	twenty-five characters and conform to one of the forms idententified
//	above.
//
//	CIMDateTime objects which are not explicitly initialized will be
//	implicitly initialized with the null time interval:
//
//	    00000000000000.000000:000
//
//	Instances can be tested for nullness with the isNull() method.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_DateTime_h
#define Pegasus_DateTime_h

#include <Pegasus/Common/Config.h>
#include <iostream>

PEGASUS_NAMESPACE_BEGIN

class CIMDateTime;
PEGASUS_COMMON_LINKAGE Boolean operator==(const CIMDateTime& x, const CIMDateTime& y);
/**
The CIMDateTime class represents the CIM datetime data type as a C++ class CIMDateTime. A CIM
datetime may contain a date or an interval. CIMDateTime is an intrinsic CIM data type which
represents the time as a formated fixedplength string.
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
    hh = hours
    mm = minutes
    ss = seconds
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

    enum { FORMAT_LENGTH = 25 };
    /// CIMDateTime CIMMethod
    CIMDateTime();
    /// CIMDateTime CIMMethod creates the CIM CIMDateTime from a string constant.
    CIMDateTime(const char* str);
    /**
    CIMDateTime CIMMethod - Creates the CIMDateTime instance from another CIMDateTime
    instance
    */
    CIMDateTime(const CIMDateTime& x);
    /** CIMDateTime method again
    */
    CIMDateTime& operator=(const CIMDateTime& x);
    /** CIMDateTime isNull method - Tests for an all zero date time
	<PRE>
	CIMDateTime dt;
	dt.clear();
    	assert(dt.isNull());
    	</PRE>
    @return This method returns true of the contents are
    "00000000000000.000000:000". Else it returns false
    */
    Boolean isNull() const;
    /// method getString
    const char* getString() const;

    /** method set - Sets the date time.
    Creates the CIMDateTime instance from the input string constant which must be
    in the datetime format.
	<PRE>
	CIMDateTime dt;
	dt.set("19991224120000.000000+360");
    	</PRE>
    @return On format error, CIMDateTime set throws the exception BadDateTimeFormat
    @exception Throws exception BadDateTimeFormat on format error.
    */
    void set(const char* str);
    /// CIMDateTime method clear - Clears the datetime class instance.
    void clear();
    /// CIMDateTime CIMMethod - ATTN: Friend operator Test for CIMDateTime equality
     PEGASUS_COMMON_LINKAGE friend Boolean operator==(
	const CIMDateTime& x,
	const CIMDateTime& y);

private:

    Boolean _set(const char* str);

    char _rep[FORMAT_LENGTH + 1];
};

PEGASUS_COMMON_LINKAGE Boolean operator==(const CIMDateTime& x, const CIMDateTime& y);

PEGASUS_COMMON_LINKAGE std::ostream& operator<<(std::ostream& os, const CIMDateTime& x);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_DateTime_h */





