//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi.zhou@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              John Alex, IBM (johnalex@us.ibm.com) - Bug#2290
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/AutoPtr.h>
#include "CIMMessage.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

void CIMResponseMessage::syncAttributes(const CIMRequestMessage* request)
{
    // Propagate request attributes to the response, as necessary
    setKey(request->getKey());
    setRouting(request->getRouting());
    setMask(request->getMask());
    setHttpMethod(request->getHttpMethod());
    setCloseConnect(request->getCloseConnect());
}

CIMResponseMessage* CIMGetClassRequestMessage::buildResponse() const
{
    AutoPtr<CIMGetClassResponseMessage> response(
        new CIMGetClassResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            CIMClass()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMGetInstanceRequestMessage::buildResponse() const
{
    AutoPtr<CIMGetInstanceResponseMessage> response(
        new CIMGetInstanceResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            CIMInstance()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMExportIndicationRequestMessage::buildResponse() const
{
    AutoPtr<CIMExportIndicationResponseMessage> response(
        new CIMExportIndicationResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMDeleteClassRequestMessage::buildResponse() const
{
    AutoPtr<CIMDeleteClassResponseMessage> response(
        new CIMDeleteClassResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMDeleteInstanceRequestMessage::buildResponse() const
{
    AutoPtr<CIMDeleteInstanceResponseMessage> response(
        new CIMDeleteInstanceResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMCreateClassRequestMessage::buildResponse() const
{
    AutoPtr<CIMCreateClassResponseMessage> response(
        new CIMCreateClassResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMCreateInstanceRequestMessage::buildResponse() const
{
    AutoPtr<CIMCreateInstanceResponseMessage> response(
        new CIMCreateInstanceResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            CIMObjectPath()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMModifyClassRequestMessage::buildResponse() const
{
    AutoPtr<CIMModifyClassResponseMessage> response(
        new CIMModifyClassResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMModifyInstanceRequestMessage::buildResponse() const
{
    AutoPtr<CIMModifyInstanceResponseMessage> response(
        new CIMModifyInstanceResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMEnumerateClassesRequestMessage::buildResponse() const
{
    AutoPtr<CIMEnumerateClassesResponseMessage> response(
        new CIMEnumerateClassesResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<CIMClass>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMEnumerateClassNamesRequestMessage::buildResponse() const
{
    AutoPtr<CIMEnumerateClassNamesResponseMessage> response(
        new CIMEnumerateClassNamesResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<CIMName>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMEnumerateInstancesRequestMessage::buildResponse() const
{
    AutoPtr<CIMEnumerateInstancesResponseMessage> response(
        new CIMEnumerateInstancesResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<CIMInstance>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMEnumerateInstanceNamesRequestMessage::buildResponse() const
{
    AutoPtr<CIMEnumerateInstanceNamesResponseMessage> response(
        new CIMEnumerateInstanceNamesResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<CIMObjectPath>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMExecQueryRequestMessage::buildResponse() const
{
    AutoPtr<CIMExecQueryResponseMessage> response(
        new CIMExecQueryResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<CIMObject>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMAssociatorsRequestMessage::buildResponse() const
{
    AutoPtr<CIMAssociatorsResponseMessage> response(
        new CIMAssociatorsResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<CIMObject>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMAssociatorNamesRequestMessage::buildResponse() const
{
    AutoPtr<CIMAssociatorNamesResponseMessage> response(
        new CIMAssociatorNamesResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<CIMObjectPath>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMReferencesRequestMessage::buildResponse() const
{
    AutoPtr<CIMReferencesResponseMessage> response(
        new CIMReferencesResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<CIMObject>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMReferenceNamesRequestMessage::buildResponse() const
{
    AutoPtr<CIMReferenceNamesResponseMessage> response(
        new CIMReferenceNamesResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<CIMObjectPath>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMGetPropertyRequestMessage::buildResponse() const
{
    AutoPtr<CIMGetPropertyResponseMessage> response(
        new CIMGetPropertyResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            CIMValue()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMSetPropertyRequestMessage::buildResponse() const
{
    AutoPtr<CIMSetPropertyResponseMessage> response(
        new CIMSetPropertyResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMGetQualifierRequestMessage::buildResponse() const
{
    AutoPtr<CIMGetQualifierResponseMessage> response(
        new CIMGetQualifierResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            CIMQualifierDecl()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMSetQualifierRequestMessage::buildResponse() const
{
    AutoPtr<CIMSetQualifierResponseMessage> response(
        new CIMSetQualifierResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMDeleteQualifierRequestMessage::buildResponse() const
{
    AutoPtr<CIMDeleteQualifierResponseMessage> response(
        new CIMDeleteQualifierResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMEnumerateQualifiersRequestMessage::buildResponse() const
{
    AutoPtr<CIMEnumerateQualifiersResponseMessage> response(
        new CIMEnumerateQualifiersResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<CIMQualifierDecl>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMInvokeMethodRequestMessage::buildResponse() const
{
    AutoPtr<CIMInvokeMethodResponseMessage> response(
        new CIMInvokeMethodResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            CIMValue(),
            Array<CIMParamValue>(),
            methodName));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMProcessIndicationRequestMessage::buildResponse() const
{
    AutoPtr<CIMProcessIndicationResponseMessage> response(
        new CIMProcessIndicationResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMNotifyProviderRegistrationRequestMessage::buildResponse() const
{
    AutoPtr<CIMNotifyProviderRegistrationResponseMessage> response(
        new CIMNotifyProviderRegistrationResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMNotifyProviderTerminationRequestMessage::buildResponse() const
{
    AutoPtr<CIMNotifyProviderTerminationResponseMessage> response(
        new CIMNotifyProviderTerminationResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMHandleIndicationRequestMessage::buildResponse() const
{
    AutoPtr<CIMHandleIndicationResponseMessage> response(
        new CIMHandleIndicationResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMCreateSubscriptionRequestMessage::buildResponse() const
{
    AutoPtr<CIMCreateSubscriptionResponseMessage> response(
        new CIMCreateSubscriptionResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMModifySubscriptionRequestMessage::buildResponse() const
{
    AutoPtr<CIMModifySubscriptionResponseMessage> response(
        new CIMModifySubscriptionResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMDeleteSubscriptionRequestMessage::buildResponse() const
{
    AutoPtr<CIMDeleteSubscriptionResponseMessage> response(
        new CIMDeleteSubscriptionResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* 
    CIMSubscriptionInitCompleteRequestMessage::buildResponse() const
{
    AutoPtr<CIMSubscriptionInitCompleteResponseMessage> response(
        new CIMSubscriptionInitCompleteResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMDisableModuleRequestMessage::buildResponse() const
{
    AutoPtr<CIMDisableModuleResponseMessage> response(
        new CIMDisableModuleResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<Uint16>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMEnableModuleRequestMessage::buildResponse() const
{
    AutoPtr<CIMEnableModuleResponseMessage> response(
        new CIMEnableModuleResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop(),
            Array<Uint16>()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMNotifyProviderEnableRequestMessage::buildResponse() const
{
    AutoPtr<CIMNotifyProviderEnableResponseMessage> response(
        new CIMNotifyProviderEnableResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMStopAllProvidersRequestMessage::buildResponse() const
{
    AutoPtr<CIMStopAllProvidersResponseMessage> response(
        new CIMStopAllProvidersResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMInitializeProviderRequestMessage::buildResponse() const
{
    AutoPtr<CIMInitializeProviderResponseMessage> response(
        new CIMInitializeProviderResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMInitializeProviderAgentRequestMessage::buildResponse() const
{
    AutoPtr<CIMInitializeProviderAgentResponseMessage> response(
        new CIMInitializeProviderAgentResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMResponseMessage* CIMNotifyConfigChangeRequestMessage::buildResponse() const
{
    AutoPtr<CIMNotifyConfigChangeResponseMessage> response(
        new CIMNotifyConfigChangeResponseMessage(
            messageId,
            CIMException(),
            queueIds.copyAndPop()));
    response->syncAttributes(this);
    return response.release();
}

CIMMessage::CIMMessage(Uint32 type, const String& messageId_)
    : Message(type), messageId(messageId_)
{
    operationContext.insert(
        AcceptLanguageListContainer(AcceptLanguageList())); 
    operationContext.insert(
        ContentLanguageListContainer(ContentLanguageList())); 
}

CIMRequestMessage::CIMRequestMessage(
    Uint32 type_, const String& messageId_, const QueueIdStack& queueIds_)
    : CIMMessage(type_, messageId_), queueIds(queueIds_)
{
}

CIMResponseMessage::CIMResponseMessage(
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

CIMOperationRequestMessage::CIMOperationRequestMessage(
    Uint32 type_,
    const String& messageId_,
    const QueueIdStack& queueIds_,
    const CIMNamespaceName& nameSpace_,
    const CIMName& className_,
    Uint32 providerType_)
    : 
    CIMRequestMessage(type_, messageId_, queueIds_),
    nameSpace(nameSpace_),
    className(className_),
    providerType(providerType_)
{
}

PEGASUS_NAMESPACE_END
