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

#include <cstdio>
#include "cimservera.h"
#include <Executor/Executor.h>

int main(int argc, char* argv[])
{
    // Check argumnents.

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <socket-number>\n", argv[0]);
        exit(1);
    }

    // Open socket stream.

    int sock;
    {
        char* end;
        long x = strtoul(argv[1], &end, 10);

        if (*end != '\0')
        {
            fprintf(stderr, "%s : bad socket argument: %s\n", argv[0], argv[1]);
            exit(1);
        }
        
        sock = int(x);
    }

    // Wait on request.

    CimserveraRequest request;

    // Wait on request.
    if (CimserveraRecv(sock, &request, sizeof(request)) != sizeof(request))
    {
        close(sock);
        return -1;
    }

    if (strcmp(request.arg0, "authenticate") == 0)
    {
        int status = PAMAuthenticate(request.arg1, request.arg2);
        CimserveraSend(sock, &status, sizeof(status));
    }
    else if (strcmp(request.arg0, "validateUser") == 0)
    {
        int status = PAMValidateUser(request.arg1);
        CimserveraSend(sock, &status, sizeof(status));
    }
    else
    {
        fprintf(stderr, "%s: bad request\n", argv[0]);
        close(sock);
        exit(1);
    }

    close(sock);

    exit(0);
    return 0;
}
