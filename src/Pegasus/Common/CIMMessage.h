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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMMessage_h
#define Pegasus_CIMMessage_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMObject.h>

PEGASUS_NAMESPACE_BEGIN

enum CIMMessageType
{
    CIM_MESSAGE = 1000,
    CIM_REQUEST_MESSAGE,
    CIM_GET_CLASS_REQUEST_MESSAGE,
    CIM_GET_INSTANCE_REQUEST_MESSAGE,
    CIM_DELETE_CLASS_REQUEST_MESSAGE,
    CIM_DELETE_INSTANCE_REQUEST_MESSAGE,
    CIM_CREATE_CLASS_REQUEST_MESSAGE,
    CIM_CREATE_INSTANCE_REQUEST_MESSAGE,
    CIM_MODIFY_CLASS_REQUEST_MESSAGE,
    CIM_MODIFY_INSTANCE_REQUEST_MESSAGE,
    CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE,
    CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE,
    CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE,
    CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE,
    CIM_EXEC_QUERY_REQUEST_MESSAGE,
    CIM_ASSOCIATORS_REQUEST_MESSAGE,
    CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE,
    CIM_REFERENCES_REQUEST_MESSAGE,
    CIM_REFERENCE_NAMES_REQUEST_MESSAGE,
    CIM_GET_PROPERTY_REQUEST_MESSAGE,
    CIM_SET_PROPERTY_REQUEST_MESSAGE,
    CIM_GET_QUALIFIER_REQUEST_MESSAGE,
    CIM_SET_QUALIFIER_REQUEST_MESSAGE,
    CIM_DELETE_QUALIFIER_REQUEST_MESSAGE,
    CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE,
    CIM_INVOKE_METHOD_REQUEST_MESSAGE,
    CIM_RESPONSE_MESSAGE,
    CIM_GET_CLASS_RESPONSE_MESSAGE,
    CIM_GET_INSTANCE_RESPONSE_MESSAGE,
    CIM_DELETE_CLASS_RESPONSE_MESSAGE,
    CIM_DELETE_INSTANCE_RESPONSE_MESSAGE,
    CIM_CREATE_CLASS_RESPONSE_MESSAGE,
    CIM_CREATE_INSTANCE_RESPONSE_MESSAGE,
    CIM_MODIFY_CLASS_RESPONSE_MESSAGE,
    CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE,
    CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE,
    CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE,
    CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE,
    CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE,
    CIM_EXEC_QUERY_RESPONSE_MESSAGE,
    CIM_ASSOCIATORS_RESPONSE_MESSAGE,
    CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE,
    CIM_REFERENCES_RESPONSE_MESSAGE,
    CIM_REFERENCE_NAMES_RESPONSE_MESSAGE,
    CIM_GET_PROPERTY_RESPONSE_MESSAGE,
    CIM_SET_PROPERTY_RESPONSE_MESSAGE,
    CIM_GET_QUALIFIER_RESPONSE_MESSAGE,
    CIM_SET_QUALIFIER_RESPONSE_MESSAGE,
    CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE,
    CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE,
    CIM_INVOKE_METHOD_RESPONSE_MESSAGE
};

class CIMMessage : public Message
{
public:
    CIMMessage(Uint32 type, const String& messageId_) 
	: Message(type), messageId(messageId_) { }

    const String& messageId;
};

class CIMRequestMessage : public CIMMessage
{
public:
    CIMRequestMessage(Uint32 type_, const String& messageId_, Uint32 queueId_) 
	: CIMMessage(type_, messageId_), queueId(queueId_) { }

    Uint32 queueId;
};

class CIMResponseMessage : public CIMMessage
{
public:
    CIMResponseMessage(
	Uint32 type_, 
	const String& messageId_,
	CIMStatusCode code_)
	: CIMMessage(type_, messageId_), code(code_)  { }

    CIMStatusCode code;
};

class CIMGetClassRequestMessage : public CIMRequestMessage
{
public:
    
    CIMGetClassRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const String& className_,
	Boolean localOnly_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	const Array<String>& propertyList_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(CIM_GET_CLASS_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	className(className_),
	localOnly(localOnly_),
	includeQualifiers(includeQualifiers_),
	includeClassOrigin(includeClassOrigin_),
	propertyList(propertyList_)
    {
    }

    String nameSpace;
    String className;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    Array<String> propertyList;
};

class CIMGetInstanceRequestMessage : public CIMRequestMessage
{
    CIMGetInstanceRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMReference& instanceName_,
	Boolean localOnly_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	const Array<String>& propertyList_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_GET_INSTANCE_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	instanceName(instanceName_),
	localOnly(localOnly_),
	includeQualifiers(includeQualifiers_),
	includeClassOrigin(includeClassOrigin_),
	propertyList(propertyList_)
    {

    }

    String nameSpace;
    CIMReference instanceName;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    Array<String> propertyList;
};

class CIMDeleteClassRequestMessage : public CIMRequestMessage
{
public:
    
    CIMDeleteClassRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const String& className_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_DELETE_CLASS_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	className(className_)
    {

    }

    String nameSpace;
    String className;
};

class CIMDeleteInstanceRequestMessage : public CIMRequestMessage
{
public:
    
    CIMDeleteInstanceRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMReference& instanceName_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_DELETE_INSTANCE_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	instanceName(instanceName_)
    {

    }

    String nameSpace;
    CIMReference instanceName;
};

class CIMCreateClassRequestMessage : public CIMRequestMessage
{
public:

    CIMCreateClassRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMClass& newClass_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_CREATE_CLASS_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	newClass(newClass_)
    {

    }

    String nameSpace;
    CIMClass newClass;
};

class CIMCreateInstanceRequestMessage : public CIMRequestMessage
{
public:

    CIMCreateInstanceRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMInstance& newInstance_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_CREATE_INSTANCE_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	newInstance(newInstance_)
    {

    }

    String nameSpace;
    CIMInstance newInstance;
};

class CIMModifyClassRequestMessage : public CIMRequestMessage
{
public:

    CIMModifyClassRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMClass& modifiedClass_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_MODIFY_CLASS_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	modifiedClass(modifiedClass_)
    {

    }

    String nameSpace;
    CIMClass modifiedClass;
};

class CIMModifyInstanceRequestMessage : public CIMRequestMessage
{
public:

    CIMModifyInstanceRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMInstance& modifiedInstance_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_MODIFY_INSTANCE_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	modifiedInstance(modifiedInstance_)
    {

    }

    String nameSpace;
    CIMInstance modifiedInstance;
};

class CIMEnumerateClassesRequestMessage : public CIMRequestMessage
{
public:
    CIMEnumerateClassesRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const String& className_,
	Boolean deepInheritance_,
	Boolean localOnly_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	className(className_),
	deepInheritance(deepInheritance_),
	localOnly(localOnly_),
	includeQualifiers(includeQualifiers_),
	includeClassOrigin(includeClassOrigin_)
    {
    }

    String nameSpace;
    String className;
    Boolean deepInheritance;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
};

class CIMEnumerateClassNamesRequestMessage : public CIMRequestMessage
{
public:
    CIMEnumerateClassNamesRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const String& className_,
	Boolean deepInheritance_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	className(className_),
	deepInheritance(deepInheritance_)
    {

    }

    String nameSpace;
    String className;
    Boolean deepInheritance;
};

class CIMEnumerateInstancesRequestMessage : public CIMRequestMessage
{
public:

    CIMEnumerateInstancesRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const String& className_,
	Boolean deepInheritance_,
	Boolean localOnly_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	const Array<String>& propertyList_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	className(className_),
	deepInheritance(deepInheritance_),
	localOnly(localOnly_),
	includeQualifiers(includeQualifiers_),
	includeClassOrigin(includeClassOrigin_),
	propertyList(propertyList_)
    {

    }

    String nameSpace;
    String className;
    Boolean deepInheritance;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    Array<String> propertyList;
};

class CIMEnumerateInstanceNamesRequestMessage : public CIMRequestMessage
{
public:

    CIMEnumerateInstanceNamesRequestMessage(
	const String& messageId_,
	const String& nameSpace_,
	const String& className_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	className(className_)
    {
    }

    String nameSpace;
    String className;
};

class CIMExecQueryRequestMessage : public CIMRequestMessage
{
public:

    CIMExecQueryRequestMessage(
        const String& messageId_,
	const String& queryLanguage_,
	const String& query_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(CIM_EXEC_QUERY_REQUEST_MESSAGE, messageId_, queueId_),
	queryLanguage(queryLanguage_),
	query(query_)
    {
    }

    String queryLanguage;
    String query;
};

class CIMAssociatorsRequestMessage : CIMRequestMessage
{
public:    

    CIMAssociatorsRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMReference& objectName_,
	const String& assocClass_,
	const String& resultClass_,
	const String& role_,
	const String& resultRole_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	const Array<String>& propertyList_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_ASSOCIATORS_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	objectName(objectName_),
	assocClass(assocClass_),
	resultClass(resultClass_),
	role(role_),
	resultRole(resultRole_),
	includeQualifiers(includeQualifiers_),
	includeClassOrigin(includeClassOrigin_),
	propertyList(propertyList_)
    {

    }

    String nameSpace;
    CIMReference objectName;
    String assocClass;
    String resultClass;
    String role;
    String resultRole;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    Array<String> propertyList;
};

class CIMAssociatorNamesRequestMessage : CIMRequestMessage
{
public:

    CIMAssociatorNamesRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMReference& objectName_,
	const String& assocClass_,
	const String& resultClass_,
	const String& role_,
	const String& resultRole_,
	Uint32 queueId_) 
	: 
	CIMRequestMessage(
	    CIM_ASSOCIATORS_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	objectName(objectName_),
	assocClass(assocClass_),
	resultClass(resultClass_),
	role(role_),
	resultRole(resultRole_)
    {
    }

    String nameSpace;
    CIMReference objectName;
    String assocClass;
    String resultClass;
    String role;
    String resultRole;
};

class CIMReferencesRequestMessage : public CIMRequestMessage
{
public:

    CIMReferencesRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMReference& objectName_,
	const String& resultClass_,
	const String& role_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	const Array<String>& propertyList_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(CIM_REFERENCES_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	objectName(objectName_),
	resultClass(resultClass),
	role(role_),
	includeQualifiers(includeQualifiers_),
	includeClassOrigin(includeClassOrigin_),
	propertyList(propertyList_)
    {

    }

    String nameSpace;
    CIMReference objectName;
    String resultClass;
    String role;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    Array<String> propertyList;
};

class CIMReferenceNamesRequestMessage : public CIMRequestMessage
{
public:

    CIMReferenceNamesRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMReference& objectName_,
	const String& resultClass_,
	const String& role_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_REFERENCE_NAMES_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	objectName(objectName_),
	resultClass(resultClass_),
	role(role_)
    {
    }
	
    String nameSpace;
    CIMReference objectName;
    String resultClass;
    String role;
};

class CIMGetPropertyRequestMessage : public CIMRequestMessage
{
public:

    CIMGetPropertyRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMReference& instanceName_,
	const String& propertyName_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_GET_PROPERTY_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	instanceName(instanceName_),
	propertyName(propertyName_)
    {

    }

    String nameSpace;
    CIMReference instanceName;
    String propertyName;
};

class CIMSetPropertyRequestMessage : public CIMRequestMessage
{
public:

    CIMSetPropertyRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMReference& instanceName_,
	const String& propertyName_,
	const CIMValue& newValue_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_SET_PROPERTY_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	instanceName(instanceName_),
	propertyName(propertyName_),
	newValue(newValue_)
    {

    }

    String nameSpace;
    CIMReference instanceName;
    String propertyName;
    CIMValue newValue;
};

class CIMGetQualifierRequestMessage : public CIMRequestMessage
{
public:

    CIMGetQualifierRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const String& qualifierName_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_GET_QUALIFIER_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	qualifierName(qualifierName_)
    {

    }

    String nameSpace;
    const String qualifierName;
};

class CIMSetQualifierRequestMessage : public CIMRequestMessage
{
public:

    CIMSetQualifierRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMQualifierDecl& qualifierDeclaration_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_GET_QUALIFIER_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	qualifierDeclaration(qualifierDeclaration_)
    {

    }

    String nameSpace;
    const CIMQualifierDecl qualifierDeclaration;
};

class CIMDeleteQualifierRequestMessage : public CIMRequestMessage
{
public:

    CIMDeleteQualifierRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const String& qualifierName_,
	Uint32 queueId_) 
	:
	CIMRequestMessage(
	    CIM_DELETE_QUALIFIER_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	qualifierName(qualifierName_)
    {

    }

    String nameSpace;
    const String qualifierName;
};

class CIMEnumerateQualifiersRequestMessage : public CIMRequestMessage
{
public:

    CIMEnumerateQualifiersRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	Uint32 queueId_)
	:
	CIMRequestMessage(
	    CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_)
    {

    }

    String nameSpace;
};

class CIMInvokeMethodRequestMessage : public CIMRequestMessage
{
public:

    CIMInvokeMethodRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMReference& instanceName_,
	const String& methodName_,
	const Array<CIMValue>& inParameters_,
	Uint32 queueId_)
	:
	CIMRequestMessage(
	    CIM_INVOKE_METHOD_REQUEST_MESSAGE, messageId_, queueId_),
	nameSpace(nameSpace_),
	instanceName(instanceName_),
	methodName(methodName_),
	inParameters(inParameters_)
    {

    }

    String nameSpace;
    CIMReference instanceName;
    String methodName;
    Array<CIMValue> inParameters;
};

class CIMGetClassResponseMessage : public CIMResponseMessage
{
public:

    CIMGetClassResponseMessage(
        const String& messageId_,
	CIMStatusCode code_,
	const CIMClass& cimClass_)
	:
	CIMResponseMessage(CIM_GET_CLASS_RESPONSE_MESSAGE, messageId_, code_),
	cimClass(cimClass_)
    {
    }

    CIMClass cimClass;
};

class CIMGetInstanceResponseMessage : public CIMResponseMessage
{
public:

    CIMGetInstanceResponseMessage(
        const String& messageId_,
	CIMStatusCode code_,
	const CIMInstance& cimInstance_)
	:
	CIMResponseMessage(
	    CIM_GET_INSTANCE_RESPONSE_MESSAGE, messageId_, code_),
	cimInstance(cimInstance_)
    {
    }

    CIMInstance cimInstance;
};

class CIMDeleteClassResponseMessage : public CIMResponseMessage
{
public:

    CIMDeleteClassResponseMessage(
	const String& messageId_, 
	CIMStatusCode code_)
	: 
	CIMResponseMessage(
	    CIM_DELETE_CLASS_RESPONSE_MESSAGE, messageId_, code_)
    {
    }
};

class CIMDeleteInstanceResponseMessage : public CIMResponseMessage
{
public:

    CIMDeleteInstanceResponseMessage(
	const String& messageId_,
	CIMStatusCode code_)
	: 
	CIMResponseMessage(
	    CIM_DELETE_INSTANCE_RESPONSE_MESSAGE, messageId_, code_)
    {
    }
};

class CIMCreateClassResponseMessage : public CIMResponseMessage
{
public:

    CIMCreateClassResponseMessage(
	const String& messageId_,
	CIMStatusCode code_)
	: 
	CIMResponseMessage(CIM_CREATE_CLASS_RESPONSE_MESSAGE, messageId_, code_)
    {
    }
};

class CIMCreateInstanceResponseMessage : public CIMResponseMessage
{
public:

    CIMCreateInstanceResponseMessage(
	const String& messageId_,
	CIMStatusCode code_)
	: 
	CIMResponseMessage(
	    CIM_CREATE_INSTANCE_RESPONSE_MESSAGE, messageId_, code_)
    {
    }
};

class CIMModifyClassResponseMessage : public CIMResponseMessage
{
public:

    CIMModifyClassResponseMessage(
	const String& messageId_, CIMStatusCode code_)
	: 
	CIMResponseMessage(CIM_MODIFY_CLASS_RESPONSE_MESSAGE, messageId_, code_)
    {
    }
};

class CIMModifyInstanceResponseMessage : public CIMResponseMessage
{
public:

    CIMModifyInstanceResponseMessage(
        const String& messageId_,
	CIMStatusCode code_)
	: 
	CIMResponseMessage(
	    CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE, messageId_, code_)
    {
    }
};

class CIMEnumerateClassesResponseMessage : public CIMResponseMessage
{
public:

    CIMEnumerateClassesResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const Array<CIMClass>& cimClasses_)
        :
        CIMResponseMessage(
	    CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE, messageId_, code_),
	cimClasses(cimClasses_)
    {
    }

    Array<CIMClass> cimClasses;
};

class CIMEnumerateClassNameResponseMessage : public CIMResponseMessage
{
public:

    CIMEnumerateClassNameResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const Array<String>& classNames_)
        :
        CIMResponseMessage(
	    CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE, messageId_, code_),
	classNames(classNames_)
    {
    }

    Array<String> classNames;
};

class CIMEnumerateInstancesResponseMessage : public CIMResponseMessage
{
public:

    CIMEnumerateInstancesResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const Array<CIMInstance>& cimInstances_)
        :
        CIMResponseMessage(
	    CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE, messageId_, code_),
	cimInstances(cimInstances_)
    {
    }

    Array<CIMInstance> cimInstances;
};

class CIMEnumerateInstanceNameResponseMessage : public CIMResponseMessage
{
public:

    CIMEnumerateInstanceNameResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const Array<CIMReference>& instanceNames_)
        :
        CIMResponseMessage(
	    CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE, messageId_, code_),
	instanceNames(instanceNames_)
    {
    }

    Array<CIMReference> instanceNames;
};

class CIMExecQueryResponseMessage : public CIMResponseMessage
{
public:

    CIMExecQueryResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const Array<CIMInstance>& cimInstances_)
        :
        CIMResponseMessage(CIM_EXEC_QUERY_RESPONSE_MESSAGE, messageId_, code_),
        cimInstances(cimInstances_)
    {
    }

    Array<CIMInstance> cimInstances;
};

class CIMAssociatorsResponseMessage : public CIMResponseMessage
{
public:

    CIMAssociatorsResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const Array<CIMObjectWithPath>& cimObjects_)
        :
        CIMResponseMessage(CIM_ASSOCIATORS_RESPONSE_MESSAGE, messageId_, code_),
        cimObjects(cimObjects_)
    {
    }

    Array<CIMObjectWithPath> cimObjects;
};

class CIMAssociatorNamesResponseMessage : public CIMResponseMessage
{
public:

    CIMAssociatorNamesResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const Array<CIMReference>& objectNames_)
        :
        CIMResponseMessage(
	    CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE, messageId_, code_),
        objectNames(objectNames_)
    {
    }

    Array<CIMReference> objectNames;
};

class CIMReferencesResponseMessage : public CIMResponseMessage
{
public:

    CIMReferencesResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const Array<CIMObjectWithPath>& cimObjects_)
        :
        CIMResponseMessage(CIM_REFERENCES_RESPONSE_MESSAGE, messageId_, code_),
        cimObjects(cimObjects_)
    {
    }

    Array<CIMObjectWithPath> cimObjects;
};

class CIMReferenceNamesResponseMessage : public CIMResponseMessage
{
public:

    CIMReferenceNamesResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const Array<CIMReference>& objectNames_)
        :
        CIMResponseMessage(
	    CIM_REFERENCE_NAMES_RESPONSE_MESSAGE, messageId_, code_),
        objectNames(objectNames_)
    {
    }

    Array<CIMReference> objectNames;
};

class CIMGetPropertyResponseMessage : public CIMResponseMessage
{
public:

    CIMGetPropertyResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const CIMValue& value_)
        :
        CIMResponseMessage(
	    CIM_GET_PROPERTY_RESPONSE_MESSAGE, messageId_, code_),
        value(value_)
    {
    }

    CIMValue value;
};

class CIMSetPropertyResponseMessage : public CIMResponseMessage
{
public:

    CIMSetPropertyResponseMessage(const String& messageId_, CIMStatusCode code_)
        : 
	CIMResponseMessage(CIM_SET_PROPERTY_RESPONSE_MESSAGE, messageId_, code_)
    {
    }
};

class CIMGetQualifierResponseMessage : public CIMResponseMessage
{
public:

    CIMGetQualifierResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const CIMQualifierDecl& cimQualifierDecl_)
        :
        CIMResponseMessage(
	    CIM_GET_QUALIFIER_RESPONSE_MESSAGE, messageId_, code_),
        cimQualifierDecl(cimQualifierDecl_)
    {
    }

    CIMQualifierDecl cimQualifierDecl;
};

class CIMSetQualifierResponseMessage : public CIMResponseMessage
{
public:

    CIMSetQualifierResponseMessage(
	const String& messageId_, 
	CIMStatusCode code_)
        : 
	CIMResponseMessage(
	    CIM_SET_QUALIFIER_RESPONSE_MESSAGE, messageId_, code_)
    {
    }
};

class CIMDeleteQualifierResponseMessage : public CIMResponseMessage
{
public:

    CIMDeleteQualifierResponseMessage(
	const String& messageId_, 
	CIMStatusCode code_)
        : 
	CIMResponseMessage(
	    CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE, messageId_, code_)
    {
    }
};

class CIMEnumerateQualifiersResponseMessage : public CIMResponseMessage
{
public:

    CIMEnumerateQualifiersResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
        const Array<CIMQualifierDecl>& cimQualifierDecls_)
        :
        CIMResponseMessage(
	    CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE, messageId_, code_),
        cimQualifierDecls(cimQualifierDecls_)
    {
    }

    Array<CIMQualifierDecl> cimQualifierDecls;
};

class CIMInvokeMethodResponseMessage : public CIMResponseMessage
{
public:

    CIMInvokeMethodResponseMessage(
        const String& messageId_,
        CIMStatusCode code_,
	CIMValue& value_,
        const Array<CIMValue>& outParameters_)
        :
        CIMResponseMessage(
	    CIM_INVOKE_METHOD_RESPONSE_MESSAGE, messageId_, code_),
	value(value_),
        outParameters(outParameters_)
    {
    }

    CIMValue value;
    Array<CIMValue> outParameters;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMMessage_h */
