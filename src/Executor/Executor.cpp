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
#include <Pegasus/Common/Constants.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pwd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <signal.h>
#include <cstdarg>
#include <syslog.h>
#include <dirent.h>
#include "Executor.h"

typedef unsigned long long uint64;

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
//==============================================================================

#define STRLCPY(DEST, SRC, DEST_SIZE) \
    do \
    { \
        const size_t n = DEST_SIZE; \
        if (Strlcpy(DEST, SRC, n) >= n) \
            Fatal(FL, \
                "%s(%d): buffer overrun in STRLCPY()", __FILE__, __LINE__); \
    } \
    while (0)

//==============================================================================
//
// STRLCAT()
//
//==============================================================================

#define STRLCAT(DEST, SRC, DEST_SIZE) \
    do \
    { \
        const size_t n = DEST_SIZE; \
        if (Strlcat(DEST, SRC, n) >= n) \
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
// SigTermHandler()
//
//     Signal handler for SIGTERM.
//
//==============================================================================

static bool _caughtSigTerm = false;

void SigTermHandler(int signum)
{
    _caughtSigTerm = true;
}

//==============================================================================
//
// struct CimServerMainInfo
//
//     This structure maintains information about the cimservermain program.
//
//==============================================================================

struct CimServerMainInfo
{
    // Full path of cimservermain program.
    char path[EXECUTOR_MAX_PATH_LENGTH];

    // UID of user that owns the cimservermain program.
    int uid;

    // GID of user that owns the cimservermain program.
    int gid;
};

struct CimServerMainInfo _cimServerMainInfo;

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
// Log()
//
//     Log function.
//
//==============================================================================

static void Log(int priority, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vsyslog(priority, format, ap);
    va_end(ap);
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
    Log(LOG_INFO, "trace: %s(%d)", file, int(line));

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

    exit(1);
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
// GetCurrentTime()
//
//     Get microseconds (usec) ellapsed since epoch.
//
//==============================================================================

uint64 GetCurrentTime()
{
    // ATTN: delete this function?
    struct timeval  tv;
    struct timezone ignore;
    gettimeofday(&tv, &ignore);

    return uint64(tv.tv_sec) * uint64(1000000) + uint64(tv.tv_usec);
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
// Recv
//
//==============================================================================

static ssize_t Recv(int sock, void* buffer, size_t size)
{
    const long TIMEOUT_MSEC = 250;
    size_t r = size;
    char* p = (char*)buffer;

    if (size == 0)
        return -1;

    while (r)
    {
        int status = WaitForReadEnable(sock, TIMEOUT_MSEC);

        if (_caughtSigTerm)
        {
            // Terminated with SIGTERM.
            return 0;
        }

        if (status == 0)
            continue;

        ssize_t n;
        EXECUTOR_RESTART(read(sock, p, r), n);

        if (n == -1 && errno == EINTR)
        {
            if (_caughtSigTerm)
            {
                Log(LOG_INFO, "Caught sigterm");
            }
            continue;
        }

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
// Send()
//
//     Sends *size* bytes onto the given socket.
//
//==============================================================================

static ssize_t Send(int sock, const void* buffer, size_t size)
{
    const long TIMEOUT_MSEC = 250;
    size_t r = size;
    char* p = (char*)buffer;

    while (r)
    {
        // ATTN: handle this or not?
        int status = WaitForWriteEnable(sock, TIMEOUT_MSEC);

        if (_caughtSigTerm)
        {
            // ATTN: ignore?
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
// GetPegasusRepositoryDir()
//
//==============================================================================

static void GetPegasusRepositoryDir(char path[EXECUTOR_MAX_PATH_LENGTH])
{
    if (PEGASUS_REPOSITORY_DIR[0] == '/')
    {
        STRLCPY(path, PEGASUS_REPOSITORY_DIR, EXECUTOR_MAX_PATH_LENGTH);
        return;
    }

    const char* home = getenv("PEGASUS_HOME");

    if (!home)
        Fatal(FL, "Failed to locate Pegasus repository directory");

    STRLCPY(path, home, EXECUTOR_MAX_PATH_LENGTH);
    STRLCAT(path, "/", EXECUTOR_MAX_PATH_LENGTH);
    STRLCAT(path, PEGASUS_REPOSITORY_DIR, EXECUTOR_MAX_PATH_LENGTH);

    struct stat st;

    if (stat(path, &st) != 0)
        Fatal(FL, "Pegasus repository directory does not exist: \"%s\"", path);

    if (!S_ISDIR(st.st_mode))
        Fatal(FL, "not a directory \"%s\"", path);
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

        char buffer[EXECUTOR_MAX_PATH_LENGTH];
        STRLCPY(buffer, path, EXECUTOR_MAX_PATH_LENGTH);
        STRLCAT(buffer, "/", EXECUTOR_MAX_PATH_LENGTH);
        STRLCAT(buffer, name, EXECUTOR_MAX_PATH_LENGTH);

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
// ChangeRepositoryDirOwner()
//
//     Recursively change ownership of Pegasus repository directory.
//
//==============================================================================

static void ChangeRepositoryDirOwner(int uid, int gid)
{
    char path[EXECUTOR_MAX_PATH_LENGTH];
    GetPegasusRepositoryDir(path);
    ChangeDirOwnerRecursive(path, uid, gid);
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

static void GetPegasusInternalBinDir(char path[EXECUTOR_MAX_PATH_LENGTH])
{
    // Make a copy of PEGASUS_PROVIDER_AGENT_PROC_NAME:

    char buffer[EXECUTOR_MAX_PATH_LENGTH];
    STRLCPY(buffer, PEGASUS_PROVIDER_AGENT_PROC_NAME, sizeof(buffer));

    // Remove "cimprovagt" suffix.

    char* p = strrchr(buffer, '/');

    if (!p)
        p = buffer;

    *p = '\0';

    // If buffer path absolute, use this.

    if (buffer[0] == '/')
    {
        STRLCAT(path, buffer, EXECUTOR_MAX_PATH_LENGTH);
    }
    else
    {
        // Prefix with PEGASUS_HOME environment variable.

        const char* home = getenv("PEGASUS_HOME");

        if (!home)
            Fatal(FL, "Failed to locate the internal Pegasus bin directory");

        STRLCPY(path, home, EXECUTOR_MAX_PATH_LENGTH);
        STRLCAT(path, "/", EXECUTOR_MAX_PATH_LENGTH);
        STRLCAT(path, buffer, EXECUTOR_MAX_PATH_LENGTH);
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
// GetInternalPegasusProgramPath()
//
//     Get the full path name of the given program.
//
//==============================================================================

static void GetInternalPegasusProgramPath(
    const char* program,
    char path[EXECUTOR_MAX_PATH_LENGTH])
{
    GetPegasusInternalBinDir(path);
    STRLCAT(path, "/", EXECUTOR_MAX_PATH_LENGTH);
    STRLCAT(path, program, EXECUTOR_MAX_PATH_LENGTH);
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
    Log(LOG_INFO, "HandlePingRequest()");

    ExecutorPingResponse response = { EXECUTOR_PING_MAGIC };

    if (Send(sock, &response, sizeof(response)) != sizeof(response))
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

    if (Recv(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Open the file.

    Log(LOG_INFO, "HandleOpenFileRequest(): path=%s", request.path);

    int flags = 0;

    switch (request.mode)
    {
        case 'r':
            flags = O_RDONLY;
            break;

        case 'w':
            flags = O_WRONLY | O_CREAT | O_TRUNC;
            break;
    }

    int fd;

    if (flags)
        fd = open(request.path, flags);
    else
        fd = -1;

    // Send response message.

    struct ExecutorOpenFileResponse response;
    memset(&response, 0, sizeof(response));

    if (fd == -1)
    {
        Log(LOG_ERR, "open(%s, %c) failed", request.path, request.mode);
        response.status = -1;
    }
    else
        response.status = 0;

    if (Send(sock, &response, sizeof(response)) != sizeof(response))
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

    if (Recv(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    Log(LOG_INFO, "HandleStartProviderAgentRequest(): module=%s gid=%d uid=%d",
        request.module, request.gid, request.uid);

    // Map cimservermain user to root to preserve pre-privilege-separation
    // behavior.

    if (request.uid == _cimServerMainInfo.uid)
    {
        Log(LOG_INFO, 
            "using root instead of cimservermain user for cimprovagt");

        request.uid = 0;
        request.gid = 0;
    }

    if (request.uid == 0)
        Log(LOG_INFO, "***** starting provider agent as root");

    // Process request.

    int status = 0;
    int pid = -1;
    int to[2];
    int from[2];

    do
    {
        // Resolve full path of "cimprovagt".

        char path[EXECUTOR_MAX_PATH_LENGTH];
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
                        Log(LOG_ERR, "setgid(%d) failed\n", request.gid);
                }

                if ((int)getuid() != request.uid)
                {
                    if (setuid(request.uid) != 0)
                        Log(LOG_ERR, "setuid(%d) failed\n", request.uid);
                }
            }

# endif /* !defined(PEGASUS_DISABLE_PROV_USERCTXT) */

            // Exec the cimprovagt program.

            char arg1[32];
            char arg2[32];
            sprintf(arg1, "%d", to[0]);
            sprintf(arg2, "%d", from[1]);

            Log(LOG_INFO, "execl(%s, %s, %s, %s, %s)\n",
                path, path, arg1, arg2, request.module);

            execl(path, path, arg1, arg2, request.module, (char*)0);

            Log(LOG_ERR, "execl(%s, %s, %s, %s, %s): failed\n",
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

    if (Send(sock, &response, sizeof(response)) != sizeof(response))
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
    Log(LOG_INFO, "HandleDaemonizeExecutorRequest()");

    ExecutorDaemonizeExecutorResponse response = { 0 };

    // Fork:

    int pid = fork();

    if (pid < 0)
    {
        response.status = -1;
        Log(LOG_ERR, "fork() failed");

        if (Send(sock, &response, sizeof(response)) != sizeof(response))
            Fatal(FL, "failed to write response");
    }

    // Parent exits:

    if (pid > 0)
	exit(0);

    // Ignore SIGHUP:

    signal(SIGHUP, SIG_IGN);

    // Catch SIGTERM:

    signal(SIGTERM, SigTermHandler);

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

    if (Send(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// _getUserInfo()
//
//     Lookup the given user's uid and gid.
//
//==============================================================================

static int _getUserInfo(const char* user, int& uid, int& gid)
{
    struct passwd pwd;
    const unsigned int PWD_BUFF_SIZE = 1024;
    char buffer[PWD_BUFF_SIZE];
    struct passwd* ptr = 0;

    if (getpwnam_r(user, &pwd, buffer, PWD_BUFF_SIZE, &ptr) != 0 || !ptr)
    {
        Log(LOG_ERR, "getpwnam_r(%s, ...) failed", user);
        return -1;
    }

    uid = ptr->pw_uid;
    gid = ptr->pw_gid;

    return 0;
}

//==============================================================================
//
// _changeOwner()
//
//     Change the given file's owner.
//
//==============================================================================

static int _changeOwner(const char* path, const char* owner)
{
    int uid;
    int gid;

    if (_getUserInfo(owner, uid, gid) != 0)
        return -1;

    if (chown(path, uid, gid) != 0)
    {
        Log(LOG_ERR, "chown(%s, %d, %d) failed", path, uid, gid);
        return -1;
    }

    return 0;
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

    if (Recv(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Change owner.

    Log(LOG_INFO, "HandleChangeOwnerRequest(): path=%s owner=%s",
        request.path, request.owner);

    int status = _changeOwner(request.path, request.owner);

    if (status != 0)
    {
        Log(LOG_ERR, "_changeOwner(%s, %s) failed",
            request.path, request.owner);
    }

    // Send response message.

    struct ExecutorChangeOwnerResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (Send(sock, &response, sizeof(response)) != sizeof(response))
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

    if (Recv(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Rename the file.

    Log(LOG_INFO, "HandleRenameFileRequest(): oldPath=%s newPath=%s", 
        request.oldPath, request.newPath);

    // Perform the operation:

    int status = -1;

    do
    {
        unlink(request.newPath);

        if (link(request.oldPath, request.newPath) != 0)
        {
            Log(LOG_INFO, 
                "link(%s, %s) failed", request.oldPath, request.newPath);
            break;
        }

        if (unlink(request.oldPath) != 0)
        {
            Log(LOG_INFO, "unlink(%s) failed", request.oldPath);
            break;
        }

        status = 0;
    }
    while (0);

    // Send response message.

    struct ExecutorRenameFileResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (Send(sock, &response, sizeof(response)) != sizeof(response))
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

    if (Recv(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Remove the file.

    Log(LOG_INFO, "HandleRemoveFileRequest(): path=%s", request.path);

    int status = unlink(request.path);

    if (status != 0)
        Log(LOG_ERR, "unlink(%s) failed", request.path);

    // Send response message.

    struct ExecutorRemoveFileResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (Send(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// HandleChangeModeRequest()
//
//==============================================================================

static void HandleChangeModeRequest(int sock)
{
    // Read the request request.

    struct ExecutorChangeModeRequest request;

    if (Recv(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Change the mode of the file.

    Log(LOG_INFO, "HandleChangeModeRequest(): path=%s mode=%08X", 
        request.path, request.mode);

    int status = chmod(request.path, request.mode);

    if (status != 0)
        Log(LOG_ERR, "chmod(%s, %08X) failed", request.path, request.mode);

    // Send response message.

    struct ExecutorChangeModeResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (Send(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// HandleShutdownExecutorRequest()
//
//==============================================================================

static void HandleShutdownExecutorRequest(int sock)
{
    Log(LOG_INFO, "HandleShutdownExecutorRequest()");

    ExecutorShutdownExecutorResponse response = { 0 };

    if (Send(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");

    Log(LOG_NOTICE, "shutting down");
    exit(0);
}

//==============================================================================
//
// Executor()
//
//     The monitor process.
//
//==============================================================================

static void Executor(int sock, int childPid)
{
    // Prepares socket into non-blocking I/O.

    SetNonBlocking(sock);

    // Process client requests until client exists.

    for (;;)
    {
        // Receive request header.

        ExecutorRequestHeader header;

        ssize_t n = Recv(sock, &header, sizeof(header));

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

            case EXECUTOR_CHANGE_MODE_REQUEST:
                HandleChangeModeRequest(sock);
                break;

            case EXECUTOR_SHUTDOWN_EXECUTOR_REQUEST:
                HandleShutdownExecutorRequest(sock);
                break;

            default:
                Fatal(FL, "invalid request code: %d", header.code);
                break;
        }
    }

    // Reached due to socket EOF or SIGTERM.

    if (_caughtSigTerm)
    {
        Log(LOG_INFO, "caught SIGTERM");

        // Kill off cimservermain.
        Log(LOG_INFO, "killing cimservermain");
        kill(childPid, SIGTERM);
    }

    Log(LOG_INFO, "<<<<<<<<<< exit >>>>>>>>>>");

    exit(0);
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
    CimServerMainInfo& info,
    int sock)
{
    // Change ownership of Pegasus repository directory (it should be owned
    // by same user that owns CIMSERVERMAIN.

    ChangeRepositoryDirOwner(info.uid, info.gid);

    // Downgrade privileges by setting the UID and GID of this process. Use
    // the owner of the CIMSERVERMAIN program obtained above.

    if (info.uid == 0 || info.gid == 0)
    {
        Fatal(FL, "root may not own %s since the program is run as owner",
            info.path);
        exit(1);
    }

    if (setgid(info.gid) != 0)
    {
        Fatal(FL, "Failed to set gid to %d", info.gid);
        exit(1);
    }

    if (setuid(info.uid) != 0)
    {
        Fatal(FL, "Failed to set uid to %d", info.uid);
        exit(1);
    }

    if ((int)getuid() != info.uid || 
        (int)geteuid() != info.uid || 
        (int)getgid() != info.gid || 
        (int)getegid() != info.gid)
    {
        Fatal(FL, "setuid/setgid verification failed\n");
        exit(1);
    }

    Log(LOG_INFO, "creating %s with uid=%d and gid=%d", CIMSERVERMAIN, 
        info.uid, info.gid);

    // Precheck that cimxml.socket is owned by cimservermain process. If not,
    // then the bind would fail in the cimservermain process much later and
    // the cause of the error would be difficult to determine.

    if (access(PEGASUS_LOCAL_DOMAIN_SOCKET_PATH, F_OK) == 0)
    {
        struct stat st;

        if (stat(PEGASUS_LOCAL_DOMAIN_SOCKET_PATH, &st) != 0 ||
            (int)st.st_uid != info.uid || 
            (int)st.st_gid != info.gid)
        {
            Fatal(FL, "cimservermain process cannot stat or does not own %s",
                PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);
        }
    }

    // Build an argv array for child process.

    char** childArgv = (char**)malloc(sizeof(char*) * (argc + 1));
    memcpy(childArgv, argv, sizeof(char*) * argc);
    childArgv[0] = CIMSERVERMAIN;
    childArgv[argc] = NULL;

    // Exec child process.

    if (execv(info.path, childArgv) != 0)
        Fatal(FL, "failed to exec %s", info.path);

    // ATTN: log this failure.

    exit(0);
}

//==============================================================================
//
// GetCimServerMainInfo
//
//     Get information about the cimservermain program. See CimServerMainInfo
//     structure for details.
//
//==============================================================================

static void GetCimServerMainInfo(CimServerMainInfo& info)
{
    // Get program name.

    GetInternalPegasusProgramPath(CIMSERVERMAIN, info.path);

    // Get the owner uid-gid of the CIMSERVERMAIN program.

    struct stat st;

    if (stat(info.path, &st) != 0)
        Fatal(FL, "stat(%s) failed", info.path);

    info.uid = st.st_uid;
    info.gid = st.st_gid;
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

    // Create a socket pair for communicating with the child process. This must
    // be the first descriptor created since the child process assumes the 
    // inherited socket descriptor is 3 (stdin=0, stdout=1, stderr=2).

    int pair[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) != 0)
        Fatal(FL, "failed to create socket pair");

    if (pair[0] != 3)
        Fatal(FL, "internal assumption failed");

    // Open the log.

    OpenLog(false, "cimexecutor");

    Log(LOG_INFO, "<<<<<<<<<< start >>>>>>>>>>");

    // Define __PEGASAUS_EXECUTOR__ environmnent variable. The ExecutorClient
    // uses this variable to determine whether to talk to the executor over
    // a socket or to use loopback mode.

    putenv("__PEGASAUS_EXECUTOR__=1");

    // Be sure this process is running as root (otherwise fail).

    if (setuid(0) != 0 || setgid(0) != 0)
    {
        Log(LOG_CRIT, "attempted to run program as non-root user");
        fprintf(stderr, "%s: this program must be run as root\n", arg0);
        exit(0);
    }

    // Print user info.

    Log(LOG_INFO, "started executor with uid=%d, gid=%d\n", 
        (int)getuid(), (int)getgid());

    // Close pair[1] on exec system calls so the child process does not 
    // inherit it.

    CloseOnExec(pair[1]);

    // Prepare for shutdown sequence.

    if (argc == 2 && strcmp(argv[1], "-s") == 0)
    {
        _shutdownFlag = true;
    }

    // Get information about the CIMSERVERMAIN program.

    GetCimServerMainInfo(_cimServerMainInfo);

    // Fork child process.

    int childPid = fork();

    if (childPid == 0)
    {
        // Child.
        close(pair[1]);
        Child(argc, argv, _cimServerMainInfo, pair[0]);
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
