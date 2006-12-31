#include "Pegasus/Security/Cimservera/cimservera.h"

int main(int argc, char** argv)
{
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
