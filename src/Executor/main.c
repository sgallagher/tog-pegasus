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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "Config.h"
#include "Child.h"
#include "Parent.h"
#include "User.h"
#include "Fatal.h"
#include "Process.h"
#include "Path.h"
#include "Globals.h"
#include "Socket.h"
#include "Log.h"

/*
**==============================================================================
**
** GetServerUser
**
**     Determine which user to run cimservermain as.
**
**==============================================================================
*/

int GetServerUser(
    int argc,
    char** argv,
    char path[EXECUTOR_BUFFER_SIZE], 
    int* uid, 
    int* gid)
{
    struct stat st;
    const char DEFAULT_SERVER_USER[] = "pegasus";

    /* (1) Try to find serverUser as configuration parameter. */

    char user[EXECUTOR_BUFFER_SIZE];

    if (GetConfigParam(argc, argv, "serverUser", user) == 0)
    {
        if (GetUserInfo(user, uid, gid) == 0)
            return 0;

        Fatal(FL, "serverUser option specifies unknown user: %s", user);
        return -1;
    }

    /* (2) Use owner of the cimservermain program if not root. */

    if (stat(path, &st) != 0)
        Fatal(FL, "stat(%s) failed", path);

    if (st.st_uid != 0 && st.st_gid != 0)
    {
        *uid = st.st_uid;
        *gid = st.st_gid;
        return 0;
    }

    /* (3) Try the "pegasus" user (the default). */

    if (GetUserInfo(DEFAULT_SERVER_USER, uid, gid) == 0 && 
        *uid != 0 && 
        *gid != 0)
    {
        return 0;
    }

    Fatal(FL, 
        "cannot determine server user (used to run cimserermain). "
        "Please specify this value in one of four ways. (1) pass "
        "serverUser=<username> on the command line, (2) use cimconfig to "
        "set serverUser (using -p -s options), (3) make the desired "
        "user the owner of %s (i.e., use chown), or (4) create a system "
        "user whose user whose name is \"pegasus\".", path);

    return -1;
}

/*
**==============================================================================
**
** ReadPidFile()
**
**==============================================================================
*/

static int ReadPidFile(const char* path, int* pid)
{
    FILE* is = fopen(path, "r");

    if (!is) 
        return -1;

    *pid = 0;

    fscanf(is, "%d\n", pid);
    fclose(is);

    if (*pid == 0)
        return -1;

    return 0;
}

/*
**==============================================================================
**
** TestCimServerProcess()
**
**     Returns 0 if cimserver process is running.
**
**==============================================================================
*/

int TestCimServerProcess()
{
    int pid;
    char name[EXECUTOR_BUFFER_SIZE];

    if (ReadPidFile(PEGASUS_CIMSERVER_START_FILE, &pid) != 0)
        return -1;

    if (GetProcessName(pid, name) != 0 || strcmp(name, CIMSERVERMAIN) != 0)
        return -1;

    return 0;
}

/*
**==============================================================================
**
** ExecShutdown()
**
**==============================================================================
*/

void ExecShutdown()
{
    char* tmpArgv[3];

    /* Get absolute cimshutdown program name. */

    char cimshutdownPath[EXECUTOR_BUFFER_SIZE];

    if (GetInternalPegasusProgramPath(CIMSHUTDOWN, cimshutdownPath) != 0)
        Fatal(FL, "Failed to locate Pegasus program: %s", CIMSHUTDOWN);

    /* Create argument list. */

    tmpArgv[0] = CIMSHUTDOWN;
    tmpArgv[1] = EXECUTOR_FINGERPRINT;
    tmpArgv[2] = 0;

    /* Exec CIMSHUTDOWN program. */

    /* Flawfinder: ignore */
    execv(cimshutdownPath, tmpArgv);
    Fatal(FL, "failed to exec %s", cimshutdownPath);
}

/*
**==============================================================================
**
** main()
**
**==============================================================================
*/

void doit();

int main(int argc, char** argv)
{
    int i;
    char cimservermainPath[EXECUTOR_BUFFER_SIZE];
    int pair[2];
    char username[EXECUTOR_BUFFER_SIZE];
    int childPid;
    int perror;

    /* Save as global so it can be used in error and log messages. */

    globalArg0 = argv[0];

    /* Get absolute cimservermain program name. */

    if (GetInternalPegasusProgramPath(CIMSERVERMAIN, cimservermainPath) != 0)
        Fatal(FL, "Failed to locate Pegasus program: %s", CIMSERVERMAIN);

    /* If shuting down, then run "cimshutdown" client. */

    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-s") == 0)
            ExecShutdown();
    }

    /* If CIMSERVERMAIN is already running, warn and exit now. */


    if (TestCimServerProcess() == 0)
    {
        fprintf(stderr,
            "%s: cimserver is already running (the PID found in the file "
            "\"%s\" corresponds to an existing process named \"%s\").\n\n",
            globalArg0, PEGASUS_CIMSERVER_START_FILE, CIMSERVERMAIN);

        exit(1);
    }

    /* Create a socket pair for communicating with the child process. */

    if (CreateSocketPair(pair) != 0)
        Fatal(FL, "failed to create socket pair");

    CloseOnExec(pair[1]);

    /* Get the log-level from the configuration parameter. */

    GetLogLevel(argc, argv);

    /* Extract -p (perror) option. */

    perror = 0;

    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            perror = 1;
            memmove(&argv[i], &argv[i + 1], (argc-i) * sizeof(char*));
            argc--;
            break;
        }
    }

    /* Open the log. */

    OpenLog("cimexecutor", perror);

    Log(LL_INFORMATION, "starting");

    /* Be sure this process is running as root (otherwise fail). */

    if (setuid(0) != 0 || setgid(0) != 0)
    {
        Log(LL_FATAL, "attempted to run program as non-root user");
        fprintf(stderr, "%s: this program must be run as root\n", globalArg0);
        exit(0);
    }

    /* Print user info. */

    if (GetUserName(getuid(), username) != 0)
        Fatal(FL, "cannot resolve user from uid=%d", getuid());

    Log(LL_TRACE, "running as %s (uid=%d, gid=%d)",
        username, (int)getuid(), (int)getgid());

    /* Determine user for running cimservermain. */

    GetServerUser(
        argc, argv, cimservermainPath, &globalChildUid, &globalChildGid);

    /* Fork child process. */

    childPid = fork();

    if (childPid == 0)
    {
        /* Child. */
        close(pair[1]);
        Child(argc, argv, 
            cimservermainPath, globalChildUid, globalChildGid, pair[0]);
    }
    else if (childPid > 0)
    {
        /* Parent. */
        close(pair[0]);
        Parent(pair[1], childPid);
    }
    else
    {
        Fatal(FL, "fork() failed");
    }

    return 0;
}
