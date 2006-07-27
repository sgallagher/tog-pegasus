//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Author: Mike Brasher (m.brasher@inovadevelopment.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Threads.h"

PEGASUS_NAMESPACE_BEGIN

void Threads::sleep(int msec)
{
#if defined(PEGASUS_HAVE_NANOSLEEP)

    struct timespec wait;
    wait.tv_sec = msec / 1000;
    wait.tv_nsec = (msec % 1000) * 1000000;
    nanosleep(&wait, NULL);

#elif defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)

   int loop;
   int microsecs = msec * 1000; /* convert from milliseconds to microseconds */

   if (microsecs < 1000000)
       usleep(microsecs);
   else
   {
       loop = microsecs / 1000000;
       for(int i = 0; i < loop; i++)
           usleep(1000000);
       if ((loop*1000000) < microsecs)
           usleep(microsecs - (loop*1000000));
   }

#elif defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)

    if (ms == 0)
    {         
        Sleep(0);
        return;
    }

    struct _timeb end, now;
    _ftime( &end );
    end.time += (ms / 1000);
    ms -= (ms / 1000);
    end.millitm += ms;

    do
    {
        Sleep(0);
        _ftime(&now);
    } 
    while( end.millitm > now.millitm && end.time >= now.time);

#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
    int seconds;
    int microsecs = msec * 1000;

    if (microsecs < 1000000)
    {
        usleep(microsecs/2);
        pthread_testintr();
        usleep(microsecs/2);
    }
    else
    {
        // sleep for loop seconds
        seconds = microsecs / 1000000;
        sleep(seconds);

        // Usleep the remaining time
        if ((seconds*1000000) < microsecs)
            usleep(microsecs - (seconds*1000000));
    }
#elif defined(PEGASUS_OS_VMS)

    sleep(msec / 1000);

#endif
}

PEGASUS_NAMESPACE_END
