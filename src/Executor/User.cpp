#include <sys/types.h>
#include <pwd.h>
#include "User.h"
#include "Log.h"
#include "Strlcpy.h"

//==============================================================================
//
// GetUserInfo()
//
//     Lookup the given user's uid and gid.
//
//==============================================================================

int GetUserInfo(const char* user, int& uid, int& gid)
{
    struct passwd pwd;
    const unsigned int PWD_BUFF_SIZE = 4096;
    char buffer[PWD_BUFF_SIZE];
    struct passwd* ptr = 0;

    if (getpwnam_r(user, &pwd, buffer, PWD_BUFF_SIZE, &ptr) != 0 || !ptr)
    {
        Log(LL_TRACE, "getpwnam_r(%s, ...) failed", user);
        return -1;
    }

    uid = ptr->pw_uid;
    gid = ptr->pw_gid;

    return 0;
}

//==============================================================================
//
// GetUserName()
//
//     Lookup the given user's uid and gid.
//
//==============================================================================

int GetUserName(int uid, char username[EXECUTOR_BUFFER_SIZE])
{
    struct passwd pwd;
    const unsigned int PWD_BUFF_SIZE = 4096;
    char buffer[PWD_BUFF_SIZE];
    struct passwd* ptr = 0;

    if (getpwuid_r(uid, &pwd, buffer, PWD_BUFF_SIZE, &ptr) != 0 || !ptr)
    {
        Log(LL_TRACE, "getpwuid_r(%d, ...) failed", uid);
        return -1;
    }

    Strlcpy(username, ptr->pw_name, EXECUTOR_BUFFER_SIZE);
    return 0;
}
