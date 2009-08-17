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
#ifndef _CMPIClassCache_H_
#define _CMPIClassCache_H_

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/ReadWriteSem.h>
#include <Pegasus/Common/CharSet.h>
#include <Pegasus/Common/SCMOClass.h>

PEGASUS_NAMESPACE_BEGIN

struct CMPI_Broker;



class ClassCacheEntry
{
    const char* nsName;
    Uint32 nsLen;
    const char* clsName;
    Uint32 clsLen;
    Boolean allocated;

public:
    ClassCacheEntry(const char* namespaceName, 
                    const char* className )
    {
        nsLen = strlen(namespaceName);
        clsLen = strlen(className);
        nsName = namespaceName;
        clsName = className;
        allocated = false;
    };

    ClassCacheEntry( const ClassCacheEntry& oldEntry)
    {
        nsLen = oldEntry.nsLen;
        nsName = new char[nsLen+1];
        memcpy((void*)nsName, oldEntry.nsName, nsLen+1);

        clsLen = oldEntry.clsLen;
        clsName = new char[clsLen+1];
        memcpy((void*)clsName, oldEntry.clsName, clsLen+1);

        allocated = true;
    };

    ~ClassCacheEntry()
    {
        if (allocated)
        {
            delete[](clsName);
            delete[](nsName);
        }
    }

    static Boolean equal (const ClassCacheEntry& x, const ClassCacheEntry& y)
    {
        return ((x.clsLen == y.clsLen) &&
                (x.nsLen == y.nsLen) &&
                (0 == strcasecmp(x.clsName, y.clsName)) &&
                (0 == strcasecmp(x.nsName, y.nsName)));
    }

    static Uint32 hash(const ClassCacheEntry& entry)
    {
        // Simply use the lenght of the classname as hash.
        return entry.clsLen;
    }
};



class CMPIClassCache
{

public:
    CMPIClassCache()
    {
        _clsCache = new ClassCache();
        _clsCacheSCMO = new ClassCacheSCMO();
        _hintClass = NULL;
        _hint = NULL;
    };

    // clean-up cache data
    ~CMPIClassCache();

    // a single function as point of control for now
    // target is to reduce the critical section as much as possible
    CIMClass* getClass(
        const CMPI_Broker *mb,
        const CIMObjectPath &cop);

    SCMOClass* getSCMOClass(
        const CMPI_Broker *mb,
        const char* nsName,
        const char* className);

private:

    typedef HashTable<String, CIMClass *,
        EqualFunc<String>,  HashFunc<String> > ClassCache;

    typedef HashTable<ClassCacheEntry, SCMOClass *, 
        ClassCacheEntry, ClassCacheEntry> ClassCacheSCMO;

    ClassCache * _clsCache;
    ClassCacheSCMO * _clsCacheSCMO;
    // auto-initialisation due to being on the stack
    ReadWriteSem _rwsemClassCache;

    //Optimization for continuos lookups of the same class
    //Simply store away the last lookup result
    void _setHint(ClassCacheEntry& hint, SCMOClass* hintClass);
    SCMOClass* _hintClass;
    ClassCacheEntry* _hint;

};

PEGASUS_NAMESPACE_END

#endif
