#include <Executor/Path.h>
#include <Executor/Strlcpy.h>
#include <Executor/Strlcat.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

static int test(const char* path, const char* expect)
{
    char buffer[EXECUTOR_BUFFER_SIZE];
    DirName(path, buffer);
    return strcmp(buffer, expect) == 0 ? 0 : -1;
}

int main()
{
    /* DirName() */
    {
        assert(test("/aaa", "/") == 0);
        assert(test("/aaa/", "/") == 0);
        assert(test("/aaa/bbb", "/aaa") == 0);
        assert(test("/aaa/bbb/ccc", "/aaa/bbb") == 0);
        assert(test("aaa/bbb", "aaa") == 0);
        assert(test("aaa", ".") == 0);
        assert(test("/", "/") == 0);
        assert(test("////", "/") == 0);
        assert(test("/etc/passwd", "/etc") == 0);
    }

    /* GetHomedPath */
    {
        char expect[EXECUTOR_BUFFER_SIZE];
        char actual[EXECUTOR_BUFFER_SIZE];
        const char* home;

        assert((home = getenv("PEGASUS_HOME")) != NULL);
        Strlcpy(expect, home, sizeof(expect));
        Strlcat(expect, "/somefile", sizeof(expect));

        assert(GetHomedPath("somefile", actual) == 0);
        assert(strcmp(expect, actual) == 0);

        memset(actual, 0, sizeof(actual));
        assert(GetHomedPath(expect, actual) == 0);
        assert(strcmp(expect, actual) == 0);
    }

    /* GetPegasusInternalBinDir() */
    {
        char expect[EXECUTOR_BUFFER_SIZE];
        char actual[EXECUTOR_BUFFER_SIZE];
        const char* home;

        assert((home = getenv("PEGASUS_HOME")) != NULL);
        Strlcpy(expect, home, sizeof(expect));
        Strlcat(expect, "/bin", sizeof(expect));

        assert(GetPegasusInternalBinDir(actual) == 0);
        assert(strcmp(expect, actual) == 0);
    }

    printf("+++++ passed all tests\n");

    return 0;
}
