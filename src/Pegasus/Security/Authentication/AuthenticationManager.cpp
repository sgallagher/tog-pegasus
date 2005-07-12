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
// Modified By: Dave Rosckes (rosckes@us.ibm.com)
//                Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//              Sushma Fernandes, Hewlett-Packard Company(sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <Pegasus/Config/ConfigManager.h>

#include "LocalAuthenticationHandler.h"
#include "BasicAuthenticationHandler.h"
#include "AuthenticationManager.h"

#include <Pegasus/Common/AutoPtr.h>

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
#include "KerberosAuthenticationHandler.h"
#endif


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//
// Constructor
//
AuthenticationManager::AuthenticationManager()
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationManager::AuthenticationManager()");

    //
    // get authentication handlers
    //
    _localAuthHandler = _getLocalAuthHandler();

    _httpAuthHandler = _getHttpAuthHandler();

    PEG_METHOD_EXIT();
}

//
// Destructor
//
AuthenticationManager::~AuthenticationManager()
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationManager::~AuthenticationManager()");

    //
    // delete authentication handlers
    //
    if ( _localAuthHandler )
    {
        delete _localAuthHandler;
    }
    if ( _httpAuthHandler )
    {
        delete _httpAuthHandler;
    }

    PEG_METHOD_EXIT();
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
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationManager::performHttpAuthentication()");

    String authType = String::EMPTY;

    String cookie = String::EMPTY;

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		"AuthenticationManager:: performHttpAuthentication - Authority Header: $0", authHeader); 

    //
    // Parse the HTTP authentication header for authentication information
    //
    if ( !_parseHttpAuthHeader(authHeader, authType, cookie) )
    {
        PEG_METHOD_EXIT();
        return false;
    }

    Boolean authenticated = false;

    //
    // Check the authenticationinformation and do the authentication
    //
    if ( String::equalNoCase(authType, "Basic") &&
         String::equalNoCase(_httpAuthType, "Basic") )
    {
        authenticated = _httpAuthHandler->authenticate(cookie, authInfo);
    }
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    else if ( String::equalNoCase(authType, "Negotiate") &&
              String::equalNoCase(_httpAuthType, "Kerberos") )
    {
        authenticated = _httpAuthHandler->authenticate(cookie, authInfo);
    }
#endif
    // FUTURE: Add code to check for "Digest" when digest 
    // authentication is implemented.

    if ( authenticated )
    {
        authInfo->setAuthStatus(AuthenticationInfoRep::AUTHENTICATED);

        authInfo->setAuthType(authType);
    }

    PEG_METHOD_EXIT();

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
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationManager::performPegasusAuthentication()");

    Boolean authenticated = false;

    String authType = String::EMPTY; 
    String userName = String::EMPTY;
    String cookie = String::EMPTY;

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		"AuthenticationManager:: performPegasusAuthentication - Authority Header: $0",
		authHeader); 

    //
    // Parse the pegasus authentication header authentication information
    //
    if ( !_parseLocalAuthHeader(authHeader, authType, userName, cookie) )
    {
        PEG_METHOD_EXIT();
        return false;
    }

//
// Note: Pegasus LocalPrivileged authentication is not being used, but the
// code is kept here so that we can use it in the future if needed.
//
#if defined(PEGASUS_LOCAL_PRIVILEGED_AUTHENTICATION)
    if ( String::equalNoCase(authType, "LocalPrivileged") )
    {
        if (authInfo->isAuthenticated() && authInfo->isPrivileged() &&
            String::equal(userName, authInfo->getAuthenticatedUser()))
        {
            PEG_METHOD_EXIT();
            return true;
        }
    }
#endif

    if ( String::equalNoCase(authType, "Local") )
    {
        if (authInfo->isAuthenticated() &&
            String::equal(userName, authInfo->getAuthenticatedUser()))
        {
            PEG_METHOD_EXIT();
            return true;
        }
    }
    else
    {
        PEG_METHOD_EXIT();
        return false;
    }

    //
    // Check if the authentication information is present
    //
    if ( String::equal(cookie, String::EMPTY) )
    {
        PEG_METHOD_EXIT();
        return false;
    }

    authenticated = 
        _localAuthHandler->authenticate(cookie, authInfo);

    if ( authenticated )
    {
        authInfo->setAuthStatus(AuthenticationInfoRep::AUTHENTICATED);

#if defined(PEGASUS_LOCAL_PRIVILEGED_AUTHENTICATION)
        if ( String::equal(authType, "LocalPrivileged") )
        {
            authInfo->setPrivileged(true);
        }
        else
        {
            authInfo->setPrivileged(false);
        }
#endif

        authInfo->setAuthType(authType);
    }

    PEG_METHOD_EXIT();

    return ( authenticated );
}

//
// Validate user.
//
Boolean AuthenticationManager::validateUserForHttpAuth (const String& userName)
{
    return _httpAuthHandler->validateUser(userName);
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
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationManager::getPegasusAuthResponseHeader()");

    String respHeader = String::EMPTY;

    String authType = String::EMPTY;
    String userName = String::EMPTY;
    String cookie = String::EMPTY;

    //
    // Parse the pegasus authentication header authentication information
    //
    if ( !_parseLocalAuthHeader(authHeader, authType, userName, cookie) )
    {
        PEG_METHOD_EXIT();
        return (respHeader);
    }

    //
    // User name can not be empty 
    //
    if ( String::equal(userName, String::EMPTY) )
    {
        PEG_METHOD_EXIT();
        return (respHeader);
    }

    respHeader = 
        _localAuthHandler->getAuthResponseHeader(authType, userName, authInfo);

    PEG_METHOD_EXIT();

    return (respHeader);

}

//
// Get HTTP authentication response header
//
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
String AuthenticationManager::getHttpAuthResponseHeader( AuthenticationInfo* authInfo )
#else		
String AuthenticationManager::getHttpAuthResponseHeader()
#endif
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationManager::getHttpAuthResponseHeader()");

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    String respHeader = _httpAuthHandler->getAuthResponseHeader(
	String::EMPTY, String::EMPTY, authInfo);
#else
    String respHeader = _httpAuthHandler->getAuthResponseHeader();
#endif

    PEG_METHOD_EXIT();

    return (respHeader);
}

//
// parse the local authentication header
//
Boolean AuthenticationManager::_parseLocalAuthHeader(
    const String& authHeader, String& authType, String& userName, String& cookie)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationManager::_parseLocalAuthHeader()");

    //
    // Extract the authentication type:
    //
    Uint32 space = authHeader.find(' ');

    if ( space == PEG_NOT_FOUND )
    {
        PEG_METHOD_EXIT();
        return false;
    }

    authType = authHeader.subString(0, space);

    Uint32 startQuote = authHeader.find(space, '"');

    if ( startQuote == PEG_NOT_FOUND )
    {
        PEG_METHOD_EXIT();
        return false; 
    }

    Uint32 endQuote = authHeader.find(startQuote + 1, '"');

    if ( endQuote == PEG_NOT_FOUND )
    {
        PEG_METHOD_EXIT();
        return false;
    }

    String temp = authHeader.subString(
        startQuote + 1, (endQuote - startQuote - 1));

    //
    // Extract the user name and cookie:
    //
    Uint32 colon = temp.find(0, ':');

    if ( colon == PEG_NOT_FOUND )
    {
        userName = temp;
    }
    else
    {
        userName = temp.subString(0, colon);
        cookie = temp;
    }

    PEG_METHOD_EXIT();

    return true;
}

//
// parse the HTTP authentication header
//
Boolean AuthenticationManager::_parseHttpAuthHeader(
    const String& authHeader, String& authType, String& cookie)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationManager::_parseHttpAuthHeader()");

    //
    // Extract the authentication type:
    //
    Uint32 space = authHeader.find(' ');

    if ( space == PEG_NOT_FOUND )
    {
        PEG_METHOD_EXIT();
        return false;
    }

    authType = authHeader.subString(0, space);

    //
    // Extract the cookie:
    //
    cookie = authHeader.subString(space + 1);

    PEG_METHOD_EXIT();

    return true;
}
//
// Get local authentication handler
//
Authenticator* AuthenticationManager::_getLocalAuthHandler()
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationManager::_getLocalAuthHandler()");

    PEG_METHOD_EXIT();
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
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationManager::_getHttpAuthHandler()");
    AutoPtr<Authenticator> handler;

    //
    // get the configured authentication type
    //
    AutoPtr<ConfigManager> configManager(ConfigManager::getInstance());

    _httpAuthType = configManager->getCurrentValue("httpAuthType");
    configManager.release();
    //
    // create a authentication handler.
    //
    if ( String::equalNoCase(_httpAuthType, "Basic") )
    {
        handler.reset((Authenticator* ) new BasicAuthenticationHandler( ));
    }
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    else if ( String::equalNoCase(_httpAuthType, "Kerberos") )
    {
        handler.reset((Authenticator* ) new KerberosAuthenticationHandler( ));
        AutoPtr<KerberosAuthenticationHandler> kerberosHandler((KerberosAuthenticationHandler *)handler.get());
        int itFailed = kerberosHandler->initialize();
        kerberosHandler.release();
        if (itFailed)
        {
            if (handler.get())
            {
                handler.reset(0);
            }
            // L10N TODO DONE
            //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE, 
                //"CIMOM server authentication handler for Kerberos failed to initialize properly. The CIMOM server is not started.");
            Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE, 
            	"Security.Authentication.AuthenticationManager.AUTHENTICATION_HANDLER_KERBEROS_FAILED_TO_INITIALIZE",
                "CIMOM server authentication handler for Kerberos failed to initialize properly. The CIMOM server is not started.");
            // end the server because Kerberos could not initialized.
            MessageLoaderParms parms(           	"Security.Authentication.AuthenticationManager.AUTHENTICATION_HANDLER_KERBEROS_FAILED_TO_INITIALIZE",
                "CIMOM server authentication handler for Kerberos failed to initialize properly. The CIMOM server is not started.");
	    throw Exception(parms);
        }
    }
#endif
    // FUTURE: uncomment these line when Digest authentication 
    // is implemented.
    //
    //else if (String::equalNoCase(_httpAuthType, "Digest"))
    //{
    //    handler = (Authenticator* ) new DigestAuthenticationHandler( );
    //}
    else 
    {
        //
        // This should never happen. Gets here only if Security Config
        // property owner has not validated the configured http auth type.
        //
        PEGASUS_ASSERT(0);
    }
    
    PEG_METHOD_EXIT();
    return ( handler.release() );
}


PEGASUS_NAMESPACE_END

