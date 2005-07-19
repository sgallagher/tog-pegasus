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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Ben Heilbronn (ben_heilbronn@hp.com)
//              Sushma Fernandes (sushma_fernandes@hp.com)
//              Nag Boranna (nagaraja_boranna@hp.com)
//              Bapu Patil (bapu_patil@hp.com)
//              Dave Rosckes (rosckes@us.ibm.com)
//              Amit K Arora (amita@in.ibm.com) for PEP101
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Yi Zhou (yi.zhou@hp.com)
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#3194
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_OS_HPUX
# include <dl.h>
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# include <dll.h>
#elif defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
# include <fcntl.h>
# include <qycmutilu2.H>
# include <unistd.cleinc>
# include "qycmmsgclsMessage.H" // ycmMessage class
# include "OS400SystemState.h"  // OS400LoadDynamicLibrary, etc
# include "OS400ConvertChar.h"
#else
# include <dlfcn.h>
#endif

#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

#include <errno.h>
#if defined(PEGASUS_OS_SOLARIS)
# include <string.h>
#endif

#if !defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) && !defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM) && !defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU)
#include <crypt.h>
#endif

#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#include <arpa/inet.h>
#include <__ftp.h>
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
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/IPC.h>
#ifdef PEGASUS_ZOS_SECURITY
#include "DynamicLibraryzOS_inline.h"
#endif

#if defined(PEGASUS_OS_LSB)
#include <netinet/in.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#endif

PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_OS_OS400
typedef struct os400_pnstruct
{
  Qlg_Path_Name_T qlg_struct;
  char * pn;
} OS400_PNSTRUCT;
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
    struct tm tmBuffer;

    time(&rawTime);
    strftime(str, 40,"%m/%d/%Y-%T", localtime_r(&rawTime, &tmBuffer));
    return String(str);
}

void System::sleep(Uint32 seconds)
{
    sleep_wrapper(seconds);
}

Boolean System::exists(const char* path)
{
#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    return QlgAccess((Qlg_Path_Name_T *)&pathname, F_OK) == 0;
#else
    return access(path, F_OK) == 0;
#endif
}

Boolean System::canRead(const char* path)
{

#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    return QlgAccess((Qlg_Path_Name_T *)&pathname, R_OK) == 0;
#else
    return access(path, R_OK) == 0;
#endif
}

Boolean System::canWrite(const char* path)
{
#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    return QlgAccess((Qlg_Path_Name_T *)&pathname, W_OK) == 0;
#else
    return access(path, W_OK) == 0;
#endif
}

Boolean System::getCurrentDirectory(char* path, Uint32 size)
{
#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    return QlgGetcwd((Qlg_Path_Name_T *)&pathname, size) == 0;
#else
    return getcwd(path, size) != NULL;
#endif
}

Boolean System::isDirectory(const char* path)
{
    struct stat st;

#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    if (QlgStat((Qlg_Path_Name_T *)&pathname, &st) != 0)
        return false;
#else
    if (stat(path, &st) != 0)
        return false;
#endif
    return S_ISDIR(st.st_mode);
}

Boolean System::changeDirectory(const char* path)
{
#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    return QlgChdir((Qlg_Path_Name_T *)&pathname) == 0;
#else
    return chdir(path) == 0;
#endif
}

Boolean System::makeDirectory(const char* path)
{

#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    return QlgMkdir((Qlg_Path_Name_T *)&pathname, 0777) == 0;
#else
    return mkdir(path, 0777) == 0;
#endif

}

Boolean System::getFileSize(const char* path, Uint32& size)
{
    struct stat st;

#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    if (QlgStat((Qlg_Path_Name_T *)&pathname, &st) != 0)
        return false;
#else
    if (stat(path, &st) != 0)
        return false;
#endif

    size = st.st_size;
    return true;
}

Boolean System::removeDirectory(const char* path)
{
#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    return QlgRmdir((Qlg_Path_Name_T *)&pathname) == 0;
#else
    return rmdir(path) == 0;
#endif
}

Boolean System::removeFile(const char* path)
{
#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    return QlgUnlink((Qlg_Path_Name_T *)&pathname) == 0;
#else
    return unlink(path) == 0;
#endif
}

Boolean System::renameFile(const char* oldPath, const char* newPath)
{
#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT oldpathname;
    memset((void*)&oldpathname, 0x00, sizeof(OS400_PNSTRUCT));
    oldpathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(oldpathname.qlg_struct.Country_ID,"US",2);
    memcpy(oldpathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    oldpathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    oldpathname.qlg_struct.Path_Length = strlen(oldPath);
    oldpathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    oldpathname.pn = (char *)oldPath;

    OS400_PNSTRUCT newpathname;
    memset((void*)&newpathname, 0x00, sizeof(OS400_PNSTRUCT));
    newpathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(newpathname.qlg_struct.Country_ID,"US",2);
    memcpy(newpathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    newpathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    newpathname.qlg_struct.Path_Length = strlen(newPath);
    newpathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    newpathname.pn = (char *)newPath;

    if (QlgLink((Qlg_Path_Name_T *)&oldpathname,
                (Qlg_Path_Name_T *)&newpathname) != 0)
    {
        return false;
    }

    return QlgUnlink((Qlg_Path_Name_T *)&oldpathname) == 0;
#else

    if (link(oldPath, newPath) != 0)
        return false;

    return unlink(oldPath) == 0;
#endif
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
# if defined(PEGASUS_ZOS_SECURITY)
    if (!hasProgramControl(fileName))
    {
        PEG_METHOD_EXIT();
        return 0;
    }
# endif
    PEG_METHOD_EXIT();
    return DynamicLibraryHandle(dllload(fileName));
#elif defined(PEGASUS_OS_OS400)
    PEG_METHOD_EXIT();
    return DynamicLibraryHandle(OS400_LoadDynamicLibrary(fileName));
#else
    PEG_METHOD_EXIT();
    return DynamicLibraryHandle(dlopen(fileName,  RTLD_GLOBAL|RTLD_NOW));
#endif

}

void System::unloadDynamicLibrary(DynamicLibraryHandle libraryHandle)
{
    // ATTN: Should this method indicate success/failure?
#if defined(PEGASUS_OS_LINUX) || defined(PEGASUS_OS_SOLARIS) || defined(PEGASUS_OS_DARWIN)
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

#ifdef PEGASUS_OS_AIX
    dlclose(libraryHandle);
#endif

#ifdef PEGASUS_OS_ZOS
    dllfree(reinterpret_cast<dllhandle *> (libraryHandle));
#endif
}

String System::dynamicLoadError() {
    // ATTN: Is this safe in a multi-threaded process?  Should this string
    // be returned from loadDynamicLibrary?
#ifdef PEGASUS_OS_HPUX
    // If shl_load() returns NULL, errno is set to indicate the error
    return strerror(errno);
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
    static char hostname[PEGASUS_MAXHOSTNAMELEN];

    if (!*hostname)
    {
        gethostname(hostname, sizeof(hostname));
#if defined(PEGASUS_OS_OS400)
        EtoA(hostname);
#endif
    }

    return hostname;
}

String System::getFullyQualifiedHostName ()
{
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_AIX) || defined(PEGASUS_OS_LINUX) || defined(PEGASUS_OS_OS400)
    char hostName [PEGASUS_MAXHOSTNAMELEN];
    struct hostent *he;
    String fqName;

    if (gethostname (hostName, PEGASUS_MAXHOSTNAMELEN) != 0)
    {
        return String::EMPTY;
    }

    if ((he = gethostbyname (hostName)))
    {
       strcpy (hostName, he->h_name);
    }

#if defined(PEGASUS_OS_OS400)
    EtoA(hostName);
#endif

    fqName.assign (hostName);

    return fqName;
#elif defined(PEGASUS_OS_ZOS)
    char hostName [PEGASUS_MAXHOSTNAMELEN];
    char *domainName;
    String fqName;
    // receive short name of the local host
    if (gethostname(hostName, PEGASUS_MAXHOSTNAMELEN) != 0)
    {
        return String::EMPTY;
    }
    // get domain name of the local host
    domainName= __ipDomainName();
    if (domainName == 0)
    {
        return String::EMPTY;
    }
    // build fully qualified hostname
    fqName.assign(hostName);
    fqName.append(".");
    fqName.append(domainName);

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
    //
    //  The value returned should match the value of the CreationClassName key
    //  property used in the instrumentation of the CIM_ComputerSystem class
    //  as determined by the provider for the CIM_ComputerSystem class
    //
    return "CIM_ComputerSystem";
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

#ifdef PEGASUS_OS_SOLARIS
#define SERV_BUFF_SIZE 1024
    struct servent serv_result;
    char buf[SERV_BUFF_SIZE];

    if ( (serv = getservbyname_r(serviceName, TCP, &serv_result,
                                 buf, SERV_BUFF_SIZE)) != NULL )
#elif defined(PEGASUS_OS_OS400)
    struct servent serv_result;
    serv = &serv_result;
    struct servent_data buf;
    memset(&buf, 0x00, sizeof(struct servent_data));

    char srvnameEbcdic[256];
    strcpy(srvnameEbcdic, serviceName);
    AtoE(srvnameEbcdic);

    char tcpEbcdic[64];
    strcpy(tcpEbcdic, TCP);
    AtoE(tcpEbcdic);

    if ( (getservbyname_r(srvnameEbcdic, tcpEbcdic, &serv_result,
                          &buf)) == 0 )
#else // PEGASUS_OS_SOLARIS
    if ( (serv = getservbyname(serviceName, TCP)) != NULL )
#endif // PEGASUS_OS_SOLARIS
    {
        localPort = htons((uint16_t)serv->s_port);
    }
    else
    {
        localPort = defaultPort;
    }

    return localPort;
}
#if defined(PEGASUS_OS_LSB)
/*
   getpass equivalent.
   Adapted from example implementation described in GLIBC documentation
   (http://www.dusek.ch/manual/glibc/libc_32.html) and
   "Advanced Programming in the UNIX Environment" by Richard Stevens,
   pg. 350.

*/
#define MAX_PASS_LEN 1024
char *getpassword(const char *prompt)
{
  static char buf[MAX_PASS_LEN];
  struct termios old, new_val;
  char *ptr;
  int c;

  buf[0] = 0;

  /* Turn echoing off and fail if we can't. */
  if (tcgetattr (fileno (stdin), &old) != 0)
    return buf;
  new_val = old;
  new_val.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
  if (tcsetattr (fileno (stdin), TCSAFLUSH, &new_val) != 0)
    return buf;

  /* Read the password. */
  fputs (prompt, stdin);
  ptr = buf;
  while ( (c = getc(stdin)) != EOF && c != '\n') {
    if (ptr < &buf[MAX_PASS_LEN])
      *ptr++ = c;
  }
  *ptr = 0;
  putc('\n', stdin);

  /* Restore terminal. */
  (void) tcsetattr (fileno (stdin), TCSAFLUSH, &old);
  fclose(stdin);
  return buf;
}
#endif

String System::getPassword(const char* prompt)
{

    String password;

#if !defined(PEGASUS_OS_OS400)
    // Not supported on OS/400, and we don't need it.
    // 'getpass' is DEPRECATED
# if !defined(PEGASUS_OS_LSB)
    password = String(getpass( prompt ));
# else
    password = String(getpassword( prompt ));
# endif

#endif

    return password;
}

String System::getEffectiveUserName()
{
    String userName = String::EMPTY;
    struct passwd*   pwd = NULL;

#if defined(PEGASUS_OS_SOLARIS) || \
    defined(PEGASUS_OS_HPUX) || \
    defined(PEGASUS_OS_LINUX) || \
    defined(PEGASUS_OS_OS400)

    const unsigned int PWD_BUFF_SIZE = 1024;
    struct passwd       local_pwd;
    char                buf[PWD_BUFF_SIZE];

    if(getpwuid_r(geteuid(), &local_pwd, buf, PWD_BUFF_SIZE, &pwd) != 0)
    {
        String errorMsg = String("getpwuid_r failure : ") +
                            String(strerror(errno));
        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2, errorMsg);
        // L10N TODO - This message needs to be added.
        //Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::WARNING,
        //                          errorMsg);
    }
#else
    //
    //  get the currently logged in user's UID.
    //
    pwd = getpwuid(geteuid());
#endif
    if ( pwd == NULL )
    {
         // L10N TODO - This message needs to be added.
         //Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::WARNING,
         //  "getpwuid_r failure, user may have been removed just after login");
         Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL4,
             "getpwuid_r failure, user may have been removed just after login");
    }
    else
    {
#if defined(PEGASUS_OS_OS400)
        EtoA(pwd->pw_name);
#endif
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
#if defined(PEGASUS_OS_OS400)
    AtoE((char *)userName);
#endif

#if defined(PEGASUS_OS_SOLARIS) || \
    defined(PEGASUS_OS_HPUX) || \
    defined(PEGASUS_OS_LINUX) || \
    defined(PEGASUS_OS_OS400)

    const unsigned int PWD_BUFF_SIZE = 1024;
    struct passwd   pwd;
    struct passwd   *result;
    char            pwdBuffer[PWD_BUFF_SIZE];

    if (getpwnam_r(userName, &pwd, pwdBuffer, PWD_BUFF_SIZE, &result) != 0)
    {
        String errorMsg = String("getpwnam_r failure : ") +
                            String(strerror(errno));
        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2, errorMsg);
        // L10N TODO - This message needs to be added.
        //Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::WARNING,
        //                          errorMsg);
    }
    if (result == NULL)
    {
        return false;
    }
#else
    //
    //  get the password entry for the user
    //
    if  ( getpwnam(userName) == NULL )
    {
#if defined(PEGASUS_OS_OS400)
        EtoA((char *)userName);
#endif
        return false;
    }
#endif
#if defined(PEGASUS_OS_OS400)
    EtoA((char *)userName);
#endif

    return true;
}

Boolean System::isPrivilegedUser(const String& userName)
{
    //
    // Check if the given user is a privileged user
    //
#if !defined(PEGASUS_OS_OS400)
    struct passwd   pwd;
    struct passwd   *result;
    const unsigned int PWD_BUFF_SIZE = 1024;
    char            pwdBuffer[PWD_BUFF_SIZE];

    if (getpwnam_r(
          userName.getCString(), &pwd, pwdBuffer, PWD_BUFF_SIZE, &result) != 0)
    {
        String errorMsg = String("getpwnam_r failure : ") +
                            String(strerror(errno));
        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2, errorMsg);
        // L10N TODO - This message needs to be added.
        //Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::WARNING,
        //                          errorMsg);
    }

    // Check if the requested entry was found. If not return false.
    if ( result != NULL )
    {
        // Check if the uid is 0.
        if ( pwd.pw_uid == 0 )
        {
            return true;
        }
    }
    return false;

#else
    CString user = userName.getCString();
    const char * tmp = (const char *)user;
    AtoE((char *)tmp);
    return ycmCheckUserCmdAuthorities(tmp);
#endif

}

String System::getPrivilegedUserName()
{
    static String userName = String::EMPTY;

    if (userName == String::EMPTY)
    {
        struct passwd*   pwd = NULL;
#if defined(PEGASUS_OS_SOLARIS) || \
    defined(PEGASUS_OS_HPUX) || \
    defined(PEGASUS_OS_LINUX) || \
    defined(PEGASUS_OS_OS400)
        const unsigned int PWD_BUFF_SIZE = 1024;
        struct passwd   local_pwd;
        char            buf[PWD_BUFF_SIZE];

        if(getpwuid_r(0, &local_pwd, buf, PWD_BUFF_SIZE, &pwd) != 0)
        {
            String errorMsg = String("getpwuid_r failure : ") +
                            String(strerror(errno));
            PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2, errorMsg);
            // L10N TODO - This message needs to be added.
            //Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::WARNING,
            //                      errorMsg);
        }
#else
        //
        //  get the privileged user's UID.
        //
        //  (on OS/400, this is QSECOFR)
        pwd = getpwuid(0);
#endif
        if ( pwd != NULL )
        {
#if defined(PEGASUS_OS_OS400)
            EtoA(pwd->pw_name);
#endif
            //
            //  get the user name
            //
            userName.assign(pwd->pw_name);
        }
        else
        {
            Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL4,
                       "Could not find entry.");
            PEGASUS_ASSERT(0);
        }
    }

    return (userName);
}

Boolean System::isGroupMember(const char* userName, const char* groupName)
{
    struct group                        grp;
    char                                *member;
    Boolean                             retVal = false;
    const unsigned int                  PWD_BUFF_SIZE = 1024;
    const unsigned int                  GRP_BUFF_SIZE = 1024;
    struct passwd                       pwd;
    struct passwd                       *result;
    struct group                        *grpresult;
    char                                pwdBuffer[PWD_BUFF_SIZE];
    char                                grpBuffer[GRP_BUFF_SIZE];

    //
    // Search Primary group information.
    //

    // Find the entry that matches "userName"

    if (getpwnam_r(userName, &pwd, pwdBuffer, PWD_BUFF_SIZE, &result) != 0)
    {
        String errorMsg = String("getpwnam_r failure : ") +
                            String(strerror(errno));
        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2, errorMsg);
        Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::WARNING,
                                  errorMsg);
        throw InternalSystemError();
    }

    if ( result != NULL )
    {
        // User found, check for group information.
        gid_t           group_id;
        group_id = pwd.pw_gid;

        // Get the group name using group_id and compare with group passed.
        if ( getgrgid_r(group_id, &grp,
                 grpBuffer, GRP_BUFF_SIZE, &grpresult) != 0)
        {
            String errorMsg = String("getgrgid_r failure : ") +
                                 String(strerror(errno));
            PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2, errorMsg);
            Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::WARNING,
                                  errorMsg);
            throw InternalSystemError();
        }

        // Compare the user's group name to groupName.
        if ( strcmp (grp.gr_name, groupName) == 0 )
        {
             // User is a member of the group.
             return true;
        }
    }

    //
    // Search supplemental groups.
    // Get a user group entry
    //
#if defined(PEGASUS_OS_LSB)
    if ( getgrnam_r((char *)groupName, &grp,
              grpBuffer, GRP_BUFF_SIZE, &grpresult) != 0 )
#else
    if ( getgrnam_r(groupName, &grp,
              grpBuffer, GRP_BUFF_SIZE, &grpresult) != 0 )

#endif
    {
        String errorMsg = String("getgrnam_r failure : ") +
                            String(strerror(errno));
        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2, errorMsg);
        Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::WARNING,
                                  errorMsg);
        throw InternalSystemError();
    }

    // Check if the requested group was found.
    if (grpresult == NULL)
    {
        return false;
    }

    Uint32 j = 0;

    //
    // Get all the members of the group
    //
    member = grp.gr_mem[j++];

    while (member)
    {
        //
        // Check if the user is a member of the group
        //
        if ( strcmp(userName, member) == 0 )
        {
            retVal = true;
            break;
        }
        member = grp.gr_mem[j++];
    }

    return retVal;
}
#ifndef PEGASUS_OS_OS400
Boolean System::changeUserContext(const char* userName)
{
    const unsigned int PWD_BUFF_SIZE = 1024;
    struct passwd pwd;
    struct passwd *result;
    char pwdBuffer[PWD_BUFF_SIZE];

# if defined(PEGASUS_OS_OS400)
    AtoE((char *)userName);
# endif

    int rc = getpwnam_r(userName, &pwd, pwdBuffer, PWD_BUFF_SIZE, &result);

# if defined(PEGASUS_OS_OS400)
    EtoA((char *)userName);
# endif

    if (rc != 0)
    {
        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            String("getpwnam_r failed: ") + String(strerror(errno)));
        return false;
    }

    if (result == 0)
    {
        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            "getpwnam_r failed.");
        return false;
    }

    Tracer::trace(TRC_OS_ABSTRACTION, Tracer::LEVEL4,
        "Changing user context to: uid = %d, gid = %d",
        (int)pwd.pw_uid, (int)pwd.pw_gid);

    if (setgid(pwd.pw_gid) != 0)
    {
        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            String("setgid failed: ") + String(strerror(errno)));
        return false;
    }

    if (setuid(pwd.pw_uid) != 0)
    {
        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            String("setuid failed: ") + String(strerror(errno)));
        return false;
    }

    return true;
}
#endif
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
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    int fd = QlgOpen((Qlg_Path_Name_T *)&pathname, O_WRONLY);
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

// Changes file permissions on the given file.
Boolean System::changeFilePermissions(const char* path, mode_t mode)
{
    Sint32 ret = 0;

    const char * tmp = path;

#if defined(PEGASUS_OS_OS400)
    // ATTN: Update this code to handle UTF8 when path contains UTF8
    AtoE((char *)tmp);
#endif

    ret = ::chmod(tmp, mode);

    return ( ret != -1 );
}

Boolean System::verifyFileOwnership(const char* path)
{
    struct stat st;

#if defined(PEGASUS_OS_OS400)
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(path);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)path;

    if (QlgStat((Qlg_Path_Name_T *)&pathname, &st) != 0)
    {
        return false;
    }
#else
    if (lstat(path, &st) != 0)
    {
        return false;
    }
#endif

    return ((st.st_uid == geteuid()) &&    // Verify the file owner
            S_ISREG(st.st_mode) &&         // Verify it is a regular file
            (st.st_nlink == 1));           // Verify it is not a hard link
}

void System::syslog(const String& ident, Uint32 severity, const char* message)
{
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_LINUX)

    // Since the openlog(), syslog(), and closelog() function calls must be
    // coordinated (see below), we need a thread control.

    static Mutex logMutex;

    AutoMutex loglock(logMutex);

    // Get a const char* representation of the identifier string.  Note: The
    // character string passed to the openlog() function must persist until
    // closelog() is called.  The syslog() method uses this pointer directly
    // rather than a copy of the string it refers to.

    CString identCString = ident.getCString();
    openlog(identCString, LOG_PID, LOG_DAEMON);

    // Map from the Logger log level to the system log level.

    Uint32 syslogLevel;
    if (severity & Logger::FATAL)
    {
        syslogLevel = LOG_CRIT;
    }
    else if (severity & Logger::SEVERE)
    {
        syslogLevel = LOG_ERR;
    }
    else if (severity & Logger::WARNING)
    {
        syslogLevel = LOG_WARNING;
    }
    else if (severity & Logger::INFORMATION)
    {
        syslogLevel = LOG_INFO;
    }
    else // if (severity & Logger::TRACE)
    {
        syslogLevel = LOG_DEBUG;
    }

    // Write the message to the system log.

    ::syslog(syslogLevel, "%s", message);

    closelog();

#elif defined(PEGASUS_OS_OS400)

    std::string replacementData = message;
    // All messages will go to the joblog. In the future
    // some messages may go to other message queues yet
    // to be determined.
    if ((severity & Logger::TRACE) ||
        (severity & Logger::INFORMATION))
    {

        // turn into ycmMessage so we can put it in the job log
# pragma convert(37)
        ycmMessage theMessage("CPIDF80",
                              message,
                              strlen(message),
                              "Logger",
                              ycmCTLCIMID,
                              TRUE);
# pragma convert(0)

        // put the message in the joblog
        theMessage.joblogIt(UnitOfWorkError,
                            ycmMessage::Informational);
    }

    if ((severity & Logger::WARNING) ||
        (severity & Logger::SEVERE)  ||
        (severity & Logger::FATAL))
    {
        // turn into ycmMessage so we can put it in the job log
# pragma convert(37)
        ycmMessage theMessage("CPDDF82",
                              message,
                              strlen(message),
                              "Logger",
                              ycmCTLCIMID,
                              TRUE);
# pragma convert(0)
        // put the message in the joblog
        theMessage.joblogIt(UnitOfWorkError,
                            ycmMessage::Diagnostic);
    }

#endif
}

// System ID constants for Logger::put and Logger::trace
#if defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
const String System::CIMSERVER = "qycmcimom";  // Server system ID
#else
const String System::CIMSERVER = "cimserver";  // Server system ID
#endif




//
// System Initializater for AIX
//
#ifdef PEGASUS_OS_AIX
#include <cstdlib>

class SystemInitializer
{

public:
    /**
     *
     * Default constructor.
     *
     */
    SystemInitializer();
};



SystemInitializer::SystemInitializer()
{
    putenv("XPG_SUS_ENV=ON");
}

static SystemInitializer initializer;

#endif

PEGASUS_NAMESPACE_END
