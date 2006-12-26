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

PEGASUS_NAMESPACE_BEGIN

// The exectuor socket will always be three since it is the first descriptor
// created by the forked process (recall that stdin=0, stdout=1, stderr=2).
static const size_t _sock = 3;

// All requests to executor are serialized with this mutex.
static Mutex _mutex;

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

int ExecutorClient::ping()
{
    AutoMutex autoMutex(_mutex);

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_PING_REQUEST;

    if (ExecutorSend(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Receive the response

    ExecutorPingResponse response;

    if (ExecutorRecv(_sock, &response, sizeof(response)) != sizeof(response))
        return -1;

    if (response.magic == EXECUTOR_PING_MAGIC)
        return 0;

    return -1;
}

int ExecutorClient::openFileForRead(const char* path)
{
    AutoMutex autoMutex(_mutex);

    // Reject paths longer than EXECUTOR_MAX_PATH_LENGTH.

    size_t n = strlen(path);

    if (n >= EXECUTOR_MAX_PATH_LENGTH)
        return -1;

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_OPEN_FILE_REQUEST;

    if (ExecutorSend(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Send request body.

    ExecutorOpenFileRequest request;
    memset(&request, 0, sizeof(request));
    memcpy(request.path, path, n);
    request.flags = R_OK;

    if (ExecutorSend(_sock, &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorOpenFileResponse response;

    if (ExecutorRecv(_sock, &response, sizeof(response)) != sizeof(response))
        return -1;

    // Receive descriptor (if response successful).

    if (response.status == 0)
        return _receiveDescriptor(_sock);

    return -1;
}

int ExecutorClient::startProviderAgent(
    const char* module, 
    int gid, 
    int uid,
    int& pid,
    int& readFd,
    int& writeFd)
{
    AutoMutex autoMutex(_mutex);

    readFd = -1;
    writeFd = -1;

    // Reject strings longer than EXECUTOR_MAX_PATH_LENGTH.

    size_t n = strlen(module);

    if (n >= EXECUTOR_MAX_PATH_LENGTH)
        return -1;

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_START_PROVIDER_AGENT_REQUEST;

    if (ExecutorSend(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Send request body.

    ExecutorStartProviderAgentRequest request;
    memset(&request, 0, sizeof(request));
    memcpy(request.module, module, n);
    request.uid = uid;
    request.gid = gid;

    if (ExecutorSend(_sock, &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorStartProviderAgentResponse response;

    if (ExecutorRecv(_sock, &response, sizeof(response)) != sizeof(response))
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
        readFd = descriptors[0];
        writeFd = descriptors[1];
    }

    return result;
}

int ExecutorClient::daemonizeExecutor()
{
    AutoMutex autoMutex(_mutex);

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_DAEMONIZE_EXECUTOR_REQUEST;

    if (ExecutorSend(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Receive the response

    ExecutorDaemonizeExecutorResponse response;

    if (ExecutorRecv(_sock, &response, sizeof(response)) != sizeof(response))
        return -1;

    // Check response status and pid.

    return response.status;
}

int ExecutorClient::changeOwner(
    const char* path,
    const char* owner)
{
    AutoMutex autoMutex(_mutex);

    // Send request header:

    ExecutorRequestHeader header;
    header.code = EXECUTOR_CHANGE_OWNER_REQUEST;

    if (ExecutorSend(_sock, &header, sizeof(header)) != sizeof(header))
        return -1;

    // Send request body:

    ExecutorChangeOwnerRequest request;

    {
        size_t r = Strlcpy(request.path, path, sizeof(request.path));

        if (r >= sizeof(request.path))
            return -1;
    }

    {
        size_t r = Strlcpy(request.owner, owner, sizeof(request.owner));

        if (r >= sizeof(request.owner))
            return -1;
    }

    if (ExecutorSend(_sock, &request, sizeof(request)) != sizeof(request))
        return -1;

    // Receive the response

    ExecutorDaemonizeExecutorResponse response;

    if (ExecutorRecv(_sock, &response, sizeof(response)) != sizeof(response))
        return -1;

    // Check response status and pid.

    return response.status;
}

PEGASUS_NAMESPACE_END
