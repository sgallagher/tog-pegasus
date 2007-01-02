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
#include <cstdlib>
#include <cstdlib>
#include <cstring>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
#  include <windows.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <unistd.h>
# include <sys/time.h>
# include <sys/resource.h>
#endif

#include "Constants.h"
#include "Executor.h"
#include "Mutex.h"
#include "FileSystem.h"
#include "String.h"
#include <Pegasus/Security/Cimservera/Strlcpy.h>
#include <Pegasus/Security/Cimservera/Strlcat.h>

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
# include <Executor/Executor.h>
#endif

#if defined(PEGASUS_PAM_AUTHENTICATION)
# include <Pegasus/Security/Cimservera/cimservera.h>
#endif

PEGASUS_NAMESPACE_BEGIN

static int _sock = -1;
static Mutex _mutex;

static int _getSock()
{
    int sock;
    _mutex.lock();
    sock = _sock;
    _mutex.unlock();
    return sock;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////
//// InProcess stubs:
////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int InProcess_ping()
{
    // Nothing to do.
    return 0;
}

FILE* InProcess_openFile(
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

static int InProcess_renameFile(
    const char* oldPath,
    const char* newPath)
{
    return FileSystem::renameFile(oldPath, newPath) ? 0 : -1;
}

static int InProcess_removeFile(
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

static int InProcess_startProviderAgent(
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

static int InProcess_startProviderAgent(
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

static int InProcess_startProviderAgent(
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

static int InProcess_daemonizeExecutor()
{
    // Nothing to do.
    return 0;
}

static int InProcess_changeOwner(
    const char* path,
    const char* owner)
{
    return FileSystem::changeFileOwner(path, owner) ? 0 : -1;
}

static int InProcess_waitPid(
    int pid)
{
    int status;

    while ((status = waitpid(pid, 0, 0)) == -1 && errno == EINTR)
        ;

    return status;
}

static int InProcess_pamAuthenticate(
    const char* username,
    const char* password)
{
#if defined(PEGASUS_PAM_AUTHENTICATION)
    return PAMAuthenticate(username, password);
#else
    return -1;
#endif
}

static int InProcess_pamValidateUser(
    const char* username)
{
#if defined(PEGASUS_PAM_AUTHENTICATION)
    return PAMValidateUser(username);
#else
    return -1;
#endif
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////
//// Out-of-process stubs.
////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)

//==============================================================================
//
// _recv()
//
//     Receives *size* bytes from the given socket.
//
//==============================================================================

static ssize_t _recv(int sock, void* buffer, size_t size)
{
    size_t r = size;
    char* p = (char*)buffer;

    if (size == 0)
        return -1;

    while (r)
    {
        ssize_t n;

        CIMSERVERA_RESTART(read(sock, p, r), n);

        if (n == -1)
            return -1;
        else if (n == 0)
            return size - r;

        r -= n;
        p += n;
    }

    return size - r;
}

//==============================================================================
//
// _send()
//
//     Sends *size* bytes on the given socket.
//
//==============================================================================

static ssize_t _send(int sock, void* buffer, size_t size)
{
    size_t r = size;
    char* p = (char*)buffer;

    while (r)
    {
        ssize_t n;
        CIMSERVERA_RESTART(write(sock, p, r), n);

        if (n == -1)
            return -1;
        else if (n == 0)
            return size - r;

        r -= n;
        p += n;
    }

    return size - r;
}

static int _receiveDescriptorArray(int sock, int descriptors[], size_t count)
{
    // This control data begins with a cmsghdr struct followed by the data
    // (a descriptor in this case). The union ensures that the data is aligned 
    // suitably for the leading cmsghdr struct. The descriptor itself is
    // properly aligned since the cmsghdr ends on a boundary that is suitably 
    // aligned for any type (including int).
    //
    //     ControlData = [ cmsghdr | int ]

    size_t size = CMSG_SPACE(sizeof(int) * count);
    char* data = (char*)malloc(size);

    // Define a msghdr that refers to the control data, which is filled in
    // by calling recvmsg() below.

    msghdr mh;
    memset(&mh, 0, sizeof(mh));
    mh.msg_control = data;
    mh.msg_controllen = size;

    // The other process sends a single-byte message. This byte is not
    // used since we only need the control data (the descriptor) but we
    // must request at least one byte from recvmsg().

    struct iovec iov[1];
    memset(iov, 0, sizeof(iov));

    char dummy;
    iov[0].iov_base = &dummy;
    iov[0].iov_len = 1;
    mh.msg_iov = iov;
    mh.msg_iovlen = 1;

    // Receive the message from the other process.

    ssize_t n = recvmsg(sock, &mh, 0);

    if (n <= 0)
        return -1;

    // Get a pointer to control message. Return if the header is null or does
    // not contain what we expect.

    cmsghdr* cmh = CMSG_FIRSTHDR(&mh);

    if (!cmh || 
        cmh->cmsg_len != CMSG_LEN(sizeof(int) * count) ||
        cmh->cmsg_level != SOL_SOCKET ||
        cmh->cmsg_type != SCM_RIGHTS)
    {
        return -1;
    }

    // Copy the data:

    memcpy(descriptors, CMSG_DATA(cmh), sizeof(int) * count);

    return 0;
}

static int OutOfProcess_ping()
{
    AutoMutex autoMutex(_mutex);

    // _send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_PING_REQUEST;

    if (_send(_getSock(), &header, sizeof(header)) != sizeof(header))
        return -1;

    ExecutorPingResponse response;

    if (_recv(_getSock(), &response, sizeof(response)) != sizeof(response))
        return -1;

    if (response.magic == EXECUTOR_PING_MAGIC)
        return 0;

    return -1;
}

FILE* OutOfProcess_openFile(
    const char* path,
    int mode)
{
    AutoMutex autoMutex(_mutex);

    if (mode != 'r' && mode != 'w')
        return NULL;

    // _send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_OPEN_FILE_REQUEST;

    if (_send(_getSock(), &header, sizeof(header)) != sizeof(header))
        return NULL;

    // _send request body.

    ExecutorOpenFileRequest request;
    memset(&request, 0, sizeof(request));
    Strlcpy(request.path, path, EXECUTOR_BUFFER_SIZE);
    request.mode = mode;

    if (_send(_getSock(), &request, sizeof(request)) != sizeof(request))
        return NULL;

    // Receive the response

    ExecutorOpenFileResponse response;

    if (_recv(_getSock(), &response, sizeof(response)) != sizeof(response))
        return NULL;

    // Receive descriptor (if response successful).

    if (response.status == 0)
    {
        int fds[1];

        if (_receiveDescriptorArray(_getSock(), fds, 1) != 0)
            return NULL;

        if (fds[0] == -1)
            return NULL;
        else
        {
            if (mode == 'r')
                return fdopen(fds[0], "rb");
            else
                return fdopen(fds[0], "wb");
        }
    }

    return NULL;
}

static int OutOfProcess_renameFile(
    const char* oldPath,
    const char* newPath)
{
    AutoMutex autoMutex(_mutex);

    // _send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_RENAME_FILE_REQUEST;

    if (_send(_getSock(), &header, sizeof(header)) != sizeof(header))
        return -1;

    // _send request body.

    ExecutorRenameFileRequest request;
    memset(&request, 0, sizeof(request));
    Strlcpy(request.oldPath, oldPath, EXECUTOR_BUFFER_SIZE);
    Strlcpy(request.newPath, newPath, EXECUTOR_BUFFER_SIZE);

    if (_send(_getSock(), &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorRenameFileResponse response;

    if (_recv(_getSock(), &response, sizeof(response)) != sizeof(response))
        return -1;

    return response.status;
}

static int OutOfProcess_removeFile(
    const char* path)
{
    AutoMutex autoMutex(_mutex);

    // _send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_REMOVE_FILE_REQUEST;

    if (_send(_getSock(), &header, sizeof(header)) != sizeof(header))
        return -1;

    // _send request body.

    ExecutorRemoveFileRequest request;
    memset(&request, 0, sizeof(request));
    Strlcpy(request.path, path, EXECUTOR_BUFFER_SIZE);

    if (_send(_getSock(), &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorRemoveFileResponse response;

    if (_recv(_getSock(), &response, sizeof(response)) != sizeof(response))
        return -1;

    return response.status;
}

static int OutOfProcess_startProviderAgent(
    const char* module, 
    int uid,
    int gid, 
    int& pid,
    AnonymousPipe*& readPipe,
    AnonymousPipe*& writePipe)
{
    AutoMutex autoMutex(_mutex);

    readPipe = 0;
    writePipe = 0;

    // Reject strings longer than EXECUTOR_BUFFER_SIZE.

    size_t n = strlen(module);

    if (n >= EXECUTOR_BUFFER_SIZE)
        return -1;

    // _send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_START_PROVIDER_AGENT_REQUEST;

    if (_send(_getSock(), &header, sizeof(header)) != sizeof(header))
        return -1;

    // _send request body.

    ExecutorStartProviderAgentRequest request;
    memset(&request, 0, sizeof(request));
    memcpy(request.module, module, n);
    request.uid = uid;
    request.gid = gid;

    if (_send(_getSock(), &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorStartProviderAgentResponse response;

    if (_recv(_getSock(), &response, sizeof(response)) != sizeof(response))
        return -1;

    // Check response status and pid.

    if (response.status != 0)
        return -1;

    // Get pid:

    pid = response.pid;

    // Receive descriptors.

    int descriptors[2];
    int result = _receiveDescriptorArray(_getSock(), descriptors, 2);

    if (result == 0)
    {
        int readFd = descriptors[0];
        int writeFd = descriptors[1];

        // Create to and from AnonymousPipe instances to correspond to the pipe
        // descriptors created above.

        char readFdStr[32];
        char writeFdStr[32];
        sprintf(readFdStr, "%d", readFd);
        sprintf(writeFdStr, "%d", writeFd);

        readPipe = new AnonymousPipe(readFdStr, 0);
        writePipe = new AnonymousPipe(0, writeFdStr);
    }

    return result;
}

static int OutOfProcess_daemonizeExecutor()
{
    AutoMutex autoMutex(_mutex);

    // _send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_DAEMONIZE_EXECUTOR_REQUEST;

    if (_send(_getSock(), &header, sizeof(header)) != sizeof(header))
        return -1;

    // Receive the response

    ExecutorDaemonizeExecutorResponse response;

    if (_recv(_getSock(), &response, sizeof(response)) != sizeof(response))
        return -1;

    return response.status;
}

static int OutOfProcess_changeOwner(
    const char* path,
    const char* owner)
{
    AutoMutex autoMutex(_mutex);

    // _send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_CHANGE_OWNER_REQUEST;

    if (_send(_getSock(), &header, sizeof(header)) != sizeof(header))
        return -1;

    // _send request body:

    ExecutorChangeOwnerRequest request;
    Strlcpy(request.path, path, sizeof(request.path));
    Strlcpy(request.owner, owner, sizeof(request.owner));

    if (_send(_getSock(), &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorChangeOwnerResponse response;

    if (_recv(_getSock(), &response, sizeof(response)) != sizeof(response))
        return -1;

    return response.status;
}

static int OutOfProcess_waitPid(
    int pid)
{
    AutoMutex autoMutex(_mutex);

    // _send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_WAIT_PID_REQUEST;

    if (_send(_getSock(), &header, sizeof(header)) != sizeof(header))
        return -1;

    // _send request body:

    ExecutorWaitPidRequest request;
    request.pid = pid;

    if (_send(_getSock(), &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorWaitPidResponse response;

    if (_recv(_getSock(), &response, sizeof(response)) != sizeof(response))
        return -1;

    return response.status;
}

static int OutOfProcess_pamAuthenticate(
    const char* username,
    const char* password)
{
    AutoMutex autoMutex(_mutex);

    // _send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_PAM_AUTHENTICATE_REQUEST;

    if (_send(_getSock(), &header, sizeof(header)) != sizeof(header))
        return -1;

    // _send request body.

    ExecutorPAMAuthenticateRequest request;
    memset(&request, 0, sizeof(request));
    Strlcpy(request.username, username, EXECUTOR_BUFFER_SIZE);
    Strlcpy(request.password, password, EXECUTOR_BUFFER_SIZE);

    if (_send(_getSock(), &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorPAMAuthenticateResponse response;

    if (_recv(_getSock(), &response, sizeof(response)) != sizeof(response))
        return -1;

    return response.status;
}

static int OutOfProcess_pamValidateUser(
    const char* username)
{
    AutoMutex autoMutex(_mutex);

    // _send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_PAM_VALIDATE_USER_REQUEST;

    if (_send(_getSock(), &header, sizeof(header)) != sizeof(header))
        return -1;

    // _send request body.

    ExecutorPAMValidateUserRequest request;
    memset(&request, 0, sizeof(request));
    Strlcpy(request.username, username, EXECUTOR_BUFFER_SIZE);

    if (_send(_getSock(), &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorPAMValidateUserResponse response;

    if (_recv(_getSock(), &response, sizeof(response)) != sizeof(response))
        return -1;

    return response.status;
}

#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////
//// Executor Methods:
////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Executor::setSock(int sock)
{
    _mutex.lock();
    _sock = sock;
    _mutex.unlock();
}

int Executor::ping()
{
    if (_getSock() == -1)
        return InProcess_ping();

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
    return OutOfProcess_ping();
#else
    return -1;
#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */
}

FILE* Executor::openFile(
    const char* path,
    int mode)
{
    if (_getSock() == -1)
        return InProcess_openFile(path, mode);

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
    return OutOfProcess_openFile(path, mode);
#else
    return -1;
#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */
}

int Executor::renameFile(
    const char* oldPath,
    const char* newPath)
{
    if (_getSock() == -1)
        return InProcess_renameFile(oldPath, newPath);

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
    return OutOfProcess_renameFile(oldPath, newPath);
#else
    return -1;
#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */
}

int Executor::removeFile(
    const char* path)
{
    if (_getSock() == -1)
        return InProcess_removeFile(path);

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
    return OutOfProcess_removeFile(path);
#else
    return -1;
#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */
}

int Executor::startProviderAgent(
    const char* module, 
    int uid,
    int gid, 
    int& pid,
    AnonymousPipe*& readPipe,
    AnonymousPipe*& writePipe)
{
    if (_getSock() == -1)
        return InProcess_startProviderAgent(
            module, uid, gid, pid, readPipe, writePipe);

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
    return OutOfProcess_startProviderAgent(
        module, uid, gid, pid, readPipe, writePipe);
#else
    return -1;
#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */
}

int Executor::daemonizeExecutor()
{
    if (_getSock() == -1)
        return InProcess_daemonizeExecutor();

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
    return OutOfProcess_daemonizeExecutor();
#else
    return -1;
#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */
}

int Executor::changeOwner(
    const char* path,
    const char* owner)
{
    if (_getSock() == -1)
        return InProcess_changeOwner(path, owner);

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
    return OutOfProcess_changeOwner(path, owner);
#else
    return -1;
#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */
}

int Executor::waitPid(
    int pid)
{
    if (_getSock() == -1)
        return InProcess_waitPid(pid);

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
    return OutOfProcess_waitPid(pid);
#else
    return -1;
#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */
}

int Executor::pamAuthenticate(
    const char* username,
    const char* password)
{
    if (_getSock() == -1)
        return InProcess_pamAuthenticate(username, password);

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
    return OutOfProcess_pamAuthenticate(username, password);
#else
    return -1;
#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */
}

int Executor::pamValidateUser(
    const char* username)
{
    if (_getSock() == -1)
        return InProcess_pamValidateUser(username);

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
    return OutOfProcess_pamValidateUser(username);
#else
    return -1;
#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */
}

PEGASUS_NAMESPACE_END
