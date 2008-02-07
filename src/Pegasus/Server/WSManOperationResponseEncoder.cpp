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
#include <cctype>
#include <cstdio>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
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
    : Base(PEGASUS_QUEUENAME_WSOPRESPENCODER)
{
}

WSManOperationResponseEncoder::~WSManOperationResponseEncoder()
{
}

void WSManOperationResponseEncoder::sendResponse(
    CIMResponseMessage* response,
    const String& name,
    Boolean isImplicit,
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

    if (! httpQueue)
    {
        PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "ERROR: Unknown queue type. queueId = %u, response not sent.",
            queueId));
        PEG_METHOD_EXIT();
        return;
    }

    Boolean isChunkRequest = httpQueue->isChunkRequested();
    HttpMethod httpMethod = response->getHttpMethod();
    String& messageId = response->messageId;
    CIMException& cimException = response->cimException;
    Buffer message;

    // Note: the language is ALWAYS passed empty to the xml formatters because
    // it is HTTPConnection that needs to make the decision of whether to add
    // the languages to the HTTP message.
    ContentLanguageList contentLanguage;

    CIMName cimName(name);
    Uint32 messageIndex = response->getIndex();
    Boolean isFirst = messageIndex == 0 ? true : false;
    Boolean isLast = response->isComplete();
    Buffer bodylocal;
    Buffer& body = bodygiven ? *bodygiven : bodylocal;

    // STAT_SERVEREND sets the getTotalServerTime() value in the message class
    STAT_SERVEREND

#ifndef PEGASUS_DISABLE_PERFINST
    Uint64 serverTime = response->getTotalServerTime();
#else
    Uint64 serverTime = 0;
#endif

    Buffer (*formatResponse)(
        const CIMName& iMethodName,
        const String& messageId,
        HttpMethod httpMethod,
        const ContentLanguageList& httpContentLanguages,
        const Buffer& body,
        Uint64 serverResponseTime,
        Boolean isFirst,
        Boolean isLast);

    Buffer (*formatError)(
        const CIMName& methodName,
        const String& messageId,
        HttpMethod httpMethod,
        const CIMException& cimException);

    if (isImplicit == false)
    {
        formatResponse = XmlWriter::formatSimpleMethodRspMessage;
        formatError = XmlWriter::formatSimpleMethodErrorRspMessage;
    }
    else
    {
        formatResponse = XmlWriter::formatSimpleIMethodRspMessage;
        formatError = XmlWriter::formatSimpleIMethodErrorRspMessage;
    }

    if (cimException.getCode() != CIM_ERR_SUCCESS)
    {
        STAT_SERVEREND_ERROR

        // only process the FIRST error
        if (httpQueue->cimException.getCode() == CIM_ERR_SUCCESS)
        {
            // NOTE: even if this error occurs in the middle, HTTPConnection
            // will flush the entire queued message and reformat.
            if (isChunkRequest == false)
                message =
                    formatError(name, messageId, httpMethod, cimException);

            // uri encode the error (for the http header) only when it is
            // non-chunking or the first error with chunking
            if (isChunkRequest == false ||
                (isChunkRequest == true && isFirst == true))
            {
                String msg =
                    TraceableCIMException(cimException).getDescription();
                String uriEncodedMsg = XmlWriter::encodeURICharacters(msg);
                CIMException cimExceptionUri(
                    cimException.getCode(), uriEncodedMsg);
                cimExceptionUri.setContentLanguages(
                    cimException.getContentLanguages());
                cimException = cimExceptionUri;
            }
        } // if first error in response stream

        // never put the error in chunked response (because it will end up in
        // the trailer), so just use the non-error response formatter to send
        // more data

        if (isChunkRequest == true)
        {
            message = formatResponse(
                cimName,
                messageId,
                httpMethod,
                contentLanguage,
                body,
                serverTime,
                isFirst,
                isLast);
        }
    }
    else
    {
        // else non-error condition
        try
        {
            message = formatResponse(
                cimName,
                messageId,
                httpMethod,
                contentLanguage,
                body,
                serverTime,
                isFirst,
                isLast);
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
            sendResponse(response, name, isImplicit);
            PEG_METHOD_EXIT();
            return;
        }

        STAT_BYTESSENT
    }

    AutoPtr<HTTPMessage> httpMessage(
        new HTTPMessage(message, 0, &cimException));
    httpMessage->setComplete(isLast);
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
        case CIM_GET_CLASS_RESPONSE_MESSAGE:
            break;

        case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
            break;

        case CIM_DELETE_CLASS_RESPONSE_MESSAGE:
            break;

        case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
            break;

        case CIM_CREATE_CLASS_RESPONSE_MESSAGE:
            break;

        case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
            break;

        case CIM_MODIFY_CLASS_RESPONSE_MESSAGE:
            break;

        case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
            break;

        case CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE:
            break;

        case CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE:
            break;

        case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
            break;

        case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
            break;

        case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
            break;

        case CIM_ASSOCIATORS_RESPONSE_MESSAGE:
            break;

        case CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE:
            break;

        case CIM_REFERENCES_RESPONSE_MESSAGE:
            break;

        case CIM_REFERENCE_NAMES_RESPONSE_MESSAGE:
            break;

        case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
            break;

        case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
            break;

        case CIM_GET_QUALIFIER_RESPONSE_MESSAGE:
            break;

        case CIM_SET_QUALIFIER_RESPONSE_MESSAGE:
            break;

        case CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE:
            break;

        case CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE:
            break;

        case CIM_INVOKE_METHOD_RESPONSE_MESSAGE:
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


PEGASUS_NAMESPACE_END
