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

//==============================================================================
//
// Child
//
//     The child process.
//
//==============================================================================

void Child(
    int argc, 
    char** argv, 
    char path[EXECUTOR_BUFFER_SIZE],
    int uid,
    int gid,
    int sock)
{
    // Build argument list, adding "-x <sock>" option if sock non-negative.

    char** execArgv = (char**)malloc(sizeof(char*) * (argc + 4));
    memcpy(execArgv + 4, argv + 1, sizeof(char*) * argc);

    char sockStr[EXECUTOR_BUFFER_SIZE];
    sprintf(sockStr, "%d", sock);

    execArgv[0] = CIMSERVERMAIN;
    execArgv[1] = EXECUTOR_FINGERPRINT;
    execArgv[2] = "-x";
    execArgv[3] = sockStr;

    // Locate repository directory.

    char repositoryDir[EXECUTOR_BUFFER_SIZE];

    if (LocateRepositoryDirectory(argc, argv, repositoryDir) != 0)
        Fatal(FL, "failed to locate repository directory");

    // Check whether repository directory exists.

    if (AccessDir(repositoryDir) != 0)
        Fatal(FL, 
            "failed to access repository directory: %s", repositoryDir);

    // Change ownership of Pegasus repository directory (it should be owned
    // by same user that owns CIMSERVERMAIN).

    ChangeDirOwnerRecursive(repositoryDir, uid, gid);

    Log(LL_TRACE, "Pegasus repositoryDir is \"%s\"", repositoryDir);

    // Downgrade privileges by setting the UID and GID of this process. Use
    // the owner of the CIMSERVERMAIN program obtained above.

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

    // Log user info.

    char username[EXECUTOR_BUFFER_SIZE];

    if (GetUserName(uid, username) != 0)
        Fatal(FL, "cannot resolve user from uid=%d", uid);

    Log(LL_TRACE, "%s running as %s (uid=%d, gid=%d)", CIMSERVERMAIN, 
        username, uid, gid);

    // Precheck that cimxml.socket is owned by cimservermain process. If 
    // not, then the bind would fail in the cimservermain process much 
    // later and the cause of the error would be difficult to determine.

    /* Flawfinder: ignore */
    if (access(PEGASUS_LOCAL_DOMAIN_SOCKET_PATH, F_OK) == 0)
    {
        struct stat st;

        if (stat(PEGASUS_LOCAL_DOMAIN_SOCKET_PATH, &st) != 0 ||
            (int)st.st_uid != uid || 
            (int)st.st_gid != gid)
        {
            Fatal(FL, 
                "cimservermain process cannot stat or does not own %s",
                PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);
        }
    }

    // Exec child process.

    /* Flawfinder: ignore */
    if (execv(path, execArgv) != 0)
        Fatal(FL, "failed to exec %s", path);

    exit(0);
}
