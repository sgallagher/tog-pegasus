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

#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/SCMOClassCache.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CIMNameCast.h>

PEGASUS_NAMESPACE_BEGIN

SCMOClassCache* SCMOClassCache::_theInstance = NULL;

SCMOClassCache::~SCMOClassCache()
{
    // Cleanup the class cache
    SCMOClassHashTable::Iterator i2=_clsCacheSCMO->start();
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

void SCMOClassCache::destroy()
{
    delete _theInstance;
    _theInstance == NULL;
}

SCMOClassCache* SCMOClassCache::getInstance()
{
    if(_theInstance == NULL)
    {
        _theInstance = new SCMOClassCache();
    }
    return _theInstance;
}


void SCMOClassCache::_setHint(ClassCacheEntry& hint, SCMOClass* hintClass)
{
    if (_hint)
    {
        delete(_hint);
    }
    _hint = new ClassCacheEntry(hint);
    _hintClass = hintClass;
}


SCMOClass* SCMOClassCache::getSCMOClass(
        const char* nsName,
        Uint32 nsNameLen,
        const char* className,
        Uint32 classNameLen)
{
    //fprintf(stderr,"SCMOClassCache::getSCMOClass - Enter()\n");

    if (nsName && className)
    {

        ClassCacheEntry key(nsName,nsNameLen,className,classNameLen);

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

            PEGASUS_ASSERT(_resolveCallBack);

            CIMClass cc = _resolveCallBack(
                CIMNamespaceNameCast(String(nsName,nsNameLen)),
                CIMNameCast(String(className,classNameLen)));

            if (cc.isUninitialized())
            {
                scmoClass = NULL;
            }
            else
            {
                scmoClass = new SCMOClass(cc,nsName);
                _clsCacheSCMO->insert(key,scmoClass);
                _setHint(key, scmoClass);
            }
            
            return scmoClass;
        }
        catch (const CIMException &e)
        {
            PEG_TRACE((TRC_SERVER,Tracer::LEVEL1,
                "CIMException: %s",(const char*)e.getMessage().getCString()));
        }
    }

    return 0;
}

PEGASUS_NAMESPACE_END
