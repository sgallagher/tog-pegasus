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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: 
//     Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By: Dave Rosckes (rosckes@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_System_h
#define Pegasus_System_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Logger.h>

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
#if !defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) && !defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)         
typedef struct DynamicSymbolHandle_* DynamicSymbolHandle; 
#else                                                     
extern "C" {typedef int (* DynamicSymbolHandle)(void);}   
#endif                                                    



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

    static Boolean copyFile(const char* fromPath, const char* toPath);

    static DynamicLibraryHandle loadDynamicLibrary(const char* fileName);

    static void unloadDynamicLibrary(DynamicLibraryHandle libraryHandle);

    static String dynamicLoadError(void);

    static DynamicSymbolHandle loadDynamicSymbol(
	DynamicLibraryHandle libraryHandle,
	const char* symbolName);

    static String getHostName();
    static String getFullyQualifiedHostName ();
    static String getSystemCreationClassName ();

    static Uint32 lookupPort(
        const char * serviceName,
        Uint32 defaultPort);

    static String getEffectiveUserName();

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
    static Boolean isSystemUser(const char* userName);

    /**
    Checks whether the given user is a privileged user.

    @param userName     User name to be checked.
    @return             true if the user is a privileged user, else false
    */
    static Boolean isPrivilegedUser(const String userName);

    /**
    This function returns the privileged user name on the system.

    @return             the privileged user name
    */
    static String getPrivilegedUserName();

    /**
    This function is used to get the process ID of the calling process.

    @return             Process ID
    */
    static Uint32 getPID();

    static Boolean truncateFile(const char* path, size_t newSize);

    /** Compare two strings but ignore any case differences.
        This method is provided only because some platforms lack a strcasecmp
        function in the standard library.
    */
    static Sint32 strcasecmp(const char* s1, const char* s2);
    
    /** Return just the file or directory name from the path into basename.
        This method returns a file or directory name at the end of a path.
        The path can be relative or absolute. If the path is the root,
        then empty string is returned.        
    */
    static char *extract_file_name(const char *fullpath, char *basename);

    /** Return just the pathname into dirname. The fullpath can be relative
        or absolute. This method returns a path minus the file or
        directory name at the end of a supplied path (fullpath).
        If the fullpath is the root, then fullpath is returned.
        The resulting path will contain a trailing slash unless fullpath is
        a file or directory name, in which case, just the file or directory
        name is returned. 
    */
    static char *extract_file_path(const char *fullpath, char *dirname);

    // Is absolute path?
    static Boolean is_absolute_path(const char *path);

#if defined(PEGASUS_OS_HPUX)
    static Boolean bindVerbose;
#endif

    /** This function is an abstraction for the openlog interface used in the Logger
        class.  Each platform intending to use system logs should support this interface
    */
    static void openlog(const String);

    /** This function is an abstraction for the syslog interface used in the Logger
        class.  Each platform intending to use system logs should support this interface
    */
    static void syslog(Uint32, const char *);

    /** This function is an abstraction for the closelog interface used in the Logger
        class.  Each platform intending to use system logs should support this interface
    */
    static void closelog();

    // System ID constants for Logger::put and Logger::trace
    static const String CIMSERVER;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_System_h */
