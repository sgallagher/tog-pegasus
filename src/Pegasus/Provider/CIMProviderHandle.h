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

#ifndef Pegasus_CIMProviderHandle_h
#define Pegasus_CIMProviderHandle_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/ProviderHandle.h>
#include <Pegasus/Provider/CIMProvider.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PROVIDER_LINKAGE CIMProviderHandle : public ProviderHandle
{
public:
	CIMProviderHandle(CIMProvider * provider);
	virtual ~CIMProviderHandle(void);

	virtual void initialize(CIMOMHandle & cimom);
	virtual void terminate(void);

	virtual CIMClass getClass(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const Array<String>& propertyList);
	
	virtual void getClassAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const Array<String>& propertyList,
		ResponseHandler<CIMClass> & handler);

	virtual Array<CIMClass> enumerateClasses(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin);
	
	virtual void enumerateClassesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		ResponseHandler<CIMClass> & handler);

	virtual Array<String> enumerateClassNames(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance);

	virtual void enumerateClassNamesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance,
		ResponseHandler<CIMReference> & handler);

	virtual void createClass(
		const OperationContext & context,
		const String& nameSpace,
		const CIMClass& newClass);
	
	virtual void createClassAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMClass& newClass,
		ResponseHandler<CIMClass> & handler);

	virtual void modifyClass(
		const OperationContext & context,
		const String& nameSpace,
		const CIMClass& modifiedClass);

	virtual void modifyClassAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMClass& modifiedClass,
		ResponseHandler<CIMClass> & handler);

	virtual void deleteClass(
		const OperationContext & context,
		const String& nameSpace,
		const String& className);
	
	virtual void deleteClassAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		ResponseHandler<CIMClass> & handler);

	virtual CIMInstance getInstance(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const Array<String>& propertyList);
	
	virtual void getInstanceAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const Array<String>& propertyList,
		ResponseHandler<CIMInstance> & handler);

	virtual Array<CIMInstance> enumerateInstances(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const Array<String>& propertyList);

	virtual void enumerateInstancesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const Array<String>& propertyList,
		ResponseHandler<CIMInstance> & handler);

	virtual Array<CIMReference> enumerateInstanceNames(
		const OperationContext & context,
		const String& nameSpace,
		const String& className);
	
	virtual void enumerateInstanceNamesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		ResponseHandler<CIMReference> & handler);

	virtual void createInstance(
		const OperationContext & context,
		const String& nameSpace,
		const CIMInstance& newInstance);

	virtual void createInstanceAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMInstance& newInstance,
		ResponseHandler<CIMInstance> & handler);

	virtual void modifyInstance(
		const OperationContext & context,
		const String& nameSpace,
		const CIMInstance& modifiedInstance);

	virtual void modifyInstanceAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMInstance& modifiedInstance,
		ResponseHandler<CIMInstance> & handler);

	virtual void deleteInstance(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName);
	
	virtual void deleteInstanceAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		ResponseHandler<CIMInstance> & handler);

	virtual Array<CIMInstance> execQuery(
		const OperationContext & context,
		const String& queryLanguage,
		const String& query);
	
	virtual void execQueryAsync(
		const OperationContext & context,
		const String& queryLanguage,
		const String& query,
		ResponseHandler<CIMObject> & handler);

	virtual Array<CIMObjectWithPath> associators(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const Array<String>& propertyList);

	virtual void associatorsAsync(
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
		ResponseHandler<CIMObjectWithPath> & handler);

	virtual Array<CIMReference> associatorNames(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole);

	virtual void associatorNamesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole,
		ResponseHandler<CIMReference> & handler);

	virtual Array<CIMObjectWithPath> references(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& resultClass,
		const String& role,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const Array<String>& propertyList);

	virtual void referencesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& resultClass,
		const String& role,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const Array<String>& propertyList,
		ResponseHandler<CIMObjectWithPath> & handler);

	virtual Array<CIMReference> referenceNames(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& resultClass,
		const String& role);

	virtual void referenceNamesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& resultClass,
		const String& role,
		ResponseHandler<CIMReference> & handler);

	virtual CIMValue getProperty(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& propertyName);
	
	virtual void getPropertyAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& propertyName,
		ResponseHandler<CIMValue> & handler);

	virtual void setProperty(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& propertyName,
		const CIMValue& newValue);
	
	virtual void setPropertyAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& propertyName,
		const CIMValue& newValue,
		ResponseHandler<CIMValue> & handler);

	virtual CIMValue invokeMethod(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& methodName,
		const Array<CIMParamValue>& inParameters,
		Array<CIMParamValue>& outParameters);
	
	virtual void invokeMethodAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& methodName,
		const Array<CIMParamValue>& inParameters,
		Array<CIMParamValue>& outParameters,
		ResponseHandler<CIMValue> & handler);

protected:
	CIMProvider * _provider;

};

PEGASUS_NAMESPACE_END

#endif
