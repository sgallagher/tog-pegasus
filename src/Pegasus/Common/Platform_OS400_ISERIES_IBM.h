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
// Author: Chuck Carmack (carmack@us.ibm.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Platform_OS400_ISERIES_IBM_h
#define Pegasus_Platform_OS400_ISERIES_IBM_h

//#define _XOPEN_SOURCE_EXTENDED 1
//#define _OPEN_SYS 1
#define _MSE_PROTOS
#define _OPEN_SOURCE 3

#define _MULTI_THREADED 

#ifndef PEGASUS_INTERNALONLY
  // Need this because Sint8 is typedef as a char,
  // and char defaults to unsigned on OS/400.
  // Internal Pegasus code needs char to be unsigned
  // so that Sint8 * can be cast to char *.
  // External code needs char to be signed for comparison
  // operators (<, <=, etc).
  #pragma chars(signed)
#endif 

#include <cstddef>

#define OS400_DEFAULT_PEGASUS_HOME "/QIBM/UserData/OS400/CIM"

#define PEGASUS_OS_TYPE_UNIX

#include <ifs.cleinc>
  
#define PEGASUS_OS_OS400

#define PEGASUS_ARCHITECTURE_ISERIES

#define PEGASUS_COMPILER_IBM

#define PEGASUS_UINT64 unsigned long long

#define PEGASUS_SINT64 signed long long  

#define PEGASUS_HAVE_NAMESPACES

#define PEGASUS_HAVE_EXPLICIT

#define PEGASUS_HAVE_MUTABLE

#define PEGASUS_HAVE_FOR_SCOPE

#define PEGASUS_HAVE_TEMPLATE_SPECIALIZATION

#define PEGASUS_HAVE_EBCDIC

#define PEGASUS_HAVE_IOS_BINARY

#define PEGASUS_STATIC_CDECL __cdecl

#define PEGASUS_NO_PASSWORDFILE

#define PEGASUS_LOCAL_DOMAIN_SOCKET

#define PEGASUS_HAS_PERFINST

#endif /* Pegasus_Platform_OS400_ISERIES_IBM_h */
