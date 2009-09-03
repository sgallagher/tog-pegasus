//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <cassert>
#include "CIMDateTime.h"
#include "CIMDateTimeRep.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// Julian day of "1 BCE January 1".
static const Uint32 JULIAN_ONE_BCE = 1721060;

// Number of microseconds in one second.
static const Uint64 SECOND = 1000000;

// Number of microseconds in one minute.
static const Uint64 MINUTE = 60 * SECOND;

// Number of microseconds in one hour.
static const Uint64 HOUR = 60 * MINUTE;

// Number of microseconds in one day.
static const Uint64 DAY = 24 * HOUR;

/** Convert month, day, and year to a Julian day (in the Gregorian calendar).
    Return julian day.
*/
static inline Uint32 _toJulianDay(Uint32 year, Uint32 month, Uint32 day)
{
    // Formula adapted from "FREQUENTLY ASKED QUESTIONS ABOUT CALENDARS"
    // (see http://www.tondering.dk/claus/calendar.html).

    int a = (14 - month)/12;
    int y = year+4800-a;
    int m = month + 12*a - 3;
    return day + (153*m+2)/5 + y*365 + y/4 - y/100 + y/400 - 32045;
}

/** Convert a Julian day number (in the Gregorian calendar) to year, month,
    and day.
*/
static inline void _fromJulianDay(
    Uint32 jd, Uint32& year, Uint32& month, Uint32& day)
{
    // Formula adapted from "FREQUENTLY ASKED QUESTIONS ABOUT CALENDARS"
    // (see http://www.tondering.dk/claus/calendar.html).

    int a = jd + 32044;
    int b = (4*a+3)/146097;
    int c = a - (b*146097)/4;
    int d = (4*c+3)/1461;
    int e = c - (1461*d)/4;
    int m = (5*e+2)/153;
    day   = e - (153*m+2)/5 + 1;
    month = m + 3 - 12*(m/10);
    year  = b*100 + d - 4800 + m/10;
}

/** Converts a CIMDateTimeRep representation to its canonical string
    representation as defined in the "CIM infrastructure Specification".
    Note that this implementation preserves any wildcard characters used
    to initially create the CIMDateTime object.
*/
static inline void _DateTimetoCStr(const CIMDateTimeRep* rep, char buffer[26])
{
    if (rep->sign == ':')
    {
        // Extract components:

        Uint64 usec = rep->usec;
        Uint32 microseconds = Uint32(usec % SECOND);
        Uint32 seconds = Uint32((usec / SECOND) % 60);
        Uint32 minutes = Uint32((usec / MINUTE) % 60);
        Uint32 hours = Uint32((usec / HOUR) % 24);
        Uint32 days = Uint32((usec / DAY));

        // Format the string.

        sprintf(
            buffer,
            "%08u%02u%02u%02u.%06u:000",
            Uint32(days),
            Uint32(hours),
            Uint32(minutes),
            Uint32(seconds),
            Uint32(microseconds));
    }
    else
    {
        // Extract components:

        Uint64 usec = rep->usec;
        Uint32 microseconds = Uint32(usec % SECOND);
        Uint32 seconds = Uint32((usec / SECOND) % 60);
        Uint32 minutes = Uint32((usec / MINUTE) % 60);
        Uint32 hours = Uint32((usec / HOUR) % 24);
        Uint32 days = Uint32((usec / DAY));
        Uint32 jd = Uint32(days + JULIAN_ONE_BCE);

        // Convert back from julian to year/month/day:

        Uint32 year;
        Uint32 month;
        Uint32 day;
        _fromJulianDay(jd, year, month, day);

        // Format the string.

        sprintf(
            buffer,
            "%04u%02u%02u%02u%02u%02u.%06u%c%03u",
            Uint32(year),
            Uint32(month),
            Uint32(day),
            Uint32(hours),
            Uint32(minutes),
            Uint32(seconds),
            Uint32(microseconds),
            rep->sign,
            rep->utcOffset);
    }

    // Fill buffer with '*' chars (if any).
    {
        char* first = buffer + 20;
        char* last = buffer + 20 - rep->numWildcards;

        if (rep->numWildcards > 6)
            last--;

        for (; first != last; first--)
        {
            if (*first != '.')
                *first = '*';
        }
    }
}

PEGASUS_NAMESPACE_END
