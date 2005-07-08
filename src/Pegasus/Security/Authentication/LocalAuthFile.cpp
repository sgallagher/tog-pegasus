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
// Author: Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//
// Modified By:
//                Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//                Sushma Fernandes, Hewlett-Packard Company 
//                        (sushma_fernandes.hp.com)
//                Amit K Arora, IBM (amita@in.ibm.com) for Bug#1090
//                Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////

#include "LocalAuthFile.h"

#include <fstream>
#ifndef PEGASUS_OS_TYPE_WINDOWS
#include <unistd.h>
#endif
#include <errno.h>
#ifndef PEGASUS_OS_TYPE_WINDOWS
#include <pwd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#ifndef PEGASUS_OS_TYPE_WINDOWS
#include <sys/time.h>
#endif

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/IPC.h>
#if defined(PEGASUS_OS_OS400)
#include "OS400ConvertChar.h"
#endif

#include "LocalAuthFile.h"

#if defined(PEGASUS_OS_OS400) || defined(PEGASUS_OS_TYPE_WINDOWS)
// srandom( ) and random( ) are srand( ) and rand( ) on OS/400 and windows
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
const Uint32 INT_BUFFER_SIZE = 16;

//
// Constant representing the random device name
//
const char DEV_URANDOM []    =  "/dev/urandom";

//
// Constant representing the size of random entropy needed
//
const Uint32 RANDOM_BYTES_NEEDED = 20;  // require minimum 160 bits = 20 bytes of randomness

//
// A unique sequence number used in random file name creation.
//
static Uint32 sequenceCount = 1;
static Mutex sequenceCountLock;



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
    _authFilePath = PEGASUS_LOCAL_AUTH_DIR;

#if defined(PEGASUS_OS_TYPE_WINDOWS)
    System::makeDirectory((const char *)_authFilePath.getCString());
#endif
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
    Uint32 mySequenceNumber;

    System::getCurrentTime(secs, milliSecs);

    //
    // extension size is username plus the sequence count
    //
    {
      AutoMutex autoMut(sequenceCountLock);
      mySequenceNumber = sequenceCount++;
    } // mutex unlocks here

    char extension[2*INT_BUFFER_SIZE];
    sprintf(extension,"_%u_%u", mySequenceNumber, milliSecs);
    extension[strlen(extension)] = 0;

    String filePath = String::EMPTY;

	//Check to see whether a domain was specified. If so, strip the domain from the
	//local auth file name, since the backslash and '@' are invalid filename characters.
	//Store this in another variable, since we need to keep the domain around for 
	//the rest of the operations. Bug 3076
	String fileUserName = _userName;
	Uint32 index = _userName.find('\\');
	if (index != PEG_NOT_FOUND) 
	{
		fileUserName = _userName.subString(index + 1);

	} else
	{
		index = _userName.find('@');
		if (index != PEG_NOT_FOUND) 
		{
			fileUserName = _userName.subString(0, index);
		}
	}

    filePath.append(_authFilePath);
    filePath.append(fileUserName);//_userName);
    filePath.append(extension);
    CString filePathCString = filePath.getCString();

    //
    // 1. Create a file name for authentication.
    //
#if defined(PEGASUS_OS_OS400)
    ofstream outfs(filePathCString, PEGASUS_STD(_CCSID_T(1208)));
#else
    ofstream outfs(filePathCString);
#endif
    if (!outfs)
    {
        // unable to create file
        PEG_TRACE_STRING(TRC_AUTHENTICATION, Tracer::LEVEL4,
            "Failed to create local auth file: " + 
             filePath + ", " + strerror(errno));
        PEG_METHOD_EXIT();
        throw CannotOpenFile (filePath);
    }
    outfs.clear();

    //
    // 2. Set file permission to read only by the owner.
    //
#if defined(PEGASUS_OS_OS400)
    CString tempName = filePathCString;
    const char * tmp = tempName;
    AtoE((char *)tmp);
    Sint32 ret = chmod(tmp, S_IRUSR);
#elif defined(PEGASUS_OS_TYPE_WINDOWS)
    Sint32 ret = 0;
#else
    Sint32 ret = chmod(filePathCString, S_IRUSR);
#endif
    if ( ret == -1)
    {
        String errorMsg = strerror(errno);
        PEG_TRACE_STRING(TRC_AUTHENTICATION, Tracer::LEVEL4,
            "Failed to change mode on file: " + filePath 
            + ", err is: " + errorMsg);
        PEG_METHOD_EXIT();
        
        // Unable to change the local auth file permissions, remove the file 
        // and throw CannotOpenFile error.

        if (filePath.size())
        {
            if (FileSystem::exists(filePath))
            {
                FileSystem::removeFile(filePath);
            }
        }

        throw CannotOpenFile (filePath);
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
#if !defined(PEGASUS_OS_TYPE_WINDOWS)
    if (!_changeFileOwner(filePath))
    {
        String errorMsg = strerror(errno);
        PEG_TRACE_STRING(TRC_AUTHENTICATION, Tracer::LEVEL4, 
            "Failed to change owner of file '" + filePath + "' to '" +
            _userName + "', err is: " + errorMsg);
        PEG_METHOD_EXIT();

        // Unable to change owner on local auth file, remove the file
        // and throw CannotOpenFile error.

        if (filePath.size())
        {
            if (FileSystem::exists(filePath))
            {
                FileSystem::removeFile(filePath);
            }
        }

        throw CannotOpenFile (filePath);
    }
#endif

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

#if !defined(PEGASUS_OS_TYPE_WINDOWS)

//
// changes the file owner to one specified
//
Boolean LocalAuthFile::_changeFileOwner(const String& fileName)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, "LocalAuthFile::_changeFileOwner()");

    struct passwd*        userPasswd;
#if defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC) || \
    defined(PEGASUS_OS_HPUX) || \
    defined (PEGASUS_OS_LINUX)

    const unsigned int PWD_BUFF_SIZE = 1024;
    struct passwd  pwd;
    struct passwd *result;
    char pwdBuffer[PWD_BUFF_SIZE];

    if(getpwnam_r(_userName.getCString(), &pwd, pwdBuffer, PWD_BUFF_SIZE,
                  &userPasswd) != 0)
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
#endif

//
// Generate  random token string
//
String LocalAuthFile::_generateRandomTokenString()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION, 
        "LocalAuthFile::_generateRandomTokenString()");

    String randomToken = String::EMPTY;

    String randFile = String(DEV_URANDOM);
    FILE *fh;

    //
    // If /dev/urandom exists then read random token from /dev/urandom
    //
    if ( ( fh = fopen( (const char *)randFile.getCString(), "r") ) != NULL )
    {
        char token[RANDOM_BYTES_NEEDED+1];

        setvbuf(fh, NULL, _IONBF, 0);   // need unbuffered input
        Uint32 n = fread( (unsigned char *)token, 1, RANDOM_BYTES_NEEDED, fh );
        fclose(fh);
        token[n]=0;

        randomToken.clear();
        char hexChar[10];

        for (Uint32 i=0; i < n; i++)
        {
            sprintf(hexChar, "%X", (unsigned char)token[i]);
            randomToken.append(String(hexChar));
            memset(hexChar, 0x00, sizeof(hexChar));
        }
    }

    char token[2*INT_BUFFER_SIZE];
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
