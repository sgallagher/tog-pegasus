#include <Executor/Executor.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main()
{
    {
        char buffer[1024];
        size_t n;
        strcpy(buffer, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

        n = Strlcpy(buffer, "abc", sizeof(buffer));
        assert(n == 3);
        assert(strcmp(buffer, "abc") == 0);
    }

    {
        char buffer[1024];
        size_t n;
        strcpy(buffer, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

        n = Strlcpy(buffer, "abcdefg", 4);
        assert(n == 7);
        assert(strcmp(buffer, "abc") == 0);
    }

    {
        char buffer[1024];
        size_t n;
        strcpy(buffer, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

        n = Strlcpy(buffer, "", 4);
        assert(n == 0);
        assert(strcmp(buffer, "") == 0);
    }

    {
        char buffer[1024];
        size_t n;
        strcpy(buffer, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

        n = Strlcpy(buffer, "", 1);
        assert(n == 0);
        assert(strcmp(buffer, "") == 0);
    }

    {
        char buffer[1024];
        size_t n;
        strcpy(buffer, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

        n = Strlcpy(buffer, "1234567890", 1);
        assert(n == 10);
        assert(strcmp(buffer, "") == 0);
    }

    {
        char buffer[1024];
        size_t n;
        strcpy(buffer, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

        n = Strlcpy(buffer, "1234567890", 2);
        assert(n == 10);
        assert(strcmp(buffer, "1") == 0);
    }

    printf("+++++ passed all tests\n");

    return 0;
}
