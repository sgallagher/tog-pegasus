//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Sushma Fernandes, Hewlett-Packard Company
//                  (sushma_fernandes@hp.com)
//              Chip Vincent (cvincent@us.ibm.com)
//              Jair Santos, Hewlett-Packard Company (jair.santos@hp.com)
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
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/AcceptLanguages.h>  // l10n
#include <Pegasus/Common/ContentLanguages.h>  // l10n

/*   ProviderType should become part of Pegasus/Common     PEP# 99
   #include <Pegasus/ProviderManager2/ProviderType.h>
   #define TYPE_INSTANCE    ProviderType::INSTANCE
   #define TYPE_CLASS       ProviderType::CLASS
   #define TYPE_METHOD      ProviderType::METHOD
   #define TYPE_ASSOCIATION ProviderType::ASSOCIATION
   #define TYPE_QUERY       ProviderType::QUERY
*/ 
// using these equations instead      PEP# 99
   #define TYPE_INSTANCE    2
   #define TYPE_CLASS       3
   #define TYPE_METHOD      4
   #define TYPE_ASSOCIATION 5
   #define TYPE_QUERY       6


PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

// l10n - added AcceptLanguages and ContentLanguages below

class PEGASUS_COMMON_LINKAGE CIMMessage : public Message
{
public:

    CIMMessage(Uint32 type,
	 const String& messageId_,
	 const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	 const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
	: Message(type),
	 messageId(messageId_),
	 contentLanguages(contentLanguages_),
	 acceptLanguages(acceptLanguages_)
    {
    }

    virtual void print(PEGASUS_STD(ostream)& os, Boolean printHeader) const
    {
	if (printHeader)
	{
	    os << "CIMMessage\n";
	    os << "{";
	}

	Message::print(os, false);

	os << "    messageId=" << messageId << endl;

	if (printHeader)
	{
	    os << "}";
	}
    }

    const String messageId;
    ContentLanguages contentLanguages;
    AcceptLanguages acceptLanguages;
};

class PEGASUS_COMMON_LINKAGE CIMRequestMessage : public CIMMessage
{
public:
    CIMRequestMessage(
        Uint32 type_,
	const String& messageId_,
	QueueIdStack queueIds_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMMessage(type_, messageId_, contentLanguages_, acceptLanguages_),
	queueIds(queueIds_)
    {
    }

    virtual void print(PEGASUS_STD(ostream)& os, Boolean printHeader) const
    {
	if (printHeader)
	{
	    os << "CIMRequestMessage\n";
	    os << "{";
	}

	CIMMessage::print(os, false);

	os << "    queueIds=" << "<not shown for now>" << endl;

	if (printHeader)
	{
	    os << "}";
	}
    }

    QueueIdStack queueIds;
    OperationContext operationContext;
};

class PEGASUS_COMMON_LINKAGE CIMResponseMessage : public CIMMessage
{
public:
    CIMResponseMessage(
        Uint32 type_,
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMMessage(type_, messageId_, contentLanguages_, acceptLanguages_),
        queueIds(queueIds_),
        cimException(cimException_)
    {
    }

    QueueIdStack queueIds;
    CIMException cimException;
};

class PEGASUS_COMMON_LINKAGE CIMOperationRequestMessage : public CIMRequestMessage
{	// PEP# 99
public:
    CIMOperationRequestMessage(
        Uint32 type_,
	const String& messageId_,
	QueueIdStack queueIds_,
        const CIMNamespaceName& nameSpace_,
	const CIMName& className_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY,
	Uint32 providerType_ = TYPE_INSTANCE)
    : CIMRequestMessage(type_, messageId_, queueIds_,
	 contentLanguages_, acceptLanguages_),
        nameSpace(nameSpace_),
        className(className_),
        providerType(providerType_)
    {
    }
    CIMNamespaceName nameSpace;
    CIMName className;
    Uint32 providerType;
};

class PEGASUS_COMMON_LINKAGE CIMIndicationRequestMessage : public CIMRequestMessage
{	// PEP# 99
public:
    CIMIndicationRequestMessage(
        Uint32 type_,
        const String & messageId_,
        QueueIdStack queueIds_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMRequestMessage(type_, messageId_, queueIds_,
	 contentLanguages_, acceptLanguages_),
        provider(provider_),
        providerModule(providerModule_)
    {
    }
    CIMInstance provider;
    CIMInstance providerModule;
};

class PEGASUS_COMMON_LINKAGE CIMGetClassRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMGetClassRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& className_,
        Boolean localOnly_,
        Boolean includeQualifiers_,
        Boolean includeClassOrigin_,
        const CIMPropertyList& propertyList_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(CIM_GET_CLASS_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,className_,
	 contentLanguages_, acceptLanguages_,
	 TYPE_CLASS),
        localOnly(localOnly_),
        includeQualifiers(includeQualifiers_),
        includeClassOrigin(includeClassOrigin_),
        propertyList(propertyList_),
        authType(authType_),
        userName(userName_)
    {
    }

    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    CIMPropertyList propertyList;
    String authType;
    String userName;
};

class CIMGetInstanceRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMGetInstanceRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& instanceName_,
        Boolean localOnly_,
        Boolean includeQualifiers_,
        Boolean includeClassOrigin_,
        const CIMPropertyList& propertyList_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_GET_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,instanceName_.getClassName(),
	 contentLanguages_, acceptLanguages_),
	instanceName(instanceName_),
        localOnly(localOnly_),
        includeQualifiers(includeQualifiers_),
        includeClassOrigin(includeClassOrigin_),
        propertyList(propertyList_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMObjectPath instanceName;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    CIMPropertyList propertyList;
    String authType;
    String userName;
};

class CIMExportIndicationRequestMessage : public CIMRequestMessage
{
public:
    CIMExportIndicationRequestMessage(
        const String& messageId_,
        const String& destinationPath_,
        const CIMInstance& indicationInstance_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMRequestMessage(
        CIM_EXPORT_INDICATION_REQUEST_MESSAGE, messageId_, queueIds_,
	 contentLanguages_, acceptLanguages_),
        destinationPath(destinationPath_),
        indicationInstance(indicationInstance_),
        authType(authType_),
        userName(userName_)
    {
    }

    String destinationPath;
    CIMInstance indicationInstance;
    String authType;
    String userName;
};

class CIMDeleteClassRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMDeleteClassRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& className_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_DELETE_CLASS_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,className_,
	 contentLanguages_, acceptLanguages_,
	 TYPE_CLASS),
        authType(authType_),
        userName(userName_)
    {
    }

    String authType;
    String userName;
};

class CIMDeleteInstanceRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMDeleteInstanceRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& instanceName_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_DELETE_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,instanceName_.getClassName(),
	 contentLanguages_, acceptLanguages_),
        instanceName(instanceName_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMObjectPath instanceName;
    String authType;
    String userName;
};

class CIMCreateClassRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMCreateClassRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMClass& newClass_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_CREATE_CLASS_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,newClass_.getClassName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_CLASS),
        newClass(newClass_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMClass newClass;
    String authType;
    String userName;
};

class CIMCreateInstanceRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMCreateInstanceRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMInstance& newInstance_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_CREATE_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,newInstance_.getClassName(),
	 contentLanguages_, acceptLanguages_),
        newInstance(newInstance_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMInstance newInstance;
    String authType;
    String userName;
};

class CIMModifyClassRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMModifyClassRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMClass& modifiedClass_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_MODIFY_CLASS_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,modifiedClass_.getClassName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_CLASS),
        modifiedClass(modifiedClass_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMClass modifiedClass;
    String authType;
    String userName;
};

class CIMModifyInstanceRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMModifyInstanceRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMInstance& modifiedInstance_,
        Boolean includeQualifiers_,
        const CIMPropertyList& propertyList_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_MODIFY_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,modifiedInstance_.getClassName(),
	 contentLanguages_, acceptLanguages_),
        modifiedInstance(modifiedInstance_),
        includeQualifiers(includeQualifiers_),
        propertyList(propertyList_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMInstance modifiedInstance;
    Boolean includeQualifiers;
    CIMPropertyList propertyList;
    String authType;
    String userName;
};

class CIMEnumerateClassesRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMEnumerateClassesRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& className_,
        Boolean deepInheritance_,
        Boolean localOnly_,
        Boolean includeQualifiers_,
        Boolean includeClassOrigin_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,className_,
	 contentLanguages_, acceptLanguages_,
	 TYPE_CLASS),
        deepInheritance(deepInheritance_),
        localOnly(localOnly_),
        includeQualifiers(includeQualifiers_),
        includeClassOrigin(includeClassOrigin_),
        authType(authType_),
        userName(userName_)
    {
    }

    Boolean deepInheritance;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    String authType;
    String userName;
};

class CIMEnumerateClassNamesRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMEnumerateClassNamesRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& className_,
        Boolean deepInheritance_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,className_,
	 contentLanguages_, acceptLanguages_,
	 TYPE_CLASS),
        deepInheritance(deepInheritance_),
        authType(authType_),
        userName(userName_)
    {
    }

    Boolean deepInheritance;
    String authType;
    String userName;
};

class CIMEnumerateInstancesRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMEnumerateInstancesRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& className_,
        Boolean deepInheritance_,
        Boolean localOnly_,
        Boolean includeQualifiers_,
        Boolean includeClassOrigin_,
        const CIMPropertyList& propertyList_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,className_,
	 contentLanguages_, acceptLanguages_),
        deepInheritance(deepInheritance_),
        localOnly(localOnly_),
        includeQualifiers(includeQualifiers_),
        includeClassOrigin(includeClassOrigin_),
        propertyList(propertyList_),
        authType(authType_),
        userName(userName_)
    {
    }

    Boolean deepInheritance;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    CIMPropertyList propertyList;
    String authType;
    String userName;
};

class CIMEnumerateInstanceNamesRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMEnumerateInstanceNamesRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& className_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,className_,
	 contentLanguages_, acceptLanguages_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMEnumerateInstanceNamesRequestMessage(
	const CIMEnumerateInstanceNamesRequestMessage& x)
    : CIMOperationRequestMessage(
        CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE, x.messageId, x.queueIds,
	x.nameSpace, x.className),
        authType(x.authType),
        userName(x.userName)
    {
    }

    virtual void print(PEGASUS_STD(ostream)& os, Boolean printHeader) const
    {
	if (printHeader)
	{
	    os << "CIMEnumerateInstanceNamesRequestMessage\n";
	    os << "{";
	}

	CIMRequestMessage::print(os, false);

	os << "    nameSpace=" << nameSpace << endl;
	os << "    className=" << className << endl;
	os << "    authType=" << authType << endl;
	os << "    userName=" << userName << endl;

	if (printHeader)
	{
	    os << "}";
	}
    }

    String authType;
    String userName;
};

class CIMExecQueryRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMExecQueryRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const String& queryLanguage_,
        const String& query_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(CIM_EXEC_QUERY_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,CIMName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_QUERY),
        queryLanguage(queryLanguage_),
        query(query_),
        authType(authType_),
        userName(userName_)
    {
    }

    String queryLanguage;
    String query;
    String authType;
    String userName;
};

class CIMAssociatorsRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMAssociatorsRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& objectName_,
        const CIMName& assocClass_,
        const CIMName& resultClass_,
        const String& role_,
        const String& resultRole_,
        Boolean includeQualifiers_,
        Boolean includeClassOrigin_,
        const CIMPropertyList& propertyList_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ASSOCIATORS_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,objectName_.getClassName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_ASSOCIATION),
        objectName(objectName_),
        assocClass(assocClass_),
        resultClass(resultClass_),
        role(role_),
        resultRole(resultRole_),
        includeQualifiers(includeQualifiers_),
        includeClassOrigin(includeClassOrigin_),
        propertyList(propertyList_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMObjectPath objectName;
    CIMName assocClass;
    CIMName resultClass;
    String role;
    String resultRole;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    CIMPropertyList propertyList;
    String authType;
    String userName;
};

class CIMAssociatorNamesRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMAssociatorNamesRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& objectName_,
        const CIMName& assocClass_,
        const CIMName& resultClass_,
        const String& role_,
        const String& resultRole_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,objectName_.getClassName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_ASSOCIATION),
        objectName(objectName_),
        assocClass(assocClass_),
        resultClass(resultClass_),
        role(role_),
        resultRole(resultRole_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMObjectPath objectName;
    CIMName assocClass;
    CIMName resultClass;
    String role;
    String resultRole;
    String authType;
    String userName;
};

class CIMReferencesRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMReferencesRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& objectName_,
        const CIMName& resultClass_,
        const String& role_,
        Boolean includeQualifiers_,
        Boolean includeClassOrigin_,
        const CIMPropertyList& propertyList_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
         CIM_REFERENCES_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,objectName_.getClassName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_ASSOCIATION),
        objectName(objectName_),
        resultClass(resultClass_),
        role(role_),
        includeQualifiers(includeQualifiers_),
        includeClassOrigin(includeClassOrigin_),
        propertyList(propertyList_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMObjectPath objectName;
    CIMName resultClass;
    String role;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    CIMPropertyList propertyList;
    String authType;
    String userName;
};

class CIMReferenceNamesRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMReferenceNamesRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& objectName_,
        const CIMName& resultClass_,
        const String& role_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_REFERENCE_NAMES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,objectName_.getClassName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_ASSOCIATION),
        objectName(objectName_),
        resultClass(resultClass_),
        role(role_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMObjectPath objectName;
    CIMName resultClass;
    String role;
    String authType;
    String userName;
};

class CIMGetPropertyRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMGetPropertyRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& instanceName_,
        const CIMName& propertyName_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_GET_PROPERTY_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,instanceName_.getClassName(),
	 contentLanguages_, acceptLanguages_),
        instanceName(instanceName_),
        propertyName(propertyName_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMObjectPath instanceName;
    CIMName propertyName;
    String authType;
    String userName;
};

class CIMSetPropertyRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMSetPropertyRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& instanceName_,
        const CIMName& propertyName_,
        const CIMValue& newValue_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_SET_PROPERTY_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,instanceName_.getClassName(),
	 contentLanguages_, acceptLanguages_),
        instanceName(instanceName_),
        propertyName(propertyName_),
        newValue(newValue_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMObjectPath instanceName;
    CIMName propertyName;
    CIMValue newValue;
    String authType;
    String userName;
};

class CIMGetQualifierRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMGetQualifierRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& qualifierName_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_GET_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,CIMName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_CLASS),
        qualifierName(qualifierName_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMName qualifierName;
    String authType;
    String userName;
};

class CIMSetQualifierRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMSetQualifierRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMQualifierDecl& qualifierDeclaration_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_SET_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,CIMName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_CLASS),
        qualifierDeclaration(qualifierDeclaration_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMQualifierDecl qualifierDeclaration;
    String authType;
    String userName;
};

class CIMDeleteQualifierRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMDeleteQualifierRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& qualifierName_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_DELETE_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,CIMName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_CLASS),
        qualifierName(qualifierName_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMName qualifierName;
    String authType;
    String userName;
};

class CIMEnumerateQualifiersRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMEnumerateQualifiersRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,CIMName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_CLASS),
        authType(authType_),
        userName(userName_)
    {
    }

    String authType;
    String userName;
};

class CIMInvokeMethodRequestMessage : public CIMOperationRequestMessage
{
public:
    CIMInvokeMethodRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& instanceName_,
        const CIMName& methodName_,
        const Array<CIMParamValue>& inParameters_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMOperationRequestMessage(
        CIM_INVOKE_METHOD_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,instanceName_.getClassName(),
	 contentLanguages_, acceptLanguages_,
	 TYPE_METHOD),
        instanceName(instanceName_),
        methodName(methodName_),
        inParameters(inParameters_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMObjectPath instanceName;
    CIMName methodName;
    Array<CIMParamValue> inParameters;
    String authType;
    String userName;
};

class CIMProcessIndicationRequestMessage : public CIMRequestMessage
{
public:
    CIMProcessIndicationRequestMessage(
        const String & messageId_,
        const CIMNamespaceName & nameSpace_,
        const CIMInstance& indicationInstance_,
        const Array<CIMObjectPath> & subscriptionInstanceNames_,
        const CIMInstance & provider_,
        QueueIdStack queueIds_,
		const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
		const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMRequestMessage(
        CIM_PROCESS_INDICATION_REQUEST_MESSAGE, messageId_, queueIds_,
		 contentLanguages_, acceptLanguages_),
        nameSpace (nameSpace_),
        indicationInstance(indicationInstance_),
        subscriptionInstanceNames(subscriptionInstanceNames_),
        provider(provider_)
    {
    }

    CIMNamespaceName nameSpace;
    CIMInstance indicationInstance;
    Array<CIMObjectPath> subscriptionInstanceNames;
    CIMInstance provider;
};

class CIMConsumeIndicationRequestMessage : public CIMRequestMessage
{
   public:
      CIMConsumeIndicationRequestMessage(
	 const String & messageId_,
	 const CIMNamespaceName & nameSpace_,     // ns of the origin of the indication
	 const CIMInstance & indicationInstance_,
	 const CIMInstance & consumer_provider_,
	 const CIMInstance & consumer_module_,
	 QueueIdStack queueIds_,
	 const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	 const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
	 : CIMRequestMessage(
	    CIM_CONSUME_INDICATION_REQUEST_MESSAGE, messageId_, queueIds_,
            contentLanguages_, acceptLanguages_),
	   nameSpace(nameSpace_),
	   indicationInstance(indicationInstance_),
	   consumer_provider(consumer_provider_),
	   consumer_module(consumer_module_)
      {
      }

      CIMNamespaceName nameSpace;
      CIMInstance indicationInstance;
      CIMInstance consumer_provider;
      CIMInstance consumer_module;
};



class CIMEnableIndicationsRequestMessage : public CIMIndicationRequestMessage
{
public:
    CIMEnableIndicationsRequestMessage(
        const String & messageId_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
        QueueIdStack queueIds_)
    : CIMIndicationRequestMessage(
        CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE,
        messageId_,
        queueIds_,
	provider_,providerModule_)
    {
    }
};

class CIMDisableIndicationsRequestMessage : public CIMIndicationRequestMessage
{
public:
    CIMDisableIndicationsRequestMessage(
        const String & messageId_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
        QueueIdStack queueIds_)
    : CIMIndicationRequestMessage(
        CIM_DISABLE_INDICATIONS_REQUEST_MESSAGE,
        messageId_,
        queueIds_,
	provider_,providerModule_)
    {
    }
};

class CIMNotifyProviderRegistrationRequestMessage : public CIMRequestMessage
{
public:
    enum Operation
    {
        OP_CREATE = 1, OP_DELETE = 2, OP_MODIFY = 3
    };

    CIMNotifyProviderRegistrationRequestMessage(
        const String & messageId_,
        const Operation operation_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
        const CIMName & className_,
        const Array <CIMNamespaceName> & newNamespaces_,
        const Array <CIMNamespaceName> & oldNamespaces_,
        const CIMPropertyList & newPropertyNames_,
        const CIMPropertyList & oldPropertyNames_,
        QueueIdStack queueIds_)
    : CIMRequestMessage(
        CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE,
        messageId_, queueIds_),
        provider (provider_),
        providerModule (providerModule_),
        className (className_),
        newNamespaces (newNamespaces_),
        oldNamespaces (oldNamespaces_),
        newPropertyNames (newPropertyNames_),
        oldPropertyNames (oldPropertyNames_),
        operation(operation_)
    {
    }

    CIMInstance provider;
    CIMInstance providerModule;
    CIMName className;
    Array <CIMNamespaceName> newNamespaces;
    Array <CIMNamespaceName> oldNamespaces;
    CIMPropertyList newPropertyNames;
    CIMPropertyList oldPropertyNames;
    Operation operation;
};

class CIMNotifyProviderTerminationRequestMessage : public CIMRequestMessage
{
public:
    CIMNotifyProviderTerminationRequestMessage(
        const String & messageId_,
        const Array <CIMInstance> & providers_,
        QueueIdStack queueIds_)
    : CIMRequestMessage(
        CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE,
        messageId_, queueIds_),
        providers (providers_)
    {

    }

    Array <CIMInstance> providers;
};

class CIMHandleIndicationRequestMessage : public CIMRequestMessage
{
public:
    CIMHandleIndicationRequestMessage(
        const String& messageId_,
        const CIMNamespaceName & nameSpace_,
        const CIMInstance& handlerInstance_,
        const CIMInstance& indicationInstance_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMRequestMessage(
        CIM_HANDLE_INDICATION_REQUEST_MESSAGE, messageId_, queueIds_,
	 contentLanguages_, acceptLanguages_),
        nameSpace(nameSpace_),
        handlerInstance(handlerInstance_),
        indicationInstance(indicationInstance_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMNamespaceName nameSpace;
    CIMInstance handlerInstance;
    CIMInstance indicationInstance;
    String authType;
    String userName;
};

class CIMCreateSubscriptionRequestMessage : public CIMIndicationRequestMessage
{
public:
    CIMCreateSubscriptionRequestMessage(
        const String& messageId_,
        const CIMNamespaceName & nameSpace_,
        const CIMInstance & subscriptionInstance_,
        const Array<CIMName> & classNames_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
        const CIMPropertyList & propertyList_,
        const Uint16 repeatNotificationPolicy_,
        const String & condition_,
        const String & queryLanguage_,
        QueueIdStack queueIds_,
        const String & authType_ = String::EMPTY,
        const String & userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMIndicationRequestMessage(
        CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE,
        messageId_,
        queueIds_,
        provider_,providerModule_,
        contentLanguages_, acceptLanguages_),
        nameSpace (nameSpace_),
        subscriptionInstance(subscriptionInstance_),
        classNames(classNames_),
        propertyList (propertyList_),
        repeatNotificationPolicy (repeatNotificationPolicy_),
        condition (condition_),
        queryLanguage (queryLanguage_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMNamespaceName nameSpace;
    CIMInstance subscriptionInstance;
    Array <CIMName> classNames;
    CIMPropertyList propertyList;
    Uint16 repeatNotificationPolicy;
    String condition;
    String queryLanguage;
    String authType;
    String userName;
};

class CIMModifySubscriptionRequestMessage : public CIMIndicationRequestMessage
{
public:
    CIMModifySubscriptionRequestMessage(
        const String& messageId_,
        const CIMNamespaceName & nameSpace_,
        const CIMInstance & subscriptionInstance_,
        const Array<CIMName> & classNames_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
        const CIMPropertyList & propertyList_,
        const Uint16 repeatNotificationPolicy_,
        const String & condition_,
        const String & queryLanguage_,
        QueueIdStack queueIds_,
        const String & authType_ = String::EMPTY,
        const String & userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMIndicationRequestMessage(
        CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE,
        messageId_,
        queueIds_,
        provider_,providerModule_,
	contentLanguages_, acceptLanguages_),
        nameSpace(nameSpace_),
        subscriptionInstance(subscriptionInstance_),
        classNames(classNames_),
        propertyList (propertyList_),
        repeatNotificationPolicy (repeatNotificationPolicy_),
        condition (condition_),
        queryLanguage (queryLanguage_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMNamespaceName nameSpace;
    CIMInstance subscriptionInstance;
    Array<CIMName> classNames;
    CIMPropertyList propertyList;
    Uint16 repeatNotificationPolicy;
    String condition;
    String queryLanguage;
    String authType;
    String userName;
};

class CIMDeleteSubscriptionRequestMessage : public CIMIndicationRequestMessage
{
public:
    CIMDeleteSubscriptionRequestMessage(
        const String& messageId_,
        const CIMNamespaceName & nameSpace_,
        const CIMInstance & subscriptionInstance_,
        const Array<CIMName> & classNames_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY,
	const AcceptLanguages& acceptLanguages_ = AcceptLanguages::EMPTY)
    : CIMIndicationRequestMessage(
        CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE,
        messageId_,
        queueIds_,
        provider_,providerModule_,
        contentLanguages_, acceptLanguages_),
        nameSpace(nameSpace_),
        subscriptionInstance(subscriptionInstance_),
        classNames(classNames_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMNamespaceName nameSpace;
    CIMInstance subscriptionInstance;
    Array<CIMName> classNames;
    String authType;
    String userName;
};

class CIMDisableModuleRequestMessage : public CIMRequestMessage
{
public:
    CIMDisableModuleRequestMessage(
        const String& messageId_,
        const CIMInstance& providerModule_,
        const Array<CIMInstance>& providers_,
	Boolean disableProviderOnly_,
	const Array<Boolean>& indicationProviders_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMRequestMessage(
        CIM_DISABLE_MODULE_REQUEST_MESSAGE,
        messageId_,
        queueIds_),
        providerModule(providerModule_),
        providers(providers_),
	disableProviderOnly(disableProviderOnly_),
	indicationProviders(indicationProviders_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMInstance providerModule;
    Array<CIMInstance> providers;
    Boolean disableProviderOnly;
    Array<Boolean> indicationProviders;
    String authType;
    String userName;
};

class CIMEnableModuleRequestMessage : public CIMRequestMessage
{
public:
    CIMEnableModuleRequestMessage(
        const String& messageId_,
        const CIMInstance& providerModule_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMRequestMessage(
        CIM_ENABLE_MODULE_REQUEST_MESSAGE,
        messageId_,
        queueIds_),
        providerModule(providerModule_),
        authType(authType_),
        userName(userName_)
    {
    }

    CIMInstance providerModule;
    String authType;
    String userName;
};

class CIMNotifyProviderEnableRequestMessage : public CIMRequestMessage
{
public:
    CIMNotifyProviderEnableRequestMessage(
	const String & messageId_,
	const CIMInstance & providerModule_,
	const CIMInstance & provider_,
	const Array <CIMInstance> & capInstances_,
	QueueIdStack queueIds_)
    : CIMRequestMessage(
	CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE,
	messageId_,
        queueIds_),
	providerModule(providerModule_),
	provider(provider_),
	capInstances(capInstances_)
	{
	}
    
    CIMInstance providerModule;
    CIMInstance provider;
    Array <CIMInstance> capInstances;
};

class CIMStopAllProvidersRequestMessage : public CIMRequestMessage
{
   public:
    CIMStopAllProvidersRequestMessage(
        const String& messageId_,
        QueueIdStack queueIds_)
        :
        CIMRequestMessage(
            CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE,
            messageId_,
            queueIds_)
    {
    }
};

class PEGASUS_COMMON_LINKAGE CIMGetClassResponseMessage
    : public CIMResponseMessage
{
public:
    CIMGetClassResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMClass& cimClass_,
		const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_GET_CLASS_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
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
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMInstance& cimInstance_,
		const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_GET_INSTANCE_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
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
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_EXPORT_INDICATION_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class CIMDeleteClassResponseMessage : public CIMResponseMessage
{
public:
    CIMDeleteClassResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_DELETE_CLASS_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class CIMDeleteInstanceResponseMessage : public CIMResponseMessage
{
public:
    CIMDeleteInstanceResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_DELETE_INSTANCE_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class CIMCreateClassResponseMessage : public CIMResponseMessage
{
public:
    CIMCreateClassResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_CREATE_CLASS_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class CIMCreateInstanceResponseMessage : public CIMResponseMessage
{
public:
    CIMCreateInstanceResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMObjectPath& instanceName_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_CREATE_INSTANCE_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
        instanceName(instanceName_)
    {
    }

    CIMObjectPath instanceName;
};

class CIMModifyClassResponseMessage : public CIMResponseMessage
{
public:
    CIMModifyClassResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_MODIFY_CLASS_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class CIMModifyInstanceResponseMessage : public CIMResponseMessage
{
public:
    CIMModifyInstanceResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class CIMEnumerateClassesResponseMessage : public CIMResponseMessage
{
public:
    CIMEnumerateClassesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMClass>& cimClasses_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
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
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMName>& classNames_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
        classNames(classNames_)
    {
    }

    Array<CIMName> classNames;
};

class CIMEnumerateInstancesResponseMessage : public CIMResponseMessage
{
public:
    CIMEnumerateInstancesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMInstance>& cimNamedInstances_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
        cimNamedInstances(cimNamedInstances_)
    {
    }

    Array<CIMInstance> cimNamedInstances;
};

class CIMEnumerateInstanceNamesResponseMessage : public CIMResponseMessage
{
public:
    CIMEnumerateInstanceNamesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObjectPath>& instanceNames_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
        instanceNames(instanceNames_)
    {
    }

    Array<CIMObjectPath> instanceNames;
};

class CIMExecQueryResponseMessage : public CIMResponseMessage
{
public:
    CIMExecQueryResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObject>& cimObjects_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_EXEC_QUERY_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
        cimObjects(cimObjects_)
    {
    }

    Array<CIMObject> cimObjects;
};

class CIMAssociatorsResponseMessage : public CIMResponseMessage
{
public:
    CIMAssociatorsResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObject>& cimObjects_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_ASSOCIATORS_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
        cimObjects(cimObjects_)
    {
    }

    Array<CIMObject> cimObjects;
};

class CIMAssociatorNamesResponseMessage : public CIMResponseMessage
{
public:
    CIMAssociatorNamesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObjectPath>& objectNames_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
        objectNames(objectNames_)
    {
    }

    Array<CIMObjectPath> objectNames;
};

class CIMReferencesResponseMessage : public CIMResponseMessage
{
public:
    CIMReferencesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObject>& cimObjects_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_REFERENCES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
        cimObjects(cimObjects_)
    {
    }

    Array<CIMObject> cimObjects;
};

class CIMReferenceNamesResponseMessage : public CIMResponseMessage
{
public:
    CIMReferenceNamesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObjectPath>& objectNames_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_REFERENCE_NAMES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
        objectNames(objectNames_)
    {
    }

    Array<CIMObjectPath> objectNames;
};

class CIMGetPropertyResponseMessage : public CIMResponseMessage
{
public:
    CIMGetPropertyResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMValue& value_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_GET_PROPERTY_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
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
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_SET_PROPERTY_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class CIMGetQualifierResponseMessage : public CIMResponseMessage
{
public:
    CIMGetQualifierResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMQualifierDecl& cimQualifierDecl_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_GET_QUALIFIER_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
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
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    :
    CIMResponseMessage(CIM_SET_QUALIFIER_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class CIMDeleteQualifierResponseMessage : public CIMResponseMessage
{
public:

    CIMDeleteQualifierResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    :
    CIMResponseMessage(CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class CIMEnumerateQualifiersResponseMessage : public CIMResponseMessage
{
public:
    CIMEnumerateQualifiersResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMQualifierDecl>& qualifierDeclarations_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
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
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMValue& retValue_,
        const Array<CIMParamValue>& outParameters_,
        const CIMName& methodName_,
	const ContentLanguages& contentLanguages_ = ContentLanguages::EMPTY)
    : CIMResponseMessage(CIM_INVOKE_METHOD_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_,
	contentLanguages_),
        retValue(retValue_),
        outParameters(outParameters_),
        methodName(methodName_)
    {
    }

    CIMValue retValue;
    Array<CIMParamValue> outParameters;
    CIMName methodName;
};

class CIMProcessIndicationResponseMessage : public CIMResponseMessage
{
public:
    CIMProcessIndicationResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_CREATE_INSTANCE_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class CIMHandleIndicationResponseMessage : public CIMResponseMessage
{
public:
    CIMHandleIndicationResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_HANDLE_INDICATION_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class CIMCreateSubscriptionResponseMessage : public CIMResponseMessage
{
public:
    CIMCreateSubscriptionResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(
        CIM_CREATE_SUBSCRIPTION_RESPONSE_MESSAGE,
        messageId_,
        cimException_,
        queueIds_)
    {
    }
};

class CIMModifySubscriptionResponseMessage : public CIMResponseMessage
{
public:
    CIMModifySubscriptionResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(
        CIM_MODIFY_SUBSCRIPTION_RESPONSE_MESSAGE,
        messageId_,
        cimException_,
        queueIds_)
    {
    }
};

class CIMDeleteSubscriptionResponseMessage : public CIMResponseMessage
{
public:
    CIMDeleteSubscriptionResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(
        CIM_DELETE_SUBSCRIPTION_RESPONSE_MESSAGE,
        messageId_,
        cimException_,
        queueIds_)
    {
    }
};

class CIMEnableIndicationsResponseMessage : public CIMResponseMessage
{
public:
    CIMEnableIndicationsResponseMessage(
        const String & messageId_,
        const CIMException & cimException_,
        const QueueIdStack & queueIds_)
    : CIMResponseMessage(
        CIM_ENABLE_INDICATIONS_RESPONSE_MESSAGE,
        messageId_,
        cimException_,
        queueIds_)
    {
    }
};

class CIMDisableIndicationsResponseMessage : public CIMResponseMessage
{
public:
    CIMDisableIndicationsResponseMessage(
        const String & messageId_,
        const CIMException & cimException_,
        QueueIdStack queueIds_)
    : CIMResponseMessage(
        CIM_DISABLE_INDICATIONS_RESPONSE_MESSAGE,
        messageId_,
        cimException_,
        queueIds_)
    {
    }
};

class PEGASUS_COMMON_LINKAGE CIMDisableModuleResponseMessage
    : public CIMResponseMessage
{
public:
    CIMDisableModuleResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<Uint16>& operationalStatus_)
    : CIMResponseMessage(CIM_DISABLE_MODULE_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        operationalStatus(operationalStatus_)
    {
    }

    Array<Uint16> operationalStatus;
};

class PEGASUS_COMMON_LINKAGE CIMEnableModuleResponseMessage
    : public CIMResponseMessage
{
public:
    CIMEnableModuleResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<Uint16>& operationalStatus_)
    : CIMResponseMessage(CIM_ENABLE_MODULE_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        operationalStatus(operationalStatus_)
    {
    }

    Array<Uint16> operationalStatus;
};

class PEGASUS_COMMON_LINKAGE CIMStopAllProvidersResponseMessage
   : public CIMResponseMessage
{
   public:

      CIMStopAllProvidersResponseMessage(
         const String& messageId_,
         const CIMException& cimException_,
         const QueueIdStack& queueIds_)
         :
         CIMResponseMessage(CIM_STOP_ALL_PROVIDERS_RESPONSE_MESSAGE,
                            messageId_, cimException_, queueIds_)
      {
      }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMMessage_h */
