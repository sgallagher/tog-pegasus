#include "LocalAuth.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "Defines.h"
#include "Strlcpy.h"
#include "Strlcat.h"
#include "User.h"
#include "Random.h"
#include "Time.h"
#include "SessionKey.h"
#include "Log.h"
#include "User.h"

static const int TOKEN_LENGTH = 40;

//==============================================================================
//
// CreateLocalAuthFile()
//
//     This function creates a local authentication file for the given *user*.
//     it populates the *path* argument and return 0 on success. The file has
//     the following format.
//
//         PEGASUS_LOCAL_AUTH_DIR/cimclient_<user>_<timestamp>_<seq>
//
//     For example:
//
//
//     The algorithm:
//
//         1. Form the path name as shown above.
//            (e.g., /tmp/cimclient_jsmith_1_232).
//
//         2. Generate a random token 
//            (e.g., 8F85CB1129B2B93F77F5CCA16850D659CCD16FE0).
//
//         3. Create the file (owner=root, permissions=0400).
//
//         4. Write random token to file.
//
//         5. Change owner of file to *user*.
//
//==============================================================================

static int CreateLocalAuthFile(
    const char* user,
    char path[EXECUTOR_BUFFER_SIZE])
{
    static unsigned int _nextSeq = 1;
    static pthread_mutex_t _nextSeqMutex = PTHREAD_MUTEX_INITIALIZER;

    // Assign next sequence number.

    pthread_mutex_lock(&_nextSeqMutex);
    unsigned int seq = _nextSeq++;
    pthread_mutex_unlock(&_nextSeqMutex);

    // Get microseconds elapsed since epoch.

    TimeStamp ts = GetTimeStamp();

    // Build path:

    Strlcpy(path, PEGASUS_LOCAL_AUTH_DIR, EXECUTOR_BUFFER_SIZE);
    Strlcat(path, "/cimclient_", EXECUTOR_BUFFER_SIZE);
    Strlcat(path, user, EXECUTOR_BUFFER_SIZE);
    char buffer[EXECUTOR_BUFFER_SIZE];
    sprintf(buffer, "_%u_%u", seq, (int)(ts / 1000));
    Strlcat(path, buffer, EXECUTOR_BUFFER_SIZE);

    // Generate random token.

    char token[TOKEN_LENGTH+1];
    {
        unsigned char data[TOKEN_LENGTH/2];
        FillRandomBytes(data, sizeof(data));
        RandBytesToHexASCII(data, sizeof(data), token);
    }

    // Create the file as read-only by user.

    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR);

    if (fd < 0)
        return -1;

    // Write the random token.

    if (write(fd, token, TOKEN_LENGTH) != TOKEN_LENGTH)
    {
        close(fd);
        unlink(path);
        return -1;
    }

    // Change owner of file.

    int uid;
    int gid;

    if (GetUserInfo(user, uid, gid) != 0)
    {
        close(fd);
        return -1;
    }

    if (fchown(fd, uid, gid) != 0)
    {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

//==============================================================================
//
// CheckLocalAuthToken()
//
//     Compare the *token* with the token in the given file. Return 0 if they
//     are identical.
//
//==============================================================================

static int CheckLocalAuthToken(
    const char* path,
    const char* token)
{
    // Open the file:

    int fd = open(path, O_RDONLY);

    if (fd < 0)
        return -1;

    // Read the token.

    char buffer[TOKEN_LENGTH+1];

    if (read(fd, buffer, TOKEN_LENGTH) != TOKEN_LENGTH)
    {
        close(fd);
        return -1;
    }

    buffer[TOKEN_LENGTH] = '\0';

    // Compare the token.

    if (strcmp(token, buffer) != 0)
    {
        close(fd);
        return -1;
    }

    // Okay!
    return 0;
}

//==============================================================================
//
// _destructor()
//
//     Destructor for session key data.
//
//==============================================================================

static void _destructor(void* data)
{
    if (!data)
        return;

    unlink((char*)data);
    free((char*)data);
}

//==============================================================================
//
// StartLocalAuthentication()
//
//     Initiate first phase of local authentication.
//
//==============================================================================

int StartLocalAuthentication(
    const char* user,
    char path[EXECUTOR_BUFFER_SIZE],
    SessionKey* key)
{
    // Get uid:

    int uid;
    int gid;

    if (GetUserInfo(user, uid, gid) != 0)
        return -1;

    // Create the local authentication file.

    if (CreateLocalAuthFile(user, path) != 0)
    {
        return -1;
    }

    // Create the session key (associated with path).

    *key = NewSessionKey(uid, strdup(path), _destructor);

    return 0;
}

//==============================================================================
//
// FinishLocalAuthentication()
//
//     Initiate second and last phase of local authentication.
//
//==============================================================================

int FinishLocalAuthentication(
    const SessionKey* key,
    const char* token,
    SessionKey* newKey)
{
    // Get session key data (the path).

    void* data = 0;

    if (GetSessionKeyData(key, &data) != 0)
        return -1;

    int uid;

    if (GetSessionKeyUid(key, &uid) != 0)
        return -1;

    // Check token against the one in the file.

    if (CheckLocalAuthToken((const char*)data, token) != 0)
    {
        DeleteSessionKey(key);
        return -1;
    }

    // Delete session key (and file).

    if (DeleteSessionKey(key) != 0)
        return -1;

    // Create new session key.

    *newKey = NewSessionKey(uid, 0, 0);

    return 0;
}
