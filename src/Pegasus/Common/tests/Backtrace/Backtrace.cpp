#include <Pegasus/Common/Backtrace.h>
#include <Pegasus/Common/String.h>

PEGASUS_USING_PEGASUS;

void gggggggggg()
{
    Pegasus::String s = "hello";
    PEGASUS_BACKTRACE;
}

void ffffffffff()
{
    gggggggggg();
}

int main()
{
    ffffffffff();
    return 0;
}
