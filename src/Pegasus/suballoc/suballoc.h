//%///////////-*-c++-*-//////////////////////////////////////////////////////
//
// Copyright (c) 2000 - 2002 The Open group, BMC Software, Tivoli Systems, IBM
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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PEGASUS_SUBALLOC_INCLUDE
#define PEGASUS_SUBALLOC_INCLUDE 1
 
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/internal_dq.h>
#include <Pegasus/Common/IPC.h>

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#include <windows.h>
#include <process.h>
typedef HANDLE PEGASUS_MUTEX_T

#elif defined (PEGASUS_PLATFORM_LINUX_IX86_GNU) 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_AIX_RS_IBMCXX)
#include <sched.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_HPUX_PARISC_ACC) 
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_NSK_NONSTOP_NMCPLUS)

typedef Uint32 PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_LINUX_IA64_GNU)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
typedef pthread_mutex_t PEGASUS_MUTEX_T;
#else
# error "<Pegasus/Common/Config.h>: Unsupported Platform"
#endif

#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <errno.h>
#include <signal.h>

PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_DEBUG_ALLOC

#define GUARD_SIZE 0x10
#define MAX_PATH_LEN 0xff
#define MAX_LINE_LEN 0x14
typedef struct _suballocHandle {
	void (*de_init)(Uint32);
	void *(*malloc)(size_t, Uint32, Sint8 *f, Sint32 l);
	void *(*calloc)(size_t, size_t, Uint32, Sint8 *f, Sint32 l);
	void *(*realloc)(void *, size_t, Uint32, Sint8 *f, Sint32 l);
	Sint8 *(*strdup)(const Sint8 *, Uint32, Sint8 *f, Sint32 l);
	Sint16 *(*wcsdup)(const Sint16 *, Uint32, Sint8 *f, Sint32 l);
	BOOL (*unfreed)(Uint32);
	Sint8 logpath[256];

}SUBALLOC_HANDLE;


#else

typedef struct _suballocHandle {
	void (*de_init)(Uint32);
	void *(*malloc)(size_t, Uint32);
	void *(*calloc)(size_t, size_t, Uint32);
	void *(*realloc)(void *, size_t, Uint32);
	Sint8 *(*strdup)(const Sint8 *, Uint32);
	Sint16 *(*wcsdup)(const Sint16 *, Uint32);
	BOOL (*unfreed)(Uint32);
}SUBALLOC_HANDLE;

#endif

typedef struct _subAllocTemplate {
	struct _subAllocNode *next;
	struct _subAllocNode *prev;
	BOOL isHead;
	BOOL avail;
	Uint32 allocSize;
#ifdef PEGASUS_DEBUG_ALLOC
	Uint8 guardPre[GUARD_SIZE];
#endif
} SUB_TEMPLATE;

typedef struct _subAllocNode {
	struct _subAllocNode *next;
	struct _subAllocNode *prev;
	BOOL isHead;
	BOOL avail;
	Uint32 allocSize;
#ifdef PEGASUS_DEBUG_ALLOC
	Uint8 guardPre[GUARD_SIZE];
#endif
	void *allocPtr;
#ifdef PEGASUS_DEBUG_ALLOC
	Uint8 guardPost[GUARD_SIZE];
	Sint8 file[MAX_PATH_LEN + 1];
	Sint8 line[MAX_LINE_LEN + 1];
#endif
	void *concurrencyHandle;
} SUBALLOC_NODE;

/* is node x the head of the list? */
#define IS_HEAD(x) \
	(x)->isHead
/* where h is the head of the list */
#define IS_EMPTY(h) \
	( (h)->next == (h) && (h)->prev == (h) ) ? TRUE : FALSE

/* where n is the new node, insert it immediately after node x */
/* x can be the head of the list */
#define INSERT(n, x)   	\
        {(n)->prev = (x); 		\
	(n)->next = (x)->next; 	\
	(x)->next->prev = (n); 	\
	(x)->next = (n);}

#define INSERT_AFTER INSERT
#define INSERT_BEFORE(n, x)   \
	{(n)->next = (x);					\
	(n)->prev = (x)->prev;				\
	(x)->prev->next = (n);				\
	(x)->prev = (n); }

/* delete node x  - harmless if mib is empty */
/* void DELETE_MIB(MIBVARS *x); */
#define _DELETE(x)				\
	{(x)->prev->next = (x)->next;	\
	(x)->next->prev = (x)->prev;}	

/* given the head of the list h, determine if node x is the last node */
#define IS_LAST(h, x) \
	((x)->prev == (h) && (h)->prev == (x)) ? TRUE : FALSE

/* given the head of the list h, determine if node x is the first node */
#define IS_FIRST(h, x) \
	((x)->prev == (h) && (h)->next == (x)) ? TRUE : FALSE

/* given the head of the list h, determine if node x is the only node */
#define IS_ONLY(h, x) \
	((x)->next == (h) && (x)->prev == (h)) ? TRUE : FALSE

#define PRE_ALLOCATE 0
#define STEP_ALLOCATE 1

PEGASUS_NAMESPACE_END

#endif	/* SUBALLOC_INCLUDE */

