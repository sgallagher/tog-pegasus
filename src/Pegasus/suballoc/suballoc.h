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
//         David Eger (dteger@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PEGASUS_SUBALLOC_INCLUDE
#define PEGASUS_SUBALLOC_INCLUDE 1

#include <Pegasus/Common/Config.h>
#include <Pegasus/suballoc/Linkage.h>
#include <stdio.h>
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#elif defined (PEGASUS_PLATFORM_LINUX_GENERIC_GNU) 
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <malloc.h>

#elif defined (PEGASUS_PLATFORM_AIX_RS_IBMCXX)
#include <sched.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <malloc.h>

#elif defined (PEGASUS_PLATFORM_HPUX_ACC) 
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <malloc.h>

#elif defined (PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <malloc.h>

#elif defined (PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <malloc.h>

#elif defined (PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <malloc.h>

#elif defined (PEGASUS_PLATFORM_OS400_ISERIES_IBM)
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.cleinc>
#include <stdlib.h>

#else
# error "<Pegasus/Common/Config.h>: Unsupported Platform"
#endif

#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <signal.h>
#include <new.h>
namespace 
{
   static const int GUARD_SIZE = 0x10;
   static const int MAX_PATH_LEN = 0xff;
   static const int MAX_LINE_LEN = 0x14;
   static const int MAX_CLASS_LEN = 0x40;
   static const int PRE_ALLOCATE = 0x00;
   static const int STEP_ALLOCATE = 0x01;
   static const int AVAILABLE = 0x00;
   static const int NORMAL = 0x00;
   static const int ARRAY = 0x01;
   static const unsigned int IS_HEAD_NODE =    0x00000001;
   static const unsigned int AVAIL =           0x00000002;
   static const unsigned int ARRAY_NODE =      0x00000004;
   static const unsigned int CHECK_FAILED =    0x00000008;
   static const unsigned int ALREADY_DELETED = 0x00000010;
   static const unsigned int OVERWRITE =       0x00000020;
   static const unsigned int CHECK_LEAK =      0x00000040;
   static const unsigned int HUGE_NODE =       0x00000080;
}

PEGASUS_NAMESPACE_BEGIN

struct SUBALLOC_HANDLE;

#if defined(PEGASUS_DEBUG_MEMORY)
PEGASUS_SUBALLOC_LINKAGE void * pegasus_alloc(size_t, 
					      void *,
					      int type, 
					      const Sint8 *classname, 
					      Sint8 *file, 
					      Uint32 line );

PEGASUS_SUBALLOC_LINKAGE void pegasus_free(void *dead, 
					   void *,
					   int type, 
					   Sint8 *classname, 
					   Sint8 *file, 
					   Uint32 line);
#else
PEGASUS_SUBALLOC_LINKAGE void * pegasus_alloc(size_t);
PEGASUS_SUBALLOC_LINKAGE void pegasus_free(void *);
#endif 





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

#elif defined (PEGASUS_PLATFORM_LINUX_GENERIC_GNU) 

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_AIX_RS_IBMCXX)

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_HPUX_ACC) 

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_NSK_NONSTOP_NMCPLUS)

typedef Uint32 PEGASUS_MUTEX_T;

#elif defined (PEGASUS_PLATFORM_OS400_ISERIES_IBM)

typedef pthread_mutex_t PEGASUS_MUTEX_T;

#endif

// to reduce the number of wasted bytes:
// reduce GUARD_SIZE
// eliminate guard checking from SUBALLOC_NODEs 
// 

class peg_suballoc;

#if defined(PEGASUS_DEBUG_MEMORY)
#define PEGASUS_NEW(a, h) new((pegasus_alloc(sizeof(a), ((void *)h), NORMAL, (#a), __FILE__, __LINE__)))
#define PEGASUS_ARRAY_NEW(a, i, h) new(pegasus_alloc(sizeof(a) * (i), ((void *)(h)), ARRAY, (#a), __FILE__, __LINE__)) a
#define PEGASUS_DELETE(a) pegasus_free((a), NULL, NORMAL, (#a), __FILE__, __LINE__)
#define PEGASUS_ARRAY_DELETE(a) pegasus_free(a, NULL, ARRAY, (#a), __FILE__, __LINE__)
#define PEGASUS_CHECK_FOR_LEAKS(h) peg_suballocator::get_instance()->_UnfreedNodes((h))
#define PEGASUS_START_LEAK_CHECK() peg_suballocator::get_instance()->set_leak_mode(true)
#define PEGASUS_STOP_LEAK_CHECK() peg_suballocator::get_instance()->set_leak_mode(false)
#define PEGASUS_PRINT_NODE(a, b) peg_suballocator::PrintNodeInfo((void *)(a), (Sint8 *)(b), __FILE__, __LINE__)
#else
#define PEGASUS_NEW(a, h) new
#define PEGASUS_ARRAY_NEW(a, i, h) new a[(i)] 
#define PEGASUS_DELETE(a) delete((a))
#define PEGASUS_ARRAY_DELETE(a) delete [] ((a))
#define PEGASUS_CHECK_FOR_LEAKS(h) 
#define PEGASUS_START_LEAK_CHECK()
#define PEGASUS_STOP_LEAK_CHECK()
#define PEGASUS_PRINT_NODE(a, b)
#endif 


class PEGASUS_SUBALLOC_LINKAGE peg_suballocator 
{
   private:
      peg_suballocator(const peg_suballocator &);
      peg_suballocator & operator = (const peg_suballocator &);
      Boolean _debug;
      typedef struct _subAllocNode {
	    struct _subAllocNode *next;
	    struct _subAllocNode *prev;
	    Uint32 flags;
	    Uint32 allocSize;
	    Uint32 nodeSize;
	    void *concurrencyHandle;
#if defined(PEGASUS_DEBUG_MEMORY)
	    Sint8 file[MAX_PATH_LEN + 1];
	    Uint32 line;
	    Sint8 classname[MAX_CLASS_LEN + 1];
	    Sint8 d_file[MAX_PATH_LEN + 1];
	    Uint32 d_line;
	    Sint8 d_classname[MAX_CLASS_LEN + 1];
	    Uint8 guardPre[GUARD_SIZE];
#endif
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
      Boolean abort_on_error;
      Boolean check_for_leaks;
  
   public:
      static int CREATE_MUTEX(PEGASUS_MUTEX_T *mut);
      static void WAIT_MUTEX(PEGASUS_MUTEX_T *mut, Uint32 msec, int *result);
      static void WAIT_MUTEX(PEGASUS_MUTEX_T *mut);
      static void RELEASE_MUTEX(PEGASUS_MUTEX_T *mut);
      static void CLOSE_MUTEX(PEGASUS_MUTEX_T *mut);
   private:
      static Boolean IS_ARRAY(SUBALLOC_NODE *x);
      static Boolean IS_HEAD(SUBALLOC_NODE *x);
      static void INSERT(SUBALLOC_NODE *new_node, SUBALLOC_NODE *after);
      static void INSERT_AFTER(SUBALLOC_NODE *new_node, SUBALLOC_NODE *after);
      static void INSERT_BEFORE(SUBALLOC_NODE *new_node, SUBALLOC_NODE *before);
      static void _DELETE(SUBALLOC_NODE *x);
      static Boolean IS_LAST(SUBALLOC_NODE *head, SUBALLOC_NODE *node);
      static Boolean IS_FIRST(SUBALLOC_NODE *head, SUBALLOC_NODE *node);
      static Boolean IS_ONLY(SUBALLOC_NODE *head, SUBALLOC_NODE *node);
      static Boolean IS_EMPTY(SUBALLOC_NODE *h) ;
      
      Boolean _Allocate(Sint32, Sint32, Sint32);
      void _DeAllocate(Sint32 vector, Sint32 index);
      SUBALLOC_NODE *GetNode(Sint32 vector, Sint32 index);
      SUBALLOC_NODE *GetHugeNode(Sint32 size);
      void PutNode(Sint32 vector, Sint32 index, SUBALLOC_NODE *node);
      void PutHugeNode(SUBALLOC_NODE *node);
#if defined(PEGASUS_DEBUG_MEMORY)      
      static const Uint8 guard[];
      static const Uint8 alloc_pattern;
      static const Uint8 delete_pattern;
      
      static Boolean _CheckGuard(SUBALLOC_NODE *);
      
      
      static Uint32 _CheckNode(void *m);
      SUBALLOC_NODE *_CheckNode(void *m, int type, Sint8 *file, Uint32 line);
   public:
      static Uint32  CheckMemory(void *);
      static void PrintNodeInfo(void *, Sint8 *, Sint8 *, Uint32);
      
#endif
   public:

      peg_suballocator(void);
      peg_suballocator(Boolean mode );
      virtual ~peg_suballocator(void);

      inline Boolean PEGASUS_DEBUG_ALLOC(void) { return _debug; }

      typedef class PEGASUS_SUBALLOC_LINKAGE _suballocHandle 
      { 
	 public:
	    Sint8 logpath[MAX_PATH_LEN + 1];
	    Sint8 classname[MAX_CLASS_LEN + 1];
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

      Boolean InitializeSubAllocator(void);
      static SUBALLOC_HANDLE *InitializeProcessHeap(Sint8 *f = NULL);
#if defined(PEGASUS_DEBUG_MEMORY)
      void *vs_malloc(size_t size, void *handle, int type = NORMAL, const Sint8 *classname = 0, const Sint8 *f = 0, Uint32 l = 0);
      void *vs_calloc(size_t num, size_t size, void *handle, int type = NORMAL, Sint8 *f = 0, Uint32 l = 0);
      void *vs_realloc(void *pblock, size_t newsize, void *handle, int type = NORMAL, Sint8 *f = 0, Uint32 l = 0);
      Sint8 * vs_strdup(const Sint8 *string, void *handle, int type = NORMAL, Sint8 *f = 0, Uint32 l = 0);
      void vs_free(void *m, void *handle, int type, Sint8 *classname, Sint8 *f, Uint32 l);
      Boolean _UnfreedNodes(void *handle);
      Boolean get_mode(void) { return debug_mode; }
      void set_leak_mode(Boolean mode) { check_for_leaks = mode; }
#else
      void *vs_malloc(size_t size);
      void *vs_calloc(size_t num, size_t size);
      void *vs_realloc(void *pblock, size_t newsize);
      Sint8 *vs_strdup(const Sint8 *str);
      void vs_free(void *m);
      Boolean get_mode(void) { return false; }
      void set_leak_mode(Boolean mode) {  }
#endif
      void DeInitSubAllocator(void *handle);
      static peg_suballocator *get_instance(void);
      SUBALLOC_HANDLE & get_handle(void) { return internal_handle; }
   private:
      _suballocHandle internal_handle;
      static peg_suballocator *_suballoc_instance;
      
};

inline Boolean peg_suballocator::IS_ARRAY(SUBALLOC_NODE *x)
{
   return (x->flags & ARRAY_NODE) ? true : false ;
}

inline Boolean peg_suballocator::IS_HEAD(SUBALLOC_NODE *x) 
{ 
   return (x->flags & IS_HEAD_NODE) ? true : false ; 
}

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


#elif defined (PEGASUS_PLATFORM_LINUX_GENERIC_GNU) 

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

#elif defined (PEGASUS_PLATFORM_HPUX_ACC) 

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

#elif defined (PEGASUS_PLATFORM_OS400_ISERIES_IBM)

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

#endif

PEGASUS_NAMESPACE_END

#endif	 /* SUBALLOC_INCLUDE */


