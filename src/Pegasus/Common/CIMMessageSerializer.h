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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMMessageSerializer_h
#define Pegasus_CIMMessageSerializer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMMessage.h>

PEGASUS_NAMESPACE_BEGIN

/**
    CIMMessageSerializer provides a mechanism to convert a CIMMessage (or one
    of its subclasses) to a stream of bytes.  This stream of bytes can be
    converted back to a CIMMessage object through use of the related
    CIMMessageDeserializer class.

    The format of the serialized message is not defined and is therefore
    subject to change.  The only requirement is that the CIMMessageSerializer
    and CIMMessageDeserializer classes remain in sync to provide a two-way
    mapping.  (A quasi-XML encoding is currently used as an expedient
    solution.  However, this encoding is not compliant with the CIM-XML
    specification.  A number of shortcuts have been taken to improve
    operational efficiency.)

    Note:  Changes or additions to the CIMMessage definitions must be
    reflected in these serialization classes.  Likewise, changes to the
    structure of member data (such as the AcceptLanguageList class) and
    addition of new OperationContext containers will affect message
    serialization.
 */
class PEGASUS_COMMON_LINKAGE CIMMessageSerializer
{
public:

    static void serialize(Buffer& out, CIMMessage* cimMessage);

private:

    static void _serializeCIMRequestMessage(
        Buffer& out,
        CIMRequestMessage* cimMessage);

    static void _serializeCIMResponseMessage(
        Buffer& out,
        CIMResponseMessage* cimMessage);

    //
    // Utility Methods
    //

    static void _serializeUserInfo(
        Buffer& out,
        const String& authType,
        const String& userName);

    static void _serializeQueueIdStack(
        Buffer& out,
        const QueueIdStack& queueIdStack);

    static void _serializeOperationContext(
        Buffer& out,
        const OperationContext& operationContext);

    static void _serializeContentLanguageList(
        Buffer& out,
        const ContentLanguageList& contentLanguages);

    static void _serializeAcceptLanguageList(
        Buffer& out,
        const AcceptLanguageList& acceptLanguages);

    static void _serializeCIMException(
        Buffer& out,
        const CIMException& cimException);

    static void _serializeCIMPropertyList(
        Buffer& out,
        const CIMPropertyList& cimPropertyList);

    static void _serializeCIMObjectPath(
        Buffer& out,
        const CIMObjectPath& cimObjectPath);

    static void _serializeCIMInstance(
        Buffer& out,
        const CIMInstance& cimInstance);

    static void _serializeCIMNamespaceName(
        Buffer& out,
        const CIMNamespaceName& cimNamespaceName);

    static void _serializeCIMName(
        Buffer& out,
        const CIMName& cimName);

    static void _serializeCIMObject(
        Buffer& out,
        const CIMObject& object);

    //
    //
    // CIM Request Messages
    //
    //

    //
    // CIMOperationRequestMessages
    //

    static void _serializeCIMGetInstanceRequestMessage(
        Buffer& out,
        CIMGetInstanceRequestMessage* message);

    static void _serializeCIMDeleteInstanceRequestMessage(
        Buffer& out,
        CIMDeleteInstanceRequestMessage* message);

    static void _serializeCIMCreateInstanceRequestMessage(
        Buffer& out,
        CIMCreateInstanceRequestMessage* message);

    static void _serializeCIMModifyInstanceRequestMessage(
        Buffer& out,
        CIMModifyInstanceRequestMessage* message);

    static void _serializeCIMEnumerateInstancesRequestMessage(
        Buffer& out,
        CIMEnumerateInstancesRequestMessage* message);

    static void _serializeCIMEnumerateInstanceNamesRequestMessage(
        Buffer& out,
        CIMEnumerateInstanceNamesRequestMessage* message);

    static void _serializeCIMExecQueryRequestMessage(
        Buffer& out,
        CIMExecQueryRequestMessage* message);

    static void _serializeCIMAssociatorsRequestMessage(
        Buffer& out,
        CIMAssociatorsRequestMessage* message);

    static void _serializeCIMAssociatorNamesRequestMessage(
        Buffer& out,
        CIMAssociatorNamesRequestMessage* message);

    static void _serializeCIMReferencesRequestMessage(
        Buffer& out,
        CIMReferencesRequestMessage* message);

    static void _serializeCIMReferenceNamesRequestMessage(
        Buffer& out,
        CIMReferenceNamesRequestMessage* message);

    static void _serializeCIMGetPropertyRequestMessage(
        Buffer& out,
        CIMGetPropertyRequestMessage* message);

    static void _serializeCIMSetPropertyRequestMessage(
        Buffer& out,
        CIMSetPropertyRequestMessage* message);

    static void _serializeCIMInvokeMethodRequestMessage(
        Buffer& out,
        CIMInvokeMethodRequestMessage* message);

    //
    // CIMIndicationRequestMessages
    //

    static void _serializeCIMCreateSubscriptionRequestMessage(
        Buffer& out,
        CIMCreateSubscriptionRequestMessage* message);

    static void _serializeCIMModifySubscriptionRequestMessage(
        Buffer& out,
        CIMModifySubscriptionRequestMessage* message);

    static void _serializeCIMDeleteSubscriptionRequestMessage(
        Buffer& out,
        CIMDeleteSubscriptionRequestMessage* message);

    //
    // Generic CIMRequestMessages
    //

    static void _serializeCIMExportIndicationRequestMessage(
        Buffer& out,
        CIMExportIndicationRequestMessage* message);

    static void _serializeCIMProcessIndicationRequestMessage(
        Buffer& out,
        CIMProcessIndicationRequestMessage* message);

    static void _serializeCIMDisableModuleRequestMessage(
        Buffer& out,
        CIMDisableModuleRequestMessage* message);

    static void _serializeCIMEnableModuleRequestMessage(
        Buffer& out,
        CIMEnableModuleRequestMessage* message);

    static void _serializeCIMStopAllProvidersRequestMessage(
        Buffer& out,
        CIMStopAllProvidersRequestMessage* message);

    static void _serializeCIMInitializeProviderRequestMessage(
        Buffer& out,
        CIMInitializeProviderRequestMessage* message);

    static void _serializeCIMInitializeProviderAgentRequestMessage(
        Buffer& out,
        CIMInitializeProviderAgentRequestMessage* message);

    static void _serializeCIMNotifyConfigChangeRequestMessage(
        Buffer& out,
        CIMNotifyConfigChangeRequestMessage* message);

    static void _serializeCIMSubscriptionInitCompleteRequestMessage(
        Buffer& out,
        CIMSubscriptionInitCompleteRequestMessage* message);


    //
    //
    // CIM Response Messages
    //
    //

    //
    // CIMOperationResponseMessages
    //

    static void _serializeCIMGetInstanceResponseMessage(
        Buffer& out,
        CIMGetInstanceResponseMessage* message);

    static void _serializeCIMDeleteInstanceResponseMessage(
        Buffer& out,
        CIMDeleteInstanceResponseMessage* message);

    static void _serializeCIMCreateInstanceResponseMessage(
        Buffer& out,
        CIMCreateInstanceResponseMessage* message);

    static void _serializeCIMModifyInstanceResponseMessage(
        Buffer& out,
        CIMModifyInstanceResponseMessage* message);

    static void _serializeCIMEnumerateInstancesResponseMessage(
        Buffer& out,
        CIMEnumerateInstancesResponseMessage* message);

    static void _serializeCIMEnumerateInstanceNamesResponseMessage(
        Buffer& out,
        CIMEnumerateInstanceNamesResponseMessage* message);

    static void _serializeCIMExecQueryResponseMessage(
        Buffer& out,
        CIMExecQueryResponseMessage* message);

    static void _serializeCIMAssociatorsResponseMessage(
        Buffer& out,
        CIMAssociatorsResponseMessage* message);

    static void _serializeCIMAssociatorNamesResponseMessage(
        Buffer& out,
        CIMAssociatorNamesResponseMessage* message);

    static void _serializeCIMReferencesResponseMessage(
        Buffer& out,
        CIMReferencesResponseMessage* message);

    static void _serializeCIMReferenceNamesResponseMessage(
        Buffer& out,
        CIMReferenceNamesResponseMessage* message);

    static void _serializeCIMGetPropertyResponseMessage(
        Buffer& out,
        CIMGetPropertyResponseMessage* message);

    static void _serializeCIMSetPropertyResponseMessage(
        Buffer& out,
        CIMSetPropertyResponseMessage* message);

    static void _serializeCIMInvokeMethodResponseMessage(
        Buffer& out,
        CIMInvokeMethodResponseMessage* message);

    //
    // CIMIndicationResponseMessages
    //

    static void _serializeCIMCreateSubscriptionResponseMessage(
        Buffer& out,
        CIMCreateSubscriptionResponseMessage* message);

    static void _serializeCIMModifySubscriptionResponseMessage(
        Buffer& out,
        CIMModifySubscriptionResponseMessage* message);

    static void _serializeCIMDeleteSubscriptionResponseMessage(
        Buffer& out,
        CIMDeleteSubscriptionResponseMessage* message);

    //
    // Generic CIMResponseMessages
    //

    static void _serializeCIMExportIndicationResponseMessage(
        Buffer& out,
        CIMExportIndicationResponseMessage* message);

    static void _serializeCIMProcessIndicationResponseMessage(
        Buffer& out,
        CIMProcessIndicationResponseMessage* message);

    static void _serializeCIMDisableModuleResponseMessage(
        Buffer& out,
        CIMDisableModuleResponseMessage* message);

    static void _serializeCIMEnableModuleResponseMessage(
        Buffer& out,
        CIMEnableModuleResponseMessage* message);

    static void _serializeCIMStopAllProvidersResponseMessage(
        Buffer& out,
        CIMStopAllProvidersResponseMessage* message);

    static void _serializeCIMInitializeProviderResponseMessage(
        Buffer& out,
        CIMInitializeProviderResponseMessage* message);

    static void _serializeCIMInitializeProviderAgentResponseMessage(
        Buffer& out,
        CIMInitializeProviderAgentResponseMessage* message);

    static void _serializeCIMNotifyConfigChangeResponseMessage(
        Buffer& out,
        CIMNotifyConfigChangeResponseMessage* message);

    static void _serializeCIMSubscriptionInitCompleteResponseMessage(
        Buffer& out,
        CIMSubscriptionInitCompleteResponseMessage* message);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMMessageSerializer_h */
