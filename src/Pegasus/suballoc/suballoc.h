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
//============================================================================
//
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PEGASUS_SUBALLOC_INCLUDE
#define PEGASUS_SUBALLOC_INCLUDE 1

#include <Pegasus/Common/Config.h>
#include <Pegasus/suballoc/Linkage.h>

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#include <windows.h>
#include <process.h>
#include <stdio.h>
#elif defined (PEGASUS_PLATFORM_LINUX_IX86_GNU) 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#elif defined (PEGASUS_PLATFORM_AIX_RS_IBMCXX)
#include <sched.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#elif defined (PEGASUS_PLATFORM_HPUX_PARISC_ACC) 
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#elif defined (PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#elif defined (PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#elif defined (PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#elif defined (PEGASUS_PLATFORM_LINUX_IA64_GNU)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

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

namespace 
{
   static const int GUARD_SIZE = 0x10;
   static const int MAX_PATH_LEN = 0xff;
   static const int MAX_LINE_LEN = 0x14;
   static const int MAX_CLASS_LEN = 0x40;
   static const int PRE_ALLOCATE = 0x00;
   static const int STEP_ALLOCATE = 0x01;
   static const int AVAILABLE = 0x00;
   static const int NORMAL = 0x01;
   static const int ARRAY = 0x02;
}

PEGASUS_NAMESPACE_BEGIN

class guardian;

PEGASUS_SUBALLOC_LINKAGE void * pegasus_alloc(size_t);
PEGASUS_SUBALLOC_LINKAGE void * pegasus_alloc(size_t, const Sint8 *classname, Sint8 *file, int line );
PEGASUS_SUBALLOC_LINKAGE void pegasus_free(void *, int, Sint8 *, Sint8 *, int);

PEGASUS_SUBALLOC_LINKAGE void pegasus_free(void *);

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)

#define ENOTSOCK WSAENOTSOCK
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL 
#define EAFNOSUPPORT WSAEAFNOSUPPORT 
#define EISCONN WSAEISCONN 
#define ETIMEDOUT WSAETIMEDOUT
#define ECONNREFUSED WSAECONNREFUSED
#define ENETUNREACH WSAENETUNREACH
#define EADDRINUSE WSAEADDRINUSE
#define EINPROGRESS WSAEINPROGRESS 
#define EALREADY WSAEALREADY 
#define EWOULDBLOCK WSAEWOULDBLOCK
#ifndef EINVAL
#define EINVAL WSAEINVAL
#endif
#define snprintf _snprintf
typedef HANDLE PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_LINUX_IX86_GNU) 

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_AIX_RS_IBMCXX)

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_HPUX_PARISC_ACC) 

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_NSK_NONSTOP_NMCPLUS)

typedef Uint32 PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_LINUX_IA64_GNU)

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#endif

// to reduce the number of wasted bytes:
// reduce GUARD_SIZE
// eliminate guard checking from SUBALLOC_NODEs 
// 

class peg_suballoc;


#if defined(PEGASUS_DEBUG_MEMORY)
#define PEGASUS_MEMCHECK(a) pegasus_alloc(1, (#a), __FILE__, __LINE__)

typedef class PEGASUS_SUBALLOC_LINKAGE guardian
{
   private:
      guardian(void);
   public:
      guardian(void *ptr)
	 :_ptr(ptr)
      {
      }
      virtual ~guardian(void)
      {
	 if(_ptr != 0)
	    pegasus_free(_ptr);
      }
      void *_ptr;
} PEGASUS_CHECKED_OBJECT ;

#else 

typedef class PEGASUS_SUBALLOC_LINKAGE guardian
{
   private:
      guardian(void);
      
   public:
      guardian(void *p)
      {
      }
      
      virtual ~guardian(void)
      {
      }
      
} PEGASUS_CHECKED_OBJECT;

#define PEGASUS_MEMCHECK(a) NULL
#endif 

#define PEGASUS_CHECK_PARAM void *guard
#define PEGASUS_CHECK_INIT guardian(guard)


class PEGASUS_SUBALLOC_LINKAGE peg_suballocator 
{
   private:
      peg_suballocator(const peg_suballocator &);
      peg_suballocator & operator = (const peg_suballocator &);
      Boolean _debug;

      typedef struct _subAllocTemplate {
	    struct _subAllocNode *next;
	    struct _subAllocNode *prev;
	    Boolean isHead;
	    Boolean avail;
	    Uint32 allocSize;
	    Uint8 guardPre[GUARD_SIZE];
      } SUB_TEMPLATE;

      typedef struct _subAllocNode {
	    struct _subAllocNode *next;
	    struct _subAllocNode *prev;
	    Boolean isHead;
	    int avail;
	    Uint32 allocSize;
	    Uint8 guardPre[GUARD_SIZE];
	    void *allocPtr;
	    Uint8 guardPost[GUARD_SIZE];
	    Sint8 file[MAX_PATH_LEN + 1];
	    Sint8 line[MAX_LINE_LEN + 1];
	    Sint8 classname[MAX_CLASS_LEN + 1];
	    void *concurrencyHandle;
      } SUBALLOC_NODE;


      SUBALLOC_NODE *nodeListHeads[3][16];

      PEGASUS_MUTEX_T globalSemHandle;
      PEGASUS_MUTEX_T semHandles[3][16];
      static const Sint32 nodeSizes[3][16]; 
      Uint32 allocs[3][16];
      Uint32 wastedBytes[3][16];
      Uint32 inUse[3][16];
      Uint32 avail[3][16] ;
      Uint32 totalAllocs;
      Uint32 totalMemoryInUse;
      
      static const Uint32 preAlloc[3][16];
      static const Uint32 step[3][16];
      
      Sint32 initialized;
      Sint32 init_count;
      PEGASUS_MUTEX_T init_mutex;
      Boolean debug_mode;

   public:
      static int CREATE_MUTEX(PEGASUS_MUTEX_T *mut);
      static void WAIT_MUTEX(PEGASUS_MUTEX_T *mut, Uint32 msec, int *result);
      static void WAIT_MUTEX(PEGASUS_MUTEX_T *mut);
      static void RELEASE_MUTEX(PEGASUS_MUTEX_T *mut);
      static void CLOSE_MUTEX(PEGASUS_MUTEX_T *mut);
   private:
      Boolean IS_HEAD(SUBALLOC_NODE *x);
      void INSERT(SUBALLOC_NODE *new_node, SUBALLOC_NODE *after);
      void INSERT_AFTER(SUBALLOC_NODE *new_node, SUBALLOC_NODE *after);
      void INSERT_BEFORE(SUBALLOC_NODE *new_node, SUBALLOC_NODE *before);
      void _DELETE(SUBALLOC_NODE *x);
      Boolean IS_LAST(SUBALLOC_NODE *head, SUBALLOC_NODE *node);
      Boolean IS_FIRST(SUBALLOC_NODE *head, SUBALLOC_NODE *node);
      Boolean IS_ONLY(SUBALLOC_NODE *head, SUBALLOC_NODE *node);
      inline Boolean IS_EMPTY(SUBALLOC_NODE *h) ;
      
      Boolean _Allocate(Sint32, Sint32, Sint32);
      void _DeAllocate(Sint32 vector, Sint32 index);
      SUBALLOC_NODE *GetNode(Sint32 vector, Sint32 index);
      SUBALLOC_NODE *GetHugeNode(Sint32 size);
      void PutNode(Sint32 vector, Sint32 index, SUBALLOC_NODE *node);
      void PutHugeNode(SUBALLOC_NODE *node);
      
      static const Sint8 dumpFileName[];
      Sint8 global_log_filename[MAX_PATH_LEN + 1];
      FILE *dumpFile ;
      static const Uint8 guard[];
      static const Uint8 alloc_pattern;
      static const Uint8 delete_pattern;
      
      static Boolean _CheckGuard(SUBALLOC_NODE *);


   public:

      peg_suballocator(void);
      peg_suballocator(Sint8 *log_filename, Boolean mode = true);
      virtual ~peg_suballocator(void);

      inline Boolean PEGASUS_DEBUG_ALLOC(void) { return _debug; }

      typedef class PEGASUS_SUBALLOC_LINKAGE _suballocHandle 
      { 
	 public:
	    Sint8 logpath[MAX_PATH_LEN + 1];
	    _suballocHandle(const Sint8 *log_path = 0)
	    {
	       if(log_path)
		  snprintf(logpath, MAX_PATH_LEN, "%s", log_path);
	       else
		  snprintf(logpath, MAX_PATH_LEN, "%p_suballoc_log", this);
	    }
	    virtual ~_suballocHandle(void) 
	    { 
	    }
	    
      }SUBALLOC_HANDLE;

      Boolean InitializeSubAllocator(Sint8 *f = NULL);
      static SUBALLOC_HANDLE *InitializeProcessHeap(Sint8 *f = NULL);
      void *vs_malloc(size_t size, void *handle, int type = NORMAL, const Sint8 *classname = 0, const Sint8 *f = 0, Sint32 l = 0);
      void *vs_calloc(size_t num, size_t size, void *handle, int type = NORMAL, Sint8 *f = 0, Sint32 l = 0);
      void *vs_realloc(void *pblock, size_t newsize, void *handle, int type = NORMAL, Sint8 *f = 0, Sint32 l = 0);
      Sint8 * vs_strdup(const Sint8 *string, void *handle, int type = NORMAL, Sint8 *f = 0, Sint32 l = 0);
      void vs_free(void *m);
      void vs_free(void *m, int type, Sint8 *classname, Sint8 *f, Sint32 l);
      Boolean _UnfreedNodes(void *handle);
      void DeInitSubAllocator(void *handle);
      static void _CheckNode(void *m);
      
      Boolean get_mode(void) 
      {
	 return debug_mode;
      }
      
      SUBALLOC_HANDLE & get_handle(void)
      {
	 return internal_handle;
      }
      

   private:
      _suballocHandle internal_handle;
};



inline Boolean peg_suballocator::IS_HEAD(SUBALLOC_NODE *x) { return x->isHead; }

inline Boolean peg_suballocator::IS_EMPTY(SUBALLOC_NODE *h) 
{
   return (( (h)->next == (h) && (h)->prev == (h) ) ? true : false);
}
      
inline void peg_suballocator::INSERT(SUBALLOC_NODE *new_node, SUBALLOC_NODE *after)
{ 
   new_node->prev = after;
   new_node->next = after->next;
   after->next->prev = new_node;
   after->next = new_node;
}
      
inline void peg_suballocator::INSERT_AFTER(SUBALLOC_NODE *new_node, SUBALLOC_NODE *after)
{
   INSERT(new_node, after);
   return;
}
      
inline void peg_suballocator::INSERT_BEFORE(SUBALLOC_NODE *new_node, SUBALLOC_NODE *before)
{
   new_node->next = before;
   new_node->prev = before->prev;
   before->prev->next = new_node;
   before->prev = new_node; 
}
      
inline void peg_suballocator::_DELETE(SUBALLOC_NODE *x)
{
   x->prev->next = x->next;
   x->next->prev = x->prev;	
}
      
inline Boolean peg_suballocator::IS_LAST(SUBALLOC_NODE *head, SUBALLOC_NODE *node)
{
   return ((node->prev == head && head->prev == node) ? true : false);
}
      
inline Boolean peg_suballocator::IS_FIRST(SUBALLOC_NODE *head, SUBALLOC_NODE *node)
{
   return ((node->prev == head && head->next == node) ? true : false);
}
      
inline Boolean peg_suballocator::IS_ONLY(SUBALLOC_NODE *head, SUBALLOC_NODE *node)
{
   return ((node->next == head && node->prev == head) ? true : false);
}




#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)

inline int peg_suballocator::CREATE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   if((*mut = ::CreateMutex(NULL, false, NULL))) return 0;
   return -1;
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut, Uint32 msec, int *result)
{
   assert(mut && *mut && result);
   *result = WaitForSingleObject(*mut, INFINITE);
   return;
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut)
{
   WaitForSingleObject(*mut, INFINITE);
   return;
}

inline void peg_suballocator::RELEASE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   ::ReleaseMutex(*mut);
   return;
}

inline void peg_suballocator::CLOSE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   CloseHandle(*mut);
   *mut = 0;
   return;
}


#elif defined (PEGASUS_PLATFORM_LINUX_IX86_GNU) 

inline int peg_suballocator::CREATE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   return(pthread_mutex_init(mut, NULL));
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut, Uint32 msec, int *result)
{
   *result = pthread_mutex_lock(mut);
   return;
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_lock(mut);
}

inline void peg_suballocator::RELEASE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_unlock(mut);
   return;
}

inline void peg_suballocator::CLOSE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_destroy(mut);
   return;
}

#elif defined (PEGASUS_PLATFORM_AIX_RS_IBMCXX)


inline int peg_suballocator::CREATE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   return(pthread_mutex_init(mut, NULL));
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut, Uint32 msec, int *result)
{
   *result = pthread_mutex_lock(mut);
   return;
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_lock(mut);
}

inline void peg_suballocator::RELEASE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_unlock(mut);
   return;
}

inline void peg_suballocator::CLOSE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_destroy(mut);
   return;
}

#elif defined (PEGASUS_PLATFORM_HPUX_PARISC_ACC) 

inline int peg_suballocator::CREATE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   return(pthread_mutex_init(mut, NULL));
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut, Uint32 msec, int *result)
{
   *result = pthread_mutex_lock(mut);
   return;
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_lock(mut);
}

inline void peg_suballocator::RELEASE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_unlock(mut);
   return;
}

inline void peg_suballocator::CLOSE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_destroy(mut);
   return;
}

#elif defined (PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)

inline int peg_suballocator::CREATE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   return(pthread_mutex_init(mut, NULL));
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut, Uint32 msec, int *result)
{
   *result = pthread_mutex_lock(mut);
   return;
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_lock(mut);
}

inline void peg_suballocator::RELEASE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_unlock(mut);
   return;
}

inline void peg_suballocator::CLOSE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_destroy(mut);
   return;
}

#elif defined (PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)

inline int peg_suballocator::CREATE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   return(pthread_mutex_init(mut, NULL));
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut, Uint32 msec, int *result)
{
   *result = pthread_mutex_lock(mut);
   return;
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_lock(mut);
}

inline void peg_suballocator::RELEASE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_unlock(mut);
   return;
}

inline void peg_suballocator::CLOSE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_destroy(mut);
   return;
}

#elif defined (PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)

inline int peg_suballocator::CREATE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   return(pthread_mutex_init(mut, NULL));
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut, Uint32 msec, int *result)
{
   *result = pthread_mutex_lock(mut);
   return;
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_lock(mut);
}

inline void peg_suballocator::RELEASE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_unlock(mut);
   return;
}

inline void peg_suballocator::CLOSE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_destroy(mut);
   return;
}

#elif defined (PEGASUS_PLATFORM_NSK_NONSTOP_NMCPLUS)

inline int peg_suballocator::CREATE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   return(0);
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut, Uint32 msec, int *result)
{
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut)
{
   
}

inline void peg_suballocator::RELEASE_MUTEX(PEGASUS_MUTEX_T *mut)
{

}

inline void peg_suballocator::CLOSE_MUTEX(PEGASUS_MUTEX_T *mut)
{

}

#elif defined (PEGASUS_PLATFORM_LINUX_IA64_GNU)

inline int peg_suballocator::CREATE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   return(pthread_mutex_init(mut, NULL));
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut, Uint32 msec, int *result)
{
   *result = pthread_mutex_lock(mut);
   return;
}

inline void peg_suballocator::WAIT_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_lock(mut);
}

inline void peg_suballocator::RELEASE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_unlock(mut);
   return;
}

inline void peg_suballocator::CLOSE_MUTEX(PEGASUS_MUTEX_T *mut)
{
   pthread_mutex_destroy(mut);
   return;
}

#endif

PEGASUS_NAMESPACE_END

#endif	 /* SUBALLOC_INCLUDE */


// how to debug a specific object:
// add an operator new to the object such that 
// void * operator new(size_t size, peg_suballocator::_sub_handle = 0);
// void operator delete(void *dead, size_t size);
// have operator delete check for unfreed nodes by calling sub->unfreed_nodes

