//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_Object.h"
#include "CMPI_Broker.h"
#include "CMPI_Ftabs.h"
#include "CMPI_String.h"
#include "CMPI_SelectExp.h"

#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/ProviderManager2/ProviderManager.h>
#include <Pegasus/Common/Thread.h>

#if defined (CMPI_VER_85)
#include <Pegasus/Common/MessageLoader.h>
#endif

#include <stdarg.h>
#include <string.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

static char *resolveFileName (const char *filename)
{
   String pn=ProviderManager::_resolvePhysicalName(filename);
   CString n=pn.getCString();
   return strdup((const char*)n);
}

struct thrd_data {
   CMPI_THREAD_RETURN(CMPI_THREAD_CDECL*pgm)(void*);
   void *parm;
};

static CMPI_THREAD_RETURN CMPI_THREAD_CDECL start_driver(void *parm)
{
    Thread* my_thread = (Thread*)parm;
    thrd_data *pp = (thrd_data*)my_thread->get_parm();
    thrd_data data=*pp;
    delete pp;

    return (data.pgm)(data.parm);
}

static CMPI_THREAD_TYPE newThread
        (CMPI_THREAD_RETURN (CMPI_THREAD_CDECL *start )(void *), void *parm, int detached)
{
   thrd_data *data=new thrd_data();
   data->pgm=start;
   data->parm=parm;

   Thread *t=new Thread(start_driver,data,detached==1);
   t->run();
   return (CMPI_THREAD_TYPE)t;
}

static unsigned int createThreadKey(CMPI_THREAD_KEY_TYPE *key, void (*cleanup)(void*))
{
   return pegasus_key_create(key);
}

static void *getThreadSpecific(CMPI_THREAD_KEY_TYPE key)
{
   return pegasus_get_thread_specific(key);
}

static unsigned int setThreadSpecific(CMPI_THREAD_KEY_TYPE key, void * value)
{
   return pegasus_set_thread_specific(key,value);
}

static int threadOnce (int *once, void (*init)(void))
{
   if (*once==0) {
      *once=1;
      (init)();
   }
   return *once;
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

static int timedCondWait(CMPI_COND_TYPE c, CMPI_MUTEX_TYPE m, struct timespec *wait)
{
        printf("--- timedCondWait() not supported yet\n");
	exit(12);
}


static CMPIBrokerExtFT brokerExt_FT={
     CMPICurrentVersion,
     resolveFileName,
     newThread,
     createThreadKey,
     getThreadSpecific,
     setThreadSpecific,
     NULL,                      // Join not supported yet
     threadOnce,
     newMutex,
     destroyMutex,
     lockMutex,
     unlockMutex,
     newCondition,
     destroyCondition,
     timedCondWait,
     NULL                       // Signal not supported yet
};

CMPIBrokerExtFT *CMPI_BrokerExt_Ftab=&brokerExt_FT;


PEGASUS_NAMESPACE_END



