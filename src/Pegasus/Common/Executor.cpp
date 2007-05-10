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
#include <cstring>

#include <Pegasus/Common/Config.h>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include <windows.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/time.h>
# include <sys/resource.h>
#endif

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Executor.h>

#include <Executor/Strlcpy.h>

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
# include <Executor/Socket.h>
# include <Executor/Messages.h>
#endif

#if defined(PEGASUS_PAM_AUTHENTICATION)
# include <Executor/PAMAuth.h>
#endif

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
//
// class ExecutorImpl
//
//
////////////////////////////////////////////////////////////////////////////////

class ExecutorImpl
{
public:

    virtual ~ExecutorImpl() 
    {
    }

    virtual int detectExecutor() = 0;

    virtual int ping() = 0;

    virtual FILE* openFile(
        const char* path,
        int mode) = 0;

    virtual int renameFile(
        const char* oldPath,
        const char* newPath) = 0;

    virtual int removeFile(
        const char* path) = 0;

    virtual int startProviderAgent(
        const char* module, 
        const String& userName,
        int uid,
        int gid, 
        int& pid,
        AnonymousPipe*& readPipe,
        AnonymousPipe*& writePipe) = 0;

    virtual int daemonizeExecutor() = 0;

    virtual int reapProviderAgent(
        int pid) = 0;

    virtual int authenticatePassword(
        const char* username,
        const char* password) = 0;

    virtual int validateUser(
        const char* username) = 0;

    virtual int challengeLocal(
        const char* username,
        char challengeFilePath[EXECUTOR_BUFFER_SIZE]) = 0;

    virtual int authenticateLocal(
        const char* challengeFilePath,
        const char* response) = 0;
};

////////////////////////////////////////////////////////////////////////////////
//
//
// class ExecutorLoopbackImpl
//
//
////////////////////////////////////////////////////////////////////////////////

class ExecutorLoopbackImpl : public ExecutorImpl
{
public:

    virtual ~ExecutorLoopbackImpl() 
    {
    }

    virtual int detectExecutor()
    {
        return -1;
    }

    virtual int ping()
    {
        return -1;
    }

    virtual FILE* openFile(
        const char* path,
        int mode)
    {
        switch (mode)
        {
            case 'r':
                return fopen(path, "rb");

            case 'w':
                return fopen(path, "wb");

            case 'a':
                return fopen(path, "a+");

            default:
                return NULL;
        }
    }

    virtual int renameFile(
        const char* oldPath,
        const char* newPath)
    {
        return FileSystem::renameFile(oldPath, newPath) ? 0 : -1;
    }


    virtual int removeFile(
        const char* path)
    {
        return FileSystem::removeFile(path) ? 0 : -1;
    }


    virtual int startProviderAgent(
        const char* module, 
        const String& userName,
        int uid,
        int gid, 
        int& pid,
        AnonymousPipe*& readPipe,
        AnonymousPipe*& writePipe)
    {
        // Add logging here.

#if defined(PEGASUS_OS_TYPE_WINDOWS)

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

#elif defined(PEGASUS_OS_OS400)

        // ATTN: no implementation for OS400.
        return -1;

#else /* POSIX CASE FOLLOWS */

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

#if defined(PEGASUS_OS_VMS)
            pid = (int)vfork();
#else
            pid = (int)fork();
#endif

            if (pid < 0)
                return -1;

            // If child proceses.

            if (pid == 0)
            {
#if !defined(PEGASUS_OS_VMS)
                // Close unused pipe descriptors:

                close(to[1]);
                close(from[0]);


                // Close unused descriptors. Leave stdin, stdout, stderr, 
                // and the child's pipe descriptors open.

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
                    PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL4,
                        "Changing user context to: userName=%s uid=%d, gid=%d",
                        (const char*)userName.getCString(), uid, gid));

                    if (setgid(gid) != 0)
                    {
                        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                          String("setgid failed: ") + String(strerror(errno)));
                        return false;
                    }

                    if (setuid(uid) != 0)
                    {
                        PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                          String("setuid failed: ") + String(strerror(errno)));
                        return false;
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
                    if (execl(cstr, cstr, arg1, arg2, module, (char*)0) == -1)
                    {
                        PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL2,
                            "execl() failed.  errno = %d.", errno));
                        _exit(1);
                    }
                }
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

#endif /* !defined(START_PROVIDER_AGENT) */
    }

    virtual int daemonizeExecutor()
    {
        return -1;
    }

    virtual int reapProviderAgent(
        int pid)
    {
        int status = 0;

#if defined(PEGASUS_HAS_SIGNALS)
        while ((status = waitpid(pid, 0, 0)) == -1 && errno == EINTR)
            ;
#endif

        return status;
    }

    virtual int authenticatePassword(
        const char* username,
        const char* password)
    {
#if defined(PEGASUS_PAM_AUTHENTICATION)
        return PAMAuthenticate(username, password);
#else
        // ATTN: not handled so don't call in this case.
        return -1;
#endif
    }

    virtual int validateUser(
        const char* username)
    {
    #if defined(PEGASUS_PAM_AUTHENTICATION)
        return PAMValidateUser(username);
    #else
        // ATTN: not handled so don't call in this case.
        return -1;
    #endif
    }

    virtual int challengeLocal(
        const char* username,
        char challengeFilePath[EXECUTOR_BUFFER_SIZE])
    {
        // ATTN: not handled so don't call in this case.
        return -1;
    }

    virtual int authenticateLocal(
        const char* challengeFilePath,
        const char* response)
    {
        // ATTN: not handled so don't call in this case.
        return -1;
    }

private:

    static int _getProviderAgentPath(String& path)
    {
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

    Mutex _mutex;
};

////////////////////////////////////////////////////////////////////////////////
//
//
// class ExecutorSocketImpl : public ExecutorImpl
//
//
////////////////////////////////////////////////////////////////////////////////

#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)

class ExecutorSocketImpl : public ExecutorImpl
{
public:

    ExecutorSocketImpl(int sock) : _sock(sock)
    {
    }

    virtual ~ExecutorSocketImpl() 
    {
    }

    virtual int detectExecutor()
    {
        return 0;
    }

    virtual int ping()
    {
        AutoMutex autoMutex(_mutex);

        // _send request header:

        ExecutorRequestHeader header;
        header.code = EXECUTOR_PING_MESSAGE;

        if (_send(_sock, &header, sizeof(header)) != sizeof(header))
            return -1;

        ExecutorPingResponse response;

        if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
            return -1;

        if (response.magic == EXECUTOR_PING_MAGIC)
            return 0;

        return -1;
    }

    virtual FILE* openFile(
        const char* path,
        int mode)
    {
        AutoMutex autoMutex(_mutex);

        if (mode != 'r' && mode != 'w' && mode != 'a')
            return NULL;

        // _send request header:

        ExecutorRequestHeader header;
        header.code = EXECUTOR_OPEN_FILE_MESSAGE;

        if (_send(_sock, &header, sizeof(header)) != sizeof(header))
            return NULL;

        // _send request body.

        ExecutorOpenFileRequest request;
        memset(&request, 0, sizeof(request));
        Strlcpy(request.path, path, EXECUTOR_BUFFER_SIZE);
        request.mode = mode;

        if (_send(_sock, &request, sizeof(request)) != sizeof(request))
            return NULL;

        // Receive the response

        ExecutorOpenFileResponse response;

        if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
            return NULL;

        // Receive descriptor (if response successful).

        if (response.status == 0)
        {
            int fds[1];

            if (RecvDescriptorArray(_sock, fds, 1) != 0)
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

    virtual int renameFile(
        const char* oldPath,
        const char* newPath)
    {
        AutoMutex autoMutex(_mutex);

        // _send request header:

        ExecutorRequestHeader header;
        header.code = EXECUTOR_RENAME_FILE_MESSAGE;

        if (_send(_sock, &header, sizeof(header)) != sizeof(header))
            return -1;

        // _send request body.

        ExecutorRenameFileRequest request;
        memset(&request, 0, sizeof(request));
        Strlcpy(request.oldPath, oldPath, EXECUTOR_BUFFER_SIZE);
        Strlcpy(request.newPath, newPath, EXECUTOR_BUFFER_SIZE);

        if (_send(_sock, &request, sizeof(request)) != sizeof(request))
            return -1;

        // Receive the response

        ExecutorRenameFileResponse response;

        if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
            return -1;

        return response.status;
    }

    virtual int removeFile(
        const char* path)
    {
        AutoMutex autoMutex(_mutex);

        // _send request header:

        ExecutorRequestHeader header;
        header.code = EXECUTOR_REMOVE_FILE_MESSAGE;

        if (_send(_sock, &header, sizeof(header)) != sizeof(header))
            return -1;

        // _send request body.

        ExecutorRemoveFileRequest request;
        memset(&request, 0, sizeof(request));
        Strlcpy(request.path, path, EXECUTOR_BUFFER_SIZE);

        if (_send(_sock, &request, sizeof(request)) != sizeof(request))
            return -1;

        // Receive the response

        ExecutorRemoveFileResponse response;

        if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
            return -1;

        return response.status;
    }

    virtual int startProviderAgent(
        const char* module, 
        const String& userName,
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
        header.code = EXECUTOR_START_PROVIDER_AGENT_MESSAGE;

        if (_send(_sock, &header, sizeof(header)) != sizeof(header))
            return -1;

        // _send request body.

        ExecutorStartProviderAgentRequest request;
        memset(&request, 0, sizeof(request));
        memcpy(request.module, module, n);
        request.uid = uid;
        request.gid = gid;

        if (_send(_sock, &request, sizeof(request)) != sizeof(request))
            return -1;

        // Receive the response

        ExecutorStartProviderAgentResponse response;

        if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
            return -1;

        // Check response status and pid.

        if (response.status != 0)
            return -1;

        // Get pid:

        pid = response.pid;

        // Receive descriptors.

        int descriptors[2];
        int result = RecvDescriptorArray(_sock, descriptors, 2);

        if (result == 0)
        {
            int readFd = descriptors[0];
            int writeFd = descriptors[1];

            // Create to and from AnonymousPipe instances to correspond to 
            // the pipe descriptors created above.

            char readFdStr[32];
            char writeFdStr[32];
            sprintf(readFdStr, "%d", readFd);
            sprintf(writeFdStr, "%d", writeFd);

            readPipe = new AnonymousPipe(readFdStr, 0);
            writePipe = new AnonymousPipe(0, writeFdStr);
        }

        return result;
    }

    virtual int daemonizeExecutor()
    {
        AutoMutex autoMutex(_mutex);

        // _send request header:

        ExecutorRequestHeader header;
        header.code = EXECUTOR_DAEMONIZE_EXECUTOR_MESSAGE;

        if (_send(_sock, &header, sizeof(header)) != sizeof(header))
            return -1;

        // Receive the response

        ExecutorDaemonizeExecutorResponse response;

        if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
            return -1;

        return response.status;
    }

    virtual int reapProviderAgent(
        int pid)
    {
        AutoMutex autoMutex(_mutex);

        // _send request header:

        ExecutorRequestHeader header;
        header.code = EXECUTOR_REAP_PROVIDER_AGENT;

        if (_send(_sock, &header, sizeof(header)) != sizeof(header))
            return -1;

        // _send request body:

        ExecutorReapProviderAgentRequest request;
        memset(&request, 0, sizeof(request));
        request.pid = pid;

        if (_send(_sock, &request, sizeof(request)) != sizeof(request))
            return -1;

        // Receive the response

        ExecutorReapProviderAgentResponse response;

        if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
            return -1;

        return response.status;
    }

    virtual int authenticatePassword(
        const char* username,
        const char* password)
    {
        AutoMutex autoMutex(_mutex);

        // _send request header:

        ExecutorRequestHeader header;
        header.code = EXECUTOR_AUTHENTICATE_PASSWORD_MESSAGE;

        if (_send(_sock, &header, sizeof(header)) != sizeof(header))
            return -1;

        // _send request body.

        ExecutorAuthenticatePasswordRequest request;
        memset(&request, 0, sizeof(request));
        Strlcpy(request.username, username, EXECUTOR_BUFFER_SIZE);
        Strlcpy(request.password, password, EXECUTOR_BUFFER_SIZE);

        if (_send(_sock, &request, sizeof(request)) != sizeof(request))
            return -1;

        // Receive the response

        ExecutorAuthenticatePasswordResponse response;

        if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
            return -1;

        return response.status;
    }

    virtual int validateUser(
        const char* username)
    {
        AutoMutex autoMutex(_mutex);

        // _send request header:

        ExecutorRequestHeader header;
        header.code = EXECUTOR_VALIDATE_USER_MESSAGE;

        if (_send(_sock, &header, sizeof(header)) != sizeof(header))
            return -1;

        // _send request body.

        ExecutorValidateUserRequest request;
        memset(&request, 0, sizeof(request));
        Strlcpy(request.username, username, EXECUTOR_BUFFER_SIZE);

        if (_send(_sock, &request, sizeof(request)) != sizeof(request))
            return -1;

        // Receive the response

        ExecutorValidateUserResponse response;

        if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
            return -1;

        return response.status;
    }

    virtual int challengeLocal(
        const char* username,
        char challengeFilePath[EXECUTOR_BUFFER_SIZE])
    {
        AutoMutex autoMutex(_mutex);

        // _send request header:

        ExecutorRequestHeader header;
        header.code = EXECUTOR_CHALLENGE_LOCAL_MESSAGE;

        if (_send(_sock, &header, sizeof(header)) != sizeof(header))
            return -1;

        // _send request body.

        ExecutorChallengeLocalRequest request;
        memset(&request, 0, sizeof(request));
        Strlcpy(request.user, username, EXECUTOR_BUFFER_SIZE);

        if (_send(_sock, &request, sizeof(request)) != sizeof(request))
            return -1;

        // Receive the response

        ExecutorChallengeLocalResponse response;

        if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
            return -1;

        Strlcpy(challengeFilePath, response.challenge, EXECUTOR_BUFFER_SIZE);

        return response.status;
    }

    virtual int authenticateLocal(
        const char* challengeFilePath,
        const char* response)
    {
        AutoMutex autoMutex(_mutex);

        // _send request header:

        ExecutorRequestHeader header;
        header.code = EXECUTOR_AUTHENTICATE_LOCAL_MESSAGE;

        if (_send(_sock, &header, sizeof(header)) != sizeof(header))
            return -1;

        // _send request body.

        ExecutorAuthenticateLocalRequest request;
        memset(&request, 0, sizeof(request));
        Strlcpy(request.challenge, challengeFilePath, EXECUTOR_BUFFER_SIZE);
        Strlcpy(request.response, response, EXECUTOR_BUFFER_SIZE);

        if (_send(_sock, &request, sizeof(request)) != sizeof(request))
            return -1;

        // Receive the response

        ExecutorAuthenticateLocalResponse response_;

        if (_recv(_sock, &response_, sizeof(response_)) != sizeof(response_))
            return -1;

        return response_.status;
    }

private:

    static ssize_t _recv(int sock, void* buffer, size_t size)
    {
        size_t r = size;
        char* p = (char*)buffer;

        if (size == 0)
            return -1;

        while (r)
        {
            ssize_t n;

            EXECUTOR_RESTART(read(sock, p, r), n);

            if (n == -1)
                return -1;
            else if (n == 0)
                return size - r;

            r -= n;
            p += n;
        }

        return size - r;
    }

    static ssize_t _send(int sock, void* buffer, size_t size)
    {
        size_t r = size;
        char* p = (char*)buffer;

        while (r)
        {
            ssize_t n;
            EXECUTOR_RESTART(write(sock, p, r), n);

            if (n == -1)
                return -1;
            else if (n == 0)
                return size - r;

            r -= n;
            p += n;
        }

        return size - r;
    }

    int _sock;
    Mutex _mutex;
};

#endif /* defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION) */

////////////////////////////////////////////////////////////////////////////////
//
//
// class Executor
//
//
////////////////////////////////////////////////////////////////////////////////

static int _sock = -1;
static ExecutorImpl* _impl = 0;
static Mutex _mutex;

static ExecutorImpl* _getImpl()
{
    // Use the double-checked locking technique to avoid the overhead of a lock
    // on every call.

    if (_impl == 0)
    {
        _mutex.lock();

        if (_impl == 0)
        {
#if defined(PEGASUS_ENABLE_PRIVILEGE_SEPARATION)
            if (_sock == -1)
                _impl = new ExecutorLoopbackImpl();
            else
                _impl = new ExecutorSocketImpl(_sock);
#else
            _impl = new ExecutorLoopbackImpl();
#endif
        }

        _mutex.unlock();
    }

    return _impl;
}

void Executor::setSock(int sock)
{
    _mutex.lock();
    _sock = sock;
    _mutex.unlock();
}

int Executor::detectExecutor()
{
    return _getImpl()->detectExecutor();
}

int Executor::ping()
{
    return _getImpl()->ping();
}

FILE* Executor::openFile(
    const char* path,
    int mode)
{
    return _getImpl()->openFile(path, mode);
}

int Executor::renameFile(
    const char* oldPath,
    const char* newPath)
{
    return _getImpl()->renameFile(oldPath, newPath);
}

int Executor::removeFile(
    const char* path)
{
    return _getImpl()->removeFile(path);
}

int Executor::startProviderAgent(
    const char* module, 
    const String& userName,
    int uid,
    int gid, 
    int& pid,
    AnonymousPipe*& readPipe,
    AnonymousPipe*& writePipe)
{
    return _getImpl()->startProviderAgent(module, 
        userName, uid, gid, pid, readPipe, writePipe);
}

int Executor::daemonizeExecutor()
{
    return _getImpl()->daemonizeExecutor();
}

int Executor::reapProviderAgent(
    int pid)
{
    return _getImpl()->reapProviderAgent(pid);
}

int Executor::authenticatePassword(
    const char* username,
    const char* password)
{
    return _getImpl()->authenticatePassword(username, password);
}

int Executor::validateUser(
    const char* username)
{
    return _getImpl()->validateUser(username);
}

int Executor::challengeLocal(
    const char* user,
    char challengeFilePath[EXECUTOR_BUFFER_SIZE])
{
    return _getImpl()->challengeLocal(user, challengeFilePath);
}

int Executor::authenticateLocal(
    const char* challengeFilePath,
    const char* response)
{
    return _getImpl()->authenticateLocal(challengeFilePath, response);
}

PEGASUS_NAMESPACE_END
