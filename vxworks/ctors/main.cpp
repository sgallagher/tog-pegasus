#include <cstdio>
#include <cassert>

class Global
{
public:

    Global()
    {
        magic = 0xCCCCCCCC;
    }

    ~Global()
    {
        magic = 0xDDDDDDDD;
    }

    static unsigned int magic;
};

unsigned int Global::magic = 0x00000000;

static Global _global;

// VxWorks entry point:
extern "C" void test_ctors()
{
    assert(Global::magic == 0xCCCCCCCC);
    printf("Global constructors okay\n");
}
