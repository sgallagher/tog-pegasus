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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMMessageDeserializer_h
#define Pegasus_CIMMessageDeserializer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/XmlParser.h>

PEGASUS_NAMESPACE_BEGIN

/**
    CIMMessageDeserializer provides a mechanism to convert a stream of bytes
    generated by the CIMMessageSerializer class into a CIMMessage object.
    For additional information, please see the CIMMessageSerializer
    description.
 */
class PEGASUS_COMMON_LINKAGE CIMMessageDeserializer
{
public:

    static CIMMessage* deserialize(char* buffer);

private:

    PEGASUS_HIDDEN_LINKAGE
    static CIMRequestMessage* _deserializeCIMRequestMessage(
        XmlParser& parser,
        MessageType type);

    PEGASUS_HIDDEN_LINKAGE
    static CIMResponseMessage* _deserializeCIMResponseMessage(
        XmlParser& parser,
        MessageType type);

    //
    // Utility Methods
    //

    PEGASUS_HIDDEN_LINKAGE
    static void _deserializeUserInfo(
        XmlParser& parser,
        String& authType,
        String& userName);

    PEGASUS_HIDDEN_LINKAGE
    static void _deserializeQueueIdStack(
        XmlParser& parser,
        QueueIdStack& queueIdStack);

    PEGASUS_HIDDEN_LINKAGE
    static void _deserializeOperationContext(
        XmlParser& parser,
        OperationContext& operationContext);

    PEGASUS_HIDDEN_LINKAGE
    static void _deserializeContentLanguageList(
        XmlParser& parser,
        ContentLanguageList& contentLanguages);

    PEGASUS_HIDDEN_LINKAGE
    static void _deserializeAcceptLanguageList(
        XmlParser& parser,
        AcceptLanguageList& acceptLanguages);

    PEGASUS_HIDDEN_LINKAGE
    static void _deserializeCIMException(
        XmlParser& parser,
        CIMException& cimException);

    PEGASUS_HIDDEN_LINKAGE
    static void _deserializeCIMPropertyList(
        XmlParser& parser,
        CIMPropertyList& propertyList);

    PEGASUS_HIDDEN_LINKAGE
    static Boolean _deserializeCIMObjectPath(
        XmlParser& parser,
        CIMObjectPath& cimObjectPath);

    PEGASUS_HIDDEN_LINKAGE
    static Boolean _deserializeCIMInstance(
        XmlParser& parser,
        CIMInstance& cimInstance);

    PEGASUS_HIDDEN_LINKAGE
    static void _deserializeCIMNamespaceName(
        XmlParser& parser,
        CIMNamespaceName& cimNamespaceName);

    PEGASUS_HIDDEN_LINKAGE
    static Boolean _deserializeCIMName(
        XmlParser& parser,
        CIMName& cimName);

    PEGASUS_HIDDEN_LINKAGE
    static Boolean _deserializeCIMObject(
        XmlParser& parser,
        CIMObject& object);

    PEGASUS_HIDDEN_LINKAGE
    static Boolean _deserializeCIMParamValue(
        XmlParser& parser,
        CIMParamValue& paramValue);

    //
    //
    // CIM Request Messages
    //
    //

    //
    // CIMOperationRequestMessages
    //

    PEGASUS_HIDDEN_LINKAGE
    static CIMGetInstanceRequestMessage*
        _deserializeCIMGetInstanceRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMDeleteInstanceRequestMessage*
        _deserializeCIMDeleteInstanceRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMCreateInstanceRequestMessage*
        _deserializeCIMCreateInstanceRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMModifyInstanceRequestMessage*
        _deserializeCIMModifyInstanceRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMEnumerateInstancesRequestMessage*
        _deserializeCIMEnumerateInstancesRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMEnumerateInstanceNamesRequestMessage*
        _deserializeCIMEnumerateInstanceNamesRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMExecQueryRequestMessage*
        _deserializeCIMExecQueryRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMAssociatorsRequestMessage*
        _deserializeCIMAssociatorsRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMAssociatorNamesRequestMessage*
        _deserializeCIMAssociatorNamesRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMReferencesRequestMessage*
        _deserializeCIMReferencesRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMReferenceNamesRequestMessage*
        _deserializeCIMReferenceNamesRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMGetPropertyRequestMessage*
        _deserializeCIMGetPropertyRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMSetPropertyRequestMessage*
        _deserializeCIMSetPropertyRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMInvokeMethodRequestMessage*
        _deserializeCIMInvokeMethodRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMInitializeProviderAgentRequestMessage*
        _deserializeCIMInitializeProviderAgentRequestMessage(XmlParser& parser);

    //
    // CIMIndicationRequestMessages
    //

    static CIMCreateSubscriptionRequestMessage*
        _deserializeCIMCreateSubscriptionRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMModifySubscriptionRequestMessage*
        _deserializeCIMModifySubscriptionRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMDeleteSubscriptionRequestMessage*
        _deserializeCIMDeleteSubscriptionRequestMessage(XmlParser& parser);

    //
    // Generic CIMRequestMessages
    //

    PEGASUS_HIDDEN_LINKAGE
    static CIMExportIndicationRequestMessage*
        _deserializeCIMExportIndicationRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMProcessIndicationRequestMessage*
        _deserializeCIMProcessIndicationRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMDisableModuleRequestMessage*
        _deserializeCIMDisableModuleRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMEnableModuleRequestMessage*
        _deserializeCIMEnableModuleRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMStopAllProvidersRequestMessage*
        _deserializeCIMStopAllProvidersRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMNotifyConfigChangeRequestMessage*
        _deserializeCIMNotifyConfigChangeRequestMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMSubscriptionInitCompleteRequestMessage*
        _deserializeCIMSubscriptionInitCompleteRequestMessage
            (XmlParser& parser);

    //
    //
    // CIM Response Messages
    //
    //

    //
    // CIM Operation Response Messages
    //

    PEGASUS_HIDDEN_LINKAGE
    static CIMGetInstanceResponseMessage*
        _deserializeCIMGetInstanceResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMDeleteInstanceResponseMessage*
        _deserializeCIMDeleteInstanceResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMCreateInstanceResponseMessage*
        _deserializeCIMCreateInstanceResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMModifyInstanceResponseMessage*
        _deserializeCIMModifyInstanceResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMEnumerateInstancesResponseMessage*
        _deserializeCIMEnumerateInstancesResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMEnumerateInstanceNamesResponseMessage*
        _deserializeCIMEnumerateInstanceNamesResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMExecQueryResponseMessage*
        _deserializeCIMExecQueryResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMAssociatorsResponseMessage*
        _deserializeCIMAssociatorsResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMAssociatorNamesResponseMessage*
        _deserializeCIMAssociatorNamesResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMReferencesResponseMessage*
        _deserializeCIMReferencesResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMReferenceNamesResponseMessage*
        _deserializeCIMReferenceNamesResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMGetPropertyResponseMessage*
        _deserializeCIMGetPropertyResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMSetPropertyResponseMessage*
        _deserializeCIMSetPropertyResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMInvokeMethodResponseMessage*
        _deserializeCIMInvokeMethodResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMInitializeProviderAgentResponseMessage*
        _deserializeCIMInitializeProviderAgentResponseMessage(
            XmlParser& parser);

    //
    // CIM Indication Response Messages
    //

    PEGASUS_HIDDEN_LINKAGE
    static CIMCreateSubscriptionResponseMessage*
        _deserializeCIMCreateSubscriptionResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMModifySubscriptionResponseMessage*
        _deserializeCIMModifySubscriptionResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMDeleteSubscriptionResponseMessage*
        _deserializeCIMDeleteSubscriptionResponseMessage(XmlParser& parser);

    //
    // Generic CIMResponseMessages
    //

    PEGASUS_HIDDEN_LINKAGE
    static CIMExportIndicationResponseMessage*
        _deserializeCIMExportIndicationResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMProcessIndicationResponseMessage*
        _deserializeCIMProcessIndicationResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMDisableModuleResponseMessage*
        _deserializeCIMDisableModuleResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMEnableModuleResponseMessage*
        _deserializeCIMEnableModuleResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMStopAllProvidersResponseMessage*
        _deserializeCIMStopAllProvidersResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMNotifyConfigChangeResponseMessage*
        _deserializeCIMNotifyConfigChangeResponseMessage(XmlParser& parser);

    PEGASUS_HIDDEN_LINKAGE
    static CIMSubscriptionInitCompleteResponseMessage*
        _deserializeCIMSubscriptionInitCompleteResponseMessage
            (XmlParser& parser);

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMMessageDeserializer_h */
