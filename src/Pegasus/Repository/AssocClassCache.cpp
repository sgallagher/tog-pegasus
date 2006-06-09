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
// Author: Robert Kieninger (kieningr@de.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include "AssocClassCache.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define ASSOC_CLASS_NAME_INDEX 0
#define FROM_CLASS_NAME_INDEX 1
#define FROM_PROPERTY_NAME_INDEX 2
#define TO_CLASS_NAME_INDEX 3
#define TO_PROPERTY_NAME_INDEX 4
#define NUM_FIELDS 5

Array<AssocClassCache*> AssocClassCache::_assocClassCacheList;


/** Retrieves a singleton instance of the class cache for the
  * given namespace.
*/
AssocClassCache* AssocClassCache::getAssocClassCache(const String& nameSpace)
{
   for (Uint16 idx=0; idx<_assocClassCacheList.size(); idx++)
   {
      if (nameSpace == _assocClassCacheList[idx]->_nameSpace)
      {
         return _assocClassCacheList[idx];
      }
   }

   // If we got here, no cache exists for the the given namespace so far,
   // so we will create a new one.
   AssocClassCache * newCache = new AssocClassCache(nameSpace);
   _assocClassCacheList.append(newCache);

   return newCache;
}

void AssocClassCache::cleanupAssocClassCaches()
{
    for (Uint16 idx=_assocClassCacheList.size(); idx>0; idx--)
    {
        delete(_assocClassCacheList[idx-1]);
        _assocClassCacheList.remove(idx-1);
    }

}

/** Retrieve the list of entries for a from class through direct
 * access via the from class name.
*/
Boolean AssocClassCache::getAssocClassEntry(const String& fromClassName,
                                            Array< Array<String> >& entryList)
{
    String lowerCaseFromClassName = fromClassName;
    lowerCaseFromClassName.toLower();

   return _assocClassCache->lookup(lowerCaseFromClassName,entryList);
}

/** Add a new record to the association cache.
 * If an entry for the given from class name already exists,
 * the new entry is appended to the old entry. Otherwise a new entry
 * is added to the cache.
*/
Boolean AssocClassCache::addRecord(const String& fromClassName,
                                   Array<String> assocClassRecord)
{
   Array< Array<String> > oldAssocClassEntryList;
   String lowerCaseFromClassName = fromClassName;
   lowerCaseFromClassName.toLower();

   if (_assocClassCache->lookup(lowerCaseFromClassName, oldAssocClassEntryList))
   {
      _assocClassCache->remove(lowerCaseFromClassName);
   }

   oldAssocClassEntryList.append(assocClassRecord);

   return _assocClassCache->insert(lowerCaseFromClassName,oldAssocClassEntryList);
}

/** Remove an entry from the association cache specified by the given
 *  from class name.
*/
Boolean AssocClassCache::removeEntry(const String& fromClassName)
{
    String lowerCaseFromClassName = fromClassName;
    lowerCaseFromClassName.toLower();

   return _assocClassCache->remove(lowerCaseFromClassName);
}

/** Remove an association record from the association cache specified by the given
 *  from class name and association name.
*/
Boolean AssocClassCache::removeRecord(const String& fromClassName,
                                      const String& assocClassName)
{
   Array< Array<String> > oldAssocClassEntryList;
   String lowerCaseFromClassName = fromClassName;
   lowerCaseFromClassName.toLower();

   if (_assocClassCache->lookup(lowerCaseFromClassName, oldAssocClassEntryList))
   {
      for (Uint16 idx=0; idx < oldAssocClassEntryList.size(); idx++ )
      {
         // The first entry in each record is the association class
         // name. Find the record for the association class and remove
         // it from the cache entry.
         if (String::equalNoCase(oldAssocClassEntryList[idx][ASSOC_CLASS_NAME_INDEX],
                                 assocClassName))
         {
            _assocClassCache->remove(lowerCaseFromClassName);
            if (oldAssocClassEntryList.size() > 1)
            {
               oldAssocClassEntryList.remove(idx);
               _assocClassCache->insert(lowerCaseFromClassName,oldAssocClassEntryList);
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
    delete(_assocClassCache);
}

AssocClassCache::AssocClassCache(const String& nameSpace)
{
    _isInitialized = false;
   _nameSpace = nameSpace;
   _assocClassCache = new AssocClassCacheHashTableType(1000);
}

PEGASUS_NAMESPACE_END
