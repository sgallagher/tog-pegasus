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

#ifndef Pegasus_Client_h
#define Pegasus_Client_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/HTTPMessage.h>

PEGASUS_NAMESPACE_BEGIN

/** This class receives HTTP messages and decodes them into CIM Operation 
    Responses messages which it places on its output queue.
*/
class PEGASUS_CLIENT_LINKAGE CIMOperationResponseDecoder : public MessageQueue
{
public:

    /** Constuctor.
	@param outputQueue queue to receive decoded HTTP messages.
    */
    CIMOperationResponseDecoder(MessageQueue* outputQueue);

    /** Destructor. */
    ~CIMOperationResponseDecoder();

    /** This method is called when a message is enqueued on this queue. */
    virtual void handleEnqueue();

    /** Returns the queue name. */
    virtual const char* getQueueName() const;

private:

    void _decodeCreateClassResponse(
	HTTPMessage* httpMessage);

    void _decodeGetClassResponse(
	HTTPMessage* httpMessage);

    void _decodeModifyClassResponse(
	HTTPMessage* httpMessage);

    void _decodeEnumerateClassNamesResponse(
	HTTPMessage* httpMessage);

    void _decodeEnumerateClassesResponse(
	HTTPMessage* httpMessage);

    void _decodeDeleteClassResponse(
	HTTPMessage* httpMessage);

    void _decodeCreateInstanceResponse(
	HTTPMessage* httpMessage);

    void _decodeGetInstanceResponse(
	HTTPMessage* httpMessage);

    void _decodeModifyInstanceResponse(
	HTTPMessage* httpMessage);

    void _decodeEnumerateInstanceNamesResponse(
	HTTPMessage* httpMessage);

    void _decodeEnumerateInstancesResponse(
	HTTPMessage* httpMessage);

    void _decodeDeleteInstanceResponse(
	HTTPMessage* httpMessage);

    void _decodeSetQualifierResponse(
	HTTPMessage* httpMessage);

    void _decodeGetQualifierResponse(
	HTTPMessage* httpMessage);

    void _decodeEnumerateQualifiersResponse(
	HTTPMessage* httpMessage);

    void _decodeDeleteQualifierResponse(
	HTTPMessage* httpMessage);

    void _decodeReferenceNamesResponse(
	HTTPMessage* httpMessage);

    void _decodeReferencesResponse(
	HTTPMessage* httpMessage);

    void _decodeAssociatorNamesResponse(
	HTTPMessage* httpMessage);

    void _decodeAssociatorsResponse(
	HTTPMessage* httpMessage);

    MessageQueue* _outputQueue;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Client_h */
