//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMOperationResponseDecoder_h
#define Pegasus_CIMOperationResponseDecoder_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Client/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class XmlParser;

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

    void _handleHTTPMessage(
	HTTPMessage* message);

    void _handleMethodResponse(
	char* content);

    void _decodeCreateClassResponse(
	XmlParser& parser, const String& messageId);

    void _decodeGetClassResponse(
	XmlParser& parser, const String& messageId);

    void _decodeModifyClassResponse(
	XmlParser& parser, const String& messageId);

    void _decodeEnumerateClassNamesResponse(
	XmlParser& parser, const String& messageId);

    void _decodeEnumerateClassesResponse(
	XmlParser& parser, const String& messageId);

    void _decodeDeleteClassResponse(
	XmlParser& parser, const String& messageId);

    void _decodeCreateInstanceResponse(
	XmlParser& parser, const String& messageId);

    void _decodeGetInstanceResponse(
	XmlParser& parser, const String& messageId);

    void _decodeModifyInstanceResponse(
	XmlParser& parser, const String& messageId);

    void _decodeEnumerateInstanceNamesResponse(
	XmlParser& parser, const String& messageId);

    void _decodeEnumerateInstancesResponse(
	XmlParser& parser, const String& messageId);

    void _decodeDeleteInstanceResponse(
	XmlParser& parser, const String& messageId);

    void _decodeSetQualifierResponse(
	XmlParser& parser, const String& messageId);

    void _decodeGetQualifierResponse(
	XmlParser& parser, const String& messageId);

    void _decodeEnumerateQualifiersResponse(
	XmlParser& parser, const String& messageId);

    void _decodeDeleteQualifierResponse(
	XmlParser& parser, const String& messageId);

    void _decodeReferenceNamesResponse(
	XmlParser& parser, const String& messageId);

    void _decodeReferencesResponse(
	XmlParser& parser, const String& messageId);

    void _decodeAssociatorNamesResponse(
	XmlParser& parser, const String& messageId);

    void _decodeAssociatorsResponse(
	XmlParser& parser, const String& messageId);

    void _decodeInvokeMethodResponse(
	XmlParser& parser, const String& messageId, const String& methodName);

    MessageQueue* _outputQueue;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMOperationResponseDecoder_h */
