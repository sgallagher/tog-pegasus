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
//              sushma_fernandes@hp.com
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <cstdlib>
#include <cstdio>
#include "CIMDateTime.h"
#include "Exception.h"
#include "Array.h"

#if defined(PEGASUS_OS_TYPE_WINDOWS)
    # include <Pegasus/Common/CIMDateTimeWindows.cpp>
#elif defined(PEGASUS_OS_TYPE_UNIX)
    # include <Pegasus/Common/CIMDateTimeUnix.cpp>
#elif defined(PEGASUS_OS_TYPE_NSK)
    # include <Pegasus/Common/CIMDateTimeNsk.cpp>
#else
    # error "Unsupported platform"
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMDateTime
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

// REVIEW: Need methods for determining inequalities.

// REVIEW: Need methods for extracting components (e.g., minutes, hours)?

// REVIEW: Need methods to get current datetime into this form.

// REVIEW: Needs constructor that creates from individual elements(year,...)

static const char _NULL_INTERVAL_TYPE_STRING[] = "00000000000000.000000:000";
static const char _NULL_DATE_TYPE_STRING[] = "00000000000000.000000-000";

CIMDateTime::CIMDateTime()
{
    clear();
}

CIMDateTime::CIMDateTime(const char* str)
{
    set(str);
}

CIMDateTime::CIMDateTime(const CIMDateTime& x)
{
    memcpy(_rep, x._rep, sizeof(_rep));
}

CIMDateTime& CIMDateTime::operator=(const CIMDateTime& x)
{
    if (&x != this)
	memcpy(_rep, x._rep, sizeof(_rep));

    return *this;
}

Boolean CIMDateTime::isNull() const
{
    return strcmp(_rep, _NULL_INTERVAL_TYPE_STRING) == 0;
}

const char* CIMDateTime::getString() const
{
    return _rep;
}

void CIMDateTime::clear()
{
    strcpy(_rep, _NULL_INTERVAL_TYPE_STRING);
}

Boolean CIMDateTime::_set(const char* str)
{
    clear();

    // Be sure the incoming string is the proper length:

    if (strlen(str) != FORMAT_LENGTH)
	return false;

    // Determine the type (date or interval); examine the 21st character;
    // it must be one of ':' (interval), '+' (date), or '-' (date).

    const Uint32 SIGN_OFFSET = 21;
    const Uint32 DOT_OFFSET = 14;

    Boolean isInterval = strcmp(&str[SIGN_OFFSET], ":000") == 0;

    if (!isInterval && str[SIGN_OFFSET] != '+' && str[SIGN_OFFSET] != '-')
	return false;

    // Check for the decimal place:

    if (str[DOT_OFFSET] != '.')
	return false;

    // Check to see if other characters are digits:

    for (Uint32 i = 0; i < FORMAT_LENGTH; i++)
    {
	if (i != DOT_OFFSET && i != SIGN_OFFSET && !isdigit(str[i]))
	    return false;
    }

    // Check to see if the month and day are in range (date only):

    char buffer[16];

    if (!isInterval)
    {
	// Get the month:

	sprintf(buffer, "%2.2s", str + 4);
	long month = atoi(buffer);

	if (month == 0 || month > 12)
	    return false;

	// Get the day:

	sprintf(buffer, "%2.2s", str + 6);
	long day = atoi(buffer);

	if (day == 0 || day > 31)
	    return false;
    }

    // Check the hours and minutes:

    sprintf(buffer, "%2.2s", str + 8);
    long hours = atoi(buffer);

    if (hours > 24)
	return false;

    sprintf(buffer, "%2.2s", str + 10);
    long minutes = atoi(buffer);

    if (minutes > 59)
	return false;

    sprintf(buffer, "%2.2s", str + 12);
    long seconds = atoi(buffer);

    if (seconds > 59)
	return false;

    memcpy(_rep, str, sizeof(_rep));

    return true;
}

void CIMDateTime::set(const char* str)
{
    if (!_set(str))
	throw BadDateTimeFormat();
}

PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& os, const CIMDateTime& x)
{
    return os << x.getString();
}

Boolean operator==(const CIMDateTime& x, const CIMDateTime& y)
{
    return memcmp(x._rep, y._rep, sizeof(x._rep)) == 0;
}

void CIMDateTime::formatDateTime(char* dateTimeStr, tm* tm)
{
    Uint32 index = 0, index1 = 0;
    long   year = 0;
    char   buffer[16];

    // Initialize the components of tm structure
    tm->tm_year = 0;
    tm->tm_mon  = 0;
    tm->tm_mday = 0;
    tm->tm_hour = 0;
    tm->tm_min  = 0;
    tm->tm_sec  = 0;
    tm->tm_isdst = 0;
    tm->tm_wday  = 0;
    tm->tm_yday  = 0;

    // Extract the year.
    sprintf(buffer, "%4.4s", dateTimeStr);
    year   = atoi(buffer);
    year = year - 1900;
    tm->tm_year   = year;

    // Extract the Month.
    sprintf(buffer, "%2.2s", dateTimeStr + 4);
    tm->tm_mon   = atoi(buffer);

    // Extract the Day.
    sprintf(buffer, "%2.2s", dateTimeStr + 6);
    tm->tm_mday   = atoi(buffer);

    // Extract the Hour.
    sprintf(buffer, "%2.2s", dateTimeStr + 8);
    tm->tm_hour   = atoi(buffer);

    // Extract the Minutes.
    sprintf(buffer, "%2.2s", dateTimeStr + 10);
    tm->tm_min   = atoi(buffer);

    // Extract the Seconds.
    sprintf(buffer, "%2.2s", dateTimeStr + 12);
    tm->tm_sec   = atoi(buffer);
}

Boolean CIMDateTime::isInterval()
{
    const char*  	str; 
    const Uint32 	SIGN_OFFSET = 21;

    str = getString();
    Boolean isInterval = strcmp(&str[SIGN_OFFSET], ":000") == 0 ;
    
    return isInterval;
}

Real64 CIMDateTime::getDifference(CIMDateTime startTime, CIMDateTime finishTime)
{
    const char*         startDateTimeCString;
    const char*         finishDateTimeCString;
    char*               dateTimeOnly;
    struct tm           tmvalStart;
    struct tm           tmvalFinish;
    Real64              differenceInSeconds = 0;

    //
    // Get the dates in CString form
    //
    startDateTimeCString = startTime.getString();
    finishDateTimeCString = finishTime.getString();

    //
    // Check if the startTime or finishTime are intervals
    //
    if (startTime.isInterval() || finishTime.isInterval())
    {
        throw BadFormat();
    }

    //
    // Copy only the Start date and time in to the dateTimeOnly string
    //
    dateTimeOnly = new char [FORMATTED_DATE_TIME];
    strncpy( dateTimeOnly, startDateTimeCString, DATE_TIME_LENGTH );
    formatDateTime(dateTimeOnly ,&tmvalStart);

    //
    // Copy only the Finish date and time in to the dateTimeOnly string
    //
    strncpy( dateTimeOnly, finishDateTimeCString, DATE_TIME_LENGTH );
    formatDateTime( dateTimeOnly, &tmvalFinish );

    //
    // Get the difference between the two times
    //
    differenceInSeconds = difftime( mktime(&tmvalFinish), mktime(&tmvalStart));

    delete []dateTimeOnly;
    return differenceInSeconds;
}

PEGASUS_NAMESPACE_END
