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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By: Amit K Arora, IBM (amita@in.ibm.com) for Bug#1917
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>
#include "CMPI_Object.h"
#include "CMPI_Broker.h"
#include "CMPI_Ftabs.h"
#include "CMPI_String.h"
#include "CMPI_SelectExp.h"

#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/ProviderManager2/ProviderManager.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/AutoPtr.h>

#if defined (CMPI_VER_85)
  #include <Pegasus/Common/MessageLoader.h>
#endif

#include <stdarg.h>
#include <string.h>

#if defined(CMPI_PLATFORM_WIN32_IX86_MSVC)
  #include <sys/timeb.h>
#endif

PEGASUS_NAMESPACE_BEGIN

extern "C" {
	struct thrd_data {
	   CMPI_THREAD_RETURN(CMPI_THREAD_CDECL*pgm)(void*);
	   void *parm;	
	   CMPIProvider *provider;
	};
}

static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL start_driver(void *parm)
{
   PEGASUS_THREAD_RETURN rc;
   Thread* my_thread = (Thread*)parm;
   thrd_data *pp = (thrd_data*)my_thread->get_parm();
   thrd_data data=*pp;

   delete pp;
   rc = (PEGASUS_THREAD_RETURN)(data.pgm)(data.parm);

   // Remove the thread from the watch-list (and clean it up).
   data.provider->removeThreadFromWatch( my_thread);
   return rc;
}

  

extern "C" {

   static char *resolveFileName (const char *filename)
   {
      String pn=ProviderManager::_resolvePhysicalName(filename);
      CString n=pn.getCString();
      return strdup((const char*)n);
   }

   static CMPI_THREAD_TYPE newThread
         (CMPI_THREAD_RETURN (CMPI_THREAD_CDECL *start )(void *), void *parm, int detached)
   {
      const CMPIBroker *brk = CM_BROKER;
      const CMPI_Broker *broker = (CMPI_Broker*)brk;

      AutoPtr<thrd_data> data(new thrd_data());
      data->pgm=(CMPI_THREAD_RETURN (CMPI_THREAD_CDECL *)(void*))start;
      data->parm=parm;
      data->provider = broker->provider;
      Thread *t=new Thread(start_driver,data.get(),detached==1);

      broker->provider->addThreadToWatch(t);
      data.release();

      ThreadStatus rtn = PEGASUS_THREAD_OK;
      while ( (rtn = t->run()) != PEGASUS_THREAD_OK)
      {
		if (rtn == PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
	 		pegasus_yield();
		else
	    {
			Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL2, \
                "Could not allocate provider thread (%p) for %s provider.",
				t,  (const char *)broker->name.getCString());
			broker->provider->removeThreadFromWatch(t);
			delete t; t = 0;
			break;
	    }		
      }
     if (rtn == PEGASUS_THREAD_OK)
     {
   	Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			"Started provider thread (%p) for %s.", 
			t, (const char *)broker->name.getCString());
     }
      return (CMPI_THREAD_TYPE)t;
   }



   static int joinThread (CMPI_THREAD_TYPE thread, CMPI_THREAD_RETURN *returnCode)
   {
      ((Thread*)thread)->join();
      *returnCode=(CMPI_THREAD_RETURN)((Thread*)thread)->get_exit();
      return 0;
   }

   static int exitThread(CMPI_THREAD_RETURN return_code)
   {
      Thread::getCurrent()->exit_self((PEGASUS_THREAD_RETURN)return_code);
      return 0;
   }

   static int cancelThread(CMPI_THREAD_TYPE thread)
   {
      ((Thread*)thread)->cancel();
      return 0;
   }

   static int threadSleep(CMPIUint32 msec)
   {
      Thread::getCurrent()->sleep(msec);
      return 0;
   }

   static int threadOnce (int *once, void (*init)(void))
   {
      if (*once==0) {
         *once=1;
         (init)();
      }
      return *once;
   }


   static int createThreadKey(CMPI_THREAD_KEY_TYPE *key, void (*cleanup)(void*))
   {
      return pegasus_key_create((PEGASUS_THREAD_KEY_TYPE*)key);
   }

   static int destroyThreadKey(CMPI_THREAD_KEY_TYPE key)
   {
      return pegasus_key_delete(key);
   }

   static void *getThreadSpecific(CMPI_THREAD_KEY_TYPE key)
   {
      return pegasus_get_thread_specific(key);
   }

   static  int setThreadSpecific(CMPI_THREAD_KEY_TYPE key, void * value)
   {
      return pegasus_set_thread_specific(key,value);
   }


   static CMPI_MUTEX_TYPE newMutex (int opt)
   {
      Mutex *m=new Mutex();
      return m;
   }

   static void destroyMutex (CMPI_MUTEX_TYPE m)
   {
      delete ((Mutex*)m);
   }

   static void lockMutex (CMPI_MUTEX_TYPE m)
   {
      ((Mutex*)m)->lock(pegasus_thread_self());
   }

   static void unlockMutex (CMPI_MUTEX_TYPE m)
   {
      ((Mutex*)m)->unlock();
   }



   static CMPI_COND_TYPE newCondition (int opt)
   {
      Condition *c=new Condition();
      return c;
   }

   static void destroyCondition (CMPI_COND_TYPE c)
   {
      delete (Condition*)c;
   }

   static int condWait (CMPI_COND_TYPE c, CMPI_MUTEX_TYPE m)
   {
      // need to take care of mutex
      ((Condition*)c)->unlocked_wait(pegasus_thread_self());
      return 0;
   }

   static int timedCondWait(CMPI_COND_TYPE c, CMPI_MUTEX_TYPE m, struct timespec *wait)
   {

      int msec;
      struct timespec next=*wait;
   #if defined(CMPI_PLATFORM_WIN32_IX86_MSVC)
      struct timeval {
         long tv_sec;
         long tv_usec;
      }now;
      struct _timeb timebuffer;
   #endif

   /* this is not truely mapping to pthread_timed_wait
      but will work for the time beeing
   */

   #if defined(CMPI_PLATFORM_WIN32_IX86_MSVC)
      _ftime( &timebuffer );
      now.tv_sec=timebuffer.time;
      now.tv_usec=timebuffer.millitm*1000;
   #else
      struct timeval now;
      gettimeofday(&now, NULL);
   #endif

      if (next.tv_nsec>1000000000) {
         next.tv_sec+=next.tv_nsec/1000000000;
         next.tv_nsec=next.tv_nsec%1000000000;
      }
      msec=(next.tv_sec-now.tv_sec)*1000;
      msec+=(next.tv_nsec/1000000)-(now.tv_usec/1000);

      Thread::getCurrent()->sleep(msec);
      return 0;
   }


   static int signalCondition(CMPI_COND_TYPE cond)
   {
      ((Condition*)cond)->signal(Thread::getCurrent()->self());
      return 0;
   }

}

static CMPIBrokerExtFT brokerExt_FT={
     CMPICurrentVersion,
     resolveFileName,

     newThread,
     joinThread,
     exitThread,
     cancelThread,
     threadSleep,
     threadOnce,

     createThreadKey,
     destroyThreadKey,
     getThreadSpecific,
     setThreadSpecific,

     newMutex,
     destroyMutex,
     lockMutex,
     unlockMutex,

     newCondition,
     destroyCondition,
     condWait,
     timedCondWait,
     signalCondition                       // Signal not supported yet
};

CMPIBrokerExtFT *CMPI_BrokerExt_Ftab=&brokerExt_FT;


PEGASUS_NAMESPACE_END



