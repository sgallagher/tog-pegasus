#include <Executor/LocalAuth.h>
#include <stdio.h>
#include <assert.h>

int main()
{
    char challenge[EXECUTOR_BUFFER_SIZE];
    char response[EXECUTOR_BUFFER_SIZE];
    FILE* is;

    /* Start authentication and get challenge. */

    assert(StartLocalAuthentication("pegasus", challenge) == 0);

    /* Read response from file. */

    is = fopen(challenge, "r");
    assert(is != NULL);
    assert(fgets(response, sizeof(response), is) != NULL);

    /* Finish authentication. */

    assert(FinishLocalAuthentication(challenge, response) == 0);

    printf("+++++ passed all tests\n");

    return 0;
}
