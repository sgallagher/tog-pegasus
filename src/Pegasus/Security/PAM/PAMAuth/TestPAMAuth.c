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
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////
*/

#include <Executor/PAMAuth.h>
#include <Executor/tests/TestAssert.h>
#include <stdio.h>

#define STANDALONE

#if defined(STANDALONE)
# define VALIDATE_USER PAMValidateUser
# define AUTHENTICATE PAMAuthenticate
#else
# define VALIDATE_USER PAMValidateUserInProcess
# define AUTHENTICATE PAMAuthenticateInProcess
#endif

int main(int argc, char** argv)
{
    char prompt[EXECUTOR_BUFFER_SIZE];
    const char* user;

    /* Check arguments */

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s username\n", argv[0]);
        exit(1);
    }

    /* Test PAMValidateUserInProcess(). */

    user = argv[1];

#if 0
    if (PAMValidateUserInProcess(user) != 0)
#else
    if (PAMValidateUser(user) != 0)
#endif
    {
        fprintf(stderr, "%s: invalid user: %s\n", argv[0], user);
        exit(1);
    }

    /* Test PAMAuthenticateInProcess(). */
    {
        const char* pw;
        sprintf(prompt, "Enter password for %s: ", user);
        pw = getpass(prompt);

        if (PAMAuthenticateInProcess(user, pw) == 0)
            printf("Correct password\n");
        else
            printf("Wrong password\n");

        putchar('\n');
    }

    return 0;
}
