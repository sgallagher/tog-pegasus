//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMMessage_h
#define Pegasus_CIMMessage_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMObject.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE CIMMessage : public Message
{
public:
    CIMMessage(Uint32 type, const String& messageId_) 
	: Message(type), messageId(messageId_) { }

    virtual ~CIMMessage();

    const String messageId;
};

class PEGASUS_COMMON_LINKAGE CIMRequestMessage : public CIMMessage
{
public:

    CIMRequestMessage(
	Uint32 type_, const String& messageId_, QueueIdStack queueIds_) 
	: CIMMessage(type_, messageId_), queueIds(queueIds_) { }

    virtual ~CIMRequestMessage();

    QueueIdStack queueIds;
};

class PEGASUS_COMMON_LINKAGE CIMResponseMessage : public CIMMessage
{
public:

    CIMResponseMessage(
	Uint32 type_, 
	const String& messageId_,
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_)
	: 
	CIMMessage(type_, messageId_), 
	queueIds(queueIds_),
	errorCode(errorCode_), 
	errorDescription(errorDescription_)
    {
    }

    virtual ~CIMResponseMessage();

    QueueIdStack queueIds;
    CIMStatusCode errorCode;
    String errorDescription;
};

class PEGASUS_COMMON_LINKAGE CIMGetClassRequestMessage 
    : public CIMRequestMessage
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(CIM_GET_CLASS_REQUEST_MESSAGE, messageId_, queueIds_),
	nameSpace(nameSpace_),
	className(className_),
	localOnly(localOnly_),
	includeQualifiers(includeQualifiers_),
	includeClassOrigin(includeClassOrigin_),
	propertyList(propertyList_)
    {
    }

    virtual ~CIMGetClassRequestMessage();

    String nameSpace;
    String className;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    Array<String> propertyList;
};

class CIMGetInstanceRequestMessage : public CIMRequestMessage
{
public:

    CIMGetInstanceRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const CIMReference& instanceName_,
	Boolean localOnly_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	const Array<String>& propertyList_,
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_GET_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_),
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

class CIMExportIndicationRequestMessage : public CIMRequestMessage
{
public:

    CIMExportIndicationRequestMessage(
        const String& messageId_,
	const String& url_,
	const CIMInstance& indicationInstance_,
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_EXPORT_INDICATION_REQUEST_MESSAGE, messageId_, queueIds_),
	url(url_),
	indicationInstance(indicationInstance_)
    {

    }

    String url;
    CIMInstance indicationInstance;
};

class CIMDeleteClassRequestMessage : public CIMRequestMessage
{
public:
    
    CIMDeleteClassRequestMessage(
        const String& messageId_,
	const String& nameSpace_,
	const String& className_,
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_DELETE_CLASS_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_DELETE_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_CREATE_CLASS_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_CREATE_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_MODIFY_CLASS_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_MODIFY_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(CIM_EXEC_QUERY_REQUEST_MESSAGE, messageId_, queueIds_),
	queryLanguage(queryLanguage_),
	query(query_)
    {
    }

    String queryLanguage;
    String query;
};

class CIMAssociatorsRequestMessage : public CIMRequestMessage
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_ASSOCIATORS_REQUEST_MESSAGE, messageId_, queueIds_),
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

class CIMAssociatorNamesRequestMessage : public CIMRequestMessage
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
	QueueIdStack queueIds_) 
	: 
	CIMRequestMessage(
	    CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(CIM_REFERENCES_REQUEST_MESSAGE, messageId_, queueIds_),
	nameSpace(nameSpace_),
	objectName(objectName_),
	resultClass(resultClass_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_REFERENCE_NAMES_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_GET_PROPERTY_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_SET_PROPERTY_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_GET_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_SET_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_) 
	:
	CIMRequestMessage(
	    CIM_DELETE_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_),
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
	QueueIdStack queueIds_)
	:
	CIMRequestMessage(
	    CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE, messageId_, queueIds_),
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
	const Array<CIMParamValue>& inParameters_,
	QueueIdStack queueIds_)
	:
	CIMRequestMessage(
	    CIM_INVOKE_METHOD_REQUEST_MESSAGE, messageId_, queueIds_),
	nameSpace(nameSpace_),
	instanceName(instanceName_),
	methodName(methodName_),
	inParameters(inParameters_)
    {

    }

    String nameSpace;
    CIMReference instanceName;
    String methodName;
    Array<CIMParamValue> inParameters;
};

class PEGASUS_COMMON_LINKAGE CIMGetClassResponseMessage 
    : public CIMResponseMessage
{
public:

    CIMGetClassResponseMessage(
        const String& messageId_,
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
	const CIMClass& cimClass_)
	:
	CIMResponseMessage(CIM_GET_CLASS_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
	cimClass(cimClass_)
    {
    }

    virtual ~CIMGetClassResponseMessage();

    CIMClass cimClass;
};

class CIMGetInstanceResponseMessage : public CIMResponseMessage
{
public:

    CIMGetInstanceResponseMessage(
        const String& messageId_,
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
	const CIMInstance& cimInstance_)
	:
	CIMResponseMessage(CIM_GET_INSTANCE_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
	cimInstance(cimInstance_)
    {
    }

    CIMInstance cimInstance;
};

class CIMExportIndicationResponseMessage : public CIMResponseMessage
{
public:

    CIMExportIndicationResponseMessage(
        const String& messageId_,
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_)
	:
	CIMResponseMessage(CIM_EXPORT_INDICATION_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_)
    {
    }
};

class CIMDeleteClassResponseMessage : public CIMResponseMessage
{
public:

    CIMDeleteClassResponseMessage(
	const String& messageId_, 
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_)
	: 
	CIMResponseMessage(CIM_DELETE_CLASS_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_)
    {
    }
};

class CIMDeleteInstanceResponseMessage : public CIMResponseMessage
{
public:

    CIMDeleteInstanceResponseMessage(
	const String& messageId_,
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_)
	: 
	CIMResponseMessage(CIM_DELETE_INSTANCE_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_)
    {
    }
};

class CIMCreateClassResponseMessage : public CIMResponseMessage
{
public:

    CIMCreateClassResponseMessage(
	const String& messageId_,
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_)
	: 
	CIMResponseMessage(CIM_CREATE_CLASS_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_)
    {
    }
};

class CIMCreateInstanceResponseMessage : public CIMResponseMessage
{
public:

    CIMCreateInstanceResponseMessage(
	const String& messageId_,
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_)
	: 
	CIMResponseMessage(CIM_CREATE_INSTANCE_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_)
    {
    }
};

class CIMModifyClassResponseMessage : public CIMResponseMessage
{
public:

    CIMModifyClassResponseMessage(
	const String& messageId_, 
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_)
	: 
	CIMResponseMessage(CIM_MODIFY_CLASS_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_)
    {
    }
};

class CIMModifyInstanceResponseMessage : public CIMResponseMessage
{
public:

    CIMModifyInstanceResponseMessage(
        const String& messageId_,
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_)
	: 
	CIMResponseMessage(CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_)
    {
    }
};

class CIMEnumerateClassesResponseMessage : public CIMResponseMessage
{
public:

    CIMEnumerateClassesResponseMessage(
        const String& messageId_,
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const Array<CIMClass>& cimClasses_)
        :
        CIMResponseMessage(CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
	cimClasses(cimClasses_)
    {
    }

    Array<CIMClass> cimClasses;
};

class CIMEnumerateClassNamesResponseMessage : public CIMResponseMessage
{
public:

    CIMEnumerateClassNamesResponseMessage(
        const String& messageId_,
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const Array<String>& classNames_)
        :
        CIMResponseMessage(CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
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
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const Array<CIMInstance>& cimInstances_)
        :
        CIMResponseMessage(CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
	cimInstances(cimInstances_)
    {
    }

    Array<CIMInstance> cimInstances;
};

class CIMEnumerateInstanceNamesResponseMessage : public CIMResponseMessage
{
public:

    CIMEnumerateInstanceNamesResponseMessage(
        const String& messageId_,
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const Array<CIMReference>& instanceNames_)
        :
        CIMResponseMessage(CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
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
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const Array<CIMInstance>& cimInstances_)
        :
        CIMResponseMessage(CIM_EXEC_QUERY_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
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
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const Array<CIMObjectWithPath>& cimObjects_)
        :
        CIMResponseMessage(CIM_ASSOCIATORS_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
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
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const Array<CIMReference>& objectNames_)
        :
        CIMResponseMessage(CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
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
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const Array<CIMObjectWithPath>& cimObjects_)
        :
        CIMResponseMessage(CIM_REFERENCES_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
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
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const Array<CIMReference>& objectNames_)
        :
        CIMResponseMessage(CIM_REFERENCE_NAMES_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
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
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const CIMValue& value_)
        :
        CIMResponseMessage(CIM_GET_PROPERTY_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
        value(value_)
    {
    }

    CIMValue value;
};

class CIMSetPropertyResponseMessage : public CIMResponseMessage
{
public:

    CIMSetPropertyResponseMessage(
	const String& messageId_, 
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_)
        : 
	CIMResponseMessage(CIM_SET_PROPERTY_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_)
    {
    }
};

class CIMGetQualifierResponseMessage : public CIMResponseMessage
{
public:

    CIMGetQualifierResponseMessage(
        const String& messageId_,
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const CIMQualifierDecl& cimQualifierDecl_)
        :
        CIMResponseMessage(CIM_GET_QUALIFIER_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
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
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_)
        : 
	CIMResponseMessage(CIM_SET_QUALIFIER_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_)
    {
    }
};

class CIMDeleteQualifierResponseMessage : public CIMResponseMessage
{
public:

    CIMDeleteQualifierResponseMessage(
	const String& messageId_, 
	CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_)
        : 
	CIMResponseMessage(CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_)
    {
    }
};

class CIMEnumerateQualifiersResponseMessage : public CIMResponseMessage
{
public:

    CIMEnumerateQualifiersResponseMessage(
        const String& messageId_,
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
        const Array<CIMQualifierDecl>& qualifierDeclarations_)
        :
        CIMResponseMessage(CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
        qualifierDeclarations(qualifierDeclarations_)
    {
    }

    Array<CIMQualifierDecl> qualifierDeclarations;
};

class CIMInvokeMethodResponseMessage : public CIMResponseMessage
{
public:

    CIMInvokeMethodResponseMessage(
        const String& messageId_,
        CIMStatusCode errorCode_,
	const String& errorDescription_,
	const QueueIdStack& queueIds_,
	CIMValue& retValue_,
        const Array<CIMParamValue>& outParameters_,
	const String& methodName_)
        :
        CIMResponseMessage(CIM_INVOKE_METHOD_RESPONSE_MESSAGE, 
	    messageId_, errorCode_, errorDescription_, queueIds_),
	retValue(retValue_),
        outParameters(outParameters_),
	methodName(methodName_)
    {
    }

    CIMValue retValue;
    Array<CIMParamValue> outParameters;
    String methodName;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMMessage_h */
