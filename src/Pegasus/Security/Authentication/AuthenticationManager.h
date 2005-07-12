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

#ifndef Pegasus_AuthenticationManager_h
#define Pegasus_AuthenticationManager_h

#include <Pegasus/Common/Config.h>

#include "Authenticator.h"

#include <Pegasus/Security/Authentication/Linkage.h>

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
#include <Pegasus/Common/AuthenticationInfo.h>
#endif

PEGASUS_NAMESPACE_BEGIN

/** This class implements the HTTP authentication and Pegasus Local 
    authentication mecahnism. It provides methods to perform authentication
    and to generate authentication challenge headers.
*/

class PEGASUS_SECURITY_LINKAGE AuthenticationManager
{
public:

    /**Constructor */
    AuthenticationManager();

    /**Destructor */
    ~AuthenticationManager();

    /** Authenticates the requests from HTTP connections.
        @param authHeader String containing the Authorization header
        @param authInfo Reference to AuthenticationInfo object that holds the
        authentication information for the given connection.
        @return true on successful authentication, false otherwise
    */
    Boolean performHttpAuthentication(
        const String& authHeader,
        AuthenticationInfo* authInfo);

    /** Authenticates the requests from Local connections.
        @param authHeader String containing the Authorization header
        @param authInfo Reference to AuthenticationInfo object that holds the
        authentication information for the given connection.
        @return true on successful authentication, false otherwise
    */
    Boolean performPegasusAuthentication(
        const String& authHeader,
        AuthenticationInfo* authInfo);

    /** Validates whether the user is a valid user for requests
        from HTTP connections.
        @param  userName  name of the user
        @return true on successful validation, false otherwise
    */
    Boolean validateUserForHttpAuth (const String& userName);

    /** Constructs the Pegasus Local authentication challenge header.
        @param authHeader String containing the Authorization header
        @param authInfo reference to AuthenticationInfo object that holds the
        authentication information for the given connection.
        @return String containing the authentication challenge
    */
    String getPegasusAuthResponseHeader(
        const String& authHeader,
        AuthenticationInfo* authInfo);

    /** Constructs the HTTP authentication challenge header.
        @return String containing the authentication challenge
    */
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    String AuthenticationManager::getHttpAuthResponseHeader(
	AuthenticationInfo* authInfo = 0);
#else
    String getHttpAuthResponseHeader();
#endif

private:

    Boolean _parseLocalAuthHeader(
        const String& authHeader, 
        String& authType, 
        String& userName, 
        String& cookie);

    Boolean _parseHttpAuthHeader(
        const String& authHeader, 
        String& authType, 
        String& cookie);

    Authenticator* _getLocalAuthHandler();

    Authenticator* _getHttpAuthHandler();

    Authenticator* _localAuthHandler;
    Authenticator* _httpAuthHandler;
    String         _httpAuthType;

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_AuthenticationManager_h */

