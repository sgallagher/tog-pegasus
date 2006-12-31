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

#ifndef Pegasus_cimservera_h
#define Pegasus_cimservera_h

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <Pegasus/Common/Constants.h>
#include <Executor/Executor.h>
#include <Executor/Strlcpy.h>
#include <Executor/Strlcat.h>

#define CIMSERVERA "cimservera"

//==============================================================================
//
// cimservera
//
//     This program is used to authenticate users with the "Basic PAM 
//     Authentication" scheme. It was originally written to isolate memory
//     errors in old PAM modules to an external process.
//
//     This header defines two functions that may be called by clients of this
//     process (the parent process).
//
//         CimserveraAuthenticate()
//         CimserveraValidateUser()
//
//     Each functions forks and executes a child process that carries out
//     the request and then exits immediately. The parent and child proceses
//     communicate over a local domain socket, created by the parent just
//     before executing the client program.
//
//     Both of the functions above are defined in the header to avoid the need
//     to link a separate client library.
//
//     CAUTION: This program must not depend on any Pegasus libraries since
//     it is used by the executor process.
//
//==============================================================================

#define CIMSERVERA_MAX_BUFFER 1024

//==============================================================================
//
// CimserveraSend()
//
//==============================================================================

static ssize_t CimserveraSend(int sock, void* buffer, size_t size)
{
    size_t r = size;
    char* p = (char*)buffer;

    while (r)
    {
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
// CimserveraRecv()
//
//     Receives *size* bytes from the given socket.
//
//==============================================================================

static ssize_t CimserveraRecv(int sock, void* buffer, size_t size)
{
    size_t r = size;
    char* p = (char*)buffer;

    if (size == 0)
        return -1;

    while (r)
    {
        ssize_t n;

        EXECUTOR_RESTART(read(sock, p, r), n);

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
// CimserveraStart()
//
//     Starts the cimservera program, returning a socket used to communicate
//     with it.
//
//==============================================================================

static int CimserveraStart()
{
    // Create socket pair for communicating with child process.

    int pair[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) != 0)
        return -1;

    // Get absolute path of "cimservera" program.

    char path[CIMSERVERA_MAX_BUFFER];

    if (PEGASUS_PAM_STANDALONE_PROC_NAME[0] == '/')
        Strlcpy(path, PEGASUS_PAM_STANDALONE_PROC_NAME, CIMSERVERA_MAX_BUFFER);
    else
    {
        const char* home = getenv("PEGASUS_HOME");

        if (!home)
            return -1;

        Strlcpy(path, home, CIMSERVERA_MAX_BUFFER);
        Strlcat(path, "/", CIMSERVERA_MAX_BUFFER);
        Strlcat(path, PEGASUS_PAM_STANDALONE_PROC_NAME, CIMSERVERA_MAX_BUFFER);
    }

    // Fork child:

    int pid = fork();

    if (pid < 0)
        return -1;

    // Child process:

    if (pid == 0)
    {
        close(pair[1]);

        // Convert socket number to string.

        char sockStr[32];
        sprintf(sockStr, "%d", pair[0]);

        // Build arguments for execv().

        char* argv[3];
        argv[0] = CIMSERVERA;
        argv[1] = sockStr;
        argv[2] = 0;

        // Execute child:

        execv(path, argv);
        _exit(0);
    }

    // Parent process:

    close(pair[0]);

    return pair[1];
}

//==============================================================================
//
// CimserveraRequest
//
//==============================================================================

struct CimserveraRequest
{
    char arg0[CIMSERVERA_MAX_BUFFER];
    char arg1[CIMSERVERA_MAX_BUFFER];
    char arg2[CIMSERVERA_MAX_BUFFER];
};

//==============================================================================
//
// CimserveraAuthenticate()
//
//==============================================================================

static int CimserveraAuthenticate(const char* username, const char* password)
{
    // Create the CIMSERVERA process.

    int sock = CimserveraStart();

    if (sock == -1)
        return -1;

    // Send request to CIMSERVERA process.

    CimserveraRequest request;
    memset(&request, 0, sizeof(request));
    Strlcpy(request.arg0, "authenticate", CIMSERVERA_MAX_BUFFER);
    Strlcpy(request.arg1, username, CIMSERVERA_MAX_BUFFER);
    Strlcpy(request.arg2, password, CIMSERVERA_MAX_BUFFER);

    if (CimserveraSend(sock, &request, sizeof(request)) != sizeof(request))
    {
        close(sock);
        return -1;
    } 

    // Receive response back from child process.

    int status;

    if (CimserveraRecv(sock, &status, sizeof(status)) != sizeof(status))
    {
        close(sock);
        return -1;
    }

    return status;
}

//==============================================================================
//
// CimserveraAuthenticate()
//
//==============================================================================

static int CimserveraValidateUser(const char* username)
{
    // Create the CIMSERVERA process.

    int sock = CimserveraStart();

    if (sock == -1)
        return -1;

    // Send request to CIMSERVERA process.

    CimserveraRequest request;
    memset(&request, 0, sizeof(request));
    Strlcpy(request.arg0, "validateUser", CIMSERVERA_MAX_BUFFER);
    Strlcpy(request.arg1, username, CIMSERVERA_MAX_BUFFER);

    if (CimserveraSend(sock, &request, sizeof(request)) != sizeof(request))
    {
        close(sock);
        return -1;
    }

    // Receive response back from child process.

    int status;

    if (CimserveraRecv(sock, &status, sizeof(status)) != sizeof(status))
    {
        close(sock);
        return -1;
    }

    return status;
}

#endif /* Pegasus_cimservera_h */
