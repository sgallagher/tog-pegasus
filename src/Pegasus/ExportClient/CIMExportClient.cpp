//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/HostAddress.h>

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
   _doReconnect(false),
   _responseDecoder(0),
   _requestEncoder(0)
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportClient::CIMExportClient()");
    PEG_METHOD_EXIT();
}

CIMExportClient::~CIMExportClient()
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportClient::~CIMExportClient()");

    disconnect();

    PEG_METHOD_EXIT();
}

void CIMExportClient::_connect()
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportClient::_connect()");

    // Create response decoder:

    _responseDecoder = new CIMExportResponseDecoder(
        this, _requestEncoder, &_authenticator);

    // Attempt to establish a connection:

    try
    {
        _httpConnection = _httpConnector->connect(_connectHost,
            _connectPortNumber,
            _connectSSLContext.get(),
            _timeoutMilliseconds,
            _responseDecoder);
    }
    catch (...)
    {
        // Some possible exceptions are CannotCreateSocketException,
        // CannotConnectException, and InvalidLocatorException
        delete _responseDecoder;
        PEG_METHOD_EXIT();
        throw;
    }

    // Create request encoder:

    String connectHost = _connectHost;

#ifdef PEGASUS_ENABLE_IPV6
    if (HostAddress::isValidIPV6Address(connectHost))
    {
        connectHost = "[" + connectHost + "]";
    }
#endif

    if (connectHost.size())
    {
        char portStr[32];
        sprintf(portStr, ":%u", _connectPortNumber);
        connectHost.append(portStr);
    }

    _requestEncoder = new CIMExportRequestEncoder(
        _httpConnection, connectHost, &_authenticator);

    _responseDecoder->setEncoderQueue(_requestEncoder);

    _doReconnect = false;

    _connected = true;

    _httpConnection->setSocketWriteTimeout(_timeoutMilliseconds/1000+1);

    PEG_METHOD_EXIT();
}

void CIMExportClient::_disconnect()
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportClient::_disconnect()");

    if (_connected)
    {
        //
        // destroy response decoder
        //
        delete _responseDecoder;
        _responseDecoder = 0;

        //
        // Close the connection
        //
        if (_httpConnector)
        {
            _httpConnector->disconnect(_httpConnection);
            _httpConnection = 0;
        }

        //
        // destroy request encoder
        //
        delete _requestEncoder;
        _requestEncoder = 0;

        _connected = false;
    }

    // Reconnect no longer applies
    _doReconnect=false;

    // Let go of the cached request message if we have one
    _authenticator.setRequestMessage(0);

    PEG_METHOD_EXIT();
}

void CIMExportClient::connect(
    const String& host,
    const Uint32 portNumber)
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportClient::connect()");

    // If already connected, bail out!
    if (_connected)
    {
        PEG_METHOD_EXIT();
        throw AlreadyConnectedException();
    }

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
    _authenticator.clear();

    _connectSSLContext.reset(0);
    _connectHost = hostName;
    _connectPortNumber = portNumber;

    _connect();
    PEG_METHOD_EXIT();
}

void CIMExportClient::connect(
    const String& host,
    const Uint32 portNumber,
    const SSLContext& sslContext)
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportClient::connect()");

    // If already connected, bail out!

    if (_connected)
    {
       PEG_METHOD_EXIT();
       throw AlreadyConnectedException();
    }

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
    _authenticator.clear();

    _connectSSLContext.reset(new SSLContext(sslContext));
    _connectHost = hostName;
    _connectPortNumber = portNumber;

    try
    {
        _connect();
    }
    catch (...)
    {
        _connectSSLContext.reset();
        PEG_METHOD_EXIT();
        throw;
    }
    PEG_METHOD_EXIT();
}

void CIMExportClient::disconnect()
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportClient::disconnect()");
    _disconnect();
    _authenticator.clear();
    _connectSSLContext.reset();
    PEG_METHOD_EXIT();
}

void CIMExportClient::exportIndication(
   const String& url,
   const CIMInstance& instanceName,
   const ContentLanguageList& contentLanguages)
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportClient::exportIndication()");

    try
    {
        // encode request
        CIMRequestMessage* request = new CIMExportIndicationRequestMessage(
            String::EMPTY,
            url,
            instanceName,
            QueueIdStack(),
            String::EMPTY,
            String::EMPTY);

        request->operationContext.set
            (ContentLanguageListContainer(contentLanguages));

        Message* message = _doRequest(request,
            CIM_EXPORT_INDICATION_RESPONSE_MESSAGE);

        CIMExportIndicationResponseMessage* response =
            (CIMExportIndicationResponseMessage*)message;

        AutoPtr<CIMExportIndicationResponseMessage> ap(response);
    }
    catch (const Exception& e)
    {
        PEG_TRACE_STRING (TRC_DISCARDED_DATA, Tracer::LEVEL4,
            "Failed to export indication: " + e.getMessage ());
        throw;
    }
    catch (...)
    {
        PEG_TRACE_CSTRING (TRC_DISCARDED_DATA, Tracer::LEVEL4,
            "Failed to export indication");
        throw;
    }

    PEG_METHOD_EXIT();
}

Message* CIMExportClient::_doRequest(
    CIMRequestMessage* pRequest,
    Uint32 expectedResponseMessageType)
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportClient::_doRequest()");

    AutoPtr<CIMRequestMessage> request(pRequest);

    if (!_connected && !_doReconnect)
    {
        PEG_METHOD_EXIT();
        throw NotConnectedException();
    }

    if (_doReconnect)
    {
        try
        {
            _connect();
            _doReconnect = false;
        }
        catch (const Exception& e)
        {
            PEG_TRACE_STRING(TRC_EXPORT_CLIENT, Tracer::LEVEL2,
                "Failed to connect to indication listener: " + e.getMessage());
            PEG_METHOD_EXIT();
            throw;
        }
        catch (...)
        {
            PEG_TRACE_CSTRING(TRC_EXPORT_CLIENT, Tracer::LEVEL2,
                "Failed to connect to indication listener.");
            PEG_METHOD_EXIT();
            throw;
        }
    }


    String messageId = XmlWriter::getNextMessageId();
    const_cast<String &>(request->messageId) = messageId;

    _authenticator.setRequestMessage(0);

    // ATTN-RK-P2-20020416: We should probably clear out the queue first.
    PEGASUS_ASSERT(getCount() == 0);  // Shouldn't be any messages in our queue

    //
    //  Set HTTP method in request to POST
    //
    request->setHttpMethod(HTTP_METHOD__POST);

    _requestEncoder->enqueue(request.release());

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

        AutoPtr<Message> response(dequeue());

        if (response.get() != 0)
        {
            // Shouldn't be any more messages in our queue
            PEGASUS_ASSERT(getCount() == 0);

            //
            // Close the connection if response contained a "Connection: Close"
            // header (e.g. at authentication challenge)
            //
            if (response->getCloseConnect() == true)
            {
                _disconnect();
                _doReconnect = true;
                response->setCloseConnect(false);
            }

            //
            //  Future:  If M-POST is used and HTTP response is 501 Not
            //  Implemented or 510 Not Extended, retry with POST method
            //

            if (response->getType() == CLIENT_EXCEPTION_MESSAGE)
            {
                Exception* clientException =
                    ((ClientExceptionMessage*)response.get())->clientException;

                PEG_TRACE_CSTRING(TRC_EXPORT_CLIENT, Tracer::LEVEL4,
                    "Client Exception Message received.");

                AutoPtr<Exception> d(clientException);

                //
                // Determine and throw the specific class of client exception
                //

                CIMClientMalformedHTTPException* malformedHTTPException =
                    dynamic_cast<CIMClientMalformedHTTPException*>(
                    clientException);
                if (malformedHTTPException)
                {
                    PEG_METHOD_EXIT();
                    throw *malformedHTTPException;
                }

                CIMClientHTTPErrorException* httpErrorException =
                    dynamic_cast<CIMClientHTTPErrorException*>(
                    clientException);
                if (httpErrorException)
                {
                    PEG_METHOD_EXIT();
                    throw *httpErrorException;
                }

                CIMClientXmlException* xmlException =
                    dynamic_cast<CIMClientXmlException*>(clientException);
                if (xmlException)
                {
                    PEG_METHOD_EXIT();
                    throw *xmlException;
                }

                CIMClientResponseException* responseException =
                    dynamic_cast<CIMClientResponseException*>(clientException);
                if (responseException)
                {
                    PEG_METHOD_EXIT();
                    throw *responseException;
                }

                PEG_METHOD_EXIT();
                throw *clientException;
            }
            else if (response->getType() == expectedResponseMessageType)
            {
                PEG_TRACE_CSTRING(TRC_EXPORT_CLIENT, Tracer::LEVEL4,
                    "Received expected indication response message.");
                CIMResponseMessage* cimResponse =
                    (CIMResponseMessage*)response.get();
                if (cimResponse->messageId != messageId)
                {
                    MessageLoaderParms mlParms(
                        "ExportClient.CIMExportClient.MISMATCHED_RESPONSE_ID",
                        "Mismatched response message ID:  Got \"$0\", "
                            "expected \"$1\".",
                        cimResponse->messageId, messageId);
                    String mlString(MessageLoader::getMessage(mlParms));

                    CIMClientResponseException responseException(mlString);

                    PEG_METHOD_EXIT();
                    throw responseException;
                }
                if (cimResponse->cimException.getCode() != CIM_ERR_SUCCESS)
                {
                    PEG_TRACE_CSTRING(TRC_EXPORT_CLIENT, Tracer::LEVEL4,
                        "Received indication failure message.");
                    CIMException cimException(
                        cimResponse->cimException.getCode(),
                        cimResponse->cimException.getMessage());
                    PEG_METHOD_EXIT();
                    throw cimException;
                }
                PEG_METHOD_EXIT();
                return response.release();
            }
            else if (dynamic_cast<CIMRequestMessage*>(response.get()) != 0)
            {
                //
                // Respond to an authentication challenge.
                // Reconnect if the connection was closed.
                //
                if (_doReconnect)
                {
                    _connect();
                }

                _requestEncoder->enqueue(response.release());

                nowMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
                stopMilliseconds = nowMilliseconds + _timeoutMilliseconds;
                continue;
            }
            else
            {
                MessageLoaderParms mlParms(
                    "ExportClient.CIMExportClient.MISMATCHED_RESPONSE",
                    "Mismatched response message type.");
                String mlString(MessageLoader::getMessage(mlParms));

                CIMClientResponseException responseException(mlString);

                PEG_TRACE_STRING(TRC_EXPORT_CLIENT, Tracer::LEVEL4, mlString);

                PEG_METHOD_EXIT();
                throw responseException;
            }
        }

        nowMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
    }

    //
    // Reconnect to reset the connection (disregard late response)
    //

    PEG_TRACE_CSTRING(TRC_EXPORT_CLIENT, Tracer::LEVEL4,
        "Connection to the listener timed out.");

    _disconnect();
    _authenticator.resetChallengeStatus();
    _doReconnect = true;

    //
    // Throw timed out exception:
    //
    PEG_METHOD_EXIT();
    throw ConnectionTimeoutException();
}

PEGASUS_NAMESPACE_END
