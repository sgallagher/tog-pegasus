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

#include <security/pam_appl.h>

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Config/ConfigManager.h>

#include "PAMBasicAuthenticator.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


/**
    Constant representing the Basic authentication challenge header.
*/
static const String BASIC_CHALLENGE_HEADER = "WWW-Authenticate: Basic \"";


/** Service name for pam_start */
const char *service = "wbem";

char* userPassword = 0;

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

    String port = configManager->getCurrentValue("port");

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

    struct pam_conv pconv;
    pam_handle_t *phandle;
    char *name;

    pconv.conv = PAMBasicAuthenticator::PAMCallback;
    pconv.appdata_ptr = NULL;

    Boolean authenticated = false;

    ArrayDestroyer<char> p(password.allocateCString());
    userPassword = p.getPointer();

    ArrayDestroyer<char> user(userName.allocateCString());

    //
    //call pam_start since you need to before making any other PAM calls
    //
    if ( ( pam_start(service, 
        (const char *)user.getPointer(), &pconv, &phandle) ) != PAM_SUCCESS ) 
    {
        userPassword = 0;
        PEG_METHOD_EXIT();
        return (authenticated);
    }

    //
    //Call pam_authenticate to authenticate the user
    //
    if ( ( pam_authenticate(phandle, 0) ) != PAM_SUCCESS ) 
    {
        userPassword = 0;
        PEG_METHOD_EXIT();
        return (authenticated);
    }
    else
    {
        authenticated = true;
    }

    //
    //Call pam_end to end our PAM work
    //
    if ( ( pam_end(phandle, 0) ) != PAM_SUCCESS ) 
    {
        userPassword = 0;
        PEG_METHOD_EXIT();
        return (authenticated);
    }

    userPassword = 0;
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

Sint32 PAMBasicAuthenticator::PAMCallback(Sint32 num_msg, struct pam_message **msg,
        struct pam_response **resp, void *appdata_ptr)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::PAMCallback()");
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

    Uint32 i = 0;
    Uint32 len = 0;

    for ( i = 0; i < num_msg; i++ ) 
    {
        len = strlen(msg[i]->msg);
        switch ( msg[i]->msg_style ) 
        {
            case PAM_PROMPT_ECHO_OFF:
                // 
                // copy the user password
                // 
                resp[i]->resp = (char *)malloc(PAM_MAX_MSG_SIZE);
                strcpy(resp[i]->resp, userPassword);

                resp[i]->resp_retcode = 0;
                break;

            default:
                PEG_METHOD_EXIT();
                return PAM_CONV_ERR;

#ifdef NOT_REQUIRED
// ATTN-NB-02-20000412: Add code to do the user policy management. 
// Keeping the following code here becuase I think this will be useful
// in implementing user account policy management.
            case PAM_PROMPT_ECHO_ON:
                //
                // Remove any trailing \n from prompt
                //
                if (msg[i]->msg[len-1] == '\n')
                    msg[i]->msg[len-1] = 0;
                printf("%s", msg[i]->msg);

                //
                // Get the response from the user
                //
                resp[i]->resp = (char *)malloc(PAM_MAX_MSG_SIZE);
                fgets(resp[i]->resp, PAM_MAX_MSG_SIZE, stdin);

                //
                // Remove trailing \n from input
                //
                len = strlen(resp[i]->resp);
                if (resp[i]->resp[len-1] == '\n')
                        resp[i]->resp[len-1] = 0;
                resp[i]->resp_retcode = 0;
                break;

            case PAM_ERROR_MSG:
                //
                // Take into account that we may need to add a \n to the line
                //
                if (msg[i]->msg[len-1] == '\n') 
                {
                    printf("%s", msg[i]->msg);
                } 
                else 
                {
                    printf("%s\n", msg[i]->msg);
                }
                break;

            case PAM_TEXT_INFO:
                //
                // Take into account that we may need to add a \n to the line
                //
                if (msg[i]->msg[len-1] == '\n') 
                {
                    printf("%s", msg[i]->msg);
                } 
                else 
                {
                    printf("%s\n", msg[i]->msg);
                }
                break;

            default:
                PEG_METHOD_EXIT();
                return PAM_CONV_ERR;
#endif

        }
    }

    PEG_METHOD_EXIT();

    return PAM_SUCCESS;
}

PEGASUS_NAMESPACE_END
