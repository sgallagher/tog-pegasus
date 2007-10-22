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

#ifndef Pegasus_Repository_MemoryResidentRepository
#define Pegasus_Repository_MemoryResidentRepository

#include <Pegasus/Common/Config.h>
#include <Pegasus/Repository/Repository.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/Buffer.h>

PEGASUS_NAMESPACE_BEGIN

typedef Pair<CIMNamespaceName, CIMInstance> NamespaceInstancePair;

#define PEGASUS_ARRAY_T NamespaceInstancePair
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

class RepositoryDeclContext;
class compilerDeclContext;


/** Virtual base class for CIMRepository implementations.
*/
class PEGASUS_REPOSITORY_LINKAGE MemoryResidentRepository : public Repository
{
public:

    MemoryResidentRepository(
        const String& repositoryRoot, 
        Uint32 repositoryMode);

    virtual ~MemoryResidentRepository();

    virtual CIMClass getClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    virtual CIMInstance getInstance(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    virtual void deleteClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    virtual void deleteInstance(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName);

    virtual void createClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMClass& newClass,
        const ContentLanguageList& contentLangs);

    virtual CIMObjectPath createInstance(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMInstance& newInstance,
        const ContentLanguageList& contentLangs);

    virtual void modifyClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMClass& modifiedClass,
        const ContentLanguageList& contentLangs);

    virtual void modifyInstance(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMInstance& modifiedInstance,
        Boolean includeQualifiers,
        const CIMPropertyList& propertyList,
        const ContentLanguageList& contentLangs);

    virtual Array<CIMClass> enumerateClasses(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin);

    virtual Array<CIMName> enumerateClassNames(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance);

    virtual Array<CIMInstance> enumerateInstancesForSubtree(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    virtual Array<CIMInstance> enumerateInstancesForClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    virtual Array<CIMObjectPath> enumerateInstanceNamesForSubtree(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    virtual Array<CIMObjectPath> enumerateInstanceNamesForClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    virtual Array<CIMInstance> execQuery(
        bool lock,
        const String& queryLanguage,
        const String& query);

    virtual Array<CIMObject> associators(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const CIMName& resultClass,
        const String& role,
        const String& resultRole,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    virtual Array<CIMObjectPath> associatorNames(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const CIMName& resultClass,
        const String& role,
        const String& resultRole);

    virtual Array<CIMObject> references(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass,
        const String& role,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    virtual Array<CIMObjectPath> referenceNames(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass,
        const String& role);

    virtual CIMValue getProperty(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName);

    virtual void setProperty(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName,
        const CIMValue& newValue,
        const ContentLanguageList& contentLangs);

    virtual CIMQualifierDecl getQualifier(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    virtual void setQualifier(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMQualifierDecl& qualifierDecl,
        const ContentLanguageList& contentLangs);

    virtual void deleteQualifier(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    virtual Array<CIMQualifierDecl> enumerateQualifiers(
        bool lock,
        const CIMNamespaceName& nameSpace);

    virtual void createNameSpace(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes& attributes);

    virtual void modifyNameSpace(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes& attributes);

    virtual Array<CIMNamespaceName> enumerateNameSpaces(
        bool lock) const;

    virtual void deleteNameSpace(
        bool lock,
        const CIMNamespaceName& nameSpace);

    virtual Boolean getNameSpaceAttributes(
        bool lock,
        const CIMNamespaceName& nameSpace,
        NameSpaceAttributes& attributes);

    virtual void setDeclContext(
        bool lock,
        RepositoryDeclContext* context);

    virtual Boolean isDefaultInstanceProvider(
        bool lock);

    virtual void getSubClassNames(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance,
        Array<CIMName>& subClassNames) const;

    virtual void getSuperClassNames(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Array<CIMName>& superClassNames) const;

    virtual Boolean isRemoteNameSpace(
        bool lock,
        const CIMNamespaceName& nameSpace,
        String& remoteInfo);

#ifdef PEGASUS_DEBUG
    virtual void DisplayCacheStatistics(
        bool lock);
#endif

    // Sets the global save callback that is called whenever the memory-resident
    // instance repository is modified. The buffer argument is a serialized
    // copy of the memory-resident instance repository. The handler can do
    // things such as save the buffer on disk for later use.
    static void installSaveCallback(
        void (*callback)(const Buffer& buffer, void* data),
        void* data);

    // Sets the global load callback that is called whenever an instance of
    // MemoryResidentRepository is created in order to load the initial set
    // of instances (if any).
    static void installLoadCallback(
        void (*callback)(Buffer& buffer, void* data),
        void* data);

private:
    Uint32 _findInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName);

    void _processSaveHandler();

    void _processLoadHandler();

    Array<NamespaceInstancePair> _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Repository_MemoryResidentRepository */
