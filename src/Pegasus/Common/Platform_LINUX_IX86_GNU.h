//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Modified By: Mike Day (mdday@us.ibm.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Bapu Patil (bapu_patil@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Platform_LINUX_IX86_GNU_h
#define Pegasus_Platform_LINUX_IX86_GNU_h

#include <cstddef>

#define PEGASUS_OS_TYPE_UNIX

#ifndef PEGASUS_PLATFORM_LINUX_GENERIC_GNU
#define PEGASUS_PLATFORM_LINUX_GENERIC_GNU
#endif
#define PEGASUS_OS_LINUX

#define PEGASUS_ARCHITECTURE_IX86

#define PEGASUS_COMPILER_GNU

#define PEGASUS_UINT64 unsigned long long

#define PEGASUS_SINT64 long long

#define PEGASUS_HAVE_NAMESPACES

#define PEGASUS_HAVE_EXPLICIT

#define PEGASUS_HAVE_MUTABLE

#define PEGASUS_HAVE_FOR_SCOPE

// #define PEGASUS_HAVE_TEMPLATE_SPECIALIZATION

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef _REENTRANT
#define _REENTRANT
#endif
#define _THREAD_SAFE
#include <features.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h> // For MAXHOSTNAMELEN
#endif /* Pegasus_Platform_LINUX_IX86_GNU_h */
