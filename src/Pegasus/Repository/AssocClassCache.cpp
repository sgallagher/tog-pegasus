//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include "AssocClassCache.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

AssocClassCacheManager::AssocClassCacheManager()
{
}

AssocClassCacheManager::~AssocClassCacheManager()
{
    for (Uint32 i = _assocClassCacheList.size(); i > 0; i--)
    {
        delete _assocClassCacheList[i-1];
        _assocClassCacheList.remove(i-1);
    }
}

/**
    Retrieves a singleton instance of the class cache for the given namespace.
*/
AssocClassCache* AssocClassCacheManager::getAssocClassCache(
    const String& nameSpace)
{
    for (Uint32 i = 0; i < _assocClassCacheList.size(); i++)
    {
        if (nameSpace == _assocClassCacheList[i]->getNameSpace())
        {
            return _assocClassCacheList[i];
        }
    }

    // If we got here, no cache exists for the given namespace so far,
    // so we will create a new one.
    AssocClassCache* newCache = new AssocClassCache(nameSpace);
    _assocClassCacheList.append(newCache);

    return newCache;
}


/** Retrieve the list of entries for a from class through direct
    access via the from class name.
*/
Boolean AssocClassCache::getAssocClassEntry(
    const CIMName& fromClassName,
    Array<ClassAssociation>& entryList)
{
    return _assocTable.lookup(fromClassName.getString(), entryList);
}

/** Add a new record to the association cache.
    If an entry for the given from class name already exists,
    the new entry is appended to the old entry. Otherwise a new entry
    is added to the cache.
*/
Boolean AssocClassCache::addRecord(
    const CIMName& fromClassName,
    const ClassAssociation& assocClassRecord)
{
    Array<ClassAssociation> oldAssocClassEntryList;

    if (_assocTable.lookup(fromClassName.getString(), oldAssocClassEntryList))
    {
        _assocTable.remove(fromClassName.getString());
    }

    oldAssocClassEntryList.append(assocClassRecord);

    return _assocTable.insert(
        fromClassName.getString(), oldAssocClassEntryList);
}

/** Remove an entry from the association cache specified by the given
     from class name.
*/
Boolean AssocClassCache::removeEntry(const CIMName& fromClassName)
{
    return _assocTable.remove(fromClassName.getString());
}

/** Remove an association record from the association cache specified by the
    given from class name and association name.
*/
Boolean AssocClassCache::removeRecord(
    const CIMName& fromClassName,
    const CIMName& assocClassName)
{
    Array<ClassAssociation> oldAssocClassEntryList;

    if (_assocTable.lookup(fromClassName.getString(), oldAssocClassEntryList))
    {
        for (Uint32 idx=0; idx < oldAssocClassEntryList.size(); idx++)
        {
            // The first entry in each record is the association class
            // name. Find the record for the association class and remove
            // it from the cache entry.
            if (oldAssocClassEntryList[idx].assocClassName == assocClassName)
            {
                _assocTable.remove(fromClassName.getString());
                if (oldAssocClassEntryList.size() > 1)
                {
                    oldAssocClassEntryList.remove(idx);
                    _assocTable.insert(
                        fromClassName.getString(), oldAssocClassEntryList);
                }
                return true;
            }
        }
    }

    return false;
}

/** Check if the cache is loaded with objects already.
*/
Boolean AssocClassCache::isActive()
{
    return _isInitialized;
}

void AssocClassCache::setActive(Boolean flag)
{
    _isInitialized = flag;
}


AssocClassCache::~AssocClassCache()
{
}

AssocClassCache::AssocClassCache(const String& nameSpace)
    : _nameSpace(nameSpace),
      _isInitialized(false),
      _assocTable(1000)
{
}

PEGASUS_NAMESPACE_END
