//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author:  Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AuthenticationInfoRep_h
#define Pegasus_AuthenticationInfoRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Linkage.h>


PEGASUS_NAMESPACE_BEGIN

class AuthenticationInfo;

/**
    This class keeps the authentication information of a connection 
    persistent until the connection is destroyed.
*/
class PEGASUS_COMMON_LINKAGE AuthenticationInfoRep :  public Sharable
{
public:
    enum AuthStatus { NEW_REQUEST, CHALLENGE_SENT, AUTHENTICATED };

    AuthenticationInfoRep(Boolean flag);

    ~AuthenticationInfoRep();

    AuthStatus getAuthStatus() const 
    { 
        return _authStatus;
    } 

    void   setAuthStatus(AuthStatus status);

    String getAuthenticatedUser() const 
    { 
        return _authUser;
    } 

    void   setAuthenticatedUser(const String& userName);

    String getAuthChallenge() const 
    { 
        return _authChallenge;
    } 

    void   setAuthChallenge(const String& challenge);

    String getAuthSecret() const 
    { 
        return _authSecret;
    } 

    void   setAuthSecret(const String& secret);

    Boolean isPrivileged() const 
    { 
        return _privileged;
    } 

    void   setPrivileged(Boolean privileged);

    Boolean isAuthenticated() const 
    { 
        return (_authStatus == AUTHENTICATED) ? true : false;
    } 

    String getAuthType() const 
    { 
        return _authType;
    } 

    void   setAuthType(const String& authType);

private:

    /** Constructors  */
    AuthenticationInfoRep();

    AuthenticationInfoRep(const AuthenticationInfoRep& x);

    AuthenticationInfoRep& operator=(const AuthenticationInfoRep& x);

    String  _authUser;
    String  _authChallenge;
    String  _authSecret;
    Boolean _privileged;
    String  _authType;
    AuthStatus _authStatus;
};

PEGASUS_NAMESPACE_END

#endif   /* Pegasus_AuthenticationInfoRep_h*/
