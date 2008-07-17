//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Modified By: Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//            Sushma Fernandes, Hewlett-Packard Company(sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Security/UserManager/UserExceptions.h>
#include "SecureBasicAuthenticator.h"
#include <Pegasus/Common/Executor.h>

#ifdef PEGASUS_OS_ZOS
#include <pwd.h>
#ifdef PEGASUS_ZOS_SECURITY
// This include file will not be provided in the OpenGroup CVS for now.
// Do NOT try to include it in your compile
#include <Pegasus/Common/safCheckzOS_inline.h>
#endif
#endif

#ifdef PEGASUS_OS_PASE
#include <ILEWrapper/ILEUtilities2.h>
#endif

PEGASUS_USING_STD;


//==============================================================================
//
// VxWorks authentication callback and data. Should return 0 if authenticaiton
// successful.
//
//==============================================================================

#if defined(PEGASUS_OS_VXWORKS)

extern "C"
{
    int (*pegasusAuthCallback)(const char* user, const char* pass, void* data);
    void* pegasusAuthData;
}

#endif /* defined(PEGASUS_OS_VXWORKS) */

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// SecureBasicAuthenticator class implementation
//
//==============================================================================

/**
    Constant representing the Basic authentication challenge header.
*/
static const String BASIC_CHALLENGE_HEADER = "WWW-Authenticate: Basic \"";



/* constructor. */
SecureBasicAuthenticator::SecureBasicAuthenticator() 
{ 
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureBasicAuthenticator::SecureBasicAuthenticator()");

    //
    // get the local system name
    //
    _realm.assign(System::getHostName());

    //
    // get the configured port number
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    String port = configManager->getCurrentValue("httpPort");

    //
    // Create realm that will be used for Basic challenges
    //
    _realm.append(":");
    _realm.append(port);

    //
    // Get a user manager instance handler
    //
    _userManager = UserManager::getInstance();

    PEG_METHOD_EXIT();
}

/* destructor. */
SecureBasicAuthenticator::~SecureBasicAuthenticator() 
{ 
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureBasicAuthenticator::~SecureBasicAuthenticator()");

    PEG_METHOD_EXIT();
}

Boolean SecureBasicAuthenticator::authenticate(
    const String & userName,
    const String & password)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureBasicAuthenticator::authenticate()");

/*
ATTN:
*/
    Boolean authenticated = false;

#if defined(PEGASUS_OS_ZOS)

    // use zOS API to do the user name and password verification
    // note: write possible errors to the tracelog
    if ((password.size() > 0) &&
        (__passwd
         ((const char *) userName.getCString(),
          (const char *) password.getCString(), NULL) == 0))
    {
# ifdef PEGASUS_ZOS_SECURITY

        if (CheckProfileCIMSERVclassWBEM(userName, __READ_RESOURCE))
        {
            authenticated = true;
        }
        else
        {
            authenticated = false;
            // no READ access to security resource profile CIMSERV CL(WBEM)
            Logger::put_l(
                Logger::TRACE_LOG, ZOS_SECURITY_NAME, Logger::WARNING,
                "Security.Authentication.SecureBasicAuthenticator"
                ".NOREAD_CIMSERV_ACCESS.PEGASUS_OS_ZOS",
                "Request UserID $0 misses READ permission to profile CIMSERV "
                "CL(WBEM).",
                userName);
        }

# else /* PEGASUS_OS_ZOS */

        authenticated = true;

# endif /* !PEGASUS_OS_ZOS */
    }
    else
    {
        authenticated = false;
        PEG_TRACE_CSTRING(TRC_AUTHENTICATION, Tracer::LEVEL4,
            "Authentication failed.");
    }

#elif defined(PEGASUS_OS_PASE)
    CString userCStr = userName.getCString();
    CString pwCStr = password.getCString();

    if ((strlen(userCStr) == 0) || (strlen(pwCStr) == 0))
    {
        authenticated = false;
    }
    else
    {
        // this function only can be found in PASE environment
        authenticated = umeUserAuthenticate((const char *)userCStr,
                                            (const char *)pwCStr);
    }

#elif defined(PEGASUS_OS_VXWORKS)
    {
        CString user(userName.getCString());
        CString pass(password.getCString());

        if (pegasusAuthCallback)
            return (*pegasusAuthCallback)(user, pass, pegasusAuthData) == 0;
        else
            return true;
    }
#else /* DEFAULT (!PEGASUS_OS_ZOS) */

    // Check whether valid system user.

    if (!System::isSystemUser(userName.getCString()))
    {
        PEG_METHOD_EXIT();
        return (authenticated);
    }

    try
    {
        // Verify the CIM user password. Use executor if present. Else, use
        // conventional method.

        if (Executor::detectExecutor() == 0)
        {
            if (Executor::authenticatePassword(
                userName.getCString(), password.getCString()) == 0)
            {
                authenticated = true;
            }
        }
        else
        {
            if (_userManager->verifyCIMUserPassword(userName, password))
                authenticated = true;
        }
    }
    catch(InvalidUser &)
    {
        PEG_METHOD_EXIT();
        return (authenticated);
    }
    catch(Exception & e)
    {
        PEG_METHOD_EXIT();
        throw e;
    }

#endif /* DEFAULT (!PEGASUS_OS_ZOS) */

    PEG_METHOD_EXIT();

    return (authenticated);
}

Boolean SecureBasicAuthenticator::validateUser(const String& userName)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureBasicAuthenticator::validateUser()");

    Boolean authenticated = false;

    if ( System::isSystemUser(userName.getCString()))
    {
#if defined (PEGASUS_OS_PASE)
        authenticated = true;
#else
        if (Executor::detectExecutor() == 0)
        {
            if (Executor::validateUser(userName.getCString()) != 0)
                authenticated = true;
        }
        else if (_userManager->verifyCIMUser(userName))
        {
            authenticated = true;
        }
#endif
    }

    PEG_METHOD_EXIT();
    return (authenticated);
}

//
// Create authentication response header
//
String SecureBasicAuthenticator::getAuthResponseHeader()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureBasicAuthenticator::getAuthResponseHeader()");

    // 
    // build response header using realm
    //
    String responseHeader = BASIC_CHALLENGE_HEADER;
    responseHeader.append(_realm);
    responseHeader.append("\"");

    PEG_METHOD_EXIT();

    return (responseHeader);
}


PEGASUS_NAMESPACE_END
