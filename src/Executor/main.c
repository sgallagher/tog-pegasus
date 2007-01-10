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
#include "Strlcpy.h"
#include "Log.h"
#include "Policy.h"

/*
**==============================================================================
**
** GetServerUser
**
**     Determine which user to run cimservermain as.
**
**     Note: this algorithm is no longer in use.
**
**==============================================================================
*/

int GetServerUser(int* uid, int* gid)
{
    const char* username = PEGASUS_CIMSERVERMAIN_USER;

    if (GetUserInfo(username, uid, gid) != 0)
    {
        Fatal(FL, 
            "The cimservermain user does not exist: \"%s\". Please create "
            "a system user with that name or use an OpenPegasus build that "
            "disables privilege separation.", username);
    }

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

void ExecShutdown(int argc, char** argv)
{
    char* tmpArgv[3];
    char cimshutdownPath[EXECUTOR_BUFFER_SIZE];
    char shutdownTimeout[EXECUTOR_BUFFER_SIZE];

    /* Get shutdownTimeout configuration parameter. */

    if (GetConfigParam(argc, argv, "shutdownTimeout", shutdownTimeout) != 0)
        Strlcpy(shutdownTimeout, "5", sizeof(shutdownTimeout));

    /* Get absolute cimshutdown program name. */

    if (GetInternalPegasusProgramPath(CIMSHUTDOWN, cimshutdownPath) != 0)
        Fatal(FL, "Failed to locate Pegasus program: %s", CIMSHUTDOWN);

    /* Create argument list. */

    tmpArgv[0] = CIMSHUTDOWN;
    tmpArgv[1] = shutdownTimeout;
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
    long shutdownTimeout;

    /* Save as global so it can be used in error and log messages. */

    globals.arg0 = argv[0];

    /* If shuting down, then run "cimshutdown" client. */

    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-s") == 0)
            ExecShutdown(argc, argv);
    }

    /* Get absolute cimservermain program name. */

    if (GetInternalPegasusProgramPath(CIMSERVERMAIN, cimservermainPath) != 0)
        Fatal(FL, "Failed to locate Pegasus program: %s", CIMSERVERMAIN);

    /* If CIMSERVERMAIN is already running, warn and exit now. */

    if (TestCimServerProcess() == 0)
    {
        fprintf(stderr,
            "%s: cimserver is already running (the PID found in the file "
            "\"%s\" corresponds to an existing process named \"%s\").\n\n",
            globals.arg0, PEGASUS_CIMSERVER_START_FILE, CIMSERVERMAIN);

        exit(1);
    }

    /* Get enableAuthentication configuration option. */

    {
        char buffer[EXECUTOR_BUFFER_SIZE];

        if (GetConfigParam(argc, argv, "enableAuthentication", buffer) == 0 &&
            strcasecmp(buffer, "true") == 0)
        {
            globals.enableAuthentication = 1;
        }
    }

    /* Locate password file. */

    if (LocatePasswordFile(argc, argv, globals.passwordFilePath) != 0)
        Fatal(FL, "Failed to locate password file");

    /* Locate key file. */

    if (LocateKeyFile(argc, argv, globals.sslKeyFilePath) != 0)
        Fatal(FL, "Failed to locate key file");

    /* Locate the path of directory to contain trace files. */

    if (LocateTraceFilePath(argc, argv, globals.traceFilePath) != 0)
        Fatal(FL, "Failed to locate trace file path");

    /* Locate the sslTrustStore. */

    if (LocateSslTrustStore(argc, argv, globals.sslTrustStore) != 0)
        Fatal(FL, "Failed to locate SSL trust store");

    /* Locate the crlStore . */

    if (LocateCrlStore(argc, argv, globals.crlStore) != 0)
        Fatal(FL, "Failed to locate crl store");

    /* Define macros needed by policy facility. */

    DefinePolicyMacros();

    /* Create a socket pair for communicating with the child process. */

    if (CreateSocketPair(pair) != 0)
        Fatal(FL, "Failed to create socket pair");

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
        fprintf(stderr, "%s: this program must be run as root\n", globals.arg0);
        exit(0);
    }

    /* Warn if authentication not enabled (strange use of executor if not). */

    if (!globals.enableAuthentication)
        Log(LL_WARNING, "authentication is NOT enabled");
    else
        Log(LL_TRACE, "authentication is enabled");

    /* Print user info. */

    if (GetUserName(getuid(), username) != 0)
        Fatal(FL, "cannot resolve user from uid=%d", getuid());

    Log(LL_TRACE, "running as %s (uid=%d, gid=%d)",
        username, (int)getuid(), (int)getgid());

    /* Determine user for running cimservermain. */

    GetServerUser(&globals.childUid, &globals.childGid);

    /* Fork child process. */

    childPid = fork();

    if (childPid == 0)
    {
        /* Child. */
        close(pair[1]);
        Child(argc, argv, 
            cimservermainPath, globals.childUid, globals.childGid, pair[0]);
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
