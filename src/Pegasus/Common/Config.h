//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM,
// Compaq Computer Corporation
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
// Modified By: Rudy Schuet (rudy.schuet@compaq.com) 11/25/01
//              added NSK platform support and PEGASUS_HAVE_NO_STD option
//              K. Schopmeyer. Added  PEGASUS_TEST_EXCEPTION_DECLARATION Feb 27 2002
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Config_h
#define Pegasus_Config_h

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include <Pegasus/Common/Platform_WIN32_IX86_MSVC.h>
#elif defined (PEGASUS_PLATFORM_LINUX_IX86_GNU)
# include <Pegasus/Common/Platform_LINUX_IX86_GNU.h>
#elif defined (PEGASUS_PLATFORM_AIX_RS_IBMCXX)
# include <Pegasus/Common/Platform_AIX_RS_IBMCXX.h>
#elif defined (PEGASUS_PLATFORM_HPUX_PARISC_ACC)
# include <Pegasus/Common/Platform_HPUX_PARISC_ACC.h>
#elif defined (PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)
# include <Pegasus/Common/Platform_TRU64_ALPHA_DECCXX.h>
#elif defined (PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)
# include <Pegasus/Common/Platform_SOLARIS_SPARC_GNU.h>
#elif defined (PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# include <Pegasus/Common/Platform_ZOS_ZSERIES_IBM.h>
#elif defined (PEGASUS_PLATFORM_NSK_NONSTOP_NMCPLUS)
# include <Pegasus/Common/Platform_NSK_NONSTOP_NMCPLUS.h>
#elif defined (PEGASUS_PLATFORM_LINUX_IA64_GNU)
# include <Pegasus/Common/Platform_LINUX_IA64_GNU.h>
#else
# error "<Pegasus/Common/Config.h>: Unsupported Platform"
#endif

#include <iostream>
#include <cstdlib>


#ifdef PEGASUS_HAVE_NAMESPACES
# define PEGASUS_NAMESPACE_BEGIN namespace Pegasus {
# define PEGASUS_NAMESPACE_END }

#ifndef PEGASUS_HAVE_NO_STD
# define PEGASUS_STD(X) std::X
# define PEGASUS_USING_STD using namespace std
#else
# define PEGASUS_STD(X) X
# define PEGASUS_USING_STD
#endif
# define PEGASUS_USING_PEGASUS using namespace Pegasus
#else
# define PEGASUS_NAMESPACE_BEGIN /* empty */
# define PEGASUS_NAMESPACE_END /* empty */
# define PEGASUS_STD(X) X
# define PEGASUS_USING_STD
# define PEGASUS_USING_PEGASUS
#endif

#ifdef PEGASUS_HAVE_EXPLICIT
# define PEGASUS_EXPLICIT explicit
#else
# define PEGASUS_EXPLICIT /* empty */
#endif

#ifdef PEGASUS_HAVE_MUTABLE
# define PEGASUS_MUTABLE mutable
#else
# define PEGASUS_MUTABLE /* empty */
#endif

#ifndef PEGASUS_HAVE_FOR_SCOPE
# define for if (0) ; else for
#endif

#ifdef PEGASUS_HAVE_TEMPLATE_SPECIALIZATION
# define PEGASUS_TEMPLATE_SPECIALIZATION template <>
#else
# define PEGASUS_TEMPLATE_SPECIALIZATION
#endif


PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_HAVE_BOOLEAN
  typedef bool Boolean;
#else
# include <Pegasus/Common/Boolean.h>
#endif

typedef unsigned char Uint8;
typedef char Sint8;
typedef unsigned short Uint16;
typedef short Sint16;
typedef unsigned int Uint32;
typedef int Sint32;
typedef float Real32;
typedef double Real64;
typedef PEGASUS_UINT64 Uint64;
typedef PEGASUS_SINT64 Sint64;

#define PEGASUS_NOT_FOUND Uint32(-1)
#define PEG_NOT_FOUND Uint32(-1)

struct NoThrow { };

#define PEGASUS_NO_THROW (*((NoThrow*)0))

PEGASUS_NAMESPACE_END

#define PEGASUS_TRACE \
    PEGASUS_STD(cout) << __FILE__ << '(' << __LINE__ << ')' << PEGASUS_STD(endl)

#define PEGASUS_OUT(X) \
    PEGASUS_STD(cout) << #X << "=[" << X << "]" << PEGASUS_STD(endl)

#ifdef PEGASUS_SUPPRESS_UNREACHABLE_STATEMENTS
# define PEGASUS_UNREACHABLE(CODE)
#else
# define PEGASUS_UNREACHABLE(CODE) CODE
#endif

// used for zOS only
#ifndef PEGASUS_STATIC_CDECL
#define PEGASUS_STATIC_CDECL
#endif

#endif  /* Pegasus_Config_h */
