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

#ifndef Pegasus_Repository_h
#define Pegasus_Repository_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Repository/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class RepositoryDeclContext;
class compilerDeclContext;

/** Virtual base class for CIMRepository implementations.
*/
class Repository
{
public:

    typedef HashTable <String, String, EqualNoCaseFunc, HashLowerCaseFunc>
        NameSpaceAttributes;

    Repository(
        const String& repositoryRoot, 
        Uint32 repositoryMode);

    virtual ~Repository();

    virtual CIMClass getClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList) = 0;

    virtual CIMInstance getInstance(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList) = 0;

    virtual void deleteClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className) = 0;

    virtual void deleteInstance(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName) = 0;

    virtual void createClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMClass& newClass,
        const ContentLanguageList& contentLangs) = 0;

    virtual CIMObjectPath createInstance(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMInstance& newInstance,
        const ContentLanguageList& contentLangs) = 0;

    virtual void modifyClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMClass& modifiedClass,
        const ContentLanguageList& contentLangs) = 0;

    virtual void modifyInstance(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMInstance& modifiedInstance,
        Boolean includeQualifiers,
        const CIMPropertyList& propertyList,
        const ContentLanguageList& contentLangs) = 0;

    virtual Array<CIMClass> enumerateClasses(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin) = 0;

    virtual Array<CIMName> enumerateClassNames(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance) = 0;

    virtual Array<CIMInstance> enumerateInstancesForSubtree(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList) = 0;

    virtual Array<CIMInstance> enumerateInstancesForClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList) = 0;

    virtual Array<CIMObjectPath> enumerateInstanceNamesForSubtree(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className) = 0;

    virtual Array<CIMObjectPath> enumerateInstanceNamesForClass(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className) = 0;

    virtual Array<CIMInstance> execQuery(
        bool lock,
        const String& queryLanguage,
        const String& query) = 0;

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
        const CIMPropertyList& propertyList) = 0;

    virtual Array<CIMObjectPath> associatorNames(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const CIMName& resultClass,
        const String& role,
        const String& resultRole) = 0;

    virtual Array<CIMObject> references(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass,
        const String& role,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList) = 0;

    virtual Array<CIMObjectPath> referenceNames(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass,
        const String& role) = 0;

    virtual CIMValue getProperty(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName) = 0;

    virtual void setProperty(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName,
        const CIMValue& newValue,
        const ContentLanguageList& contentLangs) = 0;

    virtual CIMQualifierDecl getQualifier(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName) = 0;

    virtual void setQualifier(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMQualifierDecl& qualifierDecl,
        const ContentLanguageList& contentLangs) = 0;

    virtual void deleteQualifier(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName) = 0;

    virtual Array<CIMQualifierDecl> enumerateQualifiers(
        bool lock,
        const CIMNamespaceName& nameSpace) = 0;

    virtual void createNameSpace(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes& attributes) = 0;

    virtual void modifyNameSpace(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes& attributes) = 0;

    virtual Array<CIMNamespaceName> enumerateNameSpaces(
        bool lock) const = 0;

    virtual void deleteNameSpace(
        bool lock,
        const CIMNamespaceName& nameSpace) = 0;

    virtual Boolean getNameSpaceAttributes(
        bool lock,
        const CIMNamespaceName& nameSpace,
        NameSpaceAttributes& attributes) = 0;

    virtual void setDeclContext(
        bool lock,
        RepositoryDeclContext* context) = 0;

    virtual Boolean isDefaultInstanceProvider(
        bool lock) = 0;

    virtual void getSubClassNames(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance,
        Array<CIMName>& subClassNames) const = 0;

    virtual void getSuperClassNames(
        bool lock,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Array<CIMName>& superClassNames) const = 0;

    virtual Boolean isRemoteNameSpace(
        bool lock,
        const CIMNamespaceName& nameSpace,
        String& remoteInfo) = 0;

#ifdef PEGASUS_DEBUG
    virtual void DisplayCacheStatistics(
        bool lock) = 0;
#endif

protected:

    String _repositoryRoot;
    Uint32 _repositoryMode;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Repository_h */
