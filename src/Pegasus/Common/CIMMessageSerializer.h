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
    structure of member data (such as the AcceptLanguages class) and
    addition of new OperationContext containers will affect message
    serialization.
 */
class PEGASUS_COMMON_LINKAGE CIMMessageSerializer
{
public:

    static void serialize(Array<char>& out, CIMMessage* cimMessage);

private:

    static void _serializeCIMRequestMessage(
        Array<char>& out,
        CIMRequestMessage* cimMessage);

    static void _serializeCIMResponseMessage(
        Array<char>& out,
        CIMResponseMessage* cimMessage);

    //
    // Utility Methods
    //

    static void _serializeUserInfo(
        Array<char>& out,
        const String& authType,
        const String& userName);

    static void _serializeQueueIdStack(
        Array<char>& out,
        const QueueIdStack& queueIdStack);

    static void _serializeOperationContext(
        Array<char>& out,
        const OperationContext& operationContext);

    static void _serializeContentLanguages(
        Array<char>& out,
        const ContentLanguages& contentLanguages);

    static void _serializeAcceptLanguages(
        Array<char>& out,
        const AcceptLanguages& acceptLanguages);

    static void _serializeCIMException(
        Array<char>& out,
        const CIMException& cimException);

    static void _serializeCIMPropertyList(
        Array<char>& out,
        const CIMPropertyList& cimPropertyList);

    static void _serializeCIMObjectPath(
        Array<char>& out,
        const CIMObjectPath& cimObjectPath);

    static void _serializeCIMInstance(
        Array<char>& out,
        const CIMInstance& cimInstance);

    static void _serializeCIMNamespaceName(
        Array<char>& out,
        const CIMNamespaceName& cimNamespaceName);

    static void _serializeCIMName(
        Array<char>& out,
        const CIMName& cimName);

    static void _serializeCIMObject(
        Array<char>& out,
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
        Array<char>& out,
        CIMGetInstanceRequestMessage* message);

    static void _serializeCIMDeleteInstanceRequestMessage(
        Array<char>& out,
        CIMDeleteInstanceRequestMessage* message);

    static void _serializeCIMCreateInstanceRequestMessage(
        Array<char>& out,
        CIMCreateInstanceRequestMessage* message);

    static void _serializeCIMModifyInstanceRequestMessage(
        Array<char>& out,
        CIMModifyInstanceRequestMessage* message);

    static void _serializeCIMEnumerateInstancesRequestMessage(
        Array<char>& out,
        CIMEnumerateInstancesRequestMessage* message);

    static void _serializeCIMEnumerateInstanceNamesRequestMessage(
        Array<char>& out,
        CIMEnumerateInstanceNamesRequestMessage* message);

    static void _serializeCIMExecQueryRequestMessage(
        Array<char>& out,
        CIMExecQueryRequestMessage* message);

    static void _serializeCIMAssociatorsRequestMessage(
        Array<char>& out,
        CIMAssociatorsRequestMessage* message);

    static void _serializeCIMAssociatorNamesRequestMessage(
        Array<char>& out,
        CIMAssociatorNamesRequestMessage* message);

    static void _serializeCIMReferencesRequestMessage(
        Array<char>& out,
        CIMReferencesRequestMessage* message);

    static void _serializeCIMReferenceNamesRequestMessage(
        Array<char>& out,
        CIMReferenceNamesRequestMessage* message);

    static void _serializeCIMGetPropertyRequestMessage(
        Array<char>& out,
        CIMGetPropertyRequestMessage* message);

    static void _serializeCIMSetPropertyRequestMessage(
        Array<char>& out,
        CIMSetPropertyRequestMessage* message);

    static void _serializeCIMInvokeMethodRequestMessage(
        Array<char>& out,
        CIMInvokeMethodRequestMessage* message);

    //
    // CIMIndicationRequestMessages
    //

    static void _serializeCIMCreateSubscriptionRequestMessage(
        Array<char>& out,
        CIMCreateSubscriptionRequestMessage* message);

    static void _serializeCIMModifySubscriptionRequestMessage(
        Array<char>& out,
        CIMModifySubscriptionRequestMessage* message);

    static void _serializeCIMDeleteSubscriptionRequestMessage(
        Array<char>& out,
        CIMDeleteSubscriptionRequestMessage* message);

    //
    // Generic CIMRequestMessages
    //

    static void _serializeCIMExportIndicationRequestMessage(
        Array<char>& out,
        CIMExportIndicationRequestMessage* message);

    static void _serializeCIMProcessIndicationRequestMessage(
        Array<char>& out,
        CIMProcessIndicationRequestMessage* message);

    static void _serializeCIMDisableModuleRequestMessage(
        Array<char>& out,
        CIMDisableModuleRequestMessage* message);

    static void _serializeCIMEnableModuleRequestMessage(
        Array<char>& out,
        CIMEnableModuleRequestMessage* message);

    static void _serializeCIMStopAllProvidersRequestMessage(
        Array<char>& out,
        CIMStopAllProvidersRequestMessage* message);

    static void _serializeCIMInitializeProviderRequestMessage(
        Array<char>& out,
        CIMInitializeProviderRequestMessage* message);

    static void _serializeCIMInitializeProviderAgentRequestMessage(
        Array<char>& out,
        CIMInitializeProviderAgentRequestMessage* message);

    static void _serializeCIMNotifyConfigChangeRequestMessage(
        Array<char>& out,
        CIMNotifyConfigChangeRequestMessage* message);

    static void _serializeCIMSubscriptionInitCompleteRequestMessage(
        Array<char>& out,
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
        Array<char>& out,
        CIMGetInstanceResponseMessage* message);

    static void _serializeCIMDeleteInstanceResponseMessage(
        Array<char>& out,
        CIMDeleteInstanceResponseMessage* message);

    static void _serializeCIMCreateInstanceResponseMessage(
        Array<char>& out,
        CIMCreateInstanceResponseMessage* message);

    static void _serializeCIMModifyInstanceResponseMessage(
        Array<char>& out,
        CIMModifyInstanceResponseMessage* message);

    static void _serializeCIMEnumerateInstancesResponseMessage(
        Array<char>& out,
        CIMEnumerateInstancesResponseMessage* message);

    static void _serializeCIMEnumerateInstanceNamesResponseMessage(
        Array<char>& out,
        CIMEnumerateInstanceNamesResponseMessage* message);

    static void _serializeCIMExecQueryResponseMessage(
        Array<char>& out,
        CIMExecQueryResponseMessage* message);

    static void _serializeCIMAssociatorsResponseMessage(
        Array<char>& out,
        CIMAssociatorsResponseMessage* message);

    static void _serializeCIMAssociatorNamesResponseMessage(
        Array<char>& out,
        CIMAssociatorNamesResponseMessage* message);

    static void _serializeCIMReferencesResponseMessage(
        Array<char>& out,
        CIMReferencesResponseMessage* message);

    static void _serializeCIMReferenceNamesResponseMessage(
        Array<char>& out,
        CIMReferenceNamesResponseMessage* message);

    static void _serializeCIMGetPropertyResponseMessage(
        Array<char>& out,
        CIMGetPropertyResponseMessage* message);

    static void _serializeCIMSetPropertyResponseMessage(
        Array<char>& out,
        CIMSetPropertyResponseMessage* message);

    static void _serializeCIMInvokeMethodResponseMessage(
        Array<char>& out,
        CIMInvokeMethodResponseMessage* message);

    //
    // CIMIndicationResponseMessages
    //

    static void _serializeCIMCreateSubscriptionResponseMessage(
        Array<char>& out,
        CIMCreateSubscriptionResponseMessage* message);

    static void _serializeCIMModifySubscriptionResponseMessage(
        Array<char>& out,
        CIMModifySubscriptionResponseMessage* message);

    static void _serializeCIMDeleteSubscriptionResponseMessage(
        Array<char>& out,
        CIMDeleteSubscriptionResponseMessage* message);

    //
    // Generic CIMResponseMessages
    //

    static void _serializeCIMExportIndicationResponseMessage(
        Array<char>& out,
        CIMExportIndicationResponseMessage* message);

    static void _serializeCIMProcessIndicationResponseMessage(
        Array<char>& out,
        CIMProcessIndicationResponseMessage* message);

    static void _serializeCIMDisableModuleResponseMessage(
        Array<char>& out,
        CIMDisableModuleResponseMessage* message);

    static void _serializeCIMEnableModuleResponseMessage(
        Array<char>& out,
        CIMEnableModuleResponseMessage* message);

    static void _serializeCIMStopAllProvidersResponseMessage(
        Array<char>& out,
        CIMStopAllProvidersResponseMessage* message);

    static void _serializeCIMInitializeProviderResponseMessage(
        Array<char>& out,
        CIMInitializeProviderResponseMessage* message);

    static void _serializeCIMInitializeProviderAgentResponseMessage(
        Array<char>& out,
        CIMInitializeProviderAgentResponseMessage* message);

    static void _serializeCIMNotifyConfigChangeResponseMessage(
        Array<char>& out,
        CIMNotifyConfigChangeResponseMessage* message);

    static void _serializeCIMSubscriptionInitCompleteResponseMessage(
        Array<char>& out,
        CIMSubscriptionInitCompleteResponseMessage* message);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMMessageSerializer_h */
