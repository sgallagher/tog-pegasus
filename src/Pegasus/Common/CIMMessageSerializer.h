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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
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

    static void serialize(Array<Sint8>& out, CIMMessage* cimMessage);

private:

    static void _serializeCIMRequestMessage(
        Array<Sint8>& out,
        CIMRequestMessage* cimMessage);

    static void _serializeCIMResponseMessage(
        Array<Sint8>& out,
        CIMResponseMessage* cimMessage);

    //
    // Utility Methods
    //

    static void _serializeUserInfo(
        Array<Sint8>& out,
        const String& authType,
        const String& userName);

    static void _serializeQueueIdStack(
        Array<Sint8>& out,
        const QueueIdStack& queueIdStack);

    static void _serializeOperationContext(
        Array<Sint8>& out,
        const OperationContext& operationContext);

    static void _serializeContentLanguages(
        Array<Sint8>& out,
        const ContentLanguages& contentLanguages);

    static void _serializeAcceptLanguages(
        Array<Sint8>& out,
        const AcceptLanguages& acceptLanguages);

    static void _serializeCIMException(
        Array<Sint8>& out,
        const CIMException& cimException);

    static void _serializeCIMPropertyList(
        Array<Sint8>& out,
        const CIMPropertyList& cimPropertyList);

    static void _serializeCIMObjectPath(
        Array<Sint8>& out,
        const CIMObjectPath& cimObjectPath);

    static void _serializeCIMInstance(
        Array<Sint8>& out,
        const CIMInstance& cimInstance);

    static void _serializeCIMNamespaceName(
        Array<Sint8>& out,
        const CIMNamespaceName& cimNamespaceName);

    static void _serializeCIMName(
        Array<Sint8>& out,
        const CIMName& cimName);

    //
    //
    // CIM Request Messages
    //
    //

    //
    // CIMOperationRequestMessages
    //

    static void _serializeCIMGetInstanceRequestMessage(
        Array<Sint8>& out,
        CIMGetInstanceRequestMessage* message);

    static void _serializeCIMDeleteInstanceRequestMessage(
        Array<Sint8>& out,
        CIMDeleteInstanceRequestMessage* message);

    static void _serializeCIMCreateInstanceRequestMessage(
        Array<Sint8>& out,
        CIMCreateInstanceRequestMessage* message);

    static void _serializeCIMModifyInstanceRequestMessage(
        Array<Sint8>& out,
        CIMModifyInstanceRequestMessage* message);

    static void _serializeCIMEnumerateInstancesRequestMessage(
        Array<Sint8>& out,
        CIMEnumerateInstancesRequestMessage* message);

    static void _serializeCIMEnumerateInstanceNamesRequestMessage(
        Array<Sint8>& out,
        CIMEnumerateInstanceNamesRequestMessage* message);

    static void _serializeCIMExecQueryRequestMessage(
        Array<Sint8>& out,
        CIMExecQueryRequestMessage* message);

    static void _serializeCIMAssociatorsRequestMessage(
        Array<Sint8>& out,
        CIMAssociatorsRequestMessage* message);

    static void _serializeCIMAssociatorNamesRequestMessage(
        Array<Sint8>& out,
        CIMAssociatorNamesRequestMessage* message);

    static void _serializeCIMReferencesRequestMessage(
        Array<Sint8>& out,
        CIMReferencesRequestMessage* message);

    static void _serializeCIMReferenceNamesRequestMessage(
        Array<Sint8>& out,
        CIMReferenceNamesRequestMessage* message);

    static void _serializeCIMGetPropertyRequestMessage(
        Array<Sint8>& out,
        CIMGetPropertyRequestMessage* message);

    static void _serializeCIMSetPropertyRequestMessage(
        Array<Sint8>& out,
        CIMSetPropertyRequestMessage* message);

    static void _serializeCIMInvokeMethodRequestMessage(
        Array<Sint8>& out,
        CIMInvokeMethodRequestMessage* message);

    //
    // CIMIndicationRequestMessages
    //

    static void _serializeCIMEnableIndicationsRequestMessage(
        Array<Sint8>& out,
        CIMEnableIndicationsRequestMessage* message);

    static void _serializeCIMDisableIndicationsRequestMessage(
        Array<Sint8>& out,
        CIMDisableIndicationsRequestMessage* message);

    static void _serializeCIMCreateSubscriptionRequestMessage(
        Array<Sint8>& out,
        CIMCreateSubscriptionRequestMessage* message);

    static void _serializeCIMModifySubscriptionRequestMessage(
        Array<Sint8>& out,
        CIMModifySubscriptionRequestMessage* message);

    static void _serializeCIMDeleteSubscriptionRequestMessage(
        Array<Sint8>& out,
        CIMDeleteSubscriptionRequestMessage* message);

    //
    // Generic CIMRequestMessages
    //

    static void _serializeCIMExportIndicationRequestMessage(
        Array<Sint8>& out,
        CIMExportIndicationRequestMessage* message);

    static void _serializeCIMProcessIndicationRequestMessage(
        Array<Sint8>& out,
        CIMProcessIndicationRequestMessage* message);

    static void _serializeCIMConsumeIndicationRequestMessage(
        Array<Sint8>& out,
        CIMConsumeIndicationRequestMessage* message);

    static void _serializeCIMDisableModuleRequestMessage(
        Array<Sint8>& out,
        CIMDisableModuleRequestMessage* message);

    static void _serializeCIMEnableModuleRequestMessage(
        Array<Sint8>& out,
        CIMEnableModuleRequestMessage* message);

    static void _serializeCIMStopAllProvidersRequestMessage(
        Array<Sint8>& out,
        CIMStopAllProvidersRequestMessage* message);

    //
    //
    // CIM Response Messages
    //
    //

    //
    // CIMOperationResponseMessages
    //

    static void _serializeCIMGetInstanceResponseMessage(
        Array<Sint8>& out,
        CIMGetInstanceResponseMessage* message);

    static void _serializeCIMDeleteInstanceResponseMessage(
        Array<Sint8>& out,
        CIMDeleteInstanceResponseMessage* message);

    static void _serializeCIMCreateInstanceResponseMessage(
        Array<Sint8>& out,
        CIMCreateInstanceResponseMessage* message);

    static void _serializeCIMModifyInstanceResponseMessage(
        Array<Sint8>& out,
        CIMModifyInstanceResponseMessage* message);

    static void _serializeCIMEnumerateInstancesResponseMessage(
        Array<Sint8>& out,
        CIMEnumerateInstancesResponseMessage* message);

    static void _serializeCIMEnumerateInstanceNamesResponseMessage(
        Array<Sint8>& out,
        CIMEnumerateInstanceNamesResponseMessage* message);

    static void _serializeCIMExecQueryResponseMessage(
        Array<Sint8>& out,
        CIMExecQueryResponseMessage* message);

    static void _serializeCIMAssociatorsResponseMessage(
        Array<Sint8>& out,
        CIMAssociatorsResponseMessage* message);

    static void _serializeCIMAssociatorNamesResponseMessage(
        Array<Sint8>& out,
        CIMAssociatorNamesResponseMessage* message);

    static void _serializeCIMReferencesResponseMessage(
        Array<Sint8>& out,
        CIMReferencesResponseMessage* message);

    static void _serializeCIMReferenceNamesResponseMessage(
        Array<Sint8>& out,
        CIMReferenceNamesResponseMessage* message);

    static void _serializeCIMGetPropertyResponseMessage(
        Array<Sint8>& out,
        CIMGetPropertyResponseMessage* message);

    static void _serializeCIMSetPropertyResponseMessage(
        Array<Sint8>& out,
        CIMSetPropertyResponseMessage* message);

    static void _serializeCIMInvokeMethodResponseMessage(
        Array<Sint8>& out,
        CIMInvokeMethodResponseMessage* message);

    //
    // CIMIndicationResponseMessages
    //

    static void _serializeCIMEnableIndicationsResponseMessage(
        Array<Sint8>& out,
        CIMEnableIndicationsResponseMessage* message);

    static void _serializeCIMDisableIndicationsResponseMessage(
        Array<Sint8>& out,
        CIMDisableIndicationsResponseMessage* message);

    static void _serializeCIMCreateSubscriptionResponseMessage(
        Array<Sint8>& out,
        CIMCreateSubscriptionResponseMessage* message);

    static void _serializeCIMModifySubscriptionResponseMessage(
        Array<Sint8>& out,
        CIMModifySubscriptionResponseMessage* message);

    static void _serializeCIMDeleteSubscriptionResponseMessage(
        Array<Sint8>& out,
        CIMDeleteSubscriptionResponseMessage* message);

    //
    // Generic CIMResponseMessages
    //

    static void _serializeCIMExportIndicationResponseMessage(
        Array<Sint8>& out,
        CIMExportIndicationResponseMessage* message);

    static void _serializeCIMProcessIndicationResponseMessage(
        Array<Sint8>& out,
        CIMProcessIndicationResponseMessage* message);

#if 0    // ATTN: This message type is currently not defined
    static void _serializeCIMConsumeIndicationResponseMessage(
        Array<Sint8>& out,
        CIMConsumeIndicationResponseMessage* message);
#endif

    static void _serializeCIMDisableModuleResponseMessage(
        Array<Sint8>& out,
        CIMDisableModuleResponseMessage* message);

    static void _serializeCIMEnableModuleResponseMessage(
        Array<Sint8>& out,
        CIMEnableModuleResponseMessage* message);

    static void _serializeCIMStopAllProvidersResponseMessage(
        Array<Sint8>& out,
        CIMStopAllProvidersResponseMessage* message);

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMMessageSerializer_h */
