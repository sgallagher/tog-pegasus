//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PegasusDispatcher_Dispatcher_h
#define PegasusDispatcher_Dispatcher_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMOperations.h>
#include <Pegasus/Server/ProviderTable.h>

PEGASUS_NAMESPACE_BEGIN

class CIMRepository;
class ProviderTable;

class PEGASUS_SERVER_LINKAGE Dispatcher : public CIMOperations
{
public:

    Dispatcher(CIMRepository* repository);
    
    virtual ~Dispatcher();

    virtual CIMClass getClass(
	const String& nameSpace,
	const String& className,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = EmptyStringArray());

    virtual CIMInstance getInstance(
	const String& nameSpace,
	const CIMReference& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = EmptyStringArray());

    virtual void deleteClass(
	const String& nameSpace,
	const String& className);

    virtual void deleteInstance(
	const String& nameSpace,
	const CIMReference& instanceName);

    virtual void createClass(
	const String& nameSpace,
	CIMClass& newClass);

    virtual void createInstance(
	const String& nameSpace,
	CIMInstance& newInstance) ;

    virtual void modifyClass(
	const String& nameSpace,
	CIMClass& modifiedClass);

    virtual void modifyInstance(
	const String& nameSpace,
	CIMInstance& modifiedInstance);

    virtual Array<CIMClass> enumerateClasses(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false,
	Boolean localOnly = true,
	Boolean includeQualifiers  = true,
	Boolean includeClassOrigin = false);

    virtual Array<String> enumerateClassNames(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false);

    virtual Array<CIMInstance> enumerateInstances(
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = EmptyStringArray());

    virtual Array<CIMReference> enumerateInstanceNames(
	const String& nameSpace,
	const String& className);

    virtual Array<CIMInstance> execQuery(
	const String& queryLanguage,
	const String& query) ;

    virtual Array<CIMInstance> associators(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = EmptyStringArray());

    virtual Array<CIMReference> associatorNames(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY);

    virtual Array<CIMInstance> references(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = EmptyStringArray());

    virtual Array<CIMReference> referenceNames(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY);

    virtual CIMValue getProperty(
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& propertyName);

    virtual void setProperty(
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& propertyName,
	const CIMValue& newValue = CIMValue());

    virtual CIMQualifierDecl getQualifier(
	const String& nameSpace,
	const String& qualifierName);

    virtual void setQualifier(
	const String& nameSpace,
	const CIMQualifierDecl& qualifierDecl);

    virtual void deleteQualifier(
	const String& nameSpace,
	const String& qualifierName);

    virtual Array<CIMQualifierDecl> enumerateQualifiers(
	const String& nameSpace);

    virtual CIMValue invokeMethod(
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& methodName,
	const Array<CIMValue>& inParameters,
	Array<CIMValue>& outParameters);

private:
	
    CIMProvider* _lookupProviderForClass(
	const String& nameSpace,
	const String& className);

    CIMRepository* _repository;
    ProviderTable _providerTable;
};

PEGASUS_NAMESPACE_END

#endif /* PegasusDispatcher_Dispatcher_h */
