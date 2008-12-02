//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_Object.h"

#if !defined(PEGASUS_OS_TYPE_WINDOWS)
# include <pthread.h>
#endif
#include <limits.h>


PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

TSDKeyType CMPI_ThreadContext::contextKey;
int CMPI_ThreadContext::context_key_once=1;

void CMPI_ThreadContext::context_key_alloc()
{
    TSDKey::create(&contextKey);
}

TSDKeyType CMPI_ThreadContext::getContextKey()
{
    if( context_key_once )
    {
        context_key_alloc();
        context_key_once=0;
    }
    return contextKey;
}

void CMPI_ThreadContext::add(CMPI_Object *o)
{
    ENQ_TOP_LIST(o,CIMfirst,CIMlast,next,prev);
}

void CMPI_ThreadContext::addObject(CMPI_Object* o) 
{
    CMPI_ThreadContext* ctx=getThreadContext();
    if (ctx)
    {
        ctx->add(o);
    }
}

void CMPI_ThreadContext::remove(CMPI_Object *o)
{
    if( o->next!=reinterpret_cast<CMPI_Object*>((void*)-1l))
    {
        DEQ_FROM_LIST(o,CIMfirst,CIMlast,next,prev);
        o->next=reinterpret_cast<CMPI_Object*>((void*)-1l);
    }
}

void CMPI_ThreadContext::remObject(CMPI_Object* o)
{
    CMPI_ThreadContext* ctx=getThreadContext();
    if (ctx)
    {
        ctx->remove(o);
    }
}

CMPI_ThreadContext* CMPI_ThreadContext::getThreadContext()
{
    TSDKeyType k=getContextKey();
    return(CMPI_ThreadContext*)TSDKey::get_thread_specific(k);
}

const CMPIBroker* CMPI_ThreadContext::getBroker() 
{
    /**
      return getThreadContext()->broker;
   */
    CMPI_ThreadContext *ctx = getThreadContext();
    if( ctx )
    {
        return ctx->broker;
    }
    return 0;
}

const CMPIContext* CMPI_ThreadContext::getContext() 
{
    return getThreadContext()->context;
}

CMPI_ThreadContext::CMPI_ThreadContext(
const CMPIBroker *mb,
const CMPIContext *ctx )
{
    CIMfirst=CIMlast=NULL;
    broker=mb;
    context=ctx;
    TSDKeyType k=getContextKey();
    prev=(CMPI_ThreadContext*)TSDKey::get_thread_specific(k);
    TSDKey::set_thread_specific(k,this);
    return;
}

CMPI_ThreadContext::~CMPI_ThreadContext()
{
    for( CMPI_Object *nxt,*cur=CIMfirst; cur; cur=nxt )
    {
        nxt=cur->next;
        (reinterpret_cast<CMPIInstance*>(cur))->ft->release(
        reinterpret_cast<CMPIInstance*>(cur));
    }

    TSDKeyType k=getContextKey();
    TSDKey::set_thread_specific(k,prev);
}


PEGASUS_NAMESPACE_END

