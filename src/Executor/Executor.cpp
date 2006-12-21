#define _XOPEN_SOURCE_EXTENDED 1
#include <Pegasus/Common/Constants.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include "Executor.h"

#define TRACE printf("TRACE: %s(%d)\n", __FILE__, __LINE__)
#define FL __FILE__, __LINE__

#define CIMSERVERMAIN "cimservermainx"

static const char* arg0;

/*
**==============================================================================
**
** STRLCPY()
**
**==============================================================================
*/

#define STRLCPY(DEST, SRC, DEST_SIZE) \
    do \
    { \
        const size_t n = DEST_SIZE; \
        if (Strlcpy(DEST, SRC, n) >= n) \
            Fatal(FL, "buffer overrun in STRLCPY"); \
    } \
    while (0)

/*
**==============================================================================
**
** STRLCAT()
**
**==============================================================================
*/

#define STRLCAT(DEST, SRC, DEST_SIZE) \
    do \
    { \
        const size_t n = DEST_SIZE; \
        if (Strlcat(DEST, SRC, n) >= n) \
            Fatal(FL, "buffer overrun in STRLCAT"); \
    } \
    while (0)

/*
**==============================================================================
**
** Fatal()
**
**     Report fatal errors. The callar set fatal_file and fatal_line before
**     calling this function. Note that since this is a single threaded
**     application, there is no attempt to synchronize access to these
**     globals.
**
**==============================================================================
*/

static void Fatal(const char* file, size_t line, const char* format, ...)
{
    /* ATTN: report fatal errors with syslog. */

    va_list ap;
    va_start(ap, format);
    fprintf(stderr, "%s: fatal: %s(%ld): ", arg0, file, (long)line);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(1);
}

/*
**==============================================================================
**
** CloseOnExec()
**
**     Direct the kernel not to keep the given file descriptor open across
**     exec() system call.
**
**==============================================================================
*/

static int CloseOnExec(int fd)
{
    return fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
}

/*
**==============================================================================
**
** CreateSocketPair()
**
**     Create an anonymous UNIX-domain socket pair.
**
**==============================================================================
*/

static int CreateSocketPair(int pair[2])
{
    return socketpair(AF_UNIX, SOCK_STREAM, 0, pair);
}

/*
**==============================================================================
**
** SendDescriptor()
**
**     Send a descriptor (file, socket, pipe) to the child process. The child
**     process is waiting in a call to ExecutorReceiveDescriptor().
**
**==============================================================================
*/

static ssize_t SendDescriptor(int sock, int desc)
{
    /* 
     * This control data begins with a cmsghdr struct followed by the data
     * (a descriptor in this case). The union ensures that the data is aligned 
     * suitably for the leading cmsghdr struct. The descriptor itself is
     * properly aligned since the cmsghdr ends on a boundary that is suitably 
     * aligned for any type (including int).
     *
     * ControlData = [ cmsghdr | int ]
     */
    union ControlData
    {
        struct cmsghdr cmh;
        char data[CMSG_SPACE(sizeof(int))];
    };
    union ControlData cd;
    struct msghdr mh;
    struct cmsghdr* cmh;
    struct iovec iov[1];
    char dummy = '\0';

    /* Initialize msghdr struct to refer to control data. */

    memset(&mh, 0, sizeof(mh));

    memset(&cd, 0, sizeof(cd));

    mh.msg_control = cd.data;
    mh.msg_controllen = sizeof(cd.data);
    mh.msg_name = NULL;
    mh.msg_namelen = 0;

    /* Fill in the control data struct with the descriptor and other fields. */

    cmh = CMSG_FIRSTHDR(&mh);
    cmh->cmsg_len = CMSG_LEN(sizeof(int));
    cmh->cmsg_level = SOL_SOCKET;
    cmh->cmsg_type = SCM_RIGHTS;
    *((int *)CMSG_DATA(cmh)) = desc;

    /* Prepare to send single dummy byte. It will not be used but we must send
     * at least one byte otherwise the call will fail on some platforms.
     */

    memset(iov, 0, sizeof(iov));

    iov[0].iov_base = &dummy;
    iov[0].iov_len = 1;
    mh.msg_iov = iov;
    mh.msg_iovlen = 1;

    /* Send message to child. */

    return sendmsg(sock, &mh, 0);
}

/*
**==============================================================================
**
** HandlePingRequest()
**
**     Handle ping request.
**
**==============================================================================
*/

static void HandlePingRequest(int sock)
{
    struct ExecutorPingResponse response;

    printf("%s: HandlePingRequest()\n", arg0);

    /* Send response message */

    response.magic = EXECUTOR_PING_MAGIC;

    if (ExecutorSend(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

/*
**==============================================================================
**
** HandleOpenFileRequest()
**
**     Handle a request from a child to open a file.
**
**==============================================================================
*/

static void HandleOpenFileRequest(int sock)
{
    struct ExecutorOpenFileRequest request;
    struct ExecutorOpenFileResponse response;
    int fd;
    size_t n;

    printf("%s: HandleOpenFileRequest()\n", arg0);

    /* Read the request request */

    if (ExecutorRecv(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    /* Open the file */

    fd = open(request.path, request.flags);

    /* Send response message */

    memset(&response, 0, sizeof(response));

    if (fd == -1)
        response.status = -1;
    else
        response.status = 0;

    if (ExecutorSend(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");

    /* Send descriptor to calling process (if any to send). */

    if (fd != -1)
    {
        n = SendDescriptor(sock, fd);
        close(fd);
    }
}

/*
**==============================================================================
**
** Executor()
**
**     The monitor process.
**
**==============================================================================
*/

static void Executor(int sock, int child_pid)
{
    /* Process client requests until client exists. */

    for (;;)
    {
        struct ExecutorRequestHeader header;

        /* Receive request header. */

        ssize_t n = ExecutorRecv(sock, &header, sizeof(header));

        if (n == 0)
        {
            /* Client has closed its end of the pipe, either explicitly by
             * calling close() or explicitly by exiting (or crashing).
             */
            break;
        }

        if (n != sizeof(header))
            Fatal(FL, "failed to read header");

        /* Dispatch request */

        switch (header.code)
        {
            case EXECUTOR_OPEN_FILE_REQUEST:
                HandleOpenFileRequest(sock);
                break;

            case EXECUTOR_PING_REQUEST:
                HandlePingRequest(sock);
                break;

            default:
                Fatal(FL, "invalid request code");
                break;
        }
    }

    /* Reached if read return 0, indicating that client has exited */

    exit(0);
}

/*
**==============================================================================
**
** GetPegasusInternalBinDir()
**
**     Get the Pegasus "lbin" directory. This is the directory that contains
**     internal Pegasus programs. Note that administrative tools are contained
**     in the "sbin" directory.
**
**==============================================================================
*/

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

/*
**==============================================================================
**
** GetInternalPegasusProgramPath()
**
**     Get the full path name of the given program.
**
**==============================================================================
*/

static void GetInternalPegasusProgramPath(
    const char* program,
    char path[EXECUTOR_MAX_PATH_LENGTH])
{
    size_t r;

    GetPegasusInternalBinDir(path);
    STRLCAT(path, "/", EXECUTOR_MAX_PATH_LENGTH);
    STRLCAT(path, program, EXECUTOR_MAX_PATH_LENGTH);
}

/*
**==============================================================================
**
** Child
**
**     The child process.
**
**==============================================================================
*/

static void Child(const char* arg0, int sock)
{
    char path[EXECUTOR_MAX_PATH_LENGTH];

    /* Get program name */

    GetInternalPegasusProgramPath(CIMSERVERMAIN, path);

    /* Exec child process. */

    if (execl(path, CIMSERVERMAIN, NULL) != 0)
        Fatal(FL, "failed to exec %s", path);

    exit(0);
}

/*
**==============================================================================
**
** main()
**
**==============================================================================
*/

int main(int argc, char** argv)
{
    arg0 = argv[0];
    int pair[2];
    int child_pid;

    /* Create a socket pair for communicating with the child process. This
     * must be the first descriptor created since child process assumes the 
     * inherited socket descriptor value is 3 (recall that stdin=0, stdout=1,
     * and stderr=2).
     */

    if (CreateSocketPair(pair) != 0)
        Fatal(FL, "failed to create socket pair");

    if (pair[0] != 3)
        Fatal(FL, "internal assumption failed");

    /* Close pair[1] on exec system calls so child processes do not inherit 
     * it.
     */

    CloseOnExec(pair[1]);

    /* Fork child process */

    child_pid = fork();

    if (child_pid == 0)
    {
        /* Child */
        close(pair[1]);
        Child(arg0, pair[0]);
    }
    else if (child_pid > 0)
    {
        /* Parent */
        close(pair[0]);
        Executor(pair[1], child_pid);
    }
    else
        Fatal(FL, "fork() failed");

    return 0;
}
