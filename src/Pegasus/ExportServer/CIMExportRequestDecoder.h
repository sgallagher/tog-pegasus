//%///-*-c++-*-/////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMExportRequestDecoder_h
#define Pegasus_CIMExportRequestDecoder_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/ExportServer/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class XmlParser;

/** This class decodes CIM operation requests and passes them down-stream.
 */
class PEGASUS_EXPORT_SERVER_LINKAGE CIMExportRequestDecoder 
   : public MessageQueueService
{
   public:

      typedef MessageQueueService Base;
  
      CIMExportRequestDecoder(
	 MessageQueueService* outputQueue,
	 Uint32 returnQueueId);

      ~CIMExportRequestDecoder();

      void sendResponse(
	 Uint32 queueId, 
	 Array<Sint8>& message);

      void sendEMethodError(
	 Uint32 queueId, 
	 const String& messageId,
	 const String& methodName,
	 const CIMException& cimException);

      void sendHttpError(
	 Uint32 queueId,
	 const String& status,
	 const String& cimError = String::EMPTY,
	 const String& messageBody = String::EMPTY);

      virtual void handleEnqueue(Message *);

      virtual void handleEnqueue();

      virtual const char* getQueueName() const;

      void handleHTTPMessage(HTTPMessage* httpMessage);

      void handleMethodRequest(
	 Uint32 queueId,
	 Sint8* content,
	 const String& requestUri,
	 const String& cimProtocolVersionInHeader,
	 const String& cimExportMethodInHeader,
	 const String& userName);

      CIMExportIndicationRequestMessage* decodeExportIndicationRequest(
	 Uint32 queueId,
	 XmlParser& parser, 
	 const String& messageId,
	 const String& nameSpace);

      /** Sets the flag to indicate whether or not the CIMServer is 
	  shutting down.
      */
      void setServerTerminating(Boolean flag);

   private:

      MessageQueue* _outputQueue;

      // Queue where responses should be enqueued.
      Uint32 _returnQueueId;

      // Flag to indicate whether or not the CIMServer is shutting down.
      Boolean _serverTerminating;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMExportRequestDecoder_h */
