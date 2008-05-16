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

#ifndef Pegasus_Platform_ZOS_ZSERIES_IBM_h
#define Pegasus_Platform_ZOS_ZSERIES_IBM_h

// added for Native ASCII Support
#pragma runopts("FILETAG(AUTOCVT,AUTOTAG)")

#include <sched.h>
// include here to avoid redefinition from below
#include <pthread.h>

#ifdef __cplusplus
#include <cstddef>
#endif

#include <stdarg.h>
#include <stdio.h>
// usleep() function in Threads.h
#include <unistd.h>

#define PEGASUS_OS_TYPE_UNIX

#define PEGASUS_OS_ZOS

#define PEGASUS_ARCHITECTURE_ZSERIES

#define PEGASUS_COMPILER_IBM

#define PEGASUS_UINT64 unsigned long long int

#define PEGASUS_SINT64 long long int

#define PEGASUS_HAVE_NAMESPACES

#define PEGASUS_HAVE_FOR_SCOPE

#define PEGASUS_HAVE_TEMPLATE_SPECIALIZATION

#define PEGASUS_HAS_SIGNALS

#define PEGASUS_MAXHOSTNAMELEN  256

#define PEGASUS_NO_PASSWORDFILE


//#define snprintf(sptr,len,form,data) sprintf(sptr,form,data)

#define ZOS_SECURITY_NAME "CIMServer Security"

#define ZOS_DEFAULT_PEGASUS_REPOSITORY "/var/wbem"

#define PEGASUS_HAVE_BROKEN_GLOBAL_CONSTRUCTION

#define PEGASUS_HAVE_PTHREADS

// use the NON-POSIX conform interface version if required
// to avoid lots of ifdefs for z/OS
#define pthread_getspecific(x) pthread_getspecific_d8_np(x);

#ifndef _CMPIR_PROXY_PROVIDER_H_
// use my own snprintf implementation using sprintf mapping
// until the Java SDK is fixed to work with _ISOC99_SOURCE
// ATTN: a local definition of _ISOC99_SOURCE resides in
//       TraceLogHandler.cpp
inline int snprintf(char *sptr, size_t len, const char* form, ...)
{
    int returnvalue;
    va_list arguments;
    va_start(arguments, form);
    returnvalue = vsprintf(sptr, form, arguments);
    va_end(arguments);
    return returnvalue;
}
#endif

#endif /* Pegasus_Platform_ZOS_ZSERIES_IBM_h */
