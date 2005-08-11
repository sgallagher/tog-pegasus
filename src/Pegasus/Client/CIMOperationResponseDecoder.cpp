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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Sushma Fernandes, Hewlett-Packard Company
//                  (sushma_fernandes@hp.com)
//              Dave Rosckes (rosckes@us.ibm.com)
//              Seema Gupta (gseema@in.ibm.com) for PEP135
//              Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase1
//              Willis White, IBM (whiwill@us.ibm.com)
//              John Alex, IBM (johnalex@us.ibm.com) - Bug#2290
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Exception.h>
#include "CIMOperationResponseDecoder.h"


// l10n
#include <Pegasus/Common/MessageLoader.h> 



PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMOperationResponseDecoder::CIMOperationResponseDecoder(
    MessageQueue* outputQueue,
    MessageQueue* encoderQueue,
    ClientAuthenticator* authenticator,
    Uint32 showInput)
    :
    MessageQueue(PEGASUS_QUEUENAME_OPRESPDECODER),
    _outputQueue(outputQueue),
    _encoderQueue(encoderQueue),
    _authenticator(authenticator),
    _showInput(showInput)
{

}

CIMOperationResponseDecoder::~CIMOperationResponseDecoder()
{

}

void  CIMOperationResponseDecoder::setEncoderQueue(MessageQueue* encoderQueue)
{
    _encoderQueue = encoderQueue;
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
            _handleHTTPMessage(httpMessage);
            break;
        }

        default:
            PEGASUS_ASSERT(0);
            break;
    }

    delete message;
}

void CIMOperationResponseDecoder::_handleHTTPMessage(HTTPMessage* httpMessage)
{
    //
    // Parse the HTTP message:
    //

    ClientPerfDataStore* dataStore = ClientPerfDataStore::Instance();
    TimeValue networkEndTime = TimeValue::getCurrentTime();

    String  connectClose;
    String  startLine;
    Array<HTTPHeader> headers;
    char*   content;
    Uint32  contentLength;
    Boolean cimReconnect=false;

    if (httpMessage->message.size() == 0)
    {
        // l10n

        // CIMClientMalformedHTTPException* malformedHTTPException =
        //   new CIMClientMalformedHTTPException("Empty HTTP response message.");

        MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.EMPTY_RESPONSE", "Empty HTTP response message.");
        String mlString(MessageLoader::getMessage(mlParms));

        CIMClientMalformedHTTPException* malformedHTTPException =
            new CIMClientMalformedHTTPException(mlString);

        ClientExceptionMessage * response =
            new ClientExceptionMessage(malformedHTTPException);

        _outputQueue->enqueue(response);
        return;
    }


    httpMessage->parse(startLine, headers, contentLength);

    //
    // Check for Connection: Close 
    //
    if (HTTPMessage::lookupHeader(headers, "Connection", connectClose, false))
    {
        if (String::equalNoCase(connectClose, "Close"))
        {
            //reconnect and then resend next request.
            cimReconnect=true;
        }
    }
    //
    // Get the status line info
    //
    String httpVersion;
    Uint32 statusCode;
    String reasonPhrase;

    Boolean parsableMessage = HTTPMessage::parseStatusLine(
        startLine, httpVersion, statusCode, reasonPhrase);
    if (!parsableMessage)
    {
        // l10n

        // CIMClientMalformedHTTPException* malformedHTTPException = new
        //   CIMClientMalformedHTTPException("Malformed HTTP response message.");

        MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.MALFORMED_RESPONSE", "Malformed HTTP response message.");
        String mlString(MessageLoader::getMessage(mlParms));

        CIMClientMalformedHTTPException* malformedHTTPException =
            new CIMClientMalformedHTTPException(mlString);

        ClientExceptionMessage * response =
            new ClientExceptionMessage(malformedHTTPException);
      
        response->setCloseConnect(cimReconnect);

        _outputQueue->enqueue(response);
        return;
    }

#ifdef PEGASUS_CLIENT_TRACE_ENABLE
    if (_showInput & 1)
    {
        cout << "CIMOperatonResponseDecoder";
        httpMessage->printAll(cout);
    }
    if (_showInput & 2)
    {
        Uint32 size = httpMessage->message.size();
        char* tmpBuf = new char[size+1];

        strncpy( tmpBuf, httpMessage->message.getData(), size );
        tmpBuf[size] = '\0';
        Logger::put(Logger::STANDARD_LOG,
            "CIMCLIENT",
            Logger::TRACE,
            "CIMOperationRequestDecoder::Response, XML content: $1",
            tmpBuf);
        delete []tmpBuf;
    }
#endif

    try
    {
        if (_authenticator->checkResponseHeaderForChallenge(headers))
        {
            //
            // Get the original request, put that in the encoder's queue for
            // re-sending with authentication challenge response.
            //
            Message* reqMessage = _authenticator->releaseRequestMessage();

            if (cimReconnect == true)
            {
                reqMessage->setCloseConnect(cimReconnect);
                _outputQueue->enqueue(reqMessage);
            } 
            else 
            {
                _encoderQueue->enqueue(reqMessage);
            }

            return;
        }
        else
        {
            //
            // Received a valid/error response from the server.
            // We do not need the original request message anymore, hence delete
            // the request message by getting the handle from the ClientAuthenticator.
            //
            Message* reqMessage = _authenticator->releaseRequestMessage();
            delete reqMessage;
        }
    }
    catch(InvalidAuthHeader& e)
    {
        CIMClientMalformedHTTPException* malformedHTTPException =
            new CIMClientMalformedHTTPException(e.getMessage());
        ClientExceptionMessage * response =
            new ClientExceptionMessage(malformedHTTPException);

        response->setCloseConnect(cimReconnect);
        _outputQueue->enqueue(response);
        return;
    }

    // We have the response.  If authentication failed, we will generate a
    // CIMClientHTTPErrorException below with the "401 Unauthorized" status
    // in the (re-challenge) response.

    //
    // Check for a success (200 OK) response
    //
    if (statusCode != HTTP_STATUSCODE_OK)
    {
        String cimError;
        String pegasusError;

        HTTPMessage::lookupHeader(headers, "CIMError", cimError, true);
        HTTPMessage::lookupHeader(headers, PEGASUS_HTTPHEADERTAG_ERRORDETAIL, pegasusError);
        try
        {
            pegasusError = XmlReader::decodeURICharacters(pegasusError);
        }
        catch (ParseError&)
        {
            // Ignore this exception.  We're more interested in having the
            // message in encoded form than knowing that the format is invalid.
        }

        CIMClientHTTPErrorException* httpError =
            new CIMClientHTTPErrorException(statusCode, reasonPhrase,
                                            cimError, pegasusError);
        ClientExceptionMessage * response =
            new ClientExceptionMessage(httpError);

        response->setCloseConnect(cimReconnect);
        _outputQueue->enqueue(response);
        return;
    }

    //
    // Search for "CIMOperation" header:
    //
    String cimOperation;

    if (!HTTPMessage::lookupHeader(
        headers, "CIMOperation", cimOperation, true))
    {
      // l10n

      // CIMClientMalformedHTTPException* malformedHTTPException = new
      //   CIMClientMalformedHTTPException("Missing CIMOperation HTTP header");

      MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.MISSING_CIMOP_HEADER", "Missing CIMOperation HTTP header");
      String mlString(MessageLoader::getMessage(mlParms));

      CIMClientMalformedHTTPException* malformedHTTPException = new
        CIMClientMalformedHTTPException(mlString);

        ClientExceptionMessage * response =
            new ClientExceptionMessage(malformedHTTPException);

        response->setCloseConnect(cimReconnect);

        _outputQueue->enqueue(response);
        return;
    }


		// l10n start
// l10n end
    //
    // Search for "Content-Type" header:
    //

    // BUG 572, Use of Content-Type header and change error msg.
    // If header exists, test type.  If not, ignore. We will find
    // content type errors in text analysis.
    // content-type header  value format:
    //              type "/" subtype *( ";" parameter )
    // ex. text/xml;Charset="utf8"
    String cimContentType;

    if (HTTPMessage::lookupHeader(
            headers, "Content-Type", cimContentType, true))
    {
        Uint32 len;
        String contentTypeValue;
        if ((len = cimContentType.find(';')) != PEG_NOT_FOUND)
            contentTypeValue = cimContentType.subString(0,len);
        else
            contentTypeValue = cimContentType;

        if (!String::equalNoCase(contentTypeValue, "text/xml") &&
            !String::equalNoCase(contentTypeValue, "application/xml"))
        {
            CIMClientMalformedHTTPException* malformedHTTPException = new
                CIMClientMalformedHTTPException
                    ("Bad Content-Type HTTP header; " + contentTypeValue);
            ClientExceptionMessage * response =
                new ClientExceptionMessage(malformedHTTPException);

            response->setCloseConnect(cimReconnect);

            _outputQueue->enqueue(response);
            return;
        }
    }
    // comment out the error rejection code if the content-type header does
    //    not exist
#ifdef PEGASUS_REJECT_ON_MISSING_CONTENTTYPE_HEADER
    else
    {
        CIMClientMalformedHTTPException* malformedHTTPException = new
            CIMClientMalformedHTTPException
                ("Missing Content-Type HTTP header; ");
        ClientExceptionMessage * response =
            new ClientExceptionMessage(malformedHTTPException);

        response->setCloseConnect(cimReconnect);

        _outputQueue->enqueue(response);
        return;
    }
#endif

    // look for any cim status codes. The HTTPConnection level would have
    // added them here.

    String cimStatusCodeValue;
    Boolean found = HTTPMessage::lookupHeader(headers, "CIMStatusCode",
        cimStatusCodeValue, true);
    CIMStatusCode cimStatusCodeNumber = CIM_ERR_SUCCESS;

    if (found == true &&
        (cimStatusCodeNumber = (CIMStatusCode)
         atoi(cimStatusCodeValue.getCString())) != CIM_ERR_SUCCESS)
    {
        String cimStatusCodeDescription;
        found = HTTPMessage::lookupHeader(headers, "CIMStatusCodeDescription",
            cimStatusCodeDescription, true);
        if (found == true && cimStatusCodeDescription.size() > 0)
        {
            try
            {
                cimStatusCodeDescription =
                    XmlReader::decodeURICharacters(cimStatusCodeDescription);
            }
            catch (ParseError&)
            {
            }
        } // if there is a description with the code

        CIMException* cimStatusException =
            new CIMException(cimStatusCodeNumber,cimStatusCodeDescription);
        cimStatusException->setContentLanguages(httpMessage->contentLanguages);
        ClientExceptionMessage * response =
            new ClientExceptionMessage(cimStatusException);
  
        response->setCloseConnect(cimReconnect);

        _outputQueue->enqueue(response);
        return;
    }
    String serverTime;
    if(HTTPMessage::lookupHeader(headers, "WBEMServerResponseTime", serverTime, true))
    {
        //cout << "serverTime was set it is " << serverTime.getCString() << endl;
        Uint32 sTime = (Uint32) atol(serverTime.getCString());
        dataStore->setServerTime(sTime);
    }


    //
    // Zero-terminate the message:
    //
    httpMessage->message.append('\0');

    // Calculate the beginning of the content from the message size and
    // the content length.  Subtract 1 to take into account the null
    // character we just added to the end of the message.

    content = (char *) httpMessage->message.getData() +
        httpMessage->message.size() - contentLength - 1;

    //
    // If it is a method response, then dispatch it to be handled:
    //

    if (!String::equalNoCase(cimOperation, "MethodResponse"))
    {
        // l10n

        // CIMClientMalformedHTTPException* malformedHTTPException =
        //   new CIMClientMalformedHTTPException(
        //        String("Received CIMOperation HTTP header value \"") +
        //        cimOperation + "\", expected \"MethodResponse\"");

        MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.EXPECTED_METHODRESPONSE", "Received CIMOperation HTTP header value \"$1\", expected \"MethodResponse\"", cimOperation);
        String mlString(MessageLoader::getMessage(mlParms));

        CIMClientMalformedHTTPException* malformedHTTPException =
            new CIMClientMalformedHTTPException(mlString);

        ClientExceptionMessage * response =
            new ClientExceptionMessage(malformedHTTPException);

        response->setCloseConnect(cimReconnect);

        _outputQueue->enqueue(response);
        return;
    }

    dataStore->setResponseSize(contentLength);
    dataStore->setEndNetworkTime(networkEndTime);
    //dataStore->print();
    _handleMethodResponse(content, httpMessage->contentLanguages,cimReconnect);  // l10n
}

void CIMOperationResponseDecoder::_handleMethodResponse(
    char* content,
    const ContentLanguages& contentLanguages,
    Boolean cimReconnect) //l10n
{
    Message* response = 0;

    //
    // Create and initialize XML parser:
    //

    XmlParser parser((char*)content);
    XmlEntry entry;

    try
    {
        //
        // Process <?xml ... >
        //

        const char* xmlVersion = 0;
        const char* xmlEncoding = 0;

        XmlReader::getXmlDeclaration(parser, xmlVersion, xmlEncoding);

        //
        // Process <CIM ... >
        //

        const char* cimVersion = 0;
        const char* dtdVersion = 0;

        // ATTN-RK-P3-20020416: Need to validate these versions?
        XmlReader::getCimStartTag(parser, cimVersion, dtdVersion);

        //
        // Expect <MESSAGE ... >
        //

        String messageId;
        String protocolVersion;

        if (!XmlReader::getMessageStartTag(parser, messageId, protocolVersion))
        {
            // l10n

            // throw XmlValidationError(
            // parser.getLine(), "expected MESSAGE element");

            MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.EXPECTED_MESSAGE_ELEMENT",
                "expected MESSAGE element");

            throw XmlValidationError(parser.getLine(), mlParms);
        }


        //
        // This code for checking the protocol version was taken from the server code.
        //
        Boolean protocolVersionAccepted = false;

        if ((protocolVersion.size() >= 3) &&
           (protocolVersion[0] == '1') &&
           (protocolVersion[1] == '.'))
        {
           // Verify that all characters after the '.' are digits
           Uint32 index = 2;
           while ((index < protocolVersion.size()) &&
                  (protocolVersion[index] >= '0') &&
                  (protocolVersion[index] <= '9'))
           {
              index++;
           }

           if (index == protocolVersion.size())
           {
              protocolVersionAccepted = true;
           }
        }

        if (!protocolVersionAccepted)
        {
            MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.UNSUPPORTED_PROTOCOL", "Received unsupported protocol version \"$0\", expected \"$1\"", protocolVersion, "1.[0-9]+");
            String mlString(MessageLoader::getMessage(mlParms));

            CIMClientResponseException* responseException =
                new CIMClientResponseException(mlString);

            ClientExceptionMessage * response =
                new ClientExceptionMessage(responseException);

            response->setCloseConnect(cimReconnect);

            _outputQueue->enqueue(response);
            return;
        }

        //
        // Expect <SIMPLERSP ... >
        //

        XmlReader::expectStartTag(parser, entry, "SIMPLERSP");

        //
        // Expect <IMETHODRESPONSE ... >
        //

        const char* iMethodResponseName = 0;
        Boolean isEmptyTag = false;

        if (XmlReader::getIMethodResponseStartTag(
                parser, iMethodResponseName, isEmptyTag))
        {
            //
            // Dispatch the method:
            //

            if (System::strcasecmp(iMethodResponseName, "GetClass") == 0)
                response = _decodeGetClassResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "GetInstance") == 0)
                response = _decodeGetInstanceResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "EnumerateClassNames") == 0)
                response = _decodeEnumerateClassNamesResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "References") == 0)
                response = _decodeReferencesResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "ReferenceNames") == 0)
                response = _decodeReferenceNamesResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "AssociatorNames") == 0)
                response = _decodeAssociatorNamesResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "Associators") == 0)
                response = _decodeAssociatorsResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "CreateInstance") == 0)
                response = _decodeCreateInstanceResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName,"EnumerateInstanceNames") == 0)
                response = _decodeEnumerateInstanceNamesResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName,"EnumerateInstances") == 0)
                response = _decodeEnumerateInstancesResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "GetProperty") == 0)
                response = _decodeGetPropertyResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "SetProperty") == 0)
                response = _decodeSetPropertyResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "DeleteQualifier") == 0)
                response = _decodeDeleteQualifierResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "GetQualifier") == 0)
                response = _decodeGetQualifierResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "SetQualifier") == 0)
                response = _decodeSetQualifierResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "EnumerateQualifiers") == 0)
                response = _decodeEnumerateQualifiersResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "EnumerateClasses") == 0)
                response = _decodeEnumerateClassesResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "CreateClass") == 0)
                response = _decodeCreateClassResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "ModifyClass") == 0)
                response = _decodeModifyClassResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "ModifyInstance") == 0)
                response = _decodeModifyInstanceResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "DeleteClass") == 0)
                response = _decodeDeleteClassResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "DeleteInstance") == 0)
                response = _decodeDeleteInstanceResponse(
                    parser, messageId, isEmptyTag);
            else if (System::strcasecmp(iMethodResponseName, "ExecQuery") == 0)
                response = _decodeExecQueryResponse(
                    parser, messageId, isEmptyTag);
            else
            {
                // l10n

                // Unrecognized IMethodResponse name attribute
                // throw XmlValidationError(parser.getLine(),
                //   String("Unrecognized IMethodResponse name \"") +
                //        iMethodResponseName + "\"");

                MessageLoaderParms mlParms(
                    "Client.CIMOperationResponseDecoder.UNRECOGNIZED_NAME",
                    "Unrecognized IMethodResponse name \"$0\"",
                    iMethodResponseName);
                throw XmlValidationError(parser.getLine(), mlParms);
            }

            //
            // Handle end tag:
            //

            if (!isEmptyTag)
            {
                XmlReader::expectEndTag(parser, "IMETHODRESPONSE");
            }
        }
        else if (XmlReader::getMethodResponseStartTag(
                     parser, iMethodResponseName, isEmptyTag))
        {
            response = _decodeInvokeMethodResponse(
                parser, messageId, iMethodResponseName, isEmptyTag);


            //
            // Handle end tag:
            //
            if (!isEmptyTag)
            {
                XmlReader::expectEndTag(parser, "METHODRESPONSE");
            }
        }
        else
        {
            // l10n

            // throw XmlValidationError(parser.getLine(),
            //   "expected METHODRESPONSE or IMETHODRESPONSE element");

            MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.EXPECTED_METHODRESPONSE_OR_IMETHODRESPONSE_ELEMENT",
                "expected METHODRESPONSE or IMETHODRESPONSE element");

            throw XmlValidationError(parser.getLine(), mlParms);
        }

        //
        // Handle end tags:
        //
        XmlReader::expectEndTag(parser, "SIMPLERSP");
        XmlReader::expectEndTag(parser, "MESSAGE");
        XmlReader::expectEndTag(parser, "CIM");
    }
    catch (XmlException& x)
    {
        Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::TRACE,
            "CIMOperationResponseDecoder::_handleMethodResponse - XmlException has occurred. Message: $0",x.getMessage());

        if (response)
        {
//#ifdef PEGASUS_SNIA_INTEROP_TEST
//         httpMessage->printAll(cout);
//#endif

            delete response;
        }

        response = new ClientExceptionMessage(
            new CIMClientXmlException(x.getMessage()));
    }
    catch (Exception& x)
    {
        // Might get MalformedObjectNameException, InvalidNameException, etc.

        if (response)
        {
            delete response;
        }

        response = new ClientExceptionMessage(
            new CIMClientResponseException(x.getMessage()));
    }

//l10n start
// l10n TODO - might want to move A-L and C-L to Message
// to make this more maintainable
    // Add the language header to the request
    CIMMessage * cimmsg = dynamic_cast<CIMMessage *>(response);
    if (cimmsg != NULL)
    {
        cimmsg->operationContext.set(ContentLanguageListContainer(contentLanguages));
    }
    else
    {
        ;    // l10n TODO - error back to client here
    }
// l10n end

    response->setCloseConnect(cimReconnect);


    _outputQueue->enqueue(response);
}

CIMCreateClassResponseMessage* CIMOperationResponseDecoder::_decodeCreateClassResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMCreateClassResponseMessage(
                messageId,
                cimException,
                QueueIdStack()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMCreateClassResponseMessage(
        messageId,
        cimException,
        QueueIdStack()));
}

CIMGetClassResponseMessage* CIMOperationResponseDecoder::_decodeGetClassResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (isEmptyImethodresponseTag)
    {
        MessageLoaderParms mlParms("Common.XmlReader.EXPECTED_OPEN",
            "Expected open of $0 element", "IMETHODRESPONSE");
        throw XmlValidationError(parser.getLine(), mlParms);
    }
    else if (XmlReader::getErrorElement(parser, cimException))
    {
        return(new CIMGetClassResponseMessage(
            messageId,
            cimException,
            QueueIdStack(),
            CIMClass()));
    }
    else if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
    {
        CIMClass cimClass;

        if ((entry.type == XmlEntry::EMPTY_TAG) ||
            !XmlReader::getClassElement(parser, cimClass))
        {
            // l10n

            // throw XmlValidationError(parser.getLine(),"expected CLASS element");

            MessageLoaderParms mlParms(
                "Client.CIMOperationResponseDecoder.EXPECTED_CLASS_ELEMENT",
                "expected CLASS element");
            throw XmlValidationError(parser.getLine(), mlParms);
        }

        XmlReader::expectEndTag(parser, "IRETURNVALUE");

        return(new CIMGetClassResponseMessage(
            messageId,
            cimException,
            QueueIdStack(),
            cimClass));
    }
    else
    {
        // l10n

        // throw XmlValidationError(parser.getLine(),
        //   "expected ERROR or IRETURNVALUE element");

        MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.EXPECTED_ERROR_OR_IRETURNVALUE_ELEMENT",
            "expected ERROR or IRETURNVALUE element");

        throw XmlValidationError(parser.getLine(), mlParms);
    }
}

CIMModifyClassResponseMessage* CIMOperationResponseDecoder::_decodeModifyClassResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMModifyClassResponseMessage(
                messageId,
                cimException,
                QueueIdStack()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMModifyClassResponseMessage(
        messageId,
        cimException,
        QueueIdStack()));
}

CIMEnumerateClassNamesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateClassNamesResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;
    Array<CIMName> classNames;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMEnumerateClassNamesResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                Array<CIMName>()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                CIMName className;

                while (XmlReader::getClassNameElement(parser, className, false))
                    classNames.append(className);

                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMEnumerateClassNamesResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        classNames));
}

CIMEnumerateClassesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateClassesResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;
    Array<CIMClass> cimClasses;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMEnumerateClassesResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                Array<CIMClass>()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                CIMClass cimClass;

                while (XmlReader::getClassElement(parser, cimClass))
                    cimClasses.append(cimClass);

                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMEnumerateClassesResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        cimClasses));
}

CIMDeleteClassResponseMessage* CIMOperationResponseDecoder::_decodeDeleteClassResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMDeleteClassResponseMessage(
                messageId,
                cimException,
                QueueIdStack()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMDeleteClassResponseMessage(
        messageId,
        cimException,
        QueueIdStack()));
}

CIMCreateInstanceResponseMessage* CIMOperationResponseDecoder::_decodeCreateInstanceResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (isEmptyImethodresponseTag)
    {
        MessageLoaderParms mlParms("Common.XmlReader.EXPECTED_OPEN",
            "Expected open of $0 element", "IMETHODRESPONSE");
        throw XmlValidationError(parser.getLine(), mlParms);
    }
    else if (XmlReader::getErrorElement(parser, cimException))
    {
        return(new CIMCreateInstanceResponseMessage(
            messageId,
            cimException,
            QueueIdStack(),
            CIMObjectPath()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
        CIMObjectPath instanceName;
        XmlReader::getInstanceNameElement(parser, instanceName);

        XmlReader::expectEndTag(parser, "IRETURNVALUE");

        return(new CIMCreateInstanceResponseMessage(
            messageId,
            cimException,
            QueueIdStack(),
            instanceName));
    }
    else
    {
        // l10n

        // throw XmlValidationError(parser.getLine(),
        //   "expected ERROR or IRETURNVALUE element");

        MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.EXPECTED_ERROR_OR_IRETURNVALUE_ELEMENT",
            "expected ERROR or IRETURNVALUE element");

        throw XmlValidationError(parser.getLine(), mlParms);
    }
}

CIMGetInstanceResponseMessage* CIMOperationResponseDecoder::_decodeGetInstanceResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (isEmptyImethodresponseTag)
    {
        MessageLoaderParms mlParms("Common.XmlReader.EXPECTED_OPEN",
            "Expected open of $0 element", "IMETHODRESPONSE");
        throw XmlValidationError(parser.getLine(), mlParms);
    }
    else if (XmlReader::getErrorElement(parser, cimException))
    {
        return(new CIMGetInstanceResponseMessage(
            messageId,
            cimException,
            QueueIdStack(),
            CIMInstance()));
    }
    else if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
    {
        CIMInstance cimInstance;

        if ((entry.type == XmlEntry::EMPTY_TAG) ||
            !XmlReader::getInstanceElement(parser, cimInstance))
        {
            // l10n

            // throw XmlValidationError(
            // parser.getLine(), "expected INSTANCE element");

            MessageLoaderParms mlParms(
                "Client.CIMOperationResponseDecoder.EXPECTED_INSTANCE_ELEMENT",
                "expected INSTANCE element");

            throw XmlValidationError(parser.getLine(), mlParms);
        }

        XmlReader::expectEndTag(parser, "IRETURNVALUE");

        return(new CIMGetInstanceResponseMessage(
            messageId,
            cimException,
            QueueIdStack(),
            cimInstance));
    }
    else
    {
        // l10n

        // throw XmlValidationError(parser.getLine(),
        //   "expected ERROR or IRETURNVALUE element");

        MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.EXPECTED_ERROR_OR_IRETURNVALUE_ELEMENT",
            "expected ERROR or IRETURNVALUE element");

        throw XmlValidationError(parser.getLine(), mlParms);
    }
}

CIMModifyInstanceResponseMessage* CIMOperationResponseDecoder::_decodeModifyInstanceResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMModifyInstanceResponseMessage(
                messageId,
                cimException,
                QueueIdStack()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMModifyInstanceResponseMessage(
        messageId,
        cimException,
        QueueIdStack()));
}

CIMEnumerateInstanceNamesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateInstanceNamesResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;
    Array<CIMObjectPath> instanceNames;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMEnumerateInstanceNamesResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                Array<CIMObjectPath>()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                String className;
                Array<CIMKeyBinding> keyBindings;

                while (XmlReader::getInstanceNameElement(
                    parser, className, keyBindings))
                {
                    CIMObjectPath r(
                        String::EMPTY,
                        CIMNamespaceName(),
                        className,
                        keyBindings);
                    instanceNames.append(r);
                }

                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMEnumerateInstanceNamesResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        instanceNames));
}

CIMEnumerateInstancesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateInstancesResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;
    Array<CIMInstance> namedInstances;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMEnumerateInstancesResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                Array<CIMInstance>()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                CIMInstance namedInstance;

                while (XmlReader::getNamedInstanceElement(
                           parser, namedInstance))
                {
                    namedInstances.append(namedInstance);
                }

                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMEnumerateInstancesResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        namedInstances));
}

CIMDeleteInstanceResponseMessage* CIMOperationResponseDecoder::_decodeDeleteInstanceResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMDeleteInstanceResponseMessage(
                messageId,
                cimException,
                QueueIdStack()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMDeleteInstanceResponseMessage(
        messageId,
        cimException,
        QueueIdStack()));
}

CIMGetPropertyResponseMessage* CIMOperationResponseDecoder::_decodeGetPropertyResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;
    CIMValue cimValue(CIMTYPE_STRING, false);

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMGetPropertyResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                CIMValue()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                if (!XmlReader::getPropertyValue(parser, cimValue))
                {
                    // No value given; just return a null String value
                }

                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
        else
        {
            // No value given; just return a null String value
        }
    }

    return(new CIMGetPropertyResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        cimValue));
}

CIMSetPropertyResponseMessage* CIMOperationResponseDecoder::_decodeSetPropertyResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMSetPropertyResponseMessage(
                messageId,
                cimException,
                QueueIdStack()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMSetPropertyResponseMessage(
        messageId,
        cimException,
        QueueIdStack()));
}

CIMSetQualifierResponseMessage* CIMOperationResponseDecoder::_decodeSetQualifierResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMSetQualifierResponseMessage(
                messageId,
                cimException,
                QueueIdStack()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMSetQualifierResponseMessage(
        messageId,
        cimException,
        QueueIdStack()));
}

CIMGetQualifierResponseMessage* CIMOperationResponseDecoder::_decodeGetQualifierResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (isEmptyImethodresponseTag)
    {
        MessageLoaderParms mlParms("Common.XmlReader.EXPECTED_OPEN",
            "Expected open of $0 element", "IMETHODRESPONSE");
        throw XmlValidationError(parser.getLine(), mlParms);
    }
    else if (XmlReader::getErrorElement(parser, cimException))
    {
        return(new CIMGetQualifierResponseMessage(
            messageId,
            cimException,
            QueueIdStack(),
            CIMQualifierDecl()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
        CIMQualifierDecl qualifierDecl;
        XmlReader::getQualifierDeclElement(parser, qualifierDecl);

        XmlReader::expectEndTag(parser, "IRETURNVALUE");

        return(new CIMGetQualifierResponseMessage(
            messageId,
            cimException,
            QueueIdStack(),
            qualifierDecl));
    }
    else
    {
        // l10n

        // throw XmlValidationError(parser.getLine(),
        //   "expected ERROR or IRETURNVALUE element");

        MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.EXPECTED_ERROR_OR_IRETURNVALUE_ELEMENT",
        "expected ERROR or IRETURNVALUE element");

        throw XmlValidationError(parser.getLine(), mlParms);
    }
}

CIMEnumerateQualifiersResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateQualifiersResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;
    Array<CIMQualifierDecl> qualifierDecls;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMEnumerateQualifiersResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                Array<CIMQualifierDecl>()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                CIMQualifierDecl qualifierDecl;

                while (XmlReader::getQualifierDeclElement(
                           parser, qualifierDecl))
                {
                    qualifierDecls.append(qualifierDecl);
                }

                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMEnumerateQualifiersResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        qualifierDecls));
}

CIMDeleteQualifierResponseMessage* CIMOperationResponseDecoder::_decodeDeleteQualifierResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMDeleteQualifierResponseMessage(
                messageId,
                cimException,
                QueueIdStack()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMDeleteQualifierResponseMessage(
        messageId,
        cimException,
        QueueIdStack()));
}

CIMReferenceNamesResponseMessage* CIMOperationResponseDecoder::_decodeReferenceNamesResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;
    Array<CIMObjectPath> objectPaths;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMReferenceNamesResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                Array<CIMObjectPath>()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                CIMObjectPath objectPath;

                while (XmlReader::getObjectPathElement(parser, objectPath))
                    objectPaths.append(objectPath);

                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMReferenceNamesResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        objectPaths));
}

CIMReferencesResponseMessage* CIMOperationResponseDecoder::_decodeReferencesResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;
    Array<CIMObject> objectWithPathArray;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMReferencesResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                Array<CIMObject>()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                CIMObject objectWithPath;

                while (XmlReader::getValueObjectWithPathElement(
                           parser, objectWithPath))
                {
                    objectWithPathArray.append(objectWithPath);
                }

                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMReferencesResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        objectWithPathArray));
}

CIMAssociatorNamesResponseMessage* CIMOperationResponseDecoder::_decodeAssociatorNamesResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;
    Array<CIMObjectPath> objectPaths;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMAssociatorNamesResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                Array<CIMObjectPath>()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                CIMObjectPath objectPath;

                while (XmlReader::getObjectPathElement(parser, objectPath))
                    objectPaths.append(objectPath);

                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMAssociatorNamesResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        objectPaths));
}

CIMAssociatorsResponseMessage* CIMOperationResponseDecoder::_decodeAssociatorsResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;
    Array<CIMObject> objectWithPathArray;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMAssociatorsResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                Array<CIMObject>()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                CIMObject objectWithPath;

                while (XmlReader::getValueObjectWithPathElement(
                           parser, objectWithPath))
                {
                    objectWithPathArray.append(objectWithPath);
                }

                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMAssociatorsResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        objectWithPathArray));
}

CIMExecQueryResponseMessage* CIMOperationResponseDecoder::_decodeExecQueryResponse(
    XmlParser& parser,
    const String& messageId,
    Boolean isEmptyImethodresponseTag)
{
    XmlEntry entry;
    CIMException cimException;
    Array<CIMObject> objectWithPathArray;

    if (!isEmptyImethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMExecQueryResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                Array<CIMObject>()));
        }

        if (XmlReader::testStartTagOrEmptyTag(parser, entry, "IRETURNVALUE"))
        {
            if (entry.type != XmlEntry::EMPTY_TAG)
            {
                XmlReader::getObjectArray(parser, objectWithPathArray);

                XmlReader::expectEndTag(parser, "IRETURNVALUE");
            }
        }
    }

    return(new CIMExecQueryResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        objectWithPathArray));
}

CIMInvokeMethodResponseMessage* CIMOperationResponseDecoder::_decodeInvokeMethodResponse(
    XmlParser& parser,
    const String& messageId,
    const String& methodName,
    Boolean isEmptyMethodresponseTag)
{
    CIMException cimException;

    CIMParamValue paramValue;
    Array<CIMParamValue> outParameters;
    CIMValue returnValue;

    if (!isEmptyMethodresponseTag)
    {
        if (XmlReader::getErrorElement(parser, cimException))
        {
            return(new CIMInvokeMethodResponseMessage(
                messageId,
                cimException,
                QueueIdStack(),
                returnValue,
                outParameters,
                methodName));
        }

        Boolean isReturnValue = false;
        Boolean isParamValue = false;
        Boolean gotReturnValue = false;

        while ((isReturnValue =
                    XmlReader::getReturnValueElement(parser, returnValue)) ||
               (isParamValue =
                    XmlReader::getParamValueElement(parser, paramValue)))
        {
            if (isReturnValue)
            {
                if (gotReturnValue)
                {
                    // l10n

                    // throw XmlValidationError(parser.getLine(),
                    //   "unexpected RETURNVALUE element");

                    MessageLoaderParms mlParms("Client.CIMOperationResponseDecoder.EXPECTED_RETURNVALUE_ELEMENT",
                        "unexpected RETURNVALUE element");

                    throw XmlValidationError(parser.getLine(), mlParms);
                }
                gotReturnValue = true;
            }
            else    // isParamValue == true
            {
                outParameters.append(paramValue);
            }

            isReturnValue = false;
            isParamValue = false;
        }
    }

    return(new CIMInvokeMethodResponseMessage(
        messageId,
        cimException,
        QueueIdStack(),
        returnValue,
        outParameters,
        methodName));
}

PEGASUS_NAMESPACE_END
