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

#define _XOPEN_SOURCE_EXTENDED 1
#include "Executor.h"
#include <Pegasus/Common/Constants.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pwd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <signal.h>
#include <stdarg.h>
#include <syslog.h>
#include <dirent.h>
#include <Pegasus/Security/Cimservera/Strlcpy.h>
#include <Pegasus/Security/Cimservera/Strlcat.h>

#if defined(PEGASUS_OS_HPUX)
# include <sys/pstat.h>
#endif

#if defined(PEGASUS_PAM_AUTHENTICATION)
#include <Pegasus/Security/Cimservera/cimservera.h>
#endif

//==============================================================================
//
// TRACE
//
//     Macro used for debug tracing.
//
//==============================================================================

#define TRACE printf("CIMEXECUTOR: TRACE: %s(%d)\n", __FILE__, __LINE__)

//==============================================================================
//
// LOG_TRACE
//
//     Macro used for debug tracing.
//
//==============================================================================

#define LOG_TRACE Log(LL_TRACE, "TRACE: %s(%d)\n", __FILE__, __LINE__)

//==============================================================================
//
// FL
//
//     Shorthand macro for passing __FILE__ and __LINE__ arguments to a
//     function.
//
//==============================================================================

#define FL __FILE__, __LINE__

//==============================================================================
//
// CIMSERVERMAIN
//
//     The name of the main CIM server program.
//
//==============================================================================

#define CIMSERVERMAIN "cimservermain"

//==============================================================================
//
// CIMSHUTDOWN
//
//     The name of the main CIM shutdown program.
//
//==============================================================================

#define CIMSHUTDOWN "cimshutdown"

//==============================================================================
//
// CIMPROVAGT
//
//     The name of the provider agent program.
//
//==============================================================================

#define CIMPROVAGT "cimprovagt"

//==============================================================================
//
// STRLCPY()
//
//     Buffer-overrun-checked version of Strlcpy().
//
//==============================================================================

#define STRLCPY(DEST, SRC, DEST_SIZE) \
    do \
    { \
        char* dest = (DEST); \
        const char* src = (SRC); \
        const size_t n = (DEST_SIZE); \
        if (Strlcpy(dest, src, n) >= n) \
            Fatal(FL, \
                "%s(%d): buffer overrun in STRLCPY()", __FILE__, __LINE__); \
    } \
    while (0)

//==============================================================================
//
// STRLCAT()
//
//     Buffer-overrun-checked version of Strlcat().
//
//==============================================================================

#define STRLCAT(DEST, SRC, DEST_SIZE) \
    do \
    { \
        char* dest = (DEST); \
        const char* src = (SRC); \
        const size_t n = (DEST_SIZE); \
        if (Strlcat(dest, src, n) >= n) \
            Fatal(FL, \
                "%s(%d): buffer overrun in STRLCAT()", __FILE__, __LINE__); \
    } \
    while (0)

//==============================================================================
//
// arg0
//
//     Same as the argv[0] parameter passed to main program.
//
//==============================================================================

static const char* arg0;

//==============================================================================
//
// Child process information (cimservermain process)
//
//     _childPid
//     _childUid
//     _childGid
//
//==============================================================================

static int _childPid;
static int _childUid;
static int _childGid;

//==============================================================================
//
// SetBit()
//
//     Set the n-th bit the the given mask.
//
//==============================================================================

inline void SetBit(unsigned long& mask, int n)
{
    mask |= (1 << n);
}

//==============================================================================
//
// ClrBit()
//
//     Clear the n-th bit the the given mask.
//
//==============================================================================

inline void ClrBit(unsigned long& mask, int n)
{
    mask &= ~(1 << n);
}

//==============================================================================
//
// TstBit()
//
//     Test the n-th bit the the given mask.
//
//==============================================================================

inline bool TstBit(unsigned long mask, int n)
{
    return mask & (1 << n);
}

//==============================================================================
//
// SigHandler()
//
//     Signal handler for SIGTERM.
//
//==============================================================================

static unsigned long _signalMask = 0;

void SigHandler(int signum)
{
    SetBit(_signalMask, signum);
}

//==============================================================================
//
// _shutdownFlag
//
//     This flag indicates that the cimservermain process is shutting down.
//     This flag is set when "cimserver -s" used.
//
//==============================================================================

static bool _shutdownFlag = false;

//==============================================================================
//
// OpenLog()
//
//     Opens a session with the SYSLOG facility.
//
//==============================================================================

static void OpenLog(bool perror, const char* program)
{
    int option = LOG_PID;

    if (perror)
        option |= LOG_PERROR;

    openlog(program, option, LOG_DAEMON);
}

//==============================================================================
//
// LogLevel
//
//     These tags map to the Pegasus log types (see Pegasus/Common/Logger.h).
//
//==============================================================================

enum LogLevel
{
    LL_FATAL,
    LL_SEVERE,
    LL_WARNING,
    LL_INFORMATION,
    LL_TRACE,
};

//==============================================================================
//
// Log()
//
//     Sends a log message to the SYSLOG facility.
//
//==============================================================================

static LogLevel _logLevel = LL_INFORMATION;

static int _logPriorities[] =
{
    LOG_ALERT, // LL_FATAL,
    LOG_CRIT, // LL_SEVERE
    LOG_WARNING, // LL_WARNING
    LOG_NOTICE, // LL_INFORMATION
    LOG_INFO, // LL_TRACE
};

static void Log(LogLevel type, const char *format, ...)
{
    // This array maps Pegasus "log levels" to syslog priorities.

    if ((int)type <= (int)_logLevel)
    {
        va_list ap;
        va_start(ap, format);
        vsyslog(_logPriorities[(int)type], format, ap);
        va_end(ap);
    }
}

//==============================================================================
//
// Exit()
//
//     The executor exit routine, which kills the cimservermain process.
//
//==============================================================================

static void Exit(int status)
{
    Log(LL_INFORMATION, "exiting");

    // Kill cimservermain.

    if (_childPid > 0)
        kill(_childPid, SIGTERM);

    exit(status);
}

//==============================================================================
//
// Fatal()
//
//     Report fatal errors. The callar set fatal_file and fatal_line before
//     calling this function. Note that since this is a single threaded
//     application, there is no attempt to synchronize access to these
//     globals.
//
//==============================================================================

static void Fatal(const char* file, size_t line, const char* format, ...)
{
    Log(LL_FATAL, "%s(%d): Fatal() called", file, int(line));

    {
        va_list ap;
        va_start(ap, format);
        vsyslog(LOG_CRIT, format, ap);
        va_end(ap);
    }

    {
        fprintf(stderr, "%s: %s(%d): ", arg0, file, (int)line);
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
        fputc('\n', stderr);
    }

    Exit(1);
}

//==============================================================================
//
// CloseOnExec()
//
//     Direct the kernel not to keep the given file descriptor open across
//     exec() system call.
//
//==============================================================================

static inline int CloseOnExec(int fd)
{
    return fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
}

//==============================================================================
//
// SetNonBlocking()
//
//     Set the given socket into non-blocking mode.
//
//==============================================================================

int SetNonBlocking(int sock)
{
    return fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
}

//==============================================================================
//
// WaitForReadEnable()
//
//     Wait until the given socket is read-enabled. Returns 1 if read enabled
//     and 0 on timed out.
//
//==============================================================================

int WaitForReadEnable(int sock, long timeoutMsec)
{
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sock, &readSet);

    struct timeval timeout;
    timeout.tv_sec = timeoutMsec / 1000;
    timeout.tv_usec = (timeoutMsec % 1000) * 1000;

    return select(sock + 1, &readSet, 0, 0, &timeout);
}

//==============================================================================
//
// WaitForWriteEnable()
//
//     Wait until the given socket is write-enabled. Returns 1 if write enabled
//     and 0 on timed out.
//
//==============================================================================

int WaitForWriteEnable(int sock, long timeoutMsec)
{
    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(sock, &writeSet);

    struct timeval timeout;
    timeout.tv_sec = timeoutMsec / 1000;
    timeout.tv_usec = (timeoutMsec % 1000) * 1000;

    return select(sock + 1, 0, &writeSet, 0, &timeout);
}

//==============================================================================
//
// RecvNonBlock()
//
//     Receive at least size bytes from the given non-blocking socket.
//
//==============================================================================

static ssize_t RecvNonBlock(int sock, void* buffer, size_t size)
{
    const long TIMEOUT_MSEC = 250;
    size_t r = size;
    char* p = (char*)buffer;

    if (size == 0)
        return -1;

    while (r)
    {
        int status = WaitForReadEnable(sock, TIMEOUT_MSEC);

        if (TstBit(_signalMask, SIGTERM) || TstBit(_signalMask, SIGINT))
        {
            // Exit on either of these signals.
            Exit(0);
        }

        if (status == 0)
            continue;

        ssize_t n;
        EXECUTOR_RESTART(read(sock, p, r), n);

        if (n == -1 && errno == EINTR)
            continue;

        if (n == -1)
        {
            if (errno == EWOULDBLOCK)
            {
                size_t total = size - r;

                if (total)
                    return total;

                return -1;
            }
            else
                return -1;
        }
        else if (n == 0)
            return size - r;

        r -= n;
        p += n;
    }

    return size - r;
}

//==============================================================================
//
// SendNonBlock()
//
//     Sends at least size bytes on the given non-blocking socket.
//
//==============================================================================

static ssize_t SendNonBlock(int sock, const void* buffer, size_t size)
{
    const long TIMEOUT_MSEC = 250;
    size_t r = size;
    char* p = (char*)buffer;

    while (r)
    {
        // ATTN: handle this or not?
        int status = WaitForWriteEnable(sock, TIMEOUT_MSEC);

        if (TstBit(_signalMask, SIGTERM) || TstBit(_signalMask, SIGINT))
        {
            // Exit on either of these signals.
            Exit(0);
        }

        if (status == 0)
            continue;

        ssize_t n;
        EXECUTOR_RESTART(write(sock, p, r), n);

        if (n == -1)
        {
            if (errno == EWOULDBLOCK)
                return size - r;
            else 
                return -1;
        }
        else if (n == 0)
            return size - r;

        r -= n;
        p += n;
    }

    return size - r;
}

//==============================================================================
//
// SendDescriptorArray()
//
//     Send an array of descriptors (file, socket, pipe) to the child process. 
//
//==============================================================================

static ssize_t SendDescriptorArray(int sock, int descriptors[], size_t count)
{
    // Allocate space for control header plus descriptors.

    size_t size = CMSG_SPACE(sizeof(int) * count);
    char* data = (char*)malloc(size);

    // Initialize msghdr struct to refer to control data.

    struct msghdr mh;
    memset(&mh, 0, sizeof(mh));
    mh.msg_control = data;
    mh.msg_controllen = size;

    // Fill in the control data struct with the descriptor and other fields.

    struct cmsghdr* cmh = CMSG_FIRSTHDR(&mh);
    cmh->cmsg_len = CMSG_LEN(sizeof(int) * count);
    cmh->cmsg_level = SOL_SOCKET;
    cmh->cmsg_type = SCM_RIGHTS;
    memcpy((int*)CMSG_DATA(cmh), descriptors, sizeof(int) * count);

    // Prepare to send single dummy byte. It will not be used but we must send
    // at least one byte otherwise the call will fail on some platforms.

    struct iovec iov[1];
    memset(iov, 0, sizeof(iov));

    char dummy = '\0';
    iov[0].iov_base = &dummy;
    iov[0].iov_len = 1;
    mh.msg_iov = iov;
    mh.msg_iovlen = 1;

    // Send message to child.

    int result = sendmsg(sock, &mh, 0);
    free(data);
    return result;
}

//==============================================================================
//
// GetHomedPath()
//
//     Get the absolute path of the given named file or directory. If already
//     absolute it just returns. Otherwise, it prepends the PEGASUS_HOME
//     environment variable.
//
//==============================================================================

static int GetHomedPath(
    const char* name,
    char path[EXECUTOR_BUFFER_SIZE])
{
    // If absolute, then use the name as is.

    if (name[0] == '/')
    {
        STRLCPY(path, name, EXECUTOR_BUFFER_SIZE);
        return 0;
    }

    // Use PEGASUS_HOME to 

    const char* home = getenv("PEGASUS_HOME");

    if (!home)
        return -1;

    STRLCPY(path, home, EXECUTOR_BUFFER_SIZE);
    STRLCAT(path, "/", EXECUTOR_BUFFER_SIZE);
    STRLCAT(path, name, EXECUTOR_BUFFER_SIZE);

    return 0;
}

//==============================================================================
//
// ChangeDirOwnerRecursive()
//
//==============================================================================

static void ChangeDirOwnerRecursive(
    const char* path,
    int uid,
    int gid)
{
    // Change permission of this direcotry.

    if (chown(path, uid, gid) != 0)
        Fatal(FL, "chown(%s, %d, %d) failed", path, uid, gid);

    // Open directory:

    DIR* dir = opendir(path);

    if (dir == NULL)
        Fatal(FL, "opendir(%s) failed", path);

    // For each node in this directory:

    dirent* ent;
    
    while ((ent = readdir(dir)) != NULL)
    {
        // Skip over "." and ".."

        const char* name = ent->d_name;

        if (strcmp(name, ".")  == 0 || strcmp(name, "..") == 0)
            continue;

        // Build full path name for this file:

        char buffer[EXECUTOR_BUFFER_SIZE];
        STRLCPY(buffer, path, EXECUTOR_BUFFER_SIZE);
        STRLCAT(buffer, "/", EXECUTOR_BUFFER_SIZE);
        STRLCAT(buffer, name, EXECUTOR_BUFFER_SIZE);

        // Determine file type (skip soft links and directories).

        struct stat st;

        if (lstat(buffer, &st) == -1)
            Fatal(FL, "lstat(%s) failed", buffer);

        // If it's a directory, save the name:

        if (S_ISDIR(st.st_mode))
        {
            ChangeDirOwnerRecursive(buffer, uid, gid);
            continue;
        }

        // Skip soft links:

        if (S_ISLNK(st.st_mode))
            continue;

        // Process the current file.

        if (chown(buffer, uid, gid) != 0)
            Fatal(FL, "chown(%s, %d, %d) failed", buffer, uid, gid);
    }

    // Close this directory:

    closedir(dir);
}

//==============================================================================
//
// GetPegasusInternalBinDir()
//
//     Get the Pegasus "lbin" directory. This is the directory that contains
//     internal Pegasus programs. Note that administrative tools are contained
//     in the "sbin" directory.
//
//==============================================================================

static void GetPegasusInternalBinDir(char path[EXECUTOR_BUFFER_SIZE])
{
    // Make a copy of PEGASUS_PROVIDER_AGENT_PROC_NAME:

    char buffer[EXECUTOR_BUFFER_SIZE];
    STRLCPY(buffer, PEGASUS_PROVIDER_AGENT_PROC_NAME, sizeof(buffer));

    // Remove "cimprovagt" suffix.

    char* p = strrchr(buffer, '/');

    if (!p)
        p = buffer;

    *p = '\0';

    // If buffer path absolute, use this.

    if (buffer[0] == '/')
    {
        STRLCAT(path, buffer, EXECUTOR_BUFFER_SIZE);
    }
    else
    {
        // Prefix with PEGASUS_HOME environment variable.

        const char* home = getenv("PEGASUS_HOME");

        if (!home)
            Fatal(FL, "Failed to locate the internal Pegasus bin directory");

        STRLCPY(path, home, EXECUTOR_BUFFER_SIZE);
        STRLCAT(path, "/", EXECUTOR_BUFFER_SIZE);
        STRLCAT(path, buffer, EXECUTOR_BUFFER_SIZE);
    }

    // Fail if no such directory.

    struct stat st;

    if (stat(path, &st) != 0)
        Fatal(FL, "Failed to stat \"%s\"", path);

    if (!S_ISDIR(st.st_mode))
        Fatal(FL, "Not a directory \"%s\"", path);
}

//==============================================================================
//
// AccessDir()
//
//     Returns 0 if able to stat given path and it is a directory.
//
//==============================================================================

int AccessDir(const char* path)
{
    struct stat st;

    if (stat(path, &st) != 0)
        return -1;

    if (!S_ISDIR(st.st_mode))
        return -1;

    return 0;
}

//==============================================================================
//
// GetInternalPegasusProgramPath()
//
//     Get the full path name of the given program.
//
//==============================================================================

static void GetInternalPegasusProgramPath(
    const char* program,
    char path[EXECUTOR_BUFFER_SIZE])
{
    GetPegasusInternalBinDir(path);
    STRLCAT(path, "/", EXECUTOR_BUFFER_SIZE);
    STRLCAT(path, program, EXECUTOR_BUFFER_SIZE);
}

//==============================================================================
//
// GetUserInfo()
//
//     Lookup the given user's uid and gid.
//
//==============================================================================

static int GetUserInfo(const char* user, int& uid, int& gid)
{
    struct passwd pwd;
    const unsigned int PWD_BUFF_SIZE = 4096;
    char buffer[PWD_BUFF_SIZE];
    struct passwd* ptr = 0;

    if (getpwnam_r(user, &pwd, buffer, PWD_BUFF_SIZE, &ptr) != 0 || !ptr)
    {
        Log(LL_TRACE, "getpwnam_r(%s, ...) failed", user);
        return -1;
    }

    uid = ptr->pw_uid;
    gid = ptr->pw_gid;

    return 0;
}

//==============================================================================
//
// GetUserName()
//
//     Lookup the given user's uid and gid.
//
//==============================================================================

static int GetUserName(int uid, char username[EXECUTOR_BUFFER_SIZE])
{
    struct passwd pwd;
    const unsigned int PWD_BUFF_SIZE = 4096;
    char buffer[PWD_BUFF_SIZE];
    struct passwd* ptr = 0;

    if (getpwuid_r(uid, &pwd, buffer, PWD_BUFF_SIZE, &ptr) != 0 || !ptr)
    {
        Log(LL_TRACE, "getpwuid_r(%d, ...) failed", uid);
        return -1;
    }

    STRLCPY(username, ptr->pw_name, EXECUTOR_BUFFER_SIZE);
    return 0;
}

//==============================================================================
//
// ChangeOwner()
//
//     Change the given file's owner.
//
//==============================================================================

static int ChangeOwner(const char* path, const char* owner)
{
    int uid;
    int gid;

    if (GetUserInfo(owner, uid, gid) != 0)
        return -1;

    if (chown(path, uid, gid) != 0)
    {
        Log(LL_TRACE, "chown(%s, %d, %d) failed", path, uid, gid);
        return -1;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////
//// REQUESTS FOLLOW
////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//==============================================================================
//
// FindCommandLineOption()
//
//     Attempt to find a command line config option of the form name=value. 
//     For example: cimservermain repositoryDir=/opt/pegasus/repository. 
//     Return 0 if found.
//
//==============================================================================

static int FindCommandLineOption(
    int argc,
    char** argv,
    const char* name,
    char value[EXECUTOR_BUFFER_SIZE])
{
    size_t n = strlen(name);

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], name, n) == 0 && argv[i][n] == '=')
        {
            const char* p = argv[i] + n + 1;
            STRLCPY(value, argv[i] + n + 1, EXECUTOR_BUFFER_SIZE);
            return 0;
        }
    }

    return -1;
}

//==============================================================================
//
// FindConfigFileOption()
//
//     Attempt to find the named option in the configuration file. If found,
//     set value and return 0.
//
//==============================================================================

static int FindConfigFileOption(
    const char* path,
    const char* name,
    char value[EXECUTOR_BUFFER_SIZE])
{
    FILE* is = fopen(path, "r");

    if (!is)
        return -1;

    char buffer[EXECUTOR_BUFFER_SIZE];
    const size_t n = strlen(name);

    while (fgets(buffer, sizeof(buffer), is) != 0)
    {
        // Skip comments.

        if (buffer[0] == '#')
            continue;

        // Remove trailing whitespace.

        size_t r = strlen(buffer);

        while (r--)
        {
            if (isspace(buffer[r]))
                buffer[r] = '\0';
        }

        // Skip blank lines.

        if (buffer[0] == '\0')
            continue;

        // Check option.

        if (strncmp(buffer, name, n) == 0 &&  buffer[n] == '=')
        {
            STRLCPY(value, buffer + n + 1, EXECUTOR_BUFFER_SIZE);
            fclose(is);
            return 0;
        }
    }

    // Not found!
    fclose(is);
    return -1;
}

//==============================================================================
//
// FindConfigOption()
//
//     Attempt to find a configuration setting for the given name. First,
//     search the command line and then the config file.
//
//==============================================================================

static int FindConfigOption(
    int argc,
    char** argv,
    const char* name,
    char value[EXECUTOR_BUFFER_SIZE])
{
    // (1) First check command line.

    if (FindCommandLineOption(argc, argv, name, value) == 0)
        return 0;

    // (2) Next check config file.

    // ATTN: Is this right. Should we check the current or the planned?

    char path[EXECUTOR_BUFFER_SIZE];

    if (GetHomedPath(PEGASUS_PLANNED_CONFIG_FILE_PATH, path) == 0 &&
        FindConfigFileOption(path, name, value) == 0)
        return 0;

    // Not found!
    return -1;
}

//==============================================================================
//
// LocateRepositoryDirectory()
//
//==============================================================================

static int LocateRepositoryDirectory(
    int argc, 
    char** argv, 
    char path[EXECUTOR_BUFFER_SIZE])
{
    if (FindConfigOption(argc, argv, "repositoryDir", path) == 0)
        return 0;

    if (GetHomedPath(PEGASUS_REPOSITORY_DIR, path) == 0)
        return 0;

    // Not found!
    return -1;
}

//==============================================================================
//
// GetServerUser
//
//     Determine which user to run cimservermain as.
//
//==============================================================================

int GetServerUser(
    int argc,
    char** argv,
    char path[EXECUTOR_BUFFER_SIZE], 
    int& uid, 
    int& gid)
{
    // (1) First try to find serverUser configuration option.

    char user[EXECUTOR_BUFFER_SIZE];

    if (FindConfigOption(argc, argv, "serverUser", user) == 0)
    {
        if (GetUserInfo(user, uid, gid) == 0)
            return 0;

        Fatal(FL, "serverUser option specifies unknown user: %s", user);
        return -1;
    }

    // (2) Now just use the owner of the cimservermain program.

    struct stat st;

    if (stat(path, &st) != 0)
        Fatal(FL, "stat(%s) failed", path);

    if (st.st_uid != 0 && st.st_gid != 0)
    {
        uid = st.st_uid;
        gid = st.st_gid;
        return 0;
    }

    // (3) Try the "pegasus" user (the default).

    const char DEFAULT_SERVER_USER[] = "pegasus";

    if (GetUserInfo(DEFAULT_SERVER_USER, uid, gid) == 0 && 
        uid != 0 && 
        gid != 0)
    {
        return 0;
    }

    Fatal(FL, 
        "cannot determine server user (used to run cimserermain). "
        "Please specify this value in one of three ways. (1) pass "
        "serverUser=<username> on the command line, (2) use cimconfig to "
        "set serverUser (using -p -s options), or (3) make the desired "
        "user the owner of %s (i.e., use chown).", path);

    return -1;
}

//==============================================================================
//
// HandlePingRequest()
//
//     Handle ping request.
//
//==============================================================================

static void HandlePingRequest(int sock)
{
    Log(LL_TRACE, "HandlePingRequest()");

    ExecutorPingResponse response = { EXECUTOR_PING_MAGIC };

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// HandleOpenFileRequest()
//
//     Handle a request from a child to open a file.
//
//==============================================================================

static void HandleOpenFileRequest(int sock)
{
    // Read the request request.

    struct ExecutorOpenFileRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Open the file.

    Log(LL_TRACE, "HandleOpenFileRequest(): path=%s", request.path);

    int fd = -1;

    switch (request.mode)
    {
        case 'r':
            fd = open(request.path, O_RDONLY);
            break;

        case 'w':
            fd = open(
                request.path, 
                O_WRONLY | O_CREAT | O_TRUNC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            break;
    }

    // Send response message.

    struct ExecutorOpenFileResponse response;
    memset(&response, 0, sizeof(response));

    if (fd == -1)
    {
        Log(LL_WARNING, "open(%s, %c) failed", request.path, request.mode);
        response.status = -1;
    }
    else
        response.status = 0;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");

    // Send descriptor to calling process (if any to send).

    if (fd != -1)
    {
        int descriptors[1];
        descriptors[0] = fd;
        SendDescriptorArray(sock, descriptors, 1);
        close(fd);
    }
}

//==============================================================================
//
// HandleStartProviderAgentRequest()
//
//==============================================================================

static void HandleStartProviderAgentRequest(int sock)
{
    // Read request.

    struct ExecutorStartProviderAgentRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    Log(LL_TRACE, "HandleStartProviderAgentRequest(): module=%s gid=%d uid=%d",
        request.module, request.gid, request.uid);

    // Map cimservermain user to root to preserve pre-privilege-separation
    // behavior.

    if (request.uid == _childUid)
    {
        Log(LL_TRACE, 
            "using root instead of cimservermain user for cimprovagt");

        request.uid = 0;
        request.gid = 0;
    }

    // Process request.

    int status = 0;
    int pid = -1;
    int to[2];
    int from[2];

    do
    {
        // Resolve full path of "cimprovagt".

        char path[EXECUTOR_BUFFER_SIZE];
        GetInternalPegasusProgramPath(CIMPROVAGT, path);

        // Create "to-agent" pipe:

        if (pipe(to) != 0)
        {
            status = -1;
            break;
        }

        // Create "from-agent" pipe:

        if (pipe(from) != 0)
        {
            status = -1;
            break;
        }

        // Fork process:

        pid = fork();

        if (pid < 0)
        {
            // ATTN: log this.
            status = -1;
            break;
        }

        // If child:

        if (pid == 0)
        {
            // Close unused pipe descriptors:

            close(to[1]);
            close(from[0]);

            // Close unused descriptors. Leave stdin, stdout, stderr, and the
            // child's pipe descriptors open.

            struct rlimit rlim;

            if (getrlimit(RLIMIT_NOFILE, &rlim) == 0)
            {
                for (int i = 3; i < int(rlim.rlim_cur); i++)
                {
                    if (i != to[0] && i != from[1])
                        close(i);
                }
            }

# if !defined(PEGASUS_DISABLE_PROV_USERCTXT)

            if (request.uid != -1 && request.gid != -1)
            {
                if ((int)getgid() != request.gid)
                {
                    if (setgid(request.gid) != 0)
                        Log(LL_SEVERE, "setgid(%d) failed\n", request.gid);
                }

                if ((int)getuid() != request.uid)
                {
                    if (setuid(request.uid) != 0)
                        Log(LL_SEVERE, "setuid(%d) failed\n", request.uid);
                }
            }

            char username[EXECUTOR_BUFFER_SIZE];

            if (GetUserName(getuid(), username) != 0)
                Fatal(FL, "failed to resolve username for uid=%d", getuid());

            Log(LL_INFORMATION, "starting %s on module %s as user %s",
                path, request.module, username);

# endif /* !defined(PEGASUS_DISABLE_PROV_USERCTXT) */

            // Exec the cimprovagt program.

            char arg1[32];
            char arg2[32];
            sprintf(arg1, "%d", to[0]);
            sprintf(arg2, "%d", from[1]);

            Log(LL_TRACE, "execl(%s, %s, %s, %s, %s)\n",
                path, path, arg1, arg2, request.module);

            execl(path, path, arg1, arg2, request.module, (char*)0);

            Log(LL_SEVERE, "execl(%s, %s, %s, %s, %s): failed\n",
                path, path, arg1, arg2, request.module);

            return;
        }
    }
    while (0);

    // Close unused pipe descriptors.

    close(to[0]);
    close(from[1]);

    // Send response.

    ExecutorStartProviderAgentResponse response;
    response.status = status;
    response.pid = pid;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");

    // Send descriptors to calling process.

    if (response.status == 0)
    {
        int descriptors[2];
        descriptors[0] = from[0];
        descriptors[1] = to[1];

        SendDescriptorArray(sock, descriptors, 2);
        close(from[0]);
        close(to[1]);
    }
}

//==============================================================================
//
// HandleDaemonizeExecutorRequest()
//
//==============================================================================

static void HandleDaemonizeExecutorRequest(int sock)
{
    Log(LL_TRACE, "HandleDaemonizeExecutorRequest()");

    ExecutorDaemonizeExecutorResponse response = { 0 };

    // Fork:

    int pid = fork();

    if (pid < 0)
    {
        response.status = -1;
        Fatal(FL, "fork() failed");

        if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
            Fatal(FL, "failed to write response");
    }

    // Parent exits:

    if (pid > 0)
        exit(0);

    // Ignore SIGHUP:

    signal(SIGHUP, SIG_IGN);

    // Catch SIGTERM:

    signal(SIGTERM, SigHandler);

    // Set current directory to root:

    chdir("/");

    // Close these file descriptors (stdin, stdout, stderr).

    close(0);
    close(1);
    close(2);

    // Direct standard input, output, and error to /dev/null:

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// HandleChangeOwnerRequest()
//
//==============================================================================

static void HandleChangeOwnerRequest(int sock)
{
    // Read the request request.

    struct ExecutorChangeOwnerRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Change owner.

    Log(LL_TRACE, "HandleChangeOwnerRequest(): path=%s owner=%s",
        request.path, request.owner);

    int status = ChangeOwner(request.path, request.owner);

    if (status != 0)
    {
        Log(LL_WARNING, "ChangeOwner(%s, %s) failed",
            request.path, request.owner);
    }

    // Send response message.

    struct ExecutorChangeOwnerResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// HandleRenameFileRequest()
//
//==============================================================================

static void HandleRenameFileRequest(int sock)
{
    // Read the request request.

    struct ExecutorRenameFileRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Rename the file.

    Log(LL_TRACE, "HandleRenameFileRequest(): oldPath=%s newPath=%s", 
        request.oldPath, request.newPath);

    // Perform the operation:

    int status = -1;

    do
    {
        unlink(request.newPath);

        if (link(request.oldPath, request.newPath) != 0)
        {
            Log(LL_WARNING, 
                "link(%s, %s) failed", request.oldPath, request.newPath);
            break;
        }

        if (unlink(request.oldPath) != 0)
        {
            Log(LL_WARNING, "unlink(%s) failed", request.oldPath);
            break;
        }

        status = 0;
    }
    while (0);

    // Send response message.

    struct ExecutorRenameFileResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// HandleRemoveFileRequest()
//
//==============================================================================

static void HandleRemoveFileRequest(int sock)
{
    // Read the request request.

    struct ExecutorRemoveFileRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Remove the file.

    Log(LL_TRACE, "HandleRemoveFileRequest(): path=%s", request.path);

    int status = unlink(request.path);

    if (status != 0)
        Log(LL_WARNING, "unlink(%s) failed", request.path);

    // Send response message.

    struct ExecutorRemoveFileResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// HandleWaitPidRequest()
//
//==============================================================================

static void HandleWaitPidRequest(int sock)
{
    // Read the request request.

    struct ExecutorWaitPidRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Wait on the PID:

    Log(LL_TRACE, "HandleWaitPidRequest(): pid=%d", request.pid);

    int status;
    EXECUTOR_RESTART(waitpid(request.pid, 0, 0), status);

    if (status == -1)
        Log(LL_WARNING, "waitpid(%d, 0, 0) failed", request.pid);

    // Send response message.

    struct ExecutorWaitPidResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// HandlePAMAuthenticateRequest()
//
//==============================================================================

static void HandlePAMAuthenticateRequest(int sock)
{
    // Read the request request.

    struct ExecutorPAMAuthenticateRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Rename the file.

    Log(LL_TRACE, "HandlePAMAuthenticateRequest(): username=%s",
        request.username);

    // Perform the operation:

#if defined(PEGASUS_PAM_AUTHENTICATION)
    int status = PAMAuthenticate(request.username, request.password);
#else
    int status = -1;
#endif

    if (status != 0)
    {
        Log(LL_WARNING, "PAM authentication failed for username %s", 
            request.username);
    }
    else
    {
        Log(LL_INFORMATION, "PAM authentication succeeded for username %s", 
            request.username);
    }

    // Send response message.

    struct ExecutorPAMAuthenticateResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// HandlePAMValidateUserRequest()
//
//==============================================================================

static void HandlePAMValidateUserRequest(int sock)
{
    // Read the request request.

    struct ExecutorPAMValidateUserRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Validate the user with PAM.

    Log(LL_TRACE, 
        "HandlePAMValidateUserRequest(): username=%s", request.username);

#if defined(PEGASUS_PAM_AUTHENTICATION)
    int status = PAMValidateUser(request.username);
#else
    int status = -1;
#endif

    if (status != 0)
        Log(LL_WARNING, "PAM user validation failed on %s", request.username);

    // Send response message.

    struct ExecutorPAMValidateUserResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// Executor()
//
//     The executor process.
//
//==============================================================================

static void Executor(int sock, int childPid)
{
    // Handle Ctrl-C.

    signal(SIGINT, SigHandler);

    // Catch SIGTERM, sent by kill program.

    signal(SIGTERM, SigHandler);

    // Ignore SIGPIPE, which occurs if a child with whom the executor shares
    // a local domain socket unexpectedly dies. In such a case, the socket
    // read/write functions will return an error. There are two child processes
    // the executor talks to over sockets: cimservera and cimservermain.

    signal(SIGPIPE, SIG_IGN);

    // Save child PID globally; it is used by Exit() function.

    _childPid = childPid;

    // Prepares socket into non-blocking I/O.

    SetNonBlocking(sock);

    // Process client requests until client exists.

    for (;;)
    {
        // Receive request header.

        ExecutorRequestHeader header;

        ssize_t n = RecvNonBlock(sock, &header, sizeof(header));

        if (n == 0)
        {
            // Either client closed its end of the pipe (possibly by exiting)
            // or we caught a SIGTERM.
            break;
        }

        if (n != sizeof(header))
            Fatal(FL, "failed to read header");

        // Dispatch request.

        switch (RequestCode(header.code))
        {
            case EXECUTOR_PING_REQUEST:
                HandlePingRequest(sock);
                break;

            case EXECUTOR_OPEN_FILE_REQUEST:
                HandleOpenFileRequest(sock);
                break;

            case EXECUTOR_START_PROVIDER_AGENT_REQUEST:
                HandleStartProviderAgentRequest(sock);
                break;

            case EXECUTOR_DAEMONIZE_EXECUTOR_REQUEST:
                HandleDaemonizeExecutorRequest(sock);
                break;

            case EXECUTOR_CHANGE_OWNER_REQUEST:
                HandleChangeOwnerRequest(sock);
                break;

            case EXECUTOR_RENAME_FILE_REQUEST:
                HandleRenameFileRequest(sock);
                break;

            case EXECUTOR_REMOVE_FILE_REQUEST:
                HandleRemoveFileRequest(sock);
                break;

            case EXECUTOR_WAIT_PID_REQUEST:
                HandleWaitPidRequest(sock);
                break;

            case EXECUTOR_PAM_AUTHENTICATE_REQUEST:
                HandlePAMAuthenticateRequest(sock);
                break;

            case EXECUTOR_PAM_VALIDATE_USER_REQUEST:
                HandlePAMValidateUserRequest(sock);
                break;

            default:
                Fatal(FL, "invalid request code: %d", header.code);
                break;
        }
    }

    // Reached due to socket EOF or SIGTERM.

    Exit(0);
}

//==============================================================================
//
// Child
//
//     The child process.
//
//==============================================================================

static void Child(
    int argc, 
    char** argv, 
    char path[EXECUTOR_BUFFER_SIZE],
    int uid,
    int gid,
    int sock)
{
    // Build argument list, adding "-x <sock>" option if sock non-negative.

    char** execArgv = (char**)malloc(sizeof(char*) * (argc + 4));
    memcpy(execArgv + 4, argv + 1, sizeof(char*) * argc);

    char sockStr[EXECUTOR_BUFFER_SIZE];
    sprintf(sockStr, "%d", sock);

    execArgv[0] = CIMSERVERMAIN;
    execArgv[1] = EXECUTOR_FINGERPRINT;
    execArgv[2] = "-x";
    execArgv[3] = sockStr;

    // Locate repository directory.

    char repositoryDir[EXECUTOR_BUFFER_SIZE];

    if (LocateRepositoryDirectory(argc, argv, repositoryDir) != 0)
        Fatal(FL, "failed to locate repository directory");

    // Check whether repository directory exists.

    if (AccessDir(repositoryDir) != 0)
        Fatal(FL, 
            "failed to access repository directory: %s", repositoryDir);

    // Change ownership of Pegasus repository directory (it should be owned
    // by same user that owns CIMSERVERMAIN).

    ChangeDirOwnerRecursive(repositoryDir, uid, gid);

    Log(LL_TRACE, "Pegasus repositoryDir is \"%s\"", repositoryDir);

    // Downgrade privileges by setting the UID and GID of this process. Use
    // the owner of the CIMSERVERMAIN program obtained above.

    if (uid == 0 || gid == 0)
    {
        Fatal(FL, "root may not own %s since the program is run as owner",
            path);
    }

    if (setgid(gid) != 0)
    {
        Fatal(FL, "Failed to set gid to %d", gid);
    }

    if (setuid(uid) != 0)
    {
        Fatal(FL, "Failed to set uid to %d", uid);
    }

    if ((int)getuid() != uid || 
        (int)geteuid() != uid || 
        (int)getgid() != gid || 
        (int)getegid() != gid)
    {
        Fatal(FL, "setuid/setgid verification failed\n");
    }

    // Log user info.

    char username[EXECUTOR_BUFFER_SIZE];

    if (GetUserName(uid, username) != 0)
        Fatal(FL, "cannot resolve user from uid=%d", uid);

    Log(LL_TRACE, "%s running as %s (uid=%d, gid=%d)", CIMSERVERMAIN, 
        username, uid, gid);

    // Precheck that cimxml.socket is owned by cimservermain process. If 
    // not, then the bind would fail in the cimservermain process much 
    // later and the cause of the error would be difficult to determine.

    if (access(PEGASUS_LOCAL_DOMAIN_SOCKET_PATH, F_OK) == 0)
    {
        struct stat st;

        if (stat(PEGASUS_LOCAL_DOMAIN_SOCKET_PATH, &st) != 0 ||
            (int)st.st_uid != uid || 
            (int)st.st_gid != gid)
        {
            Fatal(FL, 
                "cimservermain process cannot stat or does not own %s",
                PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);
        }
    }

    // Exec child process.

    if (execv(path, execArgv) != 0)
        Fatal(FL, "failed to exec %s", path);

    exit(0);
}

//==============================================================================
//
// GetLogLevel()
//
//==============================================================================

void GetLogLevel(int argc, char** argv)
{
    char buffer[EXECUTOR_BUFFER_SIZE];

    if (FindConfigOption(argc, argv, "logLevel", buffer) == 0)
    {
        if (strcasecmp(buffer, "TRACE") == 0)
            _logLevel = LL_TRACE;
        else if (strcasecmp(buffer, "INFORMATION") == 0)
            _logLevel = LL_INFORMATION;
        else if (strcasecmp(buffer, "WARNING") == 0)
            _logLevel = LL_WARNING;
        else if (strcasecmp(buffer, "SEVERE") == 0)
            _logLevel = LL_SEVERE;
        else if (strcasecmp(buffer, "FATAL") == 0)
            _logLevel = LL_FATAL;
    }
}

//==============================================================================
//
// ReadPidFile()
//
//==============================================================================

static int ReadPidFile(const char* path, int& pid)
{
    FILE* is = fopen(path, "r");

    if (!is) 
        return -1;

    pid = 0;

    fscanf(is, "%d\n", &pid);
    fclose(is);

    if (pid == 0)
        return -1;

    return 0;
}

//==============================================================================
//
// GetProcessName()
//
//==============================================================================

#if defined(PEGASUS_OS_HPUX)

static int GetProcessName(int pid, char name[EXECUTOR_BUFFER_SIZE])
{
    struct pst_status psts;

    if (pstat_getproc(&psts, sizeof(psts), 0, pid) == -1)
        return -1;

    STRLCPY(name, pstru.pst_ucomm, EXECUTOR_BUFFER_SIZE);

    return 0;
}

#elif defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)

static int GetProcessName(int pid, char name[EXECUTOR_BUFFER_SIZE])
{
    // Read the process name from the file.

    static char buffer[1024];
    sprintf(buffer, "/proc/%d/stat", pid);
    FILE* is = fopen(buffer, "r");

    if (!is)
        return -1;

    // Read the first line of the file.

    if (fgets(buffer, sizeof(buffer), is) == NULL)
    {
        fclose(is);
        return -1;
    }

    fclose(is);

    // Extract the PID enclosed in parentheses.

    char* start = strchr(buffer, '(');

    if (!start)
        return -1;

    start++;

    char* end = strchr(start, ')');

    if (!end)
        return -1;

    if (start == end)
        return -1;

    *end = '\0';

    STRLCPY(name, start, EXECUTOR_BUFFER_SIZE);

    return 0;
}

#else
# error "not implemented on this platform."
#endif /* PEGASUS_PLATFORM_LINUX_GENERIC_GNU */

//==============================================================================
//
// TestCimServerProcess()
//
//     Returns 0 if cimserver process is running.
//
//==============================================================================

int TestCimServerProcess()
{
    int pid;

    if (ReadPidFile(PEGASUS_CIMSERVER_START_FILE, pid) != 0)
        return -1;

    char name[EXECUTOR_BUFFER_SIZE];

    if (GetProcessName(pid, name) != 0 || strcmp(name, CIMSERVERMAIN) != 0)
        return -1;

    return 0;
}

//==============================================================================
//
// main()
//
//==============================================================================

int main(int argc, char** argv)
{
    // Save as global so it can be used in error and log messages.

    arg0 = argv[0];

    // Get absolute cimservermain program name.

    char cimservermainPath[EXECUTOR_BUFFER_SIZE];
    GetInternalPegasusProgramPath(CIMSERVERMAIN, cimservermainPath);

    // Get absolute cimshutdown program name.

    char cimshutdownPath[EXECUTOR_BUFFER_SIZE];
    GetInternalPegasusProgramPath(CIMSHUTDOWN, cimshutdownPath);

    // If shuting down, then run "cimshutdown" client.

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-s") == 0)
        {
            char* tmpArgv[3];
            tmpArgv[0] = CIMSHUTDOWN;
            tmpArgv[1] = EXECUTOR_FINGERPRINT;
            tmpArgv[2] = 0;

            execv(cimshutdownPath, tmpArgv);
            Fatal(FL, "failed to exec %s", cimshutdownPath);
        }
    }

    // If CIMSERVERMAIN is already running, warn and exit now.

    if (TestCimServerProcess() == 0)
    {
        fprintf(stderr,
            "%s: cimserver is already running (the PID found in the file "
            "\"%s\" corresponds to an existing process named \"%s\").\n\n",
            arg0, PEGASUS_CIMSERVER_START_FILE, CIMSERVERMAIN);

        exit(1);
    }

    // Create a socket pair for communicating with the child process.

    int pair[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) != 0)
        Fatal(FL, "failed to create socket pair");

    CloseOnExec(pair[1]);

    // Get the log-level from the configuration parameter.

    GetLogLevel(argc, argv);

    // Open the log.

    OpenLog(false, "cimexecutor");

    Log(LL_INFORMATION, "starting");

    // Be sure this process is running as root (otherwise fail).

    if (setuid(0) != 0 || setgid(0) != 0)
    {
        Log(LL_FATAL, "attempted to run program as non-root user");
        fprintf(stderr, "%s: this program must be run as root\n", arg0);
        exit(0);
    }

    // Print user info.

    char username[EXECUTOR_BUFFER_SIZE];

    if (GetUserName(getuid(), username) != 0)
        Fatal(FL, "cannot resolve user from uid=%d", getuid());

    Log(LL_TRACE, "running as %s (uid=%d, gid=%d)",
        username, (int)getuid(), (int)getgid());

    // Determine user for running cimservermain.

    GetServerUser(argc, argv, cimservermainPath, _childUid, _childGid);

    // Fork child process.

    int childPid = fork();

    if (childPid == 0)
    {
        // Child.
        close(pair[1]);
        Child(argc, argv, cimservermainPath, _childUid, _childGid, pair[0]);
    }
    else if (childPid > 0)
    {
        // Parent.
        close(pair[0]);
        Executor(pair[1], childPid);
    }
    else
        Fatal(FL, "fork() failed");

    return 0;
}
