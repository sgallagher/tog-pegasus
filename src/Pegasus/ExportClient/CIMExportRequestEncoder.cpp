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
#include "CIMExportRequestEncoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportRequestEncoder::CIMExportRequestEncoder(
    MessageQueue* outputQueue) : _outputQueue(outputQueue)
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

    switch (message->getType())
    {
	case CIM_GET_CLASS_REQUEST_MESSAGE:
	    _encodeGetClassRequest((CIMGetClassRequestMessage*)message);
	    break;
    }

    delete message;
}

const char* CIMExportRequestEncoder::getQueueName() const
{
    return "CIMExportRequestEncoder";
}

void CIMExportRequestEncoder::_encodeGetClassRequest(
    CIMGetClassRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendClassNameParameter(
	params, "ClassName", message->className);
	
    if (message->localOnly != true)
	XmlWriter::appendBooleanParameter(params, "LocalOnly", false);

    if (message->includeQualifiers != true)
	XmlWriter::appendBooleanParameter(params, "IncludeQualifiers", false);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanParameter(params, "IncludeClassOrigin", true);

    Array<Sint8> buffer = XmlWriter::formatSimpleReqMessage(
	_hostName, message->nameSpace, "GetClass", message->messageId, params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

PEGASUS_NAMESPACE_END
