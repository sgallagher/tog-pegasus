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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ClientAuthenticator_h
#define Pegasus_ClientAuthenticator_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Client/Linkage.h>

PEGASUS_NAMESPACE_BEGIN


/** This class owns client authentication 
*/
class PEGASUS_CLIENT_LINKAGE ClientAuthenticator
{
public:

    enum AuthType { BASIC, DIGEST, LOCAL, LOCALPRIVILEGED };

    /** Constuctor. */
    ClientAuthenticator();

    /** Destructor. */
    ~ClientAuthenticator();

    /** Build the request authentication header.
    */
    String buildRequestAuthHeader();

    /** Check whether the response header has authentication challenge 
        (i.e, WWW-Authenticate header).
    */
    Boolean checkResponseHeaderForChallenge(Array<HTTPHeader> headers);

    /** Save the request message for resending on a challenge.
    */
    void setRequestMessage(Message* message);

    /** Get the request message saved for resending on a challenge.
    */
    Message* getRequestMessage();

    /** Clear the request buffer and other authentication informations
        from the earlier request.
    */
    void clearRequest();

    /** Set the user name
    */
    void setUserName(const String& userName);

    /** Get the user name
    */
    String getUserName();

    /** Set the upasswor
    */
    void setPassword(const String& password);

    /** Get the password
    */
    String getPassword();

    /** Set the authentication type
    */
    void setAuthType(AuthType type);

    /** Get the authentication type
    */
    AuthType getAuthType();

private:

    String _getFileContent(String filePath);

    Message* _requestMessage;    

    Boolean  _challengeReceived;

    String   _challengeResponse;

    String   _userName;

    String   _password;

    AuthType _authType;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ClientAuthenticator_h */
