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
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include "CIMExportRequestEncoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportRequestEncoder::CIMExportRequestEncoder(
    MessageQueue* outputQueue, ClientAuthenticator* authenticator)
    :
    _outputQueue(outputQueue),
    _authenticator(authenticator)
{
    String tmpHostName = System::getHostName();
    _hostName = tmpHostName.allocateCString();
}

CIMExportRequestEncoder::~CIMExportRequestEncoder()
{
    delete [] _hostName;
}

void CIMExportRequestEncoder::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
	return;

    _authenticator->setRequestMessage(message);

    switch (message->getType())
    {
	case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
	    _encodeExportIndicationRequest((CIMExportIndicationRequestMessage*)message);
	    break;
    }

    //ATTN: Do not delete the message here.
    //
    // ClientAuthenticator needs this message for resending the request on
    // authentication challenge from the server. The message is deleted in
    // the decoder after receiving the valid response from thr server.
    //
    //delete message;
}

const char* CIMExportRequestEncoder::getQueueName() const
{
    return "CIMExportRequestEncoder";
}

void CIMExportRequestEncoder::_encodeExportIndicationRequest(
    CIMExportIndicationRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendInstanceParameter(
	params, "NewIndication", message->indicationInstance);
	
    Array<Sint8> buffer = XmlWriter::formatSimpleIndicationReqMessage(
	message->url.allocateCString(), 
	"ExportIndication", 
	message->messageId, 
	_authenticator->buildRequestAuthHeader(), 
	params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

PEGASUS_NAMESPACE_END
