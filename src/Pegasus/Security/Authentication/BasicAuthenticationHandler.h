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
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//            Sushma Fernandes, Hewlett-Packard Company(sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_BasicAuthenticationHandler_h
#define Pegasus_BasicAuthenticationHandler_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

#include "Authenticator.h"
#include "BasicAuthenticator.h"

#include <Pegasus/Security/Authentication/Linkage.h>


PEGASUS_NAMESPACE_BEGIN

/** 
    Implements the AuthenticationHandler for Basic authentication
*/

class PEGASUS_SECURITY_LINKAGE BasicAuthenticationHandler : public Authenticator
{
public:

    /** Constructors  */
    BasicAuthenticationHandler();

    /** Destructor  */
    ~BasicAuthenticationHandler();
    
    /** Verify the authentication of the user passed in the authorization header.
        @param authHeader String containing the Authorization header
        @param authInfo Reference to AuthenticationInfo object
        @return true on successful authentication, false otherwise
    */
    Boolean authenticate(
        const String& authHeader, 
        AuthenticationInfo* authInfo);
    
    /** Construct and return the Basic authentication challenge header
        @param authType An optional string containing the HTTP authentication type 
        @param userName An optional string containing the user name 
        (Parameters are not used to generate the Basic authentication challenge 
         header, hence they are optional) 
        @return A string containing the authentication challenge header.
    */
    String getAuthResponseHeader(
        const String& authType = String::EMPTY,
        const String& userName = String::EMPTY,
        AuthenticationInfo* authInfo = 0);

    Boolean validateUser(const String& userName);

private:

    BasicAuthenticator* _basicAuthenticator;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_BasicAuthenticationHandler_h*/
