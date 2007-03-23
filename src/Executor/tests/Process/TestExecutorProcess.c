#include <Executor/Process.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    char name[EXECUTOR_BUFFER_SIZE];

    assert(GetProcessName(getpid(), name) == 0);
    assert(strcmp(name, "TestExProcess") == 0);

    printf("+++++ passed all tests\n");

    return 0;
}
