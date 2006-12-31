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

#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Constants.h>
#include "private/ExecutorClientLoopbackImpl.h"

#if defined(PEGASUS_PAM_AUTHENTICATION)
#include <Pegasus/Security/Cimservera/cimservera.h>
#endif

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include <windows.h>
#else
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <sys/time.h>
# include <sys/resource.h>
# include <unistd.h>
#endif

PEGASUS_NAMESPACE_BEGIN

ExecutorClientLoopbackImpl::ExecutorClientLoopbackImpl()
{
}

ExecutorClientLoopbackImpl::~ExecutorClientLoopbackImpl()
{
}

int ExecutorClientLoopbackImpl::ping()
{
    // Nothing to do.
    return 0;
}

FILE* ExecutorClientLoopbackImpl::openFile(
    const char* path,
    int mode)
{
    switch (mode)
    {
        case 'r':
            return fopen(path, "rb");

        case 'w':
            return fopen(path, "wb");

        default:
            return NULL;
    }
}

int ExecutorClientLoopbackImpl::renameFile(
    const char* oldPath,
    const char* newPath)
{
    return FileSystem::renameFile(oldPath, newPath) ? 0 : -1;
}

int ExecutorClientLoopbackImpl::removeFile(
    const char* path)
{
    return FileSystem::removeFile(path) ? 0 : -1;
}

static int _getProviderAgentPath(String& path)
{
    // ATTN: is this really a sufficient replacement for getHomedPath().
    // Does getHomedPath() use the configuration file?

    path = PEGASUS_PROVIDER_AGENT_PROC_NAME;

    if (path[0] != '/')
    {
        const char* env = getenv("PEGASUS_HOME");

        if (!env)
            return -1;

        path = String(env) + String("/") + path;
    }

    return 0;
}

#if defined(PEGASUS_OS_TYPE_WINDOWS)

int ExecutorClientLoopbackImpl::startProviderAgent(
    const char* module, 
    int uid,
    int gid, 
    int& pid,
    AnonymousPipe*& readPipe,
    AnonymousPipe*& writePipe)
{
    AutoMutex autoMutex(_mutex);

    // Set output parameters in case of failure.

    pid = 0;
    readPipe = 0;
    writePipe = 0;

    // Create pipes. Export handles to string.

    AnonymousPipe* pipeFromAgent = new AnonymousPipe();
    AnonymousPipe* pipeToAgent = new AnonymousPipe();

    char readHandle[32];
    char writeHandle[32];
    pipeToAgent->exportReadHandle(readHandle);
    pipeFromAgent->exportWriteHandle(writeHandle);

    // Initialize PROCESS_INFORMATION.

    PROCESS_INFORMATION piProcInfo;
    ZeroMemory(&piProcInfo, sizeof (PROCESS_INFORMATION));

    // Initialize STARTUPINFO.

    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof (STARTUPINFO));
    siStartInfo.cb = sizeof (STARTUPINFO);

    // Build full path of "cimprovagt" program.

    String path;

    if (_getProviderAgentPath(path) != 0)
    {
        delete pipeToAgent;
        delete pipeFromAgent;
        return -1;
    }

    // Format command line.

    char cmdLine[2048];

    sprintf(cmdLine, "\"%s\" %s %s \"%s\"",
        (const char*)path.getCString(),
        readHandle, 
        writeHandle, 
        module);

    //  Create provider agent proess.

    if (!CreateProcess (
        NULL,          //
        cmdLine,       //  command line
        NULL,          //  process security attributes
        NULL,          //  primary thread security attributes
        TRUE,          //  handles are inherited
        0,             //  creation flags
        NULL,          //  use parent's environment
        NULL,          //  use parent's current directory
        &siStartInfo,  //  STARTUPINFO
        &piProcInfo))  //  PROCESS_INFORMATION
    {
        delete pipeToAgent;
        delete pipeFromAgent;
        return -1;
    }

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    // Close our copies of the agent's ends of the pipes

    pipeToAgent->closeReadHandle();
    pipeFromAgent->closeWriteHandle();

    readPipe = pipeFromAgent;
    writePipe = pipeToAgent;

    return 0;
}

#elif defined(PEGASUS_OS_OS400)

int ExecutorClientLoopbackImpl::startProviderAgent(
    const char* module, 
    int uid,
    int gid, 
    int& pid,
    AnonymousPipe*& readPipe,
    AnonymousPipe*& writePipe)
{
    // ATTN: no implementation for OS400.
    return -1;
}

#else /* POSIX CASE FOLLOWS */

int ExecutorClientLoopbackImpl::startProviderAgent(
    const char* module, 
    int uid,
    int gid, 
    int& pid,
    AnonymousPipe*& readPipe,
    AnonymousPipe*& writePipe)
{
    AutoMutex autoMutex(_mutex);

    // Initialize output parameters in case of error.

    pid = -1;
    readPipe = 0;
    writePipe = 0;

    // Pipes:

    int to[2];
    int from[2];

    do
    {
        // Resolve full path of "cimprovagt".

        String path;

        if (_getProviderAgentPath(path) != 0)
            return -1;

        // Create "to-agent" pipe:

        if (pipe(to) != 0)
            return -1;

        // Create "from-agent" pipe:

        if (pipe(from) != 0)
            return -1;

        // Fork process:

#if !defined(PEGASUS_OS_VMS)
        pid = (int)vfork();
#else
        pid = (int)fork();
#endif

        if (pid < 0)
            return -1;

        // If child proceses.

        if (pid == 0)
        {
            // Close unused pipe descriptors:

            close(to[1]);
            close(from[0]);

#if !defined(PEGASUS_OS_VMS)

            // Close unused descriptors. Leave stdin, stdout, stderr, and the
            // child's pipe descriptors open.

            struct rlimit rlim;

            if (getrlimit(RLIMIT_NOFILE, &rlim) == 0)
            {
                for (int i = 3; i < int(rlim.rlim_cur); i++)
                {
                    if (i != to[0] && i != from[1])
                        close(i);
                }
            }

#endif /* !defined(PEGASUS_OS_VMS) */

            // Set uid and gid for the new provider agent process.

# if !defined(PEGASUS_DISABLE_PROV_USERCTXT)

            if (uid != -1 && gid != -1)
            {
                if ((int)getgid() != gid)
                {
                    // ATTN: log failure!
                    setgid(gid);
                }

                if ((int)getuid() != uid)
                {
                    // ATTN: log failure!
                    setuid(uid);
                }
            }

# endif /* !defined(PEGASUS_DISABLE_PROV_USERCTXT) */

            // Exec the cimprovagt program.

            char arg1[32];
            char arg2[32];
            sprintf(arg1, "%d", to[0]);
            sprintf(arg2, "%d", from[1]);

            {
                CString cstr = path.getCString();
                execl(cstr, cstr, arg1, arg2, module, (char*)0);
                _exit(1);
            }

            // ATTN: log failure!
        }
    }
    while (0);

    // Close unused pipe descriptors.

    close(to[0]);
    close(from[1]);

    // Set output parameters.

    int readFd = from[0];
    int writeFd = to[1];

    // Create to and from AnonymousPipe instances to correspond to the pipe
    // descriptors created above.

    char readFdStr[32];
    char writeFdStr[32];
    sprintf(readFdStr, "%d", readFd);
    sprintf(writeFdStr, "%d", writeFd);

    readPipe = new AnonymousPipe(readFdStr, 0);
    writePipe = new AnonymousPipe(0, writeFdStr);

    return 0;
}

#endif /* !defined(START_PROVIDER_AGENT) */

int ExecutorClientLoopbackImpl::daemonizeExecutor()
{
    // Nothing to do.
    return 0;
}

int ExecutorClientLoopbackImpl::changeOwner(
    const char* path,
    const char* owner)
{
    return FileSystem::changeFileOwner(path, owner) ? 0 : -1;
}

int ExecutorClientLoopbackImpl::waitPid(
    int pid)
{
    int status;

    while ((status = waitpid(pid, 0, 0)) == -1 && errno == EINTR)
        ;

    return status;
}

int ExecutorClientLoopbackImpl::pamAuthenticate(
    const char* username,
    const char* password)
{
#if defined(PEGASUS_PAM_AUTHENTICATION)
    return PAMAuthenticate(username, password);
#else
    return -1;
#endif
}

int ExecutorClientLoopbackImpl::pamValidateUser(
    const char* username)
{
#if defined(PEGASUS_PAM_AUTHENTICATION)
    return PAMValidateUser(username);
#else
    return -1;
#endif
}

PEGASUS_NAMESPACE_END
