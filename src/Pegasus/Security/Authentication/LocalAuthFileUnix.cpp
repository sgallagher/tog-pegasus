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
// Author: Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//
// Modified By:
//                Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>
#if defined(PEGASUS_OS_OS400)
#include "OS400ConvertChar.h"
#endif

#include "LocalAuthFile.h"

#ifdef PEGASUS_OS_OS400
// srandom( ) and random( ) are srand( ) and rand( ) on OS/400
#define srandom(x) srand(x)
#define random() rand()
#endif


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//
// Constant representing the random seed for srandom function
//
const Uint32 RANDOM_SEED     =  100;

//
// File path used to create temporary random file
//
const char TMP_AUTH_FILE_NAME []  =  "/cimclient_";

//
// Constant representing the int buffer size
//
const Uint32 INT_BUFFER_SIZE = 64;

//
// A unique sequence number used in random file name creation.
//
static Uint32 sequenceCount  =  212;



LocalAuthFile::LocalAuthFile(const String& userName)
    : _userName(userName),
      _filePathName(String::EMPTY),
      _challenge(String::EMPTY)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, "LocalAuthFile::LocalAuthFile()");

    srandom(RANDOM_SEED);

    //
    // get the configured temporary authentication file path
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    _authFilePath = configManager->getCurrentValue("tempLocalAuthDir");

    _authFilePath.append(TMP_AUTH_FILE_NAME);

    PEG_METHOD_EXIT();
}

LocalAuthFile::~LocalAuthFile() 
{ 
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, "LocalAuthFile::~LocalAuthFile()");

    PEG_METHOD_EXIT();
}

//
// create a file and make a random token data entry to it.
// send the file name back to caller
//
String LocalAuthFile::create()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, "LocalAuthFile::create()");

    Uint32 secs, milliSecs;

    System::getCurrentTime(secs, milliSecs);

    //
    // extension size is username plus the sequence count
    //
    sequenceCount++;

    char extension[INT_BUFFER_SIZE];
    sprintf(extension,"_%d", sequenceCount + milliSecs);
    extension[strlen(extension)] = 0;

    String filePath = String::EMPTY;

    filePath.append(_authFilePath);
    filePath.append(_userName);
    filePath.append(extension);
    CString filePathCString = filePath.getCString();

    //
    // 1. Create a file name for authentication.
    //
#if defined(PEGASUS_OS_OS400)
    CString tempPath = filePathCString;
    const char * tmp = tempPath;
    AtoE((char *)tmp);
    ofstream outfs(tmp, PEGASUS_STD(_CCSID_T(1208)));
#else
    ofstream outfs(filePathCString);
#endif
    if (!outfs)
    {
        // unable to create file
        PEG_TRACE_STRING(
            TRC_AUTHENTICATION, 4, "Failed to create file: " + filePath);
        PEG_METHOD_EXIT();
        return(_filePathName);
    }
    outfs.clear();

    //
    // 2. Set file permission to read only by the owner.
    //
#if defined(PEGASUS_OS_OS400)
    Sint32 ret = chmod(tmp, S_IRUSR);
#else
    Sint32 ret = chmod(filePathCString, S_IRUSR);
#endif
    if ( ret == -1)
    {
        PEG_METHOD_EXIT();
        return(_filePathName);
    }

    //
    // 3. Generate random token and write the token to the file.
    //
    String randomToken = _generateRandomTokenString();
    outfs << randomToken;
    outfs.close();

    //
    // 4. Change the file owner to the requesting user.
    //
    if (!_changeFileOwner(filePath))
    {
        PEG_TRACE_STRING(TRC_AUTHENTICATION, Tracer::LEVEL3, 
            "Could not change owner of file '" + filePath + "' to '" +
            _userName + "'.");
        PEG_METHOD_EXIT();
        return(_filePathName);
    }

    _challenge = randomToken;

    _filePathName = filePath;

    PEG_METHOD_EXIT();

    return(_filePathName);
}

//
//  Removes the file that was created
//
Boolean LocalAuthFile::remove()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, "LocalAuthFile::remove()");

    Boolean retVal = true;

    //
    // remove the file
    //
    if (FileSystem::exists(_filePathName))
    {
        retVal = FileSystem::removeFile(_filePathName);
    }

    PEG_METHOD_EXIT();

    return(retVal);
}

//
// Get the string that was created as a challenge string
//
String LocalAuthFile::getChallengeString()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, "LocalAuthFile::getChallengeString()");

    PEG_METHOD_EXIT();

    return(_challenge);
}

//
// changes the file owner to one specified
//
Boolean LocalAuthFile::_changeFileOwner(const String& fileName)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, "LocalAuthFile::_changeFileOwner()");

    struct passwd*        userPasswd;
#ifdef PEGASUS_PLATFORM_SOLARIS_SPARC_CC
    struct passwd  pwd;
    struct passwd *result;
    char        pwdBuffer[1024];

    if(getpwnam_r(_userName.getCstring(), &pwd, pwdBuffer, 1024, &userPasswd) != 0)
    {
	userPasswd=(struct passwd *)NULL;
    }

#elif defined(PEGASUS_OS_OS400)
    CString tempName = _userName.getCString();
    const char * tmp = tempName;
    AtoE((char *)tmp);
    userPasswd = getpwnam(tmp);
#else

    userPasswd = getpwnam(_userName.getCString());
#endif

    if ( userPasswd  == NULL)
    {
        PEG_METHOD_EXIT();
        return (false);
    }

#if defined(PEGASUS_OS_OS400)
    CString tempPath = fileName.getCString();
    const char * tmp1 = tempPath;
    AtoE((char *)tmp1);
    Sint32 ret = chown(tmp1, userPasswd->pw_uid, userPasswd->pw_gid);
#else
    Sint32 ret = chown(fileName.getCString(), userPasswd->pw_uid, userPasswd->pw_gid);
#endif
    if ( ret == -1)
    {
        PEG_METHOD_EXIT();
        return (false);
    }
   
    PEG_METHOD_EXIT();

    return (true);
}

//
// Generate  random token string
//
String LocalAuthFile::_generateRandomTokenString()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, 
        "LocalAuthFile::_generateRandomTokenString()");

    String randomToken = String::EMPTY;

    char token[INT_BUFFER_SIZE];
    Uint32 seconds, milliseconds;

    System::getCurrentTime(seconds, milliseconds);

    //
    // generate a random token
    //
    char randnum[] = { '0' + (random() % 10), '0' + (random() % 10), '\0' };
    long randomNum = atol(randnum);

    sprintf (token,"%ld%d", randomNum, seconds + milliseconds );
    token[strlen(token)] = 0;

    randomToken.append(_userName);
    randomToken.append(token);

    PEG_METHOD_EXIT();

    return (randomToken);
}

PEGASUS_NAMESPACE_END
