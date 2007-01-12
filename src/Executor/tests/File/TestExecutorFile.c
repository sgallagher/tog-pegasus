#include <Executor/File.h>
#include <Executor/User.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int touch(const char* path)
{
    FILE* fp = fopen(path, "w");

    if (fp)
        fclose(fp);

    return fp ? 0 : -1;
}

int owns(const char* path, int uid, int gid)
{
    struct stat st;

    if (stat(path, &st) != 0)
        return -1;

    if ((int)st.st_uid != uid || (int)st.st_gid != gid)
        return -1;

    return 0;
}

int main()
{
    int uid = -1;
    int gid = -1;

    /* AccessDir() */
    {
        assert(AccessDir("../File") == 0);
        assert(AccessDir("NoSuchDir") != 0);
    }

    /* Get uid-gid of PEGASUS_CIMSERVERMAIN_USER. */

    assert(GetUserInfo(PEGASUS_CIMSERVERMAIN_USER, &uid, &gid) == 0);
    assert(uid != -1);
    assert(gid != -1);

    /* ChangeDirOwnerRecursive() */
    {
        /* Create files and directories. */
        assert(mkdir("./a", 700) == 0);
        assert(touch("./a/a.tmp") == 0);
        assert(mkdir("./a/b", 700) == 0);
        assert(touch("./a/b/b.tmp") == 0);
        assert(mkdir("./a/c", 700) == 0);
        assert(touch("./a/c/c.tmp") == 0);
        assert(mkdir("./a/d", 700) == 0);
        assert(touch("./a/d/d.tmp") == 0);

        /* Change ownership */
        assert(ChangeDirOwnerRecursive("a", uid, gid) == 0);

        /* Check ownership. */
        assert(owns("./a", uid, gid) == 0);
        assert(owns("./a/a.tmp", uid, gid) == 0);
        assert(owns("./a/b", uid, gid) == 0);
        assert(owns("./a/b/b.tmp", uid, gid) == 0);
        assert(owns("./a/c", uid, gid) == 0);
        assert(owns("./a/c/c.tmp", uid, gid) == 0);
        assert(owns("./a/d", uid, gid) == 0);
        assert(owns("./a/d/d.tmp", uid, gid) == 0);

        /* Delete files and directories. */
        assert(unlink("./a/d/d.tmp") == 0);
        assert(rmdir("./a/d") == 0);
        assert(unlink("./a/c/c.tmp") == 0);
        assert(rmdir("./a/c") == 0);
        assert(unlink("./a/b/b.tmp") == 0);
        assert(rmdir("./a/b") == 0);
        assert(unlink("./a/a.tmp") == 0);
        assert(rmdir("./a") == 0);
    }

    printf("+++++ passed all tests\n");

    return 0;
}
