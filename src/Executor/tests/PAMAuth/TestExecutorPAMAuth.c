#include <Executor/PAMAuth.h>
#include <stdio.h>
#include <assert.h>

int main()
{
    char prompt[EXECUTOR_BUFFER_SIZE];

    /* Test PAMValidateUserInProcess(). */

    assert(PAMValidateUser(PEGASUS_CIMSERVERMAIN_USER) == 0);

    /* Test PAMAuthenticateInProcess(). */
    {
        const char* pw;
        sprintf(prompt, "Enter password for %s: ", PEGASUS_CIMSERVERMAIN_USER);
        pw = getpass(prompt);

        if (PAMAuthenticate(PEGASUS_CIMSERVERMAIN_USER, pw) == 0)
            printf("Correct password\n");
        else
            printf("Wrong password\n");

        putchar('\n');
    }

    return 0;
}
