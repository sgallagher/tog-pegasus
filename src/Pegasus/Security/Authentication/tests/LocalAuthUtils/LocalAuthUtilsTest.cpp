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
// Author: Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
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

#include <Pegasus/Security/Authentication/LocalAuthUtils.h>

#define PWENT_BUFFER_LEN 256

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

    cout << "getPwd entered" << endl;

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

    cout << "getCurrentLoginName Entered" << endl;
    //
    //  get the real user's UID.
    //
    uid_t uid = getuid();
    cout << "current user UID:" << uid << endl;


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
    cout << "current user Name:" << pwd.pw_name<< endl;
    currentUser = new char[strlen(pwd.pw_name)];
    strncpy(currentUser, pwd.pw_name, strlen(pwd.pw_name));
    currentUser[strlen(pwd.pw_name)]=0;

    cout << "returning user as: "<< currentUser << endl;
    return((char *)currentUser);
}




int main()
{
    //String path;
    char thisFileName[256];
   
   /*
   if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " library_path" << endl;
        exit(1);
    }
   */


    // create a file with random number
    cout << "Test 1: Started" << endl;

    char *testUser = getCurrentLoginName();

    LocalAuthUtils localAuthFile;
    char * fileName = localAuthFile.createFile((char *) testUser);
    fileName[strlen(fileName)]=0;
    strcpy(thisFileName, fileName);
    thisFileName[strlen(fileName)]=0;
    cout << "File Name :" << fileName << endl;
    assert(fileName != NULL);

    cout << "....Test1 Done" << endl;
    cout << " ";
    cout << "Test 2: Started" << endl;
 
    bool isValid = localAuthFile.compareContents((char*)"Test");

    // Now we will delete file
    bool isdelete = localAuthFile.deleteFile();
    assert(isdelete != false);
    cout << "....Test2 Done" << endl;

    cout << " ";
    cout << "+++++ passed all tests" << endl;

    return 0;
}
