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

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Security/Authentication/LocalAuthenticationHandler.h>
#include <Pegasus/Security/Authentication/BasicAuthenticationHandler.h>
#include "AuthenticationManager.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//
// Constructor
//
AuthenticationManager::AuthenticationManager()
{
    const char METHOD_NAME[] = "AuthenticationManager::AuthenticationManager()";

    PEG_FUNC_ENTER(TRC_AUTHENTICATION, METHOD_NAME);

    //
    // get authentication handler
    //
    _localAuthHandler = _getLocalAuthHandler();

    _httpAuthHandler = _getHttpAuthHandler();

    //
    // Build the Basic authentication challenge header 
    // "hostname" + ":" + "portNo" using the hostname and port number
    //

    //
    // get the local system name
    //
    _realm.assign(System::getHostName());

    //
    // get the configured port number
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    String port = configManager->getCurrentValue("port");

    _realm.append(":"); 
    _realm.append(port); 

    PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
}

//
// Destructor
//
AuthenticationManager::~AuthenticationManager()
{
    const char METHOD_NAME[] = "AuthenticationManager::~AuthenticationManager()";

    PEG_FUNC_ENTER(TRC_AUTHENTICATION, METHOD_NAME);

    //
    // delete authentication handler
    //
    if (_localAuthHandler)
    {
        delete _localAuthHandler;
    }
    if (_httpAuthHandler)
    {
        delete _httpAuthHandler;
    }

    PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
}

//
// Perform http authentication
//
Boolean AuthenticationManager::performHttpAuthentication
(
    const String& authHeader,
    AuthenticationInfo* authInfo
)
{
    const char METHOD_NAME[] = "AuthenticationManager::performHttpAuthentication()";

    PEG_FUNC_ENTER(TRC_AUTHENTICATION, METHOD_NAME);

    Boolean authenticated = false;

    String type = String::EMPTY;
    String userName = String::EMPTY;
    String cookie = String::EMPTY;

    //
    // Check whether the auth header has the authentication
    // information or not and call authentication handlers
    // authenticate method.
    //
    _parseAuthHeader(authHeader, type, userName, cookie);

    //
    // Check if the user is already authenticated
    //
    if (authInfo->isAuthenticated() && (authInfo->isPrivileged() ||
        String::equal(userName, authInfo->getAuthenticatedUser())))
    {
        PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
        return true;
    }

    //
    // get the configured authentication type
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    String authType = configManager->getCurrentValue("httpAuthType");

    //
    // Check whether the auth header has the authentication 
    // information or not.
    //
    if (String::equalNoCase(authHeader, "Basic"))
    {
        //
        // Check if Basic authentication is supported or not.
        //
        if (!String::equalNoCase(authType, "Basic"))
        {
            // ATTN: Log basic authentication not supported message
            PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
            return ( authenticated );
        }

        Uint32 pos = authHeader.find("Basic");

        if (authHeader.size() > (pos + 5))
        {
            cookie = authHeader.subString(pos + 6);
        }

        authenticated = _httpAuthHandler->authenticate(cookie, authInfo);
    }
    // else  ATTN: add code for digest authentication

    // else  ATTN: Log authentication type not supported message

    if (authenticated)
    {
        authInfo->setAuthStatus(AuthenticationInfo::AUTHENTICATED);
    }

    PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);

    return ( authenticated );
}

//
// Perform pegasus sepcific local authentication
//
Boolean AuthenticationManager::performPegasusAuthentication
(
    const String& authHeader,
    AuthenticationInfo* authInfo
)
{
    const char METHOD_NAME[] = "AuthenticationManager::performPegasusAuthentication()";

    PEG_FUNC_ENTER(TRC_AUTHENTICATION, METHOD_NAME);

    Boolean authenticated = false;

    String authType = String::EMPTY; 
    String userName = String::EMPTY;
    String cookie = String::EMPTY;

    //
    // Check whether the auth header has the authentication 
    // information or not and call authentication handlers
    // authenticate method.
    //
    _parseAuthHeader(authHeader, authType, userName, cookie);


    //
    // Check if the user is already authenticated
    //
    if (authInfo->isAuthenticated() && (authInfo->isPrivileged() ||
        String::equal(userName, authInfo->getAuthenticatedUser())))
    {
        PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
        return true;
    }

    //
    // Check if the authentication information is present
    //
    if (String::equal(cookie, String::EMPTY))
    {
        PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
        return false;
    }

    authenticated = 
        _localAuthHandler->authenticate(cookie, authInfo);

    if (authenticated)
    {
        authInfo->setAuthStatus(AuthenticationInfo::AUTHENTICATED);

        if ( String::equal(authType, "LocalPrivileged") )
        {
            authInfo->setPrivileged(true);
        }
        else
        {
            authInfo->setPrivileged(false);
        }
    }

    PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);

    return ( authenticated );
}

//
// Get pegasus/local authentication response header
//
String AuthenticationManager::getPegasusAuthResponseHeader
(
    const String& authHeader,
    AuthenticationInfo* authInfo
)
{
    const char METHOD_NAME[] = "AuthenticationManager::getPegasusAuthResponseHeader()";

    PEG_FUNC_ENTER(TRC_AUTHENTICATION, METHOD_NAME);

    String authType = String::EMPTY;
    String userName = String::EMPTY;
    String cookie = String::EMPTY;

    //
    // Check whether the auth header has the authentication
    // information or not and call authentication handlers
    // authenticate method.
    //
    _parseAuthHeader(authHeader, authType, userName, cookie);

    //
    // Check if the authentication information is present
    //
    if (String::equal(userName, String::EMPTY))
    {
        //
        // User name can not be empty 
        //
        // ATTN: throw an exception
        PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
        return (String::EMPTY);
    }

    PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);

    return(_localAuthHandler->getAuthResponseHeader(authType, userName, authInfo));
}

//
// Get HTTP authentication response header
//
String AuthenticationManager::getHttpAuthResponseHeader()
{
    const char METHOD_NAME[] = "AuthenticationManager::getHttpAuthResponseHeader()";

    PEG_FUNC_ENTER(TRC_AUTHENTICATION, METHOD_NAME);

    PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);

    return (_httpAuthHandler->getAuthResponseHeader(_realm));
}

//
// parse the authentication header
//
void AuthenticationManager::_parseAuthHeader(
    const String& authHeader, String& authType, String& userName, String& cookie)
{
    const char METHOD_NAME[] = "AuthenticationManager::_parseAuthHeader()";

    PEG_FUNC_ENTER(TRC_AUTHENTICATION, METHOD_NAME);

    Uint32 pos;

    if ( (pos = authHeader.find("LocalPrivileged")) == PEG_NOT_FOUND )
    {
        if ( (pos = authHeader.find("Local")) == PEG_NOT_FOUND )
        {
            //
            //Invalid authorization header
            //
            //ATTN: throw exception
            PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
            return;
        }
    }

    Uint32 startQuote = authHeader.find(pos, '"');
    if (startQuote == PEG_NOT_FOUND)
    {
        //
        //Invalid authorization header
        //
        //ATTN: throw exception
        PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
        return; 
    }

    Uint32 endQuote = authHeader.find(startQuote + 1, '"');
    if (endQuote == PEG_NOT_FOUND)
    {
        //
        //Invalid authorization header
        //
        //ATTN: throw exception
        PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
        return;
    }

    authType = authHeader.subString(pos, (startQuote - pos) - 1);

    String temp = authHeader.subString(
        startQuote + 1, (endQuote - startQuote - 1));

    Uint32 colonPos;

    if ((colonPos = temp.find(0, ':')) == PEG_NOT_FOUND)
    {
        userName = temp;
        PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
        return;
    }
    else
    {
        userName = temp.subString(0, colonPos);
        cookie = temp;
    }

    PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
}

//
// Get local authentication handler
//
Authenticator* AuthenticationManager::_getLocalAuthHandler()
{
    const char METHOD_NAME[] = "AuthenticationManager::_getLocalAuthHandler()";

    PEG_FUNC_ENTER(TRC_AUTHENTICATION, METHOD_NAME);

    PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);
    //
    // create and return a local authentication handler.
    //
    return (new LocalAuthenticationHandler());
}


//
// Get Http authentication handler
//
Authenticator* AuthenticationManager::_getHttpAuthHandler()
{
    const char METHOD_NAME[] = "AuthenticationManager::_getHttpAuthHandler()";

    PEG_FUNC_ENTER(TRC_AUTHENTICATION, METHOD_NAME);

    Authenticator* handler = 0;

    //
    // get the configured/default authentication type
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    String authType = configManager->getCurrentValue("httpAuthType");
    
    //
    // If Basic authentication is configured then 
    // create a basic authentication handler.
    //
    if (String::equal(authType, "Basic"))
    {
        handler = (Authenticator* ) new BasicAuthenticationHandler( );
    }

    //ATTN: add support for Digest authentication.
    //else if (authType.equalNoCase("Digest"))
    //{
    //    handler = (Authenticator* ) new DigestAuthenticationHandler( );
    //}
    
    PEG_FUNC_EXIT(TRC_AUTHENTICATION, METHOD_NAME);

    return ( handler );
}


PEGASUS_NAMESPACE_END

