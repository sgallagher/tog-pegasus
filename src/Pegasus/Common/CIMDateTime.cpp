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
//                  (sushma_fernandes@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)   
#include <Pegasus/Common/Config.h>              
#endif                                          

#include <cctype>
#include <time.h>
#include <Pegasus/Common/Destroyer.h>
#include "CIMDateTime.h" 
#include "InternalException.h" 

#include <errno.h>

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

// ATTN: P3 KS 04/17/02 Need methods for determining inequalities.

// ATTN: P3 KS 04/17/02 Need methods for extracting components (e.g., minutes, hours)?

// ATTN: P3 KS 04/17/02 Needs constructor that creates from individual elements(year,...)

static const char _NULL_INTERVAL_TYPE_STRING[] = "00000000000000.000000:000";
static const char _NULL_DATE_TYPE_STRING[] = "00000000000000.000000-000";

class CIMDateTimeRep
{
public:
    enum { FORMAT_LENGTH = 25 };

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

    char data[FORMAT_LENGTH + 1];
};


CIMDateTime::CIMDateTime()
{
    _rep = new CIMDateTimeRep();
    clear();
}

CIMDateTime::CIMDateTime(const String & str)
{
    _rep = new CIMDateTimeRep();
    if (!_set(str))
    {
        delete _rep;
        throw InvalidDateTimeFormatException();
    }
}

CIMDateTime::CIMDateTime(const CIMDateTime& x)
{
    _rep = new CIMDateTimeRep();
    memcpy(_rep->data, x._rep->data, sizeof(_rep->data));
}

CIMDateTime& CIMDateTime::operator=(const CIMDateTime& x)
{
    if (&x != this)
        memcpy(_rep->data, x._rep->data, sizeof(_rep->data));

    return *this;
}

CIMDateTime::~CIMDateTime()
{
    delete _rep;
}

String CIMDateTime::toString () const
{
    return String (_rep->data);
}

void CIMDateTime::clear()
{
    strcpy(_rep->data, _NULL_INTERVAL_TYPE_STRING);
}

Boolean CIMDateTime::_set(const String & dateTimeStr)
{
    clear();

    CString dtStr = dateTimeStr.getCString();
    const char* str = dtStr;

    // Be sure the incoming string is the proper length:

    if (strlen(str) != CIMDateTimeRep::FORMAT_LENGTH)
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

    for (Uint32 i = 0; i < CIMDateTimeRep::FORMAT_LENGTH; i++)
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

    if (hours > 23)
	return false;

    sprintf(buffer, "%2.2s", str + 10);
    long minutes = atoi(buffer);

    if (minutes > 59)
	return false;

    sprintf(buffer, "%2.2s", str + 12);
    long seconds = atoi(buffer);

    if (seconds > 59)
	return false;

    memcpy(_rep->data, str, sizeof(_rep->data));

    return true;
}

void CIMDateTime::set(const String & str)
{
    if (!_set(str))
	throw InvalidDateTimeFormatException();
}

Boolean operator==(const CIMDateTime& x, const CIMDateTime& y)
{
    return x.equal (y);
}

void formatDateTime(char* dateTimeStr, tm* tm)
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
    const Uint32 	SIGN_OFFSET = 21;

    Boolean isInterval = strcmp(&_rep->data[SIGN_OFFSET], ":000") == 0 ;
    
    return isInterval;
}

Boolean CIMDateTime::equal (const CIMDateTime & x) const
{
    return memcmp (this->_rep->data, x._rep->data, sizeof (this->_rep->data)) 
        == 0;
}

Sint64 CIMDateTime::getDifference(CIMDateTime startTime, CIMDateTime finishTime)
{
    const char*         startDateTimeCString;
    const char*         finishDateTimeCString;
    char                dateTimeOnly[CIMDateTimeRep::FORMATTED_DATE_TIME];
    struct tm           tmvalStart;
    struct tm           tmvalFinish;
    Sint64              differenceInSeconds = 0;
    time_t		timeStartInSeconds;
    time_t 		timeFinishInSeconds;
    char 		sign;
    Uint32 		offset;
    char 		buffer[4];

    //
    // Get the dates in CString form
    //
    startDateTimeCString = startTime._rep->data;
    finishDateTimeCString = finishTime._rep->data;

    //
    // Check if the startTime or finishTime are intervals
    //
    if (startTime.isInterval() && finishTime.isInterval())
    {
        char 		intervalBuffer[9];
        //
        //  NOTE: although a Uint64 is not required to hold the maximum
        //  value for these variables, if they are not defined as Uint64s,
        //  overflow/truncation can occur during the calculation of the
        //  number of microseconds, and the final result may be incorrect
        //
	Uint64		startIntervalDays;
	Uint64		startIntervalHours;
	Uint64 		startIntervalMinutes;
	Uint64		startIntervalSeconds; 
	Uint64		startIntervalMicroseconds; 
	Uint64		finishIntervalDays;
	Uint64		finishIntervalHours;
	Uint64 		finishIntervalMinutes;
	Uint64		finishIntervalSeconds; 
	Uint64		finishIntervalMicroseconds; 
	Uint64		startIntervalInMicroseconds; 
	Uint64		finishIntervalInMicroseconds; 
	Sint64		intervalDifferenceInMicroseconds; 

        // Parse the start time interval and get the days, minutes, hours
        // and seconds

        // Extract the days.
        sprintf(intervalBuffer, "%8.8s", startDateTimeCString);
        startIntervalDays   = atoi(intervalBuffer);

        // Extract the Hour.
        sprintf(intervalBuffer, "%2.2s", startDateTimeCString + 8);
        startIntervalHours   = atoi(intervalBuffer);

        // Extract the Minutes.
        sprintf(intervalBuffer, "%2.2s", startDateTimeCString + 10);
        startIntervalMinutes = atoi(intervalBuffer);

        // Extract the Seconds.
        sprintf(intervalBuffer, "%2.2s", startDateTimeCString + 12);
        startIntervalSeconds = atoi(intervalBuffer);

        // Extract the Microseconds
        sprintf(intervalBuffer, "%6.6s", startDateTimeCString + 15);
        startIntervalMicroseconds = atoi(intervalBuffer);

        // Parse the finish time interval and get the days, minutes, hours
        // and seconds

        // Extract the days.
        sprintf(intervalBuffer, "%8.8s", finishDateTimeCString);
        finishIntervalDays   = atoi(intervalBuffer);

        // Extract the Hour.
        sprintf(intervalBuffer, "%2.2s", finishDateTimeCString + 8);
        finishIntervalHours   = atoi(intervalBuffer);

        // Extract the Minutes.
        sprintf(intervalBuffer, "%2.2s", finishDateTimeCString + 10);
        finishIntervalMinutes = atoi(intervalBuffer);

        // Extract the Seconds.
        sprintf(intervalBuffer, "%2.2s", finishDateTimeCString + 12);
        finishIntervalSeconds = atoi(intervalBuffer);

        // Extract the Microseconds
        sprintf(intervalBuffer, "%6.6s", finishDateTimeCString + 15);
        finishIntervalMicroseconds = atoi(intervalBuffer);

        // Convert all values to seconds and compute the start and finish
        // intervals in seconds.
        startIntervalInMicroseconds = 
            (startIntervalDays*PEGASUS_UINT64_LITERAL(86400000000)) +
            (startIntervalHours*PEGASUS_UINT64_LITERAL(3600000000)) +
            (startIntervalMinutes*60000000) +
            (startIntervalSeconds*1000000) +
             startIntervalMicroseconds;

        finishIntervalInMicroseconds = 
            (finishIntervalDays*PEGASUS_UINT64_LITERAL(86400000000)) +
            (finishIntervalHours*PEGASUS_UINT64_LITERAL(3600000000)) +
            (finishIntervalMinutes*60000000) + 
            (finishIntervalSeconds*1000000) +
             finishIntervalMicroseconds;

        // Get the difference.
        intervalDifferenceInMicroseconds =
            (Sint64)(finishIntervalInMicroseconds -
                     startIntervalInMicroseconds);

        return intervalDifferenceInMicroseconds;
    }
    else if ( startTime.isInterval() || finishTime.isInterval() )
    {
        // ATTN-RK-20020815: Wrong exception to throw.
        throw InvalidDateTimeFormatException();
    }

    //
    // Copy only the Start date and time in to the dateTimeOnly string
    //
    strncpy( dateTimeOnly, startDateTimeCString, CIMDateTimeRep::DATE_TIME_LENGTH );
    dateTimeOnly[CIMDateTimeRep::DATE_TIME_LENGTH] = 0;
    formatDateTime(dateTimeOnly ,&tmvalStart);

    //
    // Copy only the Finish date and time in to the dateTimeOnly string
    //
    strncpy( dateTimeOnly, finishDateTimeCString, CIMDateTimeRep::DATE_TIME_LENGTH );
    dateTimeOnly[CIMDateTimeRep::DATE_TIME_LENGTH] = 0;
    formatDateTime( dateTimeOnly, &tmvalFinish );

    // Convert local time to seconds since the epoch
    timeStartInSeconds  = mktime(&tmvalStart);
  
    // Check if the date is within the supported range of mktime.
    // If not return an error. Unix mktime sets errno to ERANGE. 
    // Check for both return code from mktime as well as errno. 
    // In case of Windows errno is not set, only check for return code 
    // from mktime for Windows platform.
#ifdef PEGASUS_OS_HPUX
    if ( timeStartInSeconds == (time_t)-1 && errno == ERANGE)
#else
    if ( timeStartInSeconds == (time_t)-1 )
#endif
    {
        throw DateTimeOutOfRangeException(startTime.toString());
    }
    
    timeFinishInSeconds = mktime(&tmvalFinish);
  
    // Check if the date is within the supported range of mktime.
    // If not return an error. Unix mktime sets errno to ERANGE. 
    // Check for both return code from mktime as well as errno. 
    // In case of Windows errno is not set, only check for return code 
    // from mktime for Windows platform.
#ifdef PEGASUS_OS_HPUX
    if ( timeFinishInSeconds == (time_t)-1 && errno == ERANGE)
#else
    if ( timeFinishInSeconds == (time_t)-1 )
#endif
    {
        throw DateTimeOutOfRangeException(finishTime.toString());
    }
    
    // Convert start time to UTC
    // Get the sign and UTC offset.
    sign = startDateTimeCString[21];
    sprintf(buffer, "%3.3s",  startDateTimeCString + 22);
    offset = atoi(buffer);

    if ( sign == '+' )
    {
        // Convert the offset from minutes to seconds and subtract it from
        // Start time
        timeStartInSeconds = timeStartInSeconds - ( offset * 60 );
    }
    else
    {
        // Convert the offset from minutes to seconds and add it to
        // Start time
        timeStartInSeconds = timeStartInSeconds + (offset * 60);
    }

    // Convert finish time to UTC
    // Get the sign and UTC offset.
    sign = finishDateTimeCString[21];
    sprintf(buffer, "%3.3s",  finishDateTimeCString + 22);
    offset = atoi(buffer);

    if ( sign == '+' )
    {
        // Convert the offset from minutes to seconds and subtract it from
        // finish time
        timeFinishInSeconds = timeFinishInSeconds - ( offset * 60 );
    }
    else
    {
        // Convert the offset from minutes to seconds and add it to
        // finish time
        timeFinishInSeconds = timeFinishInSeconds + (offset * 60);
    }

    //
    // Get the difference between the two times
    //
    differenceInSeconds = (Sint64) difftime( timeFinishInSeconds, timeStartInSeconds );

    //
    //  ATTN-CAKG-P1-20020816: struct tm and difftime don't handle microseconds
    //
    Sint64 differenceInMicroseconds = differenceInSeconds * 1000000;
    startDateTimeCString = startTime._rep->data;
    finishDateTimeCString = finishTime._rep->data;
    char dateBuffer [9];
    sprintf (dateBuffer, "%6.6s", startDateTimeCString + 15);
    Uint32 startDateMicroseconds = atoi (dateBuffer);
    sprintf (dateBuffer, "%6.6s", finishDateTimeCString + 15);
    Uint32 finishDateMicroseconds = atoi (dateBuffer);
    (finishDateMicroseconds > startDateMicroseconds) ?
        differenceInMicroseconds +=
            (finishDateMicroseconds - startDateMicroseconds) :
        differenceInMicroseconds -=
            (startDateMicroseconds - finishDateMicroseconds);

    return differenceInMicroseconds;
}

PEGASUS_NAMESPACE_END
