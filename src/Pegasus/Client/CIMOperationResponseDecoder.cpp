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

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/CIMMessage.h>
#include "CIMOperationResponseDecoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMOperationResponseDecoder::CIMOperationResponseDecoder(
    MessageQueue* outputQueue) : _outputQueue(outputQueue)
{

}

CIMOperationResponseDecoder::~CIMOperationResponseDecoder()
{

}

void CIMOperationResponseDecoder::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
	return;

    switch (message->getType())
    {
	case HTTP_MESSAGE:
	{
	    HTTPMessage* httpMessage = (HTTPMessage*)message;
	}

	default:
	    // ATTN: send this to the orphan queue!
	    break;
    }

    delete message;
}

const char* CIMOperationResponseDecoder::getQueueName() const
{
    return "CIMOperationResponseDecoder";
}

void CIMOperationResponseDecoder::_decodeCreateClassResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeGetClassResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeModifyClassResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeEnumerateClassNamesResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeEnumerateClassesResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeDeleteClassResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeCreateInstanceResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeGetInstanceResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeModifyInstanceResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeEnumerateInstanceNamesResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeDeleteInstanceResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeSetQualifierResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeGetQualifierResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeEnumerateQualifiersResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeDeleteQualifierResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeReferenceNamesResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeReferencesResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeAssociatorNamesResponse(
    HTTPMessage* httpMessage)
{
}

void CIMOperationResponseDecoder::_decodeAssociatorsResponse(
    HTTPMessage* httpMessage)
{
}

PEGASUS_NAMESPACE_END
