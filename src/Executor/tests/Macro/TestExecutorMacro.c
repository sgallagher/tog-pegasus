#include <Executor/Macro.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main()
{

    /* Test DefineMacro() */
    {
        assert(DefineMacro("x", "100") == 0);
        assert(DefineMacro("y", "hello") == 0);
        assert(DefineMacro("z", "true") == 0);
    }

    /* Test FindMacro() */
    {
        const char* x;
        const char* y;
        const char* z;

        x = FindMacro("x");
        assert(x != NULL && strcmp(x, "100") == 0);

        y = FindMacro("y");
        assert(y != NULL && strcmp(y, "hello") == 0);

        z = FindMacro("z");
        assert(z != NULL && strcmp(z, "true") == 0);
    }

    /* Test ExpandMacro() */
    {
        char buffer[EXECUTOR_BUFFER_SIZE];

        assert(ExpandMacros("${x} ${y} ${z}", buffer) == 0);
        assert(strcmp(buffer, "100 hello true") == 0);
    }

#if 0
    DumpMacros();
#endif

    printf("+++++ passed all tests\n");

    return 0;
}
