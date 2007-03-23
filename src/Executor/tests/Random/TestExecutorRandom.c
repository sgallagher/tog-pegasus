#include <Executor/Random.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main()
{
    unsigned char data[32];
    char ascii[65];
    int i;

    for (i = 0; i < 1024; i++)
    {
        FillRandomBytes(data, sizeof(data));
        RandBytesToHexASCII(data, sizeof(data), ascii);
        printf("%s\n", ascii);
    }

    printf("+++++ passed all tests\n");
    return 0;
}
