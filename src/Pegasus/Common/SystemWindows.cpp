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
// Modified By: Sushma Fernandes (sushma_fernandes@hp.com)
//
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By: Dave Rosckes (rosckes@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "System.h"

PEGASUS_NAMESPACE_BEGIN

#include <windows.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <sys/timeb.h>
#include <io.h>
#include <direct.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <windows.h>
#include <process.h>

#define ACCESS_EXISTS 0
#define ACCESS_WRITE 2
#define ACCESS_READ 4
#define ACCESS_READ_AND_WRITE 6

void System::getCurrentTime(Uint32& seconds, Uint32& milliseconds)
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER largeInt = { ft.dwLowDateTime, ft.dwHighDateTime };
    largeInt.QuadPart -= 0x19db1ded53e8000;
    seconds = long(largeInt.QuadPart / (10000 * 1000));
    milliseconds = long((largeInt.QuadPart % (10000 * 1000)) / 10);
    // This is a real hack. Added the following line after timevalue was
    // corrected and this apparently wrong. ks 7 apri 2002
    milliseconds = milliseconds / 1000;
}

String System::getCurrentASCIITime()
{
    char tmpbuf[128];
    _strdate( tmpbuf );
    String date = tmpbuf;
    _strtime( tmpbuf );
    date.append("-");
    date.append(tmpbuf);
    return date;
}

void System::sleep(Uint32 seconds)
{
    Sleep(seconds * 1000);
}

Boolean System::exists(const char* path)
{
    return _access(path, ACCESS_EXISTS) == 0;
}

Boolean System::canRead(const char* path)
{
    return _access(path, ACCESS_READ) == 0;
}

Boolean System::canWrite(const char* path)
{
    return _access(path, ACCESS_WRITE) == 0;
}

Boolean System::getCurrentDirectory(char* path, Uint32 size)
{
    return GetCurrentDirectory(size, path) != 0;
}

Boolean System::isDirectory(const char* path)
{
    struct stat st;

    if (stat(path, &st) != 0)
	return false;

    return (st.st_mode & _S_IFDIR) != 0;
}

Boolean System::changeDirectory(const char* path)
{
    return chdir(path) == 0;
}

Boolean System::makeDirectory(const char* path)
{
    return _mkdir(path) == 0;
}

Boolean System::getFileSize(const char* path, Uint32& size)
{
    struct stat st;

    if (stat(path, &st) != 0)
	return false;

    size = st.st_size;
    return true;
}

Boolean System::removeDirectory(const char* path)
{
    return rmdir(path) == 0;	
}

Boolean System::removeFile(const char* path)
{
    return unlink(path) == 0;	
}

Boolean System::renameFile(const char* oldPath, const char* newPath)
{
    return rename(oldPath, newPath) == 0;
}

DynamicLibraryHandle System::loadDynamicLibrary(const char* fileName)
{
    return DynamicLibraryHandle(LoadLibrary(fileName));
}

void System::unloadDynamicLibrary(DynamicLibraryHandle libraryHandle)
{
	FreeLibrary(HINSTANCE(libraryHandle));
}

String System::dynamicLoadError(void) {
return String();
}

DynamicSymbolHandle System::loadDynamicSymbol(
    DynamicLibraryHandle libraryHandle,
    const char* symbolName)
{
    return DynamicSymbolHandle(GetProcAddress(
	(HINSTANCE)libraryHandle, symbolName));
}

String System::getHostName()
{
    static char hostname[64];

    if (!*hostname)
        gethostname(hostname, sizeof(hostname));

    return hostname;
}

String System::getFullyQualifiedHostName ()
{
    //
    //  ATTN: Implement this method to return the fully qualified host name
    //
    return String::EMPTY;
}

String System::getSystemCreationClassName ()
{
    //
    //  ATTN: Implement this method to return the system creation class name
    //
    return String::EMPTY;
}

Uint32 System::lookupPort(
    const char * serviceName,
    Uint32 defaultPort)
{
    Uint32 localPort;

    struct servent *serv;

    //
    // Get wbem-local port from /etc/services
    //
    if (  (serv = getservbyname(serviceName, TCP)) != NULL )
    {
        localPort = serv->s_port;
    }
    else
    {
        localPort = defaultPort;
    }

    return localPort;
}

String System::getPassword(const char* prompt)
{
    //ATTN: Implement this method to get password from User with no echo
    //      This is used in cimuser CLI
    String password("dummy");

    return password;
}

String System::getEffectiveUserName()
{
    //ATTN: Implement this method to get the current login user name
    //      This is used in local authentication.

    return String();
}

String System::encryptPassword(const char* password, const char* salt)
{
    //ATTN: Implement this method to encrypt the password
    //      This is used in User Manager
    return (String("dummy"));
}

Boolean System::isSystemUser(const char* userName)
{
    //ATTN: Implement this method to verify if user is vaild on the local system
    //      This is used in User Manager
    return true;
}

Boolean System::isPrivilegedUser(const String userName)
{
    // ATTN: Implement this method to verify if user executing the current
    //       command is a priviliged user, when user name is not passed as
    //       as argument. If user name is passed the function checks 
    //       whether the given user is a priviliged user.
    //       This is used in cimuser CLI and CIMOperationRequestAuthorizer
    return true;
}

String System::getPrivilegedUserName()
{
    // ATTN-NB-03-20000304: Implement better way to get the privileged
    // user on the system.

    return (String("Administrator"));
}
    
Uint32 System::getPID()
{
    return _getpid();
}

Boolean System::truncateFile(
    const char* path, 
    size_t newSize)
{
    int fd = open(path, O_RDWR);

    if (fd == -1)
        return false;

    if (chsize(fd, newSize) != 0)
	return false;

    close(fd);
    return true;
}

// Is absolute path?
Boolean System::is_absolute_path(const char *path)
{
  char full[_MAX_PATH];
 
  return (strcasecmp(_fullpath( full, path, _MAX_PATH ), path) == 0) ? true : false;
}

void System::openlog(const String ident)
{
    return;
}

void System::syslog(Uint32 severity, const char *data)
{
    return;
}

void System::closelog()
{
    return;
}

// System ID constants for Logger::put and Logger::trace
const String System::CIMSERVER = "cimserver";  // Server system ID

PEGASUS_NAMESPACE_END
