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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By: Seema Gupta (gseema@in.ibm.com) for PEP135
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Jenny Yu, Hewlett-Packard Company (jenny.yu@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/OperationContextInternal.h>

#include "CIMMessageSerializer.h"

PEGASUS_NAMESPACE_BEGIN

void CIMMessageSerializer::serialize(Array<char>& out, CIMMessage* cimMessage)
{
    if (cimMessage == 0)
    {
        // No message to serialize
        return;
    }

    // ATTN: Need to serialize the Message class members?

    XmlWriter::append(out, "<PGMESSAGE ID=\"");
    XmlWriter::append(out, cimMessage->messageId);
    XmlWriter::append(out, "\" TYPE=\"");
    XmlWriter::append(out, cimMessage->getType());
    XmlWriter::append(out, "\">");

    _serializeOperationContext(out, cimMessage->operationContext);

    CIMRequestMessage* cimReqMessage;
    cimReqMessage = dynamic_cast<CIMRequestMessage*>(cimMessage);

    CIMResponseMessage* cimRespMessage;
    cimRespMessage = dynamic_cast<CIMResponseMessage*>(cimMessage);

    if (cimReqMessage)
    {
        _serializeCIMRequestMessage(out, cimReqMessage);
    }
    else if (cimRespMessage)
    {
        _serializeCIMResponseMessage(out, cimRespMessage);
    }
    else
    {
        // No other CIMMessage types are currently defined
        PEGASUS_ASSERT(0);
    }

    XmlWriter::append(out, "</PGMESSAGE>");
}

//
// _serializeCIMRequestMessage
//
void CIMMessageSerializer::_serializeCIMRequestMessage(
    Array<char>& out,
    CIMRequestMessage* cimMessage)
{
    PEGASUS_ASSERT(cimMessage != 0);

    XmlWriter::append(out, "<PGREQ>");

    _serializeQueueIdStack(out, cimMessage->queueIds);

    CIMOperationRequestMessage* cimOpReqMessage;
    cimOpReqMessage = dynamic_cast<CIMOperationRequestMessage*>(cimMessage);

    CIMIndicationRequestMessage* cimIndReqMessage;
    cimIndReqMessage = dynamic_cast<CIMIndicationRequestMessage*>(cimMessage);

    if (cimOpReqMessage)
    {
        XmlWriter::append(out, "<PGOPREQ>\n");

        _serializeCIMNamespaceName(out, cimOpReqMessage->nameSpace);
        _serializeCIMName(out, cimOpReqMessage->className);

        // Encode cimOpReqMessage->providerType as an integer
        XmlWriter::appendValueElement(out, cimOpReqMessage->providerType);

        switch (cimMessage->getType())
        {
        case CIM_GET_CLASS_REQUEST_MESSAGE:
        case CIM_DELETE_CLASS_REQUEST_MESSAGE:
        case CIM_CREATE_CLASS_REQUEST_MESSAGE:
        case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
        case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
        case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
        case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
        case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
        case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
        case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
            // Not implemented.  No provider can support this message type.
            PEGASUS_ASSERT(0);
            break;

        // Instance operations
        case CIM_GET_INSTANCE_REQUEST_MESSAGE:
            _serializeCIMGetInstanceRequestMessage(
                out, (CIMGetInstanceRequestMessage*)cimMessage);
            break;
        case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
            _serializeCIMDeleteInstanceRequestMessage(
                out, (CIMDeleteInstanceRequestMessage*)cimMessage);
            break;
        case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
            _serializeCIMCreateInstanceRequestMessage(
                out, (CIMCreateInstanceRequestMessage*)cimMessage);
            break;
        case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
            _serializeCIMModifyInstanceRequestMessage(
                out, (CIMModifyInstanceRequestMessage*)cimMessage);
            break;
        case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
            _serializeCIMEnumerateInstancesRequestMessage(
                out, (CIMEnumerateInstancesRequestMessage*)cimMessage);
            break;
        case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
            _serializeCIMEnumerateInstanceNamesRequestMessage(
                out, (CIMEnumerateInstanceNamesRequestMessage*)cimMessage);
            break;
        case CIM_EXEC_QUERY_REQUEST_MESSAGE:
            _serializeCIMExecQueryRequestMessage(
                out, (CIMExecQueryRequestMessage*)cimMessage);
            break;

        // Property operations
        case CIM_GET_PROPERTY_REQUEST_MESSAGE:
            _serializeCIMGetPropertyRequestMessage(
                out, (CIMGetPropertyRequestMessage*)cimMessage);
            break;
        case CIM_SET_PROPERTY_REQUEST_MESSAGE:
            _serializeCIMSetPropertyRequestMessage(
                out, (CIMSetPropertyRequestMessage*)cimMessage);
            break;

        // Association operations
        case CIM_ASSOCIATORS_REQUEST_MESSAGE:
            _serializeCIMAssociatorsRequestMessage(
                out, (CIMAssociatorsRequestMessage*)cimMessage);
            break;
        case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
            _serializeCIMAssociatorNamesRequestMessage(
                out, (CIMAssociatorNamesRequestMessage*)cimMessage);
            break;
        case CIM_REFERENCES_REQUEST_MESSAGE:
            _serializeCIMReferencesRequestMessage(
                out, (CIMReferencesRequestMessage*)cimMessage);
            break;
        case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
            _serializeCIMReferenceNamesRequestMessage(
                out, (CIMReferenceNamesRequestMessage*)cimMessage);
            break;

        // Method operations
        case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
            _serializeCIMInvokeMethodRequestMessage(
                out, (CIMInvokeMethodRequestMessage*)cimMessage);
            break;

        default:
            PEGASUS_ASSERT(0);
        }

        XmlWriter::append(out, "</PGOPREQ>");
    }
    else if (cimIndReqMessage)
    {
        XmlWriter::append(out, "<PGINDREQ>");

        switch (cimMessage->getType())
        {
        case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
            _serializeCIMCreateSubscriptionRequestMessage(
                out, (CIMCreateSubscriptionRequestMessage*)cimMessage);
            break;
        case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
            _serializeCIMModifySubscriptionRequestMessage(
                out, (CIMModifySubscriptionRequestMessage*)cimMessage);
            break;
        case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
            _serializeCIMDeleteSubscriptionRequestMessage(
                out, (CIMDeleteSubscriptionRequestMessage*)cimMessage);
            break;
        default:
            PEGASUS_ASSERT(0);
        }

        XmlWriter::append(out, "</PGINDREQ>");
    }
    else    // Other message types
    {
        XmlWriter::append(out, "<PGOTHERREQ>");

        switch (cimMessage->getType())
        {
        case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
            _serializeCIMExportIndicationRequestMessage(
                out, (CIMExportIndicationRequestMessage*)cimMessage);
            break;
        case CIM_PROCESS_INDICATION_REQUEST_MESSAGE:
            _serializeCIMProcessIndicationRequestMessage(
                out, (CIMProcessIndicationRequestMessage*)cimMessage);
            break;
        case CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE:
            // ATTN: No need to serialize this yet
            PEGASUS_ASSERT(0);
            //_serializeCIMNotifyProviderRegistrationRequestMessage(
            //    out, (CIMNotifyProviderRegistrationRequestMessage*)cimMessage);
            break;
        case CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE:
            // ATTN: No need to serialize this yet
            PEGASUS_ASSERT(0);
            //_serializeCIMNotifyProviderTerminationRequestMessage(
            //    out, (CIMNotifyProviderTerminationRequestMessage*)cimMessage);
            break;
        case CIM_HANDLE_INDICATION_REQUEST_MESSAGE:
            // ATTN: No need to serialize this yet
            PEGASUS_ASSERT(0);
            //_serializeCIMHandleIndicationRequestMessage(
            //    out, (CIMHandleIndicationRequestMessage*)cimMessage);
            break;
        case CIM_DISABLE_MODULE_REQUEST_MESSAGE:
            _serializeCIMDisableModuleRequestMessage(
                out, (CIMDisableModuleRequestMessage*)cimMessage);
            break;
        case CIM_ENABLE_MODULE_REQUEST_MESSAGE:
            _serializeCIMEnableModuleRequestMessage(
                out, (CIMEnableModuleRequestMessage*)cimMessage);
            break;
        case CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE:
            // ATTN: No need to serialize this yet
            PEGASUS_ASSERT(0);
            //_serializeCIMNotifyProviderEnableRequestMessage(
            //    out, (CIMNotifyProviderEnableRequestMessage*)cimMessage);
            break;
        case CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE:
            _serializeCIMStopAllProvidersRequestMessage(
                out, (CIMStopAllProvidersRequestMessage*)cimMessage);
            break;
        case CIM_INITIALIZE_PROVIDER_REQUEST_MESSAGE:
            _serializeCIMInitializeProviderRequestMessage(
                out, (CIMInitializeProviderRequestMessage*)cimMessage);
            break;
        case CIM_INITIALIZE_PROVIDER_AGENT_REQUEST_MESSAGE:
            _serializeCIMInitializeProviderAgentRequestMessage(
                out, (CIMInitializeProviderAgentRequestMessage*)cimMessage);
            break;

        case CIM_NOTIFY_CONFIG_CHANGE_REQUEST_MESSAGE:
            _serializeCIMNotifyConfigChangeRequestMessage(
                out, (CIMNotifyConfigChangeRequestMessage*)cimMessage);
            break;

        case CIM_SUBSCRIPTION_INIT_COMPLETE_REQUEST_MESSAGE:
            _serializeCIMSubscriptionInitCompleteRequestMessage(
                out, 
                (CIMSubscriptionInitCompleteRequestMessage *)
                cimMessage);
            break;

        default:
            PEGASUS_ASSERT(0);
        }

        XmlWriter::append(out, "</PGOTHERREQ>");
    }

    XmlWriter::append(out, "</PGREQ>");
}

//
// _serializeCIMResponseMessage
//
void CIMMessageSerializer::_serializeCIMResponseMessage(
    Array<char>& out,
    CIMResponseMessage* cimMessage)
{
    PEGASUS_ASSERT(cimMessage != 0);

    XmlWriter::append(out, "<PGRESP>\n");

    _serializeQueueIdStack(out, cimMessage->queueIds);
    _serializeCIMException(out, cimMessage->cimException);

    switch (cimMessage->getType())
    {
        //
        // CIM Operation Response Messages
        //

        case CIM_GET_CLASS_RESPONSE_MESSAGE:
        case CIM_DELETE_CLASS_RESPONSE_MESSAGE:
        case CIM_CREATE_CLASS_RESPONSE_MESSAGE:
        case CIM_MODIFY_CLASS_RESPONSE_MESSAGE:
        case CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE:
        case CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE:
        case CIM_GET_QUALIFIER_RESPONSE_MESSAGE:
        case CIM_SET_QUALIFIER_RESPONSE_MESSAGE:
        case CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE:
        case CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE:
            // Not implemented.  No provider can support this message type.
            PEGASUS_ASSERT(0);
            break;

        // Instance operations
        case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
            _serializeCIMGetInstanceResponseMessage(
                out, (CIMGetInstanceResponseMessage*)cimMessage);
            break;
        case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
            _serializeCIMDeleteInstanceResponseMessage(
                out, (CIMDeleteInstanceResponseMessage*)cimMessage);
            break;
        case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
            _serializeCIMCreateInstanceResponseMessage(
                out, (CIMCreateInstanceResponseMessage*)cimMessage);
            break;
        case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
            _serializeCIMModifyInstanceResponseMessage(
                out, (CIMModifyInstanceResponseMessage*)cimMessage);
            break;
        case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
            _serializeCIMEnumerateInstancesResponseMessage(
                out, (CIMEnumerateInstancesResponseMessage*)cimMessage);
            break;
        case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
            _serializeCIMEnumerateInstanceNamesResponseMessage(
                out, (CIMEnumerateInstanceNamesResponseMessage*)cimMessage);
            break;
        case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
            _serializeCIMExecQueryResponseMessage(
                out, (CIMExecQueryResponseMessage*)cimMessage);
            break;

        // Property operations
        case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
            _serializeCIMGetPropertyResponseMessage(
                out, (CIMGetPropertyResponseMessage*)cimMessage);
            break;
        case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
            _serializeCIMSetPropertyResponseMessage(
                out, (CIMSetPropertyResponseMessage*)cimMessage);
            break;

        // Association operations
        case CIM_ASSOCIATORS_RESPONSE_MESSAGE:
            _serializeCIMAssociatorsResponseMessage(
                out, (CIMAssociatorsResponseMessage*)cimMessage);
            break;
        case CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE:
            _serializeCIMAssociatorNamesResponseMessage(
                out, (CIMAssociatorNamesResponseMessage*)cimMessage);
            break;
        case CIM_REFERENCES_RESPONSE_MESSAGE:
            _serializeCIMReferencesResponseMessage(
                out, (CIMReferencesResponseMessage*)cimMessage);
            break;
        case CIM_REFERENCE_NAMES_RESPONSE_MESSAGE:
            _serializeCIMReferenceNamesResponseMessage(
                out, (CIMReferenceNamesResponseMessage*)cimMessage);
            break;

        // Method operations
        case CIM_INVOKE_METHOD_RESPONSE_MESSAGE:
            _serializeCIMInvokeMethodResponseMessage(
                out, (CIMInvokeMethodResponseMessage*)cimMessage);
            break;

        //
        // CIM Indication Response Messages
        //

        case CIM_CREATE_SUBSCRIPTION_RESPONSE_MESSAGE:
            _serializeCIMCreateSubscriptionResponseMessage(
                out, (CIMCreateSubscriptionResponseMessage*)cimMessage);
            break;
        case CIM_MODIFY_SUBSCRIPTION_RESPONSE_MESSAGE:
            _serializeCIMModifySubscriptionResponseMessage(
                out, (CIMModifySubscriptionResponseMessage*)cimMessage);
            break;
        case CIM_DELETE_SUBSCRIPTION_RESPONSE_MESSAGE:
            _serializeCIMDeleteSubscriptionResponseMessage(
                out, (CIMDeleteSubscriptionResponseMessage*)cimMessage);
            break;

        //
        // Other CIM Response Messages
        //

        case CIM_EXPORT_INDICATION_RESPONSE_MESSAGE:
            _serializeCIMExportIndicationResponseMessage(
                out, (CIMExportIndicationResponseMessage*)cimMessage);
            break;
        case CIM_PROCESS_INDICATION_RESPONSE_MESSAGE:
            _serializeCIMProcessIndicationResponseMessage(
                out, (CIMProcessIndicationResponseMessage*)cimMessage);
            break;
        case CIM_NOTIFY_PROVIDER_REGISTRATION_RESPONSE_MESSAGE:
            // ATTN: No need to serialize this yet
            PEGASUS_ASSERT(0);
            //_serializeCIMNotifyProviderRegistrationResponseMessage(
            //    out, (CIMNotifyProviderRegistrationResponseMessage*)cimMessage);
            break;
        case CIM_NOTIFY_PROVIDER_TERMINATION_RESPONSE_MESSAGE:
            // ATTN: No need to serialize this yet
            PEGASUS_ASSERT(0);
            //_serializeCIMNotifyProviderTerminationResponseMessage(
            //    out, (CIMNotifyProviderTerminationResponseMessage*)cimMessage);
            break;
        case CIM_HANDLE_INDICATION_RESPONSE_MESSAGE:
            // ATTN: No need to serialize this yet
            PEGASUS_ASSERT(0);
            //_serializeCIMHandleIndicationResponseMessage(
            //    out, (CIMHandleIndicationResponseMessage*)cimMessage);
            break;
        case CIM_DISABLE_MODULE_RESPONSE_MESSAGE:
            _serializeCIMDisableModuleResponseMessage(
                out, (CIMDisableModuleResponseMessage*)cimMessage);
            break;
        case CIM_ENABLE_MODULE_RESPONSE_MESSAGE:
            _serializeCIMEnableModuleResponseMessage(
                out, (CIMEnableModuleResponseMessage*)cimMessage);
            break;
        case CIM_NOTIFY_PROVIDER_ENABLE_RESPONSE_MESSAGE:
            // ATTN: No need to serialize this yet
            PEGASUS_ASSERT(0);
            //_serializeCIMNotifyProviderEnableResponseMessage(
            //    out, (CIMNotifyProviderEnableResponseMessage*)cimMessage);
            break;
        case CIM_STOP_ALL_PROVIDERS_RESPONSE_MESSAGE:
            _serializeCIMStopAllProvidersResponseMessage(
                out, (CIMStopAllProvidersResponseMessage*)cimMessage);
            break;
        case CIM_INITIALIZE_PROVIDER_RESPONSE_MESSAGE:
            _serializeCIMInitializeProviderResponseMessage(
                out, (CIMInitializeProviderResponseMessage*)cimMessage);
            break;
        case CIM_INITIALIZE_PROVIDER_AGENT_RESPONSE_MESSAGE:
            _serializeCIMInitializeProviderAgentResponseMessage(
                out, (CIMInitializeProviderAgentResponseMessage*)cimMessage);
            break;
        case CIM_NOTIFY_CONFIG_CHANGE_RESPONSE_MESSAGE:
            _serializeCIMNotifyConfigChangeResponseMessage(
                out, (CIMNotifyConfigChangeResponseMessage*)cimMessage);
            break;
        case CIM_SUBSCRIPTION_INIT_COMPLETE_RESPONSE_MESSAGE:
            _serializeCIMSubscriptionInitCompleteResponseMessage(
                out, 
                (CIMSubscriptionInitCompleteResponseMessage *)
                cimMessage);
            break;

        default:
            PEGASUS_ASSERT(0);
    }

    XmlWriter::append(out, "</PGRESP>");
}


//
// Utility Methods
//

//
// _serializeUserInfo consolidates encoding of these common message attributes
//
void CIMMessageSerializer::_serializeUserInfo(
    Array<char>& out,
    const String& authType,
    const String& userName)
{
    XmlWriter::appendValueElement(out, authType);
    XmlWriter::appendValueElement(out, userName);
}

//
// _serializeQueueIdStack
//
void CIMMessageSerializer::_serializeQueueIdStack(
    Array<char>& out,
    const QueueIdStack& queueIdStack)
{
    QueueIdStack stackCopy = queueIdStack;

    // Use a PGQIDSTACK element to encapsulate the QueueIdStack encoding
    XmlWriter::append(out, "<PGQIDSTACK>\n");
    while (!stackCopy.isEmpty())
    {
        Uint32 item = stackCopy.top();
        stackCopy.pop();
        XmlWriter::appendValueElement(out, item);
    }
    XmlWriter::append(out, "</PGQIDSTACK>\n");
}

//
// _serializeOperationContext
//
void CIMMessageSerializer::_serializeOperationContext(
    Array<char>& out,
    const OperationContext& operationContext)
{
    // Use a PGOP element to encapsulate the OperationContext encoding
    XmlWriter::append(out, "<PGOC>\n");

    // Note: OperationContext class does not allow iteration through Containers

    try
    {
        const IdentityContainer container =
            operationContext.get(IdentityContainer::NAME);

        XmlWriter::append(out, "<PGOCID>\n");
        XmlWriter::appendValueElement(out, container.getUserName());
        XmlWriter::append(out, "</PGOCID>\n");
    }
    catch (...)
    {
    }

    try
    {
        const SubscriptionInstanceContainer container =
            operationContext.get(SubscriptionInstanceContainer::NAME);

        XmlWriter::append(out, "<PGOCSI>\n");
        _serializeCIMInstance(out, container.getInstance());
        XmlWriter::append(out, "</PGOCSI>\n");
    }
    catch (...)
    {
    }

    try
    {
        const SubscriptionFilterConditionContainer container =
            operationContext.get(SubscriptionFilterConditionContainer::NAME);

        XmlWriter::append(out, "<PGOCSFC>\n");
        XmlWriter::appendValueElement(out, container.getFilterCondition());
        XmlWriter::appendValueElement(out, container.getQueryLanguage());
        XmlWriter::append(out, "</PGOCSFC>\n");
    }
    catch (...)
    {
    }

    try
    {
        const SubscriptionFilterQueryContainer container =
            operationContext.get(SubscriptionFilterQueryContainer::NAME);

        XmlWriter::append(out, "<PGOCSFQ>\n");
        XmlWriter::appendValueElement(out, container.getFilterQuery());
        XmlWriter::appendValueElement(out, container.getQueryLanguage());
        _serializeCIMNamespaceName(out, container.getSourceNameSpace());
        XmlWriter::append(out, "</PGOCSFQ>\n");
    }
    catch (...)
    {
    }

    try
    {
        const SubscriptionInstanceNamesContainer container =
            operationContext.get(SubscriptionInstanceNamesContainer::NAME);

        XmlWriter::append(out, "<PGOCSIN>\n");

        Array<CIMObjectPath> cimObjectPaths = container.getInstanceNames();
        for (Uint32 i=0; i < cimObjectPaths.size(); i++)
        {
            _serializeCIMObjectPath(out, cimObjectPaths[i]);
        }

        XmlWriter::append(out, "</PGOCSIN>\n");
    }
    catch (...)
    {
    }

    try
    {
        const TimeoutContainer container =
            operationContext.get(TimeoutContainer::NAME);

        XmlWriter::append(out, "<PGOCTO>\n");
        XmlWriter::appendValueElement(out, container.getTimeOut());
        XmlWriter::append(out, "</PGOCTO>\n");
    }
    catch (...)
    {
    }

    try
    {
        const AcceptLanguageListContainer container =
            operationContext.get(AcceptLanguageListContainer::NAME);

        XmlWriter::append(out, "<PGOCALL>\n");
        _serializeAcceptLanguages(out, container.getLanguages());
        XmlWriter::append(out, "</PGOCALL>\n");
    }
    catch (...)
    {
    }

    try
    {
        const SubscriptionLanguageListContainer container =
            operationContext.get(SubscriptionLanguageListContainer::NAME);

        XmlWriter::append(out, "<PGOCSLL>\n");
        _serializeAcceptLanguages(out, container.getLanguages());
        XmlWriter::append(out, "</PGOCSLL>\n");
    }
    catch (...)
    {
    }

    try
    {
        const ContentLanguageListContainer container =
            operationContext.get(ContentLanguageListContainer::NAME);

        XmlWriter::append(out, "<PGOCCLL>\n");
        _serializeContentLanguages(out, container.getLanguages());
        XmlWriter::append(out, "</PGOCCLL>\n");
    }
    catch (...)
    {
    }

    try
    {
        const SnmpTrapOidContainer container =
            operationContext.get(SnmpTrapOidContainer::NAME);

        XmlWriter::append(out, "<PGOCSTO>\n");
        XmlWriter::appendValueElement(out, container.getSnmpTrapOid());
        XmlWriter::append(out, "</PGOCSTO>\n");
    }
    catch (...)
    {
    }

    try
    {
        const LocaleContainer container =
            operationContext.get(LocaleContainer::NAME);

        XmlWriter::append(out, "<PGOCL>\n");
        XmlWriter::appendValueElement(out, container.getLanguageId());
        XmlWriter::append(out, "</PGOCL>\n");
    }
    catch (...)
    {
    }

    try
    {
        const ProviderIdContainer container =
            operationContext.get(ProviderIdContainer::NAME);

        XmlWriter::append(out, "<PGOCPI>\n");
        _serializeCIMInstance(out, container.getModule());
        _serializeCIMInstance(out, container.getProvider());
        XmlWriter::appendValueElement(out, container.isRemoteNameSpace());
        XmlWriter::appendValueElement(out, container.getRemoteInfo());
        XmlWriter::append(out, "</PGOCPI>\n");
    }
    catch (...)
    {
    }

    XmlWriter::append(out, "</PGOC>\n");
}

//
// _serializeContentLanguages
//
void CIMMessageSerializer::_serializeContentLanguages(
    Array<char>& out,
    const ContentLanguages& contentLanguages)
{
    Array<ContentLanguageElement> elements;
    contentLanguages.getAllLanguageElements(elements);

    // Use a PGCONTLANGS element to encapsulate the ContentLanguages encoding
    XmlWriter::append(out, "<PGCONTLANGS>\n");
    for (Uint32 i=0; i < elements.size(); i++)
    {
        XmlWriter::appendValueElement(out, elements[i].getTag());
    }
    XmlWriter::append(out, "</PGCONTLANGS>\n");
}

//
// _serializeAcceptLanguages
//
void CIMMessageSerializer::_serializeAcceptLanguages(
    Array<char>& out,
    const AcceptLanguages& acceptLanguages)
{
    Array<AcceptLanguageElement> elements;
    acceptLanguages.getAllLanguageElements(elements);

    // Use a PGACCLANGS element to encapsulate the AcceptLanguages encoding
    XmlWriter::append(out, "<PGACCLANGS>\n");
    for (Uint32 i=0; i < elements.size(); i++)
    {
        XmlWriter::appendValueElement(out, elements[i].getTag());
        XmlWriter::appendValueElement(out, elements[i].getQuality());
    }
    XmlWriter::append(out, "</PGACCLANGS>\n");
}

//
// _serializeCIMException
//
void CIMMessageSerializer::_serializeCIMException(
    Array<char>& out,
    const CIMException& cimException)
{
    TraceableCIMException e(cimException);

    // Use a PGCIMEXC element to encapsulate the CIMException encoding
    // (Note: This is not truly necessary and could be removed.)
    XmlWriter::append(out, "<PGCIMEXC>\n");

    XmlWriter::appendValueElement(out, Uint32(e.getCode()));
    XmlWriter::appendValueElement(out, e.getMessage());
    XmlWriter::appendValueElement(out, e.getCIMMessage());
    XmlWriter::appendValueElement(out, e.getFile());
    XmlWriter::appendValueElement(out, e.getLine());
    _serializeContentLanguages(out, e.getContentLanguages());

    XmlWriter::append(out, "</PGCIMEXC>\n");
}

//
// _serializeCIMPropertyList
//
void CIMMessageSerializer::_serializeCIMPropertyList(
    Array<char>& out,
    const CIMPropertyList& cimPropertyList)
{
    // Need IPARAMVALUE wrapper because the value can be null.
    XmlWriter::appendPropertyListIParameter(out, cimPropertyList);
}

//
// _serializeCIMObjectPath
//
void CIMMessageSerializer::_serializeCIMObjectPath(
    Array<char>& out,
    const CIMObjectPath& cimObjectPath)
{
    // Use a PGPATH element to encapsulate the CIMObjectPath encoding
    // to account for uninitialized objects
    XmlWriter::append(out, "<PGPATH>\n");
    if (!cimObjectPath.getClassName().isNull())
    {
        XmlWriter::appendValueReferenceElement(out, cimObjectPath, true);
    }
    XmlWriter::append(out, "</PGPATH>\n");
}

//
// _serializeCIMInstance
//
void CIMMessageSerializer::_serializeCIMInstance(
    Array<char>& out,
    const CIMInstance& cimInstance)
{
    // Use a PGINST element to encapsulate the CIMInstance encoding
    // to account for uninitialized objects
    XmlWriter::append(out, "<PGINST>\n");
    if (!cimInstance.isUninitialized())
    {
        XmlWriter::appendInstanceElement(out, cimInstance);
        _serializeCIMObjectPath(out, cimInstance.getPath());
    }
    XmlWriter::append(out, "</PGINST>\n");
}

//
// _serializeCIMNamespaceName
//
void CIMMessageSerializer::_serializeCIMNamespaceName(
    Array<char>& out,
    const CIMNamespaceName& cimNamespaceName)
{
    // Encode CIMNamespaceName as a String for efficiency and so that null
    // values can be handled
    XmlWriter::appendValueElement(out, cimNamespaceName.getString());
}

//
// _serializeCIMName
//
void CIMMessageSerializer::_serializeCIMName(
    Array<char>& out,
    const CIMName& cimName)
{
    // Encode CIMName as a String so that null values can be handled
    XmlWriter::appendValueElement(out, cimName.getString());
}

//
// _serializeCIMObject
//
void CIMMessageSerializer::_serializeCIMObject(
    Array<char>& out,
    const CIMObject& object)
{
    // Use a PGOBJ element to encapsulate the CIMObject encoding
    // to account for uninitialized objects
    XmlWriter::append(out, "<PGOBJ>\n");
    if (!object.isUninitialized())
    {
        XmlWriter::appendObjectElement(out, object);
        _serializeCIMObjectPath(out, object.getPath());
    }
    XmlWriter::append(out, "</PGOBJ>\n");
}

//
//
// Request Messages
//
//

//
//
// CIMOperationRequestMessages
//
//

//
// _serializeCIMGetInstanceRequestMessage
//
void CIMMessageSerializer::_serializeCIMGetInstanceRequestMessage(
    Array<char>& out,
    CIMGetInstanceRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMObjectPath(out, message->instanceName);
    XmlWriter::appendValueElement(out, message->localOnly);
    XmlWriter::appendValueElement(out, message->includeQualifiers);
    XmlWriter::appendValueElement(out, message->includeClassOrigin);
    _serializeCIMPropertyList(out, message->propertyList);
}

//
// _serializeCIMDeleteInstanceRequestMessage
//
void CIMMessageSerializer::_serializeCIMDeleteInstanceRequestMessage(
    Array<char>& out,
    CIMDeleteInstanceRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMObjectPath(out, message->instanceName);
}

//
// _serializeCIMCreateInstanceRequestMessage
//
void CIMMessageSerializer::_serializeCIMCreateInstanceRequestMessage(
    Array<char>& out,
    CIMCreateInstanceRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMInstance(out, message->newInstance);
}

//
// _serializeCIMModifyInstanceRequestMessage
//
void CIMMessageSerializer::_serializeCIMModifyInstanceRequestMessage(
    Array<char>& out,
    CIMModifyInstanceRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMInstance(out, message->modifiedInstance);
    XmlWriter::appendValueElement(out, message->includeQualifiers);
    _serializeCIMPropertyList(out, message->propertyList);
}

//
// _serializeCIMEnumerateInstancesRequestMessage
//
void CIMMessageSerializer::_serializeCIMEnumerateInstancesRequestMessage(
    Array<char>& out,
    CIMEnumerateInstancesRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    XmlWriter::appendValueElement(out, message->deepInheritance);
    XmlWriter::appendValueElement(out, message->localOnly);
    XmlWriter::appendValueElement(out, message->includeQualifiers);
    XmlWriter::appendValueElement(out, message->includeClassOrigin);
    _serializeCIMPropertyList(out, message->propertyList);
}

//
// _serializeCIMEnumerateInstanceNamesRequestMessage
//
void CIMMessageSerializer::_serializeCIMEnumerateInstanceNamesRequestMessage(
    Array<char>& out,
    CIMEnumerateInstanceNamesRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    // No additional attributes to serialize!
}

//
// _serializeCIMExecQueryRequestMessage
//
void CIMMessageSerializer::_serializeCIMExecQueryRequestMessage(
    Array<char>& out,
    CIMExecQueryRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    XmlWriter::appendValueElement(out, message->queryLanguage);
    XmlWriter::appendValueElement(out, message->query);
}

//
// _serializeCIMAssociatorsRequestMessage
//
void CIMMessageSerializer::_serializeCIMAssociatorsRequestMessage(
    Array<char>& out,
    CIMAssociatorsRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMObjectPath(out, message->objectName);
    _serializeCIMName(out, message->assocClass);
    _serializeCIMName(out, message->resultClass);
    XmlWriter::appendValueElement(out, message->role);
    XmlWriter::appendValueElement(out, message->resultRole);
    XmlWriter::appendValueElement(out, message->includeQualifiers);
    XmlWriter::appendValueElement(out, message->includeClassOrigin);
    _serializeCIMPropertyList(out, message->propertyList);
}

//
// _serializeCIMAssociatorNamesRequestMessage
//
void CIMMessageSerializer::_serializeCIMAssociatorNamesRequestMessage(
    Array<char>& out,
    CIMAssociatorNamesRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMObjectPath(out, message->objectName);
    _serializeCIMName(out, message->assocClass);
    _serializeCIMName(out, message->resultClass);
    XmlWriter::appendValueElement(out, message->role);
    XmlWriter::appendValueElement(out, message->resultRole);
}

//
// _serializeCIMReferencesRequestMessage
//
void CIMMessageSerializer::_serializeCIMReferencesRequestMessage(
    Array<char>& out,
    CIMReferencesRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMObjectPath(out, message->objectName);
    _serializeCIMName(out, message->resultClass);
    XmlWriter::appendValueElement(out, message->role);
    XmlWriter::appendValueElement(out, message->includeQualifiers);
    XmlWriter::appendValueElement(out, message->includeClassOrigin);
    _serializeCIMPropertyList(out, message->propertyList);
}

//
// _serializeCIMReferenceNamesRequestMessage
//
void CIMMessageSerializer::_serializeCIMReferenceNamesRequestMessage(
    Array<char>& out,
    CIMReferenceNamesRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMObjectPath(out, message->objectName);
    _serializeCIMName(out, message->resultClass);
    XmlWriter::appendValueElement(out, message->role);
}

//
// _serializeCIMGetPropertyRequestMessage
//
void CIMMessageSerializer::_serializeCIMGetPropertyRequestMessage(
    Array<char>& out,
    CIMGetPropertyRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMObjectPath(out, message->instanceName);
    _serializeCIMName(out, message->propertyName);
}

//
// _serializeCIMSetPropertyRequestMessage
//
void CIMMessageSerializer::_serializeCIMSetPropertyRequestMessage(
    Array<char>& out,
    CIMSetPropertyRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMObjectPath(out, message->instanceName);

    // Use PARAMVALUE element so we can preserve the CIMType information
    XmlWriter::appendParamValueElement(
        out,
        CIMParamValue(
            message->propertyName.getString(), message->newValue, true));
}

//
// _serializeCIMInvokeMethodRequestMessage
//
void CIMMessageSerializer::_serializeCIMInvokeMethodRequestMessage(
    Array<char>& out,
    CIMInvokeMethodRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMObjectPath(out, message->instanceName);
    _serializeCIMName(out, message->methodName);

    // Use PGPARAMS element so we can find the end of the PARAMVALUE elements
    XmlWriter::append(out, "<PGPARAMS>\n");
    for (Uint32 i=0; i < message->inParameters.size(); i++)
    {
        XmlWriter::appendParamValueElement(out, message->inParameters[i]);
    }
    XmlWriter::append(out, "</PGPARAMS>\n");
}


//
//
// CIMIndicationRequestMessages
//
//

//
// _serializeCIMCreateSubscriptionRequestMessage
//
void CIMMessageSerializer::_serializeCIMCreateSubscriptionRequestMessage(
    Array<char>& out,
    CIMCreateSubscriptionRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMNamespaceName(out, message->nameSpace);

    _serializeCIMInstance(out, message->subscriptionInstance);

    // Use PGNAMEARRAY element to encapsulate the CIMName elements
    XmlWriter::append(out, "<PGNAMEARRAY>\n");
    for (Uint32 i=0; i < message->classNames.size(); i++)
    {
        _serializeCIMName(out, message->classNames[i]);
    }
    XmlWriter::append(out, "</PGNAMEARRAY>\n");

    _serializeCIMPropertyList(out, message->propertyList);

    // Encode message->repeatNotificationPolicy as an integer
    XmlWriter::appendValueElement(out, message->repeatNotificationPolicy);

    XmlWriter::appendValueElement(out, message->query);
}

//
// _serializeCIMModifySubscriptionRequestMessage
//
void CIMMessageSerializer::_serializeCIMModifySubscriptionRequestMessage(
    Array<char>& out,
    CIMModifySubscriptionRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMNamespaceName(out, message->nameSpace);

    _serializeCIMInstance(out, message->subscriptionInstance);

    // Use PGNAMEARRAY element to encapsulate the CIMName elements
    XmlWriter::append(out, "<PGNAMEARRAY>\n");
    for (Uint32 i=0; i < message->classNames.size(); i++)
    {
        _serializeCIMName(out, message->classNames[i]);
    }
    XmlWriter::append(out, "</PGNAMEARRAY>\n");

    _serializeCIMPropertyList(out, message->propertyList);

    // Encode message->repeatNotificationPolicy as an integer
    XmlWriter::appendValueElement(out, message->repeatNotificationPolicy);

    XmlWriter::appendValueElement(out, message->query);
}

//
// _serializeCIMDeleteSubscriptionRequestMessage
//
void CIMMessageSerializer::_serializeCIMDeleteSubscriptionRequestMessage(
    Array<char>& out,
    CIMDeleteSubscriptionRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMNamespaceName(out, message->nameSpace);

    _serializeCIMInstance(out, message->subscriptionInstance);

    // Use PGNAMEARRAY element to encapsulate the CIMName elements
    XmlWriter::append(out, "<PGNAMEARRAY>\n");
    for (Uint32 i=0; i < message->classNames.size(); i++)
    {
        _serializeCIMName(out, message->classNames[i]);
    }
    XmlWriter::append(out, "</PGNAMEARRAY>\n");
}


//
//
// Other CIMRequestMessages
//
//

//
// _serializeCIMExportIndicationRequestMessage
//
void CIMMessageSerializer::_serializeCIMExportIndicationRequestMessage(
    Array<char>& out,
    CIMExportIndicationRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    XmlWriter::appendValueElement(out, message->destinationPath);
    _serializeCIMInstance(out, message->indicationInstance);
}

//
// _serializeCIMProcessIndicationRequestMessage
//
void CIMMessageSerializer::_serializeCIMProcessIndicationRequestMessage(
    Array<char>& out,
    CIMProcessIndicationRequestMessage* message)
{
    _serializeCIMNamespaceName(out, message->nameSpace);

    _serializeCIMInstance(out, message->indicationInstance);

    // Use PGPATHARRAY element to encapsulate the object path elements
    XmlWriter::append(out, "<PGPATHARRAY>\n");
    for (Uint32 i=0; i < message->subscriptionInstanceNames.size(); i++)
    {
        _serializeCIMObjectPath(out, message->subscriptionInstanceNames[i]);
    }
    XmlWriter::append(out, "</PGPATHARRAY>\n");

    _serializeCIMInstance(out, message->provider);
}

//
// _serializeCIMDisableModuleRequestMessage
//
void CIMMessageSerializer::_serializeCIMDisableModuleRequestMessage(
    Array<char>& out,
    CIMDisableModuleRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMInstance(out, message->providerModule);

    // Use PGINSTARRAY element to encapsulate the CIMInstance elements
    XmlWriter::append(out, "<PGINSTARRAY>\n");
    for (Uint32 i=0; i < message->providers.size(); i++)
    {
        _serializeCIMInstance(out, message->providers[i]);
    }
    XmlWriter::append(out, "</PGINSTARRAY>\n");

    XmlWriter::appendValueElement(out, message->disableProviderOnly);

    // Use PGBOOLARRAY element to encapsulate the Boolean elements
    XmlWriter::append(out, "<PGBOOLARRAY>\n");
    for (Uint32 i=0; i < message->indicationProviders.size(); i++)
    {
        XmlWriter::appendValueElement(out, message->indicationProviders[i]);
    }
    XmlWriter::append(out, "</PGBOOLARRAY>\n");
}

//
// _serializeCIMEnableModuleRequestMessage
//
void CIMMessageSerializer::_serializeCIMEnableModuleRequestMessage(
    Array<char>& out,
    CIMEnableModuleRequestMessage* message)
{
    _serializeUserInfo(out, message->authType, message->userName);

    _serializeCIMInstance(out, message->providerModule);
}

//
// _serializeCIMStopAllProvidersRequestMessage
//
void CIMMessageSerializer::_serializeCIMStopAllProvidersRequestMessage(
    Array<char>& out,
    CIMStopAllProvidersRequestMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMInitializeProviderRequestMessage
//
void CIMMessageSerializer::_serializeCIMInitializeProviderRequestMessage(
    Array<char>& out,
    CIMInitializeProviderRequestMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMInitializeProviderAgentRequestMessage
//
void CIMMessageSerializer::_serializeCIMInitializeProviderAgentRequestMessage(
    Array<char>& out,
    CIMInitializeProviderAgentRequestMessage* message)
{
    XmlWriter::appendValueElement(out, message->pegasusHome);

    // Use PGCONFARRAY element to encapsulate the config property elements
    XmlWriter::append(out, "<PGCONFARRAY>\n");
    for (Uint32 i=0; i < message->configProperties.size(); i++)
    {
        XmlWriter::appendValueElement(out, message->configProperties[i].first);
        XmlWriter::appendValueElement(out, message->configProperties[i].second);
    }
    XmlWriter::append(out, "</PGCONFARRAY>\n");

    XmlWriter::appendValueElement(out, message->bindVerbose);

    XmlWriter::appendValueElement(out, message->subscriptionInitComplete);
}

//
// _serializeCIMNotifyConfigChangeRequestMessage
//
void CIMMessageSerializer::_serializeCIMNotifyConfigChangeRequestMessage(
    Array<char>& out,
    CIMNotifyConfigChangeRequestMessage* message)
{
    XmlWriter::appendValueElement(out, message->propertyName);
    XmlWriter::appendValueElement(out, message->newPropertyValue);
    XmlWriter::appendValueElement(out, message->currentValueModified);
}

//
// _serializeCIMSubscriptionInitCompleteRequestMessage
//
void 
CIMMessageSerializer::_serializeCIMSubscriptionInitCompleteRequestMessage(
    Array<char>& out,
    CIMSubscriptionInitCompleteRequestMessage* message)
{
    // No additional attributes to serialize!
}

//
//
// Response Messages
//
//

//
//
// CIM Operation Response Messages
//
//

//
// _serializeCIMGetInstanceResponseMessage
//
void CIMMessageSerializer::_serializeCIMGetInstanceResponseMessage(
    Array<char>& out,
    CIMGetInstanceResponseMessage* message)
{
    _serializeCIMInstance(out, message->cimInstance);
}

//
// _serializeCIMDeleteInstanceResponseMessage
//
void CIMMessageSerializer::_serializeCIMDeleteInstanceResponseMessage(
    Array<char>& out,
    CIMDeleteInstanceResponseMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMCreateInstanceResponseMessage
//
void CIMMessageSerializer::_serializeCIMCreateInstanceResponseMessage(
    Array<char>& out,
    CIMCreateInstanceResponseMessage* message)
{
    _serializeCIMObjectPath(out, message->instanceName);
}

//
// _serializeCIMModifyInstanceResponseMessage
//
void CIMMessageSerializer::_serializeCIMModifyInstanceResponseMessage(
    Array<char>& out,
    CIMModifyInstanceResponseMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMEnumerateInstancesResponseMessage
//
void CIMMessageSerializer::_serializeCIMEnumerateInstancesResponseMessage(
    Array<char>& out,
    CIMEnumerateInstancesResponseMessage* message)
{
    // Use PGINSTARRAY element to encapsulate the CIMInstance elements
    XmlWriter::append(out, "<PGINSTARRAY>\n");
    for (Uint32 i=0; i < message->cimNamedInstances.size(); i++)
    {
        _serializeCIMInstance(out, message->cimNamedInstances[i]);
    }
    XmlWriter::append(out, "</PGINSTARRAY>\n");
}

//
// _serializeCIMEnumerateInstanceNamesResponseMessage
//
void CIMMessageSerializer::_serializeCIMEnumerateInstanceNamesResponseMessage(
    Array<char>& out,
    CIMEnumerateInstanceNamesResponseMessage* message)
{
    // Use PGPATHARRAY element to encapsulate the object path elements
    XmlWriter::append(out, "<PGPATHARRAY>\n");
    for (Uint32 i=0; i < message->instanceNames.size(); i++)
    {
        _serializeCIMObjectPath(out, message->instanceNames[i]);
    }
    XmlWriter::append(out, "</PGPATHARRAY>\n");
}

//
// _serializeCIMExecQueryResponseMessage
//
void CIMMessageSerializer::_serializeCIMExecQueryResponseMessage(
    Array<char>& out,
    CIMExecQueryResponseMessage* message)
{
    // Use PGOBJARRAY element to encapsulate the CIMObject elements
    XmlWriter::append(out, "<PGOBJARRAY>\n");
    for (Uint32 i=0; i < message->cimObjects.size(); i++)
    {
        _serializeCIMObject(out, message->cimObjects[i]);
    }
    XmlWriter::append(out, "</PGOBJARRAY>\n");
}

//
// _serializeCIMAssociatorsResponseMessage
//
void CIMMessageSerializer::_serializeCIMAssociatorsResponseMessage(
    Array<char>& out,
    CIMAssociatorsResponseMessage* message)
{
    // Use PGOBJARRAY element to encapsulate the CIMObject elements
    XmlWriter::append(out, "<PGOBJARRAY>\n");
    for (Uint32 i=0; i < message->cimObjects.size(); i++)
    {
        _serializeCIMObject(out, message->cimObjects[i]);
    }
    XmlWriter::append(out, "</PGOBJARRAY>\n");
}

//
// _serializeCIMAssociatorNamesResponseMessage
//
void CIMMessageSerializer::_serializeCIMAssociatorNamesResponseMessage(
    Array<char>& out,
    CIMAssociatorNamesResponseMessage* message)
{
    // Use PGPATHARRAY element to encapsulate the object path elements
    XmlWriter::append(out, "<PGPATHARRAY>\n");
    for (Uint32 i=0; i < message->objectNames.size(); i++)
    {
        _serializeCIMObjectPath(out, message->objectNames[i]);
    }
    XmlWriter::append(out, "</PGPATHARRAY>\n");
}

//
// _serializeCIMReferencesResponseMessage
//
void CIMMessageSerializer::_serializeCIMReferencesResponseMessage(
    Array<char>& out,
    CIMReferencesResponseMessage* message)
{
    // Use PGOBJARRAY element to encapsulate the CIMObject elements
    XmlWriter::append(out, "<PGOBJARRAY>\n");
    for (Uint32 i=0; i < message->cimObjects.size(); i++)
    {
        _serializeCIMObject(out, message->cimObjects[i]);
    }
    XmlWriter::append(out, "</PGOBJARRAY>\n");
}

//
// _serializeCIMReferenceNamesResponseMessage
//
void CIMMessageSerializer::_serializeCIMReferenceNamesResponseMessage(
    Array<char>& out,
    CIMReferenceNamesResponseMessage* message)
{
    // Use PGPATHARRAY element to encapsulate the object path elements
    XmlWriter::append(out, "<PGPATHARRAY>\n");
    for (Uint32 i=0; i < message->objectNames.size(); i++)
    {
        _serializeCIMObjectPath(out, message->objectNames[i]);
    }
    XmlWriter::append(out, "</PGPATHARRAY>\n");
}

//
// _serializeCIMGetPropertyResponseMessage
//
void CIMMessageSerializer::_serializeCIMGetPropertyResponseMessage(
    Array<char>& out,
    CIMGetPropertyResponseMessage* message)
{
    // Use PARAMVALUE element so we can preserve the CIMType information
    XmlWriter::appendParamValueElement(
        out,
        CIMParamValue(String("ignore"), message->value, true));
}

//
// _serializeCIMSetPropertyResponseMessage
//
void CIMMessageSerializer::_serializeCIMSetPropertyResponseMessage(
    Array<char>& out,
    CIMSetPropertyResponseMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMInvokeMethodResponseMessage
//
void CIMMessageSerializer::_serializeCIMInvokeMethodResponseMessage(
    Array<char>& out,
    CIMInvokeMethodResponseMessage* message)
{
    // Use PARAMVALUE element so we can preserve the CIMType information
    XmlWriter::appendParamValueElement(
        out,
        CIMParamValue(String("ignore"), message->retValue, true));

    // Use PGPARAMS element so we can find the end of the PARAMVALUE elements
    XmlWriter::append(out, "<PGPARAMS>\n");
    for (Uint32 i=0; i < message->outParameters.size(); i++)
    {
        XmlWriter::appendParamValueElement(out, message->outParameters[i]);
    }
    XmlWriter::append(out, "</PGPARAMS>\n");

    _serializeCIMName(out, message->methodName);
}


//
//
// CIM Indication Response Messages
//
//

//
// _serializeCIMCreateSubscriptionResponseMessage
//
void CIMMessageSerializer::_serializeCIMCreateSubscriptionResponseMessage(
    Array<char>& out,
    CIMCreateSubscriptionResponseMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMModifySubscriptionResponseMessage
//
void CIMMessageSerializer::_serializeCIMModifySubscriptionResponseMessage(
    Array<char>& out,
    CIMModifySubscriptionResponseMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMDeleteSubscriptionResponseMessage
//
void CIMMessageSerializer::_serializeCIMDeleteSubscriptionResponseMessage(
    Array<char>& out,
    CIMDeleteSubscriptionResponseMessage* message)
{
    // No additional attributes to serialize!
}


//
//
// Other CIMResponseMessages
//
//

//
// _serializeCIMExportIndicationResponseMessage
//
void CIMMessageSerializer::_serializeCIMExportIndicationResponseMessage(
    Array<char>& out,
    CIMExportIndicationResponseMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMProcessIndicationResponseMessage
//
void CIMMessageSerializer::_serializeCIMProcessIndicationResponseMessage(
    Array<char>& out,
    CIMProcessIndicationResponseMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMDisableModuleResponseMessage
//
void CIMMessageSerializer::_serializeCIMDisableModuleResponseMessage(
    Array<char>& out,
    CIMDisableModuleResponseMessage* message)
{
    // Use PGUINT16ARRAY element to encapsulate the Uint16 elements
    XmlWriter::append(out, "<PGUINT16ARRAY>\n");
    for (Uint32 i=0; i < message->operationalStatus.size(); i++)
    {
        XmlWriter::appendValueElement(out, message->operationalStatus[i]);
    }
    XmlWriter::append(out, "</PGUINT16ARRAY>\n");
}

//
// _serializeCIMEnableModuleResponseMessage
//
void CIMMessageSerializer::_serializeCIMEnableModuleResponseMessage(
    Array<char>& out,
    CIMEnableModuleResponseMessage* message)
{
    // Use PGUINT16ARRAY element to encapsulate the Uint16 elements
    XmlWriter::append(out, "<PGUINT16ARRAY>\n");
    for (Uint32 i=0; i < message->operationalStatus.size(); i++)
    {
        XmlWriter::appendValueElement(out, message->operationalStatus[i]);
    }
    XmlWriter::append(out, "</PGUINT16ARRAY>\n");
}

//
// _serializeCIMStopAllProvidersResponseMessage
//
void CIMMessageSerializer::_serializeCIMStopAllProvidersResponseMessage(
    Array<char>& out,
    CIMStopAllProvidersResponseMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMInitializeProviderResponseMessage
//
void CIMMessageSerializer::_serializeCIMInitializeProviderResponseMessage(
    Array<char>& out,
    CIMInitializeProviderResponseMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMInitializeProviderAgentResponseMessage
//
void CIMMessageSerializer::_serializeCIMInitializeProviderAgentResponseMessage(
    Array<char>& out,
    CIMInitializeProviderAgentResponseMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMNotifyConfigChangeResponseMessage
//
void CIMMessageSerializer::_serializeCIMNotifyConfigChangeResponseMessage(
    Array<char>& out,
    CIMNotifyConfigChangeResponseMessage* message)
{
    // No additional attributes to serialize!
}

//
// _serializeCIMSubscriptionInitCompleteResponseMessage
//
void 
CIMMessageSerializer::_serializeCIMSubscriptionInitCompleteResponseMessage
   (Array<char>& out,
    CIMSubscriptionInitCompleteResponseMessage* message)
{
    // No additional attributes to serialize!
}

PEGASUS_NAMESPACE_END
