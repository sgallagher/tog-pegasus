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
#ifndef _SCMOClassCache_H_
#define _SCMOClassCache_H_

#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/ReadWriteSem.h>
#include <Pegasus/Common/CharSet.h>
#include <Pegasus/Common/SCMOClass.h>
#include <Pegasus/Common/System.h>

PEGASUS_NAMESPACE_BEGIN

typedef CIMClass (*SCMOClassCacheCallbackPtr)(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);


class PEGASUS_COMMON_LINKAGE ClassCacheEntry
{
    char* nsName;
    Uint32 nsLen;
    char* clsName;
    Uint32 clsLen;
    Boolean allocated;

public:
    ClassCacheEntry(const char* namespaceName,
                    Uint32 namespaceNameLen,
                    const char* className,
                    Uint32 classNameLen ):
        nsLen(namespaceNameLen),        
        clsLen(classNameLen)
    {
        nsName = (char*) malloc(nsLen+1);
        clsName = (char*) malloc(clsLen+1);
        if (0 == clsName || 0 == nsName)
        {
            free(nsName);
            free(clsName);
            throw PEGASUS_STD(bad_alloc)();
        }
        memcpy(nsName, namespaceName, namespaceNameLen);
        memcpy(clsName, className, classNameLen);        
    };

    ClassCacheEntry( const ClassCacheEntry& x)
    {
        // free up what we currently have, whatever that might be
        free(nsName);
        free(clsName);

        nsName = (char*) malloc(x.nsLen+1);
        clsName = (char*) malloc(x.clsLen+1);
        if (0 == clsName || 0 == nsName)
        {
            free(nsName);
            free(clsName);
            nsName=0;
            clsName=0;
            throw PEGASUS_STD(bad_alloc)();
        }

        nsLen = x.nsLen;
        clsLen = x.clsLen;

        memcpy(nsName, x.nsName, nsLen+1);
        memcpy(clsName, x.clsName, clsLen+1);
    };

    ~ClassCacheEntry()
    {
        free(clsName);
        free(nsName);
    }

    static Boolean equal(const ClassCacheEntry& x, const ClassCacheEntry& y)
    {
        return System::strncasecmp(x.clsName,x.clsLen,y.clsName,y.clsLen);
    }

    static Uint32 hash(const ClassCacheEntry& entry)
    {
        // Simply use the lenght of the classname as hash.
        return entry.clsLen;
    }
};



class PEGASUS_COMMON_LINKAGE SCMOClassCache
{

public:

    SCMOClass* getSCMOClass(
        const char* nsName,
        Uint32 nsNameLen,
        const char* className,
        Uint32 classNameLen);

    static SCMOClassCache* getInstance();

    void setCallBack(SCMOClassCacheCallbackPtr clb)
    {
       _resolveCallBack = clb;
    }

    static void destroy();

private:

    typedef HashTable<ClassCacheEntry, SCMOClass *,
        ClassCacheEntry, ClassCacheEntry> SCMOClassHashTable;

    static SCMOClassCache* _theInstance;

    SCMOClassCache()
        : _hintClass(NULL),
          _hint(NULL),
          _resolveCallBack(NULL)
    {
        _clsCacheSCMO = new SCMOClassHashTable();
    };

    // clean-up cache data
    ~SCMOClassCache();

    SCMOClassHashTable * _clsCacheSCMO;

    // auto-initialisation due to being on the stack
    ReadWriteSem _rwsemClassCache;

    SCMOClass* _hintClass;
    ClassCacheEntry* _hint;

    // the call back function pointer to get CIMClass's
    SCMOClassCacheCallbackPtr _resolveCallBack;

    //Optimization for continuos lookups of the same class
    //Simply store away the last lookup result
    void _setHint(ClassCacheEntry& hint, SCMOClass* hintClass);

};

PEGASUS_NAMESPACE_END

#endif
