//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cctype>
#include <cstdio>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/ContentLanguages.h>  // l10n
#include "CIMExportResponseEncoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportResponseEncoder::CIMExportResponseEncoder()
   : Base(PEGASUS_QUEUENAME_EXPORTRESPENCODER)
{

}

CIMExportResponseEncoder::~CIMExportResponseEncoder()
{

}

void CIMExportResponseEncoder::sendResponse(
   Uint32 queueId, 
   Array<Sint8>& message)
{
   MessageQueue* queue = MessageQueue::lookup(queueId);

   if (queue)
   {
      HTTPMessage* httpMessage = new HTTPMessage(message);
      queue->enqueue(httpMessage);
   }
}

void CIMExportResponseEncoder::sendEMethodError(
   Uint32 queueId, 
   HttpMethod httpMethod,
   const String& messageId,
   const String& eMethodName,
   const CIMException& cimException) 
{
    Array<Sint8> message;
    message = XmlWriter::formatSimpleEMethodErrorRspMessage(
        eMethodName,
        messageId,
        httpMethod,
        cimException);

    sendResponse(queueId, message);
}

void CIMExportResponseEncoder::sendEMethodError(
   CIMResponseMessage* response,
   const String& cimMethodName)
{
   Uint32 queueId = response->queueIds.top();
   response->queueIds.pop();

   sendEMethodError(
      queueId,
      response->getHttpMethod(),
      response->messageId, 
      cimMethodName, 
      response->cimException);
}

void CIMExportResponseEncoder::handleEnqueue(Message *message)
{
   if (!message)
      return;

   switch (message->getType())
   {
      case CIM_EXPORT_INDICATION_RESPONSE_MESSAGE:
	 encodeExportIndicationResponse(
	    (CIMExportIndicationResponseMessage*)message);
	 break;
   }
   
   delete message;
}


void CIMExportResponseEncoder::handleEnqueue()
{
   Message* message = dequeue();
   if(message)
      handleEnqueue(message);
}

void CIMExportResponseEncoder::encodeExportIndicationResponse(
   CIMExportIndicationResponseMessage* response)
{
   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendEMethodError(response, "ExportIndication");
      return;
   }

   Array<Sint8> body;
    
// l10n
   // Note: Content-Language will not be set in the response. 
   // Export responses are sent in the default language of the
   // ExportServer.
   Array<Sint8> message = XmlWriter::formatSimpleEMethodRspMessage(
      CIMName ("ExportIndication"), response->messageId, 
      response->getHttpMethod(),
      ContentLanguages::EMPTY,
      body);

   sendResponse(response->queueIds.top(), message);
}

PEGASUS_NAMESPACE_END
