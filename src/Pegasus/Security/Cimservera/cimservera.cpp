//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Ben Heilbronn, Hewlett-Packard Company(ben_heilbronn@hp.com)
//
//         Parts of this code originated within PAMBasicAuthenticator.
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#include "cimservera.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#include <security/pam_appl.h>

#define BUFFERLEN    1024
#define HEADER_LEN      4

typedef struct
{
    CString userPassword;
} APP_DATA;

/* constructor. */
Cimservera::Cimservera() 
{ 
}

/* destructor. */
Cimservera::~Cimservera() 
{ 
}

Boolean Cimservera::authenticateByPAM(
    const String& userName, 
    const String& password) 
{
    Boolean authenticated = false;
    struct pam_conv pconv;
    pam_handle_t *phandle;
    char *name;
    APP_DATA mydata;

    const char *service = "wbem";

    //
    // Store the password for PAM authentication
    //
    mydata.userPassword = password.getCString();

    pconv.conv = Cimservera::PAMCallback;
    pconv.appdata_ptr = &mydata;

    //
    //Call pam_start since you need to before making any other PAM calls
    //
    if ( ( pam_start(service, 
        (const char *)userName.getCString(), &pconv, &phandle) ) != PAM_SUCCESS ) 
    {
        return (authenticated);
    }

    //
    //Call pam_authenticate to authenticate the user
    //
    if ( ( pam_authenticate(phandle, 0) ) == PAM_SUCCESS ) 
    {
        //
        //Call pam_acct_mgmt, to check if the user account is valid. This includes 
        //checking for password and account expiration, as well as verifying access 
        //hour restrictions.
        //
        if ( ( pam_acct_mgmt(phandle, 0) ) == PAM_SUCCESS ) 
        {
            authenticated = true;
        }
    }

    //
    //Call pam_end to end our PAM work
    //
    pam_end(phandle, 0);

    return (authenticated);
}

#if defined PEGASUS_OS_LINUX
Sint32 Cimservera::PAMCallback(Sint32 num_msg, const struct pam_message **msg,
        struct pam_response **resp, void *appdata_ptr)
#else
Sint32 Cimservera::PAMCallback(Sint32 num_msg, struct pam_message **msg,
        struct pam_response **resp, void *appdata_ptr)
#endif
{
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
            return PAM_BUF_ERR;
        }
    } 
    else 
    {
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
               return PAM_CONV_ERR;
        }
    }

    return PAM_SUCCESS;
}

PEGASUS_NAMESPACE_END;
PEGASUS_USING_PEGASUS;

int main(int argc, char* argv[])
{
    int  n, len;
    char username[BUFFERLEN], password[BUFFERLEN];
    char * _username = &username[0];
    char * _password = &password[0];
    char username_len[10], password_len[10];
    char return_buf[10];

    typedef struct
    {
        CString userPassword;
    } APP_DATA;

    Boolean authenticated;

    // STDIN and STDOUT are used on the back-end for reading and writing

   for (;;) {                        // Just loop on username/passwords

     authenticated = false;

     // Read in the username

     len = read(STDIN_FILENO, username_len, HEADER_LEN);
     if (len != HEADER_LEN) {
        break;
     }
     username_len[HEADER_LEN] = '\0';
     n = atoi(username_len);
     read(STDIN_FILENO, username, n);
     username[n] = '\0';

     // Read in the password

     len = read(STDIN_FILENO, password_len, HEADER_LEN);
     if (len != HEADER_LEN) {
        break;
     }
     password_len[HEADER_LEN] = '\0';
     n = atoi(password_len);
     read(STDIN_FILENO, password, n);
     password[n] = '\0';

     authenticated = Cimservera::authenticateByPAM(_username, _password);

     if (authenticated) {
         strncpy(return_buf, "T", 1);
     } else {
         strncpy(return_buf, "F", 1);
     }
     write(STDOUT_FILENO, return_buf, 1);
   }
}
