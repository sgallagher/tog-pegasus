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

#include "cimservera.h"
#include <cstdio>
#include <syslog.h>

static void Exit(int status)
{
    if (status == 0)
        syslog(LOG_INFO, "exiting with status=%d", status);
    else
        syslog(LOG_WARNING, "exiting with status=%d", status);

    exit(status);
}

int main(int argc, char* argv[])
{
    // Open syslog:

    openlog("cimservera", 0, LOG_AUTH);
    syslog(LOG_INFO, "started");

    // ATTN: Insert fingerprint logic to detect running of this program as
    // non-child of executor.

    // Check argumnents.

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <socket-number>\n", argv[0]);
        Exit(1);
    }

    // Open socket stream.

    int sock;
    {
        char* end;
        long x = strtoul(argv[1], &end, 10);

        if (*end != '\0')
        {
            fprintf(stderr, "%s : bad socket argument: %s\n", argv[0], argv[1]);
            Exit(1);
        }
        
        sock = int(x);
    }

    // Wait on request.

    CimserveraRequest request;

    // Wait on request.
    if (Recv(sock, &request, sizeof(request)) != sizeof(request))
    {
        close(sock);
        Exit(1);
    }

    if (strcmp(request.arg0, "authenticate") == 0)
    {
        int status = PAMAuthenticateInProcess(request.arg1, request.arg2);

        if (status != 0)
        {
            syslog(LOG_WARNING, "PAM authentication failed on user \"%s\"",
                request.arg1);
        }

        Send(sock, &status, sizeof(status));
    }
    else if (strcmp(request.arg0, "validateUser") == 0)
    {
        int status = PAMValidateUserInProcess(request.arg1);

        if (status != 0)
        {
            syslog(LOG_WARNING, "PAM user validation failed on user \"%s\"",
                request.arg1);
        }

        Send(sock, &status, sizeof(status));
    }
    else
    {
        syslog(LOG_WARNING, "invalid request");
        close(sock);
        Exit(1);
    }

    close(sock);

    Exit(0);
}
