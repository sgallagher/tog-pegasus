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
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//            : Sushma Fernandes, Hewlett-Packard Company 
//                (sushma_fernandes@hp.com)
//                Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/FileSystem.h>

#if defined (PEGASUS_OS_HPUX)
#include <prot.h>
#endif

#if defined (PEGASUS_USE_PAM_STANDALONE_PROC)
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/IPC.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif


#include "PAMBasicAuthenticator.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#include <security/pam_appl.h>

#if defined(PEGASUS_OS_LSB)
#ifndef PAM_MAX_MSG_SIZE
#define PAM_MAX_MSG_SIZE      512
#endif
#endif

#define BUFFERLEN 1024

/**
    Constant representing the Basic authentication challenge header.
*/
static const String BASIC_CHALLENGE_HEADER = "WWW-Authenticate: Basic \"";

Mutex PAMBasicAuthenticator::_authSerializeMutex;

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

    //
    // Check for platforms that allow PAM Standalone Process
    //
#if defined(PEGASUS_USE_PAM_STANDALONE_PROC)
    //
    // Set up the separate process to do PAM Authentication
    //
    _pamBasicAuthenticatorStandAlone = 
            new PAMBasicAuthenticatorStandAlone();
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

#if !defined(PEGASUS_USE_PAM_STANDALONE_PROC)
        authenticated = _authenticateByPAM(userName, password);
#else
        //
        // Mutex to Serialize Authentication calls.
        //
        Tracer::trace(TRC_AUTHENTICATION, Tracer::LEVEL4,
           "Authentication Mutex lock.");
        AutoMutex lock(_authSerializeMutex);
        authenticated =
                _pamBasicAuthenticatorStandAlone->authenticate(userName,
                                                              password);
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
         "pam_authenticate successful.");
        //
        //Call pam_acct_mgmt, to check if the user account is valid. This includes 
        //checking for password and account expiration, as well as verifying access 
        //hour restrictions.
        //
        if ( ( pam_acct_mgmt(phandle, 0) ) == PAM_SUCCESS ) 
        {
           Tracer::trace(TRC_AUTHENTICATION, Tracer::LEVEL4,
              "pam_acct_mgmt successful.");
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
        // 
        // Since resp->resp needs to be initialized in all possible scenarios,
        // use calloc for memory allocation. 
        //
        *resp = (struct pam_response *)calloc(num_msg, sizeof(struct pam_response));

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

/** Routines to access PAM Authentication via a standalone process **/

#if defined(PEGASUS_USE_PAM_STANDALONE_PROC)

int     fd_1[2], fd_2[2];
Boolean continue_PAMauthentication;
Boolean printed_err_since_success=false;

/* constructor. */
PAMBasicAuthenticatorStandAlone::PAMBasicAuthenticatorStandAlone()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticatorStandAlone::PAMBasicAuthenticatorStandAlone()");

    _createPAMStandalone();

    PEG_METHOD_EXIT();
}

/* destructor. */
PAMBasicAuthenticatorStandAlone::~PAMBasicAuthenticatorStandAlone()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticatorStandAlone::~PAMBasicAuthenticatorStandAlone()");

    PEG_METHOD_EXIT();
}

Boolean PAMBasicAuthenticatorStandAlone::authenticate(
    const String& userName,
    const String& password)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticatorStandAlone::authenticate()");

    Boolean authenticated;
    char    auth_reply[10];
    char    line[BUFFERLEN];
    int     n, ret_code;

    try
    {
            // Callout to stand-alone process replaces above call...

            // Send over the username ...
            CString copy_of_userName=userName.getCString();
            n = strlen(copy_of_userName);
            sprintf(line, "%4u%s", n, (const char*)copy_of_userName);
            n = strlen(line);
            ret_code = write(fd_1[1], line, n);
            if (ret_code != n)
            {
                continue_PAMauthentication = false;
                if (printed_err_since_success == false)
                {
                    printed_err_since_success = true;
                    Logger::put(Logger::ERROR_LOG, "CIMServer",
                       Logger::SEVERE,
                       "Error processing PAM Authentication request (write).");
                }
                //
                // on EPIPE, try restarting the authentication process
                //
                if (errno == EPIPE)
                {
                  close(fd_1[1]);   // Close to keep used fd number down
                  close(fd_2[0]);   // Close to keep used fd number down
                  _createPAMStandalone();
                  ret_code = write(fd_1[1], line, n);
                  if (ret_code != n)
                  {
                    continue_PAMauthentication = false;
                    if (printed_err_since_success == false)
                    {
                      printed_err_since_success = true;
                      //L10N TODO
                      Logger::put(Logger::ERROR_LOG, "CIMServer",
                        Logger::SEVERE,
                        "Error processing PAM Authentication request (write).");
                    }
                  }
                }  //  if (errno == EPIPE)
            } // if (ret_code != n) from 1st write

            // Send over the password ... */
            if (continue_PAMauthentication)
            {
                CString copy_of_password = password.getCString();
                n = strlen(copy_of_password);
                sprintf(line, "%4u%s", n, (const char*)copy_of_password);
                n = strlen(line);
                if (write(fd_1[1], line, n) != n) {
                    continue_PAMauthentication = false;
                    if (printed_err_since_success == false)
                    {
                      printed_err_since_success = true;
                      //L10N TODO
                      Logger::put(Logger::ERROR_LOG, "CIMServer",
                        Logger::SEVERE,
                        "Error processing PAM Authentication request (write).");
                    }
                }
            }

            // Now read back the PAM Authentication status value (T/F)
            if (continue_PAMauthentication)
            {
                n = read(fd_2[0], auth_reply, 2);  /* read back the reply */

                if (n < 0)
                {
                    continue_PAMauthentication = false;
                    if (printed_err_since_success == false)
                    {
                      printed_err_since_success = true;
                      //L10N TODO
                      Logger::put(Logger::ERROR_LOG, "CIMServer",
                        Logger::SEVERE,
                        "Error processing PAM Authentication request (read).");
                    }
                }
                else
                {
                    auth_reply[n] = '\0';
                }
            }

            authenticated = false;
            if ((continue_PAMauthentication) && (auth_reply[0] == 'T'))
            {
                authenticated = true;
                printed_err_since_success = false;
            }
    }
    catch (...)
    {
         throw;
    }

    PEG_METHOD_EXIT();
    return (authenticated);
}


void PAMBasicAuthenticatorStandAlone::_createPAMStandalone()
{
    pid_t   pid;

    continue_PAMauthentication = true;
    if (pipe(fd_1) < 0)   // Pipe to write to authentication proc
    {
        continue_PAMauthentication = false;
        if (printed_err_since_success == false)
        {
            printed_err_since_success = true;
            //L10N TODO
            Logger::put(Logger::ERROR_LOG, "CIMServer",
                  Logger::SEVERE,
                  "Error processing PAM Authtication request (pipe).");
        }
    }
    if (continue_PAMauthentication)
    {
        if (pipe(fd_2) < 0)   // Pipe to read from the authentication proc
        {
            continue_PAMauthentication = false;
            if (printed_err_since_success == false)
            {
                printed_err_since_success = true;
                //L10N TODO
                Logger::put(Logger::ERROR_LOG, "CIMServer",
                      Logger::SEVERE,
                      "Error processing PAM Authentication request (pipe).");
            }
        }
    }
    if (continue_PAMauthentication)
    {
        if ((pid = fork()) < 0)
        {
            continue_PAMauthentication = false;
            if (printed_err_since_success == false)
            {
                printed_err_since_success = true;
                //L10N TODO
                Logger::put(Logger::ERROR_LOG, "CIMServer",
                      Logger::SEVERE,
                      "Error processing PAM Authentication request (fork).");
            }
        }
        else if (pid > 0)       // This is the PARENT side of the fork
        {
            close(fd_1[0]);     // close read end on 1st pipe
            close(fd_2[1]);     // close write end on 2nd pipe
        }
        else                     // This is the CHILD side of the fork
        {
            close(fd_1[1]);      // close write end on 1st pipe
            close(fd_2[0]);      // close read end on 2nd pipe
            if (fd_1[0] != STDIN_FILENO)
            {
                if (dup2(fd_1[0], STDIN_FILENO) == -1)
                {
                    continue_PAMauthentication = false;
                    if (printed_err_since_success == false)
                    {
                        printed_err_since_success = true;
                        //L10N TODO
                        Logger::put(Logger::ERROR_LOG, "CIMServer",
                              Logger::SEVERE,
                              "Error processing PAM Authentication request (dup2).");
                    }
                }
                close(fd_1[0]);  // don't need this after dup2
            }
            if (continue_PAMauthentication)
            {
                if (fd_2[1] != STDOUT_FILENO)
                {
                    if (dup2(fd_2[1], STDOUT_FILENO) == -1)
                    {
                        continue_PAMauthentication = false;
                        if (printed_err_since_success == false)
                        {
                            printed_err_since_success = true;
                            //L10N TODO
                            Logger::put(Logger::ERROR_LOG, "CIMServer",
                                Logger::SEVERE,
                                "Error processing PAM Authentication request (dup2).");
                        }
                    }
                    close(fd_2[1]);   // don't need this after dup2
                }
                if (continue_PAMauthentication)
                {
                    //
                    // Get environment variables:
                    //
                    String certpath = ConfigManager::getHomedPath(
                        PEGASUS_PAM_STANDALONE_PROC_NAME);
                    if (execl((const char*)certpath.getCString(),
                              (const char*)certpath.getCString(), (char*)0) < 0)
                    {
                        continue_PAMauthentication = false;
                        if (printed_err_since_success == false)
                        {
                            printed_err_since_success = true;
                            //L10N TODO
                            Logger::put(Logger::ERROR_LOG, "CIMServer",
                                Logger::SEVERE,
                                "Error creating PAM Authentication process (execl).");
                        }
                        exit(0);
                    }
                }
            }
        }
    }
}

#endif  /* if defined(PEGASUS_OS_HPUX) || ... */





PEGASUS_NAMESPACE_END

