//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Sushma Fernandes, Hewlett Packard Company (sushma_fernandes@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// DateTime 
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/CIMDateTime.h>

#include <sys/time.h>
#include <cstring>

typedef struct Timestamp {
    char year[4];
    char month[2];
    char day[2];
    char hour[2];
    char minutes[2];
    char seconds[2];
    char dot;
    char microSeconds[6];
    char plusOrMinus;
    char utcOffset[3];
    char padding[3];
} Timestamp_t;

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//
// Returns the local time.
// Note: Original code was taken from OperatingSystem::getLocalDateTime ()
//
CIMDateTime CIMDateTime::getCurrentDateTime()
{
    CIMDateTime 	currentDateTime;
    int         	year;
    Timestamp_t  	dateTime;
    char   		mTmpString[80];
    time_t 		mSysTime;
    struct 		timeval   tv;
    struct 		timezone  tz;
    struct tm* 		tmval;
    struct tm		local_tm;
#if defined (PEGASUS_PLATFORM_SOLARIS_SPARC_CC) || \
    defined(PEGASUS_OS_LINUX)
    time_t		utc_offset;
#endif
    // Get the system date and time
    mSysTime = time(NULL);

    // Get the localtime
#if defined PEGASUS_PLATFORM_SOLARIS_SPARC_CC
    tmval = localtime_r(&mSysTime, &local_tm);
    PEGASUS_ASSERT(tmval != 0);
    gettimeofday(&tv,NULL);
    utc_offset = (tmval->tm_isdst > 0 && daylight) ? altzone : timezone ;
    utc_offset /= 60;	// CIM only uses minutes, not seconds.
#elif defined(PEGASUS_OS_LINUX)
    tmval = localtime_r(&mSysTime, &local_tm);
    PEGASUS_ASSERT(tmval != 0);
    gettimeofday(&tv,NULL);
    utc_offset = local_tm.tm_gmtoff/60;
#else
    tmval = localtime_r(&mSysTime, &local_tm);
    PEGASUS_ASSERT(tmval != 0);
    gettimeofday(&tv,&tz);
#endif

    // Initialize the year 
    year = 1900;

    // Format the date
    sprintf((char *)&dateTime,"%04d%02d%02d%02d%02d%02d.%06ld+%03d",
                    year + tmval->tm_year,
                    tmval->tm_mon + 1,
                    tmval->tm_mday,
                    tmval->tm_hour,
                    tmval->tm_min,
                    tmval->tm_sec,
                    tv.tv_usec,
#if defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC) || \
    defined(PEGASUS_OS_LINUX)
		    abs((int)utc_offset));

    // Set UTC sign
    if(utc_offset > 0)
    {
	dateTime.plusOrMinus = '-';
    }
#else
                    abs(tz.tz_minuteswest)); // take care of the sign later on

    // Set the UTC Sign
    if (tz.tz_minuteswest > 0)
    {
        dateTime.plusOrMinus = '-';
    }
    else
    {
        dateTime.plusOrMinus = '+';
    }
#endif
    currentDateTime.clear();
    strcpy(mTmpString, (char *)&dateTime);
    currentDateTime.set(mTmpString);

    return currentDateTime;
}

PEGASUS_NAMESPACE_END



