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

#ifndef Pegasus_SQLiteStore_h
#define Pegasus_SQLiteStore_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CommonUTF.h>
#include <Pegasus/Repository/PersistentStore.h>
#include <Pegasus/Repository/Linkage.h>

#include <sqlite3.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_REPOSITORY_LINKAGE SQLiteStore : public PersistentStore
{
public:

    static Boolean isExistingRepository(const String& repositoryRoot);

    SQLiteStore(
        const String& repositoryRoot,
        ObjectStreamer* streamer);

    ~SQLiteStore();

    Boolean storeCompleteClassDefinitions()
    {
        return false;
    }

    Array<NamespaceDefinition> enumerateNameSpaces();
    void createNameSpace(
        const CIMNamespaceName& nameSpace,
        Boolean shareable,
        Boolean updatesAllowed,
        const String& parentNameSpace);
    void modifyNameSpace(
        const CIMNamespaceName& nameSpace,
        Boolean shareable,
        Boolean updatesAllowed);
    void deleteNameSpace(const CIMNamespaceName& nameSpace);
    Boolean isNameSpaceEmpty(const CIMNamespaceName& nameSpace);

    Array<CIMQualifierDecl> enumerateQualifiers(
        const CIMNamespaceName& nameSpace);
    /**
        Gets a qualifier declaration for a specified qualifier name in a
        specified namespace.  Returns an uninitialized object if the qualifier
        is not found.
    */
    CIMQualifierDecl getQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);
    void setQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMQualifierDecl& qualifierDecl);
    void deleteQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    Array<Pair<String, String> > enumerateClassNames(
        const CIMNamespaceName& nameSpace);
    CIMClass getClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMName& superClassName);
    void createClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& newClass,
        const Array<ClassAssociation>& classAssocEntries);
    void modifyClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& modifiedClass,
        const CIMName& oldSuperClassName,
        Boolean isAssociation,
        const Array<ClassAssociation>& classAssocEntries);
    void deleteClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMName& superClassName,
        Boolean isAssociation,
        const Array<CIMNamespaceName>& dependentNameSpaceNames);
    Array<CIMObjectPath> enumerateInstanceNamesForClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);
    Array<CIMInstance> enumerateInstancesForClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);
    CIMInstance getInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName);
    void createInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMInstance& cimInstance,
        const Array<InstanceAssociation>& instAssocEntries);
    void modifyInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMInstance& cimInstance);
    void deleteInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName);
    Boolean instanceExists(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName);

    void getClassAssociatorNames(
        const CIMNamespaceName& nameSpace,
        const Array<CIMName>& classList,
        const Array<CIMName>& assocClassList,
        const Array<CIMName>& resultClassList,
        const String& role,
        const String& resultRole,
        Array<String>& associatorNames);
    void getClassReferenceNames(
        const CIMNamespaceName& nameSpace,
        const Array<CIMName>& classList,
        const Array<CIMName>& resultClassList,
        const String& role,
        Array<String>& referenceNames);

    void getInstanceAssociatorNames(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const Array<CIMName>& assocClassList,
        const Array<CIMName>& resultClassList,
        const String& role,
        const String& resultRole,
        Array<String>& associatorNames);
    void getInstanceReferenceNames(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const Array<CIMName>& resultClassList,
        const String& role,
        Array<String>& referenceNames);

private:

    sqlite3* _openDb(const char* fileName);
    sqlite3* _openDb(const CIMNamespaceName& nameSpace);

    void _execDbStatement(
        sqlite3* db,
        const char* sqlStatement);

    void _initSchema(sqlite3* db);

    void _beginTransaction(sqlite3* db);
    void _commitTransaction(sqlite3* db);

    String _getNormalizedName(const CIMName& className)
    {
        String cn = className.getString();
        cn.toLower();
        return cn;
    }

    String _getDbPath(const CIMNamespaceName& nameSpace)
    {
        String dbFileName = nameSpace.getString();
        dbFileName.toLower();

        for (Uint32 i = 0; i < dbFileName.size(); i++)
        {
            if (dbFileName[i] == '/')
            {
                dbFileName[i] = '#';
            }
        }

        return _repositoryRoot + "/" + escapeStringEncoder(dbFileName) + ".db";
    }

    void _addClassAssociationEntries(
        sqlite3* db,
        const CIMNamespaceName& nameSpace,
        const Array<ClassAssociation>& classAssocEntries);
    void _removeClassAssociationEntries(
        sqlite3* db,
        const CIMNamespaceName& nameSpace,
        const CIMName& assocClassName);

    void _addInstanceAssociationEntries(
        sqlite3* db,
        const CIMNamespaceName& nameSpace,
        const Array<InstanceAssociation>& instanceAssocEntries);
    void _removeInstanceAssociationEntries(
        sqlite3* db,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& assocInstanceName);

    String _repositoryRoot;
    ObjectStreamer* _streamer;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SQLiteStore_h */
