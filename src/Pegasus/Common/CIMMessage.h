//%////-*-c++-*-////////////////////////////////////////////////////////////////
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
// Modified By: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
//
// Modified By: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Sushma Fernandes, Hewlett-Packard Company
//              (sushma_fernandes@hp.com)
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
#include <Pegasus/Common/CIMNamedInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE CIMMessage : public Message
{
   public:
      typedef Message Base;

      CIMMessage(Uint32 type, const String& messageId_)
	 : Message(type), messageId(messageId_) { }


      CIMMessage(CIMMessage & msg)
	 : Base(msg)
      {
	 if( this != &msg)
	 {
	    const_cast<String &>(messageId) = msg.messageId;
	 }
      }


      virtual ~CIMMessage();

      const String messageId;
};

class PEGASUS_COMMON_LINKAGE CIMRequestMessage : public CIMMessage
{
   public:

      typedef CIMMessage Base;

      CIMRequestMessage(
	 Uint32 type_, const String& messageId_, QueueIdStack queueIds_)
	 : CIMMessage(type_, messageId_), queueIds(queueIds_) { }


      CIMRequestMessage(CIMRequestMessage & msg)
	 : Base(msg)
      {
	 if(this != &msg)
	 {
	    queueIds = msg.queueIds;
	 }
      }

      virtual ~CIMRequestMessage();

      QueueIdStack queueIds;
};

class PEGASUS_COMMON_LINKAGE CIMResponseMessage : public CIMMessage
{
   public:

      typedef CIMMessage Base;

      CIMResponseMessage(
	 Uint32 type_,
	 const String& messageId_,
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_)
	 :
	 CIMMessage(type_, messageId_),
	 queueIds(queueIds_),
	 cimException(cimException_)
      {
      }


      CIMResponseMessage(CIMResponseMessage & msg)
	 : Base( msg )
      {
	 if(this != &msg)
	 {
	    queueIds = msg.queueIds;
	    cimException = msg.cimException;
	 }
      }

      virtual ~CIMResponseMessage();

      QueueIdStack queueIds;
      CIMException cimException;
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
	 const CIMPropertyList& propertyList_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(CIM_GET_CLASS_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 className(className_),
	 localOnly(localOnly_),
	 includeQualifiers(includeQualifiers_),
	 includeClassOrigin(includeClassOrigin_),
	 propertyList(propertyList_),
	 authType(authType_),
	 userName(userName_)
      {
      }

      virtual ~CIMGetClassRequestMessage();

      String nameSpace;
      String className;
      Boolean localOnly;
      Boolean includeQualifiers;
      Boolean includeClassOrigin;
      CIMPropertyList propertyList;
      String authType;
      String userName;
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
	 const CIMPropertyList& propertyList_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_GET_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 instanceName(instanceName_),
	 localOnly(localOnly_),
	 includeQualifiers(includeQualifiers_),
	 includeClassOrigin(includeClassOrigin_),
	 propertyList(propertyList_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      CIMReference instanceName;
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
	 const String& url_,
	 const CIMInstance& indicationInstance_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_EXPORT_INDICATION_REQUEST_MESSAGE, messageId_, queueIds_),
	 url(url_),
	 indicationInstance(indicationInstance_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String url;
      CIMInstance indicationInstance;
      String authType;
      String userName;
};

class CIMDeleteClassRequestMessage : public CIMRequestMessage
{
   public:

      CIMDeleteClassRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const String& className_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_DELETE_CLASS_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 className(className_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      String className;
      String authType;
      String userName;
};

class CIMDeleteInstanceRequestMessage : public CIMRequestMessage
{
   public:

      CIMDeleteInstanceRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const CIMReference& instanceName_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_DELETE_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 instanceName(instanceName_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      CIMReference instanceName;
      String authType;
      String userName;
};

class CIMCreateClassRequestMessage : public CIMRequestMessage
{
   public:

      CIMCreateClassRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const CIMClass& newClass_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_CREATE_CLASS_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 newClass(newClass_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      CIMClass newClass;
      String authType;
      String userName;
};

class CIMCreateInstanceRequestMessage : public CIMRequestMessage
{
   public:

      CIMCreateInstanceRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const CIMInstance& newInstance_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_CREATE_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 newInstance(newInstance_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      CIMInstance newInstance;
      String authType;
      String userName;
};

class CIMModifyClassRequestMessage : public CIMRequestMessage
{
   public:

      CIMModifyClassRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const CIMClass& modifiedClass_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_MODIFY_CLASS_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 modifiedClass(modifiedClass_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      CIMClass modifiedClass;
      String authType;
      String userName;
};

class CIMModifyInstanceRequestMessage : public CIMRequestMessage
{
   public:

      CIMModifyInstanceRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const CIMNamedInstance& modifiedInstance_,
	 Boolean includeQualifiers_,
	 const CIMPropertyList& propertyList_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_MODIFY_INSTANCE_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 modifiedInstance(modifiedInstance_),
	 includeQualifiers(includeQualifiers_),
	 propertyList(propertyList_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      CIMNamedInstance modifiedInstance;
      Boolean includeQualifiers;
      CIMPropertyList propertyList;
      String authType;
      String userName;
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
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 className(className_),
	 deepInheritance(deepInheritance_),
	 localOnly(localOnly_),
	 includeQualifiers(includeQualifiers_),
	 includeClassOrigin(includeClassOrigin_),
	 authType(authType_),
	 userName(userName_)
      {
      }

      String nameSpace;
      String className;
      Boolean deepInheritance;
      Boolean localOnly;
      Boolean includeQualifiers;
      Boolean includeClassOrigin;
      String authType;
      String userName;
};

class CIMEnumerateClassNamesRequestMessage : public CIMRequestMessage
{
   public:

      CIMEnumerateClassNamesRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const String& className_,
	 Boolean deepInheritance_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 className(className_),
	 deepInheritance(deepInheritance_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      String className;
      Boolean deepInheritance;
      String authType;
      String userName;
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
	 const CIMPropertyList& propertyList_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 className(className_),
	 deepInheritance(deepInheritance_),
	 localOnly(localOnly_),
	 includeQualifiers(includeQualifiers_),
	 includeClassOrigin(includeClassOrigin_),
	 propertyList(propertyList_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      String className;
      Boolean deepInheritance;
      Boolean localOnly;
      Boolean includeQualifiers;
      Boolean includeClassOrigin;
      CIMPropertyList propertyList;
      String authType;
      String userName;
};

class CIMEnumerateInstanceNamesRequestMessage : public CIMRequestMessage
{
   public:

      CIMEnumerateInstanceNamesRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const String& className_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 className(className_),
	 authType(authType_),
	 userName(userName_)
      {
      }

      String nameSpace;
      String className;
      String authType;
      String userName;
};

class CIMExecQueryRequestMessage : public CIMRequestMessage
{
   public:

      CIMExecQueryRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const String& queryLanguage_,
	 const String& query_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(CIM_EXEC_QUERY_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 queryLanguage(queryLanguage_),
	 query(query_),
	 authType(authType_),
	 userName(userName_)
      {
      }

      String nameSpace;
      String queryLanguage;
      String query;
      String authType;
      String userName;
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
	 const CIMPropertyList& propertyList_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
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
	 propertyList(propertyList_),
	 authType(authType_),
	 userName(userName_)
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
      CIMPropertyList propertyList;
      String authType;
      String userName;
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
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 objectName(objectName_),
	 assocClass(assocClass_),
	 resultClass(resultClass_),
	 role(role_),
	 resultRole(resultRole_),
	 authType(authType_),
	 userName(userName_)
      {
      }

      String nameSpace;
      CIMReference objectName;
      String assocClass;
      String resultClass;
      String role;
      String resultRole;
      String authType;
      String userName;
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
	 const CIMPropertyList& propertyList_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(CIM_REFERENCES_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
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

      String nameSpace;
      CIMReference objectName;
      String resultClass;
      String role;
      Boolean includeQualifiers;
      Boolean includeClassOrigin;
      CIMPropertyList propertyList;
      String authType;
      String userName;
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
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_REFERENCE_NAMES_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 objectName(objectName_),
	 resultClass(resultClass_),
	 role(role_),
	 authType(authType_),
	 userName(userName_)
      {
      }
	
      String nameSpace;
      CIMReference objectName;
      String resultClass;
      String role;
      String authType;
      String userName;
};

class CIMGetPropertyRequestMessage : public CIMRequestMessage
{
   public:

      CIMGetPropertyRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const CIMReference& instanceName_,
	 const String& propertyName_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_GET_PROPERTY_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 instanceName(instanceName_),
	 propertyName(propertyName_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      CIMReference instanceName;
      String propertyName;
      String authType;
      String userName;
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
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_SET_PROPERTY_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 instanceName(instanceName_),
	 propertyName(propertyName_),
	 newValue(newValue_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      CIMReference instanceName;
      String propertyName;
      CIMValue newValue;
      String authType;
      String userName;
};

class CIMGetQualifierRequestMessage : public CIMRequestMessage
{
   public:

      CIMGetQualifierRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const String& qualifierName_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_GET_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 qualifierName(qualifierName_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      const String qualifierName;
      String authType;
      String userName;
};

class CIMSetQualifierRequestMessage : public CIMRequestMessage
{
   public:

      CIMSetQualifierRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const CIMQualifierDecl& qualifierDeclaration_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_SET_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 qualifierDeclaration(qualifierDeclaration_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      const CIMQualifierDecl qualifierDeclaration;
      String authType;
      String userName;
};

class CIMDeleteQualifierRequestMessage : public CIMRequestMessage
{
   public:

      CIMDeleteQualifierRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 const String& qualifierName_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_DELETE_QUALIFIER_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 qualifierName(qualifierName_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      const String qualifierName;
      String authType;
      String userName;
};

class CIMEnumerateQualifiersRequestMessage : public CIMRequestMessage
{
   public:

      CIMEnumerateQualifiersRequestMessage(
	 const String& messageId_,
	 const String& nameSpace_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      String authType;
      String userName;
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
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_INVOKE_METHOD_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 instanceName(instanceName_),
	 methodName(methodName_),
	 inParameters(inParameters_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      CIMReference instanceName;
      String methodName;
      Array<CIMParamValue> inParameters;
      String authType;
      String userName;
};

//
//  ATTN-CAKG-P2-20020326: To Be Removed - once the ProviderManagerService
//  has been modified to use the new messages
//
class CIMEnableIndicationSubscriptionRequestMessage : public CIMRequestMessage
{
   public:

      CIMEnableIndicationSubscriptionRequestMessage(
	 const String & messageId_,
	 const String & nameSpace_,
	 const CIMNamedInstance & subscription_,
	 const Array<String> & classNames_,
	 const CIMInstance & provider_,
	 const CIMInstance & providerModule_,
	 const CIMPropertyList & propertyList_,
	 const Uint16 repeatNotificationPolicy_,
	 const String & condition_,
	 const String & queryLanguage_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY,
	 const String & otherRepeatNotificationPolicy_ = String::EMPTY,
	 const CIMDateTime & repeatNotificationInterval_ = CIMDateTime (),
	 const CIMDateTime & repeatNotificationGap_ = CIMDateTime (),
	 const Uint16 repeatNotificationCount_ = 0)
	 :
	 CIMRequestMessage
      (CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE,
       messageId_,
       queueIds_),
	 nameSpace (nameSpace_),
	 classNames (classNames_),
	 provider (provider_),
	 providerModule (providerModule_),
	 propertyList (propertyList_),
	 repeatNotificationPolicy (repeatNotificationPolicy_),
	 otherRepeatNotificationPolicy (otherRepeatNotificationPolicy_),
	 repeatNotificationInterval (repeatNotificationInterval_),
	 repeatNotificationGap (repeatNotificationGap_),
	 repeatNotificationCount (repeatNotificationCount_),
	 condition (condition_),
	 queryLanguage (queryLanguage_),
	 subscription (subscription_.getInstance ()),
	 authType(authType_),
	 userName(userName_)
      {
      }

      String nameSpace;
      Array<String> classNames;
      CIMInstance provider;
      CIMInstance providerModule;
      CIMPropertyList propertyList;
      Uint16 repeatNotificationPolicy;
      String otherRepeatNotificationPolicy;
      CIMDateTime repeatNotificationInterval;
      CIMDateTime repeatNotificationGap;
      Uint16 repeatNotificationCount;
      String condition;
      String queryLanguage;
      CIMInstance subscription;
      String authType;
      String userName;
};

class CIMEnableIndicationsRequestMessage : public CIMRequestMessage
{
public:
    CIMEnableIndicationsRequestMessage(
	const String & messageId_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
	QueueIdStack queueIds_)
    :
    CIMRequestMessage(
	CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE,
        messageId_,
        queueIds_),
    provider(provider_),
    providerModule(providerModule_)
    {
    }

    CIMInstance provider;
    CIMInstance providerModule;
};

class CIMDisableIndicationsRequestMessage : public CIMRequestMessage
{
public:
    CIMDisableIndicationsRequestMessage(
	const String & messageId_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
	QueueIdStack queueIds_)
    :
    CIMRequestMessage(
	CIM_DISABLE_INDICATIONS_REQUEST_MESSAGE,
        messageId_,
        queueIds_),
    provider(provider_),
    providerModule(providerModule_)

    {
    }

    CIMInstance provider;
    CIMInstance providerModule;
};


//  ATTN-CAKG-P2-20020326: To Be Removed - once the ProviderManagerService
//  has been modified to use the new messages
//
class CIMModifyIndicationSubscriptionRequestMessage : public CIMRequestMessage
{
   public:

      CIMModifyIndicationSubscriptionRequestMessage(
	 const String & messageId_,
	 const String & nameSpace_,
	 const CIMNamedInstance & subscription_,
	 const Array<String> & classNames_,
	 const CIMInstance & provider_,
	 const CIMInstance & providerModule_,
	 const CIMPropertyList & propertyList_,
	 const Uint16 repeatNotificationPolicy_,
	 const String & condition_,
	 const String & queryLanguage_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY,
	 const String & otherRepeatNotificationPolicy_ = String::EMPTY,
	 const CIMDateTime & repeatNotificationInterval_ = CIMDateTime (),
	 const CIMDateTime & repeatNotificationGap_ = CIMDateTime (),
	 const Uint16 repeatNotificationCount_ = 0)
	 :
	 CIMRequestMessage
      (CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE,
       messageId_,
       queueIds_),
	 nameSpace (nameSpace_),
	 classNames (classNames_),
	 provider (provider_),
	 providerModule (providerModule_),
	 propertyList (propertyList_),
	 repeatNotificationPolicy (repeatNotificationPolicy_),
	 otherRepeatNotificationPolicy (otherRepeatNotificationPolicy_),
	 repeatNotificationInterval (repeatNotificationInterval_),
	 repeatNotificationGap (repeatNotificationGap_),
	 repeatNotificationCount (repeatNotificationCount_),
	 condition (condition_),
	 queryLanguage (queryLanguage_),
	 subscription (subscription_.getInstance ()),
	 authType(authType_),
	 userName(userName_)
      {
      }

      String nameSpace;
      Array<String> classNames;
      CIMInstance provider;
      CIMInstance providerModule;
      CIMPropertyList propertyList;
      Uint16 repeatNotificationPolicy;
      String otherRepeatNotificationPolicy;
      CIMDateTime repeatNotificationInterval;
      CIMDateTime repeatNotificationGap;
      Uint16 repeatNotificationCount;
      String condition;
      String queryLanguage;
      CIMInstance subscription;
      String authType;
      String userName;
};

//
//  ATTN-CAKG-P2-20020326: To Be Removed - once the ProviderManagerService
//  has been modified to use the new messages
//
class CIMDisableIndicationSubscriptionRequestMessage : public CIMRequestMessage
{
   public:

      CIMDisableIndicationSubscriptionRequestMessage(
	 const String & messageId_,
	 const String & nameSpace_,
	 const CIMNamedInstance & subscription_,
	 const Array<String> & classNames_,
	 const CIMInstance & provider_,
	 const CIMInstance & providerModule_,
	 QueueIdStack queueIds_,
	 const String& authType_ = String::EMPTY,
	 const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage
      (CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE,
       messageId_,
       queueIds_),
	 nameSpace (nameSpace_),
	 classNames (classNames_),
	 provider (provider_),
	 providerModule (providerModule_),
	 subscription (subscription_.getInstance ()),
	 authType(authType_),
	 userName(userName_)
      {
      }

      String nameSpace;
      Array<String> classNames;
      CIMInstance provider;
      CIMInstance providerModule;
      CIMInstance subscription;
      String authType;
      String userName;
};

class CIMProcessIndicationRequestMessage : public CIMRequestMessage
{
   public:

      CIMProcessIndicationRequestMessage(
	 const String & messageId_,
	 const String & nameSpace_,
	 const CIMInstance& indicationInstance_,
	 QueueIdStack queueIds_)
	 :
	 CIMRequestMessage(
            CIM_PROCESS_INDICATION_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace (nameSpace_),
	 indicationInstance(indicationInstance_)
      {

      }

      String nameSpace;
      CIMInstance indicationInstance;
};

class CIMNotifyProviderRegistrationRequestMessage : public CIMRequestMessage
{
   public:
      enum Operation {OP_CREATE = 1, OP_DELETE = 2, OP_MODIFY = 3};

      CIMNotifyProviderRegistrationRequestMessage(
	 const String & messageId_,
	 const Operation operation_,
	 const CIMInstance & provider_,
         const CIMInstance & providerModule_,
	 const String & className_,
         const Array <String> & newNamespaces_,
         const Array <String> & oldNamespaces_,
	 const CIMPropertyList & newPropertyNames_,
	 const CIMPropertyList & oldPropertyNames_,
	 QueueIdStack queueIds_)
	 :
	 CIMRequestMessage(
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
      String className;
      Array <String> newNamespaces;
      Array <String> oldNamespaces;
      CIMPropertyList newPropertyNames;
      CIMPropertyList oldPropertyNames;
      Operation operation;
};

class CIMNotifyProviderTerminationRequestMessage : public CIMRequestMessage
{
   public:
      CIMNotifyProviderTerminationRequestMessage(
	 const String & messageId_,
	 const CIMInstance & provider_,
	 QueueIdStack queueIds_)
	 :
	 CIMRequestMessage(
            CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE,
	    messageId_, queueIds_),
	 provider (provider_)
      {

      }

      CIMInstance provider;
};

class CIMHandleIndicationRequestMessage : public CIMRequestMessage
{
   public:

      CIMHandleIndicationRequestMessage(
	  const String& messageId_,
	  const String & nameSpace_,
	  const CIMInstance& handlerInstance_,
	  const CIMInstance& indicationInstance_,
	  QueueIdStack queueIds_,
	  const String& authType_ = String::EMPTY,
	  const String& userName_ = String::EMPTY)
	 :
	 CIMRequestMessage(
	    CIM_HANDLE_INDICATION_REQUEST_MESSAGE, messageId_, queueIds_),
	 nameSpace(nameSpace_),
	 handlerInstance(handlerInstance_),
	 indicationInstance(indicationInstance_),
	 authType(authType_),
	 userName(userName_)
      {

      }

      String nameSpace;
      CIMInstance handlerInstance;
      CIMInstance indicationInstance;
      String authType;
      String userName;
};

class CIMCreateSubscriptionRequestMessage : public CIMRequestMessage
{
   public:

    CIMCreateSubscriptionRequestMessage(
        const String& messageId_,
        const String & nameSpace_,
        //
        //  ATTN-CAKG-P2-20020326: May want to change to CIMNamedInstance -
        //  Both the instance and instance name (CIMReference) are needed, so
        //  it would be convenient to have the CIMNamedInstance
        //
        const CIMInstance & subscriptionInstance_,
        const Array<String> & classNames_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
        const CIMPropertyList & propertyList_,
        const Uint16 repeatNotificationPolicy_,
        const String & condition_,
        const String & queryLanguage_,
        QueueIdStack queueIds_,
        const String & authType_ = String::EMPTY,
        const String & userName_ = String::EMPTY)
        :
        CIMRequestMessage(
            CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE,
            messageId_,
            queueIds_),
        nameSpace (nameSpace_),
        subscriptionInstance(subscriptionInstance_),
        classNames(classNames_),
        provider (provider_),
        providerModule (providerModule_),
        propertyList (propertyList_),
        repeatNotificationPolicy (repeatNotificationPolicy_),
        condition (condition_),
        queryLanguage (queryLanguage_),
	authType(authType_),
	userName(userName_)
    {
    }

    String nameSpace;
    CIMInstance subscriptionInstance;
    Array <String> classNames;
    CIMInstance provider;
    CIMInstance providerModule;
    CIMPropertyList propertyList;
    Uint16 repeatNotificationPolicy;
    String condition;
    String queryLanguage;
    String authType;
    String userName;
};

class CIMModifySubscriptionRequestMessage : public CIMRequestMessage
{
   public:

    CIMModifySubscriptionRequestMessage(
        const String& messageId_,
        const String & nameSpace_,
        //
        //  ATTN-CAKG-P2-20020326: May want to change to CIMNamedInstance -
        //  Both the instance and instance name (CIMReference) are needed, so
        //  it would be convenient to have the CIMNamedInstance
        //
        const CIMInstance & subscriptionInstance_,
        const Array<String> & classNames_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
        const CIMPropertyList & propertyList_,
        const Uint16 repeatNotificationPolicy_,
        const String & condition_,
        const String & queryLanguage_,
        QueueIdStack queueIds_,
        const String & authType_ = String::EMPTY,
        const String & userName_ = String::EMPTY)
        :
        CIMRequestMessage(
            CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE,
            messageId_,
            queueIds_),
        nameSpace(nameSpace_),
        subscriptionInstance(subscriptionInstance_),
        classNames(classNames_),
        provider (provider_),
        providerModule (providerModule_),
        propertyList (propertyList_),
        repeatNotificationPolicy (repeatNotificationPolicy_),
        condition (condition_),
        queryLanguage (queryLanguage_),
	authType(authType_),
	userName(userName_)
    {
    }

    String nameSpace;
    CIMInstance subscriptionInstance;
    Array<String> classNames;
    CIMInstance provider;
    CIMInstance providerModule;
    CIMPropertyList propertyList;
    Uint16 repeatNotificationPolicy;
    String condition;
    String queryLanguage;
    String authType;
    String userName;
};

class CIMDeleteSubscriptionRequestMessage : public CIMRequestMessage
{
   public:

    CIMDeleteSubscriptionRequestMessage(
        const String& messageId_,
        const String & nameSpace_,
        //
        //  ATTN-CAKG-P2-20020326: May want to change to CIMNamedInstance -
        //  Both the instance and instance name (CIMReference) are needed, so
        //  it would be convenient to have the CIMNamedInstance
        //
        const CIMInstance & subscriptionInstance_,
        const Array<String> & classNames_,
        const CIMInstance & provider_,
        const CIMInstance & providerModule_,
        QueueIdStack queueIds_,
        const String& authType_ = String::EMPTY,
        const String& userName_ = String::EMPTY)
        :
        CIMRequestMessage(
            CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE,
	    messageId_,
	    queueIds_),
        nameSpace(nameSpace_),
        subscriptionInstance(subscriptionInstance_),
        classNames(classNames_),
        provider (provider_),
        providerModule (providerModule_),
	authType(authType_),
	userName(userName_)
    {
    }

    String nameSpace;
    CIMInstance subscriptionInstance;
    Array<String> classNames;
    CIMInstance provider;
    CIMInstance providerModule;
    String authType;
    String userName;
};

class PEGASUS_COMMON_LINKAGE CIMGetClassResponseMessage
   : public CIMResponseMessage
{
   public:

      CIMGetClassResponseMessage(
	 const String& messageId_,
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_,
	 const CIMClass& cimClass_)
	 :
	 CIMResponseMessage(CIM_GET_CLASS_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_,
	 const CIMInstance& cimInstance_)
	 :
	 CIMResponseMessage(CIM_GET_INSTANCE_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 :
	 CIMResponseMessage(CIM_EXPORT_INDICATION_RESPONSE_MESSAGE,
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
	 :
	 CIMResponseMessage(CIM_DELETE_CLASS_RESPONSE_MESSAGE,
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
	 :
	 CIMResponseMessage(CIM_DELETE_INSTANCE_RESPONSE_MESSAGE,
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
	 :
	 CIMResponseMessage(CIM_CREATE_CLASS_RESPONSE_MESSAGE,
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
	 const CIMReference& instanceName_)
	 :
	 CIMResponseMessage(CIM_CREATE_INSTANCE_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
	 instanceName(instanceName_)
      {
      }

      CIMReference instanceName;	
};

class CIMModifyClassResponseMessage : public CIMResponseMessage
{
   public:

      CIMModifyClassResponseMessage(
	 const String& messageId_,
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_)
	 :
	 CIMResponseMessage(CIM_MODIFY_CLASS_RESPONSE_MESSAGE,
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
	 :
	 CIMResponseMessage(CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE,
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
	 const Array<CIMClass>& cimClasses_)
	 :
	 CIMResponseMessage(CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 const Array<String>& classNames_)
	 :
	 CIMResponseMessage(CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_,
	 const Array<CIMNamedInstance>& cimNamedInstances_)
	 :
	 CIMResponseMessage(CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
	 cimNamedInstances(cimNamedInstances_)
      {
      }

      Array<CIMNamedInstance> cimNamedInstances;
};

class CIMEnumerateInstanceNamesResponseMessage : public CIMResponseMessage
{
   public:

      CIMEnumerateInstanceNamesResponseMessage(
	 const String& messageId_,
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_,
	 const Array<CIMReference>& instanceNames_)
	 :
	 CIMResponseMessage(CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_,
	 const Array<CIMObjectWithPath>& cimObjects_)
	 :
	 CIMResponseMessage(CIM_EXEC_QUERY_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
	 cimObjects(cimObjects_)
      {
      }

      Array<CIMObjectWithPath> cimObjects;
};

class CIMAssociatorsResponseMessage : public CIMResponseMessage
{
   public:

      CIMAssociatorsResponseMessage(
	 const String& messageId_,
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_,
	 const Array<CIMObjectWithPath>& cimObjects_)
	 :
	 CIMResponseMessage(CIM_ASSOCIATORS_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_,
	 const Array<CIMReference>& objectNames_)
	 :
	 CIMResponseMessage(CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_,
	 const Array<CIMObjectWithPath>& cimObjects_)
	 :
	 CIMResponseMessage(CIM_REFERENCES_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_,
	 const Array<CIMReference>& objectNames_)
	 :
	 CIMResponseMessage(CIM_REFERENCE_NAMES_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_,
	 const CIMValue& value_)
	 :
	 CIMResponseMessage(CIM_GET_PROPERTY_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 :
	 CIMResponseMessage(CIM_SET_PROPERTY_RESPONSE_MESSAGE,
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
	 const CIMQualifierDecl& cimQualifierDecl_)
	 :
	 CIMResponseMessage(CIM_GET_QUALIFIER_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 const Array<CIMQualifierDecl>& qualifierDeclarations_)
	 :
	 CIMResponseMessage(CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
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
	 const String& methodName_)
	 :
	 CIMResponseMessage(CIM_INVOKE_METHOD_RESPONSE_MESSAGE,
			    messageId_, cimException_, queueIds_),
	 retValue(retValue_),
	 outParameters(outParameters_),
	 methodName(methodName_)
      {
      }

      CIMValue retValue;
      Array<CIMParamValue> outParameters;
      String methodName;
};

//
//  ATTN-CAKG-P2-20020326: To Be Removed - once the ProviderManagerService
//  has been modified to use the new messages
//
class CIMEnableIndicationSubscriptionResponseMessage : public CIMResponseMessage
{
   public:

      CIMEnableIndicationSubscriptionResponseMessage (
	 const String & messageId_,
	 const CIMException& cimException_,
	 const QueueIdStack & queueIds_)
	 :
	 CIMResponseMessage (CIM_ENABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE,
			     messageId_, cimException_, queueIds_)
      {
      }
};

//
//  ATTN-CAKG-P2-20020326: To Be Removed - once the ProviderManagerService
//  has been modified to use the new messages
//
class CIMModifyIndicationSubscriptionResponseMessage : public CIMResponseMessage
{
   public:

      CIMModifyIndicationSubscriptionResponseMessage (
	 const String & messageId_,
	 const CIMException& cimException_,
	 const QueueIdStack & queueIds_)
	 :
	 CIMResponseMessage (CIM_MODIFY_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE,
			     messageId_, cimException_, queueIds_)
      {
      }
};

//
//  ATTN-CAKG-P2-20020326: To Be Removed - once the ProviderManagerService
//  has been modified to use the new messages
//
class CIMDisableIndicationSubscriptionResponseMessage :
   public CIMResponseMessage
{
   public:

      CIMDisableIndicationSubscriptionResponseMessage (
	 const String & messageId_,
	 const CIMException& cimException_,
	 const QueueIdStack & queueIds_)
	 :
	 CIMResponseMessage
      (CIM_DISABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE,
       messageId_, cimException_, queueIds_)
      {
      }
};

class CIMProcessIndicationResponseMessage : public CIMResponseMessage
{
   public:

      CIMProcessIndicationResponseMessage(
	 const String& messageId_,
	 const CIMException& cimException_,
	 const QueueIdStack& queueIds_)
	 :
	 CIMResponseMessage(CIM_CREATE_INSTANCE_RESPONSE_MESSAGE,
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
	 :
	 CIMResponseMessage(CIM_HANDLE_INDICATION_RESPONSE_MESSAGE,
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
	 :
	 CIMResponseMessage(
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
	 :
	 CIMResponseMessage(
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
	 :
	 CIMResponseMessage(
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
	cimException,
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
	cimException,
        queueIds_)
    {
    }

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMMessage_h */
