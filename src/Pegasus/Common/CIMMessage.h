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
//              Adrian Schuur (schuur@de.ibm.com)
//              Seema Gupta (gseema@in.ibm.com for PEP135
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
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/ArrayInternal.h>

/*   ProviderType should become part of Pegasus/Common     PEP# 99
   #include <Pegasus/ProviderManager2/ProviderType.h>
   #define TYPE_INSTANCE    ProviderType::INSTANCE
   #define TYPE_CLASS       ProviderType::CLASS
   #define TYPE_METHOD      ProviderType::METHOD
   #define TYPE_ASSOCIATION ProviderType::ASSOCIATION
   #define TYPE_QUERY       ProviderType::QUERY
*/ 
// using these equations instead      PEP# 99
   #define TYPE_CLASS       1
   #define TYPE_INSTANCE    2
   #define TYPE_ASSOCIATION 3
   #define TYPE_METHOD      5
   #define TYPE_QUERY       7


PEGASUS_NAMESPACE_BEGIN

// l10n - added AcceptLanguages and ContentLanguages below

class PEGASUS_COMMON_LINKAGE CIMMessage : public Message
{
public:

    CIMMessage(Uint32 type, const String& messageId_);

#ifdef PEGASUS_DEBUG
    virtual void print(PEGASUS_STD(ostream)& os, Boolean printHeader) const
    {
	if (printHeader)
	{
	    os << "CIMMessage\n";
	    os << "{";
	}

	Message::print(os, false);

	os << "    messageId=" << messageId << PEGASUS_STD(endl);

	if (printHeader)
	{
	    os << "}";
	}
    }
#endif

    String messageId;
	OperationContext operationContext;
};


class CIMResponseMessage;

class PEGASUS_COMMON_LINKAGE CIMRequestMessage : public CIMMessage
{
public:
    CIMRequestMessage(
        Uint32 type_, const String& messageId_, const QueueIdStack& queueIds_);

    virtual CIMResponseMessage* buildResponse() = 0;

#ifdef PEGASUS_DEBUG
    virtual void print(PEGASUS_STD(ostream)& os, Boolean printHeader) const
    {
	if (printHeader)
	{
	    os << "CIMRequestMessage\n";
	    os << "{";
	}

	CIMMessage::print(os, false);

	os << "    queueIds=" << "<not shown for now>" << PEGASUS_STD(endl);

	if (printHeader)
	{
	    os << "}";
	}
    }
#endif

    QueueIdStack queueIds;
    Boolean requestIsOOP;
};


class PEGASUS_COMMON_LINKAGE CIMResponseMessage : public CIMMessage
{
public:

    CIMResponseMessage(
        Uint32 type_,
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_);

    void syncAttributes(CIMRequestMessage* request);

    QueueIdStack queueIds;
    CIMException cimException;
};

//
// CIMRequestMessages
//
class PEGASUS_COMMON_LINKAGE CIMOperationRequestMessage
    : public CIMRequestMessage
{	// PEP# 99
public:

    CIMOperationRequestMessage(
        Uint32 type_,
	const String& messageId_,
	const QueueIdStack& queueIds_,
        const CIMNamespaceName& nameSpace_,
	const CIMName& className_,
	Uint32 providerType_ = TYPE_INSTANCE);

    CIMNamespaceName nameSpace;
    CIMName className;
    Uint32 providerType;
};

class PEGASUS_COMMON_LINKAGE CIMIndicationRequestMessage
    : public CIMRequestMessage
{	// PEP# 99
public:
    CIMIndicationRequestMessage(
        Uint32 type_,
        const String & messageId_,
        const QueueIdStack& queueIds_)
	: 
	CIMRequestMessage(type_, messageId_, queueIds_)
    {
    }
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
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(CIM_GET_CLASS_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,className_,
	 TYPE_CLASS),
        localOnly(localOnly_),
        includeQualifiers(includeQualifiers_),
        includeClassOrigin(includeClassOrigin_),
        propertyList(propertyList_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    CIMPropertyList propertyList;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMGetInstanceRequestMessage
    : public CIMOperationRequestMessage
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
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_GET_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,instanceName_.getClassName()),
	instanceName(instanceName_),
        localOnly(localOnly_),
        includeQualifiers(includeQualifiers_),
        includeClassOrigin(includeClassOrigin_),
        propertyList(propertyList_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMObjectPath instanceName;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    CIMPropertyList propertyList;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMExportIndicationRequestMessage
    : public CIMRequestMessage
{
public:
    CIMExportIndicationRequestMessage(
        const String& messageId_,
        const String& destinationPath_,
        const CIMInstance& indicationInstance_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMRequestMessage(
        CIM_EXPORT_INDICATION_REQUEST_MESSAGE, messageId_, queueIds_),
        destinationPath(destinationPath_),
        indicationInstance(indicationInstance_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    String destinationPath;
    CIMInstance indicationInstance;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMDeleteClassRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMDeleteClassRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& className_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_DELETE_CLASS_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,className_,
	 TYPE_CLASS),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMDeleteInstanceRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMDeleteInstanceRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& instanceName_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_DELETE_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,instanceName_.getClassName()),
        instanceName(instanceName_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMObjectPath instanceName;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMCreateClassRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMCreateClassRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMClass& newClass_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_CREATE_CLASS_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,newClass_.getClassName(),
	 TYPE_CLASS),
        newClass(newClass_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMClass newClass;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMCreateInstanceRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMCreateInstanceRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMInstance& newInstance_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_CREATE_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,newInstance_.getClassName()),
        newInstance(newInstance_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMInstance newInstance;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMModifyClassRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMModifyClassRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMClass& modifiedClass_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_MODIFY_CLASS_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,modifiedClass_.getClassName(),
	 TYPE_CLASS),
        modifiedClass(modifiedClass_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMClass modifiedClass;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMModifyInstanceRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMModifyInstanceRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMInstance& modifiedInstance_,
        Boolean includeQualifiers_,
        const CIMPropertyList& propertyList_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_MODIFY_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,modifiedInstance_.getClassName()),
        modifiedInstance(modifiedInstance_),
        includeQualifiers(includeQualifiers_),
        propertyList(propertyList_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMInstance modifiedInstance;
    Boolean includeQualifiers;
    CIMPropertyList propertyList;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMEnumerateClassesRequestMessage
    : public CIMOperationRequestMessage
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
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,className_,
	 TYPE_CLASS),
        deepInheritance(deepInheritance_),
        localOnly(localOnly_),
        includeQualifiers(includeQualifiers_),
        includeClassOrigin(includeClassOrigin_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    Boolean deepInheritance;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMEnumerateClassNamesRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMEnumerateClassNamesRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& className_,
        Boolean deepInheritance_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,className_,
	 TYPE_CLASS),
        deepInheritance(deepInheritance_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    Boolean deepInheritance;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMEnumerateInstancesRequestMessage
    : public CIMOperationRequestMessage
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
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,className_),
        deepInheritance(deepInheritance_),
        localOnly(localOnly_),
        includeQualifiers(includeQualifiers_),
        includeClassOrigin(includeClassOrigin_),
        propertyList(propertyList_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    Boolean deepInheritance;
    Boolean localOnly;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    CIMPropertyList propertyList;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMEnumerateInstanceNamesRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMEnumerateInstanceNamesRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& className_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,className_),
        authType(authType_),
        userName(userName_)
    {
    }

#ifdef PEGASUS_DEBUG
    virtual void print(PEGASUS_STD(ostream)& os, Boolean printHeader) const
    {
	if (printHeader)
	{
	    os << "CIMEnumerateInstanceNamesRequestMessage\n";
	    os << "{";
	}

	CIMRequestMessage::print(os, false);

	os << "    nameSpace=" << nameSpace << PEGASUS_STD(endl);
	os << "    className=" << className << PEGASUS_STD(endl);
	os << "    authType=" << authType << PEGASUS_STD(endl);
	os << "    userName=" << userName << PEGASUS_STD(endl);

	if (printHeader)
	{
	    os << "}";
	}
    }
#endif

    virtual CIMResponseMessage* buildResponse();

    String authType;
    String userName;
};


class PEGASUS_COMMON_LINKAGE CIMExecQueryRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMExecQueryRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const String& queryLanguage_,
        const String& query_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(CIM_EXEC_QUERY_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,CIMName(),
	 TYPE_QUERY),
        queryLanguage(queryLanguage_),
        query(query_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    String queryLanguage;
    String query;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMAssociatorsRequestMessage
    : public CIMOperationRequestMessage
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
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ASSOCIATORS_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,objectName_.getClassName(),
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

    virtual CIMResponseMessage* buildResponse();

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

class PEGASUS_COMMON_LINKAGE CIMAssociatorNamesRequestMessage
    : public CIMOperationRequestMessage
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
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,objectName_.getClassName(),
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

    virtual CIMResponseMessage* buildResponse();

    CIMObjectPath objectName;
    CIMName assocClass;
    CIMName resultClass;
    String role;
    String resultRole;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMReferencesRequestMessage
    : public CIMOperationRequestMessage
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
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
         CIM_REFERENCES_REQUEST_MESSAGE, messageId_, queueIds_,
         nameSpace_,objectName_.getClassName(),
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

    virtual CIMResponseMessage* buildResponse();

    CIMObjectPath objectName;
    CIMName resultClass;
    String role;
    Boolean includeQualifiers;
    Boolean includeClassOrigin;
    CIMPropertyList propertyList;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMReferenceNamesRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMReferenceNamesRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& objectName_,
        const CIMName& resultClass_,
        const String& role_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_REFERENCE_NAMES_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,objectName_.getClassName(),
	 TYPE_ASSOCIATION),
        objectName(objectName_),
        resultClass(resultClass_),
        role(role_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMObjectPath objectName;
    CIMName resultClass;
    String role;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMGetPropertyRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMGetPropertyRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& instanceName_,
        const CIMName& propertyName_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_GET_PROPERTY_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,instanceName_.getClassName()),
        instanceName(instanceName_),
        propertyName(propertyName_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMObjectPath instanceName;
    CIMName propertyName;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMSetPropertyRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMSetPropertyRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& instanceName_,
        const CIMName& propertyName_,
        const CIMValue& newValue_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_SET_PROPERTY_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,instanceName_.getClassName()),
        instanceName(instanceName_),
        propertyName(propertyName_),
        newValue(newValue_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMObjectPath instanceName;
    CIMName propertyName;
    CIMValue newValue;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMGetQualifierRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMGetQualifierRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& qualifierName_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_GET_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,CIMName(),
	 TYPE_CLASS),
        qualifierName(qualifierName_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMName qualifierName;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMSetQualifierRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMSetQualifierRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMQualifierDecl& qualifierDeclaration_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_SET_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,CIMName(),
	 TYPE_CLASS),
        qualifierDeclaration(qualifierDeclaration_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMQualifierDecl qualifierDeclaration;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMDeleteQualifierRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMDeleteQualifierRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMName& qualifierName_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_DELETE_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,CIMName(),
	 TYPE_CLASS),
        qualifierName(qualifierName_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMName qualifierName;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMEnumerateQualifiersRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMEnumerateQualifiersRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,CIMName(),
	 TYPE_CLASS),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMInvokeMethodRequestMessage
    : public CIMOperationRequestMessage
{
public:
    CIMInvokeMethodRequestMessage(
        const String& messageId_,
        const CIMNamespaceName& nameSpace_,
        const CIMObjectPath& instanceName_,
        const CIMName& methodName_,
        const Array<CIMParamValue>& inParameters_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMOperationRequestMessage(
        CIM_INVOKE_METHOD_REQUEST_MESSAGE, messageId_, queueIds_,
	 nameSpace_,instanceName_.getClassName(),
	 TYPE_METHOD),
        instanceName(instanceName_),
        methodName(methodName_),
        inParameters(inParameters_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMObjectPath instanceName;
    CIMName methodName;
    Array<CIMParamValue> inParameters;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMProcessIndicationRequestMessage
    : public CIMRequestMessage
{
public:
    CIMProcessIndicationRequestMessage(
        const String & messageId_,
        const CIMNamespaceName & nameSpace_,
        const CIMInstance& indicationInstance_,
        const Array<CIMObjectPath> & subscriptionInstanceNames_,
        const CIMInstance & provider_,
        const QueueIdStack& queueIds_)
    : CIMRequestMessage(
        CIM_PROCESS_INDICATION_REQUEST_MESSAGE, messageId_, queueIds_),
        nameSpace (nameSpace_),
        indicationInstance(indicationInstance_),
        subscriptionInstanceNames(subscriptionInstanceNames_),
        provider(provider_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMNamespaceName nameSpace;
    CIMInstance indicationInstance;
    Array<CIMObjectPath> subscriptionInstanceNames;
    CIMInstance provider;
};

class PEGASUS_COMMON_LINKAGE CIMNotifyProviderRegistrationRequestMessage
    : public CIMRequestMessage
{
public:
    enum Operation
    {
        OP_CREATE = 1, OP_DELETE = 2, OP_MODIFY = 3
    };

    CIMNotifyProviderRegistrationRequestMessage(
        const String & messageId_,
        const Operation operation_,
        const CIMName & className_,
        const Array <CIMNamespaceName> & newNamespaces_,
        const Array <CIMNamespaceName> & oldNamespaces_,
        const CIMPropertyList & newPropertyNames_,
        const CIMPropertyList & oldPropertyNames_,
        const QueueIdStack& queueIds_)
    : CIMRequestMessage(
        CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE,
        messageId_, queueIds_),
        className (className_),
        newNamespaces (newNamespaces_),
        oldNamespaces (oldNamespaces_),
        newPropertyNames (newPropertyNames_),
        oldPropertyNames (oldPropertyNames_),
        operation(operation_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMName className;
    Array <CIMNamespaceName> newNamespaces;
    Array <CIMNamespaceName> oldNamespaces;
    CIMPropertyList newPropertyNames;
    CIMPropertyList oldPropertyNames;
    Operation operation;
};

class PEGASUS_COMMON_LINKAGE CIMNotifyProviderTerminationRequestMessage
    : public CIMRequestMessage
{
public:
    CIMNotifyProviderTerminationRequestMessage(
        const String & messageId_,
        const Array <CIMInstance> & providers_,
        const QueueIdStack& queueIds_)
    : CIMRequestMessage(
        CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE,
        messageId_, queueIds_),
        providers (providers_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    Array <CIMInstance> providers;
};

class PEGASUS_COMMON_LINKAGE CIMHandleIndicationRequestMessage
    : public CIMRequestMessage
{
public:
    CIMHandleIndicationRequestMessage(
        const String& messageId_,
        const CIMNamespaceName & nameSpace_,
        const CIMInstance& handlerInstance_,
        const CIMInstance& indicationInstance_,
	const CIMInstance& subscriptionInstance_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMRequestMessage(
        CIM_HANDLE_INDICATION_REQUEST_MESSAGE, messageId_, queueIds_),
        nameSpace(nameSpace_),
        handlerInstance(handlerInstance_),
        indicationInstance(indicationInstance_),
	subscriptionInstance(subscriptionInstance_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMNamespaceName nameSpace;
    CIMInstance handlerInstance;
    CIMInstance indicationInstance;
    CIMInstance subscriptionInstance;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMCreateSubscriptionRequestMessage
    : public CIMIndicationRequestMessage
{
public:
    CIMCreateSubscriptionRequestMessage(
        const String& messageId_,
        const CIMNamespaceName & nameSpace_,
        const CIMInstance & subscriptionInstance_,
        const Array<CIMName> & classNames_,
        const CIMPropertyList & propertyList_,
        const Uint16 repeatNotificationPolicy_,
        const String & query_,
        const QueueIdStack& queueIds_,
        const String & authType_ = String::EMPTY,
        const String & userName_ = String::EMPTY)
    : CIMIndicationRequestMessage(
        CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE,
        messageId_,
        queueIds_),
        nameSpace (nameSpace_),
        subscriptionInstance(subscriptionInstance_),
        classNames(classNames_),
        propertyList (propertyList_),
        repeatNotificationPolicy (repeatNotificationPolicy_),
        query (query_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMNamespaceName nameSpace;
    CIMInstance subscriptionInstance;
    Array <CIMName> classNames;
    CIMPropertyList propertyList;
    Uint16 repeatNotificationPolicy;
    String query;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMModifySubscriptionRequestMessage
    : public CIMIndicationRequestMessage
{
public:
    CIMModifySubscriptionRequestMessage(
        const String& messageId_,
        const CIMNamespaceName & nameSpace_,
        const CIMInstance & subscriptionInstance_,
        const Array<CIMName> & classNames_,
        const CIMPropertyList & propertyList_,
        const Uint16 repeatNotificationPolicy_,
        const String & query_,
        const QueueIdStack& queueIds_,
        const String & authType_ = String::EMPTY,
        const String & userName_ = String::EMPTY)
    : CIMIndicationRequestMessage(
        CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE,
        messageId_,
        queueIds_),
        nameSpace(nameSpace_),
        subscriptionInstance(subscriptionInstance_),
        classNames(classNames_),
        propertyList (propertyList_),
        repeatNotificationPolicy (repeatNotificationPolicy_),
        query (query_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMNamespaceName nameSpace;
    CIMInstance subscriptionInstance;
    Array<CIMName> classNames;
    CIMPropertyList propertyList;
    Uint16 repeatNotificationPolicy;
    String query;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMDeleteSubscriptionRequestMessage
    : public CIMIndicationRequestMessage
{
public:
    CIMDeleteSubscriptionRequestMessage(
        const String& messageId_,
        const CIMNamespaceName & nameSpace_,
        const CIMInstance & subscriptionInstance_,
        const Array<CIMName> & classNames_,
        const QueueIdStack& queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
    : CIMIndicationRequestMessage(
        CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE,
        messageId_,
        queueIds_),
        nameSpace(nameSpace_),
        subscriptionInstance(subscriptionInstance_),
        classNames(classNames_),
        authType(authType_),
        userName(userName_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    CIMNamespaceName nameSpace;
    CIMInstance subscriptionInstance;
    Array<CIMName> classNames;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE 
    CIMSubscriptionInitCompleteRequestMessage
    : public CIMRequestMessage
{
public:
    CIMSubscriptionInitCompleteRequestMessage(
        const String & messageId_,
        const QueueIdStack & queueIds_)
    : CIMRequestMessage
       (CIM_SUBSCRIPTION_INIT_COMPLETE_REQUEST_MESSAGE,
        messageId_,
        queueIds_)
    {
    }

    virtual CIMResponseMessage * buildResponse ();
};

class PEGASUS_COMMON_LINKAGE CIMDisableModuleRequestMessage
    : public CIMRequestMessage
{
public:
    CIMDisableModuleRequestMessage(
        const String& messageId_,
        const CIMInstance& providerModule_,
        const Array<CIMInstance>& providers_,
	Boolean disableProviderOnly_,
	const Array<Boolean>& indicationProviders_,
        const QueueIdStack& queueIds_,
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

    virtual CIMResponseMessage* buildResponse();

    CIMInstance providerModule;
    Array<CIMInstance> providers;
    Boolean disableProviderOnly;
    Array<Boolean> indicationProviders;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMEnableModuleRequestMessage
    : public CIMRequestMessage
{
public:
    CIMEnableModuleRequestMessage(
        const String& messageId_,
        const CIMInstance& providerModule_,
        const QueueIdStack& queueIds_,
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

    virtual CIMResponseMessage* buildResponse();

    CIMInstance providerModule;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMNotifyProviderEnableRequestMessage
    : public CIMRequestMessage
{
public:
    CIMNotifyProviderEnableRequestMessage(
	const String & messageId_,
	const Array <CIMInstance> & capInstances_,
        const QueueIdStack& queueIds_)
    : CIMRequestMessage(
	CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE,
	messageId_,
        queueIds_),
	capInstances(capInstances_)
    {
    }

    virtual CIMResponseMessage* buildResponse();
    
    Array <CIMInstance> capInstances;
};

class PEGASUS_COMMON_LINKAGE CIMStopAllProvidersRequestMessage
    : public CIMRequestMessage
{
public:
    CIMStopAllProvidersRequestMessage(
        const String& messageId_,
        const QueueIdStack& queueIds_)
    : CIMRequestMessage(
        CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE,
        messageId_,
        queueIds_)
    {
    }

    virtual CIMResponseMessage* buildResponse();
};

class PEGASUS_COMMON_LINKAGE CIMInitializeProviderRequestMessage
    : public CIMRequestMessage
{
public:
    CIMInitializeProviderRequestMessage(
	const String & messageId_,
        const QueueIdStack& queueIds_)
    : CIMRequestMessage(
	CIM_INITIALIZE_PROVIDER_REQUEST_MESSAGE,
	messageId_,
        queueIds_)
    {
    }

    virtual CIMResponseMessage* buildResponse();
};

// Used to pass initialization data to an Out-of-Process Provider Agent process
class PEGASUS_COMMON_LINKAGE CIMInitializeProviderAgentRequestMessage
    : public CIMRequestMessage
{
public:
    CIMInitializeProviderAgentRequestMessage(
	const String & messageId_,
        const String& pegasusHome_,
        const Array<Pair<String, String> >& configProperties_,
        Boolean bindVerbose_,
        Boolean subscriptionInitComplete_,
        const QueueIdStack& queueIds_)
    : CIMRequestMessage(
	CIM_INITIALIZE_PROVIDER_AGENT_REQUEST_MESSAGE,
	messageId_,
        queueIds_),
      pegasusHome(pegasusHome_),
      configProperties(configProperties_),
      bindVerbose(bindVerbose_),
      subscriptionInitComplete(subscriptionInitComplete_)
    {
    }

    virtual CIMResponseMessage* buildResponse();

    String pegasusHome;
    Array<Pair<String, String> > configProperties;
    Boolean bindVerbose;
    Boolean subscriptionInitComplete;
};

class PEGASUS_COMMON_LINKAGE CIMNotifyConfigChangeRequestMessage
    : public CIMRequestMessage
{
public:
    CIMNotifyConfigChangeRequestMessage(
        const String & messageId_,
        const String & propertyName_,
        const String & newPropertyValue_,
        Boolean currentValueModified_, // false - planned value modified
        const QueueIdStack& queueIds_)
    : CIMRequestMessage(
        CIM_NOTIFY_CONFIG_CHANGE_REQUEST_MESSAGE,
        messageId_,
        queueIds_),
        propertyName(propertyName_),
        newPropertyValue(newPropertyValue_),
        currentValueModified(currentValueModified_)
    {
    }

    virtual CIMResponseMessage* buildResponse();
    
    String propertyName;
    String newPropertyValue;
    Boolean currentValueModified;
};


//
// CIMResponseMessages
//

class PEGASUS_COMMON_LINKAGE CIMGetClassResponseMessage
    : public CIMResponseMessage
{
public:
    CIMGetClassResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMClass& cimClass_)
    : CIMResponseMessage(CIM_GET_CLASS_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        cimClass(cimClass_)
    {
    }

    CIMClass cimClass;
};

class PEGASUS_COMMON_LINKAGE CIMGetInstanceResponseMessage
    : public CIMResponseMessage
{
public:
    CIMGetInstanceResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMInstance& cimInstance_)
    : CIMResponseMessage(CIM_GET_INSTANCE_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        cimInstance(cimInstance_)
    {
    }

    CIMInstance cimInstance;
};

class PEGASUS_COMMON_LINKAGE CIMExportIndicationResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMDeleteClassResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMDeleteInstanceResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMCreateClassResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMCreateInstanceResponseMessage
    : public CIMResponseMessage
{
public:
    CIMCreateInstanceResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMObjectPath& instanceName_)
    : CIMResponseMessage(CIM_CREATE_INSTANCE_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        instanceName(instanceName_)
    {
    }

    CIMObjectPath instanceName;
};

class PEGASUS_COMMON_LINKAGE CIMModifyClassResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMModifyInstanceResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMEnumerateClassesResponseMessage
    : public CIMResponseMessage
{
public:
    CIMEnumerateClassesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMClass>& cimClasses_)
    : CIMResponseMessage(CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        cimClasses(cimClasses_)
    {
    }

    Array<CIMClass> cimClasses;
};

class PEGASUS_COMMON_LINKAGE CIMEnumerateClassNamesResponseMessage
    : public CIMResponseMessage
{
public:
    CIMEnumerateClassNamesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMName>& classNames_)
    : CIMResponseMessage(CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        classNames(classNames_)
    {
    }

    Array<CIMName> classNames;
};

class PEGASUS_COMMON_LINKAGE CIMEnumerateInstancesResponseMessage
    : public CIMResponseMessage
{
public:
    CIMEnumerateInstancesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMInstance>& cimNamedInstances_)
    : CIMResponseMessage(CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        cimNamedInstances(cimNamedInstances_)
    {
    }

    Array<CIMInstance> cimNamedInstances;
};

class PEGASUS_COMMON_LINKAGE CIMEnumerateInstanceNamesResponseMessage
    : public CIMResponseMessage
{
public:
    CIMEnumerateInstanceNamesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObjectPath>& instanceNames_)
    : CIMResponseMessage(CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        instanceNames(instanceNames_)
    {
    }

    Array<CIMObjectPath> instanceNames;
};

class PEGASUS_COMMON_LINKAGE CIMExecQueryResponseMessage
    : public CIMResponseMessage
{
public:
    CIMExecQueryResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObject>& cimObjects_)
    : CIMResponseMessage(CIM_EXEC_QUERY_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        cimObjects(cimObjects_)
    {
    }

    Array<CIMObject> cimObjects;
};

class PEGASUS_COMMON_LINKAGE CIMAssociatorsResponseMessage
    : public CIMResponseMessage
{
public:
    CIMAssociatorsResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObject>& cimObjects_)
    : CIMResponseMessage(CIM_ASSOCIATORS_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        cimObjects(cimObjects_)
    {
    }

    Array<CIMObject> cimObjects;
};

class PEGASUS_COMMON_LINKAGE CIMAssociatorNamesResponseMessage
    : public CIMResponseMessage
{
public:
    CIMAssociatorNamesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObjectPath>& objectNames_)
    : CIMResponseMessage(CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        objectNames(objectNames_)
    {
    }

    Array<CIMObjectPath> objectNames;
};

class PEGASUS_COMMON_LINKAGE CIMReferencesResponseMessage
    : public CIMResponseMessage
{
public:
    CIMReferencesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObject>& cimObjects_)
    : CIMResponseMessage(CIM_REFERENCES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        cimObjects(cimObjects_)
    {
    }

    Array<CIMObject> cimObjects;
};

class PEGASUS_COMMON_LINKAGE CIMReferenceNamesResponseMessage
    : public CIMResponseMessage
{
public:
    CIMReferenceNamesResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMObjectPath>& objectNames_)
    : CIMResponseMessage(CIM_REFERENCE_NAMES_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        objectNames(objectNames_)
    {
    }

    Array<CIMObjectPath> objectNames;
};

class PEGASUS_COMMON_LINKAGE CIMGetPropertyResponseMessage
    : public CIMResponseMessage
{
public:
    CIMGetPropertyResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMValue& value_)
    : CIMResponseMessage(CIM_GET_PROPERTY_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        value(value_)
    {
    }

    CIMValue value;
};

class PEGASUS_COMMON_LINKAGE CIMSetPropertyResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMGetQualifierResponseMessage
    : public CIMResponseMessage
{
public:
    CIMGetQualifierResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMQualifierDecl& cimQualifierDecl_)
    : CIMResponseMessage(CIM_GET_QUALIFIER_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        cimQualifierDecl(cimQualifierDecl_)
    {
    }

    CIMQualifierDecl cimQualifierDecl;
};

class PEGASUS_COMMON_LINKAGE CIMSetQualifierResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMDeleteQualifierResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMEnumerateQualifiersResponseMessage
    : public CIMResponseMessage
{
public:
    CIMEnumerateQualifiersResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const Array<CIMQualifierDecl>& qualifierDeclarations_)
    : CIMResponseMessage(CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        qualifierDeclarations(qualifierDeclarations_)
    {
    }

    Array<CIMQualifierDecl> qualifierDeclarations;
};

class PEGASUS_COMMON_LINKAGE CIMInvokeMethodResponseMessage
    : public CIMResponseMessage
{
public:
    CIMInvokeMethodResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_,
        const CIMValue& retValue_,
        const Array<CIMParamValue>& outParameters_,
        const CIMName& methodName_)
    : CIMResponseMessage(CIM_INVOKE_METHOD_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_),
        retValue(retValue_),
        outParameters(outParameters_),
        methodName(methodName_)
    {
    }

    CIMValue retValue;
    Array<CIMParamValue> outParameters;
    CIMName methodName;
};

class PEGASUS_COMMON_LINKAGE CIMProcessIndicationResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMNotifyProviderRegistrationResponseMessage
    : public CIMResponseMessage
{
public:
    CIMNotifyProviderRegistrationResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_NOTIFY_PROVIDER_REGISTRATION_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class PEGASUS_COMMON_LINKAGE CIMNotifyProviderTerminationResponseMessage
    : public CIMResponseMessage
{
public:
    CIMNotifyProviderTerminationResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_NOTIFY_PROVIDER_TERMINATION_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class PEGASUS_COMMON_LINKAGE CIMHandleIndicationResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMCreateSubscriptionResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMModifySubscriptionResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE CIMDeleteSubscriptionResponseMessage
    : public CIMResponseMessage
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

class PEGASUS_COMMON_LINKAGE 
CIMSubscriptionInitCompleteResponseMessage
    : public CIMResponseMessage
{
public:
    CIMSubscriptionInitCompleteResponseMessage
       (const String & messageId_,
        const CIMException & cimException_,
        const QueueIdStack & queueIds_)
    : CIMResponseMessage
       (CIM_SUBSCRIPTION_INIT_COMPLETE_RESPONSE_MESSAGE,
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

class PEGASUS_COMMON_LINKAGE CIMNotifyProviderEnableResponseMessage
    : public CIMResponseMessage
{
public:
    CIMNotifyProviderEnableResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_NOTIFY_PROVIDER_ENABLE_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class PEGASUS_COMMON_LINKAGE CIMStopAllProvidersResponseMessage
    : public CIMResponseMessage
{
public:
    CIMStopAllProvidersResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_STOP_ALL_PROVIDERS_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class PEGASUS_COMMON_LINKAGE CIMInitializeProviderResponseMessage
    : public CIMResponseMessage
{
public:
    CIMInitializeProviderResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_INITIALIZE_PROVIDER_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class PEGASUS_COMMON_LINKAGE CIMInitializeProviderAgentResponseMessage
    : public CIMResponseMessage
{
public:
    CIMInitializeProviderAgentResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_INITIALIZE_PROVIDER_AGENT_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

class PEGASUS_COMMON_LINKAGE CIMNotifyConfigChangeResponseMessage
    : public CIMResponseMessage
{
public:
    CIMNotifyConfigChangeResponseMessage(
        const String& messageId_,
        const CIMException& cimException_,
        const QueueIdStack& queueIds_)
    : CIMResponseMessage(CIM_NOTIFY_CONFIG_CHANGE_RESPONSE_MESSAGE,
        messageId_, cimException_, queueIds_)
    {
    }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMMessage_h */
