#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "Parent.h"
#include "Log.h"
#include "Messages.h"
#include "Socket.h"
#include "Fatal.h"
#include "Globals.h"
#include "Path.h"
#include "User.h"
#include "Bit.h"
#include "File.h"
#include "Exit.h"
#include "Strlcpy.h"
#include "LocalAuth.h"

#if defined(PEGASUS_PAM_AUTHENTICATION)
#include <Pegasus/Security/Cimservera/cimservera.h>
#endif

//==============================================================================
//
// _sigHandler()
//
//     Signal handler for SIGTERM.
//
//==============================================================================

static void _sigHandler(int signum)
{
    SetBit(globalSignalMask, signum);
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

    if (request.uid == globalChildUid)
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

        if (GetInternalPegasusProgramPath(CIMPROVAGT, path) != 0)
            Fatal(FL, "Failed to locate Pegasus program: %s", CIMPROVAGT);

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

            /* Flawfinder: ignore */
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
    // ATTN: do we need to call setsid()?
    // ATTN: do we need to fork twice?
    // ATTN: compare with Stevens daemonization example.

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

    signal(SIGTERM, _sigHandler);

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
// HandleStartLocalAuthRequest()
//
//==============================================================================

static void HandleStartLocalAuthRequest(int sock)
{
    // Read the request request.

    struct ExecutorStartLocalAuthRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    Log(LL_TRACE, "HandleStartLocalAuthRequest(): user=%s", request.user);

    // Peform operation.

    char path[EXECUTOR_BUFFER_SIZE];
    SessionKey key;

    int status = StartLocalAuthentication(request.user, path, &key);

    if (status != 0)
        Log(LL_WARNING, "Local user authentication failed on %s", request.user);

    // Send response message.

    struct ExecutorStartLocalAuthResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;
    Strlcpy(response.path, path, sizeof(response.path));
    Strlcpy(response.key, key.data, sizeof(response.key));

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// HandleFinishLocalAuthRequest()
//
//==============================================================================

static void HandleFinishLocalAuthRequest(int sock)
{
    // Read the request request.

    struct ExecutorFinishLocalAuthRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    Log(LL_TRACE, "HandleFinishLocalAuthRequest(): key=%s token=%s",
        request.key, request.token);

    // Peform operation.

    SessionKey key;
    Strlcpy(key.data, request.key, sizeof(key.data));
    SessionKey newKey;

    int status = FinishLocalAuthentication(&key, request.token, &newKey);

    // ATTN: add user to message.

    if (status != 0)
        Log(LL_WARNING, "Local user authentication failed on %s", key.data);

    struct ExecutorFinishLocalAuthResponse response;
    memset(&response, 0, sizeof(response));
    response.status = status;
    Strlcpy(response.key, newKey.data, sizeof(response.key));

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

//==============================================================================
//
// Parent()
//
//     The parent process (cimserver).
//
//==============================================================================

void Parent(int sock, int childPid)
{
    // Handle Ctrl-C.

    signal(SIGINT, _sigHandler);

    // Catch SIGTERM, sent by kill program.

    signal(SIGTERM, _sigHandler);

    // Ignore SIGPIPE, which occurs if a child with whom the executor shares
    // a local domain socket unexpectedly dies. In such a case, the socket
    // read/write functions will return an error. There are two child processes
    // the executor talks to over sockets: cimservera and cimservermain.

    signal(SIGPIPE, SIG_IGN);

    // Save child PID globally; it is used by Exit() function.

    globalChildPid = childPid;

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

        switch (ExecutorMessageCode(header.code))
        {
            case EXECUTOR_PING_MESSAGE:
                HandlePingRequest(sock);
                break;

            case EXECUTOR_OPEN_FILE_MESSAGE:
                HandleOpenFileRequest(sock);
                break;

            case EXECUTOR_START_PROVIDER_AGENT_MESSAGE:
                HandleStartProviderAgentRequest(sock);
                break;

            case EXECUTOR_DAEMONIZE_EXECUTOR_MESSAGE:
                HandleDaemonizeExecutorRequest(sock);
                break;

            case EXECUTOR_CHANGE_OWNER_MESSAGE:
                HandleChangeOwnerRequest(sock);
                break;

            case EXECUTOR_RENAME_FILE_MESSAGE:
                HandleRenameFileRequest(sock);
                break;

            case EXECUTOR_REMOVE_FILE_MESSAGE:
                HandleRemoveFileRequest(sock);
                break;

            case EXECUTOR_WAIT_PID_MESSAGE:
                HandleWaitPidRequest(sock);
                break;

            case EXECUTOR_PAM_AUTHENTICATE_MESSAGE:
                HandlePAMAuthenticateRequest(sock);
                break;

            case EXECUTOR_PAM_VALIDATE_USER_MESSAGE:
                HandlePAMValidateUserRequest(sock);
                break;

            case EXECUTOR_START_LOCAL_AUTH_MESSAGE:
                HandleStartLocalAuthRequest(sock);
                break;

            case EXECUTOR_FINISH_LOCAL_AUTH_MESSAGE:
                HandleFinishLocalAuthRequest(sock);
                break;

            default:
                Fatal(FL, "invalid request code: %d", header.code);
                break;
        }
    }

    // Reached due to socket EOF or SIGTERM.

    Exit(0);
}
