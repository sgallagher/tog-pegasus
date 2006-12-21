#include <ExecutorClient/ExecutorClient.h>
#include <unistd.h>
#include <cassert>
#include <cstdio>

#define EXECUTOR_SOCKET 3

PEGASUS_USING_PEGASUS;

int main(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("[%s]\n", argv[i]);
    }

    // exit(1);

    // Ping the exectuor.

    if (ExecutorClient::ping() != 0)
    {
        fprintf(stderr, "cimservermainx must be started by executor\n");
        exit(1);
    }

    // Ask the executor process to open a file.

    int fd = ExecutorClient::openFileForRead("/etc/passwd");

    if (fd == -1)
    {
        fprintf(stderr, "openFileForRead() failed\n");
        exit(1);
    }

    char buffer[1024];
    size_t n;

    while ((n = read(fd, buffer, sizeof(buffer))) > 0)
    {
        printf("%*.*s\n", int(n), int(n), buffer);
    }

    fprintf(stderr, "this is standard error\n");

    exit(1);

    for (;;)
    {
        printf("sleeping...\n");
        sleep(1);
    }

    return 0;
}
