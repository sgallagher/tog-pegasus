//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: ConfigUnix.h,v $
// Revision 1.1.1.1  2001/01/14 19:50:40  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// ConfigUnix_h
//
//	This file contains definitions for most Unix operating systems and
//	is included by Config.h.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ConfigUnix_h
#define Pegasus_ConfigUnix_h

#include <cstddef>

#define PEGASUS_MACHINE_IX86
#define PEGASUS_OS_UNIX
// #define PEGASUS_OS_TYPE_UNIX
#define PEGASUS_COMPILER_GCC

#define PEGASUS_NAMESPACE_BEGIN namespace Pegasus {

#define PEGASUS_NAMESPACE_END }

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_COMMON_LINKAGE /* empty */
#define PEGASUS_REPOSITORY_LINKAGE /* empty */
#define PEGASUS_PROTOCOL_LINKAGE /* empty */
#define PEGASUS_SERVER_LINKAGE /* empty */
#define PEGASUS_CLIENT_LINKAGE /* empty */

// Not a big-endian machine:

#define PEGASUS_BIG_ENDIAN 0
#define PEGASUS_LITTLE_ENDIAN 0

typedef unsigned char Uint8;
typedef char Sint8;
typedef unsigned short Uint16;
typedef short Sint16;
typedef unsigned int Uint32;
typedef int Sint32;
typedef float Real32;
typedef double Real64;
typedef bool Boolean;
typedef unsigned long long Uint64;
typedef long long Sint64;

// Flags needed by ACE:

// #define _POSIX_THREADS
// #define _POSIX_THREAD_SAFE_FUNCTIONS
// #define _REENTRANT
// #define ACE_HAS_AIO_CALLS

#define PEGASUS_IOS_BINARY /* empty */

PEGASUS_NAMESPACE_END

#endif  /* Pegasus_ConfigUnix_h */
