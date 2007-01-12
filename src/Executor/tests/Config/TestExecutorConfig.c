#include <Executor/Config.h>
#include <Executor/Globals.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main()
{
    /* Test GetConfigParamFromCommandLine() */
    {
        static const char* argv[] = { "program", "option1=one", (char*)0 };
        static const int argc = sizeof(argv) / sizeof(argv[0]);
        char buffer[EXECUTOR_BUFFER_SIZE];

        globals.argv = (char**)argv;
        globals.argc = argc;

        assert(GetConfigParamFromCommandLine("option1", buffer) == 0);
        assert(strcmp(buffer, "one") == 0);
    }

    /* Test GetConfigParamFromFile() */
    {
        char buffer[EXECUTOR_BUFFER_SIZE];

        assert(GetConfigParamFromFile("my.conf", "option1", buffer) == 0);
        assert(strcmp(buffer, "1") == 0);

        assert(GetConfigParamFromFile("my.conf", "option2", buffer) == 0);
        assert(strcmp(buffer, "2") == 0);

        assert(GetConfigParamFromFile("my.conf", "option3", buffer) == 0);
        assert(strcmp(buffer, "3") == 0);
    }

    printf("+++++ passed all tests\n");

    return 0;
}
