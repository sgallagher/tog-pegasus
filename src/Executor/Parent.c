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
#include "Exit.h"
#include "Strlcpy.h"
#include "LocalAuth.h"
#include "Strlcat.h"
#include "PasswordFile.h"
#include "Policy.h"
#include "Macro.h"

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
    globals.signalMask |= (1 << signum);
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

    memset(&response, 0, sizeof(response));
    response.magic = EXECUTOR_PING_MAGIC;

    Log(LL_TRACE, "HandlePingRequest()");

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "Failed to write response");
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

    memset(&response, 0, sizeof(response));

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "Failed to read request");

    /* Log the request. */

    Log(LL_TRACE, "HandleOpenFileRequest(): path=\"%s\", mode='%c'", 
        request.path, request.mode);

    /* Perform the operation. */

    response.status = 0;
    fd = -1;

    do
    {
        /* Check the policy. */

        if (CheckOpenFilePolicy(request.path, request.mode) != 0)
        {
            response.status = -1;
            break;
        }

        /* Open file. */

        switch (request.mode)
        {
            case 'r':
                fd = open(request.path, O_RDONLY);
                break;

            case 'w':
                fd = open(
                    request.path, 
                    O_WRONLY | O_CREAT | O_TRUNC,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // 0644
                break;

            case 'a':
            {
                fd = open(
                    request.path, 
                    O_WRONLY | O_CREAT | O_APPEND,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // 0644
                break;
            }
        }

        if (fd == -1)
            response.status = -1;
    }
    while (0);

    /* Log failure. */

    if (response.status != 0)
    {
        Log(LL_WARNING, "open(\"%s\", '%c') failed", 
            request.path, request.mode);
    }

    /* Send response. */

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "Failed to write response");

    /* Send descriptor to calling process (if any to send). */

    if (response.status == 0)
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

    memset(&response, 0, sizeof(response));

    /* Read request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "Failed to read request");

    /* Log request. */

    Log(LL_TRACE, "HandleStartProviderAgentRequest(): "
        "module=%s gid=%d uid=%d", request.module, request.gid, request.uid);

    /* Process request. */

    status = 0;
    pid = -1;

    do
    {
        /* Resolve full path of CIMPROVAGT. */

        const char* path;

        if ((path = FindMacro("cimprovagtPath")) == NULL)
            Fatal(FL, "Failed to locate %s program", CIMPROVAGT);

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
            Log(LL_SEVERE, "fork failed");
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
                Fatal(FL, "Failed to resolve username for uid=%d", getuid());

            Log(LL_TRACE, "starting %s on module %s as user %s",
                path, request.module, username);

# endif /* !defined(PEGASUS_DISABLE_PROV_USERCTXT) */

            /* Exec the CIMPROVAGT program. */

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
        Fatal(FL, "Failed to write response");

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
    struct ExecutorDaemonizeExecutorResponse response;
    int pid;

    memset(&response, 0, sizeof(response));

    Log(LL_TRACE, "HandleDaemonizeExecutorRequest()");

    /* Fork (parent exits; child continues) */

    pid = fork();

    if (pid < 0)
    {
        response.status = -1;
        Fatal(FL, "fork() failed");
    }

    if (pid > 0)
        _exit(0);

    /* Become session leader */

    if (setsid() < 0)
    {
        response.status = -1;
        Fatal(FL, "setsid() failed");
    }

    /* Ignore SIGHUP: */

    signal(SIGHUP, SIG_IGN);

    /* Fork again: */

    pid = fork();

    if (pid < 0)
    {
        response.status = -1;
        Fatal(FL, "fork() failed");
    }

    if (pid > 0)
        _exit(0);

    /* Catch SIGTERM: */

    signal(SIGTERM, _sigHandler);

    /* Make root the current directory. */

    chdir("/");

    /* Close these file descriptors (stdin, stdout, stderr). */

    close(0);
    close(1);
    close(2);

    /* Direct standard input, output, and error to /dev/null: */

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    response.status = 0;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "Failed to write response");
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
    struct ExecutorRenameFileResponse response;
    struct ExecutorRenameFileRequest request;

    memset(&response, 0, sizeof(response));

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "Failed to read request");

    /* Rename the file. */

    Log(LL_TRACE, "HandleRenameFileRequest(): oldPath=%s newPath=%s", 
        request.oldPath, request.newPath);

    /* Perform the operation: */

    response.status = 0;

    do
    {
        /* Check the policy. */

        if (CheckRenameFilePolicy(request.oldPath, request.newPath) != 0)
        {
            response.status = -1;
            break;
        }

        /* Rename the file. */

        if (rename(request.oldPath, request.newPath) != 0)
        {
            response.status = -1;
            break;
        }
    }
    while (0);

    /* Send response message. */

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "Failed to write response");
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
    struct ExecutorRemoveFileRequest request;
    struct ExecutorRemoveFileResponse response;

    memset(&response, 0, sizeof(response));

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "Failed to read request");

    /* Remove the file. */

    Log(LL_TRACE, "HandleRemoveFileRequest(): path=%s", request.path);

    response.status = 0;

    do
    {
        /* Check the policy. */

        if (CheckRemoveFilePolicy(request.path) != 0)
        {
            response.status = -1;
            break;
        }

        /* Remove the file. */

        if (unlink(request.path) != 0)
        {
            response.status = -1;
            Log(LL_WARNING, "unlink(%s) failed", request.path);
            break;
        }
    }
    while (0);

    /* Send response message. */

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "Failed to write response");
}

/*
**==============================================================================
**
** HandleReapProviderAgentRequest()
**
**==============================================================================
*/

static void HandleReapProviderAgentRequest(int sock)
{
    int status;
    struct ExecutorReapProviderAgentRequest request;
    struct ExecutorReapProviderAgentResponse response;

    memset(&response, 0, sizeof(response));

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "Failed to read request");

    /* Log request. */

    Log(LL_TRACE, "HandleReapProviderAgentRequest(): pid=%d", request.pid);

    /* Perform operation. */

    status = 0;

    do
    {
        /* Wait on the PID. */

        EXECUTOR_RESTART(waitpid(request.pid, 0, 0), status);

        if (status == -1)
            Log(LL_WARNING, "waitpid(%d, 0, 0) failed", request.pid);
    }
    while (0);

    /* Send response message. */

    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "Failed to write response");
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
    struct ExecutorAuthenticatePasswordRequest request;
    struct ExecutorAuthenticatePasswordResponse response;
    int gid;
    int uid;

    memset(&response, 0, sizeof(response));

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "Failed to read request");

    /* Rename the file. */

    Log(LL_TRACE, "HandleAuthenticatePasswordRequest(): username=%s",
        request.username);

    /* Perform the operation: */

    status = 0;

    do
    {
        if (GetUserInfo(request.username, &uid, &gid) != 0)
        {
            status = -1;
            break;
        }

#if defined(PEGASUS_PAM_AUTHENTICATION)

        if (PAMAuthenticate(request.username, request.password) != 0)
        {
            status = -1;
            break;
        }


#else /* !PEGASUS_PAM_AUTHENTICATION */

        {
            const char* path = FindMacro("passwordFilePath");

            if (!path)
            {
                status = -1;
                break;
            }

            if (CheckPasswordFile(
                path, request.username, request.password) != 0)
            {
                status = -1;
                break;
            }
        }

#endif /* !PEGASUS_PAM_AUTHENTICATION */
    }
    while (0);

    if (status != 0)
    {
        Log(LL_WARNING, "Basic authentication failed for username %s", 
            request.username);
    }
    else
    {
        Log(LL_TRACE, "Basic authentication succeeded for username %s", 
            request.username);
    }

    /* Send response message. */

    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "Failed to write response");
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

    memset(&response, 0, sizeof(response));

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "Failed to read request");

    /* Validate the user. */

    Log(LL_TRACE, 
        "HandleValidateUserRequest(): username=%s", request.username);

    /* Get the uid for the username. */

    status = 0;

#if defined(PEGASUS_PAM_AUTHENTICATION)

    if (PAMValidateUser(request.username) != 0)
        status = -1;

#else /* !PEGASUS_PAM_AUTHENTICATION */

    do
    {
        const char* path = FindMacro("passwordFilePath");

        if (!path)
        {
            status = -1;
            break;
        }

        if (CheckPasswordFile(path, request.username, NULL) != 0)
        {
            status = -1;
            break;
        }
    }
    while (0);

#endif /* !PEGASUS_PAM_AUTHENTICATION */

    if (status != 0)
        Log(LL_WARNING, "User validation failed on %s", request.username);

    /* Send response message. */

    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "Failed to write response");
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
    char challenge[EXECUTOR_BUFFER_SIZE];
    int status;
    struct ExecutorChallengeLocalRequest request;
    struct ExecutorChallengeLocalResponse response;

    memset(&response, 0, sizeof(response));

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "Failed to read request");

    Log(LL_TRACE, "HandleChallengeLocalRequest(): user=%s", request.user);

    /* Peform operation. */

    status = StartLocalAuthentication(request.user, challenge);

    if (status != 0)
    {
        Log(LL_WARNING, "Local authentication failed for user \"%s\"", 
            request.user);
    }

    /* Send response message. */

    response.status = status;

    if (status == 0)
        Strlcpy(response.challenge, challenge, sizeof(response.challenge));

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "Failed to write response");
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
    int status;
    struct ExecutorAuthenticateLocalRequest request;
    struct ExecutorAuthenticateLocalResponse response;

    memset(&response, 0, sizeof(response));

    /* Read the request request. */

    if (RecvNonBlock(sock, &request, sizeof(request)) != sizeof(request))
        Fatal(FL, "Failed to read request");

    Log(LL_TRACE, "HandleAuthenticateLocalRequest()");

    /* Peform operation. */

    status = FinishLocalAuthentication(request.challenge, request.response);

    /* Log result. */

    if (status != 0)
    {
        Log(LL_WARNING, "Local authentication failed");
    }

    /* Send response. */

    response.status = status;

    if (SendNonBlock(sock, &response, sizeof(response)) != sizeof(response))
        Fatal(FL, "Failed to write response");
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
     * the executor talks to over sockets: CIMSERVERA and CIMSERVERMAIN.
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
            Fatal(FL, "Failed to read header");

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

            case EXECUTOR_REAP_PROVIDER_AGENT:
                HandleReapProviderAgentRequest(sock);
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

            default:
                Fatal(FL, "Invalid request code: %d", header.code);
                break;
        }
    }

    /* Reached due to socket EOF, SIGTERM, or SIGINT. */

    Exit(0);
}
