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
// Modified By: Ben Heilbronn (ben_heilbronn@hp.com)
//              Sushma Fernandes (sushma_fernandes@hp.com)
//              Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By: Dave Rosckes (rosckes@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_OS_HPUX
# include <dl.h>
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# include <dll.h>
#elif defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
#  include <fcntl.h> 
#  include <qycmutiltyUtility.H>
#  include <unistd.cleinc>
#  include "qycmmsgclsMessage.H" // ycmMessage class
#  include "OS400SystemState.h"  // OS400LoadDynamicLibrary, etc 
#else
# include <dlfcn.h>
#endif

#include <unistd.h>
#include <dirent.h>
#include <pwd.h>

#if !defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) && !defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM) 
#include <crypt.h> 
#endif

#if defined(PEGASUS_USE_SYSLOGS) 
#include <syslog.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <time.h>
#include <sys/time.h>
#include <netdb.h>
#include "System.h"
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/InternalException.h>

PEGASUS_NAMESPACE_BEGIN

#if defined(PEGASUS_OS_HPUX)
Boolean System::bindVerbose = false;
#endif

inline void sleep_wrapper(Uint32 seconds)
{
    sleep(seconds);
}

void System::getCurrentTime(Uint32& seconds, Uint32& milliseconds)
{
    timeval tv;
    gettimeofday(&tv, 0);
    seconds = Uint32(tv.tv_sec);
    milliseconds = Uint32(tv.tv_usec) / 1000;
}

String System::getCurrentASCIITime()
{
    char    str[50];
    time_t  rawTime;

    time(&rawTime);
    strftime(str, 40,"%m/%d/%Y-%T", localtime(&rawTime));
    String time = str;
    return time;
}

void System::sleep(Uint32 seconds)
{
    sleep_wrapper(seconds);
}

Boolean System::exists(const char* path)
{
    return access(path, F_OK) == 0;
}

Boolean System::canRead(const char* path)
{
    return access(path, R_OK) == 0;
}

Boolean System::canWrite(const char* path)
{
    return access(path, W_OK) == 0;
}

Boolean System::getCurrentDirectory(char* path, Uint32 size)
{
    return getcwd(path, size) != NULL;
}

Boolean System::isDirectory(const char* path)
{
    struct stat st;

    if (stat(path, &st) != 0)
        return false;

    return S_ISDIR(st.st_mode);
}

Boolean System::changeDirectory(const char* path)
{
    return chdir(path) == 0;
}

Boolean System::makeDirectory(const char* path)
{
    return mkdir(path, 0777) == 0;
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
    if (link(oldPath, newPath) != 0)
        return false;

    return unlink(oldPath) == 0;
}

DynamicLibraryHandle System::loadDynamicLibrary(const char* fileName)
{
    PEG_METHOD_ENTER(TRC_OS_ABSTRACTION, "System::loadDynamicLibrary()");

    Tracer::trace(TRC_OS_ABSTRACTION, Tracer::LEVEL2, 
                  "Attempting to load library %s", fileName);

#if defined(PEGASUS_OS_HPUX)
    void* handle;
    if (bindVerbose)
    {
        handle = shl_load(fileName, 
                     BIND_IMMEDIATE | DYNAMIC_PATH | BIND_VERBOSE, 0L);
    }
    else 
    {
        handle = shl_load(fileName, BIND_IMMEDIATE | DYNAMIC_PATH, 0L);
    }
    Tracer::trace(TRC_OS_ABSTRACTION, Tracer::LEVEL2, 
                  "After loading lib %s, error code is %d", fileName, 
                  (handle == (void *)0)?errno:0);

    PEG_METHOD_EXIT();
    return DynamicLibraryHandle(handle);
#elif defined(PEGASUS_OS_TRU64)
    PEG_METHOD_EXIT();
    return DynamicLibraryHandle(dlopen(fileName, RTLD_NOW));
#elif defined(PEGASUS_OS_ZOS)
    PEG_METHOD_EXIT();
    return DynamicLibraryHandle(dllload(fileName));
#elif defined(PEGASUS_OS_OS400)
    PEG_METHOD_EXIT();
    return DynamicLibraryHandle(OS400_LoadDynamicLibrary(fileName));
#else
    PEG_METHOD_EXIT();
    return DynamicLibraryHandle(dlopen(fileName, RTLD_NOW | RTLD_GLOBAL));
#endif

}

void System::unloadDynamicLibrary(DynamicLibraryHandle libraryHandle)
{
    // ATTN: Should this method indicate success/failure?
#ifdef PEGASUS_OS_LINUX
    dlclose(libraryHandle);
#endif

#ifdef PEGASUS_OS_HPUX
    // Note: shl_unload will unload the library even if it has been loaded
    // multiple times.  No reference count is kept.
    int ignored = shl_unload(reinterpret_cast<shl_t>(libraryHandle));
#endif

#ifdef PEGASUS_OS_OS400
   OS400_UnloadDynamicLibrary((int)libraryHandle);
#endif
}

String System::dynamicLoadError() {
    // ATTN: Is this safe in a multi-threaded process?  Should this string
    // be returned from loadDynamicLibrary?
#ifdef PEGASUS_OS_HPUX
    // ATTN: If shl_load() returns NULL, this value should be strerror(errno)
    return String();
#elif defined(PEGASUS_OS_ZOS)
    return String();
#elif defined(PEGASUS_OS_OS400)
    return String(OS400_DynamicLoadError());
#else
    String dlerr = dlerror();
    return dlerr;
#endif
}


DynamicSymbolHandle System::loadDynamicSymbol(
    DynamicLibraryHandle libraryHandle,
    const char* symbolName)
{
#ifdef PEGASUS_OS_HPUX
    char* p = (char*)symbolName;
    void* proc = 0;

    if (shl_findsym((shl_t*)&libraryHandle, symbolName, TYPE_UNDEFINED,
                    &proc) == 0)
    {
        return DynamicSymbolHandle(proc);
    }

    if (shl_findsym((shl_t*)libraryHandle,
                    (String("_") + symbolName).getCString(),
                    TYPE_UNDEFINED,
                    &proc) == 0)
    {
        return DynamicSymbolHandle(proc);
    }

    return 0;

#elif defined(PEGASUS_OS_ZOS)
    return DynamicSymbolHandle(dllqueryfn((dllhandle *)libraryHandle,
                               (char*)symbolName));

#elif defined(PEGASUS_OS_OS400)
    return DynamicSymbolHandle(OS400_LoadDynamicSymbol((int)libraryHandle,
                               symbolName));
#else

    return DynamicSymbolHandle(dlsym(libraryHandle,(char *) symbolName));

#endif
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
#ifdef PEGASUS_OS_HPUX
    char hostName [MAXHOSTNAMELEN];
    struct hostent *he;
    String fqName;

    if (gethostname (hostName, MAXHOSTNAMELEN) != 0)
    {
        return String::EMPTY;
    }

    if (he = gethostbyname (hostName))
    {
       strcpy (hostName, he->h_name);
    }

    fqName.assign (hostName);

    return fqName;
#else
    //
    //  ATTN: Implement this method to return the fully qualified host name
    //
    return String::EMPTY;
#endif
}

String System::getSystemCreationClassName ()
{
#ifdef PEGASUS_OS_HPUX
    return "CIM_ComputerSystem";
#else
    //
    //  ATTN: Implement this method to return the system creation class name
    //
    return String::EMPTY;
#endif
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
#if !defined(PEGASUS_OS_OS400)
    if ( (serv = getservbyname(serviceName, TCP)) != NULL )
#else
    // Need to cast on OS/400
    if ( (serv = getservbyname((char *)serviceName, TCP)) != NULL )
#endif
    {
#ifndef PEGASUS_PLATFORM_LINUX_IX86_GNU
        localPort = serv->s_port;
#else
        localPort = htons((uint16_t)serv->s_port);
#endif
    }
    else
    {
        localPort = defaultPort;
    }

    return localPort;
}

String System::getPassword(const char* prompt)
{

    String password;

#if !defined(PEGASUS_OS_OS400)
    // Not supported on OS/400, and we don't need it.
    password = String(getpass( prompt ));
#endif

    return password;
}

String System::getEffectiveUserName()
{
    String userName = String::EMPTY;
    struct passwd*   pwd = NULL;

    //
    //  get the currently logged in user's UID.
    //
    pwd = getpwuid(geteuid());
    if ( pwd == NULL )
    {
        //ATTN: Log a message
        // "User might have been removed just after login"
    }
    else
    {
        //
        //  get the user name
        //
        userName.assign(pwd->pw_name);
    }

    return(userName);
}

String System::encryptPassword(const char* password, const char* salt)
{
#if !defined(PEGASUS_OS_OS400)
    return ( String(crypt( password,salt)) );
#else
    // Not supported on OS400, and we don't need it.
    return ( String(password) );
#endif
}

Boolean System::isSystemUser(const char* userName)
{
    //
    //  get the password entry for the user
    //
    if  ( getpwnam(userName) == NULL )
    {
	return false;
    }
    return true;
}

Boolean System::isPrivilegedUser(const String userName)
{
    //
    // Check if the given user is a privileged user
    //
#if !defined(PEGASUS_OS_OS400)
    struct passwd   pwd;
    struct passwd   *result;
    char            pwdBuffer[1024];

    if (getpwnam_r(userName.getCString(), &pwd, pwdBuffer, 1024, &result) == 0)
    {
        if ( pwd.pw_uid == 0 )
        {
            return true;
        }
    }
    return false;
#else
    return ycmCheckUserCmdAuthorities(userName.getCString());
#endif
}

String System::getPrivilegedUserName()
{
    static String userName = String::EMPTY;

    if (userName == String::EMPTY)
    {
        struct passwd*   pwd = NULL;

        //
        //  get the privileged user's UID.
        //
	//  (on OS/400, this is QSECOFR)
        pwd = getpwuid(0);
        if ( pwd != NULL )
        {
            //
            //  get the user name
            //
            userName.assign(pwd->pw_name);
        }
        else
        {
            PEGASUS_ASSERT(0);
        }
    }

    return (userName);
}

Uint32 System::getPID()
{
    //
    // Get the Process ID
    //
    Uint32 pid = getpid();

    return pid;
}

Boolean System::truncateFile(
    const char* path, 
    size_t newSize)
{
#if !defined(PEGASUS_OS_OS400)
    return (truncate(path, newSize) == 0);
#else
    int fd = open(path, O_WRONLY);
    if (fd != -1)
    {
       int rc = ftruncate(fd, newSize);
       close(fd);
       return (rc == 0);
    }
    
    return false;
#endif
}

// Is absolute path?
Boolean System::is_absolute_path(const char *path)
{
  if (path == NULL)
    return false;
  
  if (path[0] == '/')
    return true;
  
  return false;
}

void System::openlog(const String ident)
{
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)

    // ATTN: Hard-code the "cimserver" identifier until the infrastructure
    // is set up to pass this as the "ident" string.
    //::openlog(ident.getCString(), LOG_PID|LOG_CONS, LOG_DAEMON);
    ::openlog("cimserver", LOG_PID|LOG_CONS, LOG_DAEMON);

#endif

    return;
}

void System::syslog(Uint32 severity, const char *data)
{
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)

    // FUTURE-SF-P3-20020517 : Use the Syslog on HP-UX. Eventually only 
    // certain messages will go to the Syslog and others to the 
    // Pegasus Logger.
    Uint32 syslogLevel = LOG_DEBUG;

    // Map the log levels.
    if (severity & Logger::TRACE) syslogLevel =       LOG_DEBUG;
    if (severity & Logger::INFORMATION) syslogLevel = LOG_INFO;
    if (severity & Logger::WARNING) syslogLevel =     LOG_WARNING;
    if (severity & Logger::SEVERE) syslogLevel =      LOG_ERR;
    if (severity & Logger::FATAL) syslogLevel =       LOG_CRIT;

    ::syslog(syslogLevel, "%s", data);

#elif defined(PEGASUS_OS_OS400)

    std::string replacementData = data;
    // All messages will go to the joblog. In the future
    // some messages may go to other message queues yet
    // to be determined.
    if ((severity & Logger::TRACE) ||
	(severity & Logger::INFORMATION))
    {

	// turn into ycmMessage so we can put it in the job log
	ycmMessage theMessage(msgCPxDF80,
			      CPIprefix,
			      replacementData,
			      "Logger",ycmCTLCIMID);

	// put the message in the joblog
	theMessage.joblogIt(UnitOfWorkError,
			    ycmMessage::Informational);
    }

    if ((severity & Logger::WARNING) ||
	(severity & Logger::SEVERE)  ||
	(severity & Logger::FATAL))
    {
	// turn into ycmMessage so we can put it in the job log
	ycmMessage theMessage(msgCPxDF82,
			      CPDprefix,
			      replacementData,
			      "Logger",ycmCTLCIMID);

	// put the message in the joblog
	theMessage.joblogIt(UnitOfWorkError,
			    ycmMessage::Diagnostic);
    }

#endif

    return;
}

void System::closelog()
{
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)

    ::closelog();

#endif

    return;
}

// System ID constants for Logger::put and Logger::trace
#if defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
const String System::CIMSERVER = "qycmcimom";  // Server system ID
#else
const String System::CIMSERVER = "cimserver";  // Server system ID
#endif
    
PEGASUS_NAMESPACE_END
