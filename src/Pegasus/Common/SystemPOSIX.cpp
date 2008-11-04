//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#if defined(PEGASUS_OS_ZOS)
# define _OPEN_SYS_EXT
# include <sys/ps.h>
# include <sys/__messag.h>
#elif defined(PEGASUS_OS_VMS)
# include <descrip.h>           //  $DESCRIPTOR
# include <iodef.h>             // IO$_SENSEMODE
# include <ttdef.h>             // TT$M_NOBRDCST
# include <tt2def.h>            // TT2$M_PASTHRU
# include <starlet.h>
# include <stsdef.h>            // VMS_STATUS_SUCCESS
# include <prvdef>              // PRV$M_SETPRV
#endif

#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

#if defined(PEGASUS_OS_SOLARIS)
# include <string.h>
# include <sys/sockio.h>
#endif

#if !defined(PEGASUS_OS_VMS) && \
    !defined(PEGASUS_OS_ZOS) && \
    !defined(PEGASUS_OS_DARWIN)
# include <crypt.h>
#endif

#include "Network.h"

#if defined(PEGASUS_USE_SYSLOGS)
# include <syslog.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <time.h>
#include <sys/time.h>
#include "System.h"

#ifdef PEGASUS_OS_PASE
#include <ILEWrapper/ILEUtilities.h>
#endif

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/Mutex.h>

#include <net/if.h>

#include "Once.h"

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// System
//
//==============================================================================

// System ID constants for Logger::put and Logger::trace
#if defined(PEGASUS_OS_ZOS)
const String System::CIMSERVER = "CFZCIM";  // Server system ID
#else
const String System::CIMSERVER = "cimserver";  // Server system ID
#endif

void System::getCurrentTime(Uint32& seconds, Uint32& milliseconds)
{
    timeval tv;
    gettimeofday(&tv, 0);
    seconds = Uint32(tv.tv_sec);
    milliseconds = Uint32(tv.tv_usec) / 1000;
}

void System::getCurrentTimeUsec(Uint32& seconds, Uint32& microseconds)
{
    timeval tv;
    gettimeofday(&tv, 0);
    seconds = Uint32(tv.tv_sec);
    microseconds = Uint32(tv.tv_usec);
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

static inline void _sleep_wrapper(Uint32 seconds)
{
    sleep(seconds);
}

void System::sleep(Uint32 seconds)
{
    _sleep_wrapper(seconds);
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
    Boolean success = (rename(oldPath, newPath) == 0);

    if (!success)
    {
        PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL1,
            "rename(\"%s\", \"%s\") failed: %s",
            oldPath,
            newPath,
            (const char*) PEGASUS_SYSTEM_ERRORMSG.getCString()));
    }

    return success;
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

#if defined(PEGASUS_OS_SOLARIS)
# define SERV_BUFF_SIZE 1024
    struct servent serv_result;
    char buf[SERV_BUFF_SIZE];

    if ( (serv = getservbyname_r(serviceName, TCP, &serv_result,
                                 buf, SERV_BUFF_SIZE)) != NULL )
#elif defined(PEGASUS_OS_LINUX)
# define SERV_BUFF_SIZE 1024
    struct servent serv_result;
    char buf[SERV_BUFF_SIZE];
    int ret = getservbyname_r(
        serviceName, 
        TCP,  
        &serv_result,
        buf,
        SERV_BUFF_SIZE,
        &serv);
    if (ret == 0 && serv != NULL)
#else
    if ( (serv = getservbyname(serviceName, TCP)) != NULL )
#endif
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
#if defined(PEGASUS_OS_VMS)

    struct
    {
        short int numbuf;
        char frst_char;
        char rsv1;
        long rsv2;
    }
    tahead;

    typedef struct
    {                           // I/O status block
        short i_cond;           // Condition value
        short i_xfer;           // Transfer count
        long i_info;            // Device information
    }
    iosb;

    typedef struct
    {                           // Terminal characteristics
        char t_class;           // Terminal class
        char t_type;            // Terminal type
        short t_width;          // Terminal width in characters
        long t_mandl;           // Terminal's mode and length
        long t_extend;          // Extended terminal characteristics
    }
    termb;

    termb otermb;
    termb ntermb;

    short int ichan;            // Gets channel number for TT:

    int errorcode;
    int kbdflgs;                // saved keyboard fd flags
    int kbdpoll;                // in O_NDELAY mode
    int kbdqp = false;          // there is a char in kbdq
    int psize;                  // size of the prompt

    const size_t MAX_PASS_LEN = 32;
    static char buf[MAX_PASS_LEN];
    char kbdq;                  // char we've already read

    iosb iostatus;

    static long termset[2] = { 0, 0 };  // No terminator

    $DESCRIPTOR(inpdev, "TT");  // Terminal to use for input

    buf[0] = 0;

    ichan = 0;

    try
    {
        // Get a channel for the terminal

        errorcode = sys$assign(&inpdev,     // Device name
                               &ichan,      // Channel assigned
                               0,   // request KERNEL mode access
                               0);  // No mailbox assigned

        if (errorcode != SS$_NORMAL)
        {
            throw Exception("sys$assign failure");
        }

        // Read current terminal settings

        errorcode = sys$qiow(0,     // Wait on event flag zero
                             ichan, // Channel to input terminal
                             IO$_SENSEMODE, // Function - Sense Mode
                             &iostatus,     // Status after operation
                             0, 0,  // No AST service
                             &otermb,       // [P1] Address of Char Buffer
                             sizeof (otermb),       // [P2] Size of Char Buffer
                             0, 0, 0, 0);   // [P3] - [P6]

        if (errorcode != SS$_NORMAL)
        {
            throw Exception("sys$qiow IO$_SENSEMODE failure");
        }

        // setup new settings

        ntermb = otermb;

        // turn on passthru and nobroadcast

        ntermb.t_extend |= TT2$M_PASTHRU;
        ntermb.t_mandl |= TT$M_NOBRDCST;

        // Write out new terminal settings

        errorcode = sys$qiow(0,     // Wait on event flag zero
                             ichan, // Channel to input terminal
                             IO$_SETMODE,   // Function - Set Mode
                             &iostatus,     // Status after operation
                             0, 0,  // No AST service
                             &ntermb,       // [P1] Address of Char Buffer
                             sizeof (ntermb),       // [P2] Size of Char Buffer
                             0, 0, 0, 0);   // [P3] - [P6]

        if (errorcode != SS$_NORMAL)
        {
            throw Exception("sys$qiow IO$_SETMODE failure");
        }

        // Write a prompt, read characters from the terminal, performing no
        // editing
        // and doing no echo at all.

        psize = strlen(prompt);

        errorcode = sys$qiow(0,     // Event flag
                             ichan, // Input channel
                             IO$_READPROMPT | IO$M_NOECHO | IO$M_NOFILTR |
                             IO$M_TRMNOECHO,
                             // Read with prompt, no echo, no translate, no
                             // termination character echo
                             &iostatus,     // I/O status block
                             NULL,  // AST block (none)
                             0,     // AST parameter
                             &buf,  // P1 - input buffer
                             MAX_PASS_LEN,  // P2 - buffer length
                             0,     // P3 - ignored (timeout)
                             0,     // P4 - ignored (terminator char set)
                             prompt,        // P5 - prompt buffer
                             psize);        // P6 - prompt size

        if (errorcode != SS$_NORMAL)
        {
            throw Exception("sys$qiow IO$_READPROMPT failure:");
        }

        // Write out old terminal settings
        errorcode = sys$qiow(0,     // Wait on event flag zero
                             ichan, // Channel to input terminal
                             IO$_SETMODE,   // Function - Set Mode
                             &iostatus,     // Status after operation
                             0, 0,  // No AST service
                             &otermb,       // [P1] Address of Char Buffer
                             sizeof (otermb),       // [P2] Size of Char Buffer
                             0, 0, 0, 0);   // [P3] - [P6]

        if (errorcode != SS$_NORMAL)
        {
            throw Exception("sys$qiow IO$_SETMODE failure");
        }

        // Start new line

        const int CR = 0x0d;
        const int LF = 0x0a;
        fputc(CR, stdout);
        fputc(LF, stdout);

        // Remove the termination character
        psize = strlen(buf);
        buf[psize - 1] = 0;
    }

    catch (Exception &e)
    {
        PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL1, "%s: %s",
            (const char *) (e.getMessage()).getCString(),
            strerror(EVMSERR, errorcode)));
    }

    // Deassign the channel

    if (ichan) sys$dassgn(ichan);

    return buf;

#else /* default */

    return String(getpass(prompt));

#endif /* default */
}

String System::getEffectiveUserName()
{
#if defined(PEGASUS_OS_ZOS)
    char effective_username[9];
    __getuserid(effective_username, 9);
    __etoa_l(effective_username,9);
    return String(effective_username);
#else
    String userName;
    struct passwd* pwd = NULL;
    const unsigned int PWD_BUFF_SIZE = 1024;
    struct passwd local_pwd;
    char buf[PWD_BUFF_SIZE];

    if (getpwuid_r(geteuid(), &local_pwd, buf, PWD_BUFF_SIZE, &pwd) != 0)
    {
        PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL1,
            "getpwuid_r failure: %s", strerror(errno)));
    }
    else if (pwd == NULL)
    {
        PEG_TRACE_CSTRING(TRC_OS_ABSTRACTION, Tracer::LEVEL1,
            "getpwuid_r failure; user may have been removed");
    }
    else
    {
        //
        //  get the user name
        //
        userName.assign(pwd->pw_name);
    }

    return userName;
#endif
}

String System::encryptPassword(const char* password, const char* salt)
{
#if defined(PEGASUS_OS_VMS)
    const size_t MAX_PASS_LEN = 1024;
    char pbBuffer[MAX_PASS_LEN] = {0};
    Uint32 dwByteCount;
    char pcSalt[3] = {0};

    strncpy(pcSalt, salt, 2);
    dwByteCount = strlen(password);
    memcpy(pbBuffer, password, dwByteCount);

    for (Uint32 i=0; (i<dwByteCount) || (i>=MAX_PASS_LEN); i++)
    {
        (i%2 == 0) ? pbBuffer[i] ^= pcSalt[1] : pbBuffer[i] ^= pcSalt[0];
    }

    return String(pcSalt) + String((char *)pbBuffer);
#else
    return String(crypt(password, salt));
#endif
}

Boolean System::isSystemUser(const char* userName)
{
    const unsigned int PWD_BUFF_SIZE = 1024;
    struct passwd pwd;
    struct passwd *result;
    char pwdBuffer[PWD_BUFF_SIZE];

    if (getpwnam_r(userName, &pwd, pwdBuffer, PWD_BUFF_SIZE, &result) != 0)
    {
        PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL1,
            "getpwnam_r failure: %s", strerror(errno)));
    }

    return (result != NULL);
}

Boolean System::isPrivilegedUser(const String& userName)
{
#if defined(PEGASUS_OS_PASE)
    CString user = userName.getCString();
    // this function only can be found in PASE environment
    return umeIsPrivilegedUser((const char *)user);
    
#elif defined(PEGASUS_OS_VMS)
    static union prvdef old_priv_mask;
    static union prvdef new_priv_mask;
    char enbflg = 1; // 1 = enable
    char prmflg = 0; // 0 = life time of image only.
    int retStat;

    old_priv_mask.prv$v_sysprv = false;    // SYSPRV privilege.
    new_priv_mask.prv$v_sysprv = true;     // SYSPRV privilege.

    retStat = sys$setprv(enbflg, &new_priv_mask, prmflg, &old_priv_mask);
    if (!$VMS_STATUS_SUCCESS(retStat)) 
    {
        return false;
    }

    if (retStat == SS$_NOTALLPRIV) 
    {
        return false;
    }

    return true;
    
#else
    struct passwd   pwd;
    struct passwd   *result;
    const unsigned int PWD_BUFF_SIZE = 1024;
    char            pwdBuffer[PWD_BUFF_SIZE];

    if (getpwnam_r(
          userName.getCString(), &pwd, pwdBuffer, PWD_BUFF_SIZE, &result) != 0)
    {
        PEG_TRACE((
            TRC_OS_ABSTRACTION,
            Tracer::LEVEL1,
            "getpwnam_r failure : %s",
            strerror(errno)));
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
#endif
}

static String _privilegedUserName;
static Once _privilegedUserNameOnce = PEGASUS_ONCE_INITIALIZER;

static void _initPrivilegedUserName()
{
    struct passwd* pwd = NULL;
    const unsigned int PWD_BUFF_SIZE = 1024;
    struct passwd local_pwd;
    char buf[PWD_BUFF_SIZE];
    PEGASUS_UID_T uid;

#if defined(PEGASUS_OS_VMS)
    // 65540 = 10004 hex = [1,4] the UIC for [SYSTEM] on OpenVMS
    uid = 0x10004;
#else
    uid = 0;
#endif

    if (getpwuid_r(uid, &local_pwd, buf, PWD_BUFF_SIZE, &pwd) != 0)
    {
        PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL1,
            "getpwuid_r failure: %s", strerror(errno)));
    }
    else if (pwd == NULL)
    {
        PEG_TRACE_CSTRING(
            TRC_OS_ABSTRACTION, Tracer::LEVEL1,
            "getpwuid_r: Could not find entry.");
        PEGASUS_ASSERT(0);
    }
    else
    {
        _privilegedUserName.assign(pwd->pw_name);
    }
}

String System::getPrivilegedUserName()
{
    once(&_privilegedUserNameOnce, _initPrivilegedUserName);
    return _privilegedUserName;
}

#if !defined(PEGASUS_OS_VMS) || defined(PEGASUS_ENABLE_USERGROUP_AUTHORIZATION)

Boolean System::isGroupMember(const char* userName, const char* groupName)
{
    struct group grp;
    char* member;
    Boolean retVal = false;
    const unsigned int PWD_BUFF_SIZE = 1024;
    const unsigned int GRP_BUFF_SIZE = 1024;
    struct passwd pwd;
    struct passwd* result;
    struct group* grpresult;
    char pwdBuffer[PWD_BUFF_SIZE];
    char grpBuffer[GRP_BUFF_SIZE];

    // Search Primary group information.

    // Find the entry that matches "userName"

    if (getpwnam_r(userName, &pwd, pwdBuffer, PWD_BUFF_SIZE, &result) != 0)
    {
        String errorMsg = String("getpwnam_r failure : ") +
                            String(strerror(errno));
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
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
            Logger::put(
                Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
                errorMsg);
            throw InternalSystemError();
        }

        // Compare the user's group name to groupName.
        if (strcmp(grp.gr_name, groupName) == 0)
        {
             // User is a member of the group.
             return true;
        }
    }

    //
    // Search supplemental groups.
    // Get a user group entry
    //
    if (getgrnam_r((char *)groupName, &grp,
        grpBuffer, GRP_BUFF_SIZE, &grpresult) != 0)
    {
        String errorMsg = String("getgrnam_r failure : ") +
            String(strerror(errno));
        Logger::put(
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING, errorMsg);
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

#endif /* !PEGASUS_OS_VMS || PEGASUS_ENABLE_USERGROUP_AUTHORIZATION */

Boolean System::lookupUserId(
    const char* userName,
    PEGASUS_UID_T& uid,
    PEGASUS_GID_T& gid)
{
#if defined(PEGASUS_OS_PASE)
    if (!umeLookupUserProfile(userName))
    {
        PEG_TRACE_CSTRING(TRC_OS_ABSTRACTION, Tracer::LEVEL1,
                "umeLookupUserProfile failed.");
        return false;
    }
    return true;
#else
    const unsigned int PWD_BUFF_SIZE = 1024;
    struct passwd pwd;
    struct passwd *result;
    char pwdBuffer[PWD_BUFF_SIZE];

    int rc = getpwnam_r(userName, &pwd, pwdBuffer, PWD_BUFF_SIZE, &result);

    if (rc != 0)
    {
        PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL1,
            "getpwnam_r failure: %s", strerror(errno)));
        return false;
    }

    if (result == 0)
    {
        PEG_TRACE_CSTRING(TRC_OS_ABSTRACTION, Tracer::LEVEL1,
            "getpwnam_r failed.");
        return false;
    }

    uid = pwd.pw_uid;
    gid = pwd.pw_gid;

    return true;
#endif
}

Boolean System::changeUserContext_SingleThreaded(
    const char* userName,
    const PEGASUS_UID_T& uid,
    const PEGASUS_GID_T& gid)
{
#if defined(PEGASUS_OS_PASE)
    char required_ph[12];

    PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL4,
        "Changing user context to: username = %s", userName));

    if (!umeGetUserProfile(userName, required_ph))
        return false;

    if (!umeSwapUserProfile(required_ph))
        return false;

    umeReleaseUserProfile(required_ph);
    return true;
#else
    PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL4,
        "Changing user context to: username = %s, uid = %d, gid = %d",
        userName, (int)uid, (int)gid));

    if (setgid(gid) != 0)
    {
        PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL1,
            "setgid failed: %s",strerror(errno)));
        return false;
    }

# if !defined(PEGASUS_OS_VMS)
    // NOTE: initgroups() uses non-reentrant functions and should only be
    // called from a single-threaded process.
    if (initgroups(userName, gid) != 0)
    {
        PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            "initgroups failed: %s", strerror(errno)));
        return false;
    }
# endif

    if (setuid(uid) != 0)
    {
        PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL1,
            "setuid failed: %s", strerror(errno)));
        return false;
    }

    return true;
#endif
}

Uint32 System::getPID()
{
    return getpid();
}

Boolean System::truncateFile(
    const char* path,
    size_t newSize)
{
    return (truncate(path, newSize) == 0);
}

Boolean System::is_absolute_path(const char *path)
{
    if (path == NULL)
        return false;

    if (path[0] == '/')
        return true;

    return false;
}

Boolean System::changeFilePermissions(const char* path, mode_t mode)
{
    return chmod(path, mode) == 0;
}

Boolean System::verifyFileOwnership(const char* path)
{
    struct stat st;
    
    if (lstat(path, &st) != 0)
        return false;

    return ((st.st_uid == geteuid()) &&    // Verify the file owner
            S_ISREG(st.st_mode) &&         // Verify it is a regular file
            (st.st_nlink == 1));           // Verify it is not a hard link
}

void System::syslog(const String& ident, Uint32 severity, const char* message)
{
#if defined(PEGASUS_USE_SYSLOGS) && \
    (defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_LINUX))

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

#elif defined(PEGASUS_OS_ZOS) && defined(PEGASUS_USE_SYSLOGS)
#define ZOS_MSGID_LENGTH 11

    char*           zosMessageString;
    Uint32          messageLength = strlen(message);
    Uint32          syslogLevel = LOG_DEBUG;
    const char*     zos_msgid;

    // determine syslog level and create zos_msgid string
    if ((severity & Logger::SEVERE) || (severity & Logger::FATAL) )
    {
        syslogLevel = LOG_ERR;
        zos_msgid = "CFZ00004E: ";
    }
    else if (severity & Logger::WARNING)
    {
        syslogLevel = LOG_WARNING;
        zos_msgid = "CFZ00002W: ";
    }
    else if (severity & Logger::INFORMATION)
    {
        syslogLevel = LOG_INFO;
        zos_msgid = "CFZ00001I: ";
    }
    else
    {
        syslogLevel = LOG_DEBUG;
        zos_msgid = "CFZ00001I: ";
    }

    // we cut at 4000 characters
    // leaving room for 11 additional message characters
    // 
    if (messageLength > 4000)
        messageLength = 4000;

    // reserve memory for the message string, also prepend
    // z/OS message id CFZ* if necessary
    if ((strncmp(message, "CFZ", 3) != 0) &&
        (strncmp(message, "CEZ", 3) != 0) )
    {
        // reserve message + 11 char message prepend + 1 byte for \0 char
        zosMessageString = (char*) malloc(messageLength+ZOS_MSGID_LENGTH+1);
        memcpy(zosMessageString, zos_msgid, ZOS_MSGID_LENGTH);
        memcpy(zosMessageString+ZOS_MSGID_LENGTH, message, messageLength);
        messageLength+=ZOS_MSGID_LENGTH;
    } else
    {
        zosMessageString = (char*) malloc(messageLength+1);
        memcpy(zosMessageString, message, messageLength);
    }
    // terminate with a null character
    zosMessageString[messageLength]='\0';

    // write first to syslog, __console changes the content of
    // message string
    ::syslog(syslogLevel, "%s", zosMessageString);
    
    CString identCString = ident.getCString();
    // Issue important messages to the z/OS console
    // audit messages will go to a different syslog like place
    if (!(severity & Logger::TRACE) &&
        !(strcmp("cimserver audit",identCString) == 0))
    {
        struct __cons_msg   cons;
        int                 concmd=0;
        
        memset(&cons,0,sizeof(cons));
        cons.__format.__f1.__msg_length = messageLength;
        cons.__format.__f1.__msg = zosMessageString;
        __console(&cons, NULL, &concmd);
    }
    free(zosMessageString);

#else /* default */

    // Not implemented!

#endif /* default */
}

void System::openlog(const char *ident, int logopt, int facility)
{

#if defined(PEGASUS_USE_SYSLOGS) && \
    (defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_LINUX) || \
     defined(PEGASUS_OS_ZOS))
    ::openlog(ident, logopt, facility);
#else /* default */

    // Not implemented!

#endif /* default */

}

void System::closelog()
{

#if defined(PEGASUS_USE_SYSLOGS) && \
    (defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_LINUX) || \
     defined(PEGASUS_OS_ZOS))
    ::closelog();
#else /* default */

    // Not implemented!

#endif /* default */

}


// check if a given IP address is defined on the local network interfaces
Boolean System::isIpOnNetworkInterface(Uint32 inIP)
{
    // Function compares all IP addresses defined on
    // local network interface with a given IP address
#define PEGASUS_MAX_NETWORK_INTERFACES 32
    struct ifconf conf;

    conf.ifc_buf =
        (char *)calloc(PEGASUS_MAX_NETWORK_INTERFACES, sizeof(struct ifreq));
    conf.ifc_len = PEGASUS_MAX_NETWORK_INTERFACES * sizeof(struct ifreq);

    int sd=socket(AF_INET, SOCK_DGRAM, 0);
    int rc = ioctl(sd, SIOCGIFCONF, &conf);
    close(sd);
    if (-1 < rc)
    {
        struct ifreq* r = conf.ifc_req;
        sockaddr_in* addr;
        addr = reinterpret_cast<struct sockaddr_in*>(&r->ifr_addr);
        while (addr->sin_addr.s_addr != 0)
        {
            Uint32 ip = addr->sin_addr.s_addr;
            if (ip == inIP)
            {
                free(conf.ifc_buf);
                return true;
            }
            // next interface
            r++;
            addr = reinterpret_cast<struct sockaddr_in*>(&r->ifr_addr);
        }
    }
    free(conf.ifc_buf);
    return false;
}

String System::getErrorMSG_NLS(int errorCode,int errorCode2)
{
#if defined(PEGASUS_OS_ZOS)

    char buf[10];

    sprintf(buf,"%08X",errorCode2);

    MessageLoaderParms parms(
           "Common.System.ERROR_MESSAGE.PEGASUS_OS_ZOS",
           "$0 (error code $1, reason code 0x$2)",
           strerror(errorCode),errorCode,buf);

#else
    MessageLoaderParms parms(
           "Common.System.ERROR_MESSAGE.STANDARD",
           "$0 (error code $1)",strerror(errorCode),errorCode);

#endif

     return MessageLoader::getMessage(parms);
     
}

String System::getErrorMSG(int errorCode,int errorCode2)
{

    String buffer;

    char strErrorCode[32];
    sprintf(strErrorCode, "%d", errorCode);

    buffer.append(strerror(errorCode));
    buffer.append(" (error code ");
    buffer.append(strErrorCode);

#if defined(PEGASUS_OS_ZOS)
    char strErrorCode2[10];
    sprintf(strErrorCode2,"%08X",errorCode2);
    buffer.append(", reason code 0x");
    buffer.append(strErrorCode2);
#endif

    buffer.append(")");

    return buffer;
}



///////////////////////////////////////////////////////////////////////////////
// AutoFileLock class
///////////////////////////////////////////////////////////////////////////////

AutoFileLock::AutoFileLock(const char* fileName)
{
#ifdef PEGASUS_OS_TYPE_UNIX
    _fl.l_type = F_WRLCK;
    _fl.l_whence = SEEK_SET;
    _fl.l_start = 0;
    _fl.l_len = 0;
    _fl.l_pid = getpid();

    do
    {
        _fd = open(fileName, O_WRONLY);
    } while ((_fd == -1) && (errno == EINTR));

    if (_fd != -1)
    {
        int rc;

        do
        {
            rc = fcntl(_fd, F_SETLKW, &_fl);
        } while ((rc == -1) && (errno == EINTR));

        if (rc == -1)
        {
            PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "AutoFileLock: Failed to lock file '%s', error code %d.",
                fileName, errno));
            _fd = -1;
        }
    }
    else
    {
        PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "AutoFileLock: Failed to open lock file '%s', error code %d.",
            fileName, errno));
    }
#endif
}

AutoFileLock::~AutoFileLock()
{
#ifdef PEGASUS_OS_TYPE_UNIX
    if (_fd != -1)
    {
        _fl.l_type = F_UNLCK;
        int rc = fcntl(_fd, F_SETLK, &_fl);
        if (rc == -1)
        {
            PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "AutoFileLock: Failed to unlock file, error code %d.",
                errno));
        }
        close(_fd);
    }
#endif
}

#if defined(PEGASUS_OS_PASE)
class PaseSystemInitializer
{
    public:
        PaseSystemInitializer()
        {
            putenv("XPG_SUS_ENV=ON");
        };
};

static PaseSystemInitializer initializer;
#endif

//==============================================================================
//
// PEGASUS_OS_AIX
//
//==============================================================================

// System Initializater for AIX
#if defined(PEGASUS_OS_AIX)
# include <cstdlib>

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

#include <sys/ioctl.h>
#include <net/if.h>
#define AIX_MAX(x,y) ((x) > (y) ? (x) : (y))
#define AIX_SIZE(p) AIX_MAX((p).sa_len, sizeof(p))

#endif /* PEGASUS_OS_AIX */

Array<String> System::getInterfaceAddrs()
{
    Array<String> ips;

#ifdef PEGASUS_ENABLE_IPV6

#if defined(PEGASUS_HAS_GETIFADDRS)
    struct ifaddrs *array, *addrs;
    char buff[PEGASUS_INET6_ADDRSTR_LEN];

    if (0 > getifaddrs(&array))
    {
        return ips;
    }
    Boolean ipFound;
    for( addrs = array; addrs != NULL; addrs = addrs->ifa_next)
    {
        ipFound = false;
        switch(addrs->ifa_addr->sa_family)
        {
            case AF_INET :
                // Don't gather loop back addrs.
                if (System::isLoopBack(AF_INET, 
                    &((struct sockaddr_in *)addrs->ifa_addr)->sin_addr))
                {
                    continue;
                }
                HostAddress::convertBinaryToText(AF_INET, 
                    &((struct sockaddr_in *)addrs->ifa_addr)->sin_addr,
                    buff, sizeof(buff));
                ipFound = true;
                break;
            case AF_INET6 :
                if (System::isLoopBack(AF_INET6,
                    &((struct sockaddr_in6 *)addrs->ifa_addr)->sin6_addr))
                {
                    continue;
                }
                HostAddress::convertBinaryToText(AF_INET6,
                    &((struct sockaddr_in6 *)addrs->ifa_addr)->sin6_addr,
                    buff, sizeof(buff));
                ipFound = true;
                break;
        }
        //ATTN: Some Linux implementations of getifaddrs() returning duplicate
        // IPv6 Link-local unicast addresses. Filter the duplicates IPs.
        if (ipFound)
        {
            Boolean duplicateFound = false;
            for (Uint32 i = 0, n = ips.size(); i < n ; ++i)
            {
                if (String::equal(ips[i], buff))
                {
                    duplicateFound = true;
                    break;
                }
            }
            if (!duplicateFound)
            {
                 ips.append(buff);
            }
        }
    }
    if(array)
    {
        freeifaddrs(array);
    }
#elif defined(PEGASUS_OS_AIX)
    struct ifconf ifc;
    SocketHandle sd=socket(AF_INET6, SOCK_DGRAM, 0);
    // Use an AutoPtr to ensure the socket handle is closed on exception
    AutoPtr<SocketHandle, CloseSocketHandle> sockPtr(&sd);
    int bsz=sizeof(struct ifreq);
    int prevsz=bsz;
    ifc.ifc_req=0;
    ifc.ifc_len=bsz;
    do
    {
        ifc.ifc_req=(struct ifreq *)realloc(ifc.ifc_req, bsz);
        if (!ifc.ifc_req)
        {
            return ips;
        }
        ifc.ifc_len=bsz;
        if (ioctl(sd, SIOCGIFCONF, (caddr_t)&ifc) == -1)
        {
            free(ifc.ifc_req);
            return ips;
        }
        if (prevsz==ifc.ifc_len)
        {
             break;
        }
        else
        {
            bsz*=2;
            prevsz=(0==ifc.ifc_len ? bsz : ifc.ifc_len);
        }
    } while (1);

    ifc.ifc_req=(struct ifreq *)realloc(ifc.ifc_req, prevsz);

    struct sockaddr *sa;
    char *cp, *cplim, buff[PEGASUS_INET6_ADDRSTR_LEN];;
    struct ifreq *ifr=ifc.ifc_req;
    cp=(char *)ifc.ifc_req;
    cplim=cp+ifc.ifc_len;
    for (; cp<cplim; cp+=(sizeof(ifr->ifr_name) + AIX_SIZE(ifr->ifr_addr)))
    {
        ifr=(struct ifreq *)cp;
        sa=(struct sockaddr *)&(ifr->ifr_addr);
        switch(sa->sa_family)
        {
            case AF_INET:
                if (System::isLoopBack(
                        AF_INET, &(((struct sockaddr_in *)sa)->sin_addr)))
                {
                    break;
                }
                HostAddress::convertBinaryToText(
                    AF_INET,
                    &((struct sockaddr_in *)sa)->sin_addr,
                    buff,
                    sizeof(buff));

                ips.append(buff);
                break;

            case AF_INET6:
                if (System::isLoopBack(
                        AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr)))
                {
                    break;
                }
                HostAddress::convertBinaryToText(
                    AF_INET6,
                    &(((struct sockaddr_in6 *)sa)->sin6_addr),
                    buff,
                    sizeof(buff));

                ips.append(buff);
                break;
        }
    }
    
    free(ifc.ifc_req);

#elif defined(PEGASUS_OS_ZOS)
//ATTN: implement for ZOS
#elif defined(PEGASUS_OS_HPUX)
//ATTN: implement for HPUX
#elif defined(PEGASUS_OS_VMS)
//ATTN: implement for VMS
#else
//ATTN: implement for rest of UNIX flavors.
#endif

#endif // PEGASUS_ENABLE_IPV6

    return ips;
}


PEGASUS_NAMESPACE_END
