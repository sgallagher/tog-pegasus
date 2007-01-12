#include <Executor/PasswordFile.h>
#include <stdio.h>
#include <assert.h>

int main()
{
    assert(CheckPasswordFile("passwd", "smith", "changeme") == 0);
    assert(CheckPasswordFile("passwd", "smith", "wrongpassword") != 0);
    printf("+++++ passed all tests\n");

    return 0;
}
