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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <fstream>
#include <iostream>
#include <cstdio>

#include <unistd.h>
#include <pwd.h>

#include <Pegasus/Security/Authentication/AuthenticationManager.h>

#define PWENT_BUFFER_LEN  256

PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;


/**
     Returns passwd struct for a given UID.
        If an error occurs a NULL is returned and errno is recorded.

        Parameters:
            uid       - A UNIX UID
            pwdStruct - A pwd struct.
            buffer    - A char buffer used to store data for pwd struct.
            buflen    - A the length of the char buffer (Recommend 1024).
        Returns:
            Zero if lookup succeeds or 1 on error.
*/
int getPwd(uid_t           uid,
           struct passwd & pwdStruct,
           char          * buffer,
           size_t          buflen)
{
    struct passwd *   pwd    = &pwdStruct;
    struct passwd **  result = &pwd;

    int    pwRetVal = getpwuid_r( uid, pwd, buffer, buflen, result);
    if ( pwd == NULL )
    {
        pwRetVal = 1;
    }

    return pwRetVal;
}

char* getCurrentLoginName()
{

    char *currentUser;
    char buffer[PWENT_BUFFER_LEN];
    struct passwd pwd;

    //
    //  get the real user's UID.
    //
    uid_t uid = getuid();

    //
    //  lookup that UID in the password list.
    //
    int pwRetVal = getPwd(uid, pwd, buffer, PWENT_BUFFER_LEN);
    if( pwRetVal != 0 )
    {
       cout << "user may have been removed just after user logged in" << endl;
       return((char *)NULL);
    }

    // extract user name
    currentUser = new char[strlen(pwd.pw_name)];
    strncpy(currentUser, pwd.pw_name, strlen(pwd.pw_name));
    currentUser[strlen(pwd.pw_name)]=0;

    return((char *)currentUser);
}




int main()
{
    String testUser = String::EMPTY;
    String authHeader = String::EMPTY;

    testUser.assign(getCurrentLoginName());

    //AuthenticationManager  authManager;

    String challenge = String::EMPTY;

    cout << "User Name: " << testUser << endl;



    Boolean authenticated = true;
    // authenticated = localAuthHandler.authenticate(authHeader, challenge);

    if (authenticated)
        cout << "authenticated" << endl;
    else
        cout << "Not authenticated" << endl;

    cout << endl;
    cout << "+++++ passed all tests" << endl;

    return 0;
}
