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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _CMPIOS_H_
#define _CMPIOS_H_

#define CMPI_THREAD_RETURN      void*
#define CMPI_THREAD_TYPE        void*
#define CMPI_MUTEX_TYPE         void*
#define CMPI_COND_TYPE          void*

#if defined(CMPI_PLATFORM_WIN32_IX86_MSVC)
   #define CMPI_THREAD_CDECL    __stdcall
   #define CMPI_THREAD_KEY_TYPE unsigned long int

struct timespec {
   long tv_sec;
   long tv_nsec;
};

#elif defined( PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)

#ifndef __cplusplus
   #define CMPI_THREAD_CDECL
#else
   #define CMPI_THREAD_CDECL    __cdecl
#endif

   #define CMPI_THREAD_KEY_TYPE  pthread_key_t
#else
   #define CMPI_THREAD_CDECL
   #define CMPI_THREAD_KEY_TYPE unsigned long int
#endif


#endif
