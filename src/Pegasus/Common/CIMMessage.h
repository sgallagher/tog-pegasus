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

PEGASUS_NAMESPACE_BEGIN

enum CIMMessageType
{
    CIM_MESSAGE = 1000,
    CIM_REQUEST_MESSAGE,
    CIM_RESPONSE_MESSAGE,
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
    CIM_ENUMERATE_INSTANCES_NAMES_REQUEST_MESSAGE,
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
    CIM_INVOKE_METHOD_REQUEST_MESSAGE
};

class CIMMessage : public Message
{
public:
    CIMMessage(Uint32 type) : Message(type) { }
};

class CIMRequestMessage : public CIMMessage
{
public:
    CIMRequestMessage(Uint32 type_) : CIMMessage(type_) { }
};

class CIMResponseMessage : public CIMMessage
{
public:
    CIMResponseMessage(Uint32 type) : CIMMessage(type) { }
};

class CIMGetClassRequestMessage : public CIMRequestMessage
{
public:
    
    CIMGetClassRequestMessage(
	const String& nameSpace_,
	const String& className_,
	Boolean localOnly_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	const Array<String>& propertyList_) 
	:
	CIMRequestMessage(CIM_GET_CLASS_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMReference& instanceName_,
	Boolean localOnly_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	const Array<String>& propertyList_)
	:
	CIMRequestMessage(CIM_GET_INSTANCE_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const String& className_)
	:
	CIMRequestMessage(CIM_DELETE_CLASS_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMReference& instanceName_)
	:
	CIMRequestMessage(CIM_DELETE_INSTANCE_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMClass& newClass_)
	:
	CIMRequestMessage(CIM_CREATE_CLASS_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMInstance& newInstance_)
	:
	CIMRequestMessage(CIM_CREATE_INSTANCE_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMClass& modifiedClass_)
	:
	CIMRequestMessage(CIM_MODIFY_CLASS_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMInstance& modifiedInstance_)
	:
	CIMRequestMessage(CIM_MODIFY_INSTANCE_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const String& className_,
	Boolean deepInheritance_,
	Boolean localOnly_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_)
	:
	CIMRequestMessage(CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const String& className_,
	Boolean deepInheritance_)
	:
	CIMRequestMessage(CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const String& className_,
	Boolean deepInheritance_,
	Boolean localOnly_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	const Array<String>& propertyList_)
	:
	CIMRequestMessage(CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE),
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

class CIMEnumerateInstanceNames : public CIMRequestMessage
{
public:

    CIMEnumerateInstanceNames(
	const String& nameSpace_,
	const String& className_)
	:
	CIMRequestMessage(CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE),
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
	const String& queryLanguage_,
	const String& query_)
	:
	CIMRequestMessage(CIM_EXEC_QUERY_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMReference& objectName_,
	const String& assocClass_,
	const String& resultClass_,
	const String& role_,
	const String& resultRole_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	const Array<String>& propertyList_)
	:
	CIMRequestMessage(CIM_ASSOCIATORS_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMReference& objectName_,
	const String& assocClass_,
	const String& resultClass_,
	const String& role_,
	const String& resultRole_)
	: 
	CIMRequestMessage(CIM_ASSOCIATORS_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMReference& objectName_,
	const String& resultClass_,
	const String& role_,
	Boolean includeQualifiers_,
	Boolean includeClassOrigin_,
	const Array<String>& propertyList_)
	:
	CIMRequestMessage(CIM_REFERENCES_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMReference& objectName_,
	const String& resultClass_,
	const String& role_)
	:
	CIMRequestMessage(CIM_REFERENCE_NAMES_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMReference& instanceName_,
	const String& propertyName_)
	:
	CIMRequestMessage(CIM_GET_PROPERTY_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMReference& instanceName_,
	const String& propertyName_,
	const CIMValue& newValue_)
	:
	CIMRequestMessage(CIM_SET_PROPERTY_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const String& qualifierName_)
	:
	CIMRequestMessage(CIM_GET_QUALIFIER_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const CIMQualifierDecl& qualifierDeclaration_)
	:
	CIMRequestMessage(CIM_GET_QUALIFIER_REQUEST_MESSAGE),
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
	const String& nameSpace_,
	const String& qualifierName_)
	:
	CIMRequestMessage(CIM_DELETE_QUALIFIER_REQUEST_MESSAGE),
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

    CIMEnumerateQualifiersRequestMessage(const String& nameSpace_) :
	CIMRequestMessage(CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE),
	nameSpace(nameSpace_)
    {

    }

    String nameSpace;
};

class CIMInvokeMethodRequestMessage : public CIMRequestMessage
{
public:

    CIMInvokeMethodRequestMessage(
	const String& nameSpace_,
	const CIMReference& instanceName_,
	const String& methodName_,
	const Array<CIMValue>& inParameters_)
	:
	CIMRequestMessage(CIM_INVOKE_METHOD_REQUEST_MESSAGE),
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

/*
    virtual CIMValue invokeMethod(

*/

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMMessage_h */
