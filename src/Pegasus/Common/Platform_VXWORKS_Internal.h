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

#ifndef _Pegasus_Common_Platform_VXWORKS_Internal_h
#define _Pegasus_Common_Platform_VXWORKS_Internal_h

#define PEGASUS_VXWORKS_USER "vxworks"

#ifdef __cplusplus
# include <new>
# include <cstddef>
# include <cassert>
# include <cstdio>
# include <cstdlib>
# include <climits>
# include <cctype>
# include <iostream>
# include <fstream>
# include <cstdarg>
#else
# include <assert.h>
# include <stdio.h>
# include <stdlib.h>
# include <limits.h>
# include <ctype.h>
# include <stdarg.h>
#endif

#include <vxWorks.h>
#include <ioLib.h>
#include <sockLib.h>
#include <taskLib.h>
#include <dirent.h>
#include <string.h>
#include <hostLib.h>
#include <time.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <selectLib.h>
#include <pthread.h>

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

inline int pipe(int fds[2])
{
    // ATTN: not implemented for VXWorks.
    return -1;
}

inline void _exit(int status)
{
    exit(status);
}

inline int open(const char* path, int flags)
{
    return ::open(path, flags, 0777);
}

inline int mkdir(const char* path, int mode)
{
    return ::mkdir(path);
}

inline int __pegasus_ioctl(int fd, int func, ...)
{
    va_list ap;
    va_start(ap, func);
    int arg = va_arg(ap, int);
    int rc = ::ioctl(fd, func, arg);
    va_end(ap);
    return rc;
}

inline void __pegasus_bzero(void* buffer, size_t size)
{
    memset(buffer, '\0', size);
}

inline int readdir_r(DIR* dirp, struct dirent* entry, struct dirent** result)
{
    errno = 0;
    *result = readdir(dirp);
    return 0;
}

inline int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        return -1;

    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;

    if (tz != NULL)
    {
        struct tm tm;
        time_t t;

        if (localtime_r(&t, &tm) == 0)
        {
            tz->tz_minuteswest = 0;
            tz->tz_dsttime = tm.tm_isdst;
        }
        else
        {
            tz->tz_minuteswest = 0;
            tz->tz_dsttime = 0;
        }
    }

    return 0;
}

inline struct tm* __pegasus_localtime_r(time_t* time, struct tm* buffer)
{
    int rc = localtime_r(time, buffer);

    if (rc != 0)
        return 0;

    return buffer;
}

#if 0

inline void* operator new(size_t n) throw(std::bad_alloc)
{
    return malloc(n);
}

inline void operator delete(void* ptr) throw()
{
    free(ptr);
}

inline void* operator new[](size_t n) throw(std::bad_alloc)
{
    return malloc(n);
}

inline void operator delete[](void* ptr) throw()
{
    free(ptr);
}

#endif

static char* environ[] = { (char*)0 };

#define ioctl __pegasus_ioctl
#define localtime_r __pegasus_localtime_r
#define bzero __pegasus_bzero
#undef NONE
#undef LOCAL
#ifndef PEGASUS_KEEP_OK_MACRO
# undef OK
#endif
#undef READ
#undef WRITE
#undef log
#undef DETACHED
#undef JOINABLE

#endif /* _Pegasus_Common_Platform_VXWORKS_Internal_h */
