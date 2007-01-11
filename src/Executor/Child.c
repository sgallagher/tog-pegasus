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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Defines.h"
#include "Fatal.h"
#include "Path.h"
#include "File.h"
#include "Log.h"
#include "User.h"

/*
**==============================================================================
**
** Child
**
**     The child process.
**
**==============================================================================
*/

void Child(
    int argc, 
    char** argv, 
    const char* path,
    int uid,
    int gid,
    int sock,
    const char* repositoryDir)
{
    char sockStr[EXECUTOR_BUFFER_SIZE];
    char username[EXECUTOR_BUFFER_SIZE];
    char** execArgv;

    /* Build argument list, adding "-x <sock>" option if sock non-negative. */

    execArgv = (char**)malloc(sizeof(char*) * (argc + 3));
    memcpy(execArgv + 3, argv + 1, sizeof(char*) * argc);

    sprintf(sockStr, "%d", sock);

    execArgv[0] = CIMSERVERMAIN;
    execArgv[1] = "-x";
    execArgv[2] = strdup(sockStr);

    /* Check whether repository directory exists. */

    if (AccessDir(repositoryDir) != 0)
        Fatal(FL, 
            "failed to access repository directory: %s", repositoryDir);

    /* 
     * Change ownership of Pegasus repository directory (it should be owned
     * by same user that owns CIMSERVERMAIN).
     */

    ChangeDirOwnerRecursive(repositoryDir, uid, gid);

    Log(LL_TRACE, "Pegasus repositoryDir is \"%s\"", repositoryDir);

    /*
     * Downgrade privileges by setting the UID and GID of this process. Use
     * the owner of the CIMSERVERMAIN program obtained above.
     */

    if (uid == 0 || gid == 0)
    {
        Fatal(FL, "root may not own %s since the program is run as owner",
            path);
    }

    if (setgid(gid) != 0)
    {
        Fatal(FL, "Failed to set gid to %d", gid);
    }

    if (setuid(uid) != 0)
    {
        Fatal(FL, "Failed to set uid to %d", uid);
    }

    if ((int)getuid() != uid || 
        (int)geteuid() != uid || 
        (int)getgid() != gid || 
        (int)getegid() != gid)
    {
        Fatal(FL, "setuid/setgid verification failed\n");
    }

    /* Log user info. */

    if (GetUserName(uid, username) != 0)
        Fatal(FL, "cannot resolve user from uid=%d", uid);

    Log(LL_TRACE, "%s running as %s (uid=%d, gid=%d)", CIMSERVERMAIN, 
        username, uid, gid);

    /*
     * Precheck that cimxml.socket is owned by CIMSERVERMAIN process. If 
     * not, then the bind would fail in the CIMSERVERMAIN process much 
     * later and the cause of the error would be difficult to determine.
     */

    /* Flawfinder: ignore */
    if (access(PEGASUS_LOCAL_DOMAIN_SOCKET_PATH, F_OK) == 0)
    {
        struct stat st;

        if (stat(PEGASUS_LOCAL_DOMAIN_SOCKET_PATH, &st) != 0 ||
            (int)st.st_uid != uid || 
            (int)st.st_gid != gid)
        {
            Fatal(FL, "%s process cannot stat or does not own %s",
                CIMSERVERMAIN, PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);
        }
    }

    /* Exec child process. */

    /* Flawfinder: ignore */
    if (execv(path, execArgv) != 0)
        Fatal(FL, "failed to exec %s", path);

    exit(0);
}
