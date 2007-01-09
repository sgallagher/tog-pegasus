
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

int GetServerUser(
    int argc,
    char** argv,
    char path[EXECUTOR_BUFFER_SIZE], 
    int* uid, 
    int* gid)
{
    struct stat st;
    const char DEFAULT_SERVER_USER[] = "pegasus";

    /* (1) Try to find serverUser as configuration parameter. */

    char user[EXECUTOR_BUFFER_SIZE];

    if (GetConfigParam(argc, argv, "serverUser", user) == 0)
    {
        if (GetUserInfo(user, uid, gid) == 0)
            return 0;

        Fatal(FL, "serverUser option specifies unknown user: %s", user);
        return -1;
    }

    /* (2) Use owner of the cimservermain program if not root. */

    if (stat(path, &st) != 0)
        Fatal(FL, "stat(%s) failed", path);

    if (st.st_uid != 0 && st.st_gid != 0)
    {
        *uid = st.st_uid;
        *gid = st.st_gid;
        return 0;
    }

    /* (3) Try the "pegasus" user (the default). */

    if (GetUserInfo(DEFAULT_SERVER_USER, uid, gid) == 0 && 
        *uid != 0 && 
        *gid != 0)
    {
        return 0;
    }

    Fatal(FL, 
        "cannot determine server user (used to run cimserermain). "
        "Please specify this value in one of four ways. (1) pass "
        "serverUser=<username> on the command line, (2) use cimconfig to "
        "set serverUser (using -p -s options), (3) make the desired "
        "user the owner of %s (i.e., use chown), or (4) create a system "
        "user whose user whose name is \"pegasus\".", path);

    return -1;
}
