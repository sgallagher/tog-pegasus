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
// Modified By:  Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//            Sushma Fernandes, Hewlett-Packard Company(sushma_fernandes@hp.com)
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

    _localAuthenticator.reset((LocalAuthenticator*) new SecureLocalAuthenticator());

    PEG_METHOD_EXIT();
}

LocalAuthenticationHandler::~LocalAuthenticationHandler()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, 
        "LocalAuthenticationHandler::~LocalAuthenticationHandler()");

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

    //
    // Check if the user is a valid system user
    //
    if ( !System::isSystemUser( userName.getCString() ) )
    {
        PEG_METHOD_EXIT();
        return (authenticated);
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

Boolean LocalAuthenticationHandler::validateUser(const String& userName)
{
    return _localAuthenticator->validateUser(userName);
}

String LocalAuthenticationHandler::getAuthResponseHeader(
    const String& authType,
    const String& userName,
    AuthenticationInfo* authInfo)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, 
        "LocalAuthenticationHandler::getAuthResponseHeader()");

    String challenge = String::EMPTY;
    String authResp = String::EMPTY;

    //
    // Check if the user is a valid system user
    //
    if ( !System::isSystemUser( userName.getCString() ) )
    {
        PEG_METHOD_EXIT();
        return ( authResp );
    }

    authResp = _localAuthenticator->getAuthResponseHeader(authType, userName, challenge);

    authInfo->setAuthChallenge(challenge);

    PEG_METHOD_EXIT();

    return ( authResp );
}

PEGASUS_NAMESPACE_END
