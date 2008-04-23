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

#include "CIMRepository.h"
#include "DefaultRepository.h"
#include "MemoryResidentRepository.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMRepository::CIMRepository(
    const String& repositoryRoot,
    Uint32 mode)
{
#ifdef PEGASUS_USE_MEMORY_RESIDENT_REPOSITORY
    _rep = new MemoryResidentRepository();
#else
    _rep = new DefaultRepository(repositoryRoot, mode);
#endif
}

CIMRepository::~CIMRepository()
{
    delete _rep;
}

CIMClass CIMRepository::getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    return _rep->getClass(
        true, 
        nameSpace, 
        className, 
        localOnly, 
        includeQualifiers, 
        includeClassOrigin, 
        propertyList);
}

CIMInstance CIMRepository::getInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    return _rep->getInstance(
        true,
        nameSpace,
        instanceName,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

void CIMRepository::deleteClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    _rep->deleteClass(
        true,
        nameSpace,
        className);
}

void CIMRepository::deleteInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    _rep->deleteInstance(
        true,
        nameSpace,
        instanceName);
}

void CIMRepository::createClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass,
    const ContentLanguageList& contentLangs)
{
    _rep->createClass(
        true,
        nameSpace,
        newClass,
        contentLangs);
}

CIMObjectPath CIMRepository::createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance,
    const ContentLanguageList& contentLangs)
{
    return _rep->createInstance(
        true,
        nameSpace,
        newInstance,
        contentLangs);
}

void CIMRepository::modifyClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& modifiedClass,
    const ContentLanguageList& contentLangs)
{
    _rep->modifyClass(
        true,
        nameSpace,
        modifiedClass,
        contentLangs);
}

void CIMRepository::modifyInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    const ContentLanguageList& contentLangs)
{
    _rep->modifyInstance(
        true,
        nameSpace,
        modifiedInstance,
        includeQualifiers,
        propertyList,
        contentLangs);
}

Array<CIMClass> CIMRepository::enumerateClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    return _rep->enumerateClasses(
        true,
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin);
}

Array<CIMName> CIMRepository::enumerateClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    return _rep->enumerateClassNames(
        true,
        nameSpace,
        className,
        deepInheritance);
}

Array<CIMInstance> CIMRepository::enumerateInstancesForSubtree(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    return _rep->enumerateInstancesForSubtree(
        true,
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMInstance> CIMRepository::enumerateInstancesForClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    return _rep->enumerateInstancesForClass(
        true,
        nameSpace,
        className,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMObjectPath> CIMRepository::enumerateInstanceNamesForSubtree(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    return _rep->enumerateInstanceNamesForSubtree(
        true,
        nameSpace,
        className);
}

Array<CIMObjectPath> CIMRepository::enumerateInstanceNamesForClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    return _rep->enumerateInstanceNamesForClass(
        true,
        nameSpace,
        className);
}

Array<CIMInstance> CIMRepository::execQuery(
    const String& queryLanguage,
    const String& query)
{
    return _rep->execQuery(
        true,
        queryLanguage,
        query);
}

Array<CIMObject> CIMRepository::associators(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    return _rep->associators(
        true,
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMObjectPath> CIMRepository::associatorNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    return _rep->associatorNames(
        true,
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole);
}

Array<CIMObject> CIMRepository::references(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    return _rep->references(
        true,
        nameSpace,
        objectName,
        resultClass,
        role,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMObjectPath> CIMRepository::referenceNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role)
{
    return _rep->referenceNames(
        true,
        nameSpace,
        objectName,
        resultClass,
        role);
}

CIMValue CIMRepository::getProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    return _rep->getProperty(
        true,
        nameSpace,
        instanceName,
        propertyName);
}

void CIMRepository::setProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue,
    const ContentLanguageList& contentLangs)
{
    _rep->setProperty(
        true,
        nameSpace,
        instanceName,
        propertyName,
        newValue,
        contentLangs);
}

CIMQualifierDecl CIMRepository::getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    return _rep->getQualifier(
        true,
        nameSpace,
        qualifierName);
}

void CIMRepository::setQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDecl,
    const ContentLanguageList& contentLangs)
{
    _rep->setQualifier(
        true,
        nameSpace,
        qualifierDecl,
        contentLangs);
}

void CIMRepository::deleteQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    _rep->deleteQualifier(
        true,
        nameSpace,
        qualifierName);
}

Array<CIMQualifierDecl> CIMRepository::enumerateQualifiers(
    const CIMNamespaceName& nameSpace)
{
    return _rep->enumerateQualifiers(
        true,
        nameSpace);
}

void CIMRepository::createNameSpace(
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    _rep->createNameSpace(
        true,
        nameSpace,
        attributes);
}

void CIMRepository::modifyNameSpace(
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    _rep->modifyNameSpace(
        true,
        nameSpace,
        attributes);
}

Array<CIMNamespaceName> CIMRepository::enumerateNameSpaces() const
{
    return _rep->enumerateNameSpaces(true);
}

void CIMRepository::deleteNameSpace(
    const CIMNamespaceName& nameSpace)
{
    _rep->deleteNameSpace(
        true,
        nameSpace);
}

Boolean CIMRepository::getNameSpaceAttributes(
    const CIMNamespaceName& nameSpace,
    NameSpaceAttributes& attributes)
{
    return _rep->getNameSpaceAttributes(
        true,
        nameSpace,
        attributes);
}

void CIMRepository::setDeclContext(
    RepositoryDeclContext* context)
{
    _rep->setDeclContext(
        true,
        context);
}

Boolean CIMRepository::isDefaultInstanceProvider()
{
    return _rep->isDefaultInstanceProvider(true);
}

void CIMRepository::getSubClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Array<CIMName>& subClassNames) const
{
    _rep->getSubClassNames(
        true,
        nameSpace,
        className,
        deepInheritance,
        subClassNames);
}

void CIMRepository::getSuperClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Array<CIMName>& superClassNames) const
{
    _rep->getSuperClassNames(
        true,
        nameSpace,
        className,
        superClassNames);
}

Boolean CIMRepository::isRemoteNameSpace(
    const CIMNamespaceName& nameSpace,
    String& remoteInfo)
{
    return _rep->isRemoteNameSpace(
        true,
        nameSpace,
        remoteInfo);
}

#ifdef PEGASUS_DEBUG
void CIMRepository::DisplayCacheStatistics()
{
    _rep->DisplayCacheStatistics(true);
}
#endif

PEGASUS_NAMESPACE_END
