//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMProviderHandle.h"

PEGASUS_NAMESPACE_BEGIN

CIMProviderHandle::CIMProviderHandle(CIMProvider * provider) : _provider(provider)
{
}

CIMProviderHandle::~CIMProviderHandle(void)
{
}

void CIMProviderHandle::initialize(CIMOMHandle & cimom)
{
	_provider->initialize(cimom);
}

void CIMProviderHandle::terminate(void)
{
	_provider->terminate();
}

CIMClass CIMProviderHandle::getClass(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
	CIMClass cimClass = _provider->getClass(nameSpace,
		className,
		localOnly,
		includeQualifiers,
		includeClassOrigin,
		propertyList);

	return(cimClass);
}

void CIMProviderHandle::getClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMClass> CIMProviderHandle::enumerateClasses(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    Array<CIMClass> cimClasses;
	
	throw CIMException(CIM_ERR_NOT_SUPPORTED);

	return(cimClasses);
}

void CIMProviderHandle::enumerateClassesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<String> CIMProviderHandle::enumerateClassNames(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
    Array<String> cimNames;
	
	throw CIMException(CIM_ERR_NOT_SUPPORTED);

	return(cimNames);
}

void CIMProviderHandle::enumerateClassNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProviderHandle::createClass(
	const OperationContext & context,
    const String& nameSpace,
    const CIMClass& newClass)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProviderHandle::createClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMClass& newClass,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProviderHandle::modifyClass(
	const OperationContext & context,
    const String& nameSpace,
    const CIMClass& modifiedClass)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProviderHandle::modifyClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMClass& modifiedClass,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProviderHandle::deleteClass(
	const OperationContext & context,
    const String& nameSpace,
    const String& className)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProviderHandle::deleteClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMInstance CIMProviderHandle::getInstance(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
	CIMInstance cimInstance = _provider->getInstance(
		nameSpace,
		instanceName,
		localOnly,
		includeQualifiers,
		includeClassOrigin,
		propertyList);

	return(cimInstance);
}

void CIMProviderHandle::getInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMInstance> CIMProviderHandle::enumerateInstances(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
	Array<CIMInstance> cimInstances = _provider->enumerateInstances(
		nameSpace,
		className,
		deepInheritance,
		localOnly,
		includeQualifiers,
		includeClassOrigin,
		propertyList);

	return(cimInstances);
}

void CIMProviderHandle::enumerateInstancesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMReference> CIMProviderHandle::enumerateInstanceNames(
	const OperationContext & context,
    const String& nameSpace,
    const String& className)
{
    Array<CIMReference> cimReferences = _provider->enumerateInstanceNames(
		nameSpace,
		className);
	
	return(cimReferences);
}

void CIMProviderHandle::enumerateInstanceNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProviderHandle::createInstance(
	const OperationContext & context,
    const String& nameSpace,
    const CIMInstance& newInstance)
{
	_provider->createInstance(
		nameSpace,
		newInstance);
}

void CIMProviderHandle::createInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMInstance& newInstance,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProviderHandle::modifyInstance(
	const OperationContext & context,
    const String& nameSpace,
    const CIMInstance& modifiedInstance)
{
	_provider->modifyInstance(
		nameSpace,
		modifiedInstance);
}

void CIMProviderHandle::modifyInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMInstance& modifiedInstance,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProviderHandle::deleteInstance(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName)
{
	_provider->deleteInstance(
		nameSpace,
		instanceName);
}

void CIMProviderHandle::deleteInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMInstance> CIMProviderHandle::execQuery(
	const OperationContext & context,
    const String& queryLanguage,
    const String& query)
{
    Array<CIMInstance> cimInstances;

	throw CIMException(CIM_ERR_NOT_SUPPORTED);

	return(cimInstances);
}

void CIMProviderHandle::execQueryAsync(
	const OperationContext & context,
    const String& queryLanguage,
    const String& query,
	ResponseHandler<CIMObject> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMObjectWithPath> CIMProviderHandle::associators(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Array<CIMObjectWithPath> cimObjects;
	
	throw CIMException(CIM_ERR_NOT_SUPPORTED);

	return(cimObjects);
}

void CIMProviderHandle::associatorsAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList,
	ResponseHandler<CIMObjectWithPath> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMReference> CIMProviderHandle::associatorNames(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{
    Array<CIMReference> cimReferences;
	
	throw CIMException(CIM_ERR_NOT_SUPPORTED);

	return(cimReferences);
}

void CIMProviderHandle::associatorNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMObjectWithPath> CIMProviderHandle::references(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Array<CIMObjectWithPath> cimObjects;
	
	throw CIMException(CIM_ERR_NOT_SUPPORTED);

	return(cimObjects);
}

void CIMProviderHandle::referencesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList,
    ResponseHandler<CIMObjectWithPath> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMReference> CIMProviderHandle::referenceNames(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{
    Array<CIMReference> cimReferences;
	
	throw CIMException(CIM_ERR_NOT_SUPPORTED);

	return(cimReferences);
}

void CIMProviderHandle::referenceNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMValue CIMProviderHandle::getProperty(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName)
{
	CIMValue cimValue = _provider->getProperty(
		nameSpace,
		instanceName,
		propertyName);

	return(cimValue);
}

void CIMProviderHandle::getPropertyAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
	ResponseHandler<CIMValue> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProviderHandle::setProperty(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{
	_provider->setProperty(
		nameSpace,
		instanceName,
		propertyName,
		newValue);
}

void CIMProviderHandle::setPropertyAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue,
	ResponseHandler<CIMValue> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMValue CIMProviderHandle::invokeMethod(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters)
{
	// forward request
	CIMValue cimValue = _provider->invokeMethod(
		nameSpace,
		instanceName,
		methodName,
		inParameters,
		outParameters);

	return(cimValue);
}

void CIMProviderHandle::invokeMethodAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters,
	ResponseHandler<CIMValue> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

PEGASUS_NAMESPACE_END
