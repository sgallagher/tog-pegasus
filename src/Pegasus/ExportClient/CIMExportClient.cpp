//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/PegasusVersion.h>

#include "CIMExportRequestEncoder.h"
#include "CIMExportResponseDecoder.h"
#include "CIMExportClient.h"

#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportClient::CIMExportClient(
   Monitor* monitor,
   HTTPConnector* httpConnector,
   Uint32 timeoutMilliseconds)
   : 
   MessageQueue(PEGASUS_QUEUENAME_EXPORTCLIENT),
   _monitor(monitor),
   _httpConnector(httpConnector),
   _httpConnection(0),
   _timeoutMilliseconds(timeoutMilliseconds),
   _connected(false),
   _responseDecoder(0),
   _requestEncoder(0)
{
}

CIMExportClient::~CIMExportClient()
{

	disconnect();
}

void CIMExportClient::_connect()
{
   // Create response decoder:
    
   _responseDecoder = new CIMExportResponseDecoder(
      this, _requestEncoder, &_authenticator);
    
   // Attempt to establish a connection:
    
   try
   {
      _httpConnection = _httpConnector->connect(_connectHost, 
					       _connectPortNumber, 
        				       _responseDecoder);
   }
   catch (CannotCreateSocketException& e)
   {
        delete _responseDecoder;
        throw e;
   }
   catch (CannotConnectException& e)
   {
        delete _responseDecoder;
        throw e;
   }
   catch (InvalidLocatorException& e)
   {
        delete _responseDecoder;
        throw e;
   }
    
   // Create request encoder:
    
   _requestEncoder = new CIMExportRequestEncoder(
      _httpConnection, &_authenticator);

   _responseDecoder->setEncoderQueue(_requestEncoder);    

   _connected = true;
}

void CIMExportClient::_reconnect()
{
    disconnect();
    _connect();
}

void CIMExportClient::connect(
    const String& host,
    const Uint32 portNumber)
{
   // If already connected, bail out!
    
   if (_connected)
      throw AlreadyConnectedException();
    
    //
    // If the host is empty, set hostName to "localhost"
    //
    String hostName = host;
    if (host == String::EMPTY)
    {
        hostName = "localhost";
    }

    //
    // Set authentication information
    //
    _authenticator.clearRequest(true);
    _authenticator.setAuthType(ClientAuthenticator::NONE);

    _connectHost = hostName;
    _connectPortNumber = portNumber;

    _connect();
}

void CIMExportClient::disconnect()
{
    if (_connected)
    {
        //
        // destroy response decoder
        //
        if (_responseDecoder)
        {
            delete _responseDecoder;
            _responseDecoder = 0;
        }

        //
        // Close the connection
        //
        if (_httpConnector)
        {
            _httpConnector->disconnect(_httpConnection);
        }

        //
        // destroy request encoder
        //
        if (_requestEncoder)
        {
            delete _requestEncoder;
            _requestEncoder = 0;
        }

        _authenticator.clearRequest(true);

        _connected = false;
    }
}

void CIMExportClient::exportIndication(
   const String& url,
   const CIMInstance& instanceName,
   const ContentLanguages& contentLanguages)
{
   // encode request
// l10n  
   CIMRequestMessage* request = new CIMExportIndicationRequestMessage(
      String::EMPTY,
      url,
      instanceName,
      QueueIdStack(),
      String::EMPTY,
      String::EMPTY,
      contentLanguages);

   Message* message = _doRequest(request,
      CIM_EXPORT_INDICATION_RESPONSE_MESSAGE);

   CIMExportIndicationResponseMessage* response = 
      (CIMExportIndicationResponseMessage*)message;
    
   Destroyer<CIMExportIndicationResponseMessage> destroyer(response);
}

Message* CIMExportClient::_doRequest(
    CIMRequestMessage * request,
    const Uint32 expectedResponseMessageType
)
{
    if (!_connected)
    {
       delete request;
       throw NotConnectedException();
    }
    
    String messageId = XmlWriter::getNextMessageId();
    const_cast<String &>(request->messageId) = messageId;

    _authenticator.clearRequest();

    // ATTN-RK-P2-20020416: We should probably clear out the queue first.
    PEGASUS_ASSERT(getCount() == 0);  // Shouldn't be any messages in our queue

    //
    //  Set HTTP method in request to M-POST
    //
    request->setHttpMethod (HTTP_METHOD_M_POST);

    _requestEncoder->enqueue(request);

    Uint64 startMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
    Uint64 nowMilliseconds = startMilliseconds;
    Uint64 stopMilliseconds = nowMilliseconds + _timeoutMilliseconds;

    while (nowMilliseconds < stopMilliseconds)
    {
	//
	// Wait until the timeout expires or an event occurs:
	//

       _monitor->run(Uint32(stopMilliseconds - nowMilliseconds));
       
	//
	// Check to see if incoming queue has a message
	//

	Message* response = dequeue();

	if (response)
	{
            // Shouldn't be any more messages in our queue
            PEGASUS_ASSERT(getCount() == 0);

            //
            //  ATTN-CAKG-20021001: If HTTP response is 501 Not Implemented
            //  or 510 Not Extended, retry with POST method
            //

            if (response->getType() == CLIENT_EXCEPTION_MESSAGE)
            {
                Exception* clientException =
                    ((ClientExceptionMessage*)response)->clientException;
                delete response;
                Destroyer<Exception> d(clientException);
                throw *clientException;
            }
            else if (response->getType() == expectedResponseMessageType)
            {
                CIMResponseMessage* cimResponse = (CIMResponseMessage*)response;
                if (cimResponse->messageId != messageId)
                {
                    CIMClientResponseException responseException(
                        String("Mismatched response message ID:  Got \"") +
                        cimResponse->messageId + "\", expected \"" +
                        messageId + "\".");
                    delete response;
	            throw responseException;
                }
                if (cimResponse->cimException.getCode() != CIM_ERR_SUCCESS)
                {
                    CIMException cimException(
                        cimResponse->cimException.getCode(),
                        cimResponse->cimException.getMessage());
                    delete response;
	            throw cimException;
                }
                return response;
            }
            else
            {
                CIMClientResponseException responseException(
                    "Mismatched response message type.");
                delete response;
	        throw responseException;
            }
	}

        nowMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
	pegasus_yield();
    }

    //
    // Reconnect to reset the connection (disregard late response)
    //
    try
    {
        _reconnect();
    }
    catch (...)
    {
    }

    //
    // Throw timed out exception:
    //
    throw ConnectionTimeoutException();
}

PEGASUS_NAMESPACE_END
