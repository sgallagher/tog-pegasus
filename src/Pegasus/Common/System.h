//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: 
//     Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_System_h
#define Pegasus_System_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

//
// Protocal Type
//
#define TCP                        "tcp"

PEGASUS_NAMESPACE_BEGIN

/** This is an opaque type which is used to represent dynamic library
    handles returned by the System::loadDynamicLibrary() method and
    accepted by the System::loadDynamicProcedure() method.
*/
typedef struct DynamicLibraryHandle_* DynamicLibraryHandle;

/** This is an opaque type which is returned by System::loadDynamicSymbol().
    Values of this type may be casted to the appropriate target type.
*/
typedef struct DynamicSymbolHandle_* DynamicSymbolHandle;

/** The System class defines wrappers for operating system related calls.
    These are only placed here if they are extremely light. These are
    usually just direct wrappers which map more or less one to one to the
    underlying function.
*/
class PEGASUS_COMMON_LINKAGE System
{
public:
    /** getCurrentTime - Gets the current time as seconds and milliseconds
    into the provided variables using system functions.
    @param seconds Return for the seconds component of the time.
    @param milliseconds Return for the milliseconds component of the time.
    @return The value is returned in the parameters.
    The time returned is as defined in number of seconds and milliseconds
    since 00:00 Coordinated Universal Time (UTC), January 1, 1970,

    */
    static void getCurrentTime(Uint32& seconds, Uint32& milliseconds);

    /** getCurrentASCIITime Gets time/date in a fixed format. The format is
        YY MM DD-HH:MM:SS
	@return Returns String with the ASCII time date.
    */
    static String getCurrentASCIITime();

    static void sleep(Uint32 seconds);

    static Boolean exists(const char* path);

    static Boolean canRead(const char* path);

    static Boolean canWrite(const char* path);

    static Boolean getCurrentDirectory(char* path, Uint32 size);

    static Boolean isDirectory(const char* path);

    static Boolean changeDirectory(const char* path);

    static Boolean makeDirectory(const char* path);

    static Boolean getFileSize(const char* path, Uint32& size);

    static Boolean removeDirectory(const char* path);

    static Boolean removeFile(const char* path);

    static Boolean renameFile(const char* oldPath, const char* newPath);

    static DynamicLibraryHandle loadDynamicLibrary(const char* fileName);

    static void unloadDynamicLibrary(DynamicLibraryHandle libraryHandle);

    static String dynamicLoadError(void);

    static DynamicSymbolHandle loadDynamicSymbol(
	DynamicLibraryHandle libraryHandle,
	const char* symbolName);

    static String getHostName();

    static Uint32 lookupPort(
        const char * serviceName,
        Uint32 defaultPort);

    static String getCurrentLoginName();

    /**
    This function is used to input a password with echo disabled.
    The function reads up to a newline and returns a password of at most
    8 characters.

    @param  prompt      String containing the message prompt to be displayed
    @return             password obtained from the user
    */
    static String getPassword(const char* prompt);

    /**
    This function is used to encrypt the user's password. 
    The encryption is compatible with Apache's  password file (generated using
    the htpasswd command )

    @param password     Password to be encrypted.
    @param salt         Two character string chosen from the set [a-zA-Z0-9./].

    @return             Encrypted password.
    */
    static String encryptPassword(const char* password, const char* salt);

    /**
    This function is used to verify whether specified user is a user 
    on the local system.

    @param userName     User name to be verified.

    @return             true if the username is valid, else false
    */
    static Boolean isSystemUser(char* userName);

    /**
    When the user name is not passed as an argument, this function 
    checks whether the user running the command is a privileged user.
    If a user name is given this function checks whether
    the given user is a privileged user.

    @param userName     User name to be checked.

    @return             true if the user is a privileged user, else false
    */
    static Boolean isPrivilegedUser(const String userName = String::EMPTY);

    /**
    This function is used to get the process ID of the calling process.

    @return             Process ID
    */
    static Uint32 getPID();

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_System_h */
