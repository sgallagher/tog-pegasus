//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMOperationRequestEncoder_h
#define Pegasus_CIMOperationRequestEncoder_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Client/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** This class receives CIM Operation Request messages on its input queue
    and encodes them into HTTP messages which it places on its output queue.
*/
class PEGASUS_CLIENT_LINKAGE CIMOperationRequestEncoder : public MessageQueue
{
public:

    /** Constuctor.
	@param outputQueue queue to receive encoded HTTP messages.
    */
    CIMOperationRequestEncoder(MessageQueue* outputQueue);

    /** Destructor. */
    ~CIMOperationRequestEncoder();

    /** This method is called when a message is enqueued on this queue. */
    virtual void handleEnqueue();

    /** Returns the queue name. */
    virtual const char* getQueueName() const;

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

    MessageQueue* _outputQueue;
    char* _hostName;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMOperationRequestEncoder_h */
