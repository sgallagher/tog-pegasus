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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_Object.h"

#ifndef PEGASUS_PLATFORM_WIN32_IX86_MSVC
#include <pthread.h>
#endif
#include <limits.h>


PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

PEGASUS_THREAD_KEY_TYPE CMPI_ThreadContext::contextKey;
int CMPI_ThreadContext::context_key_once=1;

void CMPI_ThreadContext::context_key_alloc()
{
	pegasus_key_create(&contextKey);
}

PEGASUS_THREAD_KEY_TYPE CMPI_ThreadContext::getContextKey()
{
	if (context_key_once) {
		 context_key_alloc();
		 context_key_once=0;
	}
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
   if (reinterpret_cast<long>(o->next)!=-1) {
      DEQ_FROM_LIST(o,CIMfirst,CIMlast,next,prev);
      o->next=reinterpret_cast<CMPI_Object*>((void*)-1l);
   }
}

void CMPI_ThreadContext::remObject(CMPI_Object* o) {
   CMPI_ThreadContext* ctx=getThreadContext();
   ctx->remove(o);
}

CMPI_ThreadContext* CMPI_ThreadContext::getThreadContext() {
   PEGASUS_THREAD_KEY_TYPE k=getContextKey();
   #ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
    return (CMPI_ThreadContext*)pegasus_get_thread_specific(k);
   #else
    CMPI_ThreadContext* tCtx=NULL;
    pthread_getspecific(k,(void**)&tCtx);
    return tCtx;
   #endif
}

const CMPIBroker* CMPI_ThreadContext::getBroker() {
   //return getThreadContext()->broker;
    CMPI_ThreadContext *ctx = getThreadContext();
    if (ctx)
	return ctx->broker;
    return 0;
}

const CMPIContext* CMPI_ThreadContext::getContext() {
   return getThreadContext()->context;
}

CMPI_ThreadContext::CMPI_ThreadContext(const CMPIBroker *mb, const CMPIContext *ctx ) {
   CIMfirst=CIMlast=NULL;
   broker=mb;
   context=ctx;
   PEGASUS_THREAD_KEY_TYPE k=getContextKey();
   #ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
     prev=(CMPI_ThreadContext*)pegasus_get_thread_specific(k);
   #else
    pthread_getspecific(k,(void**)&prev);
   #endif
   pegasus_set_thread_specific(k,this);
   return;
}

CMPI_ThreadContext::~CMPI_ThreadContext() {
   for (CMPI_Object *nxt,*cur=CIMfirst; cur; cur=nxt) {
      nxt=cur->next;
      ((CMPIInstance*)cur)->ft->release((CMPIInstance*)cur);
   }

   PEGASUS_THREAD_KEY_TYPE k=getContextKey();
   pegasus_set_thread_specific(k,prev);
}


PEGASUS_NAMESPACE_END
