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
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              John Alex, IBM (johnalex@us.ibm.com) - Bug#2290
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMExportResponseDecoder_h
#define Pegasus_CIMExportResponseDecoder_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Client/ClientAuthenticator.h>
#include <Pegasus/Client/CIMClientException.h>
#include <Pegasus/ExportClient/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class XmlParser;

/**
    This message is sent from the response decoder to the CIMClient, indicating
    an error in issuing a CIM request.
*/
class ClientExceptionMessage : public Message
{
public:
    ClientExceptionMessage(Exception* clientException_)
        :
        Message(CLIENT_EXCEPTION_MESSAGE),
        clientException(clientException_)
    {
    }

    Exception* clientException;
};

/** This class receives HTTP messages and decodes them into CIM Operation
    Responses messages which it places on its output queue.
*/
class PEGASUS_EXPORT_CLIENT_LINKAGE CIMExportResponseDecoder :  public MessageQueue
{

   public:

      /** Constuctor.
          @param outputQueue queue to receive decoded HTTP messages.
          @param encoderQueue queue to receive CIM Operation Response messages.
          @param authenticator client authenticator.
      */
      CIMExportResponseDecoder(
         MessageQueue* outputQueue,
         MessageQueue* encoderQueue,
         ClientAuthenticator* authenticator);

      /** Destructor. */
      ~CIMExportResponseDecoder();

      void setEncoderQueue(MessageQueue* encoderQueue);

      /** This method is called when a message is enqueued on this queue. */
      virtual void handleEnqueue();

   private:

      void _handleHTTPMessage(
         HTTPMessage* message);

      void _handleMethodResponse(
         char* content,
         Boolean cimReconnect);

      CIMExportIndicationResponseMessage* _decodeExportIndicationResponse(
         XmlParser& parser,
         const String& messageId,
         Boolean isEmptyMethodresponseTag);

      AutoPtr<MessageQueue>        _outputQueue; //PEP101
      AutoPtr<MessageQueue>        _encoderQueue; //PEP101
      AutoPtr<ClientAuthenticator> _authenticator; //PEP101
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMExportResponseDecoder_h */
