//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HTTPAuthenticatorDelegator_h
#define Pegasus_HTTPAuthenticatorDelegator_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Security/Authentication/AuthenticationManager.h>

PEGASUS_NAMESPACE_BEGIN

/**
    This class parses the HTTP header in the HTTPMessage passed to it and
    performs authentication based on the authentication specified in the
    configuration. It sends the challenge for unauthorized requests and
    validates the response.

*/

class PEGASUS_SERVER_LINKAGE HTTPAuthenticatorDelegator : public MessageQueue
{
public:

    /** Constructor. */
    HTTPAuthenticatorDelegator(
	Uint32 operationMessageQueueId,
	Uint32 exportMessageQueueId);

    /** Destructor. */
    ~HTTPAuthenticatorDelegator();

    virtual void handleEnqueue();

    virtual const char* getQueueName() const;

    void handleHTTPMessage(HTTPMessage* httpMessage, Boolean & deleteMessage);

private:

    void _sendResponse( 
        Uint32 queueId, 
        Array<Sint8>& message); 

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
