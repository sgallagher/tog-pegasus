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
//
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//				Seema Gupta (gseema@in.ibm.com) for PEP135
//              Amit K Arora (amita@in.ibm.com) for PEP101
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/AcceptLanguages.h>  // l10n
#include <Pegasus/Common/ContentLanguages.h>  // l10n
#include "CIMExportRequestEncoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportRequestEncoder::CIMExportRequestEncoder(
   MessageQueue* outputQueue,
   const String& hostName,
   ClientAuthenticator* authenticator)
   : 
   MessageQueue(PEGASUS_QUEUENAME_EXPORTREQENCODER),
   _outputQueue(outputQueue),
   _hostName(hostName.getCString()),
   _authenticator(authenticator)
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportRequestEncoder::CIMExportRequestEncoder()");
    PEG_METHOD_EXIT();
}

CIMExportRequestEncoder::~CIMExportRequestEncoder()
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportRequestEncoder::~CIMExportRequestEncoder()");
     _authenticator.release();
    PEG_METHOD_EXIT();
}

void CIMExportRequestEncoder::handleEnqueue()
{
   PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportRequestEncoder::handleEnqueue()");
   Message* message = dequeue();

   if (!message)
      return;

   _authenticator->setRequestMessage(message);

   switch (message->getType())
   {
      case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
	 _encodeExportIndicationRequest((CIMExportIndicationRequestMessage*)message);
	 break;
   }

   //ATTN: Do not delete the message here.
   //
   // ClientAuthenticator needs this message for resending the request on
   // authentication challenge from the server. The message is deleted in
   // the decoder after receiving the valid response from the server.
   //
   //delete message;
   PEG_METHOD_EXIT();
}

void CIMExportRequestEncoder::_encodeExportIndicationRequest(
   CIMExportIndicationRequestMessage* message)
{
   PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "CIMExportRequestEncoder::_encodeExportIndicationRequest()");
   Array<char> params;

   XmlWriter::appendInstanceEParameter(
      params, "NewIndication", message->indicationInstance);
	
// l10n
   // Note:  Accept-Language will not be set in the request	
   // We will accept the default language of the export server.
   Array<char> buffer = XmlWriter::formatSimpleEMethodReqMessage(
      message->destinationPath.getCString(),
      _hostName,
      CIMName ("ExportIndication"), 
      message->messageId, 
      message->getHttpMethod(),
      _authenticator->buildRequestAuthHeader(),
      AcceptLanguages::EMPTY,
	 ((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
      params);

   _outputQueue->enqueue(new HTTPMessage(buffer));
   PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
