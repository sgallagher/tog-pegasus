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

#include <Pegasus/ProviderManager2/CMPI/CMPIClassCache.h>
#include <Pegasus/ProviderManager2/CMPI/CMPI_Broker.h>
#include <Pegasus/ProviderManager2/CMPI/CMPI_ContextArgs.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CIMNameCast.h>

PEGASUS_NAMESPACE_BEGIN

CMPIClassCache::~CMPIClassCache()
{
    // Cleanup the class cache
    ClassCache::Iterator i=_clsCache->start();
    for (; i; i++)
    {
        delete i.value();
    }
    delete _clsCache;

    ClassCacheSCMO::Iterator i2=_clsCacheSCMO->start();
    for (; i2; i2++)
    {
        delete i2.value();
    }
    delete _clsCacheSCMO;

    if (_hint)
    {
        delete( _hint );
    }
}



void CMPIClassCache::_setHint(ClassCacheEntry& hint, SCMOClass* hintClass)
{
    if (_hint)
    {
        delete(_hint);
    }
    _hint = new ClassCacheEntry(hint);
    _hintClass = hintClass;
}


SCMOClass* CMPIClassCache::getSCMOClass(
    const CMPI_Broker *mb,
    const char* nsName,
    const char* className)
{
    //fprintf(stderr,"CMPIClassCache::getSCMOClass - Enter()\n");

    if (nsName && className)
    {

        ClassCacheEntry key(nsName, className);

        SCMOClass *scmoClass;

        {
            ReadLock readLock(_rwsemClassCache);

            // We first check if the last lookup was for the same class
            // so we could directly use the saved hint.
            if (_hint && ClassCacheEntry::equal(*_hint, key))
            {
                return _hintClass;
            }

            if (_clsCacheSCMO->lookup(key,scmoClass))
            {
                _setHint(key, scmoClass);
                return scmoClass;
            }
        }

        try
        {
            WriteLock writeLock(_rwsemClassCache);

            if (_clsCacheSCMO->lookup(key,scmoClass))
            {
                _setHint(key, scmoClass);
                return scmoClass;
            }

            const CIMClass cc = ((CIMOMHandle*)mb->hdl)->getClass(
                OperationContext(),
                CIMNamespaceNameCast(nsName),
                CIMNameCast(className),
                (bool)0,
                (bool)1,
                (bool)1,
                CIMPropertyList());

            scmoClass = new SCMOClass(cc,nsName);
            _clsCacheSCMO->insert(key,scmoClass);

            _setHint(key, scmoClass);
            return scmoClass;
        }
        catch (const CIMException &e)
        {
            PEG_TRACE((TRC_CMPIPROVIDERINTERFACE,Tracer::LEVEL1,
                "CIMException: %s",(const char*)e.getMessage().getCString()));
        }
    }

    return 0;
}




CIMClass* CMPIClassCache::getClass(
    const CMPI_Broker *mb,
    const CIMObjectPath & cop)
{
    String clsId =
        cop.getNameSpace().getString()+":"+cop.getClassName().getString();

    CIMClass *ccp;

    {
        ReadLock readLock(_rwsemClassCache);

        if (_clsCache->lookup(clsId,ccp))
        {
            return ccp;
        }
    }

    try
    {
        WriteLock writeLock(_rwsemClassCache);

        if (_clsCache->lookup(clsId,ccp))
        {
            return ccp;
        }

        const CMPIContext *ctx = CMPI_ThreadContext::getContext();

        CIMClass cc = ((CIMOMHandle*)mb->hdl)->getClass(
            *CM_Context(ctx),
            cop.getNameSpace(),
            cop.getClassName(),
            (bool)0,
            (bool)1,
            (bool)0,
            CIMPropertyList());

        ccp = new CIMClass(cc);
        _clsCache->insert(clsId,ccp);
        return ccp;
    }
    catch (const CIMException &e)
    {
        PEG_TRACE((TRC_CMPIPROVIDERINTERFACE,Tracer::LEVEL1,
            "CIMException: %s",(const char*)e.getMessage().getCString()));
    }
    return 0;
}

PEGASUS_NAMESPACE_END
