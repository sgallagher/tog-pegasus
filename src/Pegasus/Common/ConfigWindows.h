//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// ConfigWindows.h
//
//	This file contains definitions for the Intel X86 running Windows98
//	using the Microsoft Visual C++ compiler. This file must
//	be included in Config.h to be used.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ConfigWindows_h
#define Pegasus_ConfigWindows_h

#define PEGASUS_OS_TYPE_WINDOWS

#define PEGASUS_IOS_BINARY ,std::ios::binary

#define PEGASUS_EXPORT __declspec(dllexport)

#define PEGASUS_IMPORT __declspec(dllimport)

#ifdef PEGASUS_COMMON_INTERNAL
# define PEGASUS_COMMON_LINKAGE PEGASUS_EXPORT
#else
# define PEGASUS_COMMON_LINKAGE PEGASUS_IMPORT
#endif

#ifdef PEGASUS_REPOSITORY_INTERNAL
# define PEGASUS_REPOSITORY_LINKAGE PEGASUS_EXPORT
#else
# define PEGASUS_REPOSITORY_LINKAGE PEGASUS_IMPORT
#endif

#ifdef PEGASUS_PROTOCOL_INTERNAL
# define PEGASUS_PROTOCOL_LINKAGE PEGASUS_EXPORT
#else
# define PEGASUS_PROTOCOL_LINKAGE PEGASUS_IMPORT
#endif

#ifdef PEGASUS_CLIENT_INTERNAL
# define PEGASUS_CLIENT_LINKAGE PEGASUS_EXPORT
#else
# define PEGASUS_CLIENT_LINKAGE PEGASUS_IMPORT
#endif

#ifdef PEGASUS_SERVER_INTERNAL
# define PEGASUS_SERVER_LINKAGE PEGASUS_EXPORT
#else
# define PEGASUS_SERVER_LINKAGE PEGASUS_IMPORT
#endif

#ifdef PEGASUS_PROVIDER_INTERNAL
# define PEGASUS_PROVIDER_LINKAGE PEGASUS_EXPORT
#else
# define PEGASUS_PROVIDER_LINKAGE PEGASUS_IMPORT
#endif

#ifdef PEGASUS_COMPILER_INTERNAL
# define PEGASUS_COMPILER_LINKAGE PEGASUS_EXPORT
#else
# define PEGASUS_COMPILER_LINKAGE PEGASUS_IMPORT
#endif

#ifdef PEGASUS_CMDLINE_INTERNAL
# define PEGASUS_CMDLINE_LINKAGE PEGASUS_EXPORT
#else
# define PEGASUS_CMDLINE_LINKAGE PEGASUS_IMPORT
#endif

#ifdef PEGASUS_GETOOPT_INTERNAL
# define PEGASUS_GETOOPT_LINKAGE PEGASUS_EXPORT
#else
# define PEGASUS_GETOOPT_LINKAGE PEGASUS_IMPORT
#endif

// Suppress this warning: "identifier was truncated to '255' characters in the
// debug information":
#pragma warning( disable : 4786 )

#pragma warning ( disable : 4251 )

#endif  /* Pegasus_ConfigWindows_h */
