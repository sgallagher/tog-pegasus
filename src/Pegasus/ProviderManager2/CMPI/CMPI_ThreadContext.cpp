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

#include "CMPI_Object.h"

#include <pthread.h>
#include <limits.h>


PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

//ulong CMPI_ThreadContext::hiKey=PTHREAD_KEYS_MAX+1;
pthread_key_t CMPI_ThreadContext::contextKey;
pthread_once_t CMPI_ThreadContext::context_key_once=PTHREAD_ONCE_INIT;

void CMPI_ThreadContext::context_key_alloc()
{
    pthread_key_create(&contextKey,NULL);
}

pthread_key_t CMPI_ThreadContext::getContextKey()
{
    pthread_once(&context_key_once,context_key_alloc);
    return contextKey;
}

void CMPI_ThreadContext::add(CMPI_Object *o) {
   ENQ_TOP_LIST(o,CIMfirst,CIMlast,next,prev);
}

void CMPI_ThreadContext::addObject(CMPI_Object* o) {
   CMPI_ThreadContext* ctx=getThreadContext();
   ctx->add(o);
}

void CMPI_ThreadContext::remove(CMPI_Object *o) {
   DEQ_FROM_LIST(o,CIMfirst,CIMlast,next,prev);
}

void CMPI_ThreadContext::remObject(CMPI_Object* o) {
   CMPI_ThreadContext* ctx=getThreadContext();
   ctx->remove(o);
}

CMPI_ThreadContext* CMPI_ThreadContext::getThreadContext() {
   pthread_key_t k=getContextKey();
//   cerr<<"+++ pthread_key_t: "<<(void*)k<<endl;
   #ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
   return (CMPI_ThreadContext*)pthread_getspecific(k);
   #else
    CMPI_ThreadContext* tCtx=NULL;
    pthread_getspecific(k,(void**)&tCtx);
    return tCtx;
   #endif
}

CMPIBroker* CMPI_ThreadContext::getBroker() {
   return getThreadContext()->broker;
}

CMPIContext* CMPI_ThreadContext::getContext() {
   return getThreadContext()->context;
}

CMPI_ThreadContext::CMPI_ThreadContext(CMPIBroker *mb, CMPIContext *ctx ) {
   CIMfirst=CIMlast=NULL;
   broker=mb;
   context=ctx;
//   cerr<<"+++++++++++++++++++++++++"<<endl;
   pthread_key_t k=getContextKey();
   #ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
   prev=(CMPI_ThreadContext*)pthread_getspecific(k);
   #else
    pthread_getspecific(k,(void**)&prev);
   #endif
   if (prev) cerr<<"+++ OLD_DATA: "<<(void*)prev<<endl;
   pthread_setspecific(k,this);
   return;
}

CMPI_ThreadContext::~CMPI_ThreadContext() {
//   cerr<<"-------------------------"<<endl;

   for (CMPI_Object *nxt,*cur=CIMfirst; cur; cur=nxt) {
      nxt=cur->next;
      ((CMPIInstance*)cur)->ft->release((CMPIInstance*)cur);
   }

   pthread_key_t k=getContextKey();
   pthread_setspecific(k,prev);
}


PEGASUS_NAMESPACE_END
