//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Jair Santos, Hewlett-Packard Company (jair.santos@hp.com)
//              Willis White (whiwill@us.ibm.com) PEP 128
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMClient.h"
#include <Pegasus/Client/CIMClientRep.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <iostream>
#include <fstream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//
// CIMClient
//
///////////////////////////////////////////////////////////////////////////////

CIMClient::CIMClient()
{
    _rep = new CIMClientRep();   
}   
    
CIMClient::~CIMClient()   
{   
    delete _rep;   
} 

Uint32 CIMClient::getTimeout() const
{
    return _rep->getTimeout();
}

void CIMClient::setTimeout(Uint32 timeoutMilliseconds)
{
    _rep->setTimeout(timeoutMilliseconds);
}

void CIMClient::connect(
    const String& host,
    const Uint32 portNumber,
    const String& userName,
    const String& password
)
{
    _rep->connect(host, portNumber, userName, password);
}

void CIMClient::connect(
    const String& host,
    const Uint32 portNumber,
    const SSLContext& sslContext,
    const String& userName,
    const String& password
)
{
    _rep->connect(host, portNumber, sslContext, userName, password);
}

void CIMClient::connectLocal()
{
    _rep->connectLocal();
}

void CIMClient::disconnect()
{
    _rep->disconnect();
}

// l10n start
void CIMClient::setRequestAcceptLanguages(const AcceptLanguages& langs)
{
    _rep->setRequestAcceptLanguages(langs);
}

AcceptLanguages CIMClient::getRequestAcceptLanguages() const
{
    return _rep->getRequestAcceptLanguages();
}

void CIMClient::setRequestContentLanguages(const ContentLanguages& langs)
{
    _rep->setRequestContentLanguages(langs);
}

ContentLanguages CIMClient::getRequestContentLanguages() const
{
    return _rep->getRequestContentLanguages();
}

ContentLanguages CIMClient::getResponseContentLanguages() const
{
    return _rep->getResponseContentLanguages();
}

void CIMClient::setRequestDefaultLanguages()
{
    _rep->setRequestDefaultLanguages();
}
// l10n end

CIMClass CIMClient::getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
)
{
    return _rep->getClass(
        nameSpace,
        className,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

CIMInstance CIMClient::getInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
)
{
    return _rep->getInstance(
        nameSpace,
        instanceName,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

void CIMClient::deleteClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className
)
{
    _rep->deleteClass(
        nameSpace,
        className);
}

void CIMClient::deleteInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName
)
{
    _rep->deleteInstance(
        nameSpace,
        instanceName);
}

void CIMClient::createClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass
)
{
    _rep->createClass(
        nameSpace,
        newClass);
}

CIMObjectPath CIMClient::createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance
)
{
    return _rep->createInstance(
        nameSpace,
        newInstance);
}

void CIMClient::modifyClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& modifiedClass
)
{
    _rep->modifyClass(
        nameSpace,
        modifiedClass);
}

void CIMClient::modifyInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList
)
{
    _rep->modifyInstance(
        nameSpace,
        modifiedInstance,
        includeQualifiers,
        propertyList);
}

Array<CIMClass> CIMClient::enumerateClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin
)
{
    return _rep->enumerateClasses(
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin);
}

Array<CIMName> CIMClient::enumerateClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance
)
{
    return _rep->enumerateClassNames(
        nameSpace,
        className,
        deepInheritance);
}

Array<CIMInstance> CIMClient::enumerateInstances(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
)
{
    return _rep->enumerateInstances(
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMObjectPath> CIMClient::enumerateInstanceNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className
)
{
    return _rep->enumerateInstanceNames(
        nameSpace,
        className);
}

Array<CIMObject> CIMClient::execQuery(
    const CIMNamespaceName& nameSpace,
    const String& queryLanguage,
    const String& query
)
{
    return _rep->execQuery(
        nameSpace,
        queryLanguage,
        query);
}

Array<CIMObject> CIMClient::associators(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
)
{
    return _rep->associators(
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

Array<CIMObjectPath> CIMClient::associatorNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole
)
{
    return _rep->associatorNames(
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole);
}

Array<CIMObject> CIMClient::references(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
)
{
    return _rep->references(
        nameSpace,
        objectName,
        resultClass,
        role,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMObjectPath> CIMClient::referenceNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role
)
{
    return _rep->referenceNames(
        nameSpace,
        objectName,
        resultClass,
        role);
}

CIMValue CIMClient::getProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName
)
{
    return _rep->getProperty(
        nameSpace,
        instanceName,
        propertyName);
}

void CIMClient::setProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue
)
{
    _rep->setProperty(
        nameSpace,
        instanceName,
        propertyName,
        newValue);
}

CIMQualifierDecl CIMClient::getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName
)
{
    return _rep->getQualifier(
        nameSpace,
        qualifierName);
}

void CIMClient::setQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDeclaration
)
{
    _rep->setQualifier(
        nameSpace,
        qualifierDeclaration);
}

void CIMClient::deleteQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName
)
{
    _rep->deleteQualifier(
        nameSpace,
        qualifierName);
}

Array<CIMQualifierDecl> CIMClient::enumerateQualifiers(
    const CIMNamespaceName& nameSpace
)
{
    return _rep->enumerateQualifiers(
        nameSpace);
}

CIMValue CIMClient::invokeMethod(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters
)
{
    return _rep->invokeMethod(
        nameSpace,
        instanceName,
        methodName,
        inParameters,
        outParameters);
}


void CIMClient::registerClientOpPerformanceDataHandler(ClientOpPerformanceDataHandler & handler)
{
  _rep->registerClientOpPerformanceDataHandler(handler);
}

   
void CIMClient::deregisterClientOpPerformanceDataHandler()
{
  _rep->deregisterClientOpPerformanceDataHandler();
}


PEGASUS_NAMESPACE_END
