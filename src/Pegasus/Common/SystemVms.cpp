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
//              Dave Rosckes (rosckes@us.ibm.com)
//              Sean Keenan (sean.keenan@hp.com)
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for PEP#101
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "System.h"

#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <time.h>
#include <lib$routines.h>
#include <sys/time.h>
#include <netdb.h>
#include <prvdef.h>
#include <descrip.h>
#include <stsdef.h>
#include <ssdef.h>
#include <starlet.h>
#include <libdef.h>
#include <cxx_exception.h>

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Config/ConfigManager.h>

PEGASUS_NAMESPACE_BEGIN

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
    {
        return false;
    }
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
    {
        return false;
    }
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
    if (rename(oldPath, newPath) != 0)
    {
        return false;
    }
    return true;
}

DynamicLibraryHandle System::loadDynamicLibrary(const char* fileName)
{
    PEG_METHOD_ENTER(TRC_OS_ABSTRACTION, "System::loadDynamicLibrary()");

    Tracer::trace(TRC_OS_ABSTRACTION, Tracer::LEVEL2, 
                  "Attempting to load library %s - 1", fileName);

    saveFileName = fileName;

    PEG_METHOD_EXIT();
    return DynamicLibraryHandle(1);
}

void System::unloadDynamicLibrary(DynamicLibraryHandle libraryHandle)
{
    // ATTN: Should this method indicate success/failure?

    dlclose(libraryHandle);
}

String System::dynamicLoadError() {
    // ATTN: Is this safe in a multi-threaded process?  Should this string
    // be returned from loadDynamicLibrary?
//    String dlerr = dlerror();
//    return dlerr;
    return String::EMPTY;
}


DynamicSymbolHandle System::loadVmsDynamicSymbol(
    const char* symbolName, 
    const char* fileName, 
    const char* vmsProviderDir)
{
  char* Errorout;
  unsigned int status;
  CString cstr;

  const char *sName = symbolName;
  const char *fName = fileName;
  const char *dName = vmsProviderDir;

  int symbolValue = 0;
  unsigned int flags = 0;

  $DESCRIPTOR(vmsFileName, "Dummy fileName");
  $DESCRIPTOR(vmsSymbolName, "Dummy symbolName");
  $DESCRIPTOR(vmsDirName, "Dummy vmsProviderDir");

  vmsFileName.dsc$b_dtype   = DSC$K_DTYPE_T;
  vmsFileName.dsc$b_class   = DSC$K_CLASS_S;
  vmsFileName.dsc$w_length  = strlen(fName);
  vmsFileName.dsc$a_pointer = (char *)fName;

  vmsSymbolName.dsc$b_dtype   = DSC$K_DTYPE_T;
  vmsSymbolName.dsc$b_class   = DSC$K_CLASS_S;
  vmsSymbolName.dsc$w_length  = strlen(sName);
  vmsSymbolName.dsc$a_pointer = (char *)sName;

  vmsDirName.dsc$b_dtype   = DSC$K_DTYPE_T;
  vmsDirName.dsc$b_class   = DSC$K_CLASS_S;
  vmsDirName.dsc$w_length     = strlen(dName);
  vmsDirName.dsc$a_pointer = (char *)dName;

//  status = lib$find_image_symbol (&vmsFileName, &vmsSymbolName, &symbolValue, &vmsDirName, flags);

  cxxl$set_condition (cxx_exception);

  try
  {
    status = lib$find_image_symbol (&vmsFileName, &vmsSymbolName, &symbolValue, &vmsDirName, flags);
  }

  catch (struct chf$signal_array &obj)
  {
    if (obj.chf$is_sig_name != LIB$_EOMWARN)
    {
      symbolValue = 0;
      cxxl$set_condition (unix_signal);
      return (DynamicSymbolHandle)symbolValue;
    }
  }

  catch (...)
  {
    symbolValue = 0;
    cxxl$set_condition (unix_signal);
    return (DynamicSymbolHandle)symbolValue;
  }

  if (!$VMS_STATUS_SUCCESS(status))
  {
    symbolValue = 0;
  }
  cxxl$set_condition (unix_signal);
  return (DynamicSymbolHandle)symbolValue;
}

DynamicSymbolHandle System::loadDynamicSymbol(
    DynamicLibraryHandle libraryHandle,
    const char* symbolName)
{
  DynamicSymbolHandle Dsh;

  char* tmp = getenv("PEGASUS_SYSSHARE");

  if (tmp == "")
  {
    throw UnrecognizedConfigProperty("PEGASUS_SYSSHARE");
  }

  String vmsProviderDir = ( tmp + saveFileName + ".exe");

  Dsh = loadVmsDynamicSymbol((const char*) symbolName, 
                             (const char*) saveFileName.getCString(),
                             (const char*) vmsProviderDir.getCString());
  return Dsh;
}

String System::getHostName()
{
    static char hostname[PEGASUS_MAXHOSTNAMELEN];

    if (!*hostname)
    {
        gethostname(hostname, sizeof(hostname));
    }
    return hostname;
}

String System::getFullyQualifiedHostName ()
{
    char hostName [PEGASUS_MAXHOSTNAMELEN];
    struct hostent *he;
    String fqName;

    if (gethostname (hostName, PEGASUS_MAXHOSTNAMELEN) != 0)
    {
        return String::EMPTY;
    }

    if (he = gethostbyname (hostName))
    {
       strcpy (hostName, he->h_name);
    }

    fqName.assign (hostName);

    return fqName;
}

String System::getSystemCreationClassName ()
{
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
    if ( (serv = getservbyname(serviceName, TCP)) != NULL )
    {
        localPort = htons((uint16_t)serv->s_port);
    }
    else
    {
        localPort = defaultPort;
    }

    return localPort;
}

String System::getPassword(const char* prompt)
{

    String password("dummy");
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
         Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL4,
             "getpwuid failure, user may have been removed just after login");
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
    return ( String("dummy") );
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

Boolean System::isPrivilegedUser(const String& userName)
{
    //
    // Check if the given user is a privileged user
    //
    int retStat;

    unsigned long int prvPrv = 0;

    retStat = sys$setprv(0, 0, 0, &prvPrv);
    if (!$VMS_STATUS_SUCCESS( retStat))
    {
      return false;
    }
    if ((PRV$M_SETPRV && prvPrv) == 1)
    {
      return true;
    }
    else
    {
      return false;
    }
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

        pwd = getpwuid(geteuid());
        if ( pwd != NULL )
        {
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

#ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION

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
        Tracer::PEG_TRACE_STRING (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                                  errorMsg);
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
            Tracer::PEG_TRACE_STRING (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                                      errorMsg);
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
    if ( getgrnam_r(groupName, &grp,
              grpBuffer, GRP_BUFF_SIZE, &grpresult) != 0 )

    {
        String errorMsg = String("getgrnam_r failure : ") +
                            String(strerror(errno));
        Tracer::PEG_TRACE_STRING (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                                  errorMsg);
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

#endif

Boolean System::changeUserContext(const char* userName)
{
    const unsigned int PWD_BUFF_SIZE = 1024;
    struct passwd pwd;
    struct passwd *result;
    char pwdBuffer[PWD_BUFF_SIZE];

    int rc = getpwnam_r(userName, &pwd, pwdBuffer, PWD_BUFF_SIZE, &result);

    if (rc != 0)
    {
        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            String("getpwnam_r failed: ") + String(strerror(errno)));
        return false;
    }

    if (result == 0)
    {
        Tracer::PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
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
    return (truncate(path, newSize) == 0);
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
    ret = chmod(tmp, mode);
    return ( ret != -1 );
}

Boolean System::verifyFileOwnership(const char* path)
{
    struct stat st;

    if (stat(path, &st) != 0)
    {
        return false;
    }
    return (st.st_uid == geteuid());
}

void System::syslog(const String& ident, Uint32 severity, const char* message)
{
    // Not implemented
}

// System ID constants for Logger::put and Logger::trace

const String System::CIMSERVER = "cimserver";  // Server system ID
    
PEGASUS_NAMESPACE_END
