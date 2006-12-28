#ifndef _Executor_Executor_h
#define _Executor_Executor_h

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#define EXECUTOR_MAX_PATH_LENGTH 4096
#define EXECUTOR_MAX_USERNAME_LENGTH 128

#define EXECUTOR_RESTART(F, X) while (((X = (F)) == -1) && (errno == EINTR))

//==============================================================================
//
// RequestCode
//
//==============================================================================

enum RequestCode
{
    EXECUTOR_PING_REQUEST = 1,
    EXECUTOR_OPEN_FILE_REQUEST,
    EXECUTOR_START_PROVIDER_AGENT_REQUEST,
    EXECUTOR_DAEMONIZE_EXECUTOR_REQUEST,
    EXECUTOR_CHANGE_OWNER_REQUEST,
    EXECUTOR_REMOVE_FILE_REQUEST,
    EXECUTOR_SHUTDOWN_EXECUTOR_REQUEST,
    EXECUTOR_RENAME_FILE_REQUEST,
    EXECUTOR_CHANGE_MODE_REQUEST,
};

//==============================================================================
//
// struct ExecutorRequestHeader
//
//==============================================================================

struct ExecutorRequestHeader
{
    unsigned int code;
};

//==============================================================================
//
// EXECUTOR_PING_REQUEST
//
//==============================================================================

#define EXECUTOR_PING_MAGIC 0x9E5EACB6

struct ExecutorPingResponse
{
    unsigned int magic;
};

//==============================================================================
//
// EXECUTOR_OPEN_FILE_REQUEST
//
//==============================================================================

struct ExecutorOpenFileRequest
{
    char path[EXECUTOR_MAX_PATH_LENGTH];
    // ('r' = read, 'w' = write)
    int mode;
};

struct ExecutorOpenFileResponse
{
    int status;
};

//==============================================================================
//
// EXECUTOR_REMOVE_FILE_REQUEST
//
//==============================================================================

struct ExecutorRemoveFileRequest
{
    char path[EXECUTOR_MAX_PATH_LENGTH];
};

struct ExecutorRemoveFileResponse
{
    int status;
};

//==============================================================================
//
// EXECUTOR_RENAME_FILE_REQUEST
//
//==============================================================================

struct ExecutorRenameFileRequest
{
    char oldPath[EXECUTOR_MAX_PATH_LENGTH];
    char newPath[EXECUTOR_MAX_PATH_LENGTH];
};

struct ExecutorRenameFileResponse
{
    int status;
};

//==============================================================================
//
// EXECUTOR_CHANGE_MODE_REQUEST
//
//==============================================================================

struct ExecutorChangeModeRequest
{
    char path[EXECUTOR_MAX_PATH_LENGTH];
    int mode;
};

struct ExecutorChangeModeResponse
{
    int status;
};

//==============================================================================
//
// EXECUTOR_START_PROVIDER_AGENT_REQUEST
//
//==============================================================================

struct ExecutorStartProviderAgentRequest
{
    char module[EXECUTOR_MAX_PATH_LENGTH];
    int uid;
    int gid;
};

struct ExecutorStartProviderAgentResponse
{
    int status;
    int pid;
};

//==============================================================================
//
// EXECUTOR_DAEMONIZE_EXECUTOR_REQUEST
//
//==============================================================================

struct ExecutorDaemonizeExecutorResponse
{
    int status;
};

//==============================================================================
//
// EXECUTOR_SHUTDOWN_EXECUTOR_REQUEST
//
//==============================================================================

struct ExecutorShutdownExecutorResponse
{
    int status;
};

//==============================================================================
//
// EXECUTOR_CHANGE_OWNER_REQUEST
//
//==============================================================================

struct ExecutorChangeOwnerRequest
{
    char path[EXECUTOR_MAX_PATH_LENGTH];
    char owner[EXECUTOR_MAX_USERNAME_LENGTH];
};

struct ExecutorChangeOwnerResponse
{
    int status;
};

//==============================================================================
//
// ExecutorSends()
//
//     Sends *size* bytes onto the given socket.
//
//==============================================================================

static ssize_t ExecutorSend(int sock, const void* buffer, size_t size)
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
// ExecutorRecv()
//
//     Receives *size* bytes from the given socket.
//
//==============================================================================

static ssize_t ExecutorRecv(int sock, void* buffer, size_t size)
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

//==============================================================================
//
// Strlcpy()
//
//     This is an original implementation of the strlcpy() function as described
//     by Todd C. Miller in his popular security paper entitled "strlcpy and 
//     strlcat - consistent, safe, string copy and concatenation".
//
//     Note that this implementation favors readability over efficiency. More
//     efficient implemetations are possible but would be to complicated
//     to verify in a security audit.
//
//==============================================================================

static size_t Strlcpy(char* dest, const char* src, size_t size)
{
    size_t i;

    for (i = 0; src[i] && i + 1 < size; i++)
        dest[i] = src[i];

    if (size > 0)
        dest[i] = '\0';

    while (src[i])
        i++;

    return i;
}

//==============================================================================
//
// Strlcat()
//
//     This is an original implementation of the strlcat() function as described
//     by Todd C. Miller in his popular security paper entitled "strlcpy and 
//     strlcat - consistent, safe, string copy and concatenation".
//
//     Note that this implementation favors readability over efficiency. More
//     efficient implemetations are possible but would be to complicated
//     to verify in a security audit.
//
//==============================================================================

static size_t Strlcat(char* dest, const char* src, size_t size)
{
    size_t i;
    size_t j;

    // Find dest null terminator.

    for (i = 0; i < size && dest[i]; i++)
        ;

    // If no-null terminator found, return size.

    if (i == size)
        return size;

    // Copy src characters to dest.

    for (j = 0; src[j] && i + 1 < size; i++, j++)
        dest[i] = src[j];

    // Null terminate size non-zero.

    if (size > 0)
        dest[i] = '\0';

    while (src[j])
    {
        j++;
        i++;
    }

    return i;
}

//==============================================================================
//
// Strlncpy()
//
//     This is a variation of the strlcpy() function as described by Todd C. 
//     Miller in his popular security paper entitled "strlcpy and strlcat - 
//     consistent, safe, string copy and concatenation".
//
//     Note that this implementation favors readability over efficiency. More
//     efficient implemetations are possible but would be to complicated
//     to verify in a security audit.
//
//==============================================================================

static size_t Strlncpy(char* dest, const char* src, size_t size, size_t n)
{
    size_t i;

    for (i = 0; i < n && src[i] && i + 1 < size; i++)
        dest[i] = src[i];

    if (size > 0)
        dest[i] = '\0';

    while (i < n && src[i])
        i++;

    return i;
}

//==============================================================================
//
// Strlncat()
//
//     This is a variation of the strlcat() function as described
//     by Todd C. Miller in his popular security paper entitled "strlcpy and 
//     strlcat - consistent, safe, string copy and concatenation".
//
//     Note that this implementation favors readability over efficiency. More
//     efficient implemetations are possible but would be to complicated
//     to verify in a security audit.
//
//==============================================================================

static size_t Strlncat(char* dest, const char* src, size_t size, size_t n)
{
    size_t i;
    size_t j;

    // Find dest null terminator.

    for (i = 0; i < size && dest[i]; i++)
        ;

    // If no-null terminator found, return size.

    if (i == size)
        return size;

    // Copy src characters to dest.

    for (j = 0; j < n && src[j] && i + 1 < size; i++, j++)
        dest[i] = src[j];

    // Null terminate size non-zero.

    if (size > 0)
        dest[i] = '\0';

    while (j < n && src[j])
    {
        j++;
        i++;
    }

    return i;
}

#endif /* _Executor_Executor_h */
