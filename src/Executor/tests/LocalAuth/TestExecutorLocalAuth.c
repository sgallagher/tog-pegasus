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

#include <Executor/User.h>
#include <Executor/LocalAuth.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

void testSuccessfulAuthentication(void)
{
    char challengeFilePath[EXECUTOR_BUFFER_SIZE];
    char response[EXECUTOR_BUFFER_SIZE];
    FILE* is;

    /* Start authentication and get challenge file path. */

    assert(StartLocalAuthentication(
        PEGASUS_CIMSERVERMAIN_USER, challengeFilePath) == 0);

    /* Read secret token from file. */

    is = fopen(challengeFilePath, "r");
    assert(is != NULL);
    assert(fgets(response, sizeof(response), is) != NULL);

    /* Finish authentication. */

    assert(FinishLocalAuthentication(challengeFilePath, response) == 0);
}

void testCreateLocalAuthFile(void)
{
    int testUid;
    int testGid;
    assert(GetUserInfo(PEGASUS_CIMSERVERMAIN_USER, &testUid, &testGid) == 0);

    /* Test with file path that already exists */
    {
        const char* path = "testlocalauthfile";
        int fd = open(path, O_WRONLY | O_EXCL | O_CREAT | O_TRUNC, S_IRUSR);
        write(fd, "test", 4);
        close(fd);
        assert(CreateLocalAuthFile(path, testUid, testGid) == 0);
        unlink(path);
    }

    /* Test with non-existent directory in file path */
    {
        const char* path =
            "/tmp/nonexistentdirectory/anotherone/pegasus/localauthtestfile";
        assert(CreateLocalAuthFile(path, testUid, testGid) != 0);
    }
}

void testCheckLocalAuthToken(void)
{
    /* Test with file path that does not exist */
    {
        const char* path = "nonexistenttestfile";
        assert(CheckLocalAuthToken(path, "secret") != 0);
    }

    /* Test with secret token that is too short */
    {
        const char* path = "testlocalauthfile";
        int fd = open(path, O_WRONLY | O_EXCL | O_CREAT | O_TRUNC, S_IRUSR);
        write(fd, "secret", 6);
        close(fd);
        assert(CheckLocalAuthToken(path, "secret") != 0);
        unlink(path);
    }

    /* Test with incorrect secret token */
    {
        const char* path = "testlocalauthfile";
        int fd = open(path, O_WRONLY | O_EXCL | O_CREAT | O_TRUNC, S_IRUSR);
        write(fd, "1234567890123456789012345678901234567890", 40);
        close(fd);
        assert(CheckLocalAuthToken(
            path, "123456789012345678901234567890123456789X") != 0);
        unlink(path);
    }
}

void testStartLocalAuthentication(void)
{
    /* Test with non-existent user */
    {
        const char* invalidUserName = "xinvaliduserx";
        int uid;
        int gid;
        char challengeFilePath[EXECUTOR_BUFFER_SIZE];

        /* Only run this test if the user does not exist on the test system */
        if (GetUserInfo(invalidUserName, &uid, &gid) != 0)
        {
            assert(StartLocalAuthentication(
                invalidUserName, challengeFilePath) != 0);
        }
    }
}

void testFinishLocalAuthentication(void)
{
    /* Test with non-existent challenge file path */
    {
        const char* path = "nonexistenttestfile";
        assert(FinishLocalAuthentication(path, "secret") != 0);
    }
}

int main()
{
    testSuccessfulAuthentication();
    testCreateLocalAuthFile();
    testCheckLocalAuthToken();
    testStartLocalAuthentication();
    testFinishLocalAuthentication();

    printf("+++++ passed all tests\n");

    return 0;
}
