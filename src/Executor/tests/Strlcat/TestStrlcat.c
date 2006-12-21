#include <Executor/Executor.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main()
{
    {
        char buf[8];
        size_t n;
        memset(buf, 'X', sizeof(buf));

        *buf = '\0';
        n = Strlcat(buf, "", sizeof(buf));
        assert(n == 0);
        assert(strcmp(buf, "") == 0);

        n = Strlcat(buf, "abc", sizeof(buf));
        assert(n == 3);
        assert(strcmp(buf, "abc") == 0);

        n = Strlcat(buf, "xyz", sizeof(buf));
        assert(n == 6);
        assert(strcmp(buf, "abcxyz") == 0);

        n = Strlcat(buf, "lmnop", sizeof(buf));
        assert(n == 11);
        assert(strcmp(buf, "abcxyzl") == 0);

        n = Strlcat(buf, "xxx", sizeof(buf));
        assert(n == 10);
        assert(strcmp(buf, "abcxyzl") == 0);
    }

    {
        char buf[8];
        size_t n;
        memset(buf, 'X', sizeof(buf));

        n = Strlcat(buf, "abc", sizeof(buf));
        assert(n == sizeof(buf));
        assert(memcmp(buf, "XXXXXXXX", 8) == 0);
    }

    {
        char buf[8];
        size_t n;
        memset(buf, 'X', sizeof(buf));

        *buf = '\0';
        n = Strlcat(buf, "1234", sizeof(buf));
        assert(n == 4);
        assert(strcmp(buf, "1234") == 0);

        n = Strlcat(buf, "5678", sizeof(buf));
        assert(n == 8);
        assert(strcmp(buf, "1234567") == 0);
    }

    printf("+++++ passed all tests\n");

    return 0;
}
