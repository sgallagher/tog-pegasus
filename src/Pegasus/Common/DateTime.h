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
// $Log: DateTime.h,v $
// Revision 1.1.1.1  2001/01/14 19:50:41  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// DateTime.h
//
//	This DateTime class represents the CIM datetime data type.
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
//	DateTime objects are constructed from C character strings or from
//	other DateTime objects. These character strings must be exactly
//	twenty-five characters and conform to one of the forms idententified
//	above.
//
//	DateTime objects which are not explicitly initialized will be
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

class DateTime;
PEGASUS_COMMON_LINKAGE Boolean operator==(const DateTime& x, const DateTime& y);

class PEGASUS_COMMON_LINKAGE DateTime
{
public:

    enum { FORMAT_LENGTH = 25 };

    DateTime();

    // Throws BadDateTimeFormat.

    DateTime(const char* str);

    DateTime(const DateTime& x);

    DateTime& operator=(const DateTime& x);

    // Returns true if contents are "00000000000000.000000:000"

    Boolean isNull() const;

    const char* getString() const;

    // Throws BadDateTimeFormat.

    void set(const char* str);

    void clear();

    PEGASUS_COMMON_LINKAGE friend Boolean operator==(
	const DateTime& x, 
	const DateTime& y);

private:

    Boolean _set(const char* str);

    char _rep[FORMAT_LENGTH + 1];
};

PEGASUS_COMMON_LINKAGE Boolean operator==(const DateTime& x, const DateTime& y);

PEGASUS_COMMON_LINKAGE std::ostream& operator<<(std::ostream& os, const DateTime& x);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_DateTime_h */
