#include <ExecutorClient/ExecutorClient.h>
#include <unistd.h>
#include <cassert>
#include <cstdio>

PEGASUS_USING_PEGASUS;

int main(int argc, char** argv)
{
    // Get inherited socket (the parent process guarntees that the socket
    // will be three).

    int executor_sock = 3;

    // Create ExecutorClient:

    ExecutorClient* client = new ExecutorClient(executor_sock);

    // Ping the exectuor.

    if (client->ping() != 0)
    {
        fprintf(stderr, "cimservermainx must be started by executor\n");
        exit(1);
    }

    // Ask the executor process to open a file.

    int fd = client->openFileForRead("/etc/passwd");

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

    for (;;)
    {
        printf("sleeping...\n");
        sleep(1);
    }

    return 0;
}
