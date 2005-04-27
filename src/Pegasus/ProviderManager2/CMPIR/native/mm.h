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
// Author: Frank Scheffler
//
// Modified By:  Adrian Schuur (schuur@de.ibm.com)
//               Marek Szermutzky, IBM (mszermutzky@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/*!
  \file mm.h
  \brief Memory Managment system for remote providers (header file).

  \author Frank Scheffler

  \sa mm.h
  \sa native.h
*/

#ifndef _REMOTE_CMPI_TOOL_MM_H
#define _REMOTE_CMPI_TOOL_MM_H

//! States cloned objects, i.e. memory that is not being tracked.
#define TOOL_MM_NO_ADD 0

//! States tracked memory objects.
#define TOOL_MM_ADD    1

//! The initial size of trackable memory pointers per thread.
/*!
  This size is increased by the same amount, once the limit is reached.
 */
#define MT_SIZE_STEP 100


typedef struct _managed_thread managed_thread;

#ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#include <dlfcn.h>
#include <sys/types.h>
#else
#include <dll.h>
#include <stddef.h>
#endif

//! Per-Thread management structure.
/*!
  This struct is returned using a global pthread_key_t and stores all allocated
  objects that are going to be freed, once the thread is flushed or dies.
 */
struct _managed_thread {
  void *broker;
  void *ctx;
  unsigned size;		/*!< current maximum number of tracked
				  pointers */
  unsigned used;		/*!< currently tracked pointers */
  void **  objs;		/*!< array of tracked pointers */
};


void * tool_mm_load_lib ( const char * libname );

void tool_mm_flush ();
void * tool_mm_alloc ( int, size_t );
void * tool_mm_realloc ( void *, size_t );
int tool_mm_add (  void * );
void tool_mm_set_broker (  void * ,  void *);
int tool_mm_remove ( void * );
void * tool_mm_get_broker(void **);

#endif
