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
// Modified By: Nitin Upasani, Hewlett-Packard (Nitin_Upasani@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veriats.com)
//              Willis White, IBM (whiwill@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMOperationRequestEncoder_h
#define Pegasus_CIMOperationRequestEncoder_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Client/ClientAuthenticator.h>
#include <Pegasus/Client/Linkage.h>
#include "ClientPerfDataStore.h"

PEGASUS_NAMESPACE_BEGIN

/** This class receives CIM Operation Request messages on its input queue
    and encodes them into HTTP messages which it places on its output queue.
*/
class PEGASUS_CLIENT_LINKAGE CIMOperationRequestEncoder : public MessageQueue
{
public:

    /** Constuctor.
	@param outputQueue queue to receive encoded HTTP messages.
        @param hostName Name of the target host for the encoded requests.
                        I.e., the value of the HTTP Host header.
    */
    CIMOperationRequestEncoder(
        MessageQueue* outputQueue,
        const String& hostName,
        ClientAuthenticator* authenticator,
        Uint32 showOutput);

    /** Destructor. */
    ~CIMOperationRequestEncoder();

    /** This method is called when a message is enqueued on this queue. */
    virtual void handleEnqueue();

private:

    void _encodeCreateClassRequest(
	CIMCreateClassRequestMessage* message);

    void _encodeGetClassRequest(
	CIMGetClassRequestMessage* message);

    void _encodeModifyClassRequest(
	CIMModifyClassRequestMessage* message);

    void _encodeEnumerateClassNamesRequest(
	CIMEnumerateClassNamesRequestMessage* message);

    void _encodeEnumerateClassesRequest(
	CIMEnumerateClassesRequestMessage* message);

    void _encodeDeleteClassRequest(
	CIMDeleteClassRequestMessage* message);

    void _encodeCreateInstanceRequest(
	CIMCreateInstanceRequestMessage* message);

    void _encodeGetInstanceRequest(
	CIMGetInstanceRequestMessage* message);

    void _encodeModifyInstanceRequest(
	CIMModifyInstanceRequestMessage* message);

    void _encodeEnumerateInstanceNamesRequest(
	CIMEnumerateInstanceNamesRequestMessage* message);

    void _encodeEnumerateInstancesRequest(
	CIMEnumerateInstancesRequestMessage* message);

    void _encodeDeleteInstanceRequest(
	CIMDeleteInstanceRequestMessage* message);

    void _encodeGetPropertyRequest(
	CIMGetPropertyRequestMessage* message);

    void _encodeSetPropertyRequest(
	CIMSetPropertyRequestMessage* message);

    void _encodeSetQualifierRequest(
	CIMSetQualifierRequestMessage* message);

    void _encodeGetQualifierRequest(
	CIMGetQualifierRequestMessage* message);

    void _encodeEnumerateQualifiersRequest(
	CIMEnumerateQualifiersRequestMessage* message);

    void _encodeDeleteQualifierRequest(
	CIMDeleteQualifierRequestMessage* message);

    void _encodeReferenceNamesRequest(
	CIMReferenceNamesRequestMessage* message);

    void _encodeReferencesRequest(
	CIMReferencesRequestMessage* message);

    void _encodeAssociatorNamesRequest(
	CIMAssociatorNamesRequestMessage* message);

    void _encodeAssociatorsRequest(
	CIMAssociatorsRequestMessage* message);

    void _encodeExecQueryRequest(
	CIMExecQueryRequestMessage* message);

    void _encodeInvokeMethodRequest(
	CIMInvokeMethodRequestMessage* message);

    void _sendRequest(Array<char>& buffer); 

    MessageQueue* _outputQueue;
    CString _hostName;
    ClientAuthenticator* _authenticator;
    // Controls client trace output. 1 = con, 2 == log
    Uint32 _showOutput;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMOperationRequestEncoder_h */
