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
// Author: Jair Santos, Hewlett-Packard Company (jair.santos@hp.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ClientInterface_h
#define Pegasus_ClientInterface_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/ContentLanguages.h> // l10n
#include <Pegasus/Client/ClientOpPerformanceDataHandler.h> //PEP# 128

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//
// CIMClientInterface
//
///////////////////////////////////////////////////////////////////////////////

class CIMClientInterface : virtual public MessageQueue
{
public:

    enum { DEFAULT_TIMEOUT_MILLISECONDS = 20000 };

    CIMClientInterface(void) :
        MessageQueue(PEGASUS_QUEUENAME_CLIENT)
    { };

    ~CIMClientInterface(void) { };

    virtual void handleEnqueue() = 0;

    virtual Uint32 getTimeout() const = 0;

    virtual void setTimeout(Uint32 timeoutMilliseconds) = 0;

// l10n start
    virtual AcceptLanguages getRequestAcceptLanguages() const = 0;
    virtual ContentLanguages getRequestContentLanguages() const = 0;
    virtual ContentLanguages getResponseContentLanguages() const = 0;
    virtual void setRequestAcceptLanguages(const AcceptLanguages& langs) = 0;
    virtual void setRequestContentLanguages(const ContentLanguages& langs) = 0;
    virtual void setRequestDefaultLanguages() = 0;
// l10n end

//  PEP# 128 Client Satistics
    virtual void registerClientOpPerformanceDataHandler(ClientOpPerformanceDataHandler & handler) = 0;
    virtual void deregisterClientOpPerformanceDataHandler() = 0;
// PEP# 128 end


    virtual void connect(
        const String& host,
        const Uint32 portNumber,
        const String& userName,
        const String& password
    ) = 0;

    virtual void connect(
        const String& host,
        const Uint32 portNumber,
        const SSLContext& sslContext,
        const String& userName,
        const String& password
    ) = 0;

    virtual void connectLocal() = 0;

    virtual void disconnect() = 0;

    virtual CIMClass getClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean localOnly = true,
        Boolean includeQualifiers = true,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()
    ) = 0;

    virtual CIMInstance getInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()
    ) = 0;

    virtual void deleteClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className
    ) = 0;

    virtual void deleteInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName
    ) = 0;

    virtual void createClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& newClass
    ) = 0;

    virtual CIMObjectPath createInstance(
        const CIMNamespaceName& nameSpace,
        const CIMInstance& newInstance
    ) = 0;

    virtual void modifyClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& modifiedClass
    ) = 0;

    virtual void modifyInstance(
        const CIMNamespaceName& nameSpace,
        const CIMInstance& modifiedInstance,
        Boolean includeQualifiers = true,
        const CIMPropertyList& propertyList = CIMPropertyList()
    ) = 0;

    virtual Array<CIMClass> enumerateClasses(
        const CIMNamespaceName& nameSpace,
        const CIMName& className = CIMName(),
        Boolean deepInheritance = false,
        Boolean localOnly = true,
        Boolean includeQualifiers = true,
        Boolean includeClassOrigin = false
    ) = 0;

    virtual Array<CIMName> enumerateClassNames(
        const CIMNamespaceName& nameSpace,
        const CIMName& className = CIMName(),
        Boolean deepInheritance = false
    ) = 0;

    virtual Array<CIMInstance> enumerateInstances(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance = true,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()
    ) = 0;

    virtual Array<CIMObjectPath> enumerateInstanceNames(
        const CIMNamespaceName& nameSpace,
        const CIMName& className
    ) = 0;

    virtual Array<CIMObject> execQuery(
        const CIMNamespaceName& nameSpace,
        const String& queryLanguage,
        const String& query
    ) = 0;

    virtual Array<CIMObject> associators(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass = CIMName(),
        const CIMName& resultClass = CIMName(),
        const String& role = String::EMPTY,
        const String& resultRole = String::EMPTY,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()
    ) = 0;

    virtual Array<CIMObjectPath> associatorNames(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass = CIMName(),
        const CIMName& resultClass = CIMName(),
        const String& role = String::EMPTY,
        const String& resultRole = String::EMPTY
    ) = 0;

    virtual Array<CIMObject> references(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass = CIMName(),
        const String& role = String::EMPTY,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()
    ) = 0;

    virtual Array<CIMObjectPath> referenceNames(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass = CIMName(),
        const String& role = String::EMPTY
    ) = 0;

    virtual CIMValue getProperty(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName
    ) = 0;

    virtual void setProperty(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName,
        const CIMValue& newValue = CIMValue()
    ) = 0;

    virtual CIMQualifierDecl getQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName
    ) = 0;

    virtual void setQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMQualifierDecl& qualifierDeclaration
    ) = 0;

    virtual void deleteQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName
    ) = 0;

    virtual Array<CIMQualifierDecl> enumerateQualifiers(
        const CIMNamespaceName& nameSpace
    ) = 0;

    virtual CIMValue invokeMethod(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& methodName,
        const Array<CIMParamValue>& inParameters,
        Array<CIMParamValue>& outParameters
    ) = 0;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Client_h */
