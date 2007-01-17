#include <Executor/Socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

static const char token[] = "A65F5382BC3D4F12AE24A1F6110015AF";

void Child(int sock)
{
    char buffer[EXECUTOR_BUFFER_SIZE];
    int fd;

    /* Echo token from parent. */

    memset(buffer, 0xFF, sizeof(buffer));
    assert(RecvNonBlock(sock, buffer, sizeof(token)) == sizeof(token));
    assert(strcmp(buffer, token) == 0);

    /* Open file for parent and pass descriptor back. */

    fd = open("token.dat", O_RDONLY);
    assert(fd >= 0);

    /* Send descriptor back to parent. */

    assert(SendNonBlock(sock, buffer, sizeof(token)) == sizeof(token));
    assert(SendDescriptorArray(sock, &fd, 1) == 0);
    assert(close(fd) == 0);
    exit(55);
}

void Parent(int pid, int sock)
{
    char buffer[EXECUTOR_BUFFER_SIZE];
    int status;
    int fd;

    /* Send token to child and read it back. */

    memset(buffer, 0xFF, sizeof(buffer));
    assert(SendNonBlock(sock, token, sizeof(token)) == sizeof(token));
    assert(RecvNonBlock(sock, buffer, sizeof(token)) == sizeof(token));
    assert(strcmp(token, buffer) == 0);

    /* Wait for descriptor from child. */

    memset(buffer, 0xFF, sizeof(buffer));

    assert(RecvDescriptorArray(sock, &fd, 1) == 0);

    assert(read(fd, buffer, sizeof(token)) == sizeof(token));
    assert(close(fd) == 0);
    assert(strcmp(token, buffer) == 0);

    /* Delete the file. */
    assert(unlink("token.dat") == 0);

    /* Wait for child to exit. */

    waitpid(pid, &status, 0);

    assert(WIFEXITED(status));
    assert(WEXITSTATUS(status) == 55);

    printf("+++++ passed all tests\n");
    exit(0);
}

int main()
{
    int pair[2];
    int pid;

    /* Create a file with a token in it. */
    {
        int fd;
        fd = open("token.dat", O_WRONLY | O_CREAT | O_TRUNC);
        assert(fd >= 0);
        assert(write(fd, token, sizeof(token)) == sizeof(token));
        close(fd);
    }

    /* Create socket pair for talking to child. */

    assert(CreateSocketPair(pair) == 0);
    assert(SetNonBlocking(pair[0]) == 0);
    assert(SetNonBlocking(pair[1]) == 0);

    /* Fork child. */

    pid = fork();
    assert(pid >= 0);

    /* Child */

    if (pid == 0)
    {
        close(pair[1]);
        Child(pair[0]);
    }

    /* Parent */

    if (pid != 0)
    {
        close(pair[0]);
        Parent(pid, pair[1]);
    }

    printf("+++++ passed all tests\n");

    return 0;
}
