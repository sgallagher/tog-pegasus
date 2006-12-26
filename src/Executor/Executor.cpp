#define _XOPEN_SOURCE_EXTENDED 1
#include <Pegasus/Common/Constants.h>
#include <sys/types.h>
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
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include "Executor.h"

#define TRACE printf("EXECUTOR: TRACE: %s(%d)\n", __FILE__, __LINE__)
#define FL __FILE__, __LINE__

#define CIMSERVERMAIN "cimservermain"
#define CIMPROVAGT "cimprovagt"

static const char* arg0;

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
            Fatal(FL, "buffer overrun in STRLCPY()"); \
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
            Fatal(FL, "buffer overrun in STRLCAT()"); \
    } \
    while (0)

//==============================================================================
//
// _shutdownFlag
//
//     This flag indicates that the cimservermain process is in the process of 
//     shutting down, indicating that -s was passed to this cimserver process.
//
//==============================================================================

static bool _shutdownFlag = false;

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
    // ATTN: report fatal errors with syslog.

    va_list ap;
    va_start(ap, format);
    fprintf(stderr, "%s: fatal: %s(%ld): ", arg0, file, (long)line);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(1);

/*
ATTN: Do we need to shut down the cimserver in some cases?
*/
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
// SendDescriptor()
//
//     Send a descriptor (file, socket, pipe) to the child process.
//
//==============================================================================

static ssize_t SendDescriptor(int sock, int desc)
{
    // This control data begins with a cmsghdr struct followed by the data
    // (a descriptor in this case). The union ensures that the data is aligned 
    // suitably for the leading cmsghdr struct. The descriptor itself is
    // properly aligned since the cmsghdr ends on a boundary that is suitably 
    // aligned for any type (including int).
    //
    // ControlData = [ cmsghdr | int ]

    union ControlData
    {
        struct cmsghdr cmh;
        char data[CMSG_SPACE(sizeof(int))];
    };

    // Initialize msghdr struct to refer to control data.

    struct msghdr mh;
    memset(&mh, 0, sizeof(mh));

    union ControlData cd;
    memset(&cd, 0, sizeof(cd));

    mh.msg_control = cd.data;
    mh.msg_controllen = sizeof(cd.data);
    mh.msg_name = NULL;
    mh.msg_namelen = 0;

    // Fill in the control data struct with the descriptor and other fields.

    struct cmsghdr* cmh = CMSG_FIRSTHDR(&mh);
    cmh->cmsg_len = CMSG_LEN(sizeof(int));
    cmh->cmsg_level = SOL_SOCKET;
    cmh->cmsg_type = SCM_RIGHTS;
    *((int *)CMSG_DATA(cmh)) = desc;

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

    return sendmsg(sock, &mh, 0);
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
    TRACE;

    ExecutorPingResponse response = { EXECUTOR_PING_MAGIC };

    if (ExecutorSend(sock, &response, sizeof(response)) != sizeof(response))
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
    TRACE;

    // Read the request request.

    struct ExecutorOpenFileRequest request;

    if (ExecutorRecv(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Open the file.

    int fd = open(request.path, O_RDONLY);

    // Send response message.

    struct ExecutorOpenFileResponse response;
    memset(&response, 0, sizeof(response));

    if (fd == -1)
        response.status = -1;
    else
        response.status = 0;

    if (ExecutorSend(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");

    // Send descriptor to calling process (if any to send).

    if (fd != -1)
    {
        SendDescriptor(sock, fd);
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
    TRACE;

    // Read request.

    struct ExecutorStartProviderAgentRequest request;

    if (ExecutorRecv(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

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

            // ATTN: set uid and gid here.

            // Exec the cimprovagt program.

            char arg1[32];
            char arg2[32];
            sprintf(arg1, "%d", to[0]);
            sprintf(arg2, "%d", from[1]);
            execl(path, path, arg1, arg2, request.module, (char*)0);

            // ATTN: log this error.
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

    if (ExecutorSend(sock, &response, sizeof(response)) != sizeof(response))
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
    TRACE;

    ExecutorDaemonizeExecutorResponse response = { 0 };

    // Fork:

    int pid = fork();

    if (pid < 0)
    {
        response.status = -1;

        if (ExecutorSend(sock, &response, sizeof(response)) != sizeof(response))
            Fatal(FL, "failed to write response");
    }

    // Parent exits:

    if (pid > 0)
	exit(0);

    // Ignore these signals:

    signal(SIGHUP, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    // Set current directory to root:

    chdir("/");

    // Close these file descriptors:

    close(0);
    close(1);
    close(2);

    // Direct standard input, output, and error to /dev/null:

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    if (ExecutorSend(sock, &response, sizeof(response)) != sizeof(response))
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
        return -1;

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
printf("_changeOwner(%s, %s)\n", path, owner);
    int uid;
    int gid;

    if (_getUserInfo(owner, uid, gid) != 0)
        return -1;

    if (chown(path, uid, gid) != 0)
        return -1;

printf("_changeOwner(): success\n");
    return 0;
}

//==============================================================================
//
// HandleChangeOwnerRequest()
//
//==============================================================================

static void HandleChangeOwnerRequest(int sock)
{
    TRACE;

    // Read the request request.

    struct ExecutorChangeOwnerRequest request;

    if (ExecutorRecv(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Change owner.

    int status = _changeOwner(request.path, request.owner);

    // Send response message.

    struct ExecutorChangeOwnerResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (ExecutorSend(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// HandleRemoveFileRequest()
//
//==============================================================================

static void HandleRemoveFileRequest(int sock)
{
    TRACE;

    // Read the request request.

    struct ExecutorRemoveFileRequest request;

    if (ExecutorRecv(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    // Remove the file.

    int status = unlink(request.path);

    // Send response message.

    struct ExecutorRemoveFileResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;

    if (ExecutorSend(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// Executor()
//
//     The monitor process.
//
//==============================================================================

static void Executor(int sock, int child_pid)
{
    // Process client requests until client exists.

    for (;;)
    {
        // Receive request header.

        ExecutorRequestHeader header;

        ssize_t n = ExecutorRecv(sock, &header, sizeof(header));

        if (n == 0)
        {
            // Client has closed its end of the pipe, either explicitly by
            // calling close() or explicitly by exiting (or crashing).
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

            case EXECUTOR_OPEN_FILE_FOR_READ_REQUEST:
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

            case EXECUTOR_REMOVE_FILE_REQUEST:
                HandleRemoveFileRequest(sock);
                break;

            default:
                Fatal(FL, "invalid request code");
                break;
        }
    }

    // Reached if read return 0, indicating that client has exited.

    exit(0);
}

//==============================================================================
//
// Child
//
//     The child process.
//
//==============================================================================

static void Child(int argc, char** argv, int sock)
{
    // Get program name.

    char path[EXECUTOR_MAX_PATH_LENGTH];
    GetInternalPegasusProgramPath(CIMSERVERMAIN, path);

    // Downgrade privileges by setting the UID and GID of this process.

    int uid;
    int gid;

    const char USER[] = "mbrasher";

    if (_getUserInfo(USER, uid, gid) != 0)
    {
        Fatal(FL, "failed to get user informaiton for user \"%s\"", USER);
    }

    if (uid == 0 || gid == 0)
    {
        Fatal(FL, "attempted to run %s as root user", CIMSERVERMAIN);
        exit(1);
    }

    if (setgid(gid) != 0)
    {
        Fatal(FL, "Failed to set gid to %d", gid);
        exit(1);
    }

    if (setuid(uid) != 0)
    {
        Fatal(FL, "Failed to set uid to %d", uid);
        exit(1);
    }

    if ((int)getuid() != uid || 
        (int)geteuid() != gid || 
        (int)getgid() != gid || 
        (int)getegid() != gid)
    {
        Fatal(FL, "setuid/setgid verfication failed\n");
        exit(1);
    }

    // Build an argv array for child process.

    char** childArgv = (char**)malloc(sizeof(char*) * (argc + 1));
    memcpy(childArgv, argv, sizeof(char*) * argc);
    childArgv[0] = CIMSERVERMAIN;
    childArgv[argc] = NULL;

    // Exec child process.

    if (execv(path, childArgv) != 0)
        Fatal(FL, "failed to exec %s", path);

    // ATTN: log this failure.

    exit(0);
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

    // Be sure this process is running as root (otherwise fail).

    if (setuid(0) != 0 || setgid(0) != 0)
    {
        fprintf(stderr, "%s: this program must be run as root\n", arg0);
        exit(0);
    }

    // Print user info.

    printf("%s: uid=%d, gid=%d\n", arg0, (int)getuid(), (int)getgid());

    // Create a socket pair for communicating with the child process. This must
    // be the first descriptor created since the child process assumes the 
    // inherited socket descriptor is 3 (stdin=0, stdout=1, stderr=2).

    int pair[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) != 0)
        Fatal(FL, "failed to create socket pair");

    if (pair[0] != 3)
        Fatal(FL, "internal assumption failed");

    // Close pair[1] on exec system calls so the child process does not 
    // inherit it.

    CloseOnExec(pair[1]);

    // Prepare for shutdown sequence.

    if (argc == 2 && strcmp(argv[1], "-s") == 0)
    {
        _shutdownFlag = true;
    }

    // Fork child process.

    int child_pid = fork();

    if (child_pid == 0)
    {
        // Child.
        close(pair[1]);
        Child(argc, argv, pair[0]);
    }
    else if (child_pid > 0)
    {
        // Parent.
        close(pair[0]);
        Executor(pair[1], child_pid);
    }
    else
        Fatal(FL, "fork() failed");

    return 0;
}
