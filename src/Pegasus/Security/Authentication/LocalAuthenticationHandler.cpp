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
#include <Pegasus/Security/Authentication/SecureLocalAuthenticator.h>
#include "LocalAuthenticationHandler.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


LocalAuthenticationHandler::LocalAuthenticationHandler()
{
    // ATTN: Load the local authentication module here
    _localAuthenticator = (LocalAuthenticator*) new SecureLocalAuthenticator();
}

LocalAuthenticationHandler::~LocalAuthenticationHandler()
{
    if (_localAuthenticator)
    {
        delete (_localAuthenticator);
    }
}

Boolean LocalAuthenticationHandler::authenticate(    
    String authHeader,
    String secretKept)
{
    Boolean authenticated   = false; 

    // Look for ':' seperator
    Uint32 colonPos = authHeader.find(':');

    if ( colonPos == PEG_NOT_FOUND )
    {
        return ( authenticated );
    }

    if ( ( colonPos > 0 ) && ( colonPos + 1 < authHeader.size() ) ) 
    {
        String userName = authHeader.subString( 0, colonPos );
        String secretReceived = authHeader.subString( colonPos + 1 );    

        authenticated = _localAuthenticator->authenticate(userName, 
            secretReceived, secretKept);
    } 

    return ( authenticated );
}

String LocalAuthenticationHandler::getAuthResponseHeader(
    String reqHeader,
    String& challenge)
{
    String userName = String::EMPTY;

    // Look for ':' seperator
    Uint32 colonPos = reqHeader.find(':');

    if ( colonPos != PEG_NOT_FOUND )
    {
        userName = reqHeader;
    }
    else
    {
        userName = reqHeader.subString(0, colonPos);
    }

    return(_localAuthenticator->getAuthResponseHeader(userName, challenge));
}

PEGASUS_NAMESPACE_END
