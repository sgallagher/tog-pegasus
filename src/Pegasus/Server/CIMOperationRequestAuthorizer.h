//%////-*-c++-*-////////////////////////////////////////////////////////////////
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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMOperationRequestAuthorizer_h
#define Pegasus_CIMOperationRequestAuthorizer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMMessage.h>


PEGASUS_NAMESPACE_BEGIN


class PEGASUS_SERVER_LINKAGE CIMOperationRequestAuthorizer : public MessageQueueService
{
   public:
  
      typedef MessageQueueService Base;

      CIMOperationRequestAuthorizer(
	 MessageQueueService* outputQueue);
      
      ~CIMOperationRequestAuthorizer();
      
      void sendResponse(
	 Uint32 queueId,
	 Array<Sint8>& message);

      void sendIMethodError(
	 Uint32 queueId,
	 const String& messageId,
	 const String& methodName,
	 const CIMException& cimException);

      virtual void handleEnqueue(Message *);

      virtual void handleEnqueue();

      /** Sets the flag to indicate whether or not the CIMServer is
	  shutting down.
      */
      void setServerTerminating(Boolean flag);

   private:

      MessageQueueService* _outputQueue;

      // Flag to indicate whether or not the CIMServer is shutting down.
      Boolean _serverTerminating;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMOperationRequestAuthorizer_h */
