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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//            : Yi Zhou, Hewlett-Packard Company (yi.zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/AutoPtr.h>
#include "CIMMessage.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

void CIMResponseMessage::syncAttributes(CIMRequestMessage* request)
{
    // Propagate request attributes to the response, as necessary
    setKey(request->getKey());
    setRouting(request->getRouting());
    setMask(request->getMask());
    setHttpMethod(request->getHttpMethod());
}

CIMResponseMessage* CIMGetClassRequestMessage::buildResponse()
{
    CIMGetClassResponseMessage* response;
    response = new CIMGetClassResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        CIMClass());
    AutoPtr<CIMGetClassResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMGetInstanceRequestMessage::buildResponse()
{
    CIMGetInstanceResponseMessage* response;
    response = new CIMGetInstanceResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        CIMInstance());
    AutoPtr<CIMGetInstanceResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMExportIndicationRequestMessage::buildResponse()
{
    CIMExportIndicationResponseMessage* response;
    response = new CIMExportIndicationResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMExportIndicationResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMDeleteClassRequestMessage::buildResponse()
{
    CIMDeleteClassResponseMessage* response;
    response = new CIMDeleteClassResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMDeleteClassResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMDeleteInstanceRequestMessage::buildResponse()
{
    CIMDeleteInstanceResponseMessage* response;
    response = new CIMDeleteInstanceResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMDeleteInstanceResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMCreateClassRequestMessage::buildResponse()
{
    CIMCreateClassResponseMessage* response;
    response = new CIMCreateClassResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMCreateClassResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMCreateInstanceRequestMessage::buildResponse()
{
    CIMCreateInstanceResponseMessage* response;
    response = new CIMCreateInstanceResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        CIMObjectPath());
    AutoPtr<CIMCreateInstanceResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMModifyClassRequestMessage::buildResponse()
{
    CIMModifyClassResponseMessage* response;
    response = new CIMModifyClassResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMModifyClassResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMModifyInstanceRequestMessage::buildResponse()
{
    CIMModifyInstanceResponseMessage* response;
    response = new CIMModifyInstanceResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMModifyInstanceResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMEnumerateClassesRequestMessage::buildResponse()
{
    CIMEnumerateClassesResponseMessage* response;
    response = new CIMEnumerateClassesResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<CIMClass>());
    AutoPtr<CIMEnumerateClassesResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMEnumerateClassNamesRequestMessage::buildResponse()
{
    CIMEnumerateClassNamesResponseMessage* response;
    response = new CIMEnumerateClassNamesResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<CIMName>());
    AutoPtr<CIMEnumerateClassNamesResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMEnumerateInstancesRequestMessage::buildResponse()
{
    CIMEnumerateInstancesResponseMessage* response;
    response = new CIMEnumerateInstancesResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<CIMInstance>());
    AutoPtr<CIMEnumerateInstancesResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMEnumerateInstanceNamesRequestMessage::buildResponse()
{
    CIMEnumerateInstanceNamesResponseMessage* response;
    response = new CIMEnumerateInstanceNamesResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<CIMObjectPath>());
    AutoPtr<CIMEnumerateInstanceNamesResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMExecQueryRequestMessage::buildResponse()
{
    CIMExecQueryResponseMessage* response;
    response = new CIMExecQueryResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<CIMObject>());
    AutoPtr<CIMExecQueryResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMAssociatorsRequestMessage::buildResponse()
{
    CIMAssociatorsResponseMessage* response;
    response = new CIMAssociatorsResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<CIMObject>());
    AutoPtr<CIMAssociatorsResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMAssociatorNamesRequestMessage::buildResponse()
{
    CIMAssociatorNamesResponseMessage* response;
    response = new CIMAssociatorNamesResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<CIMObjectPath>());
    AutoPtr<CIMAssociatorNamesResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMReferencesRequestMessage::buildResponse()
{
    CIMReferencesResponseMessage* response;
    response = new CIMReferencesResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<CIMObject>());
    AutoPtr<CIMReferencesResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMReferenceNamesRequestMessage::buildResponse()
{
    CIMReferenceNamesResponseMessage* response;
    response = new CIMReferenceNamesResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<CIMObjectPath>());
    AutoPtr<CIMReferenceNamesResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMGetPropertyRequestMessage::buildResponse()
{
    CIMGetPropertyResponseMessage* response;
    response = new CIMGetPropertyResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        CIMValue());
    AutoPtr<CIMGetPropertyResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMSetPropertyRequestMessage::buildResponse()
{
    CIMSetPropertyResponseMessage* response;
    response = new CIMSetPropertyResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMSetPropertyResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMGetQualifierRequestMessage::buildResponse()
{
    CIMGetQualifierResponseMessage* response;
    response = new CIMGetQualifierResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        CIMQualifierDecl());
    AutoPtr<CIMGetQualifierResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMSetQualifierRequestMessage::buildResponse()
{
    CIMSetQualifierResponseMessage* response;
    response = new CIMSetQualifierResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMSetQualifierResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMDeleteQualifierRequestMessage::buildResponse()
{
    CIMDeleteQualifierResponseMessage* response;
    response = new CIMDeleteQualifierResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMDeleteQualifierResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMEnumerateQualifiersRequestMessage::buildResponse()
{
    CIMEnumerateQualifiersResponseMessage* response;
    response = new CIMEnumerateQualifiersResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<CIMQualifierDecl>());
    AutoPtr<CIMEnumerateQualifiersResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMInvokeMethodRequestMessage::buildResponse()
{
    CIMInvokeMethodResponseMessage* response;
    response = new CIMInvokeMethodResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        CIMValue(),
        Array<CIMParamValue>(),
        methodName);
    AutoPtr<CIMInvokeMethodResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMProcessIndicationRequestMessage::buildResponse()
{
    CIMProcessIndicationResponseMessage* response;
    response = new CIMProcessIndicationResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMProcessIndicationResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMEnableIndicationsRequestMessage::buildResponse()
{
    CIMEnableIndicationsResponseMessage* response;
    response = new CIMEnableIndicationsResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMEnableIndicationsResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMDisableIndicationsRequestMessage::buildResponse()
{
    CIMDisableIndicationsResponseMessage* response;
    response = new CIMDisableIndicationsResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMDisableIndicationsResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMNotifyProviderRegistrationRequestMessage::buildResponse()
{
    CIMNotifyProviderRegistrationResponseMessage* response;
    response = new CIMNotifyProviderRegistrationResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMNotifyProviderRegistrationResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMNotifyProviderTerminationRequestMessage::buildResponse()
{
    CIMNotifyProviderTerminationResponseMessage* response;
    response = new CIMNotifyProviderTerminationResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMNotifyProviderTerminationResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMHandleIndicationRequestMessage::buildResponse()
{
    CIMHandleIndicationResponseMessage* response;
    response = new CIMHandleIndicationResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMHandleIndicationResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMCreateSubscriptionRequestMessage::buildResponse()
{
    CIMCreateSubscriptionResponseMessage* response;
    response = new CIMCreateSubscriptionResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMCreateSubscriptionResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMModifySubscriptionRequestMessage::buildResponse()
{
    CIMModifySubscriptionResponseMessage* response;
    response = new CIMModifySubscriptionResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMModifySubscriptionResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMDeleteSubscriptionRequestMessage::buildResponse()
{
    CIMDeleteSubscriptionResponseMessage* response;
    response = new CIMDeleteSubscriptionResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMDeleteSubscriptionResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMDisableModuleRequestMessage::buildResponse()
{
    CIMDisableModuleResponseMessage* response;
    response = new CIMDisableModuleResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<Uint16>());
    AutoPtr<CIMDisableModuleResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMEnableModuleRequestMessage::buildResponse()
{
    CIMEnableModuleResponseMessage* response;
    response = new CIMEnableModuleResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop(),
        Array<Uint16>());
    AutoPtr<CIMEnableModuleResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMNotifyProviderEnableRequestMessage::buildResponse()
{
    CIMNotifyProviderEnableResponseMessage* response;
    response = new CIMNotifyProviderEnableResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMNotifyProviderEnableResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMStopAllProvidersRequestMessage::buildResponse()
{
    CIMStopAllProvidersResponseMessage* response;
    response = new CIMStopAllProvidersResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMStopAllProvidersResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMInitializeProviderRequestMessage::buildResponse()
{
    CIMInitializeProviderResponseMessage* response;
    response = new CIMInitializeProviderResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMInitializeProviderResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMInitializeProviderAgentRequestMessage::buildResponse()
{
    CIMInitializeProviderAgentResponseMessage* response;
    response = new CIMInitializeProviderAgentResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMInitializeProviderAgentResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

CIMResponseMessage* CIMNotifyConfigChangeRequestMessage::buildResponse()
{
    CIMNotifyConfigChangeResponseMessage* response;
    response = new CIMNotifyConfigChangeResponseMessage(
        messageId,
        CIMException(),
        queueIds.copyAndPop());
    AutoPtr<CIMNotifyConfigChangeResponseMessage> responsePtr(response);

    response->syncAttributes(this);

    responsePtr.release();
    return response;
}

PEGASUS_NAMESPACE_END
