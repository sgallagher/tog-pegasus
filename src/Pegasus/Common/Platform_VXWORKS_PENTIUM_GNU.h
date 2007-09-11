/*
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
//%/////////////////////////////////////////////////////////////////////////////
*/

#ifndef Pegasus_Platform_VXWORKS_PENTIUM_GNU_h
#define Pegasus_Platform_VXWORKS_PENTIUM_GNU_h

#define PEGASUS_OS_TYPE_VXWORKS

#define PEGASUS_OS_VXWORKS

#define PEGASUS_ARCHITECTURE_IX86

#define PEGASUS_COMPILER_GNU

#define PEGASUS_UINT64 unsigned long long

#define PEGASUS_SINT64 long long

#define PEGASUS_HAVE_NAMESPACES

#define PEGASUS_HAVE_FOR_SCOPE

#define PEGASUS_HAVE_TEMPLATE_SPECIALIZATION

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#ifndef _REENTRANT
# define _REENTRANT
#endif

#define _THREAD_SAFE

#define gettimeofday __gettimeofday__

#ifdef __cplusplus
# include <cstddef>
# include <cassert>
# include <cstdio>
# include <cstdlib>
# include <climits>
# include <cctype>
#else
# include <assert.h>
# include <stdio.h>
# include <stdlib.h>
# include <limits.h>
# include <ctype.h>
#endif

#include <vxWorks.h>
#include <ioLib.h>
#include <sockLib.h>
#include <dirent.h>
#include <string.h>
#include <hostLib.h>
#include <sys/time.h>
#include <syslog.h>

#undef gettimeofday

#define PEGASUS_HAVE_PTHREADS

#define PEGASUS_HAVE_NANOSLEEP

#define PEGASUS_VXWORKS_PRIVILEGED_USER "root"

/* Undefine this macro (defined in vxWorks.h) */
#ifdef NONE
# undef NONE
#endif

/* Undefine this macro (defined in vxWorks.h) */
#ifdef OK
# undef OK
#endif

/* Undefine this macro (defined in vxWorks.h) */
#ifdef READ
# undef READ
#endif

/* Undefine this macro (defined in vxWorks.h) */
#ifdef WRITE
# undef WRITE
#endif

/* Undefine this macro (defined in vxWorks.h) */
#if defined(log)
#  undef log
#endif

#define bzero(a,b) memset(a,'\0',b)

inline int fork()
{
    // ATTN: not implemented for VXWorks.
    return -1;
}

inline int getrlimit(int resource, struct rlimit *rlim)
{
    // ATTN: not implemented for VXWorks.
    return -1;
}

inline int execl(const char *path, const char *arg, ...)
{
    // ATTN: not implemented for VXWorks.
    return -1;
}

inline int isascii(int c)
{
    return c >= 0 && c < 128;
}

inline int readdir_r(
    DIR* dirp, 
    struct dirent* entry,
    struct dirent** result)
{
    if ((*result = readdir(dirp)) == 0)
        return -1;

    return 0;
}

struct timezone
{
    int tz_minuteswest;
    int tz_dsttime;
};


inline int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        return -1;

    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;

    if (tz != NULL)
    {
        tz->tz_minuteswest = 0;
        tz->tz_dsttime = 0;
    }

    return 0;
}

#endif /* Pegasus_Platform_VXWORKS_PENTIUM_GNU_h */
