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
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HTTPAuthenticatorDelegator_h
#define Pegasus_HTTPAuthenticatorDelegator_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Security/Authentication/AuthenticationManager.h>
#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/**
   This class parses the HTTP header in the HTTPMessage passed to it and
   performs authentication based on the authentication specified in the
   configuration. It sends the challenge for unauthorized requests and
   validates the response.

*/

class PEGASUS_SERVER_LINKAGE HTTPAuthenticatorDelegator : public MessageQueueService
{
   public:

      typedef MessageQueueService Base;
  

      /** Constructor. */
      HTTPAuthenticatorDelegator(
	 Uint32 operationMessageQueueId,
	 Uint32 exportMessageQueueId);

      /** Destructor. */
      ~HTTPAuthenticatorDelegator();

      /**
          This method is overridden here to force the message to be handled
          by the same thread that enqueues it.  See Bugzilla 641 for details.
       */
      virtual void enqueue(Message* message) throw(IPCException);

      virtual void handleEnqueue(Message *);
      virtual void handleEnqueue();

      void handleHTTPMessage(HTTPMessage* httpMessage, Boolean & deleteMessage);

   private:

      void _sendResponse( 
	 Uint32 queueId, 
	 Array<Sint8>& message); 

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
      void _sendSuccess( 
	 Uint32 queueId, 
	 const String& authResponse);
#endif 

      void _sendChallenge(
	 Uint32 queueId,
	 const String& authResponse);

      void _sendError(
	 Uint32 queueId,
	 const String errorMessage);

      Uint32 _operationMessageQueueId;

      Uint32 _exportMessageQueueId;

      AuthenticationManager* _authenticationManager;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPAuthenticatorDelegator_h */
