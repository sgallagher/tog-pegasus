#include "Pegasus/Security/Cimservera/cimservera.h"
#include <sys/wait.h>

int main(int argc, char** argv)
{
    signal(SIGPIPE, SIG_IGN);

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s username password\n", argv[0]);
        exit(1);
    }

    if (CimserveraAuthenticate(argv[1], argv[2]) == 0)
        printf("Authentication suceeded\n");
    else
        printf("Authentication failed\n");

    return 0;
}
