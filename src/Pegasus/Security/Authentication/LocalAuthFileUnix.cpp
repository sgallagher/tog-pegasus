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
//                Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include "LocalAuthFile.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//
// A unique sequence number starts with  this.
//
static Uint32 sequenceCount = 212;

//
// File path used to create random file
//
const char filepath[]="/tmp/cimclient";


LocalAuthFile::LocalAuthFile(String userName)
{
    _user = userName.allocateCString();
    _user[strlen(_user)] = 0;

    _challenge = String::EMPTY;
    _filePathName = String::EMPTY;

    srandom(100);
}

LocalAuthFile::~LocalAuthFile() 
{ 

}

//
// create a file and make a random token data entry to it.
// send the file name back to caller
//
String LocalAuthFile::create()
{
    char* extension = 0;
    ofstream outfs;
    char* fileName = 0;
    String filePath = String::EMPTY;

    //
    // File name will be /tmp/cimclient_user_nnn
    //

    Uint32 secs, milliSecs;
    System::getCurrentTime(secs, milliSecs);

    //
    // extension size is username plus the sequence count
    //
    extension = new char[strlen(_user) + 4];
    sequenceCount++;
    sprintf(extension,"_%s_%d", _user, sequenceCount + milliSecs);
    extension[strlen(extension)]=0;

    fileName = strcpy(new char[strlen(filepath) + 
                            strlen(extension) + 1], filepath);
    strcat(fileName, extension);

    // create a file name with the name of the user
    outfs.open(fileName, ios::in, S_IRUSR | S_IWUSR);
    if ( !outfs )
    {
        // unable to create file
        cout << "unable to create file: " << fileName << endl;
        return(filePath);
    }

    String randomToken = _generateRandomTokenString();
    outfs << randomToken;

    outfs.close();
    _challenge.assign(randomToken);

    //change owner
    if (!_changeFileOwner(fileName))
    {
        return(filePath);
    }

    _filePathName.assign(fileName);

    cout << "File Path: " << _filePathName << endl;

    // return the name of the file
    return(_filePathName);
}


//
//  Removes the file that was created
//
Boolean LocalAuthFile::remove()
{
    // remove the file
    return(System::removeFile(_filePathName.allocateCString()));
}

//
// Get the string that was created as a challenge string
//
String LocalAuthFile::getChallengeString()
{
    return(_challenge);
}

//
// changes the file owner to one specified
//
Boolean LocalAuthFile::_changeFileOwner(char* fileName)
{
    struct    passwd*        userPasswd;
    struct    passwd        pwd;
    struct    passwd*        result;
    char    pwdBuffer[MAX_PWD_BUFFER_SIZE];

    Uint32    intUid;
    Uint32    intGid;

#ifdef  _REENTRANT
    
    //
    // This requires -D_REENTRANT flag to makefile
    //
    if (getpwnam_r (_user, &pwd, pwdBuffer, MAX_PWD_BUFFER_SIZE, &result) == 0)
    {
        // ATTN: Log message
        //printf ("Name = %s; uid = %d\n", pwd.pw_name, pwd.pw_uid);
    }
    else
    {
      // ATTN: Log getpwnam failed message
      return (false);
    }
     
    intUid = pwd.pw_uid;
    intGid = pwd.pw_gid;

#else
    userPasswd = getpwnam(_user);
    if ( userPasswd  == NULL)
    {
      // ATTN: Log getpwnam failed message
      return (false);
    }
    
    intUid = userPasswd->pw_uid;
    intGid = userPasswd->pw_gid;

#endif
     
    //
    //SYNTAX:
    //int chown(const char *path, uid_t owner, gid_t group);
    //
    Uint32 ret = chown(fileName, intUid, intGid);
    if ( ret == -1)
    {
      return (false);
    }
   
    //
    //SYNTAX:
    //  int chmod(const char *path, mode_t mode);
    //   where S_IRUSR - read only by user
    //
    Uint32 retn = chmod(fileName, S_IRUSR);
    if ( retn == -1)
    {
      return (false);
    }

    return (true);
}

//
// Generate  random token string
//
String LocalAuthFile::_generateRandomTokenString()
{
    String randomToken = String::EMPTY;
    char token[MAX_BUFFER_SIZE];
    Uint32 seconds, milliseconds;

    System::getCurrentTime(seconds, milliseconds);

    // generate a random token
    char randnum[] = { '0' + (random() % 10), '0' + (random() % 10), '\0' };
    long randomNum = atol(randnum);

    sprintf (token,"%s%ld%d", _user, randomNum, seconds + milliseconds );
    randomToken.assign(token);

    return (randomToken);
}

PEGASUS_NAMESPACE_END
