//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: Roger Kumpf, Hewlett Packard Company (roger_kumpf@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/CIMDateTime.h>

#include <sys/time.h>
#include <cstring>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMDateTime CIMDateTime::getCurrentDateTime()
{
    char dateTimeBuffer[26];
    time_t systemTime;
    struct tm* tmval;
    struct tm tmvalBuffer;
    int tzMinutesEast;

    // Get the system date and time
    systemTime = time(NULL);

    // Get the localtime
    tmval = localtime_r(&systemTime, &tmvalBuffer);
    PEGASUS_ASSERT(tmval != 0);

    // Get the UTC offset
#if defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC)
    tzMinutesEast =
        - (int)((tmval->tm_isdst > 0 && daylight) ? altzone : timezone) / 60;
#elif defined(PEGASUS_OS_HPUX)
    tzMinutesEast = - (int) timezone / 60;
    if ((tmval->tm_isdst > 0) && daylight)
    {
        // ATTN: It is unclear how to determine the DST offset.  Assume 1 hour.
        tzMinutesEast += 60;
    }
#elif defined(PEGASUS_OS_LINUX)
    tzMinutesEast = (int) tmval->tm_gmtoff/60;
#else
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    tzMinutesEast = -tz.tz_minuteswest;
#endif

    // Format the date
    sprintf(
        dateTimeBuffer,
        "%04d%02d%02d%02d%02d%02d.%06ld%+04d",
        1900 + tmval->tm_year,
        tmval->tm_mon + 1,
        tmval->tm_mday,
        tmval->tm_hour,
        tmval->tm_min,
        tmval->tm_sec,
        0L,    // localtime_r does not return sub-second data
        tzMinutesEast);

    return CIMDateTime(dateTimeBuffer);
}

PEGASUS_NAMESPACE_END
