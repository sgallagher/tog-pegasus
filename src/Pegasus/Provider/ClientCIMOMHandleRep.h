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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Client/CIMClient.h>

#include "CIMOMHandleRep.h"

PEGASUS_NAMESPACE_BEGIN

/**
    ClientCIMOMHandleRep is a CIMOMHandle implementation that communicates
    with the CIM Server through the use of a CIMClient object.  This
    implementation is intended for use with Out-of-Process Providers, for
    which communication with the CIM Server must be serialized over an
    interprocess connection.
 */
class ClientCIMOMHandleRep : public CIMOMHandleRep
{
public:
    ClientCIMOMHandleRep();
    virtual ~ClientCIMOMHandleRep();

    virtual CIMClass getClass(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    virtual Array<CIMClass> enumerateClasses(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin);

    virtual Array<CIMName> enumerateClassNames(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance);

    virtual void createClass(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMClass& newClass);

    virtual void modifyClass(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMClass& modifiedClass);

    virtual void deleteClass(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    virtual CIMInstance getInstance(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    virtual Array<CIMInstance> enumerateInstances(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    virtual Array<CIMObjectPath> enumerateInstanceNames(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    virtual CIMObjectPath createInstance(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMInstance& newInstance);

    virtual void modifyInstance(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMInstance& modifiedInstance,
        Boolean includeQualifiers,
        const CIMPropertyList& propertyList);

    virtual void deleteInstance(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName);

    virtual Array<CIMObject> execQuery(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const String& queryLanguage,
        const String& query);

    virtual Array<CIMObject> associators(
        const OperationContext & context,
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
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const CIMName& resultClass,
        const String& role,
        const String& resultRole);

    virtual Array<CIMObject> references(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass,
        const String& role,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    virtual Array<CIMObjectPath> referenceNames(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass,
        const String& role);

    // property operations
    virtual CIMValue getProperty(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName);

    virtual void setProperty(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName,
        const CIMValue& newValue);

    virtual CIMValue invokeMethod(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& methodName,
        const Array<CIMParamValue>& inParameters,
        Array<CIMParamValue>& outParameters);

    virtual OperationContext getResponseContext();

#ifdef PEGASUS_OS_OS400
    // This method has no meaning for CIMClient-based handles
    virtual void setOS400ProfileHandle(const char* profileHandle) {}
#endif

private:
    // Unimplemented
    ClientCIMOMHandleRep(const ClientCIMOMHandleRep& rep);
    // Unimplemented
    ClientCIMOMHandleRep& operator=(const ClientCIMOMHandleRep &);

    CIMClient* _client;
    Mutex _clientMutex;
};

PEGASUS_NAMESPACE_END
