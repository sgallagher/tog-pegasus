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
// Modified By: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMHandle_h
#define Pegasus_CIMHandle_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMNamedInstance.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMParamValue.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE CIMHandle : public Sharable
{
public:
	CIMHandle(void);
	virtual ~CIMHandle(void);

	// class operations
	virtual CIMClass getClass(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const CIMPropertyList& propertyList) = 0;
	
	virtual void getClassAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const CIMPropertyList& propertyList,
		ResponseHandler<CIMClass> & handler) = 0;

	virtual Array<CIMClass> enumerateClasses(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin) = 0;
	
	virtual void enumerateClassesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		ResponseHandler<CIMClass> & handler) = 0;

	virtual Array<String> enumerateClassNames(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance) = 0;

	virtual void enumerateClassNamesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance,
		ResponseHandler<CIMReference> & handler) = 0;

	virtual void createClass(
		const OperationContext & context,
		const String& nameSpace,
		const CIMClass& newClass) = 0;
	
	virtual void createClassAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMClass& newClass,
		ResponseHandler<CIMClass> & handler) = 0;

	virtual void modifyClass(
		const OperationContext & context,
		const String& nameSpace,
		const CIMClass& modifiedClass) = 0;

	virtual void modifyClassAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMClass& modifiedClass,
		ResponseHandler<CIMClass> & handler) = 0;

	virtual void deleteClass(
		const OperationContext & context,
		const String& nameSpace,
		const String& className) = 0;
	
	virtual void deleteClassAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		ResponseHandler<CIMClass> & handler) = 0;

	// instance operations
	virtual CIMInstance getInstance(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const CIMPropertyList& propertyList) = 0;
	
	virtual void getInstanceAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const CIMPropertyList& propertyList,
		ResponseHandler<CIMInstance> & handler) = 0;

	virtual Array<CIMNamedInstance> enumerateInstances(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const CIMPropertyList& propertyList) = 0;

	virtual void enumerateInstancesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance,
		Boolean localOnly,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const CIMPropertyList& propertyList,
		ResponseHandler<CIMNamedInstance> & handler) = 0;

	virtual Array<CIMReference> enumerateInstanceNames(
		const OperationContext & context,
		const String& nameSpace,
		const String& className) = 0;
	
	virtual void enumerateInstanceNamesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const String& className,
		ResponseHandler<CIMReference> & handler) = 0;

	virtual CIMReference createInstance(
		const OperationContext & context,
		const String& nameSpace,
		const CIMInstance& newInstance) = 0;

	virtual void createInstanceAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMInstance& newInstance,
		ResponseHandler<CIMInstance> & handler) = 0;

	virtual void modifyInstance(
		const OperationContext & context,
		const String& nameSpace,
		const CIMNamedInstance& modifiedInstance,
		Boolean includeQualifiers,
		const CIMPropertyList& propertyList) = 0;

	virtual void modifyInstanceAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMNamedInstance& modifiedInstance,
		Boolean includeQualifiers,
		const CIMPropertyList& propertyList,
		ResponseHandler<CIMInstance> & handler) = 0;

	virtual void deleteInstance(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName) = 0;
	
	virtual void deleteInstanceAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		ResponseHandler<CIMInstance> & handler) = 0;

	// query operations
	virtual Array<CIMInstance> execQuery(
		const OperationContext & context,
		const String& queryLanguage,
		const String& query) = 0;
	
	virtual void execQueryAsync(
		const OperationContext & context,
		const String& queryLanguage,
		const String& query,
		ResponseHandler<CIMObject> & handler) = 0;

	// associator operations
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
		const CIMPropertyList& propertyList) = 0;

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
		const CIMPropertyList& propertyList,
		ResponseHandler<CIMObjectWithPath> & handler) = 0;

	virtual Array<CIMReference> associatorNames(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole) = 0;

	virtual void associatorNamesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole,
		ResponseHandler<CIMReference> & handler) = 0;

	virtual Array<CIMObjectWithPath> references(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& resultClass,
		const String& role,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const CIMPropertyList& propertyList) = 0;

	virtual void referencesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& resultClass,
		const String& role,
		Boolean includeQualifiers,
		Boolean includeClassOrigin,
		const CIMPropertyList& propertyList,
		ResponseHandler<CIMObjectWithPath> & handler) = 0;

	virtual Array<CIMReference> referenceNames(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& resultClass,
		const String& role) = 0;

	virtual void referenceNamesAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& objectName,
		const String& resultClass,
		const String& role,
		ResponseHandler<CIMReference> & handler) = 0;

	// property operations
	virtual CIMValue getProperty(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& propertyName) = 0;
	
	virtual void getPropertyAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& propertyName,
		ResponseHandler<CIMValue> & handler) = 0;

	virtual void setProperty(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& propertyName,
		const CIMValue& newValue) = 0;
	
	virtual void setPropertyAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& propertyName,
		const CIMValue& newValue,
		ResponseHandler<CIMValue> & handler) = 0;

	// method operations
	virtual CIMValue invokeMethod(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& methodName,
		const Array<CIMParamValue>& inParameters,
		Array<CIMParamValue> & outParameters) = 0;
	
	virtual void invokeMethodAsync(
		const OperationContext & context,
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& methodName,
		const Array<CIMParamValue>& inParameters,
		Array<CIMParamValue>& outParameters,
		ResponseHandler<CIMValue> & handler) = 0;
};

PEGASUS_NAMESPACE_END

#endif
