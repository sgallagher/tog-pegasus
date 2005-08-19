//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//         Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Dave Rosckes (rosckes@us.ibm.com)
//         Amit Arora, IBM (amita@in.ibm.com)
//         Heather Sterling, IBM (hsterl@us.ibm.com)
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase1
//         Alagaraja Ramasubramanian (alags_raj@in.ibm.com) for Bug#1090
//         Amit K Arora, IBM (amita@in.ibm.com) for Bug#1097
//         Sushma Fernandes, IBM (sushma@hp.com) for Bug#2057
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//         Amit Arora, IBM (amita@in.ibm.com) for Bug#2541
//         David Dillard, VERITAS Software Corp. (david.dillard@veritas.com)
//         Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//         John Alex, IBM (johnalex@us.ibm.com) - Bug#2290
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>

#include <iostream>
#include <cctype>
#include <cstdlib>
#include "Socket.h"
#include "TLS.h"
#include "HTTPConnection.h"
#include "MessageQueue.h"
#include "Monitor.h"
#include "HTTPMessage.h"
#include "Signal.h"
#include "Tracer.h"

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
#include <Pegasus/Common/CIMKerberosSecurityAssociation.h>
#endif
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// initialize the request count

AtomicInt HTTPConnection::_requestCount = 0;

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

/*
 * string and number constants for HTTP sending/receiving
 */

// buffer size for sending receiving
static const Uint32 httpTcpBufferSize = 8192;

// string constants for HTTP header. "Name" represents strings on the left
// side of headerNameTerminator and "Value" represents strings on the right
// side of headerNameTerminator

#define headerNameTrailer "Trailer"
#undef CRLF
#define CRLF "\r\n"

static const char headerNameTransferTE[] = "TE";
static const char headerNameTransferEncoding[] = "transfer-encoding";
static const char headerNameContentLength[] = "content-length";
static const char headerValueTransferEncodingChunked[] = "chunked";
static const char headerValueTransferEncodingIdentity[] = "identity";
static const char headerValueTEchunked[] = "chunked";
static const char headerValueTEtrailers[] = "trailers";
static const char headerNameError[] = "CIMError";
static const char headerNameCode[] = "CIMStatusCode";
static const char headerNameDescription[] = "CIMStatusCodeDescription";
static const char headerNameOperation[] = "CIMOperation";
static const char headerNameContentLanguage[] = "Content-Language";

// the names comes from the HTTP specification on chunked transfer encoding

static const char headerNameTerminator[] = ": ";
static const char headerValueSeparator[] = ", ";
static const char headerLineTerminator[] = CRLF;
static const char headerTerminator[] = CRLF CRLF;
static const char chunkLineTerminator[] = CRLF;
static const char chunkTerminator[] = CRLF;
static const char chunkBodyTerminator[] = CRLF;
static const char trailerTerminator[] = CRLF;
static const char chunkExtensionTerminator[] = ";";

// string sizes

static const Uint32 headerNameContentLengthLength = sizeof(headerNameContentLength)-1;
static const Uint32 headerValueTransferEncodingChunkedLength = sizeof(headerValueTransferEncodingChunked)-1;
static const Uint32 headerNameTransferEncodingLength = sizeof(headerNameTransferEncoding)-1;
static const Uint32 headerNameTerminatorLength =sizeof(headerNameTerminator)-1;
static const Uint32 headerLineTerminatorLength =sizeof(headerLineTerminator)-1;
static const Uint32 chunkLineTerminatorLength = sizeof(chunkLineTerminator)-1;
static const Uint32 chunkTerminatorLength = sizeof(chunkTerminator)-1;
static const Uint32 chunkBodyTerminatorLength = sizeof(chunkBodyTerminator)-1;
static const Uint32 trailerTerminatorLength = sizeof(trailerTerminator)-1;
static const Uint32 chunkExtensionTerminatorLength = sizeof(chunkExtensionTerminator)-1;

// the number of bytes it takes to place a Uint32 into a string (minus null)
static const Uint32 numberAsStringLength = 10;

/*
 * given an HTTP status code, return the description. not all codes are listed
 * here. Unmapped codes result in the internal error string.
 * Add any required future codes here.
 */

static const String httpDetailDelimiter = headerValueSeparator;
static const String httpStatusInternal = HTTP_STATUS_INTERNALSERVERERROR;

/*
 * throw given http code with detail, file, line
 * This is shared client/server code. The caller will decide what to do
 * with the thrown message
 */

static void _throwEventFailure(const String &status, const String &detail,
    const char *func,
    const char *file , Uint32 line)
{
    String message = status + httpDetailDelimiter + detail;
    Tracer::trace(file, line, TRC_HTTP, Tracer::LEVEL2, message);
    if (status == httpStatusInternal)
        throw AssertionFailureException(file, line, message);
    else throw Exception(message);
}

// throw a http exception. This is used for both client and server common code.
// The macro allows is used for file, line inclusion for debugging

#define _throwEventFailure(status, detail) \
  _throwEventFailure(status, String(detail), func, __FILE__, __LINE__)

static inline Uint32 _Min(Uint32 x, Uint32 y)
{
    return x < y ? x : y;
}

static char* _FindSeparator(const char* data, Uint32 size)
{
    const char* p = data;
    const char* end = p + size;

    while (p != end)
    {
        if (*p == '\r')
        {
            Uint32 n = end - p;

            if (n >= 2 && p[1] == '\n')
                return (char*)p;
        }
        else if (*p == '\n')
            return (char*)p;

        p++;
    }

    return 0;
}

// Used to test signal handling
void * sigabrt_generator(void * parm)
{
    abort();
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// HTTPConnection
//
////////////////////////////////////////////////////////////////////////////////

HTTPConnection::HTTPConnection(
    Monitor* monitor,
    AutoPtr<MP_Socket>& socket,
    MessageQueue* ownerMessageQueue,
    MessageQueue* outputMessageQueue,
    Boolean exportConnection)
    :
    Base(PEGASUS_QUEUENAME_HTTPCONNECTION),
    _monitor(monitor),
    _socket(socket),
    _ownerMessageQueue(ownerMessageQueue),
    _outputMessageQueue(outputMessageQueue),
    _contentOffset(-1),
    _contentLength(-1),
    _connectionClosePending(false),
    _acceptPending(false)
{
    PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::HTTPConnection");

    //Socket::disableBlocking(_socket);
    _socket->disableBlocking();
    _authInfo.reset(new AuthenticationInfo(true));

    // Add SSL verification information to the authentication information
    if (_socket->isSecure())
    {
        //
        // Set the flag to indicate that the request was received on
        // export Connection
        //
        if (exportConnection)
        {
           _authInfo->setExportConnection(exportConnection);
        }

        if (_socket->isPeerVerificationEnabled() && _socket->isCertificateVerified())
        {
            _authInfo->setAuthStatus(AuthenticationInfoRep::AUTHENTICATED);
            _authInfo->setAuthType(AuthenticationInfoRep::AUTH_TYPE_SSL);
            _authInfo->setClientCertificate(_socket->getPeerCertificate());
        }
    }

    _responsePending = false;
    _connectionRequestCount = 0;
    _transferEncodingChunkOffset = 0;

    PEG_METHOD_EXIT();
}

HTTPConnection::~HTTPConnection()
{
    PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::~HTTPConnection");

     _socket->close();

    PEG_METHOD_EXIT();
}


void HTTPConnection::handleEnqueue(Message *message)
{
    PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::handleEnqueue");

    if( ! message )
    {
        PEG_METHOD_EXIT();
        return;
    }

    AutoMutex connectionLock(_connection_mut, false);
    if (pegasus_thread_self() != _connection_mut.get_owner())
    {
        connectionLock.lock();
    }

    switch (message->getType())
    {
        case SOCKET_MESSAGE:
        {
            Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                "HTTPConnection::handleEnqueue - SOCKET_MESSAGE");
            SocketMessage* socketMessage = (SocketMessage*)message;
            if (socketMessage->events & SocketMessage::READ)
                _handleReadEvent();
            break;
        }

        case HTTP_MESSAGE:
        {
            Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                "HTTPConnection::handleEnqueue - HTTP_MESSAGE");

            _handleWriteEvent(*message);
            break;
        }

        default:
            // ATTN: need unexpected message error!
            break;
   } // switch

   delete message;

   PEG_METHOD_EXIT();
}

/*
 * handle the message coming down from the above. This is shared client and
 * server code. If the message is coming in chunks, then validate the chunk
 * sequence number. If the message is being processed on the server side,
 * make sure the client has requested transfer encodings and/or trailers before
 * sending them. If not, the message must be queued up until a complete flag
 * has arrived.
 */

Boolean HTTPConnection::_handleWriteEvent(Message &message)
{
    static const char func[] = "HTTPConnection::_handleWriteEvent";
    String httpStatus;
    HTTPMessage& httpMessage = *(HTTPMessage*)&message;
    Array<char>& buffer = httpMessage.message;
    Boolean isFirst = message.isFirst();
    Boolean isLast = message.isComplete();
    Sint32 totalBytesWritten = 0;
    Uint32 messageLength = buffer.size();

    try
    {
        // delivery behavior:
        // 1 handler.processing() : header + optional body?
        // 2 handler.deliver()    : 1+ fully XML encoded object(s)
        // 3 handler.complete()   : deliver() + isLast = true

        Uint32 bytesRemaining = messageLength;
        char *messageStart = (char *) buffer.getData();
        Uint32 bytesToWrite = httpTcpBufferSize;
        Uint32 messageIndex = message.getIndex();
        Boolean isChunkResponse = false;
        Boolean isChunkRequest = false;
        Boolean isFirstException = false;

        if (_isClient() == false)
        {
            // for null termination
            buffer.reserveCapacity(messageLength + 1);
            messageStart = (char *) buffer.getData();
            messageStart[messageLength] = 0;

            if (isFirst == true)
            {
                _incomingBuffer.clear();
                // tracks the message coming from above
                _transferEncodingChunkOffset = 0;
                _mpostPrefix.clear();
                cimException = CIMException();
                _responsePending = true;
            }
            else
            {
                // this is coming from our own internal code, therefore it is an
                // internal error. somehow the chunks came out of order.
                if (_transferEncodingChunkOffset+1 != messageIndex)
                    _throwEventFailure(httpStatusInternal, "chunk sequence mismatch");
                _transferEncodingChunkOffset++;
            }

            // save the first error
            if (httpMessage.cimException.getCode() != CIM_ERR_SUCCESS)
            {
                httpStatus = httpMessage.cimException.getMessage();
                if (cimException.getCode() == CIM_ERR_SUCCESS)
                {
                    cimException = httpMessage.cimException;
                    // set language to first error language (overriding anything there)
                    contentLanguages = cimException.getContentLanguages();
                    isFirstException = true;
                }
            }
            else if (cimException.getCode() == CIM_ERR_SUCCESS)
            {
                if (isFirst == true)
                    contentLanguages = httpMessage.contentLanguages;
                else if (httpMessage.contentLanguages != contentLanguages)
                    contentLanguages = ContentLanguages::EMPTY;
                else contentLanguages = httpMessage.contentLanguages;
            }
            // check to see if the client requested chunking OR trailers. trailers
            // are tightly integrated with chunking, so it can also be used.

            if (isChunkRequested() == true)
            {
                isChunkRequest = true;
            }
            else
            {
                // we are not sending chunks because the client did not request it

                // save the entire FIRST error response for non-chunked error responses
                // this will be used as the error message

                if (isFirstException == true)
                {
                    // this shouldnt happen, but this is defensive ...
                    if (messageLength == 0)
                    {
                        CIMStatusCode code = httpMessage.cimException.getCode();
                        String httpDetail(cimStatusCodeToString(code));
                        char s[21];
                        sprintf(s, "%u", code);
                        String httpStatus(s);
                        Array<char> message = XmlWriter::formatHttpErrorRspMessage
                            (httpStatus, String(), httpDetail);
                        messageLength = message.size();
                        message.reserveCapacity(messageLength+1);
                        messageStart = (char *) message.getData();
                        messageStart[messageLength] = 0;
                    }
                    cimException = CIMException(cimException.getCode(),
                        String(messageStart, messageLength));
                }

                if (isFirst == false)
                {
                    // subsequent chunks from the server, just append

                    messageLength += _incomingBuffer.size();
                    _incomingBuffer.reserveCapacity(messageLength+1);
                    _incomingBuffer.appendArray(buffer);
                    buffer.clear();
                    // null terminate
                    messageStart = (char *) _incomingBuffer.getData();
                    messageStart[messageLength] = 0;
                    // put back in buffer, so the httpMessage parser can work below
                    _incomingBuffer.swap(buffer);
                }

                if (isLast == false)
                {
                    // this tells the send loop below to do nothing until we are at the
                    // last response
                    bytesRemaining = 0;
                }
                else
                {
                    if (cimException.getCode() != CIM_ERR_SUCCESS)
                    {
                        buffer.clear();
                        // discard all data collected to this point
                        _incomingBuffer.clear();
                        String messageS = cimException.getMessage();
                        CString messageC = messageS.getCString();
                        messageStart = (char *) (const char *) messageC;
                        messageLength = strlen(messageStart);
                        buffer.reserveCapacity(messageLength+1);
                        buffer.append(messageStart, messageLength);
                        // null terminate
                        messageStart = (char *) buffer.getData();
                        messageStart[messageLength] = 0;
                    }
                    bytesRemaining = messageLength;
                }

            } // if not sending chunks

            // We now need to adjust the contentLength line.
            // If chunking was requested and this is the first chunk, then we need
            // to enter this block so we can adjust the header and send to the client
            // the first set of bytes right away.
            // If chunking was NOT requested, we have to wait for the last chunk of
            // the message to get (and set) the size of the content because we are
            // going to send it the traditional (i.e non-chunked) way

            if (isChunkRequest == true && isFirst == true ||
                    isChunkRequest == false && isLast == true)
            {
                // need to find the end of the header
                String startLine;
                Array<HTTPHeader> headers;
                Uint32 contentLength = 0;

                // Note: this gets the content length from subtracting the header
                // length from the messageLength, not by parsing the content length
                // header field

                httpMessage.parse(startLine, headers, contentLength);
                Uint32 httpStatusCode = 0;
                String httpVersion;
                String reasonPhrase;
                Boolean isValid = httpMessage.
                    parseStatusLine(startLine, httpVersion, httpStatusCode,reasonPhrase);
                Uint32 headerLength = messageLength - contentLength;
                char save = messageStart[headerLength];
                messageStart[headerLength] = 0;

                char *contentLengthStart =
                    strstr(messageStart, headerNameContentLength);

                char* contentLengthEnd = contentLengthStart ?
                    strstr(contentLengthStart, headerLineTerminator) : 0;

                messageStart[headerLength] = save;

                // the message may or may not have the content length specified
                // depending on the type of request it is

                if (contentLengthStart)
                {
                    // the message has the content length specified.
                    // If we are NOT sending a chunked response, then we need to overlay
                    // the contentLength number to reflect the actual byte count of the
                    // content (i.e message body). If we ARE sending a chunked response,
                    // then we will overlay the transferEncoding keyword name and value
                    // on top of the contentLength keyword and value.

                    // Important note:
                    // for performance reasons, the contentLength and/or transferEncoding
                    // strings are being overlayed DIRECTLY inside the message buffer
                    // WITHOUT changing the actual length in bytes of the message.
                    // The XmlWriter has been modified to pad out the maximum number in
                    // zeros to accomodate any number. The maximum contentLength name and
                    // value is identical to the transferEncoding name and value and can
                    // be easily interchanged. By doing this, we do not have to piece
                    // together the header (and more importantly, the lengthy body)
                    // all over again!
                    // This is why the http line lengths are validated below

                    Uint32 transferEncodingLineLengthExpected =
                        headerNameTransferEncodingLength +
                        headerNameTerminatorLength +
                        headerValueTransferEncodingChunkedLength;

                    Uint32 contentLengthLineLengthExpected =
                        headerNameContentLengthLength +
                        headerNameTerminatorLength + numberAsStringLength;

                    Uint32 contentLengthLineLengthFound =
                        contentLengthEnd - contentLengthStart;

                    if (isValid == false || ! contentLengthEnd ||
                            contentLengthLineLengthFound !=
                            transferEncodingLineLengthExpected ||
                            transferEncodingLineLengthExpected !=
                            contentLengthLineLengthExpected)
                    {
                        // these should match up since this is coming directly from our
                        // code in XmlWriter! If not,some code changes have got out of sync

                        _throwEventFailure(httpStatusInternal,
                            "content length was incorrectly formatted");
                    }

                    // we will be sending a chunk response if:
                    // 1. chunking has been requested AND
                    // 2. contentLength has been set
                    //    (meaning a non-bodyless message has come in) OR
                    // 3. this is not the last message
                    //  (meaning the data is coming in pieces and we should send chunked)

                    if (isChunkRequest == true &&    (contentLength > 0 || isLast == false))
                        isChunkResponse = true;

                    save = contentLengthStart[contentLengthLineLengthExpected];
                    contentLengthStart[contentLengthLineLengthExpected] = 0;

                    // overlay the contentLength value
                    if (isChunkResponse == false)
                    {
                        // overwrite the content length number with the actual byte count
                        char *contentLengthNumberStart = contentLengthStart +
                            headerNameContentLengthLength + headerNameTerminatorLength;
                        char format[6];
                        sprintf (format, "%%.%uu", numberAsStringLength);
                        // overwrite the bytes in buffer with the content encoding length
                        sprintf(contentLengthNumberStart, format, contentLength);
                        contentLengthStart[contentLengthLineLengthExpected] = save;
                    }
                    else
                    {
                        // overlay the contentLength name and value with the
                        // transferEncoding name and value

                        sprintf(contentLengthStart, "%s%s%s",headerNameTransferEncoding,
                            headerNameTerminator,headerValueTransferEncodingChunked);
                        bytesToWrite = messageLength - contentLength;

                        contentLengthStart[contentLengthLineLengthExpected] = save;
                        String operationName = headerNameOperation;
                        // look for 2-digit prefix (if mpost was use)
                        HTTPMessage::lookupHeaderPrefix(headers, operationName,
                            _mpostPrefix);
                    } // else chunk response is true

                } // if content length was found

                if (isChunkRequest == false)
                {
                    if (isLast == true)
                    {
                        if (contentLanguages != ContentLanguages::EMPTY)
                        {
                            // we must insert the content-language into the header
                            Array<char> contentLanguagesString;

                            // this is the keyword:value(s) + header line terminator
                            contentLanguagesString << headerNameContentLanguage <<
                                headerNameTerminator <<
                                contentLanguages.toString().getCString() <<
                                headerLineTerminator;

                            Uint32 insertOffset = headerLength - headerLineTerminatorLength;
                            messageLength = contentLanguagesString.size() + buffer.size();
                            buffer.reserveCapacity(messageLength+1);
                            messageLength = contentLanguagesString.size();
                            messageStart = (char *)contentLanguagesString.getData();
                            // insert the content language line before end of header
                            // note: this can be expensive on large payloads
                            buffer.insert(insertOffset, messageStart, messageLength);
                            messageLength = buffer.size();
                            // null terminate
                            messageStart = (char *) buffer.getData();
                            messageStart[messageLength] = 0;
                            bytesRemaining = messageLength;
                        } // if there were any content languages

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
                        // The following is processing to wrap (encrypt) the response from the
                        // server when using kerberos authentications.
                        // If the security association does not exist then kerberos authentication
                        // is not being used.
                        CIMKerberosSecurityAssociation *sa = _authInfo->getSecurityAssociation();

                        if (sa)
                        {
                            // The message needs to be parsed in order to distinguish between the
                            // headers and content. When parsing, the code breaks out
                            // of the loop as soon as it finds the double separator that terminates
                            // the headers so the headers and content can be easily separated.

                            Boolean authrecExists = false;
                            String authorization = String::EMPTY;
                            if (HTTPMessage::lookupHeader(headers, "WWW-Authenticate",
                                                          authorization, false))
                            {
                                authrecExists = true;
                            }

                            // The following is processing to wrap (encrypt) the response from the
                            // server when using kerberos authentications.
                            sa->wrapResponseMessage(buffer, contentLength, authrecExists);
                            messageLength = buffer.size();

                            // null terminate
                            messageStart = (char *) buffer.getData();
                            messageStart[messageLength] = 0;
                            bytesRemaining = messageLength;
                        }  // endif kerberos security assoc exists
#endif
                    } // if this is the last chunk
                    else bytesRemaining = 0;
                } // if chunk request is false
            } // if this is the first chunk containing the header
            else
            {
                // if chunking was requested, then subsequent messages that are
                // received need to turn response chunking on

                if (isChunkRequest == true && messageIndex > 0)
                {
                    isChunkResponse = true;
                    bytesToWrite = messageLength;
                }
            }

            // the data is sitting in buffer, but we want to cache it in
            // _incomingBuffer because there may be more chunks to append
            if (isChunkRequest == false)
                _incomingBuffer.swap(buffer);

        } // if not a client

        // ATTN: convert over to asynchronous write scheme:
        // Send response message to the client (use synchronous I/O for now:
        _socket->enableBlocking();
        SignalHandler::ignore(PEGASUS_SIGPIPE);

        // use the next four lines to test the SIGABRT handler
        //getSigHandle()->registerHandler(PEGASUS_SIGABRT, sig_act);
        //getSigHandle()->activate(PEGASUS_SIGABRT);
        //Thread t(sigabrt_generator, NULL, false);
        //t.run();

        static const char errorSocket[] = "socket write error";
        char *sendStart = messageStart;
        Sint32 bytesWritten = 0;

        if (isFirst == true && isChunkResponse == true && bytesToWrite > 0)
        {
            // send the header first for chunked reponses.

            // dont include header terminator yet
            Uint32 headerLength = bytesToWrite;
            bytesToWrite -= headerLineTerminatorLength;

            bytesWritten = _socket->write(sendStart, bytesToWrite);
            if (bytesWritten < 0)
                _throwEventFailure(httpStatusInternal, errorSocket);
            totalBytesWritten += bytesWritten;
            bytesRemaining -= bytesWritten;

            // put in trailer header.
            Array<char> trailer;
            trailer << headerNameTrailer << headerNameTerminator <<
                _mpostPrefix << headerNameCode <<    headerValueSeparator <<
                _mpostPrefix << headerNameDescription << headerValueSeparator <<
                headerNameContentLanguage << headerLineTerminator;
            sendStart = (char *) trailer.getData();
            bytesToWrite = trailer.size();
            bytesWritten = _socket->write(sendStart, bytesToWrite);

            if (bytesWritten < 0)
                _throwEventFailure(httpStatusInternal, errorSocket);
            totalBytesWritten += bytesWritten;
            // the trailer is outside the header buffer, so dont include in
            // tracking variables

            // now send header terminator
            bytesToWrite = headerLineTerminatorLength;
            sendStart = messageStart + headerLength - bytesToWrite;
            bytesWritten = _socket->write(sendStart, bytesToWrite);
            if (bytesWritten < 0)
                _throwEventFailure(httpStatusInternal, errorSocket);
            totalBytesWritten += bytesWritten;
            bytesRemaining -= bytesWritten;

            messageStart += headerLength;
            messageLength -= headerLength;
            sendStart = messageStart;
            bytesWritten = 0;
            bytesToWrite = bytesRemaining;
        } // if first chunk of chunked response

        // room enough for hex string representing chunk length and terminator
        char chunkLine[sizeof(Uint32)*2 + chunkLineTerminatorLength+1];

        for (; bytesRemaining > 0; )
        {
            if (isChunkResponse == true)
            {
                // send chunk line containing hex string and chunk line terminator
                sprintf(chunkLine, "%x%s", bytesToWrite, chunkLineTerminator);
                sendStart = chunkLine;
                Sint32 chunkBytesToWrite = strlen(sendStart);
                bytesWritten = _socket->write(sendStart, chunkBytesToWrite);
                if (bytesWritten < 0)
                    _throwEventFailure(httpStatusInternal, errorSocket);
                totalBytesWritten += bytesWritten;
            }

            // for chunking, we will send the entire chunk data in one send, but
            // for non-chunking, we will send incrementally
            else bytesToWrite = _Min(bytesRemaining, bytesToWrite);

            // send non-chunked data
            sendStart = messageStart + messageLength - bytesRemaining;
            bytesWritten = _socket->write(sendStart, bytesToWrite);
            if (bytesWritten < 0)
                _throwEventFailure(httpStatusInternal, errorSocket);
            totalBytesWritten += bytesWritten;
            bytesRemaining -= bytesWritten;

            if (isChunkResponse == true)
            {
                // send chunk terminator, on the last chunk, it is the chunk body
                // terminator
                Array<char> trailer;
                trailer << chunkLineTerminator;

                // on the last chunk, attach the last chunk termination sequence:
                // 0 + last chunk terminator + optional trailer + chunkBodyTerminator

                if (isLast == true)
                {
                    if (bytesRemaining > 0)
                        _throwEventFailure(httpStatusInternal,
                            "more bytes after indicated last chunk");
                    trailer << "0" << chunkLineTerminator;
                    Uint32 httpStatus = cimException.getCode();

                    if (httpStatus != 0)
                    {
                        char httpStatusP[11];
                        sprintf(httpStatusP, "%u",httpStatus);

                        trailer << _mpostPrefix << headerNameCode << headerNameTerminator
                            << httpStatusP << headerLineTerminator;
                        const String& httpDescription = cimException.getMessage();
                        if (httpDescription.size() != 0)
                            trailer << _mpostPrefix << headerNameDescription <<
                                headerNameTerminator << httpDescription << headerLineTerminator;
                    }

                    // Add Content-Language to the trailer if requested
                    if (contentLanguages != ContentLanguages::EMPTY)
                    {
                        trailer << _mpostPrefix
                            << headerNameContentLanguage << headerNameTerminator
                            << contentLanguages.toString()
                            << headerLineTerminator;
                    }

                    // now add chunkBodyTerminator
                    trailer << chunkBodyTerminator;
                } // if isLast

                sendStart = (char *) trailer.getData();
                Sint32 chunkBytesToWrite = (Sint32) trailer.size();
                bytesWritten = _socket->write(sendStart, chunkBytesToWrite);
                if (bytesWritten < 0)
                    _throwEventFailure(httpStatusInternal, errorSocket);
                totalBytesWritten += bytesWritten;
            } // isChunkResponse == true

        } // for all bytes in message

    } // try

    catch (Exception &e)
    {
        httpStatus = e.getMessage();
    }
    catch (...)
    {
        httpStatus = HTTP_STATUS_INTERNALSERVERERROR;
        String message("Unknown internal error");
        Tracer::trace(__FILE__, __LINE__, TRC_HTTP, Tracer::LEVEL2, message);
    }

    if (isLast == true)
    {
        _incomingBuffer.clear();
        _transferEncodingTEValues.clear();

        //
        // decrement request count
        //

        _requestCount--;

        if (httpStatus.size() == 0)
        {
            static const char msg[] =
                "A response has been sent (%d of %d bytes have been written).\n"
                "There are %d requests pending within the CIM Server.\n"
                "A total of %d requests have been processed on this connection.";

            Tracer::trace(TRC_HTTP, Tracer::LEVEL4, msg, totalBytesWritten,
                messageLength, _requestCount.value(), _connectionRequestCount);
        }

        //
        // Since we are done writing, update the status of entry to IDLE
        // and notify the Monitor.
        //
        if (_isClient() == false)
        {
            // Check for message to close
            if(message.getCloseConnect()== true)
            {
                Tracer::trace(
                    TRC_HTTP,
                    Tracer::LEVEL3,
                    "HTTPConnection::_handleWriteEvent - Connection: Close in client message.");
                    _closeConnection();
            }else {
                Tracer::trace (TRC_HTTP, Tracer::LEVEL2,
                    "Now setting state to %d", _MonitorEntry::IDLE);
                _monitor->setState (_entry_index, _MonitorEntry::IDLE);
                _monitor->tickle();
            }
            _responsePending = false;
            cimException = CIMException();
        }
    }

    _socket->disableBlocking();
    return httpStatus.size() == 0 ? false : true;

}

void HTTPConnection::handleEnqueue()
{
   Message* message = dequeue();

    if (!message)
        return;
    handleEnqueue(message);
}

Boolean _IsBodylessMessage(const char* line)
{
    //ATTN: Make sure this is the right place to check for HTTP/1.1 and
    //      HTTP/1.0 that is part of the authentication challenge header.
    // ATTN-RK-P2-20020305: How do we make sure we have the complete list?

    // List of request methods which do not have message bodies
    const char* METHOD_NAMES[] =
    {
        "GET",
        "HEAD"
    };

    // List of response codes which the client accepts and which should not (normally) have
    // message bodies.  The RFC is vague regarding which response codes support or require bodies.
    // These are being reported by class (4xx, 5xx, etc) because the CIM client should be able to handle
    // any status code, including those not explicitly defined in RFC 2616.  Therefore, listing codes individually
    // will not work because the client socket will hang on a code not in this list if no content length is specified.
    // See bugzilla 1586
    const char* RESPONSE_CODES[] =
    {
        "HTTP/1.1 3XX",
        "HTTP/1.0 3XX",
        "HTTP/1.1 4XX",
        "HTTP/1.0 4XX",
        "HTTP/1.1 5XX",
        "HTTP/1.0 5XX"
    };

    // Check for bodyless HTTP request method
    const Uint32 METHOD_NAMES_SIZE = sizeof(METHOD_NAMES) / sizeof(char*);

    for (Uint32 i = 0; i < METHOD_NAMES_SIZE; i++)
    {
        Uint32 n = strlen(METHOD_NAMES[i]);

        if (strncmp(line, METHOD_NAMES[i], n) == 0 && isspace(line[n]))
            return true;
    }

    // Check for bodyless HTTP status code
    const Uint32 RESPONSE_CODES_SIZE = sizeof(RESPONSE_CODES) / sizeof(char*);

    for (Uint32 i = 0; i < RESPONSE_CODES_SIZE; i++)
    {
        Uint32 n = strlen(RESPONSE_CODES[i]);

        if (strncmp(line, RESPONSE_CODES[i], n - 2) == 0 && isspace(line[n]))
                return true;
            }

    return false;
}

/*
Boolean _IsBodylessMessage(const char* line)
{
    //ATTN: Make sure this is the right place to check for HTTP/1.1 and
    //      HTTP/1.0 that is part of the authentication challenge header.
    // ATTN-RK-P2-20020305: How do we make sure we have the complete list?
    const char* METHOD_NAMES[] =
    {
        "GET",
        "HTTP/1.1 400",
        "HTTP/1.0 400",
        "HTTP/1.1 401",
        "HTTP/1.0 401",
        "HTTP/1.1 413",
        "HTTP/1.0 413",
        "HTTP/1.1 500",
        "HTTP/1.0 500",
        "HTTP/1.1 501",
        "HTTP/1.0 501",
        "HTTP/1.1 503",
        "HTTP/1.0 503"
    };

    const Uint32 METHOD_NAMES_SIZE = sizeof(METHOD_NAMES) / sizeof(char*);

    for (Uint32 i = 0; i < METHOD_NAMES_SIZE; i++)
    {
        Uint32 n = strlen(METHOD_NAMES[i]);

        if (strncmp(line, METHOD_NAMES[i], n) == 0 && isspace(line[n]))
            return true;
    }

    return false;
}*/

void HTTPConnection::_getContentLengthAndContentOffset()
{
    static const char func[] =
    "HTTPConnection::_getContentLengthAndContentOffset";
    Uint32 size = _incomingBuffer.size();
    if (size == 0)
        return;
    char* data = (char*)_incomingBuffer.getData();
    char* line = (char*)data;
    char* sep;
    Uint32 lineNum = 0;
    Boolean bodylessMessage = false;

    while ((sep = _FindSeparator(line, size - (line - data))))
    {
        char save = *sep;
        *sep = '\0';

        // Did we find the double separator which terminates the headers?

        if (line == sep)
        {
            *sep = save;
            line = sep + ((save == '\r') ? 2 : 1);
            _contentOffset = line - _incomingBuffer.getData();

            // reserve space for entire non-chunked message
            if (_contentLength > 0)
            {
                Uint32 capacity = (Uint32)(_contentLength + _contentOffset + 1);
                _incomingBuffer.reserveCapacity(capacity);
                data = (char *)_incomingBuffer.getData();
                data[capacity-1] = 0;
            }

            break;
        }

        // If this is one of the bodyless methods, then we can assume the
        // message is complete when the "\r\n\r\n" is encountered.

        if (lineNum == 0 && _IsBodylessMessage(line))
            bodylessMessage = true;

        // Look for the content-length if not already found:

        char* colon = strchr(line, ':');

        if (colon)
        {
            *colon  = '\0';

            // remove whitespace after colon before value
            char *valueStart = colon + 1;
            while(*valueStart == ' ' || *valueStart == '\t')
                valueStart++;

            // we found some non-whitespace token
            if (valueStart != sep)
            {
                char *valueEnd = sep - 1;

                // now remove whitespace from end of line back to last byte of value
                while(*valueEnd == ' ' || *valueEnd == '\t')
                    valueEnd--;

                char valueSave = *(valueEnd+1);

                if (System::strcasecmp(line, headerNameContentLength) == 0)
                {
                    if (_transferEncodingValues.size() == 0)
                        _contentLength = atoi(valueStart);
                    else _contentLength = -1;
                }
                else if (System::strcasecmp(line, headerNameTransferEncoding) == 0)
                {
                    _transferEncodingValues.clear();

                    if (strcmp(valueStart,headerValueTransferEncodingChunked) == 0)
                        _transferEncodingValues.append(headerValueTransferEncodingChunked);
                    else if (strcmp(valueStart,headerValueTransferEncodingIdentity) == 0)
                        ; // do nothing
                    else _throwEventFailure(HTTP_STATUS_NOTIMPLEMENTED,
                                                                    "unimplemented transfer-encoding value");
                    _contentLength = -1;
                }
                else if (System::strcasecmp(line, headerNameContentLanguage) == 0)
                {
                    // note: if this is a chunked header, then this will be ignored later
                    String contentLanguagesString(valueStart, valueEnd-valueStart+1);
                    try
                    {
                        contentLanguages = ContentLanguages(contentLanguagesString);
                    }
                    catch(...)
                    {
                        Tracer::trace(TRC_HTTP, Tracer::LEVEL2,
                            "HTTPConnection: ERROR: contentLanguages had parsing"
                                " failure. clearing languages. error data=%s",
                            (const char *)contentLanguagesString.getCString());
                        contentLanguages = ContentLanguages::EMPTY;
                    }
                }
                else if (System::strcasecmp(line, headerNameTransferTE) == 0)
                {
                    _transferEncodingTEValues.clear();
                    static const char valueDelimiter = ',';
                    char *valuesStart = valueStart;

                    // now tokenize the values
                    while (*valuesStart)
                    {
                        // strip off whitepsace from the front
                        while(*valuesStart == ' ' || *valuesStart == '\t')
                            valuesStart++;

                        if (valuesStart == valueEnd)
                            break;

                        char *v = strchr(valuesStart, valueDelimiter);
                        if (v)
                        {
                            if (v == valuesStart)
                            {
                                valuesStart++;
                                continue;
                            }
                            v--;
                            // strip off whitespace from the end
                            while(*v == ' ' || *v == '\t')
                                v--;
                            v++;
                            *v = 0;
                        }

                        _transferEncodingTEValues.append(valuesStart);

                        if (v)
                        {
                            *v = valueDelimiter;
                            valuesStart = v+1;
                        }
                        else break;
                    }
                }

                *(valueEnd+1) = valueSave;
            } // if some value tokens

            *colon = ':';
        }

        *sep = save;
        line = sep + ((save == '\r') ? 2 : 1);
        lineNum++;
    }

    if (_contentOffset != -1 && bodylessMessage)
        _contentLength = 0;
}

void HTTPConnection::_clearIncoming()
{
    _contentOffset = -1;
    _contentLength = -1;
    _incomingBuffer.clear();
    _mpostPrefix.clear();
    contentLanguages.clear();
}

void HTTPConnection::_closeConnection()
{
    // return - don't send the close connection message.
    // let the monitor dispatch function do the cleanup.
    PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::_closeConnection");
    _connectionClosePending = true;

    // NOTE: if there is a response pending while a close connection request
    // occurs, then this indicates potential activity on this connection apart
    // from this thread of execution (although this code here is locked, other
    // threads may be waiting on this one.)
    // The caller MUST check this value before attempting a delete of this
    // connnection, otherwise the delete may occur while other chunked responses
    // are waiting to be delivered through this connection.
    // This condition may happen on a client error/timeout/interrupt/disconnect

    if (_isClient() == false)
    {
        if (_responsePending == true)
        {
            Tracer::trace(TRC_HTTP, Tracer::LEVEL2,
                "HTTPConnection::_closeConnection - Close connection requested while "
                "responses are still expected on this connection. "
                "connection=0x%p, socket=%d\n", (void*)this, getSocket());

        }

        // still set to DYING
        Tracer::trace(TRC_HTTP, Tracer::LEVEL2,
            "Now setting state to %d", _MonitorEntry::DYING);
        _monitor->setState (_entry_index, _MonitorEntry::DYING);
        _monitor->tickle();
    }

    if (_connectionRequestCount == 0)
    {
        Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection::_closeConnection - Connection being closed without receiving any requests.");
    }

    PEG_METHOD_EXIT();

//    Message* message= new CloseConnectionMessage(_socket->getSocket));
//    message->dest = _ownerMessageQueue->getQueueId();
//    SendForget(message);
//    _ownerMessageQueue->enqueue(message);
}

Boolean HTTPConnection::isChunkRequested()
{
    Boolean answer = false;
    if (_transferEncodingTEValues.size() > 0 &&
        (Contains(_transferEncodingTEValues, String(headerValueTEchunked)) ||
         Contains(_transferEncodingTEValues, String(headerValueTEtrailers))))
        answer = true;

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    CIMKerberosSecurityAssociation *sa = _authInfo->getSecurityAssociation();

    if (sa)
    {
        answer = false;
    }
#endif

    return answer;
}

// determine if the current code being executed is on the client side

Boolean HTTPConnection::_isClient()
{
    return strcmp(get_owner().getQueueName(),
        PEGASUS_QUEUENAME_HTTPCONNECTOR) == 0 ? true : false;
}

/*
 * determine if the data read in should be treated as transfer encoded data.
 * If so, proceed to strip out the transfer encoded meta data within the body
 * but the headers relating to transfer encoding will remain unchanged.
 * One should refer to the transfer encoding section of the HTTP protocol
 * specification to understand the parsing semantics below.
 * NOTE: this function is coded as a syncronous read! The entire message will
 * be read in before the message leaves this class and is passed up to the
 * client application.
 */

void HTTPConnection::_handleReadEventTransferEncoding()
{
    static const char func[] = "HTTPConnection::_handleReadEventTransferEncoding";
    PEG_METHOD_ENTER(TRC_HTTP, func);
    Uint32 messageLength = _incomingBuffer.size();
    Uint32 headerLength = (Uint32) _contentOffset;

    // return immediately under these conditions:

    // - Header terminator has not been reached yet (_contentOffset < 0)
    // - This is a non-transfer encoded message because the content length
    //   has been set from the given header value (_contentLength > 0)
    //   (_contentLength == 0 means bodyless, so return too - section 4.3)
    // - The message read in so far is <= to the header length
    // - No transfer encoding has been declared in the header.

    if (_contentOffset < 0 || _contentLength >= 0 ||
        messageLength <= headerLength || _transferEncodingValues.size() == 0)
    {
        PEG_METHOD_EXIT();
        return;
    }

    // on the first chunk in the message, set the encoding offset to the content
    // offset

    if (_transferEncodingChunkOffset == 0)
        _transferEncodingChunkOffset = (Uint32) _contentOffset;

    char *headerStart = (char *) _incomingBuffer.getData();
    char *messageStart = headerStart;

    // loop thru the received data (so far) and strip out all chunked meta data.
    // this logic assumes that the data read in may be only partial at any point
    // during the parsing. the variable _transferEncodingChunkOffset represents
    // the byte offset (from the start of the message) of the last NON completed
    // chunk parsed within the message. Remember that the tcp reader has padded
    // the buffer with a terminating null for easy string parsing.

    for (;;)
    {
        // we have parsed the length, but not all bytes of chunk have been read
        // in yet
        if (_transferEncodingChunkOffset >= messageLength)
            break;

        // this is the length from _transferEncodingChunkOffset to the end
        // of the message (so far). It represents the bytes that have not been
        // processed yet

        Uint32 remainderLength = messageLength - _transferEncodingChunkOffset;

        // the start of the first fully non-parsed chunk of this interation
        char *chunkLineStart = messageStart + _transferEncodingChunkOffset;
        char *chunkLineEnd = chunkLineStart;

        // Find the end of the hex string representing the data portion length of
        // the current chunk. Note that we must hit at least one non-hexdigit
        // (except null) to know we have read in the complete number

        while (isxdigit(*chunkLineEnd))
            chunkLineEnd++;

        if (! *chunkLineEnd)
            break;

        // This is the parsed chunk length in hex. From here on, this many bytes
        // plus the chunk terminator (AFTER this chunk line is done) must be
        // read in to constitute a complete chunk in which
        // _transferEncodingChunkOffset can be incremented to the next chunk

        Uint32 chunkLengthParsed =
            (Uint32) strtoul((const char *)chunkLineStart, 0, 16);

        // this also covers strings stated even larger
        if (chunkLengthParsed == PEG_NOT_FOUND)
            _throwEventFailure(HTTP_STATUS_REQUEST_TOO_LARGE,
                "stated chunk length too large");

        char *chunkExtensionStart = chunkLineEnd;
        chunkLineEnd = strstr(chunkLineEnd, chunkLineTerminator);

        // If we have not received the chunk line terminator yet, then return and
        // wait for the next iteration. This is done because the hex length given
        // only represents the non-meta data, not the chunk line itself.

        if (!chunkLineEnd)
            break;

        // the token after the hex digit must be either the chunk line terminator
        // or the chunk extension terminator. If not, the sender has sent an
        // illegal chunked encoding syntax.

        if (strncmp(chunkExtensionStart, chunkExtensionTerminator,
                chunkExtensionTerminatorLength) != 0 &&
            strncmp(chunkExtensionStart, chunkLineTerminator,
                chunkLineTerminatorLength) != 0)
            _throwEventFailure(HTTP_STATUS_BADREQUEST, "missing chunk extension");

        chunkLineEnd += chunkLineTerminatorLength;
        Uint32 chunkLineLength = chunkLineEnd - chunkLineStart;
        Uint32 chunkMetaLength = chunkLineLength;
        if (chunkLengthParsed > 0)
            chunkMetaLength += chunkTerminatorLength;
        Uint32 chunkTerminatorOffset = _transferEncodingChunkOffset +
            chunkLineLength + chunkLengthParsed;

        // The parsed length represents the non-meta data bytes which starts
        // after the chunk line terminator has been received.
        // If we dont have enough remainder bytes to process from the length parsed
        // then return and wait for the next iteration.

        if (chunkLengthParsed + chunkMetaLength > remainderLength)
            break;

        // at this point we have a complete chunk. proceed and strip out meta-data
        // NOTE: any time "remove" is called on the buffer, many variables must be
        // recomputed to reflect the data removed.

        // remove the chunk length line
        _incomingBuffer.remove(_transferEncodingChunkOffset, chunkLineLength);
        messageLength = _incomingBuffer.size();
        // always keep the byte after the last data byte null for easy string
        // processing.
        messageStart[messageLength] = 0;

        // recalculate since we just removed the chunk length line
        chunkTerminatorOffset -= chunkLineLength;

        // is this the last chunk ?
        if (chunkLengthParsed == 0)
        {
            // We are at the last chunk. The only remaining data should be:
            // 1. optional trailer first
            // 2. message terminator (will remain on incoming buffer and passed up)

            remainderLength -= chunkLineLength;

            CIMStatusCode cimStatusCode = CIM_ERR_SUCCESS;
            Uint32 httpStatusCode = HTTP_STATUSCODE_OK;
            String httpStatus;
            String cimErrorValue;

            // is there an optional trailer ?
            if (remainderLength > chunkBodyTerminatorLength)
            {
                Uint32 trailerLength = remainderLength - chunkBodyTerminatorLength;
                Uint32 trailerOffset = _transferEncodingChunkOffset;
                char *trailerStart = messageStart + trailerOffset;
                char *trailerTerminatorStart = trailerStart + trailerLength -
                    trailerTerminatorLength;

                // no trailer terminator before end of chunk body ?
                if (strncmp(trailerTerminatorStart, trailerTerminator,
                        trailerTerminatorLength) != 0)
                    _throwEventFailure(HTTP_STATUS_BADREQUEST,
                        "No chunk trailer terminator received");

                Array<char> trailer;
                // add a dummy startLine so that the parser works
                trailer << " " << headerLineTerminator;

                char save = trailerStart[trailerLength];
                trailerStart[trailerLength] = 0;
                trailer << trailerStart;
                trailerStart[trailerLength] = save;

                _incomingBuffer.remove(trailerOffset, trailerLength);
                messageLength = _incomingBuffer.size();
                messageStart[messageLength] = 0;
                remainderLength -= trailerLength;

                // parse the trailer looking for the code and description
                String startLine;
                Array<HTTPHeader> headers;
                Uint32 contentLength = 0;
                HTTPMessage httpTrailer(trailer);
                httpTrailer.parse(startLine, headers, contentLength);

                String cimErrorName = headerNameError;
                // first look for cim error. this is an http level error
                Boolean found = false;

                found = httpTrailer.lookupHeader(headers, cimErrorName, cimErrorValue,
                    true);

                if (found == true)
                {
                    // we have a cim error. parse the header to get the original http
                    // level error if any, otherwise, we have to make one up.

                    Array<char> header(messageStart, headerLength);
                    String startLine;
                    Array<HTTPHeader> headers;
                    Uint32 contentLength = 0;
                    HTTPMessage httpHeader(header);
                    httpHeader.parse(startLine, headers, contentLength);
                    String httpVersion;
                    Boolean isValid = httpHeader.
                        parseStatusLine(startLine, httpVersion, httpStatusCode,httpStatus);
                    if (isValid == false || httpStatusCode == 0 ||
                            httpStatusCode == HTTP_STATUSCODE_OK)
                    {
                        // ATTN: make up our own http code if not given ?
                        httpStatusCode = (Uint32) HTTP_STATUSCODE_BADREQUEST;
                        httpStatus = HTTP_STATUS_BADREQUEST;
                    }
                }
                else
                {
                    String codeName = headerNameCode;
                    String codeValue;
                    found = httpTrailer.lookupHeader(headers, codeName, codeValue,
                        true);
                    if (found == true && codeValue.size() > 0 &&
                            (cimStatusCode = (CIMStatusCode)atoi(codeValue.getCString()))>0)
                    {
                        HTTPMessage::lookupHeaderPrefix(headers, codeName, _mpostPrefix);
                        httpStatus = _mpostPrefix + codeName + headerNameTerminator +
                            codeValue + headerLineTerminator;

                        // look for cim status description
                        String descriptionName = headerNameDescription;
                        String descriptionValue;
                        found = httpTrailer.lookupHeader(headers, descriptionName,
                            descriptionValue,
                            true);
                        if (descriptionValue.size() == 0)
                            descriptionValue = cimStatusCodeToString(cimStatusCode);

                        httpStatus = httpStatus + _mpostPrefix + descriptionName +
                            headerNameTerminator + descriptionValue + headerLineTerminator;

                    } // if found a cim status code

                    // Get Content-Language out of the trailer, if it is there
                    String contentLanguagesString;
                    found = httpTrailer.lookupHeader(headers,
                        headerNameContentLanguage,
                        contentLanguagesString,
                        true);

                    contentLanguages = ContentLanguages::EMPTY;
                    if (found == true && contentLanguagesString.size() > 0)
                    {
                        try
                        {
                            contentLanguages = ContentLanguages(contentLanguagesString);
                        }
                        catch(...)
                        {
                            Tracer::trace(TRC_HTTP, Tracer::LEVEL2,
                                "HTTPConnection: ERROR: contentLanguages had parsing"
                                    " failure. clearing languages. error data=%s",
                                (const char *)contentLanguagesString.getCString());
                            contentLanguages = ContentLanguages::EMPTY;
                        }
                    }

                } // else not a cim error
            } // if optional trailer present

            char *chunkBodyTerminatorStart =
                messageStart + _transferEncodingChunkOffset;

            // look for chunk body terminator
            if (remainderLength != chunkBodyTerminatorLength ||
                strncmp(chunkBodyTerminatorStart, chunkBodyTerminator,
                    chunkBodyTerminatorLength) != 0)
                _throwEventFailure(HTTP_STATUS_BADREQUEST,
                    "No chunk body terminator received");

            // else the remainder is just the terminator, which we will leave
            // on the incoming buffer and pass up
            // (as if a non-transfer message arrived)

            _transferEncodingChunkOffset = 0;
            _contentLength = messageLength - headerLength;

            if (httpStatusCode != HTTP_STATUSCODE_OK)
            {
                _handleReadEventFailure(httpStatus, cimErrorValue);
            }
            else if (cimStatusCode != CIM_ERR_SUCCESS)
            {
                // discard the XML payload data (body) according to cim operations spec
                // and add code and description to the header so the next layer can
                // interpret the error correctly

                _incomingBuffer.remove(headerLength, _contentLength);
                // remove the header line terminator
                _incomingBuffer.remove(headerLength - headerLineTerminatorLength,
                    headerLineTerminatorLength);
                // append new status
                _incomingBuffer.append(httpStatus.getCString(), httpStatus.size());

                _incomingBuffer.append(headerLineTerminator, headerLineTerminatorLength);
                // null terminate - the buffer is at least as long after removing
                char *data = (char *)_incomingBuffer.getData();
                data[_incomingBuffer.size()] = 0;
                _contentLength = 0;
                _contentOffset = 0;
            }

            break;
        } // if last chunk

        // we are NOT on the last chunk! validate that the offset where the chunk
        // terminator was found matches what the parsed chunk length claimed.

        if (strncmp(messageStart + chunkTerminatorOffset, chunkTerminator,
                chunkTerminatorLength) != 0)
            _throwEventFailure(HTTP_STATUS_BADREQUEST, "Bad chunk terminator");

        // now remove the chunk terminator
        _incomingBuffer.remove(chunkTerminatorOffset, chunkTerminatorLength);
        messageLength = _incomingBuffer.size();
        messageStart[messageLength] = 0;

        // jump to the start of the next chunk (which may not have been read yet)
        _transferEncodingChunkOffset = chunkTerminatorOffset;
    } // for all remaining bytes containing chunks

    PEG_METHOD_EXIT();
}

/*
 * Handle a failure on the read or an HTTP error. This is NOT meant for
 * errors found in the cim response or the trailer.
 * The http status MAY have the detailed message attached to it using the
 * detail delimiter.
 */

void HTTPConnection::_handleReadEventFailure(String &httpStatusWithDetail,
    String cimError)
{
    Uint32 delimiterFound = httpStatusWithDetail.find(httpDetailDelimiter);
    String httpDetail;
    String httpStatus;

    if (delimiterFound != PEG_NOT_FOUND)
    {
        httpDetail = httpStatus.subString(delimiterFound+1);
        httpStatus = httpStatus.subString(0, delimiterFound);
    }

    String combined = httpStatus + httpDetailDelimiter + httpDetail +
        httpDetailDelimiter + cimError;

    Tracer::trace(__FILE__, __LINE__, TRC_HTTP, Tracer::LEVEL2, combined);
    _requestCount++;
    Array<char> message;
    message = XmlWriter::formatHttpErrorRspMessage(httpStatus, cimError,
        httpDetail);
    HTTPMessage* httpMessage = new HTTPMessage(message);
    Tracer::traceBuffer(TRC_XML_IO, Tracer::LEVEL2,
        httpMessage->message.getData(),
        httpMessage->message.size());

    // this is common error code. If we are the server side, we want to send
    // back the error to the client, but if we are the client side, then we
    // simply want to queue up this error locally so the client app can receive
    // the error. The client side's own message queue name will be the same
    // as the connector name (the server would be acceptor)

    if (_isClient() == true)
    {
        httpMessage->dest = _outputMessageQueue->getQueueId();

        _outputMessageQueue->enqueue(httpMessage);

        _clearIncoming();
    }
    else
    {
        // else server side processing error - send back to client
        handleEnqueue(httpMessage);
    }
    _closeConnection();
}

void HTTPConnection::_handleReadEvent()
{
    static const char func[] = "HTTPConnection::_handleReadEvent()";
    PEG_METHOD_ENTER(TRC_HTTP, func);

    if (_acceptPending)
    {
        PEGASUS_ASSERT(!_isClient());

        Sint32 socketAcceptStatus = _socket->accept();

        if (socketAcceptStatus < 0)
        {
            PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "HTTPConnection: SSL_accept() failed");
            _closeConnection();
            PEG_METHOD_EXIT();
            return;
        }
        else if (socketAcceptStatus == 0)
        {
            // Not enough data yet to complete the SSL handshake
            PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL2,
                "HTTPConnection: SSL_accept() pending");
            PEG_METHOD_EXIT();
            return;
        }
        else
        {
            // Add SSL verification information to the authentication info
            if (_socket->isSecure() &&
                _socket->isPeerVerificationEnabled() &&
                _socket->isCertificateVerified())
            {
                _authInfo->setAuthStatus(AuthenticationInfoRep::AUTHENTICATED);
                _authInfo->setAuthType(AuthenticationInfoRep::AUTH_TYPE_SSL);
                _authInfo->setClientCertificate(_socket->getPeerCertificate());
            }

            // Go back to the select() and wait for data on the connection
            _acceptPending = false;
            PEG_METHOD_EXIT();
            return;
        }
    }

    // -- Append all data waiting on socket to incoming buffer:

    String httpStatus;
    Sint32 bytesRead = 0;
    Boolean incompleteSecureReadOccurred = false;

    for (;;)
    {
        // save one for null
        char buffer[httpTcpBufferSize+1];
        buffer[sizeof(buffer)-1] = 0;

        Sint32 n = _socket->read(buffer, sizeof(buffer)-1);

        if (n <= 0)
        {
            if (_socket->isSecure())
            {
                // It is possible that SSL_read was not able to
                // read the entire SSL record.  This could happen
                // if the record was send in multiple packets
                // over the network and only some of the packets
                // are available.  Since SSL requires the entire
                // record to successfully decrypt, the SSL_read
                // operation will return "0 bytes" read.
                // Once all the bytes of the SSL record have been read,
                // SSL_read will return the entire record.
                // The following test was added to allow
                // handleReadEvent to distinguish between a
                // disconnect and partial read of an SSL record.
                //
                incompleteSecureReadOccurred = _socket->incompleteReadOccurred(n);
            }
            break;
        }

        try
        {
            buffer[n] = 0;
            // important: always keep message buffer null terminated for easy
            // string parsing!
            Uint32 size = _incomingBuffer.size() + n;
            _incomingBuffer.reserveCapacity(size + 1);
            _incomingBuffer.append(buffer, n);
            // put a null on it. This is safe sice we have reserved an extra byte
            char *data = (char *)_incomingBuffer.getData();
            data[size] = 0;
        }

        catch(...)
        {
            static const char detailP[] =
                "Unable to append the request to the input buffer";
            httpStatus =
                HTTP_STATUS_REQUEST_TOO_LARGE + httpDetailDelimiter + detailP;
            _handleReadEventFailure(httpStatus);
            PEG_METHOD_EXIT();
            return;
        }

        bytesRead += n;
#if defined (PEGASUS_OS_VMS)
        if (n < sizeof(buffer))
        {
            //
            // Read is smaller than the buffer size.
            // No more to read, continue.
            //
            break;
        }
#endif
    }

    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
        "Total bytesRead = %d; Bytes read this iteration = %d",
        _incomingBuffer.size(), bytesRead);

    try
    {
        if (_contentOffset == -1)
            _getContentLengthAndContentOffset();
        _handleReadEventTransferEncoding();
    }
    catch(Exception &e)
    {
        httpStatus = e.getMessage();
    }

    if (httpStatus.size() > 0)
    {
        _handleReadEventFailure(httpStatus);
        PEG_METHOD_EXIT();
        return;
    }

    // -- See if the end of the message was reached (some peers signal end of
    // -- the message by closing the connection; others use the content length
    // -- HTTP header and then there are those messages which have no bodies
    // -- at all).

    if ((bytesRead == 0 && !incompleteSecureReadOccurred) ||
        (_contentLength != -1 && _contentOffset != -1 &&
        (Sint32(_incomingBuffer.size()) >= _contentLength + _contentOffset)))
    {
        HTTPMessage* message = new HTTPMessage(_incomingBuffer, getQueueId());
        message->authInfo = _authInfo.get();

        // add any content languages
        message->contentLanguages = contentLanguages;

        //
        // increment request count
        //
        if (bytesRead > 0)
        {
            _requestCount++;
            _connectionRequestCount++;
        }
        Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "_requestCount = %d", _requestCount.value());
        message->dest = _outputMessageQueue->getQueueId();
//        SendForget(message);

        //
        // Set the entry status to BUSY.
        //
        if (_isClient() == false && !_connectionClosePending)
        {
            Tracer::trace (TRC_HTTP, Tracer::LEVEL2,
                "Now setting state to %d", _MonitorEntry::BUSY);
            _monitor->setState (_entry_index, _MonitorEntry::BUSY);
            _monitor->tickle();
        }
        _outputMessageQueue->enqueue(message);
        _clearIncoming();

        if (bytesRead == 0)
        {
            Tracer::trace(TRC_HTTP, Tracer::LEVEL3,
                "HTTPConnection::_handleReadEvent - bytesRead == 0 - Connection being closed.");
            _closeConnection();

            //
            // decrement request count
            //
            Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                "_requestCount = %d", _requestCount.value());

            PEG_METHOD_EXIT();
            return;
        }
    }
    PEG_METHOD_EXIT();
}

Uint32 HTTPConnection::getRequestCount()
{
    return(_requestCount.value());
}

Boolean HTTPConnection::run(Uint32 milliseconds)
{
    Boolean handled_events = false;
    int events = 0;
    fd_set fdread; // , fdwrite;
    struct timeval tv = { 0, 1 };
    FD_ZERO(&fdread);
    FD_SET(getSocket(), &fdread);
    events = select(FD_SETSIZE, &fdread, NULL, NULL, &tv);
#ifdef PEGASUS_OS_TYPE_WINDOWS
    if(events == SOCKET_ERROR)
#else
    if(events == -1)
#endif
    {
        return false;
    }

    if (events)
    {
        events = 0;
        if( FD_ISSET(getSocket(), &fdread))
        {
            events |= SocketMessage::READ;
            Message *msg = new SocketMessage(getSocket(), events);
            try
            {
                handleEnqueue(msg);
            }
            catch(...)
            {
                Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    "HTTPConnection::run handleEnqueue(msg) failure");
                return true;
            }
            handled_events = true;
        }
    }

    return handled_events;
}

PEGASUS_NAMESPACE_END
