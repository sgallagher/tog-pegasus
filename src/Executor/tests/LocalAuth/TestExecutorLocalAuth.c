#include <Executor/LocalAuth.h>
#include <stdio.h>
#include <assert.h>

int main()
{
    char path[EXECUTOR_BUFFER_SIZE];
    char token[EXECUTOR_BUFFER_SIZE];
    SessionKey key;
    FILE* is;

    /* Start authentication and get challenge. */

    assert(StartLocalAuthentication("pegasus", path, &key) == 0);

    /* Read token from file. */

    is = fopen(path, "r");
    assert(is != NULL);
    assert(fgets(token, sizeof(token), is) != NULL);

    /* Finish authentication. */

    assert(FinishLocalAuthentication(&key, token) == 0);

    printf("+++++ passed all tests\n");

    return 0;
}
