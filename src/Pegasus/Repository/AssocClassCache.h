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

#ifndef Pegasus_AssocClassCache_h
#define Pegasus_AssocClassCache_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Repository/Linkage.h>
#include <Pegasus/Common/HashTable.h>

PEGASUS_NAMESPACE_BEGIN

/** Maintains a cache for all association classes.
*/
class PEGASUS_REPOSITORY_LINKAGE AssocClassCache
{
public:

    /** Retrieves a singleton instance of the class cache for the
        given namespace.
    */
    static AssocClassCache* getAssocClassCache(const String& nameSpace);
    static void cleanupAssocClassCaches();

    /** Retrieve an entry for an association class through direct
        access via the from class name.
    */
    Boolean getAssocClassEntry(
        const String& fromClassName,
        Array< Array<String> >& entryList);

    /** Add a new entry to the association cache.
    */
    Boolean addRecord(
        const String& fromClassName,
        Array<String> assocClassEntry);

    /** Remove an entry from the association cache specified by the given
        association class name.
    */
    Boolean removeEntry(const String& fromClassName);

   /** Remove an association record from the association cache specified by
       the given from class name and association name.
    */
    Boolean removeRecord(
        const String& fromClassName,
        const String& assocClassName);

    /** Check if the cache is loaded with objects already.
    */
    Boolean isActive();
    void setActive(Boolean flag);

private:
    static Array<AssocClassCache*> _assocClassCacheList;
    String _nameSpace;
    Boolean _isInitialized;

    typedef HashTable<String, Array< Array<String> >,
        EqualNoCaseFunc, HashLowerCaseFunc > AssocClassCacheHashTableType;

    AssocClassCacheHashTableType *_assocClassCache;

    AssocClassCache() {/*private*/};
    ~AssocClassCache();
    AssocClassCache(const String& nameSpace);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_AssocClassCache_h */
