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
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Config/ConfigManager.h>

#ifdef PEGASUS_OS_HPUX
#include <pwd.h>
#include <prot.h>
#endif

#include "PAMBasicAuthenticator.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#include <security/pam_appl.h>

#define BUFFERLEN 1024

/**
    Constant representing the Basic authentication challenge header.
*/
static const String BASIC_CHALLENGE_HEADER = "WWW-Authenticate: Basic \"";


/** Service name for pam_start */
const char *service = "wbem";

typedef struct
{
    CString userPassword;
} APP_DATA;

/* constructor. */
PAMBasicAuthenticator::PAMBasicAuthenticator() 
{ 
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::PAMBasicAuthenticator()");

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

#ifdef PEGASUS_OS_HPUX
    //
    // get the configured usePAMAuthentication flag
    //

    if (String::equal(
        configManager->getCurrentValue("usePAMAuthentication"), "true"))
    {
        _usePAM = true;
    }
    else
    {
	_usePAM = false;
    }
#endif


    PEG_METHOD_EXIT();
}

/* destructor. */
PAMBasicAuthenticator::~PAMBasicAuthenticator() 
{ 
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::~PAMBasicAuthenticator()");

    PEG_METHOD_EXIT();
}

Boolean PAMBasicAuthenticator::authenticate(
    const String& userName, 
    const String& password) 
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::authenticate()");

    Boolean authenticated;

#ifndef PEGASUS_OS_HPUX
    authenticated = _authenticateByPAM(userName, password);
#else
    if (_usePAM)
    {
	authenticated = _authenticateByPAM(userName, password); 
    }
    else
    {
	authenticated = _authenticateByPwnam(userName.getCString(), password);
    }
#endif

    PEG_METHOD_EXIT();
    return (authenticated);
}

Boolean PAMBasicAuthenticator::_authenticateByPAM(
    const String& userName, 
    const String& password) 
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::_authenticateByPAM()");

    Boolean authenticated = false;
    struct pam_conv pconv;
    pam_handle_t *phandle;
    char *name;
    APP_DATA mydata;

    //
    // Store the password for PAM authentication
    //
    mydata.userPassword = password.getCString();

    pconv.conv = PAMBasicAuthenticator::PAMCallback;
    pconv.appdata_ptr = &mydata;

//    WARNING: Should only be uncommented for debugging in a secure environment.
//    Tracer::trace(TRC_AUTHENTICATION, Tracer::LEVEL4,
//       "PAMBasicAuthenticator::_authenticateByPAM() - userName = %s; userPassword = %s",
//       (const char *)userName.getCString(), (const char *)password.getCString());

    //
    //Call pam_start since you need to before making any other PAM calls
    //
    if ( ( pam_start(service, 
        (const char *)userName.getCString(), &pconv, &phandle) ) != PAM_SUCCESS ) 
    {
        PEG_METHOD_EXIT();
        return (authenticated);
    }

    //
    //Call pam_authenticate to authenticate the user
    //
    if ( ( pam_authenticate(phandle, 0) ) == PAM_SUCCESS ) 
    {
       Tracer::trace(TRC_AUTHENTICATION, Tracer::LEVEL4,
         "PAMBasicAuthenticator::_authenticateByPAM() pam_authenticate successful.");
        //
        //Call pam_acct_mgmt, to check if the user account is valid. This includes 
        //checking for password and account expiration, as well as verifying access 
        //hour restrictions.
        //
        if ( ( pam_acct_mgmt(phandle, 0) ) == PAM_SUCCESS ) 
        {
           Tracer::trace(TRC_AUTHENTICATION, Tracer::LEVEL4,
              "PAMBasicAuthenticator::_authenticateByPAM() pam_acct_mgmt successful.");
            authenticated = true;
        }
    }

    //
    //Call pam_end to end our PAM work
    //
    pam_end(phandle, 0);

    PEG_METHOD_EXIT();

    return (authenticated);
}

Boolean PAMBasicAuthenticator::_authenticateByPwnam(
    const char * userName, 
    const String& password) 
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::_authenticateByPwnam()");

    Boolean authenticated = false;

    String currPassword         = String::EMPTY;
    String encryptedPassword    = String::EMPTY;
    String saltStr              = String::EMPTY;

    //
    // check if the system has been converted to a trusted system.
    //

    if (iscomsec())
    {
	// system is a trusted system
	// use interface getprpwnam to get pr_passwd structure

	struct pr_passwd * pwd;

	char* _userName = strcpy(new char[strlen(userName) + 1], userName);
	
	// getprpwnam returns a pointer to a pr_passwd structure  upon success
	if ( (pwd = getprpwnam(_userName)) != NULL)
	{
           Tracer::trace(TRC_AUTHENTICATION, Tracer::LEVEL4,
              "PAMBasicAuthenticator::_authenticateByPwnam() getprpwnam successful.");
	   // get user's password from pr_passwd structure
	    currPassword = pwd->ufld.fd_encrypt; 
	}

	delete [] _userName;
    }
    else
    {
	//
	// system is not a trusted system
	// use reentrant interface getpwnam_r to get password structure
	//
	struct passwd pwd;
	struct passwd *result;
	char pwdBuffer[BUFFERLEN];

	// getpwnam_r returns zero upon success
	if (getpwnam_r(userName, &pwd, pwdBuffer, BUFFERLEN, &result) == 0)
	{
           Tracer::trace(TRC_AUTHENTICATION, Tracer::LEVEL4,
              "PAMBasicAuthenticator::_authenticateByPwnam() getpwnam_r successful.");
	   // get user's password from password file
	    currPassword = pwd.pw_passwd; 
	}
    }

    //
    // Check if the specified password mathches user's password 
    //
    saltStr = currPassword.subString(0,2);

    encryptedPassword = System::encryptPassword(password.getCString(),
			saltStr.getCString());

    if (String::equal(currPassword, encryptedPassword))
    {
	authenticated = true;
    }

    PEG_METHOD_EXIT();

    return (authenticated);
}

//
// Create authentication response header
//
String PAMBasicAuthenticator::getAuthResponseHeader()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::getAuthResponseHeader()");

    // 
    // build response header using realm
    //
    String responseHeader = BASIC_CHALLENGE_HEADER;
    responseHeader.append(_realm);
    responseHeader.append("\"");

    PEG_METHOD_EXIT();

    return (responseHeader);
}

#if defined PEGASUS_OS_LINUX
Sint32 PAMBasicAuthenticator::PAMCallback(Sint32 num_msg, const struct pam_message **msg,
        struct pam_response **resp, void *appdata_ptr)
#else
Sint32 PAMBasicAuthenticator::PAMCallback(Sint32 num_msg, struct pam_message **msg,
        struct pam_response **resp, void *appdata_ptr)
#endif
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::PAMCallback()");

    //
    // Copy the application specific data from the PAM structure.
    //
    APP_DATA *mydata;
    mydata = (APP_DATA *) appdata_ptr;

    // 
    // Allocate the response buffers 
    // 
    if ( num_msg > 0 ) 
    {
        *resp = (struct pam_response *)malloc(sizeof(struct pam_response)*num_msg);

        if ( *resp == NULL ) 
        {
            PEG_METHOD_EXIT();
            return PAM_BUF_ERR;
        }
    } 
    else 
    {
        PEG_METHOD_EXIT();
        return PAM_CONV_ERR;
    }

    for ( Sint32 i = 0; i < num_msg; i++ ) 
    {
        switch ( msg[i]->msg_style ) 
        {
            case PAM_PROMPT_ECHO_OFF:
                // 
                // copy the user password
                // 
                resp[i]->resp = (char *)malloc(PAM_MAX_MSG_SIZE);
                strcpy(resp[i]->resp, mydata->userPassword);
                resp[i]->resp_retcode = 0;
                break;

            default:
                PEG_METHOD_EXIT();
                return PAM_CONV_ERR;
        }
    }

    PEG_METHOD_EXIT();

    return PAM_SUCCESS;
}

PEGASUS_NAMESPACE_END
