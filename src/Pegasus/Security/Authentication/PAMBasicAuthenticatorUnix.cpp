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
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//            : Sushma Fernandes, Hewlett-Packard Company 
//                (sushma_fernandes@hp.com)
//                Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////

// NOCHKSRC

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Signal.h>

#if defined (PEGASUS_OS_HPUX)
#include <prot.h>
#endif

#if defined (PEGASUS_USE_PAM_STANDALONE_PROC)
#include <Pegasus/Common/Logger.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#if defined(PEGASUS_HAS_SIGNALS)
# include <sys/wait.h>
#endif
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
        PEG_TRACE_CSTRING(TRC_AUTHENTICATION, Tracer::LEVEL4,
           "Authentication Mutex lock.");
        AutoMutex lock(_authSerializeMutex);
        authenticated = _pamBasicAuthenticatorStandAlone.authenticate(
            userName, password);
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
//    PEG_TRACE((TRC_AUTHENTICATION, Tracer::LEVEL4,
//       "PAMBasicAuthenticator::_authenticateByPAM() - userName = %s; userPassword = %s",
//       (const char *)userName.getCString(), (const char *)password.getCString()));

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
       PEG_TRACE_CSTRING(TRC_AUTHENTICATION, Tracer::LEVEL4,
         "pam_authenticate successful.");
        //
        //Call pam_acct_mgmt, to check if the user account is valid. This includes 
        //checking for password and account expiration, as well as verifying access 
        //hour restrictions.
        //
        if ( ( pam_acct_mgmt(phandle, 0) ) == PAM_SUCCESS ) 
        {
           PEG_TRACE_CSTRING(TRC_AUTHENTICATION, Tracer::LEVEL4,
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

Boolean PAMBasicAuthenticator::validateUser(const String& userName)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::validateUser()");

    Boolean authenticated = false;

#if !defined(PEGASUS_USE_PAM_STANDALONE_PROC)
    struct pam_conv pconv;
    pam_handle_t *phandle;
    char *name;
    APP_DATA mydata;

    const char *service = "wbem";
    pconv.conv = PAMBasicAuthenticator::pamValidateUserCallback;
    pconv.appdata_ptr = &mydata;

    //
    // Call pam_start since you need to before making any other PAM calls
    //
    if ( pam_start(service,
     (const char *)userName.getCString(), &pconv, &phandle) != PAM_SUCCESS)
    {
        PEG_METHOD_EXIT();
        return (authenticated);
    }

    //
    // Call pam_acct_mgmt, to check if the user account is valid. This includes
    // checking for account expiration, as well as verifying access
    // hour restrictions.
    //
    if ( pam_acct_mgmt(phandle, 0) == PAM_SUCCESS )
    {
        authenticated = true;
    }

    //
    //Call pam_end to end our PAM work
    //
    pam_end(phandle, 0);

#else
    //
    // Mutex to Serialize Authentication calls.
    //
    PEG_TRACE_CSTRING(TRC_AUTHENTICATION, Tracer::LEVEL4,
           "Authentication Mutex lock.");
    AutoMutex lock(_authSerializeMutex);
    authenticated = _pamBasicAuthenticatorStandAlone.validateUser(
            userName);
#endif

    PEG_METHOD_EXIT();
    return (authenticated);

}

Sint32 PAMBasicAuthenticator::pamValidateUserCallback( Sint32 num_msg,
#if defined (PEGASUS_OS_LINUX)
        const struct pam_message **msg,
#else
        struct pam_message **msg,
#endif
        struct pam_response **resp,
        void *appdata_ptr)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::pamValidateUserCallback()");

    //
    // Allocate the response buffers
    //
    if ( num_msg > 0 )
    {
        //
        // Since resp->resp needs to be initialized in all possible scenarios,
        // use calloc for memory allocation.
        //
        *resp =
          (struct pam_response *)calloc(num_msg, sizeof(struct pam_response));

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

    PEG_METHOD_EXIT();
    return PAM_SUCCESS;
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
                strncpy(resp[i]->resp, mydata->userPassword, PAM_MAX_MSG_SIZE);
                resp[i]->resp[PAM_MAX_MSG_SIZE - 1] = 0;
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

const String PAMBasicAuthenticatorStandAlone::PAM_OPERATION_SUCCESS = "T";

const String PAMBasicAuthenticatorStandAlone::OPERATION_PAM_AUTHENTICATION = "A";

const String PAMBasicAuthenticatorStandAlone::OPERATION_PAM_ACCT_MGMT = "M";

/* constructor. */
PAMBasicAuthenticatorStandAlone::PAMBasicAuthenticatorStandAlone()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticatorStandAlone::PAMBasicAuthenticatorStandAlone()");

#if defined(PEGASUS_HAS_SIGNALS)
    _pid = -1;
#endif

    _createPAMStandalone();

    SignalHandler::ignore(PEGASUS_SIGPIPE);

    PEG_METHOD_EXIT();
}

/* destructor. */
PAMBasicAuthenticatorStandAlone::~PAMBasicAuthenticatorStandAlone()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticatorStandAlone::~PAMBasicAuthenticatorStandAlone()");

#if defined(PEGASUS_HAS_SIGNALS)
    // Harvest the status of the previous standalone process, if any
    if (_pid != -1)
    {
        waitpid(_pid, 0, WNOHANG);
        _pid = -1;
    }
#endif

    PEG_METHOD_EXIT();
}

Boolean PAMBasicAuthenticatorStandAlone::authenticate(
    const String& userName,
    const String& password)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticatorStandAlone::authenticate()");

    Boolean authenticated = false;

    // Send over the username ...
    if (continue_PAMauthentication)
    {
        //
        // On a broken connection, try restarting the authentication process 
        // and resend username.
        //
        if (_writeString (userName) == 
                PAMBasicAuthenticatorStandAlone::BROKEN_CONNECTION)
        {
            _restartProcess();
            _writeString (userName);
        }
    }

    // Send over the password ...
    if (continue_PAMauthentication)
    {
        _writeString (password);
    }

    // Send over the operation code.
    if (continue_PAMauthentication)
    {
        _writeString (OPERATION_PAM_AUTHENTICATION);
    }

    // Now read back the PAM Authentication status value (T/F)
    if (continue_PAMauthentication)
    {
        if (_readString() == PAM_OPERATION_SUCCESS)
        {
            authenticated = true;
            printed_err_since_success = false;
        }
    }

    PEG_METHOD_EXIT();
    return (authenticated);
}

Boolean PAMBasicAuthenticatorStandAlone::validateUser(
    const String& userName)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticatorStandAlone::validateUser()");

    Boolean authenticated = false;

    // Send over the username ...
    if (continue_PAMauthentication)
    {
        //
        // On a broken connection, try restarting the authentication process 
        // and resend username.
        //
        if (_writeString (userName) == 
                PAMBasicAuthenticatorStandAlone::BROKEN_CONNECTION)
        {
            _restartProcess();
            _writeString(userName);
        }
    }

    // Send over the password ...
    if (continue_PAMauthentication)
    {
        _writeString(String::EMPTY);
    }

    // Send over the operation code.
    if (continue_PAMauthentication)
    {
        _writeString (OPERATION_PAM_ACCT_MGMT);
    }

    // Now read back the PAM Authentication status value (T/F)
    if (continue_PAMauthentication)
    {
        if (_readString() == PAM_OPERATION_SUCCESS)
        {
            authenticated = true;
            printed_err_since_success = false;
        }
    }

    PEG_METHOD_EXIT();
    return (authenticated);
}

PAMBasicAuthenticatorStandAlone::_Status 
         PAMBasicAuthenticatorStandAlone::_writeString(const String& text)
{
    PAMBasicAuthenticatorStandAlone::_Status status = 
        PAMBasicAuthenticatorStandAlone::SUCCESS;

    CString copy_of_text=text.getCString();
    int stringLength = strlen(copy_of_text);

    if (stringLength > BUFFERLEN - 1)
    { 
        stringLength = BUFFERLEN - 1;
    }

    char header[5];
    sprintf(header, "%04u", stringLength);

    continue_PAMauthentication = true;

    if ((write(fd_1[1], header, 4) != 4) ||
        (write(fd_1[1], copy_of_text, stringLength) != stringLength))
    {
        continue_PAMauthentication = false;
        status = PAMBasicAuthenticatorStandAlone::OTHER_ERROR;

        if (errno == EPIPE)
        {
            status = PAMBasicAuthenticatorStandAlone::BROKEN_CONNECTION;
        }
        if (printed_err_since_success == false)
        {
            printed_err_since_success = true;
            Logger::put(Logger::ERROR_LOG, System::CIMSERVER, 
                        Logger::SEVERE,
                       "Error processing PAM Authentication request (write).");
        }
    }

    return status;
}


void PAMBasicAuthenticatorStandAlone::_restartProcess(void)
{
    _createPAMStandalone();
}

String PAMBasicAuthenticatorStandAlone::_readString()
{
    char authReply[10]; 

    authReply[0] = '\0';
    int n = read(fd_2[0], authReply, 2);  /* read back the reply */

    if (n < 0)
    {
        continue_PAMauthentication = false;
        if (printed_err_since_success == false)
        {
            printed_err_since_success = true;
            //L10N TODO
            Logger::put(Logger::ERROR_LOG, System::CIMSERVER,
              Logger::SEVERE,
             "Error processing PAM Authentication request (read).");
        }
    }
    else
    {
        authReply[n] = '\0';
    }

    return (String(authReply));
}

void PAMBasicAuthenticatorStandAlone::_createPAMStandalone()
{
    pid_t   pid;

#if defined(PEGASUS_HAS_SIGNALS)
    // Harvest the status of the previous standalone process, if any
    if (_pid != -1)
    {
        waitpid(_pid, 0, WNOHANG);
        _pid = -1;
    }
#endif

    continue_PAMauthentication = true;
    if (pipe(fd_1) < 0)   // Pipe to write to authentication proc
    {
        continue_PAMauthentication = false;
        if (printed_err_since_success == false)
        {
            printed_err_since_success = true;
            //L10N TODO
            Logger::put(Logger::ERROR_LOG, System::CIMSERVER,
                  Logger::SEVERE,
                  "Error processing PAM Authentication request (pipe).");
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
                Logger::put(Logger::ERROR_LOG, System::CIMSERVER,
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
                Logger::put(Logger::ERROR_LOG, System::CIMSERVER,
                      Logger::SEVERE,
                      "Error processing PAM Authentication request (fork).");
            }
        }
        else if (pid > 0)       // This is the PARENT side of the fork
        {
            close(fd_1[0]);     // close read end on 1st pipe
            close(fd_2[1]);     // close write end on 2nd pipe
#if defined(PEGASUS_HAS_SIGNALS)
            _pid = pid;
#endif
        }
        else                     // This is the CHILD side of the fork
        {

            // Direct standard error to /dev/null,
            // since we are running as a daemon.
            close(2);
            open("/dev/null", O_RDWR);

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
                        Logger::put(Logger::ERROR_LOG, System::CIMSERVER,
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
                            Logger::put(Logger::ERROR_LOG, System::CIMSERVER,
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
                            Logger::put(Logger::ERROR_LOG, System::CIMSERVER,
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

