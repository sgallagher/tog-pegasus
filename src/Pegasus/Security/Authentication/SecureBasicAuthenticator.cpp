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
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Security/UserManager/UserExceptions.h>

#include "SecureBasicAuthenticator.h"

#ifdef PEGASUS_OS_OS400
#include "qycmutiltyUtility.H"
#include "OS400ConvertChar.h"
#endif
#ifdef PEGASUS_OS_ZOS
#include <pwd.h>
#ifdef PEGASUS_ZOS_SECURITY
// This include file will not be provided in the OpenGroup CVS for now.
// Do NOT try to include it in your compile
#include <Pegasus/Common/safCheckzOS_inline.h>
#endif
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


/**
    Constant representing the Basic authentication challenge header.
*/
static const String BASIC_CHALLENGE_HEADER = "WWW-Authenticate: Basic \"";

#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
# if (__TARGET_LIB__ < 0x410A0000) 

#define ZOS_PEGASUS_APPLID_LEN 7
static const char* ZOS_PEGASUS_APPLID = 
                      "\xC3\xC6\xE9\xC1\xD7\xD7\xD3\x40"; // "CFZAPPL "
static const char* ZOS_PEGASUS_THLI_EYE_CATCHER = "\xE3\xC8\xD3\xC9"; // "THLI"
#endif // end __TARGET_LIB__
#endif // end PEGASUS_PLATFORM_ZOS_ZSERIES_IBM

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
                                       
#ifdef PEGASUS_OS_ZOS

    if (String::equalNoCase(
        configManager->getCurrentValue("enableCFZAPPLID"),"true"))
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
# if (__TARGET_LIB__ < 0x410A0000) 
    {

        //
        // Enable __passwd() for passticket validation 
        // for APPLID CFZAPPL in this thread.
        // 
        set_ZOS_ApplicationID();
    } 
    else
    {
        Logger::put_l(Logger::TRACE_LOG, ZOS_SECURITY_NAME, Logger::WARNING,
            "Security.Authentication.SecureBasicAuthenticator"
                 ".APPLID_OMVSAPPL.PEGASUS_OS_ZOS",
            "CIM server authentication is using application ID OMVSAPPL.");
    }

    pthread_security_np(0,__USERID_IDENTITY,0,NULL,NULL,0);
#else
    {
        _zosAPPLID = "CFZAPPL";
    }
    else
    {
        _zosAPPLID = "OMVSAPPL";
        Logger::put_l(Logger::TRACE_LOG, ZOS_SECURITY_NAME, Logger::WARNING,
            "Security.Authentication.SecureBasicAuthenticator"
                 ".APPLID_OMVSAPPL.PEGASUS_OS_ZOS",
            "CIM server authentication is using application ID OMVSAPPL.");
    }
#endif // end __TARGET_LIB__
#endif // end PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#endif // end PEGASUS_OS_ZOS

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
    const String& userName, 
    const String& password) 
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureBasicAuthenticator::authenticate()");

    Boolean authenticated = false;

#ifdef PEGASUS_OS_OS400
    // Use OS400 APIs to do user name and password verification
    // (note - need to convert to EBCDIC before calling ycm)
    CString userCStr = userName.getCString();
    const char * user = (const char *)userCStr;
    AtoE((char *)user);
    CString pwCStr = password.getCString();
    const char * pw = (const char *)pwCStr;
    AtoE((char *)pw);
    int os400auth =
      ycmVerifyUserAuthorization(user, pw);
    if (os400auth == TRUE) 
        authenticated = true;
#else
#ifdef PEGASUS_OS_ZOS
    if ( password.size() == 0 )
    {               
         Logger::put_l(Logger::TRACE_LOG, ZOS_SECURITY_NAME, Logger::WARNING,
             "Security.Authentication.SecureBasicAuthenticator"
                  ".EMPTY_PASSWD.PEGASUS_OS_ZOS",
             "Request UserID $0 misses password.",userName);
        PEG_METHOD_EXIT();
        return (false);
     }

#if (__TARGET_LIB__ >= 0x410A0000)
    if (__passwd_applid((const char*) userName.getCString(), 
             (const char*) password.getCString(), NULL,
             (const char*) _zosAPPLID.getCString()) == 0)
#else
    if (__passwd((const char*) userName.getCString(), 
             (const char*) password.getCString(), NULL) == 0)
#endif
    {
#ifdef PEGASUS_ZOS_SECURITY
        if (CheckProfileCIMSERVclassWBEM( userName , __READ_RESOURCE))
        {
            authenticated = true;
        }
        else
        {
            authenticated = false;
            // no READ access to security resource profile CIMSERV CL(WBEM)
            Logger::put_l(Logger::TRACE_LOG, ZOS_SECURITY_NAME, Logger::WARNING,
                "Security.Authentication.SecureBasicAuthenticator"
                    ".NOREAD_CIMSERV_ACCESS.PEGASUS_OS_ZOS",
                "Request UserID $0 misses READ permission "
                    "to profile CIMSERV CL(WBEM).",userName);
        }
#else
        authenticated = true;
#endif
    }
    else
    {
        authenticated = false;
        PEG_TRACE((TRC_AUTHENTICATION, Tracer::LEVEL2,
            "UserID %s not authenticated: %s (errno %d, reason code 0x%08X)",
             (const char*) userName.getCString(),
              strerror(errno),errno,__errno2()));
    }
#else
    //
    // Check if the user is a valid system user
    //
    if ( !System::isSystemUser( userName.getCString() ) )
    {
        PEG_METHOD_EXIT();
        return (authenticated);
    }

    try
    {
        //
        // Verify the CIM user password
        //
        if (_userManager->verifyCIMUserPassword( userName, password ))
        {
            authenticated = true;
        }
    }
    catch (InvalidUser& )
    {
        PEG_METHOD_EXIT();
        return (authenticated);
    }
    catch (Exception& e)
    {
        PEG_METHOD_EXIT();
        throw e;
    }
#endif // PEGASUS_OS_ZOS
#endif

    PEG_METHOD_EXIT();

    return (authenticated);
}

Boolean SecureBasicAuthenticator::validateUser(const String& userName)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureBasicAuthenticator::validateUser()");

    Boolean authenticated = false;

    if ( System::isSystemUser(userName.getCString())
         && _userManager->verifyCIMUser(userName))
    {
        authenticated = true;
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

#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
# if (__TARGET_LIB__ < 0x410A0000)
// This function is only needed if the compile target system 
// is z/OS R9 or earlier and 32 Bit !
Boolean SecureBasicAuthenticator::set_ZOS_ApplicationID( void )
{

    Boolean applIDset = false;

    char* psa  = 0;
    char* tcb  = *(char **)(psa  + 0x21C); // pas  + PSATOLD  
    char* stcb = *(char **)(tcb  + 0x138); // tcb  + TCBSTCB 
    char* otcb = *(char **)(stcb + 0x0D8); // stcb + STCBOTCB 
    char* thli = *(char **)(otcb + 0x0BC); // otcb + OTCBTHLI 

    // if I find the BPXYTHILI eye catcher everything went fine.
    if ( memcmp(ZOS_PEGASUS_THLI_EYE_CATCHER,thli,4) != 0 )
    {

        PEG_TRACE_CSTRING(TRC_AUTHENTICATION, Tracer::LEVEL2,
            "Could not set application ID CFZAPPL. "
            " BPXYTHLI control block not found.");

        applIDset = false;
    } else
    {
        // The size of applId: BPXYTHLI.THLIAPPLIDLEN 
        char* thliApplIDLen = (char *)(thli + 0x052);  
        // The applId: BPXYTHLI.THLIAPPLID  
        char* thliApplID = (char *)(thli + 0x070);   

        // The APPLID definition contains a padding space: + 1 
        memcpy(thliApplID,ZOS_PEGASUS_APPLID,ZOS_PEGASUS_APPLID_LEN+1);
        *thliApplIDLen = ZOS_PEGASUS_APPLID_LEN;

        applIDset = true;
    }

    return(applIDset);
}
#endif // end __TARGET_LIB__
#endif // end PEGASUS_PLATFORM_ZOS_ZSERIES_IBM


PEGASUS_NAMESPACE_END
