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

#include <cctype>
#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/SoapReader.h>
#include <Pegasus/Common/SoapWriter.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/MessageLoader.h>
#include "WSManOperationResponseEncoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const String WSManOperationResponseEncoder::OUT_OF_MEMORY_MESSAGE =
    "A System error has occurred. Please retry the WS-Man Operation at a "
        "later time.";

WSManOperationResponseEncoder::WSManOperationResponseEncoder()
    : MessageQueueService(PEGASUS_QUEUENAME_WSOPRESPENCODER)
{
}

WSManOperationResponseEncoder::~WSManOperationResponseEncoder()
{
}

void WSManOperationResponseEncoder::sendResponse(
    CIMResponseMessage* response,
    const String& name,
    Buffer* bodygiven)
{
    static String funcname = "WSManOperationResponseEncoder::sendResponse: ";
    static String funcnameClassS = String(funcname + "for class " + name);
    static CString funcnameClass = funcnameClassS.getCString();
    PEG_METHOD_ENTER(TRC_DISPATCHER, funcnameClass);

    if (! response)
    {
        PEG_METHOD_EXIT();
        return;
    }

    Uint32 queueId = response->queueIds.top();
    response->queueIds.pop();

    Boolean closeConnect = response->getCloseConnect();
    PEG_TRACE((
        TRC_HTTP,
        Tracer::LEVEL3,
        "WSManOperationResponseEncoder::sendResponse()- "
            "response->getCloseConnect() returned %d",
        closeConnect));

    MessageQueue* queue = MessageQueue::lookup(queueId);

    if (!queue)
    {
        PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "ERROR: non-existent queueId = %u, response not sent.", queueId));
        PEG_METHOD_EXIT();
        return;
    }

    HTTPConnection* httpQueue = dynamic_cast<HTTPConnection*>(queue);

    if (!httpQueue)
    {
        PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "ERROR: Unknown queue type. queueId = %u, response not sent.",
            queueId));
        PEG_METHOD_EXIT();
        return;
    }

    if (httpQueue->isChunkRequested())
    {
        // We don't handle chunked WS-Man requests. Bail here.
        // TODO: throw an exception.
        return;
    }

    HttpMethod httpMethod = response->getHttpMethod();
    String& messageId = response->messageId;
    CIMException& cimException = response->cimException;
    Buffer message;

    // Note: the language is ALWAYS passed empty to the xml formatters because
    // it is HTTPConnection that needs to make the decision of whether to add
    // the languages to the HTTP message.
    ContentLanguageList contentLanguage;

    Uint32 messageIndex = response->getIndex();
    Buffer bodylocal;
    Buffer& body = bodygiven ? *bodygiven : bodylocal;

    // STAT_SERVEREND sets the getTotalServerTime() value in the message class
    STAT_SERVEREND

#ifndef PEGASUS_DISABLE_PERFINST
    Uint64 serverTime = response->getTotalServerTime();
#else
    Uint64 serverTime = 0;
#endif

    if (cimException.getCode() != CIM_ERR_SUCCESS)
    {
        STAT_SERVEREND_ERROR

        if (httpQueue->cimException.getCode() == CIM_ERR_SUCCESS)
        {
            message = SoapWriter::formatWSManErrorRspMessage(
                name, messageId, httpMethod, cimException);

            // uri encode the error (for the http header)
            String msg = TraceableCIMException(cimException).getDescription();
            String uriEncodedMsg = XmlUtils::encodeURICharacters(msg);
            CIMException cimExceptionUri(
                cimException.getCode(), uriEncodedMsg);
            cimExceptionUri.setContentLanguages(
                cimException.getContentLanguages());
            cimException = cimExceptionUri;
        }
    }
    else
    {
        // else non-error condition
        try
        {
            message = SoapWriter::formatWSManRspMessage(
                name,
                messageId,
                httpMethod,
                contentLanguage,
                body,
                serverTime);
        }
#if defined(PEGASUS_OS_TYPE_WINDOWS)
        catch (std::bad_alloc&)
#else
        catch (bad_alloc&)
#endif
        {
            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                funcname + OUT_OF_MEMORY_MESSAGE);

            cimException = PEGASUS_CIM_EXCEPTION_L(
                CIM_ERR_FAILED, MessageLoaderParms(
                    "Server.WSManOperationResponseEncoder.OUT_OF_MEMORY",
                    OUT_OF_MEMORY_MESSAGE));

            // try again with new error and no body
            body.clear();
            sendResponse(response, name);
            PEG_METHOD_EXIT();
            return;
        }

        STAT_BYTESSENT
    }

    AutoPtr<HTTPMessage> httpMessage(
        new HTTPMessage(message, 0, &cimException));
    httpMessage->setIndex(messageIndex);

    if (cimException.getCode() != CIM_ERR_SUCCESS)
    {
        httpMessage->contentLanguages = cimException.getContentLanguages();
    }
    else
    {
        const OperationContext::Container& container =
            response->operationContext.get(ContentLanguageListContainer::NAME);
        const ContentLanguageListContainer& listContainer =
            *dynamic_cast<const ContentLanguageListContainer*>(&container);
        contentLanguage = listContainer.getLanguages();
        httpMessage->contentLanguages = contentLanguage;
    }

    PEG_LOGGER_TRACE((
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "WSManOperationResponseEncoder::sendResponse - QueueId: $0  "
            "XML content: $1",queueId,
        String(message.getData(), message.size())));

    httpMessage->setCloseConnect(closeConnect);
    queue->enqueue(httpMessage.release());

    PEG_METHOD_EXIT();
}

void WSManOperationResponseEncoder::enqueue(Message* message)
{
    handleEnqueue(message);
}

void WSManOperationResponseEncoder::handleEnqueue(Message* message)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "WSManOperationResponseEncoder::handleEnqueue()");

    if (!message)
    {
        PEG_METHOD_EXIT();
        return;
    }

    CIMResponseMessage* response = dynamic_cast<CIMResponseMessage*>(message);
    PEGASUS_ASSERT(response);

    response->updateThreadLanguages();

    PEG_TRACE((
        TRC_HTTP,
        Tracer::LEVEL3,
        "WSManOperationResponseEncoder::handleEnque()- "
            "message->getCloseConnect() returned %d",
        message->getCloseConnect()));

    switch (message->getType())
    {
        case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
            _encodeGetInstanceResponse(
                (CIMGetInstanceResponseMessage*)message);
            break;

        case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
            _encodeDeleteInstanceResponse(
                (CIMDeleteInstanceResponseMessage*)message);
            break;

        case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
            _encodeCreateInstanceResponse(
                (CIMCreateInstanceResponseMessage*)message);
            break;

        case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
            _encodeModifyInstanceResponse(
                (CIMModifyInstanceResponseMessage*)message);
            break;

        case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
            _encodeEnumerateInstancesResponse(
                (CIMEnumerateInstancesResponseMessage*)message);
            break;

        case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
            _encodeEnumerateInstanceNamesResponse(
                (CIMEnumerateInstanceNamesResponseMessage*)message);
            break;

        case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
            _encodeGetPropertyResponse(
                (CIMGetPropertyResponseMessage*)message);
            break;

        case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
            _encodeSetPropertyResponse(
                (CIMSetPropertyResponseMessage*)message);
            break;

        default:
            // Unexpected message type
            PEGASUS_ASSERT(0);
            break;
    }

    delete message;

    PEG_METHOD_EXIT();
    return;
}

void WSManOperationResponseEncoder::handleEnqueue()
{
    Message* message = dequeue();
    if (message)
        handleEnqueue(message);
}

void WSManOperationResponseEncoder::_encodeGetInstanceResponse(
    CIMGetInstanceResponseMessage* response)
{
    Buffer body;
    if (response->cimException.getCode() == CIM_ERR_SUCCESS)
        SoapWriter::appendInstanceElement(body, response->cimInstance);
    sendResponse(
        response, 
        SoapUtils::getSoapActionName(
            SoapNamespaces::WS_TRANSFER, "GetResponse"), 
        &body);
}

/************************************************/
/************************************************/
/************************************************/
/************************************************/
#if 1
void WSManOperationResponseEncoder::_encodeCreateInstanceResponse(
    CIMCreateInstanceResponseMessage* response)
{
    Buffer body;
    if (response->cimException.getCode() == CIM_ERR_SUCCESS)
        XmlWriter::appendInstanceNameElement(body, response->instanceName);
    sendResponse(response, "CreateResponse", &body);
}

void WSManOperationResponseEncoder::_encodeModifyInstanceResponse(
    CIMModifyInstanceResponseMessage* response)
{
    sendResponse(response, "ModifyInstance");
}

void WSManOperationResponseEncoder::_encodeEnumerateInstancesResponse(
    CIMEnumerateInstancesResponseMessage* response)
{
    Buffer body;
    if (response->cimException.getCode() == CIM_ERR_SUCCESS)
        for (Uint32 i = 0, n = response->cimNamedInstances.size(); i < n; i++)
            XmlWriter::appendValueNamedInstanceElement(
                body, response->cimNamedInstances[i]);
    sendResponse(response, "EnumerateInstances", &body);
}

void WSManOperationResponseEncoder::_encodeEnumerateInstanceNamesResponse(
    CIMEnumerateInstanceNamesResponseMessage* response)
{
    Buffer body;
    if (response->cimException.getCode() == CIM_ERR_SUCCESS)
        for (Uint32 i = 0, n = response->instanceNames.size(); i < n; i++)
            XmlWriter::appendInstanceNameElement(
                body, response->instanceNames[i]);
    sendResponse(response, "EnumerateInstanceNames", &body);
}

void WSManOperationResponseEncoder::_encodeDeleteInstanceResponse(
    CIMDeleteInstanceResponseMessage* response)
{
    sendResponse(response, "DeleteInstance");
}

void WSManOperationResponseEncoder::_encodeGetPropertyResponse(
    CIMGetPropertyResponseMessage* response)
{
    Buffer body;
    if (response->cimException.getCode() == CIM_ERR_SUCCESS)
        XmlWriter::appendValueElement(body, response->value);
    sendResponse(response, "GetProperty", &body);
}

void WSManOperationResponseEncoder::_encodeSetPropertyResponse(
    CIMSetPropertyResponseMessage* response)
{
    sendResponse(response, "SetProperty");
}
#endif

PEGASUS_NAMESPACE_END
