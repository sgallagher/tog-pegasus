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

#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>

#include "SecureLocalAuthenticator.h"
#include "LocalAuthenticationHandler.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


LocalAuthenticationHandler::LocalAuthenticationHandler()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, 
       "LocalAuthenticationHandler::LocalAuthenticationHandler()");

    _localAuthenticator = (LocalAuthenticator*) new SecureLocalAuthenticator();

    PEG_METHOD_EXIT();
}

LocalAuthenticationHandler::~LocalAuthenticationHandler()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, 
        "LocalAuthenticationHandler::~LocalAuthenticationHandler()");

    delete _localAuthenticator;

    PEG_METHOD_EXIT();
}

Boolean LocalAuthenticationHandler::authenticate(    
    const String& authHeader,
    AuthenticationInfo* authInfo)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, 
        "LocalAuthenticationHandler::authenticate()");

    Boolean authenticated   = false; 

    // Look for ':' seperator
    Uint32 colon1 = authHeader.find(':');

    if ( colon1 == PEG_NOT_FOUND )
    {
        PEG_METHOD_EXIT();
        return ( authenticated );
    }

    String userName = authHeader.subString(0, colon1);

    // Look for another ':' seperator
    Uint32 colon2 = authHeader.find(colon1 + 1, ':');

    String filePath;

    String secretReceived;

    if ( colon2 == PEG_NOT_FOUND )
    {
        filePath = String::EMPTY;

        secretReceived = authHeader.subString( colon1 + 1 );    
    }
    else
    {
        filePath = authHeader.subString( colon1 + 1, (colon2 - colon1 - 1) );

        secretReceived = authHeader.subString( colon2 + 1 );    
    }

    authenticated = _localAuthenticator->authenticate(filePath, 
        secretReceived, authInfo->getAuthChallenge());

    if (authenticated)
    {
        authInfo->setAuthenticatedUser(userName);
    }

    PEG_METHOD_EXIT();

    return ( authenticated );
}

String LocalAuthenticationHandler::getAuthResponseHeader(
    const String& authType,
    const String& userName,
    AuthenticationInfo* authInfo)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, 
        "LocalAuthenticationHandler::getAuthResponseHeader()");

    String challenge;

    String authResp = 
        _localAuthenticator->getAuthResponseHeader(authType, userName, challenge);

    authInfo->setAuthChallenge(challenge);

    PEG_METHOD_EXIT();

    return(authResp);
}

PEGASUS_NAMESPACE_END
