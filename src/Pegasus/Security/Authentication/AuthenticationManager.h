//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies of substantial portions of this software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AuthenticationManager_h
#define Pegasus_AuthenticationManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Security/Authentication/Authenticator.h>
#include "Linkage.h"


PEGASUS_NAMESPACE_BEGIN

/**
    This class parses the HTTP header in the HTTPMessage passed to it and 
    performs authentication based on the authentication specified in the 
    configuration. It sends the challenge for unauthorized requests and
    validates the response.

    NOTE: This code is not integarated in to the main Makefile because the
    dependent code is not implemented. 
*/

class PEGASUS_SECURITY_LINKAGE AuthenticationManager
    : public MessageQueue
{
public:

    AuthenticationManager(
        MessageQueue* outputQueue,
        Uint32 returnQueueId);

    ~AuthenticationManager();

    void sendResponse(
        Uint32 queueId, 
        Array<Sint8>& message);

    void sendError(
        Uint32 queueId,
        const String& messageId,
        const String& methodName,
        CIMStatusCode code,
        const String& description);

    void sendChallenge(
        Uint32 queueId, 
        const String& authResponse);

    virtual void handleEnqueue();

    virtual const char* getQueueName() const;

    void handleHTTPMessage(HTTPMessage* httpMessage);

    void handleMethodCall(
        Uint32 queueId,
        Sint8* content);

private:

    Boolean _performHttpAuthentication(Uint32 queueId, String authHeader);

    Boolean _performLocalAuthentication(Uint32 queueId, String authHeader);

    Authenticator* _getAuthHandler(String type);

    String         _realm;
    String         _authChallenge;
    String         _authenticationInfo;

    Authenticator* _authHandler;
    MessageQueue*  _outputQueue;

    // Queue where responses should be enqueued.
    Uint32         _returnQueueId;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_AuthenticationManager_h */

