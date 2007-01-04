#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include "Socket.h"
#include "Bit.h"
#include "Exit.h"
#include "Globals.h"
#include "Defines.h"

//==============================================================================
//
// CloseOnExec()
//
//     Direct the kernel not to keep the given file descriptor open across
//     exec() system call.
//
//==============================================================================

int CloseOnExec(int fd)
{
    return fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
}

//==============================================================================
//
// SetNonBlocking()
//
//     Set the given socket into non-blocking mode.
//
//==============================================================================

int SetNonBlocking(int sock)
{
    return fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
}

//==============================================================================
//
// _waitForReadEnable()
//
//     Wait until the given socket is read-enabled. Returns 1 if read enabled
//     and 0 on timed out.
//
//==============================================================================

static int _waitForReadEnable(int sock, long timeoutMsec)
{
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sock, &readSet);

    struct timeval timeout;
    timeout.tv_sec = timeoutMsec / 1000;
    timeout.tv_usec = (timeoutMsec % 1000) * 1000;

    return select(sock + 1, &readSet, 0, 0, &timeout);
}

//==============================================================================
//
// _waitForWriteEnable()
//
//     Wait until the given socket is write-enabled. Returns 1 if write enabled
//     and 0 on timed out.
//
//==============================================================================

static int _waitForWriteEnable(int sock, long timeoutMsec)
{
    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(sock, &writeSet);

    struct timeval timeout;
    timeout.tv_sec = timeoutMsec / 1000;
    timeout.tv_usec = (timeoutMsec % 1000) * 1000;

    return select(sock + 1, 0, &writeSet, 0, &timeout);
}

//==============================================================================
//
// RecvNonBlock()
//
//     Receive at least size bytes from the given non-blocking socket.
//
//==============================================================================

ssize_t RecvNonBlock(
    int sock, 
    void* buffer, 
    size_t size)
{
    const long TIMEOUT_MSEC = 250;
    size_t r = size;
    char* p = (char*)buffer;

    if (size == 0)
        return -1;

    while (r)
    {
        int status = _waitForReadEnable(sock, TIMEOUT_MSEC);

        if (TstBit(globalSignalMask, SIGTERM) || 
            TstBit(globalSignalMask, SIGINT))
        {
            // Exit on either of these signals.
            Exit(0);
        }

        if (status == 0)
            continue;

        ssize_t n;
        EXECUTOR_RESTART(read(sock, p, r), n);

        if (n == -1 && errno == EINTR)
            continue;

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

//==============================================================================
//
// SendNonBlock()
//
//     Sends at least size bytes on the given non-blocking socket.
//
//==============================================================================

ssize_t SendNonBlock(
    int sock, 
    const void* buffer, 
    size_t size)
{
    const long TIMEOUT_MSEC = 250;
    size_t r = size;
    char* p = (char*)buffer;

    while (r)
    {
        // ATTN: handle this or not?
        int status = _waitForWriteEnable(sock, TIMEOUT_MSEC);

        if (TstBit(globalSignalMask, SIGTERM) || 
            TstBit(globalSignalMask, SIGINT))
        {
            // Exit on either of these signals.
            Exit(0);
        }

        if (status == 0)
            continue;

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
// SendDescriptorArray()
//
//     Send an array of descriptors (file, socket, pipe) to the child process. 
//
//==============================================================================

ssize_t SendDescriptorArray(int sock, int descriptors[], size_t count)
{
    // Allocate space for control header plus descriptors.

    size_t size = CMSG_SPACE(sizeof(int) * count);
    char* data = (char*)malloc(size);

    // Initialize msghdr struct to refer to control data.

    struct msghdr mh;
    memset(&mh, 0, sizeof(mh));
    mh.msg_control = data;
    mh.msg_controllen = size;

    // Fill in the control data struct with the descriptor and other fields.

    struct cmsghdr* cmh = CMSG_FIRSTHDR(&mh);
    cmh->cmsg_len = CMSG_LEN(sizeof(int) * count);
    cmh->cmsg_level = SOL_SOCKET;
    cmh->cmsg_type = SCM_RIGHTS;
    memcpy((int*)CMSG_DATA(cmh), descriptors, sizeof(int) * count);

    // Prepare to send single dummy byte. It will not be used but we must send
    // at least one byte otherwise the call will fail on some platforms.

    struct iovec iov[1];
    memset(iov, 0, sizeof(iov));

    char dummy = '\0';
    iov[0].iov_base = &dummy;
    iov[0].iov_len = 1;
    mh.msg_iov = iov;
    mh.msg_iovlen = 1;

    // Send message to child.

    int result = sendmsg(sock, &mh, 0);
    free(data);
    return result;
}

//==============================================================================
//
// SendDescriptorArray()
//
//     Send an array of descriptors (file, socket, pipe) to the child process. 
//
//==============================================================================

int CreateSocketPair(int pair[2])
{
    return socketpair(AF_UNIX, SOCK_STREAM, 0, pair);
}
