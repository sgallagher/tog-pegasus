/*
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
//%/////////////////////////////////////////////////////////////////////////////
*/
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
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
# include "PAMAuth.h"
#endif

/*
**==============================================================================
**
** _sigHandler()
**
**     Signal handler for SIGTERM.
**
**==============================================================================
*/

static void _sigHandler(int signum)
{
    SetBit(&globals.signalMask, signum);
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
    struct ExecutorPingResponse response = { EXECUTOR_PING_MAGIC };

    Log(LL_TRACE, "HandlePingRequest()");

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
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
    int fd;
    struct ExecutorOpenFileResponse response;

    /* Read the request request. */

    struct ExecutorOpenFileRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    /* Open the file. */

    Log(LL_TRACE, "HandleOpenFileRequest(): path=%s", request.path);

    fd = -1;

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

        case 'a':
        {
            fd = open(
                request.path, 
                O_WRONLY | O_CREAT | O_APPEND,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            break;
        }
    }

    /* Send response message. */

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

    /* Send descriptor to calling process (if any to send). */

    if (fd != -1)
    {
        int descriptors[1];
        descriptors[0] = fd;
        SendDescriptorArray(sock, descriptors, 1);
        close(fd);
    }
}

/*
**==============================================================================
**
** HandleStartProviderAgentRequest()
**
**==============================================================================
*/

static void HandleStartProviderAgentRequest(int sock)
{
    int status;
    int pid;
    int to[2];
    int from[2];
    struct ExecutorStartProviderAgentResponse response;
    struct ExecutorStartProviderAgentRequest request;

    /* Read request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    Log(LL_TRACE, "HandleStartProviderAgentRequest(): module=%s gid=%d uid=%d",
        request.module, request.gid, request.uid);

    /* Check for a valid session key. */

    if (globals.enableAuthentication)
    {
        SessionKey key;
        int uid;

        Strlcpy(key.data, request.key, sizeof(key.data));

        if (TestValidSessionKey(&key) != 0)
        {
            Log(LL_SEVERE, 
                "Invalid session key in HandleStartProviderAgentRequest(): %s",
                key.data);
        }
        else if (TestNullSessionKey(&key) == 0)
        {
            Log(LL_SEVERE, 
                "Null session key in HandleStartProviderAgentRequest(): %s",
                key.data);
        }
        else if (GetSessionKeyUid(&key, &uid) != 0)
        {
            Log(LL_WARNING, 
                "Unknown session key in HandleStartProviderAgentRequest(): %s",
                key.data);
        }
    }

/*
MEB: remove following logic.
*/

    /*
     * Map cimservermain user to root to preserve pre-privilege-separation
     * behavior.
     */

    if (request.uid == globals.childUid)
    {
        Log(LL_TRACE, 
            "using root instead of cimservermain user for cimprovagt");

        request.uid = 0;
        request.gid = 0;
    }

    /* Process request. */

    status = 0;
    pid = -1;

    do
    {
        /* Resolve full path of "cimprovagt". */

        char path[EXECUTOR_BUFFER_SIZE];

        if (GetInternalPegasusProgramPath(CIMPROVAGT, path) != 0)
            Fatal(FL, "Failed to locate Pegasus program: %s", CIMPROVAGT);

        /* Create "to-agent" pipe: */

        if (pipe(to) != 0)
        {
            status = -1;
            break;
        }

        /* Create "from-agent" pipe: */

        if (pipe(from) != 0)
        {
            status = -1;
            break;
        }

        /* Fork process: */

        pid = fork();

        if (pid < 0)
        {
            /* ATTN: log this. */
            status = -1;
            break;
        }

        /* If child: */

        if (pid == 0)
        {
            char username[EXECUTOR_BUFFER_SIZE];
            struct rlimit rlim;
            char arg1[32];
            char arg2[32];

            /* Close unused pipe descriptors: */

            close(to[1]);
            close(from[0]);

            /* 
             * Close unused descriptors. Leave stdin, stdout, stderr, and the
             * child's pipe descriptors open. 
             */


            if (getrlimit(RLIMIT_NOFILE, &rlim) == 0)
            {
                int i;

                for (i = 3; i < (int)rlim.rlim_cur; i++)
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

            if (GetUserName(getuid(), username) != 0)
                Fatal(FL, "failed to resolve username for uid=%d", getuid());

            Log(LL_INFORMATION, "starting %s on module %s as user %s",
                path, request.module, username);

# endif /* !defined(PEGASUS_DISABLE_PROV_USERCTXT) */

            /* Exec the cimprovagt program. */

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

    /* Close unused pipe descriptors. */

    close(to[0]);
    close(from[1]);

    /* Send response. */

    response.status = status;
    response.pid = pid;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");

    /* Send descriptors to calling process. */

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

/*
**==============================================================================
**
** HandleDaemonizeExecutorRequest()
**
**==============================================================================
*/

static void HandleDaemonizeExecutorRequest(int sock)
{
    struct ExecutorDaemonizeExecutorResponse response = { 0 };
    int pid;

    /* ATTN: do we need to call setsid()? */
    /* ATTN: do we need to fork twice? */
    /* ATTN: compare with Stevens daemonization example. */

    Log(LL_TRACE, "HandleDaemonizeExecutorRequest()");

    /* Fork: */

    pid = fork();

    if (pid < 0)
    {
        response.status = -1;
        Fatal(FL, "fork() failed");

        if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
            Fatal(FL, "failed to write response");
    }

    /* Parent exits: */

    if (pid > 0)
        exit(0);

    /* Ignore SIGHUP: */

    signal(SIGHUP, SIG_IGN);

    /* Catch SIGTERM: */

    signal(SIGTERM, _sigHandler);

    /* Set current directory to root: */

    chdir("/");

    /* Close these file descriptors (stdin, stdout, stderr). */

    close(0);
    close(1);
    close(2);

    /* Direct standard input, output, and error to /dev/null: */

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

/*
**==============================================================================
**
** HandleRenameFileRequest()
**
**==============================================================================
*/

static void HandleRenameFileRequest(int sock)
{
    int status;
    struct ExecutorRenameFileResponse response;
    struct ExecutorRenameFileRequest request;

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    /* Rename the file. */

    Log(LL_TRACE, "HandleRenameFileRequest(): oldPath=%s newPath=%s", 
        request.oldPath, request.newPath);

    /* Perform the operation: */

    status = -1;

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

    /* Send response message. */

    memset(&response, 0, sizeof(response));
    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

/*
**==============================================================================
**
** HandleRemoveFileRequest()
**
**==============================================================================
*/

static void HandleRemoveFileRequest(int sock)
{
    int status;
    struct ExecutorRemoveFileRequest request;
    struct ExecutorRemoveFileResponse response;

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    /* Remove the file. */

    Log(LL_TRACE, "HandleRemoveFileRequest(): path=%s", request.path);

    status = unlink(request.path);

    if (status != 0)
        Log(LL_WARNING, "unlink(%s) failed", request.path);

    /* Send response message. */

    memset(&response, 0, sizeof(response));
    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

/*
**==============================================================================
**
** HandleWaitPidRequest()
**
**==============================================================================
*/

static void HandleWaitPidRequest(int sock)
{
    int status;
    struct ExecutorWaitPidRequest request;
    struct ExecutorWaitPidResponse response;

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    /* Wait on the PID: */

    Log(LL_TRACE, "HandleWaitPidRequest(): pid=%d", request.pid);

    EXECUTOR_RESTART(waitpid(request.pid, 0, 0), status);

    if (status == -1)
        Log(LL_WARNING, "waitpid(%d, 0, 0) failed", request.pid);

    /* Send response message. */

    memset(&response, 0, sizeof(response));
    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

/*
**==============================================================================
**
** HandleAuthenticatePasswordRequest()
**
**==============================================================================
*/

static void HandleAuthenticatePasswordRequest(int sock)
{
    int status;
    struct ExecutorAuthenticatePasswordResponse response;
    int gid;
    int uid;
    SessionKey key;

    /* Read the request request. */

    struct ExecutorAuthenticatePasswordRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    /* Rename the file. */

    Log(LL_TRACE, "HandleAuthenticatePasswordRequest(): username=%s",
        request.username);

    /* Perform the operation: */

    status = 0;
    do
    {
#if defined(PEGASUS_PAM_AUTHENTICATION)

        if (GetUserInfo(request.username, &uid, &gid) != 0)
        {
            status = -1;
            break;
        }

        if (PAMAuthenticate(request.username, request.password) != 0)
        {
            status = -1;
            break;
        }

        /* Generate session key */

        key = NewSessionKey(uid, 0, 0);
    
#else /* !PEGASUS_PAM_AUTHENTICATION */

        status = -1;
        break;

#endif /* !PEGASUS_PAM_AUTHENTICATION */
    }
    while (0);

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

    /* Send response message. */

    memset(&response, 0, sizeof(response));
    response.status = status;
    Strlcpy(response.key, key.data, sizeof(response.key));

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

/*
**==============================================================================
**
** HandleValidateUserRequest()
**
**==============================================================================
*/

static void HandleValidateUserRequest(int sock)
{
    int status;
    struct ExecutorValidateUserResponse response;
    struct ExecutorValidateUserRequest request;

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    /* Validate the user with PAM. */

    Log(LL_TRACE, 
        "HandleValidateUserRequest(): username=%s", request.username);

    /* Get the uid for the username. */

    status = 0;

#if defined(PEGASUS_PAM_AUTHENTICATION)

    if (PAMValidateUser(request.username) != 0)
        status = -1;

#else /* !PEGASUS_PAM_AUTHENTICATION */

    status = -1;

#endif /* !PEGASUS_PAM_AUTHENTICATION */

    if (status != 0)
        Log(LL_WARNING, "PAM user validation failed on %s", request.username);

    /* Send response message. */

    memset(&response, 0, sizeof(response));
    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

/*
**==============================================================================
**
** HandleChallengeLocalRequest()
**
**==============================================================================
*/

static void HandleChallengeLocalRequest(int sock)
{
    char path[EXECUTOR_BUFFER_SIZE];
    SessionKey key;
    int status;
    struct ExecutorChallengeLocalRequest request;
    struct ExecutorChallengeLocalResponse response;

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    Log(LL_TRACE, "HandleChallengeLocalRequest(): user=%s", request.user);

    /* Peform operation. */

    status = StartLocalAuthentication(request.user, path, &key);

    if (status != 0)
    {
        Log(LL_WARNING, "Local authentication failed for user \"%s\"", 
            request.user);
    }

    /* Send response message. */

    memset(&response, 0, sizeof(response));
    response.status = status;
    Strlcpy(response.challenge, path, sizeof(response.challenge));
    Strlcpy(response.key, key.data, sizeof(response.key));

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

/*
**==============================================================================
**
** HandleAuthenticateLocalRequest()
**
**==============================================================================
*/

static void HandleAuthenticateLocalRequest(int sock)
{
    SessionKey key;
    int status;
    struct ExecutorAuthenticateLocalResponse response;

    /* Read the request request. */

    struct ExecutorAuthenticateLocalRequest request;

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    Log(LL_TRACE, "HandleAuthenticateLocalRequest()");

    /* Peform operation. */

    Strlcpy(key.data, request.key, sizeof(key.data));
    status = FinishLocalAuthentication(&key, request.token);

    /* Log result. */

    if (status == 0)
        SetSessionKeyAuthenticated(&key);
    else
    {
        int uid;
        GetSessionKeyUid(&key, &uid);

        Log(LL_WARNING, "Local authentication failed for uid=%d", uid);
    }

    /* Send response. */

    memset(&response, 0, sizeof(response));
    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

/*
**==============================================================================
**
** HandleNewSessionKeyRequest()
**
**==============================================================================
*/

static void HandleNewSessionKeyRequest(int sock)
{
    struct ExecutorNewSessionKeyRequest request;
    struct ExecutorNewSessionKeyResponse response;
    SessionKey key;
    int status;

    /* Read the request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "failed to read request");

    /* Log request. */

    Log(LL_TRACE, "HandleNewSessionKeyRequest(): username=%s",request.username);

    /* Perform operation. */

    do
    {
        int uid;
        int gid;

        if (GetUserInfo(request.username, &uid, &gid) != 0)
        {
            Log(LL_WARNING, "GetUserInfo(%s, ...) failed", request.username);
            status = -1;
            break;
        }

        key = NewSessionKey(uid, 0, 0);
        status = 0;

        Log(LL_TRACE, 
            "create new session key for usr \"%s\"\n", request.username);
    }
    while (0);

    /* Send response. */

    memset(&response, 0, sizeof(response));
    response.status = status;
    Strlcpy(response.key, key.data, sizeof(response.key));

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "failed to write response");
}

/*
**==============================================================================
**
** Parent()
**
**     The parent process (cimserver).
**
**==============================================================================
*/

void Parent(int sock, int childPid)
{
    /* Handle Ctrl-C. */

    signal(SIGINT, _sigHandler);

    /* Catch SIGTERM, sent by kill program. */

    signal(SIGTERM, _sigHandler);

    /*
     * Ignore SIGPIPE, which occurs if a child with whom the executor shares
     * a local domain socket unexpectedly dies. In such a case, the socket
     * read/write functions will return an error. There are two child processes
     * the executor talks to over sockets: cimservera and cimservermain.
     */

    signal(SIGPIPE, SIG_IGN);

    /* Save child PID globally; it is used by Exit() function. */

    globals.childPid = childPid;

    /* Prepares socket into non-blocking I/O. */

    SetNonBlocking(sock);

    /* Process client requests until client exists. */

    for (;;)
    {
        size_t n;
        struct ExecutorRequestHeader header;

        /* Receive request header. */

        n = RecvNonBlock(sock, &header, sizeof(header));

        if (n == 0)
        {
            /*
             * Either client closed its end of the pipe (possibly by exiting)
             * or we caught a SIGTERM.
             */
            break;
        }

        if (n != sizeof(header))
            Fatal(FL, "failed to read header");

        /* Dispatch request. */

        switch ((enum ExecutorMessageCode)header.code)
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

            case EXECUTOR_RENAME_FILE_MESSAGE:
                HandleRenameFileRequest(sock);
                break;

            case EXECUTOR_REMOVE_FILE_MESSAGE:
                HandleRemoveFileRequest(sock);
                break;

            case EXECUTOR_WAIT_PID_MESSAGE:
                HandleWaitPidRequest(sock);
                break;

            case EXECUTOR_AUTHENTICATE_PASSWORD_MESSAGE:
                HandleAuthenticatePasswordRequest(sock);
                break;

            case EXECUTOR_VALIDATE_USER_MESSAGE:
                HandleValidateUserRequest(sock);
                break;

            case EXECUTOR_CHALLENGE_LOCAL_MESSAGE:
                HandleChallengeLocalRequest(sock);
                break;

            case EXECUTOR_AUTHENTICATE_LOCAL_MESSAGE:
                HandleAuthenticateLocalRequest(sock);
                break;

            case EXECUTOR_NEW_SESSION_KEY_MESSAGE:
                HandleNewSessionKeyRequest(sock);
                break;

            default:
                Fatal(FL, "invalid request code: %d", header.code);
                break;
        }
    }

    /* Reached due to socket EOF, SIGTERM, or SIGINT. */

    Exit(0);
}
