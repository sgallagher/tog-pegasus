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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//		   Ramnath Ravindran (Ramnath.Ravindran@compaq.com)
//
//%/////////////////////////////////////////////////////////////////////////////


//%// ---- inline implmentations of Unix/Linux IPC routines ---- 

#ifndef IPCUnix_inline_h
#define IPCUnix_inline_h






// the next two routines are macros that MUST SHARE the same stack frame
// they are implemented as macros by glibc. 
// native_cleanup_push( void (*func)(void *) ) ;
// these ALSO SET CANCEL STATE TO DEFER

/*
** For platforms which support these non-portable thread cleanup routines,
**
**     pthread_cleanup_push_defer_np
**     pthread_cleanup_pop_restore_np
**
** We define the following:
*/


#if !defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC) && !defined(PEGASUS_PLATFORM_SOLARIS_SPARC_GNU) && !defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) && !defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) && !defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM) && !defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU) && !defined(PEGASUS_OS_LSB)
# define PEGASUS_HAVE_NP_THREAD_CLEANUP_ROUTINES
#endif

/*
**
** Define native_cleanup_push() and native_cleanup_pop().
**
*/

#ifdef PEGASUS_HAVE_NP_THREAD_CLEANUP_ROUTINES
# define native_cleanup_push(func, arg) \
     pthread_cleanup_push_defer_np((func), arg)
# define native_cleanup_pop(execute) \
    pthread_cleanup_pop_restore_np(execute)
#else
#ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
# define native_cleanup_push(func, arg) \
    { \
	int _oldtype_; \
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &_oldtype_); \
	pthread_cleanup_push((func), arg);
# define native_cleanup_pop(execute) \
	pthread_cleanup_pop(execute); \
	pthread_setcanceltype(_oldtype_, NULL); \
    }
#else
// zOS special dish of the day
# define native_cleanup_push(func, arg) \
    { \
       int _oldtype_; \
       _oldtype_ = pthread_setintrtype(PTHREAD_INTR_ASYNCHRONOUS); \
       pthread_cleanup_push((func), arg);
# define native_cleanup_pop(execute) \
       pthread_cleanup_pop(execute); \
       _oldtype_ = pthread_setintrtype(_oldtype_); \
    }
#endif
#endif


#endif // IPCUnix_inline_h
