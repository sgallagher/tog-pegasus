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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/TCPChannel.h>
#include <Pegasus/Common/Selector.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Protocol/Handler.h>
#include "CIMExportClient.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

struct GetClassResult
{
    CIMStatusCode code;
    CIMClass cimClass;
};

struct CreateInstanceResult
{
    CIMStatusCode code;
};

class ExportClientHandler : public Handler
{
public:

    ExportClientHandler(Selector* selector)
	: _getClassResult(0), _blocked(false), _selector(selector)
    {

    }
    
    union
    {
	GetClassResult* _getClassResult;
	CreateInstanceResult* _createInstanceResult;
    };

    Boolean waitForResponse(Uint32 timeOutMilliseconds);
    
private:
    Boolean _blocked;
    Selector* _selector;
};


class ExportClientHandlerFactory : public ChannelHandlerFactory
{
public:

    ExportClientHandlerFactory(Selector* selector) : _selector(selector) { }

    virtual ~ExportClientHandlerFactory() { }

    virtual ChannelHandler* create() 
    { 
	return new ExportClientHandler(_selector); 
    }

private:

    Selector* _selector;
};


CIMExportClient::CIMExportClient(
    Selector* selector,
    Uint32 timeOutMilliseconds)
    : _channel(0), _timeOutMilliseconds(timeOutMilliseconds)
{
    _selector = selector;
}

CIMExportClient::~CIMExportClient()
{
    
}

void CIMExportClient::deliverIndication(
    const char* address, 
    CIMInstance& indicationInstance, 
    String nameSpace)
{
    if (_channel)
	throw AlreadyConnected();

    ChannelHandlerFactory* factory = new ExportClientHandlerFactory(_selector);

    TCPChannelConnector* connector
	= new TCPChannelConnector(factory, _selector);

    // ATTN-A: need connection timeout here:

    _channel = connector->connect(address);

    if (!_channel)
	throw FailedToConnect();

    String messageId = XmlWriter::getNextMessageId();

    Array<Sint8> params;

    XmlWriter::appendInstanceParameter(
	params, "NewIndication", indicationInstance);
	
    Array<Sint8> message = XmlWriter::formatSimpleIndicationReqMessage(
	_getHostName(), nameSpace, "ExportIndication", messageId, params);

    _channel->writeN(message.getData(), message.size());

    if (!_getHandler()->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    CreateInstanceResult* result = _getHandler()->_createInstanceResult;
    CIMStatusCode code = result->code;
    delete result;
    _getHandler()->_createInstanceResult = 0;

    if (code != CIM_ERR_SUCCESS)
	throw CIMException(code);
}

ExportClientHandler* CIMExportClient::_getHandler()
{
    return (ExportClientHandler*)_channel->getChannelHandler();
}

Boolean ExportClientHandler::waitForResponse(Uint32 timeOutMilliseconds)
{
    _blocked = true;
    long rem = long(timeOutMilliseconds);

    while (_blocked)
    {
	TimeValue start = TimeValue::getCurrentTime();

	TimeValue tv;
	tv.fromMilliseconds(rem);

	_selector->select(tv.toMilliseconds());

	TimeValue stop = TimeValue::getCurrentTime();

	long diff = stop.toMilliseconds() - start.toMilliseconds();

	if (diff >= rem)
	    break;

	rem -= diff;
    }

    Boolean gotResponse = !_blocked;
    _blocked = false;
    return gotResponse;
}

PEGASUS_NAMESPACE_END
