#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include "ExecutorClient.h"
#include <Executor/Executor.h>
#include <Pegasus/Common/Mutex.h>
#include "private/ExecutorClientSocketImpl.h"

PEGASUS_NAMESPACE_BEGIN

// The exectuor socket will always be three since it is the first descriptor
// created by the forked process (recall that stdin=0, stdout=1, stderr=2).
static const size_t _sock = 3;

//==============================================================================
//
// _send()
//
//     Sends *size* bytes onto the given socket.
//
//==============================================================================

static ssize_t _send(int sock, const void* buffer, size_t size)
{
    size_t r = size;
    char* p = (char*)buffer;

    while (r)
    {
        ssize_t n;
        EXECUTOR_RESTART(write(sock, p, r), n);

        if (n == -1)
        {
            if (errno == EWOULDBLOCK)
                return size - r;
            else 
                return -1;
        }
        else if (n == 0)
            return size - r;

        r -= n;
        p += n;
    }

    return size - r;
}

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

        EXECUTOR_RESTART(read(sock, p, r), n);

        if (n == -1)
        {
            if (errno == EWOULDBLOCK)
            {
                size_t total = size - r;

                if (total)
                    return total;

                return -1;
            }
            else
                return -1;
        }
        else if (n == 0)
            return size - r;

        r -= n;
        p += n;
    }

    return size - r;
}

// ATTN: Get rid of this extra function (on both sides of connection).

static int _receiveDescriptor(int sock)
{
    // This control data begins with a cmsghdr struct followed by the data
    // (a descriptor in this case). The union ensures that the data is aligned 
    // suitably for the leading cmsghdr struct. The descriptor itself is
    // properly aligned since the cmsghdr ends on a boundary that is suitably 
    // aligned for any type (including int).
    //
    //     ControlData = [ cmsghdr | int ]

    union ControlData
    {
        struct cmsghdr cmh;
        char data[CMSG_SPACE(sizeof(int))];
    };

    // Define a msghdr that refers to the control data, which is filled in
    // by calling recvmsg() below.

    msghdr mh;
    memset(&mh, 0, sizeof(mh));

    ControlData cd;
    memset(&cd, 0, sizeof(cd));

    mh.msg_control = cd.data;
    mh.msg_controllen = sizeof(cd.data);
    mh.msg_name = NULL;
    mh.msg_namelen = 0;

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
        cmh->cmsg_len != CMSG_LEN(sizeof(int)) ||
        cmh->cmsg_level != SOL_SOCKET ||
        cmh->cmsg_type != SCM_RIGHTS)
    {
        return -1;
    }

    // Return the descriptor.

    return *((int*)CMSG_DATA(cmh));
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

ExecutorClientSocketImpl::ExecutorClientSocketImpl()
{
}

ExecutorClientSocketImpl::~ExecutorClientSocketImpl()
{
}

int ExecutorClientSocketImpl::ping()
{
    AutoMutex autoMutex(_mutex);

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_PING_REQUEST;

    if (_send(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    ExecutorPingResponse response;

    if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
        return -1;

    if (response.magic == EXECUTOR_PING_MAGIC)
        return 0;

    return -1;
}

FILE* ExecutorClientSocketImpl::openFile(
    const char* path,
    int mode)
{
    AutoMutex autoMutex(_mutex);

    if (mode != 'r' && mode != 'w')
        return NULL;

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_OPEN_FILE_REQUEST;

    if (_send(_sock, &header, sizeof(header)) != sizeof(header))
        return NULL;

    // Send request body.

    ExecutorOpenFileRequest request;
    memset(&request, 0, sizeof(request));
    Strlcpy(request.path, path, EXECUTOR_MAX_PATH_LENGTH);
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
        int fd = _receiveDescriptor(_sock);

        if (fd == -1)
            return NULL;
        else
        {
            if (mode == 'r')
                return fdopen(fd, "rb");
            else
                return fdopen(fd, "wb");
        }
    }

    return NULL;
}

int ExecutorClientSocketImpl::renameFile(
    const char* oldPath,
    const char* newPath)
{
    AutoMutex autoMutex(_mutex);

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_RENAME_FILE_REQUEST;

    if (_send(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Send request body.

    ExecutorRenameFileRequest request;
    memset(&request, 0, sizeof(request));
    Strlcpy(request.oldPath, oldPath, EXECUTOR_MAX_PATH_LENGTH);
    Strlcpy(request.newPath, newPath, EXECUTOR_MAX_PATH_LENGTH);

    if (_send(_sock, &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorRenameFileResponse response;

    if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
        return -1;

    return response.status;
}

int ExecutorClientSocketImpl::removeFile(
    const char* path)
{
    AutoMutex autoMutex(_mutex);

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_REMOVE_FILE_REQUEST;

    if (_send(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Send request body.

    ExecutorRemoveFileRequest request;
    memset(&request, 0, sizeof(request));
    Strlcpy(request.path, path, EXECUTOR_MAX_PATH_LENGTH);

    if (_send(_sock, &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorRemoveFileResponse response;

    if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
        return -1;

    return response.status;
}

int ExecutorClientSocketImpl::changeMode(
    const char* path,
    int mode)
{
    AutoMutex autoMutex(_mutex);

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_CHANGE_MODE_REQUEST;

    if (_send(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Send request body.

    ExecutorChangeModeRequest request;
    memset(&request, 0, sizeof(request));
    Strlcpy(request.path, path, EXECUTOR_MAX_PATH_LENGTH);
    request.mode = mode;

    if (_send(_sock, &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorChangeModeResponse response;

    if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
        return -1;

    return response.status;
}

int ExecutorClientSocketImpl::startProviderAgent(
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

    // Reject strings longer than EXECUTOR_MAX_PATH_LENGTH.

    size_t n = strlen(module);

    if (n >= EXECUTOR_MAX_PATH_LENGTH)
        return -1;

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_START_PROVIDER_AGENT_REQUEST;

    if (_send(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Send request body.

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
    int result = _receiveDescriptorArray(_sock, descriptors, 2);

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

int ExecutorClientSocketImpl::daemonizeExecutor()
{
    AutoMutex autoMutex(_mutex);

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_DAEMONIZE_EXECUTOR_REQUEST;

    if (_send(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Receive the response

    ExecutorDaemonizeExecutorResponse response;

    if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
        return -1;

    // Check response status and pid.

    return response.status;
}

int ExecutorClientSocketImpl::shutdownExecutor()
{
    AutoMutex autoMutex(_mutex);

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_SHUTDOWN_EXECUTOR_REQUEST;

    if (_send(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Receive the response

    ExecutorDaemonizeExecutorResponse response;

    if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
        return -1;

    // Check response status and pid.

    return response.status;
}

int ExecutorClientSocketImpl::changeOwner(
    const char* path,
    const char* owner)
{
    AutoMutex autoMutex(_mutex);

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_CHANGE_OWNER_REQUEST;

    if (_send(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Send request body:

    ExecutorChangeOwnerRequest request;
    Strlcpy(request.path, path, sizeof(request.path));
    Strlcpy(request.owner, owner, sizeof(request.owner));

    if (_send(_sock, &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorChangeOwnerResponse response;

    if (_recv(_sock, &response, sizeof(response)) != sizeof(response))
        return -1;

    // Check response status and pid.

    return response.status;
}

PEGASUS_NAMESPACE_END
