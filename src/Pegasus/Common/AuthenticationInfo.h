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
// Author:  Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AuthenticationInfo_h
#define Pegasus_AuthenticationInfo_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>


PEGASUS_NAMESPACE_BEGIN

/**
    This class keeps the authentication information of a connection 
    persistent until the connection is destroyed.
*/

class PEGASUS_COMMON_LINKAGE AuthenticationInfo
{
public:

    enum Status { NEW_REQUEST, CHALLENGE_SENT, AUTHENTICATED };

    /** Constructors  */
    AuthenticationInfo();

    /** Destructor  */
    ~AuthenticationInfo();

    /**
    Get the authentication status of the request
    */
    Uint32 getAuthStatus() const { return _authStatus; }

    /**
    Set the authentication status of the request
    */
    void   setAuthStatus(Uint32 status);

    /**
    Get the authenticated user name
    */
    String getAuthenticatedUser() const { return _authUser; }

    /**
    Set the authenticated user name
    */
    void   setAuthenticatedUser(String userName);

    /**
    Get the authentication challenge
    */
    String getAuthChallenge() const { return _authChallenge; }

    /**
    Set the authentication challenge
    */
    void   setAuthChallenge(String challenge);

    /**
    Get the authentication secret
    */
    String getAuthSecret() const { return _authSecret; }

    /**
    Set the authentication secret
    */
    void   setAuthSecret(String secret);

    /**
    Set the privileged flag
    */
    void   setPrivileged(Boolean privileged);

    /**
    Is the request coming from a privileged user
    */
    Boolean isPrivileged() const { return _privileged; }

    /**
    Is the request authenticated
    */
    Boolean isAuthenticated() const { return (_authStatus == AUTHENTICATED) ? true : false; }

private:

    Uint32  _authStatus;

    String  _authUser;

    String  _authChallenge;

    String  _authSecret;

    Boolean _privileged;

};

PEGASUS_NAMESPACE_END

#endif   /* Pegasus_AuthenticationInfo_h*/
