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

#include "LocalAuthUtils.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
 *  A unique sequence number starts with  this.
 */
static Uint32 sequenceCount = 212;

/**
 * File path used to create random file
 *
 */
const char LocalAuthUtils::filepath[]="/tmp/cimclient";


LocalAuthUtils::LocalAuthUtils()
{
   _randomToken = 0;
}

LocalAuthUtils::~LocalAuthUtils() { }

/**
 * create a file and make a random token data entry to it.
 * send the file name back to caller
 */
char* LocalAuthUtils::createFile(char* forUser)
{
    char* extension;


    strncpy(_user, forUser, strlen(forUser));
    _user[strlen(forUser)]=0;


    /**
     * File name will be /tmp/cimclient_user_nnn
     *
     */ 

    //
    // extension size is username plus the sequence count
    //
    extension = new char[strlen(forUser) + 4];
    sequenceCount++;
    sprintf(extension,"_%s_%d", forUser, sequenceCount);
    extension[strlen(extension)]=0;

    char* thisFile = strcpy(new char[strlen(filepath) + 
                            strlen(extension) + 1], filepath);
    strcat(thisFile, extension);

    

    Uint32 retCode = _makeEntry(thisFile);
    if ( retCode != 0)
    {
      // Just return null
       delete[] thisFile;
       delete[] extension;
       thisFile = NULL;
       return (thisFile);
    } 

    // store the filename for later use 
    strncpy(_fileName, thisFile, strlen(thisFile) );
    _fileName[strlen(thisFile)]=0;



    // return the name of the file
    return(thisFile);
}

/**
 * Create a random file , generate random token and write
 * to this file
 */
Uint32 LocalAuthUtils::_makeEntry(char* fileName)
{
    ofstream outfs;
    Uint32 retCode = 0;


    // create a file name with the name of the user
    outfs.open(fileName, ios::in);
    if ( !outfs )
    {
      // unable to create file
      // error out
      retCode = 1;
      return(retCode);
    }
    Uint32 retn = chmod(fileName, S_IRUSR | S_IWUSR);
    if ( retn == -1)
    {
      retCode = 1;
      return(retCode);
    }


    // make an Entry
    //outfs << _randomToken;

    _generateRandomTokenString();
    outfs << _randomTokenString;


    outfs.close();

    //change owner
    Uint32  ret = _changeFileOwner(fileName);
    if ( ret != 0)
    {
      retCode = 1;
      return(retCode);
    }

    return(retCode);
}

/**
 *  Method compares received data  with the data
 *  it had generated previously.
 *
 */
Boolean LocalAuthUtils::compareContents(char *recvData)
{
   Boolean validation;

    if  ( strcmp(_randomTokenString,recvData) != 0 )
    {
         return false;
    }
    else
    {
         return true;
    }
}


/**
 * deletes the file that was created
 *
 */ 
Boolean LocalAuthUtils::deleteFile()
{
    // remove the file
    return(System::removeFile(_fileName));
}

/**
 * changes the file owner to one specified
 *
 */ 
Uint32 LocalAuthUtils::_changeFileOwner(char* fileName)
{
    Uint32 retCode = 0;
    struct passwd* userPasswd;
    struct passwd  pwd;
    struct passwd* result;
    char pwdBuffer [MAX_PWD_BUFFER_SIZE];

    Uint32 intUid;
    Uint32 intGid;


#ifdef  _REENTRANT
    
    //
    // int  getpwnam_r(char *name, struct passwd *pwd, char *buffer,
    //       size_t buflen, struct passwd **result);
    // This requires -D_REENTRANT flag to makefile
    if (getpwnam_r (_user, &pwd, pwdBuffer, MAX_PWD_BUFFER_SIZE, &result) == 0)
    {
        // Log the user  TODO
        //printf ("Name = %s; uid = %d\n", pwd.pw_name, pwd.pw_uid);
    }
    else
    {
      // LOG perror("getpwnam failed:");
      retCode = 1;
      return(retCode);
    }
     
    intUid = pwd.pw_uid;
    intGid = pwd.pw_gid;

#else
    //SYNTAX:
    //  struct passwd *getpwnam(const char *name);
    //  uid_t   pw_uid
    //  gid_t   pw_gid
    //
    //

    // Log the user  TODO
    // << "getpwnam user is :"<< _user << "strlen:" << strlen(_user) << endl;
    userPasswd = getpwnam(_user);
    if ( userPasswd  == NULL)
    {
      // LOG perror("getpwnam failed:");
      retCode = 1;
      return(retCode);
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
      retCode = 1;
      return(retCode);
    }
   

    //
    //SYNTAX:
    //  int chmod(const char *path, mode_t mode);
    //   where S_IRUSR - read only by user
    //
    Uint32 retn = chmod(fileName, S_IRUSR);
    if ( retn == -1)
    {
      retCode = 1;
      return(retCode);
    }

    return(retCode);
}

/**
 * Generate  random token data which is long interger
 *
 */
void LocalAuthUtils::_generateRandomToken()
{

   time_t t1;
   char randnum[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

   _randomToken = atol(randnum);
}

/**
 *
 * Generate random token data string
 */
void LocalAuthUtils::_generateRandomTokenString()
{
   char token[MAX_BUFFER_SIZE];
   Uint32 seconds, milliseconds;

   System::getCurrentTime(seconds, milliseconds);

   // generate a random token
   _generateRandomToken();
   sprintf (token,"%s%ld%d", _user, _randomToken, seconds + milliseconds );
   strncpy( _randomTokenString, token, strlen(token));
   _randomTokenString[strlen(token)]=0;

}

PEGASUS_NAMESPACE_END
