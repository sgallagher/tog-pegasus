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
//=============================================================================
//
//%////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/SoapReader.h>
#include <Pegasus/Common/SoapWriter.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/CommonUTF.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/ArrayIterator.h>

#include "WSManOperationRequestDecoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

WSManOperationRequestDecoder::WSManOperationRequestDecoder(
    MessageQueueService* outputQueue,
    Uint32 returnQueueId)
    : MessageQueueService(PEGASUS_QUEUENAME_WSOPREQDECODER),
      _outputQueue(outputQueue),
      _returnQueueId(returnQueueId),
      _serverTerminating(false)
{
}


WSManOperationRequestDecoder::~WSManOperationRequestDecoder()
{
}


void WSManOperationRequestDecoder::sendResponse(
    Uint32 queueId,
    Buffer& message,
    Boolean closeConnect)
{
    MessageQueue* queue = MessageQueue::lookup(queueId);

    if (queue)
    {
        AutoPtr<HTTPMessage> httpMessage(new HTTPMessage(message));
        httpMessage->setCloseConnect(closeConnect);
        queue->enqueue(httpMessage.release());
    }
}


void WSManOperationRequestDecoder::sendHttpError(
    Uint32 queueId,
    const String& status,
    const String& cimError,
    const String& pegasusError,
    Boolean closeConnect)
{
    Buffer message;
    message = SoapWriter::formatHttpErrorRspMessage(
        status,
        cimError,
        pegasusError);

    sendResponse(queueId, message, closeConnect);
}


void WSManOperationRequestDecoder::handleEnqueue(Message* message)
{
    if (!message)
        return;

    switch (message->getType())
    {
        case HTTP_MESSAGE:
             handleHTTPMessage((HTTPMessage*)message);
             break;

        default:
            // Unexpected message type
            PEGASUS_ASSERT(0);
            break;
    }

    delete message;
}


void WSManOperationRequestDecoder::handleEnqueue()
{
    Message* message = dequeue();
    if (message)
        handleEnqueue(message);
}


//-----------------------------------------------------------------------------
//
// From the HTTP/1.1 Specification (RFC 2626):
//
// Both types of message consist of a start-line, zero or more header fields
// (also known as "headers"), an empty line (i.e., a line with nothing
// preceding the CRLF) indicating the end of the header fields, and possibly
// a message-body.
//
//-----------------------------------------------------------------------------
void WSManOperationRequestDecoder::handleHTTPMessage(HTTPMessage* httpMessage)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "WSManOperationRequestDecoder::handleHTTPMessage()");

    // Set the Accept-Language into the thread for this service.
    // This will allow all code in this thread to get
    // the languages for the messages returned to the client.
    Thread::setLanguages(httpMessage->acceptLanguages);

    // Save queueId:
    Uint32 queueId = httpMessage->queueId;

    // Save userName and authType:
    String userName;
    String authType;
    Boolean closeConnect = httpMessage->getCloseConnect();

    PEG_TRACE((
        TRC_HTTP,
        Tracer::LEVEL3,
        "WSManOperationRequestDecoder::handleHTTPMessage()- "
        "httpMessage->getCloseConnect() returned %d",
        closeConnect));

    userName = httpMessage->authInfo->getAuthenticatedUser();
    authType = httpMessage->authInfo->getAuthType();

    // Parse the HTTP message:
    String startLine;
    Array<HTTPHeader> headers;
    char* content;
    Uint32 contentLength;
    String contentType;

    httpMessage->parse(startLine, headers, contentLength);

    // Parse the request line:
    String methodName;
    String requestUri;
    String httpVersion;
    HttpMethod httpMethod = HTTP_METHOD__POST;

    HTTPMessage::parseRequestLine(
        startLine, methodName, requestUri, httpVersion);

    //  Set HTTP method for the request
    if (methodName == "M-POST")
    {
        httpMethod = HTTP_METHOD_M_POST;
    }

    // Unsupported methods are caught in the HTTPAuthenticatorDelegator
    PEGASUS_ASSERT(methodName == "M-POST" || methodName == "POST");

    //  Mismatch of method and version is caught in HTTPAuthenticatorDelegator
    PEGASUS_ASSERT(!((httpMethod == HTTP_METHOD_M_POST) &&
                     (httpVersion == "HTTP/1.0")));

    // Process M-POST and POST messages:
    if (httpVersion == "HTTP/1.1")
    {
        // Validate the presence of a "Host" header.  The HTTP/1.1 
        // specification says this in section 14.23 regarding the Host 
        // header field:
        //
        //     All Internet-based HTTP/1.1 servers MUST respond with a 400 (Bad
        //     Request) status code to any HTTP/1.1 request message which lacks
        //     a Host header field.
        //
        // Note:  The Host header value is not validated.

        String hostHeader;
        Boolean hostHeaderFound = HTTPMessage::lookupHeader(
            headers, "Host", hostHeader, false);

        if (!hostHeaderFound)
        {
            MessageLoaderParms parms(
                "Server.WSManOperationRequestDecoder.MISSING_HOST_HEADER",
                "HTTP request message lacks a Host header field.");
            sendHttpError(
                queueId,
                HTTP_STATUS_BADREQUEST,
                "",
                MessageLoader::getMessage(parms),
                closeConnect);
            PEG_METHOD_EXIT();
            return;
        }
    }

    // Calculate the beginning of the content from the message size and
    // the content length.
    content = (char*) httpMessage->message.getData() +
        httpMessage->message.size() - contentLength;

    // Validate the "Content-Type" header:
    Boolean contentTypeHeaderFound = HTTPMessage::lookupHeader(
        headers, "Content-Type", contentType, true);
    if (!contentTypeHeaderFound || 
        !HTTPMessage::isSupportedContentType (contentType))
    {
        MessageLoaderParms parms(
            "Server.WSManOperationRequestDecoder.CONTENTTYPE_SYNTAX_ERROR",
            "HTTP Content-Type header error.");
        sendHttpError(
            queueId,
            HTTP_STATUS_BADREQUEST,
            "",
            MessageLoader::getMessage(parms),
            closeConnect);
        PEG_METHOD_EXIT();
        return;
    }

    // SoapAction header is optional, but if present, it must match
    // the content of <wsa:Action>
    String soapAction = String::EMPTY;
    HTTPMessage::lookupHeader(headers, "SOAPAction", soapAction, true);

    // Remove the quotes around the SOAPAction value
    if (soapAction[0] == '\"') 
    {
        soapAction.remove(0, 1);
    }
    if (soapAction[soapAction.size() - 1] == '\"') 
    {
        soapAction.remove(soapAction.size() - 1, 1);
    }

    // Validating content falls within UTF8
    // (required to be complaint with section C12 of Unicode 4.0 spec,
    // chapter 3.)
    Uint32 count = 0;
    while(count < contentLength)
    {
        if (!(isUTF8((char*) &content[count])))
        {
            MessageLoaderParms parms(
                "Server.WSManOperationRequestDecoder.INVALID_UTF8_CHARACTER",
                "Invalid UTF-8 character detected.");
            sendHttpError(
                queueId,
                HTTP_STATUS_BADREQUEST,
                "request-not-valid",
                MessageLoader::getMessage(parms),
                closeConnect);

            PEG_METHOD_EXIT();
            return;
        }
        UTF8_NEXT(content,count);
    }

    handleSoapMessage(
        queueId, 
        httpMethod, 
        content, 
        contentLength, 
        soapAction,
        authType, 
        userName, 
        httpMessage->ipAddress, 
        httpMessage->acceptLanguages, 
        httpMessage->contentLanguages,
        closeConnect);

    PEG_METHOD_EXIT();
}


void WSManOperationRequestDecoder::handleSoapMessage(
    Uint32 queueId,
    HttpMethod httpMethod,
    char* content,
    Uint32 contentLength,
    String& soapAction,
    const String& authType,
    const String& userName,
    const String& ipAddress,
    const AcceptLanguageList& httpAcceptLanguages,
    const ContentLanguageList& httpContentLanguages,
    Boolean closeConnect)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "WSManOperationRequestDecoder::handleSoapMessage()");

    // If CIMOM is shutting down, return "Service Unavailable" response
    if (_serverTerminating)
    {
        MessageLoaderParms parms(
            "Server.WSManOperationRequestDecoder.CIMSERVER_SHUTTING_DOWN",
            "CIM Server is shutting down.");
        sendHttpError(
            queueId,
            HTTP_STATUS_SERVICEUNAVAILABLE,
            String::EMPTY,
            MessageLoader::getMessage(parms),
            closeConnect);
        PEG_METHOD_EXIT();
        return;
    }

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "WSManOperationRequestdecoder - XML content: $0", content));

    SoapReader soapReader(content);
    AutoPtr<CIMOperationRequestMessage> request;

    // Process <?xml ... >
    try
    {
        // These values are currently unused
        const char* xmlVersion = 0;
        const char* xmlEncoding = 0;

        soapReader.getXmlDeclaration(xmlVersion, xmlEncoding);

        // Decode the SOAP envelope
        String action;
        SoapNamespaceType nsType;
        soapReader.processSoapEnvelope(soapAction);

        // Break down soapAction into namespace/action pair
        soapReader.decodeSoapAction(soapAction, action, nsType);

        switch (nsType)
        {
            case SOAP_NST_WS_TRANSFER:
            {
                if (action == "Get")
                {
                    request.reset(_decodeWSTransferGet(soapReader, queueId));
                }
                else if (action == "Put")
                {
                }
                else
                {
                    // TODO: throw an exception
                }
                break;
            }
            default:
            {
                // TODO: throw an exception
                break;
            }
        };
    }
    catch (XmlValidationError& e)
    {
        Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::TRACE,
            "WSManOperationRequestDecoder::handleSoapMessage - "
                "XmlValidationError exception has occurred. Message: $0",
            e.getMessage());

        sendHttpError(
            queueId,
            HTTP_STATUS_BADREQUEST,
            "request-not-valid",
            e.getMessage(),
            closeConnect);
        PEG_METHOD_EXIT();
        return;
    }
    catch (XmlSemanticError& e)
    {
        Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::TRACE,
            "WSManOperationRequestDecoder::handleSoapMessage - "
                "XmlSemanticError exception has occurred. Message: $0",
            e.getMessage());

        sendHttpError(
            queueId,
            HTTP_STATUS_BADREQUEST,
            "request-not-valid",
            e.getMessage(),
            closeConnect);
        PEG_METHOD_EXIT();
        return;
    }
    catch (XmlException& e)
    {
        Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::TRACE,
            "WSManOperationRequestDecoder::handleSoapMessage - "
                "XmlException has occurred. Message: $0",
            e.getMessage());

        sendHttpError(
            queueId,
            HTTP_STATUS_BADREQUEST,
            "request-not-well-formed",
            e.getMessage(),
            closeConnect);
        PEG_METHOD_EXIT();
        return;
    }
    catch (Exception& e)
    {
        // Don't know why I got this exception.  Seems like a bad thing.
        // Any exceptions we're expecting should be caught separately and
        // dealt with appropriately.  This is a last resort.
        sendHttpError(
            queueId,
            HTTP_STATUS_INTERNALSERVERERROR,
            String::EMPTY,
            e.getMessage(),
            closeConnect);
        PEG_METHOD_EXIT();
        return;
    }
    catch (...)
    {
        // Don't know why I got whatever this is.  Seems like a bad thing.
        // Any exceptions we're expecting should be caught separately and
        // dealt with appropriately.  This is a last resort.
        sendHttpError(
            queueId,
            HTTP_STATUS_INTERNALSERVERERROR,
            String::EMPTY,
            String::EMPTY,
            closeConnect);
        PEG_METHOD_EXIT();
        return;
    }

    STAT_BYTESREAD

    request->authType = authType;
    request->userName = userName;
    request->ipAddress = ipAddress;
    request->setHttpMethod (httpMethod);

    CIMMessage* cimmsg = dynamic_cast<CIMMessage*>(request.get());
    if (cimmsg != NULL)
    {
        cimmsg->operationContext.insert(IdentityContainer(userName));
        cimmsg->operationContext.set(
            AcceptLanguageListContainer(httpAcceptLanguages));
        cimmsg->operationContext.set(
            ContentLanguageListContainer(httpContentLanguages));
    }
    else
    {
        sendHttpError(
            queueId,
            HTTP_STATUS_INTERNALSERVERERROR,
            String::EMPTY,
            String::EMPTY,
            closeConnect);
        PEG_METHOD_EXIT();
        return;
    }

    request->setCloseConnect(closeConnect);
    _outputQueue->enqueue(request.release());

    PEG_METHOD_EXIT();
}


CIMGetInstanceRequestMessage* 
    WSManOperationRequestDecoder::_decodeWSTransferGet(
        SoapReader& soapReader, 
        Uint32 queueId)
{
    STAT_GETSTARTTIME

    String className;
    String nameSpace;
    String messageId;
    Array<CIMKeyBinding> keyBindings;
    CIMObjectPath instanceName;
    CIMPropertyList propertyList;

    // <wsman:ResourceURI> contains the class name.
    // <wsman:SelectorSet> contains key bindings, with one exception below.
    // <wsman:Selector Name="__cimnamespace"> has the namespace.
    soapReader.initSoapHeaderItr();
    SoapReader::SoapEntry* soapEntry;
    while ((soapEntry = soapReader.nextSoapHeaderEntry()) != 0)
    {
        if (soapReader.testSoapStartTag(
                soapEntry, SOAP_NST_WS_MAN, "ResourceURI"))
        {
            soapReader.decodeClassName(soapEntry, className);
        }
        else if (soapReader.testSoapStartTag(
                     soapEntry, SOAP_NST_WS_MAN, "SelectorSet"))
        {
            soapReader.decodeKeyBindings(soapEntry, keyBindings, nameSpace);
        }
        else if (soapReader.testSoapStartTag(
                     soapEntry, SOAP_NST_WS_ADDRESSING, "MessageID"))
        {
            soapReader.decodeMessageId(soapEntry, messageId);
        }
        else
        {
            // TODO:
            // For the tags we don't understand, we need to return an error
            // if they have mustUnderstand attribute set.
        }
    }

    // Now the instance name can be constructed from the class name 
    // in ResourceURI and key bindings in the SelectorSet.
    instanceName.set(String(), CIMNamespaceName(), className, keyBindings);

    AutoPtr<CIMGetInstanceRequestMessage> request(
        new CIMGetInstanceRequestMessage(
            messageId,
            nameSpace,
            instanceName,
            false,
            false,
            false,
            propertyList,
            QueueIdStack(queueId, _returnQueueId)));

    STAT_SERVERSTART

    return request.release();
}

PEGASUS_NAMESPACE_END
