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

#ifndef _Pegasus_Alloc_h
#define _Pegasus_Alloc_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <stdlib.h>
#include <malloc.h>

#if defined(__cplusplus)
# include <new>

extern "C" PEGASUS_COMMON_LINKAGE void* pegasus_malloc(size_t size);
extern "C" PEGASUS_COMMON_LINKAGE void (*pegasus_malloc_notifier)(size_t total);
extern "C" PEGASUS_COMMON_LINKAGE void* pegasus_realloc(void* ptr, size_t size);
extern "C" PEGASUS_COMMON_LINKAGE void pegasus_free(void* ptr);

inline void* operator new(size_t size) throw(std::bad_alloc)
{
    return pegasus_malloc(size);
}

inline void* operator new[](size_t size) throw(std::bad_alloc)
{
    return pegasus_malloc(size);
}

inline void operator delete(void* ptr)
{
    pegasus_free(ptr);
}

inline void operator delete[](void* ptr)
{
    pegasus_free(ptr);
}

inline void operator delete(void* ptr, size_t)
{
    pegasus_free(ptr);
}

inline void operator delete[](void* ptr, size_t)
{
    pegasus_free(ptr);
}

#else /* defined(__cplusplus) */

PEGASUS_COMMON_LINKAGE void* pegasus_malloc(size_t size);
PEGASUS_COMMON_LINKAGE void (*pegasus_malloc_notifier)(size_t total);
PEGASUS_COMMON_LINKAGE void* pegasus_realloc(void* ptr, size_t size);
PEGASUS_COMMON_LINKAGE void pegasus_free(void* ptr);

#endif /* !defined(__cplusplus) */

#ifndef PEGASUS_INSIDE_ALLOC_CPP
# define malloc pegasus_malloc
# define realloc pegasus_realloc
# define free pegasus_free
#endif

#endif /* _Pegasus_Alloc_h */
