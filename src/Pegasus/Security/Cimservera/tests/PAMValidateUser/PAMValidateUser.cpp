#include "Pegasus/Security/Cimservera/cimservera.h"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s username\n", argv[0]);
        exit(1);
    }

    if (CimserveraValidateUser(argv[1]) == 0)
        printf("Validation suceeded\n");
    else
        printf("Validation failed\n");

    return 0;
}
